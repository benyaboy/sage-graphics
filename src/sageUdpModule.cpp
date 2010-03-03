/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module   :  sageUdpModule.cpp
 * Author   :   Byungil Jeong, Rajvikram Singh
 * Description:   Code file for the UDP network stream of SAGE
 * Notes   :   The class uses UDP/TCP sockets with the options optimized for graphics streaming
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
***************************************************************************************************/

#include "sageUdpModule.h"
#include "sageBlock.h"
#include "sageBlockPool.h"

streamFlowData::streamFlowData(sageCircBuf *buf) : frameRate(60.0), configID(0), nextFrameSize(3072),
      frameSize(3072), winIdx(-1), returnPlace(NULL), nextWindowTime(0.0), startTime(0.0),
      totalSentSize(0), blockSize(0), active(true), closed(false), firstRound(true)
{
   blockBuf = buf;
}

streamFlowData::~streamFlowData()
{
   if (blockBuf)
      delete blockBuf;
}

void streamFlowData::insertWindow(double sTime, int dataSize)
{
   if (firstRound && winIdx+1 == FLOW_WINDOW_NUM)
      firstRound = false;

   winIdx = (winIdx+1)%FLOW_WINDOW_NUM;
   streamWindow[winIdx].startTime = sTime;
   startTime = sTime;

   double dataRate = frameRate*frameSize;
   assert(dataRate > 0);
   streamWindow[winIdx].windowInterval = dataSize/dataRate*1000000.0;

   int totalInterval = 0;
   for (int i=0; i<FLOW_WINDOW_NUM; i++)
      totalInterval += streamWindow[winIdx].windowInterval;

   int oldest = 0;
   if (!firstRound)
      oldest = (winIdx+1)%FLOW_WINDOW_NUM;

   nextWindowTime = streamWindow[oldest].startTime + totalInterval;
}

sageUdpModule::sageUdpModule() : closeFlag(false), notStarted(true), waitData(true)
{
#ifdef WIN32
   // Initialize Winsock
   WSADATA wsaData;
   WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

   protocol = SAGE_UDP;
   pthread_mutex_init(&connectionLock, NULL);
   pthread_mutex_unlock(&connectionLock);
   pthread_cond_init(&streamStart, NULL);
   pthread_cond_init(&newData, NULL);

}//End of sageUdp:sageUdp

