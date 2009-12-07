/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sagePointer.cpp
 * Author : Ratko Jagodic, Byungil Jeong
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

#include "overlayPointer.h"


// custom messages for the pointers
#define RESET  0
#define MOVE   1
#define DRAG   2
#define RESIZE 3
#define DOWN   4
#define UP     5
#define IN_APP 6  // just a color change
#define ANGLE  7
#define ZOOM   8
#define ROTATE 9


// STATES for drawing pointers
#define NORMAL_SHAPE 10
#define DRAG_SHAPE 11
#define RESIZE_SHAPE 12
#define ZOOM_SHAPE 13
#define ROTATE_SHAPE 14


const float DEG2RAD = 3.14159/180;
float z = 0;

overlayPointer::overlayPointer()
{
   ptrShape = NORMAL_SHAPE;
   orientation = 0;
   angle = 0;
   inApp = false;

   up_color[0] = 0.9;
   up_color[1] = 0.1;
   up_color[2] = 0.1;
   
   down_color[0] = 0.7;
   down_color[1] = 0.0;
   down_color[2] = 0.0;

   in_app_color[0] = 0.8;
   in_app_color[1] = 0.85;
   in_app_color[2] = 0.0;

   color = up_color;
}



int overlayPointer::init(char *name)
{
   setName(name);
   return 0;
}



int overlayPointer::draw()
{
   glDisable(GL_TEXTURE_2D);
   //glEnable(GL_DEPTH_TEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   float w = 100;
   float h = w*1.5;

   // set the pointer color depending on the state it's in
   glColor4f(color[0], color[1], color[2], 1.0);

   glPushMatrix();

   

   /**********    DRAW CURSOR   ***********/
   int i = 0;      
   if (ptrShape == DRAG_SHAPE)
   {
      w = w/2;
      h = h/2;
      for(i; i<4; i++)
      {
         glPushMatrix();
         
         glTranslatef(x, y, 0);    
         glRotatef(90*i+angle, 0,0,1);                   
         glTranslatef(0, h+10, 0);
         drawCursor(w, h);

         glPopMatrix();
      }
   }

   else if(ptrShape == RESIZE_SHAPE)
   {
      w = w/1.5;
      h = h/1.5;
      int deg = 0;
      if (orientation == 1 || orientation == 3)
         deg = 135;
      else
         deg = 45;

      for(i; i<2; i++)
      {
         glPushMatrix();
         
         glTranslatef(x, y, 0);    
         glRotatef(deg + 180*i, 0,0,1);       
         glTranslatef(0, h, 0);
         drawCursor(w, h);
         
         glPopMatrix();
      }
   }

   else if(ptrShape == ZOOM_SHAPE)
   {
      float degInRad = 0.0;
      float radius = w/2;
      float hl = 270 - radius * 0.1;  //handle left
      float hr = 270 + radius * 0.1;  //handle right
      float hh = radius*1.7;  //handle height

      glPushMatrix();
      
      // translate and rotate to the position of the pointer
      glTranslatef(x, y, 0);    
      glRotated(angle, 0,0,1);


      /**************  draw magnifying glass ****************/

      glPushMatrix();   
      glRotated(35, 0,0,1);       
      
      
      // inner transparent circle
      glColor4f(1.0, 1.0, 1.0, 0.2);  
      glBegin(GL_POLYGON);
         for (int i=0; i < 360; i++)
         {
            degInRad = i*DEG2RAD;
            glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, z);
         }
         glVertex3f(cos(DEG2RAD)*radius,sin(DEG2RAD)*radius, z); //final point
      glEnd();

      
      // outline of the circle
      glColor4f(1, 1, 1, 1.0);
      glLineWidth(radius/10);
      glBegin(GL_LINE_LOOP);
         for (int i=0; i < 360; i++)
         {
            degInRad = i*DEG2RAD;
            glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, z);
         }
      glEnd();

      
      // draw the handle of the zoom lens
      glBegin(GL_POLYGON);
         glVertex3f(cos(hl*DEG2RAD)*radius,sin(hl*DEG2RAD)*radius, z);
         glVertex3f(cos(hl*DEG2RAD)*radius,sin(hl*DEG2RAD)*radius - radius*1.7, z);
         glVertex3f(cos(hr*DEG2RAD)*radius,sin(hl*DEG2RAD)*radius - radius*1.7, z);
         glVertex3f(cos(hr*DEG2RAD)*radius,sin(hl*DEG2RAD)*radius, z);
         glVertex3f(cos(hl*DEG2RAD)*radius,sin(hl*DEG2RAD)*radius, z);
      glEnd();

      glPopMatrix();


      // plus/minus 
      float pw = radius - radius*0.1;  // line length
      float po = radius - radius*0.5;  // plus/minus offset

      glBegin(GL_LINES);
         // plus
         glVertex3f(-pw/2, po/2, z);
         glVertex3f(pw/2,  po/2, z);
         glVertex3f(0, -pw/2+po/2, z);
         glVertex3f(0,  pw/2+po/2, z);
         
         // minus
         glVertex3f(-pw/2, -po, z);
         glVertex3f(pw/2, -po, z);
      glEnd();
      
      
      
      

      /**************  draw up/down arrows ****************/
   
      glPushMatrix();
      w = radius * 0.7; 
      h = w*1.5;

      glTranslatef(radius+w, 0, 0);    
      for(int i=0; i<2; i++)
      {
         glPushMatrix();
         
         glRotated(180*i, 0,0,1); 
         glTranslatef(0, h, 0);      
         drawCursor(w, h);
         
         glPopMatrix();
      }
      glPopMatrix();
      

      glPopMatrix();
   }

   else if (ptrShape == ROTATE_SHAPE)
   {
      w = w/1.5;
      h = h/1.5;      
      float radius = w*1.2;
      glPushMatrix();
      
      // translate and rotate to the position of the pointer
      glTranslatef(x, y, 0);    
      glRotated(angle, 0,0,1);
      //glRotated(45, 1,1,0);

      glColor4f(1, 1, 1, 1.0);
      glLineWidth(radius/10);
      
      // the rotate arrows
      glPushMatrix();
      drawRotateArrows(radius);
      glRotated(180,0,0,1);
      drawRotateArrows(radius);
      glPopMatrix();
      
      
      // the inner directional arrows
      for(int i=0; i<4; i++)
      {
         glPushMatrix();
         
         //glTranslatef(x, y, 0);    
         glRotatef(90*i+angle, 0,0,1);                   
         glTranslatef(0, h/2.5+10, 0);
         drawCursor(w/2.5, h/2.5);

         glPopMatrix();
      }
   
      glPopMatrix();
   }

   else // NORMAL_SHAPE
   {
      glTranslatef(x, y, 0);    
      glRotatef(30+angle, 0,0,1);       
      drawCursor(w, h);
   }
   

   glPopMatrix();

   glDisable(GL_BLEND);   
   //glDisable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);

   return 0;
}


