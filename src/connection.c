#include "robotraconteurlite/connection.h"
#include "robotraconteurlite/util.h"

int robotraconteurlite_connection_reset(struct robotraconteurlite_connection* connection)
{
    connection->connection_state = 0;
    connection->recv_buffer_pos = 0;
    connection->recv_message_len = 0;
    connection->send_buffer_pos = 0;
    connection->send_message_len = 0;
    connection->sock = -1;
    connection->local_endpoint = 0;
    connection->remote_endpoint = 0;
    if(robotraconteurlite_nodeid_reset(&connection->remote_nodeid))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_connection_verify_preamble(struct robotraconteurlite_connection* connection, uint32_t* message_len)
{
    if (connection->recv_buffer_pos >= 12 && connection->recv_message_len == 0)
    {
        uint16_t message_version;
        /* Check the message RRAC */
        if (memcmp(connection->recv_buffer, "RRAC", 4) != 0)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        connection->recv_message_len = robotraconteurlite_util_read_uint32(connection->recv_buffer + 4);

        /* Check the message version */
        message_version = robotraconteurlite_util_read_uint16(connection->recv_buffer + 8);
        if (message_version != 2)
        {
            connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
            connection->connection_state &= ~ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED;
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_connection_message_receive(struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_reader* message_reader)
{
    struct robotraconteurlite_buffer buffer;
    struct robotraconteurlite_buffer_vec buffer_vec;
    
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED)==0 || (connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED)!=0)
    {
        return ROBOTRACONTEURLITE_ERROR_RETRY;
    }

    if(robotraconteurlite_buffer_init_scalar(&buffer, connection->recv_buffer, connection->recv_buffer_pos))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if(robotraconteurlite_buffer_vec_init_scalar(&buffer_vec, &buffer))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if(robotraconteurlite_message_reader_init(message_reader, &buffer_vec, 0, connection->recv_buffer_pos))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_connection_message_receive_consume(struct robotraconteurlite_connection* connection)
{
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED)==0)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_OPERATION;
    }
    
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_connection_begin_send_message(struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_writer* message_writer)
{
    struct robotraconteurlite_buffer buffer;
    struct robotraconteurlite_buffer_vec buffer_vec;
    if ((connection->connection_state & 
        (ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR
        | ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED
        | ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED
        | ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE)

    )!=0 
    ||
        ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED)==0)
    )
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_OPERATION;
    }

    if ((connection->connection_state & (
        ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED
        | ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING_MESSAGE
        | ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT
        | ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED
    )) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_RETRY;
    }
        
    if(robotraconteurlite_buffer_init_scalar(&buffer, connection->send_buffer, connection->send_buffer_len))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if(robotraconteurlite_buffer_vec_init_scalar(&buffer_vec, &buffer))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if(robotraconteurlite_message_writer_init(message_writer, &buffer_vec, 0, connection->send_buffer_pos))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;

}

int robotraconteurlite_connection_end_send_message(struct robotraconteurlite_connection* connection, size_t message_len)
{
    connection->send_buffer_pos = message_len;
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_connection_abort_send_message(struct robotraconteurlite_connection* connection)
{
    /* Don't need to do anything, for future use */
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_connection_close(struct robotraconteurlite_connection* connection)
{
    if ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE)!=0)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
