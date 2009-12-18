/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sageBlock.cpp - a few geometrical operations for rectangles used in SAGE
 * Author : Byungil Jeong, Luc Renambot
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

#include "sageBlock.h"

void sagePixelData::operator=(sageRect &rect)
{
   sageRect::operator=(rect);
}

int sagePixelData::initBuffer()
{
   int size = (int)ceil(width*height*bytesPerPixel/(compressX*compressY)) + BLOCK_HEADER_SIZE;

   if (size <= BLOCK_HEADER_SIZE) {
      sage::printLog("sagePixelBlock::initBuffer : incorrect block size");
      return -1;
   }
   
   releaseBuffer();

   if (allocateBuffer(size) < 0)
      return -1;

   //std::cout << "allocate " << size << " bytes" << std::endl;
   pixelData = buffer + BLOCK_HEADER_SIZE;
   
   return 0;
}

int sagePixelData::releaseBuffer()
{
   //std::cout << "release buffer" << std::endl;
   if (buffer) 
      free((void *)buffer);

   return 0;
}

int sagePixelData::allocateBuffer(int size)
{
   buffer = (char *)malloc(size);
   if (!buffer) {
      sage::printLog("sageBlock::allocateBuffer : fail to allocate %d bytes", size);
      return -1;
   }
   bufSize = size;      
   
   return 0;
}

sagePixelBlock::sagePixelBlock(int size) : valid(false), dirty(false), headerLen(0)
{
   flag = SAGE_PIXEL_BLOCK;
   allocateBuffer(size);

   //std::cout << "allocate " << size << " bytes" << std::endl;
   pixelData = buffer + BLOCK_HEADER_SIZE;
}

sagePixelBlock::sagePixelBlock(sagePixelBlock &block) 
{
   allocateBuffer(block.bufSize);
   pixelData = buffer + BLOCK_HEADER_SIZE;
   
   memcpy(buffer, block.buffer, bufSize);
   updateBlockConfig();
}

int sagePixelBlock::updateBufferHeader()
{   
   if (!buffer) {
      sage::printLog("sagePixelBlock::updateBufferHeader : buffer is null");   
      return -1;
   }
   
   memset(buffer, 0, BLOCK_HEADER_SIZE);
   headerLen = sprintf(buffer, "%d %d %d %d %d %d %d %d %ld %ld",
         bufSize, flag, x, y, width, height, frameID, blockID, timestamp_s, timestamp_u);
         
   //std::cout << "buf : " << buffer << std::endl;
   if (headerLen >= BLOCK_HEADER_SIZE) {
      sage::printLog("sagePixelBlock::updateBufferHeader : block header exceeds the maximum length");
      return -1;
   }
   
   return 0;
}

int sagePixelBlock::updateHeader(int pid, int configID)
{   
   if (!buffer) {
      sage::printLog("sagePixelBlock::updateBufferHeader : buffer is null");   
      return -1;
   }
   
   memset(buffer, 0, BLOCK_HEADER_SIZE);
   headerLen = sprintf(buffer, "%d %d %d %d %d %d %d %d %d %d &ld %ld",
         bufSize, flag, x, y, width, height, frameID, blockID, pid, configID, timestamp_s, timestamp_u);
         
   //std::cout << "buf : " << buffer << std::endl;
   if (headerLen >= BLOCK_HEADER_SIZE) {
      sage::printLog("sagePixelBlock::updateBufferHeader : block header exceeds the maximum length");
      return -1;
   }
   
   return 0;
}

bool sagePixelBlock::updateBlockConfig()
{
   if (!buffer) {
      sage::printLog("sagePixelBlock::updateBlockConfig : buffer is null");
      return false;
   }
   
   sscanf(buffer, "%d %d %d %d %d %d %d %d %ld %ld", &bufSize, &flag, &x, &y, &width, &height, 
               &frameID, &blockID, &timestamp_s, &timestamp_u);

   //std::cout << "buf : " << buffer << std::endl;
	//std::cout << "timestamp : get = " << timestamp_s << " " << timestamp_u << std::endl;
         
   return true;
}

void sagePixelBlock::clearPixelBlock()
{
   dirty = false;
   valid = true;
   clearBuffer();
}

sagePixelBlock::~sagePixelBlock()
{
   releaseBuffer();
}

sageAudioBlock::sageAudioBlock() : frameID(0), gframeID(0),
                        bytesPerSample(4), sampleFmt(SAGE_SAMPLE_FLOAT32),
                        sampleRate(0), channels(0), framePerBuffer(0),
                        extraInfo(NULL), tileID(0), nodeID(0)
{
   flag = SAGE_AUDIO_BLOCK;
}

