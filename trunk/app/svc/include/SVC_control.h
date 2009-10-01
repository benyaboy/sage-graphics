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

#ifndef SVC_CONTROL_H
#define SVC_CONTROL_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "SVC_input_1394.h"
#include "SVC_buffer_manager.h"
#include "SVC_input_module.h"
#include "SVC_output_module.h"
#include "SVC_codecs.h"
#include "SVC_common.h"
#include "SVC_output_sail.h"
#include "SVC_input_net.h"
#include "SVC_input_null.h"
#include "SVC_output_null.h"
#include "SVC_output_net.h"

//later will be included
//#include "SVC_output_sidel.h"
//#include "SVC_io_module.h"          //description of input, output module

class SVC_control
{
public:
	SVC_control();
	int create_interface_module(int argc,char** argv);
	int init(int argc, char** argv);
        int create_session();
	void run();
	void print_usage(char ** argv);
	static void sighandler (int sig);

private:
	SVC_input_module* m_input_module;
	SVC_output_module* m_output_module;
	SVC_buffer_manager* m_buffer_manager;
	int m_codec;
        int m_flag_rtp;
        char* m_input_addr;
        char* m_output_addr;
	int m_input_port;
	int m_output_port;
	int flag_1394;
	int flag_null;
	int flag_net;
	int m_color_mode;
};

#endif // SVC_CONTROL_H
