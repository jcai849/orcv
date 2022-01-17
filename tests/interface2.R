library(orcv)
PORT <- 12346L

start(PORT)
send("localhost", 12345L, 1:10)
