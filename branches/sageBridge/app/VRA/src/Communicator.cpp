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

#include "Communicator.h"
#include <stdio.h>

/*--------------------------------------------------------------------------*/

Communicator::Communicator() {
  
  // Initialize number of nodes to 0
  _numberOfNodes = 0;

  // Initialize observer to NULL
  _observer = NULL;

  // Initialize rank to 0
  _rank = 0;

}

/*--------------------------------------------------------------------------*/

Communicator::~Communicator() {
}

/*--------------------------------------------------------------------------*/

void Communicator::Barrier() {

  // MPI Barrier
  MPI_Barrier(MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::Destroy() {

  // Finalize MPI
  MPI_Finalize();

}

/*--------------------------------------------------------------------------*/

void Communicator::GetCommand() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);

  // Destination
  int destination = 0;


  // Get buffer
  MPI_Bcast(buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

  // Unpack command from buffer
  MPI_Unpack(buffer, 2048, &position, command,
             8, MPI_CHAR, MPI_COMM_WORLD);

  // Unpack destination from buffer
  MPI_Unpack(buffer, 2048, &position, &destination,
             1, MPI_INT, MPI_COMM_WORLD);


  // axis off
  if (!strcmp(command, "CMD_AOF")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetAxisOff();
    }

  }


  // axis on
  else if (!strcmp(command, "CMD_AON")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetAxisOn();
    }

  }


  // brick box off
  else if (!strcmp(command, "CMD_BBF")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBrickBoxOff();
    }

  }


  // brick box on
  else if (!strcmp(command, "CMD_BBN")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBrickBoxOn();
    }

  }


  // bounding box off
  else if (!strcmp(command, "CMD_BOF")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBoundingBoxOff();
    }

  }


  // bounding box on
  else if (!strcmp(command, "CMD_BON")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetBoundingBoxOn();
    }

  }


  // axis position
  else if (!strcmp(command, "CMD_APO")) {

    // Unpack data from buffer
    float data[3];
    MPI_Unpack(buffer, 2048, &position, data, 3, MPI_FLOAT, MPI_COMM_WORLD);
 
    // Observer
    if (_observer != NULL) {
      _observer -> SetAxisPosition(data[0], data[1], data[2]);
    }

  }


  // clipping planes
  else if (!strcmp(command, "CMD_CLI")) {

    // Unpack data from buffer
    float data[2];
    MPI_Unpack(buffer, 2048, &position, data, 2, MPI_FLOAT, MPI_COMM_WORLD);
 
    // Observer
    if ((_observer != NULL) && (destination == GetRank())) {
      _observer -> SetClippingPlanes(data[0], data[1]);
    }

  }


  // exit
  else if(!strcmp(command, "CMD_EXI")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetCommandExit();
    }

  }


  // render
  else if(!strcmp(command, "CMD_REN")) {

    // Observer
    if (_observer != NULL) {
      _observer -> SetCommandRender();
    }

  }


  // data
  else if(!strcmp(command, "CMD_DAT")) {

    // Unpack data from buffer
    char data[1024];
    int ramSize = 0;
    int vramSize = 0;
    MPI_Unpack(buffer, 2048, &position, data, 1024, MPI_CHAR, MPI_COMM_WORLD);
    MPI_Unpack(buffer, 2048, &position, &ramSize, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, 2048, &position, &vramSize, 1, MPI_INT, MPI_COMM_WORLD);

    // Observer 
    if (_observer !=NULL) {
      if (ramSize == -1 || vramSize == -1) {
        _observer -> SetData(data);
      }
      else {
        _observer -> SetData(data, ramSize, vramSize);
      }
    }

  }


  // display size
  else if(!strcmp(command, "CMD_DIS")) {

    // Unpack data from buffer
    int data[3];
    char host[128];
    int port;
    float normalCoordinates[4];
    MPI_Unpack(buffer, 2048, &position, data, 3, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, 2048, &position, host, 128, MPI_CHAR, MPI_COMM_WORLD);
    MPI_Unpack(buffer, 2048, &position, &port, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, 2048, &position, normalCoordinates, 4, 
               MPI_FLOAT, MPI_COMM_WORLD);

    // Full screen flag
    bool fullScreen = false;
    if (data[2] == 1) {
      fullScreen = true;
    }

    // Observer
    if ((_observer != NULL) && (destination == GetRank())) {
      _observer -> SetDisplaySize(data[0], data[1], fullScreen, host, port,
                                  normalCoordinates[0], normalCoordinates[1],
                                  normalCoordinates[2], normalCoordinates[3]);
    }

  }


  // frustum
  else if(!strcmp(command, "CMD_FRU")) {

    // Unpack data from buffer
    float data[4];
    MPI_Unpack(buffer, 2048, &position, data, 4, MPI_FLOAT, MPI_COMM_WORLD);

    // Observer
    if ((_observer != NULL) && (destination == GetRank())) {
      _observer -> SetFrustum(data[0], data[1], data[2], data[3]);
    }

  }


  // 8-bit map
  else if(!strcmp(command, "CMD_MAP")) {
    
    // Allocate space for data
    unsigned char* data = new unsigned char[1024];

    // Unpack data from buffer
    MPI_Unpack(buffer, 2048, &position, data, 1024, MPI_BYTE, MPI_COMM_WORLD);

    // Observer
    if (_observer != NULL) {
      _observer -> SetMap(data);
    }

  }


  // 16-bit map
  else if(!strcmp(command, "CMD_M16")) {
    
    // Payload position
    int payloadPosition = 0;

    // Payload
    unsigned char payload[65536*4];

    // Allocate space for data
    unsigned char* data = new unsigned char[65536*4];

    // MPI Barrier
    MPI_Barrier(MPI_COMM_WORLD);    

    // Get buffer
    MPI_Bcast(payload, 65536*4, MPI_BYTE, 0, MPI_COMM_WORLD);

    // Unpack data from buffer
    MPI_Unpack(payload, 65536*4, &payloadPosition, data, 65536*4, 
               MPI_BYTE, MPI_COMM_WORLD);

    // Observer
    if (_observer != NULL) {
      _observer -> SetMap16(data);
    }

  }


  // prefix
  else if(!strcmp(command, "CMD_PRE")) {

    // Unpack data from buffer
    char prefix[1024];
    MPI_Unpack(buffer, 2048, &position, prefix, 1024, 
               MPI_CHAR, MPI_COMM_WORLD);

    // Observer 
    if (_observer != NULL) {
      _observer -> SetPrefix(prefix);
    }

  }


  // rotation matrix
  else if(!strcmp(command, "CMD_ROT")) {

    // Unpack data from buffer
    float data[16];
    MPI_Unpack(buffer, 2048, &position, data, 16, MPI_FLOAT, MPI_COMM_WORLD);

    // Observer
    if (_observer != NULL) {
      _observer -> SetRotationMatrix(data);
    }

  }


  // scale matrix
  else if(!strcmp(command, "CMD_SCA")) {

    // Unpack data from buffer
    float data[16];
    MPI_Unpack(buffer, 2048, &position, data, 16, MPI_FLOAT, MPI_COMM_WORLD);

    // Observer
    if (_observer != NULL) {
      _observer -> SetScaleMatrix(data);
    }

  }


  // slice frequency
  else if(!strcmp(command, "CMD_SLF")) {

    // Unpack data from buffer
    float data;
    MPI_Unpack(buffer, 2048, &position, &data, 1, MPI_FLOAT, MPI_COMM_WORLD);

    // Observer
    if (_observer != NULL) {
      _observer -> SetSliceFrequency((double) data);
    }

  }


  // translation matrix
  else if(!strcmp(command, "CMD_TRA")) {

    // Unpack data from buffer
    float data[16];
    MPI_Unpack(buffer, 2048, &position, data, 16, MPI_FLOAT, MPI_COMM_WORLD);

    // Observer
    if (_observer != NULL) {
      _observer -> SetTranslationMatrix(data);
    }

  }


  // viewport
  else if(!strcmp(command, "CMD_VIE")) {

    // Unpack data from buffer
    int data[4];
    MPI_Unpack(buffer, 2048, &position, data, 4, MPI_INT, MPI_COMM_WORLD);

    // Observer
    if ((_observer != NULL) && (destination == GetRank())) {
      _observer -> SetViewport(data[0], data[1], data[2], data[3]);
    }

  }


  // undefined command
  else {
  }

}

