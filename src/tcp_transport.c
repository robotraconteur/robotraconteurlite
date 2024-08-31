#include "robotraconteurlite/tcp_transport.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* memcmp is used between uint8_t* buffer and const char literals in this function
   The char null terminator is not used for this comparison, and the comparison between
   char and uint8_t is safe. Disabling misra warnings for this file.
*/
/* cppcheck-suppress-file [misra-c2012-21.14, misra-c2012-21.16] */

robotraconteurlite_status robotraconteurlite_tcp_acceptor_listen(
    struct robotraconteurlite_connection_acceptor* acceptor, const struct sockaddr_storage* serv_addr, int backlog)
{

    int sock = 0;
    int last_errno = -1;
    robotraconteurlite_status rv = -1;

    assert(sizeof(struct robotraconteurlite_tcp_transport_storage) <=
           sizeof(struct robotraconteurlite_transport_storage));

    rv = robotraconteurlite_tcp_socket_begin_server(serv_addr, backlog, &acceptor->sock, &last_errno);
    return rv;
}

robotraconteurlite_status robotraconteurlite_tcp_acceptor_close(struct robotraconteurlite_connection_acceptor* acceptor)
{
    if (acceptor->sock >= 0)
    {
        (void)robotraconteurlite_tcp_socket_close(acceptor->sock);
        acceptor->sock = -1;
    }

    return 0;
}

robotraconteurlite_status robotraconteurlite_tcp_acceptor_communicate(
    struct robotraconteurlite_connection_acceptor* acceptor, struct robotraconteurlite_connection* connection_head,
    robotraconteurlite_timespec now)
{
    /* Find a connection that is idle */
    struct robotraconteurlite_connection* c = connection_head;
    int errno_out = -1;
    robotraconteurlite_status rv = -1;
    while (c != NULL)
    {
        if ((c->transport_type == ROBOTRACONTEURLITE_TCP_TRANSPORT) &&
            ((c->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) != 0U) &&
            ((c->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE) != 0U))
        {
            break;
        }
        c = c->next;
    }

    if (c == NULL)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Accept connection */
    rv = robotraconteurlite_tcp_socket_accept(acceptor->sock, &c->sock, &errno_out);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (rv == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        return rv;
    }

    c->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
    c->last_recv_message_time = now;
    c->last_send_message_time = now;
    (void)memset(&c->transport_storage, 0, sizeof(c->transport_storage));

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

void robotraconteurlite_tcp_connection_init_connection_server(struct robotraconteurlite_connection* connection)
{
    connection->transport_type = ROBOTRACONTEURLITE_TCP_TRANSPORT;
    connection->sock = -1;
    connection->config_flags |= ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER;
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE;
    connection->heartbeat_period_ms = 5000;
    connection->heartbeat_timeout_ms = 15000;
}

void robotraconteurlite_tcp_connection_init_connections_server(struct robotraconteurlite_connection* connections_head)
{
    struct robotraconteurlite_connection* c = connections_head;
    while (c != NULL)
    {
        robotraconteurlite_tcp_connection_init_connection_server(c);
        c = c->next;
    }
}

static size_t robotraconteurlite_tcp_connection_recv_websocket_header_size(uint8_t byte_two)
{
    size_t websocket_header_len = 2;
    uint8_t len1 = byte_two & 0x7FU;
    uint8_t masked = byte_two & 0x80U;
    if (len1 == 126U)
    {
        websocket_header_len += 2U;
    }
    if (len1 == 127U)
    {
        websocket_header_len += 8U;
    }

    if (masked != 0U)
    {
        websocket_header_len += 4U;
    }

    return websocket_header_len;
}

static robotraconteurlite_status robotraconteurlite_tcp_connection_buffer_recv_websocket(
    struct robotraconteurlite_connection* connection, size_t len)
{
    struct robotraconteurlite_tcp_transport_storage* storage =
        (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if ((storage->recv_websocket_frame_pos == storage->recv_websocket_frame_len) &&
        ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME) != 0U))
    {
        /* reset for next websocket frame */
        storage->recv_websocket_frame_pos = 0;
        storage->recv_websocket_frame_len = 0;
        storage->recv_websocket_header_pos = 0;
        storage->tcp_transport_state &= ~(ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME);
    }

    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME) == 0U)
    {
        size_t websocket_header_len = 2;
        uint8_t len1 = 0;
        uint8_t masked = 0;

        if (storage->recv_websocket_header_pos < 2U)
        {
            int last_errno = -1;
            robotraconteurlite_status rv =
                robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, storage->recv_websocket_header_buffer,
                                                               &storage->recv_websocket_header_pos, 2, &last_errno);
            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
                return rv;
            }

            if (storage->recv_websocket_header_pos < 2U)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
        }

        websocket_header_len =
            robotraconteurlite_tcp_connection_recv_websocket_header_size(storage->recv_websocket_header_buffer[1]);

        if (storage->recv_websocket_header_pos < websocket_header_len)
        {
            int last_errno = -1;
            robotraconteurlite_status rv = robotraconteurlite_tcp_socket_recv_nonblocking(
                connection->sock, storage->recv_websocket_header_buffer, &storage->recv_websocket_header_pos,
                websocket_header_len, &last_errno);
            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
                return rv;
            }

            if (storage->recv_websocket_header_pos < websocket_header_len)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
        }

        len1 = storage->recv_websocket_header_buffer[1] & 0x7FU;
        masked = storage->recv_websocket_header_buffer[1] & 0x80U;
        if (len1 == 126U)
        {
            storage->recv_websocket_frame_len =
                robotraconteurlite_ntohs(*(uint16_t*)&storage->recv_websocket_header_buffer[2]);
        }
        else if (len1 == 127U)
        {
            storage->recv_websocket_frame_len =
                robotraconteurlite_be64toh(*(uint64_t*)&storage->recv_websocket_header_buffer[2]);
        }
        else
        {
            storage->recv_websocket_frame_len = len1;
        }

        if (masked != 0U)
        {
            (void)memcpy(storage->recv_websocket_mask,
                         storage->recv_websocket_header_buffer + websocket_header_len - 4U, 4U);
            storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_ENABLE_MASK;
        }
        else
        {
            storage->tcp_transport_state &= ~ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_ENABLE_MASK;
        }

        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME;
    }
    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME) != 0U)
    {
        uint32_t recv_len =
            storage->recv_websocket_frame_len + (connection->recv_buffer_pos - storage->recv_websocket_frame_pos);
        int last_errno = -1;
        uint32_t prev_recv_buffer_pos = 0;
        robotraconteurlite_status rv = -1;
        size_t n = 0;
        size_t i = 0;
        if (recv_len > len)
        {
            recv_len = (uint32_t)len;
        }

        prev_recv_buffer_pos = connection->recv_buffer_pos;
        rv = robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, connection->recv_buffer,
                                                            &connection->recv_buffer_pos, recv_len, &last_errno);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return rv;
        }
        n = connection->recv_buffer_pos - prev_recv_buffer_pos;
        for (i = 0; i < n; i++)
        {
            connection->recv_buffer[prev_recv_buffer_pos + i] ^=
                storage->recv_websocket_mask[(storage->recv_websocket_frame_pos + i) % 4U];
        }
        storage->recv_websocket_frame_pos += n;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_connection_buffer_recv(
    struct robotraconteurlite_connection* connection, size_t len)
{
    int last_errno = -1;
    struct robotraconteurlite_tcp_transport_storage* storage =
        (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) != 0U)
    {
        return robotraconteurlite_tcp_connection_buffer_recv_websocket(connection, len);
    }

    return robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, connection->recv_buffer,
                                                          &connection->recv_buffer_pos, len, &last_errno);
}

