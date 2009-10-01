/******************************************************************************
 * Program: JuxtaView for SAGE
 * Module:  JuxtaSCUICommon.cpp - Part of JuxtaView's UI
 * Authors: Arun Rao, arao@evl.uic.edu,
           Ratko Jagodic, rjagodic@evl.uic.edu,
           Nicholas Schwarz, schwarz@evl.uic.edu,
           Luc Renambot, luc@evl.uic.edu,
           et al.
  Date:    30 September 2004
  Modified: 9 September 2005
 *
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
 * Direct questions, comments etc to schwarz@evl.uic.edu or
 * http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "JuxtaUIServer.h"
#include <string.h>

JuxtaUIServer::JuxtaUIServer()
{
	server = new QUANTAnet_tcpServer_c();
	client = NULL;
	recmsg = NULL;
}

JuxtaUIServer::~JuxtaUIServer()
{
	if(client)
		client->close();

	if(server)
		server->close();

	if(client)
		delete client;

	if(server)
		delete server;

	if(recmsg)
		delete [] recmsg;
}

bool JuxtaUIServer::Init(char* configfilename)
{
	FILE* fin = fopen(configfilename,"r");
	if(!fin) {
	  printf("return\n");
		return false;
	}


	int port;
	fscanf(fin,"%d\n",&port);

	fprintf(stderr, "!!!****************SERVER PORT 1: %d\n ", port);
	if( server->init(port) <  0)
	{
	  printf("!!!******************SERVER PORT 2: %d\n ", port);
		server->close();
		delete server;
		server = NULL;
		fclose(fin);
		return false;
	}

    fprintf(stderr, "!!!****************SERVER PORT 3: %d\n ", port);
	fclose(fin);
	return true;
}

char* JuxtaUIServer::WaitForMessage()
{
	if(recmsg == NULL)
		recmsg = new char[TRANSMISSION_SIZE];
	memset(recmsg,0,TRANSMISSION_SIZE);
	static int size = TRANSMISSION_SIZE;

	if(client == NULL && server != NULL)
	{
		fprintf(stderr,"Waiting For Client To Connect.\n");
		client = server->waitForNewConnection();
		fprintf(stderr,"Ready For Messages.\n");
	}

	if(client)
	{

	  int re = client->read(recmsg,&size,QUANTAnet_tcpClient_c::NON_BLOCKING);

	  // no data is read
	  if (re == QUANTAnet_tcpClient_c::NON_BLOCKING_HAS_NO_DATA) {
	    strcpy(recmsg, JVRESEND);
	    return recmsg;
	  }

	  // data is read
	  else {
	    return recmsg;
	  }

	  /*
	  client->read(recmsg,&size,QUANTAnet_tcpClient_c::NON_BLOCKING);
	  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!-%s\n", recmsg);
	  return recmsg;
	  */

	}

	return NULL;
}


void JuxtaUIServer::SendNormalizedExtents(float x, float y, float w, float h)
{
	int size;
	if(client)
	{
		fprintf(stderr,"Sending normalized coords and dims:\n%f, %f\n%f x %f\n",x,y,w,h);
		size = sizeof(float) * 4;
		float array[4];
		array[0] = x;
		array[1] = y;
		array[2] = w;
		array[3] = h;
		client->write((const char*)array,&size,QUANTAnet_tcpClient_c::BLOCKING);
	}

}

void JuxtaUIServer::SendOverview(int w, int h,unsigned char* bmp)
{
	int size;
	if(client)
	{
		fprintf(stderr,"Sending overview of size: %d x %d\n",w,h);
		size = sizeof(int) * 2;
		int array[2];
		array[0] = w;
		array[1] = h;
		//send pixel dimensions first
		client->write((const char*)array,&size,QUANTAnet_tcpClient_c::BLOCKING);
		//send pixels next
		size = w * h * 3;
		client->write((const char*)bmp,&size,QUANTAnet_tcpClient_c::BLOCKING);

	}

}

void JuxtaUIServer::GetNormalizedFreeTranslation(float* x, float* y)
{
	float array[2] = { 0.0 , 0.0 };
	int size = sizeof(float) * 2;

	if( client )
	{
		fprintf(stderr,"Reading normalized free translation from client!\n");
		client->read((char*)array,&size,QUANTAnet_tcpClient_c::BLOCKING);


	}

	*x = array[0];
	*y = array[1];
}

