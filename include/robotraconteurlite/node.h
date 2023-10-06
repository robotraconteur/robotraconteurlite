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
#include "robotraconteurlite/connection.h"

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



enum robotraconteurlite_event_type
{
  ROBOTRACONTEURLITE_EVENT_TYPE_NOOP = 0,
  ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTED,
  ROBOTRACONTEURLITE_EVENT_TYPE_DISCONNECTED,
  ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_ERROR,
  ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_RECEIVED,
  ROBOTRACONTEURLITE_EVENT_TYPE_MESSAGE_SEND_COMPLETE,
  ROBOTRACONTEURLITE_EVENT_TYPE_NEXT_CYCLE,
  ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_HEARTBEAT_TIMEOUT,
  ROBOTRACONTEURLITE_EVENT_TYPE_CONNECTION_TIMEOUT
};

struct robotraconteurlite_event
{
  enum robotraconteurlite_event_type event_type;
  struct robotraconteurlite_connection* connection;
  struct robotraconteurlite_messageentry_header received_message_header;
  struct robotraconteurlite_messageelement_reader received_message_data;
};

struct robotraconteurlite_node
{
    // Connections linked list
    struct robotraconteurlite_connection* connections_head;
    struct robotraconteurlite_connection* connections_tail;
    struct robotraconteurlite_connection* connections_next;
};


ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_init(struct robotraconteurlite_node* node, struct robotraconteurlite_nodeid* nodeid, struct robotraconteurlite_connection* connections, size_t connections_len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_shutdown(struct robotraconteurlite_node* node);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_add_connection(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_remove_connection(struct robotraconteurlite_node* node, struct robotraconteurlite_connection* connection);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_next_event(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_consume_event(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_event_special_request(struct robotraconteurlite_node* node, struct robotraconteurlite_event* event);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_node_verify_incoming_message(struct robotraconteurlite_node* node, struct robotraconteurlite_message_reader* message);

#endif //__ROBOTRACONTEURLITE_NODE_H__