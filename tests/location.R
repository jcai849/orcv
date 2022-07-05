library(orcv)

l0 <- location(0)
address(l0)
port(l0)
l1 <- location(1)
address(l1)
port(l1)
l2 <- location(2)
address(l2)
port(l2)

la <- as.Location("localhost", 0L)
lb <- as.Location("localhost", 9000L)

print(la)

dfa <- data.frame(a=la, b=lb)
lc <- c(la, lb)
rbind(dfa, dfa)

lapply(la, identity)
lapply(lc, identity)

la[1]
lc[1]

lc == la
la == lc
lb == lb
la == l0
la == l1

dfa$a == la
dfa$b == lb
