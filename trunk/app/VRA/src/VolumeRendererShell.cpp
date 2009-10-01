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

#include "VolumeRendererShell.h"

#include "SynchronizerClient.h"

/*--------------------------------------------------------------------------*/

#ifdef SAGE

// Define static variable for glXPBuffer
GLXPbufferSGIX VolumeRendererShell::_glXPBuffer = 0;

// Define static variable for X display
Display* VolumeRendererShell::_xDisplay = NULL;

// Define static variable for X error flag
int VolumeRendererShell::_xErrorFlag = 0;

// Define static variable for X visual info
XVisualInfo* VolumeRendererShell::_xVisualInfo = NULL;

#endif

/*--------------------------------------------------------------------------*/

VolumeRendererShell::VolumeRendererShell() {

  // Initialize condition variable
  pthread_cond_init(&_conditionCond, NULL);

  // Initialize mutex
  pthread_mutex_init(&_conditionMutex, NULL);

}

/*--------------------------------------------------------------------------*/

VolumeRendererShell::~VolumeRendererShell() {

  // Clean up condition variable
  pthread_cond_destroy(&_conditionCond);

  // Clean up mutex
  pthread_mutex_destroy(&_conditionMutex);

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::AbortRender(bool flag) {

  // Set abort flag
  _info._abortFlag = flag;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::DestroyDisplay() {

#ifdef SAGE

  // Clean up pbuffer
  glXDestroyGLXPbufferSGIX(_xDisplay, _glXPBuffer);

  // Clean up visual info
  XFree(_xVisualInfo);

  // Close X connection
  XCloseDisplay(_xDisplay);

#else
  
  // Quit SDL
  SDL_Quit();

#endif

}

/*--------------------------------------------------------------------------*/

#ifdef SAGE

int VolumeRendererShell::HandleXError(Display* display, XErrorEvent *event) {

  // Set error flag
  _xErrorFlag = 1;

  // Return 0
  return 0;

}

#endif

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::Init(int width, int height, bool fullScreen, 
                               char* hostname, int port,
                               float left, float right,
                               float bottom, float top,
                               int rank, int numberOfNodes) {

  // Set mutex and condition variable in thread state
  _info._conditionCond = &_conditionCond;
  _info._conditionMutex = &_conditionMutex;

  // Set display information in thread state
  _info._displaySize[0] = width;
  _info._displaySize[1] = height;
  _info._fullScreenFlag = fullScreen;
  _info._normalCoordinates[0] = left;
  _info._normalCoordinates[1] = right;
  _info._normalCoordinates[2] = bottom;
  _info._normalCoordinates[3] = top;

  // Set synchronization hostname and port
  strcpy(_info._synchronizationHostname, hostname);
  _info._synchronizationPort = port;

  // Set rank and number of nodes
  _info._rank = rank;
  _info._numberOfNodes = numberOfNodes;

  // Create thread
  pthread_create(&_thread, NULL, VolumeRendererShell::ThreadFunction, &_info);

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::InitDisplay(int width, int height, bool fullScreen, 
                                      float left, float right, 
                                      float bottom, float top) {

#ifdef SAGE

  // GLX context
  GLXContext glXContext;

  // GLX pbuffer
  GLXFBConfigSGIX glXFBConfig = 0;

  // X screen
  int xScreen = 0;

  // Open display
  _xDisplay = XOpenDisplay(NULL);
  if (!_xDisplay) {
    fprintf(stderr, "VolumeRendererShell: Could not open X display.\n");
    return;
  }

  // Get default screen
  xScreen = DefaultScreen(_xDisplay);

  // Double buffered, with depth buffer
  int fbAttribs[] = {
    GLX_RENDER_TYPE_SGIX, GLX_RGBA_BIT_SGIX,
    GLX_DRAWABLE_TYPE_SGIX, GLX_PIXMAP_BIT_SGIX,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_DEPTH_SIZE, 1,
    GLX_DOUBLEBUFFER, 1,
    GLX_STENCIL_SIZE, 0,
    None
  };

  // Attributes
  int pbAttribs[] = {
    GLX_LARGEST_PBUFFER_SGIX, True,
    GLX_PRESERVED_CONTENTS_SGIX, False,
    None
  }; 

  // Possible configurations
  GLXFBConfigSGIX* glXFBConfigs = NULL;
  int numberOfGLXFBConfigs;
  
  // Get list of possible frame buffer configurations
  glXFBConfigs = glXChooseFBConfigSGIX(_xDisplay, xScreen, 
                                       fbAttribs, &numberOfGLXFBConfigs);
  if (numberOfGLXFBConfigs == 0 || !glXFBConfigs) {
    fprintf(stderr, "VolumeRendererShell: Choosing FBConfig failed.\n");
    XCloseDisplay(_xDisplay);
    return;
  }

  // Create pbuffer using first config in the list that works
  for (int i = 0 ; i < numberOfGLXFBConfigs ; i++) {

    // Catch X errors with error handler
    int (*oldHandler)(Display*, XErrorEvent*);
    oldHandler = XSetErrorHandler(HandleXError);
    _xErrorFlag = 0;

    // Create pbuffer
    _glXPBuffer = glXCreateGLXPbufferSGIX(_xDisplay, glXFBConfigs[i], 
                                         width, height, pbAttribs);

    // Restore original X error handler
    (void) XSetErrorHandler(oldHandler);

    // Found a working configuration
    if (!_xErrorFlag && _glXPBuffer != None) {
      glXFBConfig = glXFBConfigs[i];
      break;
    }
  }

  // Clean up
  XFree(glXFBConfigs);

  // If a pbuffer couldn't be created
  if (_glXPBuffer == None) {
    fprintf(stderr, "VolumeRendererShell: Couldn't create pbuffer.\n");
    XCloseDisplay(_xDisplay);
    return;
  }

  // Get corresponding XVisualInfo
  _xVisualInfo = glXGetVisualFromFBConfigSGIX(_xDisplay, glXFBConfig);
  if (!_xVisualInfo) {
    fprintf(stderr, "VolumeRendererShell: Can't get XVisualInfo.\n");
    XCloseDisplay(_xDisplay);
    return;
  }

  // Create GLX context
  glXContext = glXCreateContext(_xDisplay, _xVisualInfo, NULL, True);
  if (!glXContext) {
    glXContext = glXCreateContext(_xDisplay, _xVisualInfo, NULL, False);
    if (!glXContext) {
      fprintf(stderr, "VolumeRendererShell: Couldn't create GLXContext.\n");
      XFree(_xVisualInfo);
      XCloseDisplay(_xDisplay);
      return;
    }
  }

  // Bind context to pbuffer
  if (!glXMakeCurrent(_xDisplay, _glXPBuffer, glXContext)) {
    fprintf(stderr, "VolumeRendererShell: glXMakeCurrent failed.\n");
    XFree(_xVisualInfo);
    XCloseDisplay(_xDisplay);
    return;
  }

#else

  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  
  // Set video mode
  if (fullScreen == false) {
    SDL_SetVideoMode(width, 
                     height, 
                     0, 
                     SDL_DOUBLEBUF | SDL_OPENGL);
  }
  else {
   SDL_SetVideoMode(width, 
                    height, 
                    0, 
                    SDL_DOUBLEBUF | SDL_OPENGL | SDL_FULLSCREEN);
   SDL_ShowCursor(SDL_DISABLE);
  }
  SDL_WM_SetCaption("Volume Renderer", NULL);

#endif

}

/*--------------------------------------------------------------------------*/

bool VolumeRendererShell::InRender() {

  // Return in render flag
  return _info._inRenderFlag;

}

/*--------------------------------------------------------------------------*/

bool VolumeRendererShell::IsInitialized() {

  // To be implemented
  return true;

}

/*--------------------------------------------------------------------------*/
  
void VolumeRendererShell::Render() {

  // Signal thread to update volume renderer and render
  pthread_mutex_lock(&_conditionMutex);
  _info._renderFlag = true;
  pthread_cond_signal(&_conditionCond);
  pthread_mutex_unlock(&_conditionMutex);

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetAxisOff() {

  // Set flag
  _info._axisOnFlag = false;

  // Set update flag
  _info._updateAxis = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetAxisOn() {

  // Set flag
  _info._axisOnFlag = true;

  // Set update flag
  _info._updateAxis = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetAxisPosition(float x, float y, float z) {

  // Set data
  _info._axisPosition[0] = x;
  _info._axisPosition[1] = y;
  _info._axisPosition[2] = z;

  // Set update flag
  _info._updateAxis = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetBrickBoxOff() {

  // Set flag
  _info._brickBoxOnFlag = false;

  // Set update flag
  _info._updateBrickBox = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetBrickBoxOn() {

  // Set flag
  _info._brickBoxOnFlag = true;

  // Set update flag
  _info._updateBrickBox = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetBoundingBoxOff() {

  // Set flag
  _info._boundingBoxOnFlag = false;

  // Set update flag
  _info._updateBoundingBox = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetBoundingBoxOn() {

  // Set flag
  _info._boundingBoxOnFlag = true;

  // Set update flag
  _info._updateBoundingBox = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetData(Octree* data) {

  // Set data
  _info._data = data;

  // Set default cache sizes
  _info._ramSize = 50;
  _info._vramSize = 10;

  // Set update flag
  _info._updateData = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetData(Octree* data, int ramSize, int vramSize) {

  // Set data
  _info._data = data;

  // Set cache sizes
  _info._ramSize = ramSize;
  _info._vramSize = vramSize;

  // Set update flag
  _info._updateData = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetExitFlag() {

  // Set exit flag
  _info._exitFlag = true;

  // Signal thread to update volume renderer and render
  pthread_mutex_lock(&_conditionMutex);
  _info._renderFlag = true;
  pthread_cond_signal(&_conditionCond);
  pthread_mutex_unlock(&_conditionMutex);

  // Wait for thread to finish
  pthread_join(_thread, NULL);

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetFrustum(float left, float right, float bottom,
                                     float top, float near, float far) {

  // Set frustum
  _info._frustum[0] = left;
  _info._frustum[1] = right;
  _info._frustum[2] = bottom;
  _info._frustum[3] = top;
  _info._frustum[4] = near;
  _info._frustum[5] = far;

  // Set update flag
  _info._updateFrustum = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetMap(unsigned char* map) {

  // Set map
  _info._map = map;
  _info._mapType = VOLUME_RENDERER_THREAD_INFO_MAP_8;

  // Set update flag
  _info._updateMap = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetMap8(unsigned char* map) {

  // Set map
  _info._map = map;
  _info._mapType = VOLUME_RENDERER_THREAD_INFO_MAP_8;

  // Set update flag
  _info._updateMap = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetMap16(unsigned char* map) {

  // Set map
  _info._map = map;
  _info._mapType = VOLUME_RENDERER_THREAD_INFO_MAP_16;

  // Set update flag
  _info._updateMap = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetR(float m[16]) {

  // Set rotation matrix
  _info._R[0] = m[0];     
  _info._R[1] = m[1]; 
  _info._R[2] = m[2]; 
  _info._R[3] = m[3]; 

  _info._R[4] = m[4];
  _info._R[5] = m[5];
  _info._R[6] = m[6];
  _info._R[7] = m[7];

  _info._R[8] = m[8];
  _info._R[9] = m[9];
  _info._R[10] = m[10];
  _info._R[11] = m[11];

  _info._R[12] = m[12];
  _info._R[13] = m[13];
  _info._R[14] = m[14];
  _info._R[15] = m[15];

  // Set update flag
  _info._updateR = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetS(float m[16]) {

  // Set scale matrix
  _info._S[0] = m[0];     
  _info._S[1] = m[1]; 
  _info._S[2] = m[2]; 
  _info._S[3] = m[3]; 

  _info._S[4] = m[4];
  _info._S[5] = m[5];
  _info._S[6] = m[6];
  _info._S[7] = m[7];

  _info._S[8] = m[8];
  _info._S[9] = m[9];
  _info._S[10] = m[10];
  _info._S[11] = m[11];

  _info._S[12] = m[12];
  _info._S[13] = m[13];
  _info._S[14] = m[14];
  _info._S[15] = m[15];

  // Set update flag
  _info._updateS = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetSliceFrequency(double frequency) {

  // Set slice frequency
  _info._sliceFrequency = frequency;

  // Set update flag
  _info._updateSliceFrequency = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRendererShell::SetT(float m[16]) {

  // Set translation matrix
  _info._T[0] = m[0];     
  _info._T[1] = m[1]; 
  _info._T[2] = m[2]; 
  _info._T[3] = m[3]; 

  _info._T[4] = m[4];
  _info._T[5] = m[5];
  _info._T[6] = m[6];
  _info._T[7] = m[7];

  _info._T[8] = m[8];
  _info._T[9] = m[9];
  _info._T[10] = m[10];
  _info._T[11] = m[11];

  _info._T[12] = m[12];
  _info._T[13] = m[13];
  _info._T[14] = m[14];
  _info._T[15] = m[15];

  // Set update flag
  _info._updateT = true;

}

/*--------------------------------------------------------------------------*/
  
void VolumeRendererShell::SetViewport(int x, int y, int w, int h) {

  // Set viewport
  _info._viewport[0] = x;
  _info._viewport[1] = y;
  _info._viewport[2] = w;
  _info._viewport[3] = h;

  // Set update flag
  _info._updateViewport = true;

}

/*--------------------------------------------------------------------------*/

void *VolumeRendererShell::ThreadFunction(void* ptr) {

#ifdef SAGE

  // Swap buffer observer class for SAGE
  class SwapBuffers : public VolumeRendererCommand {

  public:

    // Constructor
    SwapBuffers(int width, int height, 
                Display* display, GLXPbufferSGIX pbuffer,
                char* hostname, int port,
                float left, float right, float bottom, float top,
                int rank, int numberOfNodes, int* level) {

      // Current level
      _level = level;

      // Display variables
      _xDisplay = display;
      _glXPBuffer = pbuffer;      
      _height = height;
      _width = width;

      // Initialize synchronizer
      _synchronizerClient.Init(hostname, port);

      // Setup SAGE viewport
      _sageImageMap.left = left;
      _sageImageMap.right = right;
      _sageImageMap.bottom = bottom;
      _sageImageMap.top = top;

      // SAGE parameters
      _sageConfig.init("VRA.conf");
      _sageConfig.setAppName("VRA");
      _sageConfig.rank = rank;
      _sageConfig.nodeNum = numberOfNodes - 1;
      _sageConfig.resX = _width;
      _sageConfig.resY = _height;
      _sageConfig.imageMap = _sageImageMap;
      _sageConfig.pixFmt = PIXFMT_888;
      _sageConfig.rowOrd = BOTTOM_TO_TOP;
      _sageConfig.master = false;
      _sageConfig.rendering = true;

      // Initialize SAGE
      _sageInf.init(_sageConfig);

      // Check buffer
      _image = NULL;
      if (_image != NULL)
        delete [] _image;

      // Get buffer from SAGE
      _image = (GLubyte*) _sageInf.getBuffer();

    }

    // Destructor
    ~SwapBuffers() {
      
      // Finalize synchronizer
      _synchronizerClient.Finalize();
      
    }

    // Execute callback
    bool Execute(int value) { 

      // Setup readback
      glReadBuffer(GL_FRONT);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

      // Synchronize with other nodes
      _synchronizerClient.Barrier(*_level);

      // Swap buffer
      if (value == 0) {
        glXSwapBuffers(_xDisplay, _glXPBuffer);
        glReadPixels(0, 0, _width, _height, 
                     GL_RGB, GL_UNSIGNED_BYTE, _image);
        _sageInf.swapBuffer();
        _image = (GLubyte*) _sageInf.getBuffer();
        return true;
      }

      // Send current buffer
      else {
        glReadPixels(0, 0, _width, _height, 
                     GL_RGB, GL_UNSIGNED_BYTE, _image);
        _sageInf.swapBuffer();
        _image = (GLubyte*) _sageInf.getBuffer();
        return false;
      }

    }

  private:

    // Current level
    int* _level;

    // Display variables
    Display* _xDisplay;
    GLXPbufferSGIX _glXPBuffer;
    GLubyte* _image;
    int _height;
    int _width;
    
    // SAGE variables
    sageRect _sageImageMap;
    sail _sageInf;
    sailConfig _sageConfig;

    // Synchronizer
    SynchronizerClient _synchronizerClient;

  };

#else

  // Swap buffer observer class for SDL
  class SwapBuffers : public VolumeRendererCommand {

  public:

    // Constructor
    SwapBuffers(char* hostname, int port, int* level) {

      // Current level
      _level = level;

      // Initialize synchronizer
      _synchronizerClient.Init(hostname, port);

    }

    // Destructor
    ~SwapBuffers() {
 
      // Finalize synchronizer
      _synchronizerClient.Finalize();
 
    }

    // Execute callback
    virtual bool Execute(int value) {

      // Synchronizer with other nodes
      _synchronizerClient.Barrier(*_level);

      // Swap buffer
      if (value == 0) {
        SDL_GL_SwapBuffers();
        return true;
      }

      return false;

    }

  private:

    // Current level
    int* _level;

    // Synchronizer
    SynchronizerClient _synchronizerClient;

  };

#endif

  // Progress observer class
  class ProgressUpdate : public VolumeRendererCommand {
  public:
    ProgressUpdate(int* level) {
      _level = level;
    }
    virtual bool Execute(int value) {
      *_level = value;
      return true;
    }
  private:
    int* _level;
  };

  // Abort check observer class
  class AbortCheck : public VolumeRendererCommand {
  public:
    AbortCheck(VolumeRendererThreadInfo* info) {
      _info = info;
    }
    virtual bool Execute(int value) {
      if (_info -> _abortFlag == true) {
        _info -> _abortFlag = false;
        return true;
      }
      return false;
    }
  private:
    VolumeRendererThreadInfo* _info;
  };

  // Current level
  int level = 0;

  // Cast thread info class
  VolumeRendererThreadInfo* info = (VolumeRendererThreadInfo*) ptr;

  // Initialize display
  InitDisplay(info -> _displaySize[0], 
              info -> _displaySize[1],
              info -> _fullScreenFlag,
              info -> _normalCoordinates[0],
              info -> _normalCoordinates[1],
              info -> _normalCoordinates[2],
              info -> _normalCoordinates[3]);

  // Initialize volume renderer
  VolumeRenderer* renderer = new VolumeRenderer;
  renderer -> Init();

  // Abort render observer
  VolumeRendererCommand* ac = new AbortCheck(info);
  renderer -> SetAbortRenderObserver(ac);  

  // Progress observer
  VolumeRendererCommand* pu = new ProgressUpdate(&level);
  renderer -> SetProgressObserver(pu);  

#ifdef SAGE

  // Swap buffer observer
  VolumeRendererCommand* sb = 
    new SwapBuffers(info -> _displaySize[0], 
                    info -> _displaySize[1],
                    _xDisplay, _glXPBuffer,
                    info -> _synchronizationHostname,
                    info -> _synchronizationPort,
                    info -> _normalCoordinates[0],
                    info -> _normalCoordinates[1],
                    info -> _normalCoordinates[2],
                    info -> _normalCoordinates[3],
                    info -> _rank,
                    info -> _numberOfNodes,
                    &level);
  renderer -> SetSwapBuffersObserver(sb);

#else

  // Swap buffer observer
  VolumeRendererCommand* sb = 
    new SwapBuffers(info -> _synchronizationHostname,
                    info -> _synchronizationPort,
                    &level);
  renderer -> SetSwapBuffersObserver(sb);

#endif

  // Get mutex and condition variables
  pthread_mutex_t* conditionMutex = info -> _conditionMutex;
  pthread_cond_t* conditionCond = info -> _conditionCond;

  // Thread loop
  while(info -> _exitFlag == false) {

    // Wait on condition variable
    pthread_mutex_lock(conditionMutex);
    while(info -> _renderFlag == false) {
      pthread_cond_wait(conditionCond, conditionMutex);
    }
    pthread_mutex_unlock(conditionMutex);
 
    // Check exit flag
    if (info -> _exitFlag == true) {
      continue;
    }

    // Check for update axis flag
    if (info -> _updateAxis == true) {
      if (info -> _axisOnFlag == true) {
        renderer -> SetAxisOn();
      }
      else {
        renderer -> SetAxisOff();
      }
      renderer -> SetAxisPosition(info -> _axisPosition[0], 
                                  info -> _axisPosition[1], 
                                  info -> _axisPosition[2]);
      info -> _updateAxis = false;
    }

    // Check for update brick box flag
    if (info -> _updateBrickBox == true) {
      if (info -> _brickBoxOnFlag == true) {
        renderer -> SetBrickBoxOn();
      }
      else {
        renderer -> SetBrickBoxOff();
      }
    }

    // Check for update bounding box flag
    if (info -> _updateBoundingBox == true) {
      if (info -> _boundingBoxOnFlag == true) {
        renderer -> SetBoundingBoxOn();
      }
      else {
        renderer -> SetBoundingBoxOff();
      }
    }

    // Check for update data flag
    if (info -> _updateData == true) {
      renderer -> SetData(info -> _data, info -> _ramSize, info -> _vramSize);
      info -> _updateData = false;
    }

    // Check for update frustum flag
    if (info -> _updateFrustum == true) {
      renderer -> SetFrustum(info -> _frustum[0], info -> _frustum[1],
                             info -> _frustum[2], info -> _frustum[3],
                             info -> _frustum[4], info -> _frustum[5]);
      info -> _updateFrustum = false;
    }

    // Check for update map flag
    if (info -> _updateMap == true) {
      if (info -> _mapType == VOLUME_RENDERER_THREAD_INFO_MAP_8) {
        renderer -> SetMapUnsigned8Int(info -> _map);
      }
      else if (info -> _mapType == VOLUME_RENDERER_THREAD_INFO_MAP_16) {
        renderer -> SetMapUnsigned16Int(info -> _map);
      }
      info -> _updateMap = false;
    }

    // Check for update R flag
    if (info -> _updateR == true) {
      renderer -> SetR(info -> _R);
      info -> _updateR = false;
    }

    // Check for update S flag
    if (info -> _updateS == true) {
      renderer -> SetS(info -> _S);
      info -> _updateS = false;
    }

    // Check for update slice frequency flag
    if (info -> _updateSliceFrequency == true) {
      renderer -> SetSliceFrequency(info -> _sliceFrequency);
      info -> _updateSliceFrequency = false;
    }

    // Check for update T flag
    if (info -> _updateT == true) {
      renderer -> SetT(info -> _T);
      info -> _updateT = false;
    }

    // Check for update viewport flag
    if (info -> _updateViewport == true) {
      renderer -> SetViewport(info -> _viewport[0], info -> _viewport[1],
                              info -> _viewport[2], info -> _viewport[3]);
      info -> _updateViewport = false;
    }

    // Render
    info -> _inRenderFlag = true;
    renderer -> Render();
    info -> _abortFlag = false;
    info -> _inRenderFlag = false;

    // Reset render flag
    info -> _renderFlag = false;

  }

  // Clean up renderer
  delete renderer;

  // Clean up abort render observer
  delete ac;

  // Clean up progress observer
  delete pu;

  // Clean up swap buffer observer
  delete sb;

  // Clean up display
  DestroyDisplay();

}

/*--------------------------------------------------------------------------*/
