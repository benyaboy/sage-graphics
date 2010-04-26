/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageBridge.cpp - the SAGE component to distribute pixels to multiple
 *         SAGE displays
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

#include "sageVersion.h"
#include "sageBridge.h"
#include "sageAudioBridge.h"
#include "messageInterface.h"
#include "sageTcpModule.h"
#include "sageUdpModule.h"
#include "sageSharedData.h"
#include "appInstance.h"
#include "sageSync.h"
#include "sageBlockQueue.h"
#include "sageEvent.h"

sageBridge::~sageBridge()
{
   if (shared)
      delete shared;
      
   if (msgInf) {
      msgInf->shutdown();
      delete msgInf;
   }   
}

sageBridge::sageBridge(int argc, char **argv) : syncPort(0), syncGroupID(0), audioPort(44000),
      allocPolicy(ALLOC_SINGLE_NODE)
{
	nwCfg = new sageNwConfig;
	/*
	for (int i=0; i<MAX_INST_NUM; i++)
		appInstList[i] = NULL;
		*/
	appInstList.clear();
	instNum = 0;   
	
	if (argc < 2) {  // master mode with default configuration file
		initMaster("sageBridge.conf");
	}
	else if (strcmp(argv[1], "slave") != 0) {  // master mode with user configuration file
		initMaster(argv[1]);
	}   
	else {  // slave mode
		if (argc < 5) {
			sage::printLog("sageBridge::sageBridge() : more arguments are needed for SAGE bridge slaves" );
			exit(0);
		}
		
		shared = new bridgeSharedData;
		master = false;
		strcpy(masterIP, argv[2]);
		
		msgPort = atoi(argv[3]);
		
		msgInfConfig conf;
		conf.master = false;
		strcpy(conf.serverIP, masterIP);
		conf.serverPort = msgPort;
		
		msgInf = new messageInterface();
		msgInf->init(conf);
		msgInf->msgToServer(0, BRIDGE_REG_NODE, argv[4]);
		
		shared->nodeID = atoi(argv[4]);
		sage::printLog("sageBridge : start slave node %d", shared->nodeID);
		
		tcpObj = NULL;
		udpObj = NULL;
		bridgeEnd = false;   
		
		pthread_t thId;
		
		if (pthread_create(&thId, 0, msgCheckThread, (void*)this) != 0) {
			sage::printLog("sageBridge::initSlave : can't create message checking thread");
		}
		
		if (pthread_create(&thId, 0, perfReportThread, (void*)this) != 0) {
			sage::printLog("sageBrdige::initSlave : can't create performance report thread");
		}
	}
}   

