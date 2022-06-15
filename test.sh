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

# assert 0 "main() { 0; }"
# assert 42 "main() {42;}"
# # error: 255 > x
# # # assert 421 "421;"
# assert 21 "main() {5+20-4;}"
# assert 41 "main() { 12 + 34 - 5 ;}"
# assert 47 'main() {5+6*7;}'
# assert 15 'main() {5*(9-6);}'
# assert 4 'main() {(3+5)/2;}'
# assert 10 'main() {-10+20;}'
# assert 90 'main() {+10-20-(-100);}'
# assert 10 'main() {- -10;}'
# assert 10 'main() {- - +10;}'
# assert 10 'main() {a = 10;a;}'
# assert 10 'main() {abc = 10;abc;}'
# assert 14 "main() {a = 3;b = 5 * 6 - 8;a + b / 2;}"
# assert 5 "
# main() {
# return 5;
# }
# "
# assert 3 "
# main() {
# a = 3;
# return a;
# }
# "
# # # error: 255 > x
# # # assert 555 "return 555;"
# assert 14 "
# main() {
# a = 3;
# b = 5 * 6 - 8;
# return a + b / 2;
# }
# "

# # if
# assert 11 "main() {if (5) 11;}"
# assert 5 "main() {if (0) 11; else 5;}"
# assert 2 "main() {if (0) 11; else if (1) 2; else 0;}"
# assert 0 "main() {if (0) 11; else if (0) 2; else 0;}"
# assert 10 "main() {i = 4; if (i < 5) 10;}"
# assert 11 "
# main() {
# if (1)
#   if (1) 11;
# else if (0) 2;
# else 0;
# }
# "

# # while
# assert 6 "
# main() {
# a = 0;
# while (a != 6)
#   a = a + 3;
# a;
# }
# "
# assert 12 "
# main() {
# a = 0;
# while (a != 12)
#   a = a + 3;
# a;
# }
# "

# # for
# assert 5 "
# main() {
# a = 0;
# for (i = 0; i < 5; i = i + 1)
#   a = a + 1;
# a;
# }
# "

# assert 24 "
# main() {
# a = 0;
# for (i = 0; i < 12; i = i + 1)
#   a = a + 2;
# a;
# }
# "

# # block
# assert 3 "
# main() {
# a = 2;
# {
#   a = a + 1;
# }
# a;
# }
# "

# assert 1 "
# main() {
# a = 0;
# {
#   a = 5;
#   a = 4;
#   a = 3;
#   a = 2;
#   a = 1;
# }
# a;
# }
# "

# # Function Call no arguments
# assert 0 "
# main() {
# arg0();
# 0;
# }
# "

# # Function Call with arguments
# assert 0 "main() { arg1(1); 0; } "

# assert 0 "
# main() {
# arg2(1, 2);
# 0;
# }
# "
# assert 0 "
# main() {
# arg3(1, 2, 3);
# 0;
# }
# "
# assert 0 "
# main() {
# arg4(1, 2, 3, 4);
# 0;
# }
# "
# assert 0 "
# main() {
# arg5(1, 2, 3, 4, 5);
# 0;
# }
# "
# assert 0 "
# main() {
# arg6(1, 2, 3, 4, 5, 6);
# 0;
# }
# "

# # ==============================
# # Function definition
# # ==============================

# assert 0 "
# foo() {
#   2;
# }

# main() {
# foo();
# 0;
# }
# "

# assert 2 "
# foo() {
#   z = 2;
#   return z;
# }

# main() {
#   result = foo();
#   result;
# }
# "

# assert 2 "
# bar() {
#   return 2;
# }

# main() {
#   bar();
# }
# "

# assert 10 "
# foo() {
#   z = 2;
#   a = 8;
#   return a + z;
# }

# main() {
#   r = foo();
#   r;
# }
# "

# assert 6 "
# foo(a) {
#   return a;
# }

# main() {
#   b = foo(6);
#   12;
#   b;
# }
# "

# assert 24 "
# foo(a) {
#   return a;
# }

# main() {
#   b = foo(6);
#   12;
#   b + 18;
# }
# "

# assert 6 "
# foo(a, b) {
#   return a + b;
# }

# main() {
#   result = foo(1, 2);
#   55;
#   result + 3;
# }
# "

# assert 39 "
# foo(ab, cd, ef) {
#   return ab + cd + ef;
# }

# main() {
#   result = foo(1, 2, 3);
#   55;
#   result + 33;
# }
# "

# assert 2 "
# foo(abc, def, ghi, jkl) {
#   return abc + def + ghi - jkl;
# }

# main() {
#   result = foo(1, 2, 3, 4);
#   result;
# }
# "

# assert 5 "
# foo(a, b, c, d, e) {
#   z = a - b + c - d + e;
#   return z + 2;
# }

# main() {
#   result = foo(1, 2, 3, 4, 5);
#   result;
# }
# "

# assert 2 "
# foo(a, b, c, d, e, f) {
#   z = a - b + c - d + e - f;
#   return z + 5;
# }

# main() {
#   result = foo(1, 2, 3, 4, 5, 6);
#   result;
# }
# "

# # ==============================
# # Fibonacci
# # ==============================

