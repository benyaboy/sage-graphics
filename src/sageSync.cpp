/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageSync.cpp - This file contains the class which provides the syncing
 *            mechanism for keeping all the sage processes in sync.
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

#include "sageSync.h"
#include "sageBuf.h"

int syncGroup::init(int startFrame, int p, int groupID, int frameRate, int sNum)
{
   slaveNum = sNum;
   curFrame = startFrame;

   policy = p;
   id = groupID;
   setFrameRate((float)frameRate);

   syncMsgLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
   pthread_mutex_init(syncMsgLock, NULL);
   pthread_mutex_unlock(syncMsgLock);

   pthread_mutex_init(&syncLock, NULL);
   pthread_mutex_unlock(&syncLock);
   pthread_cond_init(&resumeSync, NULL);

   timer.reset();

   return 0;
}

int syncGroup::setFrameRate(float frameRate)
{
   if (frameRate > 0)
      interval = 1000000.0/frameRate; // in micro second
   else {
      sage::printLog("syncGroup::setFrameRate : can't set frame rate to zero");
      interval = 16666.6;
   }

   return 0;
}

bool syncGroup::checkInterval()
{
   double curTime = timer.getTimeUS();
   double maxErr = interval*MAX_INTERVAL_ERROR;
   double err = curTime - (interval - timeError);
   if (err > -maxErr) {
      if (timeError > maxErr)
         timeError = 0.0;
      else
         timeError = err;
      timer.reset();
      waitingInterval = false;
      return true;
   }

   waitingInterval = true;
   return false;
}

int syncGroup::processUpdate(char *data)
{
   //std::cerr << "process update " << data << std::endl;

   int groupID, frameNum, updateParam, updateType;
   sscanf(data, "%d %d %d %d", &groupID, &frameNum, &updateParam, &updateType);

   //fprintf(stderr, "syncGroup::processUpdate : for group %d, frameNum %d, updateParam %d, updateType %d \n", groupID, frameNum, updateParam, updateType);

   float hardness = 1.0;
   if (policy == SAGE_ASAP_SYNC_HARD)
      hardness = 1.0;
   else if (policy == SAGE_ASAP_SYNC_SOFT)
      hardness = 0.5;

   switch(updateType) {
      // used in SAGE Bridge only (no audio sync)
      case SAGE_UPDATE_FRAME: {
         noOfUpdates++;
         skipFrame = MAX(skipFrame, frameNum);

         if (noOfUpdates >= slaveNum*hardness) {
            curFrame = MAX(curFrame, skipFrame);
            noOfUpdates = 0;
            return NORMAL_SYNC;
         }

         break;
      }

      case SAGE_UPDATE_SETUP: {
         //sage::printLog("newFrame %d  curFrame %d\n", frameNum, curFrame);
    	  //sungwon
    	  //fprintf(stderr, "syncGroup::processUpdate(SAGE_UPDATE_SETUP) : newFrame %d, curFrame %d, rcvNum %d, noOfUpdates %d\n", frameNum, curFrame, updateParam, noOfUpdates);

         if (frameNum > curFrame) {
            videoFrame = frameNum;
            slaveNum = updateParam; // the number of receivers
            noOfUpdates++;

            if (noOfUpdates >= slaveNum*hardness) {
               if (audioFrame > 0 && frameNum >= keyFrame*audioSyncCnt && audioFrame < keyFrame*audioSyncCnt) {
                  waitForKeyFrame = true;
                  return 0;
               }
               else {
                  curFrame = frameNum; // update frame
                  noOfUpdates = 0; // reset counter
                  return NORMAL_SYNC; // will result calling sendSync() -> sending group's currentFrame
               }
            }
         }
         break;
      }

      case SAGE_UPDATE_FOLLOW: {
    	  //sungwon
    	  //fprintf(stderr, "syncGroup::processUpdate(SAGE_UPDATE_FOLLOW) : newFrame %d, curFrame %d, rcvNum %d, noOfUpdates %d\n", frameNum, curFrame, updateParam, noOfUpdates);

         if (frameNum > curFrame) {
            videoFrame = frameNum;
            noOfUpdates++;

            if (noOfUpdates >= slaveNum*hardness) {
               if (audioFrame > 0 && frameNum >= keyFrame*audioSyncCnt && audioFrame < keyFrame*audioSyncCnt) {
                  waitForKeyFrame = true;
                  return 0;
               }
               else {
                  curFrame = MAX(skipFrame, frameNum);
                  noOfUpdates = 0;
                  return NORMAL_SYNC;
               }
            }
         }

         break;
      }

      case SAGE_UPDATE_AUDIO: {
		    //sungwon
   		//fprintf(stderr, "syncGroup::processUpdate(SAGE_UPDATE_AUDIO) : for group %d, frameNum %d, updateParam %d, updateType %d \n", groupID, frameNum, updateParam, updateType);


         audioFrame = frameNum;
         keyFrame = updateParam;
         audioSyncCnt++;
         //std::cerr << "syncGroup::processUpdate : audio " << audioFrame << " video " << videoFrame << "noOfUpdate " << noOfUpdates << "slaveNum " << slaveNum << "curFrame " << curFrame << "videoFrame " << videoFrame << std::endl;

         if (waitForKeyFrame) {
            waitForKeyFrame = false;
            //std::cerr << "audio " << audioFrame << " video " << videoFrame << std::endl;
            if (policy == SAGE_ASAP_SYNC_HARD) {
               if (noOfUpdates == slaveNum) {
                  curFrame = videoFrame;
                  noOfUpdates = 0;
                  return NORMAL_SYNC;
               }
            }
            else if (policy == SAGE_ASAP_SYNC_SOFT) {
               if (noOfUpdates >= slaveNum*0.5) {
                  curFrame = videoFrame;
                  noOfUpdates = 0;
                  return NORMAL_SYNC;
               }
            }
         }
      }
   }

   return 0;
}

