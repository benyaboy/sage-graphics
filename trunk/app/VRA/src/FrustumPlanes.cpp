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

#include "FrustumPlanes.h"

/*--------------------------------------------------------------------------*/

FrustumPlanes::FrustumPlanes() {

  // Initialize MV
  _mv[0] = 1.0; _mv[4] = 0.0; _mv[8] = 0.0; _mv[12] = 0.0;
  _mv[1] = 0.0; _mv[5] = 1.0; _mv[9] = 0.0; _mv[13] = 0.0;
  _mv[2] = 0.0; _mv[6] = 0.0; _mv[10] = 1.0; _mv[14] = 0.0;
  _mv[3] = 0.0; _mv[7] = 0.0; _mv[11] = 0.0; _mv[15] = 1.0;
  
  // Initialize MVP
  _mvp[0] = 1.0; _mvp[4] = 0.0; _mvp[8] = 0.0; _mvp[12] = 0.0;
  _mvp[1] = 0.0; _mvp[5] = 1.0; _mvp[9] = 0.0; _mvp[13] = 0.0;
  _mvp[2] = 0.0; _mvp[6] = 0.0; _mvp[10] = 1.0; _mvp[14] = 0.0;
  _mvp[3] = 0.0; _mvp[7] = 0.0; _mvp[11] = 0.0; _mvp[15] = 1.0;
  
  // Initialize P
  _p[0] = 1.0; _p[4] = 0.0; _p[8] = 0.0; _p[12] = 0.0;
  _p[1] = 0.0; _p[5] = 1.0; _p[9] = 0.0; _p[13] = 0.0;
  _p[2] = 0.0; _p[6] = 0.0; _p[10] = 1.0; _p[14] = 0.0;
  _p[3] = 0.0; _p[7] = 0.0; _p[11] = 0.0; _p[15] = 1.0;

  // Initialize planes
  _planes[0][0] = 0.0;
  _planes[0][1] = 0.0;
  _planes[0][2] = 0.0;
  _planes[0][3] = 0.0;

  _planes[1][0] = 0.0;
  _planes[1][1] = 0.0;
  _planes[1][2] = 0.0;
  _planes[1][3] = 0.0;
  
  _planes[2][0] = 0.0;
  _planes[2][1] = 0.0;
  _planes[2][2] = 0.0;
  _planes[2][3] = 0.0;
  
  _planes[3][0] = 0.0;
  _planes[3][1] = 0.0;
  _planes[3][2] = 0.0;
  _planes[3][3] = 0.0;
  
  _planes[4][0] = 0.0;
  _planes[4][1] = 0.0;
  _planes[4][2] = 0.0;
  _planes[4][3] = 0.0;
  
  _planes[5][0] = 0.0;
  _planes[5][1] = 0.0;
  _planes[5][2] = 0.0;
  _planes[5][3] = 0.0;
  
}

/*--------------------------------------------------------------------------*/

FrustumPlanes::~FrustumPlanes() {
}

/*--------------------------------------------------------------------------*/

float FrustumPlanes::CalculateDistanceToFrustumPlane(int plane, float x,
                                                     float y, float z) {

  // Distnace to plane
  float distance = 0.0;
  
  // Calculate distance
  distance = _planes[plane][0] * x +
    _planes[plane][1] * y +
    _planes[plane][2] * z +
    _planes[plane][3];
  
  // Return distance
  return distance;
  
}

/*--------------------------------------------------------------------------*/

void FrustumPlanes::CalculateFrustumPlanes() {
  
  // normalize value
  float n = 0.0;

  // Calculate a combined MVP from MV and P
  mult4x4(_mvp, _p, _mv);
  

  // Extract the frustum's left clipping plane and normalize it.
  _planes[0][0] = _mvp[3] + _mvp[0];
  _planes[0][1] = _mvp[7] + _mvp[4];
  _planes[0][2] = _mvp[11] + _mvp[8];
  _planes[0][3] = _mvp[15] + _mvp[12];
  
  n = (float) sqrt(_planes[0][0] * _planes[0][0] +
                   _planes[0][1] * _planes[0][1] +
                   _planes[0][2] * _planes[0][2]);
  
  _planes[0][0] /= n;
  _planes[0][1] /= n;
  _planes[0][2] /= n;
  _planes[0][3] /= n;
  

  // Extract the frustum's right clipping plane and normalize it.
  _planes[1][0] = _mvp[3] - _mvp[0];
  _planes[1][1] = _mvp[7] - _mvp[4];
  _planes[1][2] = _mvp[11] - _mvp[8];
  _planes[1][3] = _mvp[15] - _mvp[12];
  
  n = (float) sqrt(_planes[1][0] * _planes[1][0] +
                   _planes[1][1] * _planes[1][1] +
                   _planes[1][2] * _planes[1][2]);
  
  _planes[1][0] /= n;
  _planes[1][1] /= n;
  _planes[1][2] /= n;
  _planes[1][3] /= n;


  // Extract the frustum's bottom clipping plane and normalize it.
  _planes[2][0] = _mvp[3] + _mvp[1];
  _planes[2][1] = _mvp[7] + _mvp[5];
  _planes[2][2] = _mvp[11] + _mvp[9];
  _planes[2][3] = _mvp[15] + _mvp[13];

  n = (float) sqrt(_planes[2][0] * _planes[2][0] +
                   _planes[2][1] * _planes[2][1] +
                   _planes[2][2] * _planes[2][2]);
  
  _planes[2][0] /= n;
  _planes[2][1] /= n;
  _planes[2][2] /= n;
  _planes[2][3] /= n;
  

  // Extract the frustum's top clipping plane and normalize it.
  _planes[3][0] = _mvp[3] - _mvp[1];
  _planes[3][1] = _mvp[7] - _mvp[5];
  _planes[3][2] = _mvp[11] - _mvp[9];
  _planes[3][3] = _mvp[15] - _mvp[13];
  
  n = (float) sqrt(_planes[3][0] * _planes[3][0] +
                   _planes[3][1] * _planes[3][1] +
                   _planes[3][2] * _planes[3][2]);
  
  _planes[3][0] /= n;
  _planes[3][1] /= n;
  _planes[3][2] /= n;
  _planes[3][3] /= n;


  // Extract the frustum's far clipping plane and normalize it.
  _planes[4][0] = _mvp[3] - _mvp[2];
  _planes[4][1] = _mvp[7] - _mvp[6];
  _planes[4][2] = _mvp[11] - _mvp[10];
  _planes[4][3] = _mvp[15] - _mvp[14];

  n = (float) sqrt(_planes[4][0] * _planes[4][0] +
                   _planes[4][1] * _planes[4][1] +
                   _planes[4][2] * _planes[4][2]);
  
  _planes[4][0] /= n;
  _planes[4][1] /= n;
  _planes[4][2] /= n;
  _planes[4][3] /= n;
    

  // Extract the frustum's near clipping plane and normalize it.
  _planes[5][0] = _mvp[3] + _mvp[2];
  _planes[5][1] = _mvp[7] + _mvp[6];
  _planes[5][2] = _mvp[11] + _mvp[10];
  _planes[5][3] = _mvp[15] + _mvp[14];
  
  n = (float) sqrt(_planes[5][0] * _planes[5][0] +
                   _planes[5][1] * _planes[5][1] +
                   _planes[5][2] * _planes[5][2] );
  
  _planes[5][0] /= n;
  _planes[5][1] /= n;
  _planes[5][2] /= n;
  _planes[5][3] /= n;
  
}

