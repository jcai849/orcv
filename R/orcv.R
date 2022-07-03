header <- function(x, ...) UseMethod("header", x)
payload <- function(x, ...) UseMethod("payload", x)
fd <- function(x, ...) UseMethod("fd", x)
`fd<-` <- function(x, value) UseMethod("fd<-", x)
send <- function(x, ...) UseMethod("send", x)
address <- function(x, ...) UseMethod("address", x)
port <- function(x, ...) UseMethod("port", x)
location <- function(x, ...) if (missing(x)) as.Location(.Call(C_location)) else UseMethod("location", x)
receive <- function(x, keep_conn=FALSE, ...)  {
	if (missing(x)) {
		msg <- as.Message(.Call(C_next_message)) 
		if (is.null(msg)) stop("receive error")
		if (!keep_conn) {
			close(msg)
			fd(msg) <- as.FD(-1L)
		}
		invisible(msg)
	} else UseMethod("receive", x)
}

start <- function(address=NULL, port=0L, threads=getOption("orcv.cores", 4L)) {
	stopifnot(is.character(address) || is.null(address),
		  is.integer(port),
		  is.integer(threads))
	invisible(.Call(C_start, address, port, threads))
}

as.Location <- function(x, ...) {
	stopifnot(is.integer(x), length(x) == 2L)
	names(x) <- c("address", "port")
	class(x) <- "Location"
	x
}
send.Location <- function(x, header, payload, keep_conn=FALSE, ...) {
	fd <- as.FD(.Call(C_get_socket, address(x), port(x)))
	send(fd, header, payload, keep_conn, ...)
	invisible(fd)
}
address.Location <- function(x, ...) x[[1]]
port.Location <- function(x, ...) x[[2]]

as.FD <- function(x, ...) {
	stopifnot(is.integer(x))
	class(x) <- "FD"
	x
}
send.FD <- function(x, header, payload, keep_conn=FALSE, ...) {
	stopifnot(is.character(header) && length(header) == 1)
	force(keep_conn)
	serialised_payload <- serialize(payload, NULL)
	header_length <- nchar(header, type="bytes") + 1L
	fd <- .Call(C_send_socket, x, header_length, header, serialised_payload)
	if (fd == -1) stop("send error")
	if (!keep_conn) close(x)
	invisible(fd)
}
receive.FD <- function(x, keep_conn=FALSE, ...) {
	msg <- as.Message(.Call(C_receive_socket, x))
	if (is.null(msg)) stop("receive error")
	if (!keep_conn) {
		close(msg)
		fd(msg) <- as.FD(-1L)
	}
	invisible(msg)
}
close.FD <- function(con, ...) {force(con); .Call(C_close_socket, con)}

as.Message <- function(x, ...) {
	stopifnot(is.list(x), length(x) == 4)
	names(x) <- c("fd", "loc", "header", "payload")
	x$fd <- as.FD(x$fd)
	x$loc <- as.Location(x$loc)
	x$payload <- unserialize(x$payload)
	class(x) <- "Message"
	x
}
header.Message <- function(x, ...) x$header
payload.Message <- function(x, ...) x$payload
location.Message <- function(x, ...) x$loc
fd.Message <- function(x, ...) x$fd
`fd<-.Message` <- function(x, value) {x$fd <- value; x}
close.Message <- function(con, ...) close(fd(con))

send.character <- function(x, port, header, payload, keep_conn=FALSE, ...) {
	stopifnot(is.integer(port))
	loc <- as.Location(.Call(C_loc_from_string, x, port))
	send(loc, header, payload, keep_conn, ...)
}
