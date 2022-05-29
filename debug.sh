#!/bin/bash
debug() {
	input="$1"

	./holycc "$input" > out.s
	cat out.s
}

# debug "if (5) 11;"
debug "if (0) 11; else 5;"

echo OK
