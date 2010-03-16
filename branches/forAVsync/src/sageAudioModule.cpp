/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module   :  sageAudioModule.cpp
 *   Author   :   Hyejung Hur
 *   Description:   Code file for managing audio module-create audio, play, capture, save it
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

#include "sageAudioModule.h"
#include "sageAudio.h"
#include "sail.h" 

sageAudioModule* sageAudioModule::_instance = NULL;

sageAudioModule::sageAudioModule()
: fileReader(NULL), fileWriter(NULL), formatManager(NULL), gFrameNum(0),
  mainAudio(NULL), mainBuffer(NULL), rcvEnd(false), playFlag(sageAudio::AUDIO_STOP),
  addLock(false), removeLock(false), nodeID(-1)
{
   PaError err = Pa_Initialize();
   if( err != paNoError ) {
      std::cerr << "portaudio could not initializ" << std::endl;
   }
	resolution[0] = 600;
	resolution[1] = 400;
	dimension[0] = 1;
	dimension[1] = 1;

}

sageAudioModule::~sageAudioModule()
{
	rcvEnd = true;
   if(fileWriter != NULL) {
      fileWriter->stop();
   }
   
   if(fileReader != NULL) {
      fileReader->stop();
   }
   
   sage::usleep(1000);      

	if(mainAudio) 
	{
      mainAudio->stop();
      mainAudio->closeStream();
	}

   Pa_Terminate();
   sage::usleep(2000);   

	if(mainAudio) 
	{
		delete mainAudio;
		mainAudio = NULL;
	}
      
   if(fileReader != NULL) {
      delete fileReader;
      fileReader = NULL;
   }
   
   if(fileWriter != NULL) {
      delete fileWriter;
      fileWriter = NULL;
   }
   
   if(formatManager != NULL) {
      delete formatManager;
      formatManager = NULL;
   }

	if(mainBuffer)
	{
		delete mainBuffer;
		mainBuffer = NULL;
	}

   std::vector<sageAudioCircBuf*>::iterator iterBuffer;
   sageAudioCircBuf* buffer = NULL;
   for(iterBuffer = bufferList.begin(); iterBuffer != bufferList.end(); iterBuffer++)
   {
      buffer = (sageAudioCircBuf*) *iterBuffer;
      delete buffer;
      buffer = NULL;      
   }
   bufferList.clear();

	int channel_size = channelList.size();
	audioAppInfo* app;
	for(int i=0; i < channel_size; i++)
	{
		app = (audioAppInfo*) channelList[i];	
		if(app == NULL) continue;
		delete app;
		app = NULL;
	}
	channelList.clear();
}

void sageAudioModule::setNodeID(int id)
{
	nodeID = id;
}

sageAudioModule* sageAudioModule::instance()
{
   if(_instance == NULL) {
      _instance = new sageAudioModule();
   } 
   return _instance;
}

void sageAudioModule::init(sailConfig &conf)
{
   config.deviceNum = conf.audioDeviceNum;
   config.sampleFmt = conf.sampleFmt;
   config.samplingRate = conf.samplingRate;
   config.channels = conf.channels;
   config.framePerBuffer = conf.framePerBuffer;
   config.audioMode = conf.audioMode;

   init();
}

void sageAudioModule::init(sageAudioConfig &conf)
{
   config.deviceNum = conf.deviceNum;
   config.sampleFmt = conf.sampleFmt;
   config.samplingRate = conf.samplingRate;
   config.channels = conf.channels;
   config.framePerBuffer = conf.framePerBuffer;
   config.audioMode = conf.audioMode;
   
   init();
}

void sageAudioModule::init()
{
	switch(config.audioMode)
	{
	case SAGE_AUDIO_READ :
		{
   		// create audio format manager
   		if(formatManager != NULL) {
      		delete formatManager;
      		formatManager = NULL;
   		}
   		formatManager = new audioFormatManager();
   
   		// create objects for reading and writing audio files 
   		if(fileReader != NULL) {
      		delete fileReader;
      		fileReader = NULL;
   		}
   		fileReader = new audioFileReader(formatManager);
   		fileReader->init();
		}
		break;
	case SAGE_AUDIO_WRITE :
		{
   		// create audio format manager
   		if(formatManager != NULL) {
      		delete formatManager;
      		formatManager = NULL;
   		}
   		formatManager = new audioFormatManager();
      
   		if(fileWriter != NULL) {
      		delete fileWriter;
      		fileWriter = NULL;
   		}
   		fileWriter = new audioFileWriter(formatManager);
   		fileWriter->init();
		}
		break;
	case SAGE_AUDIO_PLAY :
	{
		// create shared circular buffer for audio data
		if(mainBuffer) {
			delete mainBuffer;
			mainBuffer = NULL;
		}
		mainBuffer= new sageAudioCircBuf(nodeID);
		mainBuffer->init(-1, 64, config.sampleFmt, config.framePerBuffer * config.channels);
		mainAudio = new sageAudio();
		int err = mainAudio->init(-1, &config, SAGE_AUDIO_PLAY, mainBuffer);
		if(err <0) {
			delete mainBuffer;
			delete mainAudio;
			mainBuffer = NULL;
			mainAudio = NULL;
		}

		pthread_t thId;
		if (pthread_create(&thId, 0, mergeThread, (void*)this) != 0) {
			sage::printLog("sageAudioModule: can't create merging thread");
		}
	}
	break;
	}
   
   std::cout << "[sageAudioModule::init] initialized" << std::endl;

}

