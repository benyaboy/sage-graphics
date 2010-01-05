SHELL=/bin/bash
preprojects = QUANTA/src
subprojects = app/render app/atlantis app/checker app/FileViewer app/bitplay app/vnc

PREFIX=${RPM_BUILD_ROOT}

default:
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) install; cd - ; done
	@cd src; $(MAKE) install; cd -
	@for i in `echo $(subprojects)`; do cd $$i; $(MAKE); cd - ; done

install: default
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) $@; cd - ; done
	@for i in `echo $(subprojects)`; do cd $$i; $(MAKE) $@; cd - ; done
# make directories
	mkdir -p ${PREFIX}/usr/local/sage/bin
	mkdir -p ${PREFIX}/usr/local/sage/bin/log
	mkdir -p ${PREFIX}/usr/local/sage/bin/appLauncher
	mkdir -p ${PREFIX}/usr/local/sage/include
	mkdir -p ${PREFIX}/usr/local/sage/lib64
	mkdir -p ${PREFIX}/etc/profile.d
	mkdir -p ${PREFIX}/etc/ld.so.conf.d
# copy OS environment settings
	cp etc/sage.csh ${PREFIX}/etc/profile.d
	cp etc/sage.sh ${PREFIX}/etc/profile.d
	cp etc/sage.conf ${PREFIX}/etc/ld.so.conf.d
# copy binaries
	cp bin/fsManager ${PREFIX}/usr/local/sage/bin
	cp bin/sageDisplayManager ${PREFIX}/usr/local/sage/bin
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
	cp bin/atlantis.conf ${PREFIX}/usr/local/sage/bin
	cp bin/bitplayer.conf ${PREFIX}/usr/local/sage/bin
	cp bin/imageviewer.conf ${PREFIX}/usr/local/sage/bin
	cp bin/render.conf ${PREFIX}/usr/local/sage/bin
	cp bin/VNCViewer.conf ${PREFIX}/usr/local/sage/bin
	cp bin/fsManager.conf ${PREFIX}/usr/local/sage/bin
	cp bin/sageBridge.conf ${PREFIX}/usr/local/sage/bin
	cp bin/stdtile.conf ${PREFIX}/usr/local/sage/bin
	cp bin/stdtile-1.conf ${PREFIX}/usr/local/sage/bin
	cp bin/stdtile-2.conf ${PREFIX}/usr/local/sage/bin
	cp bin/stdtile-20.conf ${PREFIX}/usr/local/sage/bin
# copy utility files
	cp bin/yuv.vert ${PREFIX}/usr/local/sage/bin
	cp bin/yuv.frag ${PREFIX}/usr/local/sage/bin
	cp bin/sage ${PREFIX}/usr/local/sage/bin
	cp bin/sageLauncher.py ${PREFIX}/usr/local/sage/bin
	cp bin/subprocess.py ${PREFIX}/usr/local/sage/bin
# copy includes
	cp include/*.h ${PREFIX}/usr/local/sage/include
# copy libraries
	cp lib/libquanta.so ${PREFIX}/usr/local/sage/lib64
	cp lib/libsail.so ${PREFIX}/usr/local/sage/lib64
# copy appLauncher files
	cp bin/appLauncher/appLauncher.py ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/GO ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/myprint.py ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/request.py ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/data.py ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/subprocess.py ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/KILL_LAUNCHER.py ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/sage.conf ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/README ${PREFIX}/usr/local/sage/bin/appLauncher
	cp bin/appLauncher/admin.py ${PREFIX}/usr/local/sage/bin/appLauncher

clean:
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) $@; cd - ; done
	@for i in `echo src $(subprojects)`; do cd $$i; $(MAKE) $@; cd - ; done

package:
	/bin/rm -fr SAGE
	mkdir -p SAGE/bin
	/bin/cp -f bin/atlantis SAGE/bin  
	/bin/cp -f bin/atlantis.conf SAGE/bin  
	/bin/cp -f bin/fsConsole SAGE/bin  
	/bin/cp -f bin/fsManager.conf SAGE/bin  
	/bin/cp -f bin/ishare.conf SAGE/bin  
	/bin/cp -fr bin/iShareUI.app SAGE/bin  
	/bin/cp -f bin/uiConsole SAGE/bin  
	/bin/cp -f lib/libsail.dylib SAGE/bin
