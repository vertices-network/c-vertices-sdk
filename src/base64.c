/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "base64.h"

#include <stdint.h>
#include <vertices_errors.h>
#include <string.h>

// based on https://opensource.apple.com/source/QuickTimeStreamingServer/QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.c

static const unsigned char pr2six[256] =
    {
        /* ASCII table */
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
        64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
        64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
    };

static char prv_get_char_from_word(uint32_t word, int offset) {
    const char *base64_table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint8_t base64_mask = 0x3f; // one char per 6 bits
    return base64_table[(word >> (offset * 6)) & base64_mask];
}

ret_code_t
b64_encode(const char *input, size_t input_size, char *encoded_data, size_t *output_size)
{
    VTC_ASSERT_BOOL(input != NULL);
    VTC_ASSERT_BOOL(encoded_data != NULL);

    if (*output_size < (4 * ((input_size + 2) / 3)))
    {
        return VTC_ERROR_NO_MEM;
    }

    const uint8_t *bin_inp = (const uint8_t *)input;
    char *out_bufp = (char *)encoded_data;

    int curr_idx = 0;

    for (size_t bin_idx = 0; bin_idx < input_size;  bin_idx += 3) {
        const uint32_t byte0 = bin_inp[bin_idx];
        const uint32_t byte1 = ((bin_idx + 1) < input_size) ? bin_inp[bin_idx + 1] : 0;
        const uint32_t byte2 = ((bin_idx + 2) < input_size) ? bin_inp[bin_idx + 2] : 0;
        const uint32_t triple = (byte0 << 16) + (byte1 << 8) + byte2;

        out_bufp[curr_idx++] = prv_get_char_from_word(triple, 3);
        out_bufp[curr_idx++] = prv_get_char_from_word(triple, 2);
        out_bufp[curr_idx++] = ((bin_idx + 1) < input_size) ? prv_get_char_from_word(triple, 1) : '=';
        out_bufp[curr_idx++] = ((bin_idx + 2) < input_size) ? prv_get_char_from_word(triple, 0) : '=';
    }

    *output_size = (size_t) curr_idx;

    return VTC_SUCCESS;
}

ret_code_t
b64_decode(const char *input_data,
           size_t input_length,
           char *decoded_data,
           size_t *output_size)
{
    VTC_ASSERT_BOOL(input_data != NULL);
    VTC_ASSERT_BOOL(decoded_data != NULL);

    if (input_length == 0)
    {
        *output_size = 0;
        return VTC_SUCCESS;
    }

    if (*output_size < ((input_length / 4 * 3) - 1))
    {
        return VTC_ERROR_NO_MEM;
    }

    const unsigned char *bufin;
    unsigned char *bufout;
    size_t nprbytes;

    bufin = (const unsigned char *) input_data;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (size_t) (bufin - (const unsigned char *) input_data) - 1;

    bufout = (unsigned char *) decoded_data;
    bufin = (const unsigned char *) input_data;

    while (nprbytes > 4)
    {
        *(bufout++) =
            (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
        *(bufout++) =
            (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
        *(bufout++) =
            (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1)
    {
        *(bufout++) =
            (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2)
    {
        *(bufout++) =
            (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3)
    {
        *(bufout++) =
            (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }

    *output_size = (size_t)bufout - (size_t)decoded_data;

    return VTC_SUCCESS;
}
