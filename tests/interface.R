library(parallel)

invisible(mcparallel({
    suppressPackageStartupMessages(library(orcv))
    PORT <- 12346L

    start(PORT)

    event <- event_pop()
    respond(event, 1:20)
    monitor_response(event)
    last_event <- event_pop()
    respond(last_event, 1:40)
    event_complete(last_event)
}))

suppressPackageStartupMessages(library(orcv))
PORT <- 12345L

start(PORT)
#Sys.sleep(1)

fd <- event_push(1:10, "localhost", 12346L)
monitor_response(fd)
event <- event_pop()
respond(event, 1:30)
await_response(event)
event_complete(event)
