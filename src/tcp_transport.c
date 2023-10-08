#include "robotraconteurlite/tcp_transport.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

int robotraconteurlite_tcp_acceptor_listen(struct robotraconteurlite_connection_acceptor* acceptor, const struct sockaddr_storage* serv_addr, int backlog)
{
    
    int sock;
    int last_errno;
    int ret;

    assert(sizeof(struct robotraconteurlite_tcp_transport_storage) <= sizeof(struct robotraconteurlite_transport_storage));
    
    ret = robotraconteurlite_tcp_socket_begin_server(serv_addr, backlog, &acceptor->sock, &last_errno);
    return ret;
}

int robotraconteurlite_tcp_acceptor_close(struct robotraconteurlite_connection_acceptor* acceptor)
{
    if (acceptor->sock >= 0)
    {
        robotraconteurlite_tcp_socket_close(acceptor->sock);
        acceptor->sock = -1;
    }

    return 0;
}

int robotraconteurlite_tcp_acceptor_communicate(struct robotraconteurlite_connection_acceptor* acceptor, struct robotraconteurlite_connection* connection_head, robotraconteurlite_timespec now)
{
    /* Find a connection that is idle */
    struct robotraconteurlite_connection* c = connection_head;
    int errno_out;
    int ret;
    while (c)
    {
        if ((c->transport_type = ROBOTRACONTEURLITE_TCP_TRANSPORT) &&
          ((c->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) != 0) &&
          ((c->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE) != 0))
        {
            break;
        }
        c = c->next;
    }

    if (!c)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Accept connection */
    ret = robotraconteurlite_tcp_socket_accept(acceptor->sock, &c->sock, &errno_out);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        return ret;
    }

    c->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
    c->last_recv_message_time = now;
    c->last_send_message_time = now;
    memset(&c->transport_storage, 0, sizeof(c->transport_storage));
    
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
    while (c)
    {
        robotraconteurlite_tcp_connection_init_connection_server(c);
        c = c->next;
    }
}

static size_t robotraconteurlite_tcp_connection_recv_websocket_header_size(uint8_t byte_two)
{
    size_t websocket_header_len = 2;
    uint8_t len1 = byte_two & 0x7F;
    uint8_t masked = byte_two & 0x80;
    if (len1 == 126)
    {
        websocket_header_len += 2;
    }
    else if (len1 == 127)
    {
        websocket_header_len += 8;
    }

    if (masked)
    {
        websocket_header_len += 4;
    }

    return websocket_header_len;
}

