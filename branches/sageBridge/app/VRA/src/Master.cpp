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

#include "Master.h"
#include "SynchronizerServerConcreteCommand.h"
#include "UserInterfaceServerConcreteCommand.h"

/*--------------------------------------------------------------------------*/

Master::Master(Communicator* communicator) {

  // Initialize communicator
  _communicator = communicator;

  // Get number of slaves from communicator
  _numberOfSlaves = _communicator -> GetNumberOfNodes() - 1;

  // Initialize axis flag
  _axisFlag = false;

  // Initialize axis position
  _axisPosition[0] = 0.0;
  _axisPosition[1] = 0.0;
  _axisPosition[2] = 0.0;

  // Initialize clipping planes
  _clippingPlanes = new float*[_numberOfSlaves];
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    _clippingPlanes[i] = new float[2];
    _clippingPlanes[i][0] = 0.0;
    _clippingPlanes[i][1] = 0.0;
  }

  // Initialize data filename
  memset(_dataFilename, 0, 1024 * sizeof(char));

  // Initialize data prefix
  memset(_dataPrefix, 0, 1024 * sizeof(char));

  // Initialize dataset list dataset file name
  _datasetListFile = NULL;

  // Initialize dataset list dataset name
  _datasetListName = NULL;

  // Initialize dataset list dataset prefix
  _datasetListPrefix = NULL;

  // Initialize data type to 8-bit
  _dataType = 8;

  // Initialize display percentages
  _displayPercentages = new float*[_numberOfSlaves];
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    _displayPercentages[i] = new float[4];
    _displayPercentages[i][0] = 0.0;
    _displayPercentages[i][1] = 0.0;
    _displayPercentages[i][2] = 0.0;
    _displayPercentages[i][3] = 0.0;
  }

  // Initialize display size
  _displaySize = new int*[_numberOfSlaves];
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    _displaySize[i] = new int[2];
    _displaySize[i][0] = 0;
    _displaySize[i][1] = 0;
  }

  // Initialize exit flag
  _exitFlag = false;

  // Initialize frustum
  _frustum = new float*[_numberOfSlaves];
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    _frustum[i] = new float[4];
    _frustum[i][0] = 0.0;
    _frustum[i][1] = 0.0;
    _frustum[i][2] = 0.0;
    _frustum[i][3] = 0.0;
  }

  // Initialize full screen flag
  _fullScreenFlag = new bool[_numberOfSlaves];
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    _fullScreenFlag[i] = false;
  }

  // Initialize map
  _map = NULL;

  // Initialize number of datasets in dataset list
  _numberOfDatasets = 0;

  // Initialize number of levels
  _numberOfLevels = 0;

  // Initialize and register observer with user interface server
  _observer = new UserInterfaceServerConcreteCommand(this);
  _userInterfaceServer.SetObserver(_observer);
  if (_observer == NULL) {
    fprintf(stderr, 
            "Master: Error creating UserInterfaceServerConcreteCommand.\n");
  }

  // Initialize slice frequency
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

  // Initialize CacheRAM size
  _ramSize = 0;

  // Initialize CacheVRAM size
  _vramSize = 0;

  // Initialize total display dimensions
  _totalDisplayWidth = 0;
  _totalDisplayHeight = 0;

  // Initialize total display frustum
  _totalDisplayFrustum[0] = 0.0;
  _totalDisplayFrustum[1] = 0.0;
  _totalDisplayFrustum[2] = 0.0;
  _totalDisplayFrustum[3] = 0.0;
  _totalDisplayFrustum[4] = 0.0;
  _totalDisplayFrustum[5] = 0.0;

  // Initialize vertical field of view
  _vFOV = 0.0;

  // Initialize viewport
  _viewport = new int*[_numberOfSlaves];
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    _viewport[i] = new int[4];
    _viewport[i][0] = 0;
    _viewport[i][1] = 0;
    _viewport[i][2] = 0;
    _viewport[i][3] = 0;
  }

#ifdef SAGE

  // Initialize rotate device position
  _rotatePosition[0] = 0.0;
  _rotatePosition[1] = 0.0;
  _rotatePosition[2] = 0.0;

  // Initialize scale device position
  _scalePosition[0] = 0.0;
  _scalePosition[1] = 0.0;
  _scalePosition[2] = 0.0;

  // Initialize scale device position
  _scalePositionUntransformed[0] = 0.0;
  _scalePositionUntransformed[1] = 0.0;
  _scalePositionUntransformed[2] = 0.0;

  // Initialize translate device position
  _translatePosition[0] = 0.0;
  _translatePosition[1] = 0.0;
  _translatePosition[2] = 0.0;

  // Initialize world scale
  _worldScale[0] = 1.0;
  _worldScale[1] = 1.0;
  _worldScale[2] = 1.0;

  // Initialize world translate
  _worldTranslate[0] = 0.0;
  _worldTranslate[1] = 0.0;
  _worldTranslate[2] = 0.0;

#endif

}

/*--------------------------------------------------------------------------*/

