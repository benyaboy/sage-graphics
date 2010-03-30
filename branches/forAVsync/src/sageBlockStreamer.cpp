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

#ifdef DEBUG_STREAMER
	fprintf(stderr,"[%d] SBS::SBS() : blockX %d, blockY %d, blockSize %d, resX %d, resY %d, pixSize %d\n", conf.rank, conf.blockX, conf.blockY, conf.blockSize, conf.resX, conf.resY, pixSize);
	fflush(stderr);
#endif

	avDiff = 0;
	frameInterval = 0.0;

   config = conf;
   blockSize = config.blockSize;
   bytesPerPixel = pixSize;

	interval = 1000000.0/config.frameRate; //microsecond

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
#ifdef DEBUG_MEMORY
   fprintf(stderr, "SBS::createDoubleBuffer() : creating an array holds two sagePixelData * \n");
#endif
   sagePixelData **pixelBuf = new sagePixelData*[2];

   if (config.pixFmt == PIXFMT_DXT) {
      compX = 4.0;
      compY = 4.0;
      compFactor = 16.0;
   }


#ifdef DEBUG_MEMORY
	fprintf(stderr, "[%d] SBS::createDoubleBuffer() : creating the front sageBlockFrame\n", config.rank);
#endif
	// (int)ceil(resX * resY * bytePerPixel  /  (compX * compY)) + BLOCK_HEADER_SIZE(128 byte)
	pixelBuf[0] = new sageBlockFrame(config.resX, config.resY, bytesPerPixel, compX, compY); // will call sagePixelData::initBuffer()
	*pixelBuf[0] = config.imageMap;

	if ( ! config.asyncUpdate ) {
#ifdef DEBUG_MEMORY
		fprintf(stderr, "[%d] SBS::createDoubleBuffer() : creating the back sageBlockFrame\n", config.rank);
#endif
		pixelBuf[1] = new sageBlockFrame(config.resX, config.resY, bytesPerPixel, compX, compY);
   }
	else {
		pixelBuf[1] = pixelBuf[0];
		//pixelBuf[1] = new sageBlockFrame(config.resX, config.resY, bytesPerPixel, compX, compY);
	}
	*pixelBuf[1] = config.imageMap;

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

#ifdef DEBUG_STREAMER
	fprintf(stderr, "\tSBS::sendPixelBlock() : blocknum %d, entrynum %d\n", partition->getBlockNum(), partition->tableEntryNum());
	//fflush(stderr);
#endif

	if (!map) {
		//std::cerr << "---" <<  hostname << " pBlock " << block->getID() << " out of screen" << std::endl;
		nbg->pushBack(block);
		return 0;
	}

#ifdef DEBUG_STREAMER
	//fprintf(stderr, "\tSBS::sendPixelBlock() : Entering loop\n");
	//fflush(stderr);
#endif

	block->setRefCnt(map->count);
	block->setFrameID(frameID);

	block->updateBufferHeader();

	while(map) {
		//std::cerr << "---" <<  hostname << " pBlock " << block->getBuffer() << "  sent to " <<
		//   params[map->infoID].rcvID << std::endl;

		//fprintf(stderr, "SBS::sendPixelBlock() : header; Rcv %d, Fid %d, Bid %d, NumBlocks %d\n", map->infoID, block->getFrameID(), block->getID(), params[map->infoID].activeBlockNum);
		//fflush(stderr);

		// map->infoID means receiver ID (SDM ID)
		// setting it to active -> the receiver has to receive pixel
		// this setting affects sendControlBlock() below
		params[map->infoID].active = true;

		int dataSize = 0;

#ifdef DEBUG_STREAMER
		fprintf(stderr, "\t\tSBS::sendPixelBlock() : Iter %d; sending [%d of %d / %d / %d] to rcvID [%d]\n", count, block->getID(), params[map->infoID].activeBlockNum, frameID, configID, params[map->infoID].rcvID);
		//fflush(stderr);
#endif

		// this function will return unless sbg of the nwObj is full
		// if nwObj->sbg isn't full it will return. In this case, dataSize == 0
		// if it's full, it will do ::writev(), in this case, dataSize > 0
		dataSize = nwObj->sendGrp(params[map->infoID].rcvID, block, configID); // trigger sageBlockGroup::sendData()

		if (dataSize > 0) {
			totalBandWidth += dataSize;
		}
		else if (dataSize < 0) {
			sage::printLog("sageBlockStreamer::sendPixelBlock : fail to send pixel block");
			return -1;
		}
		map = map->next;
	}

#ifdef DEBUG_STREAMER
	//fprintf(stderr, "\tSBS::sendPixelBlock() : Exiting loop\n");
	//fflush(stderr);
#endif

	return 0;
}