static int robotraconteurlite_tcp_connection_buffer_recv_websocket(struct robotraconteurlite_connection* connection, size_t len)
{
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if (storage->recv_websocket_frame_pos == storage->recv_websocket_frame_len && 
        (storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME) != 0 )
	{
	  /* reset for next websocket frame */
	  storage->recv_websocket_frame_pos = 0;
	  storage->recv_websocket_frame_len = 0;
      storage->recv_websocket_header_pos = 0;
	  storage->tcp_transport_state &= ~(ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME);
	}

    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME) == 0)
    {
        size_t websocket_header_len = 2;
        uint8_t len1;
        uint8_t masked;

        if (storage->recv_websocket_header_pos < 2)
        {
            int last_errno;
            int ret = robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, storage->recv_websocket_header_buffer, &storage->recv_websocket_header_pos, 2, &last_errno);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
                return ret;
            }

            if (storage->recv_websocket_header_pos < 2)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
        }

        websocket_header_len = robotraconteurlite_tcp_connection_recv_websocket_header_size(storage->recv_websocket_header_buffer[1]);

        if (storage->recv_websocket_header_pos < websocket_header_len)
        {       
            int last_errno;
            int ret = robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, storage->recv_websocket_header_buffer, &storage->recv_websocket_header_pos, websocket_header_len, &last_errno);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
                return ret;
            }

            if (storage->recv_websocket_header_pos < websocket_header_len)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }
        }

        len1 = storage->recv_websocket_header_buffer[1] & 0x7F;
        masked = storage->recv_websocket_header_buffer[1] & 0x80;
        if (len1 == 126)
        {
            storage->recv_websocket_frame_len = robotraconteurlite_ntohs(*(uint16_t*)&storage->recv_websocket_header_buffer[2]);
        }
        else if (len1 == 127)
        {
            storage->recv_websocket_frame_len = robotraconteurlite_be64toh(*(uint64_t*)&storage->recv_websocket_header_buffer[2]);
        }
        else
        {
            storage->recv_websocket_frame_len = len1;
        }

        if (masked)
        {
            memcpy(storage->recv_websocket_mask, storage->recv_websocket_header_buffer + websocket_header_len - 4, 4);
            storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_ENABLE_MASK;
        }
        else
        {
            storage->tcp_transport_state &= ~ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_ENABLE_MASK;
        }


        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME;        
    }
    if (storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME)
    {
        uint32_t recv_len = storage->recv_websocket_frame_len + (connection->recv_buffer_pos - storage->recv_websocket_frame_pos);
        int last_errno;
        uint32_t prev_recv_buffer_pos;
        int ret;
        size_t n;
        size_t i;
        if (recv_len > len)
        {
            recv_len = len;
        }

        prev_recv_buffer_pos = connection->recv_buffer_pos;
        ret = robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, connection->recv_buffer, &connection->recv_buffer_pos, recv_len, &last_errno);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ret;
        }
        n = connection->recv_buffer_pos - prev_recv_buffer_pos;
        for (i=0; i<n; i++)
        {
            connection->recv_buffer[prev_recv_buffer_pos + i] ^= storage->recv_websocket_mask[(storage->recv_websocket_frame_pos + i) % 4];
        }
        storage->recv_websocket_frame_pos += n;        
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_tcp_connection_buffer_recv(struct robotraconteurlite_connection* connection, size_t len)
{
    int last_errno;
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) != 0)
    {
        return robotraconteurlite_tcp_connection_buffer_recv_websocket(connection, len);
    }

    return robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, connection->recv_buffer, &connection->recv_buffer_pos, len, &last_errno);
}

int robotraconteurlite_tcp_connection_communicate_recv(struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now)
{       
    /* If the connection is in an error state, return error */
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* If the message has been consumed, move the receive buffer to the beginning */
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED) != 0)
    {
        if(connection->recv_buffer_pos > connection->recv_message_len)
        {
            memmove(connection->recv_buffer, connection->recv_buffer + connection->recv_message_len, connection->recv_buffer_pos - connection->recv_message_len);
        }
        connection->recv_buffer_pos -= connection->recv_message_len;
        connection->recv_message_len = 0;
        connection->connection_state &= ~(ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED | ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED);
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
    }

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED) != 0)
    {
        
        /* Receive data */
        size_t recv_op_len = connection->recv_message_len == 0 ? 64 : connection->recv_buffer_len - connection->recv_buffer_pos;
        
        /* Receive data */
        int ret = robotraconteurlite_tcp_connection_buffer_recv(connection, recv_op_len);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
            return ret;
        }

        /* If this is the first receive, parse the message length */
        
        if(robotraconteurlite_connection_verify_preamble(connection, &connection->recv_message_len) != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        
        /* If we have received the entire message, set the message received flag */
        if (connection->recv_message_len > 0 && connection->recv_buffer_pos >= connection->recv_message_len)
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

static int robotraconteurlite_tcp_websocket_random_mask(struct robotraconteurlite_connection* connection, uint8_t mask[4])
{
    /* TODO: use better random source */
    uint32_t random_val = rand();
    memcpy(mask, &random_val, 4);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static int robotraconteurlite_tcp_connection_buffer_send_websocket(struct robotraconteurlite_connection* connection, size_t len)
{
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    size_t send_len = len - connection->send_buffer_pos;
    int last_errno;
    int ret;
    if (storage->send_websocket_frame_len == 0 && send_len > 0)
    {
        /* Prepare new frame */
        if (send_len >= UINT16_MAX)
        {
            send_len = UINT16_MAX;
        }

        storage->send_websocket_header_len = 2;
        storage->send_websocket_header_buffer[0] = 0x80 | ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_BINARY;
        if (len > 125)
        {
            storage->send_websocket_header_len += 2;
            *(uint16_t*)&storage->send_websocket_header_buffer[2] = robotraconteurlite_htons(len);
            storage->send_websocket_header_buffer[1] = 126;
        }
        else
        {
            storage->send_websocket_header_buffer[1] = (uint8_t)len;
        }

        /* Create mask if client */
        if ((connection->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) == 0)
        {
            storage->send_websocket_header_len += 4;
            storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_SEND_WEBSOCKET_ENABLE_MASK;
            robotraconteurlite_tcp_websocket_random_mask(connection, storage->send_websocket_mask);
            storage->send_websocket_header_buffer[1] |= 0x80;
            memcpy(storage->send_websocket_header_buffer + storage->send_websocket_header_len - 4, storage->send_websocket_mask, 4);

        }

        storage->send_websocket_frame_len = send_len;
        storage->send_websocket_frame_buffer_end = send_len + connection->send_buffer_pos;
        storage->send_websocket_frame_buffer_pos = connection->send_buffer_pos;

        /* Apply mask to send buffer */
        if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_SEND_WEBSOCKET_ENABLE_MASK) != 0)
        {
            size_t i;
            for (i=0; i<send_len; i++)
            {
                connection->send_buffer[connection->send_buffer_pos + i] ^= storage->send_websocket_mask[i % 4];
            }
        }
    }

    /* Send header nonblocking */
    if (storage->send_websocket_header_len > 0 && storage->send_websocket_header_pos < storage->send_websocket_header_len)
    {
        int last_errno;
        int ret = robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, storage->send_websocket_header_buffer, &storage->send_websocket_header_pos, storage->send_websocket_header_len, &last_errno);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            return ret;
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
    
    ret = robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer, &storage->send_websocket_frame_buffer_pos, storage->send_websocket_frame_buffer_end, &last_errno);

    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
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