# assert 55 "
# fibonacci(n){
#   a = 0;
#   b = 1;
#   if (n == 1){
#     return a;
#   }
#   while (n > 1){
#     c = b;
#     b = a + b;
#     a = c;
#     n = n - 1;
#   }
#   return b;
# }

# main() {
#   result = fibonacci(10);
#   result;
# }
# "

# # nested if
# assert 11 "
# main()
# {
#   a = 0;
#   b = 0;
#   n = 1;
# 	c = 10;
#   if (a == 0){
#     if (n == 1){
#       b = 33;
# 			c = 11;
#     }
#   }
#   c;
# }
# "

# assert 7 "
# foo(a, b, n)
# {
#   if (a == 0){
#     if (n == 1){
#       return 7;
#     }
#   }
#   return 12;
# }

# main()
# {
#   result = foo(0, 2, 1);
#   result;
# }
# "

# assert 77 "
# foo(a, b, n)
# {
#   if (a == 0){
#     if (n == 1){
#       return 7;
#     }
#     else {
#       return 77;
# 		}
#   }
#   return 12;
# }

# main()
# {
#   result = foo(0, 2, 10);
#   result;
# }
# "

# assert 12 "
# foo(a, b, n)
# {
#   if (a == 0){
#     if (n == 1){
#       return 7;
#     }
#     else {
#       a = 12;
# 		}
#   }
#   return a;
# }

# main()
# {
#   result = foo(0, 2, 10);
#   result;
# }
# "

# assert 14 "
# foo(a, n)
# {
#   if (a == 0){
#     if (n == 1){
#       return a;
#     }
#   }
#   x = n - 1;
#   return x;
# }

# main()
# {
#   result = foo(5, 15);
#   result;
# }
# "

# assert 5 "
# main()
# {
#   if (0 == 0){
#   }
#   5;
# }
# "

# assert 33 "
# foo()
# {
#   if (0 == 0){
#   }
#   5;
# }

# main()
# {
#   1;
#   2;
#   3;
#   33;
# }
# "

# assert 5 "
# bar() {
#   for (i = 0; i < 5; i = i + 1){}
#   return 5;
# }
# main() {
#   result = bar();
#   result;
# }
# "


# assert 7 "
# main()
# {
#   if (0 == 0){
#     5;
#   }else{
#   }
#   7;
# }
# "

# # fibonacci ではないがこれが通らなかった。push, pop の整合性が取れなくなっていた
# assert 14 "
# foo(n)
# {
#   if (0 == 0){
#     if (n == 0){
#       return 33;
#     }
#   }
#   x = n - 1;
#   return x;
# }

# main()
# {
#   result = foo(15);
#   result;
# }
# "

# assert 9 "
# bar(n)
# {
#   return n;
# }

# main()
# {
#   a = 10;
#   bar(a - 1);
# }
# "

# assert 3 "
# rec(n)
# {
#   if (n == 5){
#     return 3;
#   }
#   55;
#   rec(n - 1);
# }

# main()
# {
#   result = rec(10);
#   result;
# }
# "

# assert 4 "
# rec(n, acc)
# {
#   if (n == 0){
#     return 10;
#   }
#   b = acc  + 7;
#   return b - 3;
# }

# main()
# {
#   rec(1, 0);
# }
# "

# assert 201 "
# rec(n)
# {
#   if (n == 0){
#     return 201;
#   }
#   return rec(n - 1);
# }

# main()
# {
#   rec(10);
# }
# "

# assert 120 "
# rec(a, b)
# {
#   if (a == 0){
#     return b;
#   }
#   return rec(a - 1, b+1);
# }

# main()
# {
#   rec(100, 20);
# }
# "

# assert 3 "
# foo(a, b, c)
# {
#   if (a == 1){
#     if (b == 2){
#       return c;
#     }
#     return b;
#   }
#   return a;
# }

# main()
# {
#   result = foo(1, 2, 3);
#   result;
# }
# "

# assert 5 "
# foo(a, b, c)
# {
#   if (a == 1){
#     if (b == 2){
#       return c;
#     }
#     return b;
#   }
#   return a;
# }

# main()
# {
#   result = foo(1, 5, 3);
#   result;
# }
# "

# assert 12 "
# foo(a, b, c)
# {
#   if (a == 1){
#     if (b == 2){
#       return c;
#     }
#     return b;
#   }
#   return a;
# }

# main()
# {
#   result = foo(12, 5, 3);
#   result;
# }
# "

# assert 16 "
# foo(a, b, c)
# {
#   if (a == 1){
#     if (b == 2){
#       return c;
#     }
#     return b;
#   }
#   r = foo(a-1, b-1, c+1);
#   return r;
# }

# main()
# {
#   result = foo(5, 20, 3);
#   result;
# }
# "

# assert 55 "
# fibonacci(a, b, n)
# {
#   if (n == 1){
#     if (a == 0){
#       return a;
#     }
#     return b;
#   }
#   r = fibonacci(b, a + b, n - 1);
#   return r;
# }

# main()
# {
#   result = fibonacci(0, 1, 10);
#   result;
# }
# "

# assert 10 "
# main (){
#   a = 10;
#   addr = &a;
#   *addr;
# }
# "

assert 5 "
main (){
  int a;
  a = 5;
  a;
}
"

assert 21 "
main (){
  int a;
  int b;
  a = 13;
  b = 8;
  int c;
  c = a + b;
  c;
}
"

echo OK