robotraconteurlite_status robotraconteurlite_tcp_connection_communicate_recv(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now)
{
    /* If the connection is in an error state, return error */
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR) != 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* If the message has been consumed, move the receive buffer to the beginning */
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED) != 0U)
    {
        if (connection->recv_buffer_pos > connection->recv_message_len)
        {
            (void)memmove(connection->recv_buffer, connection->recv_buffer + connection->recv_message_len,
                          connection->recv_buffer_pos - connection->recv_message_len);
        }
        connection->recv_buffer_pos -= connection->recv_message_len;
        connection->recv_message_len = 0;
        connection->connection_state &=
            ~(ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED | ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED);
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
    }

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED) != 0U)
    {

        /* Receive data */
        size_t recv_op_len =
            (connection->recv_message_len == 0U) ? 64U : (connection->recv_buffer_len - connection->recv_buffer_pos);

        /* Receive data */
        robotraconteurlite_status rv = robotraconteurlite_tcp_connection_buffer_recv(connection, recv_op_len);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
            return rv;
        }

        /* If this is the first receive, parse the message length */

        if (robotraconteurlite_connection_verify_preamble(connection, &connection->recv_message_len) !=
            ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }

        /* If we have received the entire message, set the message received flag */
        if ((connection->recv_message_len > 0U) && (connection->recv_buffer_pos >= connection->recv_message_len))
        {
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED;
            connection->last_recv_message_time = now;
        }

        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    else
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
}

