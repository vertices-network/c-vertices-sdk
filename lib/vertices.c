//
// Created by Cyril on 17/03/2021.
//

#include <provider.h>
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
vertices_new(vertex_t * config)
{
    err_code_t err_code = VTC_SUCCESS;

    provider_init((http_remote_t *) config->providers, config->count);

    return err_code;
}
