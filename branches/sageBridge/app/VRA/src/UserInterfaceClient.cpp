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

#include "UserInterfaceClient.h"
#include <Fl/Fl.H>

/*--------------------------------------------------------------------------*/

UserInterfaceClient::UserInterfaceClient() {

  // Initialize exit flag
  _exitFlag = false;

  // Initialize hostname
  memset(_hostname, 0, 1024);

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

UserInterfaceClient::~UserInterfaceClient() {

  // Set exit flag
  _exitFlag = true;

  // If socket is open
  if (_serverSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("UserInterfaceClient: Error during socket shutdown");
    }

    // Close socket
    if (close(_serverSocketFileDescriptor) == -1) {
      perror("UserInterfaceClient: Error during socket close");
    }

  }

  // Wait for thread to finish
  if (_isInitialized == true) {
    pthread_join(_thread, NULL);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::Disconnect() {

  // Check that client is connected
  if (_isInitialized == false) {
    return;
  }

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DCT");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }


  // Set exit flag
  _exitFlag = true;


  // If socket is open
  if (_serverSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("UserInterfaceClient: Error during socket shutdown");
    }

    // Close socket
    if (close(_serverSocketFileDescriptor) == -1) {
      perror("UserInterfaceClient: Error during socket close");
    }

  }


  // Wait for thread listening for server updates to complete
  pthread_join(_thread, NULL);

  // Reset hostname
  memset(_hostname, 0, 1024);

  // Reset port
  _port = 0;

  // Reset socket file descriptor
  _serverSocketFileDescriptor = -1;

  // Set is initialized flag to false
  _isInitialized = false;

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetAxis() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_AXS");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetAxisPosition() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_APO");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetBoundingBox() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_BOX");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetBrickBox() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_BBX");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetClippingPlanes(int node) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_CLI");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetData() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_DAT");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetDatasetListFile(int index) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_DSF");

  // Destination
  int destination = index;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetDatasetListName(int index) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_DSN");

  // Destination
  int destination = index;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetDatasetListPrefix(int index) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_DSP");

  // Destination
  int destination = index;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetDatasetListSize() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_DSS");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetDatasetType() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_DTP");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetDisplaySize(int node) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_DIS");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetFrustum(int node) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_FRU");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetMap() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_MAP");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetNumberOfSlaves() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_NUM");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetOverviewVertices() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_OVV");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetPrefix() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_PRE");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetRotationMatrix() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_ROT");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetScaleMatrix() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_SCA");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetSliceFrequency() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_SLF");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetTotalDisplayDimensions() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_TDD");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetTotalDisplayFrustum() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_TDF");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetTranslationMatrix() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_TRA");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetVerticalFieldOfView() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_VFV");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::GetViewport(int node) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "GET_VIE");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

