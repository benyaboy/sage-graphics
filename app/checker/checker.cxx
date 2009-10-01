/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
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
 * Direct questions, comments etc about SAGE to sage_users@listserv.uic.edu
 *****************************************************************************/
 

//A simple test program that pushes pixels to SAGE frame buffer 
//written by Byung-il Jeong
//Feb 2005

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// headers for SAGE
#include "sail.h"
#include "misc.h"

int main(int argc, char *argv[])
{
	double t1, t2, fps;
	sail sageInf; // sail object
	int nodeID;

	if (argc < 2)
		nodeID = 0;
	else
		nodeID = atoi(argv[1]);
		 
	int resX = 400, resY = 400;
	if (argc >= 4) {
		resX = atoi(argv[2]);
		resY = atoi(argv[3]); 	 
	}	
	double rate = 10.0;
	if (argc >= 5) {
		rate = atof(argv[4]);
	}	
	
	std::cout << std::endl << "ResX = " << resX << "  ResY = " << resY << std::endl;

	sailConfig scfg;
	scfg.init("checker.conf");
	scfg.setAppName("checker");
	scfg.rank = nodeID;
			
	scfg.resX = resX;
	scfg.resY = resY;

	sageRect renderImageMap;
	renderImageMap.left = 0.0;
	renderImageMap.right = 1.0;
	renderImageMap.bottom = 0.0;
	renderImageMap.top = 1.0;

	scfg.imageMap = renderImageMap;
	scfg.pixFmt = PIXFMT_888;
	scfg.rowOrd = BOTTOM_TO_TOP;
	scfg.master = true;
	scfg.nwID = 1;
	 		 
	sageInf.init(scfg);	 
	std::cout << "sail initialized " << std::endl;
	
	void *buffer = sageInf.getBuffer();
	unsigned char color = 200;
	
	while(1) {
		t1 = sage::getTime();
		memset(buffer, color, resX*resY*3);
		sageInf.swapBuffer();
		buffer = sageInf.getBuffer();
		color = (color + 1) % 256;	
		sageMessage msg;
		if (sageInf.checkMsg(msg, false) > 0) {
			switch (msg.getCode()) {
				 case APP_QUIT : {
					sageInf.shutdown();
					exit(0);
					break;
				}
			}	
		}
		t2 = sage::getTime();
		fps = (1000000.0/(t2-t1));
		if (fps > rate) {
			sage::usleep( (1000000.0/rate) - (t2-t1)  );
		}
	}
	
	return 0;
}
