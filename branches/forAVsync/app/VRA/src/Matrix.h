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

#ifndef MATRIX_H
#define MATRIX_H

/*--------------------------------------------------------------------------*/

#include <math.h>

/*--------------------------------------------------------------------------*/

// Calculate rotation matrix about x axis
void calc_rot_x(float matrix[16], float angle);

// Calculate rotation matrix about y axis
void calc_rot_y(float matrix[16], float angle);

// Calculate rotation matrix about z axis
void calc_rot_z(float matrix[16], float angle);

// Calculate scale matrix
void calc_scale(float matrix[16], float x, float y, float z);

// Calculate translation matrix
void calc_trans(float matrix[16], float x, float y, float z);

// Calculate inverse matrix
void inverse4x4(double out[16], double in[16]);

// Calculate inverse matrix
void inverse4x4(float out[16], float in[16]);

// Return the maximum value
double maximum(double in1, double in2);

// Return the minimum value
double minimum(double in1, double in2);

// Multiply a 4x4 matrix by a 4x4 matrix
void mult4x4(float out[16], float one[16], float two[16]);

// Normalize the input vector
void normalize3V(double in[3]);

// Subtract the vectors
// out = in1 - in2
void sub3V(double out[3], double in1[3], double in2[3]);

// Translate in1 by in2
void translate3V(double out[3], double in1[16], double in2[3]);

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