Master::~Master() {

  // Finalize frame synchronization
  _synchronizationServer.Finalize();

  // Clean up observer
  if (_observer != NULL) {
    delete _observer;
  }

  // Clean up clipping planes
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    delete [] _clippingPlanes[i];
  }
  delete [] _clippingPlanes;

  // Clean up display percentages
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    delete [] _displayPercentages[i];
  }
  delete [] _displayPercentages;

  // Clean up display size
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    delete [] _displaySize[i];
  }
  delete [] _displaySize;

  // Clean up frustums
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    delete [] _frustum[i];
  }
  delete [] _frustum;

  // Clean up full screen flag
  delete [] _fullScreenFlag;

  // Clean up viewport
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    delete [] _viewport[i];
  }
  delete [] _viewport;

  // Clean up dataset list dataset file
  for (int i = 0 ; i < _numberOfDatasets ; i++) {
    delete [] _datasetListFile[i];
  }
  delete [] _datasetListFile;

  // Clean up dataset list dataset name
  for (int i = 0 ; i < _numberOfDatasets ; i++) {
    delete [] _datasetListName[i];
  }
  delete [] _datasetListName;

  // Clean up dataset list dataset prefix
  for (int i = 0 ; i < _numberOfDatasets ; i++) {
    delete [] _datasetListPrefix[i];
  }
  delete [] _datasetListPrefix;

  // Shutdown user interface server
  _userInterfaceServer.Shutdown();

}

/*--------------------------------------------------------------------------*/

void Master::GetAxis(bool* state) {

  // Set state
  *state = _axisFlag;

}

/*--------------------------------------------------------------------------*/

void Master::GetAxisPosition(float* x, float* y, float* z) {

  // Set state
  *x = _axisPosition[0];
  *y = _axisPosition[1];
  *z = _axisPosition[2];

}

/*--------------------------------------------------------------------------*/

void Master::GetBrickBox(bool* state) {

  // Set state
  *state = _brickBoxFlag;

}

/*--------------------------------------------------------------------------*/

void Master::GetBoundingBox(bool* state) {

  // Set state
  *state = _boundingBoxFlag;

}

/*--------------------------------------------------------------------------*/

void Master::GetClippingPlanes(int node, float* near, float* far) {

  // Set state
  *near = _clippingPlanes[node - 1][0];
  *far = _clippingPlanes[node - 1][1];

}

/*--------------------------------------------------------------------------*/

void Master::GetData(char* filename) {

  // Copy data meta file name
  strcpy(filename, _dataFilename);

}

/*--------------------------------------------------------------------------*/

void Master::GetData(char* filename, int* ramSize, int* vramSize) {

  // Copy data meta file name
  strcpy(filename, _dataFilename);

  // Copy cache sizes
  *ramSize = _ramSize;
  *vramSize = _vramSize;

}

/*--------------------------------------------------------------------------*/

void Master::GetDatasetListFile(int index, char* file) {

  // Copy dataset list
  strcpy(file, _datasetListFile[index]);

}

/*--------------------------------------------------------------------------*/

void Master::GetDatasetListName(int index, char* name) {

  // Copy dataset list
  strcpy(name, _datasetListName[index]);

}

/*--------------------------------------------------------------------------*/

void Master::GetDatasetListPrefix(int index, char* prefix) {

  // Copy dataset list
  strcpy(prefix, _datasetListPrefix[index]);

}

/*--------------------------------------------------------------------------*/

void Master::GetDatasetListSize(int* size) {

  // Set dataset list size
  *size = _numberOfDatasets;

}

/*--------------------------------------------------------------------------*/

void Master::GetDatasetType(int* type) {

  // Set dataset type
  *type = _dataType;

}

/*--------------------------------------------------------------------------*/

void Master::GetDisplaySize(int node, int* width, int* height, 
                            bool* fullScreen,
                            float* left, float* right, 
                            float* bottom, float* top) {

  // Set display size
  *width = _displaySize[node - 1][0];
  *height = _displaySize[node - 1][1];

  // Set full screen flag
  *fullScreen = _fullScreenFlag[node - 1];

  // Set display percentages
  *left = _displayPercentages[node - 1][0];
  *right = _displayPercentages[node - 1][1];
  *bottom = _displayPercentages[node - 1][2];
  *top = _displayPercentages[node - 1][3];

}

/*--------------------------------------------------------------------------*/

void Master::GetFrustum(int node, float* left, float* right,
                        float* bottom, float* top) {

  // Set state
  *left = _frustum[node - 1][0];
  *right = _frustum[node - 1][1];
  *bottom = _frustum[node - 1][2];
  *top = _frustum[node - 1][3];

}

/*--------------------------------------------------------------------------*/

void Master::GetMap(unsigned char* map) {

  // Set state
  memcpy(map, _map, 1024);

}

/*--------------------------------------------------------------------------*/

