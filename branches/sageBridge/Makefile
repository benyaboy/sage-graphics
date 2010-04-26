MACHINE=$(shell uname -s)
SHELL=/bin/bash
preprojects = QUANTA/src
subprojects = app/render app/atlantis app/checker app/FileViewer app/bitplay app/vnc

PREFIX=${RPM_BUILD_ROOT}


default:
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) install; cd - ; done
	@cd src; $(MAKE) install; cd -
	@for i in `echo $(subprojects)`; do cd $$i; $(MAKE); cd - ; done

build: default
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) install; cd - ; done
	@for i in `echo $(subprojects)`; do cd $$i; $(MAKE) install; cd - ; done

install: build
# make directories
	mkdir -p ${PREFIX}/usr/local/sage/bin
	mkdir -p ${PREFIX}/usr/local/sage/bin/fileServer
	mkdir -p ${PREFIX}/usr/local/sage/sageConfig/
	mkdir -p ${PREFIX}/usr/local/sage/sageConfig/applications
	mkdir -p ${PREFIX}/usr/local/sage/sageConfig/fileServer
	mkdir -p ${PREFIX}/usr/local/sage/bin/sageProxy
	mkdir -p ${PREFIX}/usr/local/sage/bin/log
	mkdir -p ${PREFIX}/usr/local/sage/bin/appLauncher
	mkdir -p ${PREFIX}/usr/local/sage/include
ifeq ($(MACHINE), Darwin)
	mkdir -p ${PREFIX}/usr/local/sage/lib
else
	mkdir -p ${PREFIX}/usr/local/sage/lib64
endif
	mkdir -p ${PREFIX}/usr/local/sage/dim
	mkdir -p ${PREFIX}/usr/local/sage/dim/devices
	mkdir -p ${PREFIX}/usr/local/sage/dim/hwcapture
	mkdir -p ${PREFIX}/usr/local/sage/dim/overlays
	mkdir -p ${PREFIX}/usr/local/sage/dim/sounds
	mkdir -p ${PREFIX}/usr/local/sage/dim/ui
	mkdir -p ${PREFIX}/usr/local/sage/ui
	mkdir -p ${PREFIX}/usr/local/sage/ui/misc
	mkdir -p ${PREFIX}/usr/local/sage/ui/connectionManager
	mkdir -p ${PREFIX}/usr/local/sage/ui/images
	mkdir -p ${PREFIX}/usr/local/sage/ui/sessions
	mkdir -p ${PREFIX}/etc/profile.d
	mkdir -p ${PREFIX}/etc/ld.so.conf.d
# copy OS environment settings
	cp etc/sage.csh ${PREFIX}/etc/profile.d
	cp etc/sage.sh ${PREFIX}/etc/profile.d
	cp etc/sage.conf ${PREFIX}/etc/ld.so.conf.d
# copy binaries
	cp bin/fsManager ${PREFIX}/usr/local/sage/bin
	cp bin/sageDisplayManager ${PREFIX}/usr/local/sage/bin
ifeq ($(MACHINE), Darwin)
	cp -r bin/sageDisplayManager.app ${PREFIX}/usr/local/sage/bin
endif
	cp bin/render ${PREFIX}/usr/local/sage/bin
	cp bin/atlantis ${PREFIX}/usr/local/sage/bin
	cp bin/checker ${PREFIX}/usr/local/sage/bin
	cp bin/imageviewer ${PREFIX}/usr/local/sage/bin
	cp bin/bplay-noglut ${PREFIX}/usr/local/sage/bin
	cp bin/bridgeConsole ${PREFIX}/usr/local/sage/bin
	cp bin/fsConsole ${PREFIX}/usr/local/sage/bin
	cp bin/sageBridge ${PREFIX}/usr/local/sage/bin
	cp bin/uiConsole ${PREFIX}/usr/local/sage/bin
	cp bin/VNCViewer ${PREFIX}/usr/local/sage/bin
