/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageDraw.cpp
 * Author : Byungil Jeong
 *
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
 * Direct questions, comments etc about SAGE to sage_users@listserv.uic.edu or 
 * http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/

#include "sageDraw.h"
#include "overlayPointer.h"
#include "overlayApp.h"

sageDraw::sageDraw(bool &dirty, int dispID) : dirtyBit(dirty), displayID(dispID)
{
   preDrawList.clear();
   postDrawList.clear();
   interDrawList.clear();
}

sageDraw::~sageDraw()
{
   std::map<int, sageDrawObject *>::iterator iter;

   for (iter = preDrawList.begin(); iter != preDrawList.end(); iter++) {
      if ((*iter).second )
         (*iter).second->destroy();
   }
   
   for (iter = postDrawList.begin(); iter != postDrawList.end(); iter++) {
      if ((*iter).second) 
         (*iter).second->destroy();
   }

   for (iter = interDrawList.begin(); iter != interDrawList.end(); iter++) {
      if ((*iter).second) 
         (*iter).second->destroy();
   }

   preDrawList.clear();
   postDrawList.clear();
   interDrawList.clear();
}

sageDrawObject* sageDraw::createDrawObject(char *name)
{
   sageDrawObject *newObj = NULL;
   
   if (strcmp(name, "pointer") == 0) {
      newObj = (sageDrawObject *)new overlayPointer;
   }
   else if (strcmp(name, "app") == 0) {
      newObj = (sageDrawObject *)new overlayApp;
   }
      
   return newObj;
}

int sageDraw::preDraw(sageRect rect) 
{
   glDisable(GL_DEPTH_TEST);

   std::map<int, sageDrawObject *>::iterator iter;
   for (iter = preDrawList.begin(); iter != preDrawList.end(); iter++) {
      if ((*iter).second && (*iter).second->visible && displayID==(*iter).second->displayID) {
         (*iter).second->setViewport(rect);
         (*iter).second->draw();
         glColor4f(1.0, 1.0, 1.0, 1.0);   // reset the color      
      }
   }

   glEnable(GL_DEPTH_TEST);

   return 0;
}

int sageDraw::postDraw(sageRect rect) 
{
   glDisable(GL_DEPTH_TEST);

   std::map<int, sageDrawObject *>::iterator iter;
   for (iter = postDrawList.begin(); iter != postDrawList.end(); iter++) {
      if ((*iter).second && (*iter).second->visible && displayID==(*iter).second->displayID) {
         (*iter).second->setViewport(rect);
         (*iter).second->draw();
         glColor4f(1.0, 1.0, 1.0, 1.0);   // reset the color      
      }
   }
   
   glEnable(GL_DEPTH_TEST);
   
   return 0;
}

int sageDraw::interDraw(sageRect rect) 
{
   std::map<int, sageDrawObject *>::iterator iter;
   for (iter = interDrawList.begin(); iter != interDrawList.end(); iter++) {
      if ((*iter).second && (*iter).second->visible && displayID==(*iter).second->displayID) {
         (*iter).second->setViewport(rect);
         (*iter).second->draw();
         glColor4f(1.0, 1.0, 1.0, 1.0);   // reset the color      
      }
   }
   return 0;
}


// add object instances to be drawn in run-time
int sageDraw::addObjectInstance(char *data)
{
   char objectName[SAGE_NAME_LEN];
   int objID, global, drawOrder = 0, dispId=0;
   sageRect layout;
   sscanf(data, "%d %s %d %d %d %d %d %d %d", &objID, objectName, &layout.x, &layout.y, 
               &layout.width, &layout.height, &global, &drawOrder, &dispId); 

   sageDrawObject *newInst = createDrawObject(objectName);
   newInst->init(objID, objectName, layout, global, dispId);
   
   if (drawOrder == SAGE_POST_DRAW) { // post-draw
      postDrawList[objID] = newInst;
   }
   else if (drawOrder == SAGE_INTER_DRAW) { // inter-draw (window border)
      interDrawList[objID] = newInst;
   }
   else { // pre-draw
      preDrawList[objID] = newInst;
   }
   
   return 0;
}

int sageDraw::updateObjectPosition(char *data)
{
   int x, y, id = -1;
   sscanf(data, "%d %d %d", &id, &x, &y);
   
   // find the object we are changing and update it
   if (preDrawList.count(id) > 0)
      preDrawList[id]->updatePosition(x,y);

   else if (postDrawList.count(id) > 0)
      postDrawList[id]->updatePosition(x,y);

   else if (interDrawList.count(id) > 0)
      interDrawList[id]->updatePosition(x,y);

   return 0;
}

int sageDraw::removeObject(int id)
{
   // find the object we are removing and erase it
   if (preDrawList.count(id) > 0) {
      preDrawList[id]->destroy();
      delete preDrawList[id];
      preDrawList.erase(id);
   }

   else if (postDrawList.count(id) > 0) {
      postDrawList[id]->destroy();
      delete postDrawList[id];
      postDrawList.erase(id);
   }

   else if (interDrawList.count(id) > 0) {
      interDrawList[id]->destroy();
      delete interDrawList[id];
      interDrawList.erase(id);
   }

   return 0;
}

int sageDraw::showObject(char *data)
{
   int id = -1;
   int doShow = 1;
   sscanf(data, "%d %d", &id, &doShow);
   
   // find the object we are changing and update it
   if (preDrawList.count(id) > 0)
      preDrawList[id]->showObject(bool(doShow));

   else if (postDrawList.count(id) > 0)
      postDrawList[id]->showObject(bool(doShow));

   else if (interDrawList.count(id) > 0)
      interDrawList[id]->showObject(bool(doShow));

   return 0;
}

int sageDraw::forwardObjectMessage(char *data)
{
   int id;

   // loop through the whole message, split it by newlines 
   // and send them off to the right objects

   char *oneMessage = strtok(data, "\n");

   while (oneMessage != NULL) {
      sscanf(oneMessage, "%d", &id);

      if (preDrawList.count(id) > 0)
         preDrawList[id]->parseMessage( oneMessage );

      else if (postDrawList.count(id) > 0)
         postDrawList[id]->parseMessage( oneMessage );
      
      else if (interDrawList.count(id) > 0)
         interDrawList[id]->parseMessage( oneMessage );

      oneMessage = strtok(NULL, "\n");
   }

   return 0;
}
