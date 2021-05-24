//
// Created by Cyril on 16/04/2021.
//

#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stddef.h>
#include <string.h>

#include "sha512_256.h"
#include "vertices_log.h"
#include "vertices_errors.h"

TEST_GROUP(VerticesSha512_256){
    void setup() override {}
    void teardown() override {}
};

const unsigned char one_block_message[32] =
    {
        0x53, 0x04, 0x8E, 0x26, 0x81, 0x94, 0x1E, 0xF9, 0x9B, 0x2E, 0x29, 0xB7, 0x6B, 0x4C, 0x7D,
        0xAB,
        0xE4, 0xC2, 0xD0, 0xC6, 0x34, 0xFC, 0x6D, 0x46, 0xE0, 0xE2, 0xF1, 0x31, 0x07, 0xE7, 0xAF,
        0x23
    };
const unsigned char two_block_message[32] =
    {
        0x39, 0x28, 0xE1, 0x84, 0xFB, 0x86, 0x90, 0xF8, 0x40, 0xDA, 0x39, 0x88, 0x12, 0x1D, 0x31,
        0xBE, 0x65, 0xCB, 0x9D, 0x3E, 0xF8, 0x3E, 0xE6, 0x14, 0x6F, 0xEA, 0xC8, 0x61, 0xE1, 0x9B,
        0x56, 0x3A
    };

static void
prv_run_encode_test_case(const unsigned char *input_str, const char expected_out_buf[32])
{
    const size_t in_len = strlen((const char *) input_str);
    unsigned char result[32] = {0};

    LOG_DEBUG("SHA512/256 - Input str len %zu", in_len);

    ret_code_t ret = (ret_code_t) sha512_256(input_str, in_len, (unsigned char *) result, sizeof(result));
    LOG_DEBUG("SHA512/256 - Ret %d", ret);

    MEMCMP_EQUAL(expected_out_buf, result, 32);
}

// tests from
// https://csrc.nist.gov/csrc/media/projects/cryptographic-standards-and-guidelines/documents/examples/sha512_256.pdf
TEST(VerticesSha512_256, Test_TestVectorsHash)
{
    prv_run_encode_test_case((const unsigned char *)"abc", (const char *) one_block_message);
    prv_run_encode_test_case((const unsigned char *)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", (const char *) two_block_message);
}


