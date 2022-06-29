library(orcv)
start("localhost", 9797L)
location()
msg <- receive()
send(fd(msg), "hello to you too", 10:1)

