//
// Created by Cyril on 18/06/2021.
//

#include <vertices_log.h>
#include "vertices_http.h"
#include "compilers.h"

/*
 * ⚠️ This file contains weak implementations of HTTP functions
 * It is provided so that the Vertices SDK can be compiled and linked as a static library.
 * For the Vertices SDK to work, the user **must** declare and define a strong implementation.
 * Examples of such implementations are available, see documentation at docs.vertices.network.
 */


__WEAK ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(void *received_data,
                                        size_t size,
                                        size_t count,
                                        void *response_payload))
{
    LOG_ERROR("Weak implementation of http_init");

    return VTC_ERROR_NOT_FOUND;
}

__WEAK ret_code_t
http_get(const provider_info_t *provider,
         const char *relative_path,
         const char *headers,
         payload_t *response_buf,
         uint32_t *response_code)
{
    LOG_ERROR("Weak implementation of http_get");

    return VTC_ERROR_NOT_FOUND;
}

__WEAK ret_code_t
http_post(const provider_info_t *provider,
          const char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          payload_t *response_buf,
          uint32_t *response_code)
{
    LOG_ERROR("Weak implementation of http_post");

    return VTC_ERROR_NOT_FOUND;
}

__WEAK void
http_close(void)
{

}
