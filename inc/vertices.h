//
// Created by Cyril on 17/03/2021.
//

#ifndef VERTICES_SRC_VERTICE_H
#define VERTICES_SRC_VERTICE_H

#include <stdint.h>
#include "account.h"
#include "vertices_errors.h"
#include "provider.h"

typedef struct
{
    provider_t provider;
} vertice_t;

err_code_t
vertices_new();


#endif //VERTICES_SRC_VERTICE_H
