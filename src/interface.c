#include <Rinternals.h>
#include <R.h>
#include "start.h"

/*
SEXP C_start(SEXP port, SEXP threads)
{
    int c_port, c_threads;
    Inputs *control;

    c_port = INTEGER(port)[0];
    c_threads = INTEGER(threads)[0];
    control = start(c_port, NULL, c_threads);
    if (control == NULL) {
        return R_NilValue;
    }
    return R_MakeExternalPtr(control, R_NilValue, R_NilValue);
}

SEXP C_send(SEXP address, SEXP port, SEXP serialised)
{
    Data data;
    const char *c_address;
    int c_port;
    SEXP fd;

    data.data = RAW(serialised);
    data.size = LENGTH(serialised);
    c_address = CHAR(STRING_ELT(address, 0));
    c_port = INTEGER(port)[0];
    fd = PROTECT(allocVector(INTSXP, 1));
    INTEGER(fd)[0] = send_data(c_address, c_port, &data);
    UNPROTECT(1);
    return fd;
}

SEXP R_event_from_msg(Message *message)
{
    SEXP data, event, fd, class;

    data = PROTECT(allocVector(RAWSXP, message->data->size));
    memcpy(RAW(data), message->data->data, message->data->size);
    free(message->data->data);
    free(message->data);

    fd = PROTECT(allocVector(INTSXP, 1));
    INTEGER(fd)[0] = message->connection;
    free(message);

    event = PROTECT(allocVector(VECSXP, 2));
    SET_VECTOR_ELT(event, 0, data);
    SET_VECTOR_ELT(event, 1, fd);

    class = PROTECT(allocVector(STRSXP, 1));
    SET_STRING_ELT(class, 0, mkChar("Event"));
    classgets(event, class);

    UNPROTECT(4);
    return event;
}

SEXP C_multiplex(SEXP control)
{
    Inputs *inputs;
    struct Message *message;

    inputs = R_ExternalPtrAddr(control);
    if ((message = multiplex(inputs)) == NULL) {
        return R_NilValue;
    }
    return R_event_from_msg(message);
}

SEXP C_respond(SEXP fd, SEXP serialised)
{
    Data data;
    Message message;
    SEXP status;

    data.data = RAW(serialised);
    data.size = LENGTH(serialised);
    message.data = &data;
    message.connection = INTEGER(fd)[0];
    status = PROTECT(allocVector(INTSXP, 1));
    INTEGER(status)[0] = send_message(&message);
    UNPROTECT(1);
    return status;
}

SEXP C_monitor_response(SEXP control, SEXP fd)
{
    Inputs *inputs;
    int connection;
    SEXP status;

    inputs = R_ExternalPtrAddr(control);
    connection = INTEGER(fd)[0];
    status = PROTECT(allocVector(INTSXP, 1));
    INTEGER(status)[0] = inputs_insert_fd(inputs, connection);
    UNPROTECT(1);
    return status;
}

SEXP C_await_response(SEXP fd)
{
    Message *msg;
    SEXP status;

    if ((msg = receive(INTEGER(fd)[0])) == NULL) {
        return R_NilValue;
    }
    return R_event_from_msg(msg);
}

SEXP C_close_connection(SEXP fd)
{
    SEXP status;

    status = PROTECT(allocVector(INTSXP, 1));
    if ((INTEGER(status)[0] = close(INTEGER(fd)[0])) != 0) {
        perror(NULL);
    }
    UNPROTECT(1);
    return status;
}
*/