int sageUdpModule::init(sageStreamMode m, int p, sageNwConfig &c)
{
   streamProtocol::init(m, p, c);

   // create the actual network objects
   switch(sMode)   {
      case SAGE_RCV:
      case SAGE_ARCV:
      case SAGE_BRIDGE: {  // prepare a server socket
         // create the sockets
         if((serverSockFd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
            sage::printLog("sageUdpModule::init(): Creating TCP socket failed");
            return -1;
         }

         setSockOpts(serverSockFd, true);

         struct sockaddr_in localAddr;
         // bind to port
         memset(&localAddr, 0, sizeof(localAddr));
         localAddr.sin_family = AF_INET;
         localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
         localAddr.sin_port = htons(rcvPort);

         if (bind(serverSockFd, (struct sockaddr *)&localAddr, sizeof(struct sockaddr_in)) != 0) {
            sage::printLog("sageUdpModule::init() : error in binding socket");
            return -1;
         }

         // put in listen mode
         ::listen(serverSockFd, 5);

         break;
      }

      case SAGE_SEND: {
         if (config.blockSize > 0 && config.groupSize > 0) {
            if (pthread_create(&thId, 0, sendingThread, (void*)this) != 0) {
               sage::printLog("sageUdpModule::init() : can't create sendingThread");
            }
         }
         break;
      }

      default:
         return -1;

   } //End of switch(opMode)

   return 0;
}//End of sageUdpModule::init()

void sageUdpModule::setupBlockPool(sageBlockPool *pool, int id)
{
   int flowNum = flowList.size()-1;

   if (id > flowNum) {
      sage::printLog("sageUdpModule::setupBlockPool() : invalid receiver ID");
      return;
   }
   else if (id == -1) {
      for (int i=0; i<flowList.size(); i++)
         flowList[i]->returnPlace = pool;
   }
   else
      flowList[id]->returnPlace = pool;
}

void sageUdpModule::setFrameSize(int id, int size)
{
   int flowNum = flowList.size()-1;

   if (id < 0 || id > flowNum) {
      sage::printLog("sageUdpModule::setFrameSize() : invalid receiver ID");
      return;
   }

   flowList[id]->nextFrameSize = size;
}

void sageUdpModule::setFrameRate(double rate, int id)
{
   int flowNum = flowList.size()-1;

   if (id > flowNum) {
      sage::printLog("sageUdpModule::setFrameRate() : invalid receiver ID");
      return;
   }
   else if (id == -1) {
      for (int i=0; i<flowList.size(); i++)
         flowList[i]->frameRate = rate;
   }
   else
      flowList[id]->frameRate = rate;
}

int sageUdpModule::checkConnections(char *msg, sageApiOption op)
{
   if(sMode == SAGE_SEND) {
      sage::printLog("sageUdpModule::checkConnection() cannot be used for senders");
      return -1;
   }

   if (op & SAGE_NON_BLOCKING)
      if (!sage::isDataReady(serverSockFd)) {
      return -1;
   }

   //accept client connections
   int addrLen;
   struct sockaddr_in clientAddr;
   addrLen = sizeof(clientAddr);

   int clientSockFd;
   if((clientSockFd = ::accept(serverSockFd, (struct sockaddr *)&clientAddr, (socklen_t*)&addrLen)) == -1){
      sage::printLog("sageUdpModule::checkConnections() - error in accepting control connection");
      return -1;
   }

   sendList.push_back(clientSockFd);

   // read registration message
   int idx = sendList.size()-1;

   char regMsg[REG_MSG_SIZE];
   int retVal = sage::recv(clientSockFd, (void *)regMsg, REG_MSG_SIZE);
   if (retVal == -1) {
      return -1;
   }
   else if (retVal == 0) {
      #ifdef WIN32
      closesocket(clientSockFd);
      #else
      shutdown(clientSockFd, SHUT_RDWR);
      #endif

      sendList[idx] = -1;
      return -1;
   }

   if (msg) {
      sprintf(msg, "%d %s", idx, regMsg);
   }
   else {
      sage::printLog("sageUdpModule::checkConnections() : registraion message buffer is NULL");
      return -1;
   }

   // creating UDP socket
   int udpSockFd;
/*
   if((udpSockFd = ::accept(serverSockFd, (struct sockaddr *)&clientAddr, (socklen_t*)&addrLen)) == -1){
      sage::printLog("sageUdpModule::checkConnections()");
      return -1;
   }
*/

   //std::cout << "sageUdpModule::checkConnections() : create UDP socket" << std::endl;
   if ((udpSockFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      sage::printLog("sageUdpModule::checkConnections(): Creating UDP socket failed");
      return -1;
   }

   setSockOpts(udpSockFd, false);

   struct sockaddr_in udpLocalAddr;
   memset(&udpLocalAddr, 0, sizeof(udpLocalAddr));
   udpLocalAddr.sin_family = AF_INET;
   udpLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   udpLocalAddr.sin_port = htons(rcvPort+10);

   //std::cout << "sageUdpModule::checkConnections() : bind UDP socket" << std::endl;
   if (bind(udpSockFd, (struct sockaddr *)&udpLocalAddr, sizeof(struct sockaddr_in)) != 0) {
      sage::printLog("sageUdpModule::checkConnections() : Error binding UDP socket");
      return -1;
   }

   socklen_t nameLen = sizeof(udpLocalAddr);
   memset(&udpLocalAddr, 0, sizeof(udpLocalAddr));
   if (getsockname(udpSockFd, (struct sockaddr *)&udpLocalAddr, &nameLen) != 0) {
      sage::printLog("sageUdpModule::checkConnections() : Error getting the name of UDP socket");
      return -1;
   }

   int udpPort = (int)ntohs(udpLocalAddr.sin_port);
   char addrMsg[TOKEN_LEN];
   sprintf(addrMsg, "%d", udpPort);
   //std::cout << "sageUdpModule::checkConnections() : send UDP server port " << udpPort << std::endl;

   if (sage::send(clientSockFd, (void *)addrMsg, TOKEN_LEN) < 0) {
      sage::printLog("sageUdpModule::checkConnections()");
      return -1;
   }

   if (sage::recv(clientSockFd, (void *)addrMsg, TOKEN_LEN) <= 0) {
      sage::printLog("sageUdpModule::checkConnections()");
      return -1;
   }

   sscanf(addrMsg, "%d", &udpPort);
   clientAddr.sin_port = htons(udpPort);
   //std::cout << "sageUdpModule::checkConnections() : UDP client port is " << udpPort << std::endl;

   if (::connect(udpSockFd, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr)) == -1) {
      sage::printLog("sageUdpModule::checkConnections()");
      return -1;
   }
   //std::cout << "sageUdpModule::checkConnections() : connected back to UDP client " << std::endl;

   udpSendList.push_back(udpSockFd);
   if (idx != (udpSendList.size()-1)) {
      std::cerr << "sageUdpModule::checkConnections() - Error : socket list index mismatch" <<
         std::endl;
      return -1;
   }

   return idx;
}//End of sageUdpModule::listen()

int sageUdpModule::connect(char *ip, char *msg)
{
   if(sMode == SAGE_RCV) {
      std::cerr << "sageUdpModule::connect() cannot be used for receivers" << std::endl;
      return -1;
   }

   // create the sockets
   int clientSockFd;
   if((clientSockFd = socket (AF_INET, SOCK_STREAM, 0)) == -1)   {
      sage::printLog("sageUdpModule::connect(): Creating TCP socket failed");
      return -1;
   }

   setSockOpts(clientSockFd, true);

   // connect to the server
   struct sockaddr_in serverAddr;
   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_addr.s_addr = inet_addr(ip);
   serverAddr.sin_port = htons(rcvPort);

   if(::connect(clientSockFd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1) {
      sage::printLog("sageUdpModule::connect()");
      return -1;
   }

   rcvList.push_back(clientSockFd);
   //sendAck.push_back(false);

   //std::cout << "UDP reg send" << std::endl;

   if (msg) {
      if (sage::send(clientSockFd, msg, REG_MSG_SIZE) == -1) {
         sage::printLog("sageUdpModule::connect()");
         return -1;
      }
   }

   int idx = rcvList.size()-1;

   // creating UDP socket
   int udpSockFd;

/*
   if((udpSockFd = socket (AF_INET, SOCK_STREAM, 0)) == -1)   {
      sage::printLog("sageUdpModule::connect(): Creating TCP socket failed");
      return -1;
   }

   setSockOpts(udpSockFd);


   if(::connect(udpSockFd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1) {
      sage::printLog("sageUdpModule::connect()");
      return -1;
   }
*/

   //std::cout << "sageUdpModule::connect() : create UDP socket" << std::endl;
   if ((udpSockFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      sage::printLog("sageUdpModule::connect(): Creating UDP socket failed");
      return -1;
   }

   setSockOpts(udpSockFd);

   struct sockaddr_in udpLocalAddr;
   memset(&udpLocalAddr, 0, sizeof(udpLocalAddr));
   udpLocalAddr.sin_family = AF_INET;
   udpLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   udpLocalAddr.sin_port = htons(0);

   //std::cout << "sageUdpModule::connect() : bind UDP socket" << std::endl;
   if (bind(udpSockFd, (struct sockaddr *)&udpLocalAddr, sizeof(struct sockaddr_in)) != 0) {
      sage::printLog("sageUdpModule::connect() : Error binding UDP socket");
      return -1;
   }

   socklen_t nameLen = sizeof(udpLocalAddr);
   memset(&udpLocalAddr, 0, sizeof(udpLocalAddr));
   if (getsockname(udpSockFd, (struct sockaddr *)&udpLocalAddr, &nameLen) != 0) {
      sage::printLog("sageUdpModule::connect() : Error getting the name of UDP socket");
      return -1;
   }

   int udpPort;
   char addrMsg[TOKEN_LEN];

   //std::cout << "sageUdpModule::connect() : waiting for server..." << std::endl;
   if (sage::recv(clientSockFd, (void *)addrMsg, TOKEN_LEN) < 0) {
      sage::printLog("sageUdpModule::connect()");
      return -1;
   }

   sscanf(addrMsg, "%d", &udpPort);
   //std::cout << "sageUdpModule::connect() : UDP server port is " << udpPort << std::endl;

   serverAddr.sin_port = htons(udpPort);

   if (::connect(udpSockFd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1) {
      sage::printLog("sageUdpModule::connect()");
      return -1;
   }

   udpPort = (int)ntohs(udpLocalAddr.sin_port);
   sprintf(addrMsg, "%d", udpPort);
   //std::cout << "sageUdpModule::connect() : send UDP client port " << udpPort << std::endl;

   if (sage::send(clientSockFd, (void *)addrMsg, TOKEN_LEN) <= 0) {
      sage::printLog("sageUdpModule::connect() - error in sending UDP port");
      return -1;
   }

   udpRcvList.push_back(udpSockFd);
   if (idx != (udpRcvList.size()-1)) {
      sage::printLog("sageUdpModule::connect() - socket list index mismatch");
      return -1;
   }

   if (config.blockSize > 0 && config.groupSize > 0) {
      sageCircBuf *buf = new sageCircBuf(config.sendBufSize/config.blockSize);

      streamFlowData *flowData = new streamFlowData(buf);

      pthread_mutex_lock(&connectionLock);
      flowList.push_back(flowData);

      if (notStarted) {
         pthread_cond_signal(&streamStart);
         notStarted = false;
      }

      pthread_mutex_unlock(&connectionLock);
   }
	else {
		sage::printLog("sageUdpModule::connect() : invalid block size or group size");
		return -1;
	}

   std::cerr << "send buf " << config.sendBufSize << std::endl
         << "mtu " << config.mtuSize << std::endl
         << "blockSize " << config.blockSize << std::endl;

   return idx;
}//End of sageUdpModule::connect()

// send pixel block data packet by packet
int sageUdpModule::send(int id, sagePixelBlock *spb)
{
	int totalSendSize = 0;
	int blockDataSize = config.blockSize - BLOCK_HEADER_SIZE;
	int packetDataSize = config.mtuSize - BLOCK_HEADER_SIZE;
	int packetNum = ((blockDataSize-1)/packetDataSize + 1);

	for (int pidx = 0; pidx < packetNum; pidx++) {
		int sendSize;
		int dataPt = packetDataSize*pidx;
		int dataBufSize = MIN(blockDataSize - dataPt, packetDataSize);

		spb->updateHeader(pidx, spb->getConfigID());

		//std::cerr << "frameID " << spb->getFrameID() << " blockID " << spb->getID() << " pid " << pidx << std::endl;

#ifdef WIN32
		WSABUF iovs[2];
		iovs[0].buf = spb->getBuffer();
		iovs[0].len = BLOCK_HEADER_SIZE;
		iovs[1].buf = spb->getPixelBuffer() + dataPt;
		iovs[1].len = dataBufSize;

		DWORD WSAFlags = 0;
		::WSASend(sockFd, iovs, 2, (DWORD*)&sendSize,
				WSAFlags, NULL, NULL);
#else
		struct iovec iovs[2];
		iovs[0].iov_base = spb->getBuffer();
		iovs[0].iov_len = BLOCK_HEADER_SIZE;
		iovs[1].iov_base = spb->getPixelBuffer() + dataPt;
		iovs[1].iov_len = dataBufSize;

		struct msghdr blockMsgHdr;
		bzero((void *)&blockMsgHdr, sizeof(blockMsgHdr));

		blockMsgHdr.msg_iov = iovs;
		blockMsgHdr.msg_iovlen = 2;

		int udpSockFd = udpRcvList[id];
		sendSize = ::sendmsg(udpSockFd, &blockMsgHdr, 0);
#endif

		if (sendSize <= 0)
			return -1;
		else
			if (sendSize < (BLOCK_HEADER_SIZE + dataBufSize))
				sage::printLog("sageUdpModule::send() : a part of the message was not sent");

		totalSendSize += sendSize;
		if (spb->getFlag() == SAGE_UPDATE_BLOCK) {
			// a frame has sent !!!
			break;
		}
	}

	return totalSendSize;
}

int sageUdpModule::send(int id, sageBlock *sb, sageApiOption op)
{
   if (id < 0 || id > rcvList.size()-1) {
      sage::printLog("sageUdpModule::send() : invalid receiver ID");
      return -1;
   }

   if (!sb) {
      sage::printLog("sageUdpModule::send() : null sage block");
      return 1;
   }

   int dataSize;

   if (op & SAGE_CONTROL) {
      int clientSockFd = rcvList[id];
      if (op & SAGE_BACKWARD)
         clientSockFd = sendList[id];

      if ((dataSize = sage::send(clientSockFd, sb->getBuffer(), BLOCK_HEADER_SIZE)) < 0)
         return -1;
   }
   else {
      //std::cout << "block size " << sb->getBufSize() << "bytes" << std::endl;

      // send data using UDP channel
      int udpSockFd = udpRcvList[id];
      dataSize = sage::send(udpSockFd, sb->getBuffer(), sb->getBufSize());
      if (dataSize < 0)
         return -1;
      //std::cout << dataSize << " bytes sent" << std::endl;
   }

   return dataSize;
}//End of sageUdpModule::send()

int sageUdpModule::sendControl(int id, int frameID, int configID)
{
	if (id < 0 || id > rcvList.size()-1) {
      sage::printLog("sageUdpModule::send() : invalid receiver ID");
      return -1;
   }

   sagePixelBlock *cBlock = new sagePixelBlock(config.blockSize);
   cBlock->setFlag(SAGE_UPDATE_BLOCK);
	cBlock->setFrameID(frameID);
   cBlock->setConfigID(configID);
   cBlock->updateBufferHeader();

   flowList[id]->blockBuf->pushBack((sageBufEntry)cBlock, true);
   waitData = false;
   pthread_cond_signal(&newData);

   return GROUP_HEADER_SIZE;
}

int sageUdpModule::sendGrp(int id, sagePixelBlock *sb, int configID)
{
   if (id < 0 || id > rcvList.size()-1) {
      sage::printLog("sageUdpModule::send() : invalid receiver ID");
      return -1;
   }

   if (!sb) {
      sage::printLog("sageUdpModule::send() : null sage block");
      return 1;
   }

   sb->setConfigID(configID);

   flowList[id]->blockBuf->pushBack((sageBufEntry)sb, true);
      waitData = false;
      pthread_cond_signal(&newData);

   return sb->getBufSize();
}//End of sageUdpModule::sendGrp()

int sageUdpModule::flush(int id, int configID)
{
      return 0;
}//End of sageUdpModule::flush()

int sageUdpModule::sendLoop()
{
   double maxPriority = 0;
   double curTime = 0;
   sageTimer flowTimer;

   pthread_mutex_lock(&connectionLock);
   if (notStarted)
      pthread_cond_wait(&streamStart, &connectionLock);
   pthread_mutex_unlock(&connectionLock);

   while(!closeFlag) {
      pthread_mutex_lock(&connectionLock);
      int streamNum = flowList.size();
      pthread_mutex_unlock(&connectionLock);

      maxPriority = 0;
      int selectedStream = -1;

      curTime = flowTimer.getTimeUS();
      waitData = true;

      // select a stream to send
      for (int i=0; i<streamNum; i++) {
         if (flowList[i]->active) {
            if (flowList[i]->blockBuf->isEmpty())
               continue;

            double priority;
            if (curTime > 0)
               priority = flowList[i]->elapsedTime(curTime)*flowList[i]->blockBuf->getEntryNum();
            else
               priority = flowList[i]->blockBuf->getEntryNum();

            if (maxPriority < priority) {
               maxPriority = priority;
               selectedStream = i;
            }
         }
         else if (!flowList[i]->closed) {
            while (!flowList[i]->blockBuf->isEmpty()) {
               sagePixelBlock *pBlock = (sagePixelBlock *)flowList[i]->blockBuf->front(false);
               flowList[i]->blockBuf->next();

               if (pBlock) {
                  if (pBlock->getFlag() == SAGE_PIXEL_BLOCK)
                     flowList[i]->returnPlace->returnBlock(pBlock);
                  else
                     delete pBlock;
               }
            }

            flowList[i]->closed = true;
            delete flowList[i]->blockBuf;
         }
      }

      //std::cout << "selected stream " << selectedStream << std::endl;

      if (selectedStream == -1) {
    	  pthread_mutex_lock(&connectionLock);
    	  while (waitData)
    		  sage::condition_wait(&newData, &connectionLock, 10);
    	  pthread_mutex_unlock(&connectionLock);
      }
      else {
    	  curTime = flowTimer.getTimeUS();
    	  unsigned sentDataSize = 0;
    	  if (flowList[selectedStream]->pastTime(curTime) >= 0) {
    		  sagePixelBlock *block = (sagePixelBlock *)flowList[selectedStream]->blockBuf->front(false);
    		  flowList[selectedStream]->blockBuf->next();

    		  while (block) {
    			  if (block->getConfigID() > flowList[selectedStream]->configID) {
    				  flowList[selectedStream]->configID = block->getConfigID();
    				  flowList[selectedStream]->frameSize = flowList[selectedStream]->nextFrameSize;
    			  }

    			  int sentSize = send(selectedStream, block);
    			  if (block->getFlag() == SAGE_PIXEL_BLOCK)
    				  flowList[selectedStream]->returnPlace->returnBlock(block);
    			  else
    				  delete block;

    			  if (sentSize > 0) {
    				  sentDataSize += sentSize;
    				  if (sentDataSize > config.sendBufSize/config.flowWindow)
    					  break;
    			  }
    			  else
    				  flowList[selectedStream]->active = false;

    			  block = (sagePixelBlock*)flowList[selectedStream]->blockBuf->front(false);
    			  flowList[selectedStream]->blockBuf->next();
    		  }
    		  flowList[selectedStream]->insertWindow(curTime, sentDataSize);
    	  }
      }
   }

   return 0;
}

void* sageUdpModule::sendingThread(void *args)
{
   sageUdpModule *This = (sageUdpModule *)args;
   This->sendLoop();

   pthread_exit(NULL);
   return NULL;
}

int sageUdpModule::recv(int id, sageBlock *sb, sageApiOption op)
{
   if (id < 0 || id > sendList.size()-1) {
      sage::printLog("sageUdpModule::recv() : invalid sender ID");
      return -1;
   }

   int udpSockFd = udpSendList[id];

   if (op & SAGE_NON_BLOCKING) {
      if (!sage::isDataReady(udpSockFd)) {
         return 0;
      }
   }

   char *bufP = sb->getBuffer();
   if (!bufP) {
      sage::printLog("sageUdpModule::recv - block buffer is null");
      return -1;
   }

   int retVal = sage::recv(udpSockFd, (void *)bufP, sb->getBufSize());

   if (retVal < 0) {
      return -1;
   }
   else if (retVal == 0) {
      #ifdef WIN32
      closesocket(sendList[id]);
      #else
      shutdown(sendList[id], SHUT_RDWR);
      #endif
      sendList[id] = -1;
      return -1;
   }

   sb->updateBlockConfig();

   return retVal;
}//End of sageUdpModule::recv()

int sageUdpModule::recv(int id, sagePixelBlock *spb, int pidx)
{
   int blockDataSize = spb->getBufSize() - BLOCK_HEADER_SIZE;
   int packetDataSize = config.mtuSize - BLOCK_HEADER_SIZE;
   int dataPt = packetDataSize*pidx;
   int dataBufSize = MIN(blockDataSize - dataPt, packetDataSize);

//std::cerr << "block " << spb->getBufSize() << " mtu " << config.mtuSize << " pid " << pidx << std::endl;

   char dummy[BLOCK_HEADER_SIZE];
   char *packetHeader;
   if (spb->isDirty())
      packetHeader = dummy;
   else
      packetHeader = spb->getBuffer();

   int recvSize = 0;;
   int udpSockFd = udpSendList[id];

//std::cerr << "data buf size " << dataBufSize << std::endl;

   #ifdef WIN32
   WSABUF iovs[2];
   iovs[0].buf = packetHeader;
   iovs[0].len = BLOCK_HEADER_SIZE;
   iovs[1].buf = spb->getPixelBuffer() + dataPt;
   iovs[1].len = dataBufSize;

   DWORD WSAFlags = 0;
   ::WSARead(udpSockFd, iovs, 2, (DWORD*)&recvSize,
         WSAFlags, NULL, NULL);
   #else
   struct iovec iovs[2];
   iovs[0].iov_base = packetHeader;
   iovs[0].iov_len = BLOCK_HEADER_SIZE;
   iovs[1].iov_base = spb->getPixelBuffer() + dataPt;
   iovs[1].iov_len = dataBufSize;

   struct msghdr blockMsgHdr;
   bzero((void *)&blockMsgHdr, sizeof(blockMsgHdr));

   blockMsgHdr.msg_iov = iovs;
   blockMsgHdr.msg_iovlen = 2;

   recvSize = ::recvmsg(udpSockFd, &blockMsgHdr, MSG_WAITALL);
//std::cerr << "msg read size " << recvSize << std::endl;

   #endif

   if (recvSize < 0) {
      perror("sageUdpModule::recv :");
   }

   if (recvSize > 0 && recvSize < (BLOCK_HEADER_SIZE + dataBufSize))
      sage::printLog("sageUdpModule::recv - message size error");

   return recvSize;
}

int sageUdpModule::skipBlock(int id, int pidx)
{
   int blockDataSize = config.blockSize - BLOCK_HEADER_SIZE;
   int packetDataSize = config.mtuSize - BLOCK_HEADER_SIZE;
   int dataPt = packetDataSize*pidx;
   int dataBufSize = MIN(blockDataSize - dataPt, packetDataSize);
   char dummy[config.blockSize];
   int udpSockFd = udpSendList[id];
   int recvSize;

   #ifdef WIN32
   WSABUF iovs[2];
   iovs[0].buf = dummy;
   iovs[0].len = BLOCK_HEADER_SIZE;
   iovs[1].buf = dummy + BLOCK_HEADER_SIZE;
   iovs[1].len = dataBufSize;

   DWORD WSAFlags = 0;
   ::WSARead(udpSockFd, iovs, 2, (DWORD*)&recvSize,
         WSAFlags, NULL, NULL);
   #else
   struct iovec iovs[2];
   iovs[0].iov_base = dummy;
   iovs[0].iov_len = BLOCK_HEADER_SIZE;
   iovs[1].iov_base = dummy + BLOCK_HEADER_SIZE;
   iovs[1].iov_len = dataBufSize;

   struct msghdr blockMsgHdr;
   bzero((void *)&blockMsgHdr, sizeof(blockMsgHdr));

   blockMsgHdr.msg_iov = iovs;
   blockMsgHdr.msg_iovlen = 2;

   recvSize = ::recvmsg(udpSockFd, &blockMsgHdr, MSG_WAITALL);
   #endif


   if (recvSize < 0) {
      perror("sageUdpModule::recv :");
   }

   if (recvSize > 0 && recvSize < (BLOCK_HEADER_SIZE + dataBufSize))
      sage::printLog("sageUdpModule::skipBlock - message size error");

   return recvSize;
}

int sageUdpModule::recvGrp(int id, sageBlockGroup *sbg)
{
   if (id < 0 || id > sendList.size()-1) {
      sage::printLog("sageUdpModule::recv() : invalid sender ID");
      return -1;
   }

   int udpSockFd = udpSendList[id];

   if (udpSockFd < 0) {
      sage::printLog("sageUdpModule::recvGrp : the socket is closed");
      return -1;
   }

   if (!sbg) {
      sage::printLog("sageUdpModule::recvGrp - block group ptr is null");
      return -1;
   }

   int curFrame = sbg->getFrameID();
   int blockIdx = 0, maxBlockID = -1, grpConfigID = -1;
   int totalReadSize = 0;
   sbg->clearBlocks();
   bool firstRead = true;

   while (blockIdx <sbg->size()) {
      int flag, frameID, blockID, packetID, configID;
      char header[BLOCK_HEADER_SIZE];
      memset(header,0, BLOCK_HEADER_SIZE);
      int headerSize = -1;
      while (headerSize < 0) {
         headerSize = sage::recv(udpSockFd, (void *)header, BLOCK_HEADER_SIZE, MSG_PEEK);
      }

      if (headerSize == 0) {
         #ifdef WIN32
         closesocket(sendList[id]);
         #else
         shutdown(sendList[id], SHUT_RDWR);
         #endif
         sendList[id] = -1;

         sage::printLog("sageUdpModule::recvGrp : shutdown connection");
      return -1;
   }

//std::cerr << "header " << header << std::endl;
      char *msgStr = sage::tokenSeek(header, 1);

//std::cerr << "msg1 " << msgStr << std::endl;
      sscanf(msgStr, "%d", &flag);
      msgStr = sage::tokenSeek(header, 6);

//std::cerr << "msg2 " << msgStr << std::endl;
      sscanf(msgStr, "%d %d %d %d", &frameID, &blockID, &packetID, &configID);

//std::cerr << "packet header " << frameID << " " << blockID << " " << packetID << " " << configID
//      << std::endl;

//std::cerr << "cur frame " << curFrame << std::endl;

      sagePixelBlock *curBlock = (*sbg)[blockIdx];
      bool nextBlock = false;

      if (frameID < curFrame) {
         int readSize = -1;

         while (readSize < 0) {
            readSize = skipBlock(id, packetID);
         }

         if (readSize == 0) {
      #ifdef WIN32
      closesocket(sendList[id]);
      #else
      shutdown(sendList[id], SHUT_RDWR);
      #endif
      sendList[id] = -1;

            sage::printLog("sageUdpModule::recvGrp : shutdown connection");
      return -1;
   }

         totalReadSize += readSize;
         curBlock = NULL;
      }
      else if (!firstRead && (frameID > curFrame || flag == SAGE_UPDATE_BLOCK)) {
         blockIdx++;  // for counting block number
         break;
      }
      else if (frameID > curFrame)
         curFrame = frameID;

      if (curBlock && curBlock->isDirty()) {
         if (curBlock->getID() != blockID) {
            if (blockID > maxBlockID)
               nextBlock = true;
            else {
               for (int i=blockIdx-1; i>=0; i--) {
                  if ((*sbg)[i]->getID() == blockID) {
                     curBlock = (*sbg)[i];
                     break;
                  }
               }

               if (curBlock->getID() != blockID)
                  nextBlock = true;
            }
         }
      }

      if (nextBlock) {
         blockIdx++;
         if (blockIdx < sbg->size())
            curBlock = (*sbg)[blockIdx];
         else
            curBlock = NULL;
      }

      if (curBlock) {
         int readSize = -1;
         while (readSize < 0) {
            readSize = recv(id, curBlock, packetID);
         }

         if (readSize == 0) {
            #ifdef WIN32
            closesocket(sendList[id]);
            #else
            shutdown(sendList[id], SHUT_RDWR);
            #endif
            sendList[id] = -1;

            sage::printLog("sageUdpModule::recvGrp : shutdown connection");
            return -1;
         }

         totalReadSize += readSize;
         curBlock->setID(blockID);
         curBlock->setDirty();
         firstRead = false;

         if (grpConfigID < 0) {
            grpConfigID = configID;
         }
         else {
            assert(grpConfigID == configID);
         }
      }

      if (flag == SAGE_UPDATE_BLOCK) {
         break;
      }
   }

//std::cerr << "group config " << blockIdx << " " << curFrame << " " << grpConfigID << std::endl;

   sbg->updateConfig(blockIdx, curFrame, grpConfigID);

//std::cerr << "read size " << totalReadSize << std::endl;

   return totalReadSize;
}//End of sageUdpModule::recvGrp()

int sageUdpModule::close(int id, int mode)
{
   if (mode == -1)
      mode = sMode;

   streamProtocol::close(id, mode);

   if (mode == SAGE_RCV || mode == SAGE_ARCV) {
      if (udpSendList[id] >= 0) {
      #ifdef WIN32
         closesocket(udpSendList[id]);
      #else
         shutdown(udpSendList[id], SHUT_RDWR);
      #endif
      }
   }

   if (mode == SAGE_SEND) {
      if (config.groupSize > 0 && flowList[id])
         flowList[id]->active = false;

      if (udpRcvList[id] >= 0) {
      #ifdef WIN32
         closesocket(udpRcvList[id]);
      #else
         shutdown(udpRcvList[id], SHUT_RDWR);
      #endif
      }
   }

   return 0;
}

int sageUdpModule::close()
{
   closeFlag = true;

   int rcvNum = rcvList.size();
   int sendNum = sendList.size();

   if (sMode != SAGE_SEND) {
#ifdef WIN32
      closesocket(serverSockFd);
#else
      shutdown(serverSockFd, SHUT_RDWR);
#endif

      for (int i=0; i<sendNum; i++) {
         if (sendList[i] < 0)
            continue;
#ifdef WIN32
         closesocket(sendList[i]);
         closesocket(udpSendList[i]);
#else
         shutdown(sendList[i], SHUT_RDWR);
         shutdown(udpSendList[i], SHUT_RDWR);
#endif
      }
   }

   if (sMode != SAGE_RCV && sMode != SAGE_ARCV) {
      for (int i=0; i<rcvNum; i++) {
#ifdef WIN32
         closesocket(rcvList[i]);
         closesocket(udpRcvList[i]);
#else
         shutdown(rcvList[i], SHUT_RDWR);
         shutdown(udpRcvList[i], SHUT_RDWR);
#endif
      }
   }

   return 0;
}

bool sageUdpModule::setSockOpts(int fd, bool noDelay)
{
   int optVal, optLen;

#if defined(WIN32) || defined(__APPLE__) || defined(__sun)

   // loosen the rules for check during bind to allow mutiple binds on the same port
   optVal=1;
   optLen = sizeof(optVal);
   if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optLen) !=0)
   {
      std::cerr << "\nsageUdpModule::setSockOpts(): Error setting SO_REUSEADDR" <<  std::endl;
      return false;
   }

   optVal= config.sendBufSize, optLen = sizeof(optVal);
	std::cout << "info: send buff size = " << config.sendBufSize  << std::endl;
	std::cout << "info: receive buff size = " << config.rcvBufSize  << std::endl;

   if(setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&optVal, optLen) !=0)
   {
      std::cerr << "sageUdpModule::setSockOpts(): Error setting SO_SNDBUF to " << optVal << " bytes";
      return false;
   }

   optVal= config.rcvBufSize; optLen = sizeof(optVal);
   if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&optVal, optLen) !=0)
   {
      std::cerr << "sageUdpModule::setSockOpts(): Error setting SO_RCVBUF to " << optVal << " bytes";
      return false;
   }

   if (noDelay) {
      optVal = 1;
      optLen=sizeof(optVal);
      if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&optVal, optLen) !=0)
         std::cerr << "sageUdpModule::setSockOpts(): Error switching off Nagle's algorithm." << std::endl;
   }
