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

#include "Fl_Transfer_Function.h"

/*--------------------------------------------------------------------------*/

Fl_Transfer_Function::Fl_Transfer_Function(int x, int y, int w, int h, 
                                           const char *l) 
  : Fl_Double_Window(x, y, 640, 220, l) {

  // Create new transfer function widget
  _ctf = new Fl_Color_Transfer_Function(0, 0, 520, 105);
  _ctf -> SetBackgroundColor(0.75, 0.75, 0.75, 1.0);
  _ctf -> SetBoxColor(0.0, 0.0, 0.0, 1.0);
  _ctf -> SetTextColor(0.0, 0.0, 0.0, 1.0);
  _ctf -> SetMoveCallback(StaticCallbackColorMove, this);
  _ctf -> SetSelectCallback(StaticCallbackColorSelect, this);
  _ctf -> SetUnselectCallback(StaticCallbackColorUnselect, this);

  // Create a nice transfer function
  float s[] = {0.0, 0.5, 1.0};
  float r[] = {0.0, 0.0, 1.0};
  float g[] = {0.0, 1.0, 0.0};
  float b[] = {1.0, 0.0, 0.0};
  _ctf -> RestoreControlPoints(3, 1.0, s, r, g, b);

  // Create color scalar input field
  _colorScalarInput = new Fl_Int_Input(565, 5, 70, 20, "Scalar:");
  _colorScalarInput -> labelsize(11);
  _colorScalarInput -> textsize(11);
  _colorScalarInput -> value("");
  _colorScalarInput -> callback(StaticCallbackChangeColorScalar, this);
  _colorScalarInput -> when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
  _colorScalarInput -> deactivate();

  // Create color scalar input field
  _redInput = new Fl_Float_Input(565, 30, 70, 20, "Red:");
  _redInput -> labelsize(11);
  _redInput -> textsize(11);
  _redInput -> value("");
  _redInput -> callback(StaticCallbackChangeRed, this);
  _redInput -> when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
  _redInput -> deactivate();

  // Create color scalar input field
  _greenInput = new Fl_Float_Input(565, 55, 70, 20, "Green:");
  _greenInput -> labelsize(11);
  _greenInput -> textsize(11);
  _greenInput -> value("");
  _greenInput -> callback(StaticCallbackChangeGreen, this);
  _greenInput -> when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
  _greenInput -> deactivate();

  // Create color scalar input field
  _blueInput = new Fl_Float_Input(565, 80, 70, 20, "Blue:");
  _blueInput -> labelsize(11);
  _blueInput -> textsize(11);
  _blueInput -> value("");
  _blueInput -> callback(StaticCallbackChangeBlue, this);
  _blueInput -> when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
  _blueInput -> deactivate();


  // Create new transfer function widget
  _otf = new Fl_Opacity_Transfer_Function(0, 115, 520, 105);
  _otf -> SetBackgroundColor(0.75, 0.75, 0.75, 1.0);
  _otf -> SetBoxColor(0.0, 0.0, 0.0, 1.0);
  _otf -> SetTextColor(0.0, 0.0, 0.0, 1.0);
  _otf -> SetMoveCallback(StaticCallbackOpacityMove, this);
  _otf -> SetSelectCallback(StaticCallbackOpacitySelect, this);
  _otf -> SetUnselectCallback(StaticCallbackOpacityUnselect, this);

  // Create scalar input field
  _scalarInput = new Fl_Int_Input(565, 120, 70, 20, "Scalar:");
  _scalarInput -> labelsize(11);
  _scalarInput -> textsize(11);
  _scalarInput -> value("");
  _scalarInput -> callback(StaticCallbackChangeScalar, this);
  _scalarInput -> when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
  _scalarInput -> deactivate();

  // Create opacity input field
  _opacityInput = new Fl_Float_Input(565, 145, 70, 20, "Opacity:");
  _opacityInput -> labelsize(11);
  _opacityInput -> textsize(11);
  _opacityInput -> value("");
  _opacityInput -> callback(StaticCallbackChangeOpacity, this);
  _opacityInput -> when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
  _opacityInput -> deactivate();

  // Initialize update callback and update callback data
  _updateCallback = NULL;
  _updateCallbackData = NULL;

}

