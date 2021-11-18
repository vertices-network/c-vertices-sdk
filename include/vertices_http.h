/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_HTTP_H
#define VERTICES_HTTP_H

#include "vertices_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// if not defined before, set default value
#ifndef HTTP_MAXIMUM_CONTENT_LENGTH
#define HTTP_MAXIMUM_CONTENT_LENGTH 4096
#endif

/// Init HTTP client
/// \param provider Remote API URL, port, specific header and certificate if needed by the client
/// \param response_payload_cb Function callback to call when data is ready to be parsed
/// \return \c VTC_SUCCESS on success, otherwise error depends on implementation
ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(char *chunk,
                                        size_t chunk_size));

/// HTTP GET request
/// \param provider pointer to provider (url, port..)
/// \param relative_path path to append to the provider base URL
/// \param headers Headers string, each one must have the format "key:value\n\r"
/// \param response_code HTTP response code
/// \return error codes:
/// - \c VTC_SUCCESS on success
/// - \c VTC_ERROR_OFFLINE if offline
/// - \c VTC_ERROR_HTTP_BASE on HTTP error: check response_code.
ret_code_t
http_get(const provider_info_t *provider,
         const char *relative_path,
         const char *headers,
         uint32_t *response_code);

/// HTTP POST request
/// \param provider pointer to provider (url, port..)
/// \param relative_path path to append to the provider base URL
/// \param headers Headers string, each one must have the format "key:value\n\r"
/// \param body HTTP body
/// \param body_size size of \c body array
/// \param response_code HTTP response code
/// \return error codes:
/// - \c VTC_SUCCESS on success
/// - \c VTC_ERROR_OFFLINE if offline
/// - \c VTC_ERROR_HTTP_BASE on HTTP error: check response_code.
ret_code_t
http_post(const provider_info_t *provider,
          const char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          uint32_t *response_code);

/// Close/deinit the client
void
http_close(void);

#ifdef __cplusplus
}
#endif

#endif //VERTICES_HTTP_H
