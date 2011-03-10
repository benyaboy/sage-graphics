include ../../../config.mk
SAGE_DIR = ../../..

# use the PDF Poppler library
#USE_POPPLER=1
# minimum poppler version 0.5.x
USE_POPPLER=$(shell pkg-config --atleast-version=0.5 poppler && echo 1)

SDIR=${SAGE_DIR}
QUANTA_CFLAGS=-I${SDIR}/QUANTA/include
QUANTA_LDFLAGS=-L${SDIR}/lib -lquanta


ifeq ($(USE_POPPLER), 1)

USE_POPPLER12=$(shell pkg-config --atleast-version=0.12 poppler && echo 1)

ifeq ($(USE_POPPLER12), 1)
CFLAGS=-DUSE_POPPLER -DSAGE_POPPLER_VERSION=12 `pkg-config --cflags poppler` `Wand-config --cflags --cppflags`  -I$(SAGE_DIR)/include -fno-stack-protector $(QUANTA_CFLAGS) $(PORTAUDIO_CFLAGS)
LIBS= `pkg-config --libs poppler` `Wand-config --ldflags --libs` -L$(SAGE_DIR)/lib -lsail $(QUANTA_LDFLAGS) $(PAUDIO_LIB) -lpthread
else
CFLAGS=-DUSE_POPPLER -DSAGE_POPPLER_VERSION=5 `pkg-config --cflags poppler-glib` `Wand-config --cflags --cppflags`  -I$(SAGE_DIR)/include -fno-stack-protector $(QUANTA_CFLAGS) $(PORTAUDIO_CFLAGS)
LIBS= `pkg-config --libs poppler-glib` `Wand-config --ldflags --libs` -L$(SAGE_DIR)/lib -lsail $(QUANTA_LDFLAGS) $(PAUDIO_LIB) -lpthread
endif

else

CFLAGS=`Wand-config --cflags --cppflags`  -I$(SAGE_DIR)/include -fno-stack-protector $(QUANTA_CFLAGS) $(PORTAUDIO_CFLAGS)
LIBS=`Wand-config --ldflags --libs` -L$(SAGE_DIR)/lib -lsail $(QUANTA_LDFLAGS) $(PAUDIO_LIB) -lpthread -lstdc++

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



default: pdfviewer

install: pdfviewer
	/bin/cp -f pdfviewer $(SAGE_DIR)/bin

pdfviewer: pdfviewer.o dxt.o libdxt.o util.o intrinsic.o 
ifeq ($(MACHINE), Darwin)
	g++ -m32 -o pdfviewer pdfviewer.o dxt.o libdxt.o intrinsic.o $(LIBS)
else
	g++ -o pdfviewer pdfviewer.o dxt.o libdxt.o intrinsic.o $(LIBS)
endif

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *.o *~ pdfviewer

