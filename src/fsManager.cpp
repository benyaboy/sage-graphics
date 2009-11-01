/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: fsManager.cpp - the container of the Free Space Manager components
 *            - fsCore, displayInstance, appData and fsServer
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
 
#include "fsManager.h"
#include "fsServer.h"
#include "fsCore.h"
#include "streamInfo.h"
#include "sageVirtualDesktop.h"
#include "displayInstance.h"
#include "streamProtocol.h"

fsManager::fsManager() : NRM(false), fsmClose(false), globalSync(true), useLocalPort(false)
{
   core = NULL;
   server = NULL;
   vdtList.clear();
   dispConnectionList.clear();
   
   winTime = 100;
   winStep = 0;
	m_execIndex = 0;
}

fsManager::~fsManager()
{
   delete core;
   delete server;

   for(int i=0; i<execList.size(); i++)
      if (execList[i])
      delete execList[i];
}

int fsManager::init(char *conf_file)
{
   char *sageDir = getenv("SAGE_DIRECTORY");
   if (!sageDir) {
      std::cout << "fsManager : cannot find the environment variable SAGE_DIRECTORY" << std::endl;
      return -1;
   }   
   
   char fsConfigFile[TOKEN_LEN];
   sprintf(fsConfigFile, "%s/bin/%s", sageDir, conf_file);
   
   FILE *fileFsConf = fopen(fsConfigFile, "r");
   
   if (!fileFsConf) {
      printf("fail to open fsManager config file [%s]\n",fsConfigFile);
      return -1;
   }

   char token[TOKEN_LEN];
   int tokenIdx = getToken(fileFsConf, token);
   bool tokenAcquired;
   bool conManEnabled = false;
   char tileConfigFile[TOKEN_LEN];
   nwInfo = new sageNwConfig;
   
   char audioConfigFile[TOKEN_LEN];

   while(tokenIdx != EOF) {
      tokenAcquired = false;
      if (strcmp(token, "fsManager") == 0) {
         getToken(fileFsConf, token);
         strcpy(fsName, token);
         getToken(fileFsConf, token);
         strcpy(fsIP, token);
         tokenIdx = getToken(fileFsConf, token);
         if (strcmp(token, "systemPort") != 0) {
            strcpy(pubIP, token);
         }
         else {
            tokenAcquired = true;   
            strcpy(pubIP, fsIP);
         }   
      }
      else if (strcmp(token, "systemPort") == 0) {   
         getToken(fileFsConf, token);
         sysPort = atoi(token);
      }
      else if (strcmp(token, "uiPort") == 0) {   
         getToken(fileFsConf, token);
         uiPort = atoi(token);
      }
      else if (strcmp(token, "trackPort") == 0) {   
         getToken(fileFsConf, token);
         trackPort = atoi(token);
      }
      else if (strcmp(token, "conManager") == 0) {
         getToken(fileFsConf, token);
         strcpy(conManIP, token);
         getToken(fileFsConf, token);
         conManPort = atoi(token);
         conManEnabled = true;
      } 
      else if (strcmp(token, "tileConfiguration") == 0) {
         getToken(fileFsConf, token);
         sprintf(tileConfigFile, "%s/bin/%s", sageDir, token);
      }
      else if (strcmp(token, "globalSync") == 0) {
         getToken(fileFsConf, token);
         sage::toupper(token);
         if (strcmp(token, "NO") == 0)
            globalSync = false;
         else
            globalSync = true;
      }      
      else if (strcmp(token, "receiverSyncPort") == 0) {
         getToken(fileFsConf, token);
         rInfo.syncPort = atoi(token);
      }
      else if (strcmp(token, "receiverStreamPort") == 0) {
         getToken(fileFsConf, token);
         rInfo.streamPort = atoi(token);
      }
      else if (strcmp(token, "receiverBufSize") == 0) {
         getToken(fileFsConf, token);
         rInfo.bufSize = atoi(token);
      }
      else if (strcmp(token, "fullScreen") == 0) {
         getToken(fileFsConf, token);
         rInfo.fullScreen = (bool)atoi(token);
      }
      else if (strcmp(token, "rcvNwBufSize") == 0) {
         getToken(fileFsConf, token);
         nwInfo->rcvBufSize = getnumber(token); // atoi(token);
      }
      else if (strcmp(token, "sendNwBufSize") == 0) {
         getToken(fileFsConf, token);
         nwInfo->sendBufSize = getnumber(token); // atoi(token);
      }
      else if (strcmp(token, "MTU") == 0) {
         getToken(fileFsConf, token);
         nwInfo->mtuSize = atoi(token);
      }
      else if (strcmp(token, "winTime") == 0) {
         getToken(fileFsConf, token);
         winTime = atoi(token);
      }
      else if (strcmp(token, "winStep") == 0) {
         getToken(fileFsConf, token);
         winStep = atoi(token);
      }
      else if (strcmp(token, "NRM") == 0) {
         getToken(fileFsConf, token);
         NRM = (bool)atoi(token);
      }
      else if (strcmp(token, "audioConfiguration") == 0) {
         getToken(fileFsConf, token);
         sprintf(audioConfigFile, "%s/bin/%s", sageDir, token);
      }
      else if (strcmp(token, "audio") == 0) {
         getToken(fileFsConf, token);
         rInfo.audioOn = (bool) (strcmp(token, "true") == 0);
      }
      else if (strcmp(token, "receiverAudioSyncPort") == 0) {
         getToken(fileFsConf, token);
         rInfo.audioSyncPort = atoi(token);
      }
      else if (strcmp(token, "receiverAudioPort") == 0) {
         getToken(fileFsConf, token);
         rInfo.audioPort = atoi(token);
      }
      else if (strcmp(token, "syncPort") == 0) {
         getToken(fileFsConf, token);
         rInfo.agSyncPort = atoi(token);
      }

      if (!tokenAcquired)
         tokenIdx = getToken(fileFsConf, token);
   }

   server = new fsServer;
   server->init(this);

   core = new fsCore;
   core->init(this);

   FILE *tileFp = fopen(tileConfigFile, "r");
   if (!tileFp) {
      printf("fail to open tile config file [%s]\n", tileConfigFile);
      return -1;
   }

   bool nextDisplay = true;
   int displayID = 0;
   
   while (nextDisplay) {
      sageVirtualDesktop *vdt = new sageVirtualDesktop(this, displayID);
      nextDisplay = vdt->parseConfigfile(tileFp, displayID > 0);
      if (globalSync && displayID > 0)
         strcpy(vdt->masterIP, vdtList[0]->masterIP);
      vdt->launchReceivers(fsIP, sysPort, rInfo.syncPort, globalSync);
      vdtList.push_back(vdt);
      displayID++;
   }
   
   if (vdtList.size() > 1)
      parseDisplayConnectionInfo(tileFp);   
   
#ifdef SAGE_AUDIO
   if(rInfo.audioOn) {
      FILE *audioFp = fopen(audioConfigFile, "r");
      std::cout << "audio on " << std::endl;

      if (!audioFp) {
         printf("fail to open tile config file [%s]\n", audioConfigFile);
         return -1;
      }
      vdtList[0]->parseAudioConfigfile(audioFp);
      vdtList[0]->launchAudioReceivers(fsIP, sysPort, rInfo.syncPort);

      fclose(audioFp);
   }
#endif

   blockCommands = false;
  
   pthread_t thId;
   
   if (conManEnabled && pthread_create(&thId, 0, msgThread, (void*)this) != 0) {
      std::cerr << "fsManager : can't create msgThread" << std::endl;
      return -1;
   }
   
   return 0;
}

