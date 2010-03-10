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
	mkdir -p ${PREFIX}/usr/local/sage/bin/log
	mkdir -p ${PREFIX}/usr/local/sage/bin/appLauncher
	mkdir -p ${PREFIX}/usr/local/sage/include
	mkdir -p ${PREFIX}/usr/local/sage/lib64
	mkdir -p ${PREFIX}/usr/local/sage/ui
	mkdir -p ${PREFIX}/usr/local/sage/ui/data
	mkdir -p ${PREFIX}/usr/local/sage/ui/misc
	mkdir -p ${PREFIX}/usr/local/sage/ui/saved-states
	mkdir -p ${PREFIX}/usr/local/sage/ui/usersServer
	mkdir -p ${PREFIX}/usr/local/sage/ui/images
	mkdir -p ${PREFIX}/usr/local/sage/ui/prefs
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
# copy UI files
	cp ui/SAGESession.py ${PREFIX}/usr/local/sage/ui/SAGESession.py
	cp ui/SAGEShape.py ${PREFIX}/usr/local/sage/ui/SAGEShape.py
	cp ui/help.py ${PREFIX}/usr/local/sage/ui/help.py
	cp ui/usersServer/UsersServer.py ${PREFIX}/usr/local/sage/ui/usersServer/UsersServer.py
	cp ui/usersServer/README ${PREFIX}/usr/local/sage/ui/usersServer/README
	cp ui/usersServer/admin.py ${PREFIX}/usr/local/sage/ui/usersServer/admin.py
	cp ui/sageGateBase.py ${PREFIX}/usr/local/sage/ui/sageGateBase.py
	cp ui/RECENT_CHANGES ${PREFIX}/usr/local/sage/ui/RECENT_CHANGES
	cp ui/misc/imsize.py ${PREFIX}/usr/local/sage/ui/misc/imsize.py
	cp ui/misc/__init__.py ${PREFIX}/usr/local/sage/ui/misc/__init__.py
	cp ui/globals.py ${PREFIX}/usr/local/sage/ui/globals.py
	cp ui/fileViewer.py ${PREFIX}/usr/local/sage/ui/fileViewer.py
	cp ui/sageApp.py ${PREFIX}/usr/local/sage/ui/sageApp.py
	cp ui/setup.py ${PREFIX}/usr/local/sage/ui/setup.py
	cp ui/Graph.py ${PREFIX}/usr/local/sage/ui/Graph.py
	cp ui/canvases.py ${PREFIX}/usr/local/sage/ui/canvases.py
	cp ui/sageDisplayInfo.py ${PREFIX}/usr/local/sage/ui/sageDisplayInfo.py
	cp ui/sageData.py ${PREFIX}/usr/local/sage/ui/sageData.py
	cp ui/pointers.py ${PREFIX}/usr/local/sage/ui/pointers.py
	cp ui/sageui.py ${PREFIX}/usr/local/sage/ui/sageui.py
	cp ui/preferences.py ${PREFIX}/usr/local/sage/ui/preferences.py
	cp ui/Mywx.py ${PREFIX}/usr/local/sage/ui/Mywx.py
	cp ui/users.py ${PREFIX}/usr/local/sage/ui/users.py
	cp ui/README ${PREFIX}/usr/local/sage/ui/README
	cp ui/images/play_over.jpg ${PREFIX}/usr/local/sage/ui/images/play_over.jpg
	cp ui/images/vncviewer_down.jpg ${PREFIX}/usr/local/sage/ui/images/vncviewer_down.jpg
	cp ui/images/juxtaview_down.jpg ${PREFIX}/usr/local/sage/ui/images/juxtaview_down.jpg
	cp ui/images/maximize_shape.gif ${PREFIX}/usr/local/sage/ui/images/maximize_shape.gif
	cp ui/images/scrollRight_down.jpg ${PREFIX}/usr/local/sage/ui/images/scrollRight_down.jpg
	cp ui/images/SAGEapp_18.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_18.jpg
	cp ui/images/stats_BW_over.jpg ${PREFIX}/usr/local/sage/ui/images/stats_BW_over.jpg
	cp ui/images/app_panel_background.png ${PREFIX}/usr/local/sage/ui/images/app_panel_background.png
	cp ui/images/SAGEapp_06.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_06.jpg
	cp ui/images/juxtaview_over.jpg ${PREFIX}/usr/local/sage/ui/images/juxtaview_over.jpg
	cp ui/images/menu_bar.jpg ${PREFIX}/usr/local/sage/ui/images/menu_bar.jpg
	cp ui/images/mplayer_up.jpg ${PREFIX}/usr/local/sage/ui/images/mplayer_up.jpg
	cp ui/images/default_down.jpg ${PREFIX}/usr/local/sage/ui/images/default_down.jpg
	cp ui/images/mplayer_down.jpg ${PREFIX}/usr/local/sage/ui/images/mplayer_down.jpg
	cp ui/images/rotate_shape.gif ${PREFIX}/usr/local/sage/ui/images/rotate_shape.gif
	cp ui/images/stats_green.jpg ${PREFIX}/usr/local/sage/ui/images/stats_green.jpg
	cp ui/images/bitplay_down.jpg ${PREFIX}/usr/local/sage/ui/images/bitplay_down.jpg
	cp ui/images/no_preview.png ${PREFIX}/usr/local/sage/ui/images/no_preview.png
	cp ui/images/reset_down.jpg ${PREFIX}/usr/local/sage/ui/images/reset_down.jpg
	cp ui/images/hdmovie_up.jpg ${PREFIX}/usr/local/sage/ui/images/hdmovie_up.jpg
	cp ui/images/SAGEapp_19.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_19.jpg
	cp ui/images/inst_up.jpg ${PREFIX}/usr/local/sage/ui/images/inst_up.jpg
	cp ui/images/SAGEapp_05.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_05.jpg
	cp ui/images/play_up.jpg ${PREFIX}/usr/local/sage/ui/images/play_up.jpg
	cp ui/images/slower_down.jpg ${PREFIX}/usr/local/sage/ui/images/slower_down.jpg
	cp ui/images/remote_over.jpg ${PREFIX}/usr/local/sage/ui/images/remote_over.jpg
	cp ui/images/teravision_up.jpg ${PREFIX}/usr/local/sage/ui/images/teravision_up.jpg
	cp ui/images/SAGEapp_03.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_03.jpg
	cp ui/images/teravision_down.jpg ${PREFIX}/usr/local/sage/ui/images/teravision_down.jpg
	cp ui/images/gadgets_up.jpg ${PREFIX}/usr/local/sage/ui/images/gadgets_up.jpg
	cp ui/images/SAGEapp_14.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_14.jpg
	cp ui/images/SAGEapp_01.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_01.jpg
	cp ui/images/vra_up.jpg ${PREFIX}/usr/local/sage/ui/images/vra_up.jpg
	cp ui/images/stats_Streams_over.jpg ${PREFIX}/usr/local/sage/ui/images/stats_Streams_over.jpg
	cp ui/images/render_up.jpg ${PREFIX}/usr/local/sage/ui/images/render_up.jpg
	cp ui/images/faster_down.jpg ${PREFIX}/usr/local/sage/ui/images/faster_down.jpg
	cp ui/images/stats_FPS_up.jpg ${PREFIX}/usr/local/sage/ui/images/stats_FPS_up.jpg
	cp ui/images/gadgets_down.jpg ${PREFIX}/usr/local/sage/ui/images/gadgets_down.jpg
	cp ui/images/faster_over.jpg ${PREFIX}/usr/local/sage/ui/images/faster_over.jpg
	cp ui/images/atlantis_up.jpg ${PREFIX}/usr/local/sage/ui/images/atlantis_up.jpg
	cp ui/images/hdmovie_over.jpg ${PREFIX}/usr/local/sage/ui/images/hdmovie_over.jpg
	cp ui/images/remote_up.jpg ${PREFIX}/usr/local/sage/ui/images/remote_up.jpg
	cp ui/images/SAGEapp_15.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_15.jpg
	cp ui/images/stats_FPS_over.jpg ${PREFIX}/usr/local/sage/ui/images/stats_FPS_over.jpg
	cp ui/images/magicarpet_over.jpg ${PREFIX}/usr/local/sage/ui/images/magicarpet_over.jpg
	cp ui/images/svc_down.jpg ${PREFIX}/usr/local/sage/ui/images/svc_down.jpg
	cp ui/images/SAGEapp_04.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_04.jpg
	cp ui/images/default_over.jpg ${PREFIX}/usr/local/sage/ui/images/default_over.jpg
	cp ui/images/vncviewer_up.jpg ${PREFIX}/usr/local/sage/ui/images/vncviewer_up.jpg
	cp ui/images/scrollLeft_over.jpg ${PREFIX}/usr/local/sage/ui/images/scrollLeft_over.jpg
	cp ui/images/atlantis_over.jpg ${PREFIX}/usr/local/sage/ui/images/atlantis_over.jpg
	cp ui/images/stats_Streams_down.jpg ${PREFIX}/usr/local/sage/ui/images/stats_Streams_down.jpg
	cp ui/images/svc_over.jpg ${PREFIX}/usr/local/sage/ui/images/svc_over.jpg
	cp ui/images/green_circle.gif ${PREFIX}/usr/local/sage/ui/images/green_circle.gif
	cp ui/images/hdmovie_down.jpg ${PREFIX}/usr/local/sage/ui/images/hdmovie_down.jpg
	cp ui/images/inst_down.jpg ${PREFIX}/usr/local/sage/ui/images/inst_down.jpg
	cp ui/images/back_down.jpg ${PREFIX}/usr/local/sage/ui/images/back_down.jpg
	cp ui/images/default_up.jpg ${PREFIX}/usr/local/sage/ui/images/default_up.jpg
	cp ui/images/back_up.jpg ${PREFIX}/usr/local/sage/ui/images/back_up.jpg
	cp ui/images/SAGEapp_07.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_07.jpg
	cp ui/images/SAGEapp_02.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_02.jpg
	cp ui/images/bitplay_over.jpg ${PREFIX}/usr/local/sage/ui/images/bitplay_over.jpg
	cp ui/images/pause_over.jpg ${PREFIX}/usr/local/sage/ui/images/pause_over.jpg
	cp ui/images/stats_FPS_down.jpg ${PREFIX}/usr/local/sage/ui/images/stats_FPS_down.jpg
	cp ui/images/app_info_background.png ${PREFIX}/usr/local/sage/ui/images/app_info_background.png
	cp ui/images/stats_Nodes_down.jpg ${PREFIX}/usr/local/sage/ui/images/stats_Nodes_down.jpg
	cp ui/images/bitplay_up.jpg ${PREFIX}/usr/local/sage/ui/images/bitplay_up.jpg
	cp ui/images/pause_up.jpg ${PREFIX}/usr/local/sage/ui/images/pause_up.jpg
	cp ui/images/stats_BW_up.jpg ${PREFIX}/usr/local/sage/ui/images/stats_BW_up.jpg
	cp ui/images/scrollLeft_down.jpg ${PREFIX}/usr/local/sage/ui/images/scrollLeft_down.jpg
	cp ui/images/SAGEapp_17.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_17.jpg
	cp ui/images/SAGEapp_10.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_10.jpg
	cp ui/images/help.png ${PREFIX}/usr/local/sage/ui/images/help.png
	cp ui/images/stats_Nodes_over.jpg ${PREFIX}/usr/local/sage/ui/images/stats_Nodes_over.jpg
	cp ui/images/retrieving_preview.png ${PREFIX}/usr/local/sage/ui/images/retrieving_preview.png
	cp ui/images/SAGEapp_08.jpg ${PREFIX}/usr/local/sage/ui/images/SAGEapp_08.jpg
	cp ui/images/red_circle.gif ${PREFIX}/usr/local/sage/ui/images/red_circle.gif
	cp ui/images/stop_over.jpg ${PREFIX}/usr/local/sage/ui/images/stop_over.jpg
	cp ui/images/juxtaview_up.jpg ${PREFIX}/usr/local/sage/ui/images/juxtaview_up.jpg
	cp ui/images/vra_down.jpg ${PREFIX}/usr/local/sage/ui/images/vra_down.jpg
	cp ui/images/scrollRight_over.jpg ${PREFIX}/usr/local/sage/ui/images/scrollRight_over.jpg
	cp ui/images/stats_Nodes_up.jpg ${PREFIX}/usr/local/sage/ui/images/stats_Nodes_up.jpg
	cp ui/images/minimize_shape.gif ${PREFIX}/usr/local/sage/ui/images/minimize_shape.gif
	cp ui/images/faster_up.jpg ${PREFIX}/usr/local/sage/ui/images/faster_up.jpg
	cp ui/images/atlantis_down.jpg ${PREFIX}/usr/local/sage/ui/images/atlantis_down.jpg
	cp ui/images/pause_down.jpg ${PREFIX}/usr/local/sage/ui/images/pause_down.jpg
	cp ui/images/scrollLeft_up.jpg ${PREFIX}/usr/local/sage/ui/images/scrollLeft_up.jpg
	cp ui/images/gadgets_over.jpg ${PREFIX}/usr/local/sage/ui/images/gadgets_over.jpg
	cp ui/images/handle.jpg ${PREFIX}/usr/local/sage/ui/images/handle.jpg
	cp ui/images/teravision_over.jpg ${PREFIX}/usr/local/sage/ui/images/teravision_over.jpg
	cp ui/images/stats_Streams_up.jpg ${PREFIX}/usr/local/sage/ui/images/stats_Streams_up.jpg
	cp ui/images/slower_over.jpg ${PREFIX}/usr/local/sage/ui/images/slower_over.jpg
	cp ui/images/stop_up.jpg ${PREFIX}/usr/local/sage/ui/images/stop_up.jpg
	cp ui/images/magicarpet_up.jpg ${PREFIX}/usr/local/sage/ui/images/magicarpet_up.jpg
	cp ui/images/back_over.jpg ${PREFIX}/usr/local/sage/ui/images/back_over.jpg
	cp ui/images/svc_up.jpg ${PREFIX}/usr/local/sage/ui/images/svc_up.jpg
	cp ui/images/handle1.jpg ${PREFIX}/usr/local/sage/ui/images/handle1.jpg
	cp ui/images/remote_down.jpg ${PREFIX}/usr/local/sage/ui/images/remote_down.jpg
	cp ui/images/mplayer_over.jpg ${PREFIX}/usr/local/sage/ui/images/mplayer_over.jpg
	cp ui/images/stop_down.jpg ${PREFIX}/usr/local/sage/ui/images/stop_down.jpg
	cp ui/images/play_down.jpg ${PREFIX}/usr/local/sage/ui/images/play_down.jpg
	cp ui/images/magicarpet_down.jpg ${PREFIX}/usr/local/sage/ui/images/magicarpet_down.jpg
	cp ui/images/stats_BW_down.jpg ${PREFIX}/usr/local/sage/ui/images/stats_BW_down.jpg
	cp ui/images/slower_up.jpg ${PREFIX}/usr/local/sage/ui/images/slower_up.jpg
	cp ui/images/close_shape.gif ${PREFIX}/usr/local/sage/ui/images/close_shape.gif
	cp ui/images/scrollRight_up.jpg ${PREFIX}/usr/local/sage/ui/images/scrollRight_up.jpg
	cp ui/images/render_down.jpg ${PREFIX}/usr/local/sage/ui/images/render_down.jpg
	cp ui/sageAppPerfInfo.py ${PREFIX}/usr/local/sage/ui/sageAppPerfInfo.py
	cp ui/SAGEDrawObject.py ${PREFIX}/usr/local/sage/ui/SAGEDrawObject.py
	cp ui/launcherAdmin.py ${PREFIX}/usr/local/sage/ui/launcherAdmin.py
	cp ui/sageGate.py ${PREFIX}/usr/local/sage/ui/sageGate.py

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
