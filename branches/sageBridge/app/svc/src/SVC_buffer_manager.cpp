/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
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
 * Direct questions, comments etc about SAGE to http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/

#include "SVC_buffer_manager.h"



SVC_buffer_manager::SVC_buffer_manager()
{
	m_svc_buffer_list = NULL;
}
SVC_buffer* SVC_buffer_manager::create_buffer_instance()
{
	return new SVC_buffer();
}



///////////////////////////////////////////////

//               SVC_buffer class

//////////////////////////////////////////////


SVC_buffer::SVC_buffer()
{
	m_buffer_list = NULL;
        m_width = 0;
        m_height = 0;
        m_byte_pixel = 0;
	m_buf_front = 0;
	m_buf_rear = 0;
	m_first_flag =1;
}

int SVC_buffer::isnotinit()
{
	return m_first_flag;
}
int SVC_buffer::init(int width, int height, int byte_pixel)
{
	int i;
	m_width = width;
	m_height = height;
	m_byte_pixel = byte_pixel;

   	pthread_mutexattr_t mutexAttr;
   	pthread_mutexattr_init(&mutexAttr);
   	pthread_mutex_init(&m_pop_lock, &mutexAttr);
   	pthread_mutex_lock(&m_pop_lock);

   	pthread_mutexattr_init(&mutexAttr);
   	pthread_mutex_init(&m_put_lock, &mutexAttr);
   	pthread_mutex_unlock(&m_put_lock);

	pthread_cond_init(&m_cond,NULL);

	m_buffer_list = (unsigned char**)malloc(sizeof(unsigned char*)*RAWDATA_BUFFER_LENGTH);

	for(i=0; i< RAWDATA_BUFFER_LENGTH; i++)
	{
		m_buffer_list[i] = (unsigned char*) malloc (sizeof(unsigned char*) * width * height * byte_pixel);
		if(m_buffer_list[i] == NULL)
			return 1;
	}
	m_first_flag=0;
	return 0;
}
void SVC_buffer::put_data(unsigned char* data, long size)
{
	if(data == NULL)
		return;
	if(size != (m_width * m_height * m_byte_pixel)){
		fprintf(stderr,"different data size in SVC_buffer from %ld to %ld\n");
		return;
	}

	if((m_buf_front+1)%RAWDATA_BUFFER_LENGTH == m_buf_rear)	//buffer_list is full
	{
		// smoothing buffer management should be implemented. now just return.
		// kind of random dropping like RED in Active Queue Management

		fprintf(stderr,"buffer_list is full in SVC_buffer\n");
		return;
	}
	else	//buffer_list has enough space
	{
		m_buf_front = (m_buf_front+1) % RAWDATA_BUFFER_LENGTH;
		memcpy (m_buffer_list[m_buf_front], data, size);
   		pthread_cond_signal(&m_cond);
	}

}
unsigned char* SVC_buffer::pop_data()
{
	if(m_buf_rear == m_buf_front)	//buffer is empty
	{
   		pthread_cond_wait(&m_cond,&m_pop_lock);
		return NULL;
	}
	else	//buffer_list has data
	{
		int temp_rear;
		temp_rear = (m_buf_rear + 1) %	RAWDATA_BUFFER_LENGTH;
		unsigned char* temp = (unsigned char*)malloc(sizeof(unsigned char)*m_width * m_height * m_byte_pixel);
	        memcpy(temp,m_buffer_list[temp_rear],sizeof(unsigned char)*m_width*m_height*m_byte_pixel);
		m_buf_rear = (m_buf_rear + 1) %	RAWDATA_BUFFER_LENGTH;
		return temp;
	}
}
int SVC_buffer::pop_data(unsigned char* output_buffer)
{
	if(m_buf_rear == m_buf_front)	//buffer is empty
	{
   		pthread_cond_wait(&m_cond,&m_pop_lock);
		return 1 ;
	}
	else	//buffer_list has data
	{
		int temp_rear;
		temp_rear = (m_buf_rear + 1) %	RAWDATA_BUFFER_LENGTH;
	        memcpy(output_buffer,m_buffer_list[temp_rear],sizeof(unsigned char)*m_width*m_height*m_byte_pixel);
		m_buf_rear = (m_buf_rear + 1) %	RAWDATA_BUFFER_LENGTH;
		return 0;
	}
}

void SVC_buffer::print_current_buflen()
{
	int diff;
	if(diff= (m_buf_front - m_buf_rear) < 0)
		diff = diff + RAWDATA_BUFFER_LENGTH -1;
	fprintf(stderr,"current buffer length is %d\n",diff);
}





