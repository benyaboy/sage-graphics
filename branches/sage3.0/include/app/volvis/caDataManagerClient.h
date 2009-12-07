/******************************************************************************
 * OptiStore - A Data Processing Server
 * Copyright (C) 2003 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * Authors: Luc Renambot luc@evl.uic.edu
 *          Shalini Venkataraman shalini@evl.uic.edu
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
 * Direct questions, comments etc about OptiStore to luc@evl.uic.edu
 *****************************************************************************/

#include "private.h"
#include "directions.h"

typedef struct _NetVertexBuffer
{
    float          *vertex;
    float          *normal;
    unsigned int   *index;
    unsigned short *length;
    int  lengths, nbpts, nbindices;
    //sort of copy constructor
    void copy(struct _NetVertexBuffer* src) {
	nbpts = src->nbpts;
	lengths = src->lengths;
	nbindices = src->nbindices;

	vertex = (float*)malloc( nbpts * 3 * sizeof(float) );
	memcpy(vertex,src->vertex, nbpts * 3 * sizeof(float));

	normal = (float*)malloc( nbpts * 3 * sizeof(float) );
	memcpy(normal,src->normal,nbpts * 3 * sizeof(float));

	index  = (unsigned int*)malloc( nbindices * sizeof(unsigned int) );
	memcpy(index, src->index,nbindices * sizeof(unsigned int));

	length = (unsigned short*)malloc( lengths * sizeof(unsigned short) );
	memcpy(length, src->length,lengths * sizeof(unsigned short));
    }
    void freeArrays() {
	    free(vertex);
	    free(normal);
	    free(index);
	    free(length);
    }

} NetVertexBuffer;

typedef struct _NetPointBuffer
{
    float *vertex;
    int    nbpts;
} NetPointBuffer;


    //attribute for each data
typedef struct _DataDesc
{
    char* name;
    int x,y,z;
} DataDesc;

////////////////////////////////////////////////////////////

class cDataManagerClient
{
protected:
    QUANTAnet_tcpClient_c *client;
#if defined(USE_RBUDP)
    QUANTAnet_rbudpReceiver_c *blastee;
#endif

    int port;
    char *hostname;
    int id;
    int num_datasets;
    DataDesc *datasets;


private:
    int status, dataSize;
    unsigned int command;
    int sizes[6];
    NetVertexBuffer *vb;
    NetPointBuffer  *pb;
    unsigned char  *dataptr;
    unsigned short *gradptr;


public:

    static int RBUDP_PORT;

    cDataManagerClient(char *_hostname = "localhost");
    ~cDataManagerClient();


    void Open(int _port = 6544);
    void Init();
    void Exit();
    int  Query();

    void Load(int ds);
    void Resample(int sx, int sy, int sz);
    void Crop(int ox, int oy, int oz, int sx, int sy, int sz);

    unsigned char*  Volume();
    unsigned short* Gradient();

    int* Histogram();
    unsigned char* Histogram2D();

    NetVertexBuffer* Isosurface(int isoval, int gaussp, float decim);
    NetPointBuffer* IsoPoints(int isoval, int inc, int maxp);

        // Get the name of a dataset
    char *Name(int _idx);

        //Given the symbolic name of a volume, returns its index
        //if the volume is not found return -1;
    int Lookup(const char* name) ;


	//Given the symbolic name of a volume as a wild card, returns list of indices that match
	//if not found, return 0;
	//caller has to free the memory
	int GetMatching(char* wildcard, int *&indexList);
        //get the data dimesions for the data indexed by idx
    void GetDataDims(unsigned int idx, unsigned int& x, unsigned int& y, unsigned int& z);
};
