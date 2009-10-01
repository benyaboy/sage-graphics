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

#ifndef SVC_INPUT_NULL_H
#define SVC_INPUT_NULL_H

#include "SVC_common.h"
#include "SVC_input_module.h"
#include "SVC_output_module.h"
#include "SVC_decode_mpeg2.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef void (*NETRECEIVE_CALLBACK)(void* obj);


class SVC_input_null : public SVC_input_module
{
public:
      SVC_input_null();
      void setOutput(SVC_output_module *output_instance);
      void init(int codec,int color_mode, int buffer_size, int rtp_flag, char* i_addr, int i_port, char* o_addr, int o_port);
      static void decode_output_callback(void* output_obj, uint8_t * const * buf, void * id, int width, int height);
      void run();
      void select_decoder(int codec);
      static void mpeg2_receive(void* obj);
      static void dv_receive(void* obj);
      void decode_ouput(uint8_t * const * buf);
      void set_imagesize (int width, int height);
      int get_input_port();
      char* get_input_addr();
      int get_buffer_size();
      SVC_output_module* m_output_module;
      int m_codec;


private:
      SVC_decode_mpeg2* m_decode_mpeg2;
      NETRECEIVE_CALLBACK m_receive_callback;
      int m_byte_pixel;
      int m_pid;
      int m_color_mode;
      int m_width;
      int m_height;
      int m_input_port;
      char* m_input_addr;
      int m_output_port;
      char* m_output_addr;
      int m_buffer_size;

};
#endif
