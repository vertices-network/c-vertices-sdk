/*
 * Copyright (c) 2021 Vertices Network <cyril@vertices.network>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "base32.h"

ret_code_t
b32_encode(const char *data, size_t length, char *encoded, size_t *output_size)
{
    if (length == 0)
    {
        *output_size = 0;
        return VTC_SUCCESS;
    }

    if (length > (1 << 28))
    {
        return VTC_ERROR_INVALID_PARAM;
    }

    size_t count = 0;
    if (length > 0)
    {
        int buffer = data[0];
        size_t next = 1;
        int bitsLeft = 8;
        while (count < *output_size && (bitsLeft > 0 || next < length))
        {
            if (bitsLeft < 5)
            {
                if (next < length)
                {
                    buffer <<= 8;
                    buffer |= data[next++] & 0xFF;
                    bitsLeft += 8;
                }
                else
                {
                    int pad = 5 - bitsLeft;
                    buffer <<= pad;
                    bitsLeft += pad;
                }
            }
            int index = 0x1F & (buffer >> (bitsLeft - 5));
            bitsLeft -= 5;
            encoded[count++] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"[index];
        }
    }
    while (count % 8 != 0 && count < *output_size)
    {
        encoded[count++] = '=';
    }

    *output_size = count;

    return VTC_SUCCESS;
}

ret_code_t
b32_decode(const char *encoded, char *decoded_data, size_t *output_size)
{
    int buffer = 0;
    int bitsLeft = 0;
    size_t count = 0;
    for (const char *ptr = encoded; count < *output_size && *ptr && *ptr != '='; ++ptr)
    {
        char ch = *ptr;
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '-')
        {
            continue;
        }
        buffer <<= 5;

        // Deal with commonly mistyped characters
        if (ch == '0')
        {
            ch = 'O';
        }
        else if (ch == '1')
        {
            ch = 'L';
        }
        else if (ch == '8')
        {
            ch = 'B';
        }

        // Look up one base32 digit
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            ch = (char) ((ch & 0x1F) - 1);
        }
        else if (ch >= '2' && ch <= '7')
        {
            ch = (char) (ch - ('2' - 26));
        }
        else
        {
            return VTC_ERROR_INVALID_PARAM;
        }

        buffer |= ch;
        bitsLeft += 5;
        if (bitsLeft >= 8)
        {
            decoded_data[count++] = (char) (buffer >> (bitsLeft - 8));
            bitsLeft -= 8;
        }
    }
    if (count < *output_size)
    {
        decoded_data[count] = '\000';
    }

    *output_size = count;

    return VTC_SUCCESS;
}
