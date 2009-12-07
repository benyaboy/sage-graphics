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

#ifndef VOLUME_RENDERER_SHELL_H
#define VOLUME_RENDERER_SHELL_H

/*--------------------------------------------------------------------------*/

#ifdef SAGE
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <sail.h>
#include <misc.h>
#endif

#include <pthread.h>
#include <SDL.h>

#include "Octree.h"
#include "VolumeRenderer.h"
#include "VolumeRendererThreadInfo.h"

/*--------------------------------------------------------------------------*/

class VolumeRendererShell {

public:

  // Default constructor
  VolumeRendererShell();

  // Default destructor
  ~VolumeRendererShell();

  // Abort render
  void AbortRender(bool flag);

  // Destroy pbuffer
  static void DestroyDisplay();

#ifdef SAGE

  // X error handler
  static int HandleXError(Display* display, XErrorEvent *event);

#endif

  // Initialize
  void Init(int width, int height, bool fullScreen, 
            char* hostname, int port, 
            float left, float right, 
            float bottom, float top,
            int rank, int numberOfNodes);

  // Initialize display
  static void InitDisplay(int width, int height, bool fullScreen, 
                          float left, float right, 
                          float bottom, float top);

  // Is a render in progress
  bool InRender();
  
  // User must call Init before any other method
  bool IsInitialized();
  
  // Render
  void Render();

  // Set axis off
  void SetAxisOff();

  // Set axis on
  void SetAxisOn();

  // Set axis position
  void SetAxisPosition(float x, float y, float z);

  // Set brick box off
  void SetBrickBoxOff();

  // Set brick box on
  void SetBrickBoxOn();

  // Set bounding box off
  void SetBoundingBoxOff();

  // Set bounding box on
  void SetBoundingBoxOn();

  // Set data
  void SetData(Octree* data);

  // Set data
  void SetData(Octree* data, int ramSize, int vramSize);

  // Set exit flag
  void SetExitFlag();
  
  // Set frustum
  void SetFrustum(float left, float right, float bottom,
                  float top, float near, float far);

  // Set color and opacity map
  void SetMap(unsigned char* map);

  // Set 8bit color and opacity map
  void SetMap8(unsigned char* map);

  // Set 16bit color and opacity map
  void SetMap16(unsigned char* map);

  // Set rotation matrix
  void SetR(float m[16]);
  
  // Set scale matrix
  void SetS(float m[16]);

  // Set slice frequency
  void SetSliceFrequency(double frequency);
  
  // Set translation matrix
  void SetT(float m[16]);
  
  // Set viewport
  void SetViewport(int x, int y, int w, int h);

  // Thread function
  static void *ThreadFunction(void* ptr);

private:

  // Condition variable
  pthread_cond_t _conditionCond;
  
  // Condition variable mutex
  pthread_mutex_t _conditionMutex;

  // State for thread
  VolumeRendererThreadInfo _info;

  // User must call Init before any other method
  bool _isInitialized;

  // Rendering thread
  pthread_t _thread;

#ifdef SAGE

  // Declare static variable for glXPBuffer
  static GLXPbufferSGIX _glXPBuffer;
  
  // Declare static variable for X display
  static Display* _xDisplay;

  // Declare static variable for X error flag
  static int _xErrorFlag;

  // Declare static variable for X visual info
  static XVisualInfo* _xVisualInfo;

#endif

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
