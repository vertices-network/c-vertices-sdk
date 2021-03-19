//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_INC_HTTP_H
#define VERTICES_INC_HTTP_H

#include "vertices_types.h"

#ifndef HTTP_MAXIMUM_CONTENT_LENGTH
#define HTTP_MAXIMUM_CONTENT_LENGTH 1024
#endif

typedef struct
{
    char *data;
    size_t size;
} payload_t;

typedef struct
{
    char *url;
    short port;
    char *token;
} http_remote_t;

err_code_t
http_init(const provider_info_t * provider, size_t (*response_payload_cb)(void *received_data, size_t size, size_t count, void *response_payload));

err_code_t
http_get(const provider_info_t * provider, char* relative_path, const char * headers, payload_t* response_buf);

err_code_t
http_post(const provider_info_t * provider, char* relative_path, const char * headers, const char* body);

void
http_close();

#endif //VERTICES_INC_HTTP_H
