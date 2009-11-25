/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: fsCore.cpp - the core part of the Free Space Manager processing
 *         user commands to run applications or send appropriate orders 
 *         to each part of SAGE.
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
 
#include "fsCore.h"
#include "fsManager.h"
#include "streamInfo.h"
#include "misc.h"
#include "displayInstance.h"
#include "sageVirtualDesktop.h"
#include "streamProtocol.h"
#include "sageDrawObject.h"

FILE *logFile;

fsCore::fsCore()
{
   fsm = NULL;
}

fsCore::~fsCore()
{
}

int fsCore::init(fsManager *m)
{
   fsm = m;
   winSteps = (int)ceil(m->winTime/20.0);
   logFile = fopen("window.log","w+");
   return 0;
}

// BEGIN HYEJUNG
int fsCore::initDisp(int instID)
{
   displayInstance *disp = new displayInstance(fsm, instID);
   fsm->dispList[instID] =disp;
   
   return 0;
}
// END

// BEGIN HYEJUNG
int fsCore::initAudio(int instID)
{
	appInExec *appExec = fsm->execList[instID];

   //int msgLen = 8 + SAGE_IP_LEN * execNum;
   int msgLen = 8 + SAGE_IP_LEN * fsm->vdtList[0]->getNodeNum();
   char *msgStr = new char[msgLen];
   memset(msgStr, 0, msgLen);

   if (!appExec) {
      //continue;
   }
   else 
   {
      fsm->vdtList[0]->generateAudioRcvInfo(fsm->rInfo.audioPort, msgStr);
      //fsm->vdt->generateAudioRcvInfo(appExec->renderNodeIP, fsm->rInfo.audioPort, msgStr);
      //printf("initaudio --> %s\n", msgStr);

      char conMessage[TOKEN_LEN];
      sprintf(conMessage, "%s %d", msgStr, instID);
      if (fsm->sendMessage(appExec->sailClient, SAIL_CONNECT_TO_ARCV, conMessage) < 0) {
			sage::printLog("fsCore : %s(%d) is stuck or shutdown", appExec->appName, instID);
			clearAppInstance(instID);
		}		
   }

   return 0;
}
// END

void fsCore::clearAppInstance(int id)
{
   // clear app instance on display nodes
   fsm->sendToAllRcvs(RCV_SHUTDOWN_APP, id);
   
   // clear app instance on audio nodes
   if (fsm->execList[id]->audioOn) {
      std::vector<int> arcvList = fsm->vdtList[0]->getAudioRcvClientList();
      int arcvNum = arcvList.size();
      for(int i=0; i<arcvNum; i++)
         fsm->sendMessage(arcvList[i], ARCV_SHUTDOWN_APP, id);
   }
      
   // clear app instace on ui clients
   int uiNum = fsm->uiList.size();
   for (int j=0; j<uiNum; j++) {
      if (fsm->uiList[j] < 0)
         continue;

      if (fsm->sendMessage(fsm->uiList[j], UI_APP_SHUTDOWN, id) < 0) {
         sage::printLog("fsCore : uiClient(%d) is stuck or shutdown", j);
         fsm->uiList[j] = -1;
      }
   }	

   // release data structure
   if (fsm->execList[id]) {
      delete fsm->execList[id];
      fsm->execList[id] = NULL;
   }   
   
   if (fsm->dispList[id]) {
      delete fsm->dispList[id];
      fsm->dispList[id] = NULL;
   }
}

// START HYEJUNG
int fsCore::getAvailableInstID(void)
{
	int ith=0;
	std::vector<appInExec*>::iterator iter_exec;
	for(iter_exec = fsm->execList.begin(); iter_exec != fsm->execList.end(); iter_exec++, ith++)
	{
		if(*iter_exec == NULL)
			return ith;
	}

	// create empty slots
	for(int i=0; i< 20; i++)
	{
		fsm->execList.push_back(NULL);
		fsm->dispList.push_back(NULL);
		std::cout << "fsCore::getAvailableInstID : extended " << std::endl;
	}
	return ith;
}
// END

//#define MAX_SAGE_WINDOW_SIZE 4096
#define MAX_SAGE_WINDOW_SIZE 8192