# copy configuration files
	cp sageConfig/applications/atlantis.conf ${PREFIX}/usr/local/sage/sageConfig/applications
	cp sageConfig/applications/bitplayer.conf ${PREFIX}/usr/local/sage/sageConfig/applications
	cp sageConfig/applications/imageviewer.conf ${PREFIX}/usr/local/sage/sageConfig/applications
	cp sageConfig/applications/render.conf ${PREFIX}/usr/local/sage/sageConfig/applications
	cp sageConfig/applications/VNCViewer.conf ${PREFIX}/usr/local/sage/sageConfig/applications
	cp sageConfig/fsManager.conf ${PREFIX}/usr/local/sage/sageConfig
	cp sageConfig/sageBridge.conf ${PREFIX}/usr/local/sage/sageConfig
	cp sageConfig/stdtile.conf ${PREFIX}/usr/local/sage/sageConfig
	cp sageConfig/stdtile-1.conf ${PREFIX}/usr/local/sage/sageConfig
	cp sageConfig/stdtile-2.conf ${PREFIX}/usr/local/sage/sageConfig
	cp sageConfig/stdtile-20.conf ${PREFIX}/usr/local/sage/sageConfig
# copy utility files
	cp bin/yuv.vert ${PREFIX}/usr/local/sage/bin
	cp bin/yuv.frag ${PREFIX}/usr/local/sage/bin
	cp bin/sage ${PREFIX}/usr/local/sage/bin
	cp bin/sageLauncher.py ${PREFIX}/usr/local/sage/bin
	cp bin/sagePath.py ${PREFIX}/usr/local/sage/bin
	cp bin/subprocess.py ${PREFIX}/usr/local/sage/bin
# copy includes
	cp include/*.h ${PREFIX}/usr/local/sage/include
# copy libraries
ifeq ($(MACHINE), Darwin)
	cp lib/libquanta.dylib ${PREFIX}/usr/local/sage/lib
	cp lib/libsail.dylib ${PREFIX}/usr/local/sage/lib
else
	cp lib/libquanta.so ${PREFIX}/usr/local/sage/lib64
	cp lib/libsail.so ${PREFIX}/usr/local/sage/lib64
endif
# copy appLauncher files
	cp bin/appLauncher/*.py ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/GO ${PREFIX}/usr/local/sage/bin/appLauncher
	cp sageConfig/applications/applications.conf ${PREFIX}/usr/local/sage/sageConfig/applications
	cp bin/appLauncher/README ${PREFIX}/usr/local/sage/bin/appLauncher
# copy file server files
	cp bin/fileServer/*.py ${PREFIX}/usr/local/sage/bin/fileServer
	cp sageConfig/fileServer/fileServer.conf ${PREFIX}/usr/local/sage/sageConfig/fileServer
ifeq ($(MACHINE), Darwin)
	cp -fr bin/fileServer/misc ${PREFIX}/usr/local/sage/bin/fileServer
else
	cp -fru bin/fileServer/misc ${PREFIX}/usr/local/sage/bin/fileServer
endif
# copy sage proxy files
	cp bin/sageProxy/*.py ${PREFIX}/usr/local/sage/bin/sageProxy
# copy UI files
	cp ui/*.py ${PREFIX}/usr/local/sage/ui/
	cp ui/misc/*.py ${PREFIX}/usr/local/sage/ui/misc/
	cp ui/connectionManager/*.py ${PREFIX}/usr/local/sage/ui/connectionManager/
	cp ui/connectionManager/README ${PREFIX}/usr/local/sage/ui/connectionManager/
	cp ui/images/*.jpg ${PREFIX}/usr/local/sage/ui/images/
	cp ui/images/*.png ${PREFIX}/usr/local/sage/ui/images/
	cp ui/images/*.gif ${PREFIX}/usr/local/sage/ui/images/
# compile and copy DIM files
	cp dim/*.py ${PREFIX}/usr/local/sage/dim/
	cp dim/devices/*.py ${PREFIX}/usr/local/sage/dim/devices/
	cp dim/hwcapture/*.py ${PREFIX}/usr/local/sage/dim/hwcapture/
	cp dim/hwcapture/go.bat ${PREFIX}/usr/local/sage/dim/hwcapture/
	cp dim/hwcapture/mouse_to_joy.PIE ${PREFIX}/usr/local/sage/dim/hwcapture/
	cp dim/overlays/*.py ${PREFIX}/usr/local/sage/dim/overlays/


clean:
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) $@; cd - ; done
	@for i in `echo src $(subprojects)`; do cd $$i; $(MAKE) $@; cd - ; done

