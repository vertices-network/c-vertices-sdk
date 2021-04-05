//
// Created by Cyril on 17/03/2021.
//

#include "vertices.h"
#include <vertices_log.h>
#include <vertices_config.h>
#include <base64.h>
#include <string.h>
#include <sodium.h>

#define ACCOUNT_NUMBER 2

static ret_code_t
vertices_evt_handler(vtc_evt_t *evt);

static provider_info_t providers =
    {.url = SERVER_URL, .port = SERVER_PORT, .header = SERVER_TOKEN_HEADER};

static account_info_t accounts[ACCOUNT_NUMBER] = {
    {.public = "KT6VFEFWGZEO3FP4V56ANK5UCTXPX4ZMFKZEUBQZQ5IOO7RULZM23UMISQ", .private_key = {
        0}, .amount = 0},
    {.public = "27J56E73WOFSEQUECLRCLRNBV3D74H7BYB7USEXCJOYPLBTACULABWMLVU", .private_key = {
        0}, .amount = 0},
};

static vertex_t m_vertex = {
    .provider = &providers,
    .vertices_evt_handler = vertices_evt_handler
};

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
                LOG_DEBUG("About to sign tx: data length %lu", tx->payload_length);

                // libsodium wants to have private and public keys concatenated
                unsigned char keys[crypto_sign_ed25519_SECRETKEYBYTES] = {0};
                memcpy(keys, accounts[0].private_key, sizeof(accounts[0].private_key));
                memcpy(&keys[32], accounts[0].public_key, sizeof(accounts[0].public_key));

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

                evt->type = VTC_EVT_TX_READY_TO_SEND;
                vertices_event_process(evt);
            }
        }
            break;

        case VTC_EVT_TX_READY_TO_SEND:
        {
            // let's create transaction files which can then be used with `goal clerk ...`
            signed_transaction_t *tx = NULL;
            err_code = vertices_transaction_get(evt->bufid, &tx);

            FILE *fstx = fopen(VERTICES_ROOT "signed_tx.bin", "wb");

            if (fstx == NULL)
            {
                return VTC_ERROR_NOT_FOUND;
            }

            fwrite(tx->payload, tx->payload_offset + tx->payload_length, 1, fstx);
            fclose(fstx);

            FILE *ftx = fopen(VERTICES_ROOT "tx.bin", "wb");

            if (ftx == NULL)
            {
                return VTC_ERROR_NOT_FOUND;
            }

            // goal-generated transaction files are packed into a map of one element: `txn`.
            // the one-element map takes 4 bytes into our message packed payload <=> `txn`
            // we also add the `map` type before
            // which results in 5-bytes to be added before the payload at `payload_offset`
            char payload[tx->payload_length + 5];
            payload[0] = (char) 0x81; // starting flag for map of one element
            memcpy(&payload[1], &tx->payload[tx->payload_offset - 4], tx->payload_length + 4);

            fwrite(payload, sizeof payload, 1, ftx);
            fclose(ftx);
        }
            break;

        default:LOG_ERROR("Unhandled event: %u", evt->type);
            break;
    }

    return err_code;
}

static ret_code_t
source_keys()
{
    LOG_INFO("Loading private key from: %s", CONFIG_PATH "private_key.bin");

    FILE *f = fopen(CONFIG_PATH "private_key.bin", "rb");

    if (f == NULL)
    {
        return VTC_ERROR_NOT_FOUND;
    }

    fread(accounts[0].private_key, 1, 32, f);
    fclose(f);

    return VTC_SUCCESS;
}

int
main(int argc, char *argv[])
{
    ret_code_t err_code;
    LOG_INFO("😎 Vertices SDK running on Unix-based OS");

    int ret = sodium_init();
    VTC_ASSERT(ret);

    // read private key from file
    err_code = source_keys();
    VTC_ASSERT(err_code);

    // create new vertex
    err_code = vertices_new(&m_vertex);
    VTC_ASSERT(err_code);

    // making sure the provider is accessible
    err_code = vertices_ping();
    VTC_ASSERT(err_code);

    // ask for provider version
    provider_version_t version = {0};
    err_code = vertices_version(&version);
    if (err_code == VTC_ERROR_OFFLINE)
    {
        LOG_WARNING("Version might not be accurate: old value is being used");
    }
    else
    {
        VTC_ASSERT(err_code);
    }

    LOG_INFO("🏎 Running on %s v.%u.%u.%u",
             version.network,
             version.major,
             version.minor,
             version.patch);

    // create accounts
    size_t account_handle_sender = 0;
    err_code = vertices_add_account(&accounts[0], &account_handle_sender);
    VTC_ASSERT(err_code);

    // creating a receiver account is not mandatory but we can use it to load the public key from the
    // base32-encoded string
    size_t account_handle_receiver = 0;
    err_code = vertices_add_account(&accounts[1], &account_handle_receiver);
    VTC_ASSERT(err_code);

    LOG_INFO("🤑 %d Algos on %s", accounts[0].amount, accounts[0].public);

    // send assets
    err_code =
        vertices_transaction_pay_new(account_handle_sender, (char *) accounts[1].public_key, 1000);
    VTC_ASSERT(err_code);

    // delete the created account
    err_code = vertices_del_account(account_handle_sender);
    VTC_ASSERT(err_code);
}