//
// Created by Cyril on 17/03/2021.
//

#include "../../inc/vertices.h"
#include <vertices_log.h>
#include "../../inc/vertices_errors.h"
#include <vertices_config.h>
#include <provider.h>

#define PROVIDERS_NUMBER 2
#define ACCOUNT_NUMBER 2

static provider_info_t providers[PROVIDERS_NUMBER] =
{
    {.url = SERVER_URL, .port = SERVER_PORT, .token = SERVER_TOKEN},
    {.url = SERVER_URL, .port = SERVER_PORT, .token = SERVER_TOKEN},
};

static account_info_t accounts[ACCOUNT_NUMBER] = {
    {.public_addr = "RQKZZG4H7XDK72D2MK34UIP2UIJRXDDYMY6P3UZU4TZSC4G4IMDF63HPHE", .amount = 0},
    {.public_addr = "RQKZZG4H7XDK72D2MK34UIP2UIJRXDDYMY6P3UZU4TZSC4G4IMDF63HPHE", .amount = 0},
};

static vertex_t m_vertex = {
    .providers = providers,
    .provider_count = PROVIDERS_NUMBER,
};

int
main(int argc, char *argv[])
{
    LOG_INFO("😎 Vertices SDK running on Unix-based OS");

    // create new vertex
    err_code_t err_code = vertices_new(&m_vertex);
    VTC_ASSERT(err_code);

    // making sure the provider is accessible
    err_code = vertices_ping();
    VTC_ASSERT(err_code);

    // ask for provider version
    provider_version_t version = {0};
    err_code = vertices_version(&version);
    if(err_code == VTC_ERROR_OFFLINE)
    {
        LOG_WARNING("Version might not be accurate: old value is being used");
    }
    else
    {
        VTC_ASSERT(err_code);
    }

    LOG_INFO("Running on %s v.%u.%u.%u. Genesis: %s",
             version.network,
             version.major,
             version.minor,
             version.patch,
             version.genesis_hash);

    // create an account
    size_t account_handle = 0;
    err_code = vertices_add_account(&accounts[0], &account_handle);
    VTC_ASSERT(err_code);

    // delete the created account
    err_code = vertices_del_account(account_handle);
    VTC_ASSERT(err_code);
}