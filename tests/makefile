CFLAGS= -g -Wall -Wextra -pipe -O0
LDFLAGS= -lpthread

.SUFFIXES: .out .test .c
.PHONY: test
.c.out:
	${CC} ${CFLAGS} -o ${.TARGET} ${.ALLSRC} ${LDFLAGS}
.out.test:
	./${.IMPSRC}

test: queue.test thread-safe-queue.test event-queue.test inputs.test
queue.out: queue.c ../src/queue.c
thread-safe-queue.out: thread-safe-queue.c ../src/thread-safe-queue.c ../src/queue.c
event-queue.out: event-queue.c ../src/event-queue.c ../src/thread-safe-queue.c ../src/queue.c
inputs.out: inputs.c ../src/inputs.c ../src/event-queue.c ../src/thread-safe-queue.c ../src/queue.c
