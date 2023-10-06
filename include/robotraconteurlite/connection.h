/* Copyright 2011-2023 Wason Technology, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ROBOTRACONTEURLITE_CONNECTION_H__
#define __ROBOTRACONTEURLITE_CONNECTION_H__

#include "robotraconteurlite/message.h"

enum robotraconteurlite_connection_config_flags
{
    ROBOTRACONTEURLITE_CONFIG_FLAGS_NULL = 0,
    ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER = 0x1
};

enum robotraconteurlite_connection_status_flags
{
    ROBOTRACONTEURLITE_STATUS_FLAGS_NULL = 0,
    ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE = 0x1,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED = 0x2,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING = 0x4,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSING = 0x8,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED = 0x10,
    ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR = 0x20,
    ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED = 0x40,
    ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVING = 0x80,
    ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED = 0x100,
    ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING = 0x200,
    ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED = 0x400,
    ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED = 0x800,
    ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING_MESSAGE = 0x1000,
    ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT = 0x2000,
    ROBOTRACONTEURLITE_STATUS_FLAGS_ESTABLISHED = 0x4000,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CLIENT_ESTABLISHED = 0x8000,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED = 0x10000,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED = 0x20000,
    ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED = 0x40000,
    ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED_CONSUMED = 0x80000
};

struct robotraconteurlite_transport_storage
{
    uint8_t _storage[32];
};

struct robotraconteurlite_user_storage
{
  void* user_data;
};

struct robotraconteurlite_connection
{
    uint32_t transport_type;
    struct robotraconteurlite_connection* next;
    struct robotraconteurlite_connection* prev;

    /* Socket storage */
    int sock;

    /* Send and receive buffers */
    uint8_t* send_buffer;
    uint8_t* recv_buffer;
    uint32_t send_buffer_len;
    uint32_t recv_buffer_len;
    uint32_t send_buffer_pos;
    uint32_t recv_buffer_pos;

    /* Control flags */
    uint32_t config_flags;
    uint32_t connection_state;

    /* Robot Raconteur information */
    uint32_t local_endpoint;
    uint32_t remote_endpoint;
    struct robotraconteurlite_nodeid remote_nodeid;
    char remote_nodename_char[128];
    struct robotraconteurlite_string remote_nodename;

    /* Message information */
    uint32_t recv_message_len;
    uint32_t send_message_len;

    /* Transport storage */
    struct robotraconteurlite_transport_storage transport_storage;

    /* User storage */
    struct robotraconteurlite_user_storage user_storage;
};

struct robotraconteurlite_connection_acceptor
{
    int32_t id;
    uint32_t transport_type;
    struct robotraconteurlite_connection_acceptor* next;
    struct robotraconteurlite_connection_acceptor* prev;

    /* Socket storage */
    int sock;
};

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_reset(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_verify_preamble(struct robotraconteurlite_connection* connection, uint32_t* message_len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_begin_send_message(struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_writer* message_writer);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_end_send_message(struct robotraconteurlite_connection* connection, size_t message_len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_abort_send_message(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_message_receive(struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_reader* message_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_message_receive_consume(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_close(struct robotraconteurlite_connection* connection);

static int robotraconteurlite_connection_is_closed(struct robotraconteurlite_connection* connection)
{
    return (connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED) != 0;
}

static int robotraconteurlite_connection_is_closed_event(struct robotraconteurlite_connection* connection)
{
    return ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED) != 0)
        && ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED) == 0);
}

static void robotraconteurlite_connection_consume_closed(struct robotraconteurlite_connection* connection)
{
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED;
}

static int robotraconteurlite_connection_is_error(struct robotraconteurlite_connection* connection)
{
    /* Check for error, but don't report if already closed or closing */
    return (connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR != 0) &&    
        (connection->connection_state & (ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED | ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED)) == 0;
}

static int robotraconteurlite_connection_is_connected(struct robotraconteurlite_connection* connection)
{
    return (connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED) != 0;
}

static void robotraconteurlite_connection_consume_connected(struct robotraconteurlite_connection* connection)
{
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED_CONSUMED;
}

static int robotraconteurlite_connection_is_connected_event(struct robotraconteurlite_connection* connection)
{
    return ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED) != 0)
        && ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED_CONSUMED) == 0);
}

static int robotraconteurlite_connection_is_message_received(struct robotraconteurlite_connection* connection)
{
    return (connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED) != 0;
}

static int robotraconteurlite_connection_is_message_received_event(struct robotraconteurlite_connection* connection)
{
    return ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED) != 0)
        && ((connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED) == 0);
}

static void robotraconteurlite_connection_consume_message_received(struct robotraconteurlite_connection* connection)
{
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED;
}

static void robotraconteurlite_connection_error(struct robotraconteurlite_connection* connection)
{
    connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR;
}

static int robotraconteurlite_connection_is_server(struct robotraconteurlite_connection* connection)
{
    return (connection->config_flags & ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER) != 0;
}

#endif /*__ROBOTRACONTEURLITE_CONNECTION_H__*/
