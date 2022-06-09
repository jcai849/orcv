CFLAGS= -g -Wall -Wextra -pipe -O0
LDFLAGS= -lpthread ../src/liborcv.a

.SUFFIXES: .out .test .c .R
.PHONY: test clean
.c.out:
	${CC} ${CFLAGS} -o ${.TARGET} ${.IMPSRC} ${LDFLAGS}
.out.test:
	./${.IMPSRC}
.R.test:
	Rscript ${.IMPSRC}

test: queue.test tsqueue.test comms.test interface.test

clean:
	rm -f *.out
	rm -f *.core
