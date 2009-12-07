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

#include "VolumeRendererThreadInfo.h"

/*--------------------------------------------------------------------------*/

VolumeRendererThreadInfo::VolumeRendererThreadInfo() {

  // Initialize abort render flag
  _abortFlag = false;

  // Initialize axis on flag
  _axisOnFlag = false;

  // Initialize axis position
  _axisPosition[0] = 0.0;
  _axisPosition[1] = 0.0;
  _axisPosition[2] = 0.0;

  // Initialize brick box flag
  _brickBoxOnFlag = false;

  // Initialize bounding box flag
  _boundingBoxOnFlag = false;

  // Initialize condition variable to NULL
  _conditionCond = NULL;

  // Initialize mutex to NULL
  _conditionMutex = NULL;

  // Initialize data to NULL
  _data = NULL;

  // Initialize display size
  _displaySize[0] = 0;
  _displaySize[1] = 1;

  // Initialize exit flag
  _exitFlag = false;

  // Initialize frustum
  _frustum[0] = 0.0;
  _frustum[1] = 0.0;
  _frustum[2] = 0.0;
  _frustum[3] = 0.0;
  _frustum[4] = 0.0;
  _frustum[5] = 0.0;

  // Initialize full screen flag
  _fullScreenFlag = false;

  // Initialize render in progress flag
  _inRenderFlag = false;

  // Initialize map to NULL
  _map = NULL;

  // Initialize map type
  _mapType = VOLUME_RENDERER_THREAD_INFO_MAP_8;

  // Initialize normal coordinates
  _normalCoordinates[0] = 0.0;
  _normalCoordinates[1] = 0.0;
  _normalCoordinates[2] = 0.0;
  _normalCoordinates[3] = 0.0;

  // Initialize number of nodes
  _numberOfNodes = 0;

  // Initialize rotation matrix to identity
  _R[0] = 1.0; _R[4] = 0.0; _R[8] = 0.0; _R[12] = 0.0;
  _R[1] = 0.0; _R[5] = 1.0; _R[9] = 0.0; _R[13] = 0.0;
  _R[2] = 0.0; _R[6] = 0.0; _R[10] = 1.0; _R[14] = 0.0;
  _R[3] = 0.0; _R[7] = 0.0; _R[11] = 0.0; _R[15] = 1.0;

  // Initialize CacheRAM Size
  _ramSize = 50;

  // Initialize rank
  _rank = 0;

  // Initialize render flag to false
  _renderFlag = false;

  // Initialize scale matrix to identity
  _S[0] = 1.0; _S[4] = 0.0; _S[8] = 0.0; _S[12] = 0.0;
  _S[1] = 0.0; _S[5] = 1.0; _S[9] = 0.0; _S[13] = 0.0;
  _S[2] = 0.0; _S[6] = 0.0; _S[10] = 1.0; _S[14] = 0.0;
  _S[3] = 0.0; _S[7] = 0.0; _S[11] = 0.0; _S[15] = 1.0;

  // Initialize slice frequency
  _sliceFrequency = 1.0;

  // Initialize synchronization server hostname
  memset(_synchronizationHostname, 0, 128);

  // Initialize synchronization server port
  _synchronizationPort = 0;

  // Initialize translation matrix to identity
  _T[0] = 1.0; _T[4] = 0.0; _T[8] = 0.0; _T[12] = 0.0;
  _T[1] = 0.0; _T[5] = 1.0; _T[9] = 0.0; _T[13] = 0.0;
  _T[2] = 0.0; _T[6] = 0.0; _T[10] = 1.0; _T[14] = 0.0;
  _T[3] = 0.0; _T[7] = 0.0; _T[11] = 0.0; _T[15] = 1.0;

  // Initialize all update property flags to false
  _updateAxis = false;
  _updateBrickBox = false;
  _updateBoundingBox = false;
  _updateData = false;
  _updateFrustum = false;
  _updateMap = false;
  _updateR = false;
  _updateS = false;
  _updateSliceFrequency = false;
  _updateT = false;
  _updateViewport = false;

  // Initialize viewport
  _viewport[0] = 0;
  _viewport[1] = 0;
  _viewport[2] = 0;
  _viewport[3] = 0;

  // Initialize CacheVRAM Size
  _vramSize = 10;

}

/*--------------------------------------------------------------------------*/

VolumeRendererThreadInfo::~VolumeRendererThreadInfo() {

}

/*--------------------------------------------------------------------------*/
