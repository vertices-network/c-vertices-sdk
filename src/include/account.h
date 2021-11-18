/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef C_VERTICES_SDK_SRC_ACCOUNT_H
#define C_VERTICES_SDK_SRC_ACCOUNT_H

#include <vertices_types.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum sig_type_e
{
    SIG_TYPE_SIG = 0,
    SIG_TYPE_MSIG,
    SIG_TYPE_LSIG,
} sig_type_t;

typedef enum delegation_status_e
{
    DELEGATION_STATUS_OFFLINE = 0,      //!< indicates that the associated account is delegated.
    DELEGATION_STATUS_ONLINE,           //!< indicates that the associated account used as part of the delegation pool.
    DELEGATION_STATUS_NOTPARTICIPATING, //!< indicates that the associated account is neither a delegator nor a delegate.
} delegation_status_t;

/// account_participation_t describes the parameters used by this account in consensus protocol.
typedef struct
{
    char selection_pkey_b64[ADDRESS_LENGTH
    ]; //!< [sel] Selection public key (if any) currently registered for this round.
    char vote_participation_pkey_b64[ADDRESS_LENGTH
    ]; //!< [vote] root participation public key (if any) currently registered for this round.
    int32_t vote_first_valid; //!< [voteFst] First round for which this participation is valid.
    int32_t vote_last_valid; //!< [voteLst] Last round for which this participation is valid.
    int32_t vote_key_dilution; //!< [voteKD] Number of subkeys in each batch of participation keys.
} account_participation_t;

/// Checkout https://developer.algorand.org/docs/reference/rest-apis/algod/v2/#applicationlocalstate
typedef struct
{
    uint64_t app_id; //!< app ID
    app_values_t key_values; //!< stored variables
} apps_local_state_t;

typedef struct
{
    account_info_t info; //!< Account generic infos: public keys and balance. ⚠️ Must be at the beginning of the structure as it will be casted to \c account_info_t.
    int32_t round; //!< The round for which this information is relevant.
    int32_t
        rewards; //!< [ern] total rewards of MicroAlgos the account has received, including pending rewards.
    int32_t pending_rewards; //!< amount of MicroAlgos of pending rewards in this account.
    int32_t
        reward_base; //!< [ebase] used as part of the rewards computation. Only applicable to accounts which are participating.
    delegation_status_t
        delegation_status; //!< string -> enum, [onl] - delegation status of the account's MicroAlgos
    sig_type_t
        sig_type; //!< sig-type string -> enum - Indicates what type of signature is used by this account, must be one of:
    account_participation_t participation;
    uint32_t app_idx;
    apps_local_state_t apps_local[APPS_MAX_COUNT];
} account_details_t;

ret_code_t
account_new(char *public_b32, account_info_t **account);

ret_code_t
account_free(account_info_t *account);

ret_code_t
account_update(account_info_t *account);

bool
account_has_app(account_info_t *account, uint64_t app_id);

ret_code_t
account_balance(account_info_t *account, int32_t *balance);

ret_code_t
account_init(void);

#endif //C_VERTICES_SDK_SRC_ACCOUNT_H
