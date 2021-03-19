//
// Created by Cyril on 19/03/2021.
//

#ifndef VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
#define VERTICES_SDK_LIB_INC_VERTICES_TYPES_H

#include "vertices_errors.h"

typedef struct
{
    char *url;
    short port;
    char *token;
} provider_info_t;

typedef struct {
    char * public_addr;
    long long amount;
} account_info_t;

typedef struct
{
    provider_info_t *providers;
    size_t provider_count;
} vertex_t;

typedef struct
{
    char network[64];
    char genesis_hash[64];
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
} provider_version_t;

#endif //VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
