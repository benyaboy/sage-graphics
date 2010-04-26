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

#include "FrustumCalculator.h"

/*--------------------------------------------------------------------------*/

FrustumCalculator::FrustumCalculator() {

  // Initialize display size
  _displayHeight = 0;
  _displayWidth = 0;

  // Initialize frustum
  _frustum[0] = 0.0;
  _frustum[1] = 0.0;
  _frustum[2] = 0.0;
  _frustum[3] = 0.0;

  // Initialize near clipping plane
  _nearClippingPlane = 0.0;

  _percentage[0] = 0.0;
  _percentage[1] = 0.0;
  _percentage[2] = 0.0;
  _percentage[3] = 0.0;

  // Initialize vertical field of view
  _verticalFieldOfView = 0.0;

}

/*--------------------------------------------------------------------------*/

FrustumCalculator::~FrustumCalculator() {
}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::CalculateFrustum() {

  // Calculate aspect ratio of display
  float aspect = (float) _displayWidth / (float) _displayHeight;

  // Calculate half frustums
  float half_x = fabs(aspect * 
                      _nearClippingPlane * 
                      tanf(_verticalFieldOfView * 0.0174532 / 2));
  float half_y = fabs(_nearClippingPlane * 
                      tanf(_verticalFieldOfView * 0.0174532 / 2));

  // Calculate full frsutums
  float full_x = 2 * half_x;
  float full_y = 2 * half_y;

  // Calculate frustum based on percentage
  _frustum[0] = -half_x + (_percentage[0] * full_x);
  _frustum[1] = -half_x + (_percentage[1] * full_x);
  _frustum[2] = -half_y + (_percentage[2] * full_y);
  _frustum[3] = -half_y + (_percentage[3] * full_y);

  // Calculate full frustum for entire display
  _fullFrustum[0] = -half_x;
  _fullFrustum[1] = half_x;
  _fullFrustum[2] = -half_y;
  _fullFrustum[3] = half_y;

}

/*--------------------------------------------------------------------------*/

int FrustumCalculator::GetDisplayHeight() {

  // Return display height
  return _displayHeight;

}

/*--------------------------------------------------------------------------*/

int FrustumCalculator::GetDisplayWidth() {

  // Return display width
  return _displayWidth;

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::GetFullFrustum(float* left, float* right,
                                       float* bottom, float* top) {

  // Get full frustum
  *left = _fullFrustum[0];
  *right = _fullFrustum[1];
  *bottom = _fullFrustum[2];
  *top = _fullFrustum[3];

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::GetFullFrustum(float array[4]) {

  // Get full frustum
  array[0] = _fullFrustum[0];
  array[1] = _fullFrustum[1];
  array[2] = _fullFrustum[2];
  array[3] = _fullFrustum[3];

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::GetFrustum(float* left, float* right,
                                   float* bottom, float* top) {

  // Get frustum
  *left = _frustum[0];
  *right = _frustum[1];
  *bottom = _frustum[2];
  *top = _frustum[3];

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::GetFrustum(float array[4]) {

  // Get frustum
  array[0] = _frustum[0];
  array[1] = _frustum[1];
  array[2] = _frustum[2];
  array[3] = _frustum[3];

}

/*--------------------------------------------------------------------------*/

float FrustumCalculator::GetNearClippingPlane() {

  // Return near clipping plane
  return _nearClippingPlane;

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::GetPercentage(float* left, float* right,
                                      float* bottom, float* top) {

  // Get display percentage
  *left = _percentage[0];
  *right = _percentage[1];
  *bottom = _percentage[2];
  *top = _percentage[3];

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::GetPercentage(float array[4]) {

  // Get display percentage
  array[0] = _percentage[0];
  array[1] = _percentage[1];
  array[2] = _percentage[2];
  array[3] = _percentage[3];

}

/*--------------------------------------------------------------------------*/

float FrustumCalculator::GetVerticalFieldOfView() {

  // Return vFOV
  return _verticalFieldOfView;

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::SetDisplayHeight(int pixels) {

  // Set display height
  _displayHeight = pixels;

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::SetDisplayWidth(int pixels) {

  // Set display width
  _displayWidth = pixels;

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::SetNearClippingPlane(float near) {

  // Set near clipping plane
  _nearClippingPlane = near;

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::SetPercentage(float left, float right, 
                                      float bottom, float top) {

  // Set display percentage
  _percentage[0] = left;
  _percentage[1] = right;
  _percentage[2] = bottom;
  _percentage[3] = top;

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::SetPercentage(float array[4]) {

  // Set display percentage
  _percentage[0] = array[0];
  _percentage[1] = array[1];
  _percentage[2] = array[2];
  _percentage[3] = array[3];

}

/*--------------------------------------------------------------------------*/

void FrustumCalculator::SetVerticalFieldOfView(float angle) {

  // Set vFOV
  _verticalFieldOfView = angle;

}

/*--------------------------------------------------------------------------*/
