/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: bridgeStreamer.cpp - distributes pixel blocks 
 *         to multiple tiled displays
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

#include "sageStreamer.h"
#include "sageBlockPool.h"
//#include "streamInfo.h"
#include "sageBlockPartition.h"

bridgeStreamer::bridgeStreamer(streamerConfig &conf, sageBlockBuf *buf, streamProtocol *obj) 
{
   config = conf; 
   nwObj = obj;
   partition = new sageBlockPartition(config.blockX, config.blockY, config.totalWidth,
      config.totalHeight);
   partition->initBlockTable();
   blockSize = conf.blockSize;
      
   blockBuffer = buf;
   frameID = blockBuffer->addReader(config.streamerID);
   if (config.streamerID > 0)
      updateType = SAGE_UPDATE_FRAME;
   else
      updateType = SAGE_UPDATE_FOLLOW;   
   
   interval = 1000000.0/config.frameRate;
   firstFrame = true;
   accInterval = 0.0;
}

int bridgeStreamer::initNetworks(char *data, bool localPort)
{
   char token[TOKEN_LEN];
   sageToken tokenBuf(data);
   tokenBuf.getToken(token);
      
	int rcvPort = atoi(token) + (int)config.protocol;
   if (nwObj) {   
   nwObj->setConfig(rcvPort, config.blockSize, config.groupSize);
   sage::printLog("bridgeStreamer : network object was initialized successfully");
   std::cout << "block size = " << config.blockSize << std::endl;
   	std::cout << "group size = " << config.groupSize << std::endl;
	}
	else {
		nwCfg.blockSize = config.blockSize;
   	nwCfg.groupSize = config.groupSize;
		nwObj = (streamProtocol *)new sageTcpModule;
      nwObj->init(SAGE_SEND, rcvPort, nwCfg);
      sage::printLog("bridgeStreamer::initNetworks : initialize TCP object");
	}
   
   connectToRcv(tokenBuf, localPort);
   setupBlockPool();
   nwObj->setFrameRate((double)config.frameRate);
   streamTimer.reset();
   
   if (pthread_create(&thId, 0, nwThread, (void*)this) != 0) {
      sage::printLog("sageBlockStreamer : can't create nwThread");
      return -1;
   }
   
   return 0;
}

void bridgeStreamer::setupBlockPool()
{ 
   for (int i=0; i<rcvNodeNum; i++) {
      nwObj->setupBlockPool(blockBuffer, params[i].rcvID); 
   }
}

int bridgeStreamer::storeStreamConfig(char *msgStr)
{
   if (config.nodeNum > 1) { 
      if (config.master) {
         config.sGroup->enqueSyncMsg(msgStr);
         config.sGroup->unblockSync();   
      }   
      //config.syncClientObj->sendSlaveUpdate(1);   
   }
   else {
      enqueMsg(msgStr);
   }
   
   return 0;
}

int bridgeStreamer::streamLoop()
{
   std::cout << "node " << config.rank << " stream loop started " << std::endl;
   while (streamerOn) {
      char *msgStr = NULL;
      
      if (config.nodeNum > 1){
         //std::cout << "node " << config.rank << " syncGroup " << config.syncID << " send update " << frameID << std::endl;
         config.syncClientObj->sendSlaveUpdate(frameID, config.syncID, config.nodeNum, updateType);
         updateType = SAGE_UPDATE_FOLLOW;
         syncMsgStruct *syncMsg = config.syncClientObj->waitForSync(config.syncID);
         if (syncMsg) {
            frameID = syncMsg->frameID;
         
            //std::cout << "node " << config.rank << " syncGroup " << config.syncID << " receive sync " << frameID << std::endl;

            if (syncMsg->data) {
               reconfigureStreams(syncMsg->data);
               //std::cout << "node " << config.rank << " streamer " << config.streamerID << "reconfigure streams" << std::endl;
            }   
            
            delete syncMsg;
         }   
      }
      else {
         pthread_mutex_lock(reconfigMutex);
         if (msgQueue.size() > 0) {
            msgStr = msgQueue.front();
            reconfigureStreams(msgStr);
            msgQueue.pop_front();
            delete [] msgStr;
            firstConfiguration = false;
            //std::cout << "reconfigure bridge streamer" << std::endl;
         }
         pthread_mutex_unlock(reconfigMutex);
      }
      
      if (config.nodeNum == 1)
         checkInterval();
      
      if (streamPixelData() < 0) {
         streamerOn = false;
      }
   }   
   
   sage::printLog("bridgeStreamer : network thread exit");
   
   return 0;
}

