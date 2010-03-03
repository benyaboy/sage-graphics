/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageReceiver.cpp
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

#include "sageReceiver.h"
#include "streamProtocol.h"
#include "sageBlock.h"
#include "sageBlockPool.h"
#include "sageSharedData.h"
#include "sageEvent.h"

// char hostname[SAGE_NAME_LEN];

void* sageReceiver::nwReadThread(void *args)
{
   sageReceiver *This = (sageReceiver *)args;

   This->readData();

   pthread_exit(NULL);
   return NULL;
}

sagePixelReceiver::sagePixelReceiver(char *msg, rcvSharedData *sh,
                                    streamProtocol *obj, sageBlockBuf *buf)
{
   char *msgPt = sage::tokenSeek(msg, 3);
   sscanf(msgPt, "%d %d %d %d", &instID, &groupSize, &blockSize, &senderNum);

   nwObj = obj;
   shared = sh;
   blockBuf = buf;
   FD_ZERO(&streamFds);
   maxSockFd = 0;
   streamList = new streamData[senderNum];
   streamIdx = 0;
   configID = 0;
   curFrame = 1;

   connecting = true;

   pthread_mutex_init(&streamLock, NULL);
   pthread_mutex_unlock(&streamLock);
   pthread_cond_init(&connectionDone, NULL);

   if (pthread_create(&thId, 0, nwReadThread, (void*)this) != 0) {
      sage::printLog("sagePixelReceiver : can't create network reading thread");
   }

// gethostname(hostname, SAGE_NAME_LEN);
}

int sagePixelReceiver::addStream(int senderID)
{
   if (streamIdx >= senderNum) {
      sage::printLog("[%d,%d] sagePixelReceiver::addStream() : stream number exceeded correct number", shared->nodeID, instID);
      return -1;
   }

   streamList[streamIdx].senderID = senderID;
   streamList[streamIdx].dataSockFd = nwObj->getRcvSockFd(senderID);
   maxSockFd = MAX(maxSockFd, streamList[streamIdx].dataSockFd);
   FD_SET(streamList[streamIdx].dataSockFd, &streamFds);

   streamIdx++;

   if (streamIdx == senderNum) {
      pthread_mutex_lock(&streamLock);
      connecting = false;
      pthread_cond_signal(&connectionDone);
      pthread_mutex_unlock(&streamLock);

      shared->eventQueue->sendEvent(EVENT_APP_CONNECTED, instID);
   }

   return streamIdx;
}

int sagePixelReceiver::checkStreams()
{
   fd_set sockFds = streamFds;
   int retVal = select(maxSockFd+1, &sockFds, NULL, NULL, NULL);
   if (retVal <= 0) {
      sage::printLog("sagePixelReceiver::checkStreams : error in stream checking");
      return -1;
   }

   for (int i=0; i<senderNum; i++) {
      if (FD_ISSET(streamList[i].dataSockFd, &sockFds)) {
         streamList[i].dataReady = true;
      }
   }

   return 0;
}

