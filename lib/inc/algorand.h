//
// Created by Cyril on 15/04/2021.
//

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
    ALGORAND_APPLICATION_CALL_TRANSATION,
} tx_type_t;

typedef struct
{
    uint8_t receiver[ADDRESS_LENGTH]; ///< The address of the account that receives the amount.
    uint64_t amount; ///< The total amount to be sent in microAlgos.
    uint8_t close_remainder_to[ADDRESS_LENGTH
    ]; ///< When set, it indicates that the transaction is requesting that the Sender account should be closed, and all remaining funds, after the fee and amount are paid, be transferred to this address.
} payment_tx_t;

typedef struct
{
    uint64_t
        first_valid; ///< The first round for when the transaction is valid. If the transaction is sent prior to this round it will be rejected by the network.
    uint64_t
        last_valid; ///< The ending round for which the transaction is valid. After this round, the transaction will be rejected by the network.
    tx_type_t tx_type; ///< Specifies the type of transaction.
    union
    {
        payment_tx_t payment_tx;
    };
} transaction_details_t;

#endif //VERTICES_SDK_LIB_INC_TRANSACTION_ALGORAND_H
