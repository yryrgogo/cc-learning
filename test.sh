#!/bin/bash
assert() {
  expected="$1"
  input="$2"
  rm tmp.s
  ./holycc "$input" "0" > tmp.s
  cc -o tmp tmp.s call_func.o pointer_calc.o && ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
  echo ""
}

assert 0 "int main() { 0; }"
assert 42 "int main() {42;}"
assert 21 "int main() {5+20-4;}"
assert 41 "int main() { 12 + 34 - 5 ;}"
assert 47 'int main() {5+6*7;}'
assert 15 'int main() {5*(9-6);}'
assert 4 'int main() {(3+5)/2;}'
assert 10 'int main() {-10+20;}'
assert 90 'int main() {+10-20-(-100);}'
assert 10 'int main() {- -10;}'
assert 10 'int main() {- - +10;}'
assert 10 'int main() {int a;a = 10;a;}'
assert 10 'int main() {int abc;abc = 10;abc;}'
assert 8 "int main() {
  int a;
  int b;
  a = 3;
  b = 5;
  a + b;
  }"

assert 5 "
int main() {
return 5;
}
"
assert 3 "
int main() {
  int a;
  a = 3;
  return a;
}
"
# # error: 255 > x
# # assert 555 "return 555;"
assert 14 "
int main() {
  int a;
  int b;
  a = 3;
  b = 5 * 6 - 8;
  return a + b / 2;
}
"

# if
assert 11 "int main() {if (5) 11;}"
assert 5 "int main() {if (0) 11; else 5;}"
assert 2 "int main() {if (0) 11; else if (1) 2; else 0;}"
assert 0 "int main() {if (0) 11; else if (0) 2; else 0;}"
assert 10 "int main() {int i; i = 4; if (i < 5) 10;}"
assert 11 "
int main() {
if (1)
  if (1) 11;
else if (0) 2;
else 0;
}
"

assert 9 "
int main() {
int a = 6;
if (a == 6)
  a = a + 3;
a;
}
"

# while
assert 6 "
int main() {
int a = 0;
while (a != 6)
  a = a + 3;
a;
}
"
assert 12 "
int main() {
  int a;
  a = 0;
  while (a != 12)
    a = a + 3;
  a;
}
"

# for
assert 5 "
int main() {
  int a;
  a = 0;
  for (int i = 0; i < 5; i = i + 1)
    a = a + 1;
  a;
}
"

assert 24 "
int main() {
  int a;
  a = 0;
  for (int i = 0; i < 12; i = i + 1)
    a = a + 2;
  a;
}
"

# block
assert 3 "
int main() {
  int a;
  a = 2;
  {
    a = a + 1;
  }
  a;
}
"

