# To enable audio, uncomment the following line
AUDIO=1
#GLSL_YUV=1
#SAIL_ONLY=1
#SUN_GCC=1
#FS_CONSOLE=fsConsole

COMPILER=g++
SAGE_CFLAGS=-Wno-deprecated -fPIC
SAGE_LDFLAGS=

# QUANTA settings
QUANTA_DIR=${SAGE_DIRECTORY}/QUANTA
QUANTA_CFLAGS=-I${QUANTA_DIR}/include
QUANTA_LDFLAGS=-L${SAGE_DIRECTORY}/lib -lquanta
QUANTA_LIB=libquanta.so

# SDL
ifndef SAIL_ONLY
SDL_CFLAGS=`sdl-config --cflags`
SDL_LIBS=`sdl-config --libs`
endif

# GLUT
GLUT_CFLAGS=
GLUT_LDFLAGS=-lglut

# READLINE settings
READLINE_CFLAGS=
READLINE_LIB=-lreadline

ifdef AUDIO
#PORTAUDIO
PORTAUDIO_DIR=/usr/local
PORTAUDIO_CFLAGS=-I${PORTAUDIO_DIR}/include -DSAGE_AUDIO
PAUDIO_LIB= -L${PORTAUDIO_DIR}/lib -lportaudio -lasound
endif

# SDL_ttf (freetype font library)
#FONT_LIB=-lSDL_ttf
FONT_LIB=

# imagemagick
MAGICK_CFLAGS=`Wand-config --cflags --cppflags`
MAGICK_LIBS=`Wand-config --ldflags --libs`

# SAIL library name
SAIL_LIB=libsail.so

# how to build a shared library
SHLD_FLAGS=-shared 

# GPU programming setting
ifeq ($(GLSL_YUV), 1)
  GLEW_LIB= -lGLEW
  GLEW_CFLAGS=
  GLSL_YUV_DEFINE=-DGLSL_YUV
else
  GLEW_LIB=
  GLEW_CFLAGS=
  GLSL_YUV_DEFINE=
endif

# Machine specific settings
MACHINE=$(shell uname -s)
   #Darwin  MacOSX
   #Linux   linux
   #Solaris SunOS

ARCHITECTURE=$(shell uname -p)
   #i386  MacOSX
   #i686    Linux 32bit
   #x86_64  Linux 64bit
   #ia64    Linux Itanium 64bit
   #ppc64   Linux PPC PS3

ifeq ($(MACHINE), Darwin)
  # SAIL library name
  SAIL_LIB=libsail.dylib

ifdef AUDIO
  PORTAUDIO_DIR=/opt/local
  PORTAUDIO_CFLAGS=-I${PORTAUDIO_DIR}/include -DSAGE_AUDIO
  PAUDIO_LIB= -L${PORTAUDIO_DIR}/lib -lportaudio
endif

  # QUANTA library name
  QUANTA_LIB=libquanta.dylib

  # READLINE settings
  READLINE_CFLAGS=-I/opt/local/include 
  READLINE_LIB=-L/opt/local/lib -lreadline

  # Lower-level graphics library
  XLIBS= -framework OpenGL -lobjc -framework aGL -framework Quicktime -framework Carbon -framework QuartzCore

  # how to build a shared library
  SHLD_FLAGS=-dynamiclib -flat_namespace -undefined suppress

else

ifeq ($(ARCHITECTURE), x86_64)
  # Lower-level graphics library
  XLIBS=-L/usr/X11R6/lib64 -lGLU -lGL -lXmu -lXi -lXext -lX11
else

ifeq ($(ARCHITECTURE), ppc64)
  # SDL
  SDL_CFLAGS=`sdl-config --cflags`
  SDL_LIBS=-lSDL

  # Lower-level graphics library
  XLIBS=-L/usr/X11R6/lib -lGLU -lGL -lXmu -lXi -lXext -lX11
else

ifeq ($(ARCHITECTURE), ia64)
  # Lower-level graphics library
  XLIBS=-L/usr/X11R6/lib64 -lGLU -lGL -lXmu -lXi -lXext -lX11
else

  # anything else is 32bit 

  # READLINE settings
  READLINE_LIB=-lreadline

  # Lower-level graphics library
  XLIBS=-L/usr/X11R6/lib -lGLU -lGL -lXmu -lXi -lXext -lX11

ifdef AUDIO
  PAUDIO_LIB= -L${PORTAUDIO_DIR}/lib -lportaudio -lasound
endif

endif
endif
endif
endif


ifeq ($(MACHINE), SunOS)

ifndef SUN_GCC
COMPILER=CC
SAGE_CFLAGS=-Kpic -m64
SUN_LIBS=-lrt -lnsl -lsocket -lCrun -lCstd
SHLD_FLAGS=-G -Kpic -m64 
else
COMPILER=c++
SAGE_CFLAGS=-m64 -Wno-deprecated -fPIC 
SUN_LIBS=-lrt -lnsl -lsocket 
SHLD_FLAGS=-shared -m64 
endif

SAGE_LDFLAGS=-m64
SUN_INCLUDE=-I/usr/local/include 

GLUT_CFLAGS=-I/opt/SUNWfreeglut/include
GLUT_LDFLAGS=-L/opt/SUNWfreeglut/lib/amd64 -lglut

# READLINE settings
READLINE_CFLAGS=-I/usr/local/include 
READLINE_LIB=-L/usr/local/lib/64 -lreadline -lcurses

endif