int bridgeStreamer::streamPixelData()
{
   if (streamNum < 1) {
      sage::printLog("bridgeStreamer::streamPixelData : No Active Streams");
      return -1;
   }

   // fetch block data from block buffer
   bool loop = true;
   int curFrame = 0;
   
   while (loop) {
      //std::cout << "waiting for data" << std::endl;
      sageBlockGroup *sbg = blockBuffer->front(config.streamerID, frameID);
      //std::cout << "get data" << std::endl;
      
      while (streamerOn && !sbg) {
         blockBuffer->next(config.streamerID);
         sbg = blockBuffer->front(config.streamerID);
      }
      
      if (!sbg)
         return -1;
   
      int flag = sbg->getFlag();
   
      if (flag == sageBlockGroup::PIXEL_DATA) {
         curFrame = sbg->getFrameID();
         for (int i=0; i<sbg->getBlockNum(); i++) {
            sagePixelBlock *block = (*sbg)[i];
            
            if (!block)
               continue;
               
            sendPixelBlock(block);
         }   
      }
      else if (flag == sageBlockGroup::END_FRAME) {
         loop = false;
      }
         
      blockBuffer->next(config.streamerID);
   }
   
   if (sendControlBlock(SAGE_UPDATE_BLOCK, ALL_CONNECTION) < 0)
      return -1;
   
   frameID = curFrame + 1;
      
   if (firstFrame) {
      firstFrame = false;
      frameTimer.reset();
      accInterval = 0.0;
   }
   else if (config.frameDrop && config.nodeNum == 1) {
      accInterval += blockBuffer->getFrameInterval();
      //accInterval += 1000000.0/config.frameRate;

      if (frameTimer.getTimeUS() > accInterval+blockBuffer->getFrameInterval()) {
         std::cout << "drop frame " << frameID << std::endl;
         frameID++;  

         accInterval = 0.0;
         frameTimer.reset();   
      }
   }
   
   frameCounter++;

   for (int j=0; j<rcvNodeNum; j++) {
      if (!params[j].active)
         nwObj->setFrameSize(params[j].rcvID, blockSize);
         
		int dataSize = nwObj->flush(params[j].rcvID, configID);
      if (dataSize > 0) {
         totalBandWidth += dataSize;
      }   
      else if (dataSize < 0) {
         sage::printLog("bridgeStreamer::streamPixelData : fail to flush pixel block group");
         return -1;
      }   
	}
   
   return 0;
}

int bridgeStreamer::sendPixelBlock(sagePixelBlock *block)
{
   if (!partition)
      sage::printLog("bridgeStreamer::sendPixelBlock : block partition is not initialized");
   pixelBlockMap *map = partition->getBlockMap(block->getID());
   
   if (map)
      block->getGroup()->reference(map->count);
   
   while(map) {
      params[map->infoID].active = true;
      int dataSize = nwObj->sendGrp(params[map->infoID].rcvID, block, configID);
      if (dataSize > 0) {
         totalBandWidth += dataSize;
      }   
      else if (dataSize < 0) {
         sage::printLog("sageBlockStreamer::sendPixelBlock : fail to send pixel block");
         return -1;
      }   
      map = map->next;
   }   

   return 0;
}

int bridgeStreamer::sendControlBlock(int flag, int cond)
{
   for (int j=0; j<rcvNodeNum; j++) {   
      bool sendCond = false;
      
      switch(cond) {
         case ALL_CONNECTION :
            sendCond = true;
            break;
         case ACTIVE_CONNECTION :
            sendCond = params[j].active;
            break;
         case INACTIVE_CONNECTION :
            sendCond = !params[j].active;
            break;         
      }
      
      if (sendCond) {
         int dataSize = nwObj->sendControl(params[j].rcvID, frameID, configID);
         if (dataSize > 0)
            totalBandWidth += dataSize;
         else if (dataSize < 0) {
            sage::printLog("bridgeStreamer::sendControlBlock : fail to send control block");
            return -1;
         }
      }
   }   
      
   return 0;
}

int bridgeStreamer::shutdown()
{
   streamerOn = false;
   std::cout << "pt0" << std::endl;
   if (config.nodeNum > 1){
      config.syncClientObj->removeSyncGroup(config.syncID);
   }
   blockBuffer->removeReader(config.streamerID);
   std::cout << "pt1" << std::endl;
   pthread_join(thId, NULL);
   std::cout << "pt2" << std::endl;
   
   for (int j=0; j<rcvNodeNum; j++)
      nwObj->close(params[j].rcvID, SAGE_SEND);
         
   sage::printLog("< bridgeStreamer shutdown >"); 
   
   return 0;
}
