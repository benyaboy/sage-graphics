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

#include "Matrix.h"

/*--------------------------------------------------------------------------*/

void calc_rot_x(float matrix[16], float angle) {
  
  matrix[0] = 1.0;
  matrix[1] = 0.0;
  matrix[2] = 0.0;
  matrix[3] = 0.0;
  
  matrix[4] = 0.0;
  matrix[5] = cos(angle * 0.0174532);
  matrix[6] = sin(angle * 0.0174532);
  matrix[7] = 0.0;
  
  matrix[8] = 0.0;
  matrix[9] = -sin(angle * 0.0174532);
  matrix[10] = cos(angle * 0.0174532);
  matrix[11] = 0.0;
  
  matrix[12] = 0.0;
  matrix[13] = 0.0;
  matrix[14] = 0.0;
  matrix[15] = 1.0;
  
}

/*--------------------------------------------------------------------------*/

void calc_rot_y(float matrix[16], float angle) {
  
  matrix[0] = cos(angle * 0.0174532);
  matrix[1] = 0.0;
  matrix[2] = -sin(angle * 0.0174532);
  matrix[3] = 0.0;
  
  matrix[4] = 0.0;
  matrix[5] = 1.0;
  matrix[6] = 0.0;
  matrix[7] = 0.0;
  
  matrix[8] = sin(angle * 0.0174532);
  matrix[9] = 0.0;
  matrix[10] = cos(angle * 0.0174532);
  matrix[11] = 0.0;
  
  matrix[12] = 0.0;
  matrix[13] = 0.0;
  matrix[14] = 0.0;
  matrix[15] = 1.0;
  
}

/*--------------------------------------------------------------------------*/

void calc_rot_z(float matrix[16], float angle) {
  
  matrix[0] = cos(angle * 0.0174532);
  matrix[1] = sin(angle * 0.0174532);
  matrix[2] = 0.0;
  matrix[3] = 0.0;
  
  matrix[4] = -sin(angle * 0.0174532);
  matrix[5] = cos(angle * 0.0174532);
  matrix[6] = 0.0;
  matrix[7] = 0.0;
  
  matrix[8] = 0.0;
  matrix[9] = 0.0;
  matrix[10] = 1.0;
  matrix[11] = 0.0;
  
  matrix[12] = 0.0;
  matrix[13] = 0.0;
  matrix[14] = 0.0;
  matrix[15] = 1.0;
  
}

/*--------------------------------------------------------------------------*/

void calc_scale(float matrix[16], float x, float y, float z) {
  
  matrix[0] = x;
  matrix[1] = 0.0;
  matrix[2] = 0.0;
  matrix[3] = 0.0;
  
  matrix[4] = 0.0;
  matrix[5] = y;
  matrix[6] = 0.0;
  matrix[7] = 0.0;
  
  matrix[8] = 0.0;
  matrix[9] = 0.0;
  matrix[10] = z;
  matrix[11] = 0.0;
  
  matrix[12] = 0.0;
  matrix[13] = 0.0;
  matrix[14] = 0.0;
  matrix[15] = 1.0;
  
}

/*--------------------------------------------------------------------------*/

void calc_trans(float matrix[16], float x, float y, float z) {

  matrix[0] = 1.0;
  matrix[1] = 0.0;
  matrix[2] = 0.0;
  matrix[3] = 0.0;
  
  matrix[4] = 0.0;
  matrix[5] = 1.0;
  matrix[6] = 0.0;
  matrix[7] = 0.0;

  matrix[8]  = 0.0;
  matrix[9]  = 0.0;
  matrix[10] = 1.0;
  matrix[11] = 0.0;
  
  matrix[12] = x;
  matrix[13] = y;
  matrix[14] = z;
  matrix[15] = 1.0;
  
}

/*--------------------------------------------------------------------------*/

void inverse4x4(double out[16], double in[16]) {
  
  double det = (double) (in[0] * in[5] * in[10] -
                         in[0] * in[6] * in[9] -
                         in[1] * in[4] * in[10] +
                         in[1] * in[6] * in[8] +
                         in[2] * in[4] * in[9] -
                         in[2] * in[5] * in[8]);

  out[0] = (in[5] * in[10] - in[6] * in[9]) / det;
  out[1] = (-in[1] * in[10] + in[2] * in[9]) / det;
  out[2] = (in[1] * in[6] - in[2] * in[5]) / det;
  out[3] = 0.0;

  out[4] = (-in[4] * in[10] + in[6] * in[8]) / det;
  out[5] = (in[0] * in[10] - in[2] * in[8])  /det;
  out[6] = (-in[0] * in[6] + in[2] * in[4]) / det;
  out[7] = 0.0;

  out[8] = (in[4] * in[9] - in[5] * in[8]) / det;
  out[9] = (-in[0] * in[9] + in[1] * in[8]) / det;
  out[10] = (in[0] * in[5] - in[1] * in[4]) / det;
  out[11] = 0.0;

  out[12] = (-in[4] * in[9] * in[14] + in[4] * in[13] * in[10] +
             in[5] * in[8] * in[14] - in[5] * in[12] * in[10] -
             in[6] * in[8] * in[13] + in[6] * in[12] * in[9]) / det;
  out[13] = (in[0] * in[9] * in[14] - in[0] * in[13] * in[10] -
             in[1] * in[8] * in[14] + in[1] * in[12] * in[10] +
             in[2] * in[8] * in[13] - in[2] * in[12] * in[9]) / det;
  out[14] = (-in[0] * in[5] * in[14] + in[0] * in[13] * in[6] +
             in[1] * in[4] * in[14] - in[1] * in[12] * in[6] -
             in[2] * in[4] * in[13] + in[2] * in[12] * in[5]) / det;
  out[15] = 1.0;

}

