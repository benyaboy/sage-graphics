/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: fsManager.h
 * Author : Byungil Jeong
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
 * Direct questions, comments etc about SAGE to sage_users@listserv.uic.edu or
 * http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/


#ifndef _FSManager_H
#define _FSManager_H

#include "sage.h"
#include "sageConfig.h"

class fsCore;
class displayInstance;
class fsServer;
class sageVirtualDesktop;

class rcvInfo {
public:
   int syncPort;
   int streamPort;
   int bufSize;
   bool fullScreen;

   bool audioOn;
   int audioPort;
   int audioSyncPort;
   int agSyncPort;
   rcvInfo() : syncPort(11000), streamPort(21000), bufSize(64), fullScreen(true),
         audioOn(false), audioSyncPort(13000), audioPort(23000), agSyncPort(15000) {}
};

class sageNwConfig;
class displayConnection;

/**
 * \brief The container of the Free Space Manager components - fsCore, displayInstance, appData, and fsServer.
 *
 * The FSManager receives from UI clients various user commands such as application execution, window move, resizing or z-order change and then executes the commands by sending control messages to SAIL nodes or SAGE receivers.
 *
 * This class is instanced by main() in the fsMain.cpp,
 * then member function mainLoop() is called after reading fsManager.conf
 */
class fsManager {
private:
   fsCore *core;
   fsServer *server;

	// BEGIN HYEJUNG
	//int m_execIndex;
	//int m_execIDList[MAX_INST_NUM];
	// END

   rcvInfo rInfo;
   sageNwConfig *nwInfo;
   std::vector<appInExec *> execList;
   std::vector<displayInstance *> dispList;
   std::vector<int> uiList;
   std::vector<int> appUiList;
	// HYEJUNG
	std::vector<int> audioList;

   std::vector<sageRect *> drawObjectList;
   std::vector<sageVirtualDesktop *> vdtList;
   std::vector<displayConnection *> dispConnectionList;

   char fsIP[SAGE_IP_LEN];
   char pubIP[SAGE_IP_LEN];
   char fsName[SAGE_NAME_LEN];
   char conManIP[SAGE_IP_LEN];
   int conManPort;
   int uiPort, sysPort, trackPort;
   int winTime;
   int winStep;
   bool NRM;
   bool blockCommands;
   bool fsmClose;
   bool globalSync;
   bool useLocalPort;

   /**
	* generated by init()
	* calls talkToConnectionManager() then exit
	*/
   static void* msgThread(void *args);

   int talkToConnectionManager();

public:
   fsManager();
   ~fsManager();

   /**
	* reads config file then sets the fsManager member variables.<BR>
	* creates fsServer and fsCore instances and calls init() of them.<BR>
	* creates sageVirtualDesktop objects that will launch sageDIsplayManager and sageAudioManager<BR>
	* starts msgThread().
	*/
   int init(char *config);

   void printAppList();
   char* getFsIP() { return fsIP; }
   int getUiPort() { return uiPort; }
   int getSysPort() { return sysPort; }
   int getTrackPort() { return trackPort; }

   /**
	* keeps calling checkClients() of the fsServer object until fsmClose is true
	*/
   void mainLoop();

   int msgToCore(sageMessage &msg, int clientID);
   int msgToDisp(sageMessage &msg, int clientID);
   int sendMessage(int cId, int code, int data);
   int sendMessage(int cId, int code, char* data);
   int sendMessage(int cId, int code);
   int sendMessage(sageMessage &msg);
   int windowChanged(int appId);
   int parseDisplayConnectionInfo(FILE *fp);
   int sendToAllRcvs(int code, char *data = NULL);
   int sendToAllRcvs(int code, int data);
   int sendToVDT(int vdtID, int code, char *data = NULL);
   int sendToVDT(int vdtID, int code, int data);
   int sendToOtherVDT(int vdtID, int code, char *data = NULL);
   int sendToOtherVDT(int vdtID, int code, int data);

   friend class fsCore;
   friend class displayInstance;
   friend class fsServer;
};

#endif