sageAudioBlock::sageAudioBlock(int frame, sageSampleFmt type, int byte, int rate, int chan, int framesperbuffer)
:                       frameID(frame), gframeID(0),
   bytesPerSample(byte), sampleFmt(type),
   sampleRate(rate), channels(chan), framePerBuffer(framesperbuffer),
   extraInfo(NULL), tileID(0), nodeID(0)
{
   flag = SAGE_AUDIO_BLOCK;
   initBuffer();
}

sageAudioBlock::~sageAudioBlock()
{
   releaseBuffer();
}

int sageAudioBlock::initBuffer()
{
   int newBufSize = framePerBuffer * channels * bytesPerSample;
   initBuffer(newBufSize);
   return 0;
}

int sageAudioBlock::initBuffer(int size)
{
   if (buffer) 
   {
      releaseBuffer();
   }

   if (allocateBuffer(size) < 0)
               return -1;
   
   bufSize = size;               
   audioData = buffer + BLOCK_HEADER_SIZE;   
   return 0;
}

int sageAudioBlock::releaseBuffer()
{
   //std::cout << "release buffer" << std::endl;
   if (buffer) {
      free(buffer);
      buffer = NULL;
      //if (buffer)
      //delete [] buffer;
   }

   return 0;
}

int sageAudioBlock::allocateBuffer(int size)
{

   buffer = (char*)malloc(size);
   bufSize = size;

   return 0;
}

int sageAudioBlock::updateBufferHeader()
{
   if (!buffer) {
      sage::printLog("sageAudioBlock::updateBufferHeader : buffer is null");
      return -1;
   }

   memset(buffer, 0, BLOCK_HEADER_SIZE);
   int headerSize = 0;

#if defined(WIN32)
   headerSize = _snprintf(buffer, BLOCK_HEADER_SIZE, "%d %d %d %d %d %d %d %d %d %d %ld %ld",
         bufSize, flag,(int)sampleFmt, sampleRate, channels, framePerBuffer, frameID, gframeID, tileID, nodeID, timestamp_s, timestamp_u);
#else
   headerSize = snprintf(buffer, BLOCK_HEADER_SIZE, "%d %d %d %d %d %d %d %d %d %d %ld %ld",
         bufSize, flag, (int)sampleFmt, sampleRate, channels, framePerBuffer, frameID, gframeID, tileID, nodeID, timestamp_s, timestamp_u);

   //std::cout << buffer << std::endl;
#endif

   if (headerSize >= BLOCK_HEADER_SIZE) {
      sage::printLog("sageAudioBlock::updateBufferHeader : block header has been truncated.");
      return -1;
   }

   return 0;
}

bool sageAudioBlock::updateBlockConfig()
{
   if (!buffer) {
      sage::printLog("sageAudioBlock::updateBlockConfig : buffer is null");
      return false;
   }

   sscanf(buffer, "%d %d %d %d %d %d %d %d %d %d %ld %ld", &bufSize, &flag, &sampleFmt, &sampleRate, 
         &channels, &framePerBuffer, &frameID,  &gframeID, &tileID, &nodeID, &timestamp_s, &timestamp_u);
   //std::cout << buffer << std::endl;
	//std::cout << "timestamp : get = " << timestamp_s << " " << timestamp_u << std::endl;

   //extraInfo = sage::tokenSeek(buffer, 10);
   //if (!extraInfo && flag == SAGE_AUDIO_BLOCK) {
   //  // sage::printLog("sageAudioBlock::updateBlockConfig : extraInfo is NULL");
   //}

   return true;
}


/*
sageControlBlock::sageControlBlock(int f, int frame, int size) : frameID(frame)
{
   flag = f;
   bufSize = size;
   
   if (size > 0)
      buffer = new char[size];
}

int sageControlBlock::updateBufferHeader(char *info)
{
   if (!buffer) {
      sage::printLog("sageControlBlock::updateBufferHeader : buffer is null");   
      return -1;
   }
   
   memset(buffer, 0, BLOCK_HEADER_SIZE);
   sprintf(buffer, "%d %d %d %s", bufSize, flag, frameID, info);
         
   return 0;
}

int sageControlBlock::updateBlockConfig()
{
   if (!buffer) {
      sage::printLog("sageControlBlock::updateBlockConfig : buffer is null");   
      return -1;
   }
   
   sscanf(buffer, "%d %d %d %s", &bufSize, &flag, &frameID, ctrlInfo);
   
   return 0;
}

sageControlBlock::~sageControlBlock()
{
   if (buffer)
      delete [] buffer;
}
*/
