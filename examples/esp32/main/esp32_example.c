/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <base64.h>
#include <sodium.h>
#include <vertices_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_tls.h"

#include "esp_http_client.h"
#include "vertices.h"
#include "config.h"

static const char *TAG = "vertices_example";

#ifndef AMOUNT_SENT
#define AMOUNT_SENT 100
#endif

/* The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.algoexplorer.io:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern const char
    algoexplorer_root_cert_pem_start[] asm("_binary_algoexplorer_root_cert_pem_start");
extern const char algoexplorer_root_cert_pem_end[] asm("_binary_algoexplorer_root_cert_pem_end");

/***
 * The binary data is taken from a 64-byte binary file encapsulating the private and public keys
 * in binary format.
 * It has been generated using the Unix example available in the repo, using the \c -n flag:
 * $ ./unix_example -n
 * You can then copy \c private_key.bin into the example's \c main directory
 * /!\ this is not a safe way to handle keys in production.
 */
extern const uint8_t signature_keys_start[] asm("_binary_private_key_bin_start");
extern const uint8_t signature_key_end[] asm("_binary_private_key_bin_end");

extern const uint8_t account_address_b32_start[] asm("_binary_account_address_start");
extern const uint8_t account_address_b32_end[] asm("_binary_account_address_end");

static provider_info_t providers =
    {.url = (char *) SERVER_URL, .port = SERVER_PORT, .header = (char *) SERVER_TOKEN_HEADER, .cert_pem = algoexplorer_root_cert_pem_start};

/// We store anything related to the account into the below structure
/// The private key is used outside of the Vertices library:
///    you don't have to pass the private key to the SDK as signing is done outside
typedef struct
{
    unsigned char private_key[ADDRESS_LENGTH];  //!< 32-bytes private key
    account_info_t *vtc_account;               //!< pointer to Vertices account data
} account_t;

// Alice's account is used to send data, keys will be retrived from config/key_files.txt
static account_t alice_account = {.private_key = {0}, .vtc_account = NULL};
// Bob is receiving the money 😎
static account_t bob_account = {.private_key = {0}, .vtc_account = NULL};

static ret_code_t
vertices_evt_handler(vtc_evt_t *evt);

static vertex_t m_vertex = {
    .provider = &providers,
    .vertices_evt_handler = vertices_evt_handler};

static ret_code_t
vertices_evt_handler(vtc_evt_t *evt)
{
    ret_code_t err_code = VTC_SUCCESS;

    switch (evt->type)
    {
        case VTC_EVT_TX_READY_TO_SIGN:
        {
            signed_transaction_t *tx = NULL;
            err_code = vertices_event_tx_get(evt->bufid, &tx);
            if (err_code == VTC_SUCCESS)
            {
                LOG_DEBUG("About to sign tx: data length %u", tx->payload_body_length);

                // libsodium wants to have private and public keys concatenated
                unsigned char keys[crypto_sign_ed25519_SECRETKEYBYTES] = {0};
                memcpy(keys, alice_account.private_key, sizeof(alice_account.private_key));
                memcpy(&keys[32],
                       alice_account.vtc_account->public_key,
                       ADDRESS_LENGTH);

                // prepend "TX" to the payload before signing
                unsigned char to_be_signed[tx->payload_body_length + 2];
                to_be_signed[0] = 'T';
                to_be_signed[1] = 'X';
                memcpy(&to_be_signed[2], &tx->payload[tx->payload_header_length], tx->payload_body_length);

                // sign the payload
                crypto_sign_ed25519_detached(tx->signature,
                                             0, to_be_signed, tx->payload_body_length + 2, keys);

                char b64_signature[128] = {0};
                size_t b64_signature_len = sizeof(b64_signature);
                b64_encode((const char *) tx->signature,
                           sizeof(tx->signature),
                           b64_signature,
                           &b64_signature_len);
                LOG_DEBUG("Signature %s (%u bytes)", b64_signature, b64_signature_len);

                // let's push the new state
                evt->type = VTC_EVT_TX_SENDING;
                err_code = vertices_event_schedule(evt);
            }
        }
            break;

        case VTC_EVT_TX_SENDING:
        {
            // nothing to be done on our side
        }
            break;

        default:ESP_LOGW(TAG, "Unhandled event: %u", evt->type);
            break;
    }

    return err_code;
}

static void
load_existing_account()
{
    char public_b32[PUBLIC_B32_STR_MAX_LENGTH] = {0};

    // Account address length (remove 1 char: `\n`)
    size_t len = account_address_b32_end-account_address_b32_start-1;

    // copy keys from Flash
    memcpy(alice_account.private_key, signature_keys_start, ADDRESS_LENGTH);
    memcpy(public_b32, account_address_b32_start, len);

    // let's compute the Algorand public address
    ret_code_t err_code = vertices_account_new_from_b32(public_b32, &alice_account.vtc_account);
    VTC_ASSERT(err_code);

    ESP_LOGI(TAG, "💳 Alice's account %s", alice_account.vtc_account->public_b32);
}

