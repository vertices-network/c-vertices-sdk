//
// Created by Cyril on 17/03/2021.
//

#include <string.h>
#include <vertices_config.h>
#include <tcp.h>
#include "vertices.h"

static vertice_t m_vertice = {0};
static int m_socket = 0;

err_code_t
vertices_new()
{
    err_code_t err_code = VTC_SUCCESS;
    memset(&m_vertice, 0, sizeof m_vertice);

    err_code = tcp_connect(SERVER_URL, &m_socket);

    return err_code;
}

err_code_t
vertices_get_health(vertice_t *new)
{
    return VTC_SUCCESS;
}