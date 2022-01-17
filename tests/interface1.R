library(orcv)
PORT <- 12345L

start(PORT)

send("localhost", 12346L, 1:10)
