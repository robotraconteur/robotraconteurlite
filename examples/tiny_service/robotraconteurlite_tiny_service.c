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

#include <robotraconteurlite/robotraconteurlite.h>

#define NUM_CONNECTIONS 4
#define CONNECTION_BUFFER_SIZE 8096

const char* node_name_str = "example.tiny_service";
const uint16_t node_port = 22228;

const char* service_name = "tiny_service";
const char* service_def = "service example.tiny_service\n\n"
                          "option version 0.10\n\n"
                          "object tiny_object\n"
                          "property double d1\n"
                          "end\n\n";
const char* service_def_qualified_name = "example.tiny_service";
const char* root_object_type = "example.tiny_service.tiny_object";

int handle_message(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event)
{
    int ret = robotraconteurlite_node_event_special_request(node, event);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        if (ret == ROBOTRACONTEURLITE_ERROR_CONSUMED || ret == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return 0;
        }
        printf("Could not handle special request\n");
        return -1;
    }

    switch (event->received_message.received_message_entry_header.entry_type)
    {
    case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETSERVICEDESC: {
        struct robotraconteurlite_node_service_definition service_def_s;
        struct robotraconteurlite_node_service_object service_obj_s;
        robotraconteurlite_string_from_c_str(service_def, &service_def_s.service_definition);
        robotraconteurlite_string_from_c_str(service_def_qualified_name, &service_def_s.qualified_name);
        robotraconteurlite_string_from_c_str(service_name, &service_obj_s.service_path);
        robotraconteurlite_string_from_c_str(root_object_type, &service_obj_s.qualified_type);
        service_obj_s.service_def = &service_def_s;

        return robotraconteurlite_node_event_special_request_service_definition(node, event, &service_obj_s, 1,
                                                                                &service_def_s, 1);
    }
    case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_OBJECTTYPENAME: {
        struct robotraconteurlite_node_service_object service_obj_s;
        robotraconteurlite_string_from_c_str(service_name, &service_obj_s.service_path);
        robotraconteurlite_string_from_c_str(root_object_type, &service_obj_s.qualified_type);
        return robotraconteurlite_node_event_special_request_object_type_name(node, event, &service_obj_s, 1);
    }
    case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CLIENTKEEPALIVEREQ: {
        int ret = robotraconteurlite_node_send_messageentry_empty_response(
            node, event->connection, &event->received_message.received_message_entry_header);
        if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return ROBOTRACONTEURLITE_ERROR_RETRY;
        }

        return ret;
    }
    case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_SERVICEPATHRELEASEDREQ: {
        /* Don't need to do anything. Consume and return */
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYGETREQ: {
        if (robotraconteurlite_event_is_member(event, "tiny_service", "d1"))
        {
            double d1 = 1.234;
            struct robotraconteurlite_node_send_messageentry_data send_data;
            struct robotraconteurlite_string element_name;
            int ret = -1;
            send_data.node = node;
            send_data.connection = event->connection;
            ret = robotraconteurlite_node_begin_send_messageentry_response(
                &send_data, &event->received_message.received_message_entry_header);
            if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
            {
                return ROBOTRACONTEURLITE_ERROR_RETRY;
            }
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not begin send message entry response\n");
                return -1;
            }

            robotraconteurlite_string_from_c_str("value", &element_name);
            if (robotraconteurlite_messageelement_writer_write_double(&send_data.element_writer, &element_name, d1))
            {
                printf("Could not write double\n");
                return -1;
            }
            ret = robotraconteurlite_node_end_send_messageentry(&send_data);
            if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
            {
                return ROBOTRACONTEURLITE_ERROR_RETRY;
            }
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not end send message entry response\n");
                return -1;
            }
            return ret;
        }
        else
        {
            if (robotraconteurlite_node_consume_event(node, event))
            {
                printf("Could not consume event\n");
                return -1;
            }
            printf("Unknown property get request, responding with error\n");
            /* Send error response */
            return robotraconteurlite_connection_send_messageentry_error_response(
                node, event->connection, &event->received_message.received_message_entry_header,
                ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDOPERATION, "RobotRaconteur.InvalidOperation",
                "Invalid operation");
        }
    }
    case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYSETREQ: {
        if (robotraconteurlite_event_is_member(event, "tiny_service", "d1"))
        {
            /* Find "value" message element */

            struct robotraconteurlite_string element_name;
            struct robotraconteurlite_messageelement_reader element_reader;
            int ret = -1;
            double d1 = 0.0;
            robotraconteurlite_string_from_c_str("value", &element_name);

            ret = robotraconteurlite_messageentry_reader_find_element_verify_scalar(
                &event->received_message.entry_reader, &element_name, &element_reader,
                ROBOTRACONTEURLITE_DATATYPE_DOUBLE);

            if (ret == ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_NOT_FOUND ||
                ret == ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH)
            {
                printf("Could not find element or type mismatch\n");
                /* Send error response */
                return robotraconteurlite_connection_send_messageentry_error_response(
                    node, event->connection, &event->received_message.received_message_entry_header,
                    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDOPERATION, "RobotRaconteur.InvalidOperation",
                    "Invalid operation");
            }

            if (robotraconteurlite_messageelement_reader_read_data_double(&element_reader, &d1))
            {
                printf("Could not read double\n");
                /* Send error response */
                return robotraconteurlite_connection_send_messageentry_error_response(
                    node, event->connection, &event->received_message.received_message_entry_header,
                    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDOPERATION, "RobotRaconteur.InvalidOperation",
                    "Invalid operation");
            }

            printf("Got set d1=%f\n", d1);

            /* Send empty response */
            ret = robotraconteurlite_node_send_messageentry_empty_response(
                node, event->connection, &event->received_message.received_message_entry_header);
            if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
            {
                return ROBOTRACONTEURLITE_ERROR_RETRY;
            }
            return ret;
        }
        else
        {
            printf("Unknown property set request, responding with error\n");

            /* Send error response */
            return robotraconteurlite_connection_send_messageentry_error_response(
                node, event->connection, &event->received_message.received_message_entry_header,
                ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDOPERATION, "RobotRaconteur.InvalidOperation",
                "Invalid operation");
        }
    }
    default: {
        printf("Could not handle message, responding with error\n");
        /* Send error response */
        return robotraconteurlite_connection_send_messageentry_error_response(
            node, event->connection, &event->received_message.received_message_entry_header,
            ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDOPERATION, "RobotRaconteur.InvalidOperation",
            "Invalid operation");
    }
    }

    return 0;
}

