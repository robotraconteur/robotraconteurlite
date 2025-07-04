/* Copyright 2011-2024 Wason Technology, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "robotraconteurlite/tcp_transport.h"

#include <stdlib.h>
#include <string.h>

/* Linux socket includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#ifndef __APPLE__
#include <endian.h>
#endif
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <poll.h>

#ifdef ROBOTRACONTEURLITE_USE_OPENSSL
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#endif

#define FLAGS_CHECK_ALL ROBOTRACONTEURLITE_FLAGS_CHECK_ALL
#define FLAGS_CHECK ROBOTRACONTEURLITE_FLAGS_CHECK
#define FLAGS_SET ROBOTRACONTEURLITE_FLAGS_SET
#define FLAGS_CLEAR ROBOTRACONTEURLITE_FLAGS_CLEAR

#define FAILED ROBOTRACONTEURLITE_FAILED

robotraconteurlite_status robotraconteurlite_tcp_sha1(const robotraconteurlite_byte* data,
                                                      robotraconteurlite_size_t len,
                                                      struct robotraconteurlite_tcp_sha1_storage* storage)
{
#ifdef ROBOTRACONTEURLITE_USE_OPENSSL
    SHA1(data, len, storage->sha1_bytes);

    return 0;
#else
    /* No SHA1 implementation on this platform! */
    assert(0);
    return ROBOTRACONTEURLITE_ERROR_NOT_IMPLEMENTED;
#endif
}

robotraconteurlite_status robotraconteurlite_tcp_base64_encode(const robotraconteurlite_byte* binary_data,
                                                               robotraconteurlite_size_t binary_len, char* base64_data,
                                                               robotraconteurlite_size_t* base64_len)
{
#ifdef ROBOTRACONTEURLITE_USE_OPENSSL
    /* Use OpenSSL base64 implementation */
    robotraconteurlite_size_t len = 0;
    char* base64_data_ptr = NULL; /* Make space for annoying null byte */
    BIO* bmem = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, binary_data, (int)binary_len);
    BIO_flush(b64);
    len = BIO_get_mem_data(bmem, &base64_data_ptr);
    assert(len <= *base64_len);
    *base64_len = len;
    (void)memcpy(base64_data, base64_data_ptr, len);
    BIO_free_all(b64);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
#else
    /* No base64 implementation on this platform! */
    assert(0);
    return ROBOTRACONTEURLITE_ERROR_NOT_IMPLEMENTED;
#endif
}