int robotraconteurlite_tcp_connection_buffer_send(struct robotraconteurlite_connection* connection, size_t len)
{
    int last_errno;
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) != 0)
    {
        return robotraconteurlite_tcp_connection_buffer_send_websocket(connection, len);
    }

    return robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer, &connection->send_buffer_pos, len, &last_errno);
}

int robotraconteurlite_tcp_connection_communicate_send(struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now)
{    
    /* If the connection is in an error state, return error */
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED))
    {
        connection->connection_state &= ~(ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED | ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT);
        connection->send_buffer_pos = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED) != 0)
    {
        int ret;
        /* Clear send requested flag */
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED;

        /* Send data */
        ret = robotraconteurlite_tcp_connection_buffer_send(connection, connection->send_message_len);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ret;
        }

        /* If we have not sent the entire message, set the message sending flag */
        if (connection->send_message_len > 0 && connection->send_buffer_pos < connection->send_message_len)
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
    else if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING) != 0)
    {
        /* Send data */
        size_t send_op_len = connection->send_message_len == 0 ? 64 : connection->send_message_len - connection->send_buffer_pos;
        int ret = robotraconteurlite_tcp_connection_buffer_send(connection, send_op_len);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ret;
        }

        /* If we have sent the entire message, set the message sent flag */
        if (connection->send_message_len > 0 && connection->send_buffer_pos >= connection->send_message_len)
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