_Noreturn void
vtc_wallet_task(void *param)
{
    // create new vertex
    ret_code_t err_code = vertices_new(&m_vertex);
    VTC_ASSERT(err_code);

    // making sure the provider is accessible
    err_code = vertices_ping();
    VTC_ASSERT(err_code);

    // ask for provider version
    provider_version_t version = {0};
    err_code = vertices_version(&version);
    if (err_code == VTC_ERROR_OFFLINE)
    {
        ESP_LOGW(TAG, "Version might not be accurate: old value is being used");
    }
    else
    {
        VTC_ASSERT(err_code);
    }

    ESP_LOGI(TAG, "🏎 Running on %s v.%u.%u.%u",
             version.network,
             version.major,
             version.minor,
             version.patch);

    // Add account to wallet from keys
    load_existing_account();

    // we want at least 1.001 Algo available
    while (alice_account.vtc_account->amount < 1001000)
    {
        ESP_LOGW(TAG,
                 "🙄 %f Algos available on account. It's too low to pass a transaction, consider adding Algos",
                 alice_account.vtc_account->amount / 1.e6);
        ESP_LOGI(TAG, "👉 Go to https://bank.testnet.algorand.network/, dispense Algos to: %s",
                 alice_account.vtc_account->public_b32);
        ESP_LOGI(TAG, "😎 Then wait for a few seconds for transaction to pass...");
        ESP_LOGI(TAG, "⏳ Retrying in 1 minute");

        vTaskDelay(60000);

        vertices_account_update(alice_account.vtc_account);
    }

    ESP_LOGI(TAG,
             "🤑 %f Algos on Alice's account (%s)",
             alice_account.vtc_account->amount / 1.e6,
             alice_account.vtc_account->public_b32);

    // create account from b32 address
    //      Note: creating a receiver account is not mandatory to send money to the account
    //      but we can use it to load the public key from the account address
    err_code = vertices_account_new_from_b32(ACCOUNT_RECEIVER, &bob_account.vtc_account);
    VTC_ASSERT(err_code);

    // send assets from Alice's account to Bob's account
    char notes[64] = {0};
    size_t len = sprintf(notes, "Alice sent %f Algos to Bob", AMOUNT_SENT / 1.e6);
    VTC_ASSERT_BOOL(len < 64);

    err_code =
        vertices_transaction_pay_new(alice_account.vtc_account,
                                     (char *) bob_account.vtc_account->public_b32,
                                     AMOUNT_SENT,
                                     notes);
    VTC_ASSERT(err_code);

    // get application information (global states)
    LOG_INFO("Application %u, global states", APP_ID);

    app_values_t app_kv = {0};
    err_code = vertices_application_get(APP_ID, &app_kv);
    VTC_ASSERT(err_code);
    for (uint32_t i = 0; i < app_kv.count; ++i)
    {
        if (app_kv.values[i].type == VALUE_TYPE_INTEGER)
        {
            LOG_INFO("%s: %llu", app_kv.values[i].name, app_kv.values[i].value_uint);
        }
        else if (app_kv.values[i].type == VALUE_TYPE_BYTESLICE)
        {
            LOG_INFO("%s: %s", app_kv.values[i].name, app_kv.values[i].value_slice);
        }
    }

    // send application call
    app_values_t kv = {0};
    kv.count = 1;
    kv.values[0].type = VALUE_TYPE_INTEGER;
    kv.values[0].value_uint = 32;

    err_code = vertices_transaction_app_call(alice_account.vtc_account, APP_ID, &kv);
    VTC_ASSERT(err_code);

    while (1)
    {
        // processing queue
        size_t queue_size = 1;
        while (queue_size && err_code == VTC_SUCCESS)
        {
            err_code = vertices_event_process(&queue_size);
        }

        if (err_code == VTC_SUCCESS)
        {
            ESP_LOGI(TAG, "💸 Alice sent %f algo to Bob", AMOUNT_SENT / 1.e6);

            // delete the created accounts from the Vertices wallet
            err_code = vertices_account_free(alice_account.vtc_account);
            VTC_ASSERT(err_code);

            err_code = vertices_account_free(bob_account.vtc_account);
            VTC_ASSERT(err_code);
        }
        else
        {
            ESP_LOGE(TAG, "Error trying to send TX: 0x%x", err_code);
            VTC_ASSERT(err_code);
        }

        // Delete the task
        vTaskDelete(NULL);
    }
}

void
app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    ESP_LOGI(TAG, "Connected to AP, begin Vertices example");

    xTaskCreatePinnedToCore(&vtc_wallet_task, "vtc_wallet_task", 16000, NULL, 5, NULL, 1);
}