int fsCore::parseMessage(sageMessage &msg, int clientID)
{
   appInExec *app;
   displayInstance *disp;   

   char token[TOKEN_LEN], dataStr[TOKEN_LEN];
   int tokenNum;
   
   if (msg.getData())
      strcpy(dataStr, (char *)msg.getData());

   switch(msg.getCode()) {
      case REG_APP : {
			// BEGIN HYEJUNG
         app = new appInExec;
			memset(app->launcherID, 0, SAGE_NAME_LEN);
			sscanf((char *)msg.getData(), "%s %d %d %d %d %d %s %d %d %d %d %d %d %s %d", 
				app->appName, &app->x, &app->y,
				&app->width, &app->height, &app->bandWidth, app->renderNodeIP, &app->imageWidth,
				&app->imageHeight, (int *)&app->audioOn, (int *)&app->protocol, &app->frameRate, 
				&app->instID, app->launcherID, &app->portForwarding );
		   
         // adjust app window size considering image resolution
			float ar = (float)(app->imageWidth) / (float)(app->imageHeight);
			if ((app->imageWidth > MAX_SAGE_WINDOW_SIZE && app->width < MAX_SAGE_WINDOW_SIZE) || 
				(app->imageHeight > MAX_SAGE_WINDOW_SIZE && app->height < MAX_SAGE_WINDOW_SIZE)) {
				if (ar > 1) {
					app->width = MAX_SAGE_WINDOW_SIZE;
					app->height = (int)(MAX_SAGE_WINDOW_SIZE/ar);
				} else {
					app->height = MAX_SAGE_WINDOW_SIZE;
					app->width = (int)(MAX_SAGE_WINDOW_SIZE*ar);
				}
         }   

         app->sailClient = clientID;   
			int instID = getAvailableInstID();

         char sailInitMsg[TOKEN_LEN];
         memset(sailInitMsg, 0, TOKEN_LEN);
         sprintf(sailInitMsg, "%d %d %d %d", instID, fsm->nwInfo->rcvBufSize,
               fsm->nwInfo->sendBufSize, fsm->nwInfo->mtuSize);
         
			if (fsm->sendMessage(clientID, SAIL_INIT_MSG, sailInitMsg) < 0) {
				sage::printLog("fsCore : %s is stuck or shutdown", app->appName);
			}
			else
         	fsm->execList[instID] = app;   
         
         if (fsm->NRM) {
            char rcvIP[SAGE_IP_LEN];
            fsm->vdtList[0]->getNodeIPs(0, rcvIP);
            char msgStr[TOKEN_LEN];
            memset(msgStr, 0, TOKEN_LEN);
            sprintf(msgStr, "%s %s %d %d", app->renderNodeIP, rcvIP, app->bandWidth, instID);

            int uiNum = fsm->uiList.size();
            for (int j=0; j<uiNum; j++) {
					if (fsm->uiList[j] < 0)
						continue;
						
               if (fsm->sendMessage(fsm->uiList[j], REQUEST_BANDWIDTH, msgStr) < 0) {
						sage::printLog("fsCore : uiClient(%d) is stuck or shutdown", j);
						fsm->uiList[j] = -1;
					}
            }
         }
         else {
				std::cout <<  "instance is created : " << instID << std::endl;
            initDisp(instID);
            if (app->audioOn) {
					std::cout << "initAudio is called" << std::endl;
               initAudio(instID);
				}
               
            //windowChanged(fsm->execList.size()-1);
            //bringToFront(fsm->execList.size()-1);
         }   
			// END
         break;
      }
      
      case NOTIFY_APP_SHUTDOWN : {
         //std::cout << "app shutdown itself" << std::endl;
         getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         int execNum = fsm->execList.size();
         
         if (appID >= 0 && appID < execNum) 
            app = fsm->execList[appID];
         else
            break;   
            
         if (!app)
            break;
         
         clearAppInstance(appID);
         break;
      }
      
      case NETWORK_RESERVED : {
         if (fsm->NRM) {
            int appID = fsm->execList.size()-1;
            app = fsm->execList[appID];

            int success = atoi(dataStr);
            if (success) {
					// BEGIN HYEJUNG ?? NO IDEA.. what this is for...
               initDisp(appID);
               if (app->audioOn)
                  initAudio(appID);   
					// END
               windowChanged(fsm->execList.size()-1);
               bringToFront(fsm->execList.size()-1);
            }
            else {
               if (fsm->sendMessage(app->sailClient, APP_QUIT) < 0) {
						sage::printLog("fsCore : %s(%d) is stuck or shutdown", app->appName, appID);
						clearAppInstance(appID);
					}
					
               fsm->execList.pop_back();
            }   
         }
         break;
      }
            
      case REG_GRCV : {
         getToken((char *)msg.getData(), token);
         int nodeID = atoi(token);
         getToken((char *)msg.getData(), token);
         int dispID = atoi(token);
         // store client ID of receivers
         fsm->vdtList[dispID]->regRcv(clientID, nodeID);
         
         char info[TOKEN_LEN];
         // get the tile config info of display node
         fsm->vdtList[dispID]->getRcvInfo(nodeID, info);
         int streamPort = fsm->vdtList[dispID]->getLocalPort(nodeID);
         if (streamPort < 1024)
            streamPort = fsm->rInfo.streamPort;
         else
            fsm->useLocalPort = true;   
         
         char msgStr[TOKEN_LEN];
         memset(msgStr, 0, TOKEN_LEN);
         sprintf(msgStr, "%d %d %d %d %d %d %d %s", fsm->nwInfo->rcvBufSize, 
            fsm->nwInfo->sendBufSize, fsm->nwInfo->mtuSize,    
            streamPort, fsm->rInfo.bufSize, (int)fsm->rInfo.fullScreen, 
            fsm->vdtList[dispID]->getNodeNum(), info);
				
         if (fsm->sendMessage(clientID, RCV_INIT, msgStr) < 0) {
				sage::printLog("fsCore : displaynode(%d) doesn't respond", nodeID);
			}
         break;
      }

      case REG_ARCV : {
         getToken((char *)msg.getData(), token);
         int nodeID = atoi(token);
         // store client ID of receivers
         fsm->vdtList[0]->regAudioRcv(clientID, nodeID);
                  
         char info[TOKEN_LEN];
         // get the tile config info of display node
         fsm->vdtList[0]->getAudioRcvInfo(nodeID, info);
         char msgStr[TOKEN_LEN]; 
         sprintf(msgStr, "%d %d %d %d %d %d %d %d %s", fsm->nwInfo->rcvBufSize, 
            fsm->nwInfo->sendBufSize, fsm->nwInfo->mtuSize,   fsm->rInfo.audioSyncPort, 
            fsm->rInfo.audioPort, fsm->rInfo.agSyncPort, fsm->rInfo.bufSize, fsm->vdtList[0]->getNodeNum(), info);
   
         //cout << " ----> fsCore : " << msgStr << endl;         
         fsm->sendMessage(clientID, ARCV_AUDIO_INIT, msgStr);
         break;
      }
      
      /*case SYNC_INIT_ARCV : {
         // find gStreamRcvs connected to this aStreamRcv
         getToken((char *)msg.getData(), token);
         int nodeID = atoi(token);
         //fsm->vdt->getgRcvs(nodeID);
         char info[TOKEN_LEN];
         fsm->vdt->getAudioNodeIPs(nodeID, info);
         
         // send message to gStreamRcvs      
         
         // for testing 
         std::vector<int> rcvList = fsm->vdt->getRcvClientList();
         int rcvNum = rcvList.size();   
         for(int i=0; i<rcvNum; i++) {
            fsm->sendMessage(rcvList[i], RCV_SYNC_INIT, info);
         }
            
         break;
         }*/
         
      case SHUTDOWN_APP : {
         int execNum = fsm->execList.size();      
         tokenNum = getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         
         if (appID >= 0 && appID < execNum) 
            app = fsm->execList[appID];
         else {
            std::cout << "FsCore::Invalid App ID " << appID << std::endl;
            break;   
         }
            
         if (!app) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
            break;   
         }

         if (fsm->sendMessage(app->sailClient, APP_QUIT) < 0) {
				sage::printLog("fsCore : %s(%d) is stuck or shutdown", app->appName, appID);
			}

         clearAppInstance(appID);
         
         break;
      }      
      
      case SAGE_APP_SHARE : {
         int execNum = fsm->execList.size();      
         tokenNum = getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         
         if (appID >= 0 && appID < execNum) 
            app = fsm->execList[appID];
         else {
            std::cout << "FsCore::Invalid App ID " << appID << std::endl;
            break;   
         }
            
         if (!app) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
               break;   
         }
         
         char fsIP[SAGE_IP_LEN];
         getToken((char *)msg.getData(), fsIP);
         getToken((char *)msg.getData(), token);
         int fsPort = atoi(token);
         char msgData[TOKEN_LEN];
         memset(msgData, 0, TOKEN_LEN);
         sprintf(msgData, "%s %d", fsIP, fsPort);
         
         //std::cout << fsIP << ":" << fsPort << std::endl;
         
         if (fsm->sendMessage(app->sailClient, SAGE_APP_SHARE, msgData) < 0) {
				sage::printLog("fsCore : %s(%d) is stuck or shutdown", app->appName, appID);
				clearAppInstance(appID);
			}

         break;
      }      
            
      case MOVE_WINDOW : {
         tokenNum = getToken((char *)msg.getData(), token);
         
         int execNum = fsm->execList.size();      
         int winID = atoi(token);
         sageRect devRect;
         
         if (winID >= 0 && winID < execNum) 
            app = fsm->execList[winID];
         else {
            std::cout << "FsCore::Invalid App ID " << winID << std::endl;
            break;   
         }
            
         if (!app) {
            std::cout << "FsCore : app "<< winID << " doesn't exist" << std::endl;
            break;   
         }
            
         // change x position(left) of app window
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            break;   
         }   
         
         tokenNum = getToken((char *)msg.getData(), token);
         devRect.x = atoi(token);
         
         // change y position(bottom) of app windows
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            break;   
         }   
         
         tokenNum = getToken((char *)msg.getData(), token);
         devRect.y = atoi(token);
         
         //fsm->dispList[winID]->modifyStream();
         
         startTime = sage::getTime();
         if (fsm->winStep > 0)
            winSteps = fsm->winStep;
            
         if (fsm->dispList[winID]->changeWindow(devRect, winSteps) < 0) 
            clearAppInstance(winID);
         else   
            windowChanged(winID);
         
         break;
      }
         
      case ADD_OBJECT : {
         if (!msg.getData())
            break;
         
         sageRect *obj = new sageRect;
         char objectName[SAGE_NAME_LEN];
         sscanf(dataStr, "%s %d %d %d %d", objectName, &obj->x, &obj->y, 
               &obj->width, &obj->height);
         int objectID = fsm->drawObjectList.size();
         fsm->drawObjectList.push_back(obj);
         
         char msgStr[TOKEN_LEN];
         sprintf(msgStr, "%d %s", objectID, dataStr);
         fsm->sendToAllRcvs(ADD_OBJECT, msgStr);
         fsm->sendMessage(clientID, UI_OBJECT_INFO, msgStr);
         
         break;
      }   
                         
      case MOVE_OBJECT : {
         if (!msg.getData())
            break;

         int id, dx, dy;
         sscanf(dataStr, "%d %d %d", &id, &dx, &dy);
         
         if (id >= fsm->drawObjectList.size())
            break;
         
         sageRect *obj = fsm->drawObjectList[id];
         obj->x += dx;
         obj->y += dy;

         char msgStr[TOKEN_LEN];
         sprintf(msgStr, "%d %d %d", id, obj->x, obj->y);
         fsm->sendToAllRcvs(UPDATE_OBJECT_POSITION, msgStr);
         
         break;
      }
      
      case REMOVE_OBJECT : {
         if (!msg.getData())
            break;

         int id;
         sscanf(dataStr, "%d", &id);
         
         if (id >= fsm->drawObjectList.size()) {
            std::cout << "fsCore: invalid object ID " << id << std::endl;
            break;
         }   
         
         fsm->sendToAllRcvs(REMOVE_OBJECT, dataStr);
         
         break;
      }
      
      case OBJECT_MESSAGE : {
         if (!msg.getData())
            break;

         int id;
         sscanf(dataStr, "%d", &id);
         
         if (id >= fsm->drawObjectList.size()) {
            std::cout << "fsCore: invalid object ID " << id << std::endl;
            break;
         }   
         
         fsm->sendToAllRcvs(OBJECT_MESSAGE, dataStr);
         
         break;
      }
      
      case SHOW_OBJECT : {
         if (!msg.getData())
            break;

         int id;
         sscanf(dataStr, "%d", &id);
         
         if (id >= fsm->drawObjectList.size()) {
            std::cout << "fsCore: invalid object ID " << id << std::endl;
            break;
         }   
         
         fsm->sendToAllRcvs(SHOW_OBJECT, dataStr);
         
         break;
      }

      case FS_TIME_MSG : {
         double endTime = sage::getTime();
         double bigTime = floor(endTime/1000000.0)*1000000.0;
         endTime = (endTime - bigTime)/1000.0;
         double sageLatency = (endTime - startTime)/1000.0;
         //std::cout << "total latency = " << sageLatency << "ms" << std::endl;
         std::cout << "Time Block received at " << endTime << "ms" << std::endl;
         
      /*
         double endTime = sageGetTime();
         double winTime = (endTime - startTime)/1000.0;
         double singleLatency = winTime / winSteps;
         
         //std::cout << winTime << std::endl;

         //std::cout << "total time = " << winTime << "ms for " << winSteps << " move/resizing" << std::endl;
         //std::cout << "single move/resizing time = " << singleLatency << "ms" << std::endl;
         winSteps = (int)(ceil)(fsm->winTime /singleLatency);
         //std::cout << "# of window change steps changed to " << winSteps << std::endl;
         */
         
         break;
      }
      
      case RESIZE_WINDOW : {
			std::cout << "resize window" << std::endl;
         tokenNum = getToken((char *)msg.getData(), token);
         
         int execNum = fsm->execList.size();      
         int winID = atoi(token);
         sageRect devRect;
         
         if (winID >= 0 && winID < execNum) 
            app = fsm->execList[winID];
         else {
            std::cout << "FsCore::Invalid Win ID " << winID << std::endl;
            break;   
         }
         
         if (!app) {
            std::cout << "FsCore : app "<< winID << " doesn't exist" << std::endl;
               break;   
         }   
            
         // change x position(left) of app window
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            break;   
         }   
         
         int left, right, bottom, top;
         
         tokenNum = getToken((char *)msg.getData(), token);
         left = atoi(token);
         
         // change width of app window
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            break;   
         }
         
         tokenNum = getToken((char *)msg.getData(), token);   
         right = atoi(token);
         
         // change y position(bottom) of app window
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            break;   
         }   
         
         tokenNum = getToken((char *)msg.getData(), token);
         bottom = atoi(token);
         
         // change height of app windows
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            break;   
         }   
         
         tokenNum = getToken((char *)msg.getData(), token);
         top = atoi(token);
         
         // adjust app window size considering image resolution
			float ar = (float)(right - left) / (float)(top - bottom);
         if (ar > 1 && app->imageWidth > MAX_SAGE_WINDOW_SIZE && right - left < MAX_SAGE_WINDOW_SIZE) {
				right = left + MAX_SAGE_WINDOW_SIZE;
				top = bottom + (int)(MAX_SAGE_WINDOW_SIZE/ar);
			}

			if (ar <= 1 && app->imageHeight > MAX_SAGE_WINDOW_SIZE && top - bottom < MAX_SAGE_WINDOW_SIZE) {
				top = bottom + MAX_SAGE_WINDOW_SIZE;
				right = left + (int)(MAX_SAGE_WINDOW_SIZE*ar);
			}   
         
         devRect.x = left - app->x;
         devRect.width = right - (app->width+app->x) - devRect.x;
         devRect.y = bottom - app->y;
         devRect.height = top - (app->y+app->height) - devRect.y;
      
         startTime = sage::getTime();
         
         if (fsm->winStep > 0)
            winSteps = fsm->winStep;
         
         if (fsm->dispList[winID]->changeWindow(devRect, winSteps) < 0)
            clearAppInstance(winID);
         else
            windowChanged(winID);

			// HYEJUNG
			fsm->vdtList[0]->changeWindow(devRect, winSteps) < 0)
         
         break;
      }                   
      
      case ROTATE_WINDOW : {
         rotateWindow((char *)msg.getData());
         break;   
      }
      
      case SAGE_UI_REG : {
         fsm->uiList.push_back(clientID);
         sendDisplayInfo(clientID);
         sendSageStatus(clientID);
         sendAppInfo(clientID);
         break;
      }
      
      case APP_UI_REG : {
         fsm->appUiList.push_back(clientID);
         tokenNum = getToken((char *)msg.getData(), token);
         
         if (strlen(token) > 0) {         
            sendAppStatus(clientID, token);
         }   
         else {
            std::cout << "More arguments are needed for this command " << std::endl;
         }
         break;
      }

      case APP_FRAME_RATE : {
         tokenNum = getToken((char *)msg.getData(), token);
         
         int execNum = fsm->execList.size();      
         int appID = atoi(token);
         
         if (appID >= 0 && appID < execNum) 
            app = fsm->execList[appID];
         else {
            std::cout << "FsCore::Invalid App ID " << appID << std::endl;
            break;   
         }
         
         if (!app) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
               break;   
         }
         
         getToken((char *)msg.getData(), token);
         if (fsm->sendMessage(app->sailClient, SAIL_FRAME_RATE, token) < 0) {
				sage::printLog("fsCore : %s(%d) is stuck or shutdown", app->appName, appID);
				clearAppInstance(appID);
			}
         
         break;
      }
   
      case PERF_INFO_REQ : {
         int dispNum = fsm->dispList.size();      
         tokenNum = getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         
         if (appID >= 0 && appID < dispNum) 
            disp = fsm->dispList[appID];
         else {
            std::cout << "FsCore : Invalid App ID " << appID << std::endl;
            break;   
         }
         
         if (!disp) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
               break;   
         }   
            
         int sendingRate;
               
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            break;   
         }   
         
         tokenNum = getToken((char *)msg.getData(), token);
         sendingRate = atoi(token);
         
         if (disp->requestPerformanceInfo(sendingRate) < 0)
            clearAppInstance(appID);
         
         break;
      }
      
      case STOP_PERF_INFO : {
         int dispNum = fsm->dispList.size();      
         tokenNum = getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         
         if (appID >= 0 && appID < dispNum) 
            disp = fsm->dispList[appID];
         else {
            std::cout << "FsCore : Invalid App ID " << appID << std::endl;
            break;   
         }
         
         if (!disp) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
               break;   
         }   
            
         if (disp->stopPerformanceInfo() < 0)
            clearAppInstance(appID);
         break;
      }
      
      case SAGE_BG_COLOR : {
         int red, green, blue;
         
         tokenNum = getToken((char *)msg.getData(), token);
         red = atoi(token);

         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            _exit(0);
         }   
         
         tokenNum = getToken((char *)msg.getData(), token);
         green = atoi(token);
                  
         if (tokenNum < 1) {
            std::cout << "More arguments are needed for this command " << std::endl;
            _exit(0);
         }   
         
         tokenNum = getToken((char *)msg.getData(), token);
         blue = atoi(token);

         for (int i=0; i<fsm->vdtList.size(); i++)
            fsm->vdtList[i]->changeBGColor(red, green, blue);
               
         break;
      }
      
      case UPDATE_WIN_PROP : {
         int dispNum = fsm->dispList.size();      
         tokenNum = getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         
         if (appID >= 0 && appID < dispNum) 
            disp = fsm->dispList[appID];
         else {
            std::cout << "FsCore : Invalid App ID " << appID << std::endl;
            break;   
         }
         
         if (!disp) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
               break;   
         }   
            
         //disp->updateWinProp((char *)msg.getData());
         windowChanged(appID);
         break;
      }
      
      case BRING_TO_FRONT : {
         tokenNum = getToken((char *)msg.getData(), token);
         int winID = atoi(token);
         bringToFront(winID); 
         
         break;
      }
      
      case SAGE_FLIP_WINDOW : {
         int execNum = fsm->execList.size();      
         tokenNum = getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         
         if (appID >= 0 && appID < execNum) 
            app = fsm->execList[appID];
         else {
            std::cout << "FsCore::Invalid App ID " << appID << std::endl;
            break;   
         }
            
         if (!app) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
               break;   
         }
         
         if (fsm->sendMessage(app->sailClient, SAIL_FLIP_WINDOW) < 0) {
				sage::printLog("fsCore : %s(%d) is stuck or shutdown", app->appName, appID);
				clearAppInstance(appID);
			}

         break;
      }      

      case SAGE_CHECK_LATENCY : {
         int execNum = fsm->execList.size();      
         tokenNum = getToken((char *)msg.getData(), token);
         int appID = atoi(token);
         
         if (appID >= 0 && appID < execNum) 
            app = fsm->execList[appID];
         else {
            std::cout << "FsCore::Invalid App ID " << appID << std::endl;
            break;   
         }
            
         if (!app) {
            std::cout << "FsCore : app "<< appID << " doesn't exist" << std::endl;
               break;   
         }
         
         if (fsm->sendMessage(app->sailClient, SAIL_SEND_TIME_BLOCK) < 0) {
				sage::printLog("fsCore : %s(%d) is stuck or shutdown", app->appName, appID);
				clearAppInstance(appID);
			}
         startTime = sage::getTime();
         
         break;
      }      

      case SAGE_Z_VALUE : {
         fsm->sendToAllRcvs(RCV_CHANGE_DEPTH, dataStr);

         int uiNum = fsm->uiList.size();
         
         for (int j=0; j<uiNum; j++) {
            if (fsm->uiList[j] < 0)
               continue;
			
            if (fsm->sendMessage(fsm->uiList[j], Z_VALUE_RETURN, dataStr) < 0) {
					sage::printLog("fsCore : uiClient(%d) is stuck or shutdown", j);
					fsm->uiList[j] = -1;
				}
			}	
               
         tokenNum = getToken((char *)msg.getData(), token);
         int numOfChange = atoi(token);

         for (int i=0; i<numOfChange; i++) {
            int appID, zValue;
            
            if (tokenNum > 0) {
               tokenNum = getToken((char *)msg.getData(), token);
               appID = atoi(token);
            }
            else {
               std::cout << "More arguments are needed for this command " << std::endl;
               _exit(0);
            }   
            
            if (tokenNum > 0) {
               tokenNum = getToken((char *)msg.getData(), token);
               zValue = atoi(token);
            }
            else {
               std::cout << "More arguments are needed for this command " << std::endl;
               _exit(0);
            }   
            
            fsm->dispList[appID]->setZValue(zValue);   
         }   
               
         break;
      }

      case SAGE_ADMIN_CHECK : {
         int execNum = fsm->execList.size();      
         
         for (int i=0; i<execNum; i++) {
            app = fsm->execList[i];
            if (!app)
               continue;
               
            displayInstance *disp = fsm->dispList[i];
               
            int left = app->x;
            int bottom = app->y;
            int right = app->x + app->width;
            int top = app->y + app->height;
            
            int dispNodes = 0; //disp->getReceiverNum();
            int renderNodes = 0;//app->nodeNum;
            int numStream = 0; //disp->getStreamNum();
         
            char msgStr[512];
            memset(msgStr, 0, 512);
            sprintf(msgStr, "App Name : %s\n   App Instance ID : %d\n",   
                     app->appName, i);
            sprintf(token, "   position (left, right, bottom, top) : ( %d , %d , %d , %d )\n",
                  left, right, bottom, top);
            strcat(msgStr, token);
            sprintf(token, "   Z-Value : %d\n", disp->getZValue());
            strcat(msgStr, token);
            sprintf(token, "   %d Rendering Nodes    %d Display Nodes    %d Streams\n",
                  renderNodes, dispNodes, numStream );
            strcat(msgStr, token);
                  
            fsm->sendMessage(clientID, UI_ADMIN_INFO, msgStr);
         }   
         
         break;
      }
      
      case SAGE_SHUTDOWN : {
         std::cout << "shutdown SAGE" << std::endl;
         int execNum = fsm->execList.size();

         for (int i=0; i<execNum; i++) {
            app = fsm->execList[i];
            if (!app)
               continue;
               
            fsm->sendMessage(app->sailClient, APP_QUIT);
         }   

         fsm->sendToAllRcvs(SHUTDOWN_RECEIVERS);
         
         std::vector<int> arcvList = fsm->vdtList[0]->getAudioRcvClientList();
         int arcvNum = arcvList.size();
         for(int i=0; i<arcvNum; i++)
            fsm->sendMessage(arcvList[i], SHUTDOWN_RECEIVERS);
         
         fsm->fsmClose = true;
         break;
      }
   }

   return 0;
}

