SRCS!=find . -name '*.c'
OBJS:=${SRCS:S/.c/.o/g}
ARFLAGS=rcs
CFLAGS=-g -Wall -Wextra -pipe -O0
liborcv.a: ${OBJS}
	${AR} ${ARFLAGS} ${.TARGET} ${.ALLSRC}
clean:
	rm -f *.o
	rm -f *.a
