//
// Created by Cyril on 18/03/2021.
//

#include "vertices_errors.h"
#include <provider.h>
#include <vertices_log.h>
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "http_esp";

static esp_http_client_handle_t m_client_handle = NULL;
static size_t (*m_response_payload_cb)(void *received_data,
                                        size_t size,
                                        size_t count,
                                        void *response_payload);
                                        
static esp_err_t http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s\n", evt->data_len, (char*)evt->data);
                if (m_response_payload_cb != NULL)
                {
                    m_response_payload_cb(evt->data, evt->data_len, 1, NULL);
                }
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            m_client_handle = NULL;
            break;
    }
    return ESP_OK;
}

ret_code_t
http_init(const provider_info_t *provider,
          size_t (*response_payload_cb)(void *received_data,
                                        size_t size,
                                        size_t count,
                                        void *response_payload))
{
    if (response_payload_cb != NULL)
    {
        m_response_payload_cb = response_payload_cb;
    }

    return VTC_SUCCESS;
}

ret_code_t
http_get(const provider_info_t *provider,
         char *relative_path,
         const char *headers,
         payload_t *response_buf,
         uint32_t *response_code)
{
    esp_http_client_config_t config = {
        .host = provider->url,
        .path = relative_path,
        .event_handler = http_event_handle,
        .user_data = response_buf->data,        // Pass address of local buffer to get response
        .buffer_size = response_buf->size,
        .disable_auto_redirect = true,
    };

    m_client_handle = esp_http_client_init(&config);
    if (m_client_handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialise HTTP connection");
        return VTC_ERROR_OFFLINE;
    }

    esp_err_t err = esp_http_client_set_method(m_client_handle, HTTP_METHOD_GET);
    VTC_ASSERT_BOOL(err == ESP_OK);

    // TODO set headers

    err = esp_http_client_perform(m_client_handle);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(m_client_handle),
                esp_http_client_get_content_length(m_client_handle));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    return VTC_SUCCESS;
}

ret_code_t
http_post(const provider_info_t *provider,
          char *relative_path,
          char *headers,
          const char *body,
          size_t body_size,
          payload_t *response_buf,
          long *response_code)
{
    return VTC_SUCCESS;
}

void http_close(void)
{
}
