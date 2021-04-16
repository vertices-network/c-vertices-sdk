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
vertices_add_account(account_info_t *account, size_t *account_id)
{
    return account_add(account, account_id);
}

ret_code_t
vertices_del_account(size_t account_handle)
{
    return account_delete(account_handle);
}

ret_code_t
vertices_transaction_pay_new(size_t account_id, char *receiver, uint64_t amount, void * params)
{
    return transaction_pay(account_id, receiver, amount, params);
}

ret_code_t
vertices_transaction_get(size_t bufid, signed_transaction_t **tx)
{
    return transaction_get(bufid, tx);
}

ret_code_t
vertices_event_process(vtc_evt_t *evt)
{
    ret_code_t err_code = VTC_SUCCESS;

    // internal handling first
    switch (evt->type)
    {
        case VTC_EVT_TX_READY_TO_SIGN:break;
        case VTC_EVT_TX_READY_TO_SEND:
        {
            transaction_pending_send(evt->bufid);
        }
            break;
        case VTC_EVT_TX_SUCCESS:
        {
            err_code = transaction_free(evt->bufid);
        }
        break;
        default:
            LOG_ERROR("Unhandled event type: %u", evt->type);
    }

    // now let's have the user handle the event
    if (err_code == VTC_SUCCESS && m_vertices_evt_handler != NULL)
    {
        err_code = m_vertices_evt_handler(evt);
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
