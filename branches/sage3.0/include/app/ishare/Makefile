include ../../config.mk

CFLAGS=-fPIC -O3 -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS) `wx-config --cflags`
CXXFLAGS=$(CFLAGS)
CC=$(COMPILER)

ifeq ($(MACHINE), Darwin)
   CFLAGS  += -FGLUT -FOpenGL -IFastDXT -march=i686 -msse2 -DDXT_INTR
   LIBS=-lpthread -L../../lib -lsail -framework OpenGL -framework Carbon -framework Cocoa -framework IOKit -lobjc -lm
else
   LIBS=-lpthread -lm -ldl -lglut $(QUANTA_LIB) $(GLEW_LIB) -L../../lib -lsail
endif

PROGRAM = ishare

FILES = ishare.o FastDXT/libdxt.o FastDXT/dxt.o FastDXT/util.o FastDXT/intrinsic.o

default: $(PROGRAM) iShareUI iShareUI.app

install: default
	cp $(PROGRAM) ../../bin
	cp -r iShareUI.app ../../bin

$(PROGRAM): $(FILES)
	$(COMPILER) -o $(PROGRAM) $(FILES) $(LIBS)

iShareUI: ui.o
	g++ -o iShareUI ui.o FastDXT/libdxt.o FastDXT/dxt.o FastDXT/util.o FastDXT/intrinsic.o `wx-config --libs` $(LIBS) -Wl,-install_name,@executable_path/../Frameworks/

iShareUI.app: Info.plist iShareUI version.plist InfoPlist.strings iShareUIMacIcons.icns AnotherResource.txt  
	/bin/rm -fr iShareUI.app
	-mkdir iShareUI.app    
	-mkdir iShareUI.app/Contents
	-mkdir iShareUI.app/Contents/MacOS
	-mkdir iShareUI.app/Contents/Resources
	-mkdir iShareUI.app/Contents/Frameworks
	-mkdir iShareUI.app/Contents/Resources/English.lproj
	/bin/cp -f Info.plist iShareUI.app/Contents/
	/bin/cp -f iShareUI iShareUI.app/Contents/MacOS/iShareUI

	/bin/cp -f ../../lib/libsail.dylib iShareUI.app/Contents/Frameworks
	/bin/cp -f /opt/local/lib/libGLEW.1.5.0.dylib  iShareUI.app/Contents/Frameworks
	/bin/cp -f /opt/local/lib/libSDL-1.2.0.dylib iShareUI.app/Contents/Frameworks
	/bin/cp -f /opt/local/lib/libportaudio.2.dylib iShareUI.app/Contents/Frameworks

	install_name_tool -change /opt/local/lib/libSDL-1.2.0.dylib @executable_path/../Frameworks/libSDL-1.2.0.dylib iShareUI.app/Contents/Frameworks/libsail.dylib
	install_name_tool -change /opt/local/lib/libGLEW.1.5.0.dylib @executable_path/../Frameworks/libGLEW.1.5.0.dylib iShareUI.app/Contents/Frameworks/libsail.dylib
	install_name_tool -change /opt/local/lib/libportaudio.2.dylib @executable_path/../Frameworks/libportaudio.2.dylib iShareUI.app/Contents/Frameworks/libsail.dylib
	install_name_tool -change libsail.dylib @executable_path/../Frameworks/libsail.dylib iShareUI.app/Contents/MacOS/iShareUI

	/bin/cp -f version.plist iShareUI.app/Contents/
	/bin/cp -f InfoPlist.strings iShareUI.app/Contents/Resources/English.lproj/

	Rez -d __DARWIN__ -t APPL -d __WXMAC__ -i . -d WXUSINGDLL -o iShareUI Carbon.r mac.r

	sed -e "s/IDENTIFIER/`echo . | sed -e 's,\.\./,,g' | sed -e 's,/,.,g'`/" \
		-e "s/EXECUTABLE/iShareUI/" \
		-e "s/VERSION/2.8.8/" \
		Info.plist.in > iShareUI.app/Contents/Info.plist

	echo -n 'APPL????' > iShareUI.app/Contents/PkgInfo
	/bin/cp -f iShareUIMacIcons.icns AnotherResource.txt iShareUI.app/Contents/Resources/

clean:
	/bin/rm -fr *~ *.o $(PROGRAM) iShareUI.app iShareUI