int sageBridge::initMaster(const char *cFile)
{
	shared = new bridgeSharedData;
	shared->nodeID = 0;
	master = true;
	bridgeEnd = false;
	
	data_path path;
	std::string found = path.get_file(cFile);
	if (found.empty()) {
		sage::printLog("sageBridge: cannot find the file [%s]", cFile);
		return -1;
	}
	const char *bridgeConfigFile = found.c_str();
	sage::printLog("sageBridge: SAGE version [%s]", SAGE_VERSION);
	sage::printLog("sageBridge: using [%s] configuration file", bridgeConfigFile);
	
	FILE *fileBridgeConf = fopen(bridgeConfigFile, "r");
	
	if (!fileBridgeConf) {
		sage::printLog("sageBridge: fail to open SAGE Bridge config file [%s]\n",bridgeConfigFile);
		return -1;
	}
	
	char token[TOKEN_LEN];
	int tokenIdx = getToken(fileBridgeConf, token);
	
	while(tokenIdx != EOF) {
		if (strcmp(token, "masterIP") == 0) {
			getToken(fileBridgeConf, masterIP);
		}
		else if (strcmp(token, "slaveList") == 0) {   
			getToken(fileBridgeConf, token);
			slaveNum = atoi(token);
			shared->nodeNum = slaveNum + 1;
			for (int i=0; i<slaveNum; i++)
				getToken(fileBridgeConf, slaveIPs[i]);
		}
		else if (strcmp(token, "streamPort") == 0) {   
			getToken(fileBridgeConf, token);
			streamPort = atoi(token);
		}
		else if (strcmp(token, "msgPort") == 0) {   
			getToken(fileBridgeConf, token);
			msgPort = atoi(token);
		}
		else if (strcmp(token, "syncPort") == 0) {
			getToken(fileBridgeConf, token);
			syncPort = atoi(token);
		}
		else if (strcmp(token, "audioPort") == 0) {
			getToken(fileBridgeConf, token);
			audioPort = atoi(token);
		}
		else if (strcmp(token, "rcvNwBufSize") == 0) {
			getToken(fileBridgeConf, token);
			nwCfg->rcvBufSize = getnumber(token); //atoi(token);
		}
		else if (strcmp(token, "sendNwBufSize") == 0) {
			getToken(fileBridgeConf, token);
			nwCfg->sendBufSize = getnumber(token); //atoi(token);
		}
		else if (strcmp(token, "MTU") == 0) {
			getToken(fileBridgeConf, token);
			nwCfg->mtuSize = atoi(token);
		}
		else if (strcmp(token, "maxBandWidth") == 0) {
			getToken(fileBridgeConf, token);
			nwCfg->maxBandWidth = atoi(token);
		}
		else if (strcmp(token, "maxCheckInterval") == 0) {
			getToken(fileBridgeConf, token);
			nwCfg->maxCheckInterval = atoi(token);
		}
		else if (strcmp(token, "flowWindowSize") == 0) {
			getToken(fileBridgeConf, token);
			nwCfg->flowWindow = atoi(token);
		}
		else if (strcmp(token, "memSize") == 0) {
			getToken(fileBridgeConf, token);
			shared->bufSize = atoi(token) * 1024*1024;
		}
		else if (strcmp(token, "nodeAllocation") == 0) {
			getToken(fileBridgeConf, token);
			if (strcmp(token, "single") == 0) 
				allocPolicy = ALLOC_SINGLE_NODE;
			else if (strcmp(token, "balanced") == 0)
				allocPolicy = ALLOC_LOAD_BALANCING;
		}
		else if (strcmp(token, "frameDrop") == 0) {
			getToken(fileBridgeConf, token);
			if (strcmp(token, "true") == 0)
				shared->frameDrop = true;
			else
				shared->frameDrop = false;
		}
		else if (strcmp(token, "nwProtocol") == 0) {
			getToken(fileBridgeConf, token);
			sage::toupper(token);
			if (strcmp(token, "TCP") == 0) 
				shared->protocol = SAGE_TCP;
			else if (strcmp(token, "UDP") == 0)
				shared->protocol = SAGE_UDP;   
		}
		
		tokenIdx = getToken(fileBridgeConf, token);
	}
	
	msgInfConfig conf;
	conf.master = true;
	conf.serverPort = msgPort;
	
	msgInf = new messageInterface;
	msgInf->init(conf);
	
	syncServerObj = new sageSyncServer;
	
	if (syncServerObj->init(syncPort) < 0) {
		sage::printLog("SAGE Bridge : Error init'ing the sync server object" );
		bridgeEnd = true;   
		return -1;
	}
	
	pthread_t thId;
	
	if (pthread_create(&thId, 0, msgCheckThread, (void*)this) != 0) {
		sage::printLog("sageBridge::initMaster : can't create message checking thread");
	}
	
	if (pthread_create(&thId, 0, perfReportThread, (void*)this) != 0) {
		sage::printLog("sageBrdige::initMaster : can't create performance report thread");
	}
	
	shared->syncClientObj = new sageSyncClient;
	
	if (shared->syncClientObj->connectToServer(strdup("127.0.0.1"), syncPort) < 0) {
		sage::printLog("SAGE Bridge : Fail to connect to sync master" );
		return -1;
	}
	
	launchSlaves();
	initNetworks();
	
	//audioBridge = new sageAudioBridge(masterIP, audioPort, msgInf, shared);
	
	return 0;
}

int sageBridge::launchSlaves()
{
   char *sageDir = getenv("SAGE_DIRECTORY");

   if (!sageDir) {
      sage::printLog("sageBridge : cannot find the environment variable SAGE_DIRECTORY" );
      return -1;
   }

   for (int i=0; i<slaveNum; i++) {
      char command[TOKEN_LEN];
      sprintf(command, "%s/bin/sageBridge slave %s %d %d", sageDir, masterIP, msgPort, i+1); 
      execRemBin(slaveIPs[i], command);
   }
   
   return 0;
}

