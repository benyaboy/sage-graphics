/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageDisplay.cpp - the display module in SAGE Receiver
 * Author : Byungil Jeong, Rajvikram Singh
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

#if defined(GLSL_YUV)
#if !defined(WIN32)
#define GLEW_STATIC 1
#endif
#include <GL/glew.h>
#include <GL/glu.h>
#include <fcntl.h>
extern GLhandleARB PHandle;
#endif


#include "sageDisplay.h"
#include "sageBlock.h"
#include "pixelDownloader.h"

#if defined(WIN32)

#define glGetProcAddress(n) wglGetProcAddress(n)
#if ! defined(GLSL_YUV)
typedef void (*PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (*PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
#endif
#if !defined(WIN32)
static PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2D    = 0;
static PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2D = 0;
#endif
#else
#if !defined(GLSL_YUV)
extern "C" void glCompressedTexImage2D (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
extern "C" void glCompressedTexSubImage2D (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
#endif
#endif


int GLprintError(const char *file, int line)
{
   //
   // Returns 1 if an OpenGL error occurred, 0 otherwise.
   //
   GLenum glErr;
   int    retCode = 0;

   glErr = glGetError();
   while (glErr != GL_NO_ERROR)
   {
      fprintf(stderr, "GL> glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
      retCode = 1;
      glErr = glGetError();
   }
   return retCode;
}


sageMontage::sageMontage(displayContext *dct, sagePixFmt pfmt) : texWidth(0), texHeight(0), 
      texture(NULL), texHandle(-1), pixelType(pfmt), depth(1.0), monIdx(-1), tileIdx(-1), 
      visible(true), context(dct) 
{  
   pInfo.initType(pfmt); 
   
#if (!defined(__APPLE__) && !defined(GLSL_YUV))
   // If no shader and not apple hardware, we have to convert YUV to RGB on CPU
   //    so data size is now 3 bytes
   if (pfmt == PIXFMT_YUV)
      pInfo.bytesPerPixel = 3;
#endif

}

int sageMontage::init(sageRect &viewPort, sageRect &blockLayout, sageRotation orientation)
{
   texCoord = viewPort;
   texCoord.setOrientation(orientation);
   texInfo = blockLayout;
   texInfo.x = texInfo.y = 0;

   updateBoundary();
   //std::cout << "pt6");
   renewTexture();
   
   genTexCoord();
   //std::cerr << "gen tex coord " << texCoord.left << " " << texCoord.right << " " << texCoord.bottom << " " << texCoord.top << std::endl;   
   return 0;
}

int sageMontage::copyConfig(sageMontage &mon)
{
   //*(sageRect*)this = (sageRect&)mon;
   x = mon.x;
   y = mon.y;
   width = mon.width;
   height = mon.height;
   left = mon.left;
   right = mon.right;
   bottom = mon.bottom;
   top = mon.top;
   
   //std::cerr << "copy config " << left << " " << right << " " << bottom << " " << top << std::endl;
   
   pInfo = mon.pInfo;
   pixelType = mon.pixelType;
   texCoord = mon.texCoord;
   texInfo = mon.texInfo;
   
   //std::cerr << "copy tex coord " << texCoord.left << " " << texCoord.right << " " << texCoord.bottom << " " << texCoord.top << std::endl;
   renewTexture();
   //genTexCoord();
   
   return 0;
}

bool sageMontage::genTexCoord()
{
#if defined(GLSL_YUV)
   if (pixelType == PIXFMT_YUV) {
      texCoord.updateBoundary();
   }
   else {
      sageRect texRect(0, 0, texWidth, texHeight);
      texCoord /= texRect;
   }
#else
   sageRect texRect(0, 0, texWidth, texHeight);
   //std::cerr << "tex size " << texWidth << " " << texHeight << std::endl;
   char msg[TOKEN_LEN];
   texCoord.sprintRect(msg);
   //std::cerr << "tex rect " << msg << std::endl;
   texCoord /= texRect;
#endif

   return true;
}

int sageMontage::loadPixelBlock(sagePixelBlock *block)
{
   //printf("\nheader %x\npixelBuffer %x\n\n", block->getBuffer(), block->getPixelBuffer()); 
   
   context->switchContext(tileIdx);
   
   glBindTexture(GL_TEXTURE_2D, texHandle);
   
   if (pixelType == PIXFMT_DXT) {
      //std::cout << "width " << block->width << " , height " << block->height 
      // << ", size " << (block->width*block->height)/2 << std::endl;
      //   fprintf(stderr, "GL> POS %d %d\n", posX, posY);
      
      glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, block->x, block->y, 
               block->width, block->height,
               pInfo.pixelFormat,
               (block->width*block->height)/2,
               block->getPixelBuffer());

      GLprintError(__FILE__, __LINE__);  // Check for OpenGL errors
      return 0;
   }
   
#if !defined(__APPLE__)

#define clip(A) ( ((A)<0) ? 0 : ( ((A)>255) ? 255 : (A) ) )

#define YUV444toRGB888(Y,U,V,R,G,B) \
   R = clip(( 298 * (Y-16)                 + 409 * (V-128) + 128) >> 8); \
      G = clip(( 298 * (Y-16) - 100 * (U-128) - 208 * (V-128) + 128) >> 8); \
      B = clip(( 298 * (Y-16) + 516 * (U-128)                 + 128) >> 8);
   
   if (pixelType == PIXFMT_YUV) {
#if defined(GLSL_YUV)
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_RECTANGLE_ARB);
      glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texHandle);
      glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, block->x, block->y, block->width, block->height,
         pInfo.pixelFormat, pInfo.pixelDataType, block->getPixelBuffer());
      glDisable(GL_TEXTURE_RECTANGLE_ARB);
      glEnable(GL_TEXTURE_2D);
#else
      unsigned char *yuv = (unsigned char*)block->getPixelBuffer();
      unsigned char u,v,y1,y2;
      unsigned char r1,r2,g1,g2,b1,b2;
      int i, k;
      k = 0;
      for (i=0;i< (block->width*block->height)/2;i++) {
         u  = yuv[4*i+0];
         y1 = yuv[4*i+1];
         v  = yuv[4*i+2];
         y2 = yuv[4*i+3];
       
         YUV444toRGB888(y1, u, v, r1,g1,b1);
         YUV444toRGB888(y2, u, v, r2,g2,b2);
         texture[k + 0] = r1;
         texture[k + 1] = g1;
         texture[k + 2] = b1;
         texture[k + 3] = r2;
         texture[k + 4] = g2;
         texture[k + 5] = b2;
         k += 6;
      }
      glTexSubImage2D(GL_TEXTURE_2D, 0, block->x, block->y, block->width, block->height,
         GL_RGB, GL_UNSIGNED_BYTE, texture);
#endif
   }
   else {
      //std::cerr << "block download " << block->x << " , " << block->y << " , " <<
      //   block->width << " , " << block->height << std::endl;   

      glTexSubImage2D(GL_TEXTURE_2D, 0, block->x, block->y, block->width, block->height,
         pInfo.pixelFormat, pInfo.pixelDataType, block->getPixelBuffer());
   }   
#else
   glTexSubImage2D(GL_TEXTURE_2D, 0, block->x, block->y, block->width, block->height,
         pInfo.pixelFormat, pInfo.pixelDataType, block->getPixelBuffer());
#endif

   return 0;
}

int sageMontage::renewTexture()
{
   context->switchContext(tileIdx);
   
   //generate the textures which will be used
   int newWidth, newHeight;
   newWidth = getMax2n(texInfo.width);
   newHeight = getMax2n(texInfo.height);
   
   int maxt;
//HYEJUNG
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&maxt);
//HYEJUNG
   if (newWidth > maxt)  {
      fprintf(stderr, "Warning: trying to create texture %d, bigger than maximun %d\n", newWidth, maxt);
      newWidth = maxt;
   }
   if (newHeight > maxt) {
      fprintf(stderr, "Warning: trying to create texture %d, bigger than maximun %d\n", newHeight, maxt);
      newHeight = maxt;
   }

   if (newWidth <= texWidth && newHeight <= texHeight) {
      //std::cout << "reuse texture : " << texWidth << " , " << texHeight);
      return 1;
   }
   
   texWidth = newWidth;
   texHeight = newHeight;

   deleteTexture();

   GLuint handle;      
   glGenTextures(1, &handle);
   texHandle = handle;

   // Create GL texture object
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Create dummy texture image.
   if (pixelType == PIXFMT_DXT) {
      if (texWidth < 64)
         texWidth = 64;
      if (texHeight < 64)
         texHeight = 64;

      texture = (GLubyte *) malloc(texWidth * texHeight / 2);
      memset(texture, 0, texWidth * texHeight / 2);

      // fprintf(stderr, "Create DXT texture: format %d, width %d height %d, size %d\n",
      //   pInfo.pixelFormat, texWidth, texHeight, texWidth * texHeight / 2);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, handle);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      
      glCompressedTexImage2D(GL_TEXTURE_2D, 0,
                  pInfo.pixelFormat,
                  texWidth, texHeight, 0,
                  texWidth * texHeight / 2, texture);
      
      GLprintError(__FILE__, __LINE__);  // Check for OpenGL errors
   }
   else {
      //texture = (GLubyte *) malloc(texWidth * texHeight * pInfo.bytesPerPixel);
      //memset(texture, 0, texWidth * texHeight * pInfo.bytesPerPixel);

      if (pixelType == PIXFMT_YUV) {
#if defined(GLSL_YUV)
         texture = (GLubyte *) malloc(texWidth * texHeight * pInfo.bytesPerPixel);
         memset(texture, 0, texWidth * texHeight * pInfo.bytesPerPixel);
         glDisable(GL_TEXTURE_2D);
         glEnable(GL_TEXTURE_RECTANGLE_ARB);
         glBindTexture(GL_TEXTURE_RECTANGLE_ARB, handle);
         glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
         glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
         glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, pInfo.internalFormat, texWidth, texHeight, 0, 
            pInfo.pixelFormat, pInfo.pixelDataType, texture);
#else
         texture = (GLubyte *) malloc(texWidth * texHeight * 3);
         memset(texture, 0, texWidth * texHeight * 3);
         glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, handle);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, 
            GL_RGB, GL_UNSIGNED_BYTE, texture);
