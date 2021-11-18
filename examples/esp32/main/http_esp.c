/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "vertices_errors.h"
#include <provider.h>
#include <vertices_log.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "http_esp";

static esp_http_client_handle_t m_client_handle = NULL;
static size_t
(*m_response_payload_cb)(char *received_data,
                         size_t size);

static esp_err_t
http_event_handle(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ERROR:
        {
            ESP_LOGV(TAG, "HTTP_EVENT_ERROR");
        }
            break;
        case HTTP_EVENT_ON_CONNECTED:
        {
            ESP_LOGV(TAG, "HTTP_EVENT_ON_CONNECTED");
        }
            break;
        case HTTP_EVENT_HEADER_SENT:
        {
            ESP_LOGV(TAG, "HTTP_EVENT_HEADER_SENT");
        }
            break;
        case HTTP_EVENT_ON_HEADER:
        {
            ESP_LOGV(TAG, "Header %.*s", evt->data_len, (char *) evt->data);
        }
            break;
        case HTTP_EVENT_ON_DATA:
        {
            ESP_LOGV(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client))
            {
                if (m_response_payload_cb != NULL)
                {
                    m_response_payload_cb(evt->data, evt->data_len);
                }
            }
        }
            break;
        case HTTP_EVENT_ON_FINISH:
        {
            ESP_LOGV(TAG, "HTTP_EVENT_ON_FINISH");
        }
            break;
        case HTTP_EVENT_DISCONNECTED:
        {
            ESP_LOGV(TAG, "HTTP_EVENT_DISCONNECTED");
        }
            break;
    }
    return ESP_OK;
}

static void
set_headers(const char *headers, size_t len)
{
    char header_copy[len + 1];
    memcpy(header_copy, headers, len);
    header_copy[len] = 0;

    int key_idx = 0;
    int value_idx = 0;
    for (int i = 0; header_copy[i] != 0;)
    {
        key_idx = i;

        while (header_copy[i] != ':')
        {
            ++i;
        }
        header_copy[i] = 0;
        while (header_copy[i] == ' ')
        {
            ++i;
        };
        value_idx = i;
        while (header_copy[i] != '\n' && header_copy[i] != '\r' && header_copy[i] != 0)
        {
            ++i;
        }

        while (header_copy[i] == '\n' || header_copy[i] == '\r')
        {
            header_copy[i] = 0;
            ++i;
        }

        ESP_LOGI(TAG, "Adding header: %s: %s", &header_copy[key_idx], &header_copy[value_idx]);
        esp_http_client_set_header(m_client_handle, &header_copy[key_idx], &header_copy[value_idx]);
    }
}

ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(char *chunk,
                                        size_t chunk_size))
{
    if (response_payload_cb != NULL)
    {
        m_response_payload_cb = response_payload_cb;
    }

    ESP_LOGI(TAG, "HTTP init: %s", provider->url);

    m_client_handle = NULL;

    return VTC_SUCCESS;
}

ret_code_t
http_get(const provider_info_t *provider,
         const char *relative_path,
         const char *headers,
         uint32_t *response_code)
{
    esp_http_client_config_t config = {
        .host = provider->url,
        .path = relative_path,
        .event_handler = http_event_handle,
        .user_data = NULL,        // Pass address of local buffer to get response
        .buffer_size = 0,
        .disable_auto_redirect = false,
        .cert_pem = provider->cert_pem,
    };

    // if the handle doesn't exist yet, init
    if (m_client_handle == NULL)
    {
        m_client_handle = esp_http_client_init(&config);
        if (m_client_handle == NULL)
        {
            ESP_LOGE(TAG, "Failed to initialise HTTP connection");
            return VTC_ERROR_OFFLINE;
        }
    }

    // set the options of the GET request
    char full_url[128] = {0};
    size_t len = sprintf(full_url, "%s%s", provider->url, relative_path);
    VTC_ASSERT_BOOL(len < 128);

    LOG_DEBUG("GET %s", full_url);

    esp_http_client_set_url(m_client_handle, full_url);

    esp_err_t err = esp_http_client_set_method(m_client_handle, HTTP_METHOD_GET);
    VTC_ASSERT_BOOL(err == ESP_OK);

    set_headers(headers, strlen(headers));

    err = esp_http_client_perform(m_client_handle);
    if (err == ESP_OK)
    {
        *response_code = esp_http_client_get_status_code(m_client_handle);

        LOG_DEBUG("GET %s response %u", full_url, *response_code);

        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                 *response_code,
                 esp_http_client_get_content_length(m_client_handle));

        if (*response_code >= 300)
        {
            return VTC_ERROR_HTTP_BASE;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return VTC_ERROR_HTTP_BASE + err;
    }

    return VTC_SUCCESS;
}

ret_code_t
http_post(const provider_info_t *provider,
          const char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          uint32_t *response_code)
{

    esp_http_client_config_t config = {
        .host = provider->url,
        .path = relative_path,
        .event_handler = http_event_handle,
        .user_data = NULL,        // Pass address of local buffer to get response
        .buffer_size = 0,
        .disable_auto_redirect = true,
        .cert_pem = provider->cert_pem,
    };

    if (m_client_handle == NULL)
    {
        m_client_handle = esp_http_client_init(&config);

        if (m_client_handle == NULL)
        {
            ESP_LOGE(TAG, "Failed to initialise HTTP connection");
            return VTC_ERROR_OFFLINE;
        }
    }

    // POST
    esp_http_client_set_method(m_client_handle, HTTP_METHOD_POST);

    // url
    char full_url[128] = {0};
    size_t len = sprintf(full_url, "%s%s", provider->url, relative_path);
    VTC_ASSERT_BOOL(len < 128);

    esp_http_client_set_url(m_client_handle, full_url);

    // headers
    set_headers(headers, strlen(headers));

    // body
    esp_http_client_set_post_field(m_client_handle, body, (int) body_size);

    // send
    esp_err_t err = esp_http_client_perform(m_client_handle);
    if (err == ESP_OK)
    {
        *response_code = esp_http_client_get_status_code(m_client_handle);

        LOG_DEBUG("POST %s response %u", full_url, *response_code);

        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 *response_code,
                 esp_http_client_get_content_length(m_client_handle));

        if (*response_code >= 300)
        {
            return VTC_ERROR_HTTP_BASE;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        return VTC_ERROR_HTTP_BASE + err;
    }

    return VTC_SUCCESS;
}

void
http_close(void)
{
    if (m_client_handle != NULL)
    {
        esp_http_client_cleanup(m_client_handle);
        m_client_handle = NULL;
    }
}

void
force_linker_inclusion()
{
    //dummy
}