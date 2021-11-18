/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_INC_TRANSACTION_H
#define VERTICES_INC_TRANSACTION_H

#include <vertices_errors.h>
#include <vertices_types.h>

#define header_xstr(s) str(s.h)
#define str(s) #s

#ifdef BLOCKCHAIN_PROVIDER

// include Blockchain-specific types
#include header_xstr(BLOCKCHAIN_PROVIDER)

#else

// Generic implementation

typedef struct
{
    uint8_t receiver[ADDRESS_LENGTH]; ///< The address of the account that receives the amount.
    uint64_t amount; ///< The total amount to be sent
} payment_tx_t;

typedef struct
{
    union
    {
        payment_tx_t payment_tx;
    };
} transaction_details_t;

#endif

typedef struct
{
    uint8_t sender_pub[ADDRESS_LENGTH];
    uint64_t fee;
    uint8_t genesis_hash[BLOCK_HASH_LENGTH];
    transaction_details_t *details;
} transaction_t;

ret_code_t
transaction_pay(account_info_t *sender, char *receiver, uint64_t amount, void * params);

ret_code_t
transaction_appl(account_info_t *sender, uint64_t app_id, void *params);

ret_code_t
transaction_get(size_t bufid, signed_transaction_t **tx);

ret_code_t
transaction_pending_send(size_t bufid);

ret_code_t
transaction_free(size_t bufid);

#endif //VERTICES_INC_TRANSACTION_H