int sageBridge::initSlave(char *data)
{
   sage::printLog("sageBridge : initialize slave %d", shared->nodeID);
   
   sscanf(data, "%d %d %d %d %d %d %d %d %d %d", &nwCfg->rcvBufSize, &nwCfg->sendBufSize,
      &nwCfg->mtuSize, (int *)&nwCfg->maxBandWidth, &nwCfg->maxCheckInterval, &nwCfg->flowWindow,
      &syncPort, &streamPort, &shared->nodeNum, &shared->bufSize);

   std::cout << "slave init info " << data << std::endl;
   
   initNetworks();
   
   syncServerObj = NULL;
   shared->syncClientObj = new sageSyncClient;

   if (shared->syncClientObj->connectToServer(masterIP, syncPort) < 0) {
      sage::printLog("SAGE Bridge : Fail to connect to sync master");
      return -1;
   }

   return 0;
}

int sageBridge::initNetworks()
{
   nwCfg->blockSize = 9000;
   nwCfg->groupSize = 65536;
   sageUdpModule *sendObj = new sageUdpModule;
   sendObj->init(SAGE_SEND, 0, *nwCfg); // starts sageUdpModule::sendingThread()
   shared->sendObj = (streamProtocol *)sendObj;
   fprintf(stderr,"sageBridge::%s() : UDP sending object init-ed", __FUNCTION__);

   tcpObj = new sageTcpModule;
   if (tcpObj->init(SAGE_RCV, streamPort, *nwCfg) == 1) {
      sage::printLog("sageBridge : error in initializing TCP object");
      bridgeEnd = true;
      return -1;
   }
   sage::printLog("sageBridge::%s() : tcp network object was initialized.", __FUNCTION__ );
   
   udpObj = new sageUdpModule;
   if (udpObj->init(SAGE_RCV, streamPort+(int)SAGE_UDP, *nwCfg) == 1) {
      sage::printLog("sageBridge : error in initializing UDP object");
      bridgeEnd = true;
      return -1;
   }
   sage::printLog("sageBridge::%s() : udp network object was initialized.", __FUNCTION__ );

   pthread_t thId;
   nwCheckThreadParam *param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = tcpObj;
   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("sageBridge::initNetwork : can't create network checking thread");
         return -1;
   }
   
   param = new nwCheckThreadParam;
   param->This = this;
   param->nwObj = udpObj;
   if (pthread_create(&thId, 0, nwCheckThread, (void*)param) != 0) {
      sage::printLog("sageBridge::initNetwork : can't create network checking thread");
         return -1;
   }
   
   return 0;
}

// Thread Functions

void* sageBridge::msgCheckThread(void *args)
{
   sageBridge *This = (sageBridge *)args;
   
   sageMessage *msg;

   while (!This->bridgeEnd) {
      msg = new sageMessage;
      //std::cout << "waiting for message" << std::endl;
      int clientID = This->msgInf->readMsg(msg, 100000) - 1;
            
      if (clientID >= 0 && !This->bridgeEnd) {
         //std::cout << "read a message" << std::endl;
         This->shared->eventQueue->sendEvent(EVENT_NEW_MESSAGE, clientID, (void *)msg);
      }   
   }
   
   sage::printLog("sageBridge::msgCheckThread : exit");
   pthread_exit(NULL);
   return NULL;
}

void* sageBridge::perfReportThread(void *args)
{
   sageBridge *This = (sageBridge *)args;
   
   while (!This->bridgeEnd) {
      This->perfReport();
      sage::usleep(100000);
   }
   
   sage::printLog("sageBridge::perfReportThread : exit");
   pthread_exit(NULL);
   return NULL;
}

void* sageBridge::nwCheckThread(void *args)
{
   nwCheckThreadParam *param = (nwCheckThreadParam *)args;
   streamProtocol *nwObj = (streamProtocol *)param->nwObj;
   sageBridge *This = (sageBridge *)param->This;
   
   int senderID = -1;
   char regMsg[SAGE_EVENT_SIZE];
   
   if (nwObj) {
      while (!This->bridgeEnd) {
         senderID = nwObj->checkConnections(regMsg);
         if (!This->bridgeEnd) {
            if (senderID >= 0)
               This->shared->eventQueue->sendEvent(EVENT_NEW_CONNECTION, regMsg, (void *)nwObj);
            else
               break;   
         }
      }
   }

   sage::printLog("sageBridge::nwCheckThread : exit");   
   pthread_exit(NULL);
   return NULL;
}

