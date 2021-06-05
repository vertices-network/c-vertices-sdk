//
// Created by Cyril on 04/06/2021.
//

#ifndef VERTICES_SDK_SRC_ALGORAND_ALGORAND_MPACK_PARSER_H
#define VERTICES_SDK_SRC_ALGORAND_ALGORAND_MPACK_PARSER_H

#include "vertices_types.h"
#include "account.h"

ret_code_t
parser_account(const char *buf, size_t length, account_details_t * account);

#endif //VERTICES_SDK_SRC_ALGORAND_ALGORAND_MPACK_PARSER_H
