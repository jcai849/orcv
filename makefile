.PHONY: install tests clean
install: R/orcv.R src/*.c src/*.h
	R CMD INSTALL .
tests: src/liborcv.a
	cd tests && $(MAKE)
src/liborcv.a: src/*.c src/*.h
	cd src && $(MAKE)
clean:
	cd src && $(MAKE) clean
	cd tests && $(MAKE) clean
