/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: pixelDownloader.cpp - manaing each application instance of SAGE Receiver
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

#include "pixelDownloader.h"
#include "sageDisplay.h"
#include "sageSync.h"
#include "sageBlockPool.h"
#include "sageSharedData.h"
#include "sageBlock.h"
#include "sageEvent.h"
#include "sageBlockPartition.h"
#include "sageReceiver.h"

int montagePair::init(displayContext *context, sagePixFmt pfmt, int index, float depth)
{
   montage[0] = new sageMontage(context, pfmt);
   montage[1] = new sageMontage(context, pfmt);
   //tileID = id;
   setLocalTileIdx(index);
   setDepth(depth);

   return 0;
}

void montagePair::swapMontage()
{
   // when a window is moved or resized, the config of back montage is
   // updated immediately, the front montage is updated when it is swapped

   if (renewMontage) {
      // copying the config of back montage to front montage
      montage[frontMon]->copyConfig(*montage[1-frontMon]);
      renewMontage = false;
   }

   frontMon = 1 - frontMon;
}

int montagePair::deleteMontage()
{
   montage[0]->deleteTexture();
   montage[1]->deleteTexture();

   return 0;
}

int montagePair::setDepth(float depth)
{
   montage[0]->depth = depth;
   montage[1]->depth = depth;

   return 0;
}

int montagePair::setLocalTileIdx(int idx)
{
   montage[0]->tileIdx = idx;
   montage[1]->tileIdx = idx;

   return 0;
}

montagePair::~montagePair()
{
   delete montage[0];
   delete montage[1];
}

pixelDownloader::pixelDownloader() : reportRate(1), updatedFrame(0), curFrame(0), recv(NULL),
   streamNum(0), bandWidth(0), montageList(NULL), configID(0), frameCheck(false),
   syncFrame(0), updateType(SAGE_UPDATE_FOLLOW), activeRcvs(0), passiveUpdate(false),
   dispConfigID(0), displayActive(false), status(PDL_WAIT_DATA), frameBlockNum(0), frameSize(0),
   partition(NULL), blockBuf(NULL),
	m_initialized(false)
{
   perfTimer.reset();

   // is this from bridge and actually parallel app?
   fromBridgeParallel = false;
}

int pixelDownloader::init(char *msg, dispSharedData *sh, streamProtocol *nwObj, bool sync, int sl)
{
	/* from sageStreamer::connectToRcv()
	char regMsg[REG_MSG_SIZE];
	      sprintf(regMsg, "%d %d %d %d %d %d %d %d %d %d %d %d",
	    		  config.streamType,
	    		  config.frameRate,
	    		  winID,
	    		  config.groupSize,
	    		  blockSize,
	    		  config.nodeNum,
	    		  (int)config.pixFmt,
	    		  config.blockX,
	    		  config.blockY,
	    		  config.totalWidth,
	    		  config.totalHeight,
	    		  config.fromBridge);

	      */
   char *msgPt = sage::tokenSeek(msg, 3);
   sscanf(msgPt, "%d %d %d", &instID, &groupSize, &blockSize);

   int blockX, blockY, imgWidth, imgHeight;
   sagePixFmt pixFmt;
   int temp = 0;

   msgPt = sage::tokenSeek(msg, 7);
   sscanf(msgPt, "%d %d %d %d %d %d", (int *)&pixFmt, &blockX, &blockY, &imgWidth, &imgHeight, &temp);
   if ( temp ) fromBridgeParallel = true;

   if ( fromBridgeParallel ) {
	   fprintf(stderr,"PDL::%s() : fromBridgeParallel true\n", __FUNCTION__);
   }

   if (partition) {
	   delete partition;
   }
   partition = new sageBlockPartition(blockX, blockY, imgWidth, imgHeight);
   if (!partition) {
		sage::printLog("[%d,%d] PDL::init() : unable to create block partition", shared->nodeID, instID);
      return -1;
   }
   partition->initBlockTable();

   shared = sh;
   syncOn = sync;
   syncLevel = sl;

   // how many tiles a node has
   tileNum = shared->displayObj->getTileNum();

   // montage INSTANTIATION
   if ( montageList ) delete [] montageList;

   montageList = new montagePair[tileNum];
   float depth = 1.0f - 0.01f*instID;

   for (int i=0; i<tileNum; i++) {
      montageList[i].init(shared->context, pixFmt, i, depth);
   }

   configQueue.clear();

   if ( blockBuf ) delete blockBuf;
   blockBuf = new sageBlockBuf(shared->bufSize, groupSize, blockSize, BUF_MEM_ALLOC | BUF_CTRL_GROUP);

   if ( recv ) delete recv;
   recv = new sagePixelReceiver(msg, (rcvSharedData *)shared, nwObj, blockBuf);

	m_initialized = true;

   return 0;
}

