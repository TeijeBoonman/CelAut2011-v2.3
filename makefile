LIBDIR = $(HOME)/lib
BINDIR = $(HOME)/bin
INCLUDE = /opt/X11/include

all: onelayer.a multilayer.a

clean: 
	$(MAKE) -C examples clean
	rm -f lib/onelayer.a
	rm -f lib/multilayer.a
	rm -f lib/*.o

install: all
	cp lib/celaut install
	sed s:libdir=\.\*:libdir=$(LIBDIR): < install > $(LIBDIR)/celaut      
	rm install
	mv $(LIBDIR)/celaut $(BINDIR)/celaut
	make onelayer.a
	make multilayer.a
	mv lib/onelayer.a $(LIBDIR)/.
	mv lib/multilayer.a $(LIBDIR)/.
	chmod +x $(BINDIR)/celaut
	rm lib/*.o
	@echo " "
	@echo "       <<CELAUT INSTALLED>>"
	@echo "Print manual for further instructions"
	@echo " "

onelayer.a:lib/control_ol.o lib/x11_ol.o lib/misc.o
	rm -f lib/onelayer.a
	ar rv lib/onelayer.a lib/control_ol.o lib/x11_ol.o lib/misc.o 

multilayer.a:lib/control_ml.o lib/x11_ml.o lib/misc.o lib/readpng.o lib/write_dxf.o
	rm -f lib/multilayer.a
	ar rv lib/multilayer.a lib/control_ml.o lib/x11_ml.o lib/misc.o lib/readpng.o lib/write_dxf.o

tar:
	rm -f celaut.tar.Z
	tar cvf celaut.tar makefile manual.ps lib examples
	compress celaut.tar

.c.o:
	cc -Wno-implicit-function-declaration -I$(INCLUDE) -c -g -o $@ $< 
