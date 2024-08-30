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

#ifndef _XOPEN_SOURCE
/* NOLINTNEXTLINE(bugprone-reserved-identifier) */
#define _XOPEN_SOURCE 500
#endif
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <robotraconteurlite/robotraconteurlite.h>

#define NUM_CONNECTIONS 1
#define CONNECTION_BUFFER_SIZE 8096

/* #define TINY_CLIENT_WEBSOCKET 1 */

const uint16_t service_port = 22229;
const char* service_ip_str = "127.0.0.1";
const char* service_name = "tiny_service";

enum tiny_client_state
{
    TINY_CLIENT_STATE_INIT = 0,
    TINY_CLIENT_STATE_GET_D1_SENT = 1,
    TINY_CLIENT_STATE_GET_D1_RECEIVED = 2,
    TINY_CLIENT_STATE_SET_D1_SENT = 3,
    TINY_CLIENT_STATE_SET_D1_RECEIVED = 4,
    TINY_CLIENT_STATE_DONE = 5,
    TINY_CLIENT_STATE_ERROR = 1000
};

int main(int argc, char* argv[])
{
    /* Variable storage */
    struct robotraconteurlite_connection connections_storage[NUM_CONNECTIONS];
    uint8_t connection_buffers[NUM_CONNECTIONS * 2 * CONNECTION_BUFFER_SIZE];
    struct robotraconteurlite_connection* connections_head = NULL;
    struct robotraconteurlite_connection* connection = NULL;
    struct robotraconteurlite_node node;
    struct robotraconteurlite_nodeid node_id;
    struct robotraconteurlite_addr service_addr;
    struct sockaddr_in* service_sockaddr = NULL;
    struct robotraconteurlite_tcp_connect_service_data connect_data;
    robotraconteurlite_timespec now = 0;
    struct robotraconteurlite_clock rr_clock;
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_event event;
    struct robotraconteurlite_string nodename_str;
    uint64_t end_time = 0;
    enum tiny_client_state state = TINY_CLIENT_STATE_INIT;
    struct robotraconteurlite_node_send_messageentry_data request_data;

    double d1_set_val = 42.2;

    ROBOTRACONTEURLITE_UNUSED(argc);
    ROBOTRACONTEURLITE_UNUSED(argv);

    /* Disable sigpipe. This is a common source of errors. Some libraries will disable this for you, but not all. */
    /* robotraconteurlite does not automatically disable sigpipe. */
    signal(SIGPIPE, SIG_IGN);

    /* Seed rand with the current time */
    srand(time(NULL));

    /* Initialize the clock */
    robotraconteurlite_clock_init(&rr_clock);

    /* Use a random nodeid. In practice this should be fixed and unique for every service */
    robotraconteurlite_nodeid_newrandom(&node_id);

    /* Initialize connections and TCP transport */
    connections_head =
        robotraconteurlite_connections_init_from_array(connections_storage, NUM_CONNECTIONS, connection_buffers,
                                                       CONNECTION_BUFFER_SIZE, (size_t)(NUM_CONNECTIONS * 2));
    if (!connections_head)
    {
        printf("Could not initialize connections\n");
        return -1;
    }

    robotraconteurlite_tcp_connection_init_connections_client(connections_head);

    /* Initialize the node */
    robotraconteurlite_string_from_c_str("tiny_client", &nodename_str);
    robotraconteurlite_node_init(&node, &node_id, &nodename_str, connections_head);

    /* Connect to the service */
    memset(&service_addr, 0, sizeof(service_addr));
    robotraconteurlite_string_from_c_str(service_name, &service_addr.service_name);
    service_sockaddr = (struct sockaddr_in*)&service_addr.socket_addr;
    service_sockaddr->sin_family = AF_INET;
    service_sockaddr->sin_port = robotraconteurlite_htons(service_port);
    if (inet_pton(AF_INET, service_ip_str, &service_sockaddr->sin_addr) != 1)
    {
        printf("Could not convert service IP address\n");
        return -1;
    }
#if TINY_CLIENT_WEBSOCKET
    /* Use websocket connection */
    service_addr.flags |= ROBOTRACONTEURLITE_ADDR_FLAGS_WEBSOCKET;
    robotraconteurlite_string_from_c_str("127.0.0.1", &service_addr.http_host);
    robotraconteurlite_string_from_c_str("/", &service_addr.http_path);
#endif

    memset(&connect_data, 0, sizeof(connect_data));
    connect_data.connections_head = connections_head;
    connect_data.service_address = &service_addr;

    printf("Begin connecting to service\n");

    robotraconteurlite_clock_gettime(&rr_clock, &now);
    rv = robotraconteurlite_tcp_connect_service(&connect_data, now);

    if (rv)
    {
        printf("Could not connect to service\n");
        return -1;
    }

    connection = connect_data.client_out;
    {
        struct robotraconteurlite_client_handshake_data handshake_data;
        memset(&handshake_data, 0, sizeof(handshake_data));
        handshake_data.node = &node;
        handshake_data.connection = connection;
        while (1)
        {

            robotraconteurlite_clock_gettime(&rr_clock, &now);
            robotraconteurlite_tcp_connection_communicate(connection, now);
            rv = robotraconteurlite_node_next_event(&node, &event, now);
            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not get next event\n");
                return -1;
            }
            rv = robotraconteurlite_client_handshake(&handshake_data, &event, now);
            if (rv == ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                break;
            }
            if (rv == ROBOTRACONTEURLITE_ERROR_RETRY)
            {
                usleep(1000);
                continue;
            }
            printf("Could not handshake with service\n");
            return -1;
        }
    }

    printf("Connection to service complete!\n");

    end_time = now + 30000;

    while (now < end_time && state != TINY_CLIENT_STATE_ERROR && state != TINY_CLIENT_STATE_DONE)
    {

        /* Run client */
        switch (state)
        {
        case TINY_CLIENT_STATE_INIT: {
            memset(&request_data, 0, sizeof(request_data));
            printf("Sending get_d1\n");
            request_data.node = &node;
            request_data.connection = connection;
            rv = robotraconteurlite_client_send_empty_request(
                &request_data, ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYGETREQ, "d1", NULL);
            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                if (rv == ROBOTRACONTEURLITE_ERROR_RETRY)
                {
                    printf("Could not send get_d1, retrying\n");
                    continue;
                }
                printf("Could not send get_d1\n");
                return -1;
            }
            state = TINY_CLIENT_STATE_GET_D1_SENT;
            break;
        }
        case TINY_CLIENT_STATE_GET_D1_RECEIVED: {
            struct robotraconteurlite_string element_name;
            memset(&request_data, 0, sizeof(request_data));
            printf("Sending set_d1\n");
            request_data.node = &node;
            request_data.connection = connection;
            rv = robotraconteurlite_client_begin_request(
                &request_data, ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYSETREQ, "d1", NULL);

            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                if (rv == ROBOTRACONTEURLITE_ERROR_RETRY)
                {
                    printf("Could not send set_d1, retrying\n");
                    continue;
                }
                printf("Could not send set_d1\n");
                return -1;
            }
            /* Write d1 value to the message */
            robotraconteurlite_string_from_c_str("value", &element_name);
            if (robotraconteurlite_messageelement_writer_write_double(&request_data.element_writer, &element_name,
                                                                      d1_set_val))
            {
                printf("Could not write double\n");
                return -1;
            }

            rv = robotraconteurlite_client_send_request(&request_data);
            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                if (rv == ROBOTRACONTEURLITE_ERROR_RETRY)
                {
                    printf("Could not send set_d1, retrying\n");
                    continue;
                }
                printf("Could not send set_d1\n");
                return -1;
            }

            state = TINY_CLIENT_STATE_SET_D1_SENT;
            break;
        }
        case TINY_CLIENT_STATE_SET_D1_RECEIVED: {
            printf("Client communication done!\n");
            state = TINY_CLIENT_STATE_DONE;
            break;
        }
        default: {
            break;
        }
        }

        do
        {
            robotraconteurlite_clock_gettime(&rr_clock, &now);
            rv = robotraconteurlite_tcp_connections_communicate(connections_head, now);
            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not communicate with connections\n");
                return -1;
            }
            if (state == TINY_CLIENT_STATE_SET_D1_SENT || state == TINY_CLIENT_STATE_GET_D1_SENT)
            {
                /* Only expect to wait in these states */

                /* One socket per connection plus acceptor and node. May vary, check documentation */
                struct robotraconteurlite_pollfd pollfds[NUM_CONNECTIONS + 2];
                size_t num_pollfds = 0;
                robotraconteurlite_timespec next_wake = 0;
                rv = robotraconteurlite_node_next_wake(&node, now, &next_wake);
                if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                {
                    printf("Could not get next wake\n");
                    return -1;
                }

                if (next_wake > now)
                {
                    rv = robotraconteurlite_tcp_connections_poll_add_fds(connections_head, pollfds, &num_pollfds,
                                                                         NUM_CONNECTIONS + 1);
                    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                    {
                        printf("Could not add connections to poll\n");
                        return -1;
                    }
                    rv = robotraconteurlite_node_poll_add_fd(&node, pollfds, &num_pollfds, NUM_CONNECTIONS + 1);
                    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                    {
                        printf("Could not add node to poll\n");
                        return -1;
                    }

                    rv = robotraconteurlite_wait_next_wake(&rr_clock, pollfds, num_pollfds, next_wake);
                    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                    {
                        printf("Could not wait for next wake\n");
                        return -1;
                    }
                    robotraconteurlite_clock_gettime(&rr_clock, &now);
                    rv = robotraconteurlite_tcp_connections_communicate(connections_head, now);
                    if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                    {
                        printf("Could not communicate with connections\n");
                        return -1;
                    }
                }
            }
            /* get next event */

            rv = robotraconteurlite_node_next_event(&node, &event, now);
            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not get next event\n");
                return -1;
            }

            rv = robotraconteurlite_node_event_special_request(&node, &event);
            if (rv == ROBOTRACONTEURLITE_ERROR_CONSUMED)
            {
                continue;
            }

            if (rv != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not handle special request\n");
                return -1;
            }

            /* Handle event */
            switch (event.event_type)
            {
            case ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_RECEIVED: {
                switch (state)
                {
                case TINY_CLIENT_STATE_GET_D1_SENT: {
                    rv = robotraconteurlite_client_end_request(&request_data, &event);
                    switch (rv)
                    {
                    case ROBOTRACONTEURLITE_ERROR_SUCCESS: {
                        struct robotraconteurlite_messageelement_reader reader;
                        struct robotraconteurlite_string element_name;
                        double d1_val = 0.0;

                        printf("Received get_d1 response\n");
                        state = TINY_CLIENT_STATE_GET_D1_RECEIVED;

                        /* Read d1 value from the message */
                        robotraconteurlite_string_from_c_str("value", &element_name);
                        if (robotraconteurlite_messageentry_reader_find_element_verify_scalar(
                                &event.received_message.entry_reader, &element_name, &reader,
                                ROBOTRACONTEURLITE_DATATYPE_DOUBLE))
                        {
                            printf("Could not find element\n");
                            return -1;
                        }
                        if (robotraconteurlite_messageelement_reader_read_data_double(&reader, &d1_val))
                        {
                            printf("Could not read double\n");
                            return -1;
                        }

                        printf("Got d1 value: %f\n", d1_val);
                        break;
                    }
                    case ROBOTRACONTEURLITE_ERROR_UNHANDLED_EVENT: {
                        printf("Unexpected message received!\n");
                        break;
                    }
                    case ROBOTRACONTEURLITE_ERROR_REQUEST_REMOTE_ERROR: {
                        state = TINY_CLIENT_STATE_ERROR;
                        printf("Remote error received!\n");
                        break;
                    }
                    default: {
                        printf("Error ending request\n");
                        return -1;
                    }
                    }
                    break;
                }
                case TINY_CLIENT_STATE_SET_D1_SENT: {
                    rv = robotraconteurlite_client_end_request(&request_data, &event);
                    switch (rv)
                    {
                    case ROBOTRACONTEURLITE_ERROR_SUCCESS: {
                        printf("Received set_d1 response\n");
                        state = TINY_CLIENT_STATE_SET_D1_RECEIVED;
                        break;
                    }
                    case ROBOTRACONTEURLITE_ERROR_UNHANDLED_EVENT: {
                        printf("Unexpected message received!\n");
                        break;
                    }
                    case ROBOTRACONTEURLITE_ERROR_REQUEST_REMOTE_ERROR: {
                        state = TINY_CLIENT_STATE_ERROR;
                        printf("Remote error received!\n");
                        break;
                    }
                    default: {
                        printf("Error ending request\n");
                        return -1;
                    }
                    }
                    break;
                }
                default: {
                    printf("Unexpected message received!\n");
                    break;
                }
                }
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_ERROR: {
                robotraconteurlite_connection_close(event.connection);
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CLOSED: {
                printf("Connection error\n");
                robotraconteurlite_node_consume_event(&node, &event);
                return -1;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_HEARTBEAT_TIMEOUT: {
                printf("Connection heartbeat timeout, sending heartbeat\n");
                robotraconteurlite_client_send_heartbeat(&node, event.connection);
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_TIMEOUT: {
                printf("Connection timeout\n");
                /* close connection */
                robotraconteurlite_connection_close(event.connection);
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE: {
                /* Loop will be broken after next_cycle */
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            default: {
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            }
        } while (event.event_type != ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE);
    }

    robotraconteurlite_connection_close(connection);

    /* TODO: Drain the connection */
    robotraconteurlite_clock_gettime(&rr_clock, &now);
    robotraconteurlite_tcp_connection_communicate(connection, now);

    printf("Done!\n");

    return 0;
}
