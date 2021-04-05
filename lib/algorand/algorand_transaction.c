//
// Created by Cyril on 01/04/2021.
//

#include <vertices_errors.h>
#include <stdio.h>
#include <transaction.h>
#include <account.h>
#include <string.h>
#include <provider.h>
#include <mpack.h>
#include <vertices_log.h>
#include <vertices.h>
#include <base64.h>

const char *algorand_tx_types[] = {"pay", "keyreg", "acfg", "axfer", "afrz", "appl"};

#define PENDING_TX_COUNT    4
static signed_transaction_t m_pending_tx[PENDING_TX_COUNT];
static size_t wr_idx = 0;

static ret_code_t
encode_tx_replace_signature(size_t bufid)
{
    ret_code_t err_code = VTC_ERROR_NOT_FOUND;

    // check that a payload is ready
    if (m_pending_tx[bufid].payload_length == 0)
    {
        return VTC_ERROR_INVALID_PARAM;
    }

    // let's find the location of the signature in the current payload
    // first we need to find a map
    // 'sig', 'msig' or 'lsig' followed by 64-byte signature
    // then the transaction, but we don't care about that part in that function
    mpack_reader_t reader;
    mpack_reader_init_data(&reader,
                           (const char *) m_pending_tx[bufid].payload,
                           m_pending_tx[bufid].payload_length);

    mpack_tag_t map_tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) == mpack_ok && mpack_tag_type(&map_tag) == mpack_type_map)
    {
        mpack_tag_t tag = mpack_read_tag(&reader);
        if (mpack_tag_type(&tag) == mpack_type_str)
        {
            // get the pointer to the beginning of the string tag
            uint32_t length = mpack_tag_str_length(&tag);
            const char *data = mpack_read_bytes_inplace(&reader, length);

            if (strncmp(data, "sig", length) == 0)
            {
                // get the pointer to the beginning of the signature
                mpack_tag_t signature_tag = mpack_read_tag(&reader);
                size_t len = mpack_tag_str_length(&signature_tag);
                if (len == SIGNATURE_LENGTH)
                {
                    char *signature_ptr = (char *) mpack_read_bytes_inplace(&reader, len);
                    // replace the signature with the current one
                    memcpy(signature_ptr, (const void *) m_pending_tx[bufid].signature, len);
                    err_code = VTC_SUCCESS;
                }
            }
        }
    }

    return err_code;
}

static ret_code_t
encode_tx_pay(transaction_t *tx, size_t bufid)
{
    ret_code_t err_code = VTC_SUCCESS;
    const int8_t empty_sig[SIGNATURE_LENGTH] = {0};

    char *data = (char *) m_pending_tx[bufid].payload;
    size_t size = sizeof(m_pending_tx[bufid].payload);

    // encode to memory buffer
    mpack_writer_t writer;
    mpack_writer_init(&writer, (char *) data, size);

    mpack_start_map(&writer, 2);
    mpack_write_cstr(&writer, "sig");
    mpack_write_bin(&writer, (const char *) empty_sig, sizeof empty_sig);

    mpack_write_cstr(&writer, "txn");

    // from there, the payload is part of the signature we store the offset location
    m_pending_tx[bufid].payload_offset = (size_t) (writer.current - writer.buffer);

    mpack_start_map(&writer, 9);
    mpack_write_cstr(&writer, "amt");
    mpack_write_uint(&writer, tx->details->payment_tx.amount);

//    todo add support to close an account using the "pay" transaction, using field "close"
//    When set, it indicates that the transaction is requesting that the Sender account should be
//    closed, and all remaining funds, after the fee and amount are paid, be transferred to this
//    address.
//    mpack_write_cstr(&writer, "close");
//    mpack_write_str(&writer, (const char *) tx->receiver_pub, sizeof tx->receiver_pub);

    mpack_write_cstr(&writer, "fee");
    mpack_write_uint(&writer, tx->fee);
    mpack_write_cstr(&writer, "fv");
    mpack_write_uint(&writer, tx->details->first_valid);
    mpack_write_cstr(&writer, "gen");
    mpack_write_str(&writer, (const char *) "testnet-v1.0", sizeof "testnet-v1.0" - 1);
    mpack_write_cstr(&writer, "gh");
    mpack_write_bin(&writer, (const char *) tx->genesis_hash, sizeof tx->genesis_hash);
    mpack_write_cstr(&writer, "lv");
    mpack_write_uint(&writer, tx->details->last_valid);
    mpack_write_cstr(&writer, "rcv");
    mpack_write_bin(&writer,
                    (const char *) tx->details->payment_tx.receiver,
                    sizeof tx->details->payment_tx.receiver);
    mpack_write_cstr(&writer, "snd");
    mpack_write_bin(&writer, (const char *) tx->sender_pub, sizeof tx->sender_pub);
    mpack_write_cstr(&writer, "type");
    mpack_write_cstr(&writer, algorand_tx_types[tx->details->tx_type]);
    mpack_finish_map(&writer);
    mpack_finish_map(&writer);

    size = mpack_writer_buffer_used(&writer);
    m_pending_tx[bufid].payload_length = size - m_pending_tx[bufid].payload_offset;

    LOG_DEBUG("mpack used %lu bytes to encode pay tx", size);

    // finish writing
    if (mpack_writer_destroy(&writer) != mpack_ok)
    {
        fprintf(stderr, "An error occurred encoding the data!\n");
        return VTC_ERROR_INTERNAL;
    }

    return err_code;
}

