//
// Created by Cyril on 18/03/2021.
//

#ifndef VERTICES_INC_TCP_H
#define VERTICES_INC_TCP_H

#include "vertices_errors.h"

err_code_t
tcp_connect(char *url, int * socket_id);

err_code_t
tcp_send(int socket_id, char *data, size_t length);

err_code_t
tcp_receive(int socket_id, char *data, size_t *size);

err_code_t
tcp_close(int * socket_id);

#endif //VERTICES_INC_TCP_H