void Master::GetOverviewVertices(float vertices[24]) {

  // Set state
  vertices[0] = _vertex[0].GetX();
  vertices[1] = _vertex[0].GetY();
  vertices[2] = _vertex[0].GetZ();
  vertices[3] = _vertex[1].GetX();
  vertices[4] = _vertex[1].GetY();
  vertices[5] = _vertex[1].GetZ();
  vertices[6] = _vertex[2].GetX();
  vertices[7] = _vertex[2].GetY();
  vertices[8] = _vertex[2].GetZ();
  vertices[9] = _vertex[3].GetX();
  vertices[10] = _vertex[3].GetY();
  vertices[11] = _vertex[3].GetZ();
  vertices[12] = _vertex[4].GetX();
  vertices[13] = _vertex[4].GetY();
  vertices[14] = _vertex[4].GetZ();
  vertices[15] = _vertex[5].GetX();
  vertices[16] = _vertex[5].GetY();
  vertices[17] = _vertex[5].GetZ();
  vertices[18] = _vertex[6].GetX();
  vertices[19] = _vertex[6].GetY();
  vertices[20] = _vertex[6].GetZ();
  vertices[21] = _vertex[7].GetX();
  vertices[22] = _vertex[7].GetY();
  vertices[23] = _vertex[7].GetZ();

}

/*--------------------------------------------------------------------------*/

void Master::GetPrefix(char* prefix) {

  // Copy data prefix
  strcpy(prefix, _dataPrefix);

}

/*--------------------------------------------------------------------------*/

void Master::GetRotationMatrix(float m[16]) {

  // Set state
  m[0] = _R[0]; m[4] = _R[4]; m[8] = _R[8]; m[12] = _R[12];
  m[1] = _R[1]; m[5] = _R[5]; m[9] = _R[9]; m[13] = _R[13];
  m[2] = _R[2]; m[6] = _R[6]; m[10] = _R[10]; m[14] = _R[14];
  m[3] = _R[3]; m[7] = _R[7]; m[11] = _R[11]; m[15] = _R[15];

}

/*--------------------------------------------------------------------------*/

void Master::GetScaleMatrix(float m[16]) {

  // Set state
  m[0] = _S[0]; m[4] = _S[4]; m[8] = _S[8]; m[12] = _S[12];
  m[1] = _S[1]; m[5] = _S[5]; m[9] = _S[9]; m[13] = _S[13];
  m[2] = _S[2]; m[6] = _S[6]; m[10] = _S[10]; m[14] = _S[14];
  m[3] = _S[3]; m[7] = _S[7]; m[11] = _S[11]; m[15] = _S[15];

}

/*--------------------------------------------------------------------------*/

void Master::GetSliceFrequency(double* frequency) {

  // Set state
  *frequency = _sliceFrequency;

}

/*--------------------------------------------------------------------------*/

void Master::GetTotalDisplayDimensions(int* w, int* h) {

  // Set state
  *w = _totalDisplayWidth;
  *h = _totalDisplayHeight;

}

/*--------------------------------------------------------------------------*/

void Master::GetTotalDisplayFrustum(float frustum[6]) {

  // Set state
  frustum[0] = _totalDisplayFrustum[0];
  frustum[1] = _totalDisplayFrustum[1];
  frustum[2] = _totalDisplayFrustum[2];
  frustum[3] = _totalDisplayFrustum[3];
  frustum[4] = _totalDisplayFrustum[4];
  frustum[5] = _totalDisplayFrustum[5];

}

/*--------------------------------------------------------------------------*/

void Master::GetTranslationMatrix(float m[16]) {

  // Set state
  m[0] = _T[0]; m[4] = _T[4]; m[8] = _T[8]; m[12] = _T[12];
  m[1] = _T[1]; m[5] = _T[5]; m[9] = _T[9]; m[13] = _T[13];
  m[2] = _T[2]; m[6] = _T[6]; m[10] = _T[10]; m[14] = _T[14];
  m[3] = _T[3]; m[7] = _T[7]; m[11] = _T[11]; m[15] = _T[15];

}

/*--------------------------------------------------------------------------*/

void Master::GetVerticalFieldOfView(float* vFOV) {

  // Set state
  *vFOV = _vFOV;

}

/*--------------------------------------------------------------------------*/

void Master::GetViewport(int node, int* x, int* y, int* w, int* h) {

  // Set state
  *x = _viewport[node - 1][0];
  *y = _viewport[node - 1][1];
  *w = _viewport[node - 1][2];
  *h = _viewport[node - 1][3];

}

/*--------------------------------------------------------------------------*/