int fsManager::talkToConnectionManager()
{
   int dimX = vdtList[0]->dimX;
   int dimY = vdtList[0]->dimY;
   int vdtWidth = vdtList[0]->width;
   int vdtHeight = vdtList[0]->height;
   int tileWidth = vdtList[0]->globalType.width;
   int tileHeight = vdtList[0]->globalType.height;
   
   char fsMsg[CMAN_MSG_SIZE];
   memset((void *)fsMsg, 0, CMAN_MSG_SIZE);
   sprintf(fsMsg, "100\n%s %s\n%s %d\n%s %d\n%d %d %d %d %d %d", fsName, SAGE_VERSION, fsIP, sysPort, pubIP, uiPort, 
            dimX, dimY, vdtWidth, vdtHeight,   tileWidth, tileHeight); 

   int dataSize = CMAN_MSG_SIZE;
   QUANTAnet_tcpClient_c *client = new QUANTAnet_tcpClient_c;
   client->setTimeOut(1);
   
   sage::printLog("try to connect to .... %s : %d", conManIP, conManPort);
   while (client->connectToServer(conManIP, conManPort) < 0) {
      sage::printLog("fsManager : fail to connect to the connection manager");
      sage::sleep(1);
   }
   sage::printLog("connected to connection manager %s : %d", conManIP, conManPort);
   
   bool reconnect = false;
   
   while (!fsmClose) {
      int status = client->write(fsMsg, &dataSize, QUANTAnet_tcpClient_c::BLOCKING);
      if (status != QUANTAnet_tcpClient_c::OK) {
         reconnect = true;
      }
      //else
      //   std::cout << "message sent to connection manager : " << fsMsg << std::endl;         
      
      sage::sleep(5);
      
      if (reconnect) {
         sage::printLog("try to connect to .... %s : %d", conManIP, conManPort);
         while (client->connectToServer(conManIP, conManPort) < 0) {
            std::cout << "fsManager : fail to connect to the connection manager" << std::endl;
            sage::sleep(1);
         }
         sage::printLog("connected to connection manager %s : %d", conManIP, conManPort);
         reconnect = false;
      }
   }
   return 0;
}

