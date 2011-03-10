SHELL=/bin/bash
SAGE_DIR = ../..
VIEWER_DIR = ./ImageViewer
SERVER_DIR = ./FileServer
PDF_DIR = ./PDFViewer


# Macs dont have -u option on cp
MACHINE=$(shell uname -s)
ifeq ($(MACHINE), Darwin)
	CP=/bin/cp -fr
else
	CP=/bin/cp -fru
endif


default: imageviewer pdfviewer

install: imageviewer
	/bin/mkdir -p -m 775 $(SAGE_DIR)/bin/fileServer
	cd $(VIEWER_DIR); make install; cd -
	cd $(PDF_DIR); make install; cd -
	$(CP) $(SERVER_DIR)/*.py $(SERVER_DIR)/misc $(SAGE_DIR)/bin/fileServer

imageviewer:
	cd $(VIEWER_DIR); make; cd -

pdfviewer:
	cd $(PDF_DIR); make; cd -

clean:
	/bin/rm -f *.o *~
	cd $(VIEWER_DIR); make clean; cd - 
	cd $(PDF_DIR); make clean; cd - 


.PHONY: imageviewer pdfviewer