int UserInterfaceClient::Init(char* hostname, int port) {

  // Assign hostname
  memcpy(_hostname, hostname, 1024);

  // Assign server port number
  _port = port;

  // Create server socket
  if ((_serverSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("UserInterfaceClient: Error creating socket");
    return USER_INTERFACE_CLIENT_ERROR;
  }

  // Set TCP_NODELAY
  int optVal = 1;
  int optLen = sizeof(optVal);
  if(setsockopt(_serverSocketFileDescriptor,
                IPPROTO_TCP,
                TCP_NODELAY,
                (void*) &optVal,
                (socklen_t) optLen) != 0) {
    perror("UserInterfaceServer: Error setting TCP_NODELAY");
  }

  // Get host by name
  if ((_server = gethostbyname(hostname)) == NULL) {
    herror("UserInterfaceClient: Error resolving hostname");
    close(_serverSocketFileDescriptor);
    return USER_INTERFACE_CLIENT_ERROR;
  }

  // Initialize server address
  memset(&_serverAddress, 0, sizeof(_serverAddress));
  _serverAddress.sin_family = AF_INET;
  bcopy((char*) _server -> h_addr,
        (char*) &_serverAddress.sin_addr.s_addr,
        _server -> h_length);
  _serverAddress.sin_port = htons(_port);

  // Connect to server
  if (connect(_serverSocketFileDescriptor, 
              (struct sockaddr*) &_serverAddress, 
              sizeof(_serverAddress)) != 0) {
    perror("UserInterfaceClient: Error connecting to server");
    close(_serverSocketFileDescriptor);
    _serverSocketFileDescriptor = -1;
    return USER_INTERFACE_CLIENT_ERROR;
  }

  // Reset exit flag
  _exitFlag = false;

  // Create thread listening for server updates
  pthread_create(&_thread,
                 NULL,
                 UserInterfaceClient::ThreadFunction,
                 this);

  // Set is initialized flag
  _isInitialized = true;

  // Return OK
  return USER_INTERFACE_CLIENT_OK;

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::ResetView() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_RVI");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendAxisOff() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_AOF");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendAxisOn() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_AON");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendAxisPosition(float x, float y, float z) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_AON");

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
  packer.PackFloat(x);
  packer.PackFloat(y);
  packer.PackFloat(z);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendBoundingBoxOff() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BOF");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendBoundingBoxOn() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BON");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendBrickBoxOff() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BBF");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendBrickBoxOn() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BBN");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendClippingPlanes(int node, 
                                             float near, 
                                             float far) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_CLI");

  // Destination
  int destination = node;

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendCommandExit() {

  // Check that client is connected
  if (_isInitialized == false) {
    return;
  }

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_EXI");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }


  // Set exit flag
  _exitFlag = true;


  // If socket is open
  if (_serverSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("UserInterfaceClient: Error during socket shutdown");
    }

    // Close socket
    if (close(_serverSocketFileDescriptor) == -1) {
      perror("UserInterfaceClient: Error during socket close");
    }

  }


  // Wait for thread listening for server updates to complete
  pthread_join(_thread, NULL);

  // Reset hostname
  memset(_hostname, 0, 1024);

  // Reset port
  _port = 0;

  // Reset socket file descriptor
  _serverSocketFileDescriptor = -1;

  // Set is initialized flag to false
  _isInitialized = false;

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendCommandRender() {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_REN");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendData(char* filename) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DAT");

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
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(filename[i]);
  }
  packer.PackInt(-1);
  packer.PackInt(-1);
  
  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendData(char* filename,
                                   int ramSize, int vramSize) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DAT");

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
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(filename[i]);
  }
  packer.PackInt(ramSize);
  packer.PackInt(vramSize);
  
  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendFrustum(int node, float left, float right, 
                                      float bottom, float top) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_FRU");

  // Destination
  int destination = node;

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
  
  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendMap(unsigned char* map) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_MAP");

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
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackUnsignedChar(map[i]);
  }
  
  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendMap16(unsigned char* map) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_M16");

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

  // Payload
  unsigned char payload[65536*4];
  memset(payload, 0, 65536*4);

  // Data packer
  DataPacker ppacker;
  ppacker.SetBuffer(payload, 65536*4);

  // Pack data
  for (int i = 0 ; i < 65536*4 ; i++) {
    ppacker.PackUnsignedChar(map[i]);
  }

  // Send command
  messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            payload, 
                            65536*4, 
                            0)) != 65536*4) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendPrefix(char* prefix) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_PRE");

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
  for (int i = 0 ; i < (int) strlen(prefix) ; i++) {
    packer.PackChar(prefix[i]);
  }

  // Pack '/'
  packer.PackChar('/');

  // Pack end of string character
  packer.PackChar('\0');
  
  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendRotationMatrix(float matrix[16]) {

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
    packer.PackFloat(matrix[i]);
  }

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendScaleMatrix(float matrix[16]) {

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
    packer.PackFloat(matrix[i]);
  }

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendSliceFrequency(double frequency) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_SLF");

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
  packer.PackFloat((float) frequency);

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendTranslationMatrix(float matrix[16]) {

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
    packer.PackFloat(matrix[i]);
  }

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SendViewport(int node, int x, int y, 
                                       int w, int h) {

  // Buffer
  unsigned char* buffer[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_VIE");

  // Destination
  int destination = node;

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

  // Send command
  int messageLength = 0;
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            2048, 
                            0)) != 2048) {
    perror("UserInterfaceClient: Error sending message");
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClient::SetObserver(UserInterfaceClientAbstractCommand* 
                                      observer) {

  // Set observer
  _observer = observer;

}

/*--------------------------------------------------------------------------*/

void *UserInterfaceClient::ThreadFunction(void* ptr) {

  // Cast user interface client class
  UserInterfaceClient* uiclient = (UserInterfaceClient*) ptr;

  // Loop accepting connections
  while (1) {

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

    // Wait for message
    if ((messageLength = recv(uiclient -> _serverSocketFileDescriptor, 
                              buffer, 
                              2048, 
                              MSG_WAITALL)) != 2048) {

      // If exit flag is set
      if (uiclient -> _exitFlag == true) {
        
        // Exit thread
        pthread_exit(NULL);

      }

      // Error receiving message from server
      else {

        // Print error message
        perror("UserInterfaceClient: Error receiving message");

        // Set exit flag
        uiclient -> _exitFlag = true;

        // If socket is open
        if (uiclient -> _serverSocketFileDescriptor != -1) {
          
          // Shutdown socket
          if (shutdown(uiclient -> _serverSocketFileDescriptor, 
                       SHUT_RDWR) == -1) {
            perror("UserInterfaceClient: Error during socket shutdown");
          }
          
          // Close socket
          if (close(uiclient -> _serverSocketFileDescriptor) == -1) {
            perror("UserInterfaceClient: Error during socket close");
          }
          
        }

        // Reset hostname
        memset(uiclient -> _hostname, 0, 1024);
        
        // Reset port
        uiclient -> _port = 0;
        
        // Reset socket file descriptor
        uiclient -> _serverSocketFileDescriptor = -1;
        
        // Set is initialized flag to false
        uiclient -> _isInitialized = false;
        
        // Observer
        if (uiclient -> _observer != NULL) {
          uiclient -> _observer -> SetCommandExit();
        }
        
        // Exit thread
        pthread_exit(NULL);

      }

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
    
    // axis off
    if (!strcmp(command, "CMD_AOF")) {

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetAxisOff();
      }

    }


    // axis on
    else if (!strcmp(command, "CMD_AON")) {

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetAxisOn();
      }

    }


    // axis position
    else if (!strcmp(command, "CMD_APO")) {

      // Unpack data from buffer
      float x = 0.0;
      float y = 0.0;
      float z = 0.0;
      unpacker.UnpackFloat(&x);
      unpacker.UnpackFloat(&y);
      unpacker.UnpackFloat(&z);
 
      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetAxisPosition(x, y, z);
      }

    }


    // bounding box off
    else if (!strcmp(command, "CMD_BOF")) {

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetBoundingBoxOff();
      }

    }


    // bounding box on
    else if (!strcmp(command, "CMD_BON")) {

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetBoundingBoxOn();
      }

    }


    // brick box off
    else if (!strcmp(command, "CMD_BBF")) {

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetBrickBoxOff();
      }

    }


    // brick box on
    else if (!strcmp(command, "CMD_BBN")) {

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetBrickBoxOn();
      }

    }


    // clipping planes
    else if (!strcmp(command, "CMD_CLI")) {

      // Unpack data from buffer
      float near = 0.0;
      float far = 0.0;
      unpacker.UnpackFloat(&near);
      unpacker.UnpackFloat(&far);
 
      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetClippingPlanes(destination, near, far);
      }

    }


    // exit
    else if(!strcmp(command, "CMD_EXI")) {

      // Set exit flag
      uiclient -> _exitFlag = true;

      // If socket is open
      if (uiclient -> _serverSocketFileDescriptor != -1) {

        // Shutdown socket
        if (shutdown(uiclient -> _serverSocketFileDescriptor, 
                     SHUT_RDWR) == -1) {
          perror("UserInterfaceClient: Error during socket shutdown");
        }

        // Close socket
        if (close(uiclient -> _serverSocketFileDescriptor) == -1) {
          perror("UserInterfaceClient: Error during socket close");
        }

      }

      // Reset hostname
      memset(uiclient -> _hostname, 0, 1024);

      // Reset port
      uiclient -> _port = 0;

      // Reset socket file descriptor
      uiclient -> _serverSocketFileDescriptor = -1;

      // Set is initialized flag to false
      uiclient -> _isInitialized = false;

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetCommandExit();
      }

      // Exit thread
      pthread_exit(NULL);

    }


    // data
    else if(!strcmp(command, "CMD_DAT")) {

      // Unpack data from buffer
      char data[1024];
      int ramSize = 0;
      int vramSize = 0;
      for (int i = 0 ; i < 1024 ; i++) {
        unpacker.UnpackChar(&data[i]);
      }
      unpacker.UnpackInt(&ramSize);
      unpacker.UnpackInt(&vramSize);
      
      // Observer 
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetData(data, ramSize, vramSize);
      }

    }


    // dataset list file
    else if(!strcmp(command, "CMD_DSF")) {

      // Unpack data from buffer
      char data[1024];
      for (int i = 0 ; i < 1024 ; i++) {
        unpacker.UnpackChar(&data[i]);
      }
      
      // Observer 
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetDatasetListFile(destination, data);
      }

    }


    // dataset list name
    else if(!strcmp(command, "CMD_DSN")) {

      // Unpack data from buffer
      char data[1024];
      for (int i = 0 ; i < 1024 ; i++) {
        unpacker.UnpackChar(&data[i]);
      }
      
      // Observer 
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetDatasetListName(destination, data);
      }

    }


    // dataset list name
    else if(!strcmp(command, "CMD_DSP")) {

      // Unpack data from buffer
      char data[1024];
      for (int i = 0 ; i < 1024 ; i++) {
        unpacker.UnpackChar(&data[i]);
      }
      
      // Observer 
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetDatasetListPrefix(destination, data);
      }

    }


    // dataset list size
    else if(!strcmp(command, "CMD_DSS")) {

      // Unpack data from buffer
      int data;
      unpacker.UnpackInt(&data);
      
      // Observer 
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetDatasetListSize(data);
      }

    }


    // dataset type
    else if(!strcmp(command, "CMD_DTP")) {

      // Unpack data from buffer
      int data;
      unpacker.UnpackInt(&data);
      
      // Observer 
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetDatasetType(data);
      }

    }


    // display size
    else if (!strcmp(command, "CMD_DIS")) {

      // Unpack data from buffer
      int width = 0;
      int height = 0;
      int flag = 0;
      float left = 0.0;
      float right = 0.0;
      float bottom = 0.0;
      float top = 0.0;
      unpacker.UnpackInt(&width);
      unpacker.UnpackInt(&height);
      unpacker.UnpackInt(&flag);
      unpacker.UnpackFloat(&left);
      unpacker.UnpackFloat(&right);
      unpacker.UnpackFloat(&bottom);
      unpacker.UnpackFloat(&top);
      
      // Full screen flag
      bool fullScreen = false;
      if (flag == 1) {
        fullScreen = true;
      }

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetDisplaySize(destination, width, height, 
                                                fullScreen, left, right, 
                                                bottom, top);
      }

    }


    // frustum
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
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetFrustum(destination, left, right, 
                                            bottom, top);
      }

    }


    // map
    else if (!strcmp(command, "CMD_MAP")) {

      // Unpack data from buffer
      unsigned char data[1024];
      for (int i = 0 ; i < 1024 ; i++) {
        unpacker.UnpackUnsignedChar(&data[i]);
      }

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetMap(data);
      }

    }


    // progress
    else if (!strcmp(command, "CMD_PRO")) {

      // Unpack data from buffer
      int value = 0;
      int total = 0;
      unpacker.UnpackInt(&value);
      unpacker.UnpackInt(&total);

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetProgress(value, total);
      }

    }


    // overview vertices
    else if (!strcmp(command, "CMD_OVV")) {

      // Unpack data from buffer
      float data[24];
      for (int i = 0 ; i < 24 ; i++) {
        unpacker.UnpackFloat(&data[i]);
      }

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetOverviewVertices(data);
      }

    }


    // prefix
    else if(!strcmp(command, "CMD_PRE")) {

      // Unpack data from buffer
      char data[1024];
      for (int i = 0 ; i < 1024 ; i++) {
        unpacker.UnpackChar(&data[i]);
      }
      
      // Observer 
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetPrefix(data);
      }

    }

    
    // rotation matrix
    else if (!strcmp(command, "CMD_ROT")) {

      // Unpack data from buffer
      float data[16];
      for (int i = 0 ; i < 16 ; i++) {
        unpacker.UnpackFloat(&data[i]);
      }

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetRotationMatrix(data);
      }

    }


    // scale matrix
    else if (!strcmp(command, "CMD_SCA")) {

      // Unpack data from buffer
      float data[16];
      for (int i = 0 ; i < 16 ; i++) {
        unpacker.UnpackFloat(&data[i]);
      }

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetScaleMatrix(data);
      }

    }


    // slice frequency
    else if (!strcmp(command, "CMD_SLF")) {

      // Unpack data from buffer
      float data;
      unpacker.UnpackFloat(&data);
      
      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetSliceFrequency((double) data);
      }

    }


    // total display dimensions
    else if (!strcmp(command, "CMD_TDD")) {
      
      // Unpack data from buffer
      int w = 0;
      int h = 0;
      unpacker.UnpackInt(&w);
      unpacker.UnpackInt(&h);

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetTotalDisplayDimensions(w, h);
      }

    }


    // total display frustum
    else if (!strcmp(command, "CMD_TDF")) {
      
      // Unpack data from buffer
      float data[6];
      unpacker.UnpackFloat(&data[0]);
      unpacker.UnpackFloat(&data[1]);
      unpacker.UnpackFloat(&data[2]);
      unpacker.UnpackFloat(&data[3]);
      unpacker.UnpackFloat(&data[4]);
      unpacker.UnpackFloat(&data[5]);

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetTotalDisplayFrustum(data);
      }

    }


    // translation matrix
    else if (!strcmp(command, "CMD_TRA")) {

      // Unpack data from buffer
      float data[16];
      for (int i = 0 ; i < 16 ; i++) {
        unpacker.UnpackFloat(&data[i]);
      }

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetTranslationMatrix(data);
      }

    }


    // vertical field of view
    else if (!strcmp(command, "CMD_VFV")) {

      // Unpack data from buffer
      float vFOV = 0.0;
      unpacker.UnpackFloat(&vFOV);

      // Observer
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetVerticalFieldOfView(vFOV);
      }

    }


    // viewport matrix
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
      if (uiclient -> _observer != NULL) {
        uiclient -> _observer -> SetViewport(destination, x, y, w, h);
      }
      
    }


    // undefined command
    else {
    }


  }

}

/*--------------------------------------------------------------------------*/