void* fsManager::msgThread(void *args)
{
   fsManager *This = (fsManager *)args;
   
   This->talkToConnectionManager();
   
   pthread_exit(NULL);
   return NULL;
}

void fsManager::mainLoop()
{   
   while (!fsmClose) {
      server->checkClients();
//      std::cout << "check clients " << std::endl;
      sage::usleep(100);
   }
}

int fsManager::msgToCore(sageMessage &msg, int clientID)
{
   return core->parseMessage(msg, clientID);
}   

int fsManager::msgToDisp(sageMessage &msg, int clientID)
{
   int dispNum = dispList.size();

   // find associated display manager and forward this message
   if (msg.getCode() < DISP_MESSAGE + 100) {
      for(int i=0; i<dispNum; i++) {
         if (!dispList[i])
            continue;
            
         if (dispList[i]->getSailClient() == clientID) {
            return dispList[i]->parseMsg(msg);
         }
      }
   }
   else {
      char token[TOKEN_LEN];
      getToken((char *)msg.getData(), token);
      int winId = atoi(token);
      //      std::cout << "disp message win id " << winId << std::endl;
      //if (winId >= dispNum) {
      //   sage::printLog("fsManager::msgToDisp : window ID is out of scope");
      //   return -1;
      //}
         
		displayInstance* disp = NULL;
		std::vector<displayInstance*>::iterator iter_disp;
		for(iter_disp = dispList.begin(); iter_disp != dispList.end(); iter_disp++)
		{
			if ((*iter_disp)->winID == winId)
			{
				disp = (displayInstance*) *iter_disp;
				break;
			}
		}

		if (!disp) {
         sage::printLog("fsManager::msgToDisp : window %d doesn't exist", winId);
         return -1;
      }
   
		return disp->parseMsg(msg);
   }
      
   return 0;
}   

int fsManager::sendMessage(int cId, int code, int data)
{
   return server->sendMessage(cId, code, data);
}   

