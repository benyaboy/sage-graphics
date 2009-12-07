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

#include "SynchronizerClient.h"

/*--------------------------------------------------------------------------*/

SynchronizerClient::SynchronizerClient() {

  // Initialize is initialized flag
  _isInitialized = false;
  
  // Initialize port number
  _port = 0;

  // Initialize server
  _server = NULL;

  // Reset socket file descriptor
  _serverSocketFileDescriptor = -1;

}

/*--------------------------------------------------------------------------*/

SynchronizerClient::~SynchronizerClient() {

  // If socket is open
  if (_serverSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("SynchronizerClient: Error during socket shutdown");
    }

    // Close socket
    if (close(_serverSocketFileDescriptor) == -1) {
      perror("SynchronizerClient: Error during socket close");
    }

  }

}

/*--------------------------------------------------------------------------*/

void SynchronizerClient::Barrier() {

  // Check is user called Init
  IsInitialized();

  // Message buffer
  char buffer[256];

  // Message length
  int messageLength = 0;

  // Initialize buffer
  memset(buffer, 0, 256);

  // Create message
  sprintf(buffer, "MSG_BARRIER 0");

  // Send message
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            255, 
                            0)) != 255) {
    perror("SynchronizerClient: Error sending barrier message");
  }

  // Wait for return message
  if ((messageLength = recv(_serverSocketFileDescriptor, 
                            buffer, 
                            255, 
                            MSG_WAITALL)) != 255) {
    perror("SynchronizerClient: Error receiving barrier message");
  }

}

/*--------------------------------------------------------------------------*/

void SynchronizerClient::Barrier(int level) {

  // Check is user called Init
  IsInitialized();                                                             

  // Message buffer
  char buffer[256];                                                            

  // Message length                                                         
  int messageLength = 0;

  // Initialize buffer
  memset(buffer, 0, 256);

  // Create message
  sprintf(buffer, "MSG_BARRIER %d", level);

  // Send message
  if ((messageLength = send(_serverSocketFileDescriptor,
                            buffer,
                            255,
                            0)) != 255) {                
    perror("SynchronizerClient: Error sending barrier message");
  }
                                                    
  // Wait for return message
  if ((messageLength = recv(_serverSocketFileDescriptor,
                            buffer,
                            255,                             
                            MSG_WAITALL)) != 255) {              
    perror("SynchronizerClient: Error receiving barrier message");
  }                    

}

/*--------------------------------------------------------------------------*/

void SynchronizerClient::Finalize() {

  // Check is user called Init
  IsInitialized();

  // Message buffer
  char buffer[256];

  // Message length
  int messageLength = 0;

  // Initialize buffer
  memset(buffer, 0, 256);

  // Create message
  sprintf(buffer, "MSG_FINALIZE 0");

  // Send message
  if ((messageLength = send(_serverSocketFileDescriptor, 
                            buffer, 
                            255, 
                            0)) != 255) {
    perror("SynchronizerClient: Error sending finalize message");
  }

  // Wait for return message
  if ((messageLength = recv(_serverSocketFileDescriptor, 
                            buffer, 
                            255, 
                            MSG_WAITALL)) != 255) {
    perror("SynchronizerClient: Error receiving finalize message");
  }

  // Shutdown socket
  if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
    perror("SynchronizerClient: Error during socket shutdown");
  }

  // Close socket
  if (close(_serverSocketFileDescriptor) == -1) {
    perror("SynchronizerClient: Error during socket close");
  }

  // Reset socket file descriptor
  _serverSocketFileDescriptor = -1;

  // Set is initialized flag to false
  _isInitialized = false;

}

/*--------------------------------------------------------------------------*/

void SynchronizerClient::Init(char* hostname, int port) {

  // Assign server port number
  _port = port;

  // Create server socket
  if ((_serverSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("SynchronizerClient: Error creating socket");
    return;
  }

  // Get host by name
  if ((_server = gethostbyname(hostname)) == NULL) {
    herror("SynchronizerClient: Error resolving hostname");
    close(_serverSocketFileDescriptor);
    return;
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
    perror("SynchronizerClient: Error connecting to server");
    close(_serverSocketFileDescriptor);
    return;
  }

  // Set is initialized flag
  _isInitialized = true;

}

/*--------------------------------------------------------------------------*/

bool SynchronizerClient::IsInitialized() {

  if (_isInitialized == false) {
    fprintf(stderr, 
            "SynchronizerClient: Call Init before any other method\n");
  }

  return _isInitialized;

}

/*--------------------------------------------------------------------------*/
