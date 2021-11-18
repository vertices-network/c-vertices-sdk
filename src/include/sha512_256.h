/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_SDK_LIB_SHA512256_H
#define VERTICES_SDK_LIB_SHA512256_H

#include <vertices_errors.h>

#ifdef __cplusplus
extern "C" {
#endif

ret_code_t
sha512_256(unsigned char const *input,
           unsigned long ilen,
           unsigned char *output,
           unsigned long olen);

#ifdef __cplusplus
}
#endif

#endif //VERTICES_SDK_LIB_SHA512256_H