int fsCore::rotateWindow(char *msgStr)
{
   char token[TOKEN_LEN];
   int tokenNum = getToken(msgStr, token);
         
   int execNum = fsm->execList.size();      
   int winID = atoi(token);
   appInExec *app = NULL;
   
   if (winID >= 0 && winID < execNum) 
      app = fsm->execList[winID];
   else {
      sage::printLog("fsCore::rotateWindow : invalid Win ID %d", winID);
      return -1;   
   }

   if (!app) {
      sage::printLog("fsCore::rotateWindow : app %d doesn't exist", winID);
         return -1;   
   }   

   if (tokenNum < 1) {
      sage::printLog("fsCore::rotateWindow : more arguments are needed for this command");
      return -1;   
   }

   getToken(msgStr, token);
   int rotation = atoi(token);
   switch(rotation) {
      case 90:
         app->rotate(CCW_90);
         break;
      case 180:
         app->rotate(CCW_180);
         break;
      case 270:
         app->rotate(CCW_270);
         break;
      case -90:
         app->rotate(CCW_270);
         break;
      case -180:
         app->rotate(CCW_180);
         break;
      case -270:
         app->rotate(CCW_90);
         break;
      default:
         sage::printLog("rotation commnad error : invalid rotation degree");
         break;
   }
   
   if (fsm->dispList[winID]->modifyStream() < 0)
      clearAppInstance(winID);
   else   
      windowChanged(winID);
   
   return 0;
}
         