static int robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(const char* recv_data, size_t recv_data_len, size_t* i_end, size_t* i_next)
{
    size_t i = *i_next;
    while (i < recv_data_len && recv_data[i] != '\n' && recv_data[i] != '\r')
    {
        i++;
    }

    if (i >= recv_data_len)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    *i_end = i;
    while (i < recv_data_len && (recv_data[i] == '\n' || recv_data[i] == '\r'))
    {
        i++;
    }
    *i_next = i;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

#define STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN 18
#define STRCONST_HTTP_SEC_WEBSOCKET_KEY "Sec-WebSocket-Key:"
#define STRCONST_HTTP_KEY_UUID_LEN 36
#define STRCONST_HTTP_KEY_UUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WEBSOCKET_KEY_BASE64_LEN 24
#define STRCONST_HTTP_RESPONSE_1 "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Protocol: robotraconteur.robotraconteur.com\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Accept: "
#define STRCONST_HTTP_RESPONSE_1_LEN 183
#define STRCONST_HTTP_RESPONSE_2 "\r\n\r\n"
#define STRCONST_HTTP_RESPONSE_2_LEN 4

static int robotraconteurlite_tcp_connection_handshake_http_handshake(struct robotraconteurlite_connection* connection)
{
    const char* recv_data = (const char*)connection->recv_buffer;
    size_t recv_data_len = connection->recv_buffer_pos;

    /* Skip first line of receive data */
    size_t i_end = 0;
    size_t i_next = 0;
    size_t i_sec_websocket = 0;
    size_t i_sec_websocket_end;
    struct robotraconteurlite_tcp_sha1_storage sha1_storage;
    uint8_t sha1_input_bytes[60];
    char sha1_base64[28];
    size_t sha1_base64_len = sizeof(sha1_base64);
    int last_errno;
    int ret = robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(recv_data, recv_data_len, &i_end, &i_next);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }
    
    /* Find the websocket sec key line */
    while (i_next < recv_data_len)
    {
        if (recv_data[i_next] == 'S')
        {
            if (i_next + STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN < recv_data_len && memcmp(recv_data + i_next, STRCONST_HTTP_SEC_WEBSOCKET_KEY, STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN) == 0)
            {
                i_sec_websocket = i_next + STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN;
                break;
            }
        }
        ret = robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(recv_data, recv_data_len, &i_end, &i_next);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            return ret;
        }
    }

    if (i_sec_websocket == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Find end of sec key line */
    ret = robotraconteurlite_tcp_connection_handshake_http_handshake_find_next_line(recv_data, recv_data_len, &i_end, &i_next);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    /* Skip any whitespace */
    while (i_sec_websocket < i_end && (recv_data[i_sec_websocket] == ' ' || recv_data[i_sec_websocket] == '\t'))
    {
        i_sec_websocket++;
    }

    /* Find end of sec key */
    i_sec_websocket_end = i_sec_websocket;
    while (i_sec_websocket_end < i_end && recv_data[i_sec_websocket_end] != ' ' && recv_data[i_sec_websocket_end] != '\t')
    {
        i_sec_websocket_end++;
    }

    /* Check the client key length */
    if (i_sec_websocket_end - i_sec_websocket != WEBSOCKET_KEY_BASE64_LEN)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* Generate the return key */
    
    /* Copy the client key to the buffer */
    memcpy(sha1_input_bytes, recv_data + i_sec_websocket, WEBSOCKET_KEY_BASE64_LEN);
    /* Copy the UUID to the buffer */
    memcpy(sha1_input_bytes + WEBSOCKET_KEY_BASE64_LEN, STRCONST_HTTP_KEY_UUID, STRCONST_HTTP_KEY_UUID_LEN);
    /* Compute the SHA1 hash */
    robotraconteurlite_tcp_sha1(sha1_input_bytes, sizeof(sha1_input_bytes), &sha1_storage);
    /* Convert the SHA1 hash to base64 */
    robotraconteurlite_tcp_base64_encode(sha1_storage.sha1_bytes, sizeof(sha1_storage.sha1_bytes), sha1_base64, &sha1_base64_len);
    assert(sha1_base64_len == sizeof(sha1_base64));

    connection->recv_buffer_pos = 0;
    /* Send the response using send_buffer */
    connection->send_buffer_pos = 0;

    /* Send the first part of the response */
    memcpy(connection->send_buffer, STRCONST_HTTP_RESPONSE_1, STRCONST_HTTP_RESPONSE_1_LEN);
    /* Send the accept hash */
    memcpy(connection->send_buffer + STRCONST_HTTP_RESPONSE_1_LEN, sha1_base64, sha1_base64_len);
    /* Send the second part of the response */
    memcpy(connection->send_buffer + STRCONST_HTTP_RESPONSE_1_LEN + sha1_base64_len, STRCONST_HTTP_RESPONSE_2, STRCONST_HTTP_RESPONSE_2_LEN);
    /* Set the send message length */
    connection->send_message_len = STRCONST_HTTP_RESPONSE_1_LEN + sha1_base64_len + STRCONST_HTTP_RESPONSE_2_LEN;
    assert(connection->send_message_len <= connection->send_buffer_len);

    /* Send the response */
    ret = robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer, &connection->send_buffer_pos, connection->send_message_len, &last_errno);
    return ret;
}

