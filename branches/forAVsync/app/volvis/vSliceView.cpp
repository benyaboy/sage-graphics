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

#include "vSliceView.h"
#include "vCG.h"
#include "vARB.h"
#include "MatrixMath.h"

vProbe::vProbe() :vPrimitive() {
	xform.scale = 1.0f/(float)global.volume->maxDim;
}

//do opngl initialization here
void vProbe::init() {
}

vProbe::~vProbe() {
}

//draws the cone here
void vProbe::draw()
{
	color[0] = 0.0f;
	color[1] = 1.0f;
	color[2] = 0.0f;
	glPushMatrix();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glTranslatef(xform.trans[0],xform.trans[1],xform.trans[2]);
	glMultMatrixf(xform.rotn);
	glutSolidSphere(xform.scale,20,20);
        glPopAttrib();
	glPopMatrix();
	return;
}

float sliceDim = 0.6f;
float sliceVertexList[4][3] = {{0.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{1.0f, 1.0f, 0.0f},
				{1.0f, 0.0f, 0.0f}};
float polygonVertexList[4][3] = {{-sliceDim*3.0f, -sliceDim, 0.0f},
				{-sliceDim*3.0f, sliceDim, 0.0f},
				{sliceDim*3.0f, sliceDim, 0.0f},
				{sliceDim*3.0f, -sliceDim, 0.0f}};
float sepVertexList[2][4] = {{sliceDim+0.5f,sliceDim+0.5f, 0.0f},
				{sliceDim+0.5f, 0.5-sliceDim, 0.0f, 1.0f}};

vSliceView::vSliceView() :vPrimitive() {
	setTranslation(0.0f,-2.0f,0.0f);
}

//do opngl initialization here
void vSliceView::init() {
}

vSliceView::~vSliceView() {
}


void vSliceView::draw() {
	//draw slice view
#if defined(ADDCGGL)
	setCurTextureCG(global.volren.origDeptexName);
#endif
#if defined(ADDARBGL)
	setCurTextureARB(global.volren.origDeptexName);
#endif
	float xptr, yptr, zptr;		//location of the probe in the volume
	global.volume->getNormProbe(xptr,yptr,zptr);//between 0..1
	axialProbe.setTranslation(xptr,1.0f-zptr,0.0f);
	coronalProbe.setTranslation(xptr,yptr,0.0f);
	sagittalProbe.setTranslation(1.0f-zptr,yptr,0.0f);

	//push the slicevew down
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glTranslatef(xform.trans[0], xform.trans[1], xform.trans[2]);
	glMultMatrixf(xform.rotn);
	glScalef(xform.scale,xform.scale,xform.scale);
	disableTex3D();
	glDisable(GL_LIGHTING);
	glLineWidth(0.5f);
	if (selected)
		glColor3f(0.5f,0.5f,0.0f);
	else
		glColor3f(0.5f,0.5f,0.5f);
	glBegin(GL_LINE_LOOP); //draw slice and texture map it
		glVertex3fv(polygonVertexList[0]);
		glVertex3fv(polygonVertexList[1]);
		glVertex3fv(polygonVertexList[2]);
		glVertex3fv(polygonVertexList[3]);
	glEnd();
	enableTex3D(global.volume->texName);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//the axial view, takes the y direction to index into the volume
	glPushMatrix();
	glTranslatef(-sliceDim*2,0.0f,0.0f);
	glTranslatef(-0.5f,-0.5f,0.0f);
	disableTex3D();
	axialProbe.draw();
	glBegin(GL_LINES);
		glVertex3fv(sepVertexList[0]);
		glVertex3fv(sepVertexList[1]);
	glEnd();
	//get the modelview matrix, multiply the pointer posn to see if it intersects this object

	enableTex3D(global.volume->texName);
	glBegin(GL_POLYGON); //draw slice and texture map it
		glTexCoord3f(0.0f, yptr, 0.0f);
		glVertex3fv(sliceVertexList[1]);
		glTexCoord3f(0.0f, yptr, 1.0f);
		glVertex3fv(sliceVertexList[0]);
		glTexCoord3f(1.0f, yptr, 1.0f);
		glVertex3fv(sliceVertexList[3]);
		glTexCoord3f(1.0f, yptr, 0.0f);
		glVertex3fv(sliceVertexList[2]);
	glEnd();
	glPopMatrix();

	//the coronal view, takes the z
	glPushMatrix();
	glTranslatef(0.0f,0.0f,0.0f);
	glTranslatef(-0.5f,-0.5f,0.0f);
	disableTex3D();
	coronalProbe.draw();
	glBegin(GL_LINES);
		glVertex3fv(sepVertexList[0]);
		glVertex3fv(sepVertexList[1]);
	glEnd();
	enableTex3D(global.volume->texName);
	glBegin(GL_POLYGON); //draw slice and texture map it
		glTexCoord3f(0.0f, 0.0f, zptr);
		glVertex3fv(sliceVertexList[0]);
		glTexCoord3f(0.0f, 1.0f, zptr);
		glVertex3fv(sliceVertexList[1]);
		glTexCoord3f(1.0f, 1.0f, zptr);
		glVertex3fv(sliceVertexList[2]);
		glTexCoord3f(1.0f, 0.0f, zptr);
		glVertex3fv(sliceVertexList[3]);
	glEnd();
	glPopMatrix();


	//the sagittal view, takes the x
	glPushMatrix();
	glTranslatef(sliceDim*2,0.0f,0.0f);
	//since we are starting (0..1)
	glTranslatef(-0.5f,-0.5f,0.0f);
	disableTex3D();
	sagittalProbe.draw();
	enableTex3D(global.volume->texName);
	glBegin(GL_POLYGON); //draw slice and texture map it
		glTexCoord3f(xptr, 0.0f, 0.0f);
		glVertex3fv(sliceVertexList[3]);
		glTexCoord3f(xptr, 0.0f, 1.0f);
		glVertex3fv(sliceVertexList[0]);
		glTexCoord3f(xptr, 1.0f, 1.0f);
		glVertex3fv(sliceVertexList[1]);
		glTexCoord3f(xptr, 1.0f, 0.0f);
		glVertex3fv(sliceVertexList[2]);
	glEnd();
	glPopMatrix();

	glPopAttrib();
	glPopMatrix();
}

/*
int vSliceView::select() {
	if (withinBounds2D(mv,polygonVertexList,global.pointerPos)) {
		selected = 1;
	}
	else
		selected = 0;
	return selected;
}
*/
