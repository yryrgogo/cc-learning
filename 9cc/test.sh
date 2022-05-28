#!/bin/bash
assert() {
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
	  echo "$input => $actual"
	else
	  echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

assert 0 "0;"
assert 42 "42;"
# error: 255 > x
# assert 421 "421;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 90 '+10-20-(-100);'
assert 10 '- -10;'
assert 10 '- - +10;'
assert 10 'a = 10;a;'
assert 10 'abc = 10;abc;'
assert 14 "a = 3;b = 5 * 6 - 8;a + b / 2;"
assert 5 "
return 5;
"
assert 3 "
a = 3;
return a;
"
assert 55 "return 55;"
# error: 255 > x
# assert 555 "return 555;"
assert 14 "
a = 3;
b = 5 * 6 - 8;
return a + b / 2;
"

# if
assert 11 "if (5) 11;"
assert 64 "if (0) 11;"
assert 5 "if (0) 11; else 5;"
assert 2 "if (0) 11; else if (1) 2; else 0;"
assert 0 "if (0) 11; else if (0) 2; else 0;"
assert 11 "
if (1)
  if (1) 11;
else if (0) 2;
else 0;"

echo OK