void sageAudioModule::setTileInfo(int width, int height, int dimX, int dimY)
{
	resolution[0] = (width * dimX) / 2;
	resolution[1] = height;
	dimension[0] = dimX;
	dimension[1] = dimY;
}

audioAppInfo* sageAudioModule::findApp(int id, int& index)
{
	audioAppInfo* temp_app= NULL;
	std::vector<audioAppInfo*>::iterator iter;
	index =0;
	for(iter = channelList.begin(); iter != channelList.end(); iter++, index++)
	{
		if ((*iter)->instID == id)
		{
			temp_app =(audioAppInfo*) *iter;
			break;
		}
	}
	return temp_app;
}

void sageAudioModule::changeWindow(int id, int left, int bottom, int width, int height, int zvalue)
{
	std::cout << "audio module" << std::endl;
	while(appLock == true) sage::usleep(1000);

	appLock = true;
	int index;
	audioAppInfo* app = findApp(id, index);
	if(app == NULL)
	{
		app = new audioAppInfo();
		channelList.push_back(app);
		app->instID = id;
		index = channelList.size() -1;
	} 

	app->left = left;
	app->bottom = bottom;
	app->width = width;
	app->height = height;
	app->depth = zvalue;

	int center_x = (left + (width /2)) / resolution[0];
	int center_half_x = (left + width) % resolution[0] - (width /2);
	int quad = width /4;
	if((center_half_x < quad) && (center_half_x > -quad))
	{
		// temporal.....
		app->channel = 2;
	} else 
		app->channel = center_x;

	appList.push_back(index);
	appLock = false;

	//int center_y = bottom + (height /2);
	std::cout << "[sageAudioModule::changeWindow] id=" << id << ", channel " << app->channel << " " << zvalue <<  std::endl;
}