void Master::Init(char* filename) {

  // Local clipping planes
  float clipNear = 0.0;
  float clipFar = 0.0;

  // Local synchronization server port
  int synchronizationPort = 0;

  // Local user interface server port
  int userInterfacePort = 0;


#ifdef SAGE

  // Connect to SAGE

  // SAGE configuration
  _sageConfig.init("VRA.conf");
  _sageConfig.setAppName("VRA");
  _sageConfig.rank = 0;
  _sageConfig.nodeNum = (_communicator -> GetNumberOfNodes()) -1;
  _sageConfig.master = true;
  _sageConfig.rendering = false;

  // Initialize SAGE
  _sageInf.init(_sageConfig);

#endif


  // Read configuration file from disk

  // File pointer
  FILE* fptr = NULL;

  // Attempt to open file
  if ((fptr = fopen(filename, "r")) == NULL) {
    fprintf(stderr, 
            "Master: Can not read display configuration file %s\n.", 
            filename);
    return;
  }

  // Read user interface port
  fscanf(fptr, "%d", &userInterfacePort);

  // Read synchronization port
  fscanf(fptr, "%d", &synchronizationPort);

  // Read near and far clipping planes
  fscanf(fptr, "%f %f", &clipNear, &clipFar);

  // Assign all clipping planes
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    _clippingPlanes[i][0] = clipNear;
    _clippingPlanes[i][1] = clipFar;
  }

  // Read total display size
  fscanf(fptr, "%d %d", &_totalDisplayWidth, &_totalDisplayHeight);

  // Read vertical field of view
  fscanf(fptr, "%f", &_vFOV);

  // Read and assign viewport and normalize coordinates for each node
  for (int i = 0 ; i < _numberOfSlaves ; i++) {

    // Read viewport and normalized coordinates
    fscanf(fptr, "%d %d %d %d %f %f %f %f",
           &_viewport[i][0], &_viewport[i][1],
           &_viewport[i][2], &_viewport[i][3],
           &_displayPercentages[i][0], &_displayPercentages[i][1],
           &_displayPercentages[i][2], &_displayPercentages[i][3]);

    // Assign display size based on viewport
    _displaySize[i][0] = _viewport[i][2];
    _displaySize[i][1] = _viewport[i][3];

    // Read and interpret full screen flag
    int flag = 0;
    fscanf(fptr, "%d", &flag);
    if (flag == 0) {
      _fullScreenFlag[i] = false;
    }
    else {
      _fullScreenFlag[i] = true;
    }

  }

  // Read number of datasets in dataset list
  fscanf(fptr, "%d", &_numberOfDatasets);

  // Allocate memory for dataset list dataset file name
  _datasetListFile = new char*[_numberOfDatasets];
  for (int i = 0 ; i < _numberOfDatasets ; i++) {
    _datasetListFile[i] = new char[1024];
  }

  // Allocate memory for dataset list dataset name
  _datasetListName = new char*[_numberOfDatasets];
  for (int i = 0 ; i < _numberOfDatasets ; i++) {
    _datasetListName[i] = new char[1024];
  }

  // Allocate memory for dataset list dataset prefix
  _datasetListPrefix = new char*[_numberOfDatasets];
  for (int i = 0 ; i < _numberOfDatasets ; i++) {
    _datasetListPrefix[i] = new char[1024];
  }

  // Read dataset list
  for (int i = 0 ; i < _numberOfDatasets ; i++) {
    fscanf(fptr, 
           "%s %s %s", 
           _datasetListName[i], 
           _datasetListPrefix[i], 
           _datasetListFile[i]);
  }

  // Close file
  fclose(fptr);


  // Calculate frustums

  // Setup frustum calculator
  FrustumCalculator frustumCalculator;
  frustumCalculator.SetDisplayWidth(_totalDisplayWidth);
  frustumCalculator.SetDisplayHeight(_totalDisplayHeight);
  frustumCalculator.SetNearClippingPlane(clipNear);
  frustumCalculator.SetVerticalFieldOfView(_vFOV);

  // Calculate frustum for each node
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    frustumCalculator.SetPercentage(_displayPercentages[i]);
    frustumCalculator.CalculateFrustum();
    frustumCalculator.GetFrustum(_frustum[i]);
  }

  // Calculate full frustum for entire display
  frustumCalculator.GetFullFrustum(&_totalDisplayFrustum[0],
                                   &_totalDisplayFrustum[1],
                                   &_totalDisplayFrustum[2],
                                   &_totalDisplayFrustum[3]);
  _totalDisplayFrustum[4] = clipNear; 
  _totalDisplayFrustum[5] = clipFar;


  // Initialize and register observer with synchronizer server
  _progressObserver = new SynchronizerServerConcreteCommand(this);
  _synchronizationServer.SetProgressObserver(_progressObserver);
  if (_progressObserver == NULL) {
    fprintf(stderr, 
            "Master: Error creating SynchronizerServerConcreteCommand.\n");
  }

  // Initialize synchronization server
  _synchronizationServer.Init(synchronizationPort);


  // Initialize slaves

  // Get name of master node
  char hostname[128];
  _communicator -> GetMasterNodeName(hostname);

  // Send display size to slaves
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    UpdateDisplaySize(i+1, _displaySize[i][0], _displaySize[i][1],
                      _fullScreenFlag[i], hostname, synchronizationPort,
                      _displayPercentages[i][0], _displayPercentages[i][1],
                      _displayPercentages[i][2], _displayPercentages[i][3]);
    _communicator -> Barrier();
  }

  // Send clipping planes to slaves
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    UpdateClippingPlanes(i+1, _clippingPlanes[i][0], _clippingPlanes[i][1]);  
    _communicator -> Barrier();
  }

  // Send frustums to slaves
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    UpdateFrustum(i+1, _frustum[i][0], _frustum[i][1],
                  _frustum[i][2], _frustum[i][3]);
    _communicator -> Barrier();
  }

  // Send viewports to slaves
  for (int i = 0 ; i < _numberOfSlaves ; i++) {
    UpdateViewport(i+1, _viewport[i][0], _viewport[i][1],
                   _viewport[i][2], _viewport[i][3]);
    _communicator -> Barrier();
  }

  // Turn axis on
  UpdateAxisOn();
  _communicator -> Barrier();

  // Set axis position
  UpdateAxisPosition(_totalDisplayFrustum[0] + 0.5,
                     _totalDisplayFrustum[2] + 0.5,
                     0.0);
  _communicator -> Barrier();

  // Turn on bounding box
  UpdateBoundingBoxOn();
  _communicator -> Barrier();

  // Turn off brick box
  UpdateBrickBoxOff();
  _communicator -> Barrier();

  // Initialize user interface server
  _userInterfaceServer.Init(userInterfacePort);

}

