.PHONY: tags install clean tests
all: install tests
install: clean R/orcv.R src/*.c src/*.h
	R CMD INSTALL .
tests: clean src/liborcv.a
	cd tests && $(MAKE)
src/liborcv.a: src/*.c src/*.h
	cd src && $(MAKE)
clean:
	cd src && $(MAKE) clean
	cd tests && $(MAKE) clean
tags:
	uctags -R
flow: # e.g. make flow SYMBOL=C_multiplex
	cflow -Tm $(SYMBOL) src/*.c | bash exec/tree2dotx | dot -Tsvg >$(SYMBOL)-flow.svg
