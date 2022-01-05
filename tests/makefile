CFLAGS= -g -Wall -Wextra -pipe -O0
queue.out: queue.c ../src/queue.c
	${CC} ${CFLAGS} -o ${.TARGET} ${.ALLSRC}

test-queue: queue.out
	./queue.out &&\
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./queue.out