int fsCore::windowChanged(int appId)
{      
   int uiNum = fsm->uiList.size();
   for (int j=0; j<uiNum; j++) {
      if (fsm->uiList[j] < 0)
         continue;

      if (sendAppInfo(appId, fsm->uiList[j]) < 0) {
			sage::printLog("fsCore : uiClient(%d) is stuck or shutdown", j);
			fsm->uiList[j] = -1;
		}
	}	

   return 0;
}         
      
int fsCore::sendSageStatus(int clientID)
{
/*
   int numApps = fsm->appDataList.size();
   
   char statusStr[SAGE_MESSAGE_SIZE], token[TOKEN_LEN];
   
   memset(statusStr, 0, SAGE_MESSAGE_SIZE);
   sprintf(statusStr, "%d\n", numApps);
   
   for (int i=0; i<numApps; i++) {
      appDataNode *dataNode = fsm->appDataList[i];
      int instNum = dataNode->instList.size();
      int execNum = dataNode->execs.size();
      
      sprintf(token, "%s %d %d ", dataNode->app, execNum, instNum);
      strcat(statusStr, token);
      
      for (int j=0; j<instNum; j++) {
         sprintf(token, "%d ", dataNode->instList[j]);
         strcat(statusStr, token);
      }
      strcat(statusStr, "\n");
   }
   
   fsm->sendMessage(clientID, SAGE_STATUS, statusStr);
   
   memset(statusStr, 0, SAGE_MESSAGE_SIZE);

   for (int i=0; i<numApps; i++) {   
      appDataNode *dataNode = fsm->appDataList[i];
      int execNum = dataNode->execs.size();
      
      sprintf(statusStr, "%s\n", dataNode->app);
      
      for(int j=0; j<execNum; j++) {
         sprintf(token,  "config %s : nodeNum=%d initX=%d initY=%d dimX=%d dimY=%d\n",
               dataNode->execs[j].name, dataNode->execs[j].nodeNum, dataNode->execs[j].posX,
               dataNode->execs[j].posY, dataNode->execs[j].dimX,
               dataNode->execs[j].dimY);
         strcat(statusStr, token);
               
         int cmdNum = dataNode->execs[j].cmdList.size();
         for (int k=0; k<cmdNum; k++) {
            sprintf(token, "exec %s %s\n", dataNode->execs[j].cmdList[k].ip, 
               dataNode->execs[j].cmdList[k].cmd);
            strcat(statusStr, token);   
         }   
         sprintf(token, "nwProtocol=%s\n", dataNode->execs[j].nwProtocol);
         strcat(statusStr, token);   
         sprintf(token, "bufNum=%d\n", dataNode->execs[j].bufNum);
         strcat(statusStr, token);   
         sprintf(token, "syncMode=%d\n", dataNode->execs[j].syncMode);
         strcat(statusStr, token);   
      }   
      
      fsm->sendMessage(clientID, APP_EXEC_CONFIG, statusStr);
   }
*/      
   return 0;
}

