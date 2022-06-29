#include <R.h>
#include <Rinternals.h>
#include "start.h"

SEXP C_start(SEXP, SEXP, SEXP);
SEXP C_next_message(void);
SEXP C_receive_socket(SEXP fd);
SEXP msg_to_sexp(Message *);
SEXP C_send_socket(SEXP, SEXP, SEXP, SEXP);
SEXP C_get_socket(SEXP, SEXP);
SEXP C_close_socket(SEXP);
SEXP C_location(void);
