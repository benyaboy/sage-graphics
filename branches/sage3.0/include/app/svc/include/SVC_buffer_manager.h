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

#ifndef SVC_BUFFER_MANAGER_H
#define SVC_BUFFER_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define RAWDATA_BUFFER_LENGTH 60



class SVC_buffer
{
public:
	SVC_buffer();
	int init(int width, int height, int byte_pixel);
	void put_data(unsigned char* data, long size);
	unsigned char* pop_data();
	int pop_data(unsigned char* output_buffer);
	void print_current_buflen();
	int isnotinit();
	int get_width(){ return m_width;};
	int get_height(){ return m_height;}
	int get_byte_pixel(){return m_byte_pixel;}


private:
	unsigned char **m_buffer_list;
	int m_width;
	int m_height;
	int m_byte_pixel;
	int m_buf_front;
	int m_buf_rear;
	int m_first_flag;
	pthread_mutex_t m_put_lock;
	pthread_mutex_t m_pop_lock;
	pthread_cond_t m_cond;
};


class SVC_buffer_manager
{
public:
	SVC_buffer_manager();
	SVC_buffer* create_buffer_instance();

private:
	SVC_buffer** m_svc_buffer_list;
};


#endif