robotraconteurlite_status robotraconteurlite_tcp_socket_recv_nonblocking(ROBOTRACONTEURLITE_SOCKET sock,
                                                                         robotraconteurlite_byte* buffer,
                                                                         robotraconteurlite_size_t* pos,
                                                                         robotraconteurlite_size_t len, int* errno_out)
{
    robotraconteurlite_size_t pos1 = *pos;
    while ((*pos - pos1) < len)
    {
        ssize_t ret = recv(sock, &buffer[*pos], len - (*pos - pos1), MSG_DONTWAIT);
        if (ret < 0)
        {
            /* False positive cppcheck warning for errno not set */
            /* cppcheck-suppress misra-c2012-22.10 */
            if (errno == EWOULDBLOCK)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
            /* False positive cppcheck warning for errno not set */
            /* cppcheck-suppress misra-c2012-22.10 */
            *errno_out = errno;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        *pos += ret;
        if (ret == 0)
        {
            if (*pos == pos1)
            {
                return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
            }
            else
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
        }
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_socket_send_nonblocking(ROBOTRACONTEURLITE_SOCKET sock,
                                                                         const robotraconteurlite_byte* buffer,
                                                                         robotraconteurlite_size_t* pos,
                                                                         robotraconteurlite_size_t len, int* errno_out)
{
    robotraconteurlite_size_t pos1 = *pos;
    while ((*pos - pos1) < len)
    {
        ssize_t ret = send(sock, &buffer[*pos], len - (*pos - pos1), MSG_DONTWAIT);
        if (ret < 0)
        {
            /* False positive cppcheck warning for errno not set */
            /* cppcheck-suppress misra-c2012-22.10 */
            if (errno == EWOULDBLOCK)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
            /* False positive cppcheck warning for errno not set */
            /* cppcheck-suppress misra-c2012-22.10 */
            *errno_out = errno;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        *pos += ret;
        if (ret == 0)
        {
            if (*pos == pos1)
            {
                return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
            }
            else
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
        }
    }
    return 0;
}

robotraconteurlite_status robotraconteurlite_tcp_socket_begin_server(const struct sockaddr_storage* serv_addr,
                                                                     robotraconteurlite_size_t backlog,
                                                                     ROBOTRACONTEURLITE_SOCKET* sock_out,
                                                                     int* errno_out)
{
    /* Create socket */
    ROBOTRACONTEURLITE_SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    int flags = 0;
    /* cppcheck-suppress misra-c2012-10.4 */
    if (sock == 0)
    {
        *errno_out = errno;
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Make socket non-blocking */
    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    flags |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, flags) < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Bind socket */
    /* cppcheck-suppress [misra-c2012-11.2,misra-c2012-11.8] */
    if (bind(sock, (struct sockaddr*)serv_addr, sizeof(struct sockaddr)) < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Listen */
    if (listen(sock, (int)backlog) < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    *sock_out = sock;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_tcp_configure_socket(ROBOTRACONTEURLITE_SOCKET sock, int* errno_out)
{
    int flags = 0;
    /* Make socket non-blocking */
    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    flags |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, flags) < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Set TCP no delay*/
    flags = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flags, sizeof(int)) < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_socket_accept(ROBOTRACONTEURLITE_SOCKET acceptor_sock,
                                                               ROBOTRACONTEURLITE_SOCKET* client_sock, int* errno_out)
{
    /* Accept connection */
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(acceptor_sock, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        /* False positive cppcheck warning for errno not set */
        /* cppcheck-suppress misra-c2012-22.10 */
        if (errno == EWOULDBLOCK)
        {
            return ROBOTRACONTEURLITE_ERROR_RETRY;
        }
        /* False positive cppcheck warning for errno not set */
        /* cppcheck-suppress misra-c2012-22.10 */
        *errno_out = errno;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    *client_sock = newsockfd;

    return robotraconteurlite_tcp_configure_socket(newsockfd, errno_out);
}

robotraconteurlite_status robotraconteurlite_tcp_socket_close(ROBOTRACONTEURLITE_SOCKET sock)
{
    close(sock);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_u16 robotraconteurlite_ntohs(robotraconteurlite_u16 netshort) { return ntohs(netshort); }

robotraconteurlite_u16 robotraconteurlite_htons(robotraconteurlite_u16 hostshort) { return htons(hostshort); }

robotraconteurlite_u64 robotraconteurlite_be64toh(robotraconteurlite_u64 big_endian_64bits)
{
    /* Implement byte swap */
    robotraconteurlite_u64 ret = 0;
    robotraconteurlite_byte* ret_bytes = (robotraconteurlite_byte*)&ret;
    robotraconteurlite_byte* big_endian_bytes = (robotraconteurlite_byte*)&big_endian_64bits;
    ret_bytes[0] = big_endian_bytes[7];
    ret_bytes[1] = big_endian_bytes[6];
    ret_bytes[2] = big_endian_bytes[5];
    ret_bytes[3] = big_endian_bytes[4];
    ret_bytes[4] = big_endian_bytes[3];
    ret_bytes[5] = big_endian_bytes[2];
    ret_bytes[6] = big_endian_bytes[1];
    ret_bytes[7] = big_endian_bytes[0];
    return ret;
}

robotraconteurlite_status robotraconteurlite_tcp_socket_connect(struct robotraconteurlite_sockaddr_storage* addr,
                                                                ROBOTRACONTEURLITE_SOCKET* sock_out, int* errno_out)
{
    ROBOTRACONTEURLITE_SOCKET sock = 0;
    robotraconteurlite_status rv = -1;
    *errno_out = 0;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    rv = robotraconteurlite_tcp_configure_socket(sock, errno_out);
    if (FAILED(rv))
    {
        return rv;
    }

    /* cppcheck-suppress misra-c2012-11.3 */
    rv = connect(sock, (struct sockaddr*)addr, sizeof(struct sockaddr));
    if (FAILED(rv))
    {
        /* False positive cppcheck warning for errno not set */
        /* cppcheck-suppress misra-c2012-22.10 */
        if (errno == EINPROGRESS)
        {
            *sock_out = sock;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        *errno_out = errno;
        close(sock);
        return -1;
    }
    *sock_out = sock;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_poll_add_fd(ROBOTRACONTEURLITE_SOCKET sock, short extra_events,
                                                                struct robotraconteurlite_pollfd* pollfds,
                                                                robotraconteurlite_size_t* pollfd_count,
                                                                robotraconteurlite_size_t max_pollfds)
{
    int i = (int)*pollfd_count;
    if (i >= (int)max_pollfds)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_PARAMETER;
    }
    (*pollfd_count) = (*pollfd_count) + 1;

    pollfds[i].fd = sock;
    pollfds[i].events = extra_events;
    pollfds[i].revents = 0;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_acceptor_poll_add_fd(
    struct robotraconteurlite_connection_acceptor* acceptor, struct robotraconteurlite_connection* connection_head,
    struct robotraconteurlite_pollfd* pollfds, robotraconteurlite_size_t* pollfd_count,
    robotraconteurlite_size_t max_pollfds)
{
    short extra_events = 0;
    struct robotraconteurlite_connection* c = connection_head;
    while (c != NULL)
    {
        if (FLAGS_CHECK(c->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE))
        {
            extra_events = POLLIN;
            break;
        }
        c = c->next;
    }

    return robotraconteurlite_poll_add_fd(acceptor->sock, extra_events, pollfds, pollfd_count, max_pollfds);
}

robotraconteurlite_status robotraconteurlite_tcp_connection_poll_add_fd(
    struct robotraconteurlite_connection* connection, struct robotraconteurlite_pollfd* pollfds,
    robotraconteurlite_size_t* pollfd_count, robotraconteurlite_size_t max_pollfds)
{
    short extra_events = 0;
    /* cppcheck-suppress misra-c2012-10.4 */
    if ((FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE)) || (connection->sock == 0))
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (FLAGS_CHECK(connection->connection_state,
                    (ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED | ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING)))
    {
        extra_events |= POLLOUT;
    }

    if (FLAGS_CHECK(connection->connection_state,
                    (ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVING |
                     ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING)))
    {
        extra_events |= POLLIN;
    }

    return robotraconteurlite_poll_add_fd(connection->sock, extra_events, pollfds, pollfd_count, max_pollfds);
}

robotraconteurlite_status robotraconteurlite_tcp_connections_poll_add_fds(
    struct robotraconteurlite_connection* connection_head, struct robotraconteurlite_pollfd* pollfds,
    robotraconteurlite_size_t* pollfd_count, robotraconteurlite_size_t max_pollfds)
{
    struct robotraconteurlite_connection* c = connection_head;
    while (c != NULL)
    {
        robotraconteurlite_status rv =
            robotraconteurlite_tcp_connection_poll_add_fd(c, pollfds, pollfd_count, max_pollfds);
        if (FAILED(rv))
        {
            return rv;
        }
        c = c->next;
    }
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
