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

#include "OpacityTransferFunctionWidget.h"

/*--------------------------------------------------------------------------*/

OpacityTransferFunctionWidget::OpacityTransferFunctionWidget(int w, int h) {

  // Initialize background color
  _backgroundColor[0] = 0.0;
  _backgroundColor[1] = 0.0;
  _backgroundColor[2] = 0.0;
  _backgroundColor[3] = 0.0;

  // Initialize box bound
  _boxBound[0] = 0.0;
  _boxBound[1] = 0.0;
  _boxBound[2] = 0.0;
  _boxBound[3] = 0.0;

  // Initialize box color
  _boxColor[0] = 1.0;
  _boxColor[1] = 1.0;
  _boxColor[2] = 1.0;
  _boxColor[3] = 1.0;

  // Initialize control point color
  _controlPointColor[0] = 1.0;
  _controlPointColor[1] = 1.0;
  _controlPointColor[2] = 1.0;
  _controlPointColor[3] = 1.0;

  // Initialize control point size
  _controlPointSize = 8;

  // Initialize focus point index
  _focusPoint = -1;

  // Initialize focus point color
  _focusPointColor[0] = 0.0;
  _focusPointColor[1] = 0.0;
  _focusPointColor[2] = 0.0;
  _focusPointColor[3] = 1.0;

  // Initialize frustum
  _frustum[0] = 0.0;
  _frustum[1] = 1.0;
  _frustum[2] = 0.0;
  _frustum[3] = 1.0;
  _frustum[4] = -1.0;
  _frustum[5] = 1.0;

  // Initialize line color
  _lineColor[0] = 1.0;
  _lineColor[1] = 1.0;  
  _lineColor[2] = 1.0;
  _lineColor[3] = 1.0;

  // Initialize number of control points
  _numberOfControlPoints = 0;

  // Initialize opacity range
  _opacityMaximum = 1.0;
  _opacityMinimum = 0.0;

  // Initialize size of window
  _pixelDimensions[0] = w;
  _pixelDimensions[1] = h;

  // Initialize scalar range
  _scalarMaximum = 1.0;
  _scalarMinimum = 0.0;

  // Initialize text color
  _textColor[0] = 1.0;
  _textColor[1] = 1.0;
  _textColor[2] = 1.0;
  _textColor[3] = 1.0;

  // Initialize transfer function
  ControlPoint controlPoint;

  // First control point
  controlPoint.SetScalar(0.0);
  controlPoint.SetOpacity(0.0);
  _controlPoint.push_back(controlPoint);

  // Second control point
  controlPoint.SetScalar(1.0);
  controlPoint.SetOpacity(1.0);
  _controlPoint.push_back(controlPoint);

  // Increment number of control points
  _numberOfControlPoints = 2;

}

/*--------------------------------------------------------------------------*/

