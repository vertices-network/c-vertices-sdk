//
// Created by Cyril on 18/03/2021.
//

#include <vertices_log.h>
#include <libc.h>
#include "vertices_errors.h"
#include <http.h>
#include "vertices.h"
#include "provider.h"
#include "cJSON.h"

static size_t
response_payload_callback(void *received_data, size_t size, size_t count, void *response_payload);

static char rx_buf[HTTP_MAXIMUM_CONTENT_LENGTH];

static provider_t m_provider = {0};

static size_t
response_payload_callback(void *received_data, size_t size, size_t count, void *response_payload)
{
    size_t received_data_size = size * count;

    VTC_ASSERT_BOOL(received_data_size < HTTP_MAXIMUM_CONTENT_LENGTH);

    payload_t *payload = (payload_t *) response_payload;
    payload->data = rx_buf;
    payload->size = received_data_size;

    memcpy(rx_buf, received_data, received_data_size);

    return received_data_size;
}

err_code_t
provider_get_version(provider_version_t *version)
{
    err_code_t
        err_code = http_get(&m_provider.providers[0], "/versions", "", &m_provider.response_buffer);

    if (err_code == VTC_SUCCESS)
    {
        memset(version, 0, sizeof(provider_version_t));

        cJSON *json = cJSON_Parse(rx_buf);
        if (json == NULL)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                LOG_ERROR("JSON, error before: %s", error_ptr);
            }
            err_code = VTC_ERROR_INTERNAL;
        }
        else
        {
            const cJSON *genesis_id = cJSON_GetObjectItemCaseSensitive(json, "genesis_id");
            if (cJSON_IsString(genesis_id) && (genesis_id->valuestring != NULL))
            {
                if (strlen(genesis_id->valuestring) >= sizeof version->network)
                {
                    err_code = VTC_ERROR_NO_MEM;
                }
                else
                {
                    strcpy(version->network, genesis_id->valuestring);
                }
            }

            const cJSON *genesis_hash = cJSON_GetObjectItemCaseSensitive(json, "genesis_hash_b64");
            if (cJSON_IsString(genesis_hash) && (genesis_hash->valuestring != NULL))
            {
                if (strlen(genesis_id->valuestring) >= sizeof version->network)
                {
                    err_code = VTC_ERROR_NO_MEM;
                }
                else
                {
                    strcpy(version->genesis_hash, genesis_hash->valuestring);
                }
            }

            const cJSON *build = cJSON_GetObjectItemCaseSensitive(json, "build");
            const cJSON *major = cJSON_GetObjectItemCaseSensitive(build, "major");
            const cJSON *minor = cJSON_GetObjectItemCaseSensitive(build, "minor");
            const cJSON *patch = cJSON_GetObjectItemCaseSensitive(build, "patch");

            if (cJSON_IsNumber(major))
            {
                version->major = major->valueint;
            }
            if (cJSON_IsNumber(minor))
            {
                version->minor = minor->valueint;
            }
            if (cJSON_IsNumber(patch))
            {
                version->patch = patch->valueint;
            }
        }

        cJSON_Delete(json);
    }

    return err_code;
}

err_code_t
provider_ping()
{
    err_code_t
        err_code = http_get(&m_provider.providers[0], "/health", "", &m_provider.response_buffer);
    return err_code;
}

err_code_t
provider_init(http_remote_t *providers, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        m_provider.providers[i].url = providers[i].url;
        m_provider.providers[i].port = providers[i].port;
        m_provider.providers[i].token = providers[i].token;
    }

    m_provider.response_payload_cb = response_payload_callback;

    err_code_t err_code = http_init(&m_provider.providers[0], response_payload_callback);
    VTC_ASSERT(err_code);

    return err_code;
}