/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageBlockStreamer.cpp - straightforward pixel block streaming to displays
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
#include "sageFrame.h"
//#include "streamInfo.h"
#include "sageBlockPartition.h"
#include "sageBlockPool.h"

// char hostname[SAGE_NAME_LEN];

sageBlockStreamer::sageBlockStreamer(streamerConfig &conf, int pixSize) : compFactor(1.0), 
   compX(1.0), compY(1.0), doubleBuf(NULL) 
{
   config = conf;
   blockSize = config.blockSize;
   bytesPerPixel = pixSize;

   interval = 1000000.0/config.frameRate;
  
   //int memSize = (config.resX*config.resY*bytesPerPixel + BLOCK_HEADER_SIZE 
   //   + sizeof(sageMemSegment)) * 4;
   //memObj = new sageMemory(memSize);
   
   createDoubleBuffer();
   
// gethostname(hostname, SAGE_NAME_LEN);
}

int sageBlockStreamer::createDoubleBuffer()
{
   if (doubleBuf) {
      sage::printLog("sageStreamer::createDoubleBuffer : double buffer exist already");
      return -1;
   }
   
   // can create different type of pixel blocks
   sagePixelData **pixelBuf = new sagePixelData*[2];
   
   if (config.pixFmt == PIXFMT_DXT) {
      compX = 4.0;
      compY = 4.0;
      compFactor = 16.0;
   }
      
   for (int i=0; i<2; i++) {
      pixelBuf[i] = new sageBlockFrame(config.resX, config.resY, bytesPerPixel,  
               compX, compY);
      *pixelBuf[i] = config.imageMap;
   }   
   
   doubleBuf = new sageDoubleBuf;
   doubleBuf->init(pixelBuf);
   
   return 0;
}

void sageBlockStreamer::setNwConfig(sageNwConfig &nc) 
{
   nwCfg = nc;

   if (config.protocol == SAGE_UDP && config.autoBlockSize) {
      double payLoadSize = nwCfg.mtuSize - BLOCK_HEADER_SIZE;
      double pixelNum = payLoadSize/bytesPerPixel*compFactor;

      config.blockX = (int)floor(sqrt(pixelNum));
      if ( (config.pixFmt == PIXFMT_DXT) || (config.pixFmt == PIXFMT_YUV) ) {
         config.blockX = (config.blockX/4)*4;
      }
      
      config.blockY = (int)floor(pixelNum/config.blockX);
      if (config.pixFmt == PIXFMT_DXT) {
         config.blockY = (config.blockY/4)*4;
      }
   }   

   partition = new sageBlockPartition(config.blockX, config.blockY, config.totalWidth,
      config.totalHeight);
   
   sageBlockFrame *buf = (sageBlockFrame *)doubleBuf->getBuffer(0);
   buf->initFrame(partition);
   buf = (sageBlockFrame *)doubleBuf->getBuffer(1);
   buf->initFrame(partition);
   blockSize = (int)ceil(config.blockX*config.blockY*bytesPerPixel/compFactor)
      + BLOCK_HEADER_SIZE;
   partition->initBlockTable();
	
   nwCfg.blockSize = blockSize;
   nwCfg.groupSize = config.groupSize;
   nwCfg.maxBandWidth = (double)config.maxBandwidth/8.0; // bytes/micro-second
   nwCfg.maxCheckInterval = config.maxCheckInterval;  // in micro-second
   nwCfg.flowWindow = config.flowWindow;
   nbg = new sageBlockGroup(blockSize, doubleBuf->bufSize(), GRP_MEM_ALLOC | GRP_CIRCULAR);
}

void sageBlockStreamer::setupBlockPool()
{ 
   nwObj->setupBlockPool(nbg); 
}

