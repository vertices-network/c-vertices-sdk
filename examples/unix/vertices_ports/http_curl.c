//
// Created by Cyril on 18/03/2021.
//

#include "vertices_errors.h"
#include <provider.h>
#include <curl/curl.h>
#include <vertices_log.h>
#include <string.h>

static CURL *m_curl;
static size_t
(*m_response_payload_cb)(char *chunk,
                         size_t chunk_size);

static size_t
response_callback(void *chunk,
                  size_t size,
                  size_t nmemb,
                  void *userdata)
{
    return m_response_payload_cb(chunk, size * nmemb);
}

ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(char *chunk,
                                        size_t chunk_size))
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    m_curl = curl_easy_init();

    if (m_curl == NULL)
    {
        return VTC_ERROR_INTERNAL;
    }

    m_response_payload_cb = response_payload_cb;

    if (provider->port != 0)
    {
        curl_easy_setopt(m_curl, CURLOPT_PORT, provider->port);
    }
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, (void *) response_callback);

    return VTC_SUCCESS;
}

ret_code_t
http_get(const provider_info_t *provider,
         const char *relative_path,
         const char *headers,
         uint32_t *response_code)
{
    VTC_ASSERT_BOOL(m_curl != NULL);

    ret_code_t err_code = VTC_SUCCESS;
    CURLcode res;
    long response;

    char url_full[512] = {0};
    sprintf(url_full, "%s%s", provider->url, relative_path);

    if (m_curl)
    {
        /* set our custom set of headers */
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, headers);
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(m_curl, CURLOPT_URL, url_full);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, NULL);

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
            curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response);
            LOG_DEBUG("GET %s response %ld", url_full, response);

            if (response >= 300)
            {
                // todo does the response code enter into uint32_t?
                *response_code = (uint32_t) response;
                err_code = VTC_HTTP_ERROR;
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
          const char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          uint32_t *response_code)
{
    VTC_ASSERT_BOOL(m_curl != NULL);

    ret_code_t err_code = VTC_SUCCESS;
    CURLcode res;

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

#ifdef DEBUG
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
#endif

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
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, NULL);

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
            curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, response_code);
            LOG_DEBUG("POST %s response %u", url_full, *response_code);

            if (*response_code >= 300)
            {
                err_code = VTC_HTTP_ERROR;
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
