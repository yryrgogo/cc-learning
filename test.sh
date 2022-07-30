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

assert 0 "int main() { return 0; }"
assert 42 "int main() { return 42;}"
assert 21 "int main() { return 5+20-4;}"
assert 41 "int main() { return 12 + 34 - 5 ;}"
assert 47 'int main() {return 5+6*7;}'
assert 15 'int main() {return 5*(9-6);}'
assert 4 'int main() {return (3+5)/2;}'
assert 10 'int main() {return -10+20;}'
assert 90 'int main() {return +10-20-(-100);}'
assert 10 'int main() {return - -10;}'
assert 10 'int main() {return - - +10;}'
assert 10 'int main() {int a;a = 10;return a;}'
assert 10 'int main() {int abc;abc = 10;return abc;}'
assert 8 "int main() {
  int a;
  int b;
  a = 3;
  b = 5;
  return a + b;
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
assert 11 "int main() {if (5) return 11;}"
assert 5 "int main() {if (0) return 11; else return 5;}"
assert 2 "int main() {if (0) return 11; else if (1) return 2; else return 0;}"
assert 0 "int main() {if (0) return 11; else if (0) return 2; else return 0;}"
assert 10 "int main() {int i; i = 4; if (i < 5) return 10;}"
assert 11 "
int main() {
if (1)
  if (1) return 11;
else if (0) return 2;
else return 0;
}
"

assert 9 "
int main() {
int a = 6;
if (a == 6)
  a = a + 3;
return a;
}
"

# while
assert 6 "
int main() {
int a = 0;
while (a != 6)
  a = a + 3;
return a;
}
"
assert 12 "
int main() {
  int a;
  a = 0;
  while (a != 12)
    a = a + 3;
  return a;
}
"

# for
assert 5 "
int main() {
  int a;
  a = 0;
  for (int i = 0; i < 5; i = i + 1)
    a = a + 1;
  return a;
}
"

assert 24 "
int main() {
  int a;
  a = 0;
  for (int i = 0; i < 12; i = i + 1)
    a = a + 2;
  return a;
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
  return a;
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
  return a;
}
"

# Function Call no arguments
assert 0 " int main() { arg0(); return 0; } "

# Function Call with arguments
assert 0 "int main() { arg1(1); return 0; } "

assert 0 "
int main() {
    arg2(1, 2);
    return 0;
}
"
assert 0 "
int main() {
    arg3(1, 2, 3);
    return 0;
}
"
assert 0 "
int main() {
    arg4(1, 2, 3, 4);
    return 0;
}
"
assert 0 "
int main() {
    arg5(1, 2, 3, 4, 5);
    return 0;
}
"
assert 0 "
int main() {
    arg6(1, 2, 3, 4, 5, 6);
    return 0;
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
  return foo(1, 3 + 18);
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
  return result;
}
"

assert 2 "
int bar() {
  return 2;
}

int main() {
  return bar();
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
  return r;
}
"

assert 6 "
int foo(int a) {
  return a;
}

int main() {
  int b;
  b = foo(6);
  return b;
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
  return result + 3;
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
  return result + 33;
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
  return result;
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
  return result;
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
  return result;
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
  return result;
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
  return c;
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
  return result;
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
  return result;
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
  return result;
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
  return result;
}
"

assert 5 "
int main()
{
  if (0 == 0){
  }
  return 5;
}
"

assert 33 "
int foo()
{
  if (0 == 0){
  }
  return 5;
}

int main()
{
  1;
  2;
  3;
  return 33;
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
  return result;
}
"

assert 7 "
int main()
{
  if (0 == 0){
    5;
  }else{
  }
  return 7;
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
  return 14;
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
  return bar(a - 1);
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
  return rec(3, 5);
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
  return rec(100, 20);
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
  return result;
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
  return result;
}
"

assert 10 "
int main (){
  int a;
  a = 10;
  int *addr;
  addr = &a;
  return *addr;
}
"

assert 7 " int main() { int x; int *y; y = &x; *y = 7; return x;} "
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

  return **z;
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
  return *p;
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

  return *p;
}
"

assert 11 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); return *(p + 0); } "
assert 22 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); return *(p + 1); } "
assert 44 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); return *(p + 2); } "
assert 88 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); return *(p + 3); } "
assert 22 " int main(){ int *p; alloc4(&p, 11, 22, 44, 88); p=p+3; return *(p - 2); } "

assert 4 "int main() { return sizeof 4;}"
assert 4 "int main() { int a; a = 10; return sizeof a;}"
assert 8 "int main() { int x; x = 10; int *y; y = &x; return sizeof y;}"
assert 8 "int main() { int x; x = 10; int *y; y = &x; return sizeof(y);}"

assert 3 " int main() { int a[2]; a[0] = 1; a[1] = 2; return a[0] + a[1]; } "

assert 15 "int main() {
  int a[10];
  a[1] = 7;
  a[2] = 8;
  return a[1] + a[2];
}"

assert 40 "int main() { int a[10]; return sizeof a; }"
assert 80 "int main() { int *a[10]; return sizeof a; }"
assert 80 "int main() { int **a[10]; return sizeof a; }"
assert 80 "int main() { int *****a[10]; return sizeof a; }"

assert 1 "
int main(){
  int a[2];
  *a = 1;
  return *a;
}
"

assert 1 " int main(){ int a[2]; *a = 1; *(a + 1) = 2; return *a; } "

assert 2 "
int main(){
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  return *(a + 1);
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

assert 5 " int main() { char x[3]; x[0] = 5; return x[0];} "
assert 9 "
int main() {
  char x[3];
  x[0] = 5;
  x[1] = 3;
  x[2] = 1;
  return x[0] + x[1] + x[2];
} "

assert 4 "
int main() {
  char a;
  char b;
  a = 1;
  b = 3;
  return a + b;
} "

# TODO: Global Variable Assign
assert 5 "int a; int main() { a = 5; return a; } "

# assert 500 "int main(){500;}"

echo OK

# alloc4(&p, 1, 2, 4, 8);

