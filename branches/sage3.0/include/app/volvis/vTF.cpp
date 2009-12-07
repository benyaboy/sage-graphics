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

#ifdef V_NETTF
#include "vTF.h"
#include "vUI.h"
#include "global.h"
#include <string.h>

//send the UI events to the network.
//Checks if there is a new tcp client that has connected in
//Add that to the image list
void* netThreadCB(void* data) {
	vTF* tf = (vTF*)data;
	while(1) {
		tf->process();
		QUANTAusleep(100);
	}
}

vTF::vTF()
{
	tcpServer = 0;
	tcpClient = 0;
}

vTF::~vTF() {
	tcpClient->close();
	tcpServer->close();
}

void vTF::init()
{
	//quanta stuff
//	QUANTAinit();
	//create a TCP server that gets the image data
	tcpServer= new QUANTAnet_tcpServer_c;
	if (tcpServer->init(7352) == 0)    {
		exit(1);
	}
	QUANTAts_thread_c *networkListenerThread = new QUANTAts_thread_c;
	networkListenerThread->create(netThreadCB, (void*) this);
}

void vTF::checkForConnections() {
	QUANTAnet_tcpClient_c* newClient = tcpServer->checkForNewConnections(0);
	if (newClient) { //new client is connecting
		if (tcpClient) { //if an old client already exists
			fprintf(stderr,"Deleting Old client\n");
			tcpClient->close(); //close the old client
			delete tcpClient; //deallocate it
		}
		tcpClient = newClient;
		tcpClient->getRemoteIP(clientIP);
		fprintf(stderr,"New Client %s connected at port %u \n", clientIP, tcpClient->getSelfPort());
	}
}
#define HISTSIZE 256*256
void vTF::sendHistogram() {
	unsigned char histogram[HISTSIZE];
	int haveHist = global.volume->hist2D(histogram);
	int size;

	//fprintf(stderr,"Going to send update histogram %d\n",haveHist);
	//writing whether we need to update the histogram
	size = sizeof(int);
	tcpClient->setSockOptions(QUANTAnet_tcpClient_c::WRITE_BUFFER_SIZE, size);
	int status = tcpClient->write((char*)&haveHist, &size, QUANTAnet_tcpClient_c::BLOCKING);
	//fprintf(stderr,"Sinished send update histogram %d\n",haveHist);
	if (status != QUANTAnet_tcpClient_c::OK)
		tcpClient->showStatus(status, size);
#ifdef SHOW_STATS
	tcpClient->showStats("After sending update histogram"," ");
#endif
	if (haveHist) { //if we have to update the hist, send the histogram
		size = HISTSIZE*sizeof(unsigned char);
		tcpClient->setSockOptions(QUANTAnet_tcpClient_c::WRITE_BUFFER_SIZE, size);
		int status = tcpClient->write((char*)histogram, &size, QUANTAnet_tcpClient_c::BLOCKING);
		if (status != QUANTAnet_tcpClient_c::OK)
			tcpClient->showStatus(status, size);
		else {
		//	fprintf(stderr,"vTF::sendHistogram: have Hist and finished sending\n");
		}
#ifdef SHOW_STATS
		tcpClient->showStats("After sending histogram"," ");
#endif
	}

}

void vTF::process() {
	checkForConnections();
	//now read from client
	if (tcpClient) {
		//fprintf(stderr,"Going to send hist and read from client\n");
		//send histogram
		sendHistogram();
		int haveLUT;
		//read if there is LUT
		int dataSize = sizeof(int);
		tcpClient->setSockOptions(QUANTAnet_tcpClient_c::READ_BUFFER_SIZE, dataSize);
		int status = tcpClient->read((char*)&haveLUT, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
		if (status == QUANTAnet_tcpClient_c::OK)  {
			//fprintf(stderr,"Reading Have LUT %d \n",haveLUT);
		}
		else if (status != QUANTAnet_tcpClient_c::NON_BLOCKING_HAS_NO_DATA) {
			fprintf(stderr,"Error while Reading Have LUT \n");
			tcpClient->showStatus(status, dataSize);
			//conection close just close the client
			tcpClient->close();
			delete tcpClient;
			tcpClient = 0;
		}
#ifdef SHOW_STATS
		tcpClient->showStats("After receive"," ");
#endif
		if (haveLUT) {
		//fprintf(stderr,"New LUT:Trying to read\n");
		//set buffer size to the network data size
		int dataSize = 256*4*sizeof(char)+sizeof(int);
		char* netBuffer= new char[dataSize];
		tcpClient->setSockOptions(QUANTAnet_tcpClient_c::READ_BUFFER_SIZE, dataSize);
		int status = tcpClient->read(netBuffer, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
		//fprintf(stderr,"Reading LUT %d bytes\n",dataSize);
		if (status == QUANTAnet_tcpClient_c::OK)  {
			setLUT(dataSize,(unsigned char*)netBuffer);
			//fprintf(stderr,"Got LUT %d bytes\n",dataSize);
		}
		else if (status != QUANTAnet_tcpClient_c::NON_BLOCKING_HAS_NO_DATA) {
			fprintf(stderr,"Connection close with client %s\n", clientIP);
			tcpClient->showStatus(status, dataSize);
			//conection close just close the client
			tcpClient->close();
			delete tcpClient;
			tcpClient = 0;
		}
#ifdef SHOW_STATS
		tcpClient->showStats("After receive"," ");
#endif
		delete [] netBuffer;
		}
		else {
		//	fprintf(stderr,"No LUT received\n");
		}

	}

}

#endif