int pixelDownloader::addStream(int senderID)
{
   if (recv) {
      recv->addStream(senderID);
   }
   else {
      sage::printLog("pixelDownloader::addStream : receiver obj is NULL");
      return -1;
   }

   return 0;
}

int pixelDownloader::clearTile(int tileIdx)
{
   montagePair &monPair = montageList[tileIdx];
   sageMontage *mon = monPair.getFrontMon();
   shared->displayObj->removeMontage(mon);
   monPair.deactivate();

   return 0;
}

int pixelDownloader::clearScreen()
{
   for (int i=0; i<tileNum; i++) {
      clearTile(i);
   }

   return 0;
}

int pixelDownloader::swapMontages()
{
   bool activeMontage = false;

   for (int i=0; i<tileNum; i++) {
      montagePair &monPair = montageList[i];

      if (monPair.isActive()) {
         if (monPair.getClearFlag()) {
            clearTile(i);
         }
         else {
            monPair.swapMontage();
            shared->displayObj->replaceMontage(monPair.getFrontMon());
         }
         activeMontage = true;
      }
   }

   if (activeMontage) {
      shared->displayObj->setDirty();
   }

   return 0;
}

void pixelDownloader::processSync(int frame, int cmd)
{
   if (!syncOn)
      return;



   syncFrame = frame;
   //if (shared->nodeID == 0)
   //std::cout << "receive sync " << syncFrame << std::endl;

   //sungwon
   //fprintf(stderr, "pixelDownloader::processSync(syncFrame %d, cmd %d), instID %d, streamNum %d, tileNum %d, updatedFrame %d\n",frame, cmd,instID, streamNum, tileNum, updatedFrame);


   // when the sync slave keeps the pace
   if (updatedFrame == syncFrame) {
      swapMontages(); // swap textures (one for display, one for pixel downloading)
   }

   // when the sync slave runs behind and the screen needs to be cleared (only for SOFT_SYNC mode)
   else {
      bool screenUpdate = false;
      for (int i=0; i<tileNum; i++) {
         montagePair &monPair = montageList[i];
         if (monPair.getClearFlag()) {
            clearTile(i);
            screenUpdate = true;
         }
      }
      if (screenUpdate)
         shared->displayObj->setDirty();

      if (cmd == SKIP_FRAME) {
         updatedFrame = syncFrame;


      }
   }


   /**
    * SAGE display has two textures for each image fragment.
One for display. The other to be written new pixels on.
Two textures are swapped once a sync signal arrived
and a new image shows up when the screen is refreshed.
This only happens when the frame number of the image
loaded on the new texture to be shown matches the sync
frame number.
It should match always for HARD-SYNC mode.
But it may not for SOFT-SYNC mode
for which sync frame can proceed even though some slaves
are not ready for going to the next frame.
But I can't guarantee the stability of the SOFT-SYNC mode
of the current implementation (especially for parallel apps).
CONSTANT-SYNC mode was not complete either.

HARD-SYNC and NO-SYNC mode are mostly tested so far.
    */
}

