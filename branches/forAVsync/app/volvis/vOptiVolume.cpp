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

//Class vOptiVolume: defines a 3d volume that is accessed through a server
//This class only manages the data(doesnt do the rendering)
//Shalini Venkataraman, Luc Renambot
//Sep 7 2003
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glUE.h"
#include "vVolume.h"
#include "VectorMath.h"
#include "global.h"

//mutex for the update histogram
//the transfer fn thread also acceses the histogram
QUANTAts_mutex_c mutex;

//constructor
vOptiVolume::vOptiVolume(const char* volName, const char* serverName):vVolume(0,0,0)
{
    opClient = new cDataManagerClient((char*)serverName);
    fprintf(stderr,"vOptiVolume::vOptiVolume %s\n",volName);
    opClient->Open();
    opClient->Init();
    int numData = opClient->Query();

	numVolumes= opClient->GetMatching((char*)volName,indexList);
	if (!numVolumes)	{
		fprintf(stderr,"ERROR: Cant find any volumes matching %s in Optistore\n",volName);
		exit(1);
	}
	cache.resize(numVolumes);
	opClient->GetDataDims(indexList[0],fileX, fileY, fileZ);
	fprintf(stderr,"vOptiVolume file Dims %d %d %d\n",fileX, fileY, fileZ);
	offsetX  = offsetY = offsetZ = 0;
	updateHist = false;
	isoValue = 128;
	curVolume = 0;
}

//destructor
vOptiVolume::~vOptiVolume() {
    opClient->Exit();
	free(indexList);
}

bool vOptiVolume::getUpdateHist() {
	bool ret;
	mutex.lock();
	ret = updateHist;
	mutex.unlock();
	return ret;
}

void vOptiVolume::setUpdateHist(bool flag) {
	mutex.lock();
	updateHist = flag;
	mutex.unlock();
}

int vOptiVolume::hist2D(unsigned char* normHist) {
	// Get 2D histogram of volume+gradient
	if (getUpdateHist()) { //if we have a new hist for the volume, copy it
		cache[curVolume].histMutex.lock();
		if (cache[curVolume].hist) {//if we have a hist send it
			memcpy(normHist,cache[curVolume].hist,H2D_DIM*sizeof(unsigned char));
			cache[curVolume].histMutex.unlock();
			setUpdateHist(false);
			return 1;
		}
		else
			cache[curVolume].histMutex.unlock();
	}
	return 0;
}

void vOptiVolume::load() {
	load(fileX, fileY, fileZ);
}

void vOptiVolume::setCurVolume(int curVolume)  {
	if (!cache[curVolume].voxelData) {//get from server
		opClient->Load(indexList[curVolume]);
		//opClient->Resample(dimX,dimY,dimZ);
		opClient->Crop(offsetX, offsetY, offsetZ ,offsetX+dimX-1,offsetY+dimY-1, offsetZ+dimZ-1);
		cache[curVolume].voxelData = new GLubyte[dimX*dimY*dimZ];
		memcpy(cache[curVolume].voxelData,opClient->Volume(),dimX*dimY*dimZ*sizeof(GLubyte));
		cache[curVolume].histMutex.lock();
		cache[curVolume].hist = new unsigned char[H2D_DIM ];
            //memcpy(cache[curVolume].hist,opClient->Histogram2D(),H2D_DIM*sizeof(unsigned char));
		cache[curVolume].histMutex.unlock();
	}
	setUpdateHist(true); //amy listeners will have to update their hist
}

void vOptiVolume::next() {
	curVolume++;
	if (curVolume >= numVolumes)
		curVolume = 0;
	setCurVolume(curVolume);
}

void vOptiVolume::prev() {
	curVolume--;
	if (curVolume < 0)
		curVolume = numVolumes-1;
	setCurVolume(curVolume);
}



void vOptiVolume::load(int loadx, int loady, int loadz) {
	setLoadDims(loadx, loady, loadz); //does the power of 2 conversion here if needed
	setCurVolume(0);
}

void vOptiVolume::roam(int roamX, int roamY, int roamZ) {
	//calculate new offset(cos the orig offset is unsigned..)
	int newOffsetX = offsetX + roamX;
	int newOffsetY = offsetY + roamY;
	int newOffsetZ = offsetZ + roamZ;
	//have to minus 1 since the bounds in Optistore are inclusive
	int cropX = dimX-1;//so if dimX = 64, crop from (0,0,0) ->(63,63,63)
	int cropY = dimY-1;
	int cropZ = dimZ-1;
	//now check if the newoFfset is withing range if not make it
	if (newOffsetX+cropX>= fileX) //eg (1+63) >=64 so, offset = 64-64 = 0
		newOffsetX = fileX-dimX;
	if (newOffsetY+cropY>= fileY)
		newOffsetY = fileY-dimY;
	if (newOffsetZ+cropZ >= fileZ)
		newOffsetZ = fileZ-dimZ;
	if (newOffsetX < 0)
		newOffsetX = 0;
	if (newOffsetY < 0)
		newOffsetY = 0;
	if (newOffsetZ < 0)
		newOffsetZ = 0;
	//copy to the offset
	offsetX = newOffsetX;
	offsetY = newOffsetY;
	offsetZ = newOffsetZ;

	//at this point delete our  cache
	for (int i=0;i<cache.size();i++) {
		delete [] cache[i].voxelData;
		cache[i].voxelData = 0;
		cache[i].histMutex.lock();
		delete [] cache[i].hist;
		cache[i].hist= 0;
		cache[i].histMutex.unlock();
	}
	opClient->Crop(offsetX, offsetY, offsetZ ,offsetX+dimX-1,offsetY+dimY-1, offsetZ+dimZ-1);
	cache[curVolume].voxelData = new GLubyte[dimX*dimY*dimZ];
	memcpy(cache[curVolume].voxelData,opClient->Volume(),dimX*dimY*dimZ*sizeof(GLubyte));
	cache[curVolume].histMutex.lock();
	cache[curVolume].hist = new unsigned char[H2D_DIM ];
        //memcpy(cache[curVolume].hist,opClient->Histogram2D(),H2D_DIM*sizeof(unsigned char));
	cache[curVolume].histMutex.unlock();
	setUpdateHist(true); //amy listeners will have to update their hist
}

NetVertexBuffer* vOptiVolume::isoSurface() {
	isoValue = probeValue;
	return (opClient->Isosurface(isoValue, 1, 0.40f));
}

NetPointBuffer* vOptiVolume::isoPoint() {
	isoValue = probeValue;
	return (opClient->IsoPoints(isoValue, 1, 8000));
}



