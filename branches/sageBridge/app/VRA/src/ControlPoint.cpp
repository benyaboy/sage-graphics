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

/*--------------------------------------------------------------------------*/

ControlPoint::ControlPoint() {

  // Initialize opacity
  _opacity = 0.0;

  // Initialize color
  _rgb[0] = 0.0;
  _rgb[1] = 0.0;
  _rgb[2] = 0.0;

  // Initialize scalar
  _scalar = 0.0;

}

/*--------------------------------------------------------------------------*/

ControlPoint::ControlPoint(const ControlPoint &copyin) {

  // Copy opacity
  _opacity = copyin._opacity;

  // Copy color
  _rgb[0] = copyin._rgb[0];
  _rgb[1] = copyin._rgb[1];
  _rgb[2] = copyin._rgb[2];

  // Copy scalar
  _scalar = copyin._scalar;

}

/*--------------------------------------------------------------------------*/

ControlPoint::~ControlPoint() {
}

/*--------------------------------------------------------------------------*/

ControlPoint& ControlPoint::operator=(const ControlPoint &rhs) {

  // Assign opacity
  this -> _opacity = rhs._opacity;

  // Assign color
  this -> _rgb[0] = rhs._rgb[0];
  this -> _rgb[1] = rhs._rgb[1];
  this -> _rgb[2] = rhs._rgb[2];

  // Assign scalar
  this -> _scalar = rhs._scalar;

  // Return pointer to this
  return *this;

}

/*--------------------------------------------------------------------------*/
  
int ControlPoint::operator==(const ControlPoint &rhs) const {

  // Compare opacity members
  if (this -> _opacity != rhs._opacity) {
    return 0;
  }

  // Compare color members
  if (this -> _rgb[0] != rhs._rgb[0] ||
      this -> _rgb[1] != rhs._rgb[1] ||
      this -> _rgb[2] != rhs._rgb[2]) {
    return 0;
  }

  // Compare scalar members
  if (this -> _scalar != rhs._scalar) {
    return 0;
  }

  // Equality
  return 1;

}

/*--------------------------------------------------------------------------*/

int ControlPoint::operator<(const ControlPoint &rhs) const {

  // Compare scalar values
  if (this -> _scalar < rhs._scalar) {
    return 1;
  }

  // Not less than
  return 0;

}

/*--------------------------------------------------------------------------*/

float ControlPoint::GetOpacity() {

  // Return opacity
  return _opacity;

}

/*--------------------------------------------------------------------------*/

void ControlPoint::GetRGB(float* r, float* g, float* b) {

  // Get color
  *r = _rgb[0];
  *g = _rgb[1];
  *b = _rgb[2];

}

/*--------------------------------------------------------------------------*/

float ControlPoint::GetScalar() {

  // Return scalar
  return _scalar;

}

/*--------------------------------------------------------------------------*/

void ControlPoint::SetOpacity(float opacity) {

  // Set opacity
  _opacity = opacity;

}
/*--------------------------------------------------------------------------*/

void ControlPoint::SetRGB(float r, float g, float b) {

  // Set color
  _rgb[0] = r;
  _rgb[1] = g;
  _rgb[2] = b;

}

/*--------------------------------------------------------------------------*/
 
void ControlPoint::SetScalar(float scalar) {

  // Set scalar
  _scalar = scalar;

}

/*--------------------------------------------------------------------------*/