/*--------------------------------------------------------------------------*/

Fl_Transfer_Function::~Fl_Transfer_Function() {
}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackChangeBlue() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float r = 0.0;
  char rString[128];
  float g = 0.0;
  char gString[128];
  float b = 0.0;
  char bString[128];

  // Get selected control point
  index = _ctf -> GetSelectedControlPoint();

  // Modify color
  _ctf -> ModifyControlPointColor(index,
                                  (float) atof(_redInput -> value()),
                                  (float) atof(_greenInput -> value()),
                                  (float) atof(_blueInput -> value()));

  // Get function coordinates
  _ctf -> GetControlPoint(index, &scalar, &r, &g, &b);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(rString, "%.3f", r);
  sprintf(gString, "%.3f", g);
  sprintf(bString, "%.3f", b);

  // Update input fields
  _colorScalarInput -> value(scalarString);
  _redInput -> value(rString);
  _greenInput -> value(gString);
  _blueInput -> value(bString);

  // Redraw widget
  _ctf -> redraw();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackChangeColorScalar() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float r = 0.0;
  char rString[128];
  float g = 0.0;
  char gString[128];
  float b = 0.0;
  char bString[128];

  // Get selected control point
  index = _ctf -> GetSelectedControlPoint();

  // Move control point
  _ctf -> MoveControlPoint(index, (float) atof(_colorScalarInput -> value())); 
                           
  // Get function coordinates
  _ctf -> GetControlPoint(index, &scalar, &r, &g, &b);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(rString, "%.4f", r);
  sprintf(gString, "%.4f", g);
  sprintf(bString, "%.4f", b);

  // Update input fields
  _colorScalarInput -> value(scalarString);
  _redInput -> value(rString);
  _greenInput -> value(gString);
  _blueInput -> value(bString);

  // Redraw widget
  _ctf -> redraw();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackChangeGreen() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float r = 0.0;
  char rString[128];
  float g = 0.0;
  char gString[128];
  float b = 0.0;
  char bString[128];

  // Get selected control point
  index = _ctf -> GetSelectedControlPoint();

  // Modify color
  _ctf -> ModifyControlPointColor(index,
                                  (float) atof(_redInput -> value()),
                                  (float) atof(_greenInput -> value()),
                                  (float) atof(_blueInput -> value()));

  // Get function coordinates
  _ctf -> GetControlPoint(index, &scalar, &r, &g, &b);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(rString, "%.3f", r);
  sprintf(gString, "%.3f", g);
  sprintf(bString, "%.3f", b);

  // Update input fields
  _colorScalarInput -> value(scalarString);
  _redInput -> value(rString);
  _greenInput -> value(gString);
  _blueInput -> value(bString);

  // Redraw widget
  _ctf -> redraw();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackChangeOpacity() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float opacity = 0.0;
  char opacityString[128];

  // Get selected control point
  index = _otf -> GetSelectedControlPoint();

  // Move control point
  _otf -> MoveControlPoint(index, 
                           (float) atof(_scalarInput -> value()), 
                           (float) atof(_opacityInput -> value()));

  // Get function coordinates
  _otf -> GetControlPoint(index, &scalar, &opacity);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(opacityString, "%.3f", opacity);

  // Update input fields
  _scalarInput -> value(scalarString);
  _opacityInput -> value(opacityString);

  // Redraw widget
  _otf -> redraw();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackChangeRed() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float r = 0.0;
  char rString[128];
  float g = 0.0;
  char gString[128];
  float b = 0.0;
  char bString[128];

  // Get selected control point
  index = _ctf -> GetSelectedControlPoint();

  // Modify color
  _ctf -> ModifyControlPointColor(index,
                                  (float) atof(_redInput -> value()),
                                  (float) atof(_greenInput -> value()),
                                  (float) atof(_blueInput -> value()));

  // Get function coordinates
  _ctf -> GetControlPoint(index, &scalar, &r, &g, &b);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(rString, "%.3f", r);
  sprintf(gString, "%.3f", g);
  sprintf(bString, "%.3f", b);

  // Update input fields
  _colorScalarInput -> value(scalarString);
  _redInput -> value(rString);
  _greenInput -> value(gString);
  _blueInput -> value(bString);

  // Redraw widget
  _ctf -> redraw();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackChangeScalar() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float opacity = 0.0;
  char opacityString[128];

  // Get selected control point
  index = _otf -> GetSelectedControlPoint();

  // Move control point
  _otf -> MoveControlPoint(index, 
                           (float) atof(_scalarInput -> value()), 
                           (float) atof(_opacityInput -> value()));

  // Get function coordinates
  _otf -> GetControlPoint(index, &scalar, &opacity);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(opacityString, "%.3f", opacity);

  // Update input fields
  _scalarInput -> value(scalarString);
  _opacityInput -> value(opacityString);

  // Redraw widget
  _otf -> redraw();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackColorMove() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float r = 0.0;
  char rString[128];
  float g = 0.0;
  char gString[128];
  float b = 0.0;
  char bString[128];

  // Get selected control point
  index = _ctf -> GetSelectedControlPoint();

  // Get function coordinates
  _ctf -> GetControlPoint(index, &scalar, &r, &g, &b);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(rString, "%.3f", r);
  sprintf(gString, "%.3f", g);
  sprintf(bString, "%.3f", b);

  // Update input fields
  _colorScalarInput -> value(scalarString);
  _redInput -> value(rString);
  _greenInput -> value(gString);
  _blueInput -> value(bString);

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackColorSelect() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float r = 0.0;
  char rString[128];
  float g = 0.0;
  char gString[128];
  float b = 0.0;
  char bString[128];

  // Get selected control point
  index = _ctf -> GetSelectedControlPoint();

  // Get function coordinates
  _ctf -> GetControlPoint(index, &scalar, &r, &g, &b);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(rString, "%.3f", r);
  sprintf(gString, "%.3f", g);
  sprintf(bString, "%.3f", b);

  // Update input fields
  _colorScalarInput -> value(scalarString);
  _redInput -> value(rString);
  _greenInput -> value(gString);
  _blueInput -> value(bString);

  // Activate input fields
  _colorScalarInput -> activate();
  _redInput -> activate();
  _greenInput -> activate();
  _blueInput -> activate();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackColorUnselect() {

  // Update input fields
  _colorScalarInput -> value("");
  _redInput -> value("");
  _greenInput -> value("");
  _blueInput -> value("");

  // Disable input fields
  _colorScalarInput -> deactivate();
  _redInput -> deactivate();
  _greenInput -> deactivate();
  _blueInput -> deactivate();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackOpacityMove() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float opacity = 0.0;
  char opacityString[128];

  // Get selected control point
  index = _otf -> GetSelectedControlPoint();

  // Get function coordinates
  _otf -> GetControlPoint(index, &scalar, &opacity);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(opacityString, "%.3f", opacity);

  // Update input fields
  _scalarInput -> value(scalarString);
  _opacityInput -> value(opacityString);

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackOpacitySelect() {

  // Temporary variables
  int index = 0;
  float scalar = 0.0;
  char scalarString[128];
  float opacity = 0.0;
  char opacityString[128];

  // Get selected control point
  index = _otf -> GetSelectedControlPoint();

  // Get function coordinates
  _otf -> GetControlPoint(index, &scalar, &opacity);

  // Convert to strings
  sprintf(scalarString, "%d", (int) scalar);
  sprintf(opacityString, "%.3f", opacity);

  // Update input fields
  _scalarInput -> value(scalarString);
  _opacityInput -> value(opacityString);

  // Activate input fields
  _scalarInput -> activate();
  _opacityInput -> activate();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::CallbackOpacityUnselect() {

  // Update input fields
  _scalarInput -> value("");
  _opacityInput -> value("");

  // Disable input fields
  _scalarInput -> deactivate();
  _opacityInput -> deactivate();

  // Update callback
  if (_updateCallback != NULL) {
    _updateCallback(_updateCallbackData);
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::GetColor(float scalar, float* r, 
                                    float* g, float* b) {

  // Get color
  _ctf -> GetColor(scalar, r, g, b);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::GetColorControlPoint(int index, float* scalar, 
                                                float* r, float* g, float* b) {

  // Return color control point
  _ctf -> GetControlPoint(index, scalar, r, g, b);

}

/*--------------------------------------------------------------------------*/

int Fl_Transfer_Function::GetNumberOfColorControlPoints() {

  // Return number of color control points
  return _ctf -> GetNumberOfControlPoints();

}

/*--------------------------------------------------------------------------*/

int Fl_Transfer_Function::GetNumberOfOpacityControlPoints() {

  // Return number of opacity control points
  return _otf -> GetNumberOfControlPoints();

}

/*--------------------------------------------------------------------------*/

float Fl_Transfer_Function::GetOpacity(float scalar) {

  // Return opacity for given value
  return _otf -> GetOpacity(scalar);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::GetOpacityControlPoint(int index, float* scalar, 
                                                  float* opacity) {

  // Get opacity control point
  _otf -> GetControlPoint(index, scalar, opacity);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::GetOpacityRange(float* min, float* max) {

  // Get opacity range
  _otf -> GetOpacityRange(min, max);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::GetScalarRange(float* min, float* max) {

  // Get scalar range: opacity and color function ranges should match
  _otf -> GetScalarRange(min, max);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::Redraw() {

  // Redraw
  _ctf -> redraw();
  _otf -> redraw();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::RestoreColorControlPoints(int number, float max, 
                                                     float* scalar, 
                                                     float* r, float* g, 
                                                     float* b) {

  // Restore color control points
  _ctf -> RestoreControlPoints(number, max, scalar, r, g, b);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::RestoreOpacityControlPoints(int number, float max, 
                                                       float* scalar, 
                                                       float* opacity) {

  // Restore opacity control points
  _otf -> RestoreControlPoints(number, max, scalar, opacity);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetBackgroundColor(float r, float g, 
                                              float b, float a) {

  // Set background color
  _ctf -> SetBackgroundColor(r, g, b, a);
  _otf -> SetBackgroundColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetBoxColor(float r, float g, float b, float a) {

  // Set box color
  _ctf -> SetBoxColor(r, g, b, a);
  _otf -> SetBoxColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetControlPointColor(float r, float g, 
                                                float b, float a) {

  // Set control point color
  _ctf -> SetControlPointColor(r, g, b, a);
  _otf -> SetControlPointColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetControlPointSize(int size) {

  // Set control point size
  _ctf -> SetControlPointSize(size);
  _otf -> SetControlPointSize(size);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetFocusPointColor(float r, float g, 
                                              float b, float a) {

  // Set focus point color
  _ctf -> SetFocusPointColor(r, g, b, a);
  _otf -> SetFocusPointColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetLineColor(float r, float g, float b, float a) {

  // Set line color
  _ctf -> SetLineColor(r, g, b, a);
  _otf -> SetLineColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetMaximumScalar(float max) {

  // Set maximum scalar
  _ctf -> SetMaximumScalar(max);
  _otf -> SetMaximumScalar(max);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetTextColor(float r, float g, float b, float a) {

  // Set text color
  _ctf -> SetTextColor(r, g, b, a);
  _otf -> SetTextColor(r, g, b, a);

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::SetUpdateCallback(void (*cb)(void* p), void* d) {

  // Set update callback and update callback data
  _updateCallback = cb;
  _updateCallbackData = d;

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackChangeBlue(Fl_Widget* w, void *v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackChangeBlue();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackChangeColorScalar(Fl_Widget* w, 
                                                           void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackChangeColorScalar();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackChangeGreen(Fl_Widget* w, void *v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackChangeGreen();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackChangeOpacity(Fl_Widget* w, void *v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackChangeOpacity();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackChangeRed(Fl_Widget* w, void *v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackChangeRed();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackChangeScalar(Fl_Widget* w, void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackChangeScalar();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackColorMove(void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackColorMove();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackColorSelect(void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackColorSelect();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackColorUnselect(void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackColorUnselect();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackOpacityMove(void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackOpacityMove();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackOpacitySelect(void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackOpacitySelect();

}

/*--------------------------------------------------------------------------*/

void Fl_Transfer_Function::StaticCallbackOpacityUnselect(void* v) {

  // Callback
  ((Fl_Transfer_Function*) v) -> CallbackOpacityUnselect();

}

/*--------------------------------------------------------------------------*/
