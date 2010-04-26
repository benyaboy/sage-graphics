/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sail.h
 * Author : Luc Renambot
 *
 * Copyright (C) 2010 Electronic Visualization Laboratory,
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
 * Direct questions, comments etc about SAGE to admin@sagecommons.org or 
 * http://www.sagecommons.org
 *
 * $Rev::                                                                     $
 * $Author::                                                                  $
 * $Date::                                                                    $
 *****************************************************************************/

#ifndef SAGEVERSION_H_
#define SAGEVERSION_H_


// This is the main version number
#define SAGE_BASE_VERSION 3.2

// Utility macros to join and convert to string some values
#define SAGE_STR(XX) #XX
#define SAGE_XSTR(XX) SAGE_STR(XX)
#define SAGE_JOIN(XX, YY) SAGE_JOIN_AGAIN(XX, YY)
#define SAGE_JOIN_AGAIN(XX, YY) XX ## . ## YY

#if defined(SAGE_REVISION)
	// If there's a revision number from SVN,
	// we add it to the base version number
#define SAGE_VERSION SAGE_XSTR(SAGE_JOIN(SAGE_BASE_VERSION, SAGE_REVISION))

#else
	// Just make it a string
#define SAGE_VERSION SAGE_XSTR( SAGE_BASE_VERSION )

#endif


#endif