/*--------------------------------------------------------------------------*/

void Communicator::GetMasterNodeName(char* name) {
  
  // Check for NULL
  if (name != NULL) {

    // Copy name
    strcpy(name, _masterNodeName);

  }

}

/*--------------------------------------------------------------------------*/

void Communicator::GetNodeName(char* name) {

  // Check for NULL
  if (name != NULL) {

    // Copy name
    strcpy(name, _nodeName);

  }

}

/*--------------------------------------------------------------------------*/

int Communicator::GetNumberOfNodes() {

  // Get number of nodes
  return _numberOfNodes;

}

/*--------------------------------------------------------------------------*/

int Communicator::GetRank() {

  // Get rank
  return _rank;

}

/*--------------------------------------------------------------------------*/

void Communicator::Init(int argc, char** argv) {

  // Length of node name
  int length = 0;

  // Initialize MPI and get the number of nodes, current rank and node name
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {

    // Get number of nodes, current rank and node name
    MPI_Comm_size(MPI_COMM_WORLD, &_numberOfNodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
    MPI_Get_processor_name(_nodeName, &length);

    // Synchronize nodes
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Send name of master node to all nodes
    if (_rank == 0) {
      strcpy(_masterNodeName, _nodeName);
      MPI_Bcast(&_nodeName, MPI_MAX_PROCESSOR_NAME, 
                MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    else {
      MPI_Bcast(&_masterNodeName, MPI_MAX_PROCESSOR_NAME,
                MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // Synchronize nodes
    MPI_Barrier(MPI_COMM_WORLD);

  }

}

/*--------------------------------------------------------------------------*/

void Communicator::SendAxisOff() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_AOF");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendAxisOn() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_AON");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendAxisPosition(float x, float y, float z) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_APO");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(&x, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&y, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&z, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendBrickBoxOff() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BBF");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendBrickBoxOn() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BBN");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendBoundingBoxOff() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BOF");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendBoundingBoxOn() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BON");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendClippingPlanes(int node, float near, float far) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_CLI");

  // Destination
  int destination = node;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(&near, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&far, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendCommandExit() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_EXI");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendCommandRender() {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_REN");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendData(char* filename) {

  // Position in buffer
  int position = 0;

  // Default cache sizes: -1 tells the slave communicator processes to call
  // versions of SetData/UpdateData that don't specify cache sizes. This
  // is something to sort out in the future.
  int ramSize = -1;
  int vramSize = -1;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DAT");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(filename, 1024, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&ramSize, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&vramSize, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendData(char* filename, int ramSize, int vramSize) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DAT");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(filename, 1024, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&ramSize, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&vramSize, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendDisplaySize(int node, 
                                   int width, 
                                   int height, 
                                   bool fullScreen, 
                                   char* hostname, 
                                   int port,
                                   float left,
                                   float right,
                                   float bottom,
                                   float top) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DIS");

  // Destination
  int destination = node;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Convert full screen flag from bool to integer
  int flag = 0;
  if (fullScreen == true) {
    flag = 1;
  }

  // Pack data
  MPI_Pack(&width, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&height, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&flag, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(hostname, 128, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&port, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&left, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&right, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&bottom, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&top, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendFrustum(int node, float left, float right,
                               float bottom, float top) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_FRU");

  // Destination
  int destination = node;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(&left, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&right, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&bottom, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&top, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendMap(unsigned char* map) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_MAP");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(map, 1024, MPI_BYTE, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendMap8(unsigned char* map) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_MAP");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(map, 1024, MPI_BYTE, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendMap16(unsigned char* map) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_M16");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

  // MPI Barrier
  MPI_Barrier(MPI_COMM_WORLD);

  // Position in payload
  position = 0;

  // Payload
  unsigned char payload[65536*4];
  memset(payload, 0, 65536*4);

  // Pack data
  MPI_Pack(map, 65536*4, MPI_BYTE, payload, 65536*4, 
           &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&payload, 65536*4, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendPrefix(char* prefix) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_PRE");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(prefix, 1024, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendRotationMatrix(float matrix[16]) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_ROT");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(matrix, 16, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendScaleMatrix(float matrix[16]) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_SCA");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(matrix, 16, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendSliceFrequency(double frequency) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_SLF");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  float data = (float) frequency;
  MPI_Pack(&data, 1, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendTranslationMatrix(float matrix[16]) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_TRA");

  // Destination
  int destination = 0;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(matrix, 16, MPI_FLOAT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SendViewport(int node, int x, int y, int w, int h) {

  // Position in buffer
  int position = 0;

  // Buffer
  unsigned char buffer[2048];
  memset(buffer, 0, 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_VIE");

  // Destination
  int destination = node;

  // Pack command
  MPI_Pack(command, 8, MPI_CHAR, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack destination
  MPI_Pack(&destination, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Pack data
  MPI_Pack(&x, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&y, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&w, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);
  MPI_Pack(&h, 1, MPI_INT, buffer, 2048, &position, MPI_COMM_WORLD);

  // Send command
  MPI_Bcast(&buffer, 2048, MPI_BYTE, 0, MPI_COMM_WORLD);

}

/*--------------------------------------------------------------------------*/

void Communicator::SetObserver(CommunicatorAbstractCommand* observer) {

  // Set observer
  _observer = observer;

}

/*--------------------------------------------------------------------------*/