/*
bool syncGroup::checkTimeOut()
{
   if (noOfUpdates > 0 && timer.getTimeSec() > 1.0) {
      curFrame++;
      noOfUpdates = 0;
      timer.reset();
      sage::printLog("sync group %d : time out", id);
      return true;
   }

   return false;
}
*/

void syncGroup::checkHold()
{
   pthread_mutex_lock(&syncLock);
   while(holdSync)
      pthread_cond_wait(&resumeSync, &syncLock);
   pthread_mutex_unlock(&syncLock);
}

void syncGroup::blockSync()
{
   pthread_mutex_lock(&syncLock);
   holdSync = true;
   pthread_mutex_unlock(&syncLock);
}

void syncGroup::unblockSync()
{
   pthread_mutex_lock(&syncLock);
   holdSync = false;
   pthread_mutex_unlock(&syncLock);
   pthread_cond_signal(&resumeSync);
}

int syncGroup::enqueSyncMsg(char *msg)
{
   char *syncMsg = new char[strlen(msg)+1];
   strcpy(syncMsg, msg);

   pthread_mutex_lock(syncMsgLock);
   syncMsgQueue.push_back(syncMsg);
   pthread_mutex_unlock(syncMsgLock);

   return 0;
}

char* syncGroup::dequeSyncMsg()
{
   char *msg = NULL;
   pthread_mutex_lock(syncMsgLock);
   if (syncMsgQueue.size() > 0) {
      msg = syncMsgQueue.front();
      syncMsgQueue.pop_front();
   }
   pthread_mutex_unlock(syncMsgLock);

   return msg;
}

//--------------------------------------------  S E R V E R   C O D E  ---------------------------------------------//

sageSyncServer::sageSyncServer() : asapSyncGroupNum(0), maxSyncGroupID(-1), syncEnd(false),
      maxSlaveSockFd(0)
{
   //for (int i=0; i<MAX_SYNC_GROUP; i++) {
   //   syncGroupArray[i] = NULL;
   //}

   FD_ZERO(&slaveFds);
}

int sageSyncServer::init(int port)
{
   int optVal, optLen;

   // create the server socket
   if((serverSockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      sage::printLog("sageSyncServer::init(): Creating server socket failed");
      return -1;
   }

   // loosen the rules for check during bind to allow mutiple binds on the same port
   optVal=1; optLen = sizeof(optVal);

#if defined(WIN32) || defined(__APPLE__) || defined (__sun)
   if(setsockopt(serverSockFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optLen) !=0)
      sage::printLog("\nsageSyncServer::init(): Error setting SO_REUSEADDR");

   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(serverSockFd, IPPROTO_TCP, TCP_NODELAY, (const char*)&optVal, optLen) !=0)
      sage::printLog("\nsageSyncServer::init(): Error switching off Nagle's algorithm.");

   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(serverSockFd, SOL_SOCKET, SO_OOBINLINE, (const char*)&optVal, optLen) !=0)
      sage::printLog("\nsageSyncServer::init(): Error switching on OOBINLINE.");

