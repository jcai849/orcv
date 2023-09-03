N <- seq(10)
sender <- function(n) {
	library(largescalemessages)
	
	msg <- paste("from", n)

	start()
	fd2 <- send("localhost", 12345L, "hello", msg, keep_conn=T)
	send(fd2, "goodbye", msg)
}
for (n in N) parallel::mcparallel(sender(n), detached=T)

library(largescalemessages)
start("localhost", 12345L, threads=4L)

msgs <- lapply(N, function(...) receive(keep_conn=T))
fds <- as.FD(sample(do.call(c, lapply(msgs, fd)), length(N)))

mult <- receive(fds)
stopifnot(all(sapply(mult, fd) == fds))