assert 1 "
int main() {
  int a;
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
assert 0 " int main() { arg0(); 0; } "

# Function Call with arguments
assert 0 "int main() { arg1(1); 0; } "

assert 0 "
int main() {
    arg2(1, 2);
    0;
}
"
assert 0 "
int main() {
    arg3(1, 2, 3);
    0;
}
"
assert 0 "
int main() {
    arg4(1, 2, 3, 4);
    0;
}
"
assert 0 "
int main() {
    arg5(1, 2, 3, 4, 5);
    0;
}
"
assert 0 "
int main() {
    arg6(1, 2, 3, 4, 5, 6);
    0;
}
"

# ==============================
# Function definition
# ==============================

assert 1 "
int foo(int a, int b) {
  return a;
}

int main() {
  foo(1, 3 + 18);
}
"

assert 2 "
int foo() {
  int z;
  z = 2;
  return z;
}

int main() {
  int result;
  result = foo();
  result;
}
"

assert 2 "
int bar() {
  return 2;
}

int main() {
  bar();
}
"

assert 10 "
int foo() {
  int z;
  int a;
  z = 2;
  a = 8;
  return a + z;
}

int main() {
  int r;
  r = foo();
  r;
}
"

assert 6 "
int foo(int a) {
  return a;
}

int main() {
  int b;
  b = foo(6);
  b;
}
"

assert 6 "
int foo(int a, int b) {
  return a + b;
}

int main() {
  int result;
  result = foo(1, 2);
  55;
  result + 3;
}
"

assert 39 "
int foo(int ab, int cd, int ef) {
  return ab + cd + ef;
}

int main() {
  int result;
  result = foo(1, 2, 3);
  55;
  result + 33;
}
"

assert 2 "
int foo(int abc, int def, int ghi, int jkl) {
  int z;
  z = abc + def + ghi - jkl;
  return z;
}

int main() {
  int result;
  result = foo(1, 2, 3, 4);
  result;
}
"

assert 5 "
int foo(int a, int b, int c, int d, int e) {
  int z;
  z = a - b + c - d + e;
  return z + 2;
}

int main() {
  int result;
  result = foo(1, 2, 3, 4, 5);
  result;
}
"

assert 2 "
int foo(int a, int b, int c, int d, int e, int f) {
  int z;
  z = a - b + c - d + e - f;
  return z + 5;
}

int main() {
  int result;
  result = foo(1, 2, 3, 4, 5, 6);
  result;
}
"

# ==============================
# Fibonacci
# ==============================

assert 55 "
int fibonacci(int n){
  int a;
  int b;
  a = 0;
  b = 1;
  if (n == 1){
    return a;
  }
  while (n > 1){
    int c;
    c = b;
    b = a + b;
    a = c;
    n = n - 1;
  }
  return b;
}

int main() {
  int result;
  result = fibonacci(10);
  result;
}
"

# nested if
assert 11 "
int main()
{
  int a;
  int b;
  int c;
  int n;
  a = 0;
  b = 0;
  n = 1;
	c = 10;
  if (a == 0){
    if (n == 1){
      b = 33;
			c = 11;
    }
  }
  c;
}
"

assert 7 "
int foo(int a, int b, int n)
{
  if (a == 0){
    if (n == 1){
      return 7;
    }
  }
  return 12;
}

int main()
{
  int result;
  result = foo(0, 2, 1);
  result;
}
"

assert 77 "
int foo(int a, int b, int n)
{
  if (a == 0){
    if (n == 1){
      return 7;
    }
    else {
      return 77;
		}
  }
  return 12;
}

int main()
{
  int result;
  result = foo(0, 2, 10);
  result;
}
"

assert 12 "
int foo(int a, int b, int n)
{
  if (a == 0){
    if (n == 1){
      return 7;
    }
    else {
      a = 12;
		}
  }
  return a;
}

int main()
{
  int result;
  result = foo(0, 2, 10);
  result;
}
"

assert 14 "
int foo(int a, int n)
{
  if (a == 0){
    if (n == 1){
      return a;
    }
  }
  int x;
  x = n - 1;
  return x;
}

int main()
{
  int result;
  result = foo(5, 15);
  result;
}
"

assert 5 "
int main()
{
  if (0 == 0){
  }
  5;
}
"

assert 33 "
int foo()
{
  if (0 == 0){
  }
  5;
}

int main()
{
  1;
  2;
  3;
  33;
}
"

assert 5 "
int bar() {
  for (int i = 0; i < 5; i = i + 1){}
  return 5;
}
int main() {
  int result;
  result = bar();
  result;
}
"

assert 7 "
int main()
{
  if (0 == 0){
    5;
  }else{
  }
  7;
}
"

# fibonacci ではないがこれが通らなかった。push, pop の整合性が取れなくなっていた
assert 14 "
int foo(int n)
{
  return 14;
}

int main()
{
  int n;
  n = 10;
  if (0 == 0){
    if (n == 0){
    }
  }
  14;
}
"

assert 9 "
int bar(int n)
{
  return n;
}

int main()
{
  int a;
  a = 10;
  bar(a - 1);
}
"

assert 8 "
int rec(int n, int a)
{
  if (n == 0){
    return a;
  }
  return rec(n - 1, a+1);
}

int main()
{
  rec(3, 5);
}
"

assert 120 "
int rec(int a, int b)
{
  if (a == 0){
    return b;
  }
  return rec(a - 1, b+1);
}

int main()
{
  rec(100, 20);
}
"

assert 16 "
int foo(int a, int b, int c)
{
  if (a == 1){
    if (b == 2){
      return c;
    }
    return b;
  }
  int r;
  r = foo(a-1, b-1, c+1);
  return r;
}

int main()
{
  int result;
  result = foo(5, 20, 3);
  result;
}
"

assert 55 "
int fibonacci(int a, int b, int n)
{
  if (n == 1){
    if (a == 0){
      return a;
    }
    return b;
  }
  int r;
  r = fibonacci(b, a + b, n - 1);
  return r;
}

int main()
{
  int result;
  result = fibonacci(0, 1, 10);
  result;
}
"

assert 10 "
int main (){
  int a;
  a = 10;
  int *addr;
  addr = &a;
  *addr;
}
"

assert 7 " int main() { int x; int *y; y = &x; *y = 7; x;} "
assert 3 " int main(){ int x; int *y; y = &x; *y = 3; return x; } "

assert 13 "
int main() {

  int x;
  x = 5;
  int *y;
  y = &x;
  *y = 13;

  int **z;
  z = &y;

  **z;
}
"

assert 3 "
int main(){
  int a;
  int b;
  a = 5;
  b = 3;
  int *p;
  p = &a;
  p = p + 1;
  *p;
}
"

stack は下方向（マイナス方向）にアドレスを確保するため、先に定義された変数を参照するにはアドレスを減算する
assert 6 "
int main(){
  int a;
  int b;
  int c;
  a = 5;
  b = 3;
  c = 6;

  int *p;
  p = &a;
  p = p + 2;

  *p;
}
"

assert 11 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); *(p + 0); } "
assert 22 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); *(p + 1); } "
assert 44 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); *(p + 2); } "
assert 88 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); *(p + 3); } "
assert 22 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); p=p+3; *(p - 2); } "

