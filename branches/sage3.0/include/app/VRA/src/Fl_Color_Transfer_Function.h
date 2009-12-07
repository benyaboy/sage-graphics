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

#ifndef FL_COLOR_TRANSFER_FUNCTION_H
#define FL_COLOR_TRANSFER_FUNCTION_H

/*--------------------------------------------------------------------------*/

#include <FL/Fl.H>
#include <Fl/Fl_Gl_Window.H>
#include <GL/gl.h>

#include "ColorTransferFunctionWidget.h"

/*--------------------------------------------------------------------------*/

#define DOWN 1
#define UP 0

/*--------------------------------------------------------------------------*/

class Fl_Color_Transfer_Function : public Fl_Gl_Window {

public:

  // Constructor
  Fl_Color_Transfer_Function(int x, int y, int w, int h, const char* l = 0);

  // Destructor
  ~Fl_Color_Transfer_Function();

  // Get control point
  void GetControlPoint(int index, float* scalar, float* r, float* g, float* b);

  // Get index of control point that lies at given pixel position, or -1
  int GetControlPointIndex(int px, int py);

  // Get index of control point that lies at given function coordinates, or -1
  int GetControlPointIndex(float scalar);

  // Get function coordinates at given pixel coordinates, if possible
  bool GetFunctionCoordinates(int px, int py, float* scalar, float* r, 
                              float* g, float* b);

  // Get number of control points
  int GetNumberOfControlPoints();

  // Get color for a given scalar value
  void GetColor(float scalar, float* r, float* g, float* b);

  // Get index of selected control point
  int GetSelectedControlPoint();

  // Get scalar range
  void GetScalarRange(float* min, float* max);

  // Modify control point's color, if possible
  bool ModifyControlPointColor(int index, float r, float g, float b);

  // Move control point to corresponding pixel coordinate, if possible
  bool MoveControlPoint(int index, int px, int py);

  // Move control point to corresponding function coordinate, if possible
  bool MoveControlPoint(int index, float scalar);

  // Remove control point
  bool RemoveControlPoint(int index);

  // Restore control points from a list
  void RestoreControlPoints(int number, float max, float* scalar, 
                            float* r, float* g, float* b);

  // Set background color
  void SetBackgroundColor(float r, float g, float b, float a);

  // Set box color
  void SetBoxColor(float r, float g, float b, float a);

  // Set control point color
  void SetControlPointColor(float r, float g, float b, float a);

  // Set control point size
  void SetControlPointSize(int size);

  // Set focus point color
  void SetFocusPointColor(float r, float g, float b, float a);

  // Set line color
  void SetLineColor(float r, float g, float b, float a);

  // Set maximum scalar value
  void SetMaximumScalar(float max);

  // Set move callback: called when a control point is moved
  void SetMoveCallback(void (*cb)(void*), void* p);

  // Set select callback: called when a control point is selected
  void SetSelectCallback(void (*cb)(void*), void* p);

  // Set text color
  void SetTextColor(float r, float g, float b, float a);

  // Set unselect callback: called when a control point is unselected
  void SetUnselectCallback(void (*cb)(void*), void* p);

  // Implementation of draw method
  void draw();

  // Implementation of handle method
  int handle(int event);

private:

  // Move callback
  void (*_callbackMove)(void*);

  // Move pointer
  void* _callbackMovePtr;

  // Select callback
  void (*_callbackSelect)(void*);

  // Select pointer
  void* _callbackSelectPtr;

  // Unselect callback
  void (*_callbackUnselect)(void*);

  // Unselect pointer
  void* _callbackUnselectPtr;

  // Control point picked
  int _controlPointPicked;

  // Mouse button states
  int _mouseButtonState[3];

  // Opacity transferfunction widget
  ColorTransferFunctionWidget* _ctfw;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
