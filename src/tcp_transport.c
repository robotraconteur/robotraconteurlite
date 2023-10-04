#include "robotraconteurlite/tcp_transport.h"

#include <stdlib.h>
#include <string.h>

// Linux socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <endian.h>
#include <errno.h>

#ifdef ROBOTRACONTEURLITE_USE_OPENSSL
#include <openssl/sha.h>
#endif




int robotraconteurlite_tcp_sha1(const uint8_t* data, size_t len, struct robotraconteurlite_tcp_sha1_storage* storage)
{
#ifdef ROBOTRACONTEURLITE_USE_OPENSSL
    SHA1(data, len, storage->sha1_bytes);
    return 0;
#else
    // No SHA1 implementation on this platform!
    assert(0);
    return ROBOTRACONTEURLITE_ERROR_NOT_IMPLEMENTED;
#endif
}


int robotraconteurlite_tcp_acceptor_listen(struct robotraconteurlite_connection_acceptor* acceptor, const struct sockaddr_storage* serv_addr, int backlog)
{
    // Create socket
    acceptor->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (acceptor->sock < 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    // Make socket non-blocking
    int flags = fcntl(acceptor->sock, F_GETFL, 0);
    if (flags < 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    flags |= O_NONBLOCK;
    if (fcntl(acceptor->sock, F_SETFL, flags) < 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    // Bind socket
    if (bind(acceptor->sock, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_storage)) < 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    // Listen
    if (listen(acceptor->sock, backlog) < 0)
    {
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    return 0;
}

int robotraconteurlite_tcp_acceptor_close(struct robotraconteurlite_connection_acceptor* acceptor)
{
    if (acceptor->sock >= 0)
    {
        close(acceptor->sock);
        acceptor->sock = -1;
    }

    return 0;
}

int robotraconteurlite_tcp_acceptor_communicate(struct robotraconteurlite_connection_acceptor* acceptor, struct robotraconteurlite_connection* connection_head)
{

    // Find a connection that is idle
    struct robotraconteurlite_connection* c = connection_head;
    while (c)
    {
        if ((c->transport_type = ROBOTRACONTEURLITE_TCP_TRANSPORT) &&
          (c->config_flags | ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) != 0 &&
          (c->connection_state | ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE) != 0)
        {
            break;
        }
        c = c->next;
    }

    if (!c)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    // Accept connection
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(acceptor->sock, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
        // TODO: Report error
        
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    // Make socket non-blocking
    int flags = fcntl(newsockfd, F_GETFL, 0);
    if (flags < 0)
    {
        // TODO: Report error
        close(newsockfd);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    flags |= O_NONBLOCK;
    if (fcntl(newsockfd, F_SETFL, flags) < 0)
    {
        // TODO: Report error
        close(newsockfd);
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    // Set socket
    c->sock = newsockfd;
    c->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED | ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
    memset(&c->transport_storage, 0, sizeof(c->transport_storage));
    
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_tcp_connection_recv_websocket(struct robotraconteurlite_connection* connection, size_t len)
{
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if (storage->recv_websocket_frame_pos == storage->recv_websocket_frame_len && 
        (storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME) != 0 )
	{
	  //reset for next websocket frame
	  storage->recv_websocket_frame_pos = 0;
	  storage->recv_websocket_frame_len = 0;
	  storage->tcp_transport_state &= ~(ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME);
	}

    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME) == 0)
    {
        size_t websocket_header_len = 2;
        if (storage->recv_websocket_header_pos >= 2)
        {            
            uint8_t len1 = connection->recv_buffer[1] & 0x7F;
            uint8_t masked = connection->recv_buffer[1] & 0x80;
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
        }

        if (storage->recv_websocket_header_pos < websocket_header_len)
        {
            size_t recv_len = websocket_header_len - storage->recv_websocket_header_pos;

            int n = recv(connection->sock, storage->recv_websocket_header_buffer + storage->recv_websocket_header_pos, recv_len, 0);
            if (n <= 0 && n!=-EWOULDBLOCK)
            {
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
                return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
            }

            if (n == -EWOULDBLOCK)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }

            storage->recv_websocket_header_pos += n;
            if (storage->recv_websocket_header_pos < websocket_header_len)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }

            uint8_t len1 = storage->recv_websocket_header_buffer[1] & 0x7F;
            uint8_t masked = storage->recv_websocket_header_buffer[1] & 0x80;
            if (len1 == 126)
            {
                storage->recv_websocket_frame_len = ntohs(*(uint16_t*)&storage->recv_websocket_header_buffer[2]);
            }
            else if (len1 == 127)
            {
                storage->recv_websocket_frame_len = be64toh(*(uint64_t*)storage->recv_websocket_header_buffer[2]);
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

        }  
    }
    else
    {
        size_t recv_len = storage->recv_websocket_frame_len - storage->recv_websocket_frame_pos;
        if (recv_len > len)
        {
            recv_len = len;
        }

        int n = recv(connection->sock, connection->recv_buffer + connection->recv_buffer_pos, recv_len, 0);
        if (n <= 0 && n!=-EWOULDBLOCK)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        
        if (n == -EWOULDBLOCK)
        {
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }

        for (size_t i=0; i<n; i++)
        {
            connection->recv_buffer[connection->recv_buffer_pos + i] ^= storage->recv_websocket_mask[(storage->recv_websocket_frame_pos + i) % 4];
        }

        storage->recv_websocket_frame_pos += n;
        connection->recv_buffer_pos += n;        
        
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_tcp_connection_recv(struct robotraconteurlite_connection* connection, size_t len)
{
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) != 0)
    {
        return robotraconteurlite_tcp_connection_recv_websocket(connection, len);
    }

    int n = recv(connection->sock, connection->recv_buffer + connection->recv_buffer_pos, len, 0);
    if (n <= 0 && n!=-EWOULDBLOCK)
    {
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }
    if (n==-EWOULDBLOCK)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    connection->recv_buffer_pos += n;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_tcp_connection_send(struct robotraconteurlite_connection* connection, size_t len)
{
    struct robotraconteurlite_tcp_transport_storage* storage = (struct robotraconteurlite_tcp_transport_storage*)&connection->transport_storage;
    if ((storage->tcp_transport_state & ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET) != 0)
    {
        return robotraconteurlite_tcp_connection_send_websocket(connection, len);
    }

    int n = send(connection->sock, connection->send_buffer + connection->send_buffer_pos, len, 0);
    if (n <= 0 && n!=-EWOULDBLOCK)
    {
        connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
        return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
    }

    if (n == -EWOULDBLOCK)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    connection->send_buffer_pos += n;
}

int robotraconteurlite_tcp_connection_communicate_recv(struct robotraconteurlite_connection* connection)
{
    if ((connection->transport_type == ROBOTRACONTEURLITE_TCP_TRANSPORT)
    && ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED) != 0))
    {
        // If the connection is in an error state, close the socket
        if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR) != 0)
        {
            close(connection->sock);
            connection->sock = -1;
            connection->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }

        // If the message has been consumed, move the receive buffer to the beginning
        if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED) != 0)
        {
            memmove(connection->recv_buffer, connection->recv_buffer + connection->recv_message_len, connection->recv_buffer_pos - connection->recv_message_len);
            connection->recv_buffer_pos -= connection->recv_message_len;
            connection->recv_message_len = 0;
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED;
        }

        if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED) != 0)
        {
            
            // Receive data
            size_t recv_op_len = connection->recv_message_len == 0 ? 64 : connection->recv_message_len - connection->recv_buffer_pos;
            
            // Receive data
            int n = robotraconteurlite_tcp_connection_recv(connection, recv_op_len);
            if (n <= 0 && n!=-EWOULDBLOCK)
            {
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
                connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
                return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
            }

            if (n == -EWOULDBLOCK)
            {
                return ROBOTRACONTEURLITE_ERROR_SUCCESS;
            }

            // If this is the first receive, parse the message length
            
            if(robotraconteurlite_connection_verify_preamble(connection, &connection->recv_message_len) != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
                connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
                return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
            }
            
            // If we have received the entire message, set the message received flag
            if (connection->recv_message_len > 0 && connection->recv_buffer_pos >= connection->recv_message_len)
            {
                connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
                connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED;
            }

            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }  
    }
}

int robotraconteurlite_tcp_connection_websocket_send(struct robotraconteurlite_connection* connection)
{

}

int robotraconteurlite_tcp_connection_send(struct robotraconteurlite_connection* connection)
{
    
}

int robotraconteurlite_tcp_connection_communicate_send(struct robotraconteurlite_connection* connection)
{

}

int robotraconteurlite_tcp_connection_communicate(struct robotraconteurlite_connection* connection)
{
    int ret = robotraconteurlite_tcp_connection_communicate_recv(connection);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    return robotraconteurlite_tcp_connection_communicate_send(connection);
}

int robotraconteurlite_tcp_connections_communicate(struct robotraconteurlite_connection* connections_head)
{
    struct robotraconteurlite_connection* c = connections_head;
    while (c)
    {
        robotraconteurlite_tcp_connection_communicate(c);
        c = c->next;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}