static robotraconteurlite_status robotraconteurlite_tcp_websocket_random_mask(
    struct robotraconteurlite_connection* connection, uint8_t mask[4])
{
    /* TODO: use better random source */
    uint32_t random_val = rand();
    ROBOTRACONTEURLITE_UNUSED(connection);
    mask[0] = (random_val >> 24) & 0xFFU;
    mask[1] = (random_val >> 16) & 0xFFU;
    mask[2] = (random_val >> 8) & 0xFFU;
    mask[3] = random_val & 0xFFU;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_tcp_connection_buffer_send_websocket(
    struct robotraconteurlite_connection* connection, size_t len)
{
    struct robotraconteurlite_tcp_transport_storage* storage =
        (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    size_t send_len = len - connection->send_buffer_pos;
    int last_errno = -1;
    robotraconteurlite_status rv = -1;
    if ((storage->send_websocket_frame_len == 0U) && (send_len > 0U))
    {
        /* Prepare new frame */
        if (send_len >= UINT16_MAX)
        {
            send_len = UINT16_MAX;
        }

        storage->send_websocket_header_len = 2;
        storage->send_websocket_header_buffer[0] = 0x80U | ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_BINARY;
        if (len > 125U)
        {
            storage->send_websocket_header_len += 2U;
            *(uint16_t*)&storage->send_websocket_header_buffer[2] = robotraconteurlite_htons(len);
            storage->send_websocket_header_buffer[1] = 126;
        }
        else
        {
            storage->send_websocket_header_buffer[1] = (uint8_t)len;
        }

        /* Create mask if client */
        if ((connection->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) == 0U)
        {
            storage->send_websocket_header_len += 4U;
            storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_SEND_WEBSOCKET_ENABLE_MASK;
            if (robotraconteurlite_tcp_websocket_random_mask(connection, storage->send_websocket_mask) != 0)
            {
                return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
            }
            storage->send_websocket_header_buffer[1] |= 0x80U;
            (void)memcpy(storage->send_websocket_header_buffer + storage->send_websocket_header_len - 4U,
                         storage->send_websocket_mask, 4);
        }

        storage->send_websocket_frame_len = send_len;
        storage->send_websocket_frame_buffer_end = send_len + connection->send_buffer_pos;
        storage->send_websocket_frame_buffer_pos = connection->send_buffer_pos;

        /* Apply mask to send buffer */
        if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_SEND_WEBSOCKET_ENABLE_MASK) != 0U)
        {
            size_t i = 0;
            for (i = 0; i < send_len; i++)
            {
                connection->send_buffer[connection->send_buffer_pos + i] ^= storage->send_websocket_mask[i % 4U];
            }
        }
    }

    /* Send header nonblocking */
    if ((storage->send_websocket_header_len > 0U) &&
        (storage->send_websocket_header_pos < storage->send_websocket_header_len))
    {
        int last_errno = -1;
        robotraconteurlite_status rv = robotraconteurlite_tcp_socket_send_nonblocking(
            connection->sock, storage->send_websocket_header_buffer, &storage->send_websocket_header_pos,
            storage->send_websocket_header_len, &last_errno);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            return rv;
        }
        if (storage->send_websocket_header_pos < storage->send_websocket_header_len)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
    }

    /* Send data nonblocking */
    if (len < storage->send_websocket_frame_len)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    rv = robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer,
                                                        &storage->send_websocket_frame_buffer_pos,
                                                        storage->send_websocket_frame_buffer_end, &last_errno);

    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return rv;
    }

    if (storage->send_websocket_frame_buffer_pos < storage->send_websocket_frame_buffer_end)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Increment buffer position */
    connection->send_buffer_pos = storage->send_websocket_frame_buffer_end;

    /* Reset frame */
    storage->send_websocket_frame_len = 0;
    storage->send_websocket_header_len = 0;
    storage->send_websocket_header_pos = 0;
    storage->tcp_transport_state &= ~ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_SEND_WEBSOCKET_ENABLE_MASK;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_connection_buffer_send(
    struct robotraconteurlite_connection* connection, size_t len)
{
    int last_errno = -1;
    struct robotraconteurlite_tcp_transport_storage* storage =
        (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) != 0U)
    {
        return robotraconteurlite_tcp_connection_buffer_send_websocket(connection, len);
    }

    return robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer,
                                                          &connection->send_buffer_pos, len, &last_errno);
}

