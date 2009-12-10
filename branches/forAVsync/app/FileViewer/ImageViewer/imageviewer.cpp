/*****************************************************************************************
 * imageviewer: loads an image and sends it to SAGE for display
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
 * Direct questions, comments etc to www.evl.uic.edu/cavern/forum
 *
 * Author: Ratko Jagodic, Luc Renambot
 *
 *****************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <wand/magick-wand.h>

// headers for SAGE
#include "sail.h"
#include "misc.h"



#if MagickLibVersion >= 0x645
#define MagickGetImagePixels MagickGetAuthenticPixels
#endif



#ifdef USE_DXT

// for dxt compression
#include "libdxt.h"
// if true, it will show the original image and not the dxt compressed one
bool loadDXT = true;

#else

bool loadDXT = false;
typedef unsigned char byte;
#define memalign(x,y) malloc((y))

#endif


using namespace std;


#define ThrowWandException(wand)		\
  {						\
    char					\
      *description;				\
						\
    ExceptionType				\
      severity;					\
						    \
    description=MagickGetException(wand,&severity);			\
    (void) fprintf(stderr,"%s %s %ld %s\n",GetMagickModule(),description); \
    description=(char *) MagickRelinquishMemory(description);		\
    exit(-1);								\
  }


// -----------------------------------------------------------------------------

#ifdef USE_DXT
void readDXT(string fileName, byte **buffer, unsigned int &width,
	     unsigned int &height)
{
    unsigned long r;
    unsigned int numBytes;
    FILE *f = fopen(fileName.data(), "rb");

    // read the size of the image from the first 8 bytes
    r = fread(&width, sizeof(unsigned int), 1, f);
    r = fread(&height, sizeof(unsigned int), 1, f);
    r = fread(&numBytes, sizeof(unsigned int), 1, f);

    // allocate buffer size and read the rest of the file into it
    *buffer = (byte*) malloc(width*height*4/8);
    memset(*buffer, 0, width*height*4/8);
    r = fread(*buffer, 1, width*height*4/8, f);
    fclose(f);
}


void writeDXT(string fileName, byte *buffer, unsigned int width,
	      unsigned int height, unsigned int numBytes)
{
    fprintf(stderr, "\n**** Writing DXT to file... %u bytes", numBytes);

    unsigned long r;
    FILE *f = fopen(fileName.data(), "wb");

    if (f != NULL)
    {
	// write the size of the image in the first 8 bytes
	r = fwrite(&width, sizeof(unsigned int), 1, f);
	r = fwrite(&height, sizeof(unsigned int), 1, f);
	r = fwrite(&numBytes, sizeof(unsigned int), 1, f);

	// write the buffer out to the file
	r = fwrite(buffer, 1, numBytes, f);
	fclose(f);
    }
    else
	fprintf(stderr, "\n**** Imageviewer ERROR: Unable to write DXT file. Check dir permissions.\n");
}
#endif


void getRGBA(string fileName, byte **rgba, unsigned int &width,
	      unsigned int &height)
{
    // use ImageMagick to read all other formats
    MagickBooleanType status;
    MagickWand *wand;

    // read file
    wand=NewMagickWand();
    status=MagickReadImage(wand, fileName.data());
    if (status == MagickFalse)
	ThrowWandException(wand);

    // get the image size
    width = MagickGetImageWidth(wand);
    height = MagickGetImageHeight(wand);

    // crop the image if necessary to make sure it's a multiple of 4
    if (loadDXT)
    {
	if (width%4 != 0 || height%4 != 0)
	{
	    fprintf(stderr, "\n**** Image cropped a few pixels to be a multiple of 4 for dxt");
	    width -= width%4;
	    height -= height%4;
	}

	// flip the image to have the correct orientation for dxt
	MagickFlipImage(wand);
    }

    // get the pixels
    *rgba = (byte*) memalign(16, width*height*4);
    memset(*rgba, 0, width*height*4);
    MagickGetImagePixels(wand, 0, 0, width, height, "RGBA", CharPixel, *rgba);
    DestroyMagickWand(wand);
}

#ifdef USE_DXT
void rgbaToDXT(string fileName, byte **rgba, byte **dxt, unsigned int width,
	       unsigned int height)
{
    unsigned int numBytes;

    // compress into DXT
    *dxt = (byte*) memalign(16, width*height*4/8);
    memset(*dxt, 0, width*height*4/8);
    numBytes = CompressDXT(*rgba, *dxt, width, height, FORMAT_DXT1, 1);

    // write this DXT out to a file (change extension to .dxt)
    string dxtFileName = string(fileName);
    dxtFileName.resize(fileName.rfind("."));
    dxtFileName += ".dxt";
    writeDXT(dxtFileName, *dxt, width, height, numBytes);
}


bool dxtFileExists(string fileName)
{
    // replace the extension with .dxt
    string dxtFileName = string(fileName);
    dxtFileName.resize(fileName.rfind("."));
    dxtFileName += ".dxt";

    // check whether the file exists by trying to open it
    FILE *dxtFile = fopen(dxtFileName.data(), "r");
    if ( dxtFile == NULL)
    {
	fprintf(stderr, "\nDXT file for %s doesn't exist yet.", fileName.data());
	return false;
    }
    else
    {
	fclose(dxtFile);
	return true;
    }
}
#endif

// -----------------------------------------------------------------------------



int main(int argc,char **argv)
{
    byte *sageBuffer = NULL;  // buffers for sage and dxt data
    byte *dxt = NULL;
    byte *rgba = NULL;
    unsigned int width, height;  // image size
    unsigned int window_width=0, window_height=0;  // sage window size


    // parse command line arguments
    if (argc < 2){
	fprintf(stderr, "\n\nUSAGE: imageviewer filename [width] [height] [-show_original]");
	return 0;
    }
    for (int argNum=2; argNum<argc; argNum++)
    {
	if (strcmp(argv[argNum], "-show_original") == 0) {
	    loadDXT = false;
	}
	else if(atoi(argv[argNum]) != 0 && atoi(argv[argNum+1]) != 0) {
	    window_width = atoi( argv[argNum] );
	    window_height = atoi( argv[argNum+1] );
	    argNum++;  // increment because we read two args here
	}
    }


    // check file extension
    string fileName, fileExt;
    fileName = string(argv[1]);
    fileExt = fileName.substr(fileName.rfind("."));

    // if image is in DXT load it directly, otherwise compress and load
    if(fileExt.compare(".dxt") == 0)   // DXT
#ifndef USE_DXT
    {
	fprintf(stderr, "\n\nImageviewer was not compiled with DXT option\n");
	exit(0);
    }
#else
	readDXT(fileName, &dxt, width, height);

    else if(loadDXT && dxtFileExists(fileName))
    {
	// replace the extension with .dxt and read the file
	string dxtFileName = string(fileName);
	dxtFileName.resize(fileName.rfind("."));
	dxtFileName += ".dxt";
	readDXT(dxtFileName, &dxt, width, height);
    }
#endif
    else                               // all other image formats
    {
	getRGBA(fileName, &rgba, width, height);
	#ifdef USE_DXT
	if (loadDXT)
	{
	    rgbaToDXT(fileName, &rgba, &dxt, width, height);
	    free(rgba);
	    rgba = NULL;
	}
	#endif
    }

    // if the user didn't specify the window size, use the image size
    if (window_height == 0 && window_width == 0)
    {
	window_width = width;
	window_height = height;
    }

    // initialize SAIL
    sail sageInf; // sail object
    sailConfig scfg;
    scfg.init("imageviewer.conf");
    scfg.setAppName("imageviewer");

    scfg.resX = width;
    scfg.resY = height;

    if (scfg.winWidth == -1 || scfg.winHeight == -1)
    {
	scfg.winWidth = window_width;
	scfg.winHeight = window_height;
    }

    if (loadDXT)
    {
	scfg.pixFmt = PIXFMT_DXT;
	scfg.rowOrd = BOTTOM_TO_TOP;
    }
    else
    {
	scfg.pixFmt = PIXFMT_8888;
	scfg.rowOrd = TOP_TO_BOTTOM;
    }
    sageInf.init(scfg);


    // get buffer from SAGE and fill it with dxt data
    sageBuffer = (byte*)sageInf.getBuffer();
    if (loadDXT)
	memcpy(sageBuffer, dxt, width*height*4/8);
    else
	memcpy(sageBuffer, rgba, width*height*4);
    sageInf.swapBuffer();

    // a second time for the backbuffer
    /* doesn't need with END_FRAME fix
    sageBuffer = (byte*)sageInf.getBuffer();
    if (loadDXT)
	memcpy(sageBuffer, dxt, width*height*4/8);
    else
	memcpy(sageBuffer, rgba, width*height*4);
    sageInf.swapBuffer();
    */

    // release the memory
    free(dxt);
    free(rgba);


    // Wait the end
    while (1)
    {
	usleep(500000);  // so that we don't keep spinning too frequently (0.5s)
	sageMessage msg;
	if (sageInf.checkMsg(msg, false) > 0) {
	    switch (msg.getCode()) {
	    case APP_QUIT:
		sageInf.shutdown();
		exit(1);
		break;
	    }
	}
    }

    return 0;
}
