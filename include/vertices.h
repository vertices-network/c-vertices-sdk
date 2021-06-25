//! @file
//!
//! Vertices Network
//! See License.txt for details
//!
//! Created by Cyril on 17/03/2021.


#ifndef VERTICES_SRC_VERTICE_H
#define VERTICES_SRC_VERTICE_H

#include <stdint.h>
#include <stdio.h>
#include "vertices_types.h"

#ifdef __cplusplus
extern "C" {
#endif

ret_code_t
vertices_version(provider_version_t *version);

ret_code_t
vertices_ping(void);

ret_code_t
vertices_account_new_from_b32(char *public_b32, account_info_t **account);

ret_code_t
vertices_account_new_from_bin(char *public_key, account_info_t **account);

ret_code_t
vertices_account_update(account_info_t *account);

ret_code_t
vertices_account_free(account_info_t *account);

/// Send \c amount of tokens from \c account_id to \c receiver
/// \param account_id Handle to the Sender account
/// \param receiver Receiver account
/// \param amount Amount of token to be sent
/// \param params Pointer to type, where type is known by blockchain implementation. Can be NULL.
/// \return \c VTC_ERROR_INVALID_PARAM if one parameter is incorrect
ret_code_t
vertices_transaction_pay_new(account_info_t *account, char *receiver, uint64_t amount, void *params);

/// Call Smart Contract DApp
/// \param account_id Account handle, see \c vertices_account_new_from_b32
/// \param app_id Application ID
/// \param params Pointer to type, where type is known by blockchain implementation. Can be NULL.
/// \return
ret_code_t
vertices_transaction_app_call(account_info_t *account, uint64_t app_id, void *params);

ret_code_t
vertices_transaction_get(size_t bufid, signed_transaction_t **tx);

/// Schedule new Vertices event, see \c vtc_evt_t
/// See \c vertices_event_process to process queued events
/// \param evt New event to be processed
/// \return Error code
/// * \c VTC_ERROR_NO_MEM if the event cannot be pushed to the event queue due to not enough memory, consider increasing \c VTC_EVENTS_COUNT
/// * \c VTC_SUCCESS on success
ret_code_t
vertices_event_schedule(vtc_evt_t *evt);

/// Call this function to have the Vertices SDK process pending events. Whenever calling a Vertices
/// function, make sure to process all the events.
/// To make sure all the events are processed, this function should be called from an infinite loop or OS thread/task.
/// This function will call the user-defined callback if one has been passed when using \see vertices_new
/// \param queue_size
/// \return \c VTC_SUCCESS if event has correctly been processed
ret_code_t
vertices_event_process(size_t *queue_size);

/// Initialize Vertices SDK
/// \param config Pass the configuration such as providers and user-defined event handler
/// \return
/// * VTC_SUCCESS on success
/// * VTC_ERROR_INTERNAL if HTTP client cannot be initialized
ret_code_t
vertices_new(vertex_t *config);

#ifdef __cplusplus
}
#endif

#endif //VERTICES_SRC_VERTICE_H
