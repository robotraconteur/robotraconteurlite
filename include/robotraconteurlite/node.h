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

#ifndef __ROBOTRACONTEURLITE_NODE_H__
#define __ROBOTRACONTEURLITE_NODE_H__

#include "robotraconteurlite/message.h"

/*

class RobotRaconteurServerConnection
{
public:
  rr_socket _sock;
  uint16_t _port;
  
  uint32_t remote_Endpoint;
  uint8_t remote_NodeID[16];
  bool connected;
  bool started;
  bool in_http_header;
  bool http_line_empty;
  uint16_t http_line_pos;
  bool http_is_sec_websocket_key_line;
  static SHA1 http_key_sha1;
  
public:
  //message variables
  uint16_t message_len;
  uint16_t message_EntryCount;
  
   
  //message entry variables
  uint16_t entry_start_pos;
  uint32_t entry_EntrySize;
  uint16_t entry_EntryType;

  char entry_ServicePath[32];
  uint8_t entry_ServicePath_len;
  char entry_MemberName[32];
    uint8_t entry_MemberName_len;
  uint32_t entry_TransactionID;
  uint16_t entry_Error;
  
  uint8_t entry_ElementCount;
  uint8_t entry_element_count_enum;
  
  //message element variables
  uint16_t element_start_pos;
  uint32_t element_ElementSize;
   uint8_t element_ElementName_len;
   char element_ElementName[32];
   uint16_t element_ElementType;
   uint32_t element_DataCount;
  
 
  
   uint16_t send_message_length_pos;
   uint16_t send_message_header_length_pos;
   uint16_t send_message_sender_endpoint_pos;
  uint32_t send_message_sender_endpoint;
   uint16_t send_message_entry_count_pos;
   uint16_t send_message_entry_count;
   uint16_t send_entry_error_pos;
   uint16_t send_entry_start_pos;
   uint16_t send_entry_length_pos;
   uint16_t send_entry_element_count_pos;
   uint16_t send_entry_element_count;
   uint16_t send_element_start_pos;
   uint16_t send_element_length_pos;
  
public:
  
  RobotRaconteurServerConnection();
  RobotRaconteurServerConnection(uint16_t port);
  
  int8_t ReadNextMessageElement(uint16_t* ElementSize, char** ElementName, uint8_t* ElementName_len,
    uint16_t* ElementType, uint16_t* DataCount, rr_socket** sock);
  
  void StartWriteMessageElement(const char* ElementName, bool ElementName_progmem, uint8_t ElementName_len,
    uint16_t ElementType, const char* ElementTypeName, bool ElementTypeName_progmem, uint8_t ElementTypeName_len, uint16_t DataCount,
    rr_socket** sock);

  void EndWriteMessageElement();
  
  void SetMessageElementError(uint16_t errcode, const char* errname, bool errname_progmem, uint8_t errname_len,
     const char* errmessage, bool errmessage_progmem, uint8_t errmessage_len);
  
  void loop();
  
  //void StartSendEvent(char* MemberName, uint8_t MemberName_len);
  
  //void EndSendEvent();
  
  void SetMemberFormatMismatchError();
  void SetMemberNotFoundError();
  
  void GetLocalNodeServices();

protected:
  void read_messagelen();
  void process_message();
  void flush_message();
  void flush_message_entry();
  void flush_message_element();
  
};
*/

enum robotraconteurlite_connection_config_flags
{
    ROBOTRACONTEURLITE_CONFIG_FLAGS_NULL = 0,
    ROBOTRACONTEURLITE_CONFIG_FLAGS_ISSERVER = 0x1,
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
};


struct robotraconteurlite_transport_storage
{
    uint8_t _storage[32];
};

struct robotraconteurlite_connection
{
    uint32_t transport_type;
    struct robotraconteurlite_connection* next;
    struct robotraconteurlite_connection* prev;

    // Socket storage
    int sock;

    // Send and receive buffers
    uint8_t* send_buffer;
    uint8_t* recv_buffer;
    size_t send_buffer_len;
    size_t recv_buffer_len;
    size_t send_buffer_pos;
    size_t recv_buffer_pos;

    // Control flags
    uint32_t config_flags;
    uint32_t connection_state;

    // Robot Raconteur information
    uint32_t local_endpoint;
    uint32_t remote_endpoint;
    struct robotraconteurlite_nodeid remote_nodeid;

    // Message information
    uint32_t recv_message_len;

    // Transport storage
    struct robotraconteurlite_transport_storage transport_storage;    
};

struct robotraconteurlite_connection_acceptor
{
    int32_t id;
    uint32_t transport_type;
    struct robotraconteurlite_connection_acceptor* next;
    struct robotraconteurlite_connection_acceptor* prev;

    // Socket storage
    int sock;
};

struct robotraconteurlite_node
{
    // Connections linked list
    struct robotraconteurlite_connection* connections_head;
    struct robotraconteurlite_connection* connections_tail;
};


ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_init(struct robotraconteurlite_node* node, struct robotraconteurlite_nodeid* nodeid, struct robotraconteurlite_connection* connections, size_t connections_len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_shutdown(struct robotraconteurlite_node* node);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_add_connection(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_remove_connection(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_init(struct robotraconteurlite_connection* connection, uint32_t transport_type, size_t buffer_len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_connection_verify_preamble(struct robotraconteurlite_connection* connection, size_t* message_len);

#endif //__ROBOTRACONTEURLITE_NODE_H__