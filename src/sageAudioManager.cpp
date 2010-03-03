/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageAudioManager.cpp
 * Author : Hyejung Hur, Byungil Jeong 
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

#include "sageAudioManager.h"

#include "sageEvent.h"
#include "sageAudioReceiver.h"
#include "sageSync.h"

#include "sageAudioCircBuf.h"
#include "sageTcpModule.h"
#include "sageUdpModule.h"

sageAudioManager::~sageAudioManager()
{
   if(eventQueue)
   {
      delete eventQueue;
      eventQueue = NULL;
   }

   if(syncClientObj)
   {
      delete syncClientObj;
      syncClientObj = NULL;
   }

   if(tcpObj)
   {
      delete tcpObj;
      tcpObj = NULL;
   }

   if(udpObj)
   {
      delete udpObj;
      udpObj = NULL;
   }

   if(audioModule)
   {
      delete audioModule;
      audioModule = NULL;
   }

}

sageAudioManager::sageAudioManager(int argc, char **argv)
{
	initialized = false;
	rcvRefreshEnd = false;

   if (argc < 5) {
      sage::printLog("SAGE Audio receiver : More arguments are needed");
      exit(0);
   }

   nodeID = atoi(argv[3]);
   syncPort = atoi(argv[4]);

   char fsIP[SAGE_IP_LEN];
   strcpy(fsIP, argv[1]);
   int fsPort = atoi(argv[2]);

   fsClient::init(fsPort);
   connect(fsIP);

   //std::cout << "initialize : " << fsIP << " " << fsPort << " sync port : " << syncPort << std::endl;

   sage::printLog("SAGE Audio Manager : register to a Free Space Manager");
   sendMessage(REG_ARCV, argv[3]);

   rcvEnd = false;
   receiverList.clear();
	/*for(int i=0; i < 20; i++)
	{
		receiverList.push_back(NULL);
	}
	*/
   eventQueue = new sageEventQueue;

   pthread_t thId;

   if (pthread_create(&thId, 0, msgCheckThread, (void*)this) != 0) {
      sage::printLog("sageAudioManager: can't create message checking thread");
   }

   if (pthread_create(&thId, 0, perfReportThread, (void*)this) != 0) {
      sage::printLog("sageAudioManager: can't create performance report thread");
   }
}

int sageAudioManager::init(char *data)
{
///////////////////
//   sprintf(msgStr, "%d %d %d %d %d %d %d %s", fsm->nwInfo->rcvBufSize,
//         fsm->nwInfo->sendBufSize, fsm->nwInfo->mtuSize,   fsm->rInfo.audioSyncPort,
//         fsm->rInfo.audioPort, fsm->rInfo.bufSize, fsm->vdt->getNodeNum(), info);
//////////////////
   //std::cout << "---> message : " << data << std::endl;

   char token[TOKEN_LEN];

   getToken(data, token);
   nwCfg.rcvBufSize = atoi(token);

   getToken(data, token);
   nwCfg.sendBufSize = atoi(token);

	// For audio, we don't need big buffer for receiving. 
   nwCfg.rcvBufSize = nwCfg.sendBufSize;

   getToken(data, token);
   nwCfg.mtuSize = atoi(token);

   getToken(data, token);
   //syncPort = atoi(token);

   getToken(data, token);
   streamPort = atoi(token);

   getToken(data, token);
   int agSyncPort = atoi(token);

   getToken(data, token);
   //bufSize = atoi(token);

   int tokenNum = getToken(data, token);
   totalRcvNum = atoi(token);

   if (tokenNum < 1) {
      std::cout << "aStreamRcv::init : insufficient parameters in RCV_INIT message" << std::endl;
      return -1;
   }

   //audioOn = true;

///////////////////
//   sprintf(info, "%s %d %d %ld %d %d", ipStr, audio->deviceId, (int)audio->sampleFmt, audio->samplingRate,
//          audio->channels, audio->framePerBuffer);
///////////////////
   char masterIp[SAGE_IP_LEN];
   getToken(data, masterIp);

   getToken(data, token);
   audioCfg.deviceNum = atoi(token);
   //std::cout << "---------> devce " << audioCfg.deviceNum << std::endl;

   getToken(data, token);
   audioCfg.sampleFmt = (sageSampleFmt) atoi(token);

   getToken(data, token);
   audioCfg.samplingRate = atoi(token);

   getToken(data, token);
   audioCfg.channels = atoi(token);

   getToken(data, token);
   audioCfg.framePerBuffer = atoi(token);

   audioCfg.audioMode = SAGE_AUDIO_PLAY;

   //std::cout << "SAGE Receiver : initialization message was successfully parsed" << std::endl;

   sageAudioModule::instance();
   audioModule = sageAudioModule::_instance;
	audioModule->setNodeID(nodeID);
   audioModule->init(audioCfg);

   if (initNetworks() < 0)
      return -1;

   syncClientObj = new sageSyncClient;

	//std::cout << "nodeID = " << nodeID << std::endl;
	int i;
	for(i=0; i <100; i++)
	{
   	if (syncClientObj->connectToServer(masterIp, syncPort, nodeID) >= 0)
			break;
   } 
	if(i == 100)
	{
		sage::printLog("[sageAudioManager::init] Fail to connect to sync master");
      return -1;
	} else 
		sage::printLog("[sageAudioManager::init] connected to sync master");

   sendMessage(SYNC_INIT_ARCV, nodeID);

   pthread_t thId;

   /*
   if (pthread_create(&thId, 0, syncCheckThread, (void*)this) != 0) {
      sage::printLog("sageAudioManager::init : can't create sync checking thread");
   }
   */

   if (pthread_create(&thId, 0, refreshThread, (void*)this) != 0) {
      sage::printLog("sageAudioManager: can't create UI event check thread");
   }
	initialized = true;

   return 0;
}

