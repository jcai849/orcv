\name{receive}
\alias{receive}
\alias{receive.FD}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Receive external \pkg{orcv} communications
}
\description{
If \var{x} is missing, blocking pops the oldest Message object from the communication queue as created by \code{\link{start}}.
An open connection with the sender may be maintained if \var{keep_conn} is set to \code{TRUE}.
Alternatively, if an existing connection as given by a \code{FD} object is passed as \var{x}, then the file descriptor is listened on.
S3 method dispatching on \var{x}.
}
\usage{
receive(x, keep_conn = FALSE, simplify = TRUE, ...)
\method{receive}{FD}(x, keep_conn=FALSE, simplify=TRUE,...)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{
	Optional file descriptor or other S3 class.
	Reads from background communication queue if missing.
}
  \item{keep_conn}{
	Logical whether to maintain or close the connection.
}
  \item{simplify}{
	Simplify Message output.
}
  \item{\dots}{
	Further arguments to methods.
}
}
\value{
If from the message queue, a Message, composed of components:

\item{header}{}
\item{payload}{}
\item{location}{}
\item{fd}{}

Arbitrary otherwise.
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{send}}, \code{\link{start}}
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.

## The function is currently defined as
function (x, keep_conn = FALSE, simplify = TRUE, ...) 
{
    stopifnot(ORCV_GLOBAL$STARTED)
    if (missing(x)) {
        next_msg <- .Call(C_next_message)
        if (is.null(next_msg)) 
            stop("receive error")
        msg <- as.Message(next_msg)
        cat(sprintf("Opening message with header \"\%s\"\n", header(msg)))
        if (!keep_conn) {
            close(msg)
            fd(msg) <- as.FD(-1L)
        }
        if (!simplify) 
            msg <- list(msg)
        invisible(msg)
    }
    else UseMethod("receive", x)
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
% \keyword{ ~kwd1 }
% \keyword{ ~kwd2 }
% Use only one keyword per line.
% For non-standard keywords, use \concept instead of \keyword:
% \concept{ ~cpt1 }
% \concept{ ~cpt2 }
% Use only one concept per line.
