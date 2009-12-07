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

#ifndef _VVOLUME_H_
#define _VVOLUME_H_
#include "mmap.h"
#include "private.h"
#include "caDataManagerClient.h"
#include <vector>

#define H2D_DIM 256*256
//for one timestep volume
class vFrameData {
public:
	GLubyte* voxelData;
	unsigned char* gradData;
	unsigned char* hist;
	QUANTAts_mutex_c histMutex;

	vFrameData() {
		voxelData = gradData = hist = 0;
	}
};

//This class defines a volume
class vVolume {
public:
	//row, column and depth of volume thats loaded into gfx
	unsigned int dimX, dimY, dimZ;
	unsigned int maxDim; //maximum dimension
	//dimensions of the whole volume - on file
	unsigned int fileX, fileY, fileZ;
	//offset into the whole volume to read the subvolume
	unsigned int offsetX, offsetY, offsetZ;
	//spacing into the whole volume to read the subvolume
	float spacingX, spacingY, spacingZ;
	//current probe point
	unsigned int probeX, probeY, probeZ;
	vector<vFrameData> cache; //pointer to the time-series volume
	int curVolume, numVolumes; //no of volumes and the current volume index
	int probeValue, isoValue; //current value of the probe
	unsigned int texName; //name of 3D texture associated with this volume
//Member functions
	vVolume(int x, int y, int z); //size of the whole volume
	virtual ~vVolume();
	//here is where the volume is actually loaded
	//needs to be overloaded
	virtual void load(int loadx, int loady, int loadz) = 0;
	virtual void load() = 0;
	//roaming the volume - overload this
	virtual void roam(int roamx, int roamy, int roamz) = 0;
	virtual int hist2D(unsigned char *normHist);
	virtual int hist1D(unsigned char *normHist);
	virtual void saveVolume(const char* filename);
	virtual void saveGradient(const char* filename = "grad");
	virtual void probe(int probeX, int probeY, int probeZ);
	virtual void next() = 0;
	virtual void prev() = 0;
	//sets the vol dims/does power of 2 checking etc
	void setLoadDims(int x, int y, int z) ;
	virtual NetVertexBuffer* isoSurface() = 0;
	virtual NetPointBuffer* isoPoint() = 0;

	inline GLubyte getValue(int d, int r, int c) {
		return cache[curVolume].voxelData[d*dimY*dimX+r*dimX+c];
	}

	inline void setValue(int d, int r, int c, GLubyte value) {
		cache[curVolume].voxelData[d*dimY*dimX+r*dimX+c] = value;
	}

	inline GLubyte* getVoxelData() {
		return cache[curVolume].voxelData;
	}

	inline void getNormProbe(float& x, float& y, float& z) {
		x = (float)probeX/(float)(dimX-1);
		y = (float)probeY/(float)(dimY-1);
		z = (float)probeZ/(float)(dimZ-1);
	}

	inline void getNormOffset(float& x, float& y, float& z) {
		x = (float)offsetX/(float)(fileX-1);
		y = (float)offsetY/(float)(fileY-1);
		z = (float)offsetZ/(float)(fileZ-1);
	}

	inline void getNormDim(float& x, float& y, float& z) {
		x = (float)dimX/(float)(fileX);
		y = (float)dimY/(float)(fileY);
		z = (float)dimZ/(float)(fileZ);
	}

	inline void incIsoValue(int inc) {
		isoValue += inc;
		if (isoValue > 255)
			isoValue = 255;
		if (isoValue < 0)
			isoValue = 0;
		fprintf(stderr,"Isovalue now is %d\n",isoValue);
	}
	inline int getIsoValue() {
		return isoValue;
	}
	inline int setIsoValue(int i) {
		isoValue = i;
	}
};
#if 0
//class that handle volume access from a file
class vFileVolume: public vVolume {
public:
	//if the volume is smaller than the volume in file, we need to pad it equally
	unsigned int padX, padY, padZ;
	char filename[256]; //filename associated with this 3d texture
	char* 	gradientFile; //gradient file associated with this 3d texture
	Mmap	mmap;
	unsigned char* fileBuffer;

//public member functions
	vFileVolume(const char* filename, int c, int r, int d);
	virtual ~vFileVolume();
	void load(int x, int y, int z);
	void load() {load(fileX,fileY,fileZ);};
	//void roam(int roamx, int roamy, int roamz);
private:
	void loadVolumeFromFile(const char* filename);
	void loadGradientFromFile(const char* filename); //load gradient info from file
	void padVolume();//if volsize > filesize, pad the volume with black
	void readASlice(GLubyte* plane, unsigned char* &curFilePtr) ; //read one plane from file
	//if the volume is bigger than the file size, this will pad the volume
	void calcGradient();
};
#endif
//class to handle volume access from a data server
class vOptiVolume: public vVolume {
public:
    cDataManagerClient* opClient; //pointer to the op client
	int* indexList;
	bool updateHist;
//public member functions
	vOptiVolume(const char*, const char*);
	virtual ~vOptiVolume();
	void load(int x, int y, int z);
	void load();
	void roam(int roamx, int roamy, int roamz);
	int hist2D(unsigned char *normHist);
	//get isosurface from opClient
	NetVertexBuffer* isoSurface();
	NetPointBuffer* isoPoint();
	//void hist1D(unsigned char *normHist);
	void setUpdateHist(bool flag) ;
	bool getUpdateHist() ;
	void next();
	void prev();
	void setCurVolume(int curVolume) ;
};
#endif

