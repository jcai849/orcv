.PHONY: tags install clean tests
all: install test
install: R/orcv.R src/*.c src/*.h
	R CMD INSTALL .
tags:
	uctags -R
test:
	cd tests && tmux new-session \; source-file test.tmux
	R -f tests/receive_multiple_fd.R
flow: # e.g. make flow SYMBOL=C_multiplex
	cflow -Tm $(SYMBOL) src/*.c | bash exec/tree2dotx | dot -Tsvg >$(SYMBOL)-flow.svg