int fsCore::sendDisplayInfo(int clientID)
{
   char displayStr[STRBUF_SIZE];
   memset(displayStr, 0, STRBUF_SIZE);
   displayStr[0] = '\0';
   
   for (int i=0; i<fsm->vdtList.size(); i++) {
      int tileNum = fsm->vdtList[i]->getTileNum();
      int dimX = fsm->vdtList[i]->dimX;
      int dimY = fsm->vdtList[i]->dimY;
      int vdtWidth = fsm->vdtList[i]->width;
      int vdtHeight = fsm->vdtList[i]->height;
      int tileWidth = fsm->vdtList[i]->tileList[0]->width;
      int tileHeight = fsm->vdtList[i]->tileList[0]->height;
      int id = fsm->vdtList[i]->displayID;

      char info[STRBUF_SIZE];
      memset(info, 0, STRBUF_SIZE);
      sprintf(info, "%d %d %d\n%d %d\n%d %d %d\n", tileNum, dimX, dimY, vdtWidth, vdtHeight,
                           tileWidth, tileHeight, id);
      strcat(displayStr, info);
   }
      
   fsm->sendMessage(clientID, SAGE_DISPLAY_INFO, displayStr);
   
   memset(displayStr, 0, STRBUF_SIZE);
   displayStr[0] = '\0';
   
   for (int i=0; i<fsm->dispConnectionList.size(); i++) {
      int disp0 = fsm->dispConnectionList[i]->displays[0]->displayID;
      int disp1 = fsm->dispConnectionList[i]->displays[1]->displayID;
      int edge0 = fsm->dispConnectionList[i]->edges[0];
      int edge1 = fsm->dispConnectionList[i]->edges[1];
      int offset = fsm->dispConnectionList[i]->offset;
      
      char info[STRBUF_SIZE];
      memset(info, 0, STRBUF_SIZE);
      sprintf(info, "%d %d %d %d %d\n", disp0, edge0, disp1, edge1, offset);
      strcat(displayStr, info);
   }
   
   if (fsm->dispConnectionList.size() > 0)
      fsm->sendMessage(clientID, DISP_CONNECTION_INFO, displayStr);
   
   return 0;
}