#endif
      }
      else {
         texture = (GLubyte *) malloc(texWidth * texHeight * pInfo.bytesPerPixel);
         memset(texture, 0, texWidth * texHeight * pInfo.bytesPerPixel);
         glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, handle);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         
         glTexImage2D(GL_TEXTURE_2D, 0, pInfo.internalFormat, texWidth, texHeight, 0, 
            pInfo.pixelFormat, pInfo.pixelDataType, texture);
      }
   }
   
   return 0;
} //End of renewTexture

int sageMontage::deleteTexture()
{
   GLuint handle;
   
   if (texHandle >= 0) {
      handle = (GLuint)texHandle;
      glDeleteTextures(1, &handle);
      if (texture)
         free(texture);
   }
   return 0;
}

sageDisplay::sageDisplay(displayContext *dct, struct sageDisplayConfig &cfg): context(dct), 
         dirty(false), drawObj(dirty, cfg.displayID) 
{
   configStruct = cfg;
   tileNum = cfg.dimX * cfg.dimY;
   if (tileNum > MAX_TILES_PER_NODE) {
      sage::printLog("sageDisplay::init() : The tile number exceeds the maximum"); 
   }
      
   for (int i=0; i<tileNum; i++) {
      noOfMontages[i] = 0;
      for (int j=0; j<MAX_MONTAGE_NUM; j++)
         montages[i][j] = NULL;
   }

#if defined(WIN32)
   fprintf(stderr, "Init GL functions: %p %p\n", glCompressedTexImage2D, glCompressedTexSubImage2D);
   glCompressedTexImage2D  = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)
      glGetProcAddress("glCompressedTexImage2D");
   glCompressedTexSubImage2D  = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)
      glGetProcAddress("glCompressedTexSubImage2D");
   fprintf(stderr, "----> GL functions: %p %p\n", glCompressedTexImage2D, glCompressedTexSubImage2D);