robotraconteurlite_status robotraconteurlite_tcp_connection_communicate_send(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now)
{
    /* If the connection is in an error state, return error */
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR) != 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED) != 0U)
    {
        connection->connection_state &=
            ~(ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED | ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT);
        connection->send_buffer_pos = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED) != 0U)
    {
        robotraconteurlite_status rv = -1;
        /* Clear send requested flag */
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED;

        /* Send data */
        rv = robotraconteurlite_tcp_connection_buffer_send(connection, connection->send_message_len);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return rv;
        }

        /* If we have not sent the entire message, set the message sending flag */
        if ((connection->send_message_len > 0U) && (connection->send_buffer_pos < connection->send_message_len))
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING;
        }
        else
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT;
            connection->last_send_message_time = now;
        }
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    else if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING) != 0U)
    {
        /* Send data */
        size_t send_op_len =
            (connection->send_message_len == 0U) ? 64U : (connection->send_message_len - connection->send_buffer_pos);
        robotraconteurlite_status rv = robotraconteurlite_tcp_connection_buffer_send(connection, send_op_len);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return rv;
        }

        /* If we have sent the entire message, set the message sent flag */
        if ((connection->send_message_len > 0U) && (connection->send_buffer_pos >= connection->send_message_len))
        {
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING;
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT;
            connection->last_send_message_time = now;
        }
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    else
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
}

static robotraconteurlite_status robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(
    const char* recv_data, size_t recv_data_len, size_t* i_end, size_t* i_next)
{
    size_t i = *i_next;
    while ((i < recv_data_len) && (recv_data[i] != '\n') && (recv_data[i] != '\r'))
    {
        i++;
    }

    if (i >= recv_data_len)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    *i_end = i;
    while ((i < recv_data_len) && ((recv_data[i] == '\n') || (recv_data[i] == '\r')))
    {
        i++;
    }
    *i_next = i;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

#define STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN 18U
#define STRCONST_HTTP_SEC_WEBSOCKET_KEY "Sec-WebSocket-Key:"
#define STRCONST_HTTP_KEY_UUID_LEN 36U
#define STRCONST_HTTP_KEY_UUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WEBSOCKET_KEY_BASE64_LEN 24U
#define STRCONST_HTTP_RESPONSE_1                                                                                       \
    "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Protocol: "        \
    "robotraconteur.robotraconteur.com\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Accept: "
#define STRCONST_HTTP_RESPONSE_1_LEN 183U
#define STRCONST_HTTP_RESPONSE_2 "\r\n\r\n"
#define STRCONST_HTTP_RESPONSE_2_LEN 4U

static robotraconteurlite_status robotraconteurlite_tcp_connection_handshake_http_handshake(
    struct robotraconteurlite_connection* connection)
{
    /* cppcheck-suppress-begin [misra-c2012-21.14, misra-c2012-21.16] */
    const char* recv_data = (const char*)connection->recv_buffer;
    size_t recv_data_len = connection->recv_buffer_pos;

    /* Skip first line of receive data */
    size_t i_end = 0;
    size_t i_next = 0;
    size_t i_sec_websocket = 0;
    size_t i_sec_websocket_end = 0;
    struct robotraconteurlite_tcp_sha1_storage sha1_storage;
    uint8_t sha1_input_bytes[60];
    char sha1_base64[28];
    size_t sha1_base64_len = sizeof(sha1_base64);
    int last_errno = -1;
    robotraconteurlite_status rv = robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(
        recv_data, recv_data_len, &i_end, &i_next);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return rv;
    }

    /* Find the websocket sec key line */
    while (i_next < recv_data_len)
    {
        if (recv_data[i_next] == 'S')
        {
            if (((i_next + STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN) < recv_data_len) &&
                /* cppcheck-suppress [misra-c2012-21.14, misra-c2012-21.16] */
                (memcmp(recv_data + i_next, STRCONST_HTTP_SEC_WEBSOCKET_KEY, STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN) == 0))
            {
                i_sec_websocket = i_next + STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN;
                break;
            }
        }
        rv = robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(recv_data, recv_data_len, &i_end,
                                                                                       &i_next);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            return rv;
        }
    }

    if (i_sec_websocket == 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Find end of sec key line */
    rv = robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(recv_data, recv_data_len, &i_end,
                                                                                   &i_next);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return rv;
    }

    /* Skip any whitespace */
    while ((i_sec_websocket < i_end) && ((recv_data[i_sec_websocket] == ' ') || (recv_data[i_sec_websocket] == '\t')))
    {
        i_sec_websocket++;
    }

    /* Find end of sec key */
    i_sec_websocket_end = i_sec_websocket;
    while ((i_sec_websocket_end < i_end) && (recv_data[i_sec_websocket_end] != ' ') &&
           (recv_data[i_sec_websocket_end] != '\t'))
    {
        i_sec_websocket_end++;
    }

    /* Check the client key length */
    if ((i_sec_websocket_end - i_sec_websocket) != WEBSOCKET_KEY_BASE64_LEN)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Generate the return key */

    /* NOLINTBEGIN(bugprone-not-null-terminated-result) */
    /* Copy the client key to the buffer */
    (void)memcpy(sha1_input_bytes, recv_data + i_sec_websocket, WEBSOCKET_KEY_BASE64_LEN);
    /* Copy the UUID to the buffer */
    (void)memcpy(sha1_input_bytes + WEBSOCKET_KEY_BASE64_LEN, STRCONST_HTTP_KEY_UUID, STRCONST_HTTP_KEY_UUID_LEN);
    /* NOLINTEND(bugprone-not-null-terminated-result) */

    /* Compute the SHA1 hash */
    if (robotraconteurlite_tcp_sha1(sha1_input_bytes, sizeof(sha1_input_bytes), &sha1_storage) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    /* Convert the SHA1 hash to base64 */
    if (robotraconteurlite_tcp_base64_encode(sha1_storage.sha1_bytes, sizeof(sha1_storage.sha1_bytes), sha1_base64,
                                             &sha1_base64_len) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    assert(sha1_base64_len == sizeof(sha1_base64));

    connection->recv_buffer_pos = 0;
    /* Send the response using send_buffer */
    connection->send_buffer_pos = 0;

    /* Send the first part of the response */
    /* NOLINTBEGIN(bugprone-not-null-terminated-result) */
    (void)memcpy(connection->send_buffer, STRCONST_HTTP_RESPONSE_1, STRCONST_HTTP_RESPONSE_1_LEN);
    /* Send the accept hash */
    (void)memcpy(connection->send_buffer + STRCONST_HTTP_RESPONSE_1_LEN, sha1_base64, sha1_base64_len);
    /* Send the second part of the response */
    (void)memcpy(connection->send_buffer + STRCONST_HTTP_RESPONSE_1_LEN + sha1_base64_len, STRCONST_HTTP_RESPONSE_2,
                 STRCONST_HTTP_RESPONSE_2_LEN);
    /* NOLINTEND(bugprone-not-null-terminated-result) */

    /* Set the send message length */
    connection->send_message_len = STRCONST_HTTP_RESPONSE_1_LEN + sha1_base64_len + STRCONST_HTTP_RESPONSE_2_LEN;
    assert(connection->send_message_len <= connection->send_buffer_len);

    /* Send the response */
    rv = robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer,
                                                        &connection->send_buffer_pos, connection->send_message_len,
                                                        &last_errno);
    return rv;
}