void* sageAudioManager::msgCheckThread(void *args)
{
   sageAudioManager *This = (sageAudioManager *)args;

   sageMessage *msg;
	int rcvSize = 0;

   while (!This->rcvEnd) {
      msg = new sageMessage;
		rcvSize = This->rcvMessageBlk(*msg);
      if (rcvSize > 0 && !This->rcvEnd) {
         //std::cout << "----> message arrive" << std::endl;
         This->eventQueue->sendEvent(EVENT_NEW_MESSAGE, 0, (void *)msg);
      } else if(rcvSize < 0)
		{
			This->shutdownApp(-1);
			This->rcvEnd = true;
			break;
		}
   }

   sage::printLog("sageAudioManager::msgCheckThread : exit");
   pthread_exit(NULL);
   return NULL;
}

void* sageAudioManager::syncCheckThread(void *args)
{
   sageAudioManager *This = (sageAudioManager *)args;

   while (!This->rcvEnd) {
      sageEvent *syncEvent = new sageEvent;
      syncEvent->eventType = EVENT_SYNC_MESSAGE;
      char *syncMsg = syncEvent->eventMsg;
      if (This->syncClientObj->waitForSync(syncMsg) == 0) {
         //std::cout << "rcv sync " << syncEvent->eventMsg << std::endl;
         This->eventQueue->sendEvent(syncEvent);
      }
   }

   sage::printLog("sageAudioManager::syncCheckThread : exit");
   pthread_exit(NULL);
   return NULL;
}

void* sageAudioManager::perfReportThread(void *args)
{
   sageAudioManager *This = (sageAudioManager *)args;

   while (!This->rcvEnd) {
      This->perfReport();
      // if (This->shared->context && !This->rcvEnd) This->shared->context->checkEvent();
      sage::usleep(100000);
   }

   sage::printLog("sageAudioManager::perfReportThread : exit");
   pthread_exit(NULL);
   return NULL;
}

void* sageAudioManager::refreshThread(void *args)
{
   sageAudioManager *This = (sageAudioManager *)args;

   while (!This->rcvRefreshEnd) {
      This->eventQueue->sendEvent(EVENT_REFRESH_SCREEN);
      sage::usleep(16666);  // 1/60fps = 0.016666sec/frame
   }

   sage::printLog("sageAudioManager::refreshThread : exit");
   pthread_exit(NULL);
   return NULL;
}

int sageAudioManager::initNetworks()
{
   sage::printLog("sageAudioManager::initNetworks() : initializing network objects....");

   tcpObj = new sageTcpModule;
   if (tcpObj->init(SAGE_ARCV, streamPort, nwCfg) == 1) {
      sage::printLog("sageAudioManager::initNetworks() : sageAudioManager is already running");
      return -1;
   }
   sage::printLog("sageAudioManager::initNetworks() : TCP has initialized successfully");

   udpObj = new sageUdpModule;
   udpObj->init(SAGE_ARCV, streamPort+(int)SAGE_UDP, nwCfg);
   sage::printLog("sageAudioManager::initNetworks() : UDP has initialized successfully");


   pthread_t thId;
   nwCheckThreadParam *param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = tcpObj;

   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("sageAudioManager::initNetwork : can't create network checking thread");
         return -1;
   }

   param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = udpObj;
   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("sageAudioManager::initNetwork : can't create network checking thread");
         return -1;
   }

   return 0;
}

