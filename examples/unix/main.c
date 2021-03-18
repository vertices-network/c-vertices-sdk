//
// Created by Cyril on 17/03/2021.
//

#include <vertices.h>
#include <vertices_log.h>
#include <vertices_errors.h>

int
main(int argc, char *argv[])
{
    LOG_INFO("😎 Vertices SDK running on Unix-based OS");

    // create new vertex
    err_code_t err_code = vertices_new();
    VTC_ASSERT(err_code);

    // ask for provider version
    provider_version_t version = {0};
    err_code = provider_get_version(&version);
    VTC_ASSERT(err_code);

    LOG_INFO("Running on %s %u.%u.%u %s",
             version.network,
             version.major,
             version.minor,
             version.patch,
             version.genesis_hash);

    LOG_DEBUG("Returned %d", err_code);
}