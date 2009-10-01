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

#include "caDataManagerClient.h"

int cDataManagerClient::RBUDP_PORT = 38049;

//a simple lightweight function to compare a string with a wildcard
//1 if there is a match, 0 otherwise
int wildcmp(char *wild, char *string) {
	char *newString, *newWild;
	while ((*string) && (*wild != '*')) {
		//if two char are normal characters they should match
		if ((*wild != *string) && (*wild != '?'))
			return 0;
		wild++;
		string++;
	}
	//found a "*"
	while (*string) {
		if (*wild == '*') {
			//last char - found a match
			if(!*++wild)
				return 1;
			newWild = wild;
			newString = string+1;
		}
		//if we found a '?' just increment both by 1
		else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		}
		//need to search with new string and wild card
		else {
			wild = newWild;
			string = newString++;
		}
	}
	//make sure that the remaining chars are just *
	while (*wild == '*')
		wild++;
	return !*wild;
}

////////////////////////////////////////////////////////////////////////////////
/////    cDataManagetClient class  /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


cDataManagerClient::cDataManagerClient(char *_hostname)
{
    hostname = strdup(_hostname);
    client = new QUANTAnet_tcpClient_c;

    char name[128];
    client->getSelfIP(name);
    fprintf(stderr, "Client> %s,%u\n", name, client->getSelfPort());

    dataptr = NULL;
    gradptr = NULL;
    vb = NULL;
    pb = NULL;

		// Initialize lookup table for gradient directions
	InitDirectionsTable();

    RBUDP_PORT = RBUDP_PORT + 1;
}

void cDataManagerClient::Open(int _port)
{
    port = _port;

        //client->setSockOptions(QUANTAnet_tcpClient_c::READ_BUFFER_SIZE, 2*256*256*256);

    if (client->connectToServer(hostname, port) < 0)
    {
        fprintf(stderr, "Cannot connect to server [%s] port %d\n", hostname, port);
        exit(-1);
    }
}

void cDataManagerClient::Init()
{
    fprintf(stderr, "Sending init\n");

        // Init phase
    dataSize = sizeof(int);
    command = Q_INIT;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    if (status != QUANTAnet_tcpClient_c::OK)
        client->showStatus(status, dataSize);

    int val[2];
    int _port;

    fprintf(stderr, "Waiting for init reply\n");

    dataSize = sizeof(int) * 2;
        //status =  client->read((char*)&id, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    status =  client->read((char*)val, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
	id = val[0];
    _port = val[1];

    fprintf(stderr, "Client> MyID is [%d]\n", id);

#if defined(USE_RBUDP)
    fprintf(stderr, "Client> My RBUDP port is [%d]\n", _port);

    sleep(1);
        // Get connection with the remote RBUDP server
        //blastee = new QUANTAnet_rbudpReceiver_c(RBUDP_PORT);
    blastee = new QUANTAnet_rbudpReceiver_c(_port);
    blastee->init(hostname);
#endif

}

void cDataManagerClient::Exit()
{
        // Exit phase
    dataSize = sizeof(int);
    command = Q_EXIT;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    fprintf(stderr, "Client> Exit\n");
}

