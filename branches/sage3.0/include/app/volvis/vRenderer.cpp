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

#ifdef WIN32
#include <windows.h>
#endif

#include "vRenderer.h"
#include "VectorMath.h"
#include "MatrixMath.h"
#include "glUE.h"
#include "vVolume.h"
#include "vCG.h"
#include "vARB.h"
#include <math.h>


#if defined(linux)
extern "C"
{
extern void glTexImage3D (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
}
#endif

float vo[8][3] = {{0,0,0},{1.0f,0,0},{0,1.0f,0},{1.0f,1.0f,0},{0,0,1.0f},{1.0f,0,1.0f},{0,1.0f,1.0f},{1.0f,1.0f,1.0f}};
float tx[8][3] = {{0,0,0},{1,0,0},{0,1,0},{1,1,0},{0,0,1},{1,0,1},{0,1,1},{1,1,1}};
float axis[3]  = {0,0,1};
double mv[16];
vRenderer::vRenderer() {
	myVolume = NULL;
	cutEnabled = true;//enable cutplane by default
	showOverview = false;
}

vRenderer::~vRenderer() {
}


int vRenderer::select() {
	if (withinBounds3D(mv,vo,global.pointerPos)) {
		selected = 1;
	}
	else
		selected = 0;
	return selected;
}


void vRenderer::init()
{
	if (myVolume) {
		glGenTextures(1, &myVolume->texName);
		loadTexture3D(myVolume->getVoxelData(), myVolume->dimX,myVolume->dimY,myVolume->dimZ);
		GlErr("vRenderer", "init");
	}

}

//load 1 byte volume
void vRenderer::loadTexture3D(unsigned char *tex, int sx, int sy, int sz)
{
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, myVolume->texName);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

#ifdef _WIN32
	glTexImage3DEXT(GL_TEXTURE_3D, 0, GL_ALPHA8, sx, sy, sz, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tex);
#else
	glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA8, sx, sy, sz, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tex);
#endif

	GlErr("vRenderer", "loadTexture3D");
	glDisable(GL_TEXTURE_3D);
}

