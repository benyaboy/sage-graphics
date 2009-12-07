/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: overlayApp.cpp
 * Author : Ratko Jagodic
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

#include "overlayApp.h"


// custom messages for the appOverlay
#define RESET  0
#define DRAG   1
#define RESIZE 2
#define Z_CHANGE 3
#define ON_CORNER 4


// STATES for drawing pointers
#define NORMAL_STATE 10
#define DRAG_STATE   11
#define RESIZE_STATE 12


// areas
#define BOTTOM       0
#define TOP          1
#define BOTTOM_LEFT  3
#define TOP_LEFT     4
#define TOP_RIGHT    5
#define BOTTOM_RIGHT 6


// boundaries
#define L 0
#define R 1
#define T 2
#define B 3


overlayApp::overlayApp()
{
   state = NORMAL_STATE;
   aspectRatio = 1;
   z = 0;
   cornerSize = 250;
   
   for(int i=0; i<7; i++)
      activeCorners[i] = 0;

   outline[L] = 0.0;
   outline[R] = 100.0;
   outline[T] = 100.0;
   outline[B] = 0.0;

   border_color[0] = 1.0;
   border_color[1] = 1.0;
   border_color[2] = 1.0;
       
   corner_color[0] = 1.0;//0.059;
   corner_color[1] = 1.0;//0.57;
   corner_color[2] = 1.0;//0.345;
   corner_color[3] = 0.3;

   color = border_color;
}



int overlayApp::init(char *name)
{
   setName(name);
   return 0;
}



int overlayApp::draw()
{
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glPushMatrix();

   //fprintf(stderr, "\nDRAWING AT: %f %f %f %f", outline[L], outline[R], outline[T], outline[B]);

   /**********    DRAW THE STUFF   ***********/
   if (state == NORMAL_STATE)  
   {
      drawOutline();
      
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(8, 21845);

      drawCorners();
      drawTop();

      glDisable(GL_LINE_STIPPLE);
   }
   else if (state == DRAG_STATE || state == RESIZE_STATE)  
   {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(8, 21845);

      drawOutline();
      drawCorners();
      drawTop();

      glDisable(GL_LINE_STIPPLE);
   }

   glPopMatrix();
   glDisable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
   
   return 0;
}

void overlayApp::drawOutline()
{
   // draw the outline of the app we are resizing
   glColor4f(color[0], color[1], color[2], 1);
   glLineWidth(2);
   
   glBegin(GL_LINE_LOOP);
      glVertex3f(outline[L], outline[B], z);
      glVertex3f(outline[L], outline[T], z);
      glVertex3f(outline[R], outline[T], z);
      glVertex3f(outline[R], outline[B], z);
   glEnd();
}


void overlayApp::drawCorners()
{
   glLineWidth(2);
   

   // top_left
   if(activeCorners[TOP_LEFT] == 1)
   {
      glBegin(GL_LINE_STRIP);
         glVertex3f(outline[L], outline[T]-cornerSize, z);
         glVertex3f(outline[L]+cornerSize, outline[T]-cornerSize, z);
         glVertex3f(outline[L]+cornerSize, outline[T], z);
      glEnd();

      glColor4f(corner_color[0], corner_color[1], 
         corner_color[2], corner_color[3]);
      glBegin(GL_POLYGON);
         glVertex3f(outline[L], outline[T]-cornerSize, z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[T]-cornerSize, z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[T], z+0.05);
         glVertex3f(outline[L], outline[T], z+0.05);
         glVertex3f(outline[L], outline[T]-cornerSize, z+0.05);
      glEnd();
   }

   // top_right
   if(activeCorners[TOP_RIGHT] == 1)
   {
      glBegin(GL_LINE_STRIP);
         glVertex3f(outline[R]-cornerSize, outline[T], z);
         glVertex3f(outline[R]-cornerSize, outline[T]-cornerSize, z);
         glVertex3f(outline[R], outline[T]-cornerSize, z);
      glEnd();
      
      glColor4f(corner_color[0], corner_color[1], 
         corner_color[2], corner_color[3]);
      glBegin(GL_POLYGON);
         glVertex3f(outline[R]-cornerSize, outline[T], z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[T]-cornerSize, z+0.05);
         glVertex3f(outline[R], outline[T]-cornerSize, z+0.05);
         glVertex3f(outline[R], outline[T], z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[T], z+0.05);
      glEnd();
   }
   
   // bottom_right
   if(activeCorners[BOTTOM_RIGHT] == 1)
   {
      glColor4f(color[0], color[1], color[2], 0.5);
      glBegin(GL_LINE_STRIP);
         glVertex3f(outline[R]-cornerSize, outline[B], z);
         glVertex3f(outline[R]-cornerSize, outline[B]+cornerSize, z);
         glVertex3f(outline[R], outline[B]+cornerSize, z);
      glEnd();
      
      glColor4f(corner_color[0], corner_color[1], 
         corner_color[2], corner_color[3]);
      glBegin(GL_POLYGON);
         glVertex3f(outline[R]-cornerSize, outline[B], z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[B]+cornerSize, z+0.05);
         glVertex3f(outline[R], outline[B]+cornerSize, z+0.05);
         glVertex3f(outline[R], outline[B], z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[B], z+0.05);
      glEnd();
   }

   // bottom_left
   if(activeCorners[BOTTOM_LEFT] == 1)
   {
      glColor4f(color[0], color[1], color[2], 0.5);
      glBegin(GL_LINE_STRIP);
         glVertex3f(outline[L], outline[B]+cornerSize, z);
         glVertex3f(outline[L]+cornerSize, outline[B]+cornerSize, z);
         glVertex3f(outline[L]+cornerSize, outline[B], z);
      glEnd();

      glColor4f(corner_color[0], corner_color[1], 
         corner_color[2], corner_color[3]);
      glBegin(GL_POLYGON);
         glVertex3f(outline[L], outline[B]+cornerSize, z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[B]+cornerSize, z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[B], z+0.05);
         glVertex3f(outline[L], outline[B], z+0.05);
         glVertex3f(outline[L], outline[B]+cornerSize, z+0.05);
      glEnd();
   }
}



