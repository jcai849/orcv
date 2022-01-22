CONTROL <- new.env(parent=emptyenv(), size=1L)
start <- function(port, threads=getOption("orcv.cores", 4L)) {
    stopifnot(is.integer(port), is.integer(threads))
    control <- .Call(C_start, port, threads)
    if (is.null(control)) stop()
    invisible(assign("control", control, CONTROL))
}
send <- function(address, port, value) {
    stopifnot(is.character(address), is.integer(port))
    serialised <- serialize(value, NULL)
    fd <- .Call(C_send, address, port, serialised)
    if (fd == -1) stop()
    invisible(structure(fd, class="FD"))
}
next_event <- function() {
    control <- get("control", CONTROL)
    event <- .Call(C_multiplex, control)
    if (is.null(event)) stop()
    names(event) <- c("data", "fd")
    class(event$fd) <- "FD"
    event$data <- unserialize(event$data)
    invisible(event)
}
is.event <- function(x) inherits(x, "Event")
respond <- function(event, value) {
    stopifnot(is.event(event))
    serialised <- serialize(value, NULL)
    status <- .Call(C_respond, event$fd, serialised)
    if (status == -1) stop()
    invisible(status)
}
await_response <- function(x, ...) UseMethod("await_response")
await_response.Event <- function(x, ...)  await_response(x$fd)
await_response.FD <- function(x, ...) {
    control <- get("control", CONTROL)
    status <- .Call(C_await_response, control, x)
    if (status != 0) stop()
    invisible(status)
}
complete_event <- function(event) {
    status <- 0
    if (!is.null(event$fd)) {
        status <- .Call(C_close_connection, event$fd)
        if (status != 0) stop()
    }
    invisible(status)
}
# stop <- function() {
#     control <- get("control", CONTROL)
#     .Call(C_stop, control)
# }
