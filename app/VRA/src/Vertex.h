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

#ifndef VERTEX_H
#define VERTEX_H

/*--------------------------------------------------------------------------*/

#include <stdio.h>

/*--------------------------------------------------------------------------*/

class Vertex {

 public:

  // Default constructor
  Vertex();

  // Constructor
  Vertex(double x, double y, double z);

  // Constructor
  Vertex(float x, float y, float z);
  
  // Constructor
  Vertex(double xyz[3]);

  // Constructor
  Vertex(float xyz[3]);

  // Default destructor
  ~Vertex();

  // Get X
  float GetX();

  // Get Y
  float GetY();

  // Get Z
  float GetZ();

  // Get XYZ
  void GetXYZ(double* x, double* y, double* z);

  // Get XYZ
  void GetXYZ(float* x, float* y, float* z);

  // Get XYZ
  void GetXYZ(double xyz[3]);

  // Get XYZ
  void GetXYZ(float xyz[3]);

  // Set X
  void SetX(double x);

  // Set X
  void SetX(float x);

  // Set Y
  void SetY(double y);

  // Set Y
  void SetY(float y);

  // Set Z 
  void SetZ(double z);

  // Set Z 
  void SetZ(float z);

  // Set XYZ
  void SetXYZ(double x, double y, double z);

  // Set XYZ
  void SetXYZ(float x, float y, float z);

  // Set XYZ
  void SetXYZ(double xyz[3]);

  // Set XYZ
  void SetXYZ(float xyz[3]);
  
 private:
  
  // Vertex
  float _xyz[3];

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