OpacityTransferFunctionWidget::~OpacityTransferFunctionWidget() {
}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::AddControlPoint(int px, int py) {

  // Box coordinates
  float bx = 0.0;
  float by = 0.0;

  // Control point
  ControlPoint controlPoint;

  // Function coordinates
  float scalar = 0.0;
  float opacity = 0.0;

  // World coordinates
  float wx = 0.0;
  float wy = 0.0;

  // Check that pixel coordinates are within world space
  if (IsPixelInWorld(px, py) == true) {

    // Get corresponding world coordinates
    PixelToWorld(px, py, &wx, &wy);

    // Check that world coordinates are within box space
    if (IsWorldInBox(wx, wy) == true) {

      // Get corresponding box coordinates
      WorldToBox(wx, wy, &bx, &by);

      // Get corresponding function coordinates
      BoxToFunction(bx, by, &scalar, &opacity);

      // Set control point
      controlPoint.SetOpacity(opacity);
      controlPoint.SetScalar(scalar);

      // Add control point
      _controlPoint.push_back(controlPoint);

      // Sort control points
      sort(_controlPoint.begin(), _controlPoint.end());

      // Get index of newly added point
      _focusPoint = GetControlPointIndex(scalar, opacity);

      // Increment number of control points
      _numberOfControlPoints++;

      // Control point added
      return true;

    }

  }

  // Set focus point
  _focusPoint = -1;

  // Did not add control point
  return false;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::BoxToFunction(float bx, 
                                                  float by,
                                                  float* scalar, 
                                                  float* opacity) {

  // Convert from box coordinates to function coordinates
  *scalar = ((bx * (_scalarMaximum - _scalarMinimum)) + _scalarMinimum);
  *opacity = ((by * (_opacityMaximum - _opacityMinimum)) + _opacityMinimum);

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::BoxToWorld(float bx, float by, 
                                               float* wx, float* wy) {

  // Convert from box coordinates to world coordinates
  *wx = (bx * (_boxBound[1] - _boxBound[0])) + _boxBound[0];
  *wy = (by * (_boxBound[3] - _boxBound[2])) + _boxBound[2];

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::Draw() {

  // Clear color
  glClearColor(_backgroundColor[0], _backgroundColor[1],
               _backgroundColor[2], _backgroundColor[3]);

  // Clear bufffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Set viewport
  glViewport(0, 0, _pixelDimensions[0], _pixelDimensions[1]);

  // Set projection mode
  glMatrixMode(GL_PROJECTION);

  // Load identity
  glLoadIdentity();

  // Set frustum
  glOrtho(_frustum[0], _frustum[1], 
          _frustum[2], _frustum[3], 
          _frustum[4], _frustum[5]);

  // Set modelview mode
  glMatrixMode(GL_MODELVIEW);


  // Text color
  glColor4f(_textColor[0], _textColor[1], _textColor[2], _textColor[3]);

  // Text border in pixels
  int textBorder = 5;

  // Text height in pixels
  int textHeight = 10;


  /*
  // Minimum opacity

  // Calculate offsets for text
  float opacityMinimumHorizontalOffset = 
    ((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    ((float) textBorder);
  float opacityMinimumVerticalOffset = 
    ((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
    ((float) textBorder + (float) textHeight + (float) textBorder);

  // Create text string
  char opacityMinimumString[128];
  sprintf(opacityMinimumString, "%.1f", _opacityMinimum);

  // Calculate width
  float opacityMinimumWidth = 0.0;
  for (int i = 0 ; i < strlen(opacityMinimumString) ; i++) {
    opacityMinimumWidth += 
      glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, opacityMinimumString[i]);
  }

  // Position text
  glRasterPos3f(_frustum[0] + opacityMinimumHorizontalOffset, 
                _frustum[2] + opacityMinimumVerticalOffset, 
                0.0);

  // Display text
  for (int i = 0 ; i < strlen(opacityMinimumString) ; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, opacityMinimumString[i]);
  }
  */


  /*
  // Maximum opacity

  // Calculate offsets for text
  float opacityMaximumHorizontalOffset = 
    ((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    ((float) textBorder);
  float opacityMaximumVerticalOffset = 
    ((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
     ((float) textBorder + (float) textHeight);

  // Create text string
  char opacityMaximumString[128];
  sprintf(opacityMaximumString, "%.1f", _opacityMaximum);

  // Calculate width
  float opacityMaximumWidth = 0.0;
  for (int i = 0 ; i < strlen(opacityMaximumString) ; i++) {
    opacityMaximumWidth += 
      glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, opacityMaximumString[i]);
  }

  // Position text
  glRasterPos3f(_frustum[0] + opacityMaximumHorizontalOffset, 
                _frustum[3] - opacityMaximumVerticalOffset, 
                0.0);

  // Display text
  for (int i = 0 ; i < strlen(opacityMinimumString) ; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, opacityMaximumString[i]);
  }
  */


  // Minimum scalar

  // Calculate offsets for text
  float scalarMinimumHorizontalOffset = 
    ((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    ((float) textBorder /*+ opacityMinimumWidth + (float) textBorder*/);
  float scalarMinimumVerticalOffset = 
    ((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
    ((float) textBorder);

  // Create text string
  char scalarMinimumString[128];
  sprintf(scalarMinimumString, "%.0f", _scalarMinimum);

  // Calculate width
  float scalarMinimumWidth = 0.0;
  for (int i = 0 ; i < (int) strlen(scalarMinimumString) ; i++) {
    //scalarMinimumWidth += 
    //  glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, scalarMinimumString[i]);
  }

  // Position text
  glRasterPos3f(_frustum[0] + scalarMinimumHorizontalOffset, 
                _frustum[2] + scalarMinimumVerticalOffset, 
                0.0);

  // Display text
  for (int i = 0 ; i < (int) strlen(scalarMinimumString) ; i++) {
    //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, scalarMinimumString[i]);
  }


  // Maximum scalar

  // Create text string
  char scalarMaximumString[128];
  sprintf(scalarMaximumString, "%.0f", _scalarMaximum);

  // Calculate width
  float scalarMaximumWidth = 0.0;
  for (int i = 0 ; i < (int) strlen(scalarMaximumString) ; i++) {
    //scalarMaximumWidth += 
    //  glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, scalarMaximumString[i]);
  }

  // Calculate offsets for text
  float scalarMaximumHorizontalOffset = 
    ((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    ((float) textBorder + scalarMaximumWidth);
  float scalarMaximumVerticalOffset = 
    ((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
    ((float) textBorder);

  // Position text
  glRasterPos3f(_frustum[1] - scalarMaximumHorizontalOffset, 
                _frustum[2] + scalarMaximumVerticalOffset, 
                0.0);

  // Display text
  for (int i = 0 ; i < (int) strlen(scalarMaximumString) ; i++) {
    //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, scalarMaximumString[i]);
  }


  // Determine left bound
  _boxBound[0] = _frustum[0] + 
    scalarMinimumHorizontalOffset +
    (((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
     scalarMinimumWidth) / 2.0;

  // Determine right bound
  _boxBound[1] = _frustum[1] -
    scalarMaximumHorizontalOffset +
    (((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    scalarMaximumWidth) / 2.0;

  // Determine bottom bound
  _boxBound[2] = _frustum[2] +
    /*opacityMinimumVerticalOffset +*/
    (((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
     ((float) textHeight + (float) textBorder + (float) textBorder)) /*/ 2.0*/;
  
  // Determine top bound
  _boxBound[3]  = _frustum[3] -
    /*opacityMaximumVerticalOffset +*/
    (((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
    (float) textHeight) / 2.0;


  /*
  // Opacity label
 
  // Opacity width
  float opacityLabelWidth = glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, 'O');

  // Calculate offsets for text
  float opacityLabelHorizontalOffset =
    ((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    ((float) textBorder);
   float opacityLabelVerticalOffset =
     ((_boxBound[3] - _boxBound[2]) / 2.0) + _boxBound[2] -
     ((((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
       (float) textHeight) / 2.0);
   
  // Position text
  glRasterPos3f(_frustum[0] + opacityLabelHorizontalOffset, 
                _frustum[2] + opacityLabelVerticalOffset, 
                0.0);

  // Display text
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'O');
  */


  // Scalar label

  // Create text string
  char scalarLabelString[128];

  // Fill text string
  sprintf(scalarLabelString, "Scalar");

  // Scalar width
  float scalarLabelWidth = 0.0;
  for (int i = 0 ; i < (int) strlen(scalarLabelString) ; i++) {
    //scalarLabelWidth += 
    //  glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, scalarLabelString[i]);
  }

  // Calculate offsets for text
  float scalarLabelHorizontalOffset = 
    ((_boxBound[1] - _boxBound[0]) / 2.0) + _boxBound[0] - 
    ((((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
      scalarLabelWidth) / 2.0);
  float scalarLabelVerticalOffset =
    ((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) * 
    ((float) textBorder);

  // Position text
  glRasterPos3f(_frustum[0] + scalarLabelHorizontalOffset, 
                _frustum[2] + scalarLabelVerticalOffset, 
                0.0);

  // Display text
  for (int i = 0 ; i < (int) strlen(scalarLabelString) ; i++) {
    //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, scalarLabelString[i]);
  }


  /*
  // Control point scalar readout

  // Calculate offsets for text
  float scalarReadoutHorizontalOffset = 
    _boxBound[0] + 
    ((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    ((float) textBorder);
  float scalarReadoutVerticalOffset = 
    (_frustum[3] - _boxBound[3]) +
    ((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) *
    ((float) textHeight + (float) textBorder);

  // Position text
  glRasterPos3f(_frustum[0] + scalarReadoutHorizontalOffset, 
                _frustum[3] - scalarReadoutVerticalOffset, 
                0.0);

  // Create text string
  char scalarReadoutString[128];

  // If a valid control point has the focus
  if (_focusPoint >= 0 && _focusPoint < _numberOfControlPoints) {
    
    // Function coordinates
    float s = 0.0;
    float o = 0.0;

    // Get control point
    GetControlPoint(_focusPoint, &s, &o);

    // Fill text string
    sprintf(scalarReadoutString, "%s %.4f", "S:", s);

  }

  // If no point has focus
  else {

    // Fill text string
    sprintf(scalarReadoutString, "%s", "S:");

  }

  // Display text
  for (int i = 0 ; i < strlen(scalarReadoutString) ; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, scalarReadoutString[i]);
  }
  */


  /*
  // Control point opacity readout

  // Calculate offsets for text
  float opacityReadoutHorizontalOffset = 
    _boxBound[0] + 
    ((_frustum[1] - _frustum[0]) / (float) _pixelDimensions[0]) * 
    ((float) textBorder);
  float opacityReadoutVerticalOffset = 
    scalarReadoutVerticalOffset +
    ((_frustum[3] - _frustum[2]) / (float) _pixelDimensions[1]) *
    ((float) textHeight + (float) textBorder);

  // Position text
  glRasterPos3f(_frustum[0] + opacityReadoutHorizontalOffset, 
                _frustum[3] - opacityReadoutVerticalOffset, 
                0.0);

  // Create text string
  char opacityReadoutString[128];

  // If a valid control point has the focus
  if (_focusPoint >= 0 && _focusPoint < _numberOfControlPoints) {
    
    // Function coordinates
    float s = 0.0;
    float o = 0.0;

    // Get control point
    GetControlPoint(_focusPoint, &s, &o);

    // Fill text string
    sprintf(opacityReadoutString, "%s %.4f", "O:", o);

  }

  // If no point has focus
  else {

    // Fill text string
    sprintf(opacityReadoutString, "%s", "O:");

  }

  // Display text
  for (int i = 0 ; i < strlen(opacityReadoutString) ; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, opacityReadoutString[i]);
  }
  */


  // Box color
  glColor4f(_boxColor[0], _boxColor[1], _boxColor[2], _boxColor[3]);

  // Draw box
  glLoadIdentity();
  glBegin(GL_LINES);
  glVertex3f(_boxBound[0], _boxBound[2], 0.0);
  glVertex3f(_boxBound[0], _boxBound[3], 0.0);
  glVertex3f(_boxBound[1], _boxBound[2], 0.0);
  glVertex3f(_boxBound[1], _boxBound[3], 0.0);
  glVertex3f(_boxBound[0], _boxBound[2], 0.0);
  glVertex3f(_boxBound[1], _boxBound[2], 0.0);
  glVertex3f(_boxBound[0], _boxBound[3], 0.0);
  glVertex3f(_boxBound[1], _boxBound[3], 0.0);
  glEnd();


  // Box coordinates
  float bx = 0.0;
  float by = 0.0;

  // Function coordinates
  float scalar = 0.0;
  float opacity = 0.0;

  // World coordinates
  float wx = 0.0;
  float wy = 0.0;
    

  // Line color
  glColor4f(_lineColor[0], _lineColor[1], _lineColor[2], _lineColor[3]);

  // Draw lines connecting control points
  glBegin(GL_LINE_STRIP);
  for (int i = 0 ; i < _numberOfControlPoints ; i++) {

    // Get control point values
    scalar = _controlPoint[i].GetScalar();
    opacity = _controlPoint[i].GetOpacity();

    // Check that function coordinates are within box space
    if (IsFunctionInBox(scalar, opacity) == true) {

      // Get corresponding box coordinates
      FunctionToBox(scalar, opacity, &bx, &by);

      // Check that box coordinates are within world space
      if (IsBoxInWorld(bx, by) == true) {

        // Get corresponding world coordinates
        BoxToWorld(bx, by, &wx, &wy);

        // Next point in line strip
        glVertex3f(wx, wy, 0.0);

      }

    }

  }
  glEnd();


  // Draw each control point
  for (int i = 0 ; i < _numberOfControlPoints ; i++) {

    // Get control point values
    scalar = _controlPoint[i].GetScalar();
    opacity = _controlPoint[i].GetOpacity();

    // Check that function coordinates are within box space
    if (IsFunctionInBox(scalar, opacity) == true) {

      // Get corresponding box coordinates
      FunctionToBox(scalar, opacity, &bx, &by);

      // Check that box coordinates are within world space
      if (IsBoxInWorld(bx, by) == true) {

        // Get corresponding world coordinates
        BoxToWorld(bx, by, &wx, &wy);

        // Draw focus point
        if (i == _focusPoint) {

          // Draw bigger outline point
          glPointSize(_controlPointSize * 2.0);
          glColor4f(_focusPointColor[0], _focusPointColor[1],
                    _focusPointColor[2], _focusPointColor[3]); 
          glBegin(GL_POINTS);
          glVertex3f(wx, wy, 0.0);
          glEnd();

          // Draw regular point inside
          glPointSize((float) _controlPointSize);
          glColor4f(_controlPointColor[0], _controlPointColor[1], 
                    _controlPointColor[2], _controlPointColor[3]);
          glBegin(GL_POINTS);
          glVertex3f(wx, wy, 0.0);
          glEnd();

          // Draw even smaller point
          glPointSize((float) _controlPointSize / 4.0);
          glColor4f(_focusPointColor[0], _focusPointColor[1],
                    _focusPointColor[2], _focusPointColor[3]); 
          glBegin(GL_POINTS);
          glVertex3f(wx, wy, 0.0);
          glEnd();
        }

        // Draw single control point
        else {
          glPointSize(_controlPointSize);
          glColor4f(_controlPointColor[0], _controlPointColor[1], 
                    _controlPointColor[2], _controlPointColor[3]);
          glBegin(GL_POINTS);
          glVertex3f(wx, wy, 0.0);
          glEnd();
        }

      }

    }

  }

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::FunctionToBox(float scalar, float opacity,
                                                  float* bx, float* by) {

  // Convert from function coordinates to box coordinates
  *bx = (scalar - _scalarMinimum) / (_scalarMaximum - _scalarMinimum);
  *by = (opacity - _opacityMinimum) / (_opacityMaximum - _opacityMinimum);

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::GetControlPoint(int index, 
                                                    float* scalar, 
                                                    float* opacity) {

  // Get control point
  *scalar = _controlPoint[index].GetScalar();
  *opacity = _controlPoint[index].GetOpacity();

  return;

}

/*--------------------------------------------------------------------------*/

int OpacityTransferFunctionWidget::GetControlPointIndex(int px, int py) {

  // Control point box coordinates
  float cbx = 0.0;
  float cby = 0.0;

  // Control point world coordinates
  float cwx = 0.0;
  float cwy = 0.0;

  // Pixel world coordinates
  float pwx = 0.0;
  float pwy = 0.0;

  // Function coordinates
  float scalar = 0.0;
  float opacity = 0.0;

  // Distances
  vector <float> distance;


  // Set focus point
  _focusPoint = -1;

  // Check that pixel coordinates are within world space
  if (IsPixelInWorld(px, py) == false) {
    return -1;
  }

  // Get corresponding world coordinates
  PixelToWorld(px, py, &pwx, &pwy);

  // Check that world coordinates are within box space
  if (IsWorldInBox(pwx, pwy) == false) {
    return -1;
  }

  // Check for control points
  if (_numberOfControlPoints < 1) {
    return -1;
  }

  
  // Calculate distance to each control point
  for (int i = 0 ; i < _numberOfControlPoints ; i++) {

    // Get control point values
    opacity = _controlPoint[i].GetOpacity();
    scalar = _controlPoint[i].GetScalar();

    // Check that function coordinates are within box space
    if (IsFunctionInBox(scalar, opacity) == true) {

      // Get corresponding box coordinates
      FunctionToBox(scalar, opacity, &cbx, &cby);

      // Check that box coordinates are within world space
      if (IsBoxInWorld(cbx, cby) == true) {

        // Get corresponding world coordinates
        BoxToWorld(cbx, cby, &cwx, &cwy);

        // Calculate distance and correct for aspect ratio
        float dX = pwx - cwx;
        float dY = (pwy - cwy) / (_pixelDimensions[0] / _pixelDimensions[1]);
        float d = sqrt(dX*dX + dY*dY);

        // Add distance
        distance.push_back(d);

      }

    }

  }


  // Size of control point in world coordinates
  float ctrlPtSizeX = 0.0;
  float ctrlPtSizeY = 0.0;

  // Calculate size of a control point in world coordinates
  PixelToWorld(_controlPointSize / 2, 
               _pixelDimensions[1] - (_controlPointSize / 2), 
               &ctrlPtSizeX, 
               &ctrlPtSizeY);


  // Minimum distance
  float minimum = distance[0];

  // Index of control point closest to pixel coordinates
  int index = 0;

  // Find minimum distance
  for (int i = 0 ; i < _numberOfControlPoints ; i++) {
    if (distance[i] < minimum) {
      minimum = distance[i];
      index = i;
    }
  }


  // Get control point values for closest point
  opacity = _controlPoint[index].GetOpacity();
  scalar = _controlPoint[index].GetScalar();

  // Check that function coordinates are within box space
  if (IsFunctionInBox(scalar, opacity) == true) {

    // Get corresponding box coordinates
    FunctionToBox(scalar, opacity, &cbx, &cby);

    // Check that box coordinates are within world space
    if (IsBoxInWorld(cbx, cby) == true) {

      // Get corresponding world coordinates
      BoxToWorld(cbx, cby, &cwx, &cwy);

      // Return control point index and set focus point if it's picked
      if (pwx >= cwx - ctrlPtSizeX && pwx <= cwx + ctrlPtSizeX &&
          pwy >= cwy - ctrlPtSizeY && pwy <= cwy + ctrlPtSizeY) {
        _focusPoint = index;
        return index;
      }

    }

  }


  // No control point picked
  return -1;

}

/*--------------------------------------------------------------------------*/

int OpacityTransferFunctionWidget::GetControlPointIndex(float scalar, 
                                                        float opacity) {

  // Check each control point for matching function coordinates
  for (int i = 0 ; i < _numberOfControlPoints ; i++) {
    if (_controlPoint[i].GetScalar() == scalar &&
        _controlPoint[i].GetOpacity() == opacity) {
      return i;
    }
  }

  // No control point picked
  return -1;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::GetFunctionCoordinates(int px, int py, 
                                                           float* scalar, 
                                                           float* opacity) {

  // Box coordinates
  float bx = 0.0;
  float by = 0.0;

  // World coordinates
  float wx = 0.0;
  float wy = 0.0;

  // Check that pixel coordinates are within world space
  if (IsPixelInWorld(px, py) == true) {

    // Get corresponding world coordinates
    PixelToWorld(px, py, &wx, &wy);

    // Check that world coordinates are within box space
    if (IsWorldInBox(wx, wy) == true) {

      // Get corresponding box coordinates
      WorldToBox(wx, wy, &bx, &by);

      // Get corresponding function coordinates
      BoxToFunction(bx, by, scalar, opacity);

      // Control point added
      return true;

    }

  }

  // Not within bounds
  return false;

}

/*--------------------------------------------------------------------------*/

int OpacityTransferFunctionWidget::GetNumberOfControlPoints() {

  // Return number of control points
  return _numberOfControlPoints;

}

/*--------------------------------------------------------------------------*/

float OpacityTransferFunctionWidget::GetOpacity(float scalar) {

  // Check minimum bound
  if (scalar < _controlPoint[0].GetScalar()) {
    return _controlPoint[0].GetOpacity();
  }

  // Check maximum bound
  if (scalar > _controlPoint[_numberOfControlPoints - 1].GetScalar()) {
    return _controlPoint[_numberOfControlPoints - 1].GetOpacity();
  }

  // Control points that border scalar value
  int left = 0;
  int right = 0;
      
  // Find control points that border scalar value
  for (int i = 1 ; i < _numberOfControlPoints ; i++) {
    if (scalar < _controlPoint[i].GetScalar()) {
      right = i;
      break;
    }
    left = i;
  }

  // Function coordinates of control points
  float xl = _controlPoint[left].GetScalar();
  float xr = _controlPoint[right].GetScalar();
  float yl = _controlPoint[left].GetOpacity();
  float yr = _controlPoint[right].GetOpacity();

  // Interpolate between bordering control points
  float opacity = yl + (((scalar - xl) / (xr - xl)) * (yr - yl));
  
  // Return opacity
  return opacity;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::GetOpacityRange(float* min, float* max) {

  // Set opacity range
  *min = _opacityMinimum;
  *max = _opacityMaximum;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::GetScalarRange(float* min, float* max) {

  // Set scalar range
  *min = _scalarMinimum;
  *max = _scalarMaximum;

  return;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::IsBoxInWorld(float bx, float by) {

  // World coordinates
  float wx = 0.0;
  float wy = 0.0;

  // Calculate world coordinates
  BoxToWorld(bx, by, &wx, &wy);

  // Check if box coordinates are in world space
  if (wx >= _frustum[0] && wx <= _frustum[1] &&
      wy >= _frustum[2] && wy <= _frustum[3]) {
    return true;
  }

  // Not in world space
  return false;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::IsFunctionInBox(float scalar, 
                                                    float opacity) {

  // Box coordinates
  float bx = 0.0;
  float by = 0.0;

  // Calculate box coordinates
  FunctionToBox(scalar, opacity, &bx, &by);

  // Check if function coordinates are in box space
  if (bx >= 0.0 && bx <= 1.0 && 
      by >= 0.0 && by <= 1.0) {
    return true;
  }

  // Not in box space
  return false;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::IsPixelInWorld(int px, int py) {

  // World coordinates
  float wx = 0.0;
  float wy = 0.0;

  // Calculate world coordinates
  PixelToWorld(px, py, &wx, &wy);

  // Check if pixel coordinates are in world space
  if (wx >= _frustum[0] && wx <= _frustum[1] &&
      wy >= _frustum[2] && wy <= _frustum[3]) {
    return true;
  }

  // Not in world space
  return false;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::IsWorldInBox(float wx, float wy) {

  // Box coordinates
  float bx = 0.0;
  float by = 0.0;

  // Calculate box coordinates
  WorldToBox(wx, wy, &bx, &by);

  // Check if world coordinates are in box space
  if (bx >= 0.0 && bx <= 1.0 &&
      by >= 0.0 && by <= 1.0) {
    return true;
  }

  // Not in box space
  return false;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::MoveControlPoint(int index, 
                                                     int px, 
                                                     int py) {

  // Box coordinates
  float bx = 0.0;
  float by = 0.0;

  // Function coordinates
  float scalar = 0.0;
  float opacity = 0.0;

  // World coordinates
  float wx = 0.0;
  float wy = 0.0;

  // Check that pixel coordinates are within world space
  if (IsPixelInWorld(px, py) == true) {

    // Get corresponding world coordinates
    PixelToWorld(px, py, &wx, &wy);

    // Check that world coordinates are within box space
    if (IsWorldInBox(wx, wy) == true) {

      // Get corresponding box coordinates
      WorldToBox(wx, wy, &bx, &by);

      // Get corresponding function coordinates
      BoxToFunction(bx, by, &scalar, &opacity);

      // Check that control point picked is not an edge point and is valid
      if (index > 0 && index < _numberOfControlPoints - 1) {

        // Check that new scalar is not past next point
        if (scalar >= _controlPoint[index + 1].GetScalar()) {
          float s = _controlPoint[index + 1].GetScalar() - 0.0001;
          _controlPoint[index].SetScalar(s);
        }

        // Check that new scalar is not before previous point
        else if (scalar <= _controlPoint[index - 1].GetScalar()) {
          float s = _controlPoint[index - 1].GetScalar() + 0.0001;
          _controlPoint[index].SetScalar(s);
        }

        // Check that new scalar is not greater than maximum scalar value
        else if (scalar > _scalarMaximum) {
          _controlPoint[index].SetScalar(_scalarMaximum);
        }

        // Check that new scalar is not less than minimum scalar value
        else if (scalar < _scalarMinimum) {
          _controlPoint[index].SetScalar(_scalarMinimum);
        }

        // New scalar value is valid
        else {
          _controlPoint[index].SetScalar(scalar);
        }

      }
      
      // Check that control point picked is valid
      if (index >= 0 && index <= _numberOfControlPoints - 1) {

        // Check that new opacity is not greater than maximum opacity value
        if (opacity > _opacityMaximum) {
          _controlPoint[index].SetOpacity(_opacityMaximum);
        }

        // Check that new opacity is not less than minimum opacity value
        else if (opacity < _opacityMinimum) {
          _controlPoint[index].SetOpacity(_opacityMinimum);
        }
        
        // New opacity value is valid
        else {
          _controlPoint[index].SetOpacity(opacity);
        }

      }

      // Control point moved
      return true;

    }

  }

  // Can't move control point to desired position
  return false;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::MoveControlPoint(int index, float scalar,
                                                     float opacity) {

  // Check that control point picked is not an edge point and is valid
  if (index > 0 && index < _numberOfControlPoints - 1) {

    // Check that new scalar is not past next point
    if (scalar >= _controlPoint[index + 1].GetScalar()) {
      float s = _controlPoint[index + 1].GetScalar() - 0.0001;
      _controlPoint[index].SetScalar(s);
    }

    // Check that new scalar is not before previous point
    else if (scalar <= _controlPoint[index - 1].GetScalar()) {
      float s = _controlPoint[index - 1].GetScalar() + 0.0001;
      _controlPoint[index].SetScalar(s);
    }

    // Check that new scalar is not greater than maximum scalar value
    else if (scalar > _scalarMaximum) {
      _controlPoint[index].SetScalar(_scalarMaximum);
    }

    // Check that new scalar is not less than minimum scalar value
    else if (scalar < _scalarMinimum) {
      _controlPoint[index].SetScalar(_scalarMinimum);
    }

    // New scalar value is valid
    else {
      _controlPoint[index].SetScalar(scalar);
    }

  }

  // Check that control point is valid
  if (index >= 0 && index <= _numberOfControlPoints - 1) {

    // Check that new opacity is not greater than maximum opacity value
    if (opacity > _opacityMaximum) {
      _controlPoint[index].SetOpacity(_opacityMaximum);
    }

    // Check that new opacity is not less than minimum opacity value
    else if (opacity < _opacityMinimum) {
      _controlPoint[index].SetOpacity(_opacityMinimum);
    }

    // New opacity value is valid
    else {
      _controlPoint[index].SetOpacity(opacity);
    }

  }

  // Control point moved
  return true;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::PixelToWorld(int px, int py, 
                                                 float* wx, float* wy) {

  // Flip y coordinate
  py = _pixelDimensions[1] - py;

  // Convert from pixel coordinates to world coordinates
  *wx = _frustum[0] + 
    (((float) px / (float) _pixelDimensions[0]) * (_frustum[1] - _frustum[0]));
  *wy = _frustum[2] + 
    (((float) py / (float) _pixelDimensions[1]) * (_frustum[3] - _frustum[2]));

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::RestoreControlPoints(int number,
                                                         float max,
                                                         float* scalar, 
                                                         float* opacity) {

  // Temporary control point
  ControlPoint tmp;

  // Set number of control points
  _numberOfControlPoints = number;

  // Set maximum scalar value
  _scalarMaximum = max;

  // Clear all control points
  _controlPoint.clear();

  // Set each control point from arrays
  for (int i = 0 ; i < number ; i++) {
    tmp.SetScalar(scalar[i]);
    tmp.SetOpacity(opacity[i]);
    _controlPoint.push_back(tmp);
  }

  // Set focus point
  _focusPoint = -1;

  return;

}

/*--------------------------------------------------------------------------*/

bool OpacityTransferFunctionWidget::RemoveControlPoint(int index) {

  // Check that index is within bounds and set focus point
  if (index < 1 || index >= _numberOfControlPoints - 1) {
    _focusPoint = index;
    return false;
  }

  // Remove control point
  _controlPoint.erase(_controlPoint.begin() + index);

  // Decrement number of control points
  _numberOfControlPoints--;

  // Set focus point
  _focusPoint = -1;

  // Success
  return true;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetBackgroundColor(float r, float g, 
                                                       float b, float a) {

  // Set background color
  _backgroundColor[0] = r;
  _backgroundColor[1] = g;
  _backgroundColor[2] = b;
  _backgroundColor[3] = a;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetBoxColor(float r, float g, 
                                                float b, float a) {

  // Set box color
  _boxColor[0] = r;
  _boxColor[1] = g;
  _boxColor[2] = b;
  _boxColor[3] = a;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetControlPointColor(float r, float g,
                                                         float b, float a) {

  // Set control point color
  _controlPointColor[0] = r;
  _controlPointColor[1] = g;
  _controlPointColor[2] = b;
  _controlPointColor[3] = a;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetControlPointSize(int size) {

  // Set control point size
  _controlPointSize = size;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetDimensions(int w, int h) {

  // Set dimensions
  _pixelDimensions[0] = w;
  _pixelDimensions[1] = h;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetFocusPointColor(float r, float g, 
                                                       float b, float a) {

  // Set focus point color
  _focusPointColor[0] = r;
  _focusPointColor[1] = g;
  _focusPointColor[2] = b;
  _focusPointColor[3] = a;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetLineColor(float r, float g, 
                                                 float b, float a) {

  // Set line color
  _lineColor[0] = r;
  _lineColor[1] = g;
  _lineColor[2] = b;
  _lineColor[3] = a;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetMaximumScalar(float max) {

  // New scalar values
  float newMax = max;
  float newMin = _scalarMinimum;

  // Old scalar values
  float oldMax = _scalarMaximum;
  float oldMin = _scalarMinimum;

  // Resample each control point's scalar value
  for (int i = 0 ; i < _numberOfControlPoints ; i++) {

    // Get old scalar value
    float oldScalar = _controlPoint[i].GetScalar();

    // Normalize old scalar value
    float normal = (oldScalar - oldMin) / (oldMax - oldMin);

    // Resample old scalar value to new bounds
    float newScalar = ((newMax - newMin) * normal) + newMin;

    // Set new scalar value
    _controlPoint[i].SetScalar(newScalar);

  }

  // Set maximum scalar
  _scalarMaximum = max;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::SetTextColor(float r, float g, 
                                                 float b, float a) {

  // Set text color
  _textColor[0] = r;
  _textColor[1] = g;
  _textColor[2] = b;
  _textColor[3] = a;

  return;

}

/*--------------------------------------------------------------------------*/

void OpacityTransferFunctionWidget::WorldToBox(float wx, float wy, 
                                               float* bx, float* by) {

  // Convert from world coordinates to box coordinates
  *bx = (wx - _boxBound[0]) / (_boxBound[1] - _boxBound[0]);
  *by = (wy - _boxBound[2]) / (_boxBound[3] - _boxBound[2]);

  return;

}

/*--------------------------------------------------------------------------*/
