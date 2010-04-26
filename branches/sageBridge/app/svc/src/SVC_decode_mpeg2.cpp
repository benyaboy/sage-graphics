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

#include "SVC_decode_mpeg2.h"

#define NEEDBYTES(x)                                            \
    do {                                                        \
        int missing;                                            \
                                                                \
        missing = (x) - bytes;                                  \
        if (missing > 0) {                                      \
            if (header == head_buf) {                           \
                if (missing <= end - buf) {                     \
                    memcpy (header + bytes, buf, missing);      \
                    buf += missing;                             \
                    bytes = (x);                                \
                } else {                                        \
                    memcpy (header + bytes, buf, end - buf);    \
                    state_bytes = bytes + end - buf;            \
                    return 0;                                   \
                }                                               \
            } else {                                            \
                memcpy (head_buf, header, bytes);               \
                state = DEMUX_HEADER;                           \
                state_bytes = bytes;                            \
                return 0;                                       \
            }                                                   \
        }                                                       \
    } while (0)

#define DONEBYTES(x)            \
    do {                        \
        if (header != head_buf) \
            buf = header + (x); \
    } while (0)


SVC_decode_mpeg2::SVC_decode_mpeg2()
{
     total_offset = 0;
     buffer = NULL;
     buf = NULL;
     nextbuf = NULL;
     data = NULL;
     end = NULL;
     m_output_callback = NULL;
     m_output_obj = NULL;	//input module
     m_output_module = NULL;	//output module
}
void SVC_decode_mpeg2::init(int program_id, int color_mode,int buf_size,char* output_addr, int output_port)
{
     m_byte_pixel = convert_format2pixelbyte(color_mode);
     demux_pid = -1;
     m_color_mode = color_mode;
     m_output_addr = output_addr;
     m_output_port = output_port;
     buffer_size = buf_size;
     mpeg2_initialization();
}
void SVC_decode_mpeg2::decode (unsigned char* input_data, int input_size)
{
	int pid;
	//end = buf + fread (buf, 1, buffer + buffer_size - buf, in_file);
	memcpy(buf,input_data,input_size);
	//fprintf(stderr,"input_size in decode is %d\n ",input_size);
	end = buf + input_size;

        buf = buffer;

        for (; (nextbuf = buf + 188) <= end; buf = nextbuf) {
	    if (*buf != 0x47) {
                fprintf (stderr, "bad sync byte\n");
                nextbuf = buf + 1;
                continue;
            }
            pid = ((buf[1] << 8) + buf[2]) & 0x1fff;
            if (pid != demux_pid && demux_pid != -1)
	    {
                continue;
	    }
            data = buf + 4;
            if (buf[3] & 0x20) {        /* buf contains an adaptation field */
                data = buf + 5 + buf[4];
                if (data > nextbuf)
                    continue;
            }

	    //Begin_automatically detecting video pid
	    if((buf[1] & 0x40 && demux_pid == -1) ){       	//payload unit start indicator
                        if(((data[0] << 16) + (data[1] << 8) + data[2]) == 0x000001){	//pes start code
                                if((data[3] >> 4) == 0xe){	//pes -> stream_id
                                        demux_pid = pid;
                                }
				else
					continue;
                        }
                        else
                                continue;
            }
	    //End_automatically detecting video pid


            if (buf[3] & 0x10)
	    {
                demux (data, nextbuf,
                       (buf[1] & 0x40) ? DEMUX_PAYLOAD_START : 0);
	    }
        }

        if (end != buffer + input_size)
        {
		fprintf(stderr,"strange size in decode()\n");
		return;
	}
        memcpy (buffer, buf, end - buf);
        buf = buffer + (end - buf);
}
void SVC_decode_mpeg2::mpeg2_initialization()
{
	mpeg2dec = mpeg2_init ();
	if(mpeg2dec == NULL)
	{
		fprintf (stderr, "Can not get mpeg2dec\n");
		exit(1);
	}
	mpeg2_malloc_hooks (malloc_hook, NULL);

	buffer = (uint8_t *) malloc (buffer_size);
    	if (buffer == NULL || buffer_size < 188)
        	exit (1);
    	buf = buffer;
}
void* SVC_decode_mpeg2::malloc_hook (unsigned size, mpeg2_alloc_t reason)
{
	void* buf;
	if ((int)reason < 0) {
        return NULL;
    	}
    	buf = mpeg2_malloc (size, (mpeg2_alloc_t)-1);
    	if (buf && (reason == MPEG2_ALLOC_YUV || reason == MPEG2_ALLOC_CONVERTED))
        	memset (buf, 0, size);
    	return buf;
}

void SVC_decode_mpeg2::stop()
{
	mpeg2_close (mpeg2dec);
}

