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

#include "robotraconteurlite/connection.h"
#include "robotraconteurlite/util.h"
#include <assert.h>

#define FLAGS_CHECK_ALL ROBOTRACONTEURLITE_FLAGS_CHECK_ALL
#define FLAGS_CHECK ROBOTRACONTEURLITE_FLAGS_CHECK
#define FLAGS_SET ROBOTRACONTEURLITE_FLAGS_SET
#define FLAGS_CLEAR ROBOTRACONTEURLITE_FLAGS_CLEAR

robotraconteurlite_status robotraconteurlite_connection_reset(struct robotraconteurlite_connection* connection)
{
    connection->connection_state = ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE;
    connection->recv_buffer_pos = 0;
    connection->recv_message_len = 0;
    connection->send_buffer_pos = 0;
    connection->send_message_len = 0;
    connection->sock = -1;
    connection->local_endpoint = 0;
    connection->remote_endpoint = 0;
    if (robotraconteurlite_nodeid_reset(&connection->remote_nodeid) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_connection_verify_preamble(
    struct robotraconteurlite_connection* connection, uint32_t* message_len)
{
    const char rrac_magic[4] = {'R', 'R', 'A', 'C'};

    if ((connection->recv_buffer_pos >= 12U) && (connection->recv_message_len == 0U))
    {
        uint16_t message_version = 0U;
        /* Check the message RRAC */
        if (memcmp(connection->recv_buffer, rrac_magic, sizeof(rrac_magic)) != 0)
        {
            FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR);
            FLAGS_CLEAR(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED);
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
        connection->recv_message_len = robotraconteurlite_util_read_uint32(&connection->recv_buffer[4]);
        *message_len = connection->recv_message_len;

        /* Check the message version */
        message_version = robotraconteurlite_util_read_uint16(&connection->recv_buffer[8]);
        if (message_version != 2U)
        {
            FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR);
            FLAGS_CLEAR(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED);
            return ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR;
        }
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_connection_message_receive(
    struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_reader* message_reader,
    struct robotraconteurlite_buffer_vec* buffer_storage)
{

    if ((!FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED)) ||
        FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED))
    {
        return ROBOTRACONTEURLITE_ERROR_RETRY;
    }

    assert(buffer_storage->buffer_vec_cnt >= 1U);
    if (robotraconteurlite_buffer_init_scalar(&buffer_storage->buffer_vec[0], connection->recv_buffer,
                                              connection->recv_buffer_pos) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    buffer_storage->buffer_vec_cnt = 1U;
    if (robotraconteurlite_message_reader_init(message_reader, buffer_storage, 0U, connection->recv_buffer_pos) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_connection_message_receive_consume(
    struct robotraconteurlite_connection* connection)
{
    if (!FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED))
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_OPERATION;
    }

    FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_connection_begin_send_message(
    struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_writer* message_writer,
    struct robotraconteurlite_buffer_vec* buffer_storage)
{
    if (FLAGS_CHECK(connection->connection_state,
                    (ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR | ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED |
                     ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED | ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE)) ||
        (!FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED)))
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_OPERATION;
    }

    if (FLAGS_CHECK(connection->connection_state,
                    (ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED | ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING |
                     ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT |
                     ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED |
                     ROBOTRACONTEURLITE_STATUS_FLAGS_BLOCK_SEND)))
    {
        return ROBOTRACONTEURLITE_ERROR_RETRY;
    }

    if (robotraconteurlite_buffer_init_scalar(&buffer_storage->buffer_vec[0], connection->send_buffer,
                                              connection->send_buffer_len) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    if (robotraconteurlite_message_writer_init(message_writer, buffer_storage, 0U, connection->send_buffer_pos) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_connection_end_send_message(
    struct robotraconteurlite_connection* connection, size_t message_len)
{
    connection->send_message_len = message_len;
    FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED);

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_connection_abort_send_message(
    struct robotraconteurlite_connection* connection)
{
    ROBOTRACONTEURLITE_UNUSED(connection);
    /* Don't need to do anything, for future use */
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_connection_close(struct robotraconteurlite_connection* connection)
{
    if (FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE))
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED);

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

struct robotraconteurlite_connection* robotraconteurlite_connections_init_from_array(
    struct robotraconteurlite_connection connections_fixed_storage[], size_t connections_fixed_storage_len,
    uint8_t buffers[], size_t buffer_size, size_t buffer_count)
{
    size_t i = 0U;
    assert(connections_fixed_storage_len > 0U);
    assert(buffer_count >= (connections_fixed_storage_len * 2U));
    assert(buffer_size > 1024U);

    for (i = 0; i < connections_fixed_storage_len; i++)
    {
        (void)memset(&connections_fixed_storage[i], 0, sizeof(struct robotraconteurlite_connection));
        connections_fixed_storage[i].recv_buffer = &buffers[(i * 2U * buffer_size)];
        connections_fixed_storage[i].send_buffer = &buffers[(i * 2U * buffer_size) + buffer_size];
        connections_fixed_storage[i].recv_buffer_len = buffer_size;
        connections_fixed_storage[i].send_buffer_len = buffer_size;

        if (i > 0U)
        {
            connections_fixed_storage[i - 1U].next = &connections_fixed_storage[i];
            connections_fixed_storage[i].prev = &connections_fixed_storage[i - 1U];
        }
    }

    return &connections_fixed_storage[0];
}

robotraconteurlite_status robotraconteurlite_connection_next_wake(struct robotraconteurlite_connection* connection,
                                                                  robotraconteurlite_timespec now,
                                                                  robotraconteurlite_timespec* next_wake)
{
    if (FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE))
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (FLAGS_CHECK(connection->connection_state,
                    (ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED | ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR)))
    {
        *next_wake = now;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT))
    {
        *next_wake = now;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED))
    {
        *next_wake = now;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (connection->heartbeat_next_check_ms > 0)
    {
        if ((now > connection->heartbeat_next_check_ms) ||
            ((connection->heartbeat_next_check_ms == 0) && (connection->heartbeat_period_ms > 0)))
        {
            *next_wake = now;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }

        if (connection->heartbeat_next_check_ms < *next_wake)
        {
            *next_wake = connection->heartbeat_next_check_ms;
        }
    }

    if (connection->transport_next_wake > 0)
    {
        if (connection->transport_next_wake < *next_wake)
        {
            *next_wake = connection->transport_next_wake;
        }
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
