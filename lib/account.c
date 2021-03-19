//
// Created by Cyril on 17/03/2021.
//

#include <string.h>
#include <vertices_log.h>
#include <provider.h>
#include "account.h"

#define ACCOUNTS_MAXIMUM_COUNT  2

static account_details_t m_accounts[ACCOUNTS_MAXIMUM_COUNT] = {0};

err_code_t
account_add(account_info_t *account, size_t * id)
{
    VTC_ASSERT_BOOL(account != NULL);
    err_code_t err_code;

    // look for free spot to store the new account
    size_t i = 0;
    for (i = 0; i < ACCOUNTS_MAXIMUM_COUNT; ++i)
    {
        if(m_accounts[i].info == NULL)
        {
            LOG_DEBUG("Creating account #%zu", i);
            break;
        }
    }

    // cannot store another account
    if (i == ACCOUNTS_MAXIMUM_COUNT)
    {
        return VTC_ERROR_NO_MEM;
    }

    m_accounts[i].info = account;

    // update account info
    err_code = provider_get_account_info(&m_accounts[i]);

    *id = i;

    return err_code;
}

err_code_t
account_delete(size_t id)
{
    if (m_accounts[id].info == NULL)
    {
        return VTC_ERROR_INVALID_STATE;
    }

    memset(&m_accounts[id], 0, sizeof(account_details_t));

    return VTC_SUCCESS;
}

err_code_t
account_init()
{
    memset(m_accounts, 0, sizeof m_accounts);

    return VTC_SUCCESS;
}