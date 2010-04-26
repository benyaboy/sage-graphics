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

#ifndef _VRENDERER_H
#define _VRENDERER_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "global.h"
#include "vVolume.h"
#include "vCut.h"
#include "vPrimitive.h"
#include "vSliceView.h"
#define depX 256
#define depY 256
class vRenderer :public vPrimitive
{
public:
	vRenderer();
	virtual ~vRenderer();
	//the volume to render
	void setVolume(vVolume* theVolume) {
		myVolume = theVolume;
	}

	void init();          //primitive call-back
	void draw();          //primitive call-back
	void update();		//update the variables here - called every loop
	int select() ;
	inline void enableCut() {
		cutEnabled = true;
	}

	inline void disableCut() {
		cutEnabled = false;
	}
	inline void enableShowOverview() {
		showOverview = true;
	}

	inline void disableShowOverview() {
		showOverview = false;
	}
	void prev();
	void next();

private:
	//loads the buffer to the texture mem
	void loadTexture3D(unsigned char *tex, int sx, int sy, int sz);
	//draw the bounding box
	void drawBoundBox();
	//draw the bounding box for the subvolume if any
	void drawSubvolBoundBox();
	//draws the 3 orthogonal slice views
	void drawSliceView();
	void renderAll();     //renders 3d texture, slice views, clip plane, bound box etc


	//generic slice allong axis volume renderer
	void renderTexture3D(float sampleFrequency,
		              GLdouble mv[16],  //Model View Matrix
		              float vo[8][3],	  //volume vertex coords model-space coords
		              float tx[8][3],	  //texture vertex coords tex-space coords
		              float axis[3]);	  //axis to slice along world-space coords
	//to render one slice -called in the loop from renderTexture3D
	inline void renderSlice(int edges, float tc[6][3],float pc[6][3],int   order[6]);
	//does the intersection between a slice plane and the texture3D
	inline int intersect(const float p0[3], const float p1[3], //line end points
		     const float t0[3], const float t1[3], //texture points
		     const float v0[3], const float v1[3], //view coord points
		     const float sp[3], const float sn[3], //plane point & norm
		     float pnew[3], float tnew[3], float vnew[3]); //new values

	//Dependant texture related functions
	//loads a dependant texture(interactive or good) for CG to handle the map
	void loadDepTex(unsigned int deptexName, unsigned char *dtex);
	//takes the data from the 2d texture sent from the UI and copies it to the dependant texture(1D)
	//that will be passed to CG
	//For now, just extract the first row of the UI 2d texture to the dependant texture
	//scale the alpha values depending on the sample rate
	void copyScale(float sampleRate, unsigned char *cgTex);

	//---------------------member variables--------------
	vVolume*	myVolume; //pointer to the volume Im supposed to render
	vCut cut;			//cutting plane
	vProbe volProbe;		//probe for the volume
	float xptr, yptr, zptr;		//location of the probe in the volume
	bool cutEnabled;	//whether cutplane is enabled
	bool showOverview; //show the subvolumes that are rendered
};

#endif
