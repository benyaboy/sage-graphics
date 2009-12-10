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

#ifndef FL_TRANSFER_FUNCTION_H
#define FL_TRANSFER_FUNCTION_H

/*--------------------------------------------------------------------------*/

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <Fl/Fl_Double_Window.H>
#include <Fl/Fl_Float_Input.H>
#include <Fl/Fl_Int_Input.H>

#include "Fl_Color_Transfer_Function.h"
#include "Fl_Opacity_Transfer_Function.h"

/*--------------------------------------------------------------------------*/

class Fl_Transfer_Function : public Fl_Double_Window {

public:

  // Constructor
  Fl_Transfer_Function(int x, int y, int w, int h, const char* l = 0);

  // Destructor
  ~Fl_Transfer_Function();

  // Color blue field changed
  void CallbackChangeBlue();

  // Color scalar field changed
  void CallbackChangeColorScalar();

  // Color green field changed
  void CallbackChangeGreen();

  // Opacity opacity field changed
  void CallbackChangeOpacity();

  // Color red field changed
  void CallbackChangeRed();

  // Opacity scalar field changed
  void CallbackChangeScalar();

  // Color move
  void CallbackColorMove();

  // Color select
  void CallbackColorSelect();

  // Color unselect
  void CallbackColorUnselect();

  // Opacity move
  void CallbackOpacityMove();

  // Opacity select
  void CallbackOpacitySelect();

  // Opacity unselect
  void CallbackOpacityUnselect();

  // Get color for a given scalar value
  void GetColor(float scalar, float* r, float* g, float* b);

  // Get color control point
  void GetColorControlPoint(int index, float* scalar, float* r, 
                            float* g, float* b);

  // Get number of color control points
  int GetNumberOfColorControlPoints();

  // Get number of opacity control points
  int GetNumberOfOpacityControlPoints();

  // Get opacity for a given scalar value
  float GetOpacity(float scalar);

  // Get opacity control point
  void GetOpacityControlPoint(int index, float* scalar, float* opacity);

  // Get opacity range
  void GetOpacityRange(float* min, float* max);

  // Get scalar range
  void GetScalarRange(float* min, float* max);

  // Redraw
  void Redraw();

  // Restore color control points from a list
  void RestoreColorControlPoints(int number, float max, float* scalar, 
                                 float* r, float* g, float* b);

  // Restore opacity control points from a list
  void RestoreOpacityControlPoints(int number, float max, float* scalar, 
                                   float* opacity);

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

  // Set text color
  void SetTextColor(float r, float g, float b, float a);

  // Set update callback
  void SetUpdateCallback(void (*cb)(void* p), void* d);

private:

  // Static blue field changed
  static void StaticCallbackChangeBlue(Fl_Widget* w, void *v);

  // Static color scalar field changed
  static void StaticCallbackChangeColorScalar(Fl_Widget* w, void* v);

  // Static green field changed
  static void StaticCallbackChangeGreen(Fl_Widget* w, void *v);

  // Static opacity opacity field changed
  static void StaticCallbackChangeOpacity(Fl_Widget* w, void *v);

  // Static red field changed
  static void StaticCallbackChangeRed(Fl_Widget* w, void *v);

  // Static opacity scalar field changed
  static void StaticCallbackChangeScalar(Fl_Widget* w, void* v);

  // Static color move 
  static void StaticCallbackColorMove(void* v);

  // Static color select
  static void StaticCallbackColorSelect(void* v);

  // Static color unselect
  static void StaticCallbackColorUnselect(void* f);

  // Static opacity move 
  static void StaticCallbackOpacityMove(void* v);

  // Static opacity select
  static void StaticCallbackOpacitySelect(void* v);

  // Static opacity unselect
  static void StaticCallbackOpacityUnselect(void* f);

  // Color transfer function
  Fl_Color_Transfer_Function* _ctf;

  // Scalar input field
  Fl_Int_Input* _colorScalarInput;

  // Red input field
  Fl_Float_Input* _redInput;

  // Green input field
  Fl_Float_Input* _greenInput;

  // Blue input field
  Fl_Float_Input* _blueInput;

  // Opacity transfer function
  Fl_Opacity_Transfer_Function* _otf;

  // Scalar input field
  Fl_Int_Input* _scalarInput;

  // Opacity input field
  Fl_Float_Input* _opacityInput;

  // Update callback
  void (*_updateCallback)(void* p);

  // Update callback data
  void* _updateCallbackData;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
