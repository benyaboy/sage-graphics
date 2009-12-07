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

#ifndef SLAVE_H
#define SLAVE_H

/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "Communicator.h"
#include "CommunicatorAbstractCommand.h"
#include "Octree.h"
#include "VolumeRendererShell.h"

/*--------------------------------------------------------------------------*/

class Slave {

public:

  // Constructor
  Slave(Communicator* communicator);

  // Destructor
  ~Slave();

  // Main loop
  void Run();

  // Axis off callback
  void UpdateAxisOff();

  // Axis on callback
  void UpdateAxisOn();

  // Axis position callback
  void UpdateAxisPosition(float x, float y, float z);

  // Brick box off callback
  void UpdateBrickBoxOff();

  // Brick box on callback
  void UpdateBrickBoxOn();

  // Bounding box off callback
  void UpdateBoundingBoxOff();

  // Bounding box on callback
  void UpdateBoundingBoxOn();

  // Clipping planes callback
  void UpdateClippingPlanes(float near, float far);

  // Exit callback
  void UpdateCommandExit();

  // Render callback
  void UpdateCommandRender();

  // Data callback
  void UpdateData(char* filename);

  // Data callback
  void UpdateData(char* filename, int ramSize, int vramSize);

  // Display size callback
  void UpdateDisplaySize(int width, int height, bool fullScreen,
                         char* hostname, int port,
                         float left, float right, 
                         float bottom, float top);

  // Frustum callback
  void UpdateFrustum(float left, float right, float bottom, float top);
  
  // 8-bit map callback
  void UpdateMap(unsigned char* map);

  // 16-bit map callback
  void UpdateMap16(unsigned char* map);

  // Prefix callback
  void UpdatePrefix(char* prefix);

  // Rotation matrix callback
  void UpdateRotationMatrix(float m[16]);

  // Scale matrix callback
  void UpdateScaleMatrix(float m[16]);

  // Slice frequency callback
  void UpdateSliceFrequency(double frequency);

  // Translation matrix callback
  void UpdateTranslationMatrix(float m[16]);

  // Viewport callback
  void UpdateViewport(int x, int y, int w, int h);

private:

  // Axis on flag
  bool _axisOnFlag;

  // Brick box on flag
  bool _brickBoxOnFlag;

  // Bounding box on flag
  bool _boundingBoxOnFlag;

  // Axis position
  float _axisPosition[3];

  // Clipping planes
  float _clippingPlanes[2];

  // Communicator
  Communicator* _communicator;

  // Data
  Octree* _data;

  // Data meta filename
  char _dataFilename[1024];

  // Data meta prefix
  char _dataPrefix[1024];

  // Display size
  int _displaySize[2];

  // Exit flag
  bool _exitFlag;

  // Frustum
  float _frustum[4];

  // Full screen flag
  bool _fullScreenFlag;

  // Map
  unsigned char* _map;

  // Normalized coordinates of rendered area
  float _normalCoordinates[4];

  // Observer
  CommunicatorAbstractCommand* _observer;

  // Slice frequency
  double _sliceFrequency;

  // Transformation matrices
  float _R[16];
  float _S[16];
  float _T[16];

  // Viewport
  int _viewport[4];

  // Volume renderer shell
  VolumeRendererShell* _volumeRendererShell;

};

/*--------------------------------------------------------------------------*/

#endif 

/*--------------------------------------------------------------------------*/
