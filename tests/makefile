CFLAGS= -g -Wall -Wextra -pipe -O0

.SUFFIXES: .out .test .c
.PHONY: test
.c.out:
	${CC} ${CFLAGS} -o ${.TARGET} ${.ALLSRC}
.out.test:
	./${.IMPSRC} && \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./${.IMPSRC}


test: queue.test
queue.out: queue.c ../src/queue.c
