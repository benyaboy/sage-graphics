/***************************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module:  sageEvent.h
 * Author : Byungil Jeong
 *
 *   Description: Queue for event driven control in a SAGE component
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
*****************************************************************************************/

#ifndef _SAGE_EVENT_H
#define _SAGE_EVENT_H

#include "sageBase.h"

#define SAGE_EVENT_SIZE 1280

#define EVENT_NEW_MESSAGE    100
#define EVENT_NEW_CONNECTION 101
#define EVENT_READ_BLOCK     102
#define EVENT_REFRESH_SCREEN 103
#define EVENT_SYNC_MESSAGE   104
#define EVENT_APP_CONNECTED  105
#define EVENT_AUDIO_CONNECTION 106

#define EVENT_SLAVE_PERF_INFO  200
#define EVENT_MASTER_PERF_INFO 201
#define EVENT_APP_SHUTDOWN     202
#define EVENT_BRIDGE_SHUTDOWN  203

/**
 * sageEvent
 */
class sageEvent {
public:
   int eventType;
   char eventMsg[SAGE_EVENT_SIZE];
   void *param;

   sageEvent() : eventType(0), param(NULL) {}
   sageEvent(int type, char* msg, void *p = NULL);

   void setMsg(char *msg) { if (msg) strcpy(eventMsg, msg); }
};

/**
 * sageEventQueue
 */
class sageEventQueue {
protected:
   std::deque<sageEvent*> eventQueue;

   pthread_mutex_t *queueLock;
   pthread_cond_t *notEmpty;

   bool empty;

public:
   sageEventQueue();

   sageEvent* getEvent();  // get event from the queue
   void sendEvent(sageEvent* event);
   void sendEvent(int type, char *msg = NULL, void *param = NULL);
   void sendEvent(int type, int info, void *param = NULL);

   bool isEmpty()  { return (eventQueue.size() == 0); }
   ~sageEventQueue();
};

#endif
