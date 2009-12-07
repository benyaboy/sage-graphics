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

#include "Fl_Color_Transfer_Function.h"

/*--------------------------------------------------------------------------*/

Fl_Color_Transfer_Function::Fl_Color_Transfer_Function(int x, int y, 
                                                       int w, int h, 
                                                       const char *l) 
  : Fl_Gl_Window(x, y, w, h, l) {

  // Initialize callbacks
  _callbackMove = NULL;
  _callbackSelect = NULL;
  _callbackUnselect = NULL;

  // Initialize callback data
  _callbackMovePtr = NULL;
  _callbackSelectPtr = NULL;
  _callbackUnselectPtr = NULL;

  // Initialize control point picked
  _controlPointPicked = -1;

  // Create new transfer function widget
  _ctfw = new ColorTransferFunctionWidget(w, h);

}

/*--------------------------------------------------------------------------*/

Fl_Color_Transfer_Function::~Fl_Color_Transfer_Function() {

  // Clean up
  delete _ctfw;

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::GetControlPoint(int index, float* scalar, 
                                                 float* r, float* g, float* b) {

  // Get control point
  _ctfw -> GetControlPoint(index, scalar, r, g, b);

}

/*--------------------------------------------------------------------------*/

int Fl_Color_Transfer_Function::GetControlPointIndex(int px, int py) {

  // Get control point index
  return _ctfw -> GetControlPointIndex(px, py);

}

/*--------------------------------------------------------------------------*/

int Fl_Color_Transfer_Function::GetControlPointIndex(float scalar) {

  // Get control point index
  return _ctfw -> GetControlPointIndex(scalar);

}

/*--------------------------------------------------------------------------*/

bool Fl_Color_Transfer_Function::GetFunctionCoordinates(int px, int py, 
                                                        float* scalar, float* r,
                                                        float* g, float* b) {

  // Get function coordinates
  return _ctfw -> GetFunctionCoordinates(px, py, scalar, r, g, b);

}

/*--------------------------------------------------------------------------*/

int Fl_Color_Transfer_Function::GetNumberOfControlPoints() {

  // Get number of control points
  return _ctfw -> GetNumberOfControlPoints();

}

/*--------------------------------------------------------------------------*/

int Fl_Color_Transfer_Function::GetSelectedControlPoint() {

  // Return slected control point
  return _controlPointPicked;

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::GetColor(float scalar, float* r, float* g,
                                           float* b) {

  // Get color
  _ctfw -> GetColor(scalar, r, g, b);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::GetScalarRange(float* min, float* max) {

  // Get scalar range
  _ctfw -> GetScalarRange(min, max);

}

/*--------------------------------------------------------------------------*/

bool Fl_Color_Transfer_Function::ModifyControlPointColor(int index, float r, 
                                                         float g, float b) {

  // Modify control point color
  return _ctfw -> ModifyControlPointColor(index, r, g, b);

}

/*--------------------------------------------------------------------------*/

bool Fl_Color_Transfer_Function::MoveControlPoint(int index, int px, int py) {

  // Move control point
  return _ctfw -> MoveControlPoint(index, px, py);

}

/*--------------------------------------------------------------------------*/

bool Fl_Color_Transfer_Function::MoveControlPoint(int index, float scalar) {

  // Move control point
  return _ctfw -> MoveControlPoint(index, scalar);

}

/*--------------------------------------------------------------------------*/

bool Fl_Color_Transfer_Function::RemoveControlPoint(int index) {

  // Remove control point
  return _ctfw -> RemoveControlPoint(index);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::RestoreControlPoints(int number, 
                                                      float max, 
                                                      float* scalar, 
                                                      float* r,
                                                      float* g,
                                                      float* b) {

  // Restore control points
  _ctfw -> RestoreControlPoints(number, max, scalar, r, g, b);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetBackgroundColor(float r, float g, 
                                                    float b, float a) {

  // Set background color
  _ctfw -> SetBackgroundColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetBoxColor(float r, float g, 
                                             float b, float a) {

  // Set box color
  _ctfw -> SetBoxColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetControlPointColor(float r, float g, 
                                                      float b, float a) {

  // Set control point color
  _ctfw -> SetControlPointColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetControlPointSize(int size) {

  // Set control point size
  _ctfw -> SetControlPointSize(size);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetFocusPointColor(float r, float g, 
                                                    float b, float a) {

  // Set focus point color
  _ctfw -> SetFocusPointColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetLineColor(float r, float g, 
                                              float b, float a) {

  // Set line color
  _ctfw -> SetLineColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetMaximumScalar(float max) {

  // Set maximum scalar
  _ctfw -> SetMaximumScalar(max);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetMoveCallback(void (*cb)(void*), 
                                                 void* p) {

  // Set callback
  _callbackMove = cb;

  // Set pointer
  _callbackMovePtr = p;

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetSelectCallback(void (*cb)(void*), 
                                                   void* p) {

  // Set callback
  _callbackSelect = cb;

  // Set pointer
  _callbackSelectPtr = p;

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetTextColor(float r, float g, 
                                              float b, float a) {

  // Set text color
  _ctfw -> SetTextColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::SetUnselectCallback(void (*cb)(void*),
                                                     void* p) {

  // Set callback
  _callbackUnselect = cb;

  // Set pointer
  _callbackUnselectPtr = p;

}

/*--------------------------------------------------------------------------*/

void Fl_Color_Transfer_Function::draw() {

  // Set dimensions
  _ctfw -> SetDimensions(w(), h());

  // Draw widget
  _ctfw -> Draw();

}

/*--------------------------------------------------------------------------*/

int Fl_Color_Transfer_Function::handle(int event) {

  // Check event
  switch(event) {


  case FL_PUSH: {

    // Get button
    int button = Fl::event_button();

    // Check select button
    if (button == 1) {
      _mouseButtonState[0] = DOWN;

      // Get point picked
      _controlPointPicked = 
        _ctfw -> GetControlPointIndex(Fl::event_x(), Fl::event_y());

      // Check callback
      if (_callbackSelect != NULL && _callbackUnselect != NULL) {

        // Unselect if no point picked
        if (_controlPointPicked == -1) {
          _callbackUnselect(_callbackUnselectPtr);
        }

        // Select if point picked
        else {
          _callbackSelect(_callbackSelectPtr);
        }

      }

    }

    // Check remove button
    else if (button == 2) {
      _mouseButtonState[1] = DOWN;

      // Get point index
      _controlPointPicked = 
        _ctfw -> GetControlPointIndex(Fl::event_x(), Fl::event_y());

      // Check callback
      if (_callbackSelect != NULL && _callbackUnselect != NULL) {

        // Unselect if point not removed
        if (_ctfw -> RemoveControlPoint(_controlPointPicked) == true ||
            _controlPointPicked == -1) {
          _callbackUnselect(_callbackUnselectPtr);
        }

        // Select if no point removed
        else {
          _callbackSelect(_callbackSelectPtr);
        }

      }

    }

    // Check add button
    else if (button == 3) {
      _mouseButtonState[2] = DOWN;

      // Check that a point was added
      if (_ctfw -> AddControlPoint(Fl::event_x(), Fl::event_y()) == true) {

        // Get scalar value for point added
        float s, r, g, b;
        GetFunctionCoordinates(Fl::event_x(), Fl::event_y(), &s, &r, &g, &b);

        // Get point index for point added
        _controlPointPicked = _ctfw -> GetControlPointIndex(s);

        // Check callback
        if (_callbackSelect != NULL) {
          _callbackSelect(_callbackSelectPtr);
        }

      }

    }

    // Draw scene
    redraw();

    // Swap buffers
    swap_buffers();

    // Return
    return 1;

  }


  case FL_DRAG: {

    // Move the picked control point
    if (_mouseButtonState[0] == DOWN) {
      _ctfw -> MoveControlPoint(_controlPointPicked, 
                                Fl::event_x(), Fl::event_y());

      // Check callback
      if (_callbackMove != NULL) {
        if (_controlPointPicked != -1) {
          _callbackMove(_callbackMovePtr);
        }
      }

    }

    // Draw scene
    redraw();

    // Swap buffers
    swap_buffers();

    // Return
    return 1;

  }


  case FL_RELEASE: {

    // Get button
    int button = Fl::event_button();
    
    // Check which button
    if (button == 1) {
      _mouseButtonState[0] = UP;
      //_controlPointPicked = -1;
    }

    // Draw scene
    redraw();

    // Swap buffers
    swap_buffers();

    // Return
    return 1;

  }


  default: {

    // Pass other events to the base class
    return Fl_Gl_Window::handle(event);

  }

  }

}

/*--------------------------------------------------------------------------*/
