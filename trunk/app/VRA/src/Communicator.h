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

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

/*--------------------------------------------------------------------------*/

#include <mpi.h>
#include <string.h>

#include "CommunicatorAbstractCommand.h"

/*--------------------------------------------------------------------------*/

class Communicator {

public:

  // Constructor
  Communicator();

  // Destructor
  ~Communicator();

  // Barrier
  void Barrier();

  // Destroy communication
  void Destroy();

  // Get command
  void GetCommand();

  // Get the master node's name
  void GetMasterNodeName(char* name);

  // Get the current node's name
  void GetNodeName(char* name);

  // Get total number of nodes
  int GetNumberOfNodes();

  // Get rank of current node
  int GetRank();

  // Initialize
  void Init(int argc, char** argv);

  // Turn axis off
  void SendAxisOff();

  // Turn axis on
  void SendAxisOn();

  // Turn brick box off
  void SendBrickBoxOff();

  // Turn brick box on
  void SendBrickBoxOn();

  // Turn bounding box off
  void SendBoundingBoxOff();

  // Turn bounding box on
  void SendBoundingBoxOn();

  // Send axis poistion
  void SendAxisPosition(float x, float y, float z);

  // Send clipping planes to specified node
  void SendClippingPlanes(int node, float near, float far);

  // Send exit command to all nodes
  void SendCommandExit();

  // Send render command to all nodes
  void SendCommandRender();

  // Send meta data filename to all nodes to load new data
  void SendData(char* filename);

  // Send meta data filename and cache sizes to all nodes to load new data
  void SendData(char* filename, int ramSize, int vramSize);

  // Send display size information to specified node
  void SendDisplaySize(int node, 
                       int width, 
                       int height, 
                       bool fullScreen, 
                       char* hostname, 
                       int port,
                       float left,
                       float right,
                       float bottom,
                       float top);

  // Send frustum to specified node
  void SendFrustum(int node, float left, float right,
                   float bottom, float top);

  // Send 8-bit color and opacity transfer function to all nodes
  void SendMap(unsigned char* map);

  // Send 8-bit color and opacity transfer function to all nodes
  void SendMap8(unsigned char* map);

  // Send 16-bit color and opacity transfer function to all nodes
  void SendMap16(unsigned char* map);

  // Send data prefix to all nodes
  void SendPrefix(char* prefix);

  // Send rotation matrix to all nodes
  void SendRotationMatrix(float matrix[16]);

  // Send scale matrix to all nodes
  void SendScaleMatrix(float matrix[16]);

  // Send slice frequency
  void SendSliceFrequency(double frequency);

  // Send translation matrix to all nodes
  void SendTranslationMatrix(float matrix[16]);

  // Send viewport to specified node
  void SendViewport(int node, int x, int y, int w, int h);

  // Set observer
  void SetObserver(CommunicatorAbstractCommand* observer);

private:
  
  // Master node name
  char _masterNodeName[MPI_MAX_PROCESSOR_NAME];

  // Node name
  char _nodeName[MPI_MAX_PROCESSOR_NAME];

  // Total number of nodes
  int _numberOfNodes;

  // Observer
  CommunicatorAbstractCommand* _observer;

  // Rank of current node
  int _rank;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