#else
   int reuseFlag = 1;
   optLen = sizeof(reuseFlag);
   if (setsockopt(fd, SOL_SOCKET,SO_REUSEADDR, (char *) &reuseFlag, optLen) < 0)
   {
      std::cerr << "sageUdpModule::SetSockOpts(): Cannot set SO_REUSEADDR on socket" << std::endl;
      return false;
   }

std::cerr << "sendBufSize " << config.sendBufSize << std::endl;
   optVal= config.sendBufSize;
   optLen = sizeof(optVal);
   if(setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void*)&optVal, (socklen_t)optLen) != 0)
   {
      std::cerr << "sageUdpModule::SetSockOpts(): Error setting SO_SNDBUF to " << config.sendBufSize << " bytes" << std::endl;
      return false;
   }

std::cerr << "rcvBufSize " << config.rcvBufSize << std::endl;
   optVal= config.rcvBufSize; optLen = sizeof(optVal);
   if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void*)&optVal, (socklen_t)optLen) != 0)
   {
      std::cerr << "sageUdpModule::SetSockOpts(): Error setting SO_SNDBUF to " << config.rcvBufSize << " bytes" << std::endl;
      return false;
   }

   if (noDelay) {
      optVal = 1;
      optLen=sizeof(optVal);
      if(setsockopt(fd, SOL_TCP, TCP_NODELAY, (void*)&optVal, (socklen_t)optLen) !=0)
         std::cerr << "sageUdpModule::setSockOpts(): Error switching off Nagle's algorithm." << std::endl;
   }
#endif

   return true;
} //End of sageUdpModule::setSockOpts()

sageUdpModule::~sageUdpModule()
{
   close();

   for (int i=0; i<flowList.size(); i++) {
      streamFlowData *fData = flowList[i];
      if (fData)
         delete fData;
   }

   flowList.clear();

   pthread_join(thId, NULL);
}