#else
   if(setsockopt(serverSockFd, SOL_SOCKET, SO_REUSEADDR, (void*)&optVal, (socklen_t)optLen) !=0)
      perror("sageSyncServer::init():: Error setting SO_REUSEADDR");

   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(serverSockFd, SOL_TCP, TCP_NODELAY, (void*)&optVal, (socklen_t)optLen) !=0)
      sage::printLog("\nsageSyncServer::init(): Error switching off Nagle's algorithm.");

   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(serverSockFd, SOL_SOCKET, SO_OOBINLINE, (const char*)&optVal, optLen) !=0)
      sage::printLog("\nsageSyncServer::init(): Error switching on OOBINLINE.");

#endif

   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serverAddr.sin_port = htons(port);

   if (bind(serverSockFd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) != 0) {
      sage::printLog("sageSyncServer::init():: Error binding server socket");
      return -1;
   }

   // put in listen mode
   listen(serverSockFd, 10);

   // create the syncServerThread
   if (pthread_create(&syncThreadID, NULL, syncServerThread, (void*) this) !=0) {
      sage::printLog("sageSyncServer::init(): Creating sync thread failed");
      return -1;
   }

   //timer.reset();

   return 0;
}

// The sync server thread, which is responsible for listening for new sync clients
// This thread also polls each connected slave to look for frame updates
// and then updates the slave data vector

void* sageSyncServer :: syncServerThread(void *args)
{
   sageSyncServer *This;
   This = (sageSyncServer *)args;

   int tempSockFd;
   int addrLen;
   int optVal,optLen;

   sockaddr_in clientAddr;

   while (!This->syncEnd)   {

      // accept connections
      addrLen = sizeof(clientAddr);

      if ((tempSockFd = accept(This->serverSockFd, (struct sockaddr *)&clientAddr,
            (socklen_t*)&addrLen)) == -1) {
         sage::printLog("sageSyncServer::syncServerThread()::Quitting sync server thread.");
         pthread_exit(NULL);
         continue;
      }

#if defined(WIN32) || defined(__APPLE__) || defined (__sun)
      optVal = 1;
      optLen=sizeof(optVal);
      if(setsockopt(tempSockFd, IPPROTO_TCP, TCP_NODELAY, (const char*)&optVal, optLen) !=0)
         sage::printLog("\nsageSyncServer: Error switching off Nagle's algorithm.");

      optVal = 1;
      optLen=sizeof(optVal);
      if(setsockopt(tempSockFd, SOL_SOCKET, SO_OOBINLINE, (const char*)&optVal, optLen) !=0)
         sage::printLog("\nsageSyncServer: Error switching on OOBINLINE.");

#else
      optVal = 1;
      optLen=sizeof(optVal);
      if(setsockopt(tempSockFd, SOL_TCP, TCP_NODELAY, (void*)&optVal, (socklen_t)optLen) !=0)
         sage::printLog("\nsageSyncServer: Error switching off Nagle's algorithm.");

      optVal = 1;
      optLen=sizeof(optVal);
      if(setsockopt(tempSockFd, SOL_SOCKET, SO_OOBINLINE, (const char*)&optVal, optLen) !=0)
         sage::printLog("\nsageSyncServer: Error switching on OOBINLINE.");

#endif

      // add the client's info to the list
      syncSlaveData newClient;
      newClient.clientSockFd = tempSockFd;
      memcpy((void *)&(newClient.clientAddr), (void *)&clientAddr, sizeof(clientAddr));
      This->syncSlaves.push_back(newClient);
      This->maxSlaveSockFd = MAX(This->maxSlaveSockFd, tempSockFd);
      FD_SET(tempSockFd, &This->slaveFds);
   }

   //pacify compiler
   return NULL;
} // end of sageSyncServer :: syncServerThread()

