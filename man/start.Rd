\name{start}
\alias{start}
\title{Begin Communication Node}
\description{
This function starts a communication node at the machine on which it is run.
The communication node exists as a message queue running on a separate thread.
Received messages are stored in a local queue and may be retrieved via the \code{\link{receive}} function.
}
\usage{
start(address = NULL, port = 0L, threads = getOption("orcv.cores", 4L))
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{address}{
A character address for the communication node to be reachable by. Leave \code{NULL} for localhost.
}
  \item{port}{
Integer port to bind to.
}
  \item{threads}{
Number of threads made available to the listening queue. Controlled by the \code{"orcv.cores"} option.
}
}
\value{
An invisible \code{0L} if no error.
}
\seealso{
\code{\link{receive}}, \code{\link{send}}
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.

## The function is currently defined as
function (address = NULL, port = 0L, threads = getOption("orcv.cores", 
    4L)) 
{
    stopifnot(is.character(address) || is.null(address))
    res <- .Call(C_start, address, as.integer(port), as.integer(threads))
    ORCV_GLOBAL$STARTED <- TRUE
    invisible(res)
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
\keyword{programming}
% \keyword{ ~kwd2 }
% Use only one keyword per line.
% For non-standard keywords, use \concept instead of \keyword:
% \concept{ ~cpt1 }
% \concept{ ~cpt2 }
% Use only one concept per line.
