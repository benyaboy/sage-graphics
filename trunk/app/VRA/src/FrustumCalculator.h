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

#ifndef VOLUME_FRUSTUM_CALCULATOR_H
#define VOLUME_FRUSTUM_CALCULATOR_H

/*--------------------------------------------------------------------------*/

#include <math.h>

/*--------------------------------------------------------------------------*/

class FrustumCalculator {

 public:

  // Constructor
  FrustumCalculator();

  // Destructor
  ~FrustumCalculator();

  // Calculate frustum based on percentage
  void CalculateFrustum();

  // Get display height in pixels
  int GetDisplayHeight();

  // Get display width in pixels
  int GetDisplayWidth();

  // Get full frustum for display (call CalculateFrustum to compute)
  void GetFullFrustum(float* left, float* right,
                      float* bottom, float* top);

  // Get full frustum for display (call CalculateFrustum to compute)
  void GetFullFrustum(float array[4]);

  // Get frustum (call CalculateFrustum to compute the frustum)
  void GetFrustum(float* left, float* right,
                  float* bottom, float* top);

  // Get frustum (call CalculateFrustum to compute the frustum)
  void GetFrustum(float array[4]);

  // Get near clipping plane
  float GetNearClippingPlane();

  // Get the percentage of the display for the frustum
  void GetPercentage(float* left, float* right,
                     float* bottom, float* top);

  // Get the percentage of the display for the frustum
  void GetPercentage(float array[4]);

  // Get the vertical field of view as an angle
  float GetVerticalFieldOfView();

  // Set display height in pixels
  void SetDisplayHeight(int pixels);

  // Set display width in pixels
  void SetDisplayWidth(int pixels);

  // Set near clipping plane
  void SetNearClippingPlane(float near);

  // Set the percentage of the display for the frustum
  void SetPercentage(float left, float right, float bottom, float top);

  // Set the percentage of the display for the frustum
  void SetPercentage(float array[4]);

  // Set the vertical field of view as an angle
  void SetVerticalFieldOfView(float angle);

 private:

  // Height of display in pixels
  int _displayHeight;

  // Width of display in pixels
  int _displayWidth;

  // View frustum as left, right, bottom, top
  float _frustum[4];

  // Full view frustum for display
  float _fullFrustum[4];

  // Near clipping plane
  float _nearClippingPlane;

  // Percentage of the display for the frustum
  float _percentage[4];

  // Vertical field of view as an angle
  float _verticalFieldOfView;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
