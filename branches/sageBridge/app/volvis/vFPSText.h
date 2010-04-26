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

//CFrameTimer & vFPSText.h Class
//Calculates FPS depending on frame history
//so that the transition is smooth
//abd vFPSText.just draw the text and queries the timer
//Shalini Venkataraman Apr 2002
// Added changes for linux port, Atul Nayak , Jun 27 2002
#ifndef _VFPSTEXT_H_
#define _VFPSTEXT_H_
#ifdef WIN32
#include <windows.h>
#endif

#if defined(linux) || defined(__APPLE__)
#include <sys/time.h>
#endif

#define FRAME_HISTORY 10 //number of frames used in smoothing
#include "vPrimitive.h"

class CFrameTimer {
	double elapsedTime; //time elapsed since timer was instantiated
#ifdef WIN32
  	LARGE_INTEGER last; //last counter
#endif
#if defined(linux) || defined(__APPLE__)
	struct timeval _tstart;
	struct timezone tz;
#endif

	double timerResolution; //time for one count
	double frameTimes[FRAME_HISTORY]; //array of previous times
	int frameLocation; //next index into frameTimes array
	unsigned int framesProcessed; //no of times the counter was pinged
public:
	CFrameTimer();
	//ping counter to calculate the time elapsed and store it
	void pingFrameCounter();
	//get the current smoothed FPS depending on frame history
	float getFPS();

};
class vText : public vPrimitive {
public:
	vText():vPrimitive() { };
	vText(const char* str);
	void draw();
	void init();
	char myText[256];
};

class vFPSText : public vText {
public:
	vFPSText():vText() {};
	void draw();
	void init();
private:
	CFrameTimer* timer;
};

class vVolText : public vText {
public:
	vVolText():vText() {};
	void draw();
	void init();
private:
};

#endif

