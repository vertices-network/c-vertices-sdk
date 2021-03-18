#include <vertices_errors.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <vertices_log.h>
#include "vertices.h"
#include "vertices_config.h"

#if !(__unix__ || __APPLE__)
#error You are trying to compile a file running on Unix-based OS
#endif


err_code_t
tcp_connect(char *url, int * socket_id)
{
    struct hostent *server;
    struct sockaddr_in serv_addr;

    // create the socket
    *socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_id < 0)
    {
        errno = EOPNOTSUPP;
        LOG_ERROR("ERROR opening socket");
        return VTC_ERROR_INVALID_STATE;
    }

    // lookup the ip address
    server = gethostbyname(url);
    if (server == NULL)
    {
        errno = EHOSTUNREACH;
        LOG_ERROR("ERROR, no such host");
        return VTC_ERROR_NOT_FOUND;
    }

    // fill in the structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // connect the socket
    if (connect(*socket_id, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        errno = ENOTCONN;
        LOG_ERROR("ERROR connecting");
        return VTC_ERROR_NOT_FOUND;
    }

    LOG_INFO("Connected to host: %s", url);

    return VTC_SUCCESS;
}

err_code_t
tcp_send(int socket_id, char *data, size_t length)
{
    size_t sent = 0;
    size_t bytes = 0;
    do
    {
        bytes = write(socket_id, data + sent, length - sent);

        if (bytes < 0)
        {
            LOG_ERROR("ERROR writing message to socket");
        }

        if (bytes == 0)
        {
            break;
        }

        sent += bytes;
    }
    while (sent < length);

    LOG_INFO("Sent %zu bytes to remote host (socket %d)", sent, socket_id);

    return VTC_SUCCESS;
}

err_code_t
tcp_receive(int socket_id, char *data, size_t *size)
{
    size_t buffer_size = *size;

    memset(data, 0, buffer_size);
    size_t received = 0;
    size_t bytes = 0;
    do
    {
        bytes = read(socket_id, data + received, buffer_size - received);

        if (bytes < 0)
        {
            LOG_ERROR("ERROR reading response from socket");
        }

        if (bytes == 0)
        {
            // we are done receiving data
            break;
        }

        received += bytes;
    }
    while (received < buffer_size);

    if (received == buffer_size)
    {
        LOG_ERROR("ERROR storing complete response from socket");
        return VTC_ERROR_NO_MEM;
    }

    // update size to received data size
    *size = received;

    LOG_INFO("Received %zu bytes from remote (socket %d)", *size, socket_id);

    return VTC_SUCCESS;
}

err_code_t
tcp_close(int * socket_id)
{
    int sock = *socket_id;

    close(*socket_id);
    *socket_id = 0;

    LOG_INFO("Closed socket %d", sock);

    return VTC_SUCCESS;
}