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

//functions to handle UI interaction - if current
//command is not flushed user cant do anything anymore
#include "vUI.h"
#include "global.h"
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "MatrixMath.h"
#include <stdlib.h>
#include "vGeometry.h"
#include "vPrimitive.h"

extern vector<vPrimitive*>   renderList;

void mouseToWorld(int x, int y, float world[3]) {
	float dx = (float)x/(float)global.win.width - 0.5;
	float dy = (float)y/(float)global.win.height - 0.5;
	world[0] = dx *(global.env.frustumList[0].right-global.env.frustumList[0].left) * (global.env.eye[2]-world[2])/global.env.clip[0];
	world[1] = dy *(global.env.frustumList[0].top-global.env.frustumList[0].bottom) * (global.env.eye[2]-world[2])/global.env.clip[0];
}

void startRotation(int x, int y) {
	glutSetWindowTitle("Rotate");
	if (global.ui.cutEnabled)
		cutTrack.start((2.0 * x - global.win.width) / global.win.width,(2.0 * y - global.win.height) / global.win.height);
	else {
		renderList[global.curPrimIdx]->track.start((2.0 * x - global.win.width) / global.win.width,(2.0 * y - global.win.height) / global.win.height);
	}
	global.mouse.pos[0] = global.mouse.last[0] = x;
	global.mouse.pos[1] = global.mouse.last[1] = y;
	global.volren.lowRes = true;
#ifdef V_DISTRIBUTED
	net->setLowRes(true);
#endif
}

//when user is rotating typically mouse drag, call this
void doRotation(int x, int y) {
	if (global.ui.cutEnabled) {
		global.cut.update = true;
		cutTrack.update((2.0 * x - global.win.width) /global.win.width,(2.0 * y - global.win.height) /global.win.height);
		cutTrack.getAxisAngle(global.cut.angle,global.cut.axis);
#if defined(V_DISTRIBUTED)
		net->setCutRotation(global.cut.angle, global.cut.axis);
#endif
	}
	else {
		renderList[global.curPrimIdx]->track.update((2.0 * x - global.win.width) / global.win.width,(2.0 * y - global.win.height) / global.win.height);
		renderList[global.curPrimIdx]->track.buildRotMatrix((float (*)[4])renderList[global.curPrimIdx]->xform.rotn);
#if defined(V_DISTRIBUTED)
		net->setRotn(renderList[global.curPrimIdx]->xform.rotn);
#endif
	}
	global.mouse.last[0] = x;
	global.mouse.last[1] = y;

}

//during a button relase
void endInteract() {
	glutSetWindowTitle("Vol-a-Tile");
	global.volren.lowRes = false;
#ifdef V_DISTRIBUTED
	net->setLowRes(false);
#endif
}

void startZoom(int x, int y) {
	glutSetWindowTitle("Zoom");
	global.mouse.pos[0] = global.mouse.last[0] = x;
	global.mouse.pos[1] = global.mouse.last[1] = y;
	global.volren.lowRes = true;
#ifdef V_DISTRIBUTED
	net->setLowRes(true);
#endif
}

void doZoom(int x, int y) {
	float dx = (x - global.mouse.last[0])/(float)global.win.width;
	float dy = (y - global.mouse.last[1])/(float)global.win.height;
	float change;
	if (fabs(dy) > fabs(dx))
		change = dy;
	else
		change = dx;
	if (global.ui.cutEnabled) {
		global.cut.user[3]  += change; //between -1 and 1
#if defined(V_DISTRIBUTED)
		net->setCutZoom(change);
#endif
	}
	else {
//		global.env.eye[2] += dx;
		renderList[global.curPrimIdx]->xform.scale += change;
#if defined(V_DISTRIBUTED)
		net->setZoom(change);
#endif
	}
	global.mouse.last[0] = x;
	global.mouse.last[1] = y;
}

//TRANSLATE

//on a mouse-down call this
void startTranslate(int x, int y) {
	glutSetWindowTitle("Translate");
	global.mouse.pos[0] = global.mouse.last[0] = x;
	global.mouse.pos[1] = global.mouse.last[1] = y;
	global.volren.lowRes = true;
#ifdef V_DISTRIBUTED
	net->setLowRes(true);
#endif
}

