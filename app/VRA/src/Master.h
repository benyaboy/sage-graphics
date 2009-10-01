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

#ifndef MASTER_H
#define MASTER_H

/*--------------------------------------------------------------------------*/

#ifdef SAGE
#include <sail.h>
#include <misc.h>
#endif

#include <stdio.h>

#include "Communicator.h"
#include "FrustumCalculator.h"
#include "Matrix.h"
#include "Octree.h"
#include "OctreeNode.h"
#include "SynchronizerServer.h"
#include "SynchronizerServerAbstractCommand.h"
#include "Trackball.h"
#include "UserInterfaceServer.h"
#include "UserInterfaceServerAbstractCommand.h"
#include "Vertex.h"

/*--------------------------------------------------------------------------*/

class Master {

public:

  // Constructor
  Master(Communicator* communicator);

  // Destructor
  ~Master();

  // Get axis on callback
  void GetAxis(bool* state);

  // Get axis position callback
  void GetAxisPosition(float* x, float* y, float* z);

  // Get brick box callback
  void GetBrickBox(bool* state);

  // Get bounding box callback
  void GetBoundingBox(bool* state);

  // Get clipping planes callback
  void GetClippingPlanes(int node, float* near, float* far);

  // Get data callback
  void GetData(char* filename);

  // Get data callback
  void GetData(char* filename, int* ramSize, int* vramSize);

  // Get dataset list callback
  void GetDatasetListFile(int index, char* file);

  // Get dataset list callback
  void GetDatasetListName(int index, char* name);

  // Get dataset list callback
  void GetDatasetListPrefix(int index, char* prefix);

  // Get dataset list size
  void GetDatasetListSize(int* size);

  // Get dataset type
  void GetDatasetType(int* type);

  // Get display size callback
  void GetDisplaySize(int node, int* width, int* height, 
                      bool* fullScreen,
                      float* left, float* right, 
                      float* bottom, float* top);

  // Get frustum callback
  void GetFrustum(int node, float* left, float* right,
                  float* bottom, float* top);

  // Get map callback
  void GetMap(unsigned char* map);

  // Get overview vertices
  void GetOverviewVertices(float vertices[24]);

  // Get prefix callback
  void GetPrefix(char* prefix);

  // Get rotation matrix callback
  void GetRotationMatrix(float m[16]);

  // Get scale matrix callback
  void GetScaleMatrix(float m[16]);

  // Get slice frequency
  void GetSliceFrequency(double* frequency);

  // Get total display dimensions
  void GetTotalDisplayDimensions(int* w, int* h);

  // Get total display frustum
  void GetTotalDisplayFrustum(float frustum[6]);

  // Get translation matrix callback
  void GetTranslationMatrix(float m[16]);

  // Get vertical field of view callback
  void GetVerticalFieldOfView(float* vFOV);

  // Get viewport callback
  void GetViewport(int node, int* x, int* y, int* w, int* h);

  // Initialize
  void Init(char* filename);

  // Reset view
  void ResetView();

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
  void UpdateClippingPlanes(int node, float near, float far);

  // Exit callback
  void UpdateCommandExit();

  // Render callback
  void UpdateCommandRender();

  // Data callback
  void UpdateData(char* filename);

  // Data callback
  void UpdateData(char* filename, int ramSize, int vramSize);

  // Display size callback
  void UpdateDisplaySize(int node, int width, int height, 
                         bool fullScreen, char* hostname, int port,
                         float left, float right, 
                         float bottom, float top);

  // Frustum callback
  void UpdateFrustum(int node, float left, float right, 
                     float bottom, float top);
  
  // 8-bit map callback
  void UpdateMap(unsigned char* map);

  // 16-bit map callback
  void UpdateMap16(unsigned char* map);

  // Prefix callback
  void UpdatePrefix(char* prefix);

  // Progress callback
  void UpdateProgress(int value);

  // Rotation matrix callback
  void UpdateRotationMatrix(float m[16]);

  // Scale matrix callback
  void UpdateScaleMatrix(float m[16]);

  // Slice frequency callback
  void UpdateSliceFrequency(double frequency);

  // Translation matrix callback
  void UpdateTranslationMatrix(float m[16]);

  // Viewport callback
  void UpdateViewport(int node, int x, int y, int w, int h);

private:

  // Axis flag
  bool _axisFlag;

  // Axis position
  float _axisPosition[3];

  // Box around bricks flag
  bool _brickBoxFlag;

  // Bounding box around entire dataset flag
  bool _boundingBoxFlag;

  // Clipping planes
  float** _clippingPlanes;

  // Communicator
  Communicator* _communicator;

  // Data meta filename
  char _dataFilename[1024];

  // Data prefix
  char _dataPrefix[1024];

  // Dataset list dataset file name
  char** _datasetListFile;

  // Dataset list dataset name
  char** _datasetListName;

  // Dataset list dataset prefix
  char** _datasetListPrefix;

  // Data type
  int _dataType;

  // Display percentages
  float** _displayPercentages;

  // Display size
  int** _displaySize;

  // Exit flag
  bool _exitFlag;

  // Frustum
  float** _frustum;

  // Full screen flag
  bool* _fullScreenFlag;

  // Map
  unsigned char* _map;

  // Number of datasets in dataset list
  int _numberOfDatasets;

  // Number of levels in current dataset
  int _numberOfLevels;

  // Number of slaves
  int _numberOfSlaves;

  // Observer
  UserInterfaceServerAbstractCommand* _observer;

  // Progress observer
  SynchronizerServerAbstractCommand* _progressObserver;

  // Slice frequency
  double _sliceFrequency;

  // Transformation matrices
  float _R[16];
  float _S[16];
  float _T[16];

  // CacheRAM size
  int _ramSize;

  // CacheVRAM size
  int _vramSize;

  // Total display dimensions
  int _totalDisplayWidth;
  int _totalDisplayHeight;

  // Total display frustum
  float _totalDisplayFrustum[6];

  // Synchronization server
  SynchronizerServer _synchronizationServer;

  // User interface server
  UserInterfaceServer _userInterfaceServer;

  // Overview vertices
  Vertex _vertex[8];

  // Vertical field of view
  float _vFOV;

  // Viewport
  int** _viewport;

#ifdef SAGE

  // Positon of the rotate device in normalized window coordinates
  float _rotatePosition[3];

  // Position of scale device in normalized window coordinates
  float _scalePosition[3];

  // Position of scale device with respect to the untransfomred object
  // in world space
  float _scalePositionUntransformed[3];

  // Trackball for SAGE rotation event
  Trackball _eventTrackball;

  // Position of translate device in normalized window coordinates
  float _translatePosition[3];

  // SAIL configuration
  sailConfig _sageConfig;

  // SAIL object
  sail _sageInf;

  // Scale in world space
  float _worldScale[3];

  // Translation in world space
  float _worldTranslate[3];

#endif

};

/*--------------------------------------------------------------------------*/

#endif 

/*--------------------------------------------------------------------------*/
