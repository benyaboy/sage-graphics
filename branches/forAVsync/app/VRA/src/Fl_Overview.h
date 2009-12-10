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

#ifndef FL_OVERVIEW_H
#define FL_OVERVIEW_H

/*--------------------------------------------------------------------------*/

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <GL/gl.h>

#include "Matrix.h"
#include "Trackball.h"
#include "Vertex.h"

/*--------------------------------------------------------------------------*/

#define FL_OVERVIEW_DOWN 0
#define FL_OVERVIEW_UP 1

/*--------------------------------------------------------------------------*/

class Fl_Overview : public Fl_Gl_Window {

public:

  // Constructor
  Fl_Overview(int x, int y, int w, int h, const char* l = 0);

  // Destructor
  ~Fl_Overview();

  // Get scale matrix
  void GetScaleMatrix(float m[16]);

  // Get translation matrix
  void GetTranslationMatrix(float m[16]);

  // Reset view
  void ResetView();

  // Set background color
  void SetBackgroundColor(float r, float g, float b, float a);

  // Set frustum
  void SetFrustum(float frustum[6]);

  // Set render callback
  void SetRenderCallback(void (*cb)());

  // Set rotation callback
  void SetRotationCallback(void (*cb)(float* m));

  // Set rotation matrix
  void SetRotationMatrix(float m[16]);

  // Set scale callback
  void SetScaleCallback(void (*cb)(float* m));

  // Set scale matrix
  void SetScaleMatrix(float m[16]);

  // Set translation callback
  void SetTranslationCallback(void (*cb)(float* m));

  // Set translation matrix
  void SetTranslationMatrix(float m[16]);

  // Set overview vertices
  void SetVertices(Vertex vertex[8]);

  // Implementation of draw method
  void draw();

  // Implementation of handle method
  int handle(int event);

private:

  // Background color
  float _backgroundColor[4];

  // Frustum
  float _frustum[6];

  // Line color
  float _lineColor[4];

  // Mouse button state
  int _mouseButtonState[3];

  // Mouse position
  int _mousePosition[2];

  // Rotation matrix
  float _R[16];

  // Scale matrix
  float _S[16];

  // Translation matrix
  float _T[16];

  // Trackball rotation
  Trackball trackball;

  // Update render callback
  void (*_updateRenderCallback)();

  // Update rotation callback
  void (*_updateRotationCallback)(float* m);

  // Update scale callback
  void (*_updateScaleCallback)(float* m);

  // Update translation callback
  void (*_updateTranslationCallback)(float* m);

  // Vertex
  Vertex _vertex[8];

  // World rotate factor
  float _worldRotate[3];

  // World scale factor
  float _worldScale[3];

  // World translation factor
  float _worldTranslate[3];

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
