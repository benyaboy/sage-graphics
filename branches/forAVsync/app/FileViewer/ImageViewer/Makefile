DXT=1   # comment out for no DXT support

SAGE_DIR = ../../..

CFLAGS=`Wand-config --cflags --cppflags`  -I$(SAGE_DIR)/include -fno-stack-protector
LIBS= `Wand-config --ldflags --libs` -L/usr/local/lib -lpthread -lstdc++ -lltdl -L$(SAGE_DIR)/lib -lsail  -lquanta

FILES=imageviewer.o 
TARGET=imageviewer


#----------------  BEGIN FastDXT stuff  ----------------#
ifdef DXT
#echo "USING DXT"
FILES+=dxt.o libdxt.o util.o intrinsic.o
TARGET+=imgToDxt
CFLAGS+=-DUSE_DXT

OPTIM=1

MACHINE=$(shell uname -s)
ARCHITECTURE=$(shell uname -p)

ifeq ($(MACHINE), Darwin)

CFLAGS+=-I/opt/local/include

ifeq ($(ARCHITECTURE), i386)
	# Intel Mac

ifeq ($(OPTIM), 1)
# MacPro
# CXXFLAGS+=-march=nocona -msse3 -DDXT_INTR

# MacbookPro
CFLAGS+=-march=i686 -msse2 -DDXT_INTR
endif

endif

ifeq ($(ARCHITECTURE), powerpc)
	# PowerPC Mac
#echo "Disabling SSE instructions on PowerPC"
CFLAGS+=
endif
endif

ifeq ($(ARCHITECTURE), x86_64)
	# Opteron
CFLAGS+=-march=opteron -msse2 -DDXT_INTR

else

	# everything considered i386/linux
CFLAGS+=-msse2 -DDXT_INTR

endif


endif
#------------------  END FastDXT stuff  -------------------#



default: $(TARGET)

install: $(TARGET)
	/bin/cp -f imageviewer $(SAGE_DIR)/bin
ifdef DXT
	/bin/cp -f imgToDxt dirToDxt.py $(SAGE_DIR)/bin/file_server
endif
	
imageviewer: $(FILES)
	g++ -o imageviewer $(FILES) $(LIBS)

imgToDxt: imgToDxt.o dxt.o libdxt.o util.o intrinsic.o 
	g++ -o imgToDxt imgToDxt.o dxt.o libdxt.o util.o intrinsic.o $(LIBS)

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *.o *~ imageviewer imgToDxt

