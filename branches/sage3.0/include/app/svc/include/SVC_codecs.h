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

#ifndef SVC_CODEC_H
#define SVC_CODEC_H

#include <inttypes.h>
#include <stdio.h>
/* Codec Define */
#define DV        77
#define MPEG2     78
#define UNCOM     79

/* Output Pixel Format Define */
#define RGB16     90
#define RGB24	  91
#define RGB32	  92
#define YUV	  93

static int convert_format2pixelbyte(int color_mode)
{
	switch(color_mode)
	{
	 case RGB16:
		return 2;
	 case RGB24:
		return 3;
	 case RGB32:
		return 4;
	 case YUV:
		return 2;
	 default:
		fprintf(stderr,"Incorrect color_mode in convert_format2pixelbyte()\n");
	}
}

/* RTP Enable Option Define */
#define RTP_ENABLE  100
#define RTP_DISABLE 101

/* Global Constant Define */
#define VLC_SEND_BUFFER_SIZE		1316	/* MPEG2TS x 7*/
#define STD_MPEG2TS_READ_BUFFER_SIZE	1316	/* MPEG2TS x 14*/
#define SVC_MAX_INPUTNET_BUFFER_SIZE	3000
#define CAPTURE1394_SEND_BUFFER_SIZE	188	/* MPEG2TS * 1 */
#define RTP_HEADER_SIZE 		12
#define MPEG2TS_PACKET_SIZE 		188
#define SVC_INPUT_NET_PORT 		9090
#define SVC_OUTPUT_NET_PORT 		9090
#define DEFAULT_INPUT_ADDRESS		NULL
#define DEFAULT_OUTPUT_ADDRESS		"127.0.0.1"
/* Callback Function Define */
typedef void (*DECODEOUTPUT_CALLBACK) (void* output_obj, uint8_t * const * buf, void * id, int width, int height);

#endif
