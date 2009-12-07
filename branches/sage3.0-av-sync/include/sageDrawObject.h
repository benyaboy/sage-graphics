/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageDrawObject.h
 * Author : Byungil Jeong
 *
 *   Description: the super class of simple drawing objects 
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

#ifndef SAGE_DRAW_OBJECT_H
#define SAGE_DRAW_OBJECT_H

#include "sageBase.h"

#if defined(__APPLE__)
#include <OpenGL/gl.h>   // Header File For The OpenGL Library
#include <OpenGL/glu.h>   // Header File For The GLu Library
#include <OpenGL/OpenGL.h>

#else
#include <GL/gl.h>   // Header File For The OpenGL Library
#include <GL/glu.h>   // Header File For The GLu Library
#endif

class sageDrawObject : public sageRect {
protected:
   char objName[SAGE_NAME_LEN];
   int objectID;
   bool global;
   sageRect tileRect;

   int setGlobalView();
   int setLocalView();
      
public:
   int displayID;
   bool visible;
   sageDrawObject() : objectID(0), displayID(0), global(true), visible(true) {}
   void setName(char *name);
   char *getName() { return objName; }
   int getID() { return objectID; }
   void init(int id, char *name, sageRect &layout, bool g, int dispId);   
   void updatePosition(int nx, int ny) { x = nx; y = ny; }
   int setViewport(sageRect &rect);
   void showObject(bool doShow);
   
   virtual int init(char *name) = 0;   
   virtual int draw() = 0;   
   virtual int destroy() = 0;
   virtual int parseMessage(char *msg) = 0;
};

#endif
