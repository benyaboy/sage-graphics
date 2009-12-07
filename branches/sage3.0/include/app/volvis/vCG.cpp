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

//vCG.cpp
//has the CG related code from Luc Renambot
#if defined(ADDCGGL)

#include "vCG.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
CGcontext context;
CGprogram vertexProgram, fragmentProgram;
CGprofile fprofile, vprofile;
CGparameter ModelViewProj;
CGparameter hTexture0, hTexture1;
CGparameter param;
int curTexId ;
void handleCgError()
{
    fprintf(stderr, "Cg> error: %s\n", cgGetErrorString(cgGetError()));
    exit(1);
}

int initCG()
{
	// Basic Cg setup; register a callback function for any errors
	// and create an initial context
	cgSetErrorCallback(handleCgError);

	/* cgContext creation */
	context = cgCreateContext();
	assert(cgIsContext(context));

	fprintf(stderr, "---------------------------------------------------------\n");

	if (cgGLIsProfileSupported(CG_PROFILE_FP30))
    {
		fprintf(stderr, "CG> success : FP30 fragment profile supported on this system.\n");
		fprofile = CG_PROFILE_FP30;
    }
	else
    {
		fprintf(stderr, "CG> warning : FP30 fragment profile NOT supported on this system.\n");
		if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
		{
			fprintf(stderr, "CG> success : ARBFP1 fragment profile supported on this system.\n");
			fprofile = CG_PROFILE_ARBFP1;
		}
		else
		{
			fprintf(stderr, "CG> warning : ARBFP1 fragment profile NOT supported on this system.\n");
			if (cgGLIsProfileSupported(CG_PROFILE_FP20))
			{
				fprintf(stderr, "CG> success : FP20 fragment profile supported on this system.\n");
				fprofile = CG_PROFILE_FP20;
			}
			else
			{
				fprintf(stderr, "CG> warning : FP20 fragment profile NOT supported on this system.\n");
				fprintf(stderr, "CG> error : NO fragment profile supported on this system.\n");
				fprintf(stderr, "---------------------------------------------------------\n");
				exit(1);
			}
		}
    }

	fprintf(stderr, "---------------------------------------------------------\n");

	if (cgGLIsProfileSupported(CG_PROFILE_VP30))
    {
		fprintf(stderr, "CG> success : VP30 vertex profile supported on this system.\n");
		vprofile = CG_PROFILE_VP30;
    }
	else
    {
		fprintf(stderr, "CG> warning : VP30 vertex profile NOT supported on this system.\n");
		if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
		{
			fprintf(stderr, "CG> success : ARBVP1 vertex profile supported on this system.\n");
			vprofile = CG_PROFILE_ARBVP1;
		}
		else
		{
			fprintf(stderr, "CG> warning : ARBVP1 vertex profile NOT supported on this system.\n");
			if (cgGLIsProfileSupported(CG_PROFILE_VP20))
			{
				fprintf(stderr, "CG> success : VP20 vertex profile supported on this system.\n");
				vprofile = CG_PROFILE_VP20;
			}
			else
			{
				fprintf(stderr, "CG> warning : VP20 vertex profile NOT supported on this system.\n");
				fprintf(stderr, "CG> error : NO vertex profile supported on this system.\n");
				fprintf(stderr, "---------------------------------------------------------\n");
				exit(1);
			}
		}
    }

	fprintf(stderr, "---------------------------------------------------------\n");

	// adding source text to context
	//shalini commenting out for cutplane
/*	vertexProgram = cgCreateProgramFromFile(
		context, CG_SOURCE, "shaders/vertex.cg", vprofile, "C4E1v_transform", NULL);
	fprintf(stderr, "CG> Compiling Vertex [shaders/vertex.cg]....\n");
	if (!cgIsProgramCompiled(vertexProgram))
		cgCompileProgram(vertexProgram);
	fprintf(stderr, "CG> \t done\n");
	cgGLLoadProgram(vertexProgram);

	ModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj");
*/
	if (fprofile == CG_PROFILE_FP20)
    {
		fragmentProgram = cgCreateProgramFromFile(
			context, CG_SOURCE, "shaders/fragment20.cg", fprofile, "C2E2f_passthrough", NULL);
		fprintf(stderr, "CG> Compiling Fragment [shaders/fragment20.cg] ....\n");
    }
	else
		if (fprofile == CG_PROFILE_FP30)
		{
			fragmentProgram = cgCreateProgramFromFile(
				context, CG_SOURCE, "shaders/fragment30.cg", fprofile, "C2E2f_passthrough", NULL);
			fprintf(stderr, "CG> Compiling Fragment [shaders/fragment30.cg] ....\n");
		}

		if (!cgIsProgramCompiled(fragmentProgram))
			cgCompileProgram(fragmentProgram);
		fprintf(stderr, "CG> \t done\n");
		cgGLLoadProgram(fragmentProgram);

		hTexture0 = cgGetNamedParameter(fragmentProgram, "texture0");
		hTexture1 = cgGetNamedParameter(fragmentProgram, "texture1");

		return 1;

}

void enableCG(unsigned int volTex) {
	cgGLBindProgram(fragmentProgram);
	cgGLEnableProfile(fprofile);

	cgGLSetTextureParameter(hTexture0, global.volume->texName);
	cgGLSetTextureParameter(hTexture1, curTexId);

	cgGLEnableTextureParameter(hTexture0);
	cgGLEnableTextureParameter(hTexture1);
}

void setCurTextureCG(int texName) {
	curTexId = texName;
}

void disableCG() {
	cgGLDisableProfile(vprofile);
	cgGLDisableProfile(fprofile);
	cgGLDisableTextureParameter(hTexture0);
	cgGLDisableTextureParameter(hTexture1);
}

void exitCG() {
//	cgDestroyProgram(vertexProgram);
	cgDestroyProgram(fragmentProgram);
	cgDestroyContext(context);
}

#endif