int handle_event(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event)
{
    switch (event->event_type)
    {
    case ROBOTRACONTEURLITE_EVENT_TYPE_NOOP: {
        printf("Noop event\n");
        /* Consume the event */
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE: {
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        /* End of cycle, exit loop */
        return 1;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_RECEIVED: {
        int ret = -1;
        printf("Message received\n");
        /* Handle the message */
        ret = handle_message(node, event);
        if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return 0;
        }
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            printf("Could not handle message\n");
            return -1;
        }
        /* Consume the event */
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_SEND_COMPLETE: {
        printf("Message sent\n");
        /* Consume the event */
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_HEARTBEAT_TIMEOUT: {
        /* TODO: Heartbeat timeout */
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_TIMEOUT: {
        printf("Connection timed out\n");
        /* Close the connection on timeout */
        if (robotraconteurlite_connection_close(event->connection))
        {
            printf("Could not close connection\n");
            return -1;
        }
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_ERROR: {
        /* Close the connection on error */
        if (robotraconteurlite_connection_close(event->connection))
        {
            printf("Could not close connection\n");
            return -1;
        }
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CONNECTED: {
        printf("Connection connected!\n");
        /* Consume the event */
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CLOSED: {
        printf("Connection closed!\n");
        /* Consume the event */
        if (robotraconteurlite_node_consume_event(node, event))
        {
            printf("Could not consume event\n");
            return -1;
        }
        break;
    }
    default: {
        printf("Unexpected event type\n");
        return -1;
    }
    }
    return 0;
}

volatile sig_atomic_t signal_received = 0;

void signal_handler(int signum)
{
    ROBOTRACONTEURLITE_UNUSED(signum);
    signal_received = 1;
}

int main(int argc, char* argv[])
{
    /* Variable storage */
    struct robotraconteurlite_connection connections_storage[NUM_CONNECTIONS];
    uint8_t connection_buffers[NUM_CONNECTIONS * 2 * CONNECTION_BUFFER_SIZE];
    struct robotraconteurlite_connection* connections_head = NULL;
    struct robotraconteurlite_connection_acceptor tcp_acceptor;
    struct robotraconteurlite_node node;
    struct sockaddr_in listen_addr;
    struct robotraconteurlite_nodeid node_id;
    struct robotraconteurlite_string node_name;
    struct robotraconteurlite_clock clock;
    robotraconteurlite_timespec now = 0;
    struct sigaction sa;

    ROBOTRACONTEURLITE_UNUSED(argc);
    ROBOTRACONTEURLITE_UNUSED(argv);

    /* Disable sigpipe. This is a common source of errors. Some libraries will disable this for you, but not all. */
    /* robotraconteurlite does not automatically disable sigpipe. */
    signal(SIGPIPE, SIG_IGN);

    /* Listen for SIGINT and SIGTERM to break loop */
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* Seed rand with the current time */
    srand(time(NULL));

    /* Initialize the clock */
    robotraconteurlite_clock_init(&clock);

    /* Read the clock */
    robotraconteurlite_clock_gettime(&clock, &now);

    /* Use a random nodeid. In practice this should be fixed and unique for every service */
    robotraconteurlite_nodeid_newrandom(&node_id);

    /* Initialize connections and TCP transport */
    connections_head = robotraconteurlite_connections_init_from_array(
        connections_storage, NUM_CONNECTIONS, connection_buffers, CONNECTION_BUFFER_SIZE, NUM_CONNECTIONS * 2);
    if (!connections_head)
    {
        printf("Could not initialize connections\n");
        return -1;
    }

    robotraconteurlite_tcp_connection_init_connections_server(connections_head);

    /* Initialize the node */

    robotraconteurlite_string_from_c_str(node_name_str, &node_name);

    if (robotraconteurlite_node_init(&node, &node_id, &node_name, connections_head))
    {
        printf("Could not initialize node\n");
        return -1;
    }

    /* Start TCP acceptor */
    memset(&listen_addr, 0, sizeof(listen_addr));
    /* Implicit listen address of 0.0.0.0, or all interfaces */
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(node_port);

    if (robotraconteurlite_tcp_acceptor_listen(&tcp_acceptor, (struct sockaddr_storage*)&listen_addr, 4))
    {
        printf("Could not start TCP acceptor\n");
        return -1;
    }

    printf("robotraconteur_tiny_service started\n");

    do
    {
        /* One socket per connection plus acceptor and node. May vary, check documentation */
        struct robotraconteurlite_pollfd pollfds[NUM_CONNECTIONS + 2];
        size_t num_pollfds = 0;
        int ret = -1;
        robotraconteurlite_timespec next_wake = 0;

        robotraconteurlite_clock_gettime(&clock, &now);
        /* Accept TCP connections */
        if (robotraconteurlite_tcp_acceptor_communicate(&tcp_acceptor, connections_head, now))
        {
            printf("Could not accept TCP connections\n");
            return -1;
        }

        /* Communicate with all connections */
        if (robotraconteurlite_tcp_connections_communicate(connections_head, now))
        {
            printf("Could not communicate with connections\n");
            return -1;
        }
        ret = robotraconteurlite_node_next_wake(&node, now, &next_wake);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            printf("Could not get next wake\n");
            return -1;
        }

        if (next_wake > now)
        {
            ret = robotraconteurlite_tcp_acceptor_poll_add_fd(&tcp_acceptor, connections_head, pollfds, &num_pollfds,
                                                              NUM_CONNECTIONS + 2);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not add acceptor to poll\n");
                return -1;
            }
            ret = robotraconteurlite_tcp_connections_poll_add_fds(connections_head, pollfds, &num_pollfds,
                                                                  NUM_CONNECTIONS + 2);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not add connections to poll\n");
                return -1;
            }
            ret = robotraconteurlite_node_poll_add_fd(&node, pollfds, &num_pollfds, NUM_CONNECTIONS + 2);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not add node to poll\n");
                return -1;
            }

            ret = robotraconteurlite_wait_next_wake(&clock, pollfds, num_pollfds, next_wake);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                if (signal_received)
                {
                    printf("Exiting\n");
                    return 0;
                }
                printf("Could not wait for next wake\n");
                return -1;
            }
            /* Accept TCP connections */
            if (robotraconteurlite_tcp_acceptor_communicate(&tcp_acceptor, connections_head, now))
            {
                printf("Could not accept TCP connections\n");
                return -1;
            }

            /* Communicate with all connections */
            if (robotraconteurlite_tcp_connections_communicate(connections_head, now))
            {
                printf("Could not communicate with connections\n");
                return -1;
            }
        }

        /* Run the event loop. Exit if no events are available*/

        do
        {
            struct robotraconteurlite_event event;
            int ret = robotraconteurlite_node_next_event(&node, &event, now);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not get event\n");
                return -1;
            }

            ret = handle_event(&node, &event);
            if (ret == 1)
            {
                break;
            }
            else if (ret < 0)
            {
                if (ret != ROBOTRACONTEURLITE_ERROR_RETRY)
                {
                    printf("Could not handle event\n");
                    return -1;
                }
            }
        } while (1);

        if (signal_received)
        {
            break;
        }

    } while (1);

    /* Close all connections */
    robotraconteurlite_tcp_connections_close(connections_head);

    /* Close the acceptor */
    robotraconteurlite_tcp_acceptor_close(&tcp_acceptor);

    printf("robotraconteur_tiny_service shut down\n");

    return 0;
}
