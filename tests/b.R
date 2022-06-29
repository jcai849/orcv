library(orcv)
start("localhost", 9898L)
location()

INADDR_LOOPBACK <- strtoi("7f000001", 16)
other <- as.Location(c(INADDR_LOOPBACK, 9797L))
fd <- send(other, "hello", 1:10)
receive(fd)

