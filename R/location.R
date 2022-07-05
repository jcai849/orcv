address <- function(x, ...) UseMethod("address", x)
port <- function(x, ...) UseMethod("port", x)
location <- function(x, ...) if (missing(x)) as.Location(.Call(C_location)) else UseMethod("location", x)
as.Location <- function(x, ...) UseMethod("as.Location", x)

location.numeric <- function(x, ...) location(as.integer(x))
location.integer <- function(x, ...) {
	if (x < 0) stop("invalid 'length' argument")
	if (x == 0) return(as.Location(list()))
	rep(as.Location(integer(2)), x)
}

as.Location.list <- function(x, ...) {
	structure(x, class="Location")
}
as.Location.character <- function(x, port, ...) {
        stopifnot(is.integer(port))
        as.Location(.Call(C_loc_from_string, x, port))
}
as.Location.integer <- function(x, port, ...) {
        if (!missing(port)) x <- c(x, port)
        names(x) <- c("address", "port")
	as.Location(list(x))
}
is.Location <- function(x, ...) {
	inherits(x, "Location")
}

rep.Location <- function(x, times) {
	as.Location(rep(unclass(x), times))
}
c.Location <- function(...) {
	as.Location(c(sapply(list(...), unclass)))
}
as.data.frame.Location <- function(x, row.names=NULL, optional=FALSE, ...) {
	nm <- deparse1(substitute(x))
	df <- list2DF(list(unclass(x)))
	if (optional)
		names(df) <- nm
	if (!is.null(row.names))
		attr(df, "row.names") <- row.names
        df
}

as.list.Location <- function(x, ...) {
	lapply(unclass(x), as.Location)
}
`[.Location` <- function(x, i) {
	as.Location(unclass(x)[i])
}
`[<-.Location` <- function(x, i, value) {
	stopifnot(is.Location(value))
	x <- unclass(x)
	x[i] <- value
	as.Location(x)
}

address.Location <- function(x, ...) vapply(unclass(x), "[[", integer(1), 1)
port.Location <- function(x, ...) vapply(unclass(x), "[[", integer(1), 2)

format.Location <- function(x, ...) {
	format(data.frame(address=address(x), port=port(x)))
}
print.Location <- function(x, ...) {
	cat("Location:\n")
	print(format(x))
}

Ops.Location <- function(e1, e2) {
	stopifnot(is.Location(e2))
	do.call(.Generic, address(e1), address(e2)) &
		do.call(.Generic, port(e1), port(e2))
}

send.Location <- function(x, header, payload=NULL, keep_conn=FALSE, ...) {
        fd <- as.FD(mapply(function(a, p) .Call(C_get_socket, a, p),  address(x), port(x)))
        send(fd, header, payload, keep_conn, ...)
        invisible(fd)
}
