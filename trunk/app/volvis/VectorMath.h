/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
 * Copyright (C) 2004 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Illinois at Chicago nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/


#ifndef __VectorMath_defined
#define __VectorMath_defined

#ifdef WIN32
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <math.h>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#define EPSILON 2e-07
#define V_PI    3.1415926535897932384626433832795

#if 0 //matrix representation for mat[16], same as OpenGl matrix rep

+-          -+
|0  4  8   12|
|1  5  9   13|
|2  6  10  14|
|3  7  11  15|
+-          -+

#endif

/*
******** NRRD_AFFINE(i,x,I,o,O)
**
** given intervals [i,I], [o,O] and a value x which may or may not be
** inside [i,I], return the value y such that y stands in the same
** relationship to [o,O] that x does with [i,I].  Or:
**
**    y - o         x - i
**   -------   =   -------
**    O - o         I - i
**
** It is the callers responsibility to make sure I-i and O-o are
** both greater than zero
*/

#define CLAMP(x) ((((x)>0) ? (((x)<1) ? x : 1) : 0))
#define MAX(x,y) (((x)>(y)) ? (x) : (y))
#define MIN(x,y) (((x)<(y)) ? (x) : (y))
#define ABS(x) ((x)<0 ? (-x) : (x))
#define NOTPOW2(num) ((num) & (num - 1))

inline double CLAMP_ARB(const double c, const double x, const double C)
{
	return ((((x)>c) ? (((x)<(C)) ? x : (C)) : c));
}

inline double affine(const double i, const double x, const double I,
					 const double o, const double O)
{
	return  ( ((O)-(o))*((x)-(i)) / ((I)-(i)) + (o) );
}

// make a vector all zeros -----------------------------------------
inline void zeroV3(GLfloat* in)
{
	in[0] = in[1] = in[2] = 0;
}

inline void setV3(GLfloat* v, float x, float y, float z)
{
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

// negate all components of a vector -------------------------------
inline void negateV3(GLfloat* in)
{
	in[0] = -in[0];
	in[1] = -in[1];
	in[2] = -in[2];
}


// copy vector 'out' = 'in';---------------------------------------
inline void copyV3(GLfloat* out, GLfloat* in)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

inline void copyV3(GLfloat* out, GLdouble* in)
{
	out[0] = (float)in[0];
	out[1] = (float)in[1];
	out[2] = (float)in[2];
}

inline void copyV3(GLdouble* out, GLfloat* in)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

// out = inl - inr -----------------------------------------------
inline void subV3(GLfloat* out, GLfloat* inl, GLfloat* inr)
{
	out[0] = inl[0] - inr[0];
	out[1] = inl[1] - inr[1];
	out[2] = inl[2] - inr[2];
}

inline void subV3(GLdouble* out, GLdouble* inl, GLdouble* inr)
{
	out[0] = inl[0] - inr[0];
	out[1] = inl[1] - inr[1];
	out[2] = inl[2] - inr[2];
}

// out = inl + inr ---------------------------------------------
inline void addV3(GLfloat *out, GLfloat inl[3], GLfloat inr[3])
{
	out[0] = inl[0] + inr[0];
	out[1] = inl[1] + inr[1];
	out[2] = inl[2] + inr[2];
}

inline void addV3(GLdouble *out, GLdouble inl[3], GLdouble inr[3])
{
	out[0] = inl[0] + inr[0];
	out[1] = inl[1] + inr[1];
	out[2] = inl[2] + inr[2];
}

// one *= s --------------------------------------------------
inline void scaleV3(float s, GLfloat *one)
{
	one[0] *= s;
	one[1] *= s;
	one[2] *= s;
}

inline void scaleV3(double s, GLdouble *one)
{
	one[0] *= s;
	one[1] *= s;
	one[2] *= s;
}

// out = in * s --------------------------------------------
inline void cscaleV3(GLfloat *out, float s, GLfloat in[3])
{
	out[0] = s*in[0];
	out[1] = s*in[1];
	out[2] = s*in[2];
}

inline void cscaleV3(GLdouble *out, double s, GLdouble in[3])
{
	out[0] = s*in[0];
	out[1] = s*in[1];
	out[2] = s*in[2];
}

// set a matrix 'm' to the identity -----------------------
inline void identityMatrix(GLfloat m[16])
{
	m[0]=1; m[4]=0; m[8]= 0; m[12]=0;
	m[1]=0; m[5]=1; m[9]= 0; m[13]=0;
	m[2]=0; m[6]=0; m[10]=1; m[14]=0;
	m[3]=0; m[7]=0; m[11]=0; m[15]=1;
}

inline void identityMatrix(GLdouble m[16])
{
	m[0]=1; m[4]=0; m[8]= 0; m[12]=0;
	m[1]=0; m[5]=1; m[9]= 0; m[13]=0;
	m[2]=0; m[6]=0; m[10]=1; m[14]=0;
	m[3]=0; m[7]=0; m[11]=0; m[15]=1;
}

// vector matrix multiplication [4] vector ----------------------
// out = mat * in
inline void translateV4(GLfloat out[4], GLfloat mat[16], GLfloat in[4])
{
	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]*in[3];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]*in[3];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]*in[3];
	out[3] = mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15]*in[3];
}

