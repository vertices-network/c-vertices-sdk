//
// Created by Cyril on 17/03/2021.
//

#include <provider.h>
#include <account.h>
#include "vertices.h"

static int m_socket = 0;

err_code_t
vertices_version(provider_version_t *version)
{
    return provider_get_version(version);
}

err_code_t
vertices_ping()
{
    return provider_ping();
}

err_code_t
vertices_add_account(account_info_t * account, size_t * account_id)
{
    return account_add(account, account_id);
}

err_code_t
vertices_del_account(size_t account_handle)
{
    return account_delete(account_handle);
}

err_code_t
vertices_new(vertex_t * config)
{
    err_code_t err_code = VTC_SUCCESS;

    provider_init(config->providers, config->provider_count);
    account_init();

    return err_code;
}
