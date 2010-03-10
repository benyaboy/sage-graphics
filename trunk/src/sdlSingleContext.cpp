/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Module: sdlSingleContext.cpp
 * Author : Byungil Jeong
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
 * Direct questions, comments etc about SAGE to sage_users@listserv.uic.edu or 
 * http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/

#if defined(GLSL_YUV)
#if !defined(WIN32)
#define GLEW_STATIC 1
#endif
#include <GL/glew.h>
#include <GL/glu.h>
#include <fcntl.h>
GLhandleARB FSHandle,PHandle;
#endif

#include "sdlSingleContext.h"

#if defined(GLSL_YUV)

#define GLSLVertexShader   1
#define GLSLFragmentShader 2

int GLSLprintlError(char *file, int line)
{
   //
   // Returns 1 if an OpenGL error occurred, 0 otherwise.
   //
   GLenum glErr;
   int    retCode = 0;

   glErr = glGetError();
   while (glErr != GL_NO_ERROR)
   {
      fprintf(stderr, "GLSL> glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
      retCode = 1;
      glErr = glGetError();
   }
   return retCode;
}


//
// Print out the information log for a shader object
//
static
void GLSLprintShaderInfoLog(GLuint shader)
{
   int infologLength = 0;
   int charsWritten  = 0;
   GLchar *infoLog;

   GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors

   glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

   GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors

   if (infologLength > 0)
   {
      infoLog = (GLchar *)malloc(infologLength);
      if (infoLog == NULL)
      {
         fprintf(stderr, "GLSL> ERROR: Could not allocate InfoLog buffer\n");
         exit(1);
      }
      glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
      fprintf(stderr, "GLSL> Shader InfoLog:%s\n", infoLog);
      free(infoLog);
   }
   GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors
}

//
// Print out the information log for a program object
//
static
void GLSLprintProgramInfoLog(GLuint program)
{
   int infologLength = 0;
   int charsWritten  = 0;
   GLchar *infoLog;

   GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors

   glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

   GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors

   if (infologLength > 0)
   {
      infoLog = (GLchar *)malloc(infologLength);
      if (infoLog == NULL)
      {
         fprintf(stderr, "GLSL> ERROR: Could not allocate InfoLog buffer\n");
         exit(1);
      }
      glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
      fprintf(stderr, "GLSL> Program InfoLog:%s\n", infoLog);
      free(infoLog);
   }
   GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors
}

static
int GLSLShaderSize(char *fileName, int shaderType)
{
   //
   // Returns the size in bytes of the shader fileName.
   // If an error occurred, it returns -1.
   //
   // File name convention:
   //
   // <fileName>.vert
   // <fileName>.frag
   //
   int fd;
   char name[256];
   int count = -1;

   memset(name, 0, 256);
   strcpy(name, fileName);

   switch (shaderType)
   {
      case GLSLVertexShader:
         strcat(name, ".vert");
         break;
      case GLSLFragmentShader:
         strcat(name, ".frag");
         break;
      default:
         fprintf(stderr, "GLSL> ERROR: unknown shader file type\n");
         exit(1);
         break;
   }

   //
   // Open the file, seek to the end to find its length
   //
#ifdef WIN32
   fd = _open(name, _O_RDONLY);
   if (fd != -1)
   {
      count = _lseek(fd, 0, SEEK_END) + 1;
      _close(fd);
   }
#else
   fd = open(name, O_RDONLY);
   if (fd != -1)
   {
      count = lseek(fd, 0, SEEK_END) + 1;
      close(fd);
   }
#endif
   return count;
}

static
int GLSLreadShader(char *fileName, int shaderType, char *shaderText, int size)
{
   //
   // Reads a shader from the supplied file and returns the shader in the
   // arrays passed in. Returns 1 if successful, 0 if an error occurred.
   // The parameter size is an upper limit of the amount of bytes to read.
   // It is ok for it to be too big.
   //
   FILE *fh;
   char name[100];
   int count;

   strcpy(name, fileName);

   switch (shaderType) 
   {
      case GLSLVertexShader:
         strcat(name, ".vert");
         break;
      case GLSLFragmentShader:
         strcat(name, ".frag");
         break;
      default:
         fprintf(stderr, "GLSL> ERROR: unknown shader file type\n");
         exit(1);
         break;
   }

   //
   // Open the file
   //
   fh = fopen(name, "r");
   if (!fh)
      return -1;

   //
   // Get the shader from a file.
   //
   fseek(fh, 0, SEEK_SET);
   count = (int) fread(shaderText, 1, size, fh);
   shaderText[count] = '\0';

   if (ferror(fh))
      count = 0;

   fclose(fh);
   return count;
}


int GLSLreadShaderSource(char *fileName, GLchar **vertexShader, GLchar **fragmentShader)
{
   int vSize, fSize;

   //
   // Allocate memory to hold the source of our shaders.
   //

   fprintf(stderr, "GLSL> load shader %s\n", fileName);
   if (vertexShader) {
      vSize = GLSLShaderSize(fileName, GLSLVertexShader);

      if (vSize == -1) {
         fprintf(stderr, "GLSL> Cannot determine size of the vertex shader %s\n", fileName);
         return 0;
      }
      
      *vertexShader = (GLchar *) malloc(vSize);
   
      //
      // Read the source code
      //
      if (!GLSLreadShader(fileName, GLSLVertexShader, *vertexShader, vSize)) {
         fprintf(stderr, "GLSL> Cannot read the file %s.vert\n", fileName);
         return 0;
      }
   }
   
   if (fragmentShader) {
      fSize = GLSLShaderSize(fileName, GLSLFragmentShader);

      if (fSize == -1) {
         fprintf(stderr, "GLSL> Cannot determine size of the fragment shader %s\n", fileName);
         return 0;
      }

      *fragmentShader = (GLchar *) malloc(fSize);

      if (!GLSLreadShader(fileName, GLSLFragmentShader, *fragmentShader, fSize)) {
         fprintf(stderr, "GLSL> Cannot read the file %s.frag\n", fileName);
         return 0;
      }
   }
   
   return 1;
}

GLuint GLSLinstallShaders(const GLchar *Vertex, const GLchar *Fragment)
{
   GLuint VS, FS, Prog;   // handles to objects
   GLint  vertCompiled, fragCompiled;    // status values
   GLint  linked;

   // Create a program object
   Prog = glCreateProgram();
   
   // Create a vertex shader object and a fragment shader object
   if (Vertex) {
      VS = glCreateShader(GL_VERTEX_SHADER);
      
      // Load source code strings into shaders
      glShaderSource(VS, 1, &Vertex, NULL);

      // Compile the vertex shader, and print out
      // the compiler log file.

      glCompileShader(VS);
      GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors
      glGetShaderiv(VS, GL_COMPILE_STATUS, &vertCompiled);
      GLSLprintShaderInfoLog(VS);

      if (!vertCompiled)
         return 0;
        
      glAttachShader(Prog, VS); 
   }
   
   if (Fragment) {
      FS = glCreateShader(GL_FRAGMENT_SHADER);

      glShaderSource(FS, 1, &Fragment, NULL);

      glCompileShader(FS);
      GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors
      glGetShaderiv(FS, GL_COMPILE_STATUS, &fragCompiled);
      GLSLprintShaderInfoLog(FS);

      if (!fragCompiled)
         return 0;
        
      glAttachShader(Prog, FS);
   }
   
   // Link the program object and print out the info log
   glLinkProgram(Prog);
   GLSLprintlError(__FILE__, __LINE__);  // Check for OpenGL errors
   glGetProgramiv(Prog, GL_LINK_STATUS, &linked);
   GLSLprintProgramInfoLog(Prog);

   if (!linked)
      return 0;

   return Prog;     
}

#endif

int sdlSingleContext::init(struct sageDisplayConfig &cfg)
{
   singleContext = true;
   configStruct = cfg;

#if defined(WIN32)
   // Weird position with windows
   _putenv("SDL_VIDEO_CENTERED=1");
#endif

   if ( configStruct.fullScreenFlag ) {
      char posStr[SAGE_NAME_LEN];
      sprintf(posStr, "SDL_VIDEO_WINDOW_POS=%d,%d",configStruct.winX, configStruct.winY);
      putenv(posStr);
   }   

   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      sage::printLog("Unable to initialize SDL: %s", SDL_GetError());
      return -1;
   }


   tileNum = cfg.dimX * cfg.dimY;
   if (tileNum > MAX_TILES_PER_NODE) {
      sage::printLog("sdlSingleContext::init() : The tile number exceeds the maximum"); 
      return -1;
   }

   if (!winCreatFlag) {
      Uint32 flags = SDL_OPENGL;
      if ( configStruct.fullScreenFlag ) {
         flags |= SDL_NOFRAME;
         flags |= SDL_RESIZABLE;
         flags |= SDL_FULLSCREEN;
      }
      
      window_width  = cfg.width*cfg.dimX;
      window_height = cfg.height*cfg.dimY;
      surface = SDL_SetVideoMode(window_width, window_height, 0, flags);
      if (surface == NULL) {
         sage::printLog("Unable to create OpenGL screen: %s", SDL_GetError());
         SDL_Quit();
         exit(2);
      }
      
      SDL_WM_SetCaption("SAGE DISPLAY", NULL);
      winCreatFlag = true;
      
      if ( configStruct.fullScreenFlag )
         SDL_ShowCursor(SDL_DISABLE);

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);
      
#if defined(GLSL_YUV)
      // Initialize the "OpenGL Extension Wrangler" library
      glewInit();

      // Load the shaders

      GLchar *FragmentShaderSource;
      GLchar *VertexShaderSource;

      char *sfn, *sd;
      sfn = (char*)malloc(256);
      memset(sfn, 0, 256);
      sd = getenv("SAGE_DIRECTORY");
      sprintf(sfn, "%s/bin/yuv", sd);

      GLSLreadShaderSource(sfn, &VertexShaderSource, &FragmentShaderSource);
      PHandle = GLSLinstallShaders(VertexShaderSource, FragmentShaderSource);

      /* Finally, use the program. */
      glUseProgramObjectARB(PHandle);

      free(sfn);

      glUseProgramObjectARB(0);
#endif

      sage::printLog("sdlSingleContext:init(): Window created");
   }

   return 0;
} // End of sdlSingleContext::init()