/*--------------------------------------------------------------------------*/

void Master::ResetView() {

#ifdef SAGE

  // Reset trackball
  _eventTrackball.Clear();
  
#endif

}

/*--------------------------------------------------------------------------*/

void Master::Run() {

  // Event loop
  while(_exitFlag == false) {

    // Wait for a new connection from 
    _userInterfaceServer.Accept();

    // Client error
    int error = USER_INTERFACE_SERVER_OK;

    // Loop getting commands from user as long as a client is connected
    while (error != USER_INTERFACE_SERVER_ERROR) {

      // Get command from client
      error = _userInterfaceServer.GetCommand();

      // If client received a command call barrier
      if (error == USER_INTERFACE_SERVER_OK) {
        _communicator -> Barrier();
      }

#ifdef SAGE

      // SAGE message
      sageMessage msg;

      // Check for SAGE message
      if (_sageInf.checkMsg(msg, false) > 0) {

        // Get SAGE message data
        char* data = (char*) msg.getData();

        // Determine message
        switch(msg.getCode()) {
          
        // Quit
        case APP_QUIT:
          
          // Send exit command to UI
          _userInterfaceServer.SendCommandExit();

          // Send exit command
          UpdateCommandExit();         

          // Stop this loop
          error = USER_INTERFACE_SERVER_ERROR;
          
          break;
        

        // Click event
        case EVT_CLICK: {

          // Click event x and y location normalized to size of window
          float clickX, clickY;

          // Ckick device Id, button Id, and is down flag
          int clickDeviceId, clickButtonId, clickIsDown, clickEvent;

          // Parse message
          sscanf(data, 
                 "%d %f %f %d %d %d", 
                 &clickDeviceId, &clickX, &clickY, 
                 &clickButtonId, &clickIsDown, &clickEvent);

          // If down event for EVT_PAN
          if (clickIsDown && clickEvent == EVT_PAN) {

            // Set initial position of click event
            _translatePosition[0] = clickX;
            _translatePosition[1] = clickY;

          }

          // If down event for EVT_ZOOM
          if (clickIsDown && clickEvent == EVT_ZOOM) {

            // Set initial position of click event
            _scalePosition[0] = clickX;
            _scalePosition[1] = clickY;

            // Calculate the location of the device in world space
            float initX = 
              (_scalePosition[0] * 
               (_totalDisplayFrustum[1] - _totalDisplayFrustum[0])) +
              _totalDisplayFrustum[0];
            float initY =
              (_scalePosition[1] * 
               (_totalDisplayFrustum[3] - _totalDisplayFrustum[2])) +
              _totalDisplayFrustum[2];

            // Calculate the location of the device with respect to the
            // untransfomred object in world space
            initX -= _worldTranslate[0];
            initY -= _worldTranslate[1];
            initX /= _worldScale[0];
            initY /= _worldScale[1];

            // Keep these values
            _scalePositionUntransformed[0] = initX;
            _scalePositionUntransformed[1] = initY;
            
          }

          // If down event for EVT_ROTATE
          if (clickIsDown && clickEvent == EVT_ROTATE) {

            // Set inital position
            _rotatePosition[0] = clickX;
            _rotatePosition[1] = clickY;

            // Normalize object's translation to (-1.0, 1.0)
            float nTx = (_worldTranslate[0] - _totalDisplayFrustum[0]) /
              (_totalDisplayFrustum[1] - _totalDisplayFrustum[0]);
            nTx = (nTx * 2.0) - 1.0;
            float nTy = (_worldTranslate[1] - _totalDisplayFrustum[2]) /
              (_totalDisplayFrustum[3] - _totalDisplayFrustum[2]);
            nTy = (nTy * 2.0) - 1.0;
        
            // Normalize puck's position to (-1.0, 1.0)
            float nXpos = (clickX * 2.0) - 1.0;
            float nYpos = (clickY * 2.0) - 1.0;
        
            // Start trackball
            _eventTrackball.Start(nXpos - nTx, nYpos - nTy);

          }

          // If up event
          if (clickIsDown == 0) {

            // Force the renderer to update
            UpdateCommandRender();
            _communicator -> Barrier();
            UpdateCommandRender();
            _communicator -> Barrier();
            UpdateCommandRender();
            _communicator -> Barrier();
            UpdateCommandRender();
            _communicator -> Barrier();
            UpdateCommandRender();
            _communicator -> Barrier();

          }

          // Done with EVT_CLICK case
          break;

        }
        

        // Pan event
        case EVT_PAN: {

          // Pan event properties
          int panDeviceId;
          
          // Pan event x and y location and change in x, y and z direction
          // normalized to size of window
          float panX, panY, panDX, panDY, panDZ;
          sscanf(data, 
                 "%d %f %f %f %f %f", 
                 &panDeviceId, &panX, &panY, &panDX, &panDY, &panDZ);

          // Print event
          //fprintf(stderr,
          //        "PAN MESSAGE: %f %f %f %f %f\n",
          //        panX, panY, panDX, panDY, panDZ);

          // Calculate amount the device has moved in world space since it 
          // was clicked.
          float deltaT[2] = {((_translatePosition[0]-
                               (_translatePosition[0] + panDX)) * 
                              (_totalDisplayFrustum[1] - 
                               _totalDisplayFrustum[0])), 
                             ((_translatePosition[1]-
                               (_translatePosition[1] + panDY)) *
                              (_totalDisplayFrustum[3] - 
                               _totalDisplayFrustum[2]))};
          
          // Add the offset
          _worldTranslate[0] -= deltaT[0];
          _worldTranslate[1] -= deltaT[1];
          
          // Determine translation matrix
          float T[16];
          calc_trans(T, 
                     _worldTranslate[0], 
                     _worldTranslate[1], 
                     0.0);
          
          // Send translation matrix to UI
          _userInterfaceServer.SendTranslationMatrix(T);

          // Set the translation matrix
          UpdateTranslationMatrix(T);
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();

          // Update translate device position
          _translatePosition[0] += panDX;
          _translatePosition[1] += panDY;
          
          // Done with EVT_PAN case
          break;

        }

        // Zoom event
        case EVT_ZOOM: {

          // Zoom event properties
          int zoomDeviceId;
          
          // Zoom event x and y location and change in x, y and z direction
          // normalized to size of window
          float zoomX, zoomY, zoomDX, zoomDY, zoomDZ;
          sscanf(data, 
                 "%d %f %f %f %f %f", 
                 &zoomDeviceId, &zoomX, &zoomY, &zoomDX, &zoomDY, &zoomDZ);

          // Print event
          //fprintf(stderr,
          //        "ZOOM MESSAGE: %f %f %f %f %f\n",
          //        zoomX, zoomY, zoomDX, zoomDY, zoomDZ);

          // Calcualte amount the device has moved
          float deltaZ[2] = {-zoomDX, -zoomDY};

          // Update world scale by delta
          _worldScale[0] *= 1.0 - (deltaZ[0] + deltaZ[1]);
          _worldScale[1] *= 1.0 - (deltaZ[0] + deltaZ[1]);
          _worldScale[2] *= 1.0 - (deltaZ[0] + deltaZ[1]);

          // Don't let world scale go below 1.0
          if (_worldScale[0] < 1.0) _worldScale[0] = 1.0;
          if (_worldScale[1] < 1.0) _worldScale[1] = 1.0;
          if (_worldScale[2] < 1.0) _worldScale[2] = 1.0;

          // Determine scale matrix
          float S[16];
          calc_scale(S, 
                     _worldScale[0], 
                     _worldScale[1], 
                     _worldScale[2]);

          // Calculate the initial location of the device in world space
          float initX = 
            (zoomX * 
             (_totalDisplayFrustum[1] - _totalDisplayFrustum[0])) +
            _totalDisplayFrustum[0];
          float initY =
            (zoomY * 
             (_totalDisplayFrustum[3] - _totalDisplayFrustum[2])) +
            _totalDisplayFrustum[2];

          // Determine new translation based on scale
          _worldTranslate[0] = 
            initX - (_scalePositionUntransformed[0] * _worldScale[0]);
          _worldTranslate[1] = 
            initY - (_scalePositionUntransformed[1] * _worldScale[1]);

          // Determine new translation matrix
          float T[16];
          calc_trans(T, 
                     _worldTranslate[0], 
                     _worldTranslate[1], 
                     0.0);

          // Send scale matrix to UI
          _userInterfaceServer.SendScaleMatrix(S);

          // Send translation matrix to UI
          _userInterfaceServer.SendTranslationMatrix(T);

          // Set the scale and translation matrices
          UpdateScaleMatrix(S);
          _communicator -> Barrier();
          UpdateTranslationMatrix(T);
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();

          // Done with EVT_ZOOM case
          break;
             
        }

        // Rotate event
        case EVT_ROTATE: {

          // Rotate event properties
          int rotateDeviceId;
          
          // Rotate event x and y location and change in x, y and z direction
          // normalized to size of window
          float rotateX, rotateY, rotateDX, rotateDY, rotateDZ;
          sscanf(data, 
                 "%d %f %f %f %f %f", 
                 &rotateDeviceId, &rotateX, &rotateY, 
                 &rotateDX, &rotateDY, &rotateDZ);

          //fprintf(stderr, "EVT_ROTATE: %f %f %f %f %f\n",
          //        rotateX, rotateY, rotateDX, rotateDY, rotateDZ);

          // Normalize object's translation to (-1.0, 1.0)
          float nTx = (_worldTranslate[0] - _totalDisplayFrustum[0]) /
            (_totalDisplayFrustum[1] - _totalDisplayFrustum[0]);
          nTx = (nTx * 2.0) - 1.0;
          float nTy = (_worldTranslate[1] - _totalDisplayFrustum[2]) /
            (_totalDisplayFrustum[3] - _totalDisplayFrustum[2]);
          nTy = (nTy * 2.0) - 1.0;

          // Normalize puck's position to (-1.0, 1.0)
          float nXpos = (_rotatePosition[0] * 2.0) - 1.0;
          float nYpos = (_rotatePosition[1] * 2.0) - 1.0;

          // Update trackball
          _eventTrackball.Update(nXpos - nTx, nYpos - nTy);
      
          // Get rotation from trackball
          float R[16];
          _eventTrackball.GetRotationMatrix(R);

          // Send rotation matrix to UI
          _userInterfaceServer.SendRotationMatrix(R);

          // Set the rotation matrix
          UpdateRotationMatrix(R);
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();
          UpdateCommandRender();
          _communicator -> Barrier();

          _rotatePosition[0] += rotateDX;
          _rotatePosition[1] += rotateDY;

          // Done with EVT_ZOOM case
          break;

        }

        }
        
      }

#endif

    }

  }

  // One last barrier
  _communicator -> Barrier();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateAxisOff() {

  // Set state
  _axisFlag = false;

  // Send command
  _communicator -> SendAxisOff();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateAxisOn() {

  // Set state
  _axisFlag = true;

  // Send command
  _communicator -> SendAxisOn();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateAxisPosition(float x, float y, float z) {

  // Set state
  _axisPosition[0] = x;
  _axisPosition[1] = y;
  _axisPosition[2] = z;

  // Send command
  _communicator -> SendAxisPosition(_axisPosition[0], 
                                    _axisPosition[1], 
                                    _axisPosition[2]);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateBrickBoxOff() {

  // Set state
  _brickBoxFlag = false;

  // Send command
  _communicator -> SendBrickBoxOff();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateBrickBoxOn() {

  // Set state
  _brickBoxFlag = true;

  // Send command
  _communicator -> SendBrickBoxOn();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateBoundingBoxOff() {

  // Set state
  _boundingBoxFlag = false;

  // Send command
  _communicator -> SendBoundingBoxOff();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateBoundingBoxOn() {

  // Set state
  _boundingBoxFlag = true;

  // Send command
  _communicator -> SendBoundingBoxOn();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateClippingPlanes(int node, float near, float far) {

  // Set state
  _clippingPlanes[node - 1][0] = near;
  _clippingPlanes[node - 1][1] = far;

  // Send command
  _communicator -> SendClippingPlanes(node,
                                      _clippingPlanes[node - 1][0],
                                      _clippingPlanes[node - 1][1]);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateCommandExit() {

#ifdef SAGE

  // Shutdown SAGE
  _sageInf.shutdown();

#endif

  // Set state
  _exitFlag = 1;

  // Send command
  _communicator -> SendCommandExit();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateCommandRender() {

  // Send command
  _communicator -> SendCommandRender();

}

/*--------------------------------------------------------------------------*/

void Master::UpdateData(char* filename) {

  // Check if not NULL
  if (filename != NULL) {

    // Set state
    strcpy(_dataFilename, filename);
    _ramSize = 0;
    _vramSize = 0;

    // Send command
    _communicator -> SendData(_dataFilename);

    // Load metadata
    Octree* data = new Octree();
    data -> SetPrefix(_dataPrefix);
    data -> LoadFromFile2(_dataFilename);
    _numberOfLevels = data -> GetNumberOfLevels();
    if (data -> GetDataType() == OCTREE_NODE_UNSIGNED_8_INT) {
      _dataType = 8;
    }
    else if (data -> GetDataType() == OCTREE_NODE_UNSIGNED_16_INT) {
      _dataType = 16;
    }

    // Get vertices from root node
    OctreeNode* node = data -> GetRoot();
    for (int i = 0 ; i < 8 ; i++) {
      node -> GetVertex(i, &_vertex[i]);
    }

    // Clean up
    delete data;

  }

}

/*--------------------------------------------------------------------------*/

void Master::UpdateData(char* filename, int ramSize, int vramSize) {

  // Check if not NULL
  if (filename != NULL) {

    // Set state
    strcpy(_dataFilename, filename);
    _ramSize = ramSize;
    _vramSize = vramSize;

    // Send command
    _communicator -> SendData(_dataFilename, _ramSize, _vramSize);

    // Load metadata
    Octree* data = new Octree();
    data -> SetPrefix(_dataPrefix);
    data -> LoadFromFile2(_dataFilename);
    _numberOfLevels = data -> GetNumberOfLevels();
    if (data -> GetDataType() == OCTREE_NODE_UNSIGNED_8_INT) {
      _dataType = 8;
    }
    else if (data -> GetDataType() == OCTREE_NODE_UNSIGNED_16_INT) {
      _dataType = 16;
    }

    // Get vertices from root node
    OctreeNode* node = data -> GetRoot();
    for (int i = 0 ; i < 8 ; i++) {
      node -> GetVertex(i, &_vertex[i]);
    }

    // Clean up
    delete data;

  }

}

/*--------------------------------------------------------------------------*/

void Master::UpdateDisplaySize(int node, int width, int height, 
                               bool fullScreen, char* hostname, int port,
                               float left, float right, 
                               float bottom, float top) {

  // Set state
  _displaySize[node - 1][0] = width;
  _displaySize[node - 1][1] = height;
  _fullScreenFlag[node - 1] = fullScreen;
  _displayPercentages[node - 1][0] = left;
  _displayPercentages[node - 1][1] = right;
  _displayPercentages[node - 1][2] = bottom;
  _displayPercentages[node - 1][3] = top;

  // Send command
  _communicator -> SendDisplaySize(node,
                                   _displaySize[node - 1][0],
                                   _displaySize[node - 1][1],
                                   _fullScreenFlag[node - 1],
                                   hostname,
                                   port,
                                   _displayPercentages[node - 1][0],
                                   _displayPercentages[node - 1][1],
                                   _displayPercentages[node - 1][2],
                                   _displayPercentages[node - 1][3]);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateFrustum(int node, float left, float right, 
                           float bottom, float top) {

  // Set state
  _frustum[node - 1][0] = left;
  _frustum[node - 1][1] = right;
  _frustum[node - 1][2] = bottom;
  _frustum[node - 1][3] = top;

  // Send command
  _communicator -> SendFrustum(node,
                               _frustum[node - 1][0],
                               _frustum[node - 1][1],
                               _frustum[node - 1][2],
                               _frustum[node - 1][3]);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateMap(unsigned char* map) {

  // Set state
  _map = map;

  // Send command
  _communicator -> SendMap(_map);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateMap16(unsigned char* map) {

  // Set state
  _map = map;

  // Send command
  _communicator -> SendMap16(_map);

}

/*--------------------------------------------------------------------------*/

void Master::UpdatePrefix(char* prefix) {

  // Check if not NULL
  if (prefix != NULL) {

    // Set state
    strcpy(_dataPrefix, prefix);

    // Send command
    _communicator -> SendPrefix(_dataPrefix);

  }

}

/*--------------------------------------------------------------------------*/

void Master::UpdateProgress(int value) {

  // Send progress to UI server
  if (_userInterfaceServer.IsInit() == true) {
    _userInterfaceServer.SendProgress(value, _numberOfLevels);
  }

}

/*--------------------------------------------------------------------------*/

void Master::UpdateRotationMatrix(float m[16]) {

  // Set state
  _R[0] = m[0]; _R[4] = m[4]; _R[8] = m[8]; _R[12] = m[12];
  _R[1] = m[1]; _R[5] = m[5]; _R[9] = m[9]; _R[13] = m[13];
  _R[2] = m[2]; _R[6] = m[6]; _R[10] = m[10]; _R[14] = m[14];
  _R[3] = m[3]; _R[7] = m[7]; _R[11] = m[11]; _R[15] = m[15];

  // Send command
  _communicator -> SendRotationMatrix(_R);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateScaleMatrix(float m[16]) {

#ifdef SAGE

  // Update world scale for SAGE events
  _worldScale[0] = m[0];
  _worldScale[1] = m[5];
  _worldScale[2] = m[10];

#endif

  // Set statea
  _S[0] = m[0]; _S[4] = m[4]; _S[8] = m[8]; _S[12] = m[12];
  _S[1] = m[1]; _S[5] = m[5]; _S[9] = m[9]; _S[13] = m[13];
  _S[2] = m[2]; _S[6] = m[6]; _S[10] = m[10]; _S[14] = m[14];
  _S[3] = m[3]; _S[7] = m[7]; _S[11] = m[11]; _S[15] = m[15];

  // Send command
  _communicator -> SendScaleMatrix(_S);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateSliceFrequency(double frequency) {

  // Set statea
  _sliceFrequency = frequency;

  // Send command
  _communicator -> SendSliceFrequency(_sliceFrequency);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateTranslationMatrix(float m[16]) {
  
#ifdef SAGE

  // Update world translate for SAGE events
  _worldTranslate[0] = m[12];
  _worldTranslate[1] = m[13];
  _worldTranslate[2] = m[14];

#endif

  // Set state
  _T[0] = m[0]; _T[4] = m[4]; _T[8] = m[8]; _T[12] = m[12];
  _T[1] = m[1]; _T[5] = m[5]; _T[9] = m[9]; _T[13] = m[13];
  _T[2] = m[2]; _T[6] = m[6]; _T[10] = m[10]; _T[14] = m[14];
  _T[3] = m[3]; _T[7] = m[7]; _T[11] = m[11]; _T[15] = m[15];

  // Send command
  _communicator -> SendTranslationMatrix(_T);

}

/*--------------------------------------------------------------------------*/

void Master::UpdateViewport(int node, int x, int y, int w, int h) {

  // Set state
  _viewport[node-1][0] = x;
  _viewport[node-1][1] = y;
  _viewport[node-1][2] = w;
  _viewport[node-1][3] = h;

  // Send command
  _communicator -> SendViewport(node,
                                _viewport[node-1][0],
                                _viewport[node-1][1],
                                _viewport[node-1][2],
                                _viewport[node-1][3]);

}

/*--------------------------------------------------------------------------*/
