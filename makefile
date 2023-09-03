.PHONY: tags install clean tests
all: install
install: R/largescalemessages.R src/*.c src/*.h
	R CMD INSTALL .
tags:
	uctags -R
test:
	cd inst/dev-tests && tmux new-session \; source-file test.tmux
	R -f inst/dev-tests/receive_multiple_fd.R
flow: # e.g. make flow SYMBOL=C_multiplex
	cflow -Tm $(SYMBOL) src/*.c | bash exec/tree2dotx | dot -Tsvg >$(SYMBOL)-flow.svg
