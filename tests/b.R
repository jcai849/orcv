library(orcv)
INADDR_LOOPBACK <- strtoi("7f000001", 16)
start("localhost", 9898L)
location()
other <- as.Location(c(INADDR_LOOPBACK, 9797L))

## INIT

# 1) receive loc from queue
#  i) header, payload, location fd of message
# 2) send self_loc to loc

## X 1

# 1) receive from queue before other send, keep_conn a
# 2) receive from queue before other send
# 3) receive from queue after other send, keep_conn b
# 4) receive from queue after other send
# 5) receive from fd a before other receive, keep_conn a
# 6) receive from fd a before other receive, keep_conn a
# 7) receive from fd b after other receive, keep_conn b
# 8) receive from fd b after other receive
# 9) receive from fd a when closed by other
#10) receive from fd b when closed by self

## X 100