int sageBlockStreamer::sendPixelBlock(sagePixelBlock *block)
{
   if (!partition) {
      sage::printLog("sageBlockStreamer::sendPixelBlock : block partition is not initialized");
      return -1;
   }
      
   pixelBlockMap *map = partition->getBlockMap(block->getID());
   
   if (!map) {
      //std::cerr << "---" <<  hostname << " pBlock " << block->getID() << " out of screen" << std::endl;
      nbg->pushBack(block);
      return 0;   
   }
   
   block->setRefCnt(map->count);
      block->setFrameID(frameID);
      block->updateBufferHeader();
   
   while(map) {
      //std::cerr << "---" <<  hostname << " pBlock " << block->getBuffer() << "  sent to " <<
      //   params[map->infoID].rcvID << std::endl;
      
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

int sageBlockStreamer::sendControlBlock(int flag, int cond)
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
            sage::printLog("sageBlockStreamer::sendControlBlock : fail to send control block");
            return -1;
         }
      }
   }   

   return 0;
}

int sageBlockStreamer::streamPixelData(sageBlockFrame *buf)
{
   if (streamNum < 1) {
      sage::printLog("sageBlockStreamer::streamPixelData : No Active Streams");
      return -1;
   }

   bool flag = true;
   buf->resetBlockIndex();

   //std::cout << config.rank << " stream frame " << frameID << std::endl;

   int cnt = 0;
   while (flag) {
      sagePixelBlock *pBlock = nbg->front();
      nbg->next();
      if (!pBlock) {
         sage::printLog("sageBlockStreamer::streamPixelData : pixel block is NULL");
         continue;
      }   
      flag = buf->extractPixelBlock(pBlock, config.rowOrd);

      if (sendPixelBlock(pBlock) < 0)
         return -1;
   }

   if (sendControlBlock(SAGE_UPDATE_BLOCK, ALL_CONNECTION) < 0)
      return -1;

   //std::cerr << hostname << " frame " << frameID << " transmitted" << std::endl;

   frameID++;
   frameCounter++;
   //frameID = frameID + 10;

	for (int j=0; j<rcvNodeNum; j++) {
		int dataSize = nwObj->flush(params[j].rcvID, configID);
      if (dataSize > 0) {
         totalBandWidth += dataSize;
      }   
      else if (dataSize < 0) {
         sage::printLog("sageBlockStreamer::streamPixelData : fail to send pixel block");
         return -1;
      }   
	}
   
   return 0;
}

int sageBlockStreamer::streamLoop()
{
   while (streamerOn) {
      //int syncFrame = 0;
      //sage::printLog("\n========= wait for a frame ========\n");
      sageBlockFrame *buf = (sageBlockFrame *)doubleBuf->getBackBuffer();
      //sage::printLog("\n========= got a frame ==========\n");
      
      char *msgStr = NULL;
      if (config.nodeNum > 1) {
         config.syncClientObj->sendSlaveUpdate(frameID);
         //sage::printLog("send update %d", config.rank);
         config.syncClientObj->waitForSyncData(msgStr);
         //sage::printLog("receive sync %d", config.rank);
         if (msgStr) {
            //std::cout << "reconfigure " << msgStr << std::endl;
            reconfigureStreams(msgStr);
            //firstConfiguration = false;
         }   
      }
      else {
         pthread_mutex_lock(reconfigMutex);
         if (msgQueue.size() > 0) {
            msgStr = msgQueue.front();
            reconfigureStreams(msgStr);
            //std::cout << "config ID : " << configID << std::endl;
            msgQueue.pop_front();
            firstConfiguration = false;
         }
         pthread_mutex_unlock(reconfigMutex);
      }
      
      if (config.nodeNum == 1)
         checkInterval();
      
      if (streamPixelData(buf) < 0) {
         streamerOn = false;
      }
         
      doubleBuf->releaseBackBuffer();
      //std::cout << "pt1" << std::endl;
   }   

   // for quiting other processes waiting a sync signal
   if (config.nodeNum > 1) {
      config.syncClientObj->sendSlaveUpdate(frameID);
   }
   
   sage::printLog("sageStreamer : network thread exit");
   
   return 0;
}

void sageBlockStreamer::shutdown()
{ 
   streamerOn = false; 
   if (doubleBuf)
      doubleBuf->releaseLocks();
      
   pthread_join(thId, NULL);
}

sageBlockStreamer::~sageBlockStreamer()
{
   if (doubleBuf)
      delete doubleBuf;
      
   if (nwObj)
      delete nwObj;
}