#endif
}

int sageDisplay::changeBGColor(int red, int green, int blue)
{   
   context->changeBackground(red, green, blue);
   dirty = true;
   
   return 0;
}

int sageDisplay::addMontage(sageMontage *mon)
{
   if (!mon) {
      sage::printLog("sageDisplay::addMontage : Error - input is NULL");
      return -1;
   }
   
   if (mon->tileIdx < 0)
      return -1;
      
   mon->visible = false;
   
   for (int i=0; i<MAX_MONTAGE_NUM; i++) {
      if (montages[mon->tileIdx][i] == NULL) {
         montages[mon->tileIdx][i] = mon;
         mon->monIdx = i;
         if (i+1 > noOfMontages[mon->tileIdx])
            noOfMontages[mon->tileIdx] = i+1;
         return i;
      }
   }
      
   sage::printLog("sageDisplay::addMontage : Error - montage list is full");
   return -1;
}

int sageDisplay::removeMontage(sageMontage *mon)
{
   if (!mon) {
      sage::printLog("sageDisplay::removeMontage : Error - input is NULL");
      return -1;
   }
   
   if (mon->tileIdx < 0 || mon->monIdx < 0)
      return 0;
      
   montages[mon->tileIdx][mon->monIdx] = NULL;
   mon->monIdx = -1;
   
   return 0;
}

