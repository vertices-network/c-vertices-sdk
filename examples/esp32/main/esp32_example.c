/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include <base64.h>
#include <sodium.h>
#include <vertices_log.h>
#include <sha512_256.h>
#include <base32.h>
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

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";

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
 * It has been generated using the Unix example available on the repo, using the \c -n flag
 * $ ./unix_example -n
 * You can then copy \c private_key.bin into the example's \c main directory
 */
extern const uint8_t signature_keys_start[] asm("_binary_private_key_bin_start");
extern const uint8_t signature_key_end[] asm("_binary_private_key_bin_end");

static provider_info_t providers =
    {.url = (char *) "https://api.testnet.algoexplorer.io", .port = 0, .header = (char *) "", .cert_pem = algoexplorer_root_cert_pem_start};

// Alice's account is used to send data, private key is taken from config/private_key.bin
static account_info_t alice_account =
    {.public_b32 = {""},
        .private_key = { 0 },
        .amount = 0};
// Bob is receiving the money 😎
static account_info_t bob_account =
    {.public_b32 = "27J56E73WOFSEQUECLRCLRNBV3D74H7BYB7USEXCJOYPLBTACULABWMLVU", .private_key = {
        0}, .amount = 0};

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
            err_code = vertices_transaction_get(evt->bufid, &tx);
            if (err_code == VTC_SUCCESS)
            {
                LOG_DEBUG("About to sign tx: data length %zu", tx->payload_length);

                // libsodium wants to have private and public keys concatenated
                unsigned char keys[crypto_sign_ed25519_SECRETKEYBYTES] = {0};
                memcpy(keys, alice_account.private_key, sizeof(alice_account.private_key));
                memcpy(&keys[32], alice_account.public_key, sizeof(alice_account.public_key));

                // prepend "TX" to the payload before signing
                unsigned char to_be_signed[tx->payload_length + 2];
                to_be_signed[0] = 'T';
                to_be_signed[1] = 'X';
                memcpy(&to_be_signed[2], &tx->payload[tx->payload_offset], tx->payload_length);

                // sign the payload
                crypto_sign_ed25519_detached(tx->signature,
                                             0, to_be_signed, tx->payload_length + 2, keys);

                char b64_signature[128] = {0};
                size_t b64_signature_len = sizeof(b64_signature);
                b64_encode((const char *) tx->signature,
                           sizeof(tx->signature),
                           b64_signature,
                           &b64_signature_len);
                LOG_DEBUG("Signature %s (%zu bytes)", b64_signature, b64_signature_len);

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

void
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

    // create accounts
    size_t alice_account_handle = 0;
    err_code = vertices_account_add(&alice_account, &alice_account_handle);
    VTC_ASSERT(err_code);

    // we want at least 1.001 Algo available
    while (alice_account.amount < 1001000)
    {
        ESP_LOGW(TAG,
                 "🙄 %f Algos available on account. It's too low to pass a transaction, consider adding Algos", alice_account.amount / 1.e6);
        ESP_LOGI(TAG, "👉 Go to https://bank.testnet.algorand.network/, dispense Algos to: %s",
                 alice_account.public_b32);
        ESP_LOGI(TAG, "😎 Then wait for a few seconds for transaction to pass...");
        ESP_LOGI(TAG, "⏳ Retrying in 1 minute");

        vTaskDelay(60000);

        vertices_account_update(alice_account_handle);
    }

    ESP_LOGI(TAG,
             "🤑 %f Algos on Alice's account (%s)",
             alice_account.amount / 1.e6,
             alice_account.public_b32);

    // creating a receiver account is not mandatory but we can use it to load the public key from the
    // base32-encoded string
    size_t bob_account_handle = 0;
    err_code = vertices_account_add(&bob_account, &bob_account_handle);
    VTC_ASSERT(err_code);

    // send assets from Alice's account to Bob's account
    char notes[64] = {0};
    size_t len = sprintf(notes, "Alice sent %f Algos to Bob", AMOUNT_SENT / 1.e6);
    VTC_ASSERT_BOOL(len < 64);

    err_code =
        vertices_transaction_pay_new(alice_account_handle,
                                     (char *) bob_account.public_key,
                                     AMOUNT_SENT,
                                     notes);
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
            err_code = vertices_account_del(alice_account_handle);
            VTC_ASSERT(err_code);

            err_code = vertices_account_del(bob_account_handle);
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

static void
source_keys()
{
    // copy keys, first part is the private key and last part is the public key
    memcpy(alice_account.private_key, signature_keys_start, ADDRESS_LENGTH);
    memcpy(alice_account.public_key, &signature_keys_start[ADDRESS_LENGTH], ADDRESS_LENGTH);

    ESP_LOGD(TAG, "Sourced keys: 0x%02x%02x%02x... 0x%02x%02x%02x...", alice_account.private_key[0],
             alice_account.private_key[1],
             alice_account.private_key[2],
             alice_account.public_key[0],
             alice_account.public_key[1],
             alice_account.public_key[2]);

    unsigned char checksum[32] = {0};
    char public_key_checksum[36] = {0};
    memcpy(public_key_checksum, alice_account.public_key, sizeof(alice_account.public_key));

    ret_code_t err_code = sha512_256(alice_account.public_key, sizeof(alice_account.public_key), checksum, sizeof(checksum));
    VTC_ASSERT(err_code);

    memcpy(&public_key_checksum[32], &checksum[32 - 4], 4);

    size_t size = 58;
    memset(alice_account.public_b32,
           0,
           sizeof(alice_account.public_b32)); // make sure init to zeros (string)
    err_code = b32_encode((const char *) public_key_checksum,
                          sizeof(public_key_checksum),
                          alice_account.public_b32,
                          &size);
    VTC_ASSERT(err_code);

    ESP_LOGI(TAG, "💳 Alice's account %s", alice_account.public_b32);
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
    ESP_LOGI(TAG, "Connected to AP, begin http example");

    source_keys();

    xTaskCreatePinnedToCore(&vtc_wallet_task, "vtc_wallet_task", 16000, NULL, 5, NULL, 1);
}
