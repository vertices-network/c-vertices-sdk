//
// Created by Cyril on 02/04/2021.
//

#ifndef VERTICES_SDK_LIB_BASE32_H
#define VERTICES_SDK_LIB_BASE32_H

#include <stdint.h>
#include <vertices_errors.h>
#include <stdio.h>

ret_code_t
base32_decode(const uint8_t *encoded, uint8_t *result, int buf_size, size_t *output_size);

ret_code_t
base32_encode(const uint8_t *data, int length, uint8_t *result,
              int buf_size, size_t *output_size);

#endif //VERTICES_SDK_LIB_BASE32_H
