//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_INC_HTTP_H
#define VERTICES_INC_HTTP_H

#include "provider.h"

err_code_t
http_init(const provider_t * provider);

err_code_t
http_get(const provider_t * provider, char* relative_path, const char * headers);

err_code_t
http_post(const provider_t * provider, char* relative_path, const char * headers, const char* body);

void
http_close();

#endif //VERTICES_INC_HTTP_H