int SVC_decode_mpeg2::output_setup (unsigned int width,
                         unsigned int height, unsigned int chroma_width,
                         unsigned int chroma_height)
{
	m_width = width;
        m_height = height;
        m_chroma_width = chroma_width;
        m_chroma_height = chroma_height;

	/* setting m_convert_result according to color mode */
	switch(m_color_mode)
	{
		case RGB16:
			m_convert_result.convert = mpeg2convert_rgb16;
		break;

		case RGB24:
			m_convert_result.convert = mpeg2convert_rgb24;
		break;

		case RGB32:
			m_convert_result.convert = mpeg2convert_rgb32;
		break;

                case YUV:
                        m_convert_result.convert = mpeg2convert_uyvy;
		break;
		default:
			fprintf(stderr,"not supported color mode\n");
			return 1;
		break;
	}
	return 0;
}

void SVC_decode_mpeg2::set_output_module(SVC_output_module * o_module)
{
	m_output_module = o_module;
}
void SVC_decode_mpeg2::decode_mpeg2 (uint8_t * current, uint8_t * end)
{
    const mpeg2_info_t * info;
    mpeg2_state_t state;

    mpeg2_buffer (mpeg2dec, current, end);
    total_offset += end - current;

    info = mpeg2_info (mpeg2dec);
    while (1) {
        state = mpeg2_parse (mpeg2dec);
        switch (state) {
        case STATE_BUFFER:
            return;
        case STATE_SEQUENCE:
            /* might set nb fbuf, convert format, stride */
            /* might set fbufs */
            if (output_setup (info->sequence->width,
                               info->sequence->height,
                               info->sequence->chroma_width,
                               info->sequence->chroma_height)) {
                fprintf (stderr, "display setup failed\n");
                exit (1);
            }

	    if (m_convert_result.convert &&
                mpeg2_convert (mpeg2dec, m_convert_result.convert, NULL)) {
                fprintf (stderr, "color conversion setup failed\n");
                exit (1);
            }

	if(m_output_module != NULL)
  	    m_output_module -> init(m_width, m_height, m_color_mode,m_output_addr,m_output_port);

	    for (int i = 0; i < 3; i++) {
                fbuf[i][0] = (unsigned char *) malloc (sizeof(unsigned char)*m_width*m_height*m_byte_pixel);
                fbuf[i][1] = fbuf[i][2] = NULL;
                if (!fbuf[i][0]) {
                    fprintf (stderr, "Could not allocate an output buffer.\n");
                    exit (1);
                }

                mpeg2_set_buf (mpeg2dec, fbuf[i], NULL);
            }

            mpeg2_skip (mpeg2dec, 0);	//maybe not skip
            break;

	case STATE_PICTURE:
            /* might skip */
            /* might set fbuf */
            break;
        case STATE_SLICE:
        case STATE_END:
        case STATE_INVALID_END:

            /* draw current picture */
            /* might free frame buffer */
            if (info->display_fbuf) {
                if (m_output_callback && m_output_obj)
                    m_output_callback (m_output_obj, info->display_fbuf->buf,
                                  info->display_fbuf->id, m_width, m_height);
            }

  	   /*
	    	//for discard, it is not used
            if (output->discard && info->discard_fbuf)
                output->discard (m_output_obj, info->discard_fbuf->buf,
                                 info->discard_fbuf->id);
	    */
            break;
        default:
            break;
        }
    }


}
void SVC_decode_mpeg2::set_output_callback(void* output_obj, DECODEOUTPUT_CALLBACK callback)
{
	m_output_obj = output_obj;
	m_output_callback = callback;
}

