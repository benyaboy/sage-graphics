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

#ifndef SVC_INPUT_1394_H
#define SVC_INPUT_1394_H

#include <stdio.h>
#include "libiec61883/iec61883.h"
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "SVC_input_module.h"
#include "SVC_output_module.h"
#include "SVC_decode_mpeg2.h"
#include "SVC_buffer_manager.h"

typedef void (*CAPTURE_CALLBACK)(raw1394handle_t handle, void* obj, int channe);

class SVC_input_1394 : public SVC_input_module
{
public:
    SVC_input_1394();
    void process();
    void init(int codec, int color_mode, char* o_addr, int o_port);
    static int write_frame_dv (unsigned char *data, int len, int complete, void *callback_data);
    static int write_packet_mpeg2 (unsigned char *data, int len, unsigned int dropped, void *callback_data);
    static void dv_receive( raw1394handle_t handle, void* obj, int channel);
    static void mpeg2_receive (raw1394handle_t handle, void *obj, int channel);
    int insert2buffer(unsigned char* data, int len);
    static void* input_thread(void* obj);
    void input_thread2();
    void select_decoder(int codec);
    int insert2buffer(unsigned char* data, long len);
    void setOutput(SVC_output_module* output);
    void stop();
    void run();
    void join();
    void setBuffer(SVC_buffer*);
    static void decode_output_callback(void* output_obj, uint8_t * const * buf, void * id, int width, int height);
    SVC_decode_mpeg2* m_decode_mpeg2;
    void set_imagesize(int width, int height);
    void decode_ouput(uint8_t * const * buf);
    int m_codec;
    SVC_output_module* m_output_module;

private:
    raw1394handle_t handle;
    CAPTURE_CALLBACK m_capture_callback;
    pthread_t inputThreadID;
    SVC_buffer* m_svc_buffer;
    int m_byte_pixel;
    int m_pid;
    int m_color_mode;
    int m_width;
    int m_height;
    char* m_output_addr;
    int m_output_port;

};


#endif