static robotraconteurlite_status robotraconteurlite_tcp_connection_handshake_http_recv_header(
    struct robotraconteurlite_connection* connection)
{
    int newline_found = 0;
    robotraconteurlite_status rv = -1;
    /* Receive byte by byte until two newlines character */
    while (connection->recv_buffer_pos < connection->recv_buffer_len)
    {
        int last_errno = -1;
        size_t last_recv_buffer_pos = connection->recv_buffer_pos;
        rv = robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, connection->recv_buffer,
                                                            &connection->recv_buffer_pos,
                                                            connection->recv_buffer_pos + 1U, &last_errno);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return rv;
        }
        if (connection->recv_buffer_pos == last_recv_buffer_pos)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }
        /* Check for ending of \r\n\r\n or \n\n or \r\r or \n\r\n\r */
        if (connection->recv_buffer_pos >= 4U)
        {
            uint8_t* end_minus_4 = connection->recv_buffer + connection->recv_buffer_pos - 4U;
            /* cppcheck-suppress [misra-c2012-21.14, misra-c2012-21.16] */
            if ((memcmp(end_minus_4, "\r\n\r\n", 4) == 0) || (memcmp(end_minus_4, "\n\r\n\r", 4) == 0))
            {
                newline_found = 1;
            }
        }

        if (connection->recv_buffer_pos >= 2U)
        {
            uint8_t* end_minus_2 = connection->recv_buffer + connection->recv_buffer_pos - 2U;
            /* cppcheck-suppress [misra-c2012-21.14, misra-c2012-21.16] */
            if ((memcmp(end_minus_2, "\n\n", 2) == 0) || (memcmp(end_minus_2, "\r\r", 2) == 0))
            {
                newline_found = 1;
            }
        }

        if (newline_found != 0)
        {
            break;
        }
    }

    /* Check that two newlines were found */
    if (!newline_found)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_tcp_connection_handshake_server(
    struct robotraconteurlite_connection* connection)
{
    const char rrac_magic[4] = {'R', 'R', 'A', 'C'};

