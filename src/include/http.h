//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_INC_HTTP_H
#define VERTICES_INC_HTTP_H

#include "vertices_types.h"

#ifndef HTTP_MAXIMUM_CONTENT_LENGTH
#define HTTP_MAXIMUM_CONTENT_LENGTH 4096
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

ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(void *received_data,
                                        size_t size,
                                        size_t count,
                                        void *response_payload));

ret_code_t
http_get(const provider_info_t *provider,
         const char *relative_path,
         const char *headers,
         payload_t *response_buf,
         uint32_t *response_code);

/// Post HTTP payload
/// \param provider
/// \param relative_path
/// \param headers Headers, must be separated by `\r\n`
/// \param body
/// \param body_size
/// \return
ret_code_t
http_post(const provider_info_t *provider,
          const char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          payload_t *response_buf,
          long *response_code);

void
http_close(void);

#endif //VERTICES_INC_HTTP_H
