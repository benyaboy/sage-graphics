/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageDisplayManager.cpp
 * Author : Byungil Jeong, Rajvikram Singh
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

#if defined(__APPLE__)
#include "appleMultiContext.h"
#else
#include "sdlSingleContext.h"
#endif

#include "sageDisplayManager.h"
#include "sageSharedData.h"
#include "sageEvent.h"
#include "sageReceiver.h"
#include "sageSync.h"
#include "sageDisplay.h"
#include "sageTcpModule.h"
#include "sageUdpModule.h"
#include "pixelDownloader.h"
#include "sageBlockQueue.h"

sageDisplayManager::~sageDisplayManager()
{
   if (shared)
      delete shared;

   if (tcpObj)
      delete tcpObj;

   if (udpObj)
      delete udpObj;

   if (syncBBServerObj)
      delete syncBBServerObj;


   if (syncServerObj)
      delete syncServerObj;

	std::vector<pixelDownloader*>::iterator iter;
	std::vector<char*>::iterator iter_str= reconfigStr.begin();

	pixelDownloader* temp_app= NULL;
	char* temp_str= NULL;
	for(iter = downloaderList.begin(); iter != downloaderList.end(); iter++, iter_str++)
	{
		temp_app = (pixelDownloader*) *iter;
		if(!temp_app) continue;
		delete temp_app;
		temp_str = (char*) *iter_str;
		if(!temp_str) continue;
		delete temp_str;
	}
	downloaderList.clear();
	reconfigStr.clear();
}

sageDisplayManager::sageDisplayManager(int argc, char **argv)
{
   if (argc < 7) {
      sage::printLog("SAGE receiver : More arguments are needed");
      exit(0);
   }

   shared = new dispSharedData;

   /**
    * for example,
    * 1, 67.58.62.100 // fs IP
    * 2. 20002 // fs port
    * 3. 17 // node number)
    * 4. 12000 // sync port
    * 5. 0 // display ID
    * 6, 1 // global sync
    *
    * 7, 11001 // sync barrier port
    * 8, 100 // refresh rate in Hz
    * 9, syncMaster polling interval (in while loop, how long select() waits
    * 10, 0 nosync, 1 data sync only, 2 swapbuffer sync(default), 3 NTP
    */

   shared->nodeID = atoi(argv[3]); // node number on which this object is running

   syncPort = atoi(argv[4]);
   displayID = atoi(argv[5]);


   syncBarrierPort = atoi(argv[7]);
   syncRefreshRate = atoi(argv[8]); // Hz
   syncMasterPollingInterval = atoi(argv[9]); // in usec, 1000 = 1ms
   syncLevel = atoi(argv[10]);

   /**
    * 0th node (67.58.62.93) will become the syncMaster
    */
   bool globalSync = (bool)atoi(argv[6]);
   if (globalSync)
      syncMaster = ((shared->nodeID == 0) && (displayID == 0));
   else
	   syncMaster = (shared->nodeID == 0);


   /**
    * creating syncServerObject
    */
   if (syncMaster && syncLevel > 0) {
	   syncServerObj = NULL;
	   sage::printLog("\nSDM::SDM() : SDM %d creating the syncBBServer.", shared->nodeID);

	   syncBBServerObj = new sageSyncBBServer(syncLevel);

	   // init opens socket and starts syncServerThread
	   if (syncBBServerObj->init(syncPort) < 0) {
		   sage::printLog("SAGE receiver : Error init'ing the sync server object");
		   delete syncBBServerObj;
		   exit(0);
	   }

	   // two phase
	   if ( syncLevel == 2 ) {
		   sage::printLog("\nSDM::SDM() : SDM %d initializing syncBarrierServer", shared->nodeID);
		   if ( syncBBServerObj->initBarrier(syncBarrierPort) < 0 ) {
			   sage::printLog("sageDisplayManager::sageDisplayManager() : Error in sageSyncServer::initBarrier(%d)", syncBarrierPort);
			   delete syncBBServerObj;
			   exit(0);
		   }
	   }
   }
   else if (syncMaster && syncLevel == -1) { // OLD one
	   syncBBServerObj = NULL;
	   sage::printLog("\nSDM SyncMaster : start OLD Sync Server");
	   syncServerObj = new sageSyncServer;

	   if (syncServerObj->init(syncPort) < 0) {
		   sage::printLog("SAGE receiver : Error init'ing the sync server object");
		   delete syncServerObj;
		   exit(0);
	   }
   }

   char fsIP[SAGE_IP_LEN];
   strcpy(fsIP, argv[1]);
   int fsPort = atoi(argv[2]);

   /**
    * connecting to fsManager
    */
   fsClient::init(fsPort);
   connect(fsIP);

   //sage::printLog("sageDisplayManager::sageDisplayManager() : SDM %d register to a Free Space Manager",shared->nodeID);
   char regMsg[TOKEN_LEN];
   sprintf(regMsg, "%d %d", shared->nodeID, displayID);
   sendMessage(REG_GRCV, regMsg);

   rcvEnd = false;
   sprintf(CLEAR_STR, "0 0 0 0 0 0");

   pthread_t thId;

   if (pthread_create(&thId, 0, msgCheckThread, (void*)this) != 0) {
      sage::printLog("sageDisplayManager: can't create message checking thread");
   }

   if (pthread_create(&thId, 0, perfReportThread, (void*)this) != 0) {
      sage::printLog("sageDisplayManager: can't create performance report thread");
   }
}

