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

#ifndef __VPRIMITIVE_H
#define __VPRIMITIVE_H
#include "global.h"
#include "Trackball.h"
//The mother of all the graphics objects
//in the main just create a list of vPrimitives
//and then call init/draw
class vPrimitive {
public:
	vPrimitive();
	virtual ~vPrimitive() { };

	virtual  void          init() { };
	virtual  void          draw() { };

	virtual void		next() { };
	virtual void		prev() { };

	inline void setName(const char *n) {
		strcpy(name,n);
	}

	inline void disable() {
		enabled = false;
	}

	inline void enable() {
		enabled = true;
	}

	inline void setTranslation(float x, float y, float z) {
		xform.trans[0] = x;
		xform.trans[1] = y;
		xform.trans[2] = z;
	}

	inline void setScale(float s) {
		xform.scale = s;
	}

	inline void setColor(float col[]) {
		color[0] = col[0];
		color[1] = col[1];
		color[2] = col[2];
		color[3] = col[3];
	}

	inline void setSelected(bool s) {
		selected = s;
	}

	inline bool isSelected() {
		return selected;
	}

	inline bool isAnimating() {
		return animate;
	}

	inline void toggleAnimate() {
		animate = !animate;
	}

	//return 1 if the key press was for you
	//virtual  int           key(unsigned char k, int x, int y);
	//virtual  int           special(int k, int x, int y);
	//virtual  int           mouse(int button, int state, int x, int y);
	//virtual  int           move(int x, int y);
	//virtual  int           release();   //tells widget it is done
	vTransform xform;
	bool rot;
	float rotAngle;
	float color[4];
	Trackball	track;	//global trackball
protected:
	//the above wil be used for the picking
	char name[256];
	bool enabled;
	bool selected;
	bool animate;
};

#endif
