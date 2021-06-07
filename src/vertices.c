//
// Created by Cyril on 17/03/2021.
//

#include <provider.h>
#include <account.h>
#include <transaction.h>
#include <vertices_log.h>
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

ret_code_t
vertices_version(provider_version_t *version)
{
    return provider_version_get(version);
}

ret_code_t
vertices_ping()
{
    return provider_ping();
}

ret_code_t
vertices_account_add(account_info_t *account, size_t *account_id)
{
    return account_add(account, account_id);
}

ret_code_t
vertices_account_del(size_t account_handle)
{
    return account_delete(account_handle);
}

ret_code_t
vertices_account_update(size_t account_handle)
{
    return account_update(account_handle);
}

ret_code_t
vertices_transaction_pay_new(size_t account_id, char *receiver, uint64_t amount, void *params)
{
    return transaction_pay(account_id, receiver, amount, params);
}

ret_code_t
vertices_transaction_app_call(size_t account_id, uint64_t app_id, void *params)
{
    return transaction_appl(account_id, app_id, params);
}

ret_code_t
vertices_transaction_get(size_t bufid, signed_transaction_t **tx)
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
        if (err_code == VTC_SUCCESS && m_vertices_evt_handler != NULL)
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
