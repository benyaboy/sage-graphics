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

#ifndef TRACKBALL_H
#define TRACKBALL_H

/*--------------------------------------------------------------------------*/

#include <math.h>

/*--------------------------------------------------------------------------*/

class Trackball {

public:

  // Default constructor
  Trackball();

  // Default destructor
  ~Trackball();

  // Clear
  void Clear();

  // Get the axis and angle for the current rotation
  void GetAxisAngle(float* angle, float axis[3]);

  // Build a rotation matrix from the current quaternion rotation
  void GetRotationMatrix(float m[16]);

  // Compute a quaternion based on an axis (defined by the given vector) and 
  // an angle about which to rotate. The angle is expressed in radians. The 
  // result is put into the third argument.
  void Reapply();

  // Start rotation.
  // Input is in the range (-1.0, 1.0)
  void Start(float x, float y);

  // Project the points onto the virtual trackball, then figure out the axis 
  // of rotation, which is the cross product of P1 P2 and O P1 (O is the center
  // of the ball, 0,0,0)
  // Note:  This is a deformed trackball -- this is a trackball in the center,
  // but it is deformed into a hyperbolic sheet of rotation away from the
  // center.
  // Input is in the range (-1.0, 1.0)
  void Update(float x, float y);

private:

  // Given two rotations, e1 and e2, expressed as quaternion rotations,
  // figure out the equivalent single rotation and stuff it into dest.
  // This routine also normalizes the result every RENORMCOUNT times it is
  // called, to keep error from creeping in.
  // NOTE: This routine is written so that q1 or q2 may be the same
  // as dest (or each other).
  void AddQuaternions(float q1[4], float q2[4], float dest[4]);

  // Create quaternion based on axis and angle
  void AxisToQuaternion(float a[3], float phi, float q[4]);

  // Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
  // If they don't add up to 1.0, dividing by their magnitued will
  // renormalize them.
  //
  // Note: See the following for more information on quaternions:
  // - Shoemake, K., Animating rotation with quaternion curves, Computer
  //   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
  // - Pletinckx, D., Quaternion calculus as a basic tool in computer
  //   graphics, The Visual Computer 5, 2-13, 1989.
  void NormalizeQuaternion(float q[4]);

  // Project an x, y pair on to a sphere of radius r or a hyperbolic 
  // sheet if we are away from the center of the sphere
  float ProjectToSphere(float r, float x, float y);

  // Add two vectors
  void vAdd(const float* src1, const float* src2, float* dst);

  // Copy two vectors
  void vCopy(const float* v1, float* v2);

  // Compute cross product of two vectors
  void vCross(const float* v1, const float* v2, float* cross);

  // Compute dot produce of two vectors
  float vDot(const float* v1, const float* v2);

  // Compute length of a vector
  float vLength(const float *v);

  // Normalize vector
  void vNormal(float* v);

  // Scale a vector
  void vScale(float* v, float div);

  // Set a vector
  void vSet(float* v, float x, float y, float z);

  // Subtract two vectors
  void vSub(const float* src1, const float* src2, float* dst);

  // Zero out a vector
  void vZero(float* v);

  // This size should really be based on the distance from the center of
  // rotation to the point on the object underneath the mouse. That
  // point would then track the mouse as closely as possible.
  float _trackballSize;
  
  // Number of iterations before renormalizing
  int _renormalizeCount;

  // Current quaternion
  float _currentQuaternion[4];

  // Last quaternion
  float _lastQuaternion[4];

  // Last X position
  float _lastX;

  // Last Y position
  float _lastY;

  // Axis of rotation
  float	_axis[3];

  // Amount to rotate about axis of roatation
  float	_phi;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
