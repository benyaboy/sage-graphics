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

#include "SynchronizerServer.h"

/*--------------------------------------------------------------------------*/

SynchronizerServer::SynchronizerServer() {

  // Initialize condition mutex
  pthread_mutex_init(&_conditionMutex, NULL);

  // Initialize condition variable
  pthread_cond_init (&_conditionVariable, NULL);

  // Initialize finalize flag
  _finalizeFlag = false;

  // Initialize current level
  _level = 0;

  // Initialize number of clients connected
  _numberOfClients = 0;

  // Initialize number of clients waiting at barrier
  _numberOfClientsWaiting = 0;

  // Initialize port number
  _port = 0;

  // Initialize progress observer
  _progressObserver = NULL;

  // Initialize server socket file descriptor
  _serverSocketFileDescriptor = -1;

}

/*--------------------------------------------------------------------------*/

SynchronizerServer::~SynchronizerServer() {

  // Clean up mutex
  pthread_mutex_destroy(&_conditionMutex);

  // Clean up condition variable
  pthread_cond_destroy(&_conditionVariable);

  // If socket is open
  if (_serverSocketFileDescriptor != -1) {

    // Shutdown socket
    if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
      perror("SynchronizerServer: Error during socket shutdown");
    }

    // Close socket
    if (close(_serverSocketFileDescriptor) == -1) {
      perror("SynchronizerServer: Error during socket close");
    }

  }

  // Close client sockets
  for (int i = 0 ; i < _numberOfClients ; i++) {

    // If socket is opne
    if (_clientInfo[i]._clientSocketFileDescriptor != -1) {

      // Shutdown socket
      if (shutdown(_clientInfo[i]._clientSocketFileDescriptor, 
                   SHUT_RDWR) == -1) {
        perror("SynchronizerServer: Error during socket shutdown");
      }

      // Close socket
      if (close(_clientInfo[i]._clientSocketFileDescriptor) == -1) {
        perror("SynchronizerServer: Error during socket close");
      }

    }

  }

}

/*--------------------------------------------------------------------------*/

void SynchronizerServer::Finalize() {

  // If no clients are connected, force it to quit
  if (_numberOfClients == 0) {
    _finalizeFlag = true;
  }

  // Wait for monitor thread to complete
  pthread_join(_monitorThread, NULL);

  // Shutdown socket
  if (shutdown(_serverSocketFileDescriptor, SHUT_RDWR) == -1) {
    perror("SynchronizerServer: Error during server socket shutdown");
  }

  // Close socket
  if (close(_serverSocketFileDescriptor) == -1) {
    perror("SynchronizerServer: Error during server socket close");
  }

  // Wait for listen thread to complete
  pthread_join(_listenThread, NULL);

  // Wait for client threads to complete
  for (int i = 0 ; i < _numberOfClients ; i++) {
    pthread_join(_clientThread[i], NULL);
  }

  // Reset socket file descriptor
  _serverSocketFileDescriptor = -1;

  // Reset number of connected clients
  _numberOfClients = 0;

  // Reset number of clients waiting at barrier
  _numberOfClientsWaiting = 0;

  // Reset port
  _port = 0;

}

/*--------------------------------------------------------------------------*/

void SynchronizerServer::Init(int port) {

  // Set thread state
  _listenInfo._clientInfo = _clientInfo;
  _listenInfo._clientThread = _clientThread;
  _listenInfo._conditionMutex = &_conditionMutex;
  _listenInfo._conditionVariable = &_conditionVariable;
  _listenInfo._finalizeFlag = &_finalizeFlag;
  _listenInfo._level = &_level;
  _listenInfo._numberOfClients = &_numberOfClients;
  _listenInfo._numberOfClientsWaiting = &_numberOfClientsWaiting;
  _listenInfo._port = &_port;
  _listenInfo._progressObserver = _progressObserver;
  _listenInfo._serverAddress = &_serverAddress;
  _listenInfo._serverSocketFileDescriptor = &_serverSocketFileDescriptor;

  // Initialize server port
  _port = port;

  // Create thread to listen for new clients
  pthread_create(&_listenThread, 
                 NULL, 
                 SynchronizerServer::ListenThreadFunction,
                 &_listenInfo);

  // Create thread to monitor number of clients waiting at barrier
  pthread_create(&_monitorThread,
                 NULL,
                 SynchronizerServer::MonitorThreadFunction,
                 &_listenInfo);

}

/*--------------------------------------------------------------------------*/