void vRenderer::drawBoundBox() {
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(-0.5f,-0.5f,-0.5f);
	glBegin(GL_LINES);
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

void vRenderer::next() {
	myVolume->next();
	global.volren.loadVolume = true;
}

void vRenderer::prev() {
	myVolume->prev();
	global.volren.loadVolume = true;
}


void vRenderer::drawSubvolBoundBox() {
	float scaleX, scaleY, scaleZ;
	float offsetX, offsetY, offsetZ;
	global.volume->getNormOffset(offsetX, offsetY, offsetZ);
	global.volume->getNormDim(scaleX, scaleY, scaleZ);

	glPushMatrix();
	glTranslatef(offsetX, offsetY, offsetZ);
	glTranslatef(-0.5f, -0.5f, -0.5f);
	glScalef(scaleX,scaleY,scaleZ);

	glBegin(GL_LINES);
		glColor3f(1.0f,0.0f,0.0f);
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

}
//update any variables here- must be called from draw
void vRenderer::update() {
	//check if we need to reload volume, typically set by the roamer
	if (global.volren.loadVolume) {
		loadTexture3D(myVolume->getVoxelData(),myVolume->dimX,myVolume->dimY,myVolume->dimZ);
		global.volren.loadVolume = false;
	}
	myVolume->getNormProbe(xptr,yptr,zptr);//between 0..1
}

//called by the main
void vRenderer::draw()
{
	//update any variables before doing the draw
	update();
	//finally render
	renderAll();
}

//
//     (010)        (110)
//       6 +---------+ 7   Where 1's are the size of the brick
//        /|        /|      allong that axis
//       / |       / |
// (011)/  |(111) /  |
//   4 +---------+ 5 |
//     |   |     |   |(100) y axis
//     | 2 +-----+---+ 3    ^
//     |  /(000) |  /       |
//     | /       | /        |
//     |/        |/         |
//   0 +---------+ 1        +-------> x axis
//  (001)      (101)       /
//                        /z axis

void vRenderer::renderAll()
{
	//now start the render
	glPushMatrix();
	{ //move to the volume location
		glTranslatef(xform.trans[0], xform.trans[1], xform.trans[2]);
		/*if (rot) {
			glRotatef(rotAngle,0.0f,1.0f,0.0f);
			rotAngle ++;
		}*/
		glMultMatrixf(xform.rotn);  //rotate
		glScalef(myVolume->spacingX*xform.scale, myVolume->spacingY*xform.scale, myVolume->spacingZ*xform.scale);
		glGetDoublev(GL_MODELVIEW_MATRIX, mv);  //save modelview matrix
		//translate the volume and render it
		if (global.cut.update && cutEnabled) {
			cut.setTransform(xform.rotn);
			global.cut.update = false;
		}
		if (cutEnabled)
			cut.draw();
		//draw the box framing everything
		if (global.ui.bboxEnabled) {
			glColor3f(0.0f,0.25f,0.25f);
			drawBoundBox();
		}
		if (selected) {
			glColor3f(0.5f,0.5f,0.0f);
			drawBoundBox();
		}
		if (showOverview)
			drawSubvolBoundBox();

		//draw just the 3d textures
		enableTex3D(myVolume->texName);
		if (cutEnabled)
			cut.enable();

		glPushMatrix();
			glTranslatef(-0.5, -0.5,-0.5);
			disableTex3D();
			volProbe.setTranslation(xptr,yptr,zptr);
			volProbe.draw();

			glGetDoublev(GL_MODELVIEW_MATRIX, mv);  //save modelview matrix
#if defined(ADDCGGL)
			setCurTextureCG(global.volren.scaledDeptexName);
#endif
#if defined(ADDARBGL)
			setCurTextureARB(global.volren.scaledDeptexName);
#endif


			enableTex3D(myVolume->texName);
			renderTexture3D(global.volren.sampleRate,mv,vo,tx,axis);


		glPopMatrix();

		if (cutEnabled)
			cut.disable();
	}glPopMatrix();

	disableTex3D();

}
void enableTex3D(unsigned int texName)
{
	glDisable(GL_LIGHTING); //light makes it look bad!
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if defined(ADDCGGL)
	enableCG(texName);
#endif
#if defined(ADDARBGL)
	enableARB(texName);
#endif
}

void disableTex3D() {
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_3D);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);

#if defined(ADDCGGL)
	disableCG();
#endif
#if defined(ADDARBGL)
	disableARB();
#endif
}

