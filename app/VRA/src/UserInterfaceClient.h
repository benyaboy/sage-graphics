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

#ifndef USER_INTERFACE_CLIENT_H
#define USER_INTERFACE_CLIENT_H

/*--------------------------------------------------------------------------*/

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DataPacker.h"
#include "UserInterfaceClientAbstractCommand.h"

/*--------------------------------------------------------------------------*/

#define USER_INTERFACE_CLIENT_ERROR -1
#define USER_INTERFACE_CLIENT_OK 0

/*--------------------------------------------------------------------------*/

class UserInterfaceClient {

public:

  // Default constructor
  UserInterfaceClient();

  // Default destructor
  ~UserInterfaceClient();

  // Disconnect from server
  void Disconnect();

  // Get axis state
  void GetAxis();

  // Get axis position
  void GetAxisPosition();

  // Get bounding box state
  void GetBoundingBox();

  // Get brick box state
  void GetBrickBox();

  // Get clipping planes
  void GetClippingPlanes(int node);

  // Get meta data filename
  void GetData();

  // Get dataset list file
  void GetDatasetListFile(int index);

  // Get dataset list name
  void GetDatasetListName(int index);

  // Get dataset list prefix
  void GetDatasetListPrefix(int index);

  // Get dataset list size
  void GetDatasetListSize();

  // Get dataset type
  void GetDatasetType();

  // Get display size
  void GetDisplaySize(int node);

  // Get frustum
  void GetFrustum(int node);

  // Get map
  void GetMap();

  // Get number of slaves
  void GetNumberOfSlaves();

  // Get overview vertices
  void GetOverviewVertices();

  // Get data prefix
  void GetPrefix();

  // Get rotation matrix
  void GetRotationMatrix();

  // Get scale matrix
  void GetScaleMatrix();

  // Get slice frequency
  void GetSliceFrequency();

  // Get total display dimensions
  void GetTotalDisplayDimensions();

  // Get total display frustum
  void GetTotalDisplayFrustum();

  // Get translation matrix
  void GetTranslationMatrix();

  // Get vertical field of view
  void GetVerticalFieldOfView();

  // Get viewport
  void GetViewport(int node);

  // Initialize connection to server
  int Init(char* hostname, int port);

  // Reset view
  void ResetView();

  // Send axis off
  void SendAxisOff();

  // Send axis on
  void SendAxisOn();

  // Send axis position
  void SendAxisPosition(float x, float y, float z);

  // Send bounding box off
  void SendBoundingBoxOff();

  // Send bounding box on
  void SendBoundingBoxOn();

  // Send brick box off
  void SendBrickBoxOff();

  // Send brick box on
  void SendBrickBoxOn();

  // Send clipping planes
  void SendClippingPlanes(int node, float near, float far);

  // Send command exit
  void SendCommandExit();

  // Send command render
  void SendCommandRender();

  // Send meta data filename
  void SendData(char* filename);

  // Send meta data filename and cache sizes
  void SendData(char* filename, int ramSize, int vramSize);

  // Send frustum
  void SendFrustum(int node, float left, float right, 
                   float bottom, float top);

  // Send map
  void SendMap(unsigned char* map);

  // Send 16-bit map
  void SendMap16(unsigned char* map);

  // Send data prefix
  void SendPrefix(char* prefix);

  // Send rotation matrix
  void SendRotationMatrix(float matrix[16]);

  // Send scale matrix
  void SendScaleMatrix(float matrix[16]);

  // Send slice frequency
  void SendSliceFrequency(double frequency);

  // Send translation matrix
  void SendTranslationMatrix(float matrix[16]);

  // Send viewport
  void SendViewport(int node, int x, int y, int w, int h);

  // Set observer
  void SetObserver(UserInterfaceClientAbstractCommand* observer);

  // Thread to listen for updates from server
  static void *ThreadFunction(void* ptr);

private:

  // Exit flag
  bool _exitFlag;

  // Server hostname
  char _hostname[1024];

  // Is initialized flag
  bool _isInitialized;

  // Observer
  UserInterfaceClientAbstractCommand* _observer;

  // Server port number
  int _port;

  // Server
  struct hostent* _server;

  // Server address
  struct sockaddr_in _serverAddress;

  // Socket file descriptor
  int _serverSocketFileDescriptor;

  // Thread listening for server updates
  pthread_t _thread;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