static int robotraconteurlite_tcp_connection_handshake_http_recv_header(struct robotraconteurlite_connection* connection)
{
    int newline_found = 0;
    int ret = 0;
    /* Receive byte by byte until two newlines character */
    while (connection->recv_buffer_pos < connection->recv_buffer_len)
    {
        int last_errno;
        size_t last_recv_buffer_pos = connection->recv_buffer_pos;
        ret = robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, connection->recv_buffer, &connection->recv_buffer_pos, connection->recv_buffer_pos+1, &last_errno);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ret;
        }
        if (connection->recv_buffer_pos == last_recv_buffer_pos)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }
        /* Check for ending of \r\n\r\n or \n\n or \r\r or \n\r\n\r */
        if (connection->recv_buffer_pos >= 4)
        {
            uint8_t* end_minus_4 = connection->recv_buffer + connection->recv_buffer_pos - 4;
            if (memcmp(end_minus_4, "\r\n\r\n", 4) == 0 || memcmp(end_minus_4, "\n\r\n\r", 4) == 0)
            {
                newline_found = 1;
                break;
            }
        }

        if (connection->recv_buffer_pos >= 2)
        {
            uint8_t* end_minus_2 = connection->recv_buffer + connection->recv_buffer_pos - 2;
            if (memcmp(end_minus_2, "\n\n", 2) == 0 || memcmp(end_minus_2, "\r\r", 2) == 0)
            {
                newline_found = 1;
                break;
            }
        }
        
    }

    /* Check that two newlines were found */
    if (!newline_found)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static int robotraconteurlite_tcp_connection_handshake_server(struct robotraconteurlite_connection* connection)
{
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    int ret = 0;
    
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING) == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (connection->send_message_len > 0)
    {
        int last_errno;
        int ret = robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer, &connection->send_buffer_pos, connection->send_message_len, &last_errno);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
            {
                return ROBOTRACONTEURLITE_ERROR_RETRY;
            }
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ret;
        }
        if (connection->send_buffer_pos < connection->send_message_len)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }

        /* We are now connected using websocket! */
        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
        /* Clear send buffer */
        connection->send_buffer_pos = 0;
        connection->send_message_len = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (connection->recv_buffer_pos < 4)
    {
        int last_errno;
        int ret = robotraconteurlite_tcp_socket_recv_nonblocking(connection->sock, connection->recv_buffer, &connection->recv_buffer_pos, 4, &last_errno);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ret;
        }
        if (connection->recv_buffer_pos < 4)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }
    }

    /* Check if first 4 bytes are RRAC */
    if (memcmp(connection->recv_buffer, "RRAC", 4) == 0)
    {
        /* We are connected with a non-websocket client */
        storage->tcp_transport_state &= ~ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Check if first 4 bytes are GET */
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

    ret = robotraconteurlite_tcp_connection_handshake_http_recv_header(connection);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return ROBOTRACONTEURLITE_ERROR_RETRY;
        }
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return ret;
    }

    /* Handshake */
    ret = robotraconteurlite_tcp_connection_handshake_http_handshake(connection);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return ret;
    }

    /* Check if the response was sent */
    if (connection->send_message_len > 0 && connection->send_buffer_pos == connection->send_message_len)
    {
        /* We are connected using websockets! */
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
        connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING;
        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
        /* Clear send buffer */
        connection->send_buffer_pos = 0;
        connection->send_message_len = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    return ROBOTRACONTEURLITE_ERROR_CONSUMED;
}

