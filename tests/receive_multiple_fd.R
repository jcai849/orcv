library(orcv)

start("localhost", 12345L, threads=4L)
fd1 <- receive(keep_conn=T)
fd2 <- receive(keep_conn=T)

fds <- c(fd1, fd2)
receive(fds)