void *SynchronizerServer::ClientThreadFunction(void* ptr) {

  // Cast thread info class
  SynchronizerServerClientThreadInfo* info = 
    (SynchronizerServerClientThreadInfo*) ptr;

  // Exit flag
  bool exitFlag = false;

  // Message buffer
  char buffer[256];

  // Message length
  int messageLength = 0;

  // Loop receiving messages
  while (exitFlag == false) {

    // Initialize message length
    messageLength = 0;

    // Initialize buffer
    memset(buffer, 0, 256);

    // Wait for message from client
    if ((messageLength = recv(info -> _clientSocketFileDescriptor, 
                              buffer, 
                              255, 
                              MSG_WAITALL)) != 255) {
      perror("SynchronizerServer: Error receiving message");

      // Set exit flag
      exitFlag = true;
    }

    // Command
    char command[256];
    memset(command, 0, 256);

    // Level
    int level = 0;

    // Get command
    sscanf(buffer, "%s %d", command, &level);

    // Process barrier message
    if (!strcmp(command, "MSG_BARRIER")) {

      // Lock mutex
      pthread_mutex_lock(info -> _conditionMutex);

      // Increment number of clients waiting at barrier
      *(info -> _numberOfClientsWaiting) += 1;

      // Update level
      *(info -> _level) = level;

      // Check condition and signal if all nodes are waiting on barrier
      if (*(info -> _numberOfClientsWaiting) >= *(info -> _numberOfClients)) {
        pthread_cond_signal(info -> _conditionVariable);
      }

      // Unlock mutex
      pthread_mutex_unlock(info -> _conditionMutex);

    }

    // Process disconnect message
    else if(!strcmp(command, "MSG_FINALIZE")) {
      
      // Lock mutex
      pthread_mutex_lock(info -> _conditionMutex);

      // Set finalize flag
      *(info -> _finalizeFlag) = true;

      // Increment number of clients waiting at barrier
      *(info -> _numberOfClientsWaiting) += 1;

      // Update level
      *(info -> _level) = level;

      // Check condition and signal if all nodes are waiting on barrier
      if (*(info -> _numberOfClientsWaiting) >= *(info -> _numberOfClients)) {
        pthread_cond_signal(info -> _conditionVariable);
      }

      // Unlock mutex
      pthread_mutex_unlock(info -> _conditionMutex);

      // Set exit flag
      exitFlag = true;

    }
    
    // Unknown message
    else {
      fprintf(stderr, "SynchronizerServer: Unknown message from client\n");
    }
    
  }

}

/*--------------------------------------------------------------------------*/

void *SynchronizerServer::ListenThreadFunction(void* ptr) {

  // Cast listen thread info class
  SynchronizerServerListenThreadInfo* info = 
    (SynchronizerServerListenThreadInfo*) ptr;

  // Create server socket
  if ((*(info -> _serverSocketFileDescriptor) = socket(AF_INET,
                                                       SOCK_STREAM, 
                                                       0)) == -1) {
    perror("SynchronizerServer: Error creating socket");
  }

  // Set SO_REUSEADDR
  int optVal = 1;
  int optLen = sizeof(optVal);
  if(setsockopt(*(info -> _serverSocketFileDescriptor), 
                SOL_SOCKET, 
                SO_REUSEADDR, 
                (void*) &optVal, 
                (socklen_t) optLen) != 0) {
    perror("SynchronizerServer: Error setting SO_REUSEADDR");
  }

  // Initialize server address
  memset(info -> _serverAddress, 0, sizeof(*(info -> _serverAddress)));
  (*(info -> _serverAddress)).sin_family = AF_INET;
  (*(info -> _serverAddress)).sin_port = htons(*(info -> _port));

  // Bind address
  if (bind(*(info -> _serverSocketFileDescriptor), 
           (struct sockaddr*) info -> _serverAddress, 
           sizeof(*(info -> _serverAddress))) != 0) {
    perror("SynchronizerServer: Error binding address");
    close(*(info -> _serverSocketFileDescriptor));
  }

  // Set listen mode
  if (listen(*(info -> _serverSocketFileDescriptor), 5) != 0) {
    perror("SynchronizerServer: Error setting listen mode");
    close(*(info -> _serverSocketFileDescriptor));
  }

  // Loop accepting new connections
  while (1) {

    // Variables to hold client info
    struct sockaddr_in clientAddress;
    int clientLength;
    int clientSocketFileDescriptor;

    // Initialize variables to hold client info
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientLength = sizeof(clientAddress);
    clientSocketFileDescriptor = 0;

    // Wait for a new connection
    clientSocketFileDescriptor = 
      accept(*(info -> _serverSocketFileDescriptor), 
             (struct sockaddr*) &clientAddress, 
             (socklen_t*) &clientLength);

    // Check for error with accept
    if (clientSocketFileDescriptor == -1) {
      if (*(info -> _finalizeFlag) == true) {
        pthread_exit(NULL);
      }
      else {
        perror("SynchronizerServer: Error accepting new connections");
      }
    }

    // Set SO_REUSEADDR
    int optVal = 1;
    int optLen = sizeof(optVal);
    if(setsockopt(clientSocketFileDescriptor, 
                  SOL_SOCKET, 
                  SO_REUSEADDR, 
                  (void*) &optVal, 
                  (socklen_t) optLen) != 0) {
      perror("SynchronizerServer: Error setting SO_REUSEADDR");
    }

    // Lock mutex
    pthread_mutex_lock(info -> _conditionMutex);

    // Check for error during accept
    if (clientSocketFileDescriptor == -1) {
      perror("SynchronizerServer: Error accepting new connection");
      close(*(info -> _serverSocketFileDescriptor));
    }

    // Update client address
    memcpy(&((info->_clientInfo)[*(info->_numberOfClients)]._clientAddress),
           &clientAddress,
           sizeof(clientAddress));

    // Update client socket file descriptor
    (info->_clientInfo)[*(info->_numberOfClients)]._clientSocketFileDescriptor =
      clientSocketFileDescriptor;

    // Update mutex and condition variable
    (info->_clientInfo)[*(info->_numberOfClients)]._conditionMutex =
      info -> _conditionMutex;
    (info->_clientInfo)[*(info->_numberOfClients)]._conditionVariable =
      info -> _conditionVariable;

    // Update finalize flag
    (info->_clientInfo)[*(info->_numberOfClients)]._finalizeFlag =
      info -> _finalizeFlag;

    // Update finalize flag
    (info->_clientInfo)[*(info->_numberOfClients)]._level =
      info -> _level;

    // Update number of clients and number of clients waiting
    (info->_clientInfo)[*(info->_numberOfClients)]._numberOfClients =
      info -> _numberOfClients;
    (info->_clientInfo)[*(info->_numberOfClients)]._numberOfClientsWaiting =
      info -> _numberOfClientsWaiting;

    // Create thread to handle new client
    pthread_create(&(info -> _clientThread)[*(info -> _numberOfClients)], 
                   NULL, 
                   SynchronizerServer::ClientThreadFunction,
                   &(info -> _clientInfo)[*(info -> _numberOfClients)]);

    // Increment number of connected clients
    *(info -> _numberOfClients) += 1;

    // Unlock mutex
    pthread_mutex_unlock(info -> _conditionMutex);

  }

}

