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

#include "SVC_output_net.h"


SVC_output_net::SVC_output_net()
{
	m_width = 0;
	m_height = 0;
	m_byte_pixel = 0;
	m_flag_notinit = 1;
	m_inter_buffer = NULL;
	m_count =0;
}
unsigned char* SVC_output_net::init(int width, int height, int color_mode,char* output_addr, int output_port)
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

void SVC_output_net::push_data(unsigned char* buf , int width, int height, int b_pixel)
{
	if(buf == NULL || fd < 0)
	{
		fprintf(stderr, "socket init is not yet\n");
		return;
	}

	memcpy(m_inter_buffer_ptr, buf, width);
	m_count += width;
	m_inter_buffer_ptr += width;

	if(m_count >= m_width)
	{
		result = sendto(fd,m_inter_buffer,m_width,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
		//result = fwrite(m_inter_buffer,1, m_width,stdout);
		m_inter_buffer_ptr = m_inter_buffer;
		m_count =0;
	}

	if(result != m_width)
             fprintf(stderr, "SVC_output_net::Incorrect send buf size %d -> %d\n",m_width,result);
}
void SVC_output_net::init2()
{
	fd = -1;
        cli_len = sizeof(cli_addr);
        serv_len = sizeof(serv_addr);

	//Sender
        bzero((char*) &cli_addr, sizeof(cli_addr));
        cli_addr.sin_family    = AF_INET;
        cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        cli_addr.sin_port = htons(0);

	//Receiver
        bzero((char*) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family    = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(m_output_addr);
        serv_addr.sin_port = htons(m_output_port);

	fprintf(stderr,"Sending address is %s port is %d\n",m_output_addr,m_output_port);


        if(( fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        {
             fprintf(stderr, "SVC_output_net::Fail in creating socket\n ");
             exit(1);
        }

        if( bind(fd, (struct sockaddr*) &cli_addr, sizeof(cli_addr)) < 0)
        {
             fprintf(stderr, "SVC_output_net::Fail in binding socket\n ");
             exit(1);
         }

	 m_inter_buffer = (unsigned char*) malloc (m_width*sizeof(unsigned char));
	 m_inter_buffer_ptr = m_inter_buffer;
}