void* sageAudioManager::nwCheckThread(void *args)
{
   nwCheckThreadParam *param = (nwCheckThreadParam *)args;
   streamProtocol *nwObj = (streamProtocol *)param->nwObj;
   sageAudioManager *This = (sageAudioManager *)param->This;

   int senderID = -1;
   char regMsg[SAGE_EVENT_SIZE];

   if (nwObj) {
      while (!This->rcvEnd) {
         senderID = nwObj->checkConnections(regMsg);
         if (senderID >= 0 && !This->rcvEnd) {
            This->eventQueue->sendEvent(EVENT_NEW_CONNECTION, regMsg, (void *)nwObj);
         }
      }
   }

   sage::printLog("sageAudioManager::nwCheckThread : exit");
   pthread_exit(NULL);
   return NULL;
}

int sageAudioManager::initStreams(char *msg, streamProtocol *nwObj)
{
   int senderID, instID, sailNodeNum, streamType, blockSize, frameRate;
   int syncType, keyframe;
   sscanf(msg, "%d %d %d %d %d %d %d %d %d %d %d %d",
					&senderID, &streamType, &instID, &sailNodeNum, &blockSize,
					&syncType, (int*) &audioCfg.sampleFmt, &audioCfg.samplingRate,
					&audioCfg.channels, &audioCfg.framePerBuffer, &frameRate, &keyframe);

	std::cout << "[sageAudioManager::initStreams] got init stream" << std::endl;
	bool instExist = false;
	for (int i=0; i<receiverList.size(); i++) {
		if (!receiverList[i])
			continue;
		if (receiverList[i]->getInstID() == instID) {
			instExist = true;
			receiverList[i]->addStream(senderID);
			std::cout << "[sageAudioManager::initStreams] existing stream " << instID << std::endl;
			return 0;
		}
	}

	if (instExist == false) {

      sageAudioCircBuf *buffer = audioModule->createObject(instID, &audioCfg);

      if(buffer != NULL) {
         buffer->setInstID(instID);
         if(streamType != SAGE_BLOCK_NO_SYNC)
         {
            buffer->connectSyncClient(syncClientObj);
         }

         sageAudioReceiver *recv = new sageAudioReceiver(msg, eventQueue, nwObj, buffer, audioModule->getSampleFmt());
         recv->addStream(senderID);
			receiverList.push_back(recv);
         std::cout << "[sageAudioManager::initStreams] inst init " << instID << std::endl;
      }

   }

   return 0;
}

sageAudioReceiver* sageAudioManager::findApp(int id, int& index)
{
	sageAudioReceiver* temp_app= NULL;
	std::vector<sageAudioReceiver*>::iterator iter;
	index =0;
	for(iter = receiverList.begin(); iter != receiverList.end(); iter++, index++)
	{
		if ((*iter)->instID == id)
		{
			temp_app =(sageAudioReceiver*) *iter;
			break;
		}
	}
	return temp_app;
}

int sageAudioManager::shutdownApp(int instID)
{
   //sage::printLog("sageAudioManager is shutting down an application");
	//if (instID < 0 || instID >= receiverList.size())
	//	return -1;

	int index;
   sageAudioReceiver *receiver = findApp(instID, index);
	if(receiver != NULL) {
		delete receiver;
      audioModule->deleteObject(instID);
		receiver = NULL;
		receiverList.erase(receiverList.begin() + index);
		std::cout << "[sageAudioManager::shutdownApp] " << instID << " is shutting down" << std::endl;
   }

   return 0;
}

int sageAudioManager::parseEvent(sageEvent *event)
{
   if (!event) {
      sage::printLog("sageAudioManager::parseEvent : event object is Null");
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
         processSync((char *)event->eventMsg);
         break;
      }
   }

   delete event;

   return 0;
}

