#include <unistd.h>
#include <poll.h>

#include "interface.h"

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
	SEXP msg;

	event = next_event();
	if (!event) return R_NilValue;

	msg = msg_to_sexp(event);
	delete_message(event);

	return msg;
}

SEXP C_receive_socket(SEXP fd)
{
	int nfds;
	int i;
	int *c_fds;
	struct ReceiverArgs *receiver_args;
	Message *c_msglist;
	SEXP msglist;

	c_fds = INTEGER(fd);
	nfds = LENGTH(fd);
	c_msglist = foreground_messages(c_fds, nfds);
	if (!c_msglist) return R_NilValue;

	msglist = PROTECT(allocVector(VECSXP, nfds));

	for (i=0; i<nfds; i++) {
		msg = msg_to_sexp(c_msglist[i]);
		delete_message(&c_msglist[i]);
		SET_VECTOR_ELT(msglist, i, msg);
	}

	UNPROTECT(1);
	return msglist;
}

SEXP msg_to_sexp(Message *event)
{
	SEXP msg, fd, loc, header, payload;

	msg = PROTECT(allocVector(VECSXP, 4));
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

	UNPROTECT(5);
	return msg;
}

/* returns -1 if error, fd otherwise */
SEXP C_send_socket(SEXP fd, SEXP header_length, SEXP header, SEXP payload)
{
	int c_fd, c_header_length;
	int payload_size;
	char *c_payload;
	char *c_header;
	int c_error;
	SEXP error;
	
	c_fd = INTEGER(fd)[0];
	c_header_length = INTEGER(header_length)[0];
	c_header = (char *) CHAR(STRING_ELT(header, 0));
	payload_size = LENGTH(payload);
	c_payload = RAW(payload);

	c_error = send_socket(c_fd, c_header_length, c_header, payload_size, c_payload);
	error = PROTECT(allocVector(INTSXP, 1));
	INTEGER(error)[0] = c_error;

	UNPROTECT(1);
	return c_error ? error : fd;
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
	int sockfd;
	sockfd = INTEGER(fd)[0];

	error = PROTECT(allocVector(INTSXP, 1));
	INTEGER(error)[0] = close(sockfd);
	printf("FD %d closed", sockfd);

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

SEXP C_loc_from_string(SEXP addr, SEXP port)
{
	SEXP loc;
	
	loc = PROTECT(allocVector(INTSXP, 2));
	INTEGER(loc)[0] = address_from_string(CHAR(STRING_ELT(addr, 0)), INTEGER(port)[0]);
	INTEGER(loc)[1] = INTEGER(port)[0];
	
	UNPROTECT(1);
	return loc;
}
