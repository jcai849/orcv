\name{send}
\alias{send}
\alias{send.Message}
\alias{send.FD}
\alias{send.character}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Send an R object to a remote location
}
\description{
S3 generic to send an R object to a location that may be method-defined.
Message objects encapsulate a payload along with an address, but other classes require a destination as the \var{x} argument, either through a FD class, or as a character hostname.
}
\usage{
send(x, ...)
\method{send}{Message}(x, header, payload=NULL, keep_conn=FALSE, ...)
\method{send}{FD}(x, header, payload=NULL, keep_conn=FALSE, ...)
\method{send}{character}(x, port, header, payload=NULL, keep_conn=FALSE, ...) 
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{
Destination, or object encapsulating one, serving as S3 dispatch.
}
  \item{header}{
    character header
  }
  \item{payload}{
    Object to send
  }
  \item{keep_conn}{
    Boolean flag to keep connection open
  }
  \item{port}{
    Integer port to bind to.
  }
  \item{\dots}{
Passed on to further methods.
}
}
\value{
Typically a File Descriptor of the connection if successful.
\code{-1L} if connection is not kept.
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
function (x, ...) 
{
    stopifnot(ORCV_GLOBAL$STARTED)
    stopifnot(length(x) > 0)
    UseMethod("send", x)
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
