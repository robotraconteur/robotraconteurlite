#include "robotraconteurlite/tcp_transport.h"

#include <stdlib.h>
#include <string.h>

/* Linux socket includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <endian.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#ifdef ROBOTRACONTEURLITE_USE_OPENSSL
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
 #include <openssl/evp.h>
#endif

int robotraconteurlite_tcp_sha1(const uint8_t* data, size_t len, struct robotraconteurlite_tcp_sha1_storage* storage)
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

int robotraconteurlite_tcp_base64_encode(const uint8_t* binary_data, size_t binary_len, char* base64_data, size_t* base64_len)
{
#ifdef ROBOTRACONTEURLITE_USE_OPENSSL
    /* Use OpenSSL base64 implementation */
    size_t len;
    BIO* bmem = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, binary_data, binary_len);
    BIO_flush(b64);
    len = BIO_get_mem_data(bmem, &base64_data);
    assert(len <= *base64_len);
    *base64_len = len;
    BIO_free_all(b64);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
#else
    /* No base64 implementation on this platform! */
    assert(0);
    return ROBOTRACONTEURLITE_ERROR_NOT_IMPLEMENTED;
#endif
}

int robotraconteurlite_tcp_socket_recv_nonblocking(int sock, uint8_t* buffer, uint32_t* pos, size_t len, int* errno_out)
{
    ssize_t ret = recv(sock, buffer + *pos, len - *pos, MSG_DONTWAIT);
    if (ret < 0)
    {
        if (errno == EWOULDBLOCK)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        *errno_out = errno;
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    *pos += ret;
    return 0;
}

int robotraconteurlite_tcp_socket_send_nonblocking(int sock, const uint8_t* buffer, uint32_t* pos, size_t len, int* errno_out)
{
    ssize_t ret = send(sock, buffer + *pos, len - *pos, MSG_DONTWAIT);
    if (ret < 0)
    {
        if (errno == EWOULDBLOCK)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        *errno_out = errno;
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    *pos += ret;
    return 0;
}

int robotraconteurlite_tcp_socket_begin_server(const struct sockaddr_storage* serv_addr, size_t backlog, int* sock_out, int* errno_out)
{
    /* Create socket */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int flags;
    if (sock < 0)
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
    if (bind(sock, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_storage)) < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Listen */
    if (listen(sock, backlog) < 0)
    {
        *errno_out = errno;
        close(sock);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    *sock_out = sock;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_tcp_socket_accept(int acceptor_sock, int* client_sock, int* errno_out)
{
    /* Accept connection */
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int flags;
    int newsockfd = accept(acceptor_sock, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
        if (errno == EWOULDBLOCK)
        {
            return ROBOTRACONTEURLITE_ERROR_RETRY;
        }
        *errno_out = errno;        
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Make socket non-blocking */
    flags = fcntl(newsockfd, F_GETFL, 0);
    if (flags < 0)
    {
        *errno_out = errno;
        close(newsockfd);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    flags |= O_NONBLOCK;
    if (fcntl(newsockfd, F_SETFL, flags) < 0)
    {
        *errno_out = errno;
        close(newsockfd);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}


int robotraconteurlite_tcp_socket_close(int sock)
{
    close(sock);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

ROBOTRACONTEURLITE_DECL uint16_t robotraconteurlite_ntohs(uint16_t netshort)
{
    return ntohs(netshort);
}

uint16_t robotraconteurlite_htons(uint16_t hostshort)
{
    return htons(hostshort);
}

ROBOTRACONTEURLITE_DECL uint64_t robotraconteurlite_be64toh(uint64_t big_endian_64bits)
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
