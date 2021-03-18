//
// Created by Cyril on 17/03/2021.
//

#include <provider.h>
#include "vertices.h"

static int m_socket = 0;

err_code_t
vertices_new()
{
    err_code_t err_code = VTC_SUCCESS;

    provider_init();

    return err_code;
}
