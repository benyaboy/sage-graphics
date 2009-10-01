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

#ifndef __MatrixMath_defined
#define __MatrixMath_defined
#include "VectorMath.h"

//BAD SHALINI...dont assume Opengl - do this by hand
inline void computeMatrix(float angle, float axis[],GLdouble* mat) {
	// Have OpenGL compute the new transformation (simple but slow)
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, axis[0], axis[1], axis[2]);
	glMultMatrixd((GLdouble *) mat);
	glGetDoublev(GL_MODELVIEW_MATRIX, (GLdouble *) mat);
	glPopMatrix();
}

inline void computeMatrix(float angle, float axis[],GLfloat* mat) {
	// Have OpenGL compute the new transformation (simple but slow)
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, axis[0], axis[1], axis[2]);
	glMultMatrixf((GLfloat *) mat);
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) mat);
	glPopMatrix();
}


// copy two matricies -----------------------------------
inline void matrixEqual(GLfloat me[16], GLfloat m[16])
{
	for(int i=0; i< 16; ++i) me[i] = m[i];
}

inline void matrixEqual(GLdouble me[16], GLdouble m[16])
{
	for(int i=0; i< 16; ++i) me[i] = m[i];
}

// maxb = ma * mb --------------------------------------
inline void matrixMult( GLfloat maxb[16], GLfloat ma[16], GLfloat mb[16] )
{
	maxb[0] = ma[0]*mb[0] + ma[4]*mb[1] + ma[8]*mb[2] + ma[12]*mb[3];
	maxb[1] = ma[1]*mb[0] + ma[5]*mb[1] + ma[9]*mb[2] + ma[13]*mb[3];
	maxb[2] = ma[2]*mb[0] + ma[6]*mb[1] + ma[10]*mb[2] + ma[14]*mb[3];
	maxb[3] = ma[3]*mb[0] + ma[7]*mb[1] + ma[11]*mb[2] + ma[15]*mb[3];

	maxb[4] = ma[0]*mb[4] + ma[4]*mb[5] + ma[8]*mb[6] + ma[12]*mb[7];
	maxb[5] = ma[1]*mb[4] + ma[5]*mb[5] + ma[9]*mb[6] + ma[13]*mb[7];
	maxb[6] = ma[2]*mb[4] + ma[6]*mb[5] + ma[10]*mb[6] + ma[14]*mb[7];
	maxb[7] = ma[3]*mb[4] + ma[7]*mb[5] + ma[11]*mb[6] + ma[15]*mb[7];

	maxb[8] = ma[0]*mb[8] + ma[4]*mb[9] + ma[8]*mb[10] + ma[12]*mb[11];
	maxb[9] = ma[1]*mb[8] + ma[5]*mb[9] + ma[9]*mb[10] + ma[13]*mb[11];
	maxb[10] = ma[2]*mb[8] + ma[6]*mb[9] + ma[10]*mb[10] + ma[14]*mb[11];
	maxb[11] = ma[3]*mb[8] + ma[7]*mb[9] + ma[11]*mb[10] + ma[15]*mb[11];

	maxb[12] = ma[0]*mb[12] + ma[4]*mb[13] + ma[8]*mb[14] + ma[12]*mb[15];
	maxb[13] = ma[1]*mb[12] + ma[5]*mb[13] + ma[9]*mb[14] + ma[13]*mb[15];
	maxb[14] = ma[2]*mb[12] + ma[6]*mb[13] + ma[10]*mb[14] + ma[14]*mb[15];
	maxb[15] = ma[3]*mb[12] + ma[7]*mb[13] + ma[11]*mb[14] + ma[15]*mb[15];
}

