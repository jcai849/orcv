#!/usr/bin/awk -f
/FD [[:digit:]]+ ((opened)|(closed))/ {
	match($0, "[[:digit:]]+");
	fd = substr($0, RSTART, RLENGTH) + 0;
	if (match($0, "opened")) {
		openfds[fd]=fd;
	} else {
		delete openfds[fd];
	}
	printf "Open FD's: ";
	for (i in openfds) {
		printf "%d ", i ;
	}
	printf "\n";
}
