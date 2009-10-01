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
#include "CommunicatorConcreteCommand.h"
#include "Slave.h"

/*--------------------------------------------------------------------------*/

Slave::Slave(Communicator* communicator) {

  // Initialize axis on flag
  _axisOnFlag = false;

  // Initialize axis position
  _axisPosition[0] = 0.0;
  _axisPosition[1] = 0.0;
  _axisPosition[2] = 0.0;

  // Initialize brick box on flag
  _brickBoxOnFlag = false;

  // Initialize bounding box on flag
  _boundingBoxOnFlag = false;

  // Initialize clipping planes
  _clippingPlanes[0] = 0.0;
  _clippingPlanes[1] = 0.0;

  // Initialize communicator
  _communicator = communicator;

  // Initialize data
  _data = NULL;

  // Initialize data filename
  memset(_dataFilename, 0, 1024);

  // Initialize data prefix
  memset(_dataPrefix, 0, 1024);

  // Initialize display size
  _displaySize[0] = 0;
  _displaySize[1] = 0;

  // Initialize exit flag
  _exitFlag = false;

  // Initialize frustum
  _frustum[0] = 0.0;
  _frustum[1] = 0.0;
  _frustum[2] = 0.0;
  _frustum[3] = 0.0;

  // Initialize full screen flag
  _fullScreenFlag = false;

  // Initialize map
  _map = NULL;

  // Initialize normal coordinates
  _normalCoordinates[0] = 0.0;
  _normalCoordinates[1] = 0.0;
  _normalCoordinates[2] = 0.0;
  _normalCoordinates[3] = 0.0;

  // Initialize and register observer with communicator
  _observer = new CommunicatorConcreteCommand(this);
  _communicator -> SetObserver(_observer);
  if (_observer == NULL) {
    fprintf(stderr, "Slave: Error creating CommunicatorConcreteCommand.\n");
  }

  // Slice frequency
  _sliceFrequency = 1.0;

  // Initialize rotation matrix
  _R[0] = 1.0; _R[4] = 0.0; _R[8] = 0.0; _R[12] = 0.0;
  _R[1] = 0.0; _R[5] = 1.0; _R[9] = 0.0; _R[13] = 0.0;
  _R[2] = 0.0; _R[6] = 0.0; _R[10] = 1.0; _R[14] = 0.0;
  _R[3] = 0.0; _R[7] = 0.0; _R[11] = 0.0; _R[15] = 1.0;

  // Initialize scale matrix
  _S[0] = 1.0; _S[4] = 0.0; _S[8] = 0.0; _S[12] = 0.0;
  _S[1] = 0.0; _S[5] = 1.0; _S[9] = 0.0; _S[13] = 0.0;
  _S[2] = 0.0; _S[6] = 0.0; _S[10] = 1.0; _S[14] = 0.0;
  _S[3] = 0.0; _S[7] = 0.0; _S[11] = 0.0; _S[15] = 1.0;

  // Initialize translation matrix
  _T[0] = 1.0; _T[4] = 0.0; _T[8] = 0.0; _T[12] = 0.0;
  _T[1] = 0.0; _T[5] = 1.0; _T[9] = 0.0; _T[13] = 0.0;
  _T[2] = 0.0; _T[6] = 0.0; _T[10] = 1.0; _T[14] = 0.0;
  _T[3] = 0.0; _T[7] = 0.0; _T[11] = 0.0; _T[15] = 1.0;

  // Initialize viewport
  _viewport[0] = 0;
  _viewport[1] = 0;
  _viewport[2] = 0;
  _viewport[3] = 0;

  // Initialize renderer
  _volumeRendererShell = new VolumeRendererShell();
  if (_volumeRendererShell == NULL) {
    fprintf(stderr, "Slave: Error creating VolumeRendererShell.\n");
  }

}

/*--------------------------------------------------------------------------*/

Slave::~Slave() {

  // Clean up observer
  if (_observer != NULL) {
    delete _observer;
  }

  // Clean up volume renderer shell
  if (_volumeRendererShell != NULL) {
    delete _volumeRendererShell;
  }

}

/*--------------------------------------------------------------------------*/

