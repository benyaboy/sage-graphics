/*****************************************************************************************
 * imageflip: loads an image sequence and sends it to SAGE for display
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
#include "util.h"

#define FLIP_DEPTH 10


// Set of filenames
std::set<std::string> Names;
// Number of images to ne processed
int Limit;


#if MagickLibVersion >= 0x645
#define MagickGetImagePixels MagickGetAuthenticPixels
#endif



typedef unsigned char byte;
#define memalign(x,y) malloc((y))

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

void getRGB(string fileName, byte *rgb, unsigned int &width, unsigned int &height)
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

    // get the pixels
    MagickGetImagePixels(wand, 0, 0, width, height, "RGB", CharPixel, rgb);
    DestroyMagickWand(wand);
}

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------

void getDimensions(string fileName, unsigned int &width, unsigned int &height)
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

    DestroyMagickWand(wand);
}

// -----------------------------------------------------------------------------

void RefreshList(string dir)
{
   DIR *dirh;
  struct dirent *dirp;
  char pathname[1024];
  char linkname[1024];
  struct stat statbuf;

   const char *dn = dir.c_str();


if ((dirh = opendir(dn)) == NULL)
   {
   perror("opendir");
   exit(1);
   }


for (dirp = readdir(dirh); dirp != NULL; dirp = readdir(dirh))
   {
   if (strcmp(".",dirp->d_name) == 0 || strcmp("..",dirp->d_name) == 0)
      {
      continue;
      }
 
   sprintf(pathname,"%s/%s",dn,dirp->d_name);

   if (lstat(pathname,&statbuf) == -1)                /* see man stat */
     {
     perror("stat");
     continue;
     }

   if (S_ISREG(statbuf.st_mode))
      {
      //printf("%s is a regular file\n",pathname);
	string pn = string(pathname);
	string fileExt = pn.substr(pn.rfind("."));
	if ( (fileExt.compare(".JPG") == 0) || (fileExt.compare(".jpg") == 0) )
		if (Names.insert(pathname).second)
		{
			cout << pathname << " added successfully" << endl;
		}
		else
		{
			//cout << pathname << " rejected" << endl;
		}

      }

   if (S_ISDIR(statbuf.st_mode))
      {
      //printf("%s is a directory\n",pathname);
      }

   if (S_ISLNK(statbuf.st_mode))
      {
      bzero(linkname,1024);                         /* clear string */
      readlink(pathname,linkname,1024);
      //printf("%s is a link to %s\n",pathname,linkname);
      }

   //printf("The mode of %s is %o\n\n",pathname,statbuf.st_mode & 07777);
   }

   closedir(dirh);

  Limit = Names.size() < FLIP_DEPTH ? 0 : Names.size() - FLIP_DEPTH;

}

// -----------------------------------------------------------------------------



int main(int argc,char **argv)
{
    byte *sageBuffer = NULL;  // buffer for sage
    unsigned int width, height;  // image size
    unsigned int window_width=0, window_height=0;  // sage window size


    // parse command line arguments
    if (argc < 2){
	fprintf(stderr, "\n\nUSAGE: imageflip directory [width] [height]");
	return 0;
    }
    for (int argNum=2; argNum<argc; argNum++)
    {
	if(atoi(argv[argNum]) != 0 && atoi(argv[argNum+1]) != 0) {
	    window_width = atoi( argv[argNum] );
	    window_height = atoi( argv[argNum+1] );
	    argNum++;  // increment because we read two args here
	}
    }


    // check file extension
    string dirName;
    dirName = string(argv[1]);


		// Populate the filename list
	RefreshList(dirName);

   cout << Names.size() << " elements in the set" << endl;


	// List the names
  set<string>::iterator it;
  int k =0;
  for ( it = Names.begin() ; k < Limit ; it++ ) {k++;}
  for ( ; it != Names.end(); it++ )
    cout << " " << *it << endl;

   // last element
    cout << "Last element: " << *(Names.rbegin()) << endl;

    string firstname = *( Names.begin() );

    // if the user didn't specify the window size, use the image size
    if (window_height == 0 && window_width == 0)
    {
	// Get the dimensions of the first file
        getDimensions(firstname ,  width, height);
	window_width = width;
	window_height = height;
	cout << "Dimensions of image series: " << window_width << "x" << window_height << endl;
    }

    // initialize SAIL
    sail sageInf; // sail object
    sailConfig scfg;
    scfg.init("imageflip.conf");
    scfg.setAppName("imageflip");

    scfg.resX = width;
    scfg.resY = height;

    if (scfg.winWidth == -1 || scfg.winHeight == -1)
    {
	scfg.winWidth = window_width;
	scfg.winHeight = window_height;
    }

    scfg.pixFmt = PIXFMT_888;
    scfg.rowOrd = TOP_TO_BOTTOM;

    sageInf.init(scfg);


    // get buffer from SAGE and fill it with data
    sageBuffer = (byte*)sageInf.getBuffer();
	// Get some pixels
    getRGB(firstname, sageBuffer, width, height);
    sageInf.swapBuffer();

	// Rewind
    k = 0;
    for ( it = Names.begin() ; k < Limit ; it++ ) {k++;}

	// Wait the end
    while (1)
    {
	if (it == Names.end()) {
		// Rewind
		k = 0;
		for ( it = Names.begin() ; k < Limit ; it++ ) {k++;}
	}

		// get buffer from SAGE and fill it with data
	sageBuffer = (byte*)sageInf.getBuffer();
		// Get some pixels
	getRGB(*it, sageBuffer, width, height);
	cerr << "Getting " << *it << "\r";  // endl;
	sageInf.swapBuffer();
		// Next step
	it++;

		// Refresh the filename list
	RefreshList(dirName);

		// Process SAGE messages
	sageMessage msg;
	if (sageInf.checkMsg(msg, false) > 0) {
	    switch (msg.getCode()) {
	    case APP_QUIT:
		cout << "\nDone\n\n";
		sageInf.shutdown();
		exit(1);
		break;
	    }
	}
    }

    return 0;
}
