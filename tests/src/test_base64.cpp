//
// Created by Cyril on 09/04/2021.
//

#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stddef.h>
#include <string.h>

#include "base64.h"
#include "vertices_log.h"
#include "vertices_errors.h"

TEST_GROUP(VerticesBase64){
    void setup() override {}
    void teardown() override {}
};

static void
prv_run_encode_test_case(const char *input_str, const char *expected_out_str)
{
    const size_t in_len = strlen(input_str);
    size_t encode_len = BASE64_ENCODE_LEN(in_len);
    const size_t expected_size = encode_len;
    char result[encode_len];
    memset(result, 0xA5, sizeof(result));

    LOG_DEBUG("B64 encoding - Input bin len %zu, expected str len: %zu", in_len, encode_len);

    ret_code_t err_code = b64_encode(input_str, in_len, result, &encode_len);
    VTC_ASSERT(err_code);

    LONGS_EQUAL(expected_size, encode_len);
    MEMCMP_EQUAL(expected_out_str, result, expected_size);
}

// Test vectors for base64 from:
//   https://tools.ietf.org/html/rfc4648#page-12
TEST(VerticesBase64, Test_RfcTestVectorsEncode) {
    prv_run_encode_test_case("", "");
    prv_run_encode_test_case("f", "Zg==");
    prv_run_encode_test_case("fo", "Zm8=");
    prv_run_encode_test_case("foo", "Zm9v");
    prv_run_encode_test_case("foob", "Zm9vYg==");
    prv_run_encode_test_case("fooba", "Zm9vYmE=");
    prv_run_encode_test_case("foobar", "Zm9vYmFy");
}

// Run encoding over all possible values
TEST(VerticesBase64, Test_All) {
    uint8_t bin_in[256];

    for (size_t i = 0; i < sizeof(bin_in); i++) {
        bin_in[i] = i & 0xff;
    }

    const size_t bin_len = sizeof(bin_in);
    size_t encode_len = BASE64_ENCODE_LEN(bin_len);

    char result[encode_len + 1];
    memset(result, 0xA5, sizeof(result));
    result[encode_len] = '\0';

    const char *expected_out_str =
        "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUm"
        "JygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNT"
        "k9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dX"
        "Z3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2"
        "en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TF"
        "xsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7"
        "e7v8PHy8/T19vf4+fr7/P3+/w==";
    b64_encode((const char *) bin_in, bin_len, result, &encode_len);

    LONGS_EQUAL(strlen(expected_out_str), encode_len);
    STRCMP_EQUAL(expected_out_str, result);
}


static void
prv_run_decode_test_case(const char *encoded_str, const char *expected_out_buf)
{
    const size_t in_len = strlen(encoded_str);
    size_t decode_buf_len = BASE64_DECODE_MIN_LEN(in_len);
    size_t expected_size = decode_buf_len;

    if (in_len > 0)
    {
        if (encoded_str[in_len - 1] == '=') expected_size--;
    }
    if (in_len > 1)
    {
        if (encoded_str[in_len - 2] == '=') expected_size--;
    }

    char result[decode_buf_len];
    memset(result, 0x00, decode_buf_len);

    LOG_DEBUG("B64 decoding - Input str len %zu, expected binary len: %zu", in_len, decode_buf_len);

    ret_code_t err_code = b64_decode(encoded_str, in_len, result, &decode_buf_len);
    VTC_ASSERT(err_code);

    LONGS_EQUAL(expected_size, decode_buf_len);
    MEMCMP_EQUAL(expected_out_buf, result, expected_size);
}

// Test vectors for base64 from:
//   https://tools.ietf.org/html/rfc4648#page-12
TEST(VerticesBase64, Test_RfcTestVectorsDecode) {
    prv_run_decode_test_case("", "");
    prv_run_decode_test_case("Zg==", "f");
    prv_run_decode_test_case("Zm8=", "fo");
    prv_run_decode_test_case("Zm9v", "foo");
    prv_run_decode_test_case("Zm9vYg==", "foob");
    prv_run_decode_test_case("Zm9vYmE=", "fooba");
    prv_run_decode_test_case("Zm9vYmFy", "foobar");
}
