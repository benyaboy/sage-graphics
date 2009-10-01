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

/*------------------------------------------------------------------------
-	Author: Arun Rao
-	Library: Tile Config
-	Revision Date: 4/22/2004
-	Module: file_parsers.h
-	Purpose: Contains function prototypes for file i/o to load in
-	and write out tiled display system descriptions
---------------------------------------------------------------------------*/
#ifndef TILE_CONFIG_FILE_PARSERS_H
#define TILE_CONFIG_FILE_PARSERS_H

#include "tile_config.h"

//Standard File, I/O functions for tile_config library
//----------------------------------------------------
//Parameters
// [in] filename	- name of the config file
// [in, out] pd		- the returned physical display (send a pointer that isn't pointing to an actual physical display)
// [in, out] dc		- the returned display cluster (send a pointer that isn't pointing to an actual display cluster)
// [in, out] vd		- the returned virutal desktop (send a pointer that isn't pointing to an actual virtual desktop)

#ifdef _cplusplus
extern "C" {
#endif

int stdInputFileParser(char* filename,PhysicalDisplay*& pd,DisplayCluster*& dc,VirtualDesktop*& vd);
void stdOutputFileWriter(char* filename,PhysicalDisplay* pd,DisplayCluster* dc,VirtualDesktop* vd);
int corewallInputFileParser(char* filename, PhysicalDisplay*& pd, DisplayCluster*& dc, VirtualDesktop*& vd, int*& startPos);

#ifdef _cplusplus
}
#endif

#endif