int sageBridge::perfReport()
{
	appInstance* app_inst = NULL;
	std::vector<appInstance*>::iterator iter;
	for(iter = appInstList.begin(); iter != appInstList.end(); iter++)
	{
		app_inst = (appInstance*) *iter;
		if(!app_inst) continue;
		app_inst->sendPerformanceInfo();
	}

   return 0;
}

int sageBridge::findMinLoadNode()
{
   int nodeSel = 0;
   double minLoad = 0;

	appInstance* app_inst = NULL;
	std::vector<appInstance*>::iterator iter;   
	double loadSum = 0.0;
   
   for (int i=0; i<shared->nodeNum; i++) {
		loadSum = 0.0;
		for(iter = appInstList.begin(); iter != appInstList.end(); iter++) 
		{
			app_inst = (appInstance*) *iter;
			if(!app_inst) continue;
         if (app_inst->allocInfoList[0].nodeID == i)
            loadSum += app_inst->curBandWidth;
      }
      
      if (i == 0) {
         minLoad = loadSum;
      }
      else if (minLoad > loadSum) {
         nodeSel = i;
         minLoad = loadSum;
      }
      
      std::cout << "node " << i << " load " << loadSum << std::endl;      
   }

   return nodeSel;
}

int sageBridge::regApp(sageMessage &msg, int clientID)
{
   char *data = (char *)msg.getData();
   fprintf(stderr, "sageBridge::%s() : msg [%s], clientID %d\n", __FUNCTION__, data, clientID);
   
   if (master) {
      appInstance *inst = new appInstance(data, instNum, shared);
      
      inst->sailClient = clientID;   
      inst->waitNodes = slaveNum;
      
      if (allocPolicy == ALLOC_LOAD_BALANCING) {
         inst->allocateNodes(allocPolicy);
      }   
      else  {
         int selNode = findMinLoadNode();
         sage::printLog("sageBridge::%s() : allocate app to node %d", __FUNCTION__, selNode);
         inst->allocateNodes(allocPolicy, selNode);
      }   
      
      appInstList.push_back(inst);

      syncGroup *sGroup = NULL;
      char regStr[TOKEN_LEN];
      sprintf(regStr, "%s %d 0", data, inst->nodeNum);
      
      if (inst->nodeNum > 1) {
         sGroup = addSyncGroup();
         inst->firstSyncGroup = sGroup;

         if (sGroup)
            sprintf(regStr, "%s %d %d", data, inst->nodeNum, sGroup->getSyncID());
      }
      
      fprintf(stderr, "sageBridge::%s() : distributing BRIDGE_APP_REG message. regStr[%s] \n", __FUNCTION__, regStr);

      msgInf->distributeMessage(BRIDGE_APP_REG, instNum, regStr, slaveList, slaveNum);
      


      char sailInitMsg[TOKEN_LEN];
      sprintf(sailInitMsg, "%d %d %d %d", instNum , nwCfg->rcvBufSize, nwCfg->sendBufSize, nwCfg->mtuSize);
      fprintf(stderr, "sageBridge::%s() : sending SAIL_INIT_MSG [%s] \n", __FUNCTION__, sailInitMsg);
      msgInf->msgToClient(clientID, 0, SAIL_INIT_MSG, sailInitMsg);
      
      if (inst->waitNodes == 0) {
         connectApp(inst);
         inst->waitNodes = inst->nodeNum;
         sendStreamInfo(inst);
         if (inst->audioOn)
            audioBridge->startAudioStream(instNum, clientID);
      }
      
      instNum++;
   }
   else {
      std::cout << "register app on node " << shared->nodeID << std::endl;
      int instID = msg.getDest();   
      appInstance *inst = new appInstance(data, instID, shared);
		appInstList.push_back(inst);

      instNum = MAX(instID+1, instNum);
      msgInf->msgToServer(instID, BRIDGE_APP_INST_READY);
   }   
   return 0;   
}

int sageBridge::connectApp(appInstance *inst)
{
   int msgLen = 8 + SAGE_IP_LEN * inst->nodeNum;
   char *msgStr = new char[msgLen];
   
   sprintf(msgStr, "%d %d ", streamPort, inst->nodeNum);

   // list the ip addresses of all bridge nodes
   for (int i=0; i<inst->nodeNum; i++) {
      int nodeID = inst->allocInfoList[i].nodeID;
      char nodeStr[TOKEN_LEN];
      if (nodeID == 0)
         sprintf(nodeStr, "%s %d ", masterIP, nodeID);
      else   
         sprintf(nodeStr, "%s %d ", slaveIPs[nodeID-1], nodeID);
      strcat(msgStr, nodeStr);
   }
   
   sage::printLog("app-brige connection info : %s", msgStr);
   
   msgInf->msgToClient(inst->sailClient, 0, SAIL_CONNECT_TO_RCV, msgStr);

   return 0;
}

