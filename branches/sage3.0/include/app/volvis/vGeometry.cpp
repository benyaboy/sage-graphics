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

#include "global.h"
#include "vGeometry.h"
#include "VectorMath.h"    //usefull vector/matrix operations
#include "MatrixMath.h"


static float geomColor[6][4] = {{0.5f,0.0f,1.0f,1.0f},
			{0.0f,0.5f,1.0f,1.0f},
			{0.0f,1.0f,0.5f,1.0f},
			{1.0f,0.5f,0.0f,1.0f},
			{1.0f,0.0f,0.5f,1.0f},
			{0.5f,0.0f,1.0f,1.0f}};
static float geomAxes[6][3] = {{0.0,1.0,1.0},
			{1.0,1.0,0.0},
			{1.0,0.0,1.0},
			{0.5,0.0,1.0},
			{1.0,0.0,0.5},
			{1.0,0.5,0.0}};
static int geomIdx = 0;
//only one geometry in this case
vGeometry::vGeometry(NetVertexBuffer* vb) :vPrimitive() {
    opClient = NULL;
	indexList = 0;
	curFrame = 0;
	initScale = 1.0f;
	NetVertexBuffer* tmp = (NetVertexBuffer* )malloc(sizeof(NetVertexBuffer));
	tmp->copy(vb);
	cache.push_back(tmp);
	numFrames = 1;
	color[0] = geomColor[geomIdx][0];
	color[1] = geomColor[geomIdx][1];
	color[2] = geomColor[geomIdx][2];
	color[3] = geomColor[geomIdx][3];
	axes[0] = geomAxes[geomIdx][0];
	axes[1] = geomAxes[geomIdx][1];
	axes[2] = geomAxes[geomIdx][2];
	angle = 0.0f;
	geomIdx ++;
	geomIdx = geomIdx%6;
	//pb = 0;
}

//may have an animation of geometry
vGeometry::vGeometry(char* wildcard, char* serverName, float scale = 1.0f)
{
    opClient = new cDataManagerClient((char*)serverName);
    fprintf(stderr,"vGeometry::vGeometry %s \n",wildcard);
    opClient->Open();
    opClient->Init();
    int num_datasets = opClient->Query();

	indexList = 0;
	numFrames= opClient->GetMatching(wildcard,indexList);
	if (!numFrames) {
		fprintf(stderr,"ERROR: Cant find any geometry matching %s in Optistore\n",wildcard);
		exit(1);
	}
	fprintf(stderr,"Number of frames %d\n",numFrames);
	cache.resize(numFrames,NULL); //initialise the cache with null
	curFrame = 0;
	initScale = scale;
	color[0] = geomColor[geomIdx][0];
	color[1] = geomColor[geomIdx][1];
	color[2] = geomColor[geomIdx][2];
	color[3] = geomColor[geomIdx][3];
	axes[0] = geomAxes[geomIdx][0];
	axes[1] = geomAxes[geomIdx][1];
	axes[2] = geomAxes[geomIdx][2];
	angle = 0.0f;
	geomIdx ++;
	geomIdx = geomIdx%6;

	//pb = 0;
}



vGeometry::~vGeometry() {
	fprintf(stderr,"~vGeometry() after opClient->Exit()\n");
	if (numFrames) {
		for (int i=0;i<numFrames;i++) {
			if (cache[i]) {
				cache[i]->freeArrays();
				free(cache[i]);
			}
		}
	}
	if (indexList)
		free(indexList);
    if (opClient)
        opClient->Exit();

	/*if (pb) {
		free(pb->vertex);
		free(pb);
	}
	*/

}

//checks if the curframe is loaded, if not grabs it from the server
void vGeometry::setCurFrame(int curFrame) {
	if (cache[curFrame] == NULL) {//get file from server
		opClient->Load(indexList[curFrame]);//get the index of this frame
		NetVertexBuffer* vb = opClient->Isosurface(1, 1, 0.20); // fake values
		cache[curFrame] = (NetVertexBuffer* )malloc(sizeof(NetVertexBuffer));
		cache[curFrame]->copy(vb);
	}

}
//get the first geometry if its animation
void vGeometry::init() {
	fprintf(stderr,"Loading cur frame %d index %d\n",curFrame, indexList[curFrame]);
	setCurFrame(0);
}

void vGeometry::next() {
	curFrame++;
	fprintf(stderr,"next frame %d\n",curFrame);
	if (curFrame >= numFrames)
		curFrame = 0;
	setCurFrame(curFrame);
}

void vGeometry::prev() {
	curFrame--;
	if (curFrame < 0)
		curFrame = numFrames-1;
	setCurFrame(curFrame);
}

void vGeometry::drawBoundBox() {
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(-0.5f,-0.5f,-0.5f);
	glBegin(GL_LINES);
		glColor3f(0.0f,0.5f,0.5f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

//draws the triangele strip here
void vGeometry::draw()
{
	int i,l;
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glTranslatef(xform.trans[0], xform.trans[1], xform.trans[2]);
	glMultMatrixf(xform.rotn);
	glScalef(xform.scale, xform.scale,xform.scale);
	if (selected)
		drawBoundBox();
	glTranslatef(-0.5,-0.5,-0.5);
	glScalef(initScale, initScale,initScale);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	if (cache[curFrame]) {
		glEnable(GL_NORMALIZE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0, cache[curFrame]->vertex);

		//glEnableClientState(GL_COLOR_ARRAY);
		//glColorPointer(4,GL_UNSIGNED_BYTE,0,(void* )((GLubyte* )diffuse));
		glDisableClientState(GL_COLOR_ARRAY);

		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,0, cache[curFrame]->normal);
		//glDisableClientState(GL_NORMAL_ARRAY);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//glPolygonMode(GL_FRONT, GL_LINE);
		//glPolygonMode(GL_BACK, GL_LINE);

		for(i = 0,l = 0; i < cache[curFrame]->lengths; l += cache[curFrame]->length[i],i++)
		{
		    glDrawElements(GL_TRIANGLE_STRIP,
                           cache[curFrame]->length[i],GL_UNSIGNED_INT,&(((GLuint* )cache[curFrame]->index)[l]));
		}

		glDisable(GL_NORMAL_ARRAY);
		//glDisable(GL_VERTEX_ARRAY);
	}
	/*
	if (pb) {
		glBegin(GL_POINTS); // draw some points
		for (int i=0;i<pb->nbpts;i++)
			glVertex3f(pb->vertex[i*3+0],pb->vertex[i*3+1],pb->vertex[i*3+2]);
		glEnd();
	}
	*/
	glPopAttrib();
	glPopMatrix();
	return;
}

//for the pointer
/*
float geomVertexList[8][3] = {{0,0,0},{1.0f,0,0},{0,1.0f,0},{1.0f,1.0f,0},{0,0,1.0f},{1.0f,0,1.0f},{0,1.0f,1.0f},{1.0f,1.0f,1.0f}};
int vGeometry::select() {
	if (withinBounds3D(mv,geomVertexList,global.pointerPos)) {
		selected = 1;
	}
	else
		selected = 0;
	return selected;
}
*/

/*
vGeometry::vGeometry(NetPointBuffer* thePoints) :vPrimitive() {
	pb = (NetPointBuffer* )malloc(sizeof(NetPointBuffer));
	pb->nbpts = thePoints->nbpts;
	pb->vertex = (float* )malloc(sizeof(float)*pb->nbpts*3);
	numFrames = curFrame = 0;
}
*/