//3 vector with implict w=1 mult matrix to 4 vector
inline void translateV4_3W(GLfloat out[4], GLfloat mat[16], GLfloat in[3])
{
	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
	out[3] = mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15];
}

inline void translateV4(GLdouble out[4], GLdouble mat[16], GLdouble in[4])
{
	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]*in[3];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]*in[3];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]*in[3];
	out[3] = mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15]*in[3];
}

// vector matrix multiplicaiton [3] vector with no translation ---
// (only rotation) out = mat * in;
inline void translateV3(GLfloat *out, GLfloat mat[16], GLfloat in[3])
{
	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2];// + mat[12];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2];// + mat[13];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2];// + mat[14];
}

inline void translateV3(GLdouble *out, GLdouble mat[16], GLdouble in[3])
{
	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2];// + mat[12];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2];// + mat[13];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2];// + mat[14];
}

inline void translateV3(GLfloat *out, GLdouble mat[16], GLfloat in[3])
{
	out[0] = (float)(mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2]);// + mat[12];
	out[1] = (float)(mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2]);// + mat[13];
	out[2] = (float)(mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2]);// + mat[14];
}

// [3] vector * matrix --------------------------------------------
// out = mat * in (with translation)
inline void translateV3W(GLfloat *out, GLfloat mat[16], GLfloat in[3])
{
	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
}

inline void translateV3W(GLfloat *out, GLdouble mat[16], GLfloat in[3])
{
	out[0] = (float)(mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]);
	out[1] = (float)(mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]);
	out[2] = (float)(mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]);
}

inline void translateV3W(GLdouble *out, GLdouble mat[16], GLdouble in[3])
{
	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
}

//transformation of 3 vector with implicit w=1 and homoginization
inline void translateV3WD(GLfloat *out, GLfloat mat[16], GLfloat in[3])
{
	out[0] = (float)(mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]);
	out[1] = (float)(mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]);
	out[2] = (float)(mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]);
	float w= (float)(mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15]);
	out[0]/=w;
	out[1]/=w;
	out[2]/=w;
}

// legacy call
inline void transMatrixV3(GLfloat *out, GLfloat mat[16], GLfloat in[3])
{

	out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
	out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
	out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
}

// dot product of two [4] vecotrs --------------------------
inline GLfloat dotV4(GLfloat one[4], GLfloat two[4])
{
	return one[0]*two[0] + one[1]*two[1] + one[2]*two[2] + one[3]*two[3];
}

inline GLdouble dotV4(GLdouble one[4], GLdouble two[4])
{
	return one[0]*two[0] + one[1]*two[1] + one[2]*two[2] + one[3]*two[3];
}

// dot product of two [3] vectors ------------------------
inline GLfloat dotV3(GLfloat one[4], GLfloat two[4])
{
	return one[0]*two[0] + one[1]*two[1] + one[2]*two[2];
}

inline GLdouble dotV3(GLdouble one[4], GLdouble two[4])
{
	return one[0]*two[0] + one[1]*two[1] + one[2]*two[2];
}

// compute the length of a [3] vector --------------------
inline GLfloat normV3(GLfloat *one)
{
	return (GLfloat)sqrt( one[0]*one[0] + one[1]*one[1] + one[2]*one[2]);
}

inline GLdouble normV3(GLdouble *one)
{
	return sqrt( one[0]*one[0] + one[1]*one[1] + one[2]*one[2]);
}

// normalize a [4] vector --------------------------------
inline void normalizeV4(GLfloat v[4])
{
	GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
	if(len > 0){
		v[0] /= len;
		v[1] /= len;
		v[2] /= len;
		v[3] /= len;
	}
}

inline void normalizeV4(GLdouble v[4])
{
	GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
	if(len > 0){
		v[0] /= len;
		v[1] /= len;
		v[2] /= len;
		v[3] /= len;
	}
}

