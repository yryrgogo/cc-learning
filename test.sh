#!/bin/bash
test0() {
  file_path="$1"
  rm test.s
  ./holycc ./test/test0.c > test.s
  cc -o run_test test.s ./sample/call_func.c ./sample/pointer_calc.c
  ./run_test
}

pointer_array_test() {
  file_path="$1"
  rm test.s
  ./holycc ./test/pointer_array_test.c > test.s
  cc -o run_test test.s ./sample/call_func.c ./sample/pointer_calc.c
  ./run_test
}

global_test() {
  file_path="$1"
  rm test.s
  ./holycc ./test/global_test.c > test.s
  cc -o run_test test.s ./sample/call_func.c ./sample/pointer_calc.c
  ./run_test
}

char_test() {
  file_path="$1"
  rm test.s
  ./holycc ./test/char_test.c > test.s
  cc -o run_test test.s ./sample/call_func.c ./sample/pointer_calc.c
  ./run_test
}

test0
pointer_array_test
global_test
char_test
