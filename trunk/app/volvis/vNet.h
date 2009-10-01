/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
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
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/

//Class to handle the network for tiled display
//vNet.h
#if defined(V_DISTRIBUTED)
#ifndef _vNet_h_
#define _vNet_h_

#include <iostream.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "mpi.h"
#include "private.h"

//size of each command from the master
#define COMMAND_SIZE 2048

using namespace std;

#define int32 cint32
#define uint32 cuint32
#undef int32
#undef uint32

char* getServerIP(int rank, const char* serverFile) ;
//Class that handles the image control operations (including MPI broadcast)
class vNet {
public:
	//default constructor - provide the rank, total # processors and if master processor should display the image
	vNet();
	void init(int &argc, char** &argv);
	~vNet();
	//kills the tiles
	void deactivateTiles();
	//calls the dbclients process, used by network thread
	void process();
	//message parsing
	void processCmd(char* data);
	//read the config file
	bool readTileConfig(const char* configFile);

	//set the rotation matrix for all the volumes
	void setRotn(float rotn[16]) ;
	//set cutplane rotation
	void setCutRotation(float angle, float axis[3]);
	//set cutplane zoom
	void setCutZoom(float dzoom);
	//set the low res rendreing for all the volumes
	void setLowRes(int lowRes) ;
	//set zoom
	void setZoom(float factor);
	//set Translation
	void setTranslation(float translation[3]);
	//scale the sampling
	void scaleSampleRate(float factor);
	//set the transfer function
	void setLUT (int, unsigned char *);
	//toggle printing frame rate
	void togglePrintFPS();
	//toggle cut plane
	void toggleCutPlane();
	//toggle display of bound box
	void toggleBoundBox();
	//save gradient volume
	void saveGradientVolume() ;
	//roam the volume
	void setRoam() ;
	//roam the volume
	void setProbe() ;
	//roam the volume
	void roamVolume(int x, int y, int z);
	//probe the volume
	void probeVolume(int x, int y, int z);
	//get the isosurface for this isosurface
	void getIsosurface(int isoValue, float tx, float ty, float scale) ;
	//get isopoint
	void getIsopoint (int, float, float, float);
	//animate volume
	void animateVolume();
	//animate volume (rotate it on X)
	void animateVolumeRotX();
	//update the animation if any
	void updateAnim();
	//select index
	void select(int curIndex);

	//set the pointer position
	void select(float pointerPos[3]) ;

	//default dummy command - to synch the master and slave displays
	void fillDummyCommand();
	//indicate ready to exit
	void fillExitCommand();
	//returns true if Im the master, false otherwise
	inline int isActiveTile() {
		if (rank == master)
			return 1;
		else
			return 0;
	}

	inline void sync() {
		MPI_Barrier(MPI_COMM_WORLD);
	}

	inline int getRank() {
		return rank;
	}
//protected:
	char buffer[COMMAND_SIZE]; //Command buffer that is broadcast every frame
	int readyToDie;
	QUANTAnet_datapack_c dataPack;  //to be replaced by MPI datapack ?
	//network params
	int cmdFlushed; //if the current command has been flushed in order to handle the next one
	int rank; //my rank id - to read the right configuration
	int master;//the master id
	int noProcs;
	void packCommand(const char* cmdString, int flushed =0); //to pack any command
};

#endif
#endif
