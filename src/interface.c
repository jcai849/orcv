#include <unistd.h>

#include "interface.h"
#include "start.h"

SEXP C_start(SEXP address, SEXP port, SEXP threads)
{
	int c_port, c_threads;
	SEXP error;

	c_port = INTEGER(port)[0];
	c_threads = INTEGER(threads)[0];
	error = PROTECT(allocVector(INTSXP, 1));
	INTEGER(error)[0] = start(isNull(address) ? NULL : CHAR(STRING_ELT(address, 0)), c_port, c_threads);

	UNPROTECT(1);
	return error;
}

SEXP C_next_message(void)
{
	Message *event;
	SEXP msg, fd, loc, header, payload;

	event = next_event();

	msg = PROTECT(allocVector(VECSXP, 5));
	fd = PROTECT(allocVector(INTSXP, 1));
	loc = PROTECT(allocVector(INTSXP, 2));
	header = PROTECT(allocVector(STRSXP, 1));
	payload = PROTECT(allocVector(RAWSXP, event->payload_size));

	INTEGER(fd)[0] = event->fd;
	INTEGER(loc)[0] = event->addr;
	INTEGER(loc)[1] = event->port;
	SET_STRING_ELT(header, 0, mkChar(event->header));
	memcpy(RAW(payload), event->payload, event->payload_size);

	SET_VECTOR_ELT(msg, 0, fd);
	SET_VECTOR_ELT(msg, 1, loc);
	SET_VECTOR_ELT(msg, 2, header);
	SET_VECTOR_ELT(msg, 3, payload);

	delete_message(event);
	UNPROTECT(5);
	return msg;
}

SEXP C_send_socket(SEXP fd, SEXP header, SEXP payload)
{
	int c_fd;
	int header_size, payload_size;
	char *c_payload;
	const char *c_header;
	SEXP error;
	
	c_fd = INTEGER(fd)[0];
	header_size = LENGTH(header);
	c_header = CHAR(STRING_ELT(header, 0));
	payload_size = LENGTH(payload);
	c_payload = RAW(payload);

	error = PROTECT(allocVector(INTSXP, 1));
	INTEGER(error)[0] = send_socket(c_fd, header_size, c_header, payload_size, c_payload);

	UNPROTECT(1);
	return error;
}

SEXP C_get_socket(SEXP addr, SEXP port)
{
	SEXP fd;
	
	fd = PROTECT(allocVector(INTSXP, 1));
	INTEGER(fd)[0] = get_socket(INTEGER(addr)[0], INTEGER(port)[0]);

	UNPROTECT(1);
	return fd;
}

SEXP C_close_socket(SEXP fd)
{
	SEXP error;

	error = PROTECT(allocVector(INTSXP, 1));
	INTEGER(error)[0] = close(INTEGER(fd)[0]);

	UNPROTECT(1);
	return error;
}

SEXP C_location(void)
{
	SEXP loc;

	loc = PROTECT(allocVector(INTSXP, 2));
	INTEGER(loc)[0] = get_address();
	INTEGER(loc)[1] = get_port();

	UNPROTECT(1);
	return loc;
}
