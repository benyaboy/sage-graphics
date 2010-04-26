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

#include "SVC_input_null.h"

SVC_input_null::SVC_input_null()
{
    m_pid = 69;
    m_byte_pixel = 2;
    m_color_mode = RGB16;
    m_decode_mpeg2 = NULL;
}

void SVC_input_null::setOutput(SVC_output_module *o_module)
{
	m_output_module = o_module;
        	if(m_decode_mpeg2)
                	m_decode_mpeg2 -> set_output_module(m_output_module);
        if(m_codec == UNCOM)
		m_output_module ->init (m_buffer_size,1,0,m_output_addr,m_output_port);
}
void SVC_input_null::init(int codec,int color_mode, int buffer_size, int rtp_flag, char* addr, int port, char* output_addr, int output_port)
{
	//setting network configuration
	m_input_addr = addr;
	m_input_port = port;
	m_output_addr = output_addr;
	m_output_port = output_port;
	m_color_mode = color_mode;
        m_codec = codec;
	m_byte_pixel = convert_format2pixelbyte(m_color_mode);
	m_buffer_size = buffer_size;

	//setting decoder configuration
	select_decoder(codec);
}
void SVC_input_null::run()
{
	m_receive_callback((void*)this);	//process for specific decoder
}
void SVC_input_null::select_decoder(int codec)
{
    switch(codec)
    {
        case MPEG2:
                /* setting capture callback function */
                m_receive_callback = mpeg2_receive;
                fprintf(stderr,"CODEC:MPEG2\n");
                /* setting decoder */
                m_decode_mpeg2 = new SVC_decode_mpeg2();
                m_decode_mpeg2 -> init(m_pid, m_color_mode,m_buffer_size,m_output_addr,m_output_port);
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

void SVC_input_null::decode_output_callback(void* output_obj, uint8_t * const * buf, void * id, int width, int height)
{
	SVC_input_null* THIS = (SVC_input_null*) output_obj;
        THIS -> set_imagesize(width, height);
        THIS -> decode_ouput(buf);
}
void SVC_input_null::decode_ouput(uint8_t * const * buf)
{
        m_output_module -> push_data((unsigned char*)buf[0],m_width,m_height,m_byte_pixel);
}
void SVC_input_null::set_imagesize (int width, int height)
{
        m_width = width;
        m_height = height;
}
char* SVC_input_null::get_input_addr()
{
	return m_input_addr;
}
int SVC_input_null::get_input_port()
{
	return m_input_port;
}
int SVC_input_null::get_buffer_size()
{
	return m_buffer_size;
}

void SVC_input_null::mpeg2_receive(void* obj)
{
	SVC_input_null* THIS = (SVC_input_null *)obj;

	int fd =0;	//stdin
	struct timeval tv;
        fd_set rfds;
        int result = 0;
        int buf_size = THIS -> get_buffer_size();
	char* stream = (char*) malloc(sizeof(char)*SVC_MAX_INPUTNET_BUFFER_SIZE);

        fprintf (stderr, "Starting to receive input_null\n");
        int total = 0;

        do {
                  FD_ZERO (&rfds);
                  FD_SET (fd, &rfds);
                  tv.tv_sec = 0;
                  tv.tv_usec = 20000;

                  if (select (fd + 1, &rfds, NULL, NULL, &tv) > 0)    //if data is received
	  	  {
				sleep(0);
				if(FD_ISSET(fd, &rfds))
				{
				     //read data from standard input
				     result = fread(stream,1,buf_size,stdin);

					if(result == buf_size)
					{
						//fprintf(stderr, "SVC_input_null::read data %d\n",result);
						//forward data received from stdin to decode
						if(THIS-> m_codec != UNCOM)
							THIS -> m_decode_mpeg2 -> decode((unsigned char*)stream,result);
						else
							THIS -> m_output_module -> push_data((unsigned char*)stream,result,1,1);

                                                total += result;
                                                if (total > 7800) // 6 packets == 1 frame ??
                                                {
                                                        //usleep((int)(1000.0/30.0)*1000); // get 30fps
																		  usleep(1);
                                                        total = 0;
                                                }


					}
					else {
						fprintf(stderr, "SVC_input_null::incorrect received byte size %d -> %d received\n",buf_size,result);
						break;
					}
				}
				else
					fprintf(stderr, "SVC_input_null::other's data\n");

			}
			else
			{
				//fprintf(stderr, "SVC_input_null::no data\n");
			}


		}while (g_done_control == 0 ||  result == 0);	//should check

		free(stream);

                fprintf (stderr, "input_null is done.\n");
}
void SVC_input_null::dv_receive(void* obj)
{
	SVC_input_null* THIS = (SVC_input_null *)obj;

}
