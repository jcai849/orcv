library(orcv)
PORT <- 12345L

start(PORT)

fd <- send("localhost", 12346L, 1:10)
await_response(fd)
event <- next_event()
event
respond(event, 1:30)
