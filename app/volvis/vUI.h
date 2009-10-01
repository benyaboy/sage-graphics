/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
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
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/

#ifndef _VUI_H_
#define _VUI_H_
//Translates from mouse coords to world coords
void mouseToWorld(int x, int y, float world[3]) ;
void startRotation(int x, int y);
void doRotation(int x, int y);

void startZoom(int x, int y);
void doZoom(int x, int y);

void startTranslate(int x, int y);
void doTranslate(int x, int y);

void endInteract();

void printUsage();
void scaleSampleRate(float factor);
void togglePrintFPS() ;
void doExit();
void toggleCutPlane();
void toggleBoundBox() ;
void saveGradientVolume();

void doAxisRotation(float angle, char rotaxes) ;
void setLUT(int size, unsigned char* data) ;
void setLowRes(int lowres);
void genIsosurface(int x, int y) ;
void genIsopoint(int x, int y) ;
void incIsoValue(int value) ;

void setRoam();
void roamVolume(int roamX, int roamY, int roamZ) ;
void animateVolume();
void animateSelected();
void updateAnim(int value);
void setProbe();
void probeVolume(int probeX, int probeY, int probeZ) ;
void doSelect(int curIndex) ;
void doSelect(int x, int y) ;
#endif
