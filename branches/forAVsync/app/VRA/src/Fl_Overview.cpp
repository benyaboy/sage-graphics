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

#include "Fl_Overview.h"

/*--------------------------------------------------------------------------*/

Fl_Overview::Fl_Overview(int x, int y, int w, int h, const char *l) 
  : Fl_Gl_Window(x, y, w, h, l) {

  // Initialize background color
  _backgroundColor[0] = 0.75;
  _backgroundColor[1] = 0.75;
  _backgroundColor[2] = 0.75;
  _backgroundColor[3] = 0.75;

  // Initialize frustum
  _frustum[0] = -1.0;
  _frustum[1] = 1.0;
  _frustum[2] = -1.0;
  _frustum[3] = 1.0;
  _frustum[4] = -1.0;
  _frustum[5] = 1.0;

  // Initialize line color
  _lineColor[0] = 1.0;
  _lineColor[1] = 1.0;
  _lineColor[2] = 1.0;
  _lineColor[3] = 1.0;

  // Initialize mouse button state
  _mouseButtonState[0] = FL_OVERVIEW_UP;
  _mouseButtonState[1] = FL_OVERVIEW_UP;
  _mouseButtonState[2] = FL_OVERVIEW_UP;

  // Initialize mouse position
  _mousePosition[0] = 0;
  _mousePosition[1] = 0;

  // Initialize rotation matrix
  _R[0] = 1.0; _R[4] = 0.0; _R[8] = 0.0; _R[12] = 0.0;
  _R[1] = 0.0; _R[5] = 1.0; _R[9] = 0.0; _R[13] = 0.0;
  _R[2] = 0.0; _R[6] = 0.0; _R[10] = 1.0; _R[14] = 0.0;
  _R[3] = 0.0; _R[7] = 0.0; _R[11] = 0.0; _R[15] = 1.0;
  
  // Initialize scale matrix
  _S[0] = 1.0; _S[4] = 0.0; _S[8] = 0.0; _S[12] = 0.0;
  _S[1] = 0.0; _S[5] = 1.0; _S[9] = 0.0; _S[13] = 0.0;
  _S[2] = 0.0; _S[6] = 0.0; _S[10] = 1.0; _S[14] = 0.0;
  _S[3] = 0.0; _S[7] = 0.0; _S[11] = 0.0; _S[15] = 1.0;
  
  // Initialize translation matrix
  _T[0] = 1.0; _T[4] = 0.0; _T[8] = 0.0; _T[12] = 0.0;
  _T[1] = 0.0; _T[5] = 1.0; _T[9] = 0.0; _T[13] = 0.0;
  _T[2] = 0.0; _T[6] = 0.0; _T[10] = 1.0; _T[14] = 0.0;
  _T[3] = 0.0; _T[7] = 0.0; _T[11] = 0.0; _T[15] = 1.0;

  // Initialize callbacks
  _updateRenderCallback = NULL;
  _updateRotationCallback = NULL;
  _updateScaleCallback = NULL;
  _updateTranslationCallback = NULL;

  // Initialize vertices
  _vertex[0].SetXYZ(-0.5, -0.5, 0.5);
  _vertex[1].SetXYZ(-0.5, -0.5, -0.5);
  _vertex[2].SetXYZ(0.5, -0.5, -0.5);
  _vertex[3].SetXYZ(0.5, -0.5, 0.5);
  _vertex[4].SetXYZ(-0.5, 0.5, 0.5);
  _vertex[5].SetXYZ(-0.5, 0.5, -0.5);
  _vertex[6].SetXYZ(0.5, 0.5, -0.5);
  _vertex[7].SetXYZ(0.5, 0.5, 0.5);

  // Initialize world rotate factor
  _worldRotate[0] = 0.0;
  _worldRotate[1] = 0.0;
  _worldRotate[2] = 0.0;

  // Initialize world scale factor
  _worldScale[0] = 1.0;
  _worldScale[1] = 1.0;  
  _worldScale[2] = 1.0;

  // Initialize world translate factor
  _worldTranslate[0] = 0.0;
  _worldTranslate[1] = 0.0;
  _worldTranslate[2] = 0.0;

}

/*--------------------------------------------------------------------------*/

Fl_Overview::~Fl_Overview() {
}

/*--------------------------------------------------------------------------*/

