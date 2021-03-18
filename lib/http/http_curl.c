//
// Created by Cyril on 18/03/2021.
//

#include <vertices_errors.h>
#include <provider.h>
#include <curl/curl.h>
#include <vertices_log.h>
#include <string.h>
#include <libc.h>

static CURL *m_curl;

err_code_t
http_init(const provider_t *provider)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    m_curl = curl_easy_init();

    if (m_curl == NULL)
    {
        return VTC_ERROR_INTERNAL;
    }

    curl_easy_setopt(m_curl, CURLOPT_PORT, provider->port);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, provider->response_payload_cb);

    return VTC_SUCCESS;
}

err_code_t
http_get(const provider_t *provider, char *relative_path, const char *headers)
{
    VTC_ASSERT_BOOL(m_curl != NULL);

    err_code_t ret_code = VTC_SUCCESS;
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
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &provider->response_buffer);

#ifdef DEBUG
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
#endif

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(m_curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            LOG_ERROR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            ret_code = VTC_ERROR_INTERNAL;
        }

        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);
        LOG_DEBUG("GET %s response %ld", url_full, response_code);

        /* free the custom headers */
        curl_slist_free_all(chunk);
    }
    else
    {
        ret_code = VTC_ERROR_INVALID_STATE;
    }

    return ret_code;
}

err_code_t
http_post(const provider_t *provider, char *relative_path, const char *headers, const char *body)
{
    VTC_ASSERT_BOOL(m_curl != NULL);

    err_code_t ret_code = VTC_SUCCESS;
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

        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(m_curl, CURLOPT_URL, url_full);
        /* Now specify the POST data */
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, relative_path);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(m_curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            LOG_ERROR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);
        LOG_DEBUG("GET %s response %ld", url_full, response_code);

        /* free the custom headers */
        curl_slist_free_all(chunk);
    }
    else
    {
        ret_code = VTC_ERROR_INVALID_STATE;
    }

    return ret_code;
}

void
http_close()
{
    curl_easy_cleanup(m_curl);
    curl_global_cleanup();

    m_curl = NULL;
}