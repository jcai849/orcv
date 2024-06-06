ORCV_GLOBAL <- new.env(parent=emptyenv())
ORCV_GLOBAL$STARTED <- FALSE

header <- function(x, ...) UseMethod("header", x)
payload <- function(x, ...) UseMethod("payload", x)
send <- function(x, ...) {
	stopifnot(ORCV_GLOBAL$STARTED)
	stopifnot(length(x) > 0)
	UseMethod("send", x)
}
receive <- function(x, keep_conn=FALSE, simplify=TRUE, ...)  {
	stopifnot(ORCV_GLOBAL$STARTED)
	if (missing(x)) {
		next_msg <- .Call(C_next_message)
		if (is.null(next_msg)) stop("receive error")
		msg <- as.Message(next_msg) 
		if (!keep_conn) {
			close(msg)
			fd(msg) <- as.FD(-1L)
		}
		if (!simplify) msg <- list(msg)
		invisible(msg)
	} else UseMethod("receive", x)
}

start <- function(address=NULL, port=0L, threads=getOption("orcv.cores", 4L)) {
	stopifnot(is.character(address) || is.null(address))
	res <- .Call(C_start, address, as.integer(port), as.integer(threads))
	ORCV_GLOBAL$STARTED <- TRUE
	invisible(res)
}

fd <- function(x, ...) UseMethod("fd", x)
`fd<-` <- function(x, value) UseMethod("fd<-", x)
c.FD <- function(...) as.FD(do.call(c, lapply(list(...), unclass)))
is.FD <- function(x, ...) inherits(x, "FD")
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
receive.FD <- function(x, keep_conn=FALSE, simplify=TRUE,...) {
	if (anyDuplicated(x)) stop("duplicate fds given as input")
	next_msgs <- .Call(C_receive_socket, x)
	if (any(sapply(next_msgs, is.null))) stop("receive error")
	msgs <- lapply(next_msgs, as.Message)
	if (!keep_conn) {
		for (msg in msgs) {
			close(msg)
			fd(msg) <- as.FD(-1L)
		}
	}
	if (simplify && length(msg) == 1L) {
		unlist(msgs, recursive=FALSE, use.names=FALSE)
	} else invisible(msgs)
}
close.FD <- function(con, ...) {
	force(con)
	sapply(con, function(x) stopifnot(.Call(C_close_socket, x) == 0))}
unique.FD <- function(x, incomparables=FALSE, fromLast=FALSE, nmax=NA, ...)
		as.FD(unique(unclass(x), incomparables, fromLast, nmax, ...))
split.FD <- function(x, f, drop=FALSE, sep=".", lex.order=FALSE, ...)
		lapply(split(unclass(x), f, drop, sep, lex.order, ...), as.FD)
`[.FD` <- function(x, i) as.FD(unclass(x)[i])
`[[.FD` <- function(x, i) x[i]
rep.FD <- function(x, ...) as.FD(rep(unclass(x), ...))
as.data.frame.FD <- function(x, ..., nm=deparse1(substitute(x))) {
	as.data.frame.vector(x, ..., nm=nm)
}

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
