//
// Created by Cyril on 04/06/2021.
//

#include <mpack-reader.h>
#include <vertices_log.h>
#include "parser.h"

//#define DEBUG_PARSER // to enable maximum verbosity and make the parser fail on error
#ifndef DEBUG_PARSER
#undef LOG_DEBUG
#define LOG_DEBUG(...)
#endif

// # Account parsing
// ## Structure of MessagePack-encoded data (printed below as JSON)
// {
//  "algo": 17999000,
//  "appl": {
//    "15975751": {
//      "hsch": {
//        "nui": 1
//      },
//      "tkv": {
//        "LastValue": {
//          "tt": 2,
//          "ui": 25
//        }
//      }
//    }
//  },
//  "ebase": 27521,
//  "tsch": {
//    "nui": 1
//  }
//}

typedef enum
{
    FIELD_ROOT = 0, //!< starting point, root of map
    FIELD_ALGO,
    FIELD_APPL,
    FIELD_APPL_ID,
    FIELD_APP_SCH, //!< app schema
    FIELD_APP_SCH_NUI, //!< integer count
    FIELD_APP_SCH_NBS, //!< slice count
    FIELD_APP_KV,
    FIELD_APP_KV_VAR,
    FIELD_APP_KV_TYPE,
    FIELD_APP_KV_VALUE_INT,
    FIELD_APP_KV_VALUE_SLICE,
    FIELD_EBASE,
    FIELD_ERN, //!< rewards
    FIELD_TSCH, //!< total schema
    FIELD_TSCH_NUI, //!< total schema
} parser_account_step_t;

parser_account_step_t m_parsing_account_step = FIELD_ROOT;

typedef struct
{
    ret_code_t
    (*nil_element)(void *context);
    ret_code_t
    (*bool_element)(void *context, int64_t value);
    ret_code_t
    (*int_element)(void *context, int64_t value);
    ret_code_t
    (*uint_element)(void *context, uint64_t value);
    ret_code_t
    (*string_element)(void *context, const char *data, uint32_t length);

    ret_code_t
    (*start_map)(void *context, uint32_t pair_count);
    ret_code_t
    (*start_array)(void *context, uint32_t element_count);
    ret_code_t
    (*finish_map)(void *context);
    ret_code_t
    (*finish_array)(void *context);
} callbacks_t;

static ret_code_t
nil_element(void *context)
{
    LOG_DEBUG("nil");
    return VTC_SUCCESS;
}

static ret_code_t
bool_element(void *context, int64_t value)
{
    LOG_DEBUG("<bool> %s", value ? "true" : "false");
    return VTC_SUCCESS;
}

static ret_code_t
int_element(void *context, int64_t value)
{
    LOG_DEBUG("<int> %llu", value);
    return VTC_SUCCESS;
}

/// Parse unsigned integer
/// \param context Pointer to parsed object (account)
/// \param value Integer value
/// \return
/// * VTC_ERROR_NO_MEM not enough memory allocated (increase APPS_MAX_COUNT?)
static ret_code_t
uint_element(void *context, uint64_t value)
{
    LOG_DEBUG("<uint> %llu", value);

    ret_code_t err_code = VTC_SUCCESS;

    if (m_parsing_account_step != FIELD_ROOT)
    {
        account_details_t *account = (account_details_t *) context;

        switch (m_parsing_account_step)
        {
            case FIELD_ALGO:
            {
                account->info.amount = (int32_t) value;
                m_parsing_account_step = FIELD_ROOT; // field if fully parsed, reset step
            }
                break;

            case FIELD_APPL:
            {
                if (account->app_idx < APPS_MAX_COUNT)
                {
                    account->apps_local[account->app_idx].app_id = value;
                    m_parsing_account_step = FIELD_APPL_ID;
                }
                else
                {
                    err_code = VTC_ERROR_NO_MEM;
                }

                // field parsing is not over, don't reset step
            }
                break;
            case FIELD_APP_SCH:break;
            case FIELD_APP_KV:break;
            case FIELD_EBASE:
            {
                account->reward_base = (int32_t) value;
                m_parsing_account_step = FIELD_ROOT; // field if fully parsed, reset step
            }
                break;
            case FIELD_ERN:
            {
                account->rewards = (int32_t) value;
                m_parsing_account_step = FIELD_ROOT; // field if fully parsed, reset step
            }
                break;

            case FIELD_APP_KV_TYPE:
            {
                uint32_t idx = account->apps_local[account->app_idx].key_values.count;
                if (idx > APPS_KV_MAX_COUNT)
                {
                    return VTC_ERROR_NO_MEM;
                }

                account->apps_local[account->app_idx].key_values.values[idx].type = (uint8_t) value;
                m_parsing_account_step = FIELD_APP_KV_VAR;
            }
                break;

            case FIELD_APP_KV_VALUE_INT:
            {
                uint32_t idx = account->apps_local[account->app_idx].key_values.count;
                if (idx > APPS_KV_MAX_COUNT)
                {
                    return VTC_ERROR_NO_MEM;
                }

                account->apps_local[account->app_idx].key_values.values[idx].value_uint = value;
                m_parsing_account_step = FIELD_APP_KV_VAR;
            }
                break;

            case FIELD_APP_SCH_NUI:
            case FIELD_APP_SCH_NBS:
            {
                m_parsing_account_step = FIELD_APP_SCH; // field if fully parsed, reset step
            }
                break;

            case FIELD_TSCH_NUI:
                // currently ignored
                LOG_DEBUG("Ignored step: %u", m_parsing_account_step);
                break;
            default:
            {
                LOG_ERROR("Field %u not parsed as uint", m_parsing_account_step);
            }
                break;
        }
    }

    return err_code;
}

