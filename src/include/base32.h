/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef VERTICES_SDK_LIB_BASE32_H
#define VERTICES_SDK_LIB_BASE32_H

#include <stdint.h>
#include <vertices_errors.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Computes how many bytes will be needed to encode/decode a binary blob of bin_len
//! using base32
#define BASE32_ENCODE_LEN(bin_len)      ((((bin_len * 8 + 4) / 5) % 8) != 0 ? \
                                    (((bin_len * 8 + 4) / 5) + (8 - (((bin_len * 8 + 4) / 5) % 8))) \
                                    : ((bin_len * 8 + 4) / 5))
#define BASE32_DECODE_MIN_LEN(bin_len)  ((bin_len > 0) ? (bin_len * 5 / 8 + 1) : 0 )

/// Encode binary input into base-32 string. `output_size` must be set to the `encoded` buffer size.
/// `output_size` will be updated depending on how many bytes have been written.
/// `VTC_ERROR_INVALID_PARAM` is returned if the input binary is too long
/// The `\0` character is not appended to the encoded string
/// \param data Binary to encode
/// \param length Size of the binary to encode
/// \param encoded Pointer to buffer where to put the encoded binary (string)
/// \param output_size Size of the base-64 string, without the `\0` char.
/// \return error code:
///  - VTC_ERROR_INVALID_PARAM if the input binary is too long
///  - VTC_SUCCESS on success
ret_code_t
b32_encode(const char *data, size_t length, char *encoded, size_t *output_size);

/// Decode base-32 string into binary buffer. `output_size` must be set to the `decoded_data` buffer size.
/// \param encoded
/// \param decoded_data Pointer to decoded data
/// \param output_size Size of the `decoded_data` buffer. Value will be updated depending on
///  how many bytes are used
/// \return
ret_code_t
b32_decode(const char *encoded, char *decoded_data, size_t *output_size);

#ifdef __cplusplus
}
#endif

#endif //VERTICES_SDK_LIB_BASE32_H