inline void matrixMult( GLdouble maxb[16], GLdouble ma[16], GLdouble mb[16] )
{
	maxb[0] = ma[0]*mb[0] + ma[4]*mb[1] + ma[8]*mb[2] + ma[12]*mb[3];
	maxb[1] = ma[1]*mb[0] + ma[5]*mb[1] + ma[9]*mb[2] + ma[13]*mb[3];
	maxb[2] = ma[2]*mb[0] + ma[6]*mb[1] + ma[10]*mb[2] + ma[14]*mb[3];
	maxb[3] = ma[3]*mb[0] + ma[7]*mb[1] + ma[11]*mb[2] + ma[15]*mb[3];

	maxb[4] = ma[0]*mb[4] + ma[4]*mb[5] + ma[8]*mb[6] + ma[12]*mb[7];
	maxb[5] = ma[1]*mb[4] + ma[5]*mb[5] + ma[9]*mb[6] + ma[13]*mb[7];
	maxb[6] = ma[2]*mb[4] + ma[6]*mb[5] + ma[10]*mb[6] + ma[14]*mb[7];
	maxb[7] = ma[3]*mb[4] + ma[7]*mb[5] + ma[11]*mb[6] + ma[15]*mb[7];

	maxb[8] = ma[0]*mb[8] + ma[4]*mb[9] + ma[8]*mb[10] + ma[12]*mb[11];
	maxb[9] = ma[1]*mb[8] + ma[5]*mb[9] + ma[9]*mb[10] + ma[13]*mb[11];
	maxb[10] = ma[2]*mb[8] + ma[6]*mb[9] + ma[10]*mb[10] + ma[14]*mb[11];
	maxb[11] = ma[3]*mb[8] + ma[7]*mb[9] + ma[11]*mb[10] + ma[15]*mb[11];

	maxb[12] = ma[0]*mb[12] + ma[4]*mb[13] + ma[8]*mb[14] + ma[12]*mb[15];
	maxb[13] = ma[1]*mb[12] + ma[5]*mb[13] + ma[9]*mb[14] + ma[13]*mb[15];
	maxb[14] = ma[2]*mb[12] + ma[6]*mb[13] + ma[10]*mb[14] + ma[14]*mb[15];
	maxb[15] = ma[3]*mb[12] + ma[7]*mb[13] + ma[11]*mb[14] + ma[15]*mb[15];
}

// compute the inverse of a matrix
// invm = m^(-1)
inline void inverseMatrix( GLfloat invm[16], GLfloat m[16] )
{
	GLfloat det =
		m[0]*m[5]*m[10]-
		m[0]*m[6]*m[9]-
		m[1]*m[4]*m[10]+
		m[1]*m[6]*m[8]+
		m[2]*m[4]*m[9]-
		m[2]*m[5]*m[8];

	invm[0] = (m[5]*m[10]-m[6]*m[9])/det;
	invm[1] = (-m[1]*m[10]+m[2]*m[9])/det;
	invm[2] = (m[1]*m[6]-m[2]*m[5])/det;
	invm[3] = 0.0;

	invm[4] = (-m[4]*m[10]+m[6]*m[8])/det;
	invm[5] = (m[0]*m[10]-m[2]*m[8])/det;
	invm[6] = (-m[0]*m[6]+m[2]*m[4])/det;
	invm[7] = 0.0;

	invm[8] = (m[4]*m[9]-m[5]*m[8])/det;
	invm[9] = (-m[0]*m[9]+m[1]*m[8])/det;
	invm[10] = (m[0]*m[5]-m[1]*m[4])/det;
	invm[11] = 0.0;

	invm[12] = (-m[4]*m[9]*m[14]+m[4]*m[13]*m[10]+
		m[5]*m[8]*m[14]-m[5]*m[12]*m[10]-
		m[6]*m[8]*m[13]+m[6]*m[12]*m[9])/det;
	invm[13] = (m[0]*m[9]*m[14]-m[0]*m[13]*m[10]-
		m[1]*m[8]*m[14]+m[1]*m[12]*m[10]+
		m[2]*m[8]*m[13]-m[2]*m[12]*m[9])/det;
	invm[14] = (-m[0]*m[5]*m[14]+m[0]*m[13]*m[6]+
		m[1]*m[4]*m[14]-m[1]*m[12]*m[6]-
		m[2]*m[4]*m[13]+m[2]*m[12]*m[5])/det;
	invm[15] = 1.0;
}

