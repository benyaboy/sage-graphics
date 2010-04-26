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

#ifndef VOLUME_RENDERER_THREAD_INFO_H
#define VOLUME_RENDERER_THREAD_INFO_H

/*--------------------------------------------------------------------------*/

#define VOLUME_RENDERER_THREAD_INFO_MAP_8 8
#define VOLUME_RENDERER_THREAD_INFO_MAP_16 16

/*--------------------------------------------------------------------------*/

#include <pthread.h>

#include "Octree.h"

/*--------------------------------------------------------------------------*/

class VolumeRendererThreadInfo {

public:

  // Default constructor
  VolumeRendererThreadInfo();

  // Default destructor
  ~VolumeRendererThreadInfo();

  // Abort render flag
  bool _abortFlag;

  // Axis on flag
  bool _axisOnFlag;

  // Axis position
  float _axisPosition[3];

  // Brick box on flag
  bool _brickBoxOnFlag;

  // Bounding box on flag
  bool _boundingBoxOnFlag;

  // Condition variable
  pthread_cond_t* _conditionCond;

  // Condition variable mutex
  pthread_mutex_t* _conditionMutex;

  // Data
  Octree* _data;

  // Display size
  int _displaySize[2];

  // Exit flag
  bool _exitFlag;

  // Frustum
  float _frustum[6];

  // Full screen flag
  bool _fullScreenFlag;

  // In render flag
  bool _inRenderFlag;

  // Color and opacity map
  unsigned char* _map;

  // Color and opacity map type
  int _mapType;

  // Normalized coordinates of rendered area
  float _normalCoordinates[4];

  // Number of nodes
  int _numberOfNodes;

  // Rotation matrix
  float _R[16];

  // CacheRAM Size
  int _ramSize;

  // Rank
  int _rank;

  // Render flag
  bool _renderFlag;

  // Scale matrix
  float _S[16];

  // Slice frequency
  double _sliceFrequency;

  // Synchronization server hostname
  char _synchronizationHostname[128];

  // Synchronization server port
  int _synchronizationPort;

  // Translation matrix
  float _T[16];

  // Viewport
  int _viewport[4];

  // Update axis flag
  bool _updateAxis;

  // Update brick box
  bool _updateBrickBox;

  // Update bounding box
  bool _updateBoundingBox;

  // Update data flag
  bool _updateData;

  // Update frustum flag
  bool _updateFrustum;

  // Update map flag
  bool _updateMap;

  // Update rotation matrix flag
  bool _updateR;

  // Update scale matrix flag
  bool _updateS;

  // Update slice frequency flag
  bool _updateSliceFrequency;

  // Update translation matrix flag
  bool _updateT;

  // Update viewport flag
  bool _updateViewport;

  // CacheVRAM Size
  int _vramSize;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
