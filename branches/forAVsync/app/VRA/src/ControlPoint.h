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

#ifndef FL_CONTROL_POINT_H
#define FL_CONTROL_POINT_H

/*--------------------------------------------------------------------------*/

class ControlPoint {

public:

  // Default constructor
  ControlPoint();

  // Overloaded constructor
  ControlPoint(const ControlPoint &copyin);

  // Default destructor
  ~ControlPoint();

  // Overloaded operator=
  ControlPoint& operator=(const ControlPoint &rhs);

  // Overloaded operator==
  int operator==(const ControlPoint &rhs) const;

  // Overloaded operator<
  int operator<(const ControlPoint &rhs) const;

  // Get opacity
  float GetOpacity();

  // Get color
  void GetRGB(float* r, float* g, float* b);

  // Get scalar
  float GetScalar();

  // Set opacity
  void SetOpacity(float opacity);

  // Set color
  void SetRGB(float r, float g, float b);

  // Set scalar
  void SetScalar(float scalar);
  
private:

  // Opacity value
  float _opacity;

  // Color value
  float _rgb[3];

  // Scalar value
  float _scalar;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
