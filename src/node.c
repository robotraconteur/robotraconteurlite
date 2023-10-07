#include "robotraconteurlite/node.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int robotraconteurlite_node_init(struct robotraconteurlite_node* node, struct robotraconteurlite_nodeid* nodeid, struct robotraconteurlite_string* nodename, struct robotraconteurlite_connection* connections_head)
{
    memset(node,0,sizeof(struct robotraconteurlite_node));
    robotraconteurlite_nodeid_copy_to(nodeid,&node->nodeid);
    assert(nodename->len < sizeof(node->nodename_char));
    node->nodename.data=node->nodename_char;
    node->nodename.len = sizeof(node->nodename_char);
    if (robotraconteurlite_string_copy_to(nodename,&node->nodename))
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    if (connections_head)
    {
        struct robotraconteurlite_connection* c;
        node->connections_head=connections_head;
        /* Set tail */
        c=node->connections_head;
        while(c->next)
        {
            c=c->next;
        }
        node->connections_tail=c;
    }

    node->connections_next = connections_head;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_shutdown(struct robotraconteurlite_node* node)
{
    /* Request close on all connections */
    struct robotraconteurlite_connection* c=node->connections_head;
    while(c)
    {
        robotraconteurlite_connection_close(c);
        c=c->next;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_add_connection(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection)
{
    /* Add to end */
    if (node->connections_head)
    {
        node->connections_tail->next=connection;
        connection->prev=node->connections_tail;
        node->connections_tail=connection;
    }
    else
    {
        node->connections_head=connection;
        node->connections_tail=connection;
        node->connections_next=connection;
    }
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_remove_connection(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection)
{
    /* Remove from list */
    if (connection->prev)
    {
        connection->prev->next=connection->next;
    }
    else
    {
        node->connections_head=connection->next;
    }

    if (connection->next)
    {
        connection->next->prev=connection->prev;
    }
    else
    {
        node->connections_tail=connection->prev;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

void static robotraconteurlite_clear_event(struct robotraconteurlite_event* event)
{
    memset(event,0,sizeof(struct robotraconteurlite_event));
}

int robotraconteurlite_node_next_event(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event)
{
    struct robotraconteurlite_connection* c;
    if (!node->connections_next)
    {
        robotraconteurlite_clear_event(event);
        event->event_type = ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE;
        event->events_serviced = node->events_serviced;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    do
    {
        c = node->connections_next;
        node->connections_next = c->next;

        if (robotraconteurlite_connection_is_closed_event(c))
        {
            robotraconteurlite_clear_event(event);
            node->events_serviced++;
            event->event_type = ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CLOSED;
            event->connection = c;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }

        if (robotraconteurlite_connection_is_error(c))
        {
            robotraconteurlite_clear_event(event);
            node->events_serviced++;
            event->event_type = ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_ERROR;
            event->connection = c;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }

        if (robotraconteurlite_connection_is_connected_event(c))
        {
            robotraconteurlite_clear_event(event);
            node->events_serviced++;
            event->event_type = ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CONNECTED;
            event->connection = c;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }

        if (robotraconteurlite_connection_is_message_sent_event(c))
        {
            robotraconteurlite_clear_event(event);
            node->events_serviced++;
            event->event_type = ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_SEND_COMPLETE;
            event->connection = c;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        
        if (robotraconteurlite_connection_is_message_received_event(c))
        {
            robotraconteurlite_clear_event(event);
            node->events_serviced++;
            event->event_type = ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_RECEIVED;
            event->connection = c;
            event->received_message.node = node;
            event->received_message.connection = c;
            event->event_error_code = robotraconteurlite_node_receive_messageentry(&event->received_message);
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }

    }
    while (node->connections_next);

    memset(event,0,sizeof(struct robotraconteurlite_event));
    event->event_type = ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_consume_event(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event)
{
    switch (event->event_type)
    {
        case ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE:
        {
            node->connections_next = node->connections_head;
            node->events_serviced = 0;
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CLOSED:
        {
            robotraconteurlite_connection_consume_closed(event->connection);
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_ERROR:
        {
            /* Error cannot be cleared or consumed */
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        case ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_CONNECTED:
        {
            robotraconteurlite_connection_consume_connected(event->connection);
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        case ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_RECEIVED:
        {
            robotraconteurlite_connection_consume_message_received(event->connection);
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        case ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_SEND_COMPLETE:
        {
            robotraconteurlite_connection_consume_message_sent(event->connection);
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }

        default:
            return ROBOTRACONTEURLITE_ERROR_INVALID_OPERATION;
    }
}

static int robotraconteurlite_node_event_special_request_handle_error(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event, int err)
{
    if (err == ROBOTRACONTEURLITE_ERROR_RETRY)
    {
        return ROBOTRACONTEURLITE_ERROR_RETRY;
    }
    robotraconteurlite_connection_error(event->connection);
    /* Consume event */
    robotraconteurlite_node_consume_event(node, event);
    return ROBOTRACONTEURLITE_ERROR_CONSUMED;
}

int robotraconteurlite_node_event_special_request(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event)
{
    if (event->received_message.received_message_entry_header.entry_type > 500)
    {
        /* Special requests are below entry type 500 */
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    if (event->event_error_code != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        robotraconteurlite_connection_error(event->connection);
        /* Consume event */
        robotraconteurlite_node_consume_event(node, event);
        return ROBOTRACONTEURLITE_ERROR_CONSUMED;
    }

    switch (event->received_message.received_message_entry_header.entry_type)
    {
        case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_STREAMOP:
        {
            if ((robotraconteurlite_string_cmp_c_str(&event->received_message.received_message_entry_header.member_name, "CreateConnection") == 0)
             && robotraconteurlite_connection_is_server(event->connection)
             && ((event->connection->connection_state & ROBOTRACONTEURLITE_STATUS_FLAGS_ESTABLISHED) == 0))
            {
                int ret;
                /* TODO: Check the incoming target address and sender address */
                robotraconteurlite_nodeid_copy_to(&event->received_message.received_message_header.sender_nodeid, &event->connection->remote_nodeid);
                /* TODO: sender nodename */
                ret = robotraconteurlite_node_send_messageentry_empty_response(node, event->connection, &event->received_message.received_message_entry_header);
                if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                {
                    return robotraconteurlite_node_event_special_request_handle_error(node, event, ret);
                }

                /* Set the ESTABLISHED flag */
                event->connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_ESTABLISHED;

                /* Consume event */
                robotraconteurlite_node_consume_event(node, event);
                return ROBOTRACONTEURLITE_ERROR_CONSUMED;
            }
            else
            {
                break;
            }
        }
        case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETSERVICEDESC:
        case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_OBJECTTYPENAME:
        {
            /* These need to be handled by the user to avoid memory handling */
            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
        case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTCLIENT:
        {
            int ret;
            if (event->connection->local_endpoint == 0)
            {
                event->connection->local_endpoint = (uint32_t)rand();
            }
            event->connection->remote_endpoint = event->received_message.received_message_header.sender_endpoint;
            
            ret = robotraconteurlite_node_send_messageentry_empty_response(node, event->connection, &event->received_message.received_message_entry_header);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                return robotraconteurlite_node_event_special_request_handle_error(node, event, ret);
            }

            /* Set the CLIENT_ESTABLISHED flag */
            event->connection->connection_state |= ROBOTRACONTEURLITE_STATUS_FLAGS_CLIENT_ESTABLISHED;

            /* Consume event */
            robotraconteurlite_node_consume_event(node, event);
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }
        case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTIONTEST:
        case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETNODEINFO:
        case ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_DISCONNECTCLIENT:
        {
            /* TODO: handle disconnect client */
            int ret;
            ret = robotraconteurlite_node_send_messageentry_empty_response(node, event->connection, &event->received_message.received_message_entry_header);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                return robotraconteurlite_node_event_special_request_handle_error(node, event, ret);
            }

            /* Consume event */
            robotraconteurlite_node_consume_event(node, event);
            return ROBOTRACONTEURLITE_ERROR_CONSUMED;
        }


    default:
        break;
    }

    /* If message is odd, it is a request, respond with error. Otherwise close transport */
    if (event->received_message.received_message_entry_header.entry_type % 2 == 1)
    {
        int ret = robotraconteurlite_connection_send_messageentry_error_response(node, event->connection, &event->received_message.received_message_entry_header, ROBOTRACONTEURLITE_ERROR_INVALID_OPERATION, "RobotRaconteur.InvalidOperation", "Invalid operation");
        if (ret == ROBOTRACONTEURLITE_ERROR_RETRY)
        {
            return ROBOTRACONTEURLITE_ERROR_RETRY;
        }
        /* Consume event */
        robotraconteurlite_node_consume_event(node, event);
        return ROBOTRACONTEURLITE_ERROR_CONSUMED;
    }
    else
    {
        robotraconteurlite_connection_close(event->connection);
        /* Consume event */
        robotraconteurlite_node_consume_event(node, event);
        return ROBOTRACONTEURLITE_ERROR_CONSUMED;
    }
}

int robotraconteurlite_node_verify_incoming_message(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection, struct robotraconteurlite_message_header* message_header)
{
    /* TODO: verify address information */
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_begin_send_messageentry(struct robotraconteurlite_node_send_messageentry_data* send_data)
{
    int ret;
    send_data->buffer_storage.data = NULL;
    send_data->buffer_storage.len = 0;
    send_data->buffer_vec_storage.buffer_vec_cnt = 1;
    send_data->buffer_vec_storage.buffer_vec = &send_data->buffer_storage;
    ret = robotraconteurlite_connection_begin_send_message(send_data->connection, &send_data->message_writer, &send_data->buffer_vec_storage);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    memset(&send_data->message_header, 0, sizeof(struct robotraconteurlite_message_header));
    send_data->message_header.message_version = 2;
    robotraconteurlite_nodeid_copy_to(&send_data->node->nodeid, &send_data->message_header.sender_nodeid);
    robotraconteurlite_nodeid_copy_to(&send_data->connection->remote_nodeid, &send_data->message_header.receiver_nodeid);
    send_data->message_header.sender_endpoint = send_data->connection->local_endpoint;
    send_data->message_header.receiver_endpoint = send_data->connection->remote_endpoint;
    robotraconteurlite_string_shallow_copy_to(&send_data->node->nodename, &send_data->message_header.sender_nodename);
    robotraconteurlite_string_shallow_copy_to(&send_data->connection->remote_nodename, &send_data->message_header.receiver_nodename);

    ret = robotraconteurlite_message_writer_begin_message(&send_data->message_writer, &send_data->message_header, &send_data->entry_writer);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    ret = robotraconteurlite_messageentry_writer_begin_entry(&send_data->entry_writer, send_data->message_entry_header, &send_data->element_writer);
    return ret;
}

int robotraconteurlite_node_end_send_messageentry(struct robotraconteurlite_node_send_messageentry_data* send_data)
{
    int ret = robotraconteurlite_messageentry_writer_end_entry(&send_data->entry_writer, send_data->message_entry_header, &send_data->element_writer);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    ret = robotraconteurlite_message_writer_end_message(&send_data->message_writer, &send_data->message_header, &send_data->entry_writer);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    ret = robotraconteurlite_connection_end_send_message(send_data->connection, send_data->message_header.message_size);
    return ret;
}

int robotraconteurlite_node_abort_send_messageentry(struct robotraconteurlite_node_send_messageentry_data* send_data)
{
    int ret = robotraconteurlite_connection_abort_send_message(send_data->connection);
    return ret;
}

int robotraconteurlite_node_send_messageentry_empty_response(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection, struct robotraconteurlite_messageentry_header* recv_message_header)
{
    struct robotraconteurlite_node_send_messageentry_data send_data;
    struct robotraconteurlite_messageentry_header send_message_header;
    int ret;
    memcpy(&send_message_header, recv_message_header, sizeof(struct robotraconteurlite_messageentry_header));
    send_message_header.entry_type++;
    memset(&send_data, 0, sizeof(struct robotraconteurlite_node_send_messageentry_data));
    send_data.node = node;
    send_data.connection = connection;
    send_data.message_entry_header = &send_message_header;
    ret = robotraconteurlite_node_begin_send_messageentry(&send_data);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    ret = robotraconteurlite_node_end_send_messageentry(&send_data);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_begin_send_messageentry_response(struct robotraconteurlite_node_send_messageentry_data* send_data,
  struct robotraconteurlite_messageentry_header* request_message_entry_header)
{
    int ret;
    memcpy(&send_data->message_entry_header_storage, request_message_entry_header, sizeof(struct robotraconteurlite_messageentry_header));
    send_data->message_entry_header_storage.entry_type++;
    send_data->message_entry_header = &send_data->message_entry_header_storage;
    ret = robotraconteurlite_node_begin_send_messageentry(send_data);
    return ret;
}

int robotraconteurlite_connection_send_messageentry_error_response(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection, struct robotraconteurlite_messageentry_header* request_message_entry_header, uint16_t error_code, const char* error_name, const char* error_message)
{
    struct robotraconteurlite_node_send_messageentry_data send_data;
    struct robotraconteurlite_messageentry_header send_message_entry_header;
    int ret;  
    memcpy(&send_message_entry_header, request_message_entry_header, sizeof(struct robotraconteurlite_messageentry_header));
    send_message_entry_header.entry_type++;
    send_message_entry_header.error = error_code;
    memset(&send_data, 0, sizeof(struct robotraconteurlite_node_send_messageentry_data));
    send_data.node = node;
    send_data.connection = connection;
    send_data.message_entry_header = &send_message_entry_header;
    ret = robotraconteurlite_node_begin_send_messageentry(&send_data);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    ret = robotraconteurlite_messageelement_writer_write_string_c_str(&send_data.element_writer, "errorname", error_name);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }
    
    ret = robotraconteurlite_messageelement_writer_write_string_c_str(&send_data.element_writer, "errorstring", error_message);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    ret = robotraconteurlite_node_end_send_messageentry(&send_data);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_receive_messageentry(struct robotraconteurlite_node_receive_messageentry_data* receive_data)
{
    int ret;
    struct robotraconteurlite_message_reader message_reader;
    receive_data->buffer_storage.data = NULL;
    receive_data->buffer_storage.len = 0;
    receive_data->buffer_vec_storage.buffer_vec = &receive_data->buffer_storage;
    receive_data->buffer_vec_storage.buffer_vec_cnt = 1;
    ret = robotraconteurlite_connection_message_receive(receive_data->connection, &message_reader, &receive_data->buffer_vec_storage);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    /* Apply storage buffers for header strings */
    receive_data->received_message_header.receiver_nodename.data = receive_data->receiver_nodename_char;
    receive_data->received_message_header.receiver_nodename.len = sizeof(receive_data->receiver_nodename_char);
    receive_data->received_message_header.sender_nodename.data = receive_data->sender_nodename_char;
    receive_data->received_message_header.sender_nodename.len = sizeof(receive_data->sender_nodename_char);

    ret = robotraconteurlite_message_reader_read_header(&message_reader, &receive_data->received_message_header);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    ret = robotraconteurlite_node_verify_incoming_message(receive_data->node, receive_data->connection, &receive_data->received_message_header);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    /* TODO: Support multiple entries in one message */
    ret = robotraconteurlite_message_reader_begin_read_entries(&message_reader, &receive_data->entry_reader);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    /* Apply storage buffer for entry header strings */
    receive_data->received_message_entry_header.member_name.data = receive_data->member_name_char;
    receive_data->received_message_entry_header.member_name.len = sizeof(receive_data->member_name_char);
    receive_data->received_message_entry_header.service_path.data = receive_data->service_path_char;
    receive_data->received_message_entry_header.service_path.len = sizeof(receive_data->service_path_char);
    receive_data->received_message_entry_header.extended.data = receive_data->extended_char;
    receive_data->received_message_entry_header.extended.len = sizeof(receive_data->extended_char);

    ret = robotraconteurlite_messageentry_reader_read_header(&receive_data->entry_reader, &receive_data->received_message_entry_header);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

int robotraconteurlite_node_receive_messageentry_consume(struct robotraconteurlite_node_receive_messageentry_data* receive_data)
{
    return robotraconteurlite_connection_message_receive_consume(receive_data->connection);
}

int robotraconteurlite_node_event_special_request_service_definition(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event, struct robotraconteurlite_node_service_object service_objects[], size_t service_objects_len, struct robotraconteurlite_node_service_definition service_defs[], size_t service_defs_len)
{

    size_t i;
    assert(node);
    assert(event);
    assert(event->received_message.received_message_entry_header.entry_type == ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETSERVICEDESC);

    for(i=0; i<service_objects_len; i++)
    {
        /* Compare qualified_name with service_path */
        if (robotraconteurlite_string_cmp(&event->received_message.received_message_entry_header.service_path, &service_objects[i].service_path) == 0)
        {
            int ret;            
            struct robotraconteurlite_node_send_messageentry_data send_data;
            uint32_t old_remote_endpoint;
            send_data.node = event->received_message.node;
            send_data.connection = event->connection;
            old_remote_endpoint = event->connection->remote_endpoint;
            event->connection->remote_endpoint = event->received_message.received_message_header.sender_endpoint;
            ret = robotraconteurlite_node_begin_send_messageentry_response(&send_data, &event->received_message.received_message_entry_header);
            event->connection->remote_endpoint = old_remote_endpoint;
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                return ret;
            }            
            {
                /* Write service definition */
                struct robotraconteurlite_string element_name_str;
                struct robotraconteurlite_string desc_str;
                element_name_str.data = "servicedef";
                element_name_str.len = 10;
                desc_str.data = service_objects[i].service_def->service_definition.data;
                desc_str.len = service_objects[i].service_def->service_definition.len;
                ret = robotraconteurlite_messageelement_writer_write_string(&send_data.element_writer, &element_name_str, &desc_str);
                if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                {
                    return ret;
                }
            }
            {
                /* Write empty attributes */
                /* TODO: Support attributes */
                struct robotraconteurlite_messageelement_writer attr_element_writer;
                struct robotraconteurlite_messageelement_header attr_element_header;
                memset(&attr_element_header, 0, sizeof(struct robotraconteurlite_messageelement_header));
                attr_element_header.element_type = ROBOTRACONTEURLITE_DATATYPE_MAP_STRING;
                robotraconteurlite_string_from_c_str("attributes", &attr_element_header.element_name);
                ret = robotraconteurlite_messageelement_writer_begin_nested_element(&send_data.element_writer, &attr_element_header, &attr_element_writer);
                if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                {
                    return ret;
                }
                ret = robotraconteurlite_messageelement_writer_end_nested_element(&send_data.element_writer, &attr_element_header, &attr_element_writer);
                if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                {
                    return ret;
                }
            }

            ret = robotraconteurlite_node_end_send_messageentry(&send_data);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                return ret;
            }

            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }        
    }
    /* TODO: Support returning named service type */
    
    /* Return ServiceNotFound error message */
    return robotraconteurlite_connection_send_messageentry_error_response(node, event->connection, &event->received_message.received_message_entry_header, ROBOTRACONTEURLITE_MESSAGEERRORTYPE_SERVICENOTFOUND, "RobotRaconteur.ServiceNotFound", "Service not found");
    
}

int robotraconteurlite_node_event_special_request_object_type_name(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event, struct robotraconteurlite_node_service_object service_objects[], size_t service_objects_len)
{
    size_t i;
    assert(node);
    assert(event);
    assert(event->received_message.received_message_entry_header.entry_type == ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_OBJECTTYPENAME);

    for (i=0; i<service_objects_len; i++)
    {
        /* Compare qualified_name with service_path */
        if (robotraconteurlite_string_cmp(&event->received_message.received_message_entry_header.service_path, &service_objects[i].service_path) == 0)
        {
            int ret;
            struct robotraconteurlite_node_send_messageentry_data send_data;
            uint32_t old_remote_endpoint;
            send_data.node = event->received_message.node;
            send_data.connection = event->connection;
            old_remote_endpoint = event->connection->remote_endpoint;
            event->connection->remote_endpoint = event->received_message.received_message_header.sender_endpoint;
            ret = robotraconteurlite_node_begin_send_messageentry_response(&send_data, &event->received_message.received_message_entry_header);
            event->connection->remote_endpoint = old_remote_endpoint;
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                return ret;
            }
            {
                /* Write object type name */
                struct robotraconteurlite_string element_name_str;
                struct robotraconteurlite_string type_name_str;
                robotraconteurlite_string_from_c_str("objecttype", &element_name_str);
                type_name_str.data = service_objects[i].qualified_type.data;
                type_name_str.len = service_objects[i].qualified_type.len;
                ret = robotraconteurlite_messageelement_writer_write_string(&send_data.element_writer, &element_name_str, &type_name_str);
                if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
                {
                    return ret;
                }
            }
            
            ret = robotraconteurlite_node_end_send_messageentry(&send_data);
            if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
            {
                return ret;
            }

            return ROBOTRACONTEURLITE_ERROR_SUCCESS;
        }
    }

    /* Return ObjectNotFound error message */
    return robotraconteurlite_connection_send_messageentry_error_response(node, event->connection, &event->received_message.received_message_entry_header, ROBOTRACONTEURLITE_MESSAGEERRORTYPE_OBJECTNOTFOUND, "RobotRaconteur.ObjectNotFound", "Object not found");
}

int robotraconteurlite_event_is_member(struct robotraconteurlite_event* event, const char* service_path, const char* member_name)
{
    assert(event->event_type == ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_RECEIVED);
    if (robotraconteurlite_string_cmp_c_str(&event->received_message.received_message_entry_header.service_path, service_path) != 0)
    {
        return 0;
    }
    if (robotraconteurlite_string_cmp_c_str(&event->received_message.received_message_entry_header.member_name, member_name) != 0)
    {
        return 0;
    }
    return 1;
}
