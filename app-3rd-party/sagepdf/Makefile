include ../../config.mk
SAGE_DIR = ../..
#include ${SAGE_DIRECTORY}/config.mk
#SAGE_DIR=${SAGE_DIRECTORY}

USE_POPPLER=$(shell pkg-config --atleast-version=0.5 poppler && echo 1)
#USE_POPPLER=0

SDIR=${SAGE_DIR}
QUANTA_CFLAGS=-I${SDIR}/QUANTA/include
QUANTA_LDFLAGS=-L${SDIR}/lib -lquanta

ifeq ($(USE_POPPLER), 1)

USE_POPPLER12=$(shell pkg-config --atleast-version=0.12 poppler && echo 1)

ifeq ($(USE_POPPLER12), 1)
CFLAGS=-DUSE_POPPLER -DSAGE_POPPLER_VERSION=5 `pkg-config --cflags poppler-glib` `pkg-config --cflags gtk+-2.0` -I$(SAGE_DIR)/include -fno-stack-protector $(QUANTA_CFLAGS) $(PORTAUDIO_CFLAGS)
LIBS= `pkg-config --libs poppler-glib` `pkg-config --libs gtk+-2.0` -L$(SAGE_DIR)/lib -lsail $(QUANTA_LDFLAGS) $(PAUDIO_LIB) -lpthread
NOSAGE_CFLAGS=-DUSE_POPPLER -DSAGE_POPPLER_VERSION=5 `pkg-config --cflags poppler-glib` `pkg-config --cflags gtk+-2.0` -fno-stack-protector $(PORTAUDIO_CFLAGS)
NOSAGE_LIBS= `pkg-config --libs poppler-glib` `pkg-config --libs gtk+-2.0` $(PAUDIO_LIB) -lpthread
else
CFLAGS=-DUSE_POPPLER -DSAGE_POPPLER_VERSION=12 `pkg-config --cflags poppler` `pkg-config --cflags gtk+-2.0` -I$(SAGE_DIR)/include -fno-stack-protector $(QUANTA_CFLAGS) $(PORTAUDIO_CFLAGS)
LIBS= `pkg-config --libs poppler` `pkg-config --libs gtk+-2.0` -L$(SAGE_DIR)/lib -lsail $(QUANTA_LDFLAGS) $(PAUDIO_LIB) -lpthread
NOSAGE_CFLAGS=-DUSE_POPPLER -DSAGE_POPPLER_VERSION=12 `pkg-config --cflags poppler` `pkg-config --cflags gtk+-2.0` -fno-stack-protector $(PORTAUDIO_CFLAGS)
NOSAGE_LIBS= `pkg-config --libs poppler` `pkg-config --libs gtk+-2.0` $(PAUDIO_LIB) -lpthread
endif

else
	error - need poppler
endif


#----------------  BEGIN FastDXT stuff  ----------------#

OPTIM=1

ifeq ($(MACHINE), Darwin)

CFLAGS+=-m32
LIBS+=-L/opt/local/lib

CFLAGS+=-I/opt/local/include

ifeq ($(ARCHITECTURE), i386)
        # Intel Mac

ifeq ($(OPTIM), 1)
# MacPro
# CXXFLAGS+=-march=nocona -msse3 -DDXT_INTR

# MacbookPro
# CFLAGS+=-march=i686 -msse2 -DDXT_INTR
CFLAGS+=-DDXT_INTR
endif

endif

ifeq ($(ARCHITECTURE), powerpc)
        # PowerPC Mac
echo "Disabling SSE instructions on PowerPC"
CFLAGS+=
endif
endif

ifeq ($(ARCHITECTURE), x86_64)
CFLAGS+=-msse2 -DDXT_INTR

else

        # everything considered i386/linux
CFLAGS+=-msse2 -DDXT_INTR

endif



#------------------  END FastDXT stuff  -------------------#



default:	sagepdf

install:	sagepdf
		install -m 0755 sagepdf ${SAGE_DIR}/bin/

sagepdf:	sagepdf.o dxt.o libdxt.o util.o intrinsic.o
ifeq ($(MACHINE), Darwin)
		g++ -m32 -o sagepdf sagepdf.o dxt.o libdxt.o intrinsic.o $(LIBS)
else
		g++ -o sagepdf sagepdf.o dxt.o libdxt.o intrinsic.o $(LIBS)
endif


nosage:		sagepdf.cpp
		g++ -DNOSAGE $(NOSAGE_CFLAGS) -o sagepdf sagepdf.cpp $(NOSAGE_LIBS)


%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<

clean:
	rm -f sagepdf *.o