int fsCore::sendAppInfo(int appID, int clientID)
{
   if (fsm->dispList.size() <= appID)
      return 1;

   appInExec *app;

   int zValue = 0;   
   char appInfoStr[TOKEN_LEN];
   if (appID >= 0) {
      app = fsm->execList[appID];
      if (!app)
         return -1;
         
      if (fsm->dispList.size() > appID)
         zValue = fsm->dispList[appID]->getZValue();
   }   
   else {
      std::cout << "FsCore::Invalid App ID " << appID << std::endl;
      exit(0);
   }

   int left = app->x;
   int bottom = app->y;
   int right = app->x + app->width;
   int top = app->y + app->height;
   int sailID = app->sailClient;
   int degree = 90 * (int)app->getOrientation();
   
   memset(appInfoStr, 0, TOKEN_LEN);
   sprintf(appInfoStr, "%s %d %d %d %d %d %d %d %d %d %d %s %s", app->appName, appID, 
	   left, right, bottom, top, sailID, zValue, degree, app->displayID, 
	   app->instID, app->launcherID, fsm->dispList[appID]->winTitle);
   
	return fsm->sendMessage(clientID, APP_INFO_RETURN, appInfoStr);
}

int fsCore::sendAppInfo(int clientID)
{
   int appInstNum = fsm->execList.size();
   
	int retVal = 0;
   for (int i=0; i<appInstNum; i++) 
		if (sendAppInfo(i, clientID) < 0)
			retVal = 1;
      
   return retVal;   
}   