int fsManager::sendMessage(int cId, int code, char* data)
{
   return server->sendMessage(cId, code, data);
}   

int fsManager::sendMessage(int cId, int code)
{
   return server->sendMessage(cId, code);
}   

int fsManager::sendMessage(sageMessage &msg)
{
   return server->sendMessage(msg);
}   

int fsManager::windowChanged(int winId)
{
   blockCommands = false;
   //std::cout << std::endl << "window changed " << std::endl << std::endl;
   
   return core->windowChanged(winId);
}   

int fsManager::parseDisplayConnectionInfo(FILE *fp)
{
   char token[TOKEN_LEN];
   displayConnection *connection = NULL;
   
   while (getToken(fp, token) != EOF) {
      sage::toupper(token);
      if (strcmp(token, "CONNECTION") == 0) {
         connection = new displayConnection;
         for (int i=0; i<2; i++) {
            getToken(fp, token);
            int dispID = atoi(token);
            int j = 0;
            for ( ; j<vdtList.size(); j++) {
               if (vdtList[j]->displayID == dispID) { 
                  connection->displays[i] = vdtList[j];
                  vdtList[j]->connectionList.push_back(connection);
                  break;
               }
            }
            
            if (j == vdtList.size()) {
               sage::printLog("Can't find a display specified in display connection");
                  return -1;
            }
               
            getToken(fp, token);
            if (strcmp(token, "LEFT") == 0)
               connection->edges[i] = LEFT_EDGE;
            else if (strcmp(token, "RIGHT") == 0)
               connection->edges[i] = RIGHT_EDGE;
            else if (strcmp(token, "BOTTOM") == 0)
               connection->edges[i] = BOTTOM_EDGE;
            else if (strcmp(token, "TOP") == 0)
               connection->edges[i] = TOP_EDGE;
            else {
               sage::printLog("Error in parsing display connection : invalid edge name");
               return -1;
            }
         }
         
         getToken(fp, token);
         connection->offset = atoi(token);
         dispConnectionList.push_back(connection);
         
         /*
         int vdtNum = vdtList.size();
         int vdtCnt = 0;
         for (int i=0; i<vdtNum; i++) {
            if ((connection->displays[0]->displayID == vdtList[i]->displayID) ||
               (connection->displays[1]->displayID == vdtList[i]->displayID)) {
               vdtList[i]->connectionList.push_back(connection);
               vdtCnt++;
            }   
         }
         
         
         if (vdtCnt < 2) {
            sage::printLog("Can't find a display specified in display connection");
            return -1;
         }
         */   
      }
   }
   
   return 0;
}

int fsManager::sendToVDT(int vdtID, int code, char *data)
{
   vdtList[vdtID]->sendToAll(code,data);
   
   return 0;
}

int fsManager::sendToOtherVDT(int vdtID, int code, char *data)
{
   for (int i=0; i<vdtList.size(); i++) {
      if (i != vdtID)
         vdtList[i]->sendToAll(code,data);
   }   
   
   return 0;
}

int fsManager::sendToVDT(int vdtID, int code, int data)
{
   char token[TOKEN_LEN];
   sprintf(token, "%d", data);
   sendToVDT(vdtID, code, token);
   
   return 0;
}

int fsManager::sendToOtherVDT(int vdtID, int code, int data)
{
   char token[TOKEN_LEN];
   sprintf(token, "%d", data);
   sendToOtherVDT(vdtID, code, token);
   
   return 0;
}

int fsManager::sendToAllRcvs(int code, char *data)
{
   for (int i=0; i<vdtList.size(); i++) {
      vdtList[i]->sendToAll(code, data);
   }
      
   return 0;
}

int fsManager::sendToAllRcvs(int code, int data)
{
   char token[TOKEN_LEN];
   sprintf(token, "%d", data);
   sendToAllRcvs(code, token);
   
   return 0;
}