inline void inverseMatrix( GLdouble invm[16], GLdouble m[16] )
{
	GLfloat det = (float)(
		m[0]*m[5]*m[10]-
		m[0]*m[6]*m[9]-
		m[1]*m[4]*m[10]+
		m[1]*m[6]*m[8]+
		m[2]*m[4]*m[9]-
		m[2]*m[5]*m[8]);

	invm[0] = (m[5]*m[10]-m[6]*m[9])/det;
	invm[1] = (-m[1]*m[10]+m[2]*m[9])/det;
	invm[2] = (m[1]*m[6]-m[2]*m[5])/det;
	invm[3] = 0.0;

	invm[4] = (-m[4]*m[10]+m[6]*m[8])/det;
	invm[5] = (m[0]*m[10]-m[2]*m[8])/det;
	invm[6] = (-m[0]*m[6]+m[2]*m[4])/det;
	invm[7] = 0.0;

	invm[8] = (m[4]*m[9]-m[5]*m[8])/det;
	invm[9] = (-m[0]*m[9]+m[1]*m[8])/det;
	invm[10] = (m[0]*m[5]-m[1]*m[4])/det;
	invm[11] = 0.0;

	invm[12] = (-m[4]*m[9]*m[14]+m[4]*m[13]*m[10]+
		m[5]*m[8]*m[14]-m[5]*m[12]*m[10]-
		m[6]*m[8]*m[13]+m[6]*m[12]*m[9])/det;
	invm[13] = (m[0]*m[9]*m[14]-m[0]*m[13]*m[10]-
		m[1]*m[8]*m[14]+m[1]*m[12]*m[10]+
		m[2]*m[8]*m[13]-m[2]*m[12]*m[9])/det;
	invm[14] = (-m[0]*m[5]*m[14]+m[0]*m[13]*m[6]+
		m[1]*m[4]*m[14]-m[1]*m[12]*m[6]-
		m[2]*m[4]*m[13]+m[2]*m[12]*m[5])/det;
	invm[15] = 1.0;
}

//transpose a matrix
inline void transposeMatrix(GLfloat m[16])
{
	GLfloat tmp;
	tmp = m[1];
	m[1] = m[4];
	m[4] = tmp;
	tmp = m[2];
	m[2] = m[8];
	m[8] = tmp;
	tmp = m[3];
	m[3] = m[12];
	m[12] = tmp;
	tmp = m[6];
	m[6] = m[9];
	m[9] = tmp;
	tmp = m[7];
	m[7] = m[13];
	m[13] = tmp;
	tmp = m[11];
	m[11] = m[14];
	m[14] = tmp;

}

inline void transposeMatrix(GLdouble m[16])
{
	GLdouble tmp;
	tmp = m[1];
	m[1] = m[4];
	m[4] = tmp;
	tmp = m[2];
	m[2] = m[8];
	m[8] = tmp;
	tmp = m[3];
	m[3] = m[12];
	m[12] = tmp;
	tmp = m[6];
	m[6] = m[9];
	m[9] = tmp;
	tmp = m[7];
	m[7] = m[13];
	m[13] = tmp;
	tmp = m[11];
	m[11] = m[14];
	m[14] = tmp;

}

//angle and axis to a rotation matrix ----------------------------
inline void axis2Rot( GLfloat m[16], GLfloat k[3], GLfloat theta )
{
	float c = (float)cos(theta);
	float s = (float)sin(theta);
	float v = 1 - c;

	m[0] = k[0]*k[0]*v + c;
	m[4] = k[0]*k[1]*v - k[2]*s;
	m[8] = k[0]*k[2]*v + k[1]*s;

	m[1] = k[0]*k[1]*v + k[2]*s;
	m[5] = k[1]*k[1]*v + c;
	m[9] = k[1]*k[2]*v - k[0]*s;

	m[2] = k[0]*k[2]*v - k[1]*s;
	m[6] = k[1]*k[2]*v + k[0]*s;
	m[10] = k[2]*k[2]*v + c;
}

