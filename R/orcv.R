CONTROL <- new.env(parent=emptyenv(), size=1L)
start <- function(port, threads=getOption("orcv.cores", 4L)) {
    stopifnot(is.integer(port), is.integer(threads))
    control <- .Call(C_start, port, threads)
    if (is.null(control)) stop()
    invisible(assign("control", control, CONTROL))
}
event_push <- function(value, address, port) {
    stopifnot(is.character(address), is.integer(port))
    serialised <- serialize(value, NULL)
    fd <- .Call(C_send, address, port, serialised)
    if (fd == -1) stop()
    invisible(fd)
}
event_pop <- function() {
    control <- get("control", CONTROL)
    event <- .Call(C_multiplex, control)
    if (is.null(event)) stop()
    names(event) <- c("data", "fd")
    event$data <- unserialize(event$data)
    invisible(event)
}
is.event <- function(x) inherits(x, "Event")
respond <- function(x, value) UseMethod("respond")
respond.Event <- function(x, value) respond(x$fd, value)
respond.integer <- function(x, value) {
    serialised <- serialize(value, NULL)
    status <- .Call(C_respond, x, serialised)
    if (status == -1) stop()
    invisible(status)
}
monitor_response <- function(x, ...) UseMethod("monitor_response")
monitor_response.Event <- function(x, ...)  monitor_response(x$fd)
monitor_response.integer <- function(x, ...) {
    control <- get("control", CONTROL)
    status <- .Call(C_monitor_response, control, x)
    if (status != 0) stop()
    invisible(status)
}
await_response <- function(x, ...) UseMethod("await_response")
await_response.Event <- function(x, ...)  await_response(x$fd)
await_response.integer <- function(x, ...) {
    event <- .Call(C_await_response, x)
    if (is.null(event)) stop()
    names(event) <- c("data", "fd")
    event$data <- unserialize(event$data)
    invisible(event)
}
event_complete <- function(x, ...) UseMethod("event_complete")
event_complete.integer <- function(x, ...) {
    status <- .Call(C_close_connection, x)
    if (status != 0) stop()
    invisible(status)
}
event_complete.Event <- function(x, ...) {
    status <- 0
    if (!is.null(x$fd)) {
        status <- event_complete(x$fd)
    }
    invisible(status)
}
# stop <- function() {
#     control <- get("control", CONTROL)
#     .Call(C_stop, control)
# }