int fsCore::sendAppStatus(int clientID, char *appName)
{
   int appInstNum = fsm->execList.size();
   appInExec *app;
   int instNum = 0;
   char statusMsg[TOKEN_LEN];
   statusMsg[0] = '\0';
   
   for (int i=0; i<appInstNum; i++) {
      app = fsm->execList[i];
      if (!app)
         return -1;
      if (strcmp(app->appName, appName) == 0) {
         instNum++;
         char instStr[TOKEN_LEN];
         sprintf(instStr, " %d %d", i, app->sailClient);
         strcat(statusMsg, instStr);
      }      
   }
   
   char msgStr[TOKEN_LEN];
   memset(msgStr, 0, TOKEN_LEN);
   sprintf(msgStr, "%d%s", instNum, statusMsg);
   fsm->sendMessage(clientID, APP_STATUS, msgStr);
   
   return 0;
}

int fsCore::bringToFront(int winID)
{
   appInExec *app;

   int execNum = fsm->execList.size();      
   int numOfWin = 0;
   char depthStr[TOKEN_LEN];
   depthStr[0] = '\0';

   for (int i=0; i<execNum; i++) {
      app = fsm->execList[i];
      if (!app)
         continue;

      displayInstance *disp = fsm->dispList[i];
      if (i != winID)
         disp->increaseZ();
      else
         disp->setZValue(0);
         
      numOfWin++;   
      char zVal[TOKEN_LEN];
      sprintf(zVal, "%d %d ", i, disp->getZValue());
      strcat(depthStr, zVal);
   }

   char msgStr[TOKEN_LEN];
   memset(msgStr, 0, TOKEN_LEN);
   sprintf(msgStr, "%d %s", numOfWin, depthStr);
   
   fsm->sendToAllRcvs(RCV_CHANGE_DEPTH, msgStr);
   
   int uiNum = fsm->uiList.size();

   for (int j=0; j<uiNum; j++) {
      if (fsm->uiList[j] < 0)
         continue;
	
	   if (fsm->sendMessage(fsm->uiList[j], Z_VALUE_RETURN, msgStr) < 0) {
			sage::printLog("fsCore : uiClient(%d) is stuck or shutdown", j);
			fsm->uiList[j] = -1;
		}
	}	
   
   return 0;
}