int sageSyncServer::sendSync(syncGroup *grp, int cmd)
{
   if (!grp) {
      sage::printLog("sageSyncServer::sendSync : invalid sync group ID");
      return -1;
   }

   char *data = grp->dequeSyncMsg();

   int msgSize = SAGE_SYNC_MSG_LEN;
   char msg[SAGE_SYNC_MSG_LEN];
   int dataLen = 0;

   if (data)
      dataLen = strlen(data) + 1;
   sprintf(msg, "%d %d %d %d", grp->id, grp->curFrame, dataLen, cmd);

   //std::cout << "send sync " << msg << std::endl;

   int noOfSyncSlaves = syncSlaves.size();

   // send the sync message to all the slaves listed with the server
   for (int i=0; i<noOfSyncSlaves; i++) {
      // Check to see if the client has been removed
      if (syncSlaves[i].clientSockFd == -1)
         continue;


      //sungwon
      //fprintf(stderr, "sageSyncServer::sendSync: sending sync msg to slave %d, (%s)\n", i, msg);
      //fflush(stderr);

      /// sends sync message here
      int sentSize = sage::send(syncSlaves[i].clientSockFd, (void *)msg, msgSize);

      if (sentSize == 0) {
         sage::printLog("sageSyncServer::sendSync : connection shutdown - client ",
               (char *)inet_ntoa(syncSlaves[i].clientAddr.sin_addr));
         // Assuming that something went wrong with the socket connection. Closing it and marking it as dead
#ifdef WIN32
         closesocket(syncSlaves[i].clientSockFd);
#else
         shutdown(syncSlaves[i].clientSockFd, SHUT_RDWR);
         close(syncSlaves[i].clientSockFd);
#endif
         syncSlaves[i].clientSockFd = -1;
         return -1;
      }
      else if (sentSize < 0) {
         return -1;
      }
      else if (data) {

    	  //sungwon
	  //fprintf(stderr, "sageSyncServer::sendSync: syncGroup %d has data(%s) in sync msg\n", grp->id, data);

         sage::send(syncSlaves[i].clientSockFd, (void *)data, dataLen);
      }
   }

   if (data)
      delete [] data;

   return 0;
}

int sageSyncServer::removeSyncGroup(int id)
{
   /*if (id < 0) {
      sage::printLog("sageSyncClient::removeSyncGroup : group ID is out of scope");
      return -1;
   }*/
	int index;
	syncGroup* grp= findSyncGroup(id, index);
	if(grp)
	{
		//std::cout << "sageSyncServer: found... trying to kill... thread.." << std::endl;
		// kill thread... 
		grp->syncEnd = true;
		pthread_join(grp->threadID, NULL);

		// remove object
		delete grp;
		grp = NULL;
		syncGroupArray.erase(syncGroupArray.begin() + index);
	}
	asapSyncGroupNum--;
	return 0;
}

int sageSyncServer::addSyncGroup(syncGroup *grp)
{
   grp->syncServer = (sageSyncServer *)this;
   if (grp->id >= 0) {
		 //sungwon
		//fprintf(stderr, "sageSyncServer::addSyncGroup: adding group %d\n", grp->id);

      //syncGroupArray[grp->id] = grp;
      syncGroupArray.push_back(grp);
   }
   else {
      sage::printLog("sageSyncServer::addSyncGroup : invalid sync group ID");
      return -1;
   }

	//std::cout << "sageSyncServer: Add Sync Group : " << grp->id << std::endl;
	maxSyncGroupID = MAX(grp->id, maxSyncGroupID);

   if (grp->policy == SAGE_CONSTANT_SYNC) {
      if (pthread_create(&grp->threadID, NULL, managerThread, (void*)grp) !=0) {
         sage::printLog("sageSyncServer::init(): Creating manager thread failed");
         return -1;
      }
   }
   else {
      if (asapSyncGroupNum == 0) {
         if (pthread_create(&grp->threadID, NULL, managerThread, (void*)grp) !=0) {
            sage::printLog("sageSyncServer::init(): Creating manager thread failed");
            return -1;
         }
      }
      asapSyncGroupNum++;
   }

   return 0;
}

void* sageSyncServer::managerThread(void *args)
{
   syncGroup *grp = (syncGroup *)args;
   sageSyncServer *This = (sageSyncServer *)grp->syncServer;

	//sungwon
	//fprintf(stderr, "sageSyncServer::managerThread: started for group %d\n", grp->id);

   while (!grp->syncEnd) {
      if (grp->policy == SAGE_CONSTANT_SYNC) {
         This->sendSync(grp);
         sage::usleep((int)floor(grp->interval+0.5));
      }
      else
         This->manageUpdate();
   }

   //sungwon
   //fprintf(stderr, "sageSyncServer::managerThread: for group %d exiting\n", grp->id);

   pthread_exit(NULL);
   return NULL;
}

syncGroup* sageSyncServer::findSyncGroup(int id, int& index)
{
	syncGroup *grp = NULL;
	index =0;
	std::vector<syncGroup *>::iterator iter;
	for(iter = syncGroupArray.begin(); iter != syncGroupArray.end(); iter++, index++)
	{
		if((*iter)->getSyncID() == id)
		{
			grp = (syncGroup*) (*iter);
			return grp;
		}
	}
	index = -1;
	return NULL;
}

