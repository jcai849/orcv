CONTROL <- new.env(parent=emptyenv(), size=1L)
start <- function(port, threads=getOption("orcv.cores", 4L)) {
    stopifnot(is.integer(port), is.integer(threads))
    control <- .Call(C_start, port, threads)
    assign("control", control, CONTROL)
}
send <- function(address, port, value) {
    stopifnot(is.character(address), is.integer(port))
    serialised <- serialize(value, NULL)
    .Call(C_send, address, port, serialised)
}
next_event <- function() {
    control <- get("control", CONTROL)
    event <- .Call(C_multiplex, control)
    event$data <- unserialize(event$data)
    event
}
respond <- function(event, value) {
    stopifnot(is.event(event))
    serialised <- serialize(value, NULL)
    .Call(C_respond, event$fd, serialised)
}
await_response <- function(event) {
    fd <- event$fd
    control <- get("control", CONTROL)
    .Call(C_await_response, control, fd)
}
complete_event <- function(event) {
    if (!is.null(event$fd)) .Call(C_close_connection, event$fd) else TRUE
}
# stop <- function() {
#     control <- get("control", CONTROL)
#     .Call(C_stop, control)
# }
