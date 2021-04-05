//
// Created by Cyril on 01/04/2021.
//

#include <vertices_errors.h>
#include <stdio.h>
#ifndef VERTICES_SDK_LIB_BASE64_H
#define VERTICES_SDK_LIB_BASE64_H

ret_code_t
b64_encode(const char *input, size_t input_size, char *encoded, size_t *output_size);

ret_code_t
b64_decode(const char *input_data,
           size_t input_length,
           char *decoded_data,
           size_t *output_length);

#endif //VERTICES_SDK_LIB_BASE64_H
