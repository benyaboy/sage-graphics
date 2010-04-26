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

#include "SVC_input_1394.h"

SVC_input_1394::SVC_input_1394()
{
    m_pid = 2064;
    m_byte_pixel = 2;
    m_color_mode = RGB16;
    m_decode_mpeg2 = NULL;
    handle = raw1394_new_handle_on_port (0);
    if(!handle)
    {
	fprintf (stderr, "Failed to get libraw1394 handle\n");
	exit(1);
    }
}

void* SVC_input_1394::input_thread(void* obj)
{
    SVC_input_1394* THIS = (SVC_input_1394*) obj;
    THIS -> input_thread2();
    return NULL;
}

void SVC_input_1394::input_thread2()
{
    m_capture_callback (handle, (void*)this, 63);
}

void SVC_input_1394::run()
{
    m_capture_callback (handle, (void*)this, 63);
}

void SVC_input_1394::join()
{
}

void SVC_input_1394::init(int codec, int color_mode, char* output_addr, int output_port)
{
	m_output_addr = output_addr;
	m_output_port = output_port;
	m_color_mode = color_mode;
	m_codec = codec;
	m_byte_pixel = convert_format2pixelbyte(m_color_mode);
	select_decoder(codec);
}

void SVC_input_1394::setBuffer (SVC_buffer* buffer)
{
	m_svc_buffer = buffer;
}
void SVC_input_1394::select_decoder (int codec)
{
    switch(codec)
    {
	case MPEG2:
		/* setting capture callback function */
		m_capture_callback = mpeg2_receive;
		fprintf(stderr,"CODEC:MPEG2\n");
		/* setting decoder */
                m_decode_mpeg2 = new SVC_decode_mpeg2();
		m_decode_mpeg2 -> init(m_pid, m_color_mode,CAPTURE1394_SEND_BUFFER_SIZE,m_output_addr,m_output_port);
     		m_decode_mpeg2 -> set_output_callback((void*)this, decode_output_callback);

	    break;
	case DV:
		/* setting capture callback function */
		m_capture_callback = dv_receive;
		fprintf(stderr,"CODEC:DV\n");

		/* setting decoder */

	    break;
	case UNCOM:
		/* setting capture callback function */
		m_capture_callback = mpeg2_receive;
		fprintf(stderr,"CODEC:NULL\n");

		/* setting decoder */

	    break;

	default:
	    break;
    }
}
void SVC_input_1394::decode_output_callback(void* output_obj, uint8_t * const * buf, void * id, int width, int height)
{
	SVC_input_1394* THIS = (SVC_input_1394*) output_obj;
	THIS -> set_imagesize(width, height);
	THIS -> decode_ouput(buf);
}

void SVC_input_1394::decode_ouput(uint8_t * const * buf)
{
	m_output_module -> push_data((unsigned char*)buf[0],m_width,m_height,m_byte_pixel);
}
void SVC_input_1394::setOutput (SVC_output_module* o_module)
{
	m_output_module = o_module;
	if(m_decode_mpeg2)
		m_decode_mpeg2 -> set_output_module(m_output_module);
        if(m_codec == UNCOM)
                m_output_module ->init (VLC_SEND_BUFFER_SIZE,1,0,m_output_addr,m_output_port);
}
void SVC_input_1394::set_imagesize (int width, int height)
{
	m_width = width;
	m_height = height;
}

int SVC_input_1394::write_packet_mpeg2 (unsigned char *data, int len, unsigned int dropped, void *callback_obj)
{
        SVC_input_1394 *THIS = (SVC_input_1394*) callback_obj;

        if (dropped)
                fprintf (stderr, "\a%d packets dropped!\n", dropped);

	if(THIS && THIS -> m_decode_mpeg2 )
		THIS -> m_decode_mpeg2 -> decode(data,len);
	else if(THIS -> m_codec == UNCOM)
		 THIS -> m_output_module -> push_data((unsigned char*)data,len,1,1);
	else
                fprintf (stderr, "decoder is not created\n");

        return 0; //obj -> insert2buffer(data, len);
}

int SVC_input_1394::insert2buffer(unsigned char* data, int len)
{
//	m_buffer -> insert_data(data,len);

	fwrite(data,1,len,stdout);
	return 0;
}

int SVC_input_1394::write_frame_dv (unsigned char *data, int len, int complete, void *callback_obj)
{
        SVC_input_1394 *obj = (SVC_input_1394*) callback_obj;

        if (complete == 0)
                fprintf (stderr, "Error: incomplete frame received!\n");
        return obj -> insert2buffer(data, len);
}

void SVC_input_1394::mpeg2_receive (raw1394handle_t handle, void *obj, int channel)
{
   	iec61883_mpeg2_t mpeg = iec61883_mpeg2_recv_init (handle, write_packet_mpeg2,
                obj );

        if ( mpeg && iec61883_mpeg2_recv_start (mpeg, channel) == 0)
        {
                int fd = raw1394_get_fd (handle);
                struct timeval tv;
                fd_set rfds;
                int result = 0;
                fprintf (stderr, "Starting to receive 1394\n");

                do {
                        FD_ZERO (&rfds);
                        FD_SET (fd, &rfds);
                        tv.tv_sec = 0;
                        tv.tv_usec = 20000;

                        if (select (fd + 1, &rfds, NULL, NULL, &tv) > 0)
				if(FD_ISSET(fd, &rfds))
                                	result = raw1394_loop_iterate (handle);

                } while (g_done_control == 0 && result == 0);

                fprintf (stderr, "input_1394 is done.\n");
        }
        iec61883_mpeg2_close (mpeg);
}

void SVC_input_1394::dv_receive( raw1394handle_t handle, void* obj, int channel)
{
        iec61883_dv_fb_t frame = iec61883_dv_fb_init (handle, write_frame_dv, obj);

        if (frame && iec61883_dv_fb_start (frame, channel) == 0)
        {
                int fd = raw1394_get_fd (handle);
                struct timeval tv;
                fd_set rfds;
                int result = 0;

                fprintf (stderr, "Starting to receive\n");

                do {
                        FD_ZERO (&rfds);
                        FD_SET (fd, &rfds);
                        tv.tv_sec = 0;
                        tv.tv_usec = 20000;

                        if (select (fd + 1, &rfds, NULL, NULL, &tv) > 0)
                                result = raw1394_loop_iterate (handle);

                } while (g_done_control == 0 && result == 0);

                fprintf (stderr, "done.\n");
        }
        iec61883_dv_fb_close (frame);
}


void SVC_input_1394::stop ()
{
    raw1394_destroy_handle (handle);
}