/*--------------------------------------------------------------------------*/

int FrustumPlanes::CalculateRelationToFrustum(float x0, float y0, float z0,
                                              float x1, float y1, float z1,
                                              float x2, float y2, float z2,
                                              float x3, float y3, float z3,
                                              float x4, float y4, float z4,
                                              float x5, float y5, float z5,
                                              float x6, float y6, float z6,
                                              float x7, float y7, float z7) {

  // Total points
  int total = 0;

  // Determine whether a set of vertices are inside, outside or intersect
  for (int plane = 0 ; plane < 6 ; plane++) {
    
    int count = 8;
    int ptIn = 1;
    
    if (CalculateDistanceToFrustumPlane(plane, x0, y0, z0) < 0) {
      ptIn = 0;
      --count;
    }
    
    if (CalculateDistanceToFrustumPlane(plane, x1, y1, z1) < 0) {
      ptIn = 0;
      --count;
    }
    
    if (CalculateDistanceToFrustumPlane(plane, x2, y2, z2) < 0) {
      ptIn = 0;
      --count;
    }
    
    if (CalculateDistanceToFrustumPlane(plane, x3, y3, z3) < 0) {
      ptIn = 0;
      --count;
    }

    if (CalculateDistanceToFrustumPlane(plane, x4, y4, z4) < 0) {
      ptIn = 0;
      --count;
    }
    
    if (CalculateDistanceToFrustumPlane(plane, x5, y5, z5) < 0) {
      ptIn = 0;
      --count;
    }
    
    if (CalculateDistanceToFrustumPlane(plane, x6, y6, z6) < 0) {
      ptIn = 0;
      --count;
    }

    if (CalculateDistanceToFrustumPlane(plane, x7, y7, z7) < 0) {
      ptIn = 0;
      --count;
    }

    // all points outside plane
    if (count == 0) {
      return OUTSIDE_FRUSTUM;
    }
    
    total += ptIn;
    
  }
  
  // all points inside view
  if (total == 6) {
    return INSIDE_FRUSTUM;
  }
  
  // points intersect intersect frustum
  return INTERSECT_FRUSTUM;
  
}

/*--------------------------------------------------------------------------*/

int FrustumPlanes::CalculateRelationToFrustumPlane(int plane, float x,
                                                   float y, float z) {
  
  // x, y, and z should be world points
  // not multiplied by mvp, or mv, or p
  float distance = CalculateDistanceToFrustumPlane(plane, x, y, z);
  
  if (distance == 0) return INTERSECT_PLANE;
  else if (distance > 0) return INSIDE_PLANE;
  else return OUTSIDE_PLANE;
  
}

/*--------------------------------------------------------------------------*/

void FrustumPlanes::SetModelViewMatrix(float mv[16]) {

  // Set MV
  _mv[0] = mv[0]; _mv[4] = mv[4]; _mv[8] = mv[8]; _mv[12] = mv[12];
  _mv[1] = mv[1]; _mv[5] = mv[5]; _mv[9] = mv[9]; _mv[13] = mv[13];
  _mv[2] = mv[2]; _mv[6] = mv[6]; _mv[10] = mv[10]; _mv[14] = mv[14];
  _mv[3] = mv[3]; _mv[7] = mv[7]; _mv[11] = mv[11]; _mv[15] = mv[15];
  
}

/*--------------------------------------------------------------------------*/

void FrustumPlanes::SetProjectionMatrix(float p[16]) {

  // Set P
  _p[0] = p[0]; _p[4] = p[4]; _p[8] = p[8]; _p[12] = p[12];
  _p[1] = p[1]; _p[5] = p[5]; _p[9] = p[9]; _p[13] = p[13];
  _p[2] = p[2]; _p[6] = p[6]; _p[10] = p[10]; _p[14] = p[14];
  _p[3] = p[3]; _p[7] = p[7]; _p[11] = p[11]; _p[15] = p[15];
  
}

/*--------------------------------------------------------------------------*/