int sageBlockStreamer::sendControlBlock(int flag, int cond)
{
#ifdef DEBUG_STREAMER
	//fprintf(stderr, "SBS::sendControlBlock() : \n");
	//fflush(stderr);
#endif

   for (int j=0; j<rcvNodeNum; j++) {
      bool sendCond = false;

      switch(cond) {
			case ALL_CONNECTION : // macro 0
            sendCond = true;
            break;
			case ACTIVE_CONNECTION : // macro 1
            sendCond = params[j].active;
            break;
			case INACTIVE_CONNECTION : // macro 2
            sendCond = !params[j].active;
            break;
      }

      if (sendCond) {
         int dataSize = nwObj->sendControl(params[j].rcvID, frameID, configID);
			if (dataSize > 0) {
            totalBandWidth += dataSize;

#ifdef DEBUG_STREAMER
				fprintf(stderr, "[%d] SBS::sendControlBlock(flag %d, cond %d). controlBlock sent to rcvID %d. frame %d, configID %d\n", config.rank, flag,cond, params[j].rcvID, frameID, configID);
				fflush(stderr);
#endif
			}
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
		sage::printLog("SBS::streamPixelData : No Active Streams");
		return -1;
	}

	bool flag = true; // true : has more blocks to send
	buf->resetBlockIndex();



	//long int sec, usec;
	//buf->getTimeStamp(sec, usec);
	//nbg->setTimeStamp(sec, usec);
	//std::cout << "block streamer time stamp : " << sec << " " << usec << std::endl;
	///////////////////

	//std::cerr << "sageBlockStreamer::streamPixelData() : config.rank " << config.rank << " starts streaming frame " << frameID << std::endl;

	int cnt = 0;

#ifdef DEBUG_STREAMER
	fprintf(stderr, "SBS%d::streamPixelData() : Entering streaming loop. frameID %d\n", config.rank, frameID);
	fflush(stderr);
	/*
	int *temp = (int*)malloc(sizeof(int) * 28);
	for ( int i=0; i<6; i++ ) temp[i] = 0;
	 */
#endif

	while (flag) {
		sagePixelBlock *pBlock = nbg->front(); // get a free block
		nbg->next(); // advance to next block in the group

		if (!pBlock) {
			sage::printLog("[%d] SBS::streamPixelData() : pixel block is NULL. (nbg->front() returned null). moving to next block", config.rank);
			continue;
		}
		flag = buf->extractPixelBlock(pBlock, config.rowOrd);

		if (sendPixelBlock(pBlock) < 0)
			return -1;
		cnt++;
	}

	// at this point a FRAME has sent.. -> can be many sbg

#ifdef DEBUG_STREAMER
	std::cerr << "[" << config.rank << "] SBS::streamPixelData() : Stream loop finished. A frame " << frameID << " has sent. Total " << cnt << " blocks. " << std::endl;
#endif

	if (sendControlBlock(SAGE_UPDATE_BLOCK, ALL_CONNECTION) < 0)
		return -1;

	//std::cerr << hostname << " frame " << frameID << " transmitted" << std::endl;

	frameID++;

	/**
	 * below statement will prevent PDL from updating properly
	 * Because sync algorithm is based on consecutive frame number
	 */
	//frameID = frameID + 10;

	for (int j=0; j<rcvNodeNum; j++) {
		int dataSize = nwObj->flush(params[j].rcvID, configID);
		if (dataSize > 0) {
			totalBandWidth += dataSize;
		}
		else if (dataSize < 0) {
			sage::printLog("[%d] SBS::streamPixelData() : nwObj->flush returned %d. fail to send pixel block", config.rank, dataSize);
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

		//
		// will return sagePixelData*
		// This is complete single frame an application has filled
		//
		sageBlockFrame *buf = (sageBlockFrame *)doubleBuf->getBackBuffer();
#ifdef DEBUG_STREAMER
		fprintf(stderr, "\n=====================================================================\n");
		fprintf(stderr, "[%d] SBS::streamLoop() : Received a frame from the app\n", config.rank);
#endif
		//sage::printLog("\n========= got a frame ==========\n");

		/** AV sync */
		if ( avDiff < 0 ) {
			// skip video streaming
			// what about frame numbering.???
#ifdef DEBUG_AVSYNC
			fprintf(stderr, "\n\n[%d] SBS::streamLoop() : avDiff %d. skipping a frame %d\n", config.rank, avDiff, frameID);
#endif
			avDiff++; //reset or increment?
			frameID++;
			doubleBuf->releaseBackBuffer();
			continue;
		}
		else if ( avDiff > 0 ) {
#ifdef DEBUG_AVSYNC
			fprintf(stderr, "\n\n[%d] SBS::streamLoop() : avDiff %d. pausing for %d usec\n", config.rank, avDiff, avDiff * (int)(frameInterval * 1000000.0));
#endif
			sage::usleep(avDiff * (int)(frameInterval * 1000000.0)); // INTERVAL usec * number of frame
			avDiff=0;
		}

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

		//
		// the frame in the buf will be splited into many blocks
		// below sends control blocks too
		//


		//usleep(10000);
		//fprintf(stderr,"SBS::%s() : frame %d\n", __FUNCTION__, frameID);
		if (streamPixelData(buf) < 0) {
			streamerOn = false;
		}
		else {
#ifdef DEBUG_STREAMER
			fprintf(stderr, "[%d] SBS::streamLoop() : streamPixelData() returned. Releasing back buffer\n", config.rank);
			fflush(stderr);
#endif
		}

		doubleBuf->releaseBackBuffer();
		//std::cout << "pt1" << std::endl;
	}

	sage::printLog("[%d] SBS::streamLoop() : exit", config.rank);
	return 0;
}

sageBlockStreamer::~sageBlockStreamer()
{
   if (doubleBuf)
      delete doubleBuf;

   if (nwObj)
      delete nwObj;
}
