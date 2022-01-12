CFLAGS= -g -Wall -Wextra -pipe -O0
LDFLAGS= -lpthread ../src/liborcv.a

.SUFFIXES: .out .test .c
.PHONY: test
.c.out:
	${CC} ${CFLAGS} -o ${.TARGET} ${.IMPSRC} ${LDFLAGS}
.out.test:
	./${.IMPSRC}

test: queue.test thread-safe-queue.test event-queue.test inputs.test comms.test
