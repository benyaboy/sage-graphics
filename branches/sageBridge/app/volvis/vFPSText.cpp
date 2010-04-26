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

#include <stdio.h>
#include "vFPSText.h"
#include "global.h"
GLvoid
  renderBitmapString( char *string )
  {
      int i;
      int len = (int) strlen(string);
      for (i = 0; i < len; i++) {
          glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
      }
  }

  GLvoid
  renderStrokeString( void *font, char *string )
  {
      int i;
      int len = (int) strlen(string);
      for (i = 0; i < len; i++) {
          glutStrokeCharacter(font, string[i]);
      }
  }

CFrameTimer::CFrameTimer() {
  #ifdef WIN32
	LARGE_INTEGER num;
	//get current performance-counter frequency, in counts per second
	QueryPerformanceFrequency(&num);
	timerResolution = (double)num.HighPart*4294967296.0;
	timerResolution += (double)num.LowPart;
	timerResolution=1.0/timerResolution; //get time for 1 count
	QueryPerformanceCounter(&last); //store current counter as last

  #endif

#if defined(linux) || defined(__APPLE__)
	//struct timeval _tstart;
	//struct timezone tz;
	gettimeofday(&_tstart, &tz);
#endif

	elapsedTime=0.0;
	frameLocation=0;
	framesProcessed=0;

}

void CFrameTimer::pingFrameCounter(){
#ifdef WIN32
	LARGE_INTEGER t;
	double elapsed;

	QueryPerformanceCounter(&t); //get current counter
	//get elapsed = current counter - last counter
	elapsed=((double)t.HighPart-(double)last.HighPart)*4294967296.0;
	elapsed+=(double)t.LowPart-(double)last.LowPart;
	elapsed *= timerResolution;
	//update last counter
	last.HighPart=t.HighPart;
	last.LowPart=t.LowPart;
#endif

#if defined(linux) || defined(__APPLE__)
	struct timeval _tend;
	gettimeofday(&_tend,&tz);
	double t1, t2;
	t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
	t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
	//return t2-t1;
	double elapsed;
	elapsed = t2 - t1;
	_tstart = _tend;
#endif

	frameTimes[frameLocation]= elapsed; //store elapsed
	elapsedTime += frameTimes[frameLocation]; //add to the total elapsed time
	frameLocation = (frameLocation+1)%FRAME_HISTORY; //next index
	framesProcessed++;
}

float CFrameTimer::getFPS(){
	int top=(FRAME_HISTORY>framesProcessed)?framesProcessed:FRAME_HISTORY;

	double total=0.0f;
	for(int ii=0;ii<top;ii++)
		total+=frameTimes[ii];
	total/=(float)FRAME_HISTORY; //no of secs per frame
	total=1.0f/total; //get fps
	return (float)total;
}

vText::vText(const char* txt) : vPrimitive() {
	strcpy(myText,txt);
}

void vText::init() {
}

void vText::draw() {
	glPushMatrix();
	glRasterPos3f(xform.trans[0],xform.trans[1],xform.trans[2]);
	if (selected)
		glColor3f(0.0f,0.5f,0.5f);
	else
		glColor3f(1.0f,1.0f,1.0f);
	//glScalef(xform.scale,xform.scale,xform.scale);
	//glScalef(0.5, 0.5, 0.5);
	//renderStrokeString(myText);
	renderBitmapString(myText);
	glPopMatrix();
}

void vFPSText::init() {
	xform.trans[0] = -6.0f;
	xform.trans[1] = -5.0f;
	xform.trans[2] = 0.0f;
	vText::init();
	timer = new CFrameTimer();
}

void vFPSText::draw() {
	timer->pingFrameCounter();
	if (global.ui.printFPS) {
		sprintf(myText,"FPS %f",timer->getFPS());
		vText::draw();
	}
}

void vVolText::init() {
	xform.trans[0] = 1.0f;
	xform.trans[1] = -3.0f;
	xform.trans[2] = 0.0f;
	vText::init();
}

void vVolText::draw() {
	sprintf(myText,"(X,Y,Z):(%d,%d,%d) Vox:%d",global.volume->probeX, global.volume->probeY,
			global.volume->probeZ, global.volume->probeValue);
	vText::draw();
}
