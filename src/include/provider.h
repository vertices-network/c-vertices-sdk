/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_SRC_PROVIDER_H
#define VERTICES_SRC_PROVIDER_H

#include <stdio.h>
#include "vertices_types.h"
#include "vertices_http.h"
#include "transaction.h"
#include <account.h>

typedef struct
{
    char *data;
    size_t size;
} payload_t;

typedef struct
{
    provider_info_t provider;
    provider_version_t version;
    payload_t response_buffer;
    size_t
    (*response_payload_cb)(char *received_data, size_t size);
} provider_t;

ret_code_t
provider_application_info_get(uint64_t app_id, app_values_t * global_states);

ret_code_t
provider_account_info_get(account_details_t *account);

/// Get provider version
/// \param version
/// \return
/// * VTC_ERROR_OFFLINE version has not been updated with latest values due to error on HTTP request
/// * VTC_SUCCESS       version has correctly been fetched from remote
ret_code_t
provider_version_get(provider_version_t *version);

/// Get transactions parameters such as first-valid and minimum fee.
/// \param tx
/// \return
/// \c VTC_SUCCESS on success
/// \c VTC_ERROR_OFFLINE when previously fetched data is being used. Those params can be used.
/// \c VTC_ERROR_HTTP_BASE when error fetching tx params
ret_code_t
provider_tx_params_load(transaction_t *tx);

ret_code_t
provider_tx_post(const uint8_t *bin_payload, size_t length, unsigned char * tx_id);

ret_code_t
provider_ping(void);

ret_code_t
provider_init(provider_info_t *provider);

#endif //VERTICES_SRC_PROVIDER_H