int sageBridge::sendStreamInfo(appInstance *inst)
{
   int msgLen = 4 + SAGE_IP_LEN*inst->nodeNum;
   char *msgStr = new char[msgLen];
      
   sprintf(msgStr, "%d ", inst->nodeNum);
   
   for (int i=0; i<inst->nodeNum; i++) {
      char str[TOKEN_LEN];
      sprintf(str, "%d %d ", inst->allocInfoList[i].blockID, inst->allocInfoList[i].nodeID);
      strcat(msgStr, str);
   }

   msgInf->msgToClient(inst->sailClient, 0, SAIL_INIT_STREAM, msgStr);
   sage::printLog("sageBridge : stream info sent to app");
   
   return 0;
}

appInstance* sageBridge::findApp(int id, int& index)
{
	index =0;
	appInstance* app_inst = NULL;
	std::vector<appInstance*>::iterator iter;   
	for(iter = appInstList.begin(); iter != appInstList.end(); iter++) 
	{
		if((*iter)->instID == id)
		{
			app_inst = (appInstance*) *iter;
			break;
		}
	}
	return app_inst;
}

int sageBridge::shutdownApp(int instID, bool fsmToApp)
{
	int index;
   appInstance *inst = findApp(instID, index);      
   
   if (inst) {
      if (master && !inst->isActive()) {
         int clientID = inst->sailClient;
   
         if (fsmToApp) {
            msgInf->msgToClient(clientID, 0, APP_QUIT);
         }
         
         if (audioBridge && inst->audioOn)
            audioBridge->shutdownDup(instID);
         
         std::cout << "send clear app inst message" << std::endl;
         msgInf->distributeMessage(CLEAR_APP_INSTANCE, instID, slaveList, slaveNum);
         appInstList.erase(appInstList.begin() +index);
         delete inst;
      }   
   }
   else {
      sage::printLog("sageBridge::shutdownApp : invalid app instance ID");
      return -1;
   }   
       
   return 0;
}

int sageBridge::forwardToSail(int instID, sageMessage &msg)
{
	int index;
   appInstance *inst = findApp(instID, index);      
   
   if (inst) {
      int clientID = inst->sailClient;
      msg.setClientID(clientID);
      msgInf->msgToClient(msg);         
   }   

   return 0;
}

int sageBridge::shutdownAllApps()
{
	appInstance* inst = NULL;
	std::vector<appInstance*>::iterator iter;   
	for(iter = appInstList.begin(); iter != appInstList.end(); iter++) 
	{
		inst = (appInstance*) *iter;
		if(!inst) continue;

		// ????????? 
      msgInf->msgToClient(inst->sailClient, 0, APP_QUIT);
      inst->shutdownAllStreams();
      delete inst;
   }
	appInstList.clear();
      
   return 0;
}

int sageBridge::initStreams(char *msg, streamProtocol *nwObj)
{
	int senderID, instID, frameRate, streamType;

	sscanf(msg, "%d %d %d %d", &senderID, &streamType, &frameRate, &instID);

	//std::cout << "sender " << senderID << " connected to node " << shared->nodeID << std::endl;
	fprintf(stderr,"sageBridge::%s() : msg [%s]\n", __FUNCTION__, msg);

	int index;
	appInstance *inst = findApp(instID, index);
	if (inst) {
		if (!inst->initialized)
			inst->init(msg, nwObj);

		//std::cout << "node " << shared->nodeID << " init instance" << std::endl;

		int streamNum = inst->addStream(senderID);
		//std::cout << "node " << shared->nodeID << " add stream" << std::endl;
	}
	else {
		sage::printLog("sageBridge::initStreams : invalid instance ID");
		return -1;
	}

	return 0;
}

syncGroup* sageBridge::addSyncGroup()
{
   if (!master)
      return NULL;
      
   std::cout << "add sync group : sync node num " << shared->nodeNum << std::endl;
      
   syncGroup *sGroup = NULL;
   
   if (syncServerObj) {
      sGroup = new syncGroup;
      sGroup->init(0, SAGE_ASAP_SYNC_HARD, syncGroupID, MAX_FRAME_RATE, shared->nodeNum);
      syncServerObj->addSyncGroup(sGroup);
      sGroup->blockSync();
      syncGroupID++;
   }
   else
      sage::printLog("sageBridge::addSyncGroup : syncServerObj is NULL");
      
   return sGroup;   
}