//angle and axis to a rotation matrix ----------------------------
inline void axis2Rot( GLdouble m[16], GLfloat k[3], GLfloat theta )
{
	float c = (float)cos(theta);
	float s = (float)sin(theta);
	float v = 1 - c;

	m[0] = k[0]*k[0]*v + c;
	m[4] = k[0]*k[1]*v - k[2]*s;
	m[8] = k[0]*k[2]*v + k[1]*s;

	m[1] = k[0]*k[1]*v + k[2]*s;
	m[5] = k[1]*k[1]*v + c;
	m[9] = k[1]*k[2]*v - k[0]*s;

	m[2] = k[0]*k[2]*v - k[1]*s;
	m[6] = k[1]*k[2]*v + k[0]*s;
	m[10] = k[2]*k[2]*v + c;
}


inline void axis2Rot( GLdouble m[16], GLdouble k[3], GLdouble theta )
{
	float c = (float)cos(theta);
	float s = (float)sin(theta);
	float v = 1 - c;

	m[0] = k[0]*k[0]*v + c;
	m[4] = k[0]*k[1]*v - k[2]*s;
	m[8] = k[0]*k[2]*v + k[1]*s;

	m[1] = k[0]*k[1]*v + k[2]*s;
	m[5] = k[1]*k[1]*v + c;
	m[9] = k[1]*k[2]*v - k[0]*s;

	m[2] = k[0]*k[2]*v - k[1]*s;
	m[6] = k[1]*k[2]*v + k[0]*s;
	m[10] = k[2]*k[2]*v + c;
}

// Inverse angle-axis formula.-----------------------------------

inline void rot2Axis( GLfloat k[3], GLfloat *theta, GLfloat m[16] )
{
	GLfloat c = (float)(0.5 * (m[0] + m[5] + m[10] - 1.0));
	GLfloat r1 = m[6] - m[9];
	GLfloat r2 = m[8] - m[2];
	GLfloat r3 = m[1] - m[4];
	GLfloat s = (float)(0.5 * sqrt(r1*r1+r2*r2+r3*r3));

	*theta = (float)atan2(s, c);

	if( fabs(s) > EPSILON )
    {
		c = (float)(2.0*s);

		k[0] = r1 / c;
		k[1] = r2 / c;
		k[2] = r3 / c;
    }
	else
    {
		if( c > 0 ) // theta = 0
		{
			k[0] = 0;
			k[1] = 0;
			k[2] = 1;
		}
		else // theta = +-pi: Shepperd procedure
		{
			GLfloat k0_2 = (m[0] + 1)/2;
			GLfloat k1_2 = (m[5] + 1)/2;
			GLfloat k2_2 = (m[10] + 1)/2;

			if( k0_2 > k1_2 )
			{
				if( k0_2 > k2_2 ) // k0 biggest
				{
					k[0] = (float)sqrt(k1_2);
					k[1] = (m[1] + m[4])/(4*k[0]);
					k[2] = (m[2] + m[8])/(4*k[0]);
				}
				else // k2 biggest
				{
					k[2] = (float)sqrt(k2_2);
					k[0] = (m[2] + m[8])/(4*k[2]);
					k[1] = (m[6] + m[9])/(4*k[2]);
				}
			}
			else
			{
				if( k1_2 > k2_2 ) // k1 biggest
				{
					k[1] = (float)sqrt(k1_2);
					k[0] = (m[1] + m[4])/(4*k[1]);
					k[2] = (m[6] + m[9])/(4*k[1]);
				}
				else // k2 biggest
				{
					k[2] = (float)sqrt(k2_2);
					k[0] = (m[2] + m[8])/(4*k[2]);
					k[1] = (m[6] + m[9])/(4*k[2]);
				}
			}
		}
    }
}

//quaternion to rotation matrix

