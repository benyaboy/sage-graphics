/*=============================================================================

  Program: JuxtaView for SAGE
  Module:  JuxtaUIServer.h - Part of JuxtaView's UI
  Authors: Arun Rao, arao@evl.uic.edu,
           Ratko Jagodic, rjagodic@evl.uic.edu,
           Nicholas Schwarz, schwarz@evl.uic.edu,
           et al.
  Date:    30 September 2004
  Modified: 28 October 2004

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

#ifndef JUXTAUI_SERVER_H
#define JUXTAUI_SERVER_H

#include <QUANTA/QUANTAinit.hxx>
#include <QUANTA/QUANTAnet_tcp_c.hxx>
#include "JuxtaSCUICommon.h"


class JuxtaUIServer
{
private:
	QUANTAnet_tcpServer_c* server;
	QUANTAnet_tcpClient_c* client;
	char* recmsg;
public:
	JuxtaUIServer();
	~JuxtaUIServer();
	bool Init(char*); //pass the config file name
	char* WaitForMessage();
	void SendNormalizedExtents( float x, float y, float w, float h);
	void SendOverview(int w, int h, unsigned char* bmp);
	void GetNormalizedFreeTranslation( float* x, float* y);
};

#endif

