library(largescalemessages)
INADDR_LOOPBACK <- strtoi("7f000001", 16)
start("localhost", 9898L)
location()
l1 <- as.Location(c(INADDR_LOOPBACK, 9797L))

## INIT

p1 <- 1:100
h1 <- "hello"
# 1) receive loc from queue
msg1 <- receive()
#  i) header, payload, location, fd of message
h2 <- header(msg1)
stopifnot(identical(h1, h2))
p2 <- payload(msg1)
stopifnot(identical(p1, p2))
l2 <- location(msg1)
stopifnot(identical(l1, l2))
fd1 <- fd(msg1)
# 2) send self_loc to loc
send(l1, h1, p1)

## X 1

# 1) receive from queue before other send, keep_conn a
fd_a <- fd(receive(keep_conn=T))
# 2) receive from queue before other send
receive()
# 3) receive from queue after other send, keep_conn b
Sys.sleep(1)
fd_b <- fd(receive(keep_conn=T))
# 4) receive from queue after other send
Sys.sleep(1)
receive()
# 5) receive from fd a before other receive, keep_conn a
fd_a2 <- fd(receive(fd_a, keep_conn=T))
stopifnot(identical(fd_a, fd_a2))
# 6) receive from fd a before other receive, keep_conn a
receive(fd_a, keep_conn=T)
# 7) receive from fd b after other send, keep_conn b
Sys.sleep(1)
receive(fd_b, keep_conn=T)
# 8) close fd b
close(fd_b)
# 9) receive from fd a after closed by other
tryCatch(receive(fd_a), error=identity)
#10) receive from fd b after closed by self
tryCatch(receive(fd_b), error=identity)

## X 100
# 1) receive a bunch after other send
for (i in 1:100) receive()
# 2) receive one to keep_conn c
fd_c <- fd(receive(keep_conn=T))
print(fd_c)
# 3) receive a bunch from fd c after other send
for (i in 1:100) receive(fd_c, keep_conn=T)