inline void q2R( GLfloat m[16], GLfloat q[4] )
{
	m[0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
	m[1] = 2*q[1]*q[2] + 2*q[0]*q[3];
	m[2] = 2*q[1]*q[3] - 2*q[0]*q[2];

	m[4] = 2*q[1]*q[2] - 2*q[0]*q[3];
	m[5] = q[0]*q[0] + q[2]*q[2] - q[1]*q[1] - q[3]*q[3];
	m[6] = 2*q[2]*q[3] + 2*q[0]*q[1];

	m[8] = 2*q[1]*q[3] + 2*q[0]*q[2];
	m[9] = 2*q[2]*q[3] - 2*q[0]*q[1];
	m[10]= q[0]*q[0] + q[3]*q[3] - q[1]*q[1] - q[2]*q[2];
}

inline void axisToQuat(float a[3], float phi, float q[4])
{
  vNormal(a);
  vCopy(a,q);
  vScale(q,(float)(sin(phi/2.0)));
  q[3] = (float)(cos(phi/2.0));
}

/////////////////////////////////////////////////////////////////////////////
//
//  Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
//  If they don't add up to 1.0, dividing by their magnitued will
//  renormalize them.
//
//  Note: See the following for more information on quaternions:
//
//  - Shoemake, K., Animating rotation with quaternion curves, Computer
//    Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
//  - Pletinckx, D., Quaternion calculus as a basic tool in computer
//    graphics, The Visual Computer 5, 2-13, 1989.
//
inline void normalizeQuat(float q[4])
{
    float  mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    for (int i = 0; i < 4; i++) q[i] /= mag;
}

/////////////////////////////////////////////////////////////////////////////
//
//  Given two rotations, e1 and e2, expressed as quaternion rotations,
//  figure out the equivalent single rotation and stuff it into dest.
//
//  This routine also normalizes the result every RENORMCOUNT times it is
//  called, to keep error from creeping in.
//
//  NOTE: This routine is written so that q1 or q2 may be the same
//  as dest (or each other).
//
inline void addQuats(float q1[4], float q2[4], float dest[4])
{
  static int count=0;
  float t1[4], t2[4], t3[4];
  float tf[4];

  vCopy(q1,t1);
  vScale(t1,q2[3]);

  vCopy(q2,t2);
  vScale(t2,q1[3]);

  vCross(q2,q1,t3);
  vAdd(t1,t2,tf);
  vAdd(t3,tf,tf);
  tf[3] = q1[3] * q2[3] - vDot(q1,q2);

  dest[0] = tf[0];
  dest[1] = tf[1];
  dest[2] = tf[2];
  dest[3] = tf[3];

  if (++count > 97) {
    count = 0;
    normalizeQuat(dest);
  }
}



/*
* Prints matrix to stderr.
*/

inline void printMatrix( GLfloat m[16] )
{
	int i, j;

	for( i=0; i<4; i++ )
    {
		for( j=0; j<4; j++ )
			fprintf(stderr, "%f ", m[i+j*4]);
		fprintf(stderr, "\n");
    }
	fprintf(stderr, "\n");
}

inline void printMatrix( GLdouble m[16] )
{
	int i, j;

	for( i=0; i<4; i++ )
    {
		for( j=0; j<4; j++ )
			fprintf(stderr, "%f ", m[i+j*4]);
		fprintf(stderr, "\n");
    }
	fprintf(stderr, "\n");
}

inline void buildLookAt(GLfloat m[16],
						GLfloat eye[3], GLfloat at[3], GLfloat up[3])
{
	//I am not 100% certain that my cross products are in the right order

	GLfloat tmp1[3], tmp2[3], tmp3[3];

	subV3(tmp1, eye, at);
	GLfloat norm;
	norm = normV3(tmp1);
	tmp1[0] /=norm;
	tmp1[1] /=norm;
	tmp1[2] /=norm;

	m[2]     =tmp1[0];
	m[6]     =tmp1[1];
	m[10]    =tmp1[2];

	crossV3(tmp2, up, tmp1);
	norm = normV3(tmp2);
	tmp2[0] /=norm;
	tmp2[1] /=norm;
	tmp2[2] /=norm;

	m[0]     =tmp2[0];
	m[4]     =tmp2[1];
	m[8]     =tmp2[2];

	crossV3(tmp3, tmp1, tmp2);
	norm = normV3(tmp3);
	tmp3[0] /=norm;
	tmp3[1] /=norm;
	tmp3[2] /=norm;

	m[1]     =tmp3[0];
	m[5]     =tmp3[1];
	m[9]     =tmp3[2];

	m[12]= -eye[0];
	m[13]= -eye[1];
	m[14]= -eye[2];
	m[15]= 1;

	m[3]  = 0;
	m[7]  = 0;
	m[11] = 0;
}

//give the original 8 vertices and the model view matrix
//This function returns true if the given point is within the bounds of these vertices
inline int withinBounds3D(GLdouble mv[16], float bbox[8][3], float point[3]) {
	float min[3], max[3];
	min[0] = min[1] = min[2] = 10;
	max[0] = max[1] = max[2] = -10;
	float rotatedbbox[8][3];
	for(int i=0; i<8; ++i){
		translateV3W(rotatedbbox[i], mv, bbox[i]); //get the rotated vol coords
		//now get the max and min z in view space
		if(max[2]< MAX(max[2], rotatedbbox[i][2])){
			max[2] = MAX(max[2], rotatedbbox[i][2]);
		}
		if(min[2] > MIN(min[2], rotatedbbox[i][2])){
			min[2] = MIN(min[2], rotatedbbox[i][2]);
		}
		//now get the max and min x in view space
		if(max[0]< MAX(max[0], rotatedbbox[i][0])){
			max[0] = MAX(max[0], rotatedbbox[i][0]);
		}
		if(min[0] > MIN(min[0], rotatedbbox[i][0])){
			min[0] = MIN(min[0], rotatedbbox[i][0]);
		}
		//now get the max and min y in view space
		if(max[1]< MAX(max[1], rotatedbbox[i][1])){
			max[1] = MAX(max[1], rotatedbbox[i][1]);
		}
		if(min[1] > MIN(min[1], rotatedbbox[i][1])){
			min[1] = MIN(min[1], rotatedbbox[i][1]);
		}
	}
	if (point[0]>min[0] && point[0]<max[0]
	 	&& point[1]>min[1] && point[1]<max[1]) {
	 //	&& point[2]>min[2] && point[2]<max[2]) {
	       //pointer is within the bounds
		return true;
	}
	else
		return false;
}

//give the original 4 vertices and the model view matrix
//This function returns true if the given point is within the bounds of these vertices
inline int withinBounds2D(GLdouble mv[16], float bbox[4][3], float point[3]) {
	float min[3], max[3];
	min[0] = min[1] = min[2] = 10;
	max[0] = max[1] = max[2] = -10;
	float rotatedbbox[4][3];
	for(int i=0; i<4; ++i){
		translateV3W(rotatedbbox[i], mv, bbox[i]); //get the rotated vol coords
		//now get the max and min z in view space
		if(max[2]< MAX(max[2], rotatedbbox[i][2])){
			max[2] = MAX(max[2], rotatedbbox[i][2]);
		}
		if(min[2] > MIN(min[2], rotatedbbox[i][2])){
			min[2] = MIN(min[2], rotatedbbox[i][2]);
		}
		//now get the max and min x in view space
		if(max[0]< MAX(max[0], rotatedbbox[i][0])){
			max[0] = MAX(max[0], rotatedbbox[i][0]);
		}
		if(min[0] > MIN(min[0], rotatedbbox[i][0])){
			min[0] = MIN(min[0], rotatedbbox[i][0]);
		}
		//now get the max and min y in view space
		if(max[1]< MAX(max[1], rotatedbbox[i][1])){
			max[1] = MAX(max[1], rotatedbbox[i][1]);
		}
		if(min[1] > MIN(min[1], rotatedbbox[i][1])){
			min[1] = MIN(min[1], rotatedbbox[i][1]);
		}
	}
	if (point[0]>min[0] && point[0]<max[0]
	 	&& point[1]>min[1] && point[1]<max[1]) {
//	 	&& point[2]>min[2] && point[2]<max[2]) {
	       //pointer is within the bounds
		return true;
	}
	else
		return false;
}


#endif
