//
// Created by Cyril on 16/04/2021.
//

#include <string.h>
#include "sha512_256.h"
#include "mbedtls/sha512.h"

ret_code_t
sha512_256(unsigned char const *input,
           unsigned long ilen,
           unsigned char *output,
           unsigned long olen)
{
    ret_code_t err_code = VTC_SUCCESS;
    mbedtls_sha512_context ctx = {0};

    if (ilen == 0 || input == NULL || output == NULL)
    {
        return VTC_ERROR_INVALID_PARAM;
    }

    mbedtls_sha512_init(&ctx);

    // Set hash start to SHA-512/256
    ctx.state[0] = 0x22312194FC2BF72C;
    ctx.state[1] = 0x9F555FA3C84C64C2;
    ctx.state[2] = 0x2393B86B6F53B151;
    ctx.state[3] = 0x963877195940EABD;
    ctx.state[4] = 0x96283EE2A88EFFE3;
    ctx.state[5] = 0xBE5E1E2553863992;
    ctx.state[6] = 0x2B0199FC2C85B8AA;
    ctx.state[7] = 0x0EB72DDC81C52CA2;

#if defined(SOC_SHA_SUPPORT_PARALLEL_ENG) && SOC_SHA_SUPPORT_PARALLEL_ENG
    ctx.mode = ESP_MBEDTLS_SHA512_SOFTWARE;
#endif

    // copy input into 128-byte long array
    // as 128 bytes are used in the input vector
    unsigned char input_128[128] = {0};
    memcpy(input_128, input, ilen);

    int ret = mbedtls_sha512_update_ret(&ctx, input_128, ilen);

    if (ret == 0)
    {
        if (olen < 128)
        {
            unsigned char buffer_out[128] = {0};

            ret = mbedtls_sha512_finish_ret(&ctx, buffer_out);
            memcpy(output, buffer_out, olen);
        }
        else
        {
            ret = mbedtls_sha512_finish_ret(&ctx, output);
        }
    }

    if (ret != 0)
    {
        err_code = VTC_ERROR_INTERNAL;
    }

    mbedtls_sha512_free(&ctx);

    return (err_code);
}
