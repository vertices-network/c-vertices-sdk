//
// Created by Cyril on 17/03/2021.
//

#include "../../inc/vertices.h"
#include <vertices_log.h>
#include "../../inc/vertices_errors.h"
#include <vertices_config.h>

#define PROVIDERS_COUNT 2

static provider_info_t providers[PROVIDERS_COUNT] =
{
    {.url = SERVER_URL, .port = SERVER_PORT, .token = SERVER_TOKEN},
    {.url = SERVER_URL, .port = SERVER_PORT, .token = SERVER_TOKEN},
};

static vertex_t m_vertex = {
    .providers = providers,
    .count = PROVIDERS_COUNT,
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
    VTC_ASSERT(err_code);

    LOG_INFO("Running on %s %u.%u.%u %s",
             version.network,
             version.major,
             version.minor,
             version.patch,
             version.genesis_hash);

    LOG_DEBUG("Returned %d", err_code);
}