void sdlSingleContext::clearScreen()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sdlSingleContext::setupViewport(int i, sageRect &tileRect)
{
   glViewport(tileRect.x, tileRect.y, tileRect.width, tileRect.height);
}

void sdlSingleContext::refreshScreen()
{
   SDL_GL_SwapBuffers();
}

void sdlSingleContext::changeBackground(int red, int green, int blue)
{
   glClearColor((float)red/255.0f, (float)green/255.0f, (float)blue/255.0f, 0.0f);
}

sdlSingleContext::~sdlSingleContext()
{
   SDL_Quit();
} //End of sageDisplay::~sageDisplay()


void sdlSingleContext::checkEvent()
{
   // Event management with SDL
   SDL_Event e;
 
   // Service all queued events
   while (SDL_PollEvent(&e))
   {
      // Handle control keys
      if (e.type == SDL_KEYDOWN)
      {
         switch (e.key.keysym.sym)
         {
            case SDLK_ESCAPE:
               e.type = SDL_QUIT;
               break;
            case SDLK_PRINT:
               break;
            default:
               break;
         }
      }

      // More events
      switch (e.type)
      {
         case SDL_MOUSEMOTION:
            break;
         case SDL_MOUSEBUTTONDOWN:
            break;
         case SDL_MOUSEBUTTONUP:
            break;
         case SDL_JOYBUTTONDOWN:
            break;
         case SDL_JOYBUTTONUP:
            break;
         case SDL_KEYDOWN:
            break;
         case SDL_KEYUP:
            break;
         case SDL_USEREVENT:
            break;
         case SDL_VIDEOEXPOSE:
            break;
         default:
            break;
      }
  
      // Handle a clean exit.
      if (e.type == SDL_QUIT)
      {
         sage::printLog("Quitting\n");
         SDL_Quit();
         exit(2);
      }

		//It makes flickering when you move mouse
		//It was happended from OS: CentOS, Touch screen. 
		//We don't need to refresh screen by mouse/keyboard event.
		//So, refreshScreen function call is blocked
		//-H
      //refreshScreen();
   }
}
