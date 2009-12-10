/*=============================================================================

  Program: JuxtaView for SAGE
  Module:  main.cpp - main MPI control for JuxtaView
  Authors: Nicholas Schwarz, schwarz@evl.uic.edu,
           Arun Rao, arao@evl.uic.edu,
           Luc Renambot, luc@evl.uic.edu,
           et al.
  Date:    30 September 2004
  Modified: 9 September 2005

   Copyright (C) 2004 Electronic Visualization Laboratory,
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
 * Direct questions, comments etc to schwarz@evl.uic.edu or
 * http://www.evl.uic.edu/cavern/forum/

  Direct questions, comments, etc. to schwarz@evl.uic.edu or
  http://www.evl.uic.edu/cavern/forum/

=============================================================================*/


#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// for MPI
#include "mpi.h"

// for UI
#include "JuxtaUIServer.h"

// for communication with UI via QUANTA
#include <QUANTA/QUANTAinit.hxx>
#include <QUANTA/QUANTAnet_tcp_c.hxx>

// for converting global image coordinates, in pixels, to
// individual files and their respective extents
#include "GlobalCoordinatesToFileMapper.h"

// for reading tiff files
#include <tiffio.h>

// for SAGE
#include <sail.h>
#include <misc.h>

// SAGE Stuff
int winWidth, winHeight;
sail sageInf; // sail object

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define FASTLEFT 4
#define FASTRIGHT 5
#define FASTUP 6
#define FASTDOWN 7
#define ZOOMOUT 8
#define ZOOMIN 9
#define QUIT 10
#define EXTENTS 11
#define OVERVIEW 12
#define FREETRANS 13
#define RESEND 14

// windows border definitions
#define DBDR_TBLR 0
#define DBDR_TB 1
#define DBDR_LR 2
#define DBDR_TL 3
#define DBDR_TR 4
#define DBDR_BL 5
#define DBDR_BR 6
#define DBDR_T 7
#define DBDR_B 8
#define DBDR_L 9
#define DBDR_R 10
#define DBDR_N 11
#define DBDR_TLR 12
#define DBDR_BLR 13
#define DBDR_TBL 14
#define DBDR_TBR 15

#include <time.h>


/* data set information */
typedef struct {
  long origin_x;         // image origin, usually 0,0
  long origin_y;         // image origin, usually 0,0
  long image_width;      // image width
  long image_height;     // image height
  long start_x;          // start position
  long start_y;          // start position
  long pan_amount;       // number of pixels to pan
  long fast_pan_amount;  // number of pixels to fast pan
  long fetch_w;          // horizontal extent to fetch, i.e. window size
  long fetch_h;          // vertical extent to fetch, i.e. window size
  int samplerate;        // sample rate for overview file
  char name[512];        // overview file name
} description;


/* extent within the global image in pixel coordinates and the command */
typedef struct {
  long x;
  long y;
  long w;
  long h;
  long command;
  long zoom;
  long border;
} info;


/* viewport in normalized coordinates */
typedef struct {
  float x_min;
  float y_min;
  float x_max;
  float y_max;
  long border;
  char ip[16];
} viewport;