int pixelDownloader::enqueConfig(char *data)
{
   char *configData = new char[strlen(data)+1];
   if (!configData) {
      sage::printLog("pixelDownloader::enqueConfig : unable to allocate memory");
      return -1;
   }

   strcpy(configData, data);
   configQueue.push_back(configData);

   return 0;
}

bool pixelDownloader::reconfigDisplay(int confID)
{
   if (dispConfigID >= confID) {
		sage::printLog("[%d,%d] PDL::reconfigDisplay(%d) : configuration ID error", shared->nodeID, instID, confID);
      return false;
   }

   char *configStr = NULL;
   while (dispConfigID < confID) {
      if (configQueue.size() == 0)
         return false;

      configStr = configQueue.front();
      dispConfigID++;
      configQueue.pop_front();
   }

   int oldRcvs = activeRcvs;
   displayActive = false;

   sageRotation orientation;
   sscanf(configStr, "%d %d %d %d %d %d", &windowLayout.x, &windowLayout.y,
      &windowLayout.width, &windowLayout.height, &activeRcvs, (int *)&orientation);
   windowLayout.setOrientation(orientation);

   // when the window locates on a neighbor tiled display
   if (windowLayout.width == 0 || windowLayout.height == 0) {
      if (syncOn) {
         for (int i=0; i<tileNum; i++)
            montageList[i].clear();
      }
      else {
         clearScreen();
         shared->displayObj->setDirty();
      }
      return true;
   }

   partition->setDisplayLayout(windowLayout);
   partition->clearBlockTable();

   for (int i=0; i<tileNum; i++) {
      montagePair &monPair = montageList[i];
      sageMontage *mon = NULL;

      sageRect tileRect = shared->displayObj->getTileRect(i);
      if (!tileRect.crop(windowLayout)) {
         if (syncOn)
            monPair.clear();
         else {
            clearTile(i);
            shared->displayObj->setDirty();
         }
         continue;
      }

      displayActive = true;

      partition->setTileLayout(tileRect);
      sageRect viewPort = partition->getViewPort();
      sageRect blockLayout = partition->getBlockLayout();

      viewPort.moveOrigin(blockLayout);

      if (monPair.isActive()) {
         //if (shared->nodeID == 5)
         //   std::cerr << "montage active" << std::endl;
         mon = monPair.getBackMon();
         *(sageRect *)mon = tileRect;
         mon->init(viewPort, blockLayout, orientation);
         monPair.renew();
      }
      else {
         //if (shared->nodeID == 5)
         //   std::cerr << "montage inactive" << std::endl;
         mon = monPair.getFrontMon();
         //*(sageRect *)mon = tileRect;
         //mon->init(viewPort, blockLayout, orientation);
         int monIdx = shared->displayObj->addMontage(mon);

         mon = monPair.getBackMon();
         *(sageRect *)mon = tileRect;
         mon->init(viewPort, blockLayout, orientation);
         mon->monIdx = monIdx;
         monPair.renew();

         monPair.activate();
      }

      partition->genBlockTable(i);
   }

   frameSize = blockSize * partition->tableEntryNum();

   if (oldRcvs != activeRcvs)
      updateType = SAGE_UPDATE_SETUP;

   return true;
}

int pixelDownloader::downloadPixelBlock(sagePixelBlock *block, montagePair &monPair)
{
   sageMontage *mon = monPair.getBackMon();

   mon->loadPixelBlock(block);
   //monPair.update();

   return 0;
}

