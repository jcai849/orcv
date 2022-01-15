.PHONY: tests clean
tests: src/liborcv.a
	cd tests && $(MAKE)
src/liborcv.a: src/*.c src/*.h
	cd src && $(MAKE)
clean:
	cd src && $(MAKE) clean
	cd tests && $(MAKE) clean
