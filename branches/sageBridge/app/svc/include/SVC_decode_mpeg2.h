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

#ifndef SVC_DECODE_MPEG2_H
#define SVC_DECODE_MPEG2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "SVC_codecs.h"
extern "C"
{
#include "mpeg2dec/mpeg2.h"
#include "mpeg2dec/mpeg2convert.h"
}
#include "SVC_output_module.h"
//#include "SVC_decode_module.h"


#define DEMUX_PAYLOAD_START 1
#define DEMUX_HEADER 0
#define DEMUX_DATA 1
#define DEMUX_SKIP 2
static int state = DEMUX_SKIP;
static int state_bytes = 0;
static uint8_t head_buf[264];
static int buffer_size =  188;

typedef struct {
    int (* convert) (int stage, void * id,
                     const struct mpeg2_sequence_s * sequence,
                     int stride, uint32_t accel, void * arg,
                     struct mpeg2_convert_init_s * result);
} vo_setup_result_t;


class SVC_decode_mpeg2
{
public:
      SVC_decode_mpeg2();
      void init(int program_id, int color_mode, int buf_size, char* output_addr, int output_port);
      void set_output_callback(void* output_obj, DECODEOUTPUT_CALLBACK callback);
      void decode (unsigned char* input_data, int input_size);
      static void * malloc_hook (unsigned size, mpeg2_alloc_t reason);
      void mpeg2_initialization();
      void set_output_module(SVC_output_module* o_module);
      void stop();
      int demux (uint8_t * buf, uint8_t * end, int flags);
      void decode_mpeg2 (uint8_t * current, uint8_t * end);
      int output_setup (unsigned int width,
                   unsigned int height, unsigned int chroma_width,
                   unsigned int chroma_height);
private:
      int m_byte_pixel;
      int demux_pid;
      int demux_track;
      int m_color_mode;
      int total_offset;
      mpeg2dec_t * mpeg2dec;
      vo_setup_result_t m_convert_result;
      DECODEOUTPUT_CALLBACK m_output_callback;
      void * m_output_obj;
      SVC_output_module * m_output_module;
      char* m_output_addr;
      int m_output_port;

      //decoded image information
      unsigned int m_width;
      unsigned int m_height;
      unsigned int m_chroma_width;
      unsigned int m_chroma_height;
      unsigned char * fbuf[3][3];

      uint8_t * buffer;
      uint8_t * buf;
      uint8_t * nextbuf;
      uint8_t * data;
      uint8_t * end;
};
#endif

