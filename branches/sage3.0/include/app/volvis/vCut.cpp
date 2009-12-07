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
 *********************************************************************************/s

#include "vCut.h"
#include "MatrixMath.h"
#include "global.h"

vCut::vCut() {
}



//called from textures draw
void vCut::setTransform(float objectXform[]) {
	float temp[3];
	temp[0]=objectXform[0]*global.cut.axis[0]+
			objectXform[4]*global.cut.axis[1]+objectXform[8]*global.cut.axis[2];
	temp[1]=objectXform[1]*global.cut.axis[0]+
			objectXform[5]*global.cut.axis[1]+objectXform[9]*global.cut.axis[2];
	temp[2]=objectXform[2]*global.cut.axis[0]+
			objectXform[6]*global.cut.axis[1]+objectXform[10]*global.cut.axis[2];
	computeMatrix(global.cut.angle,temp,&global.cut.transform[0]);
}


void vCut::disable() {
	glDisable(GL_CLIP_PLANE0);
}

void vCut::enable() {
	global.cut.user[0] = -global.cut.transform[0];
	global.cut.user[1]=global.cut.transform[1];
	global.cut.user[2]=-global.cut.transform[2];

//	fprintf(stderr,"cut plane %f %f %f %f\n",global.cut.user[0],global.cut.user[1],
			//		global.cut.user[2], global.cut.user[3]);
	//create the points for the corners of the clip plane
	for (int ii=0;ii<4;ii++){
		plane[ii*3+0]=global.cut.transform[0]*global.cut.user[3];
		plane[ii*3+1]=-global.cut.transform[1]*global.cut.user[3];
		plane[ii*3+2]=global.cut.transform[2]*global.cut.user[3];
		plane[ii*3+0]-=global.cut.transform[4]*((ii<2)?0.5f:-0.5f);
		plane[ii*3+1] +=global.cut.transform[5]*((ii<2)?0.5f:-0.5f);
		plane[ii*3+2]-=global.cut.transform[6]*((ii<2)?0.5f:-0.5f);
		plane[ii*3+0]-=global.cut.transform[8]*((ii&0x1)?-0.5f:0.5f);
		plane[ii*3+1]+=global.cut.transform[9]*((ii&0x1)?-0.5f:0.5f);
		plane[ii*3+2]-=global.cut.transform[10]*((ii&0x1)?-0.5f:0.5f);
	}

	//configure the clip planes
	glClipPlane(GL_CLIP_PLANE0,global.cut.user);
	glEnable(GL_CLIP_PLANE0);
}

void vCut::draw() {
	glEnable(GL_TEXTURE_3D);
	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	glBegin(GL_QUADS);
		glVertex3fv(&plane[0]);
		glVertex3fv(&plane[3]);
		glVertex3fv(&plane[9]);
		glVertex3fv(&plane[6]);
	glEnd();
	glDepthMask(GL_TRUE);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_3D);

	glBegin(GL_LINES); //draw clip plane
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3fv(&plane[0]);
		glVertex3fv(&plane[3]);
		glVertex3fv(&plane[3]);
		glVertex3fv(&plane[9]);
		glVertex3fv(&plane[9]);
		glVertex3fv(&plane[6]);
		glVertex3fv(&plane[6]);
		glVertex3fv(&plane[0]);
	glEnd();
}
