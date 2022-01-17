CONTROL <- new.env(parent=emptyenv(), size=1L)
start <- function(port, threads=getOption("orcv.cores", 4L)) {
    stopifnot(is.integer(port), is.integer(threads))
    control <- .Call(C_start, port, threads)
    if (is.null(control)) stop()
    assign("control", control, CONTROL)
}
send <- function(address, port, value) {
    stopifnot(is.character(address), is.integer(port))
    serialised <- serialize(value, NULL)
    status <- .Call(C_send, address, port, serialised)
    if (status != 0) stop()
    status
}
next_event <- function() {
    control <- get("control", CONTROL)
    event <- .Call(C_multiplex, control)
    if (is.null(event)) stop()
    event$data <- unserialize(event$data)
    event
}
respond <- function(event, value) {
    stopifnot(is.event(event))
    serialised <- serialize(value, NULL)
    status <- .Call(C_respond, event$fd, serialised)
    if (status != 0) stop()
    status
}
await_response <- function(event) {
    fd <- event$fd
    control <- get("control", CONTROL)
    status <- .Call(C_await_response, control, fd)
    if (status != 0) stop()
    status
}
complete_event <- function(event) {
    status <- 0
    if (!is.null(event$fd)) {
        status <- .Call(C_close_connection, event$fd)
        if (status != 0) stop()
    }
    status
}
# stop <- function() {
#     control <- get("control", CONTROL)
#     .Call(C_stop, control)
# }
