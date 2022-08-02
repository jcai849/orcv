N <- seq(10)
sender <- function(n) {
	library(orcv)
	
	msg <- paste("from", n)

	start()
	fd2 <- send("localhost", 12345L, "hello", msg, keep_conn=T)
	send(fd2, "goodbye", msg)
}
for (n in N) parallel::mcparallel(sender(n), detached=T)

library(orcv)
start("localhost", 12345L, threads=4L)

msgs <- lapply(N, function(...) receive(keep_conn=T))
fds <- do.call(c, lapply(msgs, fd))

receive(fds)
