library(parallel)

invisible(mcparallel({
    suppressPackageStartupMessages(library(orcv))
    PORT <- 12346L

    start(PORT)

    event <- next_event()
    respond(event, 1:20)
    await_response(event)
    last_event <- next_event()
}))

suppressPackageStartupMessages(library(orcv))
PORT <- 12345L

start(PORT)
Sys.sleep(1)

fd <- send("localhost", 12346L, 1:10)
await_response(fd)
event <- next_event()
respond(event, 1:30)
