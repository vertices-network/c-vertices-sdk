//
// Created by Cyril on 18/03/2021.
//

#include <vertices_log.h>
#include "vertices_errors.h"
#include <http.h>
#include <mpack-reader.h>
#include <transaction.h>
#include <base64.h>
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

    LOG_DEBUG("Received %zu bytes", received_data_size);

    VTC_ASSERT_BOOL(received_data_size < HTTP_MAXIMUM_CONTENT_LENGTH);

    if (response_payload != NULL)
    {
        payload_t *payload = (payload_t *) response_payload;

        // append in buffer
        memcpy(&rx_buf[payload->size], received_data, received_data_size);

        payload->data = rx_buf;
        payload->size += received_data_size;

        rx_buf[payload->size] = 0;
    }
    else
    {
        memcpy(rx_buf, received_data, received_data_size);
        rx_buf[received_data_size] = 0;
    }

    LOG_DEBUG("%s", rx_buf);

    return received_data_size;
}

static ret_code_t
parse_account_field(mpack_reader_t *reader, const char *field, account_details_t *account)
{
    ret_code_t err_code = VTC_SUCCESS;

    mpack_tag_t value = mpack_read_tag(reader);

    if (strncmp(field, "algo", sizeof "algo" - 1) == 0)
    {
        account->info->amount = (int32_t) mpack_tag_uint_value(&value);
    }
    else if (strncmp(field, "ebase", sizeof "ebase" - 1) == 0)
    {
        account->reward_base = (int32_t) mpack_tag_uint_value(&value);
    }
    else if (strncmp(field, "ern", sizeof "ern" - 1) == 0)
    {
        account->rewards = (int32_t) mpack_tag_uint_value(&value);
    }
    else
    {
        err_code = VTC_ERROR_NOT_FOUND;
    }

    return err_code;
}

/// parse MessagePack data for `/accounts` response
static ret_code_t
parse_accounts_msgpack(account_details_t *account, char *buf, size_t len)
{
    // we are reading messagepack data
    mpack_reader_t reader;
    mpack_reader_init_data(&reader, buf, len);

    mpack_tag_t tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok || tag.type == mpack_type_nil)
    {
        return VTC_ERROR_INTERNAL;
    }

    // response to /account is a map with different optional fields
    // see https://developer.algorand.org/docs/reference/rest-apis/algod/v2/#account
    // `algo`, `appl`, `tsch`, `ebase` ... etc

    // Parse the map containing `count` elements
    if (mpack_tag_type(&tag) == mpack_type_map)
    {
        uint32_t count = mpack_tag_map_count(&tag);

        for (uint32_t i = 0; i < count; ++i)
        {
            mpack_tag_t mapped_tag = mpack_read_tag(&reader);

            // elements are `string` followed by `value`
            if (mpack_tag_type(&mapped_tag) == mpack_type_str)
            {
                uint32_t length = mpack_tag_str_length(&mapped_tag);
                // data is the pointer to the value
                const char *data = mpack_read_bytes_inplace(&reader, length);

                ret_code_t err_code = parse_account_field(&reader, data, account);
                VTC_ASSERT(err_code);
            }
        }

        mpack_done_map(&reader);
    }

    VTC_ASSERT_BOOL(mpack_reader_destroy(&reader) == mpack_ok);

    return VTC_SUCCESS;
}

ret_code_t
provider_account_info_get(account_details_t *account)
{
    char relative_path[128] = {0};

    int ret = sprintf(relative_path, "/v2/accounts/%s?format=msgpack", account->info->public_b32);
    VTC_ASSERT_BOOL(ret < 128 && ret >= 0);

    uint32_t response_code = 0;
    ret_code_t err_code = http_get(&m_provider.provider,
                                   relative_path,
                                   m_provider.provider.header,
                                   &m_provider.response_buffer, &response_code);
    if (err_code == VTC_SUCCESS)
    {
        parse_accounts_msgpack(account, rx_buf, m_provider.response_buffer.size);
    }

    return err_code;
}

ret_code_t
provider_tx_params_load(transaction_t *tx)
{
    uint32_t response_code = 0;
    ret_code_t
        err_code =
        http_get(&m_provider.provider,
                 (char *) "/v2/transactions/params",
                 m_provider.provider.header,
                 &m_provider.response_buffer, &response_code);

    if (err_code == VTC_SUCCESS)
    {
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
            // if we already have the genesis hash, we don't need to parse it from JSON and decode it
            if (m_provider.version.update_count == 0)
            {
                const cJSON
                    *genesis_hash = cJSON_GetObjectItemCaseSensitive(json, "genesis_hash_b64");
                if (cJSON_IsString(genesis_hash) && (genesis_hash->valuestring != NULL))
                {
                    size_t output_size = sizeof tx->genesis_hash;
                    err_code = b64_decode(genesis_hash->valuestring,
                                          strlen(genesis_hash->valuestring),
                                          (char *) tx->genesis_hash,
                                          &output_size);
                }
            }
            else
            {
                memcpy(tx->genesis_hash, m_provider.version.genesis_hash, sizeof(tx->genesis_hash));
            }

            const cJSON *fee = cJSON_GetObjectItemCaseSensitive(json, "min-fee");
            if (cJSON_IsNumber(fee))
            {
                tx->fee = (uint64_t) fee->valueint;
            }

            const cJSON *last_round = cJSON_GetObjectItemCaseSensitive(json, "last-round");
            if (cJSON_IsNumber(last_round))
            {
                tx->details->first_valid = (uint64_t) last_round->valueint + 1;
                tx->details->last_valid = tx->details->first_valid + 1000;
            }

            cJSON_Delete(json);
        }
    }

    return err_code;
}

