library(orcv)

start()
fd1 <- send("localhost", 12345L, "hello", "from 1", keep_conn=T)
send(fd1, "goodbye", "from 1")