void overlayPointer::drawRotateArrows(float radius)
{
   float degInRad = 0.0;

   // half circle
   glBegin(GL_LINE_STRIP);
      for (int i=0; i < 150; i++)
      {
         degInRad = i*DEG2RAD;
         glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius, z);
      }
   glEnd();

   // arrow
   glPushMatrix();
   glTranslatef(radius,0, 0);
   glBegin(GL_LINES);
      glVertex3f(0,0,z);
      glVertex3f(-radius/5, radius/3, z);
      glVertex3f(0,0,z);
      glVertex3f(radius/5, radius/3, z);
   glEnd();
   glPopMatrix();
}

void overlayPointer::drawCursor(double w, double h)
{
   // draw the outline of the cursor
   glColor4f(1.0, 1.0, 1.0, 1.0);  
   glBegin(GL_POLYGON);
      glVertex3f(0, 0, z);
      glVertex3f(-w/2, -h, z);
      glVertex3f(0, -h+h/5, z);
   glEnd();
   glBegin(GL_POLYGON);
      glVertex3f(0, 0, z);
      glVertex3f(0, -h+h/5, z);
      glVertex3f(w/2, -h, z);
   glEnd();

   // draw the filled polygon
   glTranslatef(0, -h*0.15/2, 0);
   glColor4f(color[0], color[1], color[2], 1.0);
   double sw = w - w*0.15;
   double sh = h - h*0.15;
   glBegin(GL_POLYGON);
      glVertex3f(0, 0, z);
      glVertex3f(-sw/2, -sh, z);
      glVertex3f(0, -sh+sh/5, z);
   glEnd();
   glBegin(GL_POLYGON);
      glVertex3f(0, 0, z);
      glVertex3f(0, -sh+sh/5, z);
      glVertex3f(sw/2, -sh, z);
   glEnd();
}



int overlayPointer::destroy()
{
   return 0;
}



int overlayPointer::parseMessage(char *msg)
{
   int id, code;
   sscanf(msg, "%d %d", &id, &code);

   switch(code) 
   {
      case MOVE: {
         int newx,newy;
         sscanf(msg, "%d %d %d %d", &id, &code, &newx, &newy); 
         x = newx;
         y = newy;
         break;
      }
      case DRAG: {
         ptrShape = DRAG_SHAPE;
         break;
      }
      case RESIZE: {
         int cId;
         ptrShape = RESIZE_SHAPE;
         sscanf(msg, "%d %d %d", &id, &code, &cId);
         orientation = cId;
         break;
      }
      case DOWN: {
         color = down_color;
         break;
      }
      case UP: {
         color = up_color;
         ptrShape = NORMAL_SHAPE;
         break;
      }
      case RESET: {
         ptrShape = NORMAL_SHAPE;
         //color = up_color;
         orientation = 0;
         break;
      }
      case IN_APP: {
         int i;
         sscanf(msg, "%d %d %d", &id, &code, &i); 
         if(i==0)
         {
            color = up_color;
            inApp = false;
         }
         else
         {
            color = in_app_color;
            inApp = true;
         }
         break;
      }
      case ANGLE: {
         int a;
         sscanf(msg, "%d %d %d", &id, &code, &a);
         angle = a;
         break;
      }
      case ZOOM: {
         ptrShape = ZOOM_SHAPE;
         break;
      }
      case ROTATE: {
         ptrShape = ROTATE_SHAPE;
         break;
      }

   }
     
   return 0;
   
}