    struct robotraconteurlite_tcp_transport_storage* storage =
        (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    robotraconteurlite_status rv = -1;

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING) == 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (connection->send_message_len > 0U)
    {
        int last_errno = -1;
        robotraconteurlite_status rv = robotraconteurlite_tcp_socket_send_nonblocking(
            connection->sock, connection->send_buffer, &connection->send_buffer_pos, connection->send_message_len,
            &last_errno);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            if (rv == ROBOTRACONTEURLITE_ERROR_RETRY)
            {
                return ROBOTRACONTEURLITE_ERROR_RETRY;
            }
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return rv;
        }
        if (connection->send_buffer_pos < connection->send_message_len)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }

        /* We are now connected using websocket! */
        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
        connection->connection_state |=
            ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
        /* Clear send buffer */
        connection->send_buffer_pos = 0;
        connection->send_message_len = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (connection->recv_buffer_pos < 4U)
    {
        int last_errno = -1;
        robotraconteurlite_status rv = robotraconteurlite_tcp_socket_recv_nonblocking(
            connection->sock, connection->recv_buffer, &connection->recv_buffer_pos, 4, &last_errno);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return rv;
        }
        if (connection->recv_buffer_pos < 4U)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }
    }

    /* Check if first 4 bytes are RRAC */
    if (memcmp(connection->recv_buffer, rrac_magic, sizeof(rrac_magic)) == 0)
    {
        /* We are connected with a non-websocket client */
        storage->tcp_transport_state &= ~ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
        connection->connection_state |=
            ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Check if first 4 bytes are GET */
    /* cppcheck-suppress [misra-c2012-21.14, misra-c2012-21.16] */
    if (memcmp(connection->recv_buffer, "GET ", 4) == 0)
    {
        /* We are connected with a websocket client, begin handshake */
        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
    }
    else
    {
        /* Unknown header */
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    rv = robotraconteurlite_tcp_connection_handshake_http_recv_header(connection);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (rv == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return ROBOTRACONTEURLITE_ERROR_RETRY;
        }
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return rv;
    }

    /* Handshake */
    rv = robotraconteurlite_tcp_connection_handshake_http_handshake(connection);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return rv;
    }

    /* Check if the response was sent */
    if ((connection->send_message_len > 0U) && (connection->send_buffer_pos == connection->send_message_len))
    {
        /* We are connected using websockets! */
        connection->connection_state |=
            ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
        /* Clear send buffer */
        connection->send_buffer_pos = 0;
        connection->send_message_len = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    return ROBOTRACONTEURLITE_ERROR_CONSUMED;
}

static robotraconteurlite_status robotraconteurlite_tcp_connection_handshake_client(
    struct robotraconteurlite_connection* connection)
{
    struct robotraconteurlite_tcp_transport_storage* storage =
        (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    int newline_found = 0;
    robotraconteurlite_status rv = -1;

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING) == 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) == 0U)
    {
        /* No handshake for raw sockets */
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Send data left in buffer */
    if (connection->send_message_len > 0U)
    {
        int last_errno = -1;
        robotraconteurlite_status rv = robotraconteurlite_tcp_socket_send_nonblocking(
            connection->sock, connection->send_buffer, &connection->send_buffer_pos, connection->send_message_len,
            &last_errno);
        if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return rv;
        }
        if (connection->send_buffer_pos < connection->send_message_len)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }
    }

    rv = robotraconteurlite_tcp_connection_handshake_http_recv_header(connection);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if ((rv == ROBOTRACONTEURLITE_ERROR_RETRY) || (rv == ROBOTRACONTEURLITE_ERROR_CONSUMED))
        {
            return rv;
        }
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return rv;
    }

    /* Make sure we actually upgraded */
    /* cppcheck-suppress [misra-c2012-21.14, misra-c2012-21.16] */
    if (memcmp(connection->recv_buffer, "HTTP/1.1 101", 12) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* TODO: We should check more of the HTTP response, but just ignore for now and begin using the connection! */

    /* We are connected using websockets! */
    connection->connection_state |=
        ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
    connection->connection_state &=
        ~(ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING | ROBOTRACONTEURLITE_STATUS_FLAGS_BLOCK_SEND |
          ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING | ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED);
    storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
    storage->tcp_transport_state &= ~ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IN_HTTP_HEADER;
    /* Clear send buffer */
    connection->send_buffer_pos = 0;
    connection->send_message_len = 0;
    connection->recv_buffer_pos = 0;
    connection->recv_message_len = 0;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_connection_handshake(struct robotraconteurlite_connection* connection)
{
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING) == 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((connection->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) != 0U)
    {
        return robotraconteurlite_tcp_connection_handshake_server(connection);
    }
    else
    {
        return robotraconteurlite_tcp_connection_handshake_client(connection);
    }
}

