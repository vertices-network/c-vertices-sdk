//
// Created by Cyril on 02/04/2021.
//

#include "base32.h"

ret_code_t
base32_decode(const uint8_t *encoded, uint8_t *result, int buf_size, size_t *output_size)
{
    int buffer = 0;
    int bitsLeft = 0;
    int count = 0;
    for (const uint8_t *ptr = encoded; count < buf_size && *ptr; ++ptr)
    {
        uint8_t ch = *ptr;
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
            ch = (ch & 0x1F) - 1;
        }
        else if (ch >= '2' && ch <= '7')
        {
            ch -= '2' - 26;
        }
        else
        {
            return -1;
        }

        buffer |= ch;
        bitsLeft += 5;
        if (bitsLeft >= 8)
        {
            result[count++] = buffer >> (bitsLeft - 8);
            bitsLeft -= 8;
        }
    }
    if (count < buf_size)
    {
        result[count] = '\000';
    }

    *output_size = count;

    return VTC_SUCCESS;
}

ret_code_t
base32_encode(const uint8_t *data, int length, uint8_t *result,
              int buf_size, size_t *output_size)
{
    if (length < 0 || length > (1 << 28))
    {
        return -1;
    }
    int count = 0;
    if (length > 0)
    {
        int buffer = data[0];
        int next = 1;
        int bitsLeft = 8;
        while (count < buf_size && (bitsLeft > 0 || next < length))
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
            result[count++] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"[index];
        }
    }
    if (count < buf_size)
    {
        result[count] = '\000';
    }

    *output_size = count;

    return VTC_SUCCESS;
}