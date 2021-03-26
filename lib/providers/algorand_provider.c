//
// Created by Cyril on 18/03/2021.
//

#include <vertices_log.h>
#include <libc.h>
#include "vertices_errors.h"
#include <http.h>
#include <mpack-reader.h>
#include "provider.h"
#include "cJSON.h"

static size_t
response_payload_callback(void *received_data, size_t size, size_t count, void *response_payload);

static char rx_buf[HTTP_MAXIMUM_CONTENT_LENGTH];
static provider_t m_provider = {0};

static char base_headers[128] = {0}; /// contains x-api-key

static size_t
response_payload_callback(void *received_data, size_t size, size_t count, void *response_payload)
{
    size_t received_data_size = size * count;

    LOG_DEBUG("Received %zu bytes", received_data_size);

    VTC_ASSERT_BOOL(received_data_size < HTTP_MAXIMUM_CONTENT_LENGTH);

    payload_t *payload = (payload_t *) response_payload;
    payload->data = rx_buf;
    payload->size = received_data_size;

    memcpy(rx_buf, received_data, received_data_size);

    return received_data_size;
}

err_code_t
provider_get_account_info(account_details_t *account)
{
    char relative_path[128] = {0};

    size_t
        len = sprintf(relative_path, "/v2/accounts/%s?format=msgpack", account->info->public_addr);
    VTC_ASSERT_BOOL(len < 128);

    err_code_t err_code = http_get(&m_provider.providers[0],
                                   relative_path, base_headers, &m_provider.response_buffer);
    if (err_code == VTC_SUCCESS)
    {
        // we are reading messagepack data
        mpack_reader_t reader;
        mpack_reader_init_data(&reader, rx_buf, m_provider.response_buffer.size);

        mpack_tag_t tag = mpack_read_tag(&reader);
        if (mpack_reader_error(&reader) != mpack_ok)
        {
            return VTC_ERROR_INTERNAL;
        }

        // response to /account is a map with different optional fields
        // see https://developer.algorand.org/docs/reference/rest-apis/algod/v2/#account
        // `algo`, `appl`, `tsch`, `ebase` ... etc
        // TODO parse in account.c?
        if (mpack_tag_type(&tag) == mpack_type_map)
        {
            uint32_t count = mpack_tag_map_count(&tag);

            for (uint32_t i = 0; i < count; ++i)
            {
                char str_buf[64] = {0};
                mpack_tag_t tag = mpack_read_tag(&reader);
                if (mpack_tag_type(&tag) == mpack_type_str)
                {
                    uint32_t length = mpack_tag_str_length(&tag);
                    const char *data = mpack_read_bytes_inplace(&reader, length);
                    memcpy(str_buf, data, length);

                    mpack_tag_t value = mpack_read_tag(&reader);
                    if (mpack_tag_type(&value) == mpack_type_uint)
                    {
                        long long long_value = mpack_tag_uint_value(&value);
                        if (strcmp(str_buf, "algo") == 0)
                        {
                            account->info->amount = long_value;
                        }
                        else if (strcmp(str_buf, "ebase") == 0)
                        {
                            // don't care at the moment
                        }
                        LOG_DEBUG("%s = %llu", str_buf, long_value);
                    }
                }
            }

            mpack_done_map(&reader);
        }

        VTC_ASSERT_BOOL(mpack_reader_destroy(&reader) == mpack_ok);
    }

    return err_code;
}

err_code_t
provider_get_version(provider_version_t *version)
{
    err_code_t
        err_code = http_get(&m_provider.providers[0], "/versions", base_headers, &m_provider.response_buffer);

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
                if (strlen(genesis_id->valuestring) >= sizeof m_provider.version.network)
                {
                    err_code = VTC_ERROR_NO_MEM;
                }
                else
                {
                    strcpy(m_provider.version.network, genesis_id->valuestring);
                }
            }

            const cJSON *genesis_hash = cJSON_GetObjectItemCaseSensitive(json, "genesis_hash_b64");
            if (cJSON_IsString(genesis_hash) && (genesis_hash->valuestring != NULL))
            {
                if (strlen(genesis_hash->valuestring) >= sizeof m_provider.version.genesis_hash)
                {
                    err_code = VTC_ERROR_NO_MEM;
                }
                else
                {
                    strcpy(m_provider.version.genesis_hash, genesis_hash->valuestring);
                }
            }

            const cJSON *build = cJSON_GetObjectItemCaseSensitive(json, "build");
            const cJSON *major = cJSON_GetObjectItemCaseSensitive(build, "major");
            const cJSON *minor = cJSON_GetObjectItemCaseSensitive(build, "minor");
            const cJSON *patch = cJSON_GetObjectItemCaseSensitive(build, "patch");

            if (cJSON_IsNumber(major))
            {
                m_provider.version.major = major->valueint;
            }
            if (cJSON_IsNumber(minor))
            {
                m_provider.version.minor = minor->valueint;
            }
            if (cJSON_IsNumber(patch))
            {
                m_provider.version.patch = patch->valueint;
            }
        }

        cJSON_Delete(json);
    }
    else if (m_provider.version.major != 0 && m_provider.version.minor != 0
        && m_provider.version.patch != 0)
    {
        err_code = VTC_ERROR_OFFLINE;
    }

    // local copy of version has been updated (or not)
    // copy version into caller structure
    strcpy(version->network, m_provider.version.network);
    strcpy(version->genesis_hash, m_provider.version.genesis_hash);

    version->major = m_provider.version.major;
    version->minor = m_provider.version.minor;
    version->patch = m_provider.version.patch;

    return err_code;
}

err_code_t
provider_ping()
{
    err_code_t
        err_code = http_get(&m_provider.providers[0], "/health", base_headers, &m_provider.response_buffer);
    return err_code;
}

err_code_t
provider_init(provider_info_t *providers, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        m_provider.providers[i].url = providers[i].url;
        m_provider.providers[i].port = providers[i].port;
        m_provider.providers[i].token = providers[i].token;
    }

    m_provider.response_payload_cb = response_payload_callback;

    size_t len = sprintf(base_headers, "x-api-key:%s", m_provider.providers[0].token);
    VTC_ASSERT_BOOL(len < 128);

    err_code_t err_code = http_init(&m_provider.providers[0], response_payload_callback);
    VTC_ASSERT(err_code);

    return err_code;
}