int sageBridge::connectToFSManager(appInstance *inst)
{
   return connectToFSManager(inst, inst->fsIP, inst->fsPort);
}

int sageBridge::connectToFSManager(appInstance *inst, char *ip, int port)
{
   int fsID = msgInf->connect(ip, port);
   int fsIdx = inst->fsList.size();
   inst->fsList.push_back(fsID);
   
   char msgStr[TOKEN_LEN];
   inst->fillAppInfo(msgStr);
   
   msgInf->msgToClient(fsID, 0, REG_APP, msgStr);
   
   return fsIdx;
}

appInstance* sageBridge::findAppInstance(int instID)
{
   //if (!appInstList[instID])
      //sage::printLog("sageBridge : can't find app instance" );
	int index;
	return findApp(instID, index);
         
   //return appInstList[instID];
}

appInstance* sageBridge::clientIDtoAppInstance(int clientID, int &orgIdx)
{
   for (int i=0; i<instNum; i++) {
      appInstance *inst = appInstList[i];
      if (inst) {
         int fsNum = inst->fsList.size();
         for (int j=0; j<fsNum; j++) {
            if (inst->fsList[j] == clientID) {
               orgIdx = j;
               return inst;   
            }   
         }
               
         if (inst->sailClient == clientID) {
            return inst;         
         }   
      }
   }
   
   return NULL;            
}

appInstance* sageBridge::forwardToAppinstance(sageMessage &msg, int clientID)
{
	appInstance* inst = NULL;
	std::vector<appInstance*>::iterator iter;   
   //for (int i=0; i<instNum; i++) {
	for(iter = appInstList.begin(); iter != appInstList.end(); iter++) 
	{
		inst = (appInstance*) *iter;
		if(!inst) continue;

		int fsNum = inst->fsList.size();
		for (int j=0; j<fsNum; j++) {
			if (inst->fsList[j] == clientID) {
				inst->parseMessage(msg, j);
				msg.setDest(inst->instID);
				msg.setAppCode(j);
				return inst;
			}   
		}
		if (inst->sailClient == clientID) {
			inst->parseMessage(msg, 0);
			msg.setDest(inst->instID);
			msg.setAppCode(0);
			return inst;
      }   
   }
   
   return NULL;
}

appInstance* sageBridge::delieverMessage(sageMessage &msg, int clientID)
{
   appInstance *inst = NULL;
   
   if (master)   {
      inst = forwardToAppinstance(msg, clientID);
      //if (!inst)
      //   sage::printLog("sageBridge : can't forward to an app instance : %d", msg.getCode());

      msgInf->distributeMessage(msg, slaveList, slaveNum);               
   }
   else {
      int instID = msg.getDest();
      //inst = findAppInstance(instID);
		int index;
      inst = findApp(instID, index);
      int fsIdx = msg.getAppCode();
      if (inst)
         inst->parseMessage(msg, fsIdx);
   }
   
   return inst;
}

int sageBridge::shareApp(char *msgData, int clientID) 
{
   if (master)   {
      int fsPort;
      char fsIP[SAGE_IP_LEN];
      sscanf(msgData, "%s %d", fsIP, &fsPort);
      //std::cout << "app share message : " << msgData << std::endl;

      int orgIdx = 0;
      appInstance *inst = clientIDtoAppInstance(clientID, orgIdx);
      
      if (!inst)
         return -1;
         
      int newIdx = connectToFSManager(inst, fsIP, fsPort);
      sage::printLog("sageBridge::%s() : connected to fsManager %s:%d", __FUNCTION__, fsIP, fsPort);
      
      syncGroup *sGroup = NULL;
      if (inst->nodeNum > 1)
         sGroup = addSyncGroup();
      
      int syncID = 0;
      if (sGroup) {
         syncID = sGroup->getSyncID();
      }
      
      //sage::printLog("added sync group %d for app instance %d", syncID, inst->instID);
      inst->addStreamer(newIdx, orgIdx, sGroup);
      sage::printLog("sageBridge::%s() : new streamer added for %s:%d", __FUNCTION__, fsIP, fsPort);
      
      char msgStr[TOKEN_LEN];
      sprintf(msgStr, "%d %d %d %d", inst->instID, orgIdx, newIdx, syncID);
      msgInf->distributeMessage(SAGE_APP_SHARE, 0, msgStr, slaveList, slaveNum);
      
      //std::cout << "pt4" << std::endl;
   }
   else {
      int instID, orgIdx, newIdx, syncID;
      sscanf(msgData, "%d %d %d %d", &instID, &orgIdx, &newIdx, &syncID);
		int index;
      appInstance *inst = findApp(instID, index);
      if (inst)
         inst->addStreamer(newIdx, orgIdx, NULL, syncID);
   }
   
   return 0;
}

