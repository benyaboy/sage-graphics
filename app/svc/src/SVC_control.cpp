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

#include "SVC_control.h"

int g_done_control=0;
int nwID;

SVC_control::SVC_control()
{

    m_input_module = NULL;
    m_output_module = NULL;
    m_buffer_manager = NULL;
    m_codec = MPEG2;
    m_flag_rtp = RTP_DISABLE;
    m_input_addr = DEFAULT_INPUT_ADDRESS;
    m_output_addr = DEFAULT_OUTPUT_ADDRESS;
    m_input_port =SVC_INPUT_NET_PORT;
    m_output_port =SVC_OUTPUT_NET_PORT;
    flag_1394 = 0;
    flag_net = 0;
    flag_null = 0;
    m_color_mode = RGB16;
}

int SVC_control::init(int argc, char** argv)
{
	 nwID = 1;
    int rt=0;

    rt = create_interface_module(argc,argv);
    if(rt)	// fail in creating interface
	return rt;

    // create buffer manager
    if (rt = create_session())
        return rt;

    return rt;
}
void SVC_control::run()
{

     signal (SIGINT, sighandler);
     signal (SIGTERM, sighandler);
     signal (SIGPIPE, sighandler);

    if(m_input_module)
    	m_input_module -> run();
}
void SVC_control::sighandler (int sig)
{
        g_done_control = 1;
}

int SVC_control::create_session()
{

    if(m_input_module && m_output_module)
	m_input_module -> setOutput(m_output_module);

    return 0;
}

int SVC_control::create_interface_module(int argc,char** argv)
{

    int opt;
    while((opt = getopt (argc, argv, "i:o:f:a:b:c:d:t:e:n:")) != -1)
    {
	switch(opt)
	{
	    case 'i':
		if(strcmp("1394", optarg) == 0)
		{
		    flag_1394 =1;
		    m_input_module = (SVC_input_module *)new SVC_input_1394();
		}
		else if (strcmp ("net",optarg) == 0)
                {
		    flag_net = 1;
		    m_input_module = (SVC_input_module *)new SVC_input_net();
                }
		else if (strcmp ("null",optarg) == 0)
                {
		    flag_null = 1;
		    m_input_module = (SVC_input_module *)new SVC_input_null();
                }
	    	else{
		    print_usage(argv);
		    return 1;
		}
		break;

	    case 'o':
	    	if(strcmp("sail", optarg) == 0)
		{
                        m_output_module = (SVC_output_module *)new SVC_output_sail();
	 		m_output_module -> init();	//initialize output module
		}
                else if (strcmp ("svclib",optarg) == 0)
		{
                        ;//m_output_module = (SVC_output_module *)new SVC_output_sidel();
		}
		else if (strcmp ("net",optarg) == 0)
		{
                        m_output_module = (SVC_output_module *)new SVC_output_net();
		}
		else if (strcmp ("null",optarg) == 0)
		{
                        m_output_module = (SVC_output_module *)new SVC_output_null();
		}
                else{
                        print_usage(argv);
			return 1;
		}
		break;

            case 'f':
	    	if(strcmp("rgb16", optarg) == 0)
		{
			m_color_mode = RGB16;
		}
                else if (strcmp ("rgb24",optarg) == 0)
		{
			m_color_mode = RGB24;
		}
		else if (strcmp ("rgb32",optarg) == 0)
		{
			m_color_mode = RGB32;
		}
                else if (strcmp ("yuv",optarg) == 0)
		{
			m_color_mode = YUV;
		}
                else{
                        print_usage(argv);
			return 1;
		}
		break;

            case 'a':
		m_input_addr = (char*) malloc (sizeof(char) * 256);
	    	memcpy(m_input_addr, optarg, 256);
		break;

            case 'b':
	    	m_input_port = atoi(optarg);
		break;

            case 'c':
		m_output_addr = (char*) malloc (sizeof(char) * 256);
	    	memcpy(m_output_addr, optarg, 256);
		break;

            case 'd':
	    	m_output_port = atoi(optarg);
		break;

            case 't':
	    	if(strcmp("mpeg2", optarg) == 0)
		{
			m_codec = MPEG2;
		}
                else if (strcmp ("dv",optarg) == 0)
		{
			m_codec = DV;
		}
                else if (strcmp ("null",optarg) == 0)
		{
			m_codec = UNCOM;
		}
                else{
                        print_usage(argv);
			return 1;
		}
		break;

		case 'n' :
			nwID = atoi(optarg);
		break;

	    default:
		print_usage(argv);
		return 1;
		break;
	}//switch
    }//while

//only for MPEG2 TS

    if(flag_1394){
	    m_input_module -> init(m_codec,m_color_mode,m_output_addr,m_output_port);
    }
    else if(flag_net){
	    m_input_module -> init(m_codec,m_color_mode,VLC_SEND_BUFFER_SIZE,RTP_DISABLE, m_input_addr, m_input_port,m_output_addr, m_output_port);
    }
    else if(flag_null){
	    m_input_module -> init(m_codec,m_color_mode,STD_MPEG2TS_READ_BUFFER_SIZE,RTP_DISABLE, m_input_addr, m_input_port,m_output_addr, m_output_port);
    }

    else{
	print_usage(argv);
	return 1;
    }

    return 0;

}

void SVC_control::print_usage(char ** argv)
{
	fprintf(stderr, "usage: "
 		"%s -i input_module (1394|net|null) -o output_module (net|sail|svclib|null)\n"
	        "\t\t[-f output pixel format (rgb16|rgb24|rgb32|yuv)]\n"
	        "\t\t[-a input address]\n"
	        "\t\t[-b input port number]\n"
	        "\t\t[-c output address]\n"
	        "\t\t[-d output port number]\n"
	        "\t\t[-t decode codec (mpeg2|dv|null)]\n"
		, argv[0]
	);
}
