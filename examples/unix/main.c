//
// Created by Cyril on 17/03/2021.
//

#include <printf.h>
#include <vertices.h>
#include <vertices_log.h>
#include <vertices_errors.h>

int
main(int argc, char *argv[])
{
    LOG_INFO("😎 Vertices SDK running on Unix-based OS");

    err_code_t err_code = vertices_new();
    LOG_DEBUG("Returned %d", err_code);
}