//when user is translating
void doTranslate(int x, int y) {
	float dx = (x - global.mouse.last[0])/(float)global.win.width;
	float dy = (y - global.mouse.last[1])/(float)global.win.height;
	renderList[global.curPrimIdx]->xform.trans[0] += dx *(global.env.frustumList[0].right-global.env.frustumList[0].left) * global.env.eye[2]/global.env.clip[0];
	renderList[global.curPrimIdx]->xform.trans[1] += dy *(global.env.frustumList[0].top-global.env.frustumList[0].bottom) * global.env.eye[2]/global.env.clip[0];
#if defined(V_DISTRIBUTED)
	net->setTranslation(renderList[global.curPrimIdx]->xform.trans);
#endif
	global.mouse.last[0] = x;
	global.mouse.last[1] = y;

}

void printUsage() {
	printf("VolViz\n");
	printf("\t'c'	  - Switch between moving clip plane and object\n");
	printf("\t'f'	  - Toggle printing FPS\n");
	printf("\t'g'     - Switch between gray and psuedo color\n");
	printf("\t'h'	  - Print usage help\n");
	printf("\t'o'	  - Write the volume to file\n");
	printf("\t'n'	  - Toggle printing number of slices\n");
	printf("\t'+'     - Increase number of slices\n");
	printf("\t'-'     - Decrease number of slices\n");
	printf("\t<PAGE UP>  - Increase Eye Separation\n");
	printf("\t<PAGE DOWN> - Decrease Eye Separation\n");
	printf("\t<ESC>   - Quit\n");
}

void togglePrintFPS() {
	global.ui.printFPS = !global.ui.printFPS;
	fprintf(stderr,"print fps %d\n",global.ui.printFPS);
	if (global.ui.printFPS)
		glutIdleFunc(idle);
	else
		glutIdleFunc(0);
#if defined(V_DISTRIBUTED)
	net->togglePrintFPS();
#endif
}

void toggleCutPlane() {
	global.ui.cutEnabled = !global.ui.cutEnabled;
#if defined(V_DISTRIBUTED)
	net->toggleCutPlane();
#endif
}

void saveGradientVolume() {
	global.volume->saveGradient();
#if defined(V_DISTRIBUTED)
        net->saveGradientVolume();
#endif
}

void toggleBoundBox() {
	global.ui.bboxEnabled = !global.ui.bboxEnabled;
#if defined(V_DISTRIBUTED)
	net->toggleBoundBox();
#endif
}
void scaleSampleRate(float factor) {
	global.volren.goodSamp *= factor;
	global.volren.interactSamp *= factor;
	global.volren.sampleRate = global.volren.goodSamp;
#if defined(V_DISTRIBUTED)
	net->scaleSampleRate(factor);
#endif
}



void doExit() {
	deallocateAll();
#if defined(ADDCG)
	exitCG():
#endif
#if defined(V_DISTRIBUTED)
	net->deactivateTiles();
#else
	exit(0);
#endif
}

void doAxisRotation(float angle, char rotaxes) {
	float axis[] = {0.0f,0.0f,0.0f};
	switch (rotaxes) {
	case 'x':
		axis[0] = 1.0f;
		break;
	case 'y':
		axis[1] = 1.0f;
		break;
	case 'z':
		axis[2] = 1.0f;
		break;
	}
	if (global.ui.cutEnabled) {
		global.cut.update = true;
		global.cut.angle = angle; //in degree
		global.cut.axis[0] = axis[0]; //in degree
		global.cut.axis[1] = axis[1]; //in degree
		global.cut.axis[2] = axis[2]; //in degree
#if defined(V_DISTRIBUTED)
		net->setCutRotation(global.cut.angle, global.cut.axis);
#endif
	}
	else {
		computeMatrix(angle, axis, renderList[global.curPrimIdx]->xform.rotn);
#if defined(V_DISTRIBUTED)
		net->setRotn(renderList[global.curPrimIdx]->xform.rotn);
#endif
	}
}

//set the lookup table, either from network or locally
void setLUT(int size, unsigned char* data) {
	int tfSize = 256*4*sizeof(char);
	for (int i=0;i<tfSize;i++)
		global.volren.deptex[i] = data[i];
	global.volren.loadTLUT = true;
	global.volren.lowRes = *((int*)&data[tfSize]);
#if defined(V_DISTRIBUTED)
	net->setLUT(size,data);
#endif
}

