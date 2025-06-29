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

#ifndef ROBOTRACONTEURLITE_TCP_TRANSPORT_H
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_H

#include "robotraconteurlite/node.h"

#define ROBOTRACONTEURLITE_TCP_TRANSPORT 0x10001U

/* robotraconteurlite_tcp_transport_state */
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_NULL 0U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_STARTED 0x1U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IN_HTTP_HEADER 0x2U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_HTTP_LINE_EMPTY 0x4U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_HTTP_IS_SEC_WEBSOCKET_KEY_LINE 0x8U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_IS_WEBSOCKET 0x10U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_IN_FRAME 0x20U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_ENABLE_MASK 0x40U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_HEADER1_RECV 0x80U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_RECV_WEBSOCKET_HEADER2_RECV 0x100U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_STATE_SEND_WEBSOCKET_ENABLE_MASK 0x200U

/* ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_OPCODE */
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_CONTINUATION 0x0U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_TEXT 0x1U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_BINARY 0x2U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_CLOSE 0x8U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_PING 0x9U
#define ROBOTRACONTEURLITE_TCP_TRANSPORT_WEBSOCKET_FLAGS_PONG 0xAU

#ifdef __cplusplus
extern "C" {
#endif

struct robotraconteurlite_tcp_sha1_storage
{
    robotraconteurlite_u8 sha1_bytes[20];
};

struct robotraconteurlite_tcp_transport_storage
{
    robotraconteurlite_u32 tcp_transport_state;
    robotraconteurlite_size_t http_line_pos;
    robotraconteurlite_size_t recv_websocket_frame_pos;
    robotraconteurlite_size_t recv_websocket_frame_len;
    robotraconteurlite_size_t recv_websocket_header_pos;
    robotraconteurlite_size_t send_websocket_header_pos;
    robotraconteurlite_u8 recv_websocket_mask[4];
    robotraconteurlite_u8 recv_websocket_header_buffer[16];
    robotraconteurlite_u8 send_websocket_header_buffer[8];
    robotraconteurlite_size_t send_websocket_frame_len;
    robotraconteurlite_size_t send_websocket_frame_buffer_pos;
    robotraconteurlite_size_t send_websocket_frame_buffer_end;
    robotraconteurlite_size_t send_websocket_header_len;
    robotraconteurlite_u8 send_websocket_mask[4];
};

struct robotraconteurlite_tcp_connect_service_data
{
    struct robotraconteurlite_connection* connections_head;
    struct robotraconteurlite_addr* service_address;
    robotraconteurlite_timespec now;
    struct robotraconteurlite_connection* client_out;
};

struct sockaddr_storage;

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_acceptor_listen(
    struct robotraconteurlite_connection_acceptor* acceptor, const struct sockaddr_storage* serv_addr, int backlog);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_tcp_acceptor_close(struct robotraconteurlite_connection_acceptor* acceptor);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_acceptor_communicate(
    struct robotraconteurlite_connection_acceptor* acceptor, struct robotraconteurlite_connection* connection_head,
    robotraconteurlite_timespec now);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_acceptor_poll_add_fd(
    struct robotraconteurlite_connection_acceptor* acceptor, struct robotraconteurlite_connection* connection_head,
    struct robotraconteurlite_pollfd* pollfds, robotraconteurlite_size_t* pollfd_count, robotraconteurlite_size_t max_pollfds);

ROBOTRACONTEURLITE_API void robotraconteurlite_tcp_connection_close(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_API void robotraconteurlite_tcp_connections_close(
    struct robotraconteurlite_connection* connections_head);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_tcp_connection_buffer_recv(struct robotraconteurlite_connection* connection, robotraconteurlite_size_t len);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_connection_communicate_recv(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_tcp_connection_buffer_send(struct robotraconteurlite_connection* connection, robotraconteurlite_size_t len);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_connection_communicate_send(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_tcp_connection_handshake(struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_connection_communicate(
    struct robotraconteurlite_connection* connection, robotraconteurlite_timespec now);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_connections_communicate(
    struct robotraconteurlite_connection* connections_head, robotraconteurlite_timespec now);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_connect_service(
    struct robotraconteurlite_tcp_connect_service_data* connect_data, robotraconteurlite_timespec now);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_connection_poll_add_fd(
    struct robotraconteurlite_connection* connection, struct robotraconteurlite_pollfd* pollfds, robotraconteurlite_size_t* pollfd_count,
    robotraconteurlite_size_t max_pollfds);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_connections_poll_add_fds(
    struct robotraconteurlite_connection* connection_head, struct robotraconteurlite_pollfd* pollfds,
    robotraconteurlite_size_t* pollfd_count, robotraconteurlite_size_t max_pollfds);

/* TCP socket functions */

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_tcp_sha1(const robotraconteurlite_u8* data, robotraconteurlite_size_t len, struct robotraconteurlite_tcp_sha1_storage* storage);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_base64_encode(const robotraconteurlite_u8* binary_data,
                                                                                      robotraconteurlite_size_t binary_len,
                                                                                      char* base64_data,
                                                                                      robotraconteurlite_size_t* base64_len);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_socket_recv_nonblocking(
    ROBOTRACONTEURLITE_SOCKET sock, robotraconteurlite_u8* buffer, robotraconteurlite_size_t* pos, robotraconteurlite_size_t len, int* errno_out);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_socket_send_nonblocking(
    ROBOTRACONTEURLITE_SOCKET sock, const robotraconteurlite_u8* buffer, robotraconteurlite_size_t* pos, robotraconteurlite_size_t len, int* errno_out);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_socket_begin_server(
    const struct sockaddr_storage* serv_addr, robotraconteurlite_size_t backlog, ROBOTRACONTEURLITE_SOCKET* sock_out, int* errno_out);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_socket_accept(
    ROBOTRACONTEURLITE_SOCKET acceptor_sock, ROBOTRACONTEURLITE_SOCKET* client_sock, int* errno_out);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_socket_close(ROBOTRACONTEURLITE_SOCKET sock);

ROBOTRACONTEURLITE_API robotraconteurlite_u16 robotraconteurlite_ntohs(robotraconteurlite_u16 netshort);

ROBOTRACONTEURLITE_API robotraconteurlite_u16 robotraconteurlite_htons(robotraconteurlite_u16 hostshort);

ROBOTRACONTEURLITE_API robotraconteurlite_u64 robotraconteurlite_be64toh(robotraconteurlite_u64 big_endian_64bits);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_tcp_socket_connect(
    struct robotraconteurlite_sockaddr_storage* addr, ROBOTRACONTEURLITE_SOCKET* sock_out, int* errno_out);

#ifdef __cplusplus
}
#endif

#endif /*ROBOTRACONTEURLITE_TCP_TRANSPORT_H*/
