/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VERTICES_SDK_LIB_BASE64_H
#define VERTICES_SDK_LIB_BASE64_H

#include <vertices_errors.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Computes how many bytes will be needed to encode/decode a binary blob of bin_len
//! using base64
#define BASE64_ENCODE_LEN(bin_len) (4 * (((bin_len) + 2) / 3))
#define BASE64_DECODE_MIN_LEN(bin_len) ((bin_len > 3) ? (bin_len / 4 * 3) : 0)

/// Encode binary input into base-64 string. `output_size` must be set to the `encoded` buffer size.
/// `output_size` will be updated depending on how many bytes have been written.
/// `VTC_ERROR_NO_MEM` is returned if the `encoded` buffer is not large enough
/// The `\0` character is not appended to the encoded string
/// \param input Binary to encode
/// \param input_size Size of the binary to encode
/// \param encoded_data Pointer to buffer where to put the encoded binary (string)
/// \param output_size Size of the base-64 string, without the `\0` char.
/// \return error code:
///  - VTC_ERROR_NO_MEM if buffer not large enough to encode the input binary
///  - VTC_SUCCESS on success
ret_code_t
b64_encode(const char *input, size_t input_size, char *encoded_data, size_t *output_size);

/// Decode base-64 string into binary buffer. `output_size` must be set to the `decoded_data` buffer size.
/// \param input_data Pointer to string
/// \param input_length String length, without termination character
/// \param decoded_data Pointer to decoded data
/// \param output_size Size of the `decoded_data` buffer. Value will be updated depending on
/// how many bytes are used
/// \return error code:
///  - VTC_ERROR_NO_MEM if buffer not large enough to encode the input binary
///  - VTC_SUCCESS on success
ret_code_t
b64_decode(const char *input_data,
           size_t input_length,
           char *decoded_data,
           size_t *output_size);

#ifdef __cplusplus
}
#endif

#endif //VERTICES_SDK_LIB_BASE64_H
