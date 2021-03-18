//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_SRC_PROVIDER_H
#define VERTICES_SRC_PROVIDER_H

#include <stdio.h>
#include "vertices_errors.h"

typedef struct
{
    char *data;
    size_t size;
} payload_t;

typedef struct
{
    char *url;
    short port;
    payload_t response_buffer;
    size_t
    (*response_payload_cb)(void *received_data, size_t size, size_t count, void *response_payload);
} provider_t;

typedef struct {
    char network[64];
    char genesis_hash[64];
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
} provider_version_t;

err_code_t
provider_get_version(provider_version_t * version);

err_code_t
provider_ping();

err_code_t
provider_init();

#endif //VERTICES_SRC_PROVIDER_H
