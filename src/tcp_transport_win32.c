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
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <winsock2.h>

/* Macros defined by win32 headers */
#undef FAILED
#undef SUCCEEDED

#define FLAGS_CHECK_ALL ROBOTRACONTEURLITE_FLAGS_CHECK_ALL
#define FLAGS_CHECK ROBOTRACONTEURLITE_FLAGS_CHECK
#define FLAGS_SET ROBOTRACONTEURLITE_FLAGS_SET
#define FLAGS_CLEAR ROBOTRACONTEURLITE_FLAGS_CLEAR

#define FAILED ROBOTRACONTEURLITE_FAILED

robotraconteurlite_status robotraconteurlite_tcp_sha1(const uint8_t* data, size_t len,
                                                      struct robotraconteurlite_tcp_sha1_storage* storage)
{
    /* Use Windows API to generate SHA1 */
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    DWORD cbHash = 0;
    DWORD dwFlags = CRYPT_VERIFYCONTEXT;
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, dwFlags))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
    {
        CryptReleaseContext(hProv, 0);
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    if (!CryptHashData(hHash, data, (DWORD)len, 0))
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    cbHash = sizeof(storage->sha1_bytes);
    if (!CryptGetHashParam(hHash, HP_HASHVAL, storage->sha1_bytes, &cbHash, 0))
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_base64_encode(const uint8_t* binary_data, size_t binary_len,
                                                               char* base64_data, size_t* base64_len)
{
    DWORD hashlen = 0;
    char base64_data2[29];
    size_t base64_len2 = sizeof(base64_data2);
    CryptBinaryToString(binary_data, (DWORD)binary_len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &hashlen);

    if (hashlen > base64_len2)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_PARAMETER;
    }

    if (!CryptBinaryToString(binary_data, (DWORD)binary_len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64_data2,
                             &hashlen))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    if (base64_data2[hashlen - 1] == '\0')
    {
        hashlen--;
    }

    if (hashlen > *base64_len)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_PARAMETER;
    }

    memcpy(base64_data, base64_data2, hashlen);

    *base64_len = hashlen;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_socket_recv_nonblocking(ROBOTRACONTEURLITE_SOCKET sock,
                                                                         uint8_t* buffer, size_t* pos, size_t len,
                                                                         int* errno_out)
{
    size_t pos1 = *pos;
    WSABUF wsaBuf;
    DWORD flags = 0;
    DWORD bytesReceived;

    if (sock == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    while (*pos < len)
    {
        wsaBuf.buf = &buffer[*pos];
        wsaBuf.len = len - *pos;
        int ret = WSARecv(sock, &wsaBuf, 1, &bytesReceived, &flags, NULL, NULL);
        if (ret == SOCKET_ERROR)
        {
            int wsaError = WSAGetLastError();
            if (wsaError == WSAEWOULDBLOCK)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
            *errno_out = wsaError;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        *pos += bytesReceived;
        if (bytesReceived == 0)
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
                                                                         const uint8_t* buffer, size_t* pos, size_t len,
                                                                         int* errno_out)
{
    size_t pos1 = *pos;
    WSABUF wsaBuf;
    DWORD bytesSent;
    DWORD flags = 0;

    if (sock == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    while (*pos < len)
    {
        wsaBuf.buf = &buffer[*pos];
        wsaBuf.len = len - *pos;
        int ret = WSASend(sock, &wsaBuf, 1, &bytesSent, flags, NULL, NULL);
        if (ret == SOCKET_ERROR)
        {
            int wsaError = WSAGetLastError();
            if (wsaError == WSAEWOULDBLOCK)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
            *errno_out = wsaError;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        *pos += bytesSent;
        if (bytesSent == 0)
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

robotraconteurlite_status robotraconteurlite_tcp_socket_begin_server(const struct sockaddr_storage* serv_addr,
                                                                     size_t backlog,
                                                                     ROBOTRACONTEURLITE_SOCKET* sock_out,
                                                                     int* errno_out)
{
    /* Create socket */
    SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
    u_long mode = 1;
    if (sock == INVALID_SOCKET)
    {
        *errno_out = WSAGetLastError();
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Make socket non-blocking */
    if (ioctlsocket(sock, FIONBIO, &mode) != NO_ERROR)
    {
        *errno_out = WSAGetLastError();
        closesocket(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Bind socket */
    if (bind(sock, (struct sockaddr*)serv_addr, sizeof(struct sockaddr_storage)) == SOCKET_ERROR)
    {
        *errno_out = WSAGetLastError();
        closesocket(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Listen */
    if (listen(sock, (int)backlog) == SOCKET_ERROR)
    {
        *errno_out = WSAGetLastError();
        closesocket(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    *sock_out = sock;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_tcp_configure_socket(ROBOTRACONTEURLITE_SOCKET sock, int* errno_out)
{
    int flags = 1;
    u_long mode = 1;

    if (sock == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Make socket non-blocking */
    if (ioctlsocket(sock, FIONBIO, &mode) != NO_ERROR)
    {
        *errno_out = WSAGetLastError();
        closesocket(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Set TCP no delay */
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flags, sizeof(int)) == SOCKET_ERROR)
    {
        *errno_out = WSAGetLastError();
        closesocket(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_socket_accept(ROBOTRACONTEURLITE_SOCKET acceptor_sock,
                                                               ROBOTRACONTEURLITE_SOCKET* client_sock, int* errno_out)
{
    /* Accept connection */
    struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);
    *errno_out = 0;
    SOCKET newsockfd = WSAAccept(acceptor_sock, (struct sockaddr*)&cli_addr, &clilen, NULL, NULL);
    if (newsockfd == INVALID_SOCKET)
    {
        int wsaError = WSAGetLastError();
        if (wsaError == WSAEWOULDBLOCK)
        {
            return ROBOTRACONTEURLITE_ERROR_RETRY;
        }
        *errno_out = wsaError;
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    *client_sock = newsockfd;

    return robotraconteurlite_tcp_configure_socket(newsockfd, errno_out);
}

robotraconteurlite_status robotraconteurlite_tcp_socket_close(ROBOTRACONTEURLITE_SOCKET sock)
{
    if (sock == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    closesocket(sock);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

uint16_t robotraconteurlite_ntohs(uint16_t netshort) { return ntohs(netshort); }

uint16_t robotraconteurlite_htons(uint16_t hostshort) { return htons(hostshort); }

uint64_t robotraconteurlite_be64toh(uint64_t big_endian_64bits)
{
    /* Implement byte swap */
    uint64_t ret = 0;
    uint8_t* ret_bytes = (uint8_t*)&ret;
    uint8_t* big_endian_bytes = (uint8_t*)&big_endian_64bits;
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
    ROBOTRACONTEURLITE_SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    *errno_out = 0;
    robotraconteurlite_status rv = robotraconteurlite_tcp_configure_socket(sock, &errno_out);
    if (FAILED(rv))
    {
        return rv;
    }

    if (connect(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_storage)) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
        {
            *sock_out = sock;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        closesocket(sock);
        *errno_out = WSAGetLastError();
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    *sock_out = sock;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_poll_add_fd(ROBOTRACONTEURLITE_SOCKET sock, short extra_events,
                                                                struct robotraconteurlite_pollfd* pollfds,
                                                                size_t* pollfd_count, size_t max_pollfds)
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
    struct robotraconteurlite_pollfd* pollfds, size_t* pollfd_count, size_t max_pollfds)
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
    struct robotraconteurlite_connection* connection, struct robotraconteurlite_pollfd* pollfds, size_t* pollfd_count,
    size_t max_pollfds)
{
    short extra_events = 0;
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
    size_t* pollfd_count, size_t max_pollfds)
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