static int robotraconteurlite_tcp_connection_handshake_client(struct robotraconteurlite_connection* connection)
{
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    int newline_found = 0;
    int ret = 0;

    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING) == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) == 0)
    {
        /* No handshake for raw sockets */
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Send data left in buffer */
    if (connection->send_message_len > 0)
    {
        int last_errno;
        int ret = robotraconteurlite_tcp_socket_send_nonblocking(connection->sock, connection->send_buffer, &connection->send_buffer_pos, connection->send_message_len, &last_errno);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ret;
        }
        if (connection->send_buffer_pos < connection->send_message_len)
        {
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }
    }

    ret = robotraconteurlite_tcp_connection_handshake_http_recv_header(connection);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (ret == ROBOTRACONTEURLITE_ERROR_RETRY || ret == ROBOTRACONTEURLITE_ERROR_CONSUMED)
        {
            return ret;
        }
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return ret;
    }

    /* Make sure we actually upgraded */
    if (memcmp(connection->recv_buffer, "HTTP/1.1 101", 12) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    /* TODO: We should check more of the HTTP response, but just ignore for now and begin using the connection! */

    /* We are connected using websockets! */
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
    connection->connection_state &= ~(ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING | ROBOTRACONTEURLITE_STATUS_FLAGS_BLOCK_SEND | ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING | ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED);
    storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET;
    storage->tcp_transport_state &= ~ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IN_HTTP_HEADER;
    /* Clear send buffer */
    connection->send_buffer_pos = 0;
    connection->send_message_len = 0;
    connection->recv_buffer_pos = 0;
    connection->recv_message_len = 0;


    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_tcp_connection_handshake(struct robotraconteurlite_connection* connection)
{
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING) == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((connection->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) != 0)
    {
        return robotraconteurlite_tcp_connection_handshake_server(connection);
    }
    else
    {
        return robotraconteurlite_tcp_connection_handshake_client(connection);
    }
}

int robotraconteurlite_tcp_connection_communicate(struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now)
{
    int ret = 0;
    if ((connection->transport_type != ROBOTRACONTEURLITE_TCP_TRANSPORT))
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED)
    {
        connection->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE;
        robotraconteurlite_connection_reset(connection);
    }

    if (connection->connection_state & (ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED | ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE ))
    {       
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (connection->connection_state & (ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED | ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSING))
    {
        /* TODO: graceful shutdown */
        robotraconteurlite_tcp_socket_close(connection->sock);
        connection->sock = -1;
        connection->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    /* Do handshake */
    ret = robotraconteurlite_tcp_connection_handshake(connection);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (ret == ROBOTRACONTEURLITE_ERROR_CONSUMED)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        return ret;
    }
    
    ret = robotraconteurlite_tcp_connection_communicate_recv(connection, now);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    return robotraconteurlite_tcp_connection_communicate_send(connection, now);
}


int robotraconteurlite_tcp_connections_communicate(struct robotraconteurlite_connection* connections_head, robotraconteurlite_timespec now)
{
    struct robotraconteurlite_connection* c = connections_head;
    while (c)
    {
        robotraconteurlite_tcp_connection_communicate(c, now);
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
        robotraconteurlite_tcp_socket_close(connection->sock);
        connection->sock = -1;
    }
}

void robotraconteurlite_tcp_connections_close(struct robotraconteurlite_connection* connections_head)
{
    struct robotraconteurlite_connection* c = connections_head;
    while (c)
    {
        robotraconteurlite_tcp_connection_close(c);
        c = c->next;
    }
}

#define STRCONST_HTTP_REQUEST_1 "GET "
#define STRCONST_HTTP_REQUEST_1_LEN 4
#define STRCONST_HTTP_REQUEST_2 " HTTP/1.1\r\nHost: "
#define STRCONST_HTTP_REQUEST_2_LEN 17
#define STRCONST_HTTP_REQUEST_3 "\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: "
#define STRCONST_HTTP_REQUEST_3_LEN 62
#define STRCONST_HTTP_REQUEST_4 "\r\nSec-WebSocket-Protocol: robotraconteur.robotraconteur.com\r\nSec-WebSocket-Version: 13\r\n\r\n"
#define STRCONST_HTTP_REQUEST_4_LEN 90