/// Parse string
/// \param context Pointer to parsed object (account)
/// \param data Pointer to string
/// \param length String length
/// \return
/// * \c VTC_ERROR_NOT_FOUND unknown key
static ret_code_t
string_element(void *context, const char *data, uint32_t length)
{
    LOG_DEBUG("<string> %.*s", length, data);

    ret_code_t err_code = VTC_SUCCESS;

    // check new keys in root map
    if (m_parsing_account_step == FIELD_ROOT)
    {
        if (strncmp(data, "algo", length) == 0)
        {
            m_parsing_account_step = FIELD_ALGO;
        }
        else if (strncmp(data, "appl", length) == 0)
        {
            m_parsing_account_step = FIELD_APPL;
        }
        else if (strncmp(data, "ebase", length) == 0)
        {
            m_parsing_account_step = FIELD_EBASE;
        }
        else if (strncmp(data, "ern", length) == 0)
        {
            m_parsing_account_step = FIELD_ERN;
        }
        else if (strncmp(data, "tsch", length) == 0)
        {
            m_parsing_account_step = FIELD_TSCH;
        }
        else
        {
            // unknown key
            err_code = VTC_ERROR_NOT_FOUND;
        }

        return err_code;
    }

    // check that we are parsing application fields
    if (m_parsing_account_step == FIELD_APPL_ID)
    {
        if (strncmp(data, "hsch", length) == 0)
        {
            m_parsing_account_step = FIELD_APP_SCH;
        }
        else if (strncmp(data, "tkv", length) == 0)
        {
            m_parsing_account_step = FIELD_APP_KV;
        }
        else
        {
            LOG_ERROR("Cannot parse FIELD_APPL_ID: %.*s", length, data);
            err_code = VTC_ERROR_NOT_FOUND;
        }
    }
    else if (m_parsing_account_step == FIELD_APP_SCH)
    {
        if (strncmp(data, "nui", length) == 0)
        {
            m_parsing_account_step = FIELD_APP_SCH_NUI;
        }
        else if (strncmp(data, "nbs", length) == 0)
        {
            m_parsing_account_step = FIELD_APP_SCH_NBS;
        }
        else
        {
            LOG_ERROR("Cannot parse FIELD_APP_SCH: %.*s", length, data);
            err_code = VTC_ERROR_NOT_FOUND;
        }
    }
    else if (m_parsing_account_step == FIELD_APP_KV)
    {
        account_details_t *account = (account_details_t *) context;
        uint32_t kv_idx = account->apps_local[account->app_idx].key_values.count;

        if (kv_idx >= APPS_KV_MAX_COUNT)
        {
            LOG_ERROR("Cannot store more values");
            err_code = VTC_ERROR_NO_MEM;
        }

        memset(account->apps_local[account->app_idx].key_values.values[kv_idx].name, 0, APPS_KV_NAME_MAX_LENGTH);
        memcpy(account->apps_local[account->app_idx].key_values.values[kv_idx].name,
               data,
               length < (APPS_KV_NAME_MAX_LENGTH - 1) ? length : (APPS_KV_NAME_MAX_LENGTH - 1));

        m_parsing_account_step = FIELD_APP_KV_VAR;
    }
    else if (m_parsing_account_step == FIELD_APP_KV_VAR)
    {
        if (strncmp(data, "tt", length) == 0)
        {
            m_parsing_account_step = FIELD_APP_KV_TYPE;
        }
        else if (strncmp(data, "tb", length) == 0)
        {
            m_parsing_account_step = FIELD_APP_KV_VALUE_SLICE;
        }
        else if (strncmp(data, "ui", length) == 0)
        {
            m_parsing_account_step = FIELD_APP_KV_VALUE_INT;
        }
        else
        {
            LOG_ERROR("Cannot parse FIELD_APP_KV_VAR: %.*s", length, data);
            err_code = VTC_ERROR_NOT_FOUND;
        }
    }
    else if (m_parsing_account_step == FIELD_TSCH)
    {
        if (strncmp(data, "nui", length) == 0)
        {
            m_parsing_account_step = FIELD_TSCH_NUI;
        }
    }

    return err_code;
}

