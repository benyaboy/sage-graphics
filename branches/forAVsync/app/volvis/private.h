/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
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
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#if defined(WIN32)
#include <string.h>
#else
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#define QUANTA_LITTLE_ENDIAN 1
#define QUANTA_THREAD_SAFE   1
#define QUANTA_USE_PTHREADS  1
#include <QUANTA.hxx>

#if !defined(WIN32)

#define USE_RBUDP 1

#if defined(USE_RBUDP)
#define RBUDP_RATE (300*1000)
#define RBUDP_FRAME 1452
//#define RBUDP_FRAME 8800
#include <QUANTAnet_rbudpSender_c.hxx>
#include <QUANTAnet_rbudpReceiver_c.hxx>
#endif

#endif

// NETWORK PROTOCOL
#define Q_INIT     0
#define Q_BYE      1
#define Q_LIST     2
#define Q_LOAD     3
#define Q_CROP     4
#define Q_SAMPLE   5
#define Q_ISO      6
#define Q_VOL      7
#define Q_POINT    8
#define Q_EXIT     9
#define Q_HISTO   10
#define Q_GRAD    11
#define Q_HISTO2D 12

