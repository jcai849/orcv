SRCS= interface.c start.c comms.c tsqueue.c queue.c
OBJS:=${SRCS:S/.c/.o/g}
ARFLAGS=rcs
CFLAGS=-g -Wall -Wextra -pipe -O0
liborcv.a: ${OBJS}
	${AR} ${ARFLAGS} ${.TARGET} ${.ALLSRC}
clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
