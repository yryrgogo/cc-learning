#!/bin/bash
debug() {
	input="$1"

	./9cc "$input" > debug_print.s
	cat out.s
}

debug "0;"

echo OK
