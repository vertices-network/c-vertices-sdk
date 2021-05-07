//
// Created by Cyril on 19/03/2021.
//

#ifndef VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
#define VERTICES_SDK_LIB_INC_VERTICES_TYPES_H

#include "vertices_config.h"
#include "vertices_errors.h"

#ifndef OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES
#define OPTIONAL_TX_FIELDS_MAX_SIZE 0
#endif

#define ADDRESS_LENGTH                  32
#define HASH_LENGTH                     32
#define TX_PAYLOAD_MAX_LENGTH           (512+OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES)
#define SIGNATURE_LENGTH                64
#define PUBLIC_B32_STR_MAX_LENGTH       65
#define TRANSACTION_HASH_STR_MAX_LENGTH 53

/// Asynchronous operations can be handled using Vertices events types
typedef enum
{
    VTC_EVT_TX_READY_TO_SIGN =
    0, ///< transaction's payload must be signed: the user must provide the signing function and emit the \c VTC_EVT_TX_READY_TO_SEND event.
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
    char public_b32[PUBLIC_B32_STR_MAX_LENGTH]; // b64 public address, with \0
    unsigned char private_key[ADDRESS_LENGTH]; // 32-bytes private key
    unsigned char public_key[ADDRESS_LENGTH]; // 32-bytes public key
    int32_t amount; // tokens on account
} account_info_t;

typedef struct
{
    unsigned char payload[TX_PAYLOAD_MAX_LENGTH];
    size_t
        payload_length;  ///< length of data to be signed. if not 0, indicates that TX is pending.
    size_t
        payload_offset;  ///< start of the payload to be signed. Full payload size is: \c payload_length + \c payload_offset
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
    char genesis_hash[HASH_LENGTH];
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
} provider_version_t;

#endif //VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