/*--------------------------------------------------------------------------*/

void *SynchronizerServer::MonitorThreadFunction(void* ptr) {

  // Cast listen thread info class
  SynchronizerServerListenThreadInfo* info = 
    (SynchronizerServerListenThreadInfo*) ptr;

  // Exit flag
  bool exitFlag = false;

  // Message buffer
  char buffer[256];

  // Message length
  int messageLength = 0;

  // Loop waiting for all clients to reach barrier or finalize
  while (exitFlag == false) {

    // Lock mutex
    pthread_mutex_lock(info -> _conditionMutex);

    // Check condition and signal if all nodes are waiting on barrier
    while (*(info -> _numberOfClientsWaiting) < *(info -> _numberOfClients)) {
      pthread_cond_wait(info -> _conditionVariable, info -> _conditionMutex);
    }

    // Initialize buffer
    memset(buffer, 0, 256);
    sprintf(buffer, "MSG_CONTINUE");

    // Initialize message length
    messageLength = 0;

    // Send continue message to all clients
    for (int i = 0 ; i < *(info -> _numberOfClients) ; i++) {
      if ((messageLength = 
           send((info -> _clientInfo)[i]._clientSocketFileDescriptor,
                buffer,
                255,
                0)) != 255) {
        perror("SynchronizerServer: Error sending barrier message");
      }
    }

    // Update level in observer
    if (info -> _progressObserver != NULL) {
      info -> _progressObserver -> Execute(*(info -> _level));
    }

    // Reset number of clients waiting at barrier
    *(info -> _numberOfClientsWaiting) = 0;

    // Check finalize flag
    if (*(info -> _finalizeFlag) == true) {
       exitFlag = true;
    }

    // Unlock mutex
    pthread_mutex_unlock(info -> _conditionMutex);

  }

  // Shutdown communication and close each client socket
  for (int i = 0 ; i < *(info -> _numberOfClients) ; i++) {

    // Shutdown communication
    if (shutdown((info -> _clientInfo)[i]._clientSocketFileDescriptor, 
                 SHUT_RDWR) == -1) {
      perror("SynchronizerServer: Error during socket shutdown");
    }

    // Close socket
    if (close((info -> _clientInfo)[i]._clientSocketFileDescriptor) == -1) {
      perror("SynchronizerServer: Error during socket close");
    }

    // Reset socket file descriptor
    (info -> _clientInfo)[i]._clientSocketFileDescriptor = -1;

  }

}

/*--------------------------------------------------------------------------*/

void SynchronizerServer::SetProgressObserver(SynchronizerServerAbstractCommand* 
                                             observer) {

  // Set observer
  _progressObserver = observer;

}

/*--------------------------------------------------------------------------*/
