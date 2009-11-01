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

#include "appleMultiContext.h"
#include "sdlSingleContext.h"
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
    */

   shared->nodeID = atoi(argv[3]); // node number on which this object is running

   syncPort = atoi(argv[4]);
   displayID = atoi(argv[5]);

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
   if (syncMaster) {
	   sage::printLog("SAGE Display Manager : start sync server");
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

   sage::printLog("SAGE Display Manager : register to a Free Space Manager");
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
   shared->bufSize = atoi(token)*1048576;

   getToken(data, token);
   int fullScreen = atoi(token);

   int tokenNum = getToken(data, token);
   totalRcvNum = atoi(token);

   if (tokenNum < 1) {
      sage::printLog("sageDisplayManager::init : insufficient parameters in RCV_INIT message");
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
      sage::printLog("sageDisplayManager::init : insufficient parameters in RCV_INIT message");
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

   sage::printLog("SAGE Display Manager : initialization message was successfully parsed");

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

   shared->context = (displayContext *) new sdlSingleContext;
   if (shared->context->init(dispCfg) < 0) {
      sage::printLog("SAGE receiver : Error creating display object ");
      return -1;
   }

   // clear the screen after startup
   shared->context->clearScreen();
   shared->context->refreshScreen();

   sage::printLog("Display Manager is creating display object");

   shared->displayObj = new sageDisplay(shared->context, dispCfg);

   if (initNetworks() < 0)
      return -1;

   // sageSyncClient
   shared->syncClientObj = new sageSyncClient;

   // connect to syncMaster
   if (shared->syncClientObj->connectToServer(masterIp, syncPort) < 0) {
      sage::printLog("SAGE receiver : Fail to connect to sync master");
      return -1;
   }
   sage::printLog("Connected to sync master %s:%d", masterIp, syncPort);

   pthread_t thId;

   // start syncCheckThread
   // this thread will continuously call shared->syncClinetObj->waitForSync()
   if (pthread_create(&thId, 0, syncCheckThread, (void*)this) != 0) {
      sage::printLog("sageDisplayManager::init : can't create sync checking thread");
   }

   if (pthread_create(&thId, 0, refreshThread, (void*)this) != 0) {
      sage::printLog("sageDisplayManager: can't create UI event check thread");
   }

   return 0;
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

   while (!This->rcvEnd) {
      sageEvent *syncEvent = new sageEvent;
      syncEvent->eventType = EVENT_SYNC_MESSAGE;
      char *syncMsg = syncEvent->eventMsg;
      if (This->shared->syncClientObj->waitForSync(syncMsg) == 0) {
         //std::cout << "rcv sync " << syncEvent->eventMsg << std::endl;
         This->shared->eventQueue->sendEvent(syncEvent);
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

void* sageDisplayManager::refreshThread(void *args)
{
   sageDisplayManager *This = (sageDisplayManager *)args;

   while (!This->rcvEnd) {
      This->shared->eventQueue->sendEvent(EVENT_REFRESH_SCREEN);
      sage::usleep(DISPLAY_REFRESH_INTERVAL);
   }

   sage::printLog("sageDisplayManager::refreshThread : exit");
   pthread_exit(NULL);
   return NULL;
}

int sageDisplayManager::initNetworks()
{
   sage::printLog("Display Manager is initializing network objects....");

   tcpObj = new sageTcpModule;
   if (tcpObj->init(SAGE_RCV, streamPort, nwCfg) == 1) {
      sage::printLog("sageDisplayManager is already running");
      return -1;
   }

   sage::printLog("SAGE Display Manager : waiting TCP connections at port %d", streamPort);

   udpObj = new sageUdpModule;
   udpObj->init(SAGE_RCV, streamPort+(int)SAGE_UDP, nwCfg);

   sage::printLog("SAGE Display Manager : waiting UDP connections at port %d", 
         streamPort+(int)SAGE_UDP);

   pthread_t thId;
   nwCheckThreadParam *param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = tcpObj;

   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("sageDisplayManager::initNetwork : can't create network checking thread");
      return -1;
   }

   param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = udpObj;
   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("sageDisplayManager::initNetwork : can't create network checking thread");
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

   sage::printLog("sageDisplayManager::nwCheckThread : exit");
   pthread_exit(NULL);
   return NULL;
}

int sageDisplayManager::initStreams(char *msg, streamProtocol *nwObj)
{
   int senderID, instID, streamType, frameRate;

   sscanf(msg, "%d %d %d %d", &senderID, &streamType, &frameRate, &instID);

   //std::cout << "stream info " << msg << std::endl;

   /**
    * if pixelDownloader object for this application hasn't created before, then create one
	*/
	int index;
	pixelDownloader*  loader = findApp(instID, index);
	std::cout << "------------- init : " << instID << " " << index << std::endl;

   if (loader) {
      loader->addStream(senderID); // EVENT_APP_CONNECTED will be arisen
   }
   else {
	   //creates pixelDownloader if it's not there.
      pixelDownloader *dwloader = new pixelDownloader;
		int index = downloaderList.size();
		dwloader->instID = instID;

      switch(streamType) {
         case SAGE_BLOCK_NO_SYNC : {
            sage::printLog("stream type : SAGE_BLOCK_NO_SYNC");
            dwloader->init(msg, shared, nwObj, false);
            break;
         }

         case SAGE_BLOCK_SOFT_SYNC : {
            sage::printLog("stream type : SAGE_BLOCK_SOFT_SYNC");
            dwloader->init(msg, shared, nwObj, true);

            if (syncMaster) {
               syncGroup *sGroup = new syncGroup;
               sGroup->init(0, SAGE_ASAP_SYNC_SOFT, instID, DISPLAY_MAX_FRAME_RATE);
               syncServerObj->addSyncGroup(sGroup);
            }
            break;
         }

         case SAGE_BLOCK_HARD_SYNC : {
            sage::printLog("stream type : SAGE_BLOCK_HARD_SYNC");
            dwloader->init(msg, shared, nwObj, true);

            if (syncMaster) {
               syncGroup *sGroup = new syncGroup;
               /*
                * int startFrame, int _policy_, int groupID, int frameRate = 1, int sNum = 1
                */
               sGroup->init(0, SAGE_ASAP_SYNC_HARD, instID, DISPLAY_MAX_FRAME_RATE);
               syncServerObj->addSyncGroup(sGroup);
            }
            break;
         }
      }

		std::cout << " reconfigStr " << reconfigStr.size() << std::endl;
		index = reconfigStr.size() -1;
      if (reconfigStr[index])
         dwloader->enqueConfig(reconfigStr[index]);

      dwloader->addStream(senderID);
		downloaderList.push_back(dwloader);
		std::cout << "end----" << std::endl;
   }

   return 0;
}

pixelDownloader* sageDisplayManager::findApp(int id, int& index)
{
	pixelDownloader* temp_app= NULL;
	std::vector<pixelDownloader*>::iterator iter;
	index = 0;
	for(iter = downloaderList.begin(); iter != downloaderList.end(); iter++, index)
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
      sage::printLog("sageDisplayManager is shutting down applications");

		std::vector<pixelDownloader*>::iterator iter;
		std::vector<char*>::iterator iter_str = reconfigStr.begin();
		for(iter = downloaderList.begin(); iter != downloaderList.end(); iter++,iter_str++)
		{
			temp_app =(pixelDownloader*) *iter;
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
         float depth = 1.0f + zValue - 0.01f*instID;
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
			std::cout << "sync message " << std::endl;
         processSync((char *)event->eventMsg);
         break;
      }

      case EVENT_REFRESH_SCREEN : {
         if (shared->displayObj->isDirty())
            shared->displayObj->updateScreen();

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

int sageDisplayManager::processSync(char *msg)
{
   int groupID, syncFrame, dataLen, cmd;
   sscanf(msg, "%d %d %d %d", &groupID, &syncFrame, &dataLen, &cmd);

	int index;
	pixelDownloader *loader = findApp(groupID, index);
   if (loader) {
      loader->processSync(syncFrame, cmd);
      if (loader->getStatus() == PDL_WAIT_SYNC)
         loader->fetchSageBlocks();
      //else
      //   sage::printLog("sageDisplayManager::processSync : error in pixel downloader status");
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

