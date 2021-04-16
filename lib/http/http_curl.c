//
// Created by Cyril on 18/03/2021.
//

#include "../../inc/vertices_errors.h"
#include <provider.h>
#include <curl/curl.h>
#include <vertices_log.h>
#include <string.h>
#include <libc.h>

static CURL *m_curl;

ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(void *, size_t, size_t, void *))
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    m_curl = curl_easy_init();

    if (m_curl == NULL)
    {
        return VTC_ERROR_INTERNAL;
    }

    if (provider->port != 0)
    {
        curl_easy_setopt(m_curl, CURLOPT_PORT, provider->port);
    }
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, response_payload_cb);

    return VTC_SUCCESS;
}

ret_code_t
http_get(const provider_info_t *provider,
         char *relative_path,
         const char *headers,
         payload_t *response_buf)
{
    VTC_ASSERT_BOOL(m_curl != NULL);

    ret_code_t err_code = VTC_SUCCESS;
    CURLcode res;
    long response_code;

    char url_full[512] = {0};
    sprintf(url_full, "%s%s", provider->url, relative_path);

    if (m_curl)
    {
        /* set our custom set of headers */
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, headers);
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(m_curl, CURLOPT_URL, url_full);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, response_buf);

#ifdef DEBUG
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
#endif

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(m_curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            LOG_ERROR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            err_code = VTC_ERROR_INTERNAL;
        }
        else
        {
            curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);
            LOG_DEBUG("GET %s response %ld", url_full, response_code);

            if (response_code >= 300)
            {
                err_code = VTC_HTTP_ERROR + response_code;
            }
        }

        /* free the custom headers */
        curl_slist_free_all(chunk);
    }
    else
    {
        err_code = VTC_ERROR_INVALID_STATE;
    }

    return err_code;
}

ret_code_t
http_post(const provider_info_t *provider,
          char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          payload_t *response_buf)
{
    VTC_ASSERT_BOOL(m_curl != NULL);

    ret_code_t err_code = VTC_SUCCESS;
    CURLcode res;
    long response_code;

    char url_full[256] = {0};
    sprintf(url_full, "%s%s", provider->url, relative_path);

    if (m_curl)
    {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(m_curl, CURLOPT_URL, url_full);
        /* Now specify the POST data */
        curl_easy_setopt(m_curl, CURLOPT_POST, 1);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, body);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, body_size);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

        /* set our custom set of headers */
        struct curl_slist *chunk = NULL;

        char *header_line = headers;
        for (size_t i = 0; headers[i] != 0; ++i)
        {
            if (headers[i] == '\r' && headers[i + 1] == '\n')
            {
                headers[i] = 0;
                chunk = curl_slist_append(chunk, header_line);
                header_line = &headers[i] + 2;
            }
        }
        chunk = curl_slist_append(chunk, header_line);

        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, response_buf);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(m_curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            LOG_ERROR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            err_code = VTC_ERROR_INTERNAL;
        }
        else
        {
            curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);
            LOG_DEBUG("POST %s response %ld", url_full, response_code);

            if (response_code >= 300)
            {
                err_code = VTC_HTTP_ERROR + response_code;
            }
        }

        /* free the custom headers */
        curl_slist_free_all(chunk);
    }
    else
    {
        err_code = VTC_ERROR_INVALID_STATE;
    }

    return err_code;
}

void
http_close()
{
    curl_easy_cleanup(m_curl);
    curl_global_cleanup();

    m_curl = NULL;
}