/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Copyright (C) 2004 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Illinois at Chicago nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Direct questions, comments etc about SAGE to http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkXOpenGLOffScreenRenderWindow.h,v $
  Language:  C++
  Date:      $Date: 2003/03/26 14:04:59 $
  Version:   $Revision: 1.22 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkXOpenGLOffScreenRenderWindow - OpenGL rendering window
// .SECTION Description
// vtkXOpenGLOffScreenRenderWindow is a concrete implementation of the abstract class
// vtkRenderWindow. vtkOpenGLRenderer interfaces to the OpenGL graphics
// library. Application programmers should normally use vtkRenderWindow
// instead of the OpenGL specific version.

#ifndef __vtkXOpenGLOffScreenRenderWindow_h
#define __vtkXOpenGLOffScreenRenderWindow_h

#include "vtk/vtkOpenGLRenderWindow.h"
#include <X11/Xlib.h> // Needed for X types used in the public interface
#include <X11/Xutil.h> // Needed for X types used in the public interface

class vtkIdList;
class vtkXOpenGLOffScreenRenderWindowInternal;

class VTK_RENDERING_EXPORT vtkXOpenGLOffScreenRenderWindow : public vtkOpenGLRenderWindow
{
public:
  static vtkXOpenGLOffScreenRenderWindow *New();
  vtkTypeRevisionMacro(vtkXOpenGLOffScreenRenderWindow,vtkOpenGLRenderWindow);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Begin the rendering process.
  virtual void Start(void);

  // Description:
  // End the rendering process and display the image.
  virtual void Frame(void);

  // Description:
  // Initialize the window for rendering.
  virtual void WindowInitialize(void);

  // Description:
  // Initialize the rendering window.
  virtual void Initialize(void);

  // Description:
  // Change the window to fill the entire screen.
  virtual void SetFullScreen(int);

  // Description:
  // Resize the window.
  virtual void WindowRemap(void);

  // Description:
  // Set the preferred window size to full screen.
  virtual void PrefFullScreen(void);

  // Description:
  // Specify the size of the rendering window.
  virtual void SetSize(int,int);
  virtual void SetSize(int a[2]) {this->SetSize(a[0], a[1]);};

  // Description:
  // Get the X properties of an ideal rendering window.
  virtual Colormap GetDesiredColormap();
  virtual Visual  *GetDesiredVisual();
  virtual XVisualInfo     *GetDesiredVisualInfo();
  virtual int      GetDesiredDepth();

  // Description:
  // Prescribe that the window be created in a stereo-capable mode. This
  // method must be called before the window is realized. This method
  // overrides the superclass method since this class can actually check
  // whether the window has been realized yet.
  virtual void SetStereoCapableWindow(int capable);

  // Description:
  // Make this window the current OpenGL context.
  void MakeCurrent();

  // Description:
  // If called, allow MakeCurrent() to skip cache-check when called.
  // MakeCurrent() reverts to original behavior of cache-checking
  // on the next render.
  void SetForceMakeCurrent();

  // Description:
  // Get report of capabilities for the render window
  const char *ReportCapabilities();

  // Description:
  // Does this render window support OpenGL? 0-false, 1-true
  int SupportsOpenGL();

  // Description:
  // Is this render window using hardware acceleration? 0-false, 1-true
  int IsDirect();

  // Description:
  // Xwindow get set functions
  virtual void *GetGenericDisplayId() {return (void *)this->GetDisplayId();};
  virtual void *GetGenericWindowId();
  virtual void *GetGenericParentId()  {return (void *)this->ParentId;};
  virtual void *GetGenericContext();
  virtual void *GetGenericDrawable()  {return (void *)this->WindowId;};

  // Description:
  // Get the size of the screen in pixels
  virtual int     *GetScreenSize();

  // Description:
  // Get this RenderWindow's X display id.
  Display *GetDisplayId();

  // Description:
  // Create the default pixmap
  Pixmap pixmap;

  // Description:
  // Set the X display id for this RenderWindow to use to a pre-existing
  // X display id.
  void     SetDisplayId(Display *);
  void     SetDisplayId(void *);

  // Description:
  // Get this RenderWindow's parent X window id.
  Window   GetParentId();

  // Description:
  // Sets the parent of the window that WILL BE created.
  void     SetParentId(Window);
  void     SetParentId(void *);

  // Description:
  // Get this RenderWindow's X window id.
  Window   GetWindowId();

  // Description:
  // Set this RenderWindow's X window id to a pre-existing window.
  void     SetWindowId(Window);
  void     SetWindowId(void *);

  // Description:
  // Specify the X window id to use if a WindowRemap is done.
  void     SetNextWindowId(Window);

  // Description:
  // Set the window id of the new window once a WindowRemap is done.
  // This is the generic prototype as required by the vtkRenderWindow
  // parent.
  void     SetNextWindowId(void *);

  void     SetWindowName(const char *);

  // Description:
  // Hide or Show the mouse cursor, it is nice to be able to hide the
  // default cursor if you want VTK to display a 3D cursor instead.
  void HideCursor();
  void ShowCursor();

  // Description:
  // Change the shape of the cursor
  virtual void SetCurrentCursor(int);

  // Description:
  // Check to see if a mouse button has been pressed.
  // All other events are ignored by this method.
  // This is a useful check to abort a long render.
  virtual  int GetEventPending();

  // Description:
  // Set this RenderWindow's X window id to a pre-existing window.
  void     SetWindowInfo(char *info);

  // Description:
  // Set the window info that will be used after WindowRemap()
  void     SetNextWindowInfo(char *info);

  // Description:
  // Sets the X window id of the window that WILL BE created.
  void     SetParentInfo(char *info);

  // Description:
  // This computes the size of the render window
  // before calling the supper classes render
  void Render();

protected:
  vtkXOpenGLOffScreenRenderWindow();
  ~vtkXOpenGLOffScreenRenderWindow();

  vtkXOpenGLOffScreenRenderWindowInternal *Internal;

  Window   ParentId;
  Window   WindowId;
  Window   NextWindowId;
  Display *DisplayId;
  Colormap ColorMap;
  int      OwnWindow;
  int      OwnDisplay;
  int      ScreenSize[2];
  int      CursorHidden;
  int      ForceMakeCurrent;
  int      UsingHardware;
  char    *Capabilities;

  // we must keep track of the cursors we are using
  Cursor XCArrow;
  Cursor XCSizeAll;
  Cursor XCSizeNS;
  Cursor XCSizeWE;
  Cursor XCSizeNE;
  Cursor XCSizeNW;
  Cursor XCSizeSE;
  Cursor XCSizeSW;

private:
  vtkXOpenGLOffScreenRenderWindow(const vtkXOpenGLOffScreenRenderWindow&);  // Not implemented.
  void operator=(const vtkXOpenGLOffScreenRenderWindow&);  // Not implemented.
};



#endif