int pixelDownloader::fetchSageBlocks()
{
   // fetch block data from the block buffer
   sageBlockGroup *sbg;

	// will use this instead of END_FRAME flag
	bool useLastBlock = true; // must be TCP
	bool proceedSwap = false;

   while (sbg = blockBuf->front()) {

		 /**
		  * the difference between updatedFrame and syncFrame should always 1
		  * because the new frame it gets is always right next frame of current frame
		  * otherwise, PDL should WAIT until others catch up
		  *
		  * This is the most important pre-requisite of the sync algorithm
		  */
	   if ( syncOn && (sbg->getFrameID() > syncFrame + 1) ) {
			 status = PDL_WAIT_SYNC; // wait for others to catch up
		   return status;
	   }

		 //
		 // pixelReceiver received entire frame
		 //
			 /** lastblock used
		 if (sbg->getFlag() == sageBlockGroup::END_FRAME) { // END_FRAME flag is set at the sagePixelReceiver::readData()
			// now the most recent frame I got(curFrame) becomes updateFrame.
			// this means that because of swapMontages() curFrames will become front montage which means it can be displayed
			// therefore, it's updatedFrame
         updatedFrame = curFrame;

			//fprintf(stderr,"SDM %d PDL %d END_FRAME, Frame %d, syncFrame %d\n", shared->nodeID, instID, updatedFrame, syncFrame);
			//fflush(stderr);

         frameCounter++;

         // calculate packet loss
         packetLoss += frameSize-(frameBlockNum*blockSize);
         frameBlockNum = 0;

         if (syncOn) {
            if (updatedFrame <= syncFrame) {
               swapMontages();
            }
            else {

            	//sungwon
            	//fprintf(stderr, "pixelDownloader::fetchSageBlocks() : instID %d, stremNum %d, tileNum %d sendSlaveUpdate(updatedFrame %d, instID %d, activeRcvs %d, updateType %d)\n"
            	//		,instID, streamNum, tileNum, updatedFrame, instID, activeRcvs, updateType);

            	// frame, id, rcvNum, type
            	// activeRcv is the number of receiver(number of tiles)
               shared->syncClientObj->sendSlaveUpdate(updatedFrame, instID, activeRcvs, updateType);
               updateType = SAGE_UPDATE_FOLLOW;
               //std::cout << "send update " << updatedFrame << std::endl;
               status = PDL_WAIT_SYNC;

               blockBuf->next();
               blockBuf->returnBG(sbg);

               return status;
            }
         }
         else {
            swapMontages();
         }
		} // end of if(END_FRAME)
			**/


		//
		// pixelReceiver received new CONFIG
		//
      if (sbg->getFlag() == sageBlockGroup::CONFIG_UPDATE) {
         if (configID < sbg->getConfigID()) {
            if (reconfigDisplay(sbg->getConfigID()))
               configID = sbg->getConfigID();
            else {
               status = PDL_WAIT_CONFIG;
               return status;
            }
         }
      }

		//
		// continuous next frame received (it means this node was displaying this app already)
		//
      else if (sbg->getFlag() == sageBlockGroup::PIXEL_DATA && sbg->getFrameID() > updatedFrame) {
    	  if (configID < sbg->getConfigID()) {
    		  if (reconfigDisplay(sbg->getConfigID()))
    			  configID = sbg->getConfigID();
    		  else {
    			  status = PDL_WAIT_CONFIG;
    			  return status;
    		  }
    	  }

    	  bandWidth += sbg->getDataSize() + GROUP_HEADER_SIZE;
    	  curFrame = sbg->getFrameID();
    	  frameBlockNum += sbg->getBlockNum();

    	  for (int i=0; i<sbg->getBlockNum(); i++) {
    		  sagePixelBlock *block = (*sbg)[i];

    		  if (!block)
    			  continue;

    		  //std::cout << "block header " << (char *)block->getBuffer() << std::endl;

    		  blockMontageMap *map = (blockMontageMap *)partition->getBlockMap(block->getID());
    		  int bx = block->x, by = block->y;

    		  while(map) {
    			  block->translate(map->x, map->y);
    			  //std::cout << "block montage " << map->infoID << " id " << block->getID() << " pos " << block->x << " , " << block->y << std::endl;
    			  downloadPixelBlock(block, montageList[map->infoID]);
    			  block->x = bx;
    			  block->y = by;
    			  map = (blockMontageMap *)map->next;
    		  }
    	  } // end of foreach block

    	  if ( !fromBridgeParallel && recv->getSenderNum() == 1 ) {
    		  if ( partition && frameBlockNum >= partition->tableEntryNum() ) { // whole frame received
    			  useLastBlock = true; // setting flag for swapMontages to be executed, since END_FRAME
    			  proceedSwap = true;
    		  }
    		  else {
    			  useLastBlock = false;
    		  }
    	  }
    	  else {
			  //fprintf(stderr,"[%d,%d] PDL::fetch() : parallel sender %d \n", shared->nodeID, instID, recv->getSenderNum());
    		  // if it's parallel app (multiple sender), we have to use END_FRAME. Because senders(for this PDL) can send duplicate blocks. This can cause one frame behind issue.
    		  // If we use counting number of blocks method, we will see black spots on the screen
    		  useLastBlock = false;
    	  }
      }


		//
		// if UDP, lastBlock checking method could fail when a block is dropped, END_FRAME flag is used in that case
		// IMPORTANT
		// When in UDP, sync performance could be very bad.
		// Because some node can use END_FRAME while others don't
		// In that case, the node that used END_FRAME will become the slowest one, and others will have to wait for this node.
		//
		else if (sbg->getFlag() == sageBlockGroup::END_FRAME) { // END_FRAME flag is set at the sagePixelReceiver::readData()
			//fprintf(stderr, "[%d,%d] PDL::fetch() : END_FRAME; updF %d, curF %d, syncF %d\n", shared->nodeID, instID, updatedFrame, curFrame, syncFrame);

			if ( updatedFrame == curFrame ) {
				// already swapMontage-ed, do nothing
				// if syncOn then, updatedF == curF == synchF
			}
			else if ( updatedFrame > curFrame ) {
				// something is badly wrong
				fprintf(stderr,"[%d,%d] PDL::fetch() : END_FRAME flag!!! FATAL_ERROR!!! curF %d, updF %d, syncF %d\n", shared->nodeID, instID, curFrame, updatedFrame, syncFrame);
			}
			else {
#ifdef DEBUG_PDL
				fprintf(stderr,"[%d,%d] PDL::fetch() : END_FRAME flag!!! Before proceeding, curF %d, updF %d, syncF %d\n", shared->nodeID, instID, curFrame, updatedFrame, syncFrame);
#endif
				proceedSwap = true;
				useLastBlock = false;
			}
		}

		//
		// unexpected case
		//
		else {
			//sage::printLog("\n[%d,%d] PDL::fetch() : invalid block order.",shared->nodeID, instID);
#ifdef DEBUG_PDL
			fprintf(stderr, "\tcurF %d, updF %d, syncF %d, curF %d, cfgID %d\n",curFrame, updatedFrame, syncFrame, configID);
			fprintf(stderr, "\tsbg->getFlag() %d, sbg->getFrameID() %d, sbg->getConfigID() %d\n", sbg->getFlag(), sbg->getFrameID(), sbg->getConfigID());
			fflush(stderr);
#endif
		}


		// to fix END_FRAME recognition.
		// Originally, frame n is recognized as complete (END_FRAME) when blocks of frame n+1 is received
		// This causes frame being displayed is always behind actual config ->  config l is applied but frame l-1 is displayed
		if ( proceedSwap ) {
			proceedSwap = false;

			// now the most recent frame I got(curFrame) becomes updateFrame.
			// this means that because of swapMontages(), curFrames will become front montage which means it can be displayed.
			// therefore, it's updatedFrame.
			updatedFrame = curFrame;
#ifdef DEBUG_PDL
			if (useLastBlock) {
				//fprintf(stderr,"[%d,%d] PDL::fetch() : !!! ProceedSwap !!! using LastBlock fBN %d of %d; updF %d, syncF %d, cfID %d\n", shared->nodeID, instID, frameBlockNum, partition->tableEntryNum(), updatedFrame, syncFrame, configID);
			}
			else {
				//fprintf(stderr,"[%d,%d] PDL::fetch() : !!! ProceedSwap !!! using END_FRAME fBN %d of %d; updF %d, syncF %d, cfID %d\n", shared->nodeID, instID, frameBlockNum, partition->tableEntryNum(), updatedFrame, syncFrame, configID);
			}
#endif
			frameCounter++;

			// calculate packet loss
			packetLoss += frameSize-(frameBlockNum*blockSize);
			frameBlockNum = 0; //reset
			//actualFrameBlockNum = 0;

			if (syncOn) {
				if ( updatedFrame > syncFrame) {
					// if this is the case, I'm too fast. I must wait for others

#ifdef DELAY_COMPENSATION
					//shared->syncClientObj->sendSlaveUpdateToBBS(updatedFrame, instID, activeRcvs, shared->nodeID, shared->latency);
#else
					if ( syncLevel == -1 ) {
						shared->syncClientObj->sendSlaveUpdate(updatedFrame, instID, activeRcvs, updateType); // old sage sync
					}
					else {
						shared->syncClientObj->sendSlaveUpdateToBBS(updatedFrame, instID, activeRcvs, shared->nodeID, 0);
					}
#endif
					updateType = SAGE_UPDATE_FOLLOW;
					status = PDL_WAIT_SYNC;

					blockBuf->next();
					blockBuf->returnBG(sbg);

					return status;
				}
				else if ( updatedFrame == syncFrame ) {
#ifdef DEBUG_PDL
					//fprintf(stderr, "\nPDL::fetch() : [%d,%d] updatedFrame == synchFrame %d, don't we need swapMontages() ? \n", syncFrame);
#endif
					//swapMontages();
				}
				else {
					fprintf(stderr, "\nPDL::fetch() : [%d,%d] FatalError! updF %d , syncF %d\n", shared->nodeID, instID, updatedFrame, syncFrame);
					fflush(stderr);
				}
			}
			else {
				// PDL::processSync() will never be called. So let's do swapMontages here.
				swapMontages();
			}
		} // end of if(proceedSwap)


		blockBuf->next();
		blockBuf->returnBG(sbg); // pushback this grp to blockBuf's dataPool (if PIXEL_DATA) or ctrlPool

   } // end of while(blockBuf->front())

   status = PDL_WAIT_DATA;
   return status;
}

