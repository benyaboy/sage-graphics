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

#include "SVC_input_net.h"

SVC_input_net::SVC_input_net()
{
    m_pid = 69;
    //m_pid = 2064;
    m_byte_pixel = 2;
    m_color_mode = RGB16;
    m_decode_mpeg2 = NULL;
}

void SVC_input_net::setOutput(SVC_output_module *o_module)
{
	m_output_module = o_module;
        	if(m_decode_mpeg2)
                	m_decode_mpeg2 -> set_output_module(m_output_module);
        if(m_codec == UNCOM)
		m_output_module ->init (m_buffer_size,1,0,m_output_addr,m_output_port);
}
void SVC_input_net::init(int codec,int color_mode, int buffer_size, int rtp_flag, char* addr, int port, char* output_addr, int output_port)
{
	//setting network configuration
	m_input_addr = addr;
	m_input_port = port;
	m_output_addr = output_addr;
	m_output_port = output_port;
	m_color_mode = color_mode;
        m_codec = codec;
	m_byte_pixel = convert_format2pixelbyte(m_color_mode);

	if(rtp_flag == RTP_ENABLE)
		m_buffer_size = buffer_size + RTP_HEADER_SIZE;
	else
		m_buffer_size = buffer_size;

	//setting decoder configuration
	select_decoder(codec);
}
void SVC_input_net::run()
{
	m_receive_callback((void*)this);	//process for specific decoder
}
void SVC_input_net::select_decoder(int codec)
{
    switch(codec)
    {
        case MPEG2:
                /* setting capture callback function */
                m_receive_callback = mpeg2_receive;
                fprintf(stderr,"CODEC:MPEG2\n");
                /* setting decoder */
                m_decode_mpeg2 = new SVC_decode_mpeg2();
                m_decode_mpeg2 -> init(m_pid, m_color_mode,VLC_SEND_BUFFER_SIZE,m_output_addr,m_output_port);
                m_decode_mpeg2 -> set_output_callback((void*)this, decode_output_callback);

            break;
        case DV:
                /* setting capture callback function */
                m_receive_callback = dv_receive;
                fprintf(stderr,"CODEC:DV\n");

                /* setting decoder */

            break;
        case UNCOM:
                m_receive_callback = mpeg2_receive;
                /* setting capture callback function */
                fprintf(stderr,"CODEC:NULL\n");
                /* setting decoder */

            break;

        default:
                fprintf(stderr,"This codec is not supported\n");
            break;
    }
}

void SVC_input_net::decode_output_callback(void* output_obj, uint8_t * const * buf, void * id, int width, int height)
{
	SVC_input_net* THIS = (SVC_input_net*) output_obj;
        THIS -> set_imagesize(width, height);
        THIS -> decode_ouput(buf);
}
void SVC_input_net::decode_ouput(uint8_t * const * buf)
{
        m_output_module -> push_data((unsigned char*)buf[0],m_width,m_height,m_byte_pixel);
}
void SVC_input_net::set_imagesize (int width, int height)
{
        m_width = width;
        m_height = height;
}
char* SVC_input_net::get_input_addr()
{
	return m_input_addr;
}
int SVC_input_net::get_input_port()
{
	return m_input_port;
}
int SVC_input_net::get_buffer_size()
{
	return m_buffer_size;
}

void SVC_input_net::mpeg2_receive(void* obj)
{
	SVC_input_net* THIS = (SVC_input_net*)obj;

	unsigned char* data;
        int data_len;

	//initialization of network interface

		int fd = -1;
                struct sockaddr_in cli_addr, serv_addr;
		struct ip_mreq mreq;
		char* addr;
		struct timeval tv;
                fd_set rfds;
                int result = 0;
		socklen_t cli_len = sizeof(cli_addr);
		int buf_size = THIS -> get_buffer_size();

		bzero((char*) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family 	= AF_INET;
		if((addr = (THIS -> get_input_addr())) == NULL)
		{
			serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			fprintf(stderr, "SVC_input_net::ADDR is NULL\n");
		}
		serv_addr.sin_port = htons(THIS -> get_input_port());

		if(( fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
		{
			fprintf(stderr, "SVC_input_net::Fail in creating socket\n ");
			exit(1);
		}

	 //set socket option
		int on = 1;
		if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
			fprintf(stderr, "SVC_input_net::Fail in REUSEADDR\n ");


		int rcvbufsize = 200000;
		if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbufsize, sizeof(rcvbufsize)) < 0)
			fprintf(stderr, "SVC_input_net::Fail in RCVBUF %d\n",rcvbufsize);

		if( bind(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		{
			fprintf(stderr, "SVC_input_net::Fail in binding socket\n ");
			exit(1);
		}

	 //check multicast
		if((addr != NULL) && THIS -> is_multicast_addr(addr))
		{

			fprintf(stderr, "SVC_input_net::MULTICAST ADDRESS\n ");

			mreq.imr_multiaddr.s_addr = inet_addr(addr);
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);
			//mreq.imr_interface.s_addr = htonl(inet_addr(""));

			unsigned char ttl = 1;
			unsigned char loop = 1;
			if (setsockopt (fd, IPPROTO_IP,IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
				fprintf(stderr, "SVC_input_net::MULTICAST_LOOP\n ");

			if(setsockopt (fd, IPPROTO_IP,IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
				fprintf(stderr, "SVC_input_net::MULTICAST_TTL\n ");

			if(setsockopt (fd, IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq)) < 0)
				fprintf(stderr, "SVC_input_net::IP_ADD_MEMBERSHIP\n ");

		}

		char* stream = (char*) malloc(sizeof(char)*SVC_MAX_INPUTNET_BUFFER_SIZE);

		//network receiving loop

                fprintf (stderr, "Starting to receive input_net port -> %d\n",ntohs(serv_addr.sin_port));

                do {
                        FD_ZERO (&rfds);
                        FD_SET (fd, &rfds);
                        tv.tv_sec = 0;
                        tv.tv_usec = 20000;

                        if (select (fd + 1, &rfds, NULL, NULL, &tv) > 0)    //if data is received
			{
				if(FD_ISSET(fd, &rfds))
				{
			             //fprintf(stderr, "SVC_input_net::ready to read data\n");

				     //read data from network
				     result = recvfrom (fd,stream,SVC_MAX_INPUTNET_BUFFER_SIZE,0,(struct sockaddr*)&cli_addr,&cli_len);

					if(result != buf_size)
					{
						fprintf(stderr, "SVC_input_net::incorrect buffer_size %d received\n",result);
						continue;
					}
					else {
						//fprintf(stderr, "SVC_input_net::read data %d\n",result);
						//forward data received from network to decode
						if(THIS-> m_codec != UNCOM)
							THIS -> m_decode_mpeg2 -> decode((unsigned char*)stream,result);
						else
							THIS -> m_output_module -> push_data((unsigned char*)stream,result,1,1);
					}
				}
				else
					fprintf(stderr, "SVC_input_net::other's data\n");

			}
			else
			{
				//fprintf(stderr, "SVC_input_net::no data\n");
			}


		}while (g_done_control == 0 ||  result == 0);	//should check

		close (fd);
		free(stream);

                fprintf (stderr, "input_net is done.\n");
}
void SVC_input_net::dv_receive(void* obj)
{
	SVC_input_net* THIS = (SVC_input_net*)obj;

}


int SVC_input_net::is_multicast_addr(char* addr_str)
{
	unsigned long addr_i = ntohl(inet_addr(addr_str));
	if((addr_i & 0xf0000000) != 0xe0000000){	//if unicast
		return 0;
	}
	else{						//if multicast
		return 1;
	}
}