int sageAudioManager::parseMessage(sageMessage *msg)
{
   if (!msg) {
      sage::printLog("sageAudioManager::parseMessage : message is NULL");
      return -1;
   }

	//std::cout << msg->getCode() << " parse mesage : " << (char *)msg->getData() << std::endl;
   switch (msg->getCode()) {
      case ARCV_AUDIO_INIT : {
			if(initialized == true) break;
         if (init((char *)msg->getData()) < 0)
            rcvEnd = true;
         break;
      }
      case ARCV_WINDOW_INIT : {
			// width, heght, dim_x, dim_y
   		char token[TOKEN_LEN];
			char* data = (char *)msg->getData();

   		getToken(data, token);
			int width = atoi(token);
   		getToken(data, token);
			int height = atoi(token);
   		getToken(data, token);
			int dim_x = atoi(token);
   		getToken(data, token);
			int dim_y = atoi(token);
			audioModule->setTileInfo(width, height, dim_x, dim_y);
			break;
		}
      case ARCV_WINDOW : {
			//std::cout << "parse mesage : " << (char *)msg->getData() << std::endl;
			// id, x, y, width, height
   		char token[TOKEN_LEN];
			char* data = (char *)msg->getData();

   		getToken(data, token);
			int id = atoi(token);
   		getToken(data, token);
			int x = atoi(token);
   		getToken(data, token);
			int y = atoi(token);
   		getToken(data, token);
			int width = atoi(token);
   		getToken(data, token);
			int height = atoi(token);
   		getToken(data, token);
			int zvalue = atoi(token);

			audioModule->changeWindow(id, x, y, width, height, zvalue);
   		//getToken(data, token);
			break;
		}
		case ARCV_WINDOW_DEPTH : {
			//std::cout << "depth mesage : " << (char *)msg->getData() << std::endl;

			// number of apps, inst id, depth, ........
   		//char token[TOKEN_LEN];
			//char* data = (char *)msg->getData();

			break;
		}
      case SHUTDOWN_RECEIVERS : {
         shutdownApp(-1);
         if(audioModule) {
            delete audioModule;
            audioModule = NULL;
         }
         if(syncClientObj) {
            delete syncClientObj;
            syncClientObj = NULL;
         }
         rcvEnd = true;
         break;
      }
      case ARCV_SHUTDOWN_APP : {
         //std::cout << "message: " <<  msg << std::endl;
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

   }

   msg->destroy();
   delete msg;

   return 0;
}

int sageAudioManager::processSync(char *msg)
{
   int groupID, syncFrame;
   sscanf(msg, "%d %d", &groupID, &syncFrame);

   sageAudioReceiver *recv;

   for (int i=0; i<receiverList.size(); i++) {
      recv = receiverList[i];

      if (!recv)
         continue;

      if (recv->getInstID() == groupID) {
         recv->processSync(syncFrame);
         break;
      }
   }

   return 0;
}

void sageAudioManager::mainLoop()
{
   sageAudioReceiver *recv;

   while(!rcvEnd) {
      //std::cout << "---->wait a event" << std::endl;
      sageEvent *newEvent = eventQueue->getEvent();
      //std::cout << "----->get the event " << newEvent->eventType << std::endl;
      parseEvent(newEvent);

   	for (int i=0; i< receiverList.size(); i++) {
      	recv = receiverList[i];
      	if (!recv) continue;
      	if (recv->isEnd() == true)
			{
      		std::cout << "shutdown app requested" << std::endl;
				shutdownApp(recv->getInstID());
			}
		}

   }
	rcvRefreshEnd = true;
	sage::usleep(16666);  // 1/60fps = 0.016666sec/frame = refresh rate...

	std::cout << "[sageAudioManager:mainLoop] Terminated" << std::endl;
}

int sageAudioManager::perfReport()
{
   for (int i=0; i<receiverList.size(); i++) {
      if (receiverList[i]) {
         char *frameStr = NULL;
         char *bandStr = NULL;
         receiverList[i]->evalPerformance(&frameStr, &bandStr);
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

int sageAudioManager::startPerformanceReport(sageMessage *msg)
{
   char *perfStr = (char *)msg->getData();
   int instID, rate;
   sscanf(perfStr, "%d %d", &instID, &rate);

   //std::cout << "start perf report " << rate << std::endl;

   for (int i=0; i<receiverList.size(); i++) {
      if (!receiverList[i])
         continue;

      if (receiverList[i]->getInstID() == instID) {
         receiverList[i]->setReportRate(rate);
         receiverList[i]->resetTimer();
         receiverList[i]->resetBandWidth();
         receiverList[i]->resetFrame();
         break;
      }
   }
   return 0;
}

int sageAudioManager::stopPerformanceReport(sageMessage *msg)
{
   int instID = atoi((char *)msg->getData());

   for (int i=0; i<receiverList.size(); i++) {
      if (!receiverList[i])
         continue;

      if (receiverList[i]->getInstID() == instID) {
         receiverList[i]->setReportRate(0);
         break;
      }
   }

   return 0;
}


int main(int argc, char **argv)
{
#ifdef WIN32
   // Initialize Winsock
   WSADATA wsaData;
   WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
   sage::initUtil();

   sageAudioManager manager(argc, argv);
   manager.mainLoop();

   return 0;
}