/*--------------------------------------------------------------------------*/

void inverse4x4(float out[16], float in[16]) {

  float det = (float)(in[0] * in[5] * in[10] -
                      in[0] * in[6] * in[9] -
                      in[1] * in[4] * in[10] +
                      in[1] * in[6] * in[8] +
                      in[2] * in[4] * in[9] -
                      in[2] * in[5] * in[8]);

  out[0] = (in[5] * in[10] - in[6] * in[9]) / det;
  out[1] = (-in[1] * in[10] + in[2] * in[9]) / det;
  out[2] = (in[1] * in[6] - in[2] * in[5]) / det;
  out[3] = 0.0;

  out[4] = (-in[4] * in[10] + in[6] * in[8]) / det;
  out[5] = (in[0] * in[10] - in[2] * in[8])  /det;
  out[6] = (-in[0] * in[6] + in[2] * in[4]) / det;
  out[7] = 0.0;

  out[8] = (in[4] * in[9] - in[5] * in[8]) / det;
  out[9] = (-in[0] * in[9] + in[1] * in[8]) / det;
  out[10] = (in[0] * in[5] - in[1] * in[4]) / det;
  out[11] = 0.0;

  out[12] = (-in[4] * in[9] * in[14] + in[4] * in[13] * in[10] +
             in[5] * in[8] * in[14] - in[5] * in[12] * in[10] -
             in[6] * in[8] * in[13] + in[6] * in[12] * in[9]) / det;
  out[13] = (in[0] * in[9] * in[14] - in[0] * in[13] * in[10] -
             in[1] * in[8] * in[14] + in[1] * in[12] * in[10] +
             in[2] * in[8] * in[13] - in[2] * in[12] * in[9]) / det;
  out[14] = (-in[0] * in[5] * in[14] + in[0] * in[13] * in[6] +
             in[1] * in[4] * in[14] - in[1] * in[12] * in[6] -
             in[2] * in[4] * in[13] + in[2] * in[12] * in[5]) / det;
  out[15] = 1.0;

}

/*---------------------------------------------------------------------------*/

double maximum(double in1, double in2) {

  // First value is greater than second value
  if (in1 > in2) {
    return in1;
  }

  // Second value is greater than or equal to first value
  return in2;

}

/*---------------------------------------------------------------------------*/

double minimum(double in1, double in2) {

  // First value is less than second value
  if (in1 < in2) {
    return in1;
  }

  // Second value is less than or equal to first value
  return in2;

}

/*--------------------------------------------------------------------------*/

void mult4x4(float out[16], float one[16], float two[16]) {

  out[0] = one[0]*two[0] + one[4]*two[1] + one[8]*two[2] + one[12]*two[3];
  out[1] = one[1]*two[0] + one[5]*two[1] + one[9]*two[2] + one[13]*two[3];
  out[2] = one[2]*two[0] + one[6]*two[1] + one[10]*two[2] + one[14]*two[3];
  out[3] = one[3]*two[0] + one[7]*two[1] + one[11]*two[2] + one[15]*two[3];
  
  out[4] = one[0]*two[4] + one[4]*two[5] + one[8]*two[6] + one[12]*two[7];
  out[5] = one[1]*two[4] + one[5]*two[5] + one[9]*two[6] + one[13]*two[7];
  out[6] = one[2]*two[4] + one[6]*two[5] + one[10]*two[6] + one[14]*two[7];
  out[7] = one[3]*two[4] + one[7]*two[5] + one[11]*two[6] + one[15]*two[7];
  
  out[8] = one[0]*two[8] + one[4]*two[9] + one[8]*two[10] + one[12]*two[11];
  out[9] = one[1]*two[8] + one[5]*two[9] + one[9]*two[10] + one[13]*two[11];
  out[10] = one[2]*two[8] + one[6]*two[9] + one[10]*two[10] + one[14]*two[11];
  out[11] = one[3]*two[8] + one[7]*two[9] + one[11]*two[10] + one[15]*two[11];
  
  out[12] 
    = one[0]*two[12] + one[4]*two[13] + one[8]*two[14] + one[12]*two[15];
  out[13] 
    = one[1]*two[12] + one[5]*two[13] + one[9]*two[14] + one[13]*two[15];
  out[14]
    = one[2]*two[12] + one[6]*two[13] + one[10]*two[14] + one[14]*two[15];
  out[15]
    = one[3]*two[12] + one[7]*two[13] + one[11]*two[14] + one[15]*two[15];

}

/*--------------------------------------------------------------------------*/

void normalize3V(double in[3]) {

  // Lenght of vector
  double  len = (double) sqrt(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]); 

  // Divide each component of input vector by its length
  if (len > 0){
    in[0] /= len;
    in[1] /= len;
    in[2] /= len;
  }

}

/*---------------------------------------------------------------------------*/

void sub3V(double out[3], double in1[3], double in2[3]) {

  out[0] = in1[0] - in2[0];
  out[1] = in1[1] - in2[1];
  out[2] = in1[2] - in2[2];

}

/*---------------------------------------------------------------------------*/

void translate3V(double out[3], double in1[16], double in2[3]) {

  out[0] = in1[0] * in2[0] + in1[4] * in2[1] + in1[8] * in2[2];
  out[1] = in1[1] * in2[0] + in1[5] * in2[1] + in1[9] * in2[2];
  out[2] = in1[2] * in2[0] + in1[6] * in2[1] + in1[10] * in2[2];

}

/*---------------------------------------------------------------------------*/