int SVC_decode_mpeg2::demux (uint8_t * buf, uint8_t * end, int flags)
{
	static int mpeg1_skip_table[16] = {
            0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    	};
    	uint8_t * header;
    	int bytes;
    	int len;

	if (flags & DEMUX_PAYLOAD_START)
	        goto payload_start;
        switch (state) {
     		case DEMUX_HEADER:
        	if (state_bytes > 0) {
            		header = head_buf;
            		bytes = state_bytes;
            		goto continue_header;
        	}
        	break;
    		case DEMUX_DATA:
        	if (demux_pid || (state_bytes > end - buf)) {
            	   	decode_mpeg2 (buf, end);
            		state_bytes -= end - buf;
            		return 0;
        	}
        	decode_mpeg2 (buf, buf + state_bytes);
        	buf += state_bytes;
        	break;
    		case DEMUX_SKIP:
        	if (demux_pid || (state_bytes > end - buf)) {
            		state_bytes -= end - buf;
            		return 0;
       		}
        	buf += state_bytes;
        	break;
    	}

    	while (1) {
        	if (demux_pid) {
            		state = DEMUX_SKIP;
            	return 0;
        	}
    payload_start:
	header = buf;
        bytes = end - buf;
    continue_header:
        NEEDBYTES (4);
        if (header[0] || header[1] || (header[2] != 1)) {
            if (demux_pid) {
                state = DEMUX_SKIP;
                return 0;
            } else if (header != head_buf) {
                buf++;
                goto payload_start;
            } else {
                header[0] = header[1];
                header[1] = header[2];
                header[2] = header[3];
                bytes = 3;
                goto continue_header;
            }
        }
        if (demux_pid) {
            if ((header[3] >= 0xe0) && (header[3] <= 0xef))
                goto pes;
            fprintf (stderr, "bad stream id %x\n", header[3]);
            exit (1);
        }
        switch (header[3]) {
        case 0xb9:      /* program end code */
            /* DONEBYTES (4); */
            /* break;         */
            return 1;
        case 0xba:      /* pack header */
            NEEDBYTES (5);
            if ((header[4] & 0xc0) == 0x40) {   /* mpeg2 */
                NEEDBYTES (14);
		len = 14 + (header[13] & 7);
                NEEDBYTES (len);
                DONEBYTES (len);
                /* header points to the mpeg2 pack header */
            } else if ((header[4] & 0xf0) == 0x20) {    /* mpeg1 */
                NEEDBYTES (12);
                DONEBYTES (12);
                /* header points to the mpeg1 pack header */
            } else {
                fprintf (stderr, "weird pack header\n");
                DONEBYTES (5);
            }
            break;
        default:
            if (header[3] == demux_track) {
            pes:
                NEEDBYTES (7);
                if ((header[6] & 0xc0) == 0x80) {       /* mpeg2 */
                    NEEDBYTES (9);
                    len = 9 + header[8];
                    NEEDBYTES (len);
                    /* header points to the mpeg2 pes header */
                    if (header[7] & 0x80) {
                        uint32_t pts, dts;

                        pts = (((header[9] >> 1) << 30) |
                               (header[10] << 22) | ((header[11] >> 1) << 15) |
                               (header[12] << 7) | (header[13] >> 1));
                        dts = (!(header[7] & 0x40) ? pts :
				(((header[14] >> 1) << 30) |
                                (header[15] << 22) |
                                ((header[16] >> 1) << 15) |
                                (header[17] << 7) | (header[18] >> 1)));
                        mpeg2_tag_picture (mpeg2dec, pts, dts);
                    }
                } else {        /* mpeg1 */
                    int len_skip;
                    uint8_t * ptsbuf;

                    len = 7;
                    while (header[len - 1] == 0xff) {
                        len++;
                        NEEDBYTES (len);
                        if (len > 23) {
                            fprintf (stderr, "too much stuffing\n");
                            break;
                        }
                    }
                    if ((header[len - 1] & 0xc0) == 0x40) {
                        len += 2;
                        NEEDBYTES (len);
                    }
                    len_skip = len;
                    len += mpeg1_skip_table[header[len - 1] >> 4];
                    NEEDBYTES (len);
                    /* header points to the mpeg1 pes header */
                    ptsbuf = header + len_skip;
                    if ((ptsbuf[-1] & 0xe0) == 0x20) {
                        uint32_t pts, dts;

                        pts = (((ptsbuf[-1] >> 1) << 30) |
                               (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
				(ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
                        dts = (((ptsbuf[-1] & 0xf0) != 0x30) ? pts :
                               (((ptsbuf[4] >> 1) << 30) |
                                (ptsbuf[5] << 22) | ((ptsbuf[6] >> 1) << 15) |
                                (ptsbuf[7] << 7) | (ptsbuf[18] >> 1)));
                        mpeg2_tag_picture (mpeg2dec, pts, dts);
                    }
                }
                DONEBYTES (len);
                bytes = 6 + (header[4] << 8) + header[5] - len;
                if (demux_pid || (bytes > end - buf)) {
                    decode_mpeg2 (buf, end);
                    state = DEMUX_DATA;
                    state_bytes = bytes - (end - buf);
                    return 0;
                } else if (bytes > 0) {
                    decode_mpeg2 (buf, buf + bytes);
                    buf += bytes;
                }
            } else if (header[3] < 0xb9) {
                fprintf (stderr,
                         "looks like a video stream, not system stream\n");
                DONEBYTES (4);
            } else {
                NEEDBYTES (6);
                DONEBYTES (6);
                bytes = (header[4] << 8) + header[5];
                if (bytes > end - buf) {
                    state = DEMUX_SKIP;
		    state_bytes = bytes - (end - buf);
                    return 0;
                }
                buf += bytes;
            }
        }
    }
}

