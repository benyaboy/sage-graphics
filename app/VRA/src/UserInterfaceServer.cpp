/*--------------------------------------------------------------------------*/
/* Volume Rendering Application                                             */
/* Copyright (C) 2006-2007 Nicholas Schwarz                                 */
/*                                                                          */
/* This software is free software; you can redistribute it and/or modify it */
/* under the terms of the GNU Lesser General Public License as published by */
/* the Free Software Foundation; either Version 2.1 of the License, or      */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This software is distributed in the hope that it will be useful, but     */
/* WITHOUT ANY WARRANTY; without even the implied warranty of               */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser  */
/* General Public License for more details.                                 */
/*                                                                          */
/* You should have received a copy of the GNU Lesser Public License along   */
/* with this library; if not, write to the Free Software Foundation, Inc.,  */
/* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                    */
/*--------------------------------------------------------------------------*/

#include "UserInterfaceServer.h"

/*--------------------------------------------------------------------------*/

UserInterfaceServer::UserInterfaceServer() {

  // Initialize client socket file descriptor
  _clientSocketFileDescriptor = -1;

  // Initialize is connected flag
  _isConnected = false;

  // Initialize is initialized flag
  _isInitialized = false;

  // Initialize observer
  _observer = NULL;
  
  // Initialize port number
  _port = 0;

  // Initialize server
  _server = NULL;

  // Initialize server socket file descriptor
  _serverSocketFileDescriptor = -1;

}

/*--------------------------------------------------------------------------*/

UserInterfaceServer::~UserInterfaceServer() {

  // If server socket is open
  if (_serverSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("UserInterfaceServer: Error during socket shutdown");
    }

    // Close socket
    if (close(_serverSocketFileDescriptor) == -1) {
      perror("UserInterfaceServer: Error during socket close");
    }

  }

  // If client socket is open
  if (_clientSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_clientSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("UserInterfaceServer: Error during socket shutdown");
    }

    // Close socket
    if (close(_clientSocketFileDescriptor) == -1) {
      perror("UserInterfaceServer: Error during socket close");
    }

  }

}

/*--------------------------------------------------------------------------*/

int UserInterfaceServer::Accept() {

  // Initialize variables to hold client info
  memset(&_clientAddress, 0, sizeof(_clientAddress));
  int clientLength = sizeof(_clientAddress);
  
  // Wait for a new connection
  _clientSocketFileDescriptor = 
    accept(_serverSocketFileDescriptor, 
           (struct sockaddr*) &_clientAddress, 
           (socklen_t*) &clientLength);
  
  // Check for error with accept
  if (_clientSocketFileDescriptor == -1) {
    perror("UserInterfaceServer: Error accepting new connections");
    return USER_INTERFACE_SERVER_ERROR;
  }
  
  // Set SO_REUSEADDR
  int optVal = 1;
  int optLen = sizeof(optVal);
  if(setsockopt(_clientSocketFileDescriptor, 
                SOL_SOCKET, 
                SO_REUSEADDR, 
                (void*) &optVal, 
                (socklen_t) optLen) != 0) {
    perror("UserInterfaceServer: Error setting SO_REUSEADDR");
  }

  // Set TCP_NODELAY
  optVal = 1;
  optLen = sizeof(optVal);
  if(setsockopt(_clientSocketFileDescriptor,
                IPPROTO_TCP,
                TCP_NODELAY,
                (void*) &optVal,
                (socklen_t) optLen) != 0) {
    perror("UserInterfaceServer: Error setting TCP_NODELAY");
  }
  
  // Check for error during accept
  if (_clientSocketFileDescriptor == -1) {
    perror("UserInterfaceServer: Error accepting new connection");
    close(_serverSocketFileDescriptor);
    return USER_INTERFACE_SERVER_ERROR;
  }

  // Set is connected flag
  _isConnected = true;

  // Return OK
  return USER_INTERFACE_SERVER_OK;

}

/*--------------------------------------------------------------------------*/

