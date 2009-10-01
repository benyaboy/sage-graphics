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

#include "Vertex.h"

/*--------------------------------------------------------------------------*/

Vertex::Vertex() {

  // Initialize vertex
  _xyz[0] = 0.0;
  _xyz[1] = 0.0;
  _xyz[2] = 0.0;
  
}

/*--------------------------------------------------------------------------*/

Vertex::Vertex(double x, double y, double z) {
  
  // Initialize vertex
  _xyz[0] = (float) x;
  _xyz[1] = (float) y;
  _xyz[2] = (float) z;
  
}

/*--------------------------------------------------------------------------*/

Vertex::Vertex(float x, float y, float z) {
  
  // Initialize vertex
  _xyz[0] = x;
  _xyz[1] = y;
  _xyz[2] = z;
  
}

/*--------------------------------------------------------------------------*/

Vertex::Vertex(double xyz[3]) {
  
  // Initialize vertex
  _xyz[0] = (float) xyz[0];
  _xyz[1] = (float) xyz[1];
  _xyz[2] = (float) xyz[2];
  
}

/*--------------------------------------------------------------------------*/

Vertex::Vertex(float xyz[3]) {
  
  // Initialize vertex
  _xyz[0] = xyz[0];
  _xyz[1] = xyz[1];
  _xyz[2] = xyz[2];
  
}

/*--------------------------------------------------------------------------*/

Vertex::~Vertex() {
}

/*--------------------------------------------------------------------------*/

float Vertex::GetX() {

  // Return X
  return _xyz[0];
  
}

/*--------------------------------------------------------------------------*/

float Vertex::GetY() {
  
  // Return Y
  return _xyz[1];
  
}

/*--------------------------------------------------------------------------*/

float Vertex::GetZ() {
  
  // Return Z
  return _xyz[2];
  
}

/*--------------------------------------------------------------------------*/

void Vertex::GetXYZ(double* x, double* y, double* z) {
  
  // Get X, Y, Z
  *x = (double) _xyz[0];
  *y = (double) _xyz[1];
  *z = (double) _xyz[2];
  
}

/*--------------------------------------------------------------------------*/

void Vertex::GetXYZ(float* x, float* y, float* z) {
  
  // Get X, Y, Z
  *x = _xyz[0];
  *y = _xyz[1];
  *z = _xyz[2];
  
}

/*--------------------------------------------------------------------------*/

void Vertex::GetXYZ(double xyz[3]) {
  
  // Get X, Y, Z
  xyz[0] = (double) _xyz[0];
  xyz[1] = (double) _xyz[1];
  xyz[2] = (double) _xyz[2];
  
}

/*--------------------------------------------------------------------------*/

void Vertex::GetXYZ(float xyz[3]) {
  
  // Get X, Y, Z
  xyz[0] = _xyz[0];
  xyz[1] = _xyz[1];
  xyz[2] = _xyz[2];
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetX(double x) {
  
  // Set X
  _xyz[0] = (float) x;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetX(float x) {
  
  // Set X
  _xyz[0] = x;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetY(double y) {
  
  // Set Y
  _xyz[1] = (float) y;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetY(float y) {
  
  // Set Y
  _xyz[1] = y;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetZ(double z) {
  
  // Set Z
  _xyz[2] = (float) z;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetZ(float z) {
  
  // Set Z
  _xyz[2] = z;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetXYZ(double x, double y, double z) {
  
  // Set X, Y, Z
  _xyz[0] = (float) x;
  _xyz[1] = (float) y;
  _xyz[2] = (float) z;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetXYZ(float x, float y, float z) {
  
  // Set X, Y, Z
  _xyz[0] = x;
  _xyz[1] = y;
  _xyz[2] = z;
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetXYZ(double xyz[3]) {
  
  // Set X, Y, Z
  _xyz[0] = (float) xyz[0];
  _xyz[1] = (float) xyz[1];
  _xyz[2] = (float) xyz[2];
  
}

/*--------------------------------------------------------------------------*/

void Vertex::SetXYZ(float xyz[3]) {
  
  // Set X, Y, Z
  _xyz[0] = xyz[0];
  _xyz[1] = xyz[1];
  _xyz[2] = xyz[2];
  
}

/*--------------------------------------------------------------------------*/
