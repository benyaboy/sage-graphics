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

#include <string.h>
#include "vPrimitive.h"
#include "VectorMath.h"
#ifdef WIN32
#include <windows.h>		//  must be included before any OpenGL
#endif

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream.h>

vPrimitive::vPrimitive()
{
	strcpy(name,"");
	identityMatrix(xform.rotn);  //init the rotation
	rot = false;
	rotAngle = 0.0f;
	xform.scale       = 1;        //identity for scale
	xform.trans[0] = xform.trans[1] = xform.trans[2] = 0.0f;
	color[0] = (rand()%100)*0.01f; //upto 2 digits precision
	color[1] = (rand()%100)*0.01f; //upto 2 digits precision
	color[2] = (rand()%100)*0.01f; //upto 2 digits precision
	color[3] = 1.0f;
	track.clear();
	selected = false;
	animate = false;
	enabled = true;
}

/*
int vPrimitive::key(unsigned char k, int x, int y)
{
  return 0;
}

int vPrimitive::special(int k, int x, int y)
{
	return 0;
}


int vPrimitive::mouse(int button, int state,
			  int x, int y)
{
  return 1;
}

int vPrimitive::move(int x, int y)
{
  return 1;
}

int vPrimitive::release()
{
  return 0;
}
*/