int sageSyncServer::manageUpdate()
{
   int noOfSyncSlaves = syncSlaves.size();
   char data[SAGE_SYNC_MSG_LEN];
   fd_set readFds = slaveFds;

   struct timeval timeOut;
   timeOut.tv_sec = 0;
   timeOut.tv_usec = SYNC_TIMEOUT;

   //std::cout << "waiting update" << std::endl;
   int returnVal = select(maxSlaveSockFd+1, &readFds, NULL, NULL, &timeOut);

   if (returnVal < 0) {
      sage::printLog("sageSyncServer::manageUpdate : error in waiting update message");
      return -1;
   }

   /**
    * Each sync group has its own maximum frame rate (or minimum interval).
Even though a sync group is ready to proceed to the next frame,
the sync master waits until the minimum interval is reached.
Thus, a sync group can have two states:
waiting for updates from slaves or waiting for the minimum interval.
This code fragment was inserted to take care of the latter cases.

If syncGroup::isWaiting() is true, the sync group has the latter state.
syncGroup::checkInterval() returns if it reaches the interval or not.

This code checks there is a sync group
which is waiting for the interval.
If yes and the interval is reached,
it sends sync signal to the group members immediately
because all the sync slaves are already ready to proceed the next frame.
    */
   //for (int i=0; i<=maxSyncGroupID; i++) {
   //   syncGroup *grp = syncGroupArray[i];
	syncGroup *grp = NULL;
	int maxSyncGroup = syncGroupArray.size();
   for (int i=0; i< maxSyncGroup; i++) {
		grp = syncGroupArray[i];

      // if it's waiting to reach the interval
      if (grp && grp->isWaiting()) {

    	  // interval reached?
         if (grp->checkInterval()) {

        	 // if this syncGroup's sync is holding then waits on the condition resumeSync
            grp->checkHold();

            // send sync signal
            sendSync(grp);
         }
      }
   }

   for (int i=0; i < noOfSyncSlaves; i++)   {
      // Check to see if the client has been removed
      if (syncSlaves[i].clientSockFd == -1)
         continue;

      if (FD_ISSET(syncSlaves[i].clientSockFd, &readFds)) {
         int status = sage::recv(syncSlaves[i].clientSockFd, (void*)data, SAGE_SYNC_MSG_LEN);

         //sungwon
         //fprintf(stderr, "sageSyncServer::manageUpdate: Received msg from slave %d, (%s)\n", i, data);

         if (status == 0) {
            // close the socket and mark it as dead
            #ifdef WIN32
            closesocket(syncSlaves[i].clientSockFd);
            #else
            shutdown(syncSlaves[i].clientSockFd, SHUT_RDWR);
            close(syncSlaves[i].clientSockFd);
            #endif
            FD_CLR(syncSlaves[i].clientSockFd, &slaveFds);
            syncSlaves[i].clientSockFd = -1;
         }
         else {
            int groupID;
            sscanf(data, "%d", &groupID);
            //syncGroup *grp = syncGroupArray[groupID];
				int index;
				syncGroup *grp = findSyncGroup(groupID, index);

            if (!grp) {
               sage::printLog("sageSyncServer::manageUpdate : invalid groupID %d", groupID);
               return -1;
            }

            //sungwon
            //fprintf(stderr, "sageSyncServer::manageUpdate: calling grp %d's processUpdate\n", grp->id);

            int cmd = grp->processUpdate(data);
            if (cmd > 0) {
               //fprintf(stderr, "send sync %d\n", grp->curFrame);
               if (grp->checkInterval()) {
                  grp->checkHold();

            //sungwon
            //fprintf(stderr, "sageSyncServer::manageUpdate: calling sendSync(%d, %d)\n", grp->id, cmd);

                  sendSync(grp, cmd);
               }
            }
         }
      } // end if
   } // end for

   //if (timer.getTimeSec() > 1.0) {
   //   checkTimeOut();
   //   timer.reset();
   //}

   return 0;
}

/*
int sageSyncServer::checkTimeOut()
{
   for (int i=0; i<=maxSyncGroupID; i++) {
      syncGroup *grp = syncGroupArray[i];
      if (grp && grp->checkTimeOut()) {
         grp->checkHold();
         sendSync(grp);
      }
   }

   return 0;
}

int sageSyncServer::queueData(char *data, int id)
{
   char *syncMsg = new char[strlen(data)+1];
   strcpy(syncMsg, data);
   syncGroupList[id]->syncMsgQueue.push_back(syncMsg);

   return 0;
}
*/

