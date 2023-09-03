library(largescalemessages)
INADDR_LOOPBACK <- strtoi("7f000001", 16)
start("localhost", 9797L)
location()
l1 <- as.Location(c(INADDR_LOOPBACK, 9898L))

## INIT
p1 <- 1:100
h1 <- "hello"
# 1) send to character address
send("localhost", 9898L, header=h1, payload=p1)
# 2) receive loc from queue
l2 <- location(receive())
stopifnot(identical(l1, l2))

## X 1

# 1) send to location after other receive, keep_conn a
Sys.sleep(1)
fd_a <- send(l1, h1, p1, keep_conn=T)
# 2) send to location after other receive
Sys.sleep(1)
send(l1, h1, p1)
# 3) send to location before receive, keep_conn b
fd_b <- send(l1, h1, p1, keep_conn=T)
# 4) send to location before receive
send(l1, h1, p1)
# 5) send to fd a after other receive, keep_conn a
Sys.sleep(1)
fd_a2 <- send(fd_a, h1, p1, keep_conn=T)
stopifnot(identical(fd_a, fd_a2))
# 6) send to fd a after other receive
Sys.sleep(1)
send(fd_a, h1, p1)
# 7) send to fd b before other receive, keep_conn b
send(fd_b, h1, p1, keep_conn=T)
# 8) (other side closes fd b)
# 9) send to fd b after closed by other
Sys.sleep(1)
send(fd_b, h1, p1)
# 10) send to fd a after closed by self
tryCatch(send(fd_a, h1, p1), error=identity)

## X 100
# 1) send a bunch to location before other receive
for (i in 1:100) send(l1, h1, p1)
# 2) send one to keep_conn c
fd_c <- send(l1, h1, p1, keep_conn=T)
# 3) send a bunch to fd c before other receive
for (i in 1:100) send(fd_c, h1, p1, keep_conn=T)

Sys.sleep(2)
q("no")
