header <- function(x, ...) UseMethod("header", x)
payload <- function(x, ...) UseMethod("payload", x)
fd <- function(x, ...) UseMethod("fd", x)
send <- function(x, ...) UseMethod("send", x)
location <- function(x, ...) {
	if (missing(x)) {
		as.Location(.Call(C_location)) 
	} else UseMethod("location", x)
}
address <- function(x, ...) UseMethod("address", x)
port <- function(x, ...) UseMethod("port", x)

start <- function(port, threads=getOption("orcv.cores", 4L)) {
	stopifnot(is.integer(port), is.integer(threads))
	invisible(.Call(C_start, port, threads))
}

as.Location <- function(x) {
	stopifnot(is.integer(x), length(x) == 2L)
	names(x) <- c("address", "port")
	class(x) <- "Location"
	x
}
send.Location <- function(x, header, payload) {
	fd <- as.FD(.Call(C_get_socket, address(x), port(x)))
	error <- send(fd, header, payload)
	if (error) error else fd
}
address.Location <- function(x, ...) x$address
port.Location <- function(x, ...) x$port

as.FD <- function(x) {
	stopifnot(is.integer(x))
	class(x) <- "FD"
	x
}
send.FD <- function(x, header, payload) {
	stopifnot(is.character(header))
	serialised_payload <- serialize(payload, NULL)
	invisible(.Call(C_send_socket, x, header, serialised_payload))
}
close.FD <- function(con, ...) .Call(C_close_socket, con)

next_message <- function() {
	msg <- .Call(C_next_message)
	as.Message(msg)
}
as.Message <- function(x, ...) {
	stopifnot(is.list(x), length(x) == 4)
	names(msg) <- c("fd", "loc", "header", "payload")
	msg$fd <- as.FD(msg$fd)
	msg$loc <- as.Location(msg$loc)
	msg$payload <- unserialize(msg$payload)
	class(msg) <- "Message"
	msg
}
header.Message <- function(x, ...) x$header
payload.Message <- function(x, ...) x$payload
location.Message <- function(x, ...) x$loc
fd.Message <- function(x, ...) x$fd
close.Message <- function(con, ...) close(fd(con))
