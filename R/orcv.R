header <- function(x, ...) UseMethod("header", x)
payload <- function(x, ...) UseMethod("payload", x)
fd <- function(x, ...) UseMethod("fd", x)
`fd<-` <- function(x, value) UseMethod("fd<-", x)
send <- function(x, ...) UseMethod("send", x)
receive <- function(x, keep_conn=FALSE, ...)  {
	if (missing(x)) {
		msg <- as.Message(.Call(C_next_message)) 
		if (is.null(msg)) stop("receive error")
		cat(sprintf("Opening message with header \"%s\"\n", header(msg)))
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

as.FD <- function(x, ...) {
	stopifnot(is.integer(x))
	class(x) <- "FD"
	x
}
send.FD <- function(x, header, payload=NULL, keep_conn=FALSE, ...) {
	stopifnot(is.character(header) && length(header) == 1)
	force(keep_conn)
	serialised_payload <- serialize(payload, NULL)
	header_length <- nchar(header, type="bytes") + 1L
	fd <- as.FD(sapply(x, function(fd) .Call(C_send_socket, fd, header_length, header, serialised_payload)))
	if (any(fd == -1)) stop("send error")
	if (!keep_conn) close(x)
	invisible(fd)
}
receive.FD <- function(x, keep_conn=FALSE, ...) {
	if (length(x) > 1) {
		message("Multiple FD's specified to receive from. Using first supplied FD")
		x <- x[1]
	}
	msg <- as.Message(.Call(C_receive_socket, x))
	if (is.null(msg)) stop("receive error")
	cat(sprintf("Opening message with header \"%s\"\n", header(msg)))
	if (!keep_conn) {
		close(msg)
		fd(msg) <- as.FD(-1L)
	}
	invisible(msg)
}
`[.FD` <- function(x, i) as.FD(unclass(x)[i])
close.FD <- function(con, ...) {force(con); sapply(con, function(x) .Call(C_close_socket, x))}

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
send.Message <- function(x, header, payload=NULL, keep_conn=FALSE, ...) send(fd(x), header, payload, keep_conn, ...)

send.character <- function(x, port, header, payload=NULL, keep_conn=FALSE, ...) {
	stopifnot(is.integer(port))
	loc <- as.Location(x, port)
	send(loc, header, payload, keep_conn, ...)
}