ret_code_t
provider_tx_post(const uint8_t *bin_payload, size_t length, unsigned char *tx_id)
{
    ret_code_t err_code;

    // add provider-specific header if a value has been set
    char header[256] = {0};
    if (m_provider.provider.header == NULL || strlen(m_provider.provider.header) == 0)
    {
        int ret =
            sprintf(header, "Content-Type: application/x-binary");
        VTC_ASSERT_BOOL(ret < 128 && ret >= 0);
    }
    else
    {
        int ret =
            sprintf(header, "%s\r\nContent-Type: application/x-binary", m_provider.provider.header);
        VTC_ASSERT_BOOL(ret < 128 && ret >= 0);
    }

    uint32_t response_code = 0;
    err_code =
        http_post(&m_provider.provider,
                  (char *) "/v2/transactions",
                  header,
                  (const char *) bin_payload, length, &m_provider.response_buffer, &response_code);

    if (err_code == VTC_SUCCESS)
    {
        cJSON *json = cJSON_Parse(rx_buf);
        if (json == NULL)
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                LOG_ERROR("JSON, error before: %s", error_ptr);
            }
            return VTC_ERROR_INTERNAL;
        }
        else
        {
            const cJSON *tx_id_json = cJSON_GetObjectItemCaseSensitive(json, "txId");
            if (cJSON_IsString(tx_id_json) && (tx_id_json->valuestring != NULL))
            {
                if (strlen(tx_id_json->valuestring) >= TRANSACTION_HASH_STR_MAX_LENGTH)
                {
                    return VTC_ERROR_NO_MEM;
                }
                else
                {
                    strcpy((char *) tx_id, tx_id_json->valuestring);
                }
            }
        }
    }

    return err_code;
}

ret_code_t
provider_version_get(provider_version_t *version)
{
    uint32_t response_code = 0;
    ret_code_t
        err_code =
        http_get(&m_provider.provider,
                 (char *) "/versions",
                 m_provider.provider.header,
                 &m_provider.response_buffer, &response_code);

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
            return VTC_ERROR_INTERNAL;
        }
        else
        {
            const cJSON *genesis_id = cJSON_GetObjectItemCaseSensitive(json, "genesis_id");
            if (cJSON_IsString(genesis_id) && (genesis_id->valuestring != NULL))
            {
                if (strlen(genesis_id->valuestring) >= sizeof m_provider.version.network)
                {
                    return VTC_ERROR_NO_MEM;
                }
                else
                {
                    strcpy(m_provider.version.network, genesis_id->valuestring);
                }
            }

            const cJSON *genesis_hash = cJSON_GetObjectItemCaseSensitive(json, "genesis_hash_b64");
            if (cJSON_IsString(genesis_hash) && (genesis_hash->valuestring != NULL))
            {
                size_t output_size = sizeof m_provider.version.genesis_hash;
                ret_code_t ret = b64_decode(genesis_hash->valuestring,
                                            strlen(genesis_hash->valuestring),
                                            m_provider.version.genesis_hash,
                                            &output_size);
                VTC_ASSERT_BOOL(output_size <= sizeof(m_provider.version.genesis_hash));
                if (ret != VTC_SUCCESS)
                {
                    return ret;
                }
            }

            const cJSON *build = cJSON_GetObjectItemCaseSensitive(json, "build");
            const cJSON *major = cJSON_GetObjectItemCaseSensitive(build, "major");
            const cJSON *minor = cJSON_GetObjectItemCaseSensitive(build, "minor");
            const cJSON *patch = cJSON_GetObjectItemCaseSensitive(build, "patch");

            if (cJSON_IsNumber(major))
            {
                m_provider.version.major = (unsigned int) major->valueint;
            }
            if (cJSON_IsNumber(minor))
            {
                m_provider.version.minor = (unsigned int) minor->valueint;
            }
            if (cJSON_IsNumber(patch))
            {
                m_provider.version.patch = (unsigned int) patch->valueint;
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
    memcpy(version->genesis_hash, m_provider.version.genesis_hash, sizeof(version->genesis_hash));

    version->major = m_provider.version.major;
    version->minor = m_provider.version.minor;
    version->patch = m_provider.version.patch;

    m_provider.version.update_count += 1;

    return err_code;
}

ret_code_t
provider_ping()
{
    uint32_t response_code = 0;
    ret_code_t
        err_code =
        http_get(&m_provider.provider,
                 (char *) "/health",
                 m_provider.provider.header,
                 &m_provider.response_buffer, &response_code);
    return err_code;
}

ret_code_t
provider_init(provider_info_t *provider)
{
    memset(&m_provider.version, 0, sizeof m_provider.version);

    m_provider.provider.url = provider->url;
    m_provider.provider.port = provider->port;
    m_provider.provider.header = provider->header;
    m_provider.provider.cert_pem = provider->cert_pem;

    m_provider.response_payload_cb = response_payload_callback;

    ret_code_t err_code = http_init(&m_provider.provider, response_payload_callback);
    return err_code;
}
