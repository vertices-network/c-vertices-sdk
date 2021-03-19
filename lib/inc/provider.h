//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_SRC_PROVIDER_H
#define VERTICES_SRC_PROVIDER_H

#include <stdio.h>
#include "../../inc/vertices_types.h"
#include "http.h"

#ifndef PROVIDER_MAXIMUM_FALLBACKS
#define PROVIDER_MAXIMUM_FALLBACKS 4
#endif

typedef struct
{
    http_remote_t providers[PROVIDER_MAXIMUM_FALLBACKS];
    provider_version_t version;
    payload_t response_buffer;
    size_t
    (*response_payload_cb)(void *received_data, size_t size, size_t count, void *response_payload);
} provider_t;

///
/// \param version
/// \return
/// * VTC_ERROR_OFFLINE version has not been updated with latest values due to error on HTTP request
/// * VTC_SUCCESS       version has correctly been fetched from remote
err_code_t
provider_get_version(provider_version_t * version);

err_code_t
provider_ping();

err_code_t
provider_init(http_remote_t *providers, size_t count);

#endif //VERTICES_SRC_PROVIDER_H
