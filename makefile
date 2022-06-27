.PHONY: tags install clean tests
all: install
install: R/orcv.R src/*.c src/*.h
	R CMD INSTALL .
tags:
	uctags -R
flow: # e.g. make flow SYMBOL=C_multiplex
	cflow -Tm $(SYMBOL) src/*.c | bash exec/tree2dotx | dot -Tsvg >$(SYMBOL)-flow.svg