int pixelDownloader::evalPerformance(char **frameStr, char **bandStr)
{
   //Calculate performance here
   double elapsedTime = perfTimer.getTimeUS();

   if (elapsedTime > 1000000.0*reportRate && reportRate > 0) {
      *bandStr = new char[TOKEN_LEN];

      float obsBandWidth = (float) (bandWidth * 8.0 / (elapsedTime));
      float obsLoss = (float) (packetLoss * 8.0 / (elapsedTime));
      bandWidth = 0;
      packetLoss = 0;
      sprintf(*bandStr, "%d %7.2f %7.2f %d", instID, obsBandWidth, obsLoss, frameSize);

      if (displayActive) {
         *frameStr = new char[TOKEN_LEN];
         float frameRate = (float) (frameCounter.getValue()*1000000.0/elapsedTime);
         frameCounter.reset();
         sprintf(*frameStr, "%d %f", instID, frameRate);
      }

      perfTimer.reset();
   }

   return 0;
}

int pixelDownloader::setDepth(float depth)
{
   for (int i=0; i<tileNum; i++) {
      montageList[i].setDepth(depth);
   }

   return 0;
}

pixelDownloader::~pixelDownloader()
{
	fprintf(stderr, "[%d,%d] PDL::~PDL()\n", shared->nodeID, instID);

   for (int i=0; i<tileNum; i++) {
      montagePair &monPair = montageList[i];
      sageMontage* mon = monPair.getFrontMon();
      shared->displayObj->removeMontage(mon);
      monPair.deleteMontage();
   }

   shared->displayObj->setDirty();

   delete [] montageList;
   delete recv;
   delete blockBuf;

   for (int i=0; i<configQueue.size(); i++) {
      char *configData = configQueue.front();
      configQueue.pop_front();
      delete [] configData;
   }
#ifdef DEBUG_PDL
   fprintf(stderr, "[%d,%d] PDL::~PDL()\n", shared->nodeID, instID);
#endif
}
