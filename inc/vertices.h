//
// Created by Cyril on 17/03/2021.
//

#ifndef VERTICES_SRC_VERTICE_H
#define VERTICES_SRC_VERTICE_H

#include <stdint.h>
#include "account.h"
#include "vertices_errors.h"

typedef struct
{
    int32_t example;
} vertice_t;

err_code_t
vertices_new();


#endif //VERTICES_SRC_VERTICE_H
