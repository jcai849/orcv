address <- function(x, ...) UseMethod("address", x)
port <- function(x, ...) UseMethod("port", x)
location <- function(x, ...) if (missing(x)) as.Location(.Call(C_location)) else UseMethod("location", x)
as.Location <- function(x, ...) UseMethod("as.Location", x)

location.integer <- function(x, ...) {
	if (x < 0) stop("invalid 'length' argument")
	if (x == 0) return(structure(list(), class="Location"))
	lapply(0:x, function(i) as.Location(integer(1), integer(1)))
}

as.Location.character <- function(x, port, ...) {
        stopifnot(is.integer(port))
        as.Location(.Call(C_loc_from_string, x, port))
}
as.Location.integer <- function(x, port, ...) {
        if (!missing(port)) x <- c(x, port)
        names(x) <- c("address", "port")
	loc <- list(x)
        class(loc) <- "Location"
       	loc 
}

address.Location <- function(x, ...) sapply(x, "[[", 1)
port.Location <- function(x, ...) sapply(x, "[[", 2)

format.Location
print.Location

as.data.frame.Location
c.Location

as.list.Location
`[.Location`

Ops.Location

send.Location <- function(x, header, payload=NULL, keep_conn=FALSE, ...) {
        fd <- as.FD(mapply(function(a, p) .Call(C_get_socket, a, p),  address(x), port(x)))
        send(fd, header, payload, keep_conn, ...)
        invisible(fd)
}
