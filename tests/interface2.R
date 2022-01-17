library(orcv)
PORT <- 12346L

start(PORT)
event <- next_event()
event
respond(event, 1:20)
await_response(event[[2]])
last_event <- next_event()
last_event
