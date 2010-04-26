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

//Class vVolume: Defines the 3D volume and the associated operations
//the inherited class should override the load/roam function
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

//constructor
vVolume::vVolume(int x, int y, int z) {
	fileY = y;
	fileX = x;
	fileZ = z;
	dimY = dimX = dimZ = maxDim = 128;
	offsetX  = offsetY = offsetZ = 0;
	spacingX  = spacingY = spacingZ = 1.0f;
	probeX = probeY = probeZ = 0;
	curVolume = numVolumes = 0;
	probeValue = 0;
	isoValue = 128;
}

//destructor
vVolume::~vVolume() {
	for (int i=0;i<cache.size();i++) {
		delete [] cache[i].voxelData;
		//delete [] cache[i].gradData;
		delete [] cache[i].hist;
	}
}

void vVolume::setLoadDims(int x, int y, int z) {
	//now get the power of 2
	dimY = makepow2_max(y);
	dimX = makepow2_max(x);
	dimZ = makepow2_max(z);
	maxDim = dimY;
	maxDim = MAX(dimY,dimX);
	maxDim = MAX(maxDim,dimZ);
	probeX = dimX/2;
	probeY = dimY/2;
	probeZ = dimZ/2;
}

void vVolume::probe(int dx, int dy, int dz) {
	//calculate new probe(cos the orig offset is unsigned..)
	int newProbeX = probeX + dx;
	int newProbeY = probeY + dy;
	int newProbeZ = probeZ + dz;
	if (newProbeX== dimX)
		newProbeX = dimX-1;
	if (newProbeY== dimY)
		newProbeY = dimY-1;
	if (newProbeZ== dimZ)
		newProbeZ = dimZ-1;

	if (newProbeX < 0)
		newProbeX = 0;
	if (newProbeY < 0)
		newProbeY = 0;
	if (newProbeZ < 0)
		newProbeZ = 0;
	//copy to the offset
	probeX = newProbeX;
	probeY = newProbeY;
	probeZ = newProbeZ;
	probeValue = getValue(probeZ, probeY, probeX);
}


int vVolume::hist2D(unsigned char *normHist)
{
	float *hist = new float[H2D_DIM];
	int i;
	for(i=0; i<H2D_DIM; ++i){
		hist[i] = 0;
	}

	for(i=0; i<dimZ; ++i){
		for(int j=0; j<dimY; ++j){
			for(int k=0; k<dimX; ++k){
				int vox = i*dimX*dimY + j*dimX + k;
				hist[cache[curVolume].voxelData[vox] + cache[curVolume].gradData[vox]*256] += 1;
			}
		}
	}

	float max = 0;
	for(i = 0; i< H2D_DIM; ++i){
		hist[i] = (float)log(hist[i]);
		max = MAX(hist[i], max);
	}
	for(i=0; i< H2D_DIM; ++i){
		normHist[i] = (unsigned char)(hist[i]/(float)max*255);
	}
	delete[] hist;
	return 1;
}

int vVolume::hist1D(unsigned char *normHist)
{
	float *hist = new float[H2D_DIM];
	int i;
	for(i=0; i<H2D_DIM; ++i){
		hist[i] = 0;
	}

	for(i=0; i<dimZ; ++i){
		for(int j=0; j<dimY; ++j){
			for(int k=0; k<dimX; ++k){
				int vox = i*dimX*dimY + j*dimX + k;
				for (int row = 0;row<256;row++)
					hist[cache[curVolume].voxelData[vox]+row*256] += 1;
			}
		}
	}

	float max = 0;
	for(i = 0; i< H2D_DIM; ++i){
	//	hist[i] = (float)log(hist[i]);
		max = MAX(hist[i], max);
	}
	for(i=0; i< H2D_DIM; ++i){
		normHist[i] = (unsigned char)(hist[i]/(float)max*255);
	}
	delete[] hist;
	return 1;
}

void vVolume::saveVolume(const char* prefix) {
	FILE* fp;
	char filename[256];
	sprintf(filename,"%s%dx%dx%d.raw",prefix,dimX,dimY,dimZ);
	if( (fp = fopen(filename, "wb" )) != NULL ) {
		int numwritten = fwrite(cache[curVolume].voxelData, sizeof(GLubyte), dimY*dimX*dimZ, fp );
		printf( "vVolume::saveVolume: Wrote %d bytes to file %s\n", numwritten, filename);
		fclose( fp );
	}
	else
		printf( "vVolume::saveVolume: Problem opening the file\n" );
}

void vVolume::saveGradient(const char* prefix) {
        FILE* fp;
        char filename[256];
        sprintf(filename,"%s%dx%dx%d.raw",prefix,dimX,dimY,dimZ);
        if( (fp = fopen(filename, "wb" )) != NULL ) {
                int numwritten = fwrite(cache[curVolume].gradData, sizeof(unsigned char), dimY*dimX*dimZ, fp );
                printf( "saveGradient: Wrote %d bytes to file %s\n", numwritten, filename);
                fclose( fp );
        }
        else
                printf( "saveGradient: Problem opening the file\n" );
}