void Slave::Run() {

  // Event loop
  while (_exitFlag == false) {

    // Process a command from the Communicator
    _communicator -> GetCommand();

    // Sync with other nodes
    _communicator -> Barrier();

  }

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateAxisOff() {

  // Update local variables
  _axisOnFlag = false;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetAxisOff();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateAxisOn() {

  // Update local variables
  _axisOnFlag = true;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetAxisOn();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateAxisPosition(float x, float y, float z) {

  // Update local variables
  _axisPosition[0] = x;
  _axisPosition[1] = y;
  _axisPosition[2] = z;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetAxisPosition(x, y, z);

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateBrickBoxOff() {

  // Update local variables
  _brickBoxOnFlag = false;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetBrickBoxOff();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateBrickBoxOn() {

  // Update local variables
  _brickBoxOnFlag = true;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetBrickBoxOn();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateBoundingBoxOff() {

  // Update local variables
  _boundingBoxOnFlag = false;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetBoundingBoxOff();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateBoundingBoxOn() {

  // Update local variables
  _boundingBoxOnFlag = true;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetBoundingBoxOn();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateClippingPlanes(float near, float far) {

  // Update local variables
  _clippingPlanes[0] = near;
  _clippingPlanes[1] = far;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetFrustum(_frustum[0], _frustum[1], 
                                     _frustum[2], _frustum[3],
                                     _clippingPlanes[0], _clippingPlanes[1]);

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateCommandExit() {

  // Update local variables
  _exitFlag = true;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetExitFlag();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateCommandRender() {

  // Update volume renderer
  _volumeRendererShell -> Render();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateData(char* filename) {

  // Update local variables
  if (filename != NULL) {
    strcpy(_dataFilename, filename);
  }

  // Allocate new octree
  _data = new Octree();
  if (_data == NULL) {
    fprintf(stderr, "Slave: Memory allocation error while loading data.\n");
    return;
  }

  // Load metadata
  _data -> SetPrefix(_dataPrefix);
  _data -> LoadFromFile2(_dataFilename);

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetData(_data);

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateData(char* filename, int ramSize, int vramSize) {

  // Update local variables
  if (filename != NULL) {
    strcpy(_dataFilename, filename);
  }

  // Allocate new octree
  _data = new Octree();
  if (_data == NULL) {
    fprintf(stderr, "Slave: Memory allocation error while loading data.\n");
    return;
  }

  // Load metadata
  _data -> SetPrefix(_dataPrefix);
  _data -> LoadFromFile2(_dataFilename);

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetData(_data, ramSize, vramSize);

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateDisplaySize(int width, int height, bool fullScreen, 
                              char* hostname, int port,
                              float left, float right, 
                              float bottom, float top) {

  // Update local variables
  _displaySize[0] = width;
  _displaySize[1] = height;
  _fullScreenFlag = fullScreen;
  _normalCoordinates[0] = left;
  _normalCoordinates[1] = right;
  _normalCoordinates[2] = bottom;
  _normalCoordinates[3] = top;

  // Update volume renderer
  _volumeRendererShell -> Init(_displaySize[0], 
                               _displaySize[1], 
                               _fullScreenFlag, 
                               hostname, 
                               port,
                               _normalCoordinates[0],
                               _normalCoordinates[1],
                               _normalCoordinates[2],
                               _normalCoordinates[3],
                               _communicator -> GetRank(),
                               _communicator -> GetNumberOfNodes());

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateFrustum(float left, float right, float bottom, float top) {

  // Update local variables
  _frustum[0] = left;
  _frustum[1] = right;
  _frustum[2] = bottom;
  _frustum[3] = top;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetFrustum(_frustum[0], _frustum[1], 
                                     _frustum[2], _frustum[3],
                                     _clippingPlanes[0], _clippingPlanes[1]);

}

/*--------------------------------------------------------------------------*/
  
void Slave::UpdateMap(unsigned char* map) {

  // Update local variables
  _map = map;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetMap(_map);

}

/*--------------------------------------------------------------------------*/
  
void Slave::UpdateMap16(unsigned char* map) {

  // Update local variables
  _map = map;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetMap16(_map);

}

/*--------------------------------------------------------------------------*/

void Slave::UpdatePrefix(char* prefix) {

  // Update local variables
  if (prefix != NULL) {
    strcpy(_dataPrefix, prefix);
  }

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateRotationMatrix(float m[16]) {

  // Update local variables
  _R[0] = m[0]; _R[4] = m[4]; _R[8] = m[8]; _R[12] = m[12];
  _R[1] = m[1]; _R[5] = m[5]; _R[9] = m[9]; _R[13] = m[13];
  _R[2] = m[2]; _R[6] = m[6]; _R[10] = m[10]; _R[14] = m[14];
  _R[3] = m[3]; _R[7] = m[7]; _R[11] = m[11]; _R[15] = m[15];

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetR(_R);
  _volumeRendererShell -> Render();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateScaleMatrix(float m[16]) {

  // Update local variables
  _S[0] = m[0]; _S[4] = m[4]; _S[8] = m[8]; _S[12] = m[12];
  _S[1] = m[1]; _S[5] = m[5]; _S[9] = m[9]; _S[13] = m[13];
  _S[2] = m[2]; _S[6] = m[6]; _S[10] = m[10]; _S[14] = m[14];
  _S[3] = m[3]; _S[7] = m[7]; _S[11] = m[11]; _S[15] = m[15];

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetS(_S);
  _volumeRendererShell -> Render();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateSliceFrequency(double frequency) {

  // Update local variable
  _sliceFrequency = frequency;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetSliceFrequency(frequency);
  _volumeRendererShell -> Render();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateTranslationMatrix(float m[16]) {

  // Update local variables
  _T[0] = m[0]; _T[4] = m[4]; _T[8] = m[8]; _T[12] = m[12];
  _T[1] = m[1]; _T[5] = m[5]; _T[9] = m[9]; _T[13] = m[13];
  _T[2] = m[2]; _T[6] = m[6]; _T[10] = m[10]; _T[14] = m[14];
  _T[3] = m[3]; _T[7] = m[7]; _T[11] = m[11]; _T[15] = m[15];

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetT(_T);
  _volumeRendererShell -> Render();

}

/*--------------------------------------------------------------------------*/

void Slave::UpdateViewport(int x, int y, int w, int h) {

  // Update local variables
  _viewport[0] = x;
  _viewport[1] = y;
  _viewport[2] = w;
  _viewport[3] = h;

  // Update volume renderer
  _volumeRendererShell -> AbortRender(true);
  _volumeRendererShell -> SetViewport(_viewport[0], _viewport[1], 
                                      _viewport[2], _viewport[3]);

}

/*--------------------------------------------------------------------------*/