int UserInterfaceServer::GetCommand() {

  // Check if server is intialized
  if (_isInitialized == false) {
    return USER_INTERFACE_SERVER_ERROR;
  }

  // Check if server is connected
  if (_isConnected == false) {
    return USER_INTERFACE_SERVER_ERROR;
  }


  // Message buffer
  char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);

  // Destination
  int destination = 0;

  // Message length
  int messageLength = 0;


  // File descriptor set
  fd_set rfds;

  // Timeout for select
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 100;

  // Setup for select
  FD_ZERO(&rfds);
  FD_SET(_clientSocketFileDescriptor, &rfds);

  // Check for data
  if (select(_clientSocketFileDescriptor + 1, &rfds, NULL, NULL, &timeout) == 
      0) {
    return USER_INTERFACE_SERVER_NO_DATA;
  }


  // Wait for message from client
  if ((messageLength = recv(_clientSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            MSG_WAITALL)) != 2048) {
    return USER_INTERFACE_SERVER_ERROR;
  }


  // Data packer
  DataPacker unpacker;
  unpacker.SetBuffer(buffer, 2048);

  // Unpack command from buffer
  for (int i = 0 ; i < 8 ; i++) {
    unpacker.UnpackChar(&command[i]);
  }

  // Unpack destination from buffer
  unpacker.UnpackInt(&destination);
  //fprintf(stderr, "UIS Command %s\n", command);

  // get axis state
  if (!strcmp(command, "GET_AXS")) {

    // Data
    bool state = false;

    // Observer
    if (_observer != NULL) {
      _observer -> GetAxis(&state);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    if (state == false) { strcpy(command, "CMD_AOF"); }
    else { strcpy(command, "CMD_AON"); }

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get axis position
  else if (!strcmp(command, "GET_APO")) {

    // Data
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    // Observer
    if (_observer != NULL) {
      _observer -> GetAxisPosition(&x, &y, &z);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_APO");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    packer.PackFloat(x);
    packer.PackFloat(y);
    packer.PackFloat(z);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get bounding box state
  else if (!strcmp(command, "GET_BOX")) {

    // Data
    bool state = false;

    // Observer
    if (_observer != NULL) {
      _observer -> GetBoundingBox(&state);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    if (state == false) { strcpy(command, "CMD_BOF"); }
    else { strcpy(command, "CMD_BON"); }

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get brick box state
  else if (!strcmp(command, "GET_BBX")) {

    // Data
    bool state = false;

    // Observer
    if (_observer != NULL) {
      _observer -> GetBoundingBox(&state);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    if (state == false) { strcpy(command, "CMD_BBF"); }
    else { strcpy(command, "CMD_BBN"); }

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get clipping planes
  else if (!strcmp(command, "GET_CLI")) {

    // Data
    float near = 0.0;
    float far = 0.0;
 
    // Observer
    if (_observer != NULL) {
      _observer -> GetClippingPlanes(destination, &near, &far);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_CLI");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Pack data
    packer.PackFloat(near);
    packer.PackFloat(far);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get data
  else if (!strcmp(command, "GET_DAT")) {

    // Data
    char data[1024];
    int ramSize = -1;
    int vramSize = -1;
      
    // Observer 
    if (_observer != NULL) {
      _observer -> GetData(data, &ramSize, &vramSize);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_DAT");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    for (int i = 0 ; i < 1024 ; i++) {
      packer.PackChar(data[i]);
    }
    packer.PackInt(ramSize);
    packer.PackInt(vramSize);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get dataset list file
  else if (!strcmp(command, "GET_DSF")) {

    // Data
    char data[1024];
      
    // Observer 
    if (_observer != NULL) {
      _observer -> GetDatasetListFile(destination, data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_DSF");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Pack data
    for (int i = 0 ; i < 1024 ; i++) {
      packer.PackChar(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get dataset list name
  else if (!strcmp(command, "GET_DSN")) {

    // Data
    char data[1024];
      
    // Observer 
    if (_observer != NULL) {
      _observer -> GetDatasetListName(destination, data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_DSN");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Pack data
    for (int i = 0 ; i < 1024 ; i++) {
      packer.PackChar(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get dataset list file
  else if (!strcmp(command, "GET_DSP")) {

    // Data
    char data[1024];
      
    // Observer 
    if (_observer != NULL) {
      _observer -> GetDatasetListPrefix(destination, data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_DSP");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Pack data
    for (int i = 0 ; i < 1024 ; i++) {
      packer.PackChar(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get data
  else if (!strcmp(command, "GET_DSS")) {

    // Data
    int size;
      
    // Observer 
    if (_observer != NULL) {
      _observer -> GetDatasetListSize(&size);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_DSS");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    packer.PackInt(size);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get display size
  else if (!strcmp(command, "GET_DIS")) {

    // Data
    int width = 0;
    int height = 0;
    bool fullScreen = false;
    float left = 0.0;
    float right = 0.0;
    float bottom = 0.0;
    float top = 0.0;
      
    // Observer
    if (_observer != NULL) {
      _observer -> GetDisplaySize(destination, &width, &height, &fullScreen, 
                                  &left, &right, &bottom, &top);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_DIS");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Flag
    int flag = 0;
    if (fullScreen == true) {
      flag = 1;
    }

    // Pack data
    packer.PackInt(width);
    packer.PackInt(height);
    packer.PackInt(flag);
    packer.PackFloat(left);
    packer.PackFloat(right);
    packer.PackFloat(bottom);
    packer.PackFloat(top);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get data type
  else if (!strcmp(command, "GET_DTP")) {

    // Data
    int type;
      
    // Observer 
    if (_observer != NULL) {
      _observer -> GetDatasetType(&type);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_DTP");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    packer.PackInt(type);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get frustum
  else if (!strcmp(command, "GET_FRU")) {

    // Data
    float left = 0.0;
    float right = 0.0;
    float bottom = 0.0;
    float top = 0.0;

    // Observer
    if (_observer != NULL) {
      _observer -> GetFrustum(destination, &left, &right, &bottom, &top);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_FRU");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Pack data
    packer.PackFloat(left);
    packer.PackFloat(right);
    packer.PackFloat(bottom);
    packer.PackFloat(top);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get map
  else if (!strcmp(command, "GET_MAP")) {

    // Data
    unsigned char data[1024];

    // Observer
    if (_observer != NULL) {
      _observer -> GetMap(data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_MAP");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    for (int i = 0 ; i < 1024 ; i++) {
      packer.PackUnsignedChar(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get map
  else if (!strcmp(command, "GET_OVV")) {

    // Data
    float data[24];

    // Observer
    if (_observer != NULL) {
      _observer -> GetOverviewVertices(data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_OVV");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    for (int i = 0 ; i < 24 ; i++) {
      packer.PackFloat(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get prefix
  else if (!strcmp(command, "GET_PRE")) {

    // Data
    char data[1024];
      
    // Observer 
    if (_observer != NULL) {
      _observer -> GetPrefix(data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_PRE");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    for (int i = 0 ; i < 1024 ; i++) {
      packer.PackChar(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get rotation matrix
  else if (!strcmp(command, "GET_ROT")) {

    // Data
    float data[16];

    // Observer
    if (_observer != NULL) {
      _observer -> GetRotationMatrix(data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_ROT");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    for (int i = 0 ; i < 16 ; i++) {
      packer.PackFloat(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get scale matrix
  else if (!strcmp(command, "GET_SCA")) {

    // Data
    float data[16];

    // Observer
    if (_observer != NULL) {
      _observer -> GetScaleMatrix(data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_SCA");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    for (int i = 0 ; i < 16 ; i++) {
      packer.PackFloat(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get slice frequency
  else if (!strcmp(command, "GET_SLF")) {

    // Data
    double data;

    // Observer
    if (_observer != NULL) {
      _observer -> GetSliceFrequency(&data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_SLF");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    float frequency = (float) data;
    packer.PackFloat(frequency);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get total display dimensions
  else if (!strcmp(command, "GET_TDD")) {

    // Data
    int w = 0;
    int h = 0;

    // Observer
    if (_observer != NULL) {
      _observer -> GetTotalDisplayDimensions(&w, &h);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_TDD");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    packer.PackInt(w);
    packer.PackInt(h);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;
    
  }


  // get total display dimensions
  else if (!strcmp(command, "GET_TDF")) {

    // Data
    float data[6];

    // Observer
    if (_observer != NULL) {
      _observer -> GetTotalDisplayFrustum(data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_TDF");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    packer.PackFloat(data[0]);
    packer.PackFloat(data[1]);
    packer.PackFloat(data[2]);
    packer.PackFloat(data[3]);
    packer.PackFloat(data[4]);
    packer.PackFloat(data[5]);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;
    
  }


  // get translation matrix
  else if (!strcmp(command, "GET_TRA")) {

    // Data
    float data[16];

    // Observer
    if (_observer != NULL) {
      _observer -> GetTranslationMatrix(data);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_TRA");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    for (int i = 0 ; i < 16 ; i++) {
      packer.PackFloat(data[i]);
    }

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get vertical field of view
  else if (!strcmp(command, "GET_VFV")) {

    // Data
    float vFOV = 0.0;

    // Observer
    if (_observer != NULL) {
      _observer -> GetVerticalFieldOfView(&vFOV);
    }
    
    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_VFV");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(0);

    // Pack data
    packer.PackFloat(vFOV);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // get viewport
  else if (!strcmp(command, "GET_VIE")) {

    // Data
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    // Observer
    if (_observer != NULL) {
      _observer -> GetViewport(destination, &x, &y, &w, &h);
    }

    // Buffer
    memset(buffer, 0, 2048);

    // Command
    memset(command, 0, 8);
    strcpy(command, "CMD_VIE");

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Pack data
    packer.PackInt(x);
    packer.PackInt(y);
    packer.PackInt(w);
    packer.PackInt(h);

    // Send message
    messageLength = 0;
    if ((messageLength = send(_clientSocketFileDescriptor,
                              buffer,
                              2048,
                              0)) != 2048) {
      perror("UserInterfaceServer: Error sending message");
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // set axis off
  else if (!strcmp(command, "CMD_AOF")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetAxisOff();
    }

  }


  // set axis on
  else if (!strcmp(command, "CMD_AON")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetAxisOn();
    }

  }


  // set axis on
  else if (!strcmp(command, "CMD_APO")) {

    // Unpack data from buffer
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    unpacker.UnpackFloat(&x);
    unpacker.UnpackFloat(&y);
    unpacker.UnpackFloat(&z);

    // Observer
    if (_observer != NULL) {
      _observer -> SetAxisPosition(x, y, z);
    }

  }


  // set bounding box off
  else if (!strcmp(command, "CMD_BOF")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBoundingBoxOff();
    }

  }


  // set bounding box on
  else if (!strcmp(command, "CMD_BON")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBoundingBoxOn();
    }

  }


  // set brick box off
  else if (!strcmp(command, "CMD_BBF")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBrickBoxOff();
    }

  }


  // set brick box on
  else if (!strcmp(command, "CMD_BBN")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBrickBoxOn();
    }

  }


  // set clipping planes
  else if (!strcmp(command, "CMD_CLI")) {

    // Unpack data from buffer
    float near = 0.0;
    float far = 0.0;
    unpacker.UnpackFloat(&near);
    unpacker.UnpackFloat(&far);
 
    // Observer
    if (_observer != NULL) {
      _observer -> SetClippingPlanes(destination, near, far);
    }

  }


  // set command disconnect
  else if (!strcmp(command, "CMD_DCT")) {

    // If client socket is open
    if (_clientSocketFileDescriptor != -1) {

      // Shutdown socket
      //if (shutdown(_clientSocketFileDescriptor, SHUT_RDWR) == -1) {
      //  perror("UserInterfaceServer: Error during socket shutdown");
      //}

      // Close socket
      if (close(_clientSocketFileDescriptor) == -1) {
        perror("UserInterfaceServer: Error during socket close");
      }

      // Set client socket file descriptor
      _clientSocketFileDescriptor = -1;

    }

    // Set connected flag
    _isConnected = false;

    // Return error
    return USER_INTERFACE_SERVER_ERROR;

  }


  // set command exit
  else if (!strcmp(command, "CMD_EXI")) {

    // If client socket is open
    if (_clientSocketFileDescriptor != -1) {

      // Shutdown socket
      //if (shutdown(_clientSocketFileDescriptor, SHUT_RDWR) == -1) {
      //  perror("UserInterfaceServer: Error during socket shutdown");
      //}

      // Close socket
      if (close(_clientSocketFileDescriptor) == -1) {
        perror("UserInterfaceServer: Error during socket close");
      }

      // Set client socket file descriptor
      _clientSocketFileDescriptor = -1;

    }

    // Set connected flag
    _isConnected = false;

    // Observer
    if (_observer != NULL) {
      _observer -> SetCommandExit();
    }

    // Return error
    return USER_INTERFACE_SERVER_ERROR;

  }


  // set command render
  else if (!strcmp(command, "CMD_REN")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetCommandRender();
    }

  }


  // set data
  else if (!strcmp(command, "CMD_DAT")) {

    // Unpack data from buffer
    char data[1024];
    int ramSize = -1;
    int vramSize = 1;
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackChar(&data[i]);
    }
    unpacker.UnpackInt(&ramSize);
    unpacker.UnpackInt(&vramSize);
      
    // Observer 
    if (_observer != NULL) {
      _observer -> SetData(data, ramSize, vramSize);
    }

  }


  // set frustum
  else if (!strcmp(command, "CMD_FRU")) {

    // Unpack data from buffer
    float left = 0.0;
    float right = 0.0;
    float bottom = 0.0;
    float top = 0.0;
    unpacker.UnpackFloat(&left);
    unpacker.UnpackFloat(&right);
    unpacker.UnpackFloat(&bottom);
    unpacker.UnpackFloat(&top);
    
    // Observer
    if (_observer != NULL) {
      _observer -> SetFrustum(destination, left, right, bottom, top);
    }

  }


  // set map
  else if (!strcmp(command, "CMD_MAP")) {

    // Unpack data from buffer
    unsigned char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackUnsignedChar(&data[i]);
    }

    // Observer
    if (_observer != NULL) {
      _observer -> SetMap(data);
    }

  }


  // set 16-bit map
  else if (!strcmp(command, "CMD_M16")) {

    // Payload
    unsigned char payload[65536*4];

    // Wait for message from client
    if ((messageLength = recv(_clientSocketFileDescriptor, 
                              payload, 
                              65536*4, 
                              MSG_WAITALL)) != 65536*4) {
      return USER_INTERFACE_SERVER_ERROR;
    }

    // Data
    unsigned char data[65536*4];

    // Data packer
    DataPacker punpacker;
    punpacker.SetBuffer(payload, 65536*4);
    
    for (int i = 0 ; i < 65536*4 ; i++) {
      punpacker.UnpackUnsignedChar(&data[i]);
    }

    // Observer
    if (_observer != NULL) {
      _observer -> SetMap16(data);
    }

  }


  // set prefix
  else if (!strcmp(command, "CMD_PRE")) {

    // Unpack data from buffer
    char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackChar(&data[i]);
    }
      
    // Observer 
    if (_observer != NULL) {

      _observer -> SetPrefix(data);
    }

  }


  // set rotation matrix
  else if (!strcmp(command, "CMD_ROT")) {

    // Unpack data from buffer
    float data[16];
    for (int i = 0 ; i < 16 ; i++) {
      unpacker.UnpackFloat(&data[i]);
    }

    // Observer
    if (_observer != NULL) {
      _observer -> SetRotationMatrix(data);
    }

  }


  // reset view
  else if (!strcmp(command, "CMD_RVI")) {

    // Observer
    if (_observer != NULL) {
      _observer -> ResetView();
    }

    // Return reply
    return USER_INTERFACE_SERVER_REPLY;

  }


  // set scale matrix
  else if (!strcmp(command, "CMD_SCA")) {

    // Unpack data from buffer
    float data[16];
    for (int i = 0 ; i < 16 ; i++) {
      unpacker.UnpackFloat(&data[i]);
    }

    // Observer
    if (_observer != NULL) {
      _observer -> SetScaleMatrix(data);
    }

  }


  // set slice frequency
  else if (!strcmp(command, "CMD_SLF")) {

    // Unpack data from buffer
    float data;
    unpacker.UnpackFloat(&data);    

    // Observer
    if (_observer != NULL) {
      _observer -> SetSliceFrequency((double) data);
    }

  }


  // set translation matrix
  else if (!strcmp(command, "CMD_TRA")) {

    // Unpack data from buffer
    float data[16];
    for (int i = 0 ; i < 16 ; i++) {
      unpacker.UnpackFloat(&data[i]);
    }

    // Observer
    if (_observer != NULL) {
      _observer -> SetTranslationMatrix(data);
    }

  }

  
  // set viewport
  else if (!strcmp(command, "CMD_VIE")) {

    // Unpack data from buffer
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    unpacker.UnpackInt(&x);
    unpacker.UnpackInt(&y);
    unpacker.UnpackInt(&w);
    unpacker.UnpackInt(&h);

    // Observer
    if (_observer != NULL) {
      _observer -> SetViewport(destination, x, y, w, h);
    }

  }

  
  // Return OK
  return USER_INTERFACE_SERVER_OK;

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::Init(int port) {
  
  // Assign server port number
  _port = port;

  // Create server socket
  if ((_serverSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("UserInterfaceServer: Error creating socket");
    return;
  }

  // Set SO_REUSEADDR
  int optVal = 1;
  int optLen = sizeof(optVal);
  if(setsockopt(_serverSocketFileDescriptor, 
                SOL_SOCKET, 
                SO_REUSEADDR, 
                (void*) &optVal, 
                (socklen_t) optLen) != 0) {
    perror("UserInterfaceServer: Error setting SO_REUSEADDR");
  }

  // Set TCP_NODELAY
  optVal = 1;
  optLen = sizeof(optVal);
  if(setsockopt(_serverSocketFileDescriptor,
                IPPROTO_TCP,
                TCP_NODELAY,
                (void*) &optVal,
                (socklen_t) optLen) != 0) {
    perror("UserInterfaceServer: Error setting TCP_NODELAY");
  }
  
  // Initialize server address
  memset(&_serverAddress, 0, sizeof(_serverAddress));
  _serverAddress.sin_family = AF_INET;
  _serverAddress.sin_port = htons(_port);
  
  // Bind address
  if (bind(_serverSocketFileDescriptor, 
           (struct sockaddr*) &_serverAddress, 
           sizeof(_serverAddress)) != 0) {
    perror("UserInterfaceServer: Error binding address");
    close(_serverSocketFileDescriptor);
  }
  
  // Set listen mode
  if (listen(_serverSocketFileDescriptor, 5) != 0) {
    perror("UserInterfaceServer: Error setting listen mode");
    close(_serverSocketFileDescriptor);
  }
  
  // Set is initialized flag
  _isInitialized = true;
  
}

/*--------------------------------------------------------------------------*/

bool UserInterfaceServer::IsInit() {

  // Check if connection is established
  if (_isInitialized == true && _isConnected == true) {
    return true;
  }

  // Connection is not established
  return false;

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::SendCommandExit() {

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);

  // Destination
  int destination = 0;
    
  // Message length
  int messageLength = 0;

  // Command
  memset(command, 0, 8);
  strcpy(command, "CMD_EXI");

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(0);

  // Send message
  if ((messageLength = send(_clientSocketFileDescriptor,
                            buffer,
                            2048,
                            0)) != 2048) {
    perror("UserInterfaceServer: Error sending message");
  }  

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::SendProgress(int value, int total) {

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);

  // Destination
  int destination = 0;
    
  // Message length
  int messageLength = 0;

  // Command
  memset(command, 0, 8);
  strcpy(command, "CMD_PRO");

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack value and total
  packer.PackInt(value);
  packer.PackInt(total);

  // Send message
  if ((messageLength = send(_clientSocketFileDescriptor,
                            buffer,
                            2048,
                            0)) != 2048) {
    perror("UserInterfaceServer: Error sending message");
  }  

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::SendRotationMatrix(float m[16]) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_ROT");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 16 ; i++) {
    packer.PackFloat(m[i]);
  }

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_clientSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::SendScaleMatrix(float m[16]) {

 // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_SCA");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 16 ; i++) {
    packer.PackFloat(m[i]);
  }

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_clientSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::SendTranslationMatrix(float m[16]) {

 // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_TRA");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 16 ; i++) {
    packer.PackFloat(m[i]);
  }

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_clientSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::SetObserver(UserInterfaceServerAbstractCommand* 
                                      observer) {

  // Set observer
  _observer = observer;

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServer::Shutdown() {

  // If server socket is open
  if (_serverSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("UserInterfaceServer: Error during socket shutdown");
    }

    // Close socket
    if (close(_serverSocketFileDescriptor) == -1) {
      perror("UserInterfaceServer: Error during socket close");
    }

    // Reset file descriptor
    _serverSocketFileDescriptor = -1;

  }

  // If client socket is open
  if (_clientSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_clientSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("UserInterfaceServer: Error during socket shutdown");
    }

    // Close socket
    if (close(_clientSocketFileDescriptor) == -1) {
      perror("UserInterfaceServer: Error during socket close");
    }

    // Reset file descriptor
    _clientSocketFileDescriptor = -1;

  }

  // Set is connected flag
  _isConnected = false;

  // Set is initialized flag
  _isInitialized = false;

}

/*--------------------------------------------------------------------------*/