int sageBridge::parseMessage(sageMessage &msg, int clientID)
{
   if (clientID >= 0){
      char *msgData;
      if (msg.getData())
         msgData = (char *)msg.getData();
      else
         msgData = (char *)strdup("\0");
            
      sageToken tokenBuf(msgData);
      char token[TOKEN_LEN];
   
      switch (msg.getCode()) {
         case BRIDGE_REG_NODE : {
            tokenBuf.getToken(token);
            int slaveID = atoi(token);
            // store client ID of a slave
            slaveList[slaveID-1] = clientID;

            char msgStr[TOKEN_LEN];
            sprintf(msgStr, "%d %d %d %d %d %d %d %d %d %d", nwCfg->rcvBufSize, nwCfg->sendBufSize, 
               nwCfg->mtuSize, (int)nwCfg->maxBandWidth, nwCfg->maxCheckInterval, nwCfg->flowWindow,
               syncPort, streamPort, shared->nodeNum, shared->bufSize);
            msgInf->msgToClient(clientID, 0, BRIDGE_SLAVE_INIT, msgStr);
            break;
         }
         
         case BRIDGE_SLAVE_INIT : {
            initSlave(msgData);
            break;
         }
         
         case BRIDGE_APP_REG : {
            regApp(msg, clientID);
            break;
         }
         
         case BRIDGE_SLAVE_READY : {
            int instID = msg.getDest();
				int index;
            appInstance *inst = findApp(instID, index);
   
            if (inst) {
               inst->waitNodes--;
               // make sure if app instances are ready on all slaves
               if (inst->waitNodes == 0) {
                  inst->waitNodes = slaveNum;
                  connectToFSManager(inst);
               }   
            }
            else {   
               sage::printLog("sageBridge::parseMessage : invalid instance ID");
            }

            break;
         }
            
         case BRIDGE_SHUTDOWN : {
            std::cout << "shuting down sage bridge...." << std::endl;
            if (master)
               msgInf->distributeMessage(msg, slaveList, slaveNum);
            
            shutdownAllApps();
            shared->eventQueue->sendEvent(EVENT_BRIDGE_SHUTDOWN);
            break;
         }         
      
         case BRIDGE_SLAVE_PERF : {
            int instID = msg.getDest();
				int index;
            appInstance *inst = findApp(instID, index);
            if (inst)
               inst->accumulateBandWidth(msgData);
            break;
         }
         
         case BRIDGE_APP_INST_READY : {
            int instID = msg.getDest();
			int index;
            appInstance *inst = findApp(instID, index);
   
            if (inst) {
               inst->waitNodes--;
               // make sure if app instances are ready on all slaves
               if (inst->waitNodes == 0) {
                  connectApp(inst);
                  sendStreamInfo(inst);
                  inst->waitNodes = inst->nodeNum;
                  if (inst->audioOn)
                     audioBridge->startAudioStream(instID, inst->sailClient);
               }
            }
            else {   
               sage::printLog("sageBridge::parseMessage : invalid instance ID");
            }

            break;
         }
         
         case SAGE_APP_SHARE : {   
            shareApp(msgData, clientID);
            break;
         }
         
         case APP_QUIT : {
            appInstance *inst = delieverMessage(msg, clientID);
            if (inst) {
               if (audioBridge && inst->audioOn) {
                  int fsIdx;
                  clientIDtoAppInstance(clientID, fsIdx);
                  audioBridge->shutdownStreams(inst->instID, fsIdx);   
               }
               
               shutdownApp(inst->instID, true);
            }
               
            break;   
         }
         
         case SAIL_CONNECT_TO_ARCV : {
            int fsIdx;
            appInstance *inst = clientIDtoAppInstance(clientID, fsIdx);
            if (inst && audioBridge) {
               audioBridge->duplicate(inst->instID, msgData, fsIdx);
				}
            break;
         }
         
         case NOTIFY_APP_SHUTDOWN : {
            appInstance *inst = delieverMessage(msg, clientID);
            if (inst)
               shutdownApp(inst->instID, false);
            break;
         }
         
         case CLEAR_APP_INSTANCE : {
            int instID = msg.getDest();
				int index;
            appInstance *inst = findApp(instID, index);
            appInstList[instID] = NULL;
            if (inst)
               delete inst;
            std::cout << "app instance cleared at node " << shared->nodeID << std::endl;
            break;
         }   
            
         case SAIL_SEND_TIME_BLOCK : {
            appInstance *inst = delieverMessage(msg, clientID);
            if (inst)
               msgInf->msgToClient(inst->sailClient, 0, SAIL_SEND_TIME_BLOCK);               
            break;
         }
         
         case SAIL_INIT_STREAM : {
            delieverMessage(msg, clientID);
            break;
         }   
         
         default : {
            appInstance *inst = delieverMessage(msg, clientID);
            break;
         }   
      }
      
      // forward app events
      if (APP_MESSAGE <= msg.getCode() && msg.getCode() < APP_MESSAGE+1000) {
         appInstance *inst = delieverMessage(msg, clientID);
         if (inst)
            forwardToSail(inst->instID, msg);
      }
   }   
   
   msg.destroy();
      
   return 0;
}

