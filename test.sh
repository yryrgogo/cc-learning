#!/bin/bash
assert() {
	expected="$1"
	input="$2"

	rm tmp.s
	./holycc "$input" > tmp.s
	cc -o tmp tmp.s call_func.o
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
	  echo "$input => $actual"
	else
	  echo "$input => $expected expected, but got $actual"
		exit 1
	fi
	echo ""
}

assert 0 "main() { 0; }"
assert 42 "main() {42;}"
# error: 255 > x
# # assert 421 "421;"
assert 21 "main() {5+20-4;}"
assert 41 "main() { 12 + 34 - 5 ;}"
assert 47 'main() {5+6*7;}'
assert 15 'main() {5*(9-6);}'
assert 4 'main() {(3+5)/2;}'
assert 10 'main() {-10+20;}'
assert 90 'main() {+10-20-(-100);}'
assert 10 'main() {- -10;}'
assert 10 'main() {- - +10;}'
assert 10 'main() {a = 10;a;}'
assert 10 'main() {abc = 10;abc;}'
assert 14 "main() {a = 3;b = 5 * 6 - 8;a + b / 2;}"
assert 5 "
main() {
return 5;
}
"
assert 3 "
main() {
a = 3;
return a;
}
"
# # error: 255 > x
# # assert 555 "return 555;"
assert 14 "
main() {
a = 3;
b = 5 * 6 - 8;
return a + b / 2;
}
"

# if
assert 11 "main() {if (5) 11;}"
assert 5 "main() {if (0) 11; else 5;}"
assert 2 "main() {if (0) 11; else if (1) 2; else 0;}"
assert 0 "main() {if (0) 11; else if (0) 2; else 0;}"
assert 10 "main() {i = 4; if (i < 5) 10;}"
assert 11 "
main() {
if (1)
  if (1) 11;
else if (0) 2;
else 0;
}
"

# while
assert 6 "
main() {
a = 0;
while (a != 6)
  a = a + 3;
a;
}
"
assert 12 "
main() {
a = 0;
while (a != 12)
  a = a + 3;
a;
}
"

# for
assert 5 "
main() {
a = 0;
for (i = 0; i < 5; i = i + 1)
  a = a + 1;
a;
}
"

assert 24 "
main() {
a = 0;
for (i = 0; i < 12; i = i + 1)
  a = a + 2;
a;
}
"

# block
assert 3 "
main() {
a = 2;
{
	a = a + 1;
}
a;
}
"

assert 1 "
main() {
a = 0;
{
	a = 5;
	a = 4;
	a = 3;
	a = 2;
	a = 1;
}
a;
}
"

# Function Call no arguments
assert 0 "
main() {
arg0();
0;
}
"

# Function Call with arguments
assert 0 "main() { arg1(1); 0; } "

assert 0 "
main() {
arg2(1, 2);
0;
}
"
assert 0 "
main() {
arg3(1, 2, 3);
0;
}
"
assert 0 "
main() {
arg4(1, 2, 3, 4);
0;
}
"
assert 0 "
main() {
arg5(1, 2, 3, 4, 5);
0;
}
"
assert 0 "
main() {
arg6(1, 2, 3, 4, 5, 6);
0;
}
"

# ==============================
# Function definition
# ==============================

assert 0 "
foo() {
  2;
}

main() {
foo();
0;
}
"

assert 7 "
foo() {
  z = 2;
  a = 8;
  return a + z;
}

main() {
  foo();
  7;
}
"

assert 10 "
foo() {
  z = 2;
  a = 8;
  return a + z;
}

main() {
  r = foo();
  r;
}
"

assert 13 "
foo() {
  a = 2;
  a = 8;
  return a;
}

main() {
  r = foo();
  r + 5;
}
"

assert 6 "
foo(a) {
  return a;
}

main() {
  b = foo(6);
  12;
  b;
}
"

assert 24 "
foo(a) {
  return a;
}

main() {
  b = foo(6);
  12;
  b + 18;
}
"

echo OK
