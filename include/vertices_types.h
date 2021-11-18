/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
#define VERTICES_SDK_LIB_INC_VERTICES_TYPES_H

#include "vertices_config.h"
#include "vertices_errors.h"
#include "stddef.h"

/// if \c TX_PAYLOAD_MAX_LENGTH is not enough, you can add up more space for encoded TX
/// define \c OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES as compiler flag
#ifndef OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES
#define OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES 0
#endif

// addresses
#define ADDRESS_LENGTH                  32
#define PUBLIC_B32_STR_MAX_LENGTH       65

// transaction
#define SIGNATURE_LENGTH                64
#define TRANSACTION_HASH_STR_MAX_LENGTH 53

// blocks
#define BLOCK_HASH_LENGTH               32

// HTTP payload
#define TX_PAYLOAD_MAX_LENGTH           (512+OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES) ///< Encoded TX maximum length in bytes


/// Asynchronous operations can be handled using Vertices events types
typedef enum
{
    VTC_EVT_TX_READY_TO_SIGN =
    0, ///< transaction's payload must be signed: the user must provide the signing function
    VTC_EVT_TX_SENDING, ///< transaction is being sent to the blockchain API. When the user got the event, the transaction has probably been already sent.
    VTC_EVT_TX_SUCCESS, ///< transaction has been successfully sent and executed, after that event, the buffer is freed.
} vtc_evt_type_t;

/// Events contains a type and a bufid. When implementing the event handler, the `bufid`
/// should be used to retrieve the data to be processed. Data type depends on event type.
typedef struct
{
    vtc_evt_type_t type; ///< \see vtc_evt_type_t
    size_t bufid; ///< internal buffer ID, used to identify a pending transaction
} vtc_evt_t;

typedef struct
{
    char *url;
    short port;
    char *header;
    const char *cert_pem;
} provider_info_t;

typedef struct
{
    char public_b32[PUBLIC_B32_STR_MAX_LENGTH]; //!< b32 public address, with `\0` termination character
    unsigned char public_key[ADDRESS_LENGTH];   //!< 32-bytes public key
    int32_t amount;                             //!< amount of tokens on account
} account_info_t;

typedef enum
{
    VALUE_TYPE_NONE = 0,
    VALUE_TYPE_BYTESLICE,
    VALUE_TYPE_INTEGER,
} value_type_t;

typedef struct
{
    char name[APPS_KV_NAME_MAX_LENGTH]; //!< variable name, ASCII-encoded, can contains up to APPS_KV_NAME_MAX_LENGTH bytes
    value_type_t type; //!< type [tt]
    union
    {
        uint64_t value_uint;
        uint8_t value_slice[APPS_KV_SLICE_MAX_SIZE];
    };
} app_key_value_t;

typedef struct
{
    uint32_t count;
    app_key_value_t values[APPS_KV_MAX_COUNT];
} app_values_t;

typedef struct
{
    unsigned char payload[TX_PAYLOAD_MAX_LENGTH]; ///< Full payload, comprised of the header + body. The body part is the signed part (prepended with "TX")
    size_t
        payload_header_length;  ///< Header size. Do not use for signing. Full \c payload size is: \c payload_header_length + \c payload_body_length
    size_t
        payload_body_length;  ///< Body size. Length of data to be signed. if not 0, indicates that TX is pending.
    unsigned char signature[SIGNATURE_LENGTH];
    unsigned char id[TRANSACTION_HASH_STR_MAX_LENGTH
    ]; ///< Unique Identifier of the transaction. It can be used to find the transaction in the ledger
} signed_transaction_t;

typedef struct
{
    provider_info_t *provider;
    ret_code_t
    (*vertices_evt_handler)(vtc_evt_t *evt);
} vertex_t;

typedef struct
{
    uint32_t update_count;
    char network[64];
    char genesis_hash[BLOCK_HASH_LENGTH];
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
} provider_version_t;

#endif //VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