int sageBridge::parseEvent(sageEvent *event)
{   
   if (!event) {
      sage::printLog("sageBridge::parseEvent : event object is Null");
      return -1;
   }
   
   switch (event->eventType) {
      case EVENT_NEW_CONNECTION : {
         initStreams(event->eventMsg, (streamProtocol *)event->param);
         break;
      }
      
      case EVENT_NEW_MESSAGE : {
         int clientID = atoi(event->eventMsg);
         sageMessage *msg = (sageMessage *)event->param;
         parseMessage(*msg, clientID);
         delete msg;
         break;
      }
      
      case EVENT_APP_CONNECTED : {
         int instID;
         sscanf(event->eventMsg, "%d", &instID);

         appInstance *inst = appInstList[instID];      

         if (inst) {
            if (master) {
               inst->waitNodes--;
               if (inst->waitNodes == 0) {
                  connectToFSManager(inst);
                  inst->waitNodes = slaveNum;
               }   
            }
            else
               msgInf->msgToServer(instID, BRIDGE_SLAVE_READY);
         }      
         break;
      }
      
      case EVENT_SLAVE_PERF_INFO : {
         int instID;
         sscanf(event->eventMsg, "%d", &instID);
         char *msgStr = sage::tokenSeek(event->eventMsg, 1);
         msgInf->msgToServer(instID, BRIDGE_SLAVE_PERF, msgStr);
         break;
      }   
      
      case EVENT_MASTER_PERF_INFO : {
         int fsClientID;
         sscanf(event->eventMsg, "%d", &fsClientID);
         char *msgStr = sage::tokenSeek(event->eventMsg, 1);
         msgInf->msgToClient(fsClientID, 0, DISP_SAIL_PERF_RPT, msgStr);
         //std::cout << "bridge perf " << msgStr << std::endl;
         break;
      }
      
      case EVENT_APP_SHUTDOWN : {
         int fsClientID;
         sscanf(event->eventMsg, "%d", &fsClientID);
         msgInf->msgToClient(fsClientID, 0, NOTIFY_APP_SHUTDOWN);
         break;
      }
      
      case EVENT_BRIDGE_SHUTDOWN : {
         bridgeEnd = true;
         delete tcpObj;
         delete udpObj;
         if (syncServerObj)
            delete syncServerObj;
            
         break;
      }
      
      case EVENT_AUDIO_CONNECTION : {
         if (audioBridge)
            audioBridge->initStreams(event->eventMsg, (streamProtocol *)event->param);
         break;
      }
   }

   delete event;
   
   return 0;
}

void sageBridge::mainLoop()
{
   while(!bridgeEnd) {
      sageEvent *newEvent = shared->eventQueue->getEvent();
      //std::cout << "get the event " << newEvent->eventType << std::endl;
      parseEvent(newEvent);
   }
}

int main(int argc, char **argv)
{
#ifdef WIN32
   sage::win32Init();
#endif
   sage::initUtil();
   
   sageBridge bridge(argc, argv);
   bridge.mainLoop();
   _exit(0);
}   