//understand the edge vertex computation from JMK@cs.utah.edu
//ASSUMPTION: eye is along z-axis
//vo: volume vertex coords model-space coords
//tx: texture vertex coords tex-space coords
//axis: axis to slice along world-space coords
void vRenderer::renderTexture3D(float sampleFrequency,GLdouble mv[16],float vo[8][3],float tx[8][3],float axis[3])
{
    float rv[8][3];     //the rotated volume (may include a scale)
    float maxval = -10; //(tmp)
    float minval = 10;
    int minvert = 0, maxvert = 0;
    GLdouble mvinv[16];
    int i, j, k;
    inverseMatrix(mvinv, mv); //invert model view matrix

    for(i=0; i<8; ++i){
	translateV3(rv[i], mv, vo[i]); //get the rotated vol coords
	//now get the max and min z in view space
	if(maxval < MAX(maxval, rv[i][2])){
	    maxval = MAX(maxval, rv[i][2]);
	    maxvert = i;
	}
	if(minval > MIN(minval, rv[i][2])){
	    minval = MIN(minval, rv[i][2]);
	    minvert = i;  //determine the starting corner for slicing
	}
    }

    //find the slice plane point 'sp' (initial) and the slice plane normal 'sn'
    //sp is the slice starting point, simply the vertex farthest from the eye
    float sp[3] = {vo[minvert][0], vo[minvert][1], vo[minvert][2]};
//    float sp[3] = {vo[maxvert][0], vo[maxvert][1], vo[maxvert][2]};
    float vpn[3];
    vpn[0] = axis[0]; vpn[1] = axis[1]; vpn[2] = axis[2];

    //now calculate sn which is the normalized vpn in the model space
    //ie where the orginal slices are stored
    float sn[3];
    translateV3(sn, mvinv, vpn); //move vpn to sn (model space);
    //now normalize this
    float normsn = (float)sqrt(sn[0]*sn[0] + sn[1]*sn[1] + sn[2]*sn[2]); //normalize
    sn[0]/=normsn;
    sn[1]/=normsn;
    sn[2]/=normsn;

    //now find the distance we need to slice (|max_vertex - min_vertex|)
    float maxd[3] = {0, 0, maxval}; //(tmp) only use z-coord (view space)
    float mind[3] = {0, 0, minval}; //(tmp) ditto	    (view space)
    float maxv[3], minv[3];	   //(tmp)
    translateV3(maxv, mvinv, maxd); //translate back to model space
    translateV3(minv, mvinv, mind); //ditto
    maxv[0] -= minv[0]; //subtract
    maxv[1] -= minv[1];
    maxv[2] -= minv[2];

    //now take the norm of this vector... we have the distance to be sampled
    //this distance is in the world space
    float dist = (float)sqrt(maxv[0]*maxv[0] + maxv[1]*maxv[1] + maxv[2]*maxv[2]);

#if defined(ADDCGGL) || defined(ADDARBGL)
    glColor4f(1.0f,1.0f,1.0f,0.01);
#else
    glColor4f(1.0f,1.0f,1.0f,0.1);
#endif

    GlErr("vRenderer","drawVA");

    //distance between samples
    float sampleSpacing = 1.0 / (myVolume->maxDim* sampleFrequency);
    float del[3] = {sn[0]*sampleSpacing, sn[1]*sampleSpacing, sn[2]*sampleSpacing};

    int samples = (int)((dist) / sampleSpacing);//(total distance to be sam	//highly un-optimized!!!!!!!!!
    float poly[6][3];   // for edge intersections
    float tcoord[6][3]; // for texture intersections
    float tpoly[6][3];  // for transformed edge intersections
    int edges;	       // total number of edge intersections

    //the dep texture should be scaled
    glBindTexture(GL_TEXTURE_3D, myVolume->texName);
    //sp:slice plane point
    //sn:the slice dirn to cut thru the volume
    //the above 2 are in world coord space

    for(i = 0 ; i < samples; ++i){ //for each slice
	//increment the slice plane point by the slice distance
//	sp[0] -= del[0];
//	sp[1] -= del[1];
//	sp[2] -= del[2];

	sp[0] += del[0];
	sp[1] += del[1];
	sp[2] += del[2];

	edges = 0;
	//now check each edge of the volume for intersection with..
	//the plane defined by sp & sn
	//front bottom edge
	edges += intersect(vo[0], vo[1], tx[0], tx[1], rv[0], rv[1], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//front left edge
	edges += intersect(vo[0], vo[2], tx[0], tx[2], rv[0], rv[2], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//front right edge
	edges += intersect(vo[1], vo[3], tx[1], tx[3], rv[1], rv[3], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//left bottom edge
	edges += intersect(vo[4], vo[0], tx[4], tx[0], rv[4], rv[0], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//right bottom edge
	edges += intersect(vo[1], vo[5], tx[1], tx[5], rv[1], rv[5], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//front top edge
	edges += intersect(vo[2], vo[3], tx[2], tx[3], rv[2], rv[3], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//back bottom edge
	edges += intersect(vo[4], vo[5], tx[4], tx[5], rv[4], rv[5], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//back left edge
	edges += intersect(vo[4], vo[6], tx[4], tx[6], rv[4], rv[6], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//back right edge
	edges += intersect(vo[5], vo[7], tx[5], tx[7], rv[5], rv[7], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//back top edge
	edges += intersect(vo[6], vo[7], tx[6], tx[7], rv[6], rv[7], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//left top edge
	edges += intersect(vo[2], vo[6], tx[2], tx[6], rv[2], rv[6], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);
	//right top edge
	edges += intersect(vo[3], vo[7], tx[3], tx[7], rv[3], rv[7], sp, sn,
			   poly[edges], tcoord[edges], tpoly[edges]);

	// B.M.E. Moret & H.D. Shapiro "P to NP" pp. 453

	float dx, dy, tt ,theta, cen[2];  //tt= TempTheta
	cen[0] = cen[1] = 0.0;
	int next;
	//rather than swap 3 arrays, only one?
	int order[6] ={0,1,2,3,4,5};

	// order[6] could be an extreemly inefficient way to do this
	for(j=0; j<edges; ++j){ //find the center of the points
	    cen[0] += tpoly[j][0];
	    cen[1] += tpoly[j][1];
	} //by averaging
	cen[0]/= edges;
	cen[1]/= edges;

	for(j=0; j<edges; ++j){ //for each vertex
	    theta = -10;	       //find one with largest angle from center..
	    next = j;
	    for (k= j; k<edges; ++k){
		//... and check angle made between other edges
		dx = tpoly[order[k]][0] - cen[0];
		dy = tpoly[order[k]][1] - cen[1];
		if( (dx == 0) && (dy == 0)){ //same as center?
		    next = k;
		    cout << "what teh " << endl;
		    break; //out of this for-loop
		}
		tt = dy/(ABS(dx) + ABS(dy)); //else compute theta [0-4]
		if( dx < 0.0 ) tt = (float)(2.0 - tt); //check quadrants 2&3
		else if( dy < 0.0 ) tt = (float)(4.0 + tt); //quadrant 4
		if( theta <= tt ){  //grab the max theta
		    next = k;
		    theta = tt;
		}
	    } //end for(k) angle checking
	    // i am using 'tt' as a temp
	    // swap polygon vertex ( is this better than another branch?)
	    // I am not sure wich is worse: swapping 3 vectors for every edge
	    // or: using an array to index into another array??? hmmm....
	    //   should have payed more attention in class
	    int tmp = order[j];
	    order[j] = order[next];
	    order[next] = tmp;

	} //end for(j) edge /angle sort
	renderSlice(edges, tcoord, poly, order);
	//}//end else compute convex hull
    }// end for(i) each slice
    //now draw each slice view

 }


inline void vRenderer::renderSlice(int edges,float tc[6][3], float pc[6][3],int   order[6])
{
    glBegin(GL_POLYGON);  {//draw slice and texture map it
	for(int j=0; j< edges; ++j)
	{
	    glTexCoord3fv(tc[order[j]]);
	    glVertex3fv(pc[order[j]]);
	}
    } glEnd();
}

inline int vRenderer::intersect(const float p0[3], const float p1[3], //line end points
				const float t0[3], const float t1[3], //texture points
				const float v0[3], const float v1[3], //view coord points
				const float sp[3], const float sn[3], //plane point & norm
				float pnew[3], float tnew[3], float vnew[3]) //new values
{
    //t = (sn.(sp - p0))/(sn.(p1 - p0))
    float t = ((sn[0]*(sp[0] - p0[0]) + sn[1]*(sp[1] - p0[1])
		+ sn[2]*(sp[2] - p0[2])) /
	       (sn[0]*(p1[0] - p0[0]) + sn[1]*(p1[1] - p0[1])
		+ sn[2]*(p1[2] - p0[2])));
    //note if the denominator is zero t is a NAN so we should have no problems?

    if( (t>=0) && (t<=1) ){
	//compute line intersection
	pnew[0] = p0[0] + t*(p1[0] - p0[0]);
	pnew[1] = p0[1] + t*(p1[1] - p0[1]);
	pnew[2] = p0[2] + t*(p1[2] - p0[2]);
	//compute texture interseciton
	tnew[0] = t0[0] + t*(t1[0] - t0[0]);
	tnew[1] = t0[1] + t*(t1[1] - t0[1]);
	tnew[2] = t0[2] + t*(t1[2] - t0[2]);
	//compute view coordinate intersections
	vnew[0] = v0[0] + t*(v1[0] - v0[0]);
	vnew[1] = v0[1] + t*(v1[1] - v0[1]);
	vnew[2] = v0[2] + t*(v1[2] - v0[2]);
	return 1;
    }
    return 0;
}


