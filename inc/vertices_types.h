//
// Created by Cyril on 19/03/2021.
//

#ifndef VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
#define VERTICES_SDK_LIB_INC_VERTICES_TYPES_H

#include "vertices_errors.h"

#define xstr(s) str(s.h)
#define str(s) #s

#define ADDRESS_LENGTH          32
#define HASH_LENGTH             32
#define TX_PAYLOAD_MAX_LENGTH   512
#define SIGNATURE_LENGTH        64

/// Asynchronous operations can be handled using Vertices events types
typedef enum
{
    VTC_EVT_TX_READY_TO_SIGN = 0,
    VTC_EVT_TX_READY_TO_SEND,
} vtc_evt_type_t;

/// Events contains a type and a bufid. When implementing the event handler, the `bufid`
/// should be used to retrieve the data to be processed. Data type depends on event type.
typedef struct
{
    vtc_evt_type_t type;
    size_t bufid;
} vtc_evt_t;

typedef struct
{
    char *url;
    short port;
    char *header;
} provider_info_t;

typedef struct
{
    char *public; // b64 public address
    unsigned char private_key[ADDRESS_LENGTH]; // 32-bytes private key
    unsigned char public_key[ADDRESS_LENGTH]; // 32-bytes public key
    int32_t amount; // micro-Algos on account
} account_info_t;

typedef struct
{
    unsigned char payload[TX_PAYLOAD_MAX_LENGTH];
    size_t payload_length;  // length of data to be signed. if not 0, indicates that tx is pending.
    size_t payload_offset;  // start of the payload to be signed
    // full payload size is: payload_length + payload_offset
    unsigned char signature[SIGNATURE_LENGTH];
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
