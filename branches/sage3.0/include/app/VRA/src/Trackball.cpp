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
/*                                                                          */
/*                                                                          */
/* (c) Copyright 1993, 1994, Silicon Graphics, Inc.                         */
/* ALL RIGHTS RESERVED                                                      */
/* Permission to use, copy, modify, and distribute this software for        */
/* any purpose and without fee is hereby granted, provided that the above   */
/* copyright notice appear in all copies and that both the copyright notice */
/* and this permission notice appear in supporting documentation, and that  */
/* the name of Silicon Graphics, Inc. not be used in advertising            */
/* or publicity pertaining to distribution of the software without specific,*/
/* written prior permission.                                                */
/*                                                                          */
/* THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"        */
/* AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,         */
/* INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR         */
/* FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON             */
/* GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,          */
/* SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY            */
/* KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,           */
/* LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF        */
/* THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN           */
/* ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON           */
/* ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE        */
/* POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.                         */
/*                                                                          */
/* US Government Users Restricted Rights                                    */
/* Use, duplication, or disclosure by the Government is subject to          */
/* restrictions set forth in FAR 52.227.19(c)(2) or subparagraph            */
/* (c)(1)(ii) of the Rights in Technical Data and Computer Software         */
/* clause at DFARS 252.227-7013 and/or in similar or successor              */
/* clauses in the FAR or the DOD or NASA FAR Supplement.                    */
/* Unpublished-- rights reserved under the copyright laws of the            */
/* United States.  Contractor/manufacturer is Silicon Graphics,             */
/* Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.            */
/*                                                                          */
/* OpenGL(TM) is a trademark of Silicon Graphics, Inc.                      */
/*--------------------------------------------------------------------------*/

#include "Trackball.h"

/*--------------------------------------------------------------------------*/

Trackball::Trackball() {

  // Initialize last x, y position
  _lastX = 0.0;
  _lastY = 0.0;

  // Initialize renormalize count
  _renormalizeCount = 97;

  // Initialize current quaternion
  _currentQuaternion[0] = 0.0;
  _currentQuaternion[1] = 0.0;
  _currentQuaternion[2] = 0.0;
  _currentQuaternion[3] = 1.0;

  // Initialize last quaternion
  _lastQuaternion[0] = 0.0;
  _lastQuaternion[1] = 0.0;
  _lastQuaternion[2] = 0.0;
  _lastQuaternion[3] = 1.0;

  // Initialize trackball size
  _trackballSize = (float) 0.5;

}

/*--------------------------------------------------------------------------*/

Trackball::~Trackball() {
}

/*--------------------------------------------------------------------------*/

void Trackball::AddQuaternions(float q1[4], float q2[4], float dest[4]) {

  static int count = 0;

  float t1[4];
  float t2[4];
  float t3[4];
  float tf[4];

  vCopy(q1, t1);
  vScale(t1, q2[3]);

  vCopy(q2, t2);
  vScale(t2, q1[3]);

  vCross(q2, q1, t3);
  vAdd(t1, t2, tf);
  vAdd(t3, tf, tf);
  tf[3] = q1[3] * q2[3] - vDot(q1, q2);

  dest[0] = tf[0];
  dest[1] = tf[1];
  dest[2] = tf[2];
  dest[3] = tf[3];
  
  if (++count > _renormalizeCount) {
    count = 0;
    NormalizeQuaternion(dest);
  }
  
}

/*--------------------------------------------------------------------------*/

void Trackball::AxisToQuaternion(float a[3], float phi, float q[4]) {

  vNormal(a);
  vCopy(a, q);
  vScale(q, (float) (sin(phi / 2.0)));
  q[3] = (float) (cos(phi / 2.0));

}

/*--------------------------------------------------------------------------*/

void Trackball::Clear() {

  // Reset last x, y position
  _lastX = 0.0;
  _lastY = 0.0;

  // Reset current quaternion
  _currentQuaternion[0] = 0.0;
  _currentQuaternion[1] = 0.0;
  _currentQuaternion[2] = 0.0;
  _currentQuaternion[3] = 1.0;

  // Reset last quaternion
  _lastQuaternion[0] = 0.0;
  _lastQuaternion[1] = 0.0;
  _lastQuaternion[2] = 0.0;
  _lastQuaternion[3] = 1.0;

}

/*--------------------------------------------------------------------------*/

void Trackball::GetAxisAngle(float* angle, float axis[3]) {

  // Computer angle
  *angle = _phi * 180.0f / 3.141517;

  // Set axis
  for (int i = 0 ; i < 3 ;i++) {
    axis[i] = _axis[i];
  }

}

/*--------------------------------------------------------------------------*/

