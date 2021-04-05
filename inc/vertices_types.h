//
// Created by Cyril on 19/03/2021.
//

#ifndef VERTICES_SDK_LIB_INC_VERTICES_TYPES_H
#define VERTICES_SDK_LIB_INC_VERTICES_TYPES_H

#include "vertices_errors.h"

#define ADDRESS_LENGTH          32
#define HASH_LENGTH             32
#define TX_PAYLOAD_MAX_LENGTH   512
#define SIGNATURE_LENGTH        64

#if ALGORAND_PROVIDER

#define TX_DEFAULT_FEE 1000

typedef enum
{
    ALGORAND_PAYMENT_TRANSACTION = 0,
    ALGORAND_KEY_REGISTRATION_TRANSACTION,
    ALGORAND_ASSET_CONFIGURATION_TRANSACTION,
    ALGORAND_ASSET_TRANSFER_TRANSACTION,
    ALGORAND_ASSET_FREEZE_TRANSACTION,
    ALGORAND_APPLICATION_CALL_TRANSATION,
} tx_type_t;

typedef struct
{
    uint8_t receiver[ADDRESS_LENGTH]; // The address of the account that receives the amount.
    uint64_t amount; // The total amount to be sent in microAlgos.
    uint8_t close_remainder_to[ADDRESS_LENGTH
    ]; // When set, it indicates that the transaction is requesting that the Sender account should be closed, and all remaining funds, after the fee and amount are paid, be transferred to this address.
} payment_tx_t;

typedef struct
{
    uint64_t
        first_valid; // The first round for when the transaction is valid. If the transaction is sent prior to this round it will be rejected by the network.
    uint64_t
        last_valid;  // The ending round for which the transaction is valid. After this round, the transaction will be rejected by the network.
    tx_type_t tx_type;    // Specifies the type of transaction.
    union
    {
        payment_tx_t payment_tx;
    };
} transaction_details_t;

#else
#error You need to define a provider between: [ALGORAND_PROVIDER]
#endif

#ifndef TX_DEFAULT_FEE
#define TX_DEFAULT_FEE 1000
#endif

#if TX_DEFAULT_FEE == 0
#warning "You might want to increase the fees for the transactions to pass"
#endif

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
    size_t payload_offset;  // start of the payload to be signed, after `txn`
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
