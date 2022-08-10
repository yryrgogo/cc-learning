#!/bin/bash
test() {
  file_path="$1"
  rm test.s
  ./holycc ./test/$file_path.c > test.s
  cc -o run_test test.s ./sample/call_func.c ./sample/pointer_calc.c
  ./run_test
}

is_debug=$1
if [ "$is_debug" = "1" ]; then
  test debug_test
else
  test basic_test
  test pointer_array_test
  test global_test
  test char_test
fi