robotraconteurlite_status robotraconteurlite_tcp_connection_communicate(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now)
{
    robotraconteurlite_status rv = -1;
    if ((connection->transport_type != ROBOTRACONTEURLITE_TCP_TRANSPORT))
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED) != 0U)
    {
        connection->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE;
        if (robotraconteurlite_connection_reset(connection) != 0)
        {
            return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
        }
    }

    if ((connection->connection_state &
         (ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED | ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE)) != 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((connection->connection_state &
         (ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED | ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSING)) != 0U)
    {
        /* TODO: graceful shutdown */
        (void)robotraconteurlite_tcp_socket_close(connection->sock);
        connection->sock = -1;
        connection->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Do handshake */
    rv = robotraconteurlite_tcp_connection_handshake(connection);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (rv == ROBOTRACONTEURLITE_ERROR_CONSUMED)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        return rv;
    }

    rv = robotraconteurlite_tcp_connection_communicate_recv(connection, now);
    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return rv;
    }

    return robotraconteurlite_tcp_connection_communicate_send(connection, now);
}

robotraconteurlite_status robotraconteurlite_tcp_connections_communicate(
    struct robotraconteurlite_connection* connections_head, robotraconteurlite_timespec now)
{
    struct robotraconteurlite_connection* c = connections_head;
    while (c != NULL)
    {
        /* TODO: pass failure warning back to caller */
        (void)robotraconteurlite_tcp_connection_communicate(c, now);
        c = c->next;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

void robotraconteurlite_tcp_connection_close(struct robotraconteurlite_connection* connection)
{
    if (connection->transport_type != ROBOTRACONTEURLITE_TCP_TRANSPORT)
    {
        return;
    }

    if (connection->sock != -1)
    {
        (void)robotraconteurlite_tcp_socket_close(connection->sock);
        connection->sock = -1;
    }
}

void robotraconteurlite_tcp_connections_close(struct robotraconteurlite_connection* connections_head)
{
    struct robotraconteurlite_connection* c = connections_head;
    while (c != NULL)
    {
        robotraconteurlite_tcp_connection_close(c);
        c = c->next;
    }
}

#define STRCONST_HTTP_REQUEST_1 "GET "
#define STRCONST_HTTP_REQUEST_1_LEN 4U
#define STRCONST_HTTP_REQUEST_2 " HTTP/1.1\r\nHost: "
#define STRCONST_HTTP_REQUEST_2_LEN 17U
#define STRCONST_HTTP_REQUEST_3 "\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: "
#define STRCONST_HTTP_REQUEST_3_LEN 62U
#define STRCONST_HTTP_REQUEST_4                                                                                        \
    "\r\nSec-WebSocket-Protocol: robotraconteur.robotraconteur.com\r\nSec-WebSocket-Version: 13\r\n\r\n"
#define STRCONST_HTTP_REQUEST_4_LEN 90U

static robotraconteurlite_status robotraconteurlite_tcp_connect_service_send_websocket_http_header(
    struct robotraconteurlite_tcp_connect_service_data* connect_data)
{
    uint8_t* send_buf = NULL;
    int i = 0;
    uint8_t websocket_key[STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN];
    uint32_t send_len = STRCONST_HTTP_REQUEST_1_LEN + connect_data->service_address->http_path.len +
                        STRCONST_HTTP_REQUEST_2_LEN + connect_data->service_address->http_host.len +
                        STRCONST_HTTP_REQUEST_3_LEN + WEBSOCKET_KEY_BASE64_LEN + STRCONST_HTTP_REQUEST_4_LEN;
    size_t sec_b64_send_len = send_len;
    assert(connect_data->client_out->transport_type == ROBOTRACONTEURLITE_TCP_TRANSPORT);
    assert(connect_data->client_out->send_buffer_pos == 0U);

    for (i = 0; i < (int)sizeof(websocket_key); i++)
    {
        websocket_key[i] = (uint8_t)((uint32_t)rand() % 256U);
    }

    if (send_len > connect_data->client_out->send_buffer_len)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_PARAMETER;
    }

    if ((connect_data->service_address->http_path.len == 0U) || (connect_data->service_address->http_host.len == 0U))
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_PARAMETER;
    }

