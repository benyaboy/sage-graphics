SHELL=/bin/bash
preprojects = QUANTA/src
subprojects = app/render app/atlantis app/checker app/FileViewer app/bitplay app/vnc


default:
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) install; cd - ; done
	@cd src; $(MAKE) install; cd -
	@for i in `echo $(subprojects)`; do cd $$i; $(MAKE); cd - ; done

install: default
	@for i in `echo $(preprojects)`; do cd $$i; $(MAKE) $@; cd - ; done
	@for i in `echo $(subprojects)`; do cd $$i; $(MAKE) $@; cd - ; done

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