void sageSyncServer::killAllClients()
{
   int noOfSyncSlaves = syncSlaves.size();
   int i;

   for(i=0; i< noOfSyncSlaves; i++)   {
      // Check to see if the client has been removed
      if(syncSlaves[i].clientSockFd == -1) continue;

#ifdef WIN32
      closesocket(syncSlaves[i].clientSockFd);
#else
      shutdown(syncSlaves[i].clientSockFd, SHUT_RDWR);
      close(syncSlaves[i].clientSockFd);
#endif

   }

   noOfSyncSlaves = 0;
}


sageSyncServer :: ~sageSyncServer()
{
#ifdef WIN32
   closesocket(serverSockFd);
#else
   shutdown(serverSockFd, SHUT_RDWR);
   close(serverSockFd);
#endif

   int noOfSyncSlaves = syncSlaves.size();
   int i;

   for (i=0; i< noOfSyncSlaves; i++)   {
      // Check to see if the client has been removed
      if(syncSlaves[i].clientSockFd == -1)
         continue;

#ifdef WIN32
      closesocket(syncSlaves[i].clientSockFd);
#else
      shutdown(syncSlaves[i].clientSockFd, SHUT_RDWR);
      close(syncSlaves[i].clientSockFd);
#endif

   }

   //wait for the thread to quit
   pthread_join(syncThreadID, NULL);

} // End of ~sageSyncServer()


//--------------------------------------------  C L I E N T   C O D E  ---------------------------------------------//




sageSyncClient::sageSyncClient() : maxGroupID(-1), syncEnd(false)
{
   int optVal, optLen;

   // create the client socket
   if((clientSockFd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
   {
      perror("sageSyncClient::sageSyncClient(): Creating server socket failed");
   }

   // loosen the rules for check during bind to allow mutiple binds on the same port
   optVal=1; optLen = sizeof(optVal);

#if defined(WIN32) || defined(__APPLE__) || defined (__sun)
   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(clientSockFd, IPPROTO_TCP, TCP_NODELAY, (const char*)&optVal, optLen) !=0)
      sage::printLog("\nsageSyncClient::sageSyncClient(): Error switching off Nagle's algorithm.");

   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(clientSockFd, SOL_SOCKET, SO_OOBINLINE, (const char*)&optVal, optLen) !=0)

   sage::printLog("\nsageSyncClient::sageSyncClient(): Error switching on OOBINLINE.");

#else

   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(clientSockFd, SOL_TCP, TCP_NODELAY, (void*)&optVal, (socklen_t)optLen) !=0)
      sage::printLog("\nsageSyncClient::sageSyncClient(): Error switching off Nagle's algorithm.");

   optVal = 1;
   optLen=sizeof(optVal);
   if(setsockopt(clientSockFd, SOL_SOCKET, SO_OOBINLINE, (const char*)&optVal, optLen) !=0)
      sage::printLog("\nsageSyncClient::sageSyncClient(): Error switching on OOBINLINE.");

#endif

} // End of sageSyncClient :: sageSyncClient()

int sageSyncClient::connectToServer(char *syncServIP, int port)
{
   int noOfConnAttempts = 0;
   sockaddr_in tempAddr;

   memset(&tempAddr, 0, sizeof(tempAddr));
   tempAddr.sin_family = AF_INET;
   tempAddr.sin_addr.s_addr = inet_addr(syncServIP);
   tempAddr.sin_port = htons(port);

   // try a few times before giving up
   //connect to the server
   while (noOfConnAttempts < 50) {
      if (connect(clientSockFd, (struct sockaddr *)&tempAddr, sizeof(struct sockaddr)) != 0) {
         char msg[TOKEN_LEN];
         sprintf(msg, "sageSyncClient :: connectToServer(): sync server %s : %d is not ready\n",
            syncServIP, port);
         sage::printLog(msg);
         noOfConnAttempts++;
      }
      else
         return 0;

      sage::sleep(1);
   }

   sage::printLog("sageSyncClient :: connectToServer(): Giving up after trying a few times");

   return -1;
}

int sageSyncClient::addSyncGroup(int id)
{
   if (id < 0 || id >= MAX_SYNC_GROUP) {
      sage::printLog("sageSyncClient::addSyncGroup : group ID is out of scope");
      return -1;
   }

   sageCircBufSingle* buf = new sageCircBufSingle(SYNC_MSG_BUF_LEN, true);
	buf->instID = id;
   syncMsgBuf.push_back(buf);

   if (maxGroupID < 0) {
      maxGroupID = id;

      //sungwon
      //fprintf(stderr, "sageSyncClient::addSyncGroup: starting syncClientThread for the group %d\n", id);

      if (pthread_create(&syncThreadID, NULL, syncClientThread, (void*) this) !=0) {
         sage::printLog("sageSyncClient::addSyncGroup(): Creating sync client thread failed");
         return -1;
      }
   }
   else
      maxGroupID = MAX(maxGroupID, id);

   std::cout << "added sync group " << id << std::endl;

   return 0;
}

