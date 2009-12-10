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

//vARB.cpp
//has the CG related code from Luc Renambot

#if defined(ADDARBGL)

#include "vARB.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glext.h>


const char *shader_code1 =
"!!ARBfp1.0\n"
"# cgc version 1.3.0001, build date Jul 16 2004 12:35:04\n"
"# command line args: -profile arbfp1 -entry C2E2f_passthrough\n"
"# source file: fragment20.cg\n"
"#vendor NVIDIA Corporation\n"
"#version 1.0.02\n"
"#profile arbfp1\n"
"#program C2E2f_passthrough\n"
"#semantic C2E2f_passthrough.texture0\n"
"#semantic C2E2f_passthrough.texture1\n"
"#var float3 texCoord0 : $vin.TEX0 : TEX0 : 0 : 1\n"
"#var sampler3D texture0 :  : texunit 0 : 1 : 1\n"
"#var sampler2D texture1 :  : texunit 1 : 2 : 1\n"
"#var float4 C2E2f_passthrough : $vout.COL : COL : -1 : 1\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP R0;\n"
"TEX R0.xw, fragment.texcoord[0], texture[0], 3D;\n"
"TEX result.color, R0.wxzw, texture[1], 2D;\n"
"END\n"
"# 2 instructions, 1 R-regs\n";

const char *shader_code =
"!!ARBfp1.0\n"
"#var float3 texCoord0 : $vin.TEXCOORD0 : TEXCOORD0 : 0 : 1\n"
"#var sampler3D texture0 :  : texunit 0 : 1 : 1\n"
"#var sampler2D texture1 :  : texunit 1 : 2 : 1\n"
"#var float4 C2E2f_passthrough : $vout.COLOR : COLOR : -1 : 1\n"
"TEMP R0;\n"
"TEX R0.xw, fragment.texcoord[0], texture[0], 3D;\n"
"TEX result.color, R0.wxww, texture[1], 2D;\n"
"END\n";

// WORKS
const char *shader_code3 =
"!!ARBfp1.0\n"
"OUTPUT out = result.color;\n"
"TEMP temp;\n"
"PARAM pink = { 1.0, 0.4, 0.4, 0.0}; \n"
"TEX temp, fragment.texcoord[0], texture[0], 3D;\n"
"ADD out, temp, pink;\n"
"END\n";



GLuint vertexProgram, fragmentProgram;
int curTexId ;

void handleARBError()
{
    fprintf(stderr, "ARB> error \n");
    exit(1);
}

int initARB()
{
	int notgood;
        const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
        size_t sysExtsLen = strlen(extensions);

	notgood = 0;

        if(strstr(extensions,"GL_ARB_vertex_program"))
                fprintf(stderr, "GL_ARB_vertex_program supported\n");
        else
        {
                fprintf(stderr, "GL_ARB_vertex_program NOT supported\n");
		notgood = 1;
        }
        if(strstr(extensions,"GL_ARB_fragment_program"))
                fprintf(stderr, "GL_ARB_fragment_program supported\n");
        else
        {
                fprintf(stderr, "GL_ARB_fragment_program NOT supported\n");
		notgood = 1;
        }

	if (notgood)
	{
		fprintf(stderr, "ARB> error : fragment and vertex program required (GL_ARB_fragment_program, GL_ARB_vertex_program).\n");
		fprintf(stderr, "---------------------------------------------------------\n");
		exit(1);
	}
	else
	{
		GLint maxInsts, maxTemps, maxParams;
		glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, &maxInsts );
		glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB, &maxTemps );
		glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB, &maxParams );
		fprintf(stderr, "ARB> GL_ARB_fragment_program, GL_ARB_vertex_program present\n");
		fprintf(stderr, "ARB> \t %d max instructions, %d max temp varriables, %d max variables\n",
			maxInsts, maxTemps, maxParams);
		fprintf(stderr, "---------------------------------------------------------\n");
	}

        glGenProgramsARB(1, &fragmentProgram);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragmentProgram);
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei) strlen(shader_code), (GLubyte *) shader_code);

        GLint error_pos;
        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_pos);
        if (error_pos != -1)
{
                const GLubyte *error_string;
                error_string = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
                fprintf(stderr, "Program error at position: %d\n%s\n", error_pos, error_string);
        }
	else
	{
		GLint maxInsts, maxTemps, maxParams;
		glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_INSTRUCTIONS_ARB, &maxInsts );
		glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_TEMPORARIES_ARB, &maxTemps );
		glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_PARAMETERS_ARB, &maxParams );
		fprintf(stderr, "ARB> GL_ARB_fragment_program, GL_ARB_vertex_program present\n");
		fprintf(stderr, "ARB> \t %d current instructions, %d current varriables, %d current variables\n",
			maxInsts, maxTemps, maxParams);
		fprintf(stderr, "---------------------------------------------------------\n");
	}

	//hTexture0 = cgGetNamedParameter(fragmentProgram, "texture0");
	//hTexture1 = cgGetNamedParameter(fragmentProgram, "texture1");

	return 1;
}

void enableARB(unsigned int volTex)
{
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragmentProgram);

	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, global.volume->texName, curTexId, 0, 0);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_3D, global.volume->texName);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, curTexId);

    glActiveTextureARB(GL_TEXTURE0_ARB);

	//cgGLSetTextureParameter(hTexture0, global.volume->texName);
	//cgGLSetTextureParameter(hTexture1, curTexId);

	//cgGLEnableTextureParameter(hTexture0);
	//cgGLEnableTextureParameter(hTexture1);


	//glActiveTextureARB( GL_TEXTURE0_ARB );
	//glBindTexture(GL_TEXTURE_3D, global.volume->texName);
	//glActiveTextureARB( GL_TEXTURE1_ARB );
	//glBindTexture(GL_TEXTURE_3D, curTexId);

}

void setCurTextureARB(int texName)
{
	curTexId = texName;
}

void disableARB()
{
    glDisable(GL_FRAGMENT_PROGRAM_ARB);

	//cgGLDisableTextureParameter(hTexture0);
	//cgGLDisableTextureParameter(hTexture1);
}

void exitARB()
{
	glDeleteProgramsARB(1, &fragmentProgram);
}

#endif
