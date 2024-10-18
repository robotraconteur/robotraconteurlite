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

#ifndef ROBOTRACONTEURLITE_CONNECTION_H
#define ROBOTRACONTEURLITE_CONNECTION_H

#include "robotraconteurlite/message.h"
#include "robotraconteurlite/clock.h"
#include "robotraconteurlite/util.h"

/* robotraconteurlite_connection_config_flags */
#define ROBOTRACONTEURLITE_CONFIG_FLAGS_NULL 0U
#define ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER 0x1U
#define ROBOTRACONTEURLITE_CONFIG_FLAGS_ENABLE_REDUCED_HEADER4 0x2U

/* robotraconteurlite_connection_status_flags */
#define ROBOTRACONTEURLITE_STATUS_FLAGS_NULL 0U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE 0x1U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED 0x2U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTING 0x4U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSING 0x8U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED 0x10U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR 0x20U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVE_REQUESTED 0x40U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_RECEIVING 0x80U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_REQUESTED 0x100U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_SENDING 0x200U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED 0x400U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED 0x800U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_BLOCK_SEND 0x1000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT 0x2000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_ESTABLISHED 0x4000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CLIENT_ESTABLISHED 0x8000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED 0x10000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED 0x20000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED 0x40000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED_CONSUMED 0x80000U
#define ROBOTRACONTEURLITE_STATUS_FLAGS_SEND_MESSAGE4 0x100000U

/* transport_capability_flags */
#define ROBOTRACONTEURLITE_TRANSPORT_CAPABILITY_CODE_PAGE_MASK 0xFFF00000U
#define ROBOTRACONTEURLITE_TRANSPORT_CAPABILITY_CODE_MESSAGE2_BASIC_PAGE 0x02000000U
#define ROBOTRACONTEURLITE_TRANSPORT_CAPABILITY_CODE_MESSAGE2_BASIC_ENABLE 0x00000001U
#define ROBOTRACONTEURLITE_TRANSPORT_CAPABILITY_CODE_MESSAGE4_BASIC_PAGE 0x04000000U
#define ROBOTRACONTEURLITE_TRANSPORT_CAPABILITY_CODE_MESSAGE4_BASIC_ENABLE 0x00000001U

#ifdef __cplusplus
extern "C" {
#endif

struct robotraconteurlite_transport_storage
{
    uint8_t _storage[128];
};

struct robotraconteurlite_user_storage
{
    void* user_data;
};

/* NOLINTNEXTLINE(clang-analyzer-optin.performance.Padding) */
struct robotraconteurlite_connection
{
    uint32_t transport_type;
    struct robotraconteurlite_connection* next;
    struct robotraconteurlite_connection* prev;

    /* Socket storage */
    ROBOTRACONTEURLITE_SOCKET sock;

    /* Send and receive buffers */
    uint8_t* send_buffer;
    uint8_t* recv_buffer;
    size_t send_buffer_len;
    size_t recv_buffer_len;
    size_t send_buffer_pos;
    size_t recv_buffer_pos;

    /* Control flags */
    uint32_t config_flags;
    uint32_t connection_state;

    /* Parameters */
    int32_t heartbeat_period_ms;
    int32_t heartbeat_timeout_ms;
    robotraconteurlite_timespec heartbeat_next_check_ms;

    /* Robot Raconteur information */
    uint32_t local_endpoint;
    uint32_t remote_endpoint;
    struct robotraconteurlite_nodeid remote_nodeid;
    char remote_nodename_char[128];
    struct robotraconteurlite_string remote_nodename;
    char remote_service_name_char[128];
    struct robotraconteurlite_string remote_service_name;
    robotraconteurlite_timespec last_recv_message_time;
    robotraconteurlite_timespec last_send_message_time;
    uint32_t last_request_id;
    uint8_t message_flags_inv_mask;

    /* Message information */
    uint32_t recv_message_len;
    uint32_t send_message_len;

    /* Transport storage */
    struct robotraconteurlite_transport_storage transport_storage;

    /* User storage */
    struct robotraconteurlite_user_storage user_storage;

    /* Transport next wake request time */
    robotraconteurlite_timespec transport_next_wake;
};

struct robotraconteurlite_connection_acceptor
{
    int32_t id;
    uint32_t transport_type;
    struct robotraconteurlite_connection_acceptor* next;
    struct robotraconteurlite_connection_acceptor* prev;

    /* Socket storage */
    ROBOTRACONTEURLITE_SOCKET sock;
};

struct robotraconteurlite_sockaddr_storage
{
    uint8_t _storage[128];
};

/* robotraconteurlite_addr_flags */
#define ROBOTRACONTEURLITE_ADDR_FLAGS_NULL 0x0U
#define ROBOTRACONTEURLITE_ADDR_FLAGS_WEBSOCKET 0x1U

struct robotraconteurlite_addr
{
    uint32_t transport_type;
    struct robotraconteurlite_sockaddr_storage socket_addr;
    struct robotraconteurlite_nodeid nodeid;
    struct robotraconteurlite_string nodename;
    struct robotraconteurlite_string service_name;
    uint32_t flags;
    struct robotraconteurlite_string http_host;
    struct robotraconteurlite_string http_path;
};

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_connection_reset(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_connection_verify_preamble(struct robotraconteurlite_connection* connection, uint32_t* message_len);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_begin_send_message(
    struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_writer* message_writer,
    struct robotraconteurlite_buffer_vec* buffer_storage);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_connection_end_send_message(struct robotraconteurlite_connection* connection, size_t message_len);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_connection_abort_send_message(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_message_receive(
    struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_reader* message_reader,
    struct robotraconteurlite_buffer_vec* buffer_storage);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_connection_message_receive_consume(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_connection_close(struct robotraconteurlite_connection* connection);

static int robotraconteurlite_connection_is_closed(struct robotraconteurlite_connection* connection)
{
    return ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED);
}

static int robotraconteurlite_connection_is_closed_event(struct robotraconteurlite_connection* connection)
{
    return (ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED)) &&
           (!ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state,
                                            ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED));
}

static void robotraconteurlite_connection_consume_closed(struct robotraconteurlite_connection* connection)
{
    ROBOTRACONTEURLITE_FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED_CONSUMED);
}

