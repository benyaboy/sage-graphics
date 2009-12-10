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

#include "Fl_Opacity_Transfer_Function.h"

/*--------------------------------------------------------------------------*/

Fl_Opacity_Transfer_Function::Fl_Opacity_Transfer_Function(int x, int y, 
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
  _otfw = new OpacityTransferFunctionWidget(w, h);

}

/*--------------------------------------------------------------------------*/

Fl_Opacity_Transfer_Function::~Fl_Opacity_Transfer_Function() {

  // Clean up
  delete _otfw;

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::GetControlPoint(int index, 
                                                   float* scalar, 
                                                   float* opacity) {

  // Get control point
  _otfw -> GetControlPoint(index, scalar, opacity);

}

/*--------------------------------------------------------------------------*/

int  Fl_Opacity_Transfer_Function::GetControlPointIndex(int px, int py) {

  // Get control point index
  return _otfw -> GetControlPointIndex(px, py);

}

/*--------------------------------------------------------------------------*/

int  Fl_Opacity_Transfer_Function::GetControlPointIndex(float scalar, 
                                                        float opacity) {

  // Get control point index
  return _otfw -> GetControlPointIndex(scalar, opacity);

}

/*--------------------------------------------------------------------------*/

bool Fl_Opacity_Transfer_Function::GetFunctionCoordinates(int px, int py, 
                                                          float* scalar, 
                                                          float* opacity) {

  // Get function coordinates
  return _otfw -> GetFunctionCoordinates(px, py, scalar, opacity);

}

/*--------------------------------------------------------------------------*/

int Fl_Opacity_Transfer_Function::GetNumberOfControlPoints() {

  // Get number of control points
  return _otfw -> GetNumberOfControlPoints();

}

/*--------------------------------------------------------------------------*/

int Fl_Opacity_Transfer_Function::GetSelectedControlPoint() {

  // Return slected control point
  return _controlPointPicked;

}

/*--------------------------------------------------------------------------*/

float Fl_Opacity_Transfer_Function::GetOpacity(float scalar) {

  // Get opacity
  return _otfw -> GetOpacity(scalar);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::GetOpacityRange(float* min, float* max) {

  // Get opacity range
  _otfw -> GetOpacityRange(min, max);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::GetScalarRange(float* min, float* max) {

  // Get scalar range
  _otfw -> GetScalarRange(min, max);

}

/*--------------------------------------------------------------------------*/

bool Fl_Opacity_Transfer_Function::MoveControlPoint(int index, int px, int py) {

  // Move control point
  return _otfw -> MoveControlPoint(index, px, py);

}

/*--------------------------------------------------------------------------*/

bool Fl_Opacity_Transfer_Function::MoveControlPoint(int index, 
                                                    float scalar, 
                                                    float opacity) {

  // Move control point
  return _otfw -> MoveControlPoint(index, scalar, opacity);

}

/*--------------------------------------------------------------------------*/

bool Fl_Opacity_Transfer_Function::RemoveControlPoint(int index) {

  // Remove control point
  return _otfw -> RemoveControlPoint(index);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::RestoreControlPoints(int number, 
                                                        float max, 
                                                        float* scalar, 
                                                        float* opacity) {

  // Restore control points
  _otfw -> RestoreControlPoints(number, max, scalar, opacity);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetBackgroundColor(float r, float g, 
                                                      float b, float a) {

  // Set background color
  _otfw -> SetBackgroundColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetBoxColor(float r, float g, 
                                               float b, float a) {

  // Set box color
  _otfw -> SetBoxColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetControlPointColor(float r, float g, 
                                                        float b, float a) {

  // Set control point color
  _otfw -> SetControlPointColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetControlPointSize(int size) {

  // Set control point size
  _otfw -> SetControlPointSize(size);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetFocusPointColor(float r, float g, 
                                                      float b, float a) {

  // Set focus point color
  _otfw -> SetFocusPointColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetLineColor(float r, float g, 
                                                float b, float a) {

  // Set line color
  _otfw -> SetLineColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetMaximumScalar(float max) {

  // Set maximum scalar
  _otfw -> SetMaximumScalar(max);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetMoveCallback(void (*cb)(void*), 
                                                   void* p) {

  // Set callback
  _callbackMove = cb;

  // Set pointer
  _callbackMovePtr = p;

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetSelectCallback(void (*cb)(void*), 
                                                     void* p) {

  // Set callback
  _callbackSelect = cb;

  // Set pointer
  _callbackSelectPtr = p;

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetTextColor(float r, float g, 
                                                float b, float a) {

  // Set text color
  _otfw -> SetTextColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::SetUnselectCallback(void (*cb)(void*),
                                                       void* p) {

  // Set callback
  _callbackUnselect = cb;

  // Set pointer
  _callbackUnselectPtr = p;

}

/*--------------------------------------------------------------------------*/

void Fl_Opacity_Transfer_Function::draw() {

  // Set dimensions
  _otfw -> SetDimensions(w(), h());

  // Draw widget
  _otfw -> Draw();

}

/*--------------------------------------------------------------------------*/

int Fl_Opacity_Transfer_Function::handle(int event) {

  // Check event
  switch(event) {


  case FL_PUSH: {

    // Get button
    int button = Fl::event_button();

    // Check select button
    if (button == 1) {
      _mouseButtonState[0] = DOWN;
      _controlPointPicked = 
        _otfw -> GetControlPointIndex(Fl::event_x(), Fl::event_y());

      // Check callback
      if (_callbackSelect != NULL && _callbackUnselect != NULL) {
        if (_controlPointPicked == -1) {
          _callbackUnselect(_callbackUnselectPtr);
        }
        else {
          _callbackSelect(_callbackSelectPtr);
        }
      }

    }

    // Check remove button
    else if (button == 2) {
      _mouseButtonState[1] = DOWN;
      _controlPointPicked = 
        _otfw -> GetControlPointIndex(Fl::event_x(), Fl::event_y());
      if (_callbackSelect != NULL && _callbackUnselect != NULL) {
        if (_otfw -> RemoveControlPoint(_controlPointPicked) == true ||
            _controlPointPicked == -1) {
          _callbackUnselect(_callbackUnselectPtr);
        }
        else {
          _callbackSelect(_callbackSelectPtr);
        }
      }
    }

    // Check add button
    else if (button == 3) {
      _mouseButtonState[2] = DOWN;
      if (_otfw -> AddControlPoint(Fl::event_x(), Fl::event_y()) == true) {
        _controlPointPicked = 
          _otfw -> GetControlPointIndex(Fl::event_x(), Fl::event_y());
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
      _otfw -> MoveControlPoint(_controlPointPicked, 
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
