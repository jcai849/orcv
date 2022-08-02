library(orcv)

start()
fd2 <- send("localhost", 12345L, "hello", "from 2", keep_conn=T)
send(fd2, "goodbye", "from 2")