int sageSyncClient::removeSyncGroup(int id)
{
   if (id < 0 || id > maxGroupID) {
      sage::printLog("sageSyncClient::removeSyncGroup : group ID is out of scope");
      return -1;
   }

	int index;
	sageCircBufSingle* buf = findSyncGroup(id, index);
	if(buf) 
	{
		//std::cout << "sageSyncClient: found... trying to kill... thread.." << std::endl;
		buf->releaseLock();
		delete buf;
		buf = NULL;
		syncMsgBuf.erase(syncMsgBuf.begin() + index);
		//std::cout << "...." << std::endl;
	}
   //if (syncMsgBuf[id])
   //   syncMsgBuf[id]->releaseLock();

   return 0;
}

sageCircBufSingle* sageSyncClient::findSyncGroup(int id, int& index)
{
	sageCircBufSingle *buf = NULL;
	index =0;
	std::vector<sageCircBufSingle *>::iterator iter;
	for(iter = syncMsgBuf.begin(); iter != syncMsgBuf.end(); iter++, index++)
	{
		if((*iter)->instID == id)
		{
			buf = (sageCircBufSingle*) (*iter);
			return buf;
		}
	}
	index = -1;
	return NULL;
}

void* sageSyncClient::syncClientThread(void* args)
{
   //sungwon
   //fprintf(stderr, "sageSyncClient::syncClientThread started\n");

   sageSyncClient *This = (sageSyncClient *)args;

   while (!This->syncEnd) {
      if (This->readSyncMsg() < 0)
         This->syncEnd = true;
   }

   pthread_exit(NULL);
   return NULL;
}

int sageSyncClient::sendSlaveUpdate(int frame, int id, int rcvNum, int type)
{
   int dataSize = SAGE_SYNC_MSG_LEN;
   char msg[SAGE_SYNC_MSG_LEN];
   sprintf(msg, "%d %d %d %d", id, frame, rcvNum, type);

   //sungwon
   //fprintf(stderr, "sageSyncClient::sendSlaveUpdate: sending update (%s)\n", msg);

   int status = sage::send(clientSockFd, (void *)msg, dataSize);

   if (status !=  dataSize) {
      perror("sageSyncClient :: sendSlaveUpdate(): Error sending update message to sync master");
      return -1;
   }

   return 0;
} //End of sageSyncClient :: sendSlaveUpdate()

int sageSyncClient::readSyncMsg()
{
   int dataSize = SAGE_SYNC_MSG_LEN;
   char msg[SAGE_SYNC_MSG_LEN];
   int groupID = -1;

   // read a message but leave it in socket buffer
   int status = sage::recv(clientSockFd, (void*)msg, dataSize);

   //sungwon
   //fprintf(stderr, "sageSyncClient::readSyncMsg: recved msg (%s)\n", msg);

   if (status !=  dataSize) {
      perror("sageSyncClient :: waitForSync(): Error receiving sync from master");
      sage::sleep(1);
      return -1;
   }

   int frameNum, dataLen;
   sscanf(msg, "%d %d %d", &groupID, &frameNum, &dataLen);

   if (groupID >= 0) {
      syncMsgStruct *syncMsg;

      if (dataLen > 0) {
         syncMsg = new syncMsgStruct(dataLen);
         sage::recv(clientSockFd, (void*)syncMsg->data, dataLen);
      }
      else {
         syncMsg = new syncMsgStruct;
      }

      syncMsg->frameID = frameNum;

      //sageCircBufSingle *msgBuf = syncMsgBuf[groupID];
		int index;
      sageCircBufSingle *msgBuf = findSyncGroup(groupID, index);
      if (msgBuf && msgBuf->isActive()) {
         if (!msgBuf->pushBack((sageBufEntry)syncMsg)) {
            sage::printLog("sageSyncClient::readSyncMsg : sync message buffer is full");
            delete syncMsg;
         }
      }
      else
         delete syncMsg;
   }
   else
      sage::printLog("sageSyncClient::readSyncMsg : invalid group ID");

   return 0;
}

