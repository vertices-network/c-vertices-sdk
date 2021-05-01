//
// Created by Cyril on 09/04/2021.
//

#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stddef.h>
#include <string.h>

#include "base32.h"
#include "vertices_log.h"
#include "vertices_errors.h"

TEST_GROUP(VerticesBase32){
    void setup() override {}
    void teardown() override {}
};

static void
prv_run_encode_test_case(const char *input_str, const char *expected_out_str)
{
    const size_t in_len = strlen(input_str);
    const size_t expected_size = strlen(expected_out_str);

    size_t encode_len = BASE32_ENCODE_LEN(in_len);

    char result[encode_len];
    memset(result, 0x00, encode_len);

    LOG_DEBUG("B32 encoding - Input binary len %zu, expected string len: %zu", in_len, encode_len);

    ret_code_t err_code = b32_encode(input_str, in_len, result, &encode_len);
    VTC_ASSERT(err_code);

    LONGS_EQUAL(expected_size, encode_len);
    MEMCMP_EQUAL(expected_out_str, result, expected_size);
}

// Test vectors for base32 from:
//   https://tools.ietf.org/html/rfc4648#page-12
TEST(VerticesBase32, Test_RfcTestVectorsEncode) {
    prv_run_encode_test_case("", "");
    prv_run_encode_test_case("f", "MY======");
    prv_run_encode_test_case("fo", "MZXQ====");
    prv_run_encode_test_case("foo", "MZXW6===");
    prv_run_encode_test_case("foob", "MZXW6YQ=");
    prv_run_encode_test_case("fooba", "MZXW6YTB");
    prv_run_encode_test_case("foobar", "MZXW6YTBOI======");
}

static void
prv_run_decode_test_case(const char *encoded_str, const char *expected_out_buf)
{
    size_t in_len = strlen(encoded_str);

    if (in_len > 1)
    {
        while (encoded_str[in_len-- -1] == '=');
    }

    size_t decode_buf_len = BASE32_DECODE_MIN_LEN(in_len);
    size_t expected_size = decode_buf_len;

    LOG_DEBUG("B32 decoding - Input str len %zu, expected binary len: %zu", in_len, decode_buf_len);

    char result[decode_buf_len];
    memset(result, 0x00, decode_buf_len);

    ret_code_t err_code = b32_decode(encoded_str, result, &decode_buf_len);
    VTC_ASSERT(err_code);

    LONGS_EQUAL(expected_size, decode_buf_len);
    MEMCMP_EQUAL(expected_out_buf, result, expected_size);
}

// Test vectors for base32 from:
//   https://tools.ietf.org/html/rfc4648#page-12
TEST(VerticesBase32, Test_RfcTestVectorsDecode) {
    prv_run_decode_test_case("", "");
    prv_run_decode_test_case("MY======", "f");
    prv_run_decode_test_case("MZXQ====", "fo");
    prv_run_decode_test_case("MZXW6===", "foo");
    prv_run_decode_test_case("MZXW6YQ=", "foob");
    prv_run_decode_test_case("MZXW6YTB", "fooba");
    prv_run_decode_test_case("MZXW6YTBOI======", "foobar");
}