void* sageAudioModule::mergeThread(void *args)
{
   sageAudioModule *This = (sageAudioModule *)args;
   std::vector<sageAudioCircBuf*>::iterator iterBuffer;
	audioBlock* input_block;
	audioBlock* output_block;
   sageAudioCircBuf* input = NULL;
   sageAudioCircBuf* output = This->mainBuffer;
	//int frameIndex = 0;
	int remove_size =0, app_size=0;
  	sageAudioCircBuf* buffer = NULL;
	int remove_id, app_id, channel_id;
   
   while (!This->rcvEnd) {
		output_block = output->getNextWriteBlock();
		if(output_block == NULL) 
		{
      	sage::usleep(100);
			continue;
		}
		if(output->merge(output_block, This->bufferList) == 1)
		{
			//output_block->frameIndex = frameIndex % 10000;
			output_block->frameIndex = 1;
			output_block->reformatted = 1;
			output->updateWriteIndex();
			//frameIndex++;
			//std::cout << frameIndex << " index number " << std::endl;
		}

   	// remove from the list
		while(This->removeLock == true) sage::usleep(100);
		This->removeLock = true;
		remove_size = This->removeList.size();
		if(remove_size > 0)
		{
			for(int i=0; i < remove_size; i++)
			{
				remove_id = This->removeList[i];
   			for(iterBuffer = This->bufferList.begin(); iterBuffer != This->bufferList.end(); iterBuffer++)
   			{
      			buffer = (sageAudioCircBuf*) *iterBuffer;
					if(buffer == NULL) continue;
					if(buffer->getInstID() == remove_id)
					{
      				delete buffer;
      				buffer = NULL;      
						This->bufferList.erase(iterBuffer);
						std::cout << "[sageAudioModule::mergeThread] buffer " << remove_id << " is deleted " << This->bufferList.size() << std::endl;
						break;
					}
				}
			}
			This->removeList.clear();
			if(This->bufferList.size() == 0)
			{
				std::cout << "[sageAudioModule::mergeThread] buffer is reset" << std::endl;
				output->reset();
			}
   	}
		This->removeLock = false;

		// add to the list
		for(iterBuffer = This->addList.begin(); iterBuffer != This->addList.end(); iterBuffer++)
		{
    		buffer = (sageAudioCircBuf*) *iterBuffer;
			if(buffer == NULL) continue;
			This->bufferList.push_back(buffer);
			std::cout << "[sageAudioModule::mergeThread] buffer is added " << This->bufferList.size() << std::endl;
		}

		while(This->addLock == true) sage::usleep(100);
		This->addLock = true;
		This->addList.clear();
		This->addLock = false;

		while(This->appLock == true) sage::usleep(100);
		This->appLock = true;
		app_size = This->appList.size();
		if(app_size > 0)
		{
			for(int i=0; i < app_size; i++)
			{
				app_id = This->appList[i];
				if(This->channelList[app_id] == NULL) continue;

				channel_id =This->channelList[app_id]->channel;
				if(channel_id >= 0) 
				{
   				for(iterBuffer = This->bufferList.begin(); iterBuffer != This->bufferList.end(); iterBuffer++)
   				{
      				buffer = (sageAudioCircBuf*) *iterBuffer;
						if(buffer == NULL) continue;
						if(buffer->getInstID() == app_id)
						{
							buffer->assignChannel(channel_id);
							std::cout << "[sageAudioModule::mergeThread] buffer " << app_id << " channel is changed " << channel_id << std::endl;
							break;
						}
					}
				}
			}
			This->appList.clear();
		}
		This->appLock = false;

		sage::usleep(100);

	}

   sage::printLog("sageAudioModule::mergeThread : exit");
   pthread_exit(NULL);
   return NULL;
}


sageSampleFmt sageAudioModule::getSampleFmt(void)
{
	if(mainBuffer)
	{
		return mainBuffer->getSampleFmt();
	}
	return config.sampleFmt;
}

sageAudioConfig* sageAudioModule::getAudioConfig()
{
   return &config;
}

sageAudioCircBuf* sageAudioModule::load(char* filename, bool loop, int nframes, long totalframes)
{
   /** todo */
   // give the loading job to fileReader
   if(fileReader != NULL) {
      return (fileReader->load(filename, loop, nframes, totalframes));
   }
   
   return NULL;
}

int sageAudioModule::save(char* filename)
{
   /** todo */
   // give the loading job to fileReader
   if(fileWriter != NULL) {
      fileWriter->write(filename, config, mainBuffer);
   }
   
   return 0;
}

int sageAudioModule::play(void)
{
	if(playFlag == sageAudio::AUDIO_PLAY) return 0;

   sageAudio* audio = NULL;
	switch(config.audioMode)
	{
   case SAGE_AUDIO_CAPTURE : 
	case SAGE_AUDIO_FWCAPTURE : 
	case SAGE_AUDIO_PLAY :
		if(playFlag != sageAudio::AUDIO_PAUSE)
			mainAudio->openStream();
		mainAudio->play();
		break;
   case SAGE_AUDIO_READ :
      fileReader->start();
		break;
   case SAGE_AUDIO_WRITE :
      fileWriter->start();
		break;
   //case SAGE_AUDIO_APP :
   //	fileWriter->start();
	//	break;
   }
	playFlag = sageAudio::AUDIO_PLAY;
   return 0;
}

int sageAudioModule::pause(void)
{
	if(playFlag == sageAudio::AUDIO_PAUSE) return 0;

   switch(config.audioMode)
	{
	case SAGE_AUDIO_CAPTURE :
	case SAGE_AUDIO_FWCAPTURE : 
	case SAGE_AUDIO_PLAY :
		//mainAudio->openStream();
		mainAudio->pause();
		break;
   case SAGE_AUDIO_READ :
      //fileReader->();
		break;
   case SAGE_AUDIO_WRITE :
		break;
   }   
   
	playFlag = sageAudio::AUDIO_PAUSE;
   return 0;
}

int sageAudioModule::stop(void)
{
	if(playFlag == sageAudio::AUDIO_STOP) return 0;

   sageAudio* audio = NULL;
   switch(config.audioMode)
	{
   case SAGE_AUDIO_CAPTURE :
	case SAGE_AUDIO_FWCAPTURE : 
	case SAGE_AUDIO_PLAY :
		mainAudio->stop();
		mainAudio->closeStream();
		break;
   case SAGE_AUDIO_READ :
      fileReader->stop();
		break;
   case SAGE_AUDIO_WRITE :
		break;
   }
   
	playFlag = sageAudio::AUDIO_STOP;
   return -1;
}