// receive sync message with group ID and additional data
syncMsgStruct* sageSyncClient::waitForSync(int id)
{
   if (id < 0 || id >= MAX_SYNC_GROUP) {
      sage::printLog("sageSyncClient::waitForSync : group ID is out of scope");
      return NULL;
   }

   //sageCircBufSingle *msgBuf = syncMsgBuf[id];
	int index;
   sageCircBufSingle *msgBuf = findSyncGroup(id, index);

   if (msgBuf) {
      syncMsgStruct *syncMsg = (syncMsgStruct *)msgBuf->front();
      if (msgBuf->isActive()) {
         if (!syncMsg) {
            sage::printLog("sageSyncClient::waitForSync : syncMsg is NULL");
         }
         msgBuf->next();

         return syncMsg;
      }
   }

   return NULL;
}

// receive sync message with additional data
int sageSyncClient::waitForSyncData(char* &data)
{
   int dataSize = SAGE_SYNC_MSG_LEN;
   char msg[SAGE_SYNC_MSG_LEN];
   int groupID = -1;
   int frameNum, dataLen;
   int status = sage::recv(clientSockFd, (void*)msg, dataSize);

   if (status !=  dataSize) {
      sage::printLog("sageSyncClient :: waitForSync(): Error receiving sync from master");
      sage::sleep(1);
      return -1;
   }

   sscanf(msg, "%d %d %d", &groupID, &frameNum, &dataLen);

   if (dataLen > 0) {
      data = new char[dataLen];
      sage::recv(clientSockFd, (void*)data, dataLen);
   }

   return frameNum;
}

// receive sync message with no additional data
int sageSyncClient::waitForSync(char* msg)
{
   int dataSize = SAGE_SYNC_MSG_LEN;

   int status = sage::recv(clientSockFd, (void*)msg, dataSize);

   if (status !=  dataSize) {
      perror("sageSyncClient :: waitForSync(): Error receiving sync from master");
      sage::sleep(1);
      return -1;
   }

   return 0;
}

int sageSyncClient::checkSync(char* &msg)
{
   int dataSize = SAGE_SYNC_MSG_LEN;

   if (sage::isDataReady(clientSockFd)) {
      int status=sage::recv(clientSockFd, (void*)msg, dataSize);
      if (status !=  dataSize) {
         perror("sageSyncClient :: checkSync(): Error receiving sync from master");
         sage::sleep(1);
         return -1;
      }

      return 1;
   }
   else
      return 0;
}

sageSyncClient :: ~sageSyncClient()
{
   syncEnd = true;

#ifdef WIN32
   closesocket(clientSockFd);
#else
   shutdown(clientSockFd, SHUT_RDWR);
   close(clientSockFd);
#endif

   pthread_join(syncThreadID, NULL);

	int maxGroupID = syncMsgBuf.size();
   for (int i=0; i<maxGroupID; i++)
      if (syncMsgBuf[i])
         delete syncMsgBuf[i];
}

sageThreadSync::sageThreadSync(int sNum)
{
   slaveNum = sNum;
   masterLock = new pthread_mutex_t[slaveNum];
   slaveLock = new pthread_mutex_t[slaveNum];

   for (int i=0; i<slaveNum; i++) {
      pthread_mutex_init(&masterLock[i], NULL);
      pthread_mutex_lock(&masterLock[i]);
   }

   for (int i=0; i<slaveNum; i++) {
      pthread_mutex_init(&slaveLock[i], NULL);
      pthread_mutex_lock(&slaveLock[i]);
   }
}

sageThreadSync::~sageThreadSync()
{
   if (masterLock)
      delete [] masterLock;

   if (slaveLock)
      delete [] slaveLock;
}

int sageThreadSync::synchronize(int rank)
{
   //sage::printLog("rank " << rank << std::endl << "slave num " << slaveNum);
   if (rank == 0) {
      for (int i=0; i<slaveNum; i++) {
         pthread_mutex_unlock(&slaveLock[i]);
      }
      //sage::printLog("unlock slave lock");
      //sage::printLog("waiting master lock");
      for (int i=0; i<slaveNum; i++) {
         pthread_mutex_lock(&masterLock[i]);
      }
      //sage::printLog("obtain master lock");
   }
   else if (rank <= slaveNum) {
      //sage::printLog("==== waiting slave lock");
      pthread_mutex_lock(&slaveLock[rank-1]);
      //sage::printLog("==== obtain slave lock");
      pthread_mutex_unlock(&masterLock[rank-1]);
      //sage::printLog("==== unlock master lock");
   }

   return 0;
}






