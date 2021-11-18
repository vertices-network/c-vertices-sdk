/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_SDK_LIB_INC_TRANSACTION_ALGORAND_H
#define VERTICES_SDK_LIB_INC_TRANSACTION_ALGORAND_H

#define TX_DEFAULT_FEE 1000

typedef enum
{
    ALGORAND_PAYMENT_TRANSACTION = 0,
    ALGORAND_KEY_REGISTRATION_TRANSACTION,
    ALGORAND_ASSET_CONFIGURATION_TRANSACTION,
    ALGORAND_ASSET_TRANSFER_TRANSACTION,
    ALGORAND_ASSET_FREEZE_TRANSACTION,
    ALGORAND_APPLICATION_CALL_TRANSACTION,
} tx_type_t;

typedef enum
{
    ALGORAND_ON_COMPLETE_NOOP = 0,
    ALGORAND_ON_COMPLETE_OPT_IN,
    ALGORAND_ON_COMPLETE_CLOSE_OUT,
    ALGORAND_ON_COMPLETE_CLEAR_STATE,
    ALGORAND_ON_COMPLETE_UPDATE_APP,
    ALGORAND_ON_COMPLETE_DELETE_APP
} tx_on_complete_t;

typedef struct
{
    uint8_t receiver[ADDRESS_LENGTH]; ///< The address of the account that receives the amount.
    uint64_t amount; ///< The total amount to be sent in microAlgos.
    uint8_t close_remainder_to[ADDRESS_LENGTH
    ]; ///< When set, it indicates that the transaction is requesting that the Sender account should be closed, and all remaining funds, after the fee and amount are paid, be transferred to this address.
} payment_tx_t;

typedef struct
{
    uint64_t app_id; ///< "apid" ID of the application being configured or empty if creating.
    tx_on_complete_t on_complete; ///< "apan" Defines what additional actions occur with the transaction. See the OnComplete section of the TEAL spec for details (https://developer.algorand.org/docs/reference/teal/specification/#oncomplete).
    // uint8_t receiver[ADDRESS_LENGTH]; ///< "apap" Logic executed for every application transaction, except when on-completion is set to "clear". It can read and write global state for the application, as well as account-specific local state. Approval programs may reject the transaction.
    app_values_t *key_values; ///< "apaa" Transaction specific arguments accessed from the application's approval-program and clear-state-program.
} appl_tx_t;

typedef struct
{
    uint64_t
        first_valid; ///< The first round for when the transaction is valid. If the transaction is sent prior to this round it will be rejected by the network.
    uint64_t
        last_valid; ///< The ending round for which the transaction is valid. After this round, the transaction will be rejected by the network.
    tx_type_t tx_type; ///< Specifies the type of transaction.
    char * note; ///< (opt) Any data up to 1000 bytes.
    union transaction
    {
        payment_tx_t pay;
        appl_tx_t appl;
    } tx;
} transaction_details_t;

#endif //VERTICES_SDK_LIB_INC_TRANSACTION_ALGORAND_H
