/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <vertices_log.h>
#include <provider.h>
#include <base32.h>
#include "account.h"

#define ACCOUNTS_MAXIMUM_COUNT  2

enum account_status_e
{
    ACCOUNT_NONE = 0,
    ACCOUNT_ADDED,
};

typedef struct local_accounts
{
    account_details_t account;
    enum account_status_e status;
} local_accounts_t;
static local_accounts_t m_accounts[ACCOUNTS_MAXIMUM_COUNT] = {0};

static ret_code_t
from_b32_init(size_t index, char *public_b32)
{
    char result[36] = {0};
    size_t result_size = sizeof result;
    b32_decode(public_b32, result, &result_size);

    // todo verify checksum

    // copy address part
    memcpy(m_accounts[index].account.info.public_key,
           result,
           sizeof(m_accounts[index].account.info.public_key));

    return VTC_SUCCESS;
}

static bool
account_exists(account_info_t *account)
{
    uint32_t i = 0;
    for (; i < ACCOUNTS_MAXIMUM_COUNT; ++i)
    {
        if (account == (account_info_t *) &m_accounts[i].account)
        {
            return true;
        }
    }

    return false;
}

ret_code_t
account_new(char *public_b32, account_info_t **account)
{
    VTC_ASSERT_BOOL(public_b32 != NULL);
    ret_code_t err_code;

    // look for free spot to store the new account
    size_t i = 0;
    for (i = 0; i < ACCOUNTS_MAXIMUM_COUNT; ++i)
    {
        if (m_accounts[i].status == ACCOUNT_NONE)
        {
            m_accounts[i].status = ACCOUNT_ADDED;

            LOG_INFO("👛 Added account to wallet: #%lu", (uint32_t) i);
            break;
        }
    }

    // cannot store another account
    if (i == ACCOUNTS_MAXIMUM_COUNT)
    {
        return VTC_ERROR_NO_MEM;
    }

    err_code = from_b32_init(i, public_b32);
    VTC_ASSERT(err_code);

    // copy public key
    memcpy(m_accounts[i].account.info.public_b32,
           public_b32,
           sizeof(m_accounts[i].account.info.public_b32));

    // update account info
    err_code = provider_account_info_get(&m_accounts[i].account);

    *account = (account_info_t *) &m_accounts[i].account;

    return err_code;
}

bool
account_has_app(account_info_t *account, uint64_t app_id)
{
    if (account_exists(account))
    {
        account_details_t *details = (account_details_t *) account;

        for (uint32_t j = 0; j < details->app_idx; ++j)
        {
            if (details->apps_local[j].app_id == app_id)
            {
                return true;
            }
        }
    }

    return false;
}

ret_code_t
account_balance(account_info_t *account, int32_t *balance)
{
    if (account_exists(account))
    {
        *balance = account->amount;
    }
    else
    {
        return VTC_ERROR_INVALID_PARAM;
    }

    return VTC_SUCCESS;
}

ret_code_t
account_free(account_info_t *account)
{
    uint32_t i = 0;
    for (; i < ACCOUNTS_MAXIMUM_COUNT; ++i)
    {
        if (account == (account_info_t *) &m_accounts[i].account)
        {
            m_accounts[i].status = ACCOUNT_NONE;
            memset(&m_accounts[i].account, 0, sizeof(account_details_t));
            break;
        }
    }

    if (i == ACCOUNTS_MAXIMUM_COUNT)
    {
        return VTC_ERROR_NOT_FOUND;
    }

    LOG_INFO("👛 Deleted account from wallet: #%u", i);

    return VTC_SUCCESS;
}

ret_code_t
account_update(account_info_t *account)
{
    if (account_exists(account))
    {
        account_details_t *details = (account_details_t *) account;

        // update account info
        return provider_account_info_get(details);
    }
    else
    {
        return VTC_ERROR_INVALID_PARAM;
    }
}

ret_code_t
account_init()
{
    memset(m_accounts, 0, sizeof m_accounts);

    return VTC_SUCCESS;
}