/* begin - read config file given file name and return array of viewport */
viewport* read_config(const char* name) {

  // number of nodes in addition to master
  int num;

  // border
  char border[10];

  // file pointer
  FILE *fptr = NULL;

  // attempt to open the configuration file.
  if ((fptr = fopen(name, "r")) == NULL)
    return NULL;

  // read number of nodes
  fscanf(fptr, "%d", &num);

  // allocate space for node configuration
  viewport* viewports = (viewport*) malloc(sizeof(viewport) * num);

  // check if memory allocation succeded
  if (viewports == NULL)
    return NULL;

  // Read configuration data
  for (int i = 0 ; i < num ; i++)
    {
      // read viewports
      fscanf(fptr, "%f", &viewports[i].x_min);
      //printf("%f\n", viewports[i].x_min);
      fscanf(fptr, "%f", &viewports[i].y_min);
      //printf("%f\n", viewports[i].y_min);
      fscanf(fptr, "%f", &viewports[i].x_max);
      //printf("%f\n", viewports[i].x_max);
      fscanf(fptr, "%f", &viewports[i].y_max);
      //printf("%f\n", viewports[i].y_max);
      fscanf(fptr, "%s", border);
      //printf("%s\n\n", border);

      // determine and set border type
      if (!strcmp(border, "BDR_TBLR")) {
	viewports[i].border = DBDR_TBLR;
      }
      else if (!strcmp(border, "BDR_TB")) {
	viewports[i].border = DBDR_TB;
      }
      else if (!strcmp(border, "BDR_LR")) {
	viewports[i].border = DBDR_LR;
      }
      else if (!strcmp(border, "BDR_TL")) {
	viewports[i].border = DBDR_TL;
      }
      else if (!strcmp(border, "BDR_TR")) {
	viewports[i].border = DBDR_TR;
      }
      else if (!strcmp(border, "BDR_BL")) {
	viewports[i].border = DBDR_BL;
      }
      else if (!strcmp(border, "BDR_BR")) {
	viewports[i].border = DBDR_BR;
      }
      else if (!strcmp(border, "BDR_T")) {
	viewports[i].border = DBDR_T;
      }
      else if (!strcmp(border, "BDR_B")) {
	viewports[i].border = DBDR_B;
      }
      else if (!strcmp(border, "BDR_L")) {
	viewports[i].border = DBDR_L;
      }
      else if (!strcmp(border, "BDR_R")) {
	viewports[i].border = DBDR_R;
      }
      else if (!strcmp(border, "BDR_N")) {
	viewports[i].border = DBDR_N;
      }
      else if (!strcmp(border, "BDR_TLR")) {
	viewports[i].border = DBDR_TLR;
      }
      else if (!strcmp(border, "BDR_BLR")) {
	viewports[i].border = DBDR_BLR;
      }
      else if (!strcmp(border, "BDR_TBL")) {
	viewports[i].border = DBDR_TBL;
      }
      else if (!strcmp(border, "BDR_TBR")) {
	viewports[i].border = DBDR_TBR;
      }
      else {
	viewports[i].border = DBDR_N;
      }

      fscanf(fptr, "%s", viewports[i].ip);
      //printf("%s\n", viewports[i].ip);

    }

  // close file
  fclose(fptr);

  // return
  return viewports;

} /* end - read config file */


/* begin - read data description file given file name */
description read_data(const char* name) {

  // data description
  description data;

  // file pointer
  FILE *fptrd = NULL;

  // attempt to open the configuration file.
  if ((fptrd = fopen(name, "r")) == NULL) {
    printf("Can not read data description file.\n");
    exit(EXIT_FAILURE);
  }

  // read origin and extent
  fscanf(fptrd, "%ld %ld %ld %ld", &data.origin_x, &data.origin_y,
	 &data.image_width, &data.image_height);

  // read starting position
  fscanf(fptrd, "%ld %ld", &data.start_x, &data.start_y);

  // read pan values
  fscanf(fptrd, "%ld %ld", &data.pan_amount, &data.fast_pan_amount);

  // width and height of data to fetch
  fscanf(fptrd, "%ld %ld", &data.fetch_w, &data.fetch_h);

  // overview file sample rate
  fscanf(fptrd, "%d", &data.samplerate);

  // overview file name
  fscanf(fptrd, "%s", data.name);

  // close file
  fclose(fptrd);

  // return true
  return data;

} /* end - read data description file */


/* begin - add border to the top; called by add_border */
void add_border_top(unsigned char* buffer, info extent) {

  unsigned char r = 139;
  unsigned char g = 161;
  unsigned char b = 105;

  for (long row = 0 ; row < 8 ; row++) {
    for (long col = 0 ; col < (extent.w / extent.zoom) ; col++) {
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3)] = r;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 1] = g;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 2] = b;
    }
  }

} /* end - add border to the top; called by add_border */


/* begin - add border to the top; called by add_border */
void add_border_bottom(unsigned char* buffer, info extent) {

  unsigned char r = 139;
  unsigned char g = 161;
  unsigned char b = 105;

  for (long row = (extent.h / extent.zoom) - 8 ;
       row < (extent.h / extent.zoom) ;
       row++) {
    for (long col = 0 ; col < (extent.w / extent.zoom) ; col++) {
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3)] = r;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 1] = g;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 2] = b;
    }
  }

} /* end - add border to the bottom; called by add_border */


/* begin - add border to the left; called by add_border */
void add_border_left(unsigned char* buffer, info extent) {

  unsigned char r = 139;
  unsigned char g = 161;
  unsigned char b = 105;

  for (long col = 0 ; col < 8 ; col++) {
    for (long row = 0 ; row < (extent.h / extent.zoom) ; row++) {
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3)] = r;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 1] = g;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 2] = b;
    }
  }

} /* end - add border to the left; called by add_border */