static ret_code_t
encode_tx(transaction_t *tx)
{
    ret_code_t err_code = VTC_ERROR_INVALID_PARAM;

    switch (tx->details->tx_type)
    {
        case ALGORAND_PAYMENT_TRANSACTION:
        {
            err_code = encode_tx_pay(tx, wr_idx);
        }
            break;
        case ALGORAND_KEY_REGISTRATION_TRANSACTION:
        case ALGORAND_ASSET_CONFIGURATION_TRANSACTION:
        case ALGORAND_ASSET_TRANSFER_TRANSACTION:
        case ALGORAND_ASSET_FREEZE_TRANSACTION:
        case ALGORAND_APPLICATION_CALL_TRANSATION:
        {
            // not implemented
            err_code = VTC_ERROR_INTERNAL;
        }
            break;

        default:
        {
            LOG_ERROR("Tx type unknown");
            err_code = VTC_ERROR_INVALID_PARAM;
        }
            break;
    }

    return err_code;
}

ret_code_t
transaction_pay(size_t account_id, char *receiver, uint64_t amount)
{
    ret_code_t err_code = VTC_SUCCESS;

    if (m_pending_tx[wr_idx].payload_length != 0)
    {
        return VTC_ERROR_NO_MEM;
    }

    m_pending_tx[wr_idx].payload_length = sizeof m_pending_tx[wr_idx].payload;

    // instantiate transaction
    transaction_details_t details = {0};
    transaction_t tx_full = {0};
    tx_full.details = &details;

    // fill generic transaction_t
    err_code = account_get_addr(account_id, (char *) tx_full.sender_pub);
    if (err_code != VTC_SUCCESS)
    {
        LOG_ERROR("Unable to get sender account address");
        return err_code;
    }

    tx_full.fee = TX_DEFAULT_FEE;
    tx_full.details->tx_type = ALGORAND_PAYMENT_TRANSACTION;

    tx_full.details->payment_tx.amount = amount;

    memcpy(tx_full.details->payment_tx.receiver,
           receiver,
           sizeof(tx_full.details->payment_tx.receiver));

    // get provider details
    err_code = provider_tx_params_load(&tx_full);
    if (err_code != VTC_SUCCESS)
    {
        LOG_ERROR("Cannot fetch tx params");
        return err_code;
    }

    // tx is now ready to be encoded
    err_code = encode_tx(&tx_full);

    vtc_evt_t evt = {.type = VTC_EVT_TX_READY_TO_SIGN, .bufid = wr_idx};

    // wr_idx spot is now taken, push index
    wr_idx = (wr_idx + 1) % PENDING_TX_COUNT;

    // push event for asynchronous operation
    vertices_event_process(&evt);

    return err_code;
}

ret_code_t
transaction_get(size_t bufid, signed_transaction_t **tx)
{
    if (m_pending_tx[bufid].payload_length == 0)
    {
        return VTC_ERROR_INVALID_PARAM;
    }

    *tx = &m_pending_tx[bufid];

    return VTC_SUCCESS;
}

ret_code_t
transaction_pending_send(size_t bufid)
{
    if (m_pending_tx[bufid].payload_length == 0)
    {
        return VTC_ERROR_INVALID_PARAM;
    }

    ret_code_t err_code = VTC_SUCCESS;

    err_code = encode_tx_replace_signature(bufid);
    VTC_ASSERT(err_code);

    size_t payload_len = m_pending_tx[bufid].payload_length
        + m_pending_tx[bufid].payload_offset;
    err_code = provider_tx_post(m_pending_tx[bufid].payload,
                                payload_len);

    // transaction has been executed, free up spot
//    if (err_code == VTC_SUCCESS)
//    {
//        memset(&m_pending_tx[bufid], 0, sizeof(m_pending_tx[bufid]));
//    }

    return err_code;
}