int sageDisplayManager::init(char *data)
{
   char token[TOKEN_LEN];
   char masterIp[SAGE_IP_LEN];
   int screenWidth, screenHeight;
   int dimX, dimY;
   int revY[MAX_TILES_PER_NODE];
   sageDisplayConfig dispCfg;

   getToken(data, token);
   nwCfg.rcvBufSize = atoi(token);

   getToken(data, token);
   nwCfg.sendBufSize = atoi(token);

   getToken(data, token);
   nwCfg.mtuSize = atoi(token);

   getToken(data, token);
   streamPort = atoi(token);

   getToken(data, token);
   shared->bufSize = atoi(token)*1048576; // receiverBufSize to MB

   getToken(data, token);
   int fullScreen = atoi(token);

   int tokenNum = getToken(data, token);
   totalRcvNum = atoi(token);

   if (tokenNum < 1) {
      sage::printLog("sageDisplayManager::init() : insufficient parameters in RCV_INIT message");
      return -1;
   }

   getToken(data, masterIp);

   getToken(data, token);
   screenWidth = atoi(token);
   getToken(data, token);
   screenHeight = atoi(token);
   getToken(data, token);
   dimX = atoi(token);
   getToken(data, token);
   dimY = atoi(token);
   getToken(data, token);
   dispCfg.winX = atoi(token);
   tokenNum = getToken(data, token);
   dispCfg.winY = atoi(token);

   if (tokenNum < 1) {
      sage::printLog("[%d] SDM::init() : insufficient parameters in RCV_INIT message !", shared->nodeID);
      return -1;
   }

   for (int i=0; i<dimY; i++) {
      for (int j=0; j<dimX; j++) {
         getToken(data, token);  // will remove this line (tile id read) later.....
         //shared->tileTable.addEntry(atoi(token));
         getToken(data, token);
         dispCfg.tileRect[i*dimX + j].x = atoi(token);
         getToken(data, token);
         dispCfg.tileRect[i*dimX + j].y = atoi(token);
         dispCfg.tileRect[i*dimX + j].width = screenWidth;
         dispCfg.tileRect[i*dimX + j].height = screenHeight;
         getToken(data, token);
         revY[i*dimX + j] = atoi(token);
      }
   }

   //shared->tileTable.generateTable();

   //sage::printLog("SDM::init() : SDM %d init message has successfully parsed",shared->nodeID);

   dispCfg.width = screenWidth;
   dispCfg.height = screenHeight;
   dispCfg.dimX = dimX;
   dispCfg.dimY = dimY;

   if (dimX > 1)
      dispCfg.mullionX = (int)floor((dispCfg.tileRect[1].x - dispCfg.tileRect[0].x - screenWidth)/2.0 + 0.5);
   else
      dispCfg.mullionX = 0;

   if (dimY > 1)
      dispCfg.mullionY = (int)floor((revY[0] - revY[dimX*(dimY-1)] - screenHeight)/2.0 + 0.5);
   else
      dispCfg.mullionY = 0;

   dispCfg.xpos = dispCfg.tileRect[0].x;
   dispCfg.ypos = revY[dimX*(dimY-1)];
   dispCfg.fullScreenFlag = fullScreen;
   dispCfg.red = 0;
   dispCfg.green = 0;
   dispCfg.blue = 0;
   dispCfg.displayID = displayID;

   //shared->context = (displayContext *) new sdlSingleContext;
   shared->context = (displayContext *) new appleMultiContext;
   if (shared->context->init(dispCfg) < 0) {
      sage::printLog("[%d] SDM::init() : Error creating display object ", shared->nodeID);
      return -1;
   }

   // clear the screen after startup
   shared->context->clearScreen();
   shared->context->refreshScreen();

   //sage::printLog("sageDisplayManager::init() : SDM %d is creating sageDisplay object", shared->nodeID);
   shared->displayObj = new sageDisplay(shared->context, dispCfg);

   if (initNetworks() < 0)
      return -1;

   // sageSyncClient
	pthread_t thId;
   if ( syncLevel > 0 ) {
		shared->syncClientObj = new sageSyncClient(syncLevel);

   // connect to syncMaster
	   // The parameter shared->nodeID will trigger send() which is for new sync
	   if (shared->syncClientObj->connectToServer(masterIp, syncPort, shared->nodeID) < 0) {
		   sage::printLog("SDM::init() : SDM %d, Fail to connect to syncServer !", shared->nodeID);
		   return -1;
	   }
	   else {
		   //sage::printLog("SDM::init() : SDM %d, Connected to sync master %s:%d", shared->nodeID, masterIp, syncPort);
	   }

	   sage::sleep(1);
	   if ( syncLevel == 2 ) {
		   if (shared->syncClientObj->connectToBarrierServer(masterIp, syncBarrierPort, shared->nodeID) < 0) {
			   sage::printLog("[%d] SDM::init() : Failed to connect to syncBarrierServer !", shared->nodeID);
			   return -1;
		   }
		   else {
			   //sage::printLog("SDM::init() : SDM %d, Connected to sync master barrier %s:%d", shared->nodeID, masterIp, syncBarrierPort);
		   }
	   }

	   // start syncCheckThread
	   // this thread will continuously call shared->syncClinetObj->waitForSync()
	   if (pthread_create(&thId, 0, syncCheckThread, (void*)this) != 0) {
		   sage::printLog("[%d] SDM::init() : Failed to create syncCheckThread !", shared->nodeID);
		   return -1;
	   }

	   // starting sync mainLoop
	   if (syncMaster) {
		   syncBBServerObj->startManagerThread(totalRcvNum, syncRefreshRate, syncMasterPollingInterval);
	   }
   }
   else if ( syncLevel == -1 ) {
	   shared->syncClientObj = new sageSyncClient(syncLevel);
		// connect to syncMaster
		if (shared->syncClientObj->connectToServer(masterIp, syncPort) < 0) {
			sage::printLog("SDM::init() : SDM %d, Fail to connect to syncServer !", shared->nodeID);
			return -1;
		}
		//sage::printLog("Connected to sync master %s:%d", masterIp, syncPort);

		pthread_t thId;

		// start syncCheckThread
		// this thread will continuously call shared->syncClinetObj->waitForSync()
		if (pthread_create(&thId, 0, syncCheckThread, (void*)this) != 0) {
			sage::printLog("[%d] SDM::init() : Failed to create syncCheckThread !", shared->nodeID);
			return -1;
		}
	   //sage::printLog("SDM::init() : old synch client is created. ");

   }

   if ( syncLevel == 0 || syncLevel == -1 ) {
		if (pthread_create(&thId, 0, refreshThread, (void*)this) != 0) {
		   sage::printLog("[%d] SDM::init() : Failed to create refreshThread !", shared->nodeID);
		   return -1;
		}
   }

   return 0;
}

void* sageDisplayManager::refreshThread(void *args) {
   sageDisplayManager *This = (sageDisplayManager *)args;

   while (!This->rcvEnd) {
      This->shared->eventQueue->sendEvent(EVENT_REFRESH_SCREEN);
      sage::usleep(DISPLAY_REFRESH_INTERVAL);
   }

   //sage::printLog("sageDisplayManager::refreshThread : exit");
   pthread_exit(NULL);
   return NULL;
}

void* sageDisplayManager::msgCheckThread(void *args)
{
   sageDisplayManager *This = (sageDisplayManager *)args;

   sageMessage *msg;

   while (!This->rcvEnd) {
      msg = new sageMessage;
      if (This->rcvMessageBlk(*msg) > 0 && !This->rcvEnd) {
         //std::cout << "message arrive" << std::endl;
         This->shared->eventQueue->sendEvent(EVENT_NEW_MESSAGE, 0, (void *)msg);
      }
   }

   sage::printLog("sageDisplayManager::msgCheckThread : exit");
   pthread_exit(NULL);
   return NULL;
}

void* sageDisplayManager::syncCheckThread(void *args)
{
	sageDisplayManager *This = (sageDisplayManager *)args;
	sage::printLog("sageDisplayManager::syncCheckThread() has started at SDM %d", This->shared->nodeID);

	int syncMsgLen = -1;
	while (!This->rcvEnd) {
		sageEvent *syncEvent = NULL;

		if ( This->syncLevel == -1 ) {
			syncEvent = new sageEvent;
			syncEvent->eventType = EVENT_SYNC_MESSAGE;
			syncMsgLen = -1;
		}
		else {
			// use MSG_PEEK to find out message length
			syncMsgLen = This->shared->syncClientObj->waitForSyncPeek();
			if ( syncMsgLen <= 0 ) {
				fprintf(stderr, "[%d] SDM::syncCheckThread() : syncMsgLen %d\n", This->shared->nodeID, syncMsgLen);
				//continue;
				break;
			}
			else {
				syncEvent = new sageSyncEvent(EVENT_SYNC_MESSAGE, syncMsgLen, NULL);
			}
		}

		char *syncMsg = syncEvent->eventMsg;
		if (This->shared->syncClientObj->waitForSync(syncMsg, syncMsgLen) == 0) {
			//std::cout << "rcv sync " << syncEvent->eventMsg << std::endl;
			/**
			* This is important !
			*/
			This->shared->eventQueue->sendEventToFront(syncEvent);
		}
	}

	sage::printLog("sageDisplayManager::syncCheckThread : exit");
	pthread_exit(NULL);
	return NULL;
}

void* sageDisplayManager::perfReportThread(void *args)
{
   sageDisplayManager *This = (sageDisplayManager *)args;

   while (!This->rcvEnd) {
      This->perfReport();
      // if (This->shared->context && !This->rcvEnd) This->shared->context->checkEvent();
      sage::usleep(100000);
   }

   sage::printLog("sageDisplayManager::perfReportThread : exit");
   pthread_exit(NULL);
   return NULL;
}

int sageDisplayManager::initNetworks()
{
   sage::printLog("SDM::initNetworks() : SDM %d is now initializing network objects.", displayID);

   tcpObj = new sageTcpModule;
   if (tcpObj->init(SAGE_RCV, streamPort, nwCfg) == 1) {
      sage::printLog("SDM::initNetworkds() : tcpObj->init() failed. SDM %d is already running", displayID);
      return -1;
   }

   sage::printLog("SDM::initNetworks() : SDM %d is waiting TCP connections on port %d", displayID, streamPort);

   udpObj = new sageUdpModule;
   udpObj->init(SAGE_RCV, streamPort+(int)SAGE_UDP, nwCfg);

   sage::printLog("SAGE Display Manager : waiting UDP connections at port %d", streamPort+(int)SAGE_UDP);

   pthread_t thId;
   nwCheckThreadParam *param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = tcpObj;

   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("SDM::initNetwork() : SDM %d failed creating nwCheckThread (TCP)", displayID);
      return -1;
   }

   param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = udpObj;
   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("SDM::initNetwork() : SDM %d failed creating nwCheckThread (UDP)", displayID);
      return -1;
   }

   return 0;
}

void* sageDisplayManager::nwCheckThread(void *args)
{
   nwCheckThreadParam *param = (nwCheckThreadParam *)args;
   streamProtocol *nwObj = (streamProtocol *)param->nwObj;
   sageDisplayManager *This = (sageDisplayManager *)param->This;

   int senderID = -1;
   char regMsg[SAGE_EVENT_SIZE];

   if (nwObj) {
      while (!This->rcvEnd) {
         senderID = nwObj->checkConnections(regMsg);
         if (senderID >= 0 && !This->rcvEnd) {
            This->shared->eventQueue->sendEvent(EVENT_NEW_CONNECTION, regMsg, (void *)nwObj);
         }
      }
   }

   sage::printLog("SDM::nwCheckThread() : exit");
   pthread_exit(NULL);
   return NULL;
}

int sageDisplayManager::initStreams(char *msg, streamProtocol *nwObj)
{
   int senderID, instID, streamType, frameRate;

   sscanf(msg, "%d %d %d %d", &senderID, &streamType, &frameRate, &instID);

   if ( syncLevel == 0 ) {
	   streamType = SAGE_BLOCK_NO_SYNC;
   }

   //std::cout << "stream info " << msg << std::endl;

   /**
    * if pixelDownloader object for this application hasn't created before, then create one
	*/
	int index;
	pixelDownloader*  dwloader = findApp(instID, index);

   if (dwloader) {
		if (dwloader->isInitialized() == true)  {
      	dwloader->addStream(senderID); // EVENT_APP_CONNECTED will be arisen
			return 0;
		}
	} else {
		dwloader = new pixelDownloader;
		dwloader->instID = instID;
		downloaderList.push_back(dwloader);
		reconfigStr.push_back(NULL);
		index = downloaderList.size()-1;
	}

   //creates pixelDownloader if it's not there.
	switch(streamType) {
         case SAGE_BLOCK_NO_SYNC : {
            if (syncMaster) {
			   sage::printLog("\nSDM::initStreams() : Application %d is starting. NO_SYNC", instID);
            }
		   dwloader->init(msg, shared, nwObj, false, syncLevel);
            break;
         }
         case SAGE_BLOCK_HARD_SYNC : {
		   if ( syncMaster ) {
			   sage::printLog("\nSDM::initStreams() : Application %d is starting. SYNC", instID);
		   }
		   dwloader->init(msg, shared, nwObj, true, syncLevel);

		   // syncLevel == -1 // OLD sync
			if (syncLevel == -1 && syncMaster && syncServerObj) {
               syncGroup *sGroup = new syncGroup;
               sGroup->init(0, SAGE_ASAP_SYNC_HARD, instID, DISPLAY_MAX_FRAME_RATE);
               syncServerObj->addSyncGroup(sGroup);
            }
            break;
         }
	}

	if (reconfigStr[index])
		dwloader->enqueConfig(reconfigStr[index]);

	dwloader->addStream(senderID);
   return 0;
}

pixelDownloader* sageDisplayManager::findApp(int id, int& index)
{
	pixelDownloader* temp_app= NULL;
	std::vector<pixelDownloader*>::iterator iter;
	index = 0;
	for(iter = downloaderList.begin(); iter != downloaderList.end(); iter++, index++)
	{
		if ((*iter)->instID == id)
		{
			temp_app =(pixelDownloader*) *iter;
			break;
		}
	}
	return temp_app;
}

int sageDisplayManager::shutdownApp(int instID)
{
   bool appShutdown = false;
	pixelDownloader* temp_app= NULL;
	char* temp_str= NULL;

   if (instID == -1) {
      sage::printLog("sageDisplayManager::shutdownApp() : SDM %d is shutting down all applications", shared->nodeID);
		std::vector<pixelDownloader*>::iterator iter;
		std::vector<char*>::iterator iter_str = reconfigStr.begin();
		for(iter = downloaderList.begin(); iter != downloaderList.end(); iter++,iter_str++)
		{
			temp_app =(pixelDownloader*) *iter;
			if (syncLevel == -1 && syncMaster && syncServerObj)
				syncServerObj->removeSyncGroup(temp_app->instID);
			delete temp_app;
			temp_app = NULL;
			temp_str =(char*) *iter_str;
			delete temp_str;
			temp_str = NULL;
		}
		if(downloaderList.size() > 0)
			appShutdown = true;
		downloaderList.clear();
		reconfigStr.clear();
   }
   else if (instID >= 0) {
		int index;
		temp_app = findApp(instID, index);
		if (temp_app)
		{
			delete temp_app;
			temp_app = NULL;
			temp_str = (char*) reconfigStr[index];
			delete temp_str;
			temp_str = NULL;
			downloaderList.erase(downloaderList.begin() + index);
			reconfigStr.erase(reconfigStr.begin() + index);
      	appShutdown = true;
			//shared->displayObj->onAppShutdown(instID);
			if (syncLevel== -1 && syncMaster && syncServerObj)
				syncServerObj->removeSyncGroup(instID);
		}
   }

   if (appShutdown)
      shared->displayObj->setDirty();
   else {
      sage::printLog("sageDisplayManager::shutdownApp : no apps to be shutdown");
      return -1;
   }

   return 0;
}

int sageDisplayManager::updateDisplay(char *msg)
{
   int instID;
   sscanf(msg, "%d", &instID);
   char *updateInfo = sage::tokenSeek(msg, 1);
	int index;
	pixelDownloader* temp_app = findApp(instID, index);

	if (temp_app) {
      temp_app->enqueConfig(updateInfo);
      if (temp_app->getStatus() == PDL_WAIT_CONFIG)
         temp_app->fetchSageBlocks();
   }
   else {
		/*
      if (reconfigStr[index]) {
         sage::printLog("sageDisplayManager::updateDisplay : invalid instance ID");
         return -1;
      }
		*/
		pixelDownloader *dwloader = new pixelDownloader;
		dwloader->instID = instID;
		downloaderList.push_back(dwloader);

		char* str_config = new char[strlen(updateInfo)+1];
      strcpy(str_config, updateInfo);
		reconfigStr.push_back(str_config);
   }

   return 0;
}

int sageDisplayManager::clearDisplay(int instID)
{
	int index;
	pixelDownloader*  temp_app = findApp(instID, index);

   if (temp_app)
	{
      temp_app->enqueConfig(CLEAR_STR);
   }
	else {
		/*
      if (reconfigStr[instID]) {
         sage::printLog("sageDisplayManager::clearDisplay : invalid instance ID");
         return -1;
      }
		*/
		pixelDownloader *dwloader = new pixelDownloader;
		dwloader->instID = instID;
		downloaderList.push_back(dwloader);

		char* str_config = new char[strlen(CLEAR_STR)+1];
      strcpy(str_config, CLEAR_STR);
		reconfigStr.push_back(str_config);
   }
}

int sageDisplayManager::changeBGColor(sageMessage *msg)
{
   char *colorStr = (char *)msg->getData();
   int red, green, blue;

   sscanf(colorStr, "%d %d %d", &red, &green, &blue);

   shared->displayObj->changeBGColor(red, green, blue);

   return 0;
}

int sageDisplayManager::changeDepth(sageMessage *msg)
{
   char *depthStr = (char *)msg->getData();
   char token[TOKEN_LEN];

   int tokenNum = getToken(depthStr, token);
   int numOfChange = atoi(token);
   bool zOrderChange = false;

	int index;
	pixelDownloader*  temp_app;
	int instID, zValue;
   for (int i=0; i<numOfChange; i++) {

      if (tokenNum > 0) {
         tokenNum = getToken(depthStr, token);
         instID = atoi(token);
      }
      else {
         sage::printLog("More arguments are needed for this command ");
         return -1;
      }

      if (tokenNum > 0) {
         tokenNum = getToken(depthStr, token);
         zValue = atoi(token);
      }
      else {
         sage::printLog("More arguments are needed for this command ");
         return -1;
      }

		temp_app = findApp(instID, index);
      if (temp_app) {
         float depth = 1.0f + zValue - 0.01f*index;
         //float depth = 1.0f + zValue;
         temp_app->setDepth(depth);
         zOrderChange = true;
      }
   }

   if (zOrderChange)
      shared->displayObj->setDirty();

   return 0;
}

int sageDisplayManager::parseEvent(sageEvent *event)
{
   if (!event) {
      sage::printLog("sageDisplayManager::parseEvent : event object is Null");
      return -1;
   }

   switch (event->eventType) {
      case EVENT_NEW_CONNECTION : {
         initStreams(event->eventMsg, (streamProtocol *)event->param);
         break;
      }

      case EVENT_NEW_MESSAGE : {
         parseMessage((sageMessage *)event->param);
         break;
      }

      case EVENT_SYNC_MESSAGE : {
         //processSync((char *)event->eventMsg);
    	  processSync( event );
         break;
      }

      case EVENT_REFRESH_SCREEN : {
         if (shared->displayObj->isDirty())
            shared->displayObj->updateScreen(shared, false);

         if (shared->context)
            shared->context->checkEvent();
         break;
      }

      case EVENT_READ_BLOCK : {
         int instID = atoi((char *)event->eventMsg);
			if (0 > instID) break;

			int index;
			pixelDownloader*  temp_app = findApp(instID, index);
			if (temp_app) {
            if (temp_app->getStatus() == PDL_WAIT_DATA)
               temp_app->fetchSageBlocks();
         }
         break;
      }

		case EVENT_APP_CONNECTED : {
         int instID = atoi((char *)event->eventMsg);
			if (0 > instID) break;

			int index;
			pixelDownloader*  temp_app = findApp(instID, index);

			if (temp_app) {
            sendMessage(DISP_APP_CONNECTED, instID);
         }
         break;
		}
   }

   delete event;

   return 0;
}

int sageDisplayManager::parseMessage(sageMessage *msg)
{
   if (!msg) {
      sage::printLog("sageDisplayManager::parseMessage : message is NULL");
      return -1;
   }

   switch (msg->getCode()) {
      case RCV_INIT : {
         //std::cout << "rcv init : " << (char *)msg->getData() << std::endl;
         if (init((char *)msg->getData()) < 0)
            rcvEnd = true;
         break;
      }

      case RCV_UPDATE_DISPLAY : {
         updateDisplay((char *)msg->getData());
         break;
      }

      case RCV_CLEAR_DISPLAY : {
         clearDisplay(atoi((char *)msg->getData()));
         break;
      }

      case SHUTDOWN_RECEIVERS : {
         shutdownApp(-1);
         //std::cout << "all apps are shutdown" << std::endl;
         //sage::sleep(1);
         rcvEnd = true;
         //delete shared->displayObj;
         //_exit(0);
         break;
      }

      case RCV_CHANGE_BGCOLOR : {
         changeBGColor(msg);
         break;
      }

      case RCV_CHANGE_DEPTH : {
         changeDepth(msg);
         break;
      }

      case RCV_SHUTDOWN_APP : {
         shutdownApp(atoi((char *)msg->getData()));
         break;
      }

      case RCV_PERF_INFO_REQ : {
         startPerformanceReport(msg);
         break;
      }

      case RCV_PERF_INFO_STOP : {
         stopPerformanceReport(msg);
         break;
      }

      case ADD_OBJECT : {
         shared->displayObj->addDrawObjectInstance((char *)msg->getData());
         break;
      }

      case UPDATE_OBJECT_POSITION : {
         shared->displayObj->updateObjectPosition((char *)msg->getData());
         break;
      }

      case REMOVE_OBJECT : {
         shared->displayObj->removeDrawObject((char *)msg->getData());
         break;
      }

      case OBJECT_MESSAGE : {
         shared->displayObj->forwardObjectMessage((char *)msg->getData());
         break;
      }

      case SHOW_OBJECT : {
         shared->displayObj->showObject((char *)msg->getData());
         break;
      }
   }

   msg->destroy();
   delete msg;

   return 0;
}

int sageDisplayManager::processSync(sageEvent *e)
{
	if ( syncLevel == -1 ) {
		int groupID, syncFrame, dataLen, cmd;
		sscanf(e->eventMsg, "%d %d %d %d", &groupID, &syncFrame, &dataLen, &cmd);

		int index;
		pixelDownloader *loader = findApp(groupID, index);
		if (loader) {
			loader->processSync(syncFrame, cmd);
			if (loader->getStatus() == PDL_WAIT_SYNC)
				loader->fetchSageBlocks();
		}
		return 0;
	}

	int *intMsg=(int *)(e->eventMsg); // one for pdl id one for syncFrame
	//int pdlID, activeRcvs, curFrame, updatedFrame, syncFrame;
	bool swapMontageDone = false;

	pixelDownloader *PDL = NULL;
	//std::vector<pixelDownloader*>::iterator iter;
	int index;
	int numIndex = e->buflen / sizeof(int);
	numIndex = numIndex - 1; // The first element is the message length in Byte
	numIndex = numIndex / 2; // this is the number of app which have updated in this round
	for ( int i=0; i<numIndex; i++ ) {
		/**
		* if it's unsigned type, then it should be checked with UINT_MAX or ULONG_MAX or ULLONG_MAX
		*/
		//if ( intMsg[2*i+1] < 0 ) break;

		PDL = findApp(intMsg[2*i+1], index);
		if ( PDL ) {
			swapMontageDone = true;

#ifdef DEBUG_SYNC
			fprintf(stderr, "[%d,%d] SDM::processSync() : It's ready for frame %d\n", shared->nodeID, intMsg[2*i], intMsg[2*i+1]);
#endif

			// trigger to swapMontage
			PDL->processSync(intMsg[2*i+2]);

			// if PDL is waiting sync
			/*
			if (PDL->getStatus() == PDL_WAIT_SYNC) {

				// wake it up
				PDL->fetchSageBlocks(); // should change PDL status
			}
			 */
		}
	}

	switch(syncLevel) {
	case 1:
		// data sync only
		// 1st phase only
		//		shared->displayObj->update(); // ratko version only
		if ( swapMontageDone ) {
			shared->displayObj->updateScreen(shared, false); // barrier flag false
		}
		else if ( shared->displayObj->isDirty() ) {
			shared->displayObj->updateScreen(NULL, false);
		}
		else {
			// do nothing
		}
		break;
	case 2:
		// swap buffer sync. default
		// 2nd phase
		//		shared->displayObj->update();
		if ( swapMontageDone ) {
			shared->displayObj->updateScreen(shared, true); // barrier flag true
		}
		else if ( shared->displayObj->isDirty() ) {
			// no precise sync is needed
			// refresh needed for Ratko

			// comment out below two when using SELECTIVE barrier
			shared->syncClientObj->sendRefreshBarrier(shared->nodeID);
			shared->syncClientObj->recvRefreshBarrier(true); // nonblocking = true

			shared->displayObj->updateScreen(shared, false); // barrier flag must be false here, otherwise it will enter barrier in sageDisplay::updateScreen() again
		}
		else  {
			// no precise sync is needed. just execute barrier here
			// no refreshing needed

			// comment out below two when using SELECTIVE barrier
			shared->syncClientObj->sendRefreshBarrier(shared->nodeID);
			shared->syncClientObj->recvRefreshBarrier(true); // nonblocking = true

			// NO SCREEN REFRESH
			//shared->displayObj->updateScreen(shared, false);
		}
		break;
	case 3:
		// NTP method
		/** delay compensation after 1st phase.
		* This replaces 2nd phase entirely
		* effectively reducing 2n messages
		*/
		//int index = SAGE_SYNC_MSG_LEN / sizeof(int); // array index
		/*
		int index = SAGE_SYNC_MSG_LEN / sizeof(long long); // array index
		shared->deltaT = intMsg[index-1];
		shared->syncMasterT.tv_sec = (time_t)intMsg[index-2];
		shared->syncMasterT.tv_usec = (suseconds_t)intMsg[index-3];
		 */
		break;
	}

	for ( int i=0; i<numIndex; i++ ) {
		PDL = findApp(intMsg[2*i+1], index);
		// if PDL is waiting sync
		if (PDL  &&  PDL->getStatus() == PDL_WAIT_SYNC) {
			// wake it up
			PDL->fetchSageBlocks(); // should change PDL status
		}
	}
	return 0;
}

void sageDisplayManager::mainLoop()
{
   while(!rcvEnd) {
      sageEvent *newEvent = shared->eventQueue->getEvent();
      //std::cout << "get the event " << newEvent->eventType << std::endl;
      parseEvent(newEvent);
   }
}

int sageDisplayManager::perfReport()
{
	std::vector<pixelDownloader*>::iterator iter;
	pixelDownloader* temp_app= NULL;
	char *frameStr = NULL;
	char *bandStr = NULL;
	for (iter =downloaderList.begin(); iter != downloaderList.end(); iter++) {
		temp_app = (pixelDownloader*) *iter;

		if (temp_app) {
         frameStr = NULL;
         bandStr = NULL;
         temp_app->evalPerformance(&frameStr, &bandStr);
         if (frameStr) {
            sendMessage(DISP_RCV_FRATE_RPT, frameStr);
            //std::cout << "send frame rate " << frameStr << std::endl;
            delete [] frameStr;
         }

         if (bandStr) {
            sendMessage(DISP_RCV_BANDWITH_RPT, bandStr);
            //std::cout << "send bandwidth " << bandStr << std::endl;
            delete [] bandStr;
         }
      }
   }

   return 0;
}

int sageDisplayManager::startPerformanceReport(sageMessage *msg)
{
   char *perfStr = (char *)msg->getData();
   int instID, rate;
   sscanf(perfStr, "%d %d", &instID, &rate);

   //std::cout << "start perf report " << rate << std::endl;

	int index;
	pixelDownloader *loader = findApp(instID, index);
   if (loader) {
      //loader->setReportRate(rate);
      //loader->resetTimer();
      //loader->resetBandWidth();
      //loader->resetFrame();
   }
   else {
      sage::printLog("sageDisplayManager::startPerformanceReport : invalid app instance ID");
      return -1;
   }

   return 0;
}

int sageDisplayManager::stopPerformanceReport(sageMessage *msg)
{
/*
   int instID = atoi((char *)msg->getData());

	int index;
	pixelDownloader *loader = findApp(instID, index);

   if (loader) {
      loader->setReportRate(0);
   }
   else {
      sage::printLog("sageDisplayManager::stopPerformanceReport : invalid app instance ID");
      return -1;
   }
*/
   return 0;
}

#if defined(_WIN32) || defined(__APPLE__)
extern "C" int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
#ifdef WIN32
   sage::win32Init();
#endif
   sage::initUtil();

   sageDisplayManager manager(argc, argv);
   manager.mainLoop();

   return 0;
}