int sageDisplay::replaceMontage(sageMontage *mon)
{
   if (!mon) {
      sage::printLog("sageDisplay::replaceMontage : Error - input is NULL");
      return -1;
   }
   
   if (mon->tileIdx < 0 || mon->monIdx < 0)
      return -1;
      
   mon->visible = true;
   montages[mon->tileIdx][mon->monIdx] = mon;
   return 0;
}

int sageDisplay::updateScreen(dispSharedData *shared, bool barrierFlag)
{
   context->clearScreen();

   for (int i=0; i<tileNum; i++)   {
      sageRect tileRect = configStruct.tileRect[i];
      tileRect.updateBoundary();
      
      int tileX = (i % configStruct.dimX) * configStruct.width;
      int tileY = (i / configStruct.dimX) * configStruct.height;
      tileRect.x = tileX;
      tileRect.y = tileY;

      context->setupViewport(i, tileRect);

      drawObj.preDraw(tileRect);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(tileRect.left, tileRect.right, tileRect.bottom, tileRect.top, 0, 100);
      glMatrixMode(GL_MODELVIEW);
      for (int j=0; j<noOfMontages[i]; j++) {
         sageMontage *mon = montages[i][j];

         if (!mon)
            continue;
            
         if (!mon->visible) 
            continue;
            
         //if (!mon->isValidTexCoord()) {
         //sage::printLog("sageDisplay::updateScreen : montage %d of tile %d doesn't have valid texture coordinates", mon->id, i);
         //continue;
         //}
         
         if (mon->pixelType == PIXFMT_YUV) {
#if defined(GLSL_YUV)
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_RECTANGLE_ARB);
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mon->texHandle);
            glUseProgramObjectARB(PHandle);
            glActiveTexture(GL_TEXTURE0);
            int h=glGetUniformLocationARB(PHandle,"yuvtex");
            glUniform1iARB(h,0);  /* Bind yuvtex to texture unit 0 */
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mon->texHandle);
#else
            glBindTexture(GL_TEXTURE_2D, mon->texHandle);
#endif
         }
         else {
            glBindTexture(GL_TEXTURE_2D, mon->texHandle);
         }
         
         //GLenum error = glGetError();
         //gluGetErrorString( error ) ) );

         float depth = -mon->depth;
         glBegin(GL_QUADS);
            float texX, texY;
            mon->texCoord.genTexCoord(LOWER_LEFT, texX, texY);
            //   std::cerr << "LL " << texX << " , " << texY << " , " ;
            glTexCoord2f(texX, texY);
            glVertex3f(mon->left, mon->bottom, depth);
				//std::cout << " drawing... " << mon->left <<  " " << mon->bottom << " " << depth << std::endl;

            mon->texCoord.genTexCoord(LOWER_RIGHT, texX, texY);
            //   std::cerr << "LR " << texX << " , " << texY << " , " ;
            glTexCoord2f(texX, texY);
            glVertex3f(mon->right, mon->bottom, depth);

            mon->texCoord.genTexCoord(UPPER_RIGHT, texX, texY);
            //   std::cerr << "UR " << texX << " , " << texY << " , " ;
            
            glTexCoord2f(texX, texY);
            glVertex3f(mon->right, mon->top, depth);
            
            mon->texCoord.genTexCoord(UPPER_LEFT, texX, texY);
            //   std::cerr << "UL " << texX << " , " << texY << std::endl;
   
            glTexCoord2f(texX, texY);
            glVertex3f(mon->left, mon->top, depth);
         glEnd();

         if (mon->pixelType == PIXFMT_YUV) {
#if defined(GLSL_YUV)
            glUseProgramObjectARB(0);
            glDisable(GL_TEXTURE_RECTANGLE_ARB);
            glEnable(GL_TEXTURE_2D);
#endif
         }

      }
      drawObj.interDraw(tileRect);
      drawObj.postDraw(tileRect);

      context->refreshTile(i);
   }   
   
   //gettimeofday(&tve,NULL);   
  
   //double e = ((double)tve.tv_sec + 0.000001*(double)tve.tv_usec) - ((double)tvs.tv_sec + 0.000001*(double)tvs.tv_usec);
   //printf("%.9f elapsed\n", e); 
   //context->refreshScreen();
   
      /** BARRIER **/

   if ( barrierFlag ) {
	   shared->syncClientObj->sendRefreshBarrier(shared->nodeID);
	   //printf("node %d sent to barrier\n", shared->nodeID);
	   shared->syncClientObj->recvRefreshBarrier(false); // blocking (set true for nonblock)
	   //printf("node %d recved frm barrier\n", shared->nodeID);

   }

