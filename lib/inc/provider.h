//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_SRC_PROVIDER_H
#define VERTICES_SRC_PROVIDER_H

#include <stdio.h>
#include "vertices_types.h"
#include "http.h"
#include "transaction.h"
#include <account.h>

typedef struct
{
    provider_info_t provider;
    provider_version_t version;
    payload_t response_buffer;
    size_t
    (*response_payload_cb)(void *received_data, size_t size, size_t count, void *response_payload);
} provider_t;

ret_code_t
provider_account_info_get(account_details_t *account);

///
/// \param version
/// \return
/// * VTC_ERROR_OFFLINE version has not been updated with latest values due to error on HTTP request
/// * VTC_SUCCESS       version has correctly been fetched from remote
ret_code_t
provider_version_get(provider_version_t *version);

ret_code_t
provider_tx_params_load(transaction_t *tx);

ret_code_t
provider_tx_post(const uint8_t *bin_payload, size_t length);

ret_code_t
provider_ping();

ret_code_t
provider_init(provider_info_t *provider);

#endif //VERTICES_SRC_PROVIDER_H
