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

#include "SVC_output_sail.h"

int g_done_out;
extern int nwID;
void *sageBuf = NULL;

SVC_output_sail::SVC_output_sail()
{
	m_width = 0;
	m_height = 0;
	m_byte_pixel = 0;
	g_done_out =0;
	m_flag_notinit = 1;
	m_inter_buffer = NULL;
	framenum = 0;
}
unsigned char* SVC_output_sail::init(int width, int height, int color_mode,char* output_addr, int output_port)
{
	m_width = width;
	m_height = height;
	m_output_addr = output_addr;
	m_output_port = output_port;
	m_color_mode = color_mode;
	m_byte_pixel = convert_format2pixelbyte(color_mode);
	if(m_flag_notinit)
	{
		init2();
		m_flag_notinit =0;
	}
    	return m_inter_buffer;
}

void SVC_output_sail::push_data(unsigned char* buf , int width, int height, int b_pixel)
{
	// push the frame

	// 15 fps
	// if ( (framenum%2) )
	// 25 fps
	if ( (framenum == 1) || (framenum == 7) || (framenum == 13) || (framenum == 19) || (framenum == 25) )
	// 30 fps
	//if ( 0 )
	{
	}
	else
	{
		memcpy(sageBuf, buf, width*height*b_pixel);
		sageInf.swapBuffer();
		sageBuf = sageInf.getBuffer();
	}
	framenum = (framenum + 1) % 30;

	// check some messages
	sageMessage msg;
	if (sageInf.checkMsg(msg, false) > 0) {
		switch (msg.getCode()) {
			case APP_QUIT : {
				exit(0);
				break;
			}

		}
	}
}

void SVC_output_sail::init2()
{
    sageRect glImageMap;
    glImageMap.left = 0.0;
    glImageMap.right = 1.0;
    glImageMap.bottom = 0.0;
    glImageMap.top = 1.0;

    sailConfig scfg;
    scfg.init("svc.conf");
    scfg.setAppName("svc");
    scfg.rank = 0;
	 scfg.nwID = nwID;

    scfg.resX = m_width;
    scfg.resY = m_height;
    scfg.imageMap = glImageMap;
    scfg.rowOrd = TOP_TO_BOTTOM;

    /* find TeraVision color format */
	switch(m_color_mode)
        {
	     case RGB16:
    		scfg.pixFmt = PIXFMT_565;
		break;
        case RGB24:
    		scfg.pixFmt = PIXFMT_888;
		break;
        case RGB32:
    		scfg.pixFmt = PIXFMT_8888;
		break;
        case YUV:
    		scfg.pixFmt = PIXFMT_YUV;
		break;
        }
    sageInf.init(scfg);
	 sageBuf = sageInf.getBuffer();
}