void Trackball::GetRotationMatrix(float m[16]) {

  // Build rotation matrix given a quaternion rotation
  m[0] = (float)(1.0 - 2.0 * (_currentQuaternion[1] * _currentQuaternion[1] + 
                              _currentQuaternion[2] * _currentQuaternion[2]));
  m[1] = (float)(2.0 * (_currentQuaternion[0] * _currentQuaternion[1] - 
                        _currentQuaternion[2] * _currentQuaternion[3]));
  m[2] = (float)(2.0 * (_currentQuaternion[2] * _currentQuaternion[0] + 
                        _currentQuaternion[1] * _currentQuaternion[3]));
  m[3] = (float)(0.0);


  m[4] = (float)(2.0 * (_currentQuaternion[0] * _currentQuaternion[1] + 
                        _currentQuaternion[2] * _currentQuaternion[3]));
  m[5] = (float)(1.0 - 2.0 * (_currentQuaternion[2] * _currentQuaternion[2] + 
                              _currentQuaternion[0] * _currentQuaternion[0]));
  m[6] = (float)(2.0 * (_currentQuaternion[1] * _currentQuaternion[2] - 
                        _currentQuaternion[0] * _currentQuaternion[3]));
  m[7] = (float)(0.0);


  m[8] = (float)(2.0 * (_currentQuaternion[2] * _currentQuaternion[0] - 
                        _currentQuaternion[1] * _currentQuaternion[3]));
  m[9] = (float)(2.0 * (_currentQuaternion[1] * _currentQuaternion[2] + 
                        _currentQuaternion[0] * _currentQuaternion[3]));
  m[10] = (float)(1.0 - 2.0 * (_currentQuaternion[1] * _currentQuaternion[1] +
                               _currentQuaternion[0] * _currentQuaternion[0]));
  m[11] = (float)(0.0);


  m[12] = (float)(0.0);
  m[13] = (float)(0.0);
  m[14] = (float)(0.0);
  m[15] = (float)(1.0);

}

/*--------------------------------------------------------------------------*/

void Trackball::NormalizeQuaternion(float q[4]) {

    float mag = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);

    for (int i = 0 ; i < 4 ; i++) {
      q[i] /= mag;
    }

}

/*--------------------------------------------------------------------------*/

float Trackball::ProjectToSphere(float r, float x, float y) {

  float d;
  float t;
  float z;

  d = (float) (sqrt(x * x + y * y));

  // Inside sphere
  if (d < r * 0.70710678118654752440) {
    z = (float) (sqrt(r*r - d*d));
  } 

  // On hyperbola
  else {
    t = (float) (r / 1.41421356237309504880);
    z = t * t / d;
  }

  return z;

}

/*--------------------------------------------------------------------------*/

void Trackball::Reapply() {

  // Add quaternions
  AddQuaternions(_lastQuaternion, _currentQuaternion, _currentQuaternion);

}

/*--------------------------------------------------------------------------*/

void Trackball::Start(float x, float y) {

  _lastX = x;
  _lastY = y;
  vZero(_lastQuaternion);
  _lastQuaternion[3] = 1.0;

}

/*--------------------------------------------------------------------------*/

void Trackball::Update(float x, float y) {

  float p1[3];
  float p2[3];
  float d[3];
  float t;

  // If no movement then return
  if (_lastX == x && _lastY == y) {
    vZero(_lastQuaternion);
    _lastQuaternion[3] = 1.0;
    return;
  }

  // Figure out z-coordinates for projection of P1 and P2 to deformed sphere
  vSet(p1, _lastX, _lastY, ProjectToSphere(_trackballSize, _lastX, _lastY));
  vSet(p2, x, y, ProjectToSphere(_trackballSize, x, y));

  // Computer the cross product of P1 and P2
  vCross(p2, p1, _axis);

  // Determine amount to rotate around that axis
  vSub(p1, p2, d);
  t = (float)(vLength(d) / (2.0 * _trackballSize));

  // Avoid problems with out-of-control values.
  if (t > 1.0) t = 1.0;
  if (t < -1.0) t = -1.0;
  _phi = (float)(2.0 * asin(t));

  // Convert axis to quaternion
  AxisToQuaternion(_axis, _phi, _lastQuaternion);

  // Update last x, y position
  _lastX = x;
  _lastY = y;

  // Add quaternions
  AddQuaternions(_lastQuaternion, _currentQuaternion, _currentQuaternion);

}

/*--------------------------------------------------------------------------*/

void Trackball::vAdd(const float* src1, const float* src2, float* dst) {

  dst[0] = src1[0] + src2[0];
  dst[1] = src1[1] + src2[1];
  dst[2] = src1[2] + src2[2];

}

/*--------------------------------------------------------------------------*/

void Trackball::vCopy(const float* v1, float* v2) {

  v2[0] = v1[0];
  v2[1] = v1[1];
  v2[2] = v1[2];

}

/*--------------------------------------------------------------------------*/

void Trackball::vCross(const float* v1, const float* v2, float* cross) {

  float temp[3];

  temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);

  vCopy(temp, cross);

}

/*--------------------------------------------------------------------------*/

float Trackball::vDot(const float* v1, const float* v2) {

  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];

}

/*--------------------------------------------------------------------------*/

float Trackball::vLength(const float* v) {

  return (float) (sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));

}

/*--------------------------------------------------------------------------*/

void Trackball::vNormal(float* v) {

  vScale(v, (float) (1.0 / vLength(v)));

}

/*--------------------------------------------------------------------------*/

void Trackball::vScale(float* v, float div) {

  v[0] *= div;
  v[1] *= div;
  v[2] *= div;

}

/*--------------------------------------------------------------------------*/

void Trackball::vSet(float* v, float x, float y, float z) {

  v[0] = x;
  v[1] = y;
  v[2] = z;

}

/*--------------------------------------------------------------------------*/

void Trackball::vSub(const float* src1, const float* src2, float* dst) {

  dst[0] = src1[0] - src2[0];
  dst[1] = src1[1] - src2[1];
  dst[2] = src1[2] - src2[2];

}

/*--------------------------------------------------------------------------*/

void Trackball::vZero(float* v) {

  v[0] = 0.0;
  v[1] = 0.0;
  v[2] = 0.0;

}

/*--------------------------------------------------------------------------*/