// normalize a [3] vector ---------------------------------
inline void normalizeV3(GLfloat v[3])
{
	GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if(len > 0){
		v[0] /= len;
		v[1] /= len;
		v[2] /= len;
	}
}

inline void normalizeV3(GLdouble v[3])
{
	GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if(len > 0){
		v[0] /= len;
		v[1] /= len;
		v[2] /= len;
	}
}

// divide out the 'W' part of a [4] vector ----------------
inline void homogV4(GLfloat v[4])
{
	v[0] /= v[3];
	v[1] /= v[3];
	v[2] /= v[3];
	v[3] = 1;
}

inline void homogV4(GLdouble v[4])
{
	v[0] /= v[3];
	v[1] /= v[3];
	v[2] /= v[3];
	v[3] = 1;
}

// compute cross product of [3] vector -------------------
// out = one X two;
inline void crossV3(GLfloat *out, GLfloat *one, GLfloat *two)
{
	out[0] = one[1]*two[2] - one[2]*two[1];
	out[1] = one[2]*two[0] - one[0]*two[2];
	out[2] = one[0]*two[1] - one[1]*two[0];
}

inline void crossV3(GLdouble *out, GLdouble *one, GLdouble *two)
{
	out[0] = one[1]*two[2] - one[2]*two[1];
	out[1] = one[2]*two[0] - one[0]*two[2];
	out[2] = one[0]*two[1] - one[1]*two[0];
}


//returns 2^n such that 2^n < number < 2^(n+1)
inline int makepow2_max(int val) {
    if (NOTPOW2(val)) {
		int power = 0;
		if(!val)
			return 0;
		while(val >>= 1)
			power++;
		return(1 << (power+1));
	}
	else
		return val;
}

//returns 2^n such that 2^n < number < 2^(n+1)
inline int makepow2_min(int val) {
    if (NOTPOW2(val)) {
		int power = 0;
		if(!val)
			return 0;
		while(val >>= 1)
			power++;
		return(1 << power);
	}
	else
		return val;
}



inline void printV3(double v[3])
{
	std::cerr << " " << v[0] << "," << v[1] << "," << v[2];
}

inline void printV3(float v[3])
{
	std::cerr << " " << v[0] << "," << v[1] << "," << v[2];
}

inline void printV4(float v[3])
{
	std::cerr << " " << v[0] << "," << v[1] << "," << v[2] << "," << v[3];
}



//=====================================================================================
//---------------------- Quantize -----------------------------------------------------
//=====================================================================================

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, unsigned short *din){

	unsigned short max = 0;
	unsigned short min = USHRT_MAX;
    int i, j, k;
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, short *din){

	short max = SHRT_MIN;
	short min = SHRT_MAX;
    int i, j, k;
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, int *din){

    int i, j, k;
	int max = INT_MIN;
	int min = INT_MAX;
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, unsigned int *din){

	unsigned int max = 0;
	unsigned int min = UINT_MAX;
    int i, j, k;
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, float *din)
{
	float max = -10000000000.0;
	float min = 10000000000.0;
    int i, j, k;
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);
			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(j = 0; j<sy; ++j){
			for(k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min, din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

//for trackball
inline void vZero(float *v)
{
  v[0] = 0.0;
  v[1] = 0.0;
  v[2] = 0.0;
}

inline void vSet(float *v, float x, float y, float z)
{
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

inline void vSub(const float *src1, const float *src2, float *dst)
{
  dst[0] = src1[0] - src2[0];
  dst[1] = src1[1] - src2[1];
  dst[2] = src1[2] - src2[2];
}

inline void vCopy(const float *v1, float *v2)
{
  for (int i = 0 ; i < 3 ; i++)
    v2[i] = v1[i];
}

inline void vCross(const float *v1, const float *v2, float *cross)
{
  float temp[3];

  temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
  vCopy(temp, cross);
}

inline float vLength(const float *v)
{
  return (float)(sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
}

inline void vScale(float *v, float div)
{
  v[0] *= div;
  v[1] *= div;
  v[2] *= div;
}

inline void vNormal(float *v)
{
  vScale(v,(float)(1.0/vLength(v)));
}

inline float vDot(const float *v1, const float *v2)
{
  return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

inline void vAdd(const float *src1, const float *src2, float *dst)
{
  dst[0] = src1[0] + src2[0];
  dst[1] = src1[1] + src2[1];
  dst[2] = src1[2] + src2[2];
}

#endif








