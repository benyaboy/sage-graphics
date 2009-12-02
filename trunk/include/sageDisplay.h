/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageDisplay.h
 * Author : Byungil Jeong, Rajvikram Singh
 *
 *   Description:   This is the header file for the video display module of SAGE.
 *
 *   Notes   :    Since the display window may receive its many pieces from many servers, we need to pass multiple buffers
 *         to this class. Each of the buffers passed contains a frame which forms a part (or whole) of the image on
 *         the display side.
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
***************************************************************************************************************************/

#ifndef SAGE_DISPLAY_H
#define SAGE_DISPLAY_H

#ifndef GL_ABGR_EXT
   #  define GL_ABGR_EXT 0x8000
#endif
#ifndef GL_BGR
   #  define GL_BGR 0x80E0
#endif
#ifndef GL_BGRA
   #  define GL_BGRA 0x80E1
#endif


#ifndef GL_UNSIGNED_SHORT_5_6_5
   #  define GL_UNSIGNED_SHORT_5_6_5      0x8363
#endif

// awf: more (similar) defines for win32...
#ifndef GL_UNSIGNED_SHORT_5_5_5_1
   #  define GL_UNSIGNED_SHORT_5_5_5_1      0x8034
#endif
#ifndef GL_UNSIGNED_SHORT_1_5_5_5_REV
   #  define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8066
#endif

#include "displayContext.h"
#include "sageDraw.h"
#include "sagePixelType.h"
#include "sageSharedData.h"



#define MAX_MONTAGE_NUM    100

class sageBlock;
class sagePixelBlock;

// The following structure holds the parameters of the montage comprising the final window

/**
 * This holds the parameters of the montage comprising the final window
 */
class sageMontage : public sageRect {
private:
   displayContext *context;
   sagePixelType pInfo;

   int      texWidth, texHeight;      // current texture size for this montage
   //bool     validTexCoord;
   //sageRect imgInfo;
   sageRect texInfo;

   GLubyte *texture;

public:
   int       texHandle;
   sageRect texCoord;
   sagePixFmt pixelType;
   float    depth;
   int      monIdx;
   int      tileIdx;
   bool      visible;
   //int id;

   sageMontage(displayContext *dct, sagePixFmt pfmt);

   int init(sageRect &viewPort, sageRect &blockLayout,
         sageRotation orientation);
   int copyConfig(sageMontage &mon);
   bool checkDispInfo(sagePixelBlock *block);
   int renewTexture();
   int deleteTexture();
   bool genTexCoord();
   int loadPixelBlock(sagePixelBlock *block);  // load a pixel block into texture memory

   //inline void getImageOrg(int &x, int &y) { x = imgInfo.x, y = imgInfo.y; }
};

/**
 * \brief The video display module of SAGE.
 *
 * This class is for drawing pixel data onto screen, and all OpenGL related codes are here
 *
 * Since the display window may receive its many pieces from many servers, we need to pass multiple buffers to this class.
 * Each of the buffers passed contains a frame which forms a part (or whole) of the image on the display side.
 */
class sageDisplay {
protected:
   displayContext *context;
   sageDisplayConfig configStruct;

   sageMontage*       montages[MAX_TILES_PER_NODE][MAX_MONTAGE_NUM];
   int noOfMontages[MAX_TILES_PER_NODE];

   int tileNum;
   bool dirty;
   sageDraw drawObj;

public:
   sageDisplay(displayContext *dct, struct sageDisplayConfig &cfg);
   int addMontage(sageMontage *mon);  // register a new montage into the montage list
   int replaceMontage(sageMontage *mon); // replace the current montage by another montage
   int removeMontage(sageMontage *mon);  // delete montage from the montage list
   int updateScreen(dispSharedData *shared=NULL, bool barrierFlag=false); /**< draw montages onto screen */
   int changeBGColor(int red, int green, int blue);

   int addDrawObjectInstance(char *data);
   int updateObjectPosition(char *data);
   int removeDrawObject(char *data);
   int forwardObjectMessage(char *data);
   int showObject(char *data);

   inline void setDirty() { dirty = true; }
   inline bool isDirty() { return dirty; }
   inline sageRect& getTileRect(int idx) { return configStruct.tileRect[idx]; }
   inline int getTileNum() { return tileNum; }
//   int setupWinBorders(char *info);
//   int drawWinBorders();
//   int updateWinBorders(char *info);
//   int updateWinDepth(int appID, float depth, float titleOffset);
//   int deleteWinBorders(int appID);

   ~sageDisplay();
};

#endif
