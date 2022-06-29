#include <R.h>
#include <Rinternals.h>
#include "start.h"

SEXP C_start(SEXP address, SEXP port, SEXP threads);
SEXP C_next_message(void);
SEXP C_receive_socket(SEXP fd);
SEXP msg_to_sexp(Message *event);
SEXP C_send_socket(SEXP fd, SEXP header_length, SEXP header, SEXP payload);
SEXP C_get_socket(SEXP addr, SEXP port);
SEXP C_close_socket(SEXP fd);
SEXP C_location(void);
