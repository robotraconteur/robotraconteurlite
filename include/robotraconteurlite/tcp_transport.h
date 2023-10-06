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

#ifndef __ROBOTRACONTEURLITE_TCP_TRANSPORT_H__
#define __ROBOTRACONTEURLITE_TCP_TRANSPORT_H__

#include "robotraconteurlite/node.h"

#define ROBOTRACONTEURLITE_TCP_TRANSPORT 0x10001

enum robotraconteurlite_tcp_transport_state
{
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_NULL = 0,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_STARTED = 0x1,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IN_HTTP_HEADER = 0x2,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_HTTP_LINE_EMPTY = 0x4,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_HTTP_IS_SEC_WEBSOCKET_KEY_LINE = 0x8,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET = 0x10,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME = 0x20,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_ENABLE_MASK = 0x40,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_HEADER1_RECV = 0x80,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_HEADER2_RECV = 0x100,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_SEND_WEBSOCKET_ENABLE_MASK = 0x40
};

enum ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_OPCODE
{
    ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_CONTINUATION = 0x0,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_TEXT = 0x1,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_BINARY = 0x2,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_CLOSE = 0x8,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_PING = 0x9,
    ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_PONG = 0xA
};

struct robotraconteurlite_tcp_sha1_storage
{
    uint8_t sha1_bytes[20];
};

struct robotraconteurlite_tcp_transport_storage
{
    uint32_t tcp_transport_state;
    uint32_t http_line_pos;
    uint32_t recv_websocket_frame_pos;
    uint32_t recv_websocket_frame_len;
    uint32_t recv_websocket_header_pos;
    uint32_t send_websocket_header_pos;
    uint8_t recv_websocket_mask[4];
    uint8_t recv_websocket_header_buffer[16];
    uint8_t send_websocket_header_buffer[8];
    uint32_t send_websocket_frame_len;
    uint32_t send_websocket_frame_buffer_pos;
    uint32_t send_websocket_frame_buffer_end;
    uint32_t send_websocket_header_len;
    uint8_t send_websocket_mask[4];
};

struct sockaddr_storage;

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_acceptor_listen(struct robotraconteurlite_connection_acceptor* acceptor, const struct sockaddr_storage* serv_addr, int backlog);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_acceptor_close(struct robotraconteurlite_connection_acceptor* acceptor);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_acceptor_communicate(struct robotraconteurlite_connection_acceptor* acceptor, struct robotraconteurlite_connection* connection_head);

ROBOTRACONTEURLITE_DECL void robotraconteurlite_tcp_connection_init_connection(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL void robotraconteurlite_tcp_connection_init_connections(struct robotraconteurlite_connection* connections_head);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_connection_buffer_recv(struct robotraconteurlite_connection* connection, size_t len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_connection_communicate_recv(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_connection_buffer_send(struct robotraconteurlite_connection* connection, size_t len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_connection_communicate_send(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_connection_handshake(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_connection_communicate(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_connections_communicate(struct robotraconteurlite_connection* connections_head);

/* TCP socket functions */

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_sha1(const uint8_t* data, size_t len, struct robotraconteurlite_tcp_sha1_storage* storage);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_base64_encode(const uint8_t* binary_data, size_t binary_len, char* base64_data, size_t* base64_len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_socket_recv_nonblocking(int sock, uint8_t* buffer, uint32_t* pos, size_t len, int* errno_out);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_socket_send_nonblocking(int sock, const uint8_t* buffer, uint32_t* pos, size_t len, int* errno_out);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_socket_begin_server(const struct sockaddr_storage* serv_addr, size_t backlog, int* sock_out, int* errno_out);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_socket_accept(int acceptor_sock, int* client_sock, int* errno_out);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_tcp_socket_close(int sock);

ROBOTRACONTEURLITE_DECL uint16_t robotraconteurlite_ntohs(uint16_t netshort);

ROBOTRACONTEURLITE_DECL uint16_t robotraconteurlite_htons(uint16_t hostshort);

ROBOTRACONTEURLITE_DECL uint64_t robotraconteurlite_be64toh(uint64_t big_endian_64bits);

#endif /*__ROBOTRACONTEURLITE_TCP_TRANSPORT_H__*/
