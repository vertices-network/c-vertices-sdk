//
// Created by Cyril on 17/03/2021.
//

#include "vertices.h"
#include <vertices_log.h>
#include <unix_config.h>
#include <string.h>
#include <sodium.h>
#include <getopt.h>
#include <stdbool.h>
#include <base32.h>
#include <base64.h>
#include <sha512_256.h>

#define ACCOUNT_NUMBER 2

static ret_code_t
vertices_evt_handler(vtc_evt_t *evt);

static provider_info_t providers =
    {.url = (char *) SERVER_URL, .port = SERVER_PORT, .header = (char *) SERVER_TOKEN_HEADER};

// Alice's account is used to send data, private key is taken from config/private_key.bin
static account_info_t alice_account = {.public_b32 = {0}, .private_key = {
    0}, .amount = 0};
// Bob is receiving the money 😎
static account_info_t bob_account =
    {.public_b32 = "27J56E73WOFSEQUECLRCLRNBV3D74H7BYB7USEXCJOYPLBTACULABWMLVU", .private_key = {
        0}, .amount = 0};

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

                evt->type = VTC_EVT_TX_READY_TO_SEND;
                err_code = vertices_event_schedule(evt);
            }
        }
            break;

        case VTC_EVT_TX_READY_TO_SEND:
        {
            // let's create transaction files which can then be used with `goal clerk ...`
            signed_transaction_t *tx = NULL;
            err_code = vertices_transaction_get(evt->bufid, &tx);

            FILE *fstx = fopen(CONFIG_PATH "../signed_tx.bin", "wb");

            if (fstx == NULL)
            {
                return VTC_ERROR_NOT_FOUND;
            }

            fwrite(tx->payload, tx->payload_offset + tx->payload_length, 1, fstx);
            fclose(fstx);

            FILE *ftx = fopen(CONFIG_PATH "../tx.bin", "wb");

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

        default:break;
    }

    return err_code;
}

/// Source the private/public keys from file.
/// \param create_new Indicates to create a new random account if private key is not found or incorrect
/// \return \c VTC_ERROR_NOT_FOUND account not found
static ret_code_t
source_keys(bool create_new)
{
    ret_code_t err_code;
    unsigned char seed[crypto_sign_ed25519_SEEDBYTES] = {0};
    unsigned char ed25519_pk[crypto_sign_ed25519_PUBLICKEYBYTES];

    FILE *f = fopen(CONFIG_PATH "private_key.bin", "rb");
    size_t bytes_read = 0;
    if (f != NULL)
    {
        LOG_INFO("🔑 Loading private key from: %s", CONFIG_PATH "private_key.bin");

        bytes_read = fread(alice_account.private_key, 1, 64, f);
        fclose(f);
    }

    if (create_new)
    {
        LOG_WARNING("🧾 Creating new random account and storing it (path " CONFIG_PATH ")");

        unsigned char ed25519_sk[crypto_sign_ed25519_SECRETKEYBYTES];
        randombytes_buf(seed, sizeof(seed));

        crypto_sign_ed25519_seed_keypair(ed25519_pk, ed25519_sk, seed);

        memcpy(alice_account.private_key, ed25519_sk, sizeof(alice_account.private_key));
        memcpy(alice_account.public_key, ed25519_pk, sizeof(alice_account.public_key));

        FILE *fw = fopen(CONFIG_PATH "private_key.bin", "wb");
        if (fw == NULL)
        {
            LOG_ERROR("Cannot create " CONFIG_PATH "private_key.bin");
            return VTC_ERROR_NOT_FOUND;
        }
        else
        {
            fwrite(ed25519_sk, 1, sizeof(ed25519_sk), fw);
            fclose(f);
        }
    }
    else if (f == NULL || bytes_read != 64)
    {
        LOG_WARNING(
            "🤔 private_key.bin does not exist or keys not found. You can pass the -n flag to create a new account");

        return VTC_ERROR_NOT_FOUND;
    }

    unsigned char checksum[32] = {0};
    char public_key_checksum[36] = {0};
    memcpy(public_key_checksum, alice_account.public_key, sizeof(alice_account.public_key));

    err_code = sha512_256(alice_account.public_key, sizeof(alice_account.public_key), checksum);
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

    LOG_INFO("💳 Alice's account %s", alice_account.public_b32);

    return VTC_SUCCESS;
}

int
main(int argc, char *argv[])
{
    ret_code_t err_code;

    bool create_new = false;
    int opt;
    while ((opt = getopt(argc, argv, "n")) != -1)
    {
        switch (opt)
        {
            case 'n': create_new = true;
                break;
            default:fprintf(stderr, "Usage: %s [-n]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    LOG_INFO("😎 Vertices SDK running on Unix-based OS");

    int ret = sodium_init();
    VTC_ASSERT_BOOL(ret == 0);

    // read private key from file
    err_code = source_keys(create_new);
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
    size_t alice_account_handle = 0;
    err_code = vertices_add_account(&alice_account, &alice_account_handle);
    VTC_ASSERT(err_code);

    // creating a receiver account is not mandatory but we can use it to load the public key from the
    // base32-encoded string
    size_t bob_account_handle = 0;
    err_code = vertices_add_account(&bob_account, &bob_account_handle);
    VTC_ASSERT(err_code);

    LOG_INFO("🤑 %f Algos on Alice's account (%s)", alice_account.amount / 1.e6, alice_account.public_b32);

    if (alice_account.amount < 1001000)
    {
        LOG_ERROR(
            "🙄 Amount available on account is too low to pass a transaction, consider adding Algos");
        LOG_INFO("👉 Go to https://bank.testnet.algorand.network/, dispense Algos to: %s",
                 alice_account.public_b32);
        LOG_INFO("😎 Then wait for a few seconds for transaction to pass...");
        return 0;
    }

    // send assets from account 0 to account 1
    char *notes = (char *) "Alice sent 1 Algo to Bob";
    err_code =
        vertices_transaction_pay_new(alice_account_handle,
                                     (char *) bob_account.public_key,
                                     1000000,
                                     notes);
    VTC_ASSERT(err_code);

    size_t queue_size = 1;
    while (queue_size && err_code == VTC_SUCCESS)
    {
        err_code = vertices_event_process(&queue_size);
    }

    LOG_INFO("💸 Alice sent 1 algo to Bob");

    // delete the created accounts from the Vertices wallet
    err_code = vertices_del_account(alice_account_handle);
    VTC_ASSERT(err_code);

    err_code = vertices_del_account(bob_account_handle);
    VTC_ASSERT(err_code);
}
