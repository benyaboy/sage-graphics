SHELL=/bin/bash
SAGE_DIR = ../..
VIEWER_DIR = ./ImageViewer
SERVER_DIR = ./FileServer

# Macs dont have -u option on cp
MACHINE=$(shell uname -s)
ifeq ($(MACHINE), Darwin)
	CP=/bin/cp -r
else
	CP=/bin/cp -ru
endif


default: imageviewer

install: imageviewer
	cd $(VIEWER_DIR); make install; cd -
	/bin/mkdir -p -m 775 $(SAGE_DIR)/bin/file_server
	$(CP) $(SERVER_DIR)/*.py $(SERVER_DIR)/*.conf $(SERVER_DIR)/misc $(SAGE_DIR)/bin/file_server


imageviewer: $(VIEWER_DIR)/imageviewer.cpp $(VIEWER_DIR)/imgToDxt.cpp
	cd $(VIEWER_DIR); make; cd -

clean:
	/bin/rm -f *.o *~; cd $(VIEWER_DIR); make clean; cd - 

