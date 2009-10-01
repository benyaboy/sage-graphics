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

#include "ControlPoint.h"

#include <math.h>
#include <FL/glut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include <vector.h>

/*--------------------------------------------------------------------------*/

class OpacityTransferFunctionWidget {

public:

  // Constructor specifying window width and height
  OpacityTransferFunctionWidget(int w, int h);

  // Destructor
  ~OpacityTransferFunctionWidget();

  // Add control point based on corresponding pixel coordinate, if valid
  bool AddControlPoint(int px, int py);

  // Draw
  void Draw();

  // Get control point
  void GetControlPoint(int index, float* scalar, float* opacity);

  // Get index of control point that lies at given pixel position, or -1
  int GetControlPointIndex(int px, int py);

  // Get index of control point that lies at given function coordinates, or -1
  int GetControlPointIndex(float scalar, float opacity);

  // Get function coordinates at given pixel coordinates, if possible
  bool GetFunctionCoordinates(int px, int py, float* scalar, float* opacity);

  // Get number of control points
  int GetNumberOfControlPoints();

  // Get opacity for a given scalar value
  float GetOpacity(float scalar);

  // Get opacity range
  void GetOpacityRange(float* min, float* max);

  // Get scalar range
  void GetScalarRange(float* min, float* max);
  
  // Move control point to corresponding pixel coordinate, if possible
  bool MoveControlPoint(int index, int px, int py);

  // Move control point to corresponding function coordinate, if possible
  bool MoveControlPoint(int index, float scalar, float opacity);

  // Restore control points from a list
  void RestoreControlPoints(int number, float max, 
                            float* scalar, float* opacity);

  // Remove control point
  bool RemoveControlPoint(int index);

  // Set background color
  void SetBackgroundColor(float r, float g, float b, float a);

  // Set box color
  void SetBoxColor(float r, float g, float b, float a);

  // Set control point color
  void SetControlPointColor(float r, float g, float b, float a);

  // Set control point size
  void SetControlPointSize(int size);

  // Set window dimensions
  void SetDimensions(int w, int h);

  // Set focus point color
  void SetFocusPointColor(float r, float g, float b, float a);

  // Set line color
  void SetLineColor(float r, float g, float b, float a);

  // Set maximum scalar value
  void SetMaximumScalar(float max);

  // Set text color
  void SetTextColor(float r, float g, float b, float a);

private:

  // Convert box coordinates to function coordinates
  void BoxToFunction(float bx, float by, float* scalar, float* opacity);
  
  // Convert box coordinates to world coordinates
  void BoxToWorld(float bx, float by, float* wx, float* wy);

  // Convert function coordinates to box coordinates
  void FunctionToBox(float scalar, float opacity, float* bx, float* by);
  
  // Is box coordinate valid in world space
  bool IsBoxInWorld(float bx, float by);

  // Is function coordinate valid in box space
  bool IsFunctionInBox(float scalar, float opacity);

  // Is pixel coordinate valid in world space
  bool IsPixelInWorld(int px, int py);

  // Is world coordinate valid in box space
  bool IsWorldInBox(float wx, float wy);

  // Convert pixel coordinates to world coordinates
  void PixelToWorld(int px, int py, float* wx, float* wy);

  // Convert world coordinates to box coordinates
  void WorldToBox(float wx, float wy, float* bx, float* by);

  // Background color
  float _backgroundColor[4];

  // Box bounds in world coordinates
  float _boxBound[4];

  // Box color
  float _boxColor[4];

  // Control points
  vector <ControlPoint> _controlPoint;

  // Control point color
  float _controlPointColor[4];

  // Control point size
  int _controlPointSize;

  // Focus point index
  int _focusPoint;

  // Focus point color
  float _focusPointColor[4];

  // Frustum
  float _frustum[6];

  // Line color
  float _lineColor[4];

  // Number of control points
  int _numberOfControlPoints;

  // Opacity range
  float _opacityMaximum;
  float _opacityMinimum;
  
  // Window size
  int _pixelDimensions[2];

  // Scalar range
  float _scalarMaximum;
  float _scalarMinimum;

  // Text color
  float _textColor[4];

};

/*--------------------------------------------------------------------------*/
