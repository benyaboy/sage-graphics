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

//implements a probe or a 3d pointer
#include "vProbe.h"
#include "VectorMath.h"    //usefull vector/matrix operations
enum {SPHERE = 1, CONE};

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

//the pointer for the environment
vPointer::vPointer() :vPrimitive() {
	xform.scale = 0.02;
}

//do opngl initialization here
void vPointer::init() {
}

vPointer::~vPointer() {
}

//draws the cone here
void vPointer::draw()
{
	//update translation
	xform.trans[0] = global.pointerPos[0];
	xform.trans[1] = global.pointerPos[1];
	xform.trans[2] = global.pointerPos[2];
	color[0] = 0.25f;
	color[1] = 0.5f;
	color[2] = 0.5f;
	glPushMatrix();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glTranslatef(xform.trans[0],xform.trans[1],xform.trans[2]);
	glMultMatrixf(xform.rotn);
	glRotatef(180.0f,0.0f,1.0f,0.0f);
	glutSolidCone(0.02, 0.1, 8, 8);
        glPopAttrib();
	glPopMatrix();
	return;
}