void overlayApp::drawTop()
{
   glColor4f(color[0], color[1], color[2], 0.5);
   glLineWidth(2);
      
   if(activeCorners[TOP] == 1)
   {
      glBegin(GL_LINES);
         glVertex3f(outline[L]+cornerSize, outline[T], z);
         glVertex3f(outline[L]+cornerSize, outline[T]-cornerSize, z);

         glVertex3f(outline[L]+cornerSize, outline[T]-cornerSize, z);
         glVertex3f(outline[R]-cornerSize, outline[T]-cornerSize, z);

         glVertex3f(outline[R]-cornerSize, outline[T], z);
         glVertex3f(outline[R]-cornerSize, outline[T]-cornerSize, z);
      glEnd();

            
      glColor4f(corner_color[0], corner_color[1], 
         corner_color[2], corner_color[3]);
      glBegin(GL_POLYGON);
         glVertex3f(outline[L]+cornerSize, outline[T], z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[T]-cornerSize, z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[T]-cornerSize, z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[T], z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[T], z+0.05);
      glEnd();
   }

   if(activeCorners[BOTTOM] == 1)
   {
      glBegin(GL_LINES);
         glVertex3f(outline[L]+cornerSize, outline[B], z);
         glVertex3f(outline[L]+cornerSize, outline[B]+cornerSize, z);

         glVertex3f(outline[L]+cornerSize, outline[B]+cornerSize, z);
         glVertex3f(outline[R]-cornerSize, outline[B]+cornerSize, z);

         glVertex3f(outline[R]-cornerSize, outline[B]+cornerSize, z);
         glVertex3f(outline[R]-cornerSize, outline[B], z);
      glEnd();

            
      glColor4f(corner_color[0], corner_color[1], 
         corner_color[2], corner_color[3]);
      glBegin(GL_POLYGON);
         glVertex3f(outline[L]+cornerSize, outline[B], z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[B]+cornerSize, z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[B]+cornerSize, z+0.05);
         glVertex3f(outline[R]-cornerSize, outline[B], z+0.05);
         glVertex3f(outline[L]+cornerSize, outline[B], z+0.05);
      glEnd();
   }
}




int overlayApp::destroy()
{
   return 0;
}



int overlayApp::parseMessage(char *msg)
{
   int id, code;
   sscanf(msg, "%d %d", &id, &code);

   switch(code) 
   {
      case DRAG: {
         int dx, dy, cId;
         sscanf(msg, "%d %d %d %d %d", &id, &code, &dx, &dy, &cId);
         cId += 2;  // the corners in the interaction manager are smaller by 2
         outline[L] += dx;
         outline[R] += dx;
         outline[T] += dy;
         outline[B] += dy;
         state = DRAG_STATE;
         activeCorners[cId] = 1;
         //fprintf(stderr, "\nDRAG (%d): %.1f %.1f %.1f %.1f", objectID, outline[L], outline[R], outline[T], outline[B]);
         break;
      }
      case RESIZE: {
         int dx,dy,cId;
         state = RESIZE_STATE;
         sscanf(msg, "%d %d %d %d %d", &id, &code, &dx, &dy, &cId);
         cId += 2;  // the corners in the interaction manager are smaller by 2
         if (cId == BOTTOM_LEFT)
         {
            outline[L] += dx;
            outline[B] += dx/aspectRatio; 
         }      
         else if(cId == TOP_LEFT)
         {
            outline[L] += dx;
            outline[T] -= dx/aspectRatio; 
         }
         else if(cId == TOP_RIGHT)
         {
            outline[R] += dx;
            outline[T] += dx/aspectRatio; 
         }
         else if(cId == BOTTOM_RIGHT)
         {
            outline[R] += dx;
            outline[B] -= dx/aspectRatio; 
         }
         //fprintf(stderr, "\nRESIZE (%d): %.1f %.1f %.1f %.1f", objectID, outline[L], outline[R], outline[T], outline[B]);
         activeCorners[cId] = 1;
         break;
      }
      case RESET: {
         int l,r,t,b,appZ,cs,dispId;
         state = NORMAL_STATE;
         sscanf(msg, "%d %d %d %d %d %d %d %d %d", &id, &code, &l, &r, &t, &b, &appZ, &cs, &dispId); 
         outline[L] = l;
         outline[R] = r;
         outline[T] = t;
         outline[B] = b;
         aspectRatio = (r-l)/float(t-b);
         cornerSize = cs;
         displayID = dispId;
         color = border_color;
         for(int i=0; i<7; i++)
            activeCorners[i] = 0;
         z = -(appZ + 0.1);  // negative cause sage does it this way...
         
         //fprintf(stderr, "\nRESET (%d): %f %d %d %d %d %f", objectID, aspectRatio, l, r, t, b, z);
         break;
      }
      case Z_CHANGE:{
         int appZ;
         sscanf(msg, "%d %d %d", &id, &code, &appZ); 
         z = -(appZ + 0.1);  // negative cause sage does it this way...
         break;
      }
      case ON_CORNER: {
         int cId, isOn;
         sscanf(msg, "%d %d %d %d", &id, &code, &cId, &isOn); 
         cId += 2;  // the corners in the interaction manager are smaller by 2
         activeCorners[cId] = isOn;
         break;
      }
   }
     
   return 0;
   
}







   
