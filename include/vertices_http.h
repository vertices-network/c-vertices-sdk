//! @file
//!
//! Vertices Network
//! See License.txt for details
//!
//! Created by Cyril on 18/03/2021.

#ifndef VERTICES_HTTP_H
#define VERTICES_HTTP_H

#include "vertices_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HTTP_MAXIMUM_CONTENT_LENGTH
#define HTTP_MAXIMUM_CONTENT_LENGTH 4096
#endif

typedef struct
{
    char *data;
    size_t size;
} payload_t;

/// Init HTTP client
/// \param provider Remote API URL, port, specific header and certificate if needed by the client
/// \param response_payload_cb Pointer to \payload_t where the response will be written.
/// \return \c VTC_SUCCESS on success, otherwise error depends on implementation
ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(void *chunk,
                                        size_t chunk_size,
                                        payload_t *response_payload));

/// HTTP GET request
/// \param provider pointer to provider (url, port..)
/// \param relative_path path to append to the provider base URL
/// \param headers Headers string, each one must have the format "key:value\n\r"
/// \param response_buf Pointer to where to put the response data (body)
/// \param response_code HTTP response code
/// \return error codes:
/// - \c VTC_SUCCESS on success
/// - \c VTC_ERROR_OFFLINE if offline
/// - \c VTC_HTTP_ERROR (+error) on HTTP error.
ret_code_t
http_get(const provider_info_t *provider,
         const char *relative_path,
         const char *headers,
         payload_t *response_buf,
         uint32_t *response_code);

/// HTTP POST request
/// \param provider pointer to provider (url, port..)
/// \param relative_path path to append to the provider base URL
/// \param headers Headers string, each one must have the format "key:value\n\r"
/// \param body HTTP body
/// \param body_size size of \c body array
/// \param response_buf Pointer to where to put the response data (body)
/// \param response_code HTTP response code
/// \return error codes:
/// - \c VTC_SUCCESS on success
/// - \c VTC_ERROR_OFFLINE if offline
/// - \c VTC_HTTP_ERROR (+error) on HTTP error.
ret_code_t
http_post(const provider_info_t *provider,
          const char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          payload_t *response_buf,
          uint32_t *response_code);

void
http_close(void);

#ifdef __cplusplus
}
#endif

#endif //VERTICES_HTTP_H