//set the low res
void setLowRes(int lowres) {
	global.volren.lowRes = lowres;
#ifdef V_DISTRIBUTED
	net->setLowRes(global.volren.lowRes);
#endif
}

//get isosurface from the optiserver
//and place it at the mouse ptr
void genIsosurface(int x, int y) {
	NetVertexBuffer* theGeom = global.volume->isoSurface();
	vGeometry* newIso = new vGeometry(theGeom);
	mouseToWorld(x,y,newIso->xform.trans);
	newIso->initScale = 1.0f/global.volume->maxDim;
	fprintf(stderr,"New geom translation %f %f scale %f\n",newIso->xform.trans[0],newIso->xform.trans[1],
			newIso->initScale);
	renderList.push_back(newIso);
#ifdef V_DISTRIBUTED
	net->getIsosurface(global.volume->getIsoValue(),newIso->xform.trans[0],newIso->xform.trans[1],newIso->xform.scale);
#endif
}

/*
void genIsopoint(int x, int y) {
	NetPointBuffer* theGeom = global.volume->isoPoint();
	vGeometry* newIso = new vGeometry(theGeom);
	float dx = (float)x/(float)global.win.width - 0.5;
	float dy = (float)y/(float)global.win.height - 0.5;
	newIso->xform.trans[0] = dx * (global.env.frustum[1]-global.env.frustum[0]) * global.env.eye[2]/global.env.clip[0];
	newIso->xform.trans[1] = dy *(global.env.frustum[3]-global.env.frustum[2]) * global.env.eye[2]/global.env.clip[0];
	newIso->xform.scale = 1.0f/global.volume->maxDim;
	fprintf(stderr,"New geom translation %f %f scale %f\n",newIso->xform.trans[0],newIso->xform.trans[1],newIso->xform.scale);
	renderList.push_back(newIso);
#ifdef V_DISTRIBUTED
	net->getIsopoint(global.volume->getIsoValue(),newIso->xform.trans[0],newIso->xform.trans[1],newIso->xform.scale);
#endif
}
*/
void incIsoValue(int value) {
	global.volume->incIsoValue(value);
}

void setRoam() {
	global.ui.navMode= V_ROAM;
#if defined(V_DISTRIBUTED)
	net->setRoam();
#endif
}

void roamVolume(int roamX, int roamY, int roamZ) {
	global.volume->roam(roamX, roamY, roamZ);
	global.volren.loadVolume = true;
#if defined(V_DISTRIBUTED)
	net->roamVolume(roamX, roamY, roamZ);
#endif
}

void animateVolume() {
	for (int i=0;i<renderList.size();i++)
		renderList[i]->toggleAnimate();
#if defined(V_DISTRIBUTED)
	net->animateVolume();
#endif
}

void updateAnim(int value)
{
	for (int i=0;i<renderList.size();i++) {
		if (renderList[i]->isAnimating()) {
			renderList[i]->next();
		}
	}
//if defined(V_DISTRIBUTED)
//	net->updateAnim();
//#endif
	glutTimerFunc(50, updateAnim, 1);
}



void setProbe() {
	global.ui.navMode= V_PROBE;
#if defined(V_DISTRIBUTED)
	net->setProbe();
#endif
}

void probeVolume(int probeX, int probeY, int probeZ) {
	global.volume->probe(probeX, probeY, probeZ);
#if defined(V_DISTRIBUTED)
	net->probeVolume(probeX, probeY, probeZ);
#endif
}

void doSelect(int curIndex) {
	for (int i=0;i<renderList.size();i++) {
		if (i==curIndex)
			renderList[i]->setSelected(true);
		else
			renderList[i]->setSelected(false);
	}
#if defined(V_DISTRIBUTED)
	net->select(curIndex);
#endif
}

void doSelect(int x, int y) {
	//get the pointer position in world coords
	mouseToWorld(x, y, global.pointerPos) ;
	global.curPrimIdx= 0;
	//select each primitive to see which is selected
	/*for (int i=0;i< renderList.size();i++) {
		if (renderList[i]->select()) {
			global.curPrimIdx= i;
			break;
		}
	}*/

#if defined(V_DISTRIBUTED)
	net->select(global.pointerPos);
#endif
}
