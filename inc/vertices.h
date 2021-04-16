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

ret_code_t
vertices_transaction_pay_new(size_t account_id, char *receiver, uint64_t amount);

ret_code_t
vertices_transaction_get(size_t bufid, signed_transaction_t **tx);

/// Call this function to have the Vertices SDK process pending events
/// This function will call the user-defined callback if any
/// \param evt
/// \return
ret_code_t
vertices_event_process(vtc_evt_t *evt);

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
