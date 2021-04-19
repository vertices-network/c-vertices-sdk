//
// Created by Cyril on 17/03/2021.
//

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
vertices_ping();

ret_code_t
vertices_add_account(account_info_t *account, size_t *account_id);

ret_code_t
vertices_del_account(size_t account_handle);

/// Send tokens
/// \param account_id Handle to the Sender account
/// \param receiver Receiver account
/// \param amount Amount of token to be sent
/// \param params Pointer to type, where type is known by blockchain implementation
/// \return \c VTC_ERROR_INVALID_PARAM if one parameter is incorrect
ret_code_t
vertices_transaction_pay_new(size_t account_id, char *receiver, uint64_t amount, void * params);

ret_code_t
vertices_transaction_get(size_t bufid, signed_transaction_t **tx);

ret_code_t
vertices_event_schedule(vtc_evt_t *evt);

/// Call this function to have the Vertices SDK process pending events
/// This function will call the user-defined callback if one has been passed when using \see vertices_new
/// \param queue_size
/// \return \c VTC_SUCCESS if event has correctly been processed
ret_code_t
vertices_event_process(size_t * queue_size);

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
