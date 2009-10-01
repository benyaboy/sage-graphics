/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
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
 * Direct questions, comments etc about SAGE to http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/
#include "sail.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <Carbon/Carbon.h>

#include <sys/time.h>

#include "libdxt.h"

// #define WW 1920
// #define HH 1200

#define WW 1440
#define HH 900

//#define WW 1024
//#define HH 768


// headers for SAGE
#include "misc.h"
int winWidth, winHeight;
GLubyte *rgbBuffer = 0;
GLubyte *dxtBuffer = 0;
sail sageInf; // sail object

int capture(char* m_pFrameRGB,int x,int y,int cx,int cy)
{
  CGLContextObj	 glContextObj = 0;
  CGLPixelFormatObj pixelFormatObj ;
  GLint numPixelFormats ;
  CGDirectDisplayID	displayId=CGMainDisplayID();
  CGOpenGLDisplayMask displayMask =
    CGDisplayIDToOpenGLDisplayMask(displayId);

  CGLPixelFormatAttribute attribs[] =
    {
      (CGLPixelFormatAttribute)kCGLPFAFullScreen,
      (CGLPixelFormatAttribute)kCGLPFADisplayMask,
      (CGLPixelFormatAttribute)displayMask,
      (CGLPixelFormatAttribute)0
    };
  CGLChoosePixelFormat( attribs, &pixelFormatObj, &numPixelFormats );
  CGLCreateContext( pixelFormatObj, NULL, &glContextObj ) ;
  CGLDestroyPixelFormat( pixelFormatObj ) ;
  CGLSetCurrentContext( glContextObj ) ;
  CGLSetFullScreen( glContextObj ) ;///UUUUUUUUUUnbelievable

  glReadBuffer(GL_FRONT);
  glReadPixels(x,y,cx,cy,GL_RGBA,GL_UNSIGNED_BYTE,m_pFrameRGB);

  CGLSetCurrentContext( NULL );
  CGLClearDrawable( glContextObj );
  CGLDestroyContext( glContextObj );
  return true;
}

int
main(int argc, char **argv)
{
  int ret;
  bool done;
  double rate, fps;
  double t1, t2;

        // Initialize some timing functions and else (DXT)
  aInitialize();


  sageRect ishareImageMap;
  ishareImageMap.left = 0.0;
  ishareImageMap.right = 1.0;
  ishareImageMap.bottom = 0.0;
  ishareImageMap.top = 1.0;

  sailConfig scfg;
  scfg.init("ishare.conf");
  scfg.setAppName("ishare");
  scfg.rank = 0;

  scfg.resX = WW;
  scfg.resY = HH;
  scfg.winWidth  = WW;
  scfg.winHeight = HH;
  scfg.imageMap = ishareImageMap;
  scfg.pixFmt = PIXFMT_DXT; // PIXFMT_888;
  scfg.rowOrd = BOTTOM_TO_TOP;
  scfg.master = true;

  sageInf.init(scfg);

  std::cout << "sail initialized " << std::endl;

  dxtBuffer = (GLubyte *)sageInf.getBuffer();
  rgbBuffer = (byte*)memalign(16, WW*HH*4);
  memset(rgbBuffer, 0,  WW*HH*4);

  rate = 15.0;

  done = false;
  while (not done)
    {
      t1 = sage::getTime();

      ret = capture((char*)rgbBuffer,0,0,WW,HH);

      CompressDXT(rgbBuffer, dxtBuffer, WW, HH, FORMAT_DXT1, 2);

      sageInf.swapBuffer();

      sageMessage msg;
      if (sageInf.checkMsg(msg, false) > 0) {
	switch (msg.getCode()) {
	case APP_QUIT:
	  done = true;
	  break;
	}
      }

      dxtBuffer = (GLubyte *)sageInf.getBuffer();

      t2 = sage::getTime();
      fps = (1000000.0/(t2-t1));
      if (fps > rate) {
	sage::usleep( (1000000.0/rate) - (t2-t1)  );
      }
    }

}