static int robotraconteurlite_connection_is_error(struct robotraconteurlite_connection* connection)
{
    /* Check for error, but don't report if already closed or closing */
    return (ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR)) &&
           (!ROBOTRACONTEURLITE_FLAGS_CHECK(
               connection->connection_state,
               (ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSED | ROBOTRACONTEURLITE_STATUS_FLAGS_CLOSE_REQUESTED)));
}

static int robotraconteurlite_connection_is_connected(struct robotraconteurlite_connection* connection)
{
    return ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED);
}

static void robotraconteurlite_connection_consume_connected(struct robotraconteurlite_connection* connection)
{
    ROBOTRACONTEURLITE_FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED_CONSUMED);
}

static int robotraconteurlite_connection_is_connected_event(struct robotraconteurlite_connection* connection)
{
    return (ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED)) &&
           (!ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state,
                                            ROBOTRACONTEURLITE_STATUS_FLAGS_CONNECTED_CONSUMED));
}

static int robotraconteurlite_connection_is_message_received(struct robotraconteurlite_connection* connection)
{
    return ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state,
                                          ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED);
}

static int robotraconteurlite_connection_is_message_received_event(struct robotraconteurlite_connection* connection)
{
    return (ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state,
                                           ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_RECEIVED)) &&
           (!ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state,
                                            ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED));
}

static void robotraconteurlite_connection_consume_message_received(struct robotraconteurlite_connection* connection)
{
    ROBOTRACONTEURLITE_FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_CONSUMED);
}

static int robotraconteurlite_connection_is_message_sent(struct robotraconteurlite_connection* connection)
{
    return ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT);
}

static int robotraconteurlite_connection_is_message_sent_event(struct robotraconteurlite_connection* connection)
{
    return (ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state,
                                           ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT)) &&
           (!ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state,
                                            ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED));
}

static void robotraconteurlite_connection_consume_message_sent(struct robotraconteurlite_connection* connection)
{
    ROBOTRACONTEURLITE_FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_MESSAGE_SENT_CONSUMED);
}

static void robotraconteurlite_connection_error(struct robotraconteurlite_connection* connection)
{
    ROBOTRACONTEURLITE_FLAGS_SET(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_ERROR);
}

static int robotraconteurlite_connection_is_server(struct robotraconteurlite_connection* connection)
{
    return ROBOTRACONTEURLITE_FLAGS_CHECK(connection->config_flags, ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER);
}

ROBOTRACONTEURLITE_API struct robotraconteurlite_connection* robotraconteurlite_connections_init_from_array(
    struct robotraconteurlite_connection connections_fixed_storage[], size_t connections_fixed_storage_len,
    uint8_t buffers[], size_t buffer_size, size_t buffer_count);

static int robotraconteurlite_connection_is_heartbeat_timeout(struct robotraconteurlite_connection* connection,
                                                              robotraconteurlite_timespec now)
{
    int64_t recv_diff_ms = now - connection->last_recv_message_time;
    int64_t send_diff_ms = now - connection->last_send_message_time;

    if ((recv_diff_ms > connection->heartbeat_timeout_ms) || (send_diff_ms > connection->heartbeat_timeout_ms))
    {
        return 2;
    }

    if ((recv_diff_ms > connection->heartbeat_period_ms) || (send_diff_ms > connection->heartbeat_period_ms))
    {
        return 1;
    }

    return 0;
}

static int robotraconteurlite_connection_is_idle(struct robotraconteurlite_connection* connection)
{
    return ROBOTRACONTEURLITE_FLAGS_CHECK(connection->connection_state, ROBOTRACONTEURLITE_STATUS_FLAGS_IDLE);
}

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_connection_next_wake(struct robotraconteurlite_connection* connection,
                                        robotraconteurlite_timespec now, robotraconteurlite_timespec* next_wake);

ROBOTRACONTEURLITE_API struct robotraconteurlite_connection* robotraconteurlite_connection_find_idle(
    struct robotraconteurlite_connection* connections_head);

ROBOTRACONTEURLITE_API void robotraconteurlite_connection_init(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_API void robotraconteurlite_connection_init_connections(
    struct robotraconteurlite_connection* connections_head);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_communicate(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now, uint32_t transport_type,
    uint8_t* close_request);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_communicate_after_close_requested(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now,
    robotraconteurlite_status close_rv);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_communicate_recv1(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now, size_t* recv_op_len);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_communicate_recv2(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now,
    robotraconteurlite_status recv_op_rv);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_communicate_send1(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now, size_t* send_op_len);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_communicate_send2(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now,
    robotraconteurlite_status send_op_rv);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_connect2(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now, uint32_t transport_type,
    const struct robotraconteurlite_addr* addr, ROBOTRACONTEURLITE_SOCKET sock);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_connection_impl_accept2(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now, uint32_t transport_type,
    ROBOTRACONTEURLITE_SOCKET sock);

#ifdef __cplusplus
}
#endif

#endif /*ROBOTRACONTEURLITE_CONNECTION_H*/