int robotraconteurlite_tcp_connect_service_send_websocket_http_header(struct robotraconteurlite_tcp_connect_service_data* connect_data)
{
    uint8_t* send_buf = 0;
    int i;
    uint8_t websocket_key[STRCONST_HTTP_SEC_WEBSOCKET_KEY_LEN];
    uint32_t send_len = STRCONST_HTTP_REQUEST_1_LEN 
        + connect_data->service_address->http_path.len 
        + STRCONST_HTTP_REQUEST_2_LEN 
        + connect_data->service_address->http_host.len 
        + STRCONST_HTTP_REQUEST_3_LEN 
        + WEBSOCKET_KEY_BASE64_LEN 
        + STRCONST_HTTP_REQUEST_4_LEN;
    size_t sec_b64_send_len = send_len;
    assert(connect_data->client_out->transport_type == ROBOTRACONTEURLITE_TCP_TRANSPORT);
    assert(connect_data->client_out->send_buffer_pos == 0);

    for (i = 0; i<sizeof(websocket_key); i++)
    {
        websocket_key[i] = rand() % 256;
    }

    if (send_len > connect_data->client_out->send_buffer_len)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_PARAMETER;
    }

    if (connect_data->service_address->http_path.len == 0 || connect_data->service_address->http_host.len == 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_PARAMETER;
    }

    send_buf = connect_data->client_out->send_buffer;
    memcpy(send_buf, STRCONST_HTTP_REQUEST_1, STRCONST_HTTP_REQUEST_1_LEN);
    send_buf += STRCONST_HTTP_REQUEST_1_LEN;
    memcpy(send_buf, connect_data->service_address->http_path.data, connect_data->service_address->http_path.len);
    send_buf += connect_data->service_address->http_path.len;
    memcpy(send_buf, STRCONST_HTTP_REQUEST_2, STRCONST_HTTP_REQUEST_2_LEN);
    send_buf += STRCONST_HTTP_REQUEST_2_LEN;
    memcpy(send_buf, connect_data->service_address->http_host.data, connect_data->service_address->http_host.len);
    send_buf += connect_data->service_address->http_host.len;
    memcpy(send_buf, STRCONST_HTTP_REQUEST_3, STRCONST_HTTP_REQUEST_3_LEN);
    send_buf += STRCONST_HTTP_REQUEST_3_LEN;
    
    robotraconteurlite_tcp_base64_encode(websocket_key, sizeof(websocket_key), (char*)send_buf, &sec_b64_send_len);
    if(sec_b64_send_len != WEBSOCKET_KEY_BASE64_LEN)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    send_buf += WEBSOCKET_KEY_BASE64_LEN;
    memcpy(send_buf, STRCONST_HTTP_REQUEST_4, STRCONST_HTTP_REQUEST_4_LEN);
    send_buf += STRCONST_HTTP_REQUEST_4_LEN;

    connect_data->client_out->send_message_len = send_len;
    connect_data->client_out->send_buffer_pos = 0;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_tcp_connect_service(struct robotraconteurlite_tcp_connect_service_data* connect_data, robotraconteurlite_timespec now)
{
    struct robotraconteurlite_connection* c = connect_data->connections_head;
    int errno_out;
    int ret;
    int sock;
    while (c)
    {
        if ((c->transport_type = ROBOTRACONTEURLITE_TCP_TRANSPORT) &&
          ((c->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) == 0) &&
          ((c->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE) != 0))
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
    c->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
    memset(&c->transport_storage, 0, sizeof(c->transport_storage));
    if (connect_data->service_address->flags & ROBOTRACONTEURLITE_ADDR_FLAGS_WEBSOCKET)
    {
        struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&c->transport_storage;
        storage->tcp_transport_state |= ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET | ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IN_HTTP_HEADER;
        c->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_BLOCK_SEND;
    }
    c->last_recv_message_time = connect_data->now;
    c->last_send_message_time = connect_data->now;

    c->remote_nodename.data = c->remote_nodename_char;
    c->remote_nodename.len = sizeof(c->remote_nodename_char);
    c->remote_service_name.data = c->remote_service_name_char;
    c->remote_service_name.len = sizeof(c->remote_service_name_char);
    robotraconteurlite_nodeid_copy_to(&connect_data->service_address->nodeid, &c->remote_nodeid);
    robotraconteurlite_string_copy_to(&connect_data->service_address->nodename, &c->remote_nodename);
    robotraconteurlite_string_copy_to(&connect_data->service_address->service_name, &c->remote_service_name);

    c->last_recv_message_time = now;
    c->last_send_message_time = now;

    connect_data->client_out = c;

    if (connect_data->service_address->flags & ROBOTRACONTEURLITE_ADDR_FLAGS_WEBSOCKET)
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
    while (c)
    {
        robotraconteurlite_tcp_connection_init_connection_client(c);
        c = c->next;
    }
}

