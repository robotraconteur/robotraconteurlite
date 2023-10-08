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

#define _XOPEN_SOURCE 500
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

const uint16_t service_port = 22229;
const char* service_ip_str = "127.0.0.1";
const char* service_name = "tiny_service";

int main(int argc, char *argv[])
{

    /* Variable storage */
    struct robotraconteurlite_connection connections_storage[NUM_CONNECTIONS];
    uint8_t connection_buffers[NUM_CONNECTIONS*2*CONNECTION_BUFFER_SIZE];
    struct robotraconteurlite_connection* connections_head;
    struct robotraconteurlite_connection* connection;
    struct robotraconteurlite_node node;
    struct robotraconteurlite_nodeid node_id;
    struct robotraconteurlite_addr service_addr;
    struct sockaddr_in* service_sockaddr;
    struct robotraconteurlite_tcp_connect_service_data connect_data;
    robotraconteurlite_timespec now;
    struct robotraconteurlite_clock rr_clock;
    int ret;
    struct robotraconteurlite_event event;
    struct robotraconteurlite_string nodename_str;
    uint64_t end_time;

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
    connections_head = robotraconteurlite_connections_init_from_array(connections_storage, NUM_CONNECTIONS, connection_buffers, CONNECTION_BUFFER_SIZE, NUM_CONNECTIONS*2);
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
    if(inet_pton(AF_INET, service_ip_str, &service_sockaddr->sin_addr) != 1)
    {
        printf("Could not convert service IP address\n");
        return -1;
    }

    memset(&connect_data, 0, sizeof(connect_data));
    connect_data.connections_head = connections_head;
    connect_data.service_address = &service_addr;

    printf("Begin connecting to service\n");

    robotraconteurlite_clock_gettime(&rr_clock, &now);
    ret = robotraconteurlite_tcp_connect_service(&connect_data, now);

    if (ret)
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
            ret = robotraconteurlite_node_next_event(&node, &event, now);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                printf("Could not get next event\n");
                return -1;
            }
            ret = robotraconteurlite_client_handshake(&handshake_data, &event, now);
            if (ret == ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                break;
            }
            if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
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

    while (now < end_time)
    {
        /* get next event */
        robotraconteurlite_clock_gettime(&rr_clock, &now);
        robotraconteurlite_tcp_connections_communicate(connections_head, now);
        ret = robotraconteurlite_node_next_event(&node, &event, now);
        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            printf("Could not get next event\n");
            return -1;
        }

        ret = robotraconteurlite_node_event_special_request(&node, &event);
        if (ret == ROBOTRACONTEURLITE_ERROR_CONSUMED)
        {
            continue;
        }

        if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
        {
            printf("Could not handle special request\n");
            return -1;
        }

        /* Handle event */
        switch (event.event_type)
        {
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_ERROR:
            {
                robotraconteurlite_connection_close(event.connection);
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CLOSED:
            {
                printf("Connection error\n");
                robotraconteurlite_node_consume_event(&node, &event);
                return -1;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_HEARTBEAT_TIMEOUT:
            {
                printf("Connection heartbeat timeout\n");
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_TIMEOUT:
            {
                printf("Connection timeout\n");
                /* close connection */
                robotraconteurlite_connection_close(event.connection);
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
            case ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE:
            {
                robotraconteurlite_node_consume_event(&node, &event);
                usleep(1000);
            }
            default:
            {
                robotraconteurlite_node_consume_event(&node, &event);
                break;
            }
        }
    }


    robotraconteurlite_connection_close(connection);

    /* TODO: Drain the connection */
    robotraconteurlite_clock_gettime(&rr_clock, &now);
    robotraconteurlite_tcp_connection_communicate(connection, now);

    printf("Done!\n");

    return 0;
}