int sagePixelReceiver::readData()
{
	if (!shared) {
		sage::printLog("sagePixelReceiver::readData : shared object is null");
		return -1;
	}

	if (!nwObj)  {
		sage::printLog("sagePixelReceiver::readData : network object is null");
		return -1;
	}

	if (!blockBuf)  {
		sage::printLog("sagePixelReceiver::readData : block buffer is null");
		return -1;
	}

	pthread_mutex_lock(&streamLock);
	if (connecting)
		pthread_cond_wait(&connectionDone, &streamLock);
	pthread_mutex_unlock(&streamLock);

	bool reuseBlockGroup = false;
	bool updated = false;

	sageBlockGroup *sbg = NULL;

#ifdef DEBUG_RECEIVER
	fprintf(stderr, "[%d,%d] sagePixelReceiver::readData() : START! (%d senders, grpSize %d, blkSize %d\n", shared->nodeID, instID, senderNum, groupSize, blockSize);
#endif

	while(!endFlag) {
		if (checkStreams() < 0)
			return -1;

		//std::cout << "pt4" << std::endl;

		int nextFrame = SAGE_INT_MAX;

		for (int i=0; i<senderNum; i++) {
			if (streamList[i].dataReady) {
				if (!reuseBlockGroup) {
					sbg = blockBuf->getFreeBlocks();
					if (!sbg) {
						endFlag = true;
						return -1;
					}
				}
				else {
					reuseBlockGroup = false;
				}

				//std::cout << "pt5" << std::endl;
				sbg->setFrameID(curFrame);
				int rcvSize = nwObj->recvGrp(streamList[i].senderID, sbg);
				streamList[i].dataReady = false;

				//std::cout << "pt6" << std::endl;

				if (rcvSize > 0) {
					if (sbg->getFrameID() == curFrame) {
#ifdef DEBUG_RECEIVER
						fprintf(stderr, "[%d,%d] sagePixelReceiver::readData() : continuous data for curFrame %d\n", shared->nodeID,instID, curFrame);
#endif
						if (sbg->getFlag() == sageBlockGroup::PIXEL_DATA) {
#ifdef DEBUG_RECEIVER
							//fprintf(stderr, "\t[%d,%d] : and it's pixel\n", shared->nodeID,instID);
#endif
							blockBuf->pushBack(sbg);
							//std::cout << "push back " << rcvSize << " " << sbg->getFrameID() << std::endl;
							if (curFrame == 1 && configID == 0)
								configID = sbg->getConfigID();

							//std::cout << "push back frame " << sbg->getFrameID() << std::endl;
							updated = true; // sets flag

							// generate the receive event
							if (blockBuf->isWaitingData())
								shared->eventQueue->sendEvent(EVENT_READ_BLOCK, instID);
						}
						else {
#ifdef DEBUG_RECEIVER
							fprintf(stderr, "\n\t[%d,%d] : and it's NOT a pixel group\n", shared->nodeID,instID);
#endif
							reuseBlockGroup = true;
							if (sbg->getConfigID() > configID) {
#ifdef DEBUG_RECEIVER
								fprintf(stderr, "\t\t[%d,%d] : it's new config %d\n", shared->nodeID, instID, sbg->getConfigID());
#endif
								configID = sbg->getConfigID();
								sageBlockGroup *cbg = blockBuf->getCtrlGroup(sageBlockGroup::CONFIG_UPDATE);
								cbg->setFrameID(sbg->getFrameID());
								cbg->setConfigID(configID);
								blockBuf->pushBack(cbg);
							}
							else {
								streamList[i].bGroup = NULL;
#ifdef DEBUG_RECEIVER
								fprintf(stderr, "\t\t[%d,%d] : it's not new config. sbg's configID %d. streamList[%d].bGroup is now NULL\n", shared->nodeID, instID, sbg->getConfigID(), i);
#endif
							}

							/**
							 * SBS always sends control block after sending a frame
							 * Once this arrived, we now know that current frame is done
							 */
							streamList[i].curFrame = sbg->getFrameID()+1;
#ifdef DEBUG_RECEIVER
							fprintf(stderr, "\t[%d,%d] : Received control data. streamList[%d].curFrame has incremented to %d\n", shared->nodeID, instID, i, streamList[i].curFrame);
#endif
							FD_CLR(streamList[i].dataSockFd, &streamFds);
						}
					}

					//
					// new/next frame
					//
					else if (sbg->getFrameID() > curFrame) {
#ifdef DEBUG_RECEIVER
						fprintf(stderr, "[%d,%d] sagePixelReceiver::readData() : The next data of frame %d\n", shared->nodeID, instID, sbg->getFrameID());
#endif
						if (sbg->getFlag() == sageBlockGroup::PIXEL_DATA) {
							//std::cout << "new block (pixel data) " << rcvSize << " " << sbg->getFrameID() <<  std::endl;

							streamList[i].bGroup = sbg;
							configID = MAX(configID, sbg->getConfigID());
#ifdef DEBUG_RECEIVER
							fprintf(stderr, "\t[%d,%d] : and it's pixel. my configID is now %d\n", shared->nodeID, instID, configID);
#endif
						}
						else {
							//std::cout << "new block " << rcvSize << " " << sbg->getFrameID() <<  std::endl;
#ifdef DEBUG_RECEIVER
							fprintf(stderr, "\t[%d,%d] : and it's NOT pixel.\n", shared->nodeID, instID);
#endif

							reuseBlockGroup = true;
							if (sbg->getConfigID() > configID) {
#ifdef DEBUG_RECEIVER
								fprintf(stderr, "\t\t[%d,%d] : it's new config %d\n", shared->nodeID, instID, sbg->getConfigID());
#endif
								configID = sbg->getConfigID();
								streamList[i].bGroup = blockBuf->getCtrlGroup(sageBlockGroup::CONFIG_UPDATE);
								streamList[i].bGroup->setFrameID(sbg->getFrameID());
								streamList[i].bGroup->setConfigID(configID);
							}
							else {
								streamList[i].bGroup = NULL;
#ifdef DEBUG_RECEIVER
								fprintf(stderr, "\t\t[%d,%d] : it's what? streamList[%d].bGroup is now NULL\n", shared->nodeID, instID, i);
#endif
							}
						}
						streamList[i].curFrame = sbg->getFrameID();
#ifdef DEBUG_RECEIVER
						fprintf(stderr, "\t[%d,%d] : streamList[%d].curFrame updated to %d\n", shared->nodeID, instID, i, streamList[i].curFrame);
#endif
						FD_CLR(streamList[i].dataSockFd, &streamFds);
						//std::cout << ">>>>> " << hostname << " cleaer stream " << i << std::endl;
					}
					else {
						// sbg->getFrameID() < curFrame
						sage::printLog("sagePixelReceiver::readData() : received PAST frame!! (received %d, but curFrame %d)", sbg->getFrameID(), curFrame);
					}
				} // end if (rcvSize>0)

				else {
					sage::printLog("[%d,%d] sagePixelReceiver::readData() : rcvSize %d, exit loop", shared->nodeID, instID, rcvSize);
					endFlag = true;
					break;
				}
			} // end if (streamList[i].dataReady )

			// update nextFrame. nextFrame is reset to INT_MAX at the beginning of this while loop
#ifdef DEBUG_RECEIVER
			fprintf(stderr, "[%d,%d] sagePixelReceiver::readData() : Updating nextFrame = MIN(%d, %d)\n", shared->nodeID, instID,nextFrame, streamList[i].curFrame);
#endif
			nextFrame = MIN(nextFrame, streamList[i].curFrame);

		} // end for each sender

		if (nextFrame < SAGE_INT_MAX && nextFrame > curFrame) {
#ifdef DEBUG_RECEIVER
			fprintf(stderr, "[%d,%d] sagePixelReceiver::readData() : nextFrame %d > curFrame %d. curFrame is updated.\n", shared->nodeID, instID, nextFrame, curFrame);
#endif
			curFrame = nextFrame;
			int pushNum = 0;
			if (updated) {
#ifdef DEBUG_RECEIVER
				fprintf(stderr, "\t[%d,%d] sagePixelReceiver::readData() : marking END_FRAME\n", shared->nodeID, instID);
#endif
				blockBuf->finishFrame();
				pushNum++;
				//std::cout << "finish frame " << std::endl;

				if (blockBuf->isWaitingData())
					shared->eventQueue->sendEvent(EVENT_READ_BLOCK, instID);
				updated = false;
			}

			//std::cout << "frame num is updated....  " << curFrame << " " << senderNum  << std::endl;

			for (int i=0; i<senderNum; i++) {
				FD_SET(streamList[i].dataSockFd, &streamFds);
				sageBlockGroup *bGrp = streamList[i].bGroup;

				//std::cout << "pt2" << std::endl;
				if (bGrp && bGrp->getFrameID() == curFrame) {
					if (bGrp->getFlag() == sageBlockGroup::PIXEL_DATA)
						updated = true;
					blockBuf->pushBack(bGrp);
					//std::cout << "push back frame " << bGrp->getFrameID() << std::endl;
					pushNum++;
				}
			}

			//std::cout << "pt3" << std::endl;
			if (blockBuf->isWaitingData())
				shared->eventQueue->sendEvent(EVENT_READ_BLOCK, instID);
		} // end if (nextFrame)
	} // end while

   sage::printLog("sagePixelReceiver::readData() : exit reading thread");
   return 0;
}

sagePixelReceiver::~sagePixelReceiver()
{
   endFlag = true;
   blockBuf->releaseLock();

   for (int i=0; i<senderNum; i++) {
      if (nwObj)
         nwObj->close(streamList[i].senderID);
   }

   pthread_join(thId, NULL);

   delete [] streamList;
   sage::printLog("[%d,%d] sagePixelReceiver shutdown", shared->nodeID, instID);
}
