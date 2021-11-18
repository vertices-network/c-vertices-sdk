/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <provider.h>
#include <account.h>
#include <transaction.h>
#include <vertices_log.h>
#include <string.h>
#include <sha512_256.h>
#include <base32.h>
#include "vertices.h"

static ret_code_t
(*m_vertices_evt_handler)(vtc_evt_t *evt) = NULL;

typedef struct
{
    size_t wr_index;
    size_t rd_index;
    vtc_evt_t evt[VTC_EVENTS_COUNT];
} vtc_events_buf_t;

#define increment_event_index(i) \
    i = ((i) + 1) & (VTC_EVENTS_COUNT - 1)

static vtc_events_buf_t m_events_queue = {0};

/// Get node version
/// \param version Pointer to \c provider_version_t
/// \return
/// * \c VTC_SUCCESS when \c version has been filled with node info
/// * \c VTC_ERROR_OFFLINE when node cannot be reached to get info. Version could still be filled with information from a previous call.
ret_code_t
vertices_version(provider_version_t *version)
{
    return provider_version_get(version);
}

/// Check if node is alive
/// \return
/// * \c VTC_SUCCESS when API can be reached
/// * \c VTC_ERROR_HTTP_BASE when an error occurs
ret_code_t
vertices_ping()
{
    return provider_ping();
}

ret_code_t
vertices_account_new_from_b32(char *public_b32, account_info_t **account)
{
    return account_new(public_b32, account);
}

ret_code_t
vertices_account_new_from_bin(char *public_key, account_info_t **account)
{
    VTC_ASSERT_BOOL(public_key != 0);
    VTC_ASSERT_BOOL(account != 0);

    ret_code_t err_code;

    unsigned char checksum[32] = {0};
    char public_key_checksum[36] = {0};
    char public_b32[PUBLIC_B32_STR_MAX_LENGTH] = {0};

    memcpy(public_key_checksum, public_key, ADDRESS_LENGTH);

    err_code = sha512_256((const unsigned char *) public_key,
                          ADDRESS_LENGTH,
                          checksum,
                          sizeof(checksum));
    VTC_ASSERT(err_code);

    memcpy(&public_key_checksum[32], &checksum[32 - 4], 4);

    size_t size = 58;
    err_code = b32_encode((const char *) public_key_checksum,
                          sizeof(public_key_checksum),
                          public_b32,
                          &size);
    VTC_ASSERT(err_code);

    return account_new(public_b32, account);
}

ret_code_t
vertices_account_free(account_info_t *account)
{
    return account_free(account);
}

ret_code_t
vertices_account_update(account_info_t *account)
{
    return account_update(account);
}

ret_code_t
vertices_transaction_pay_new(account_info_t *account, char *receiver, uint64_t amount, void *params)
{
    return transaction_pay(account, receiver, amount, params);
}

ret_code_t
vertices_transaction_app_call(account_info_t *account, uint64_t app_id, void *params)
{
    return transaction_appl(account, app_id, params);
}

ret_code_t
vertices_application_get(uint64_t app_id, app_values_t * global_states)
{
    return provider_application_info_get(app_id, global_states);
}

ret_code_t
vertices_event_tx_get(size_t bufid, signed_transaction_t **tx)
{
    return transaction_get(bufid, tx);
}

static size_t
event_queue_size(void)
{
    return (m_events_queue.wr_index - m_events_queue.rd_index) & (VTC_EVENTS_COUNT - 1);
}

ret_code_t
vertices_event_schedule(vtc_evt_t *evt)
{
    size_t next = (m_events_queue.wr_index + 1) % VTC_EVENTS_COUNT;
    if (next == m_events_queue.rd_index)
    {
        return VTC_ERROR_NO_MEM;
    }

    m_events_queue.evt[m_events_queue.wr_index].type = evt->type;
    m_events_queue.evt[m_events_queue.wr_index].bufid = evt->bufid;

    increment_event_index(m_events_queue.wr_index);

    return VTC_SUCCESS;
}

ret_code_t
vertices_event_process(size_t * queue_size)
{
    ret_code_t err_code = VTC_SUCCESS;

    if (event_queue_size() == 0)
    {
        if (queue_size != NULL)
        {
            *queue_size = event_queue_size();
        }
        return VTC_SUCCESS;
    }

    if (m_events_queue.rd_index != m_events_queue.wr_index)
    {
        // internal handling for action to be taken before the user
        switch (m_events_queue.evt[m_events_queue.rd_index].type)
        {
            case VTC_EVT_TX_READY_TO_SIGN:break;
            case VTC_EVT_TX_SENDING:
            {
                err_code =
                    transaction_pending_send(m_events_queue.evt[m_events_queue.rd_index].bufid);
            }
                break;
            default:break;
        }

        if (err_code != VTC_SUCCESS)
        {
            LOG_ERROR("Pre-processing failed. Type: %u, Error: %x",
                      m_events_queue.evt[m_events_queue.rd_index].type,
                      err_code);
            return err_code;
        }

        // now let's have the user handle the event
        if (m_vertices_evt_handler != NULL)
        {
            err_code = m_vertices_evt_handler(&m_events_queue.evt[m_events_queue.rd_index]);

            if (err_code != VTC_SUCCESS)
            {
                LOG_ERROR("User-processing failed. Type: %u, Error: %x",
                          m_events_queue.evt[m_events_queue.rd_index].type,
                          err_code);
                return err_code;
            }
        }

        // internal handling for action to be taken after the user
        switch (m_events_queue.evt[m_events_queue.rd_index].type)
        {
            case VTC_EVT_TX_SUCCESS:
            {
                err_code = transaction_free(m_events_queue.evt[m_events_queue.rd_index].bufid);
            }
                break;

            default:
                break;
        }

        if (err_code != VTC_SUCCESS)
        {
            LOG_ERROR("Post-processing failed. Type: %u, Error: %u",
                      m_events_queue.evt[m_events_queue.rd_index].type,
                      err_code);
            return err_code;
        }

        // successful processing, we can increment the read index
        increment_event_index(m_events_queue.rd_index);
    }

    if (queue_size != NULL)
    {
        *queue_size = event_queue_size();
    }

    return err_code;
}

ret_code_t
vertices_new(vertex_t *config)
{
    ret_code_t err_code;

    err_code = provider_init(config->provider);
    if (err_code != VTC_SUCCESS)
    {
        return err_code;
    }

    err_code = account_init();
    if (err_code != VTC_SUCCESS)
    {
        return err_code;
    }

    m_vertices_evt_handler = config->vertices_evt_handler;

    return err_code;
}