static ret_code_t
start_map(void *context, uint32_t pair_count)
{
    LOG_DEBUG("<map> %u elements", pair_count);
    return VTC_SUCCESS;
}

static ret_code_t
start_array(void *context, uint32_t element_count)
{
    LOG_DEBUG("<array> %u elements", element_count);
    return VTC_SUCCESS;
}

static ret_code_t
finish_map(void *context)
{
    LOG_DEBUG("<map> done %u", m_parsing_account_step);

    if (m_parsing_account_step == FIELD_APP_KV_VAR)
    {
        // we just parsed a new application variable
        // increase key-value index

        account_details_t *account = (account_details_t *) context;
        account->apps_local[account->app_idx].key_values.count++;

        m_parsing_account_step = FIELD_APP_KV;
    }
    else if (m_parsing_account_step == FIELD_APP_KV || m_parsing_account_step == FIELD_APP_SCH)
    {
        // we are done parsing schema or key-value

        m_parsing_account_step = FIELD_APPL_ID;
    }
    else if (m_parsing_account_step == FIELD_APPL_ID)
    {
        // we finished parsing application
        // increase application index

        account_details_t *account = (account_details_t *) context;
        account->app_idx++;

        m_parsing_account_step = FIELD_APPL;
    }
    else
    {
        // back to root map

        m_parsing_account_step = FIELD_ROOT;
    }

    return VTC_SUCCESS;
}

static ret_code_t
finish_array(void *context)
{
    LOG_DEBUG("<array> done");
    return VTC_SUCCESS;
}

static callbacks_t m_callbacks = {
    nil_element,
    bool_element,
    int_element,
    uint_element,
    string_element,
    start_map,
    start_array,
    finish_map,
    finish_array,
};

static ret_code_t
parse_element(mpack_reader_t *reader, int depth,
              const callbacks_t *callbacks, void *context)
{
    ret_code_t err_code = VTC_SUCCESS;

    if (depth >= 32) // critical check!
    {
        mpack_reader_flag_error(reader, mpack_error_too_big);

        VTC_ASSERT(VTC_ERROR_INVALID_PARAM);
    }

    mpack_tag_t tag = mpack_read_tag(reader);
    if (mpack_reader_error(reader) != mpack_ok)
    {
        return VTC_SUCCESS;
    }

    switch (mpack_tag_type(&tag))
    {
        case mpack_type_nil:
        {
            err_code = callbacks->nil_element(context);
        }
            break;
        case mpack_type_bool:
        {
            err_code = callbacks->bool_element(context, mpack_tag_bool_value(&tag));
        }
            break;
        case mpack_type_int:
        {
            err_code = callbacks->int_element(context, mpack_tag_int_value(&tag));
        }
            break;
        case mpack_type_uint:
        {
            err_code = callbacks->uint_element(context, mpack_tag_uint_value(&tag));
        }
            break;

        case mpack_type_str:
        {
            uint32_t length = mpack_tag_str_length(&tag);
            const char *data = mpack_read_bytes_inplace(reader, length);
            err_code = callbacks->string_element(context, data, length);
            mpack_done_str(reader);
        }
            break;

        case mpack_type_array:
        {
            uint32_t count = mpack_tag_array_count(&tag);
            err_code = callbacks->start_array(context, count);
            if (err_code != VTC_SUCCESS)
            {
                break;
            }

            while (count-- > 0)
            {
                err_code = parse_element(reader, depth + 1, callbacks, context);
                if (mpack_reader_error(reader) != mpack_ok ||
                    err_code != VTC_SUCCESS)
                {
                    break;
                }
            }
            err_code = callbacks->finish_array(context);
            mpack_done_array(reader);
        }
            break;

        case mpack_type_map:
        {
            uint32_t count = mpack_tag_map_count(&tag);
            err_code = callbacks->start_map(context, count);
            if (err_code != VTC_SUCCESS)
            {
                break;
            }
            while (count--)
            {
                err_code = parse_element(reader, depth + 1, callbacks, context);
                if (err_code != VTC_SUCCESS)
                {
                    break;
                }

                err_code = parse_element(reader, depth + 1, callbacks, context);
                if (mpack_reader_error(reader) != mpack_ok ||
                    err_code != VTC_SUCCESS) // critical check!
                {
                    break;
                }
            }
            err_code = callbacks->finish_map(context);
            mpack_done_map(reader);
        }
            break;

        default:mpack_reader_flag_error(reader, mpack_error_unsupported);
            break;
    }

    return err_code;
}

ret_code_t
parser_account(const char *buf, size_t length, account_details_t *account)
{
    // we are reading messagepack data
    mpack_reader_t reader;
    mpack_reader_init_data(&reader, buf, length);

    m_parsing_account_step = FIELD_ROOT;
    ret_code_t err_code = parse_element(&reader, 0, &m_callbacks, account);

#ifdef DEBUG_PARSER
    // crash if we are trying to debug the parser
    VTC_ASSERT(err_code);
#endif

    return err_code;
}
