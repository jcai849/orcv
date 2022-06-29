#include <R.h>
#include <Rinternals.h>

SEXP C_start(SEXP, SEXP, SEXP);
SEXP C_next_message(void);
SEXP C_send_socket(SEXP, SEXP, SEXP);
SEXP C_get_socket(SEXP, SEXP);
SEXP C_close_socket(SEXP);
SEXP C_location(void);