/* begin - add border to the right; called by add_border */
void add_border_right(unsigned char* buffer, info extent) {

  unsigned char r = 139;
  unsigned char g = 161;
  unsigned char b = 105;

  for (long col = (extent.w / extent.zoom) - 8 ;
       col < (extent.w / extent.zoom) ;
       col++) {
    for (long row = 0 ; row < (extent.h / extent.zoom) ; row++) {
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3)] = r;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 1] = g;
      buffer[row * 3 * (extent.w / extent.zoom) + (col * 3) + 2] = b;
    }
  }

} /* end - add border to the right; called by add_border */


/* begin - add border to the buffer */
void add_border(unsigned char* buffer, info extent) {

  switch(extent.border) {
  case DBDR_TBLR:
    add_border_top(buffer, extent);
    add_border_bottom(buffer, extent);
    add_border_left(buffer, extent);
    add_border_right(buffer, extent);
    break;
  case DBDR_TB:
    add_border_top(buffer, extent);
    add_border_bottom(buffer, extent);
    break;
  case DBDR_LR:
    add_border_left(buffer, extent);
    add_border_right(buffer, extent);
    break;
  case DBDR_TL:
    add_border_top(buffer, extent);
    add_border_left(buffer, extent);
    break;
  case DBDR_TR:
    add_border_top(buffer, extent);
    add_border_right(buffer, extent);
    break;
  case DBDR_BL:
    add_border_bottom(buffer, extent);
    add_border_left(buffer, extent);
    break;
  case DBDR_BR:
    add_border_bottom(buffer, extent);
    add_border_right(buffer, extent);
    break;
  case DBDR_T:
    add_border_top(buffer, extent);
    break;
  case DBDR_B:
    add_border_bottom(buffer, extent);
    break;
  case DBDR_L:
    add_border_left(buffer, extent);
    break;
  case DBDR_R:
    add_border_right(buffer, extent);
    break;
  case DBDR_N:
    break;
  case DBDR_TLR:
    add_border_top(buffer, extent);
    add_border_left(buffer, extent);
    add_border_right(buffer, extent);
    break;
  case DBDR_BLR:
    add_border_bottom(buffer, extent);
    add_border_left(buffer, extent);
    add_border_right(buffer, extent);
    break;
  case DBDR_TBL:
    add_border_top(buffer, extent);
    add_border_bottom(buffer, extent);
    add_border_left(buffer, extent);
    break;
  case DBDR_TBR:
    add_border_top(buffer, extent);
    add_border_bottom(buffer, extent);
    add_border_right(buffer, extent);
    break;
  }

} /* end - add border to the buffer */