assert 4 "int main() { sizeof 4;}"
assert 4 "int main() { int a; a = 10; sizeof a;}"
assert 8 "int main() { int x; x = 10; int *y; y = &x; sizeof y;}"
assert 8 "int main() { int x; x = 10; int *y; y = &x; sizeof(y);}"

assert 3 "
int main() {
  int a[2];
  a[0] = 1;
  a[1] = 2;
  return a[0] + a[1];
}
"

assert 15 "int main() {
  int a[10];
  a[1] = 7;
  a[2] = 8;
  a[1] + a[2];
}"

assert 40 "int main() { int a[10]; sizeof a; }"
assert 80 "int main() { int *a[10]; sizeof a; }"
assert 80 "int main() { int **a[10]; sizeof a; }"
assert 80 "int main() { int *****a[10]; sizeof a; }"

assert 1 "
int main(){
  int a[2];
  *a = 1;
  *a;
}
"

assert 1 " int main(){ int a[2]; *a = 1; *(a + 1) = 2; *a; } "

assert 2 "
int main(){
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  *(a + 1);
}
"

assert 3 "
int main() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  return *a + *(a + 1);
}
"

assert 6 "
int main() {
  int a[2];
  a[0] = 6;
  a[1] = 8;
  int *p;
  p = a;
  return *p;
}
"

assert 3 "
int main() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  int *p;
  p = a;
  return *p + *(p + 1);
}
"

assert 2 "
int main() {
  int a[3];
  a[0] = 0;
  a[1] = 1;
  a[2] = 2;
  return a[2];
}
"

# C では 2[a] の書き方も有効
assert 2 "
int main() {
  int a[3];
  a[0] = 0;
  a[1] = 1;
  a[2] = 2;
  return 2[a];
}
"

# assert 500 "int main(){500;}"

echo OK

# alloc4(&p, 1, 2, 4, 8);

