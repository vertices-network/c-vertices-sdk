/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_SDK_SRC_ALGORAND_ALGORAND_MPACK_PARSER_H
#define VERTICES_SDK_SRC_ALGORAND_ALGORAND_MPACK_PARSER_H

#include "vertices_types.h"
#include "account.h"

ret_code_t
parser_account_mpack(const char *buf, size_t length, account_details_t * account);

/// Parse global states from JSON (response from /v2/applications/{app_id})
/// \param buf
/// \param length
/// \param global_states
/// \return codes:
/// * VTC_SUCCESS on success
/// * VTC_ERROR_INTERNAL if unable to instantiate parser
/// * VTC_ERROR_NOT_FOUND if key not found
ret_code_t
parser_application_json(const char *buf, size_t length, app_values_t * global_states);

#endif //VERTICES_SDK_SRC_ALGORAND_ALGORAND_MPACK_PARSER_H