#ifdef DELAY_COMPENSATION
   if ( shared ) {
	   gettimeofday(&shared->localT, NULL); // my time

	   //fprintf(stderr,"SM sent %ld,%ld\n", shared->syncMasterT.tv_sec, shared->syncMasterT.tv_usec);
	   //fprintf(stderr,"SDM%d is %ld,%ld\n", shared->nodeID, shared->localT.tv_sec, shared->localT.tv_usec);
	   
	   double A = (double)shared->syncMasterT.tv_sec + 0.000001*(double)shared->syncMasterT.tv_usec;
	   A = A + ((double)shared->deltaT * 0.000001);
	   double B = (double)shared->localT.tv_sec + 0.000001*(double)shared->localT.tv_usec;
	   double wait = A - B;
	   wait *= 1000000.0;
	   
	   int llBusyWait = (int)wait;
	   if (  llBusyWait > 0 ) {
	     __usecDelay_RDTSC( llBusyWait );
	   }

	   //printf("SDM %d: %.6f msec waited\n", shared->nodeID, elapsed*1000.0);
   }
#endif

   context->refreshScreen();  // actual swapBuffer occurs in here at displayConext's instance
   dirty = false;
   
   return 0;
}

int sageDisplay::addDrawObjectInstance(char *data)
{
   drawObj.addObjectInstance(data);
   dirty = true;
   
   return 0;
}

int sageDisplay::updateObjectPosition(char *data)
{
   drawObj.updateObjectPosition(data);
   dirty = true;
      
   return 0;
}

int sageDisplay::removeDrawObject(char *data)
{
   int id;
   sscanf(data, "%d", &id);
   drawObj.removeObject(id);
   dirty = true;
      
   return 0;
}

int sageDisplay::forwardObjectMessage(char *data)
{
   drawObj.forwardObjectMessage(data);
   dirty = true;
      
   return 0;
}

int sageDisplay::showObject(char *data)
{
   drawObj.showObject(data);
   dirty = true;
      
   return 0;
}

sageDisplay::~sageDisplay()
{
} //End of sageDisplay::~sageDisplay()
