/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: audioConverter.h
 * Author : Hyejung Hur
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
 ***************************************************************************************************************************/

#ifndef AUDIO_CONVERTER_H
#define AUDIO_CONVERTER_H

#include <stdio.h>
#include "sageBase.h"

class audioConverter {
public:
   audioConverter();
   virtual ~audioConverter();

   virtual int init(FILE* fp);

   /** Returns a tuple (nchannels, sampwidth, framerate, nframes), 
       equivalent to output of the get*() methods ??? */   
   virtual int readHeader(int& nchan, long& samrate, int& nframes, sageSampleFmt& fmt)=0;
   virtual int writeHeader(int nchan, long samrate, int nframes, int bits)=0;   

   /** Returns a tuple (nchannels, sampwidth, framerate, nframes), 
       equivalent to output of the get*() methods ??? */
   int getParams(int& nchan, long& samrate, int& nframes, sageSampleFmt& fmt);

   /** Reads and returns at most n frames of audio, as a string of bytes */
   virtual int readFrames(void* frames) =0;
   /** Write n frames of audio, as a string of bytes */
   virtual int writeFrames(void* frames) =0;   

   /** ewind the file pointer to the beginning of the audio stream */ 
   virtual void begin();

   virtual void close()=0;
   
protected:
   FILE* fileID;
   
   unsigned int formatLength;
   short formatTag;
   short channels;
   unsigned int sampleRate;
   unsigned int avgBytesSec;
   short blockAlign;
   short bitsPerSample;   
   unsigned int dataSize;
   
   int framesPerBuffer;
   sageSampleFmt sampleFmt;
   unsigned int blockSize;
   unsigned int blockCount;
   unsigned int totalBlocks;
   
   fpos_t dataPos;
   int writeMode;
}; 

#endif
