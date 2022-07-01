library(orcv)
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
l2 <- loc(receive())
stopifnot(identical(l1, l2))

## X 1

# 1) send to location after other receive, keep_conn a
Sys.sleep(1)
fd_a <- send(l1, h1, p1, keep_conn=T)
# 2) send to location after other receive
Sys.sleep(1)
send(l1, h1, p1)
# 3) send to location before receive, keep_conn b
fd_b <- send(l1, h1, p1)
# 4) send to location before receive
send(l1, h1, p1)
# 5) send to fd a after other receive, keep_conn a
# 6) send to fd a after other receive
# 7) send to fd b before other receive, keep_conn b
# 8) send to fd b before other receive, keep_conn b
# 9) send to fd b when closed by other
#10) send to fd a when closed by self

## X 100
