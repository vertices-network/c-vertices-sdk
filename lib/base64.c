//
// Created by Cyril on 01/04/2021.
//

#include "base64.h"

#include <stdint.h>
#include <vertices_errors.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// based on https://opensource.apple.com/source/QuickTimeStreamingServer/QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.c

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

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

ret_code_t
base64_encode(const unsigned char *intput_data,
              size_t input_length,
              char *encoded_data,
              size_t *data_length)
{
    VTC_ASSERT_BOOL(intput_data != NULL);
    VTC_ASSERT_BOOL(*data_length >= (4 * ((input_length + 2) / 3)));
    VTC_ASSERT_BOOL(encoded_data != NULL);

    size_t wr_idx = 0;
    for (int i = 0; i < input_length;)
    {
        uint32_t octet_a = i < input_length ? (unsigned char) intput_data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char) intput_data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char) intput_data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[wr_idx++] = encoding_table[(triple >> 18) & 0x3F];
        encoded_data[wr_idx++] = encoding_table[(triple >> 12) & 0x3F];
        encoded_data[wr_idx++] = encoding_table[(triple >> 6) & 0x3F];
        encoded_data[wr_idx++] = encoding_table[(triple >> 0) & 0x3F];
    }

    for (; wr_idx % 4 != 0; wr_idx++)
    {
        encoded_data[wr_idx] = '=';
    }

    *data_length = (4 * ((input_length + 2) / 3));

    return VTC_SUCCESS;
}

ret_code_t
b64_encode(const char *input, size_t input_size, char *encoded, size_t *output_size)
{
    VTC_ASSERT_BOOL(input != NULL);
    VTC_ASSERT_BOOL(encoded != NULL);

    if (*output_size < (4 * ((input_size + 2) / 3)))
    {
        return VTC_ERROR_NO_MEM;
    }

    int i;
    char *p;

    p = encoded;
    for (i = 0; i < input_size - 2; i += 3)
    {
        *p++ = encoding_table[(input[i] >> 2) & 0x3F];
        *p++ = encoding_table[((input[i] & 0x3) << 4) |
            ((int) (input[i + 1] & 0xF0) >> 4)];
        *p++ = encoding_table[((input[i + 1] & 0xF) << 2) |
            ((int) (input[i + 2] & 0xC0) >> 6)];
        *p++ = encoding_table[input[i + 2] & 0x3F];
    }
    if (i < input_size)
    {
        *p++ = encoding_table[(input[i] >> 2) & 0x3F];
        if (i == (input_size - 1))
        {
            *p++ = encoding_table[((input[i] & 0x3) << 4)];
            *p++ = '=';
        }
        else
        {
            *p++ = encoding_table[((input[i] & 0x3) << 4) |
                ((int) (input[i + 1] & 0xF0) >> 4)];
            *p++ = encoding_table[((input[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    *output_size = (size_t) (p - encoded);
    *p++ = '\0';

    return VTC_SUCCESS;
};

ret_code_t
b64_decode(const char *input_data,
           size_t input_length,
           char *decoded_data,
           size_t *output_length)
{
    VTC_ASSERT_BOOL(input_data != NULL);
    VTC_ASSERT_BOOL(decoded_data != NULL);

    if (*output_length < ((input_length / 4 * 3) - 1))
    {
        return VTC_ERROR_NO_MEM;
    }

    size_t nbytesdecoded;
    const unsigned char *bufin;
    unsigned char *bufout;
    size_t nprbytes;

    bufin = (const unsigned char *) input_data;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (bufin - (const unsigned char *) input_data) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

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

    nbytesdecoded -= (4 - nprbytes) & 3;
    *output_length = nbytesdecoded;

    return VTC_SUCCESS;
}
