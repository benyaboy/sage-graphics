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

#ifndef USER_INTERFACE_SERVER_H
#define USER_INTERFACE_SERVER_H

/*--------------------------------------------------------------------------*/

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DataPacker.h"
#include "UserInterfaceServerAbstractCommand.h"

/*--------------------------------------------------------------------------*/

#define USER_INTERFACE_SERVER_ERROR -1
#define USER_INTERFACE_SERVER_OK 0
#define USER_INTERFACE_SERVER_REPLY 1
#define USER_INTERFACE_SERVER_NO_DATA 2

/*--------------------------------------------------------------------------*/

class UserInterfaceServer {

public:

  // Default constructor
  UserInterfaceServer();

  // Default destructor
  ~UserInterfaceServer();

  // Accept connection
  int Accept();

  // Process a command from the client
  int GetCommand();

  // Initialize server
  void Init(int port);

  // Is initialized
  bool IsInit();

  // Send exit command
  void SendCommandExit();

  // Send progress update
  void SendProgress(int value, int total);

  // Send rotation matrix
  void SendRotationMatrix(float m[16]);

  // Send scale matrix
  void SendScaleMatrix(float m[16]);

  // Send translation matrix
  void SendTranslationMatrix(float m[16]);

  // Set observer
  void SetObserver(UserInterfaceServerAbstractCommand* observer);

  // Shutdwon server
  void Shutdown();

private:

  // Client address
  struct sockaddr_in _clientAddress;

  // Client socket file descriptor
  int _clientSocketFileDescriptor;

  // Is connected to client flag
  bool _isConnected;

  // Is initialized flag
  bool _isInitialized;

  // Observer
  UserInterfaceServerAbstractCommand* _observer;

  // Server port number
  int _port;

  // Server
  struct hostent* _server;

  // Server address
  struct sockaddr_in _serverAddress;

  // Server socket file descriptor
  int _serverSocketFileDescriptor;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