/* begin - main */
int main(int argc, char** argv) {

  bool quit = false; // exit flag

  info extent;  // extent that each node fetches

  long zoom = 1;      // zoom out factor
  long max_zoom = 7;  // max zoom out factor

  int rank = 0;  // node rank
  int size = 0;  // number of nodes
  int frameRate = 5;

  MPI_Datatype pinfo;  // info structure
  MPI_Datatype ipinfo; // ip infor structure

  // MPI Initialization
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Type_contiguous(7, MPI_LONG, &pinfo);
  MPI_Type_contiguous(16, MPI_CHAR, &ipinfo);
  MPI_Type_commit(&pinfo);
  MPI_Type_commit(&ipinfo);

  // Print some information about the program
  printf("----------------------------------------------------------------\n");
  printf("JuxtaView for SAGE - Node %d of %d\n", rank, size);
  printf("9 September 2005\n");
  printf("----------------------------------------------------------------\n");

  // sync
  MPI_Barrier(MPI_COMM_WORLD);

  /* *********************************************************************** */
  /* Begin - Master node                                                     */

  if (rank == 0) {

    /* Initialize QUANTA */
    QUANTAinit();

    /* Create a server for the UI */
    JuxtaUIServer* server = new JuxtaUIServer();

    /* Initialize server for UI */
    if (!server -> Init(argv[4])) {
      delete server;
      printf("Can not create a UI server.\n");
      return EXIT_FAILURE;
    }

    /* read viewports for each node */
    viewport* viewports = read_config(argv[1]);
    //cout << "********MASTER********** config " << argv[1] << endl;

    /* check that viewports were read correctly */
    if (viewports == NULL) {
      printf("Can not read config file.\n");
      return EXIT_FAILURE;
    }

    /* read data description file */
    //description data = read_data("juxtadata.conf");
    description data = read_data(argv[2]);
    //cout << "********MASTER********** data " << argv[2]<< endl;

    /* begin - read overview file */

    //if the bitmap hasn't been read yet then read it!
    //for now just read the whole tiff file
    TIFF* tiffile = TIFFOpen(data.name, "r");

    uint32 width = 1, height = 1;

    if(tiffile)
      {
	//get the image width and height
	TIFFGetField(tiffile, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tiffile, TIFFTAG_IMAGELENGTH, &height);
      }

    //get the pixels
    int samplerate = data.samplerate;
    int bmpW = width / samplerate;
    int bmpH = height / samplerate;
    if( bmpW == 0)
      bmpW = 1;
    if( bmpH == 0)
      bmpH = 1;

    unsigned char* bitmap = new unsigned char[bmpW * bmpH * 3];
    fprintf(stderr,"Loading overview of size %d x %d\n",bmpW,bmpH);
    memset(bitmap,0,bmpW * bmpH * 3);
    if(tiffile)
      {
	char* scanline = (char*) _TIFFmalloc( TIFFScanlineSize(tiffile));

	for( int row = 0; row < bmpH; row++)
	  {
	    TIFFReadScanline(tiffile,scanline,row * samplerate);
	    for( int i = 0; i < bmpW; i++)
	      {
		memcpy(bitmap + (i * 3) + (row * 3 * bmpW),
		       scanline + (samplerate * i * 3),
		       3);
	      }
	  }

	_TIFFfree(scanline);
	TIFFClose(tiffile);
      }

    /* end - read overview file */

    printf("Origin X: %ld\n", data.origin_x);
    printf("Origin Y: %ld\n", data.origin_y);
    printf("Image Width: %ld\n", data.image_width);
    printf("Image Height: %ld\n", data.image_height);
    printf("Start x: %ld\n", data.start_x);
    printf("Start y: %ld\n", data.start_y);
    printf("Pan amount: %ld\n", data.pan_amount);
    printf("Fast pan amount: %d\n", data.fast_pan_amount);
    printf("Fetch w: %ld\n", data.fetch_w);
    printf("Fetch h: %ld\n", data.fetch_h);

    /* extent to fetch */
    long Xg = data.start_x;  // current x position in global image
    long Yg = data.start_y;  // current y position in global image
    long Wg = data.fetch_w;  // width of extent to fetch
    long Hg = data.fetch_h;  // height of extent to fetch

    for (int j = 0 ; j < size - 1 ; j++) {
      MPI_Send(&(viewports[j].x_min), 1, MPI_FLOAT, j+1, 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int j = 0 ; j < size - 1 ; j++) {
      MPI_Send(&(viewports[j].y_min), 1, MPI_FLOAT, j+1, 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int j = 0 ; j < size - 1 ; j++) {
      MPI_Send(&(viewports[j].x_max), 1, MPI_FLOAT, j+1, 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int j = 0 ; j < size - 1 ; j++) {
      MPI_Send(&(viewports[j].y_max), 1, MPI_FLOAT, j+1, 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int j = 0 ; j < size - 1 ; j++) {
      MPI_Send(viewports[j].ip, 1, ipinfo, j+1, 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // insert code here to show first frame before GUI starts

    // calculate and send extent for each node
    for (int i = 0 ; i < size - 1 ; i++) {
      extent.x = ((long)(Wg * viewports[i].x_min)) + Xg;
      extent.y = ((long)(Hg * viewports[i].y_min)) + Yg;
      extent.w = (long)(Wg * (viewports[i].x_max - viewports[i].x_min));
      extent.h = (long)(Hg * (viewports[i].y_max - viewports[i].y_min));
      extent.zoom = zoom;
      extent.border = viewports[i].border;
      MPI_Send(&extent, 1, pinfo, i+1, 1, MPI_COMM_WORLD);
    }

    // synchronize
    MPI_Barrier(MPI_COMM_WORLD);


    //cout << "Master entering loop" << endl;

    /* begin - event loop */
    while (!quit) {

      // block until incoming message from UI
      char* msg = server -> WaitForMessage();

      //printf("Before: xg %ld, yg %ld, wg %ld, hg %ld\n", Xg, Yg, Wg, Hg);

      // pan left
      if (!strcmp(msg, JVPAN_LEFT)) {
	if (Xg - data.pan_amount >= data.origin_x) {
	  Xg = Xg - data.pan_amount;
	}
	else {
	  Xg = data.origin_x;
	}
	extent.command = LEFT;
      }

      // pan right
      else if (!strcmp(msg, JVPAN_RIGHT)) {
	if (Xg + Wg + data.pan_amount <
	    data.origin_x + data.image_width) {
	  Xg = Xg + data.pan_amount;
	}
	else {
	  Xg = Xg + (data.origin_x + data.image_width) - (Xg + Wg) - 1;
	}
	extent.command = RIGHT;
      }

      // pan up
      else if (!strcmp(msg, JVPAN_UP)) {
	if (Yg - data.pan_amount >= data.origin_y) {
	  Yg = Yg - data.pan_amount;
	}
	else {
	  Yg = data.origin_y;
	}
	extent.command = UP;
      }

      // pan down
      else if (!strcmp(msg, JVPAN_DOWN)) {
	if (Yg + Hg + data.pan_amount <=
	    data.origin_y + data.image_height) {
	  Yg = Yg + data.pan_amount;
	}
	else {
	  Yg = Yg + (data.origin_y + data.image_height) - (Yg + Hg) - 1;
	}
	extent.command = DOWN;
      }

      // fast pan left
      else if(!strcmp(msg, JVFAST_PAN_LEFT)) {
	if (Xg - data.fast_pan_amount >= data.origin_x) {
	  Xg = Xg - data.fast_pan_amount;
	}
	else {
	  Xg = data.origin_x;
	}
	extent.command = FASTLEFT;
      }

      // fast pan right
      else if(!strcmp(msg, JVFAST_PAN_RIGHT)) {
	if (Xg + Wg + data.fast_pan_amount <
	    data.origin_x + data.image_width) {
	  Xg = Xg + data.fast_pan_amount;
	}
	else {
	  Xg = Xg + (data.origin_x + data.image_width) - (Xg + Wg) - 1;
	}
	extent.command = FASTRIGHT;
      }

      // fast pan up
      else if(!strcmp(msg, JVFAST_PAN_UP)) {
	if (Yg - data.fast_pan_amount >= data.origin_y) {
	  Yg = Yg - data.fast_pan_amount;
	}
	else {
	  Yg = data.origin_y;
	}
	extent.command = FASTUP;
      }

      // fast pan down
      else if(!strcmp(msg, JVFAST_PAN_DOWN)) {
	if (Yg + Hg + data.fast_pan_amount <=
	    data.origin_y + data.image_height) {
	  Yg = Yg + data.fast_pan_amount;
	}
	else {
	  Yg = Yg + (data.origin_y + data.image_height) - (Yg + Hg) -1;
	}
	extent.command = FASTDOWN;
      }

      // zoom out
      else if(!strcmp(msg, JVZOOM_OUT)) {
	if (zoom + 1 <= max_zoom) {

	  // calcualte new upper left corner and extents
	  long center_x = Xg + (Wg / 2);
	  long center_y = Yg + (Hg / 2);
	  long tXg = center_x - ((Wg + data.fetch_w) / 2);
	  long tYg = center_y - ((Hg + data.fetch_h) / 2);
	  long tHg = Hg + data.fetch_h;
	  long tWg = Wg + data.fetch_w;

	  // check that zoomed out image is within the image size
	  if (tWg <= data.image_width && tHg <= data.image_height) {

	    // new x position is too far left
	    if (tXg < data.origin_x) {
	      tXg = data.origin_x;
	    }

	    // new y position is too far up
	    if (tYg < data.origin_y) {
	      tYg = data.origin_y;
	    }

	    // new x extent is too far right
	    if (tXg + tWg >= data.origin_x + data.image_width) {
	      long delta_x = (tXg + tWg) - (data.origin_x + data.image_width);
	      tXg = tXg - delta_x - 1;
	    }

	    // new y extent is too far down
	    if (tYg + tHg >= data.origin_y + data.image_height) {
	      long delta_y = (tYg + tHg) - (data.origin_y + data.image_height);
	      tYg = tYg - delta_y - 1;
	    }

	    // update everthing
	    zoom++;
	    Xg = tXg;
	    Yg = tYg;
	    Hg = tHg;
	    Wg = tWg;

	  }

	}
	extent.command = ZOOMOUT;
      }

      // zoom in
      else if(!strcmp(msg, JVZOOM_IN)) {
	if (zoom - 1 >= 1) {
	  if ((Yg + Hg - data.fetch_h <= data.origin_y + data.image_height) &&
	      (Xg + Wg - data.fetch_w <= data.origin_x + data.image_width)) {
	    long center_x = Xg + (Wg / 2);
	    long center_y = Yg + (Hg / 2);
	    Xg = center_x - ((Wg - data.fetch_w) / 2);
	    Yg = center_y - ((Hg - data.fetch_h) / 2);
	    extent.command = ZOOMIN;
	    zoom--;
	    Hg -= data.fetch_h;
	    Wg -= data.fetch_w;
	  }
	}
	extent.command = ZOOMIN;
      }

      // free translation
      else if (!strcmp(msg, JVTRANSLATE)) {

	// normalized free translation coordinates
	float nx = 0.0;
	float ny = 0.0;

	// get normalized free translation coordinates
	server -> GetNormalizedFreeTranslation(&nx, &ny);

	// convert normalized values to actual pixel coordinates
	long tXg = (long) (nx * data.image_width);
	long tYg = (long) (ny * data.image_height);

	// set command
	extent.command = FREETRANS;

	// check left x
	if (tXg < data.origin_x)
	  Xg = data.origin_x;
	else
	  Xg = tXg;

	// check top y
	if (tYg < data.origin_y)
	  Yg = data.origin_y;
	else
	  Yg = tYg;

	// check right x
	if (tXg + Wg >= data.origin_x + data.image_width)
	  Xg = Xg + (data.origin_x + data.image_width) - (Xg + Wg) - 1;
	else
	  Xg = tXg;

	// check bottom y
        if (tYg + Hg >= data.origin_y + data.image_height)
          Yg = Yg + (data.origin_y + data.image_height) - (Yg + Hg) - 1;
	else
	  Yg = tYg;

	//cout << "Freetrans tXg: " << tXg << " tYg: " << tYg << endl;
	//cout << "Freetrans Xg: " << Xg << " Yg: " << Yg << endl;
	//cout << "End of freetrans" << endl;
      }

      // send normalized extent of displayed area
      else if (!strcmp(msg, JVEXTENTS)) {
	float nx = (float) Xg / (float) data.image_width;
	float ny = (float) Yg / (float) data.image_height;
	float nw = (float) Wg / (float) data.image_width;
	float nh = (float) Hg / (float) data.image_height;

	//cout << "MASTER: normalized extents nx: " << nx
	//     << " ny: " << ny << " nw: " << nw << " nh: " << nh << endl;
	server -> SendNormalizedExtents(nx, ny, nw, nh);
	extent.command = EXTENTS;
	//cout << "End of extents" << endl;
      }

      else if (!strcmp(msg, JVOVERVIEW)) {
	server -> SendOverview(bmpW, bmpH, bitmap);
	extent.command = OVERVIEW;
      }

      // quit
      else if(!strcmp(msg, JVQUIT)) {
	quit = true;
	extent.command = QUIT;
      }

      // resend
      else if(!strcmp(msg, JVRESEND)) {
        extent.command = RESEND;
      }

      //printf("After: xg %d, yg %d, wg %d, hg %d\n", Xg, Yg, Wg, Hg);

      //fprintf(logfile,"Message Returned: %s\n",msg);

      // calculate and send extent for each node
      for (int i = 0 ; i < size - 1 ; i++) {
	extent.x = ((long)(Wg * viewports[i].x_min)) + Xg;
	extent.y = ((long)(Hg * viewports[i].y_min)) + Yg;
	extent.w = (long)(Wg * (viewports[i].x_max - viewports[i].x_min));
	extent.h = (long)(Hg * (viewports[i].y_max - viewports[i].y_min));
	extent.zoom = zoom;
	extent.border = viewports[i].border;
	MPI_Send(&extent, 1, pinfo, i+1, 1, MPI_COMM_WORLD);
      }

      // synchronize
      MPI_Barrier(MPI_COMM_WORLD);

      // free memory used by message from UI
      //cout << "Master deleting message - begin" << endl;
      //delete [] msg;
      //cout << "Master deleting message - end" << endl;

    } /* end - event loop */

    // free memory used by viewport list
    free(viewports);

    // free memory used by overview
    free(bitmap);

  }

  /* End - Master node                                                       */
  /* *************************************************************************/

  /* *********************************************************************** */
  /* Begin - Slave node                                                      */

  else {

    // first time slave is run
    bool virgin = true;

    // MPI status variable
    MPI_Status status;

    // create GlobalCoordinatesToFileMapper instance
    GlobalCoordinatesToFileMapper gmapper;

    // initialize GlobalCoordinatesToFileMapper instance
    // TODO: change this to command line argument!!
    //gmapper.Initialize("juxtalayout.conf");
    gmapper.Initialize(argv[3]);
    //cout << "********SLAVE********** layout " << endl;

    // big image buffer
    unsigned char* pixelarray = NULL;

    // zoom adjusted image buffer
    unsigned char* buffer = NULL;

    // viewport information
    viewport view;

    // get viewport information
    MPI_Recv(&(view.x_min), 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Recv(&(view.y_min), 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Recv(&(view.x_max), 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Recv(&(view.y_max), 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Recv(view.ip, 1, ipinfo, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);

    /*
    cout << "Rank: " << rank << " x_min: " << view.x_min << endl;
    cout << "Rank: " << rank << " y_min: " << view.y_min << endl;
    cout << "Rank: " << rank << " x_max: " << view.x_max << endl;
    cout << "Rank: " << rank << " y_max: " << view.y_max << endl;
    */


    /* begin - event loop */
    while(!quit) {

      //fprintf(stderr, "Proc %d> Receiving\n", rank);

      // receive command from master
      MPI_Recv(&extent, 1, pinfo, 0, 1, MPI_COMM_WORLD, &status);
      //cout << "Rank: " << rank << " V1" << endl;
      if (virgin) {

	// initialize SAGE

	sailConfig scfg;
	scfg.init("JuxtaView.conf");
	scfg.setAppName("JuxtaView");
	if (argc > 6)
		scfg.nwID = atoi(argv[6]);
	scfg.rank = rank - 1;

	scfg.resX = (extent.w / extent.zoom);
	scfg.resY = (extent.h / extent.zoom);

	sageRect renderImageMap;
	renderImageMap.left = view.x_min;
	renderImageMap.right = view.x_max;
	renderImageMap.bottom = 1.0 - view.y_max;
	renderImageMap.top = 1.0 - view.y_min;

	scfg.imageMap = renderImageMap;
	scfg.pixFmt = PIXFMT_888;
	//scfg.rowOrd = TOP_TO_BOTTOM;
	scfg.rowOrd = TOP_TO_BOTTOM;

	if (scfg.rank == 0)
		scfg.master = true;
	else
		scfg.master = false;

	sageInf.init(scfg);

	if (argc > 7)
		frameRate = atoi(argv[7]);

	//cout << "Rank: " << rank << " V1.1" << endl;

	// create zoom adjusted image buffer
	if (buffer)
		delete [] buffer;

	buffer = (unsigned char*)sageInf.getBuffer();

	//cout << "Rank: " << rank << " V1.2" << endl;

	// no longer a virgin
	virgin = false;
      }

      //cout << "Rank: " << rank << " V1.3" << endl;

      // quit
      if (extent.command == QUIT) {

	// set quit flag
	quit = true;

	// synchronize
	MPI_Barrier(MPI_COMM_WORLD);

      }

      // resend
      else if (extent.command == RESEND) {
        if (!virgin) {
          // synchronize
          MPI_Barrier(MPI_COMM_WORLD);
			 /*
			 useconds_t interval = (useconds_t)floor(1.0/frameRate*1000000.0);
			 usleep(interval);
          // swap buffer
          sageInf.swapBuffer();
			 buffer = (unsigned char *)sageInf.getBuffer();
			 */
        }
      }

      // get and display data
      else {
	//cout << "Rank: " << rank << " V2" << endl;
	// print the current query for this node
	//cout << "QueryExtent at node " << rank << " " << extent.x << " "
	//     << extent.y << " " << extent.w << " " << extent.h << endl;

	// create big image buffer
	if (!pixelarray)
	  pixelarray = new unsigned char[extent.w * extent.h * 3];

	// set extent of data to fetch
	gmapper.SetQueryExtent(extent.x, extent.y, extent.w, extent.h);

	// make query - this actually makes the mapper calculate
	// all the files this program should load
	gmapper.Query();

	// print the state of the mapper
	//gmapper.PrintSelf(cout);

	// number of columns and rows in the query result
	long columnResult = 0;
	long rowResult = 0;

	// get the array of FileInfo and the number of rows and columns of
	// the 2D array of FileInfo
	FileInfo* files =
	  gmapper.GetQueryFileInfoArray(&columnResult, &rowResult);

	// print the number of columns and rows in the query result
	//cout << "FileInfoArrayDimensions: " << columnResult << " "
	//     << rowResult << endl;

	// NOTE: THIS DOES NOT WORK. IT'S A MESS!!!
	// ANOTHER NOTE: IT WORKS NOW, BUT VERY, VERY SLOWLY
	// ////////////////////////////////////////////////////////////////////
	// Begin - read from files

	long localx = 0;
	long localy = 0;
	long localw = 0;
	long localh = 0;
	long globalw = extent.w;
	long globalh = extent.h;

	//cout << "Extent.w: " << extent.w << " Extent.h: "
	//     << extent.h << endl;
	//cout << "Rank: " << rank << " V3" << endl;

	int array_index = 0;
	int array_max = columnResult * rowResult;

	//cout << "Rank: " << rank << " V4" << endl;

	memset(pixelarray, 0, globalw * globalh * 3);

	//cout << "Rank: " << rank << " V5" << endl;

	for(long c = 0; array_index < array_max && c < globalw; ){
	  for(long r = 0; array_index < array_max && r < globalh; ){

	    TIFF* tiffile = NULL;
	    tiffile = TIFFOpen(files[array_index].GetFileName(), "r");

	    // if the tif exists
	    if (tiffile != NULL) {

	      //get the scanlines needed
	      unsigned char *scanline =
		(unsigned char *) _TIFFmalloc(TIFFScanlineSize(tiffile));

	      files[array_index].GetExtent(&localx, &localy, &localw, &localh);
	      //cout << "Rank: " << rank
	      //	   << "file: " << files[array_index].GetFileName()
	      //	   << "localx: " << localx
	      //	   << "localy: " << localy
	      //	   << "localw: " << localw
	      //	   << "localh: " << localh
	      //	   << endl;

	      for (int l = 0,  k = localy ; k < localy + (localh) ; k++, l++)
		{
		  //cout << "Rank: " << rank << " row: " << k << endl;
		  TIFFReadScanline(tiffile,scanline,k);
		  //for(int j = 0; j < localw; j++){
		  memcpy(&(pixelarray[ (r + l) * globalw *3 + c * 3]),
			 &(scanline[localx * 3]),localw * 3);
		}

	      _TIFFfree(scanline);
	      TIFFClose(tiffile);

	    }

	    array_index++;
	    r += localh;
	  }
	  c += localw;
	}

	//cout << "Rank: " << rank << " V6" << endl;

	// End - read from files
	// ////////////////////////////////////////////////////////////////////

	// clear the zoom adjusted image buffer
	memset(buffer,
	       0,
	       (extent.w / extent.zoom) * (extent.h / extent.zoom) * 3);

	// adjust for zoom by skipping pixels
	// NOTE: Zooming is from the upper left, not the center, I know
	// that's bad, so some day I'll fix it.
	for (long r = 0, rz = 0 ; r < extent.h ; r += extent.zoom, rz++) {
	  for (long c = 0, cz = 0 ; c < extent.w ; c += extent.zoom, cz++) {
	    buffer[rz * 3 * (extent.w / extent.zoom) + (cz * 3)] =
	      pixelarray[r * 3 * extent.w + (c * 3)];
	    buffer[rz * 3 * (extent.w / extent.zoom) + (cz * 3) + 1] =
	      pixelarray[r * 3 * extent.w + (c * 3) + 1];
	    buffer[rz * 3 * (extent.w / extent.zoom) + (cz * 3) + 2] =
	      pixelarray[r * 3 * extent.w + (c * 3) + 2];
	  }
	}

	// delete big image buffer
	//cout << "slave deleting pixelarray - begin" << endl;
	delete [] pixelarray;
	//cout << "slave deleting pixelarray - end" << endl;
	pixelarray = NULL;

	// add borders
	add_border(buffer, extent);

	// synchronize
	MPI_Barrier(MPI_COMM_WORLD);

	// swap buffer
	sageInf.swapBuffer();
	buffer = (unsigned char *)sageInf.getBuffer();

      }

    } /* end - event loop */

    if (rank == 1)
      sageInf.shutdown();

    // delete zoom adjusted image buffer
    delete [] buffer;
    buffer = NULL;

  }

  /* End - Slave node                                                        */
  /* *************************************************************************/

  // finalize
  MPI_Finalize();

  // exit
  return EXIT_SUCCESS;

} /* end - main */