    /* NOLINTBEGIN(bugprone-not-null-terminated-result) */
    send_buf = connect_data->client_out->send_buffer;
    (void)memcpy(send_buf, STRCONST_HTTP_REQUEST_1, STRCONST_HTTP_REQUEST_1_LEN);
    send_buf += STRCONST_HTTP_REQUEST_1_LEN;
    (void)memcpy(send_buf, connect_data->service_address->http_path.data, connect_data->service_address->http_path.len);
    send_buf += connect_data->service_address->http_path.len;
    (void)memcpy(send_buf, STRCONST_HTTP_REQUEST_2, STRCONST_HTTP_REQUEST_2_LEN);
    send_buf += STRCONST_HTTP_REQUEST_2_LEN;
    (void)memcpy(send_buf, connect_data->service_address->http_host.data, connect_data->service_address->http_host.len);
    send_buf += connect_data->service_address->http_host.len;
    (void)memcpy(send_buf, STRCONST_HTTP_REQUEST_3, STRCONST_HTTP_REQUEST_3_LEN);
    send_buf += STRCONST_HTTP_REQUEST_3_LEN;

    if (robotraconteurlite_tcp_base64_encode(websocket_key, sizeof(websocket_key), (char*)send_buf,
                                             &sec_b64_send_len) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if (sec_b64_send_len != WEBSOCKET_KEY_BASE64_LEN)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    send_buf += WEBSOCKET_KEY_BASE64_LEN;
    (void)memcpy(send_buf, STRCONST_HTTP_REQUEST_4, STRCONST_HTTP_REQUEST_4_LEN);
    send_buf += STRCONST_HTTP_REQUEST_4_LEN;
    /* NOLINTEND(bugprone-not-null-terminated-result) */

    assert((send_buf - connect_data->client_out->send_buffer) == (int)send_len);
    connect_data->client_out->send_message_len = send_len;
    connect_data->client_out->send_buffer_pos = 0;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_tcp_connect_service(
    struct robotraconteurlite_tcp_connect_service_data* connect_data, robotraconteurlite_timespec now)
{
    struct robotraconteurlite_connection* c = connect_data->connections_head;
    int errno_out = 0;
    robotraconteurlite_status rv = -1;
    int sock = 0;
    while (c != NULL)
    {
        if ((c->transport_type == ROBOTRACONTEURLITE_TCP_TRANSPORT) &&
            ((c->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) == 0U) &&
            ((c->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE) != 0U))
        {
            break;
        }
        c = c->next;
    }

    if (!c)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    sock = robotraconteurlite_tcp_socket_connect(&connect_data->service_address->socket_addr);
    if (sock == -1)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    c->sock = sock;
    c->connection_state =
        (uint32_t)ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
    (void)memset(&c->transport_storage, 0, sizeof(c->transport_storage));
    if ((connect_data->service_address->flags & ROBOTRACONTEURLITE_ADDR_FLAGS_WEBSOCKET) != 0U)
    {
        struct robotraconteurlite_tcp_transport_storage* storage =
            (struct robotraconteurlite_tcp_transport_storage*)&c->transport_storage;
        storage->tcp_transport_state |=
            ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET | ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IN_HTTP_HEADER;
        c->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_BLOCK_SEND;
    }
    c->last_recv_message_time = connect_data->now;
    c->last_send_message_time = connect_data->now;

    c->remote_nodename.data = c->remote_nodename_char;
    c->remote_nodename.len = sizeof(c->remote_nodename_char);
    c->remote_service_name.data = c->remote_service_name_char;
    c->remote_service_name.len = sizeof(c->remote_service_name_char);
    if (robotraconteurlite_nodeid_copy_to(&connect_data->service_address->nodeid, &c->remote_nodeid) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if (robotraconteurlite_string_copy_to(&connect_data->service_address->nodename, &c->remote_nodename) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if (robotraconteurlite_string_copy_to(&connect_data->service_address->service_name, &c->remote_service_name) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    c->last_recv_message_time = now;
    c->last_send_message_time = now;

    connect_data->client_out = c;

    if ((connect_data->service_address->flags & ROBOTRACONTEURLITE_ADDR_FLAGS_WEBSOCKET) != 0U)
    {
        return robotraconteurlite_tcp_connect_service_send_websocket_http_header(connect_data);
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

void robotraconteurlite_tcp_connection_init_connection_client(struct robotraconteurlite_connection* connection)
{
    connection->transport_type = ROBOTRACONTEURLITE_TCP_TRANSPORT;
    connection->sock = -1;
    connection->config_flags &= ~ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER;
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE;
    connection->heartbeat_period_ms = 5000;
    connection->heartbeat_timeout_ms = 15000;
}

void robotraconteurlite_tcp_connection_init_connections_client(struct robotraconteurlite_connection* connections_head)
{
    struct robotraconteurlite_connection* c = connections_head;
    while (c != NULL)
    {
        robotraconteurlite_tcp_connection_init_connection_client(c);
        c = c->next;
    }
}