void Fl_Overview::GetScaleMatrix(float m[16]) {

  // Copy transltion matrix
  for (int i = 0 ; i < 16 ; i++) {
    m[i] = _S[i];
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::GetTranslationMatrix(float m[16]) {

  // Copy transltion matrix
  for (int i = 0 ; i < 16 ; i++) {
    m[i] = _T[i];
  }

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::ResetView() {

  // Reset rotation
  _worldRotate[0] = 0.0;
  _worldRotate[1] = 0.0;

  // Reset scale
  _worldScale[0] = 1.0;
  _worldScale[1] = 1.0;
  _worldScale[2] = 1.0;

  // Reset translation
  _worldTranslate[0] = 0.0;
  _worldTranslate[1] = 0.0;
  _worldTranslate[2] = 0.0;

  // Reset trackball
  trackball.Clear();

  // Calculate rotation matrix
  float R[16];
  float Rx[16];
  float Ry[16];
  float Rz[16];
  calc_rot_x(Rx, _worldRotate[0]);
  calc_rot_y(Ry, _worldRotate[1]);
  calc_rot_z(Rz, _worldRotate[2]);
  mult4x4(R, Rx, Ry);
  mult4x4(R, R, Rz);

  // Set rotation matrix
  SetRotationMatrix(R);

  // Calcuate scale matrix
  float S[16];
  calc_scale(S, _worldScale[0], _worldScale[1], _worldScale[2]);
  
  // Set scale matrix
  SetScaleMatrix(S);

  // Calculate translation matrix
  float T[16];
  calc_trans(T, 
             _worldTranslate[0], 
             _worldTranslate[1], 
             _worldTranslate[2]);
  
  // Set translation matrix
  SetTranslationMatrix(T);

  // Callbacks
  if (_updateRotationCallback != NULL) _updateRotationCallback(_R);
  if (_updateScaleCallback != NULL) _updateScaleCallback(_S);
  if (_updateTranslationCallback != NULL) _updateTranslationCallback(_T);
  if (_updateRenderCallback != NULL) _updateRenderCallback();
  if (_updateRenderCallback != NULL) _updateRenderCallback();

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetBackgroundColor(float r, float g, float b, float a) {

  // Set background color
  _backgroundColor[0] = r;
  _backgroundColor[1] = g;
  _backgroundColor[2] = b;
  _backgroundColor[3] = a;

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetFrustum(float frustum[6]) {

  // Set frustum
  _frustum[0] = frustum[0];
  _frustum[1] = frustum[1];
  _frustum[2] = frustum[2];
  _frustum[3] = frustum[3];
  _frustum[4] = frustum[4];
  _frustum[5] = frustum[5];

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetRenderCallback(void (*cb)()) {

  // Set callback
  _updateRenderCallback = cb;

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetRotationCallback(void (*cb)(float* m)) {

  // Set callback
  _updateRotationCallback = cb;

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetRotationMatrix(float m[16]) {

  // Set rotation matrix
  _R[0] = m[0]; _R[4] = m[4]; _R[8] = m[8]; _R[12] = m[12];
  _R[1] = m[1]; _R[5] = m[5]; _R[9] = m[9]; _R[13] = m[13];
  _R[2] = m[2]; _R[6] = m[6]; _R[10] = m[10]; _R[14] = m[14];
  _R[3] = m[3]; _R[7] = m[7]; _R[11] = m[11]; _R[15] = m[15];

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetScaleCallback(void (*cb)(float* m)) {

  // Set callback
  _updateScaleCallback = cb;

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetScaleMatrix(float m[16]) {

  _worldScale[0] = m[0];
  _worldScale[1] = m[5];
  _worldScale[2] = m[10];

  // Set scale matrix
  _S[0] = m[0]; _S[4] = m[4]; _S[8] = m[8]; _S[12] = m[12];
  _S[1] = m[1]; _S[5] = m[5]; _S[9] = m[9]; _S[13] = m[13];
  _S[2] = m[2]; _S[6] = m[6]; _S[10] = m[10]; _S[14] = m[14];
  _S[3] = m[3]; _S[7] = m[7]; _S[11] = m[11]; _S[15] = m[15];

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetTranslationCallback(void (*cb)(float* m)) {

  // Set callback
  _updateTranslationCallback = cb;

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetTranslationMatrix(float m[16]) {

  _worldTranslate[0] = m[12];
  _worldTranslate[1] = m[13];
  _worldTranslate[2] = m[14];

  // Set translation matrix
  _T[0] = m[0]; _T[4] = m[4]; _T[8] = m[8]; _T[12] = m[12];
  _T[1] = m[1]; _T[5] = m[5]; _T[9] = m[9]; _T[13] = m[13];
  _T[2] = m[2]; _T[6] = m[6]; _T[10] = m[10]; _T[14] = m[14];
  _T[3] = m[3]; _T[7] = m[7]; _T[11] = m[11]; _T[15] = m[15];

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::SetVertices(Vertex vertex[8]) {

  // Set vertex
  _vertex[0] = vertex[0];
  _vertex[1] = vertex[1];
  _vertex[2] = vertex[2];
  _vertex[3] = vertex[3];
  _vertex[4] = vertex[4];
  _vertex[5] = vertex[5];
  _vertex[6] = vertex[6];
  _vertex[7] = vertex[7];

}

/*--------------------------------------------------------------------------*/

void Fl_Overview::draw() {

  // Clear color
  glClearColor(_backgroundColor[0], _backgroundColor[1],
               _backgroundColor[2], _backgroundColor[3]);

  // Clear bufffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Set viewport 
  glViewport(0, 0, w(), h());
    
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


  // Overview

  // Push matrix
  glPushMatrix();

  // Load identity
  glLoadIdentity();

  // Add scale if zooming out beyond original scale
  if (_S[0] < 1.0 || _S[5] < 1.0 || _S[10] < 1.0) {

    // Add translation
    glMultMatrixf(_T);

    // Add scale
    glMultMatrixf(_S);

  }

  // If zooming in then divide translation by scale
  else {

    // Divide translation by scale
    float matrix[16];
    matrix[0] = 1.0;
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = 1.0;
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = 0.0;
    matrix[9] = 0.0;
    matrix[10] = 1.0;
    matrix[11] = 0.0;    
    matrix[12] = _T[12] / _S[0];
    matrix[13] = _T[13] / _S[5];
    matrix[14] = _T[14] / _S[10];
    matrix[15] = 1.0;

    // Add matrix
    glMultMatrixf(matrix);

  }

  // Add rotation
  glMultMatrixf(_R);
  
  // Line color
  glColor4f(_lineColor[0], _lineColor[1], _lineColor[2], _lineColor[3]);

  // Draw bottom face
  glBegin(GL_LINE_STRIP);
  glVertex3f(_vertex[0].GetX(), _vertex[0].GetY(), _vertex[0].GetZ());
  glVertex3f(_vertex[1].GetX(), _vertex[1].GetY(), _vertex[1].GetZ());
  glVertex3f(_vertex[3].GetX(), _vertex[2].GetY(), _vertex[2].GetZ());
  glVertex3f(_vertex[2].GetX(), _vertex[3].GetY(), _vertex[3].GetZ());
  glVertex3f(_vertex[0].GetX(), _vertex[0].GetY(), _vertex[0].GetZ());
  glEnd();
  
  // Draw top face
  glBegin(GL_LINE_STRIP);
  glVertex3f(_vertex[4].GetX(), _vertex[4].GetY(), _vertex[4].GetZ());
  glVertex3f(_vertex[5].GetX(), _vertex[5].GetY(), _vertex[5].GetZ());
  glVertex3f(_vertex[7].GetX(), _vertex[6].GetY(), _vertex[6].GetZ());
  glVertex3f(_vertex[6].GetX(), _vertex[7].GetY(), _vertex[7].GetZ());
  glVertex3f(_vertex[4].GetX(), _vertex[4].GetY(), _vertex[4].GetZ());
  glEnd();
    
  // Connect top and bottom
  glBegin(GL_LINES);
  glVertex3f(_vertex[0].GetX(), _vertex[0].GetY(), _vertex[0].GetZ());
  glVertex3f(_vertex[4].GetX(), _vertex[4].GetY(), _vertex[4].GetZ());
  glVertex3f(_vertex[1].GetX(), _vertex[1].GetY(), _vertex[1].GetZ());
  glVertex3f(_vertex[5].GetX(), _vertex[5].GetY(), _vertex[5].GetZ());
  glVertex3f(_vertex[2].GetX(), _vertex[2].GetY(), _vertex[2].GetZ());
  glVertex3f(_vertex[6].GetX(), _vertex[6].GetY(), _vertex[6].GetZ());
  glVertex3f(_vertex[3].GetX(), _vertex[3].GetY(), _vertex[3].GetZ());
  glVertex3f(_vertex[7].GetX(), _vertex[7].GetY(), _vertex[7].GetZ());
  glEnd();

  // Pop matrix
  glPopMatrix();


  // Frustum bounding box

  // Push matrix
  glPushMatrix();

  // Load identity
  glLoadIdentity();

  // Line color
  glColor4f(0.0, 0.0, 0.0, 1.0);

  // Add scale if zooming in past original scale
  if (_S[0] > 1.0 || _S[5] > 1.0 || _S[10] > 1.0) {

    // Draw box
    glBegin(GL_LINE_STRIP);
    glVertex3f(_frustum[0]/_S[0], _frustum[2]/_S[5], 0.0);
    glVertex3f(_frustum[1]/_S[0], _frustum[2]/_S[5], 0.0);
    glVertex3f(_frustum[1]/_S[0], _frustum[3]/_S[5], 0.0);
    glVertex3f(_frustum[0]/_S[0], _frustum[3]/_S[5], 0.0);
    glVertex3f(_frustum[0]/_S[0], _frustum[2]/_S[5], 0.0);
    glEnd();

  }

  // Don't add scale
  else {

    // Draw box
    glLineWidth(5.0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(_frustum[0], _frustum[2], 0.0);
    glVertex3f(_frustum[1], _frustum[2], 0.0);
    glVertex3f(_frustum[1], _frustum[3], 0.0);
    glVertex3f(_frustum[0], _frustum[3], 0.0);
    glVertex3f(_frustum[0], _frustum[2], 0.0);
    glEnd();

  }

  glLineWidth(1.0);

  // Pop matrix
  glPopMatrix();


  // Load identity
  glLoadIdentity();


  // Draw bottom border
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glVertex3f(_frustum[0], _frustum[2], 0.0);
  glVertex3f(_frustum[1], _frustum[2], 0.0);
  glEnd();
  glLineWidth(1.0);

}

/*--------------------------------------------------------------------------*/

int Fl_Overview::handle(int event) {

  // Check event
  switch(event) {


  case FL_PUSH: {

    // Get button
    int button = Fl::event_button();

    // Check which button
    if (button == 1) {
      _mouseButtonState[0] = FL_OVERVIEW_DOWN;

      // Start trackball
      trackball.Start(((2.0 * Fl::event_x() - w()) / w()) -
                      0.0, 
                      ((2.0 * (h() - Fl::event_y()) - h()) / h()) -
                      0.0);  
    }
    else if (button == 2) {
      _mouseButtonState[1] = FL_OVERVIEW_DOWN;
    }
    else if (button == 3) {
      _mouseButtonState[2] = FL_OVERVIEW_DOWN;
    }

    // If a button is down
   if (_mouseButtonState[0] == FL_OVERVIEW_DOWN ||
        _mouseButtonState[1] == FL_OVERVIEW_DOWN ||
        _mouseButtonState[2] == FL_OVERVIEW_DOWN) {

     // Callbacks
     if (_updateRotationCallback != NULL) _updateRotationCallback(_R);
     if (_updateScaleCallback != NULL) _updateScaleCallback(_S);
     if (_updateTranslationCallback != NULL) _updateTranslationCallback(_T);
     if (_updateRenderCallback != NULL) _updateRenderCallback();
   }

    // Update mouse position
    _mousePosition[0] = Fl::event_x();
    _mousePosition[1] = Fl::event_y();

    // Return
    return 1;

  }


  case FL_DRAG: {

    // Calculate mouse delta factor
    float mouseDeltaFactor[2];
    mouseDeltaFactor[0] = (_frustum[1] - _frustum[0]) / w();
    mouseDeltaFactor[1] = (_frustum[3] - _frustum[2]) / h();

    // Calculate mouse delta
    int mouseDelta[2];
    mouseDelta[0] = Fl::event_x() - _mousePosition[0];
    mouseDelta[1] = Fl::event_y() - _mousePosition[1];

    // Update mouse position
    _mousePosition[0] = Fl::event_x();
    _mousePosition[1] = Fl::event_y();

    // Rotate
    if (_mouseButtonState[0] == FL_OVERVIEW_DOWN) {

      // Calculate rotation amount
      _worldRotate[0] += mouseDelta[1];
      _worldRotate[1] += mouseDelta[0];

      // Calculate rotation matrix
      float R[16];

      // Update trackball
      trackball.Update(((2.0 * Fl::event_x() - w()) / w()) - 
                       0.0, 
                       ((2.0 * (h() - Fl::event_y()) - h()) / h()) - 
                       0.0);

      // Get rotation from trackball
      trackball.GetRotationMatrix(R);

      // Set rotation matrix
      SetRotationMatrix(R);

      // Callback
      if (_updateRotationCallback != NULL) _updateRotationCallback(_R);
      if (_updateRenderCallback != NULL) _updateRenderCallback();
    }

    // Scale
    if (_mouseButtonState[1] == FL_OVERVIEW_DOWN) {

      // Calculate scale amount
      _worldScale[0] *= 1.0 - mouseDelta[1] * 0.01;
      _worldScale[1] *= 1.0 - mouseDelta[1] * 0.01;
      _worldScale[2] *= 1.0 - mouseDelta[1] * 0.01;

      // Don't let world scale go below 1.0
      if (_worldScale[0] < 1.0) _worldScale[0] = 1.0;
      if (_worldScale[1] < 1.0) _worldScale[1] = 1.0;
      if (_worldScale[2] < 1.0) _worldScale[2] = 1.0;

      // Calcuate scale matrix
      float S[16];
      calc_scale(S, _worldScale[0], _worldScale[1], _worldScale[2]);

      // Set scale matrix
      SetScaleMatrix(S);

      // Callback
      if (_updateScaleCallback != NULL) _updateScaleCallback(_S);
      if (_updateRenderCallback != NULL) _updateRenderCallback();
    }

    // Translate
    if (_mouseButtonState[2] == FL_OVERVIEW_DOWN) {

      // Calculate translation amount
      _worldTranslate[0] += mouseDelta[0] * mouseDeltaFactor[0];
      _worldTranslate[1] -= mouseDelta[1] * mouseDeltaFactor[1];

      // Calculate translation matrix
      float T[16];
      calc_trans(T, 
                 _worldTranslate[0], 
                 _worldTranslate[1], 
                 _worldTranslate[2]);

      // Set translation matrix
      SetTranslationMatrix(T);

      // Callback
      if (_updateTranslationCallback != NULL) _updateTranslationCallback(_T);
      if (_updateRenderCallback != NULL) _updateRenderCallback();
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
      _mouseButtonState[0] = FL_OVERVIEW_UP;
    }
    else if (button == 2) {
      _mouseButtonState[1] = FL_OVERVIEW_UP;
    }
    else if (button == 3) {
      _mouseButtonState[2] = FL_OVERVIEW_UP;
    }

    // If all buttons are up
    if (_mouseButtonState[0] == FL_OVERVIEW_UP &&
        _mouseButtonState[1] == FL_OVERVIEW_UP &&
        _mouseButtonState[2] == FL_OVERVIEW_UP) {

      // Callback
      if (_updateRenderCallback != NULL) _updateRenderCallback();
    }

    // Update mouse position
    _mousePosition[0] = Fl::event_x();
    _mousePosition[1] = Fl::event_y();

    // Return
    return 1;

  }


  case FL_KEYDOWN: {

    // Get key
    int key = Fl::event_key();

    // Check which key
    if (key == 'R' || key == 'r') {

      // Reset rotation
      _worldRotate[0] = 0.0;
      _worldRotate[1] = 0.0;

      // Reset scale
      _worldScale[0] = 1.0;
      _worldScale[1] = 1.0;
      _worldScale[2] = 1.0;

      // Reset translation
      _worldTranslate[0] = 0.0;
      _worldTranslate[1] = 0.0;
      _worldTranslate[2] = 0.0;

      // Calculate rotation matrix
      float R[16];
      float Rx[16];
      float Ry[16];
      float Rz[16];
      calc_rot_x(Rx, _worldRotate[0]);
      calc_rot_y(Ry, _worldRotate[1]);
      calc_rot_z(Rz, _worldRotate[2]);
      mult4x4(R, Rx, Ry);
      mult4x4(R, R, Rz);

      // Set rotation matrix
      SetRotationMatrix(R);

      // Calcuate scale matrix
      float S[16];
      calc_scale(S, _worldScale[0], _worldScale[1], _worldScale[2]);

      // Set scale matrix
      SetScaleMatrix(S);

      // Calculate translation matrix
      float T[16];
      calc_trans(T, 
                 _worldTranslate[0], 
                 _worldTranslate[1], 
                 _worldTranslate[2]);

      // Set translation matrix
      SetTranslationMatrix(T);

      // Callbacks
      if (_updateRotationCallback != NULL) _updateRotationCallback(_R);
      if (_updateScaleCallback != NULL) _updateScaleCallback(_S);
      if (_updateTranslationCallback != NULL) _updateTranslationCallback(_T);
      if (_updateRenderCallback != NULL) _updateRenderCallback();
      
      // Draw scene
      redraw();
      
      // Swap buffers
      swap_buffers();

      // Return
      return 1;

    }

  }


  default: {

    // Pass other events to the base class
    return Fl_Gl_Window::handle(event);

  }


  }

}

/*--------------------------------------------------------------------------*/