void sageAudioModule::testDevices()
{
   if(mainAudio != NULL)
   {
      mainAudio->testDevices();
   }
}

std::vector<sageAudioCircBuf*>& sageAudioModule::getBufferList(void)
{
   return bufferList;
}

sageAudioCircBuf* sageAudioModule::createObject(int instID)
{
	return createObject(instID, &config);
}

sageAudioCircBuf* sageAudioModule::createObject(int instID, sageAudioConfig* conf)
{
   int id = bufferList.size();
   if(instID >= 0)
	   id = instID;

   // create shared circular buffer for audio data
   sageAudioCircBuf *objectBuffer= NULL;
   int bufferBlockNum = 16;  // 64
   int newBufSize = conf->framePerBuffer * conf->channels;

   int err = -1;
   // according to mode, create objects and set init values to the objects
   // create audio 
   sageAudio* audio = NULL;
   switch(config.audioMode)
	{
	case SAGE_AUDIO_CAPTURE :  
   case SAGE_AUDIO_FWCAPTURE : 
		if(mainBuffer) 
		{
			delete mainBuffer;
		}
   	mainBuffer= new sageAudioCircBuf(nodeID);
      mainBuffer->init(id, bufferBlockNum, conf->sampleFmt, newBufSize);

		if(mainAudio) 
		{
			delete mainAudio;
		}	
      mainAudio = new sageAudio();
      err = mainAudio->init(id, conf, config.audioMode, mainBuffer);
      if(err <0) {
         delete mainBuffer;
         delete mainAudio;
         mainBuffer = NULL;
         mainAudio = NULL;
         std::cerr << "[sageAudioModule::createObject] failed" << std::endl;
         return NULL;
      }
      std::cout << "[sageAudioModule::createObject] audio object is created" << std::endl;
		return mainBuffer;
		break;
   case SAGE_AUDIO_PLAY :
   	objectBuffer= new sageAudioCircBuf(nodeID);
		// create buffer with main format
      objectBuffer->init(id, bufferBlockNum, config.sampleFmt, newBufSize);
		std::cout << "[sageAudioModule::createObject] audio buffer is created " << objectBuffer->getAudioId() << std::endl;
		while(addLock == true) sage::usleep(1000);
		addLock = true;
		addList.push_back(objectBuffer);
		addLock = false;
		if(playFlag != sageAudio::AUDIO_PLAY)
			play();
		break;
   }

   return objectBuffer;
}   

int sageAudioModule::deleteObject(int id)
{
	while(removeLock == true) sage::usleep(1000);
	removeLock = true;
	removeList.push_back(id);
	removeLock = false;
   //stop(id);
   /** todo */
   // delete object
   // delete buffer
   // remove from the list
   
   return 0;
}

sageAudioCircBuf* sageAudioModule::createBuffer(int instID)
{
	return createBuffer(instID, &config, 16);
}

sageAudioCircBuf* sageAudioModule::createBuffer(int instID, sageAudioConfig* conf, int size)
{
   int id = -1;
   if(instID >= 0)
	   id = instID;

   // create shared circular buffer for audio data
   sageAudioCircBuf *objectBuffer= new sageAudioCircBuf(nodeID);
   int bufferBlockNum = size; 
   int newBufSize = conf->framePerBuffer * conf->channels;
   objectBuffer->init(id, bufferBlockNum, conf->sampleFmt, newBufSize);

	if(config.audioMode == SAGE_AUDIO_PLAY)
	{
		while(addLock == true) sage::usleep(1000);
		addLock = true;
   	addList.push_back(objectBuffer);   
		addLock = false;
	} else 
	{
		if(mainBuffer)
		{
			delete mainBuffer;
			mainBuffer = NULL;
		}
		mainBuffer = objectBuffer;

   	if (config.audioMode == SAGE_AUDIO_READ) { 
			config.sampleFmt = conf->sampleFmt;
			config.samplingRate = conf->samplingRate;
			config.channels = conf->channels;
			config.framePerBuffer = conf->framePerBuffer;
		}
	}
   std::cout << "[sageAudioModule::createBuffer] audio buffer is created" << std::endl;
	
   return objectBuffer;
}   

void sageAudioModule::setgFrameNum(long frames)
{
   gFrameNum = frames;
}
long sageAudioModule::getgFrameNum()
{
   return gFrameNum;
}