int cDataManagerClient::Query()
{
    int i;
    char name[128];
    int dims[3];

        // Query phase
    dataSize = sizeof(int);
    command = Q_LIST;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    if (status != QUANTAnet_tcpClient_c::OK)
        client->showStatus(status, dataSize);

    dataSize = sizeof(int);
    status =  client->read((char*)&num_datasets, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    fprintf(stderr, "Client> There are [%d] datasets\n", num_datasets);
    datasets = (DataDesc*)malloc(num_datasets*sizeof(DataDesc));
    for (i=0;i<num_datasets;i++)
    {
        memset(name, 0, 128);
        dataSize = 128;
        status = client->read((char*)name,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
        datasets[i].name = strdup(name);

        dataSize = 3*sizeof(int);
        status = client->read((char*)dims,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
        datasets[i].x = dims[0];
        datasets[i].y = dims[1];
        datasets[i].z = dims[2];

        //fprintf(stderr, "Client> \tDataset [%d] : [%s]  -- [%d %d %d]\n", i, name, dims[0], dims[1], dims[2]);
    }

    return num_datasets;
}

void cDataManagerClient::Load(int ds)
{
    int val;

        // Load a dataset
    dataSize = sizeof(int);
    command = Q_LOAD;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
        // Dataset #
    dataSize = sizeof(int);
    val = ds;
    status = client->write((char*)&val, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    fprintf(stderr, "Client> Picking dataset %d [%s]\n", val, datasets[val].name);
}

void cDataManagerClient::Resample(int sx, int sy, int sz)
{
        // Resample a dataset
    dataSize = sizeof(int);
    command = Q_SAMPLE;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

        // Parameter: 3 int for dimensions
    dataSize = 3 * sizeof(int);
    sizes[0] = sx;
    sizes[1] = sy;
    sizes[2] = sz;
    status = client->write((char*)sizes, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    fprintf(stderr, "Client> Asking for resampling at [%3d %3d %3d]\n", sizes[0], sizes[1], sizes[2]);
}


void cDataManagerClient::Crop(int ox, int oy, int oz, int sx, int sy, int sz)
{
        // Crop a dataset
    dataSize = sizeof(int);
    command = Q_CROP;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

        // Parameter: 3 int for dimensions
    dataSize = 6 * sizeof(int);
    sizes[0] = ox;
    sizes[1] = oy;
    sizes[2] = oz;
    sizes[3] = sx;
    sizes[4] = sy;
    sizes[5] = sz;
    status = client->write((char*)sizes, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    fprintf(stderr, "Client> Asking for cropping from [%3d %3d %3d] to [%3d %3d %3d]\n",
            sizes[0], sizes[1], sizes[2], sizes[3], sizes[4], sizes[5]);


        // Allocate data storage for volume
    if (dataptr != NULL) free(dataptr);
    dataptr = (unsigned char*)malloc ((sizes[3]-sizes[0]+1) *
                                      (sizes[4]-sizes[1]+1) * (sizes[5]-sizes[2]+1) );

    if (gradptr != NULL) free(gradptr);
    gradptr = (unsigned short*)malloc ((sizes[3]-sizes[0]+1) *
                                       (sizes[4]-sizes[1]+1) * (sizes[5]-sizes[2]+1)
                                       * sizeof(unsigned short));

        //client->setSockOptions(QUANTAnet_tcpClient_c::READ_BUFFER_SIZE, 2*sizes[0]*sizes[1]*sizes[2]);
        //fflush(stdout);
}


unsigned char* cDataManagerClient::Volume()
{
	int amount;

		/////////////////////////////////////////////////////////////////////////
		// Volume
		/////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "Client> Asking for volume\n");
	dataSize = sizeof(int);
	command = Q_VOL;
	status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

	dataSize = sizeof(int);
	status = client->read((char*)&amount,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
	dataSize = amount;
#if defined(USE_RBUDP)
	blastee->receive((void*)dataptr, dataSize, RBUDP_FRAME);
#else
	status = client->read((char*)dataptr,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
#endif

	fprintf(stderr, "Client> Got volume of %d bytes\n\n", amount);

	if (status != QUANTAnet_tcpClient_c::OK)
		client->showStatus(status, dataSize);
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////

	return dataptr;
}

unsigned short* cDataManagerClient::Gradient()
{
	int amount;

		/////////////////////////////////////////////////////////////////////////
		// Gradient
		/////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "Client> Asking for gradient volume\n");
	dataSize = sizeof(int);
	command = Q_GRAD;
	status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

	dataSize = sizeof(int);
	status = client->read((char*)&amount,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);

	dataSize = amount;
#if defined(USE_RBUDP)
	blastee->receive((void*)gradptr, dataSize, RBUDP_FRAME);
#else
	status = client->read((char*)gradptr,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
#endif

	fprintf(stderr, "Client> Got gradient volume of %d bytes\n\n", amount);

	if (status != QUANTAnet_tcpClient_c::OK)
		client->showStatus(status, dataSize);
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////

	return gradptr;
}

int* cDataManagerClient::Histogram()
{
		/////////////////////////////////////////////////////////////////////////
		// Histogram command
		/////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "Client> Asking for volume histogram\n");
	dataSize = sizeof(int);
	command = Q_HISTO;
	status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

	dataSize = 259*sizeof(int);
    int *histo = (int*)malloc(dataSize);
    memset(histo, 0, dataSize);
	status = client->read((char*)histo,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);

	fprintf(stderr, "Client> Got volume histogram of %d bytes\n", dataSize);
	fprintf(stderr, "Client>     Histogram: min %d max %d mean %d\n\n",
            histo[0], histo[1], histo[2]);

	if (status != QUANTAnet_tcpClient_c::OK)
		client->showStatus(status, dataSize);
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////

	return histo;
}

unsigned char* cDataManagerClient::Histogram2D()
{
		/////////////////////////////////////////////////////////////////////////
		// Histogram2D command
		/////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "Client> Asking for volume gradient histogram2D\n");
	dataSize = sizeof(int);
	command = Q_HISTO2D;
	status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

	dataSize = 256*256*sizeof(unsigned char);
    unsigned char *histo = (unsigned char*)malloc(dataSize);
    memset(histo, 0, dataSize);
	status = client->read((char*)histo,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);

	fprintf(stderr, "Client> Got gradient histogram2D of %d bytes\n", dataSize);

	if (status != QUANTAnet_tcpClient_c::OK)
		client->showStatus(status, dataSize);
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////

	return histo;
}

NetPointBuffer*
cDataManagerClient::IsoPoints(int isoval, int inc, int maxp)
{
    int val[3];

        /////////////////////////////////////////////////////////////////////////
        // Isosurface
        /////////////////////////////////////////////////////////////////////////
    dataSize = sizeof(int);
    command = Q_POINT;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
        // Parameter: iso value
    dataSize = 3*sizeof(int);
    val[0] = isoval;
    val[1] = inc;
    val[2] = maxp;
    status = client->write((char*)&val[0], &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    fprintf(stderr, "Client> Asking for isopoints at [%d] increment [%d]\n", val[0], val[1]);

    if (status != QUANTAnet_tcpClient_c::OK)
        client->showStatus(status, dataSize);

    if (pb)
    {
        free(pb->vertex);
        free(pb);
    }

    pb = (NetPointBuffer*)malloc(sizeof(NetPointBuffer));
    dataSize = 1 * sizeof(int);
    status =  client->read((char*)sizes, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    pb->nbpts = sizes[0];
    pb->vertex = (float*)malloc( pb->nbpts * 3 * sizeof(float) );

    dataSize = pb->nbpts * 3 * sizeof(float);
    status = client->read((char*)pb->vertex,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    fprintf(stderr, "Client> Got isopoints of %d bytes\n\n", dataSize);

    if (status != QUANTAnet_tcpClient_c::OK)
        client->showStatus(status, dataSize);

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
    return pb;
}


NetVertexBuffer* cDataManagerClient::Isosurface(int isoval, int gaussp, float decim)
{
    int val[2], total;
	float fval;

        /////////////////////////////////////////////////////////////////////////
        // Isosurface
        /////////////////////////////////////////////////////////////////////////
    dataSize = sizeof(int);
    command = Q_ISO;
    status = client->write((char*)&command, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
        // Parameter: iso value
    dataSize = 2 * sizeof(int);
    val[0] = isoval;
    val[1] = gaussp;
    status = client->write((char*)&val, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    fprintf(stderr, "Client> Asking for isosurface at [%d]\n", val[0]);

    dataSize = sizeof(float);
    fval = decim;
    status = client->write((char*)&fval, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    if (status != QUANTAnet_tcpClient_c::OK)
        client->showStatus(status, dataSize);

    if (vb)
    {
        free(vb->vertex);
        free(vb->normal);
        free(vb->index);
        free(vb->length);
        free(vb);
    }

    vb = (NetVertexBuffer*)malloc(sizeof(NetVertexBuffer));
    dataSize = 3 * sizeof(int);
    status =  client->read((char*)sizes, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);

    vb->nbpts     = sizes[0];
    vb->nbindices = sizes[1];
    vb->lengths   = sizes[2];

    vb->vertex = (float*)malloc( vb->nbpts * 3 * sizeof(float) );
    vb->normal = (float*)malloc( vb->nbpts * 3 * sizeof(float) );
    vb->index  = (unsigned int*)malloc( vb->nbindices * sizeof(unsigned int) );
    vb->length = (unsigned short*)malloc( vb->lengths * sizeof(unsigned short) );

    dataSize = vb->nbpts * 3 * sizeof(float);
    status = client->read((char*)vb->vertex,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    total = dataSize;
    dataSize = vb->nbpts * 3 * sizeof(float);
    status = client->read((char*)vb->normal,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    total += dataSize;
    dataSize = vb->nbindices * sizeof(unsigned int);
    status = client->read((char*)vb->index,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    total += dataSize;
    dataSize = vb->lengths * sizeof(unsigned short);
    status = client->read((char*)vb->length,&dataSize, QUANTAnet_tcpClient_c::BLOCKING);
    total += dataSize;

    fprintf(stderr, "Client> Got isosurface of %d bytes\n\n", total);

    if (status != QUANTAnet_tcpClient_c::OK)
        client->showStatus(status, dataSize);

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
    return vb;
}


char *cDataManagerClient::Name(int _idx)
{
    if (_idx >=0 && _idx < num_datasets)
        return datasets[_idx].name;
    else
        return NULL;
}

cDataManagerClient::~cDataManagerClient()
{
#if defined(USE_RBUDP)
    blastee->close();
#endif
}

//Given the symbolic name of a volume, returns its index
//if the volume is not found return -1;
int cDataManagerClient::Lookup(const char* name)
{
  	for (int i=0;i<num_datasets;i++)
    {
  		if (strcmp(name,datasets[i].name) == 0)
  			return i;
  	}
  	return -1;
}

//Given the symbolic name of a volume as a wild card, returns no of
//that correspond to this name,
//if not found, return 0;
//caller has to free the memory
int cDataManagerClient::GetMatching(char* wildcard, int *&indexList)
{
	int curIndex = 0;
	indexList = (int*)malloc(num_datasets*sizeof(int));
	for (int i=0;i<num_datasets;i++)
	{
		if (wildcmp(wildcard,datasets[i].name))
			indexList[curIndex++] = i;
  	}
	indexList = (int*)realloc(indexList,curIndex*sizeof(int));
  	return curIndex;
}

    //get the size of the datasets
void cDataManagerClient::GetDataDims(unsigned int idx, unsigned int& x, unsigned int& y, unsigned int& z)
{
  	x = datasets[idx].x;
  	y = datasets[idx].y;
 	z = datasets[idx].z;
}
