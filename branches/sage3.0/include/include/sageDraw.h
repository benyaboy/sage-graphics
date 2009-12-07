/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageDraw.h
 * Author : Byungil Jeong
 *
 *   Description: This is the header file for the simple drawing class of SAGE. 
 *
 * Copyright (C) 2007 Electronic Visualization Laboratory,
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
 * Direct questions, comments etc about SAGE to sage_users@listserv.uic.edu or 
 * http://www.evl.uic.edu/cavern/forum/
 *         
*********************************************************************************/

#ifndef SAGE_DRAW_H
#define SAGE_DRAW_H

#if defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#include <AGL/agl.h>
#include <OpenGL/OpenGL.h>

#else

#ifdef sun
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

#endif

#include <map>
#include "sage.h"
#include "sageDrawObject.h"


#if defined(__APPLE__)
#include <OpenGL/gl.h>   // Header File For The OpenGL Library
#include <OpenGL/glu.h>   // Header File For The GLu Library
#else
#include <GL/gl.h>   // Header File For The OpenGL Library
#include <GL/glu.h>   // Header File For The GLu Library
#endif

#define SAGE_POST_DRAW  0
#define SAGE_INTER_DRAW 1
#define SAGE_PRE_DRAW   2

class sageDraw {
private:
   // hashes of drawObjects keyed by their ID for easy access
   std::map<int, sageDrawObject *> preDrawList;
   std::map<int, sageDrawObject *> interDrawList;
   std::map<int, sageDrawObject *> postDrawList;
   bool &dirtyBit;
   int displayID;

public:
   sageDraw(bool &dirty, int dispID);
   sageDrawObject * createDrawObject(char *name);
   int preDraw(sageRect rect);  // draw objects to background
   int interDraw(sageRect rect); 
   int postDraw(sageRect rect); // draw objects to overlay
   int addObjectInstance(char *data);
   int updateObjectPosition(char *data);
   int removeObject(int id);
   int showObject(char *data);
   int forwardObjectMessage(char *data);
   ~sageDraw();
};

#endif
