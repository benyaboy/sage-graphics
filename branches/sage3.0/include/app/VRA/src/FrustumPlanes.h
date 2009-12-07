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

#ifndef FRUSTUM_PLANES_H
#define FRUSTUM_PLANES_H

/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "Matrix.h"

/*--------------------------------------------------------------------------*/

#define LEFT_PLANE 0
#define RIGHT_PLANE 1
#define TOP_PLANE 2
#define BOTTOM_PLANE 3
#define NEAR_PLANE 4
#define FAR_PLANE 5
#define INSIDE_PLANE 1
#define INTERSECT_PLANE 0
#define OUTSIDE_PLANE -1
#define INSIDE_FRUSTUM 1
#define INTERSECT_FRUSTUM 0
#define OUTSIDE_FRUSTUM -1

/*--------------------------------------------------------------------------*/

class FrustumPlanes {

 public:
  
  // Default constructor
  FrustumPlanes();

  // Default destructor
  ~FrustumPlanes();

  // Calculate the distance from a plane to a point
  float CalculateDistanceToFrustumPlane(int plane, float x, float y, float z);

  // Calculate each plane of the frustum
  void CalculateFrustumPlanes();

  // Calculate a cube's relation to the frustum
  int CalculateRelationToFrustum(float x0, float y0, float z0,
                                 float x1, float y1, float z1,
                                 float x2, float y2, float z2,
                                 float x3, float y3, float z3,
                                 float x4, float y4, float z4,
                                 float x5, float y5, float z5,
                                 float x6, float y6, float z6,
                                 float x7, float y7, float z7);

  // Calculate a plane's relation to the frustum
  int CalculateRelationToFrustumPlane(int plane, float x, float y, float z);

  // Set ModelView matrix
  void SetModelViewMatrix(float mv[16]);

  // Set Projection matrix
  void SetProjectionMatrix(float p[16]);
  
 private:

  // ModelView matrix
  float _mv[16];

  // ModelView-Projection matrix
  float _mvp[16];

  // Projection matrix
  float _p[16];

  // Frustum planes
  float _planes[6][4];
  
};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
