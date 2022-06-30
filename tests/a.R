library(orcv)
INADDR_LOOPBACK <- strtoi("7f000001", 16)
start("localhost", 9797L)
location()
other <- as.Location(c(INADDR_LOOPBACK, 9898L))

## INIT

# 1) send to character address
# 2) receive loc from queue

## X 1

# 1) send to location after other receive, keep_conn a
# 2) send to location after other receive
# 3) send to location before receive, keep_conn b
# 4) send to location before receive
# 5) send to fd a after other receive, keep_conn a
# 6) send to fd a after other receive
# 7) send to fd b before other receive, keep_conn b
# 8) send to fd b before other receive, keep_conn b
# 9) send to fd b when closed by other
#10) send to fd a when closed by self
