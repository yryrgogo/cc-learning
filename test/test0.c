int test(int expected, int actual) {
  if(expected == actual) {
    pass();
    return 0;
  } else {
    fail(expected, actual);
    return 1;
  }
  return 0;
}

int case1() { return 42; }
int case2() { return 5 + 20 - 4; }
int case3() {
  int a = 10;
  int b = 20;
  return a + b;
}

int case4() { return 5 + 20 - 4; }
int case5() { return 12 + 34 - 5; }
int case6() { return 5 + 6 * 7; }
int case7() { return 5 * (9 - 6); }
int case8() { return (3 + 5) / 2; }
int case9() { return -10 + 20; }
int case10() { return +10 - 20 - (-100); }
int case11() { return - -10; }
int case12() { return - -+10; }
int case13() {
  int a;
  a = 10;
  return a;
}
int case14() {
  int abc;
  abc = 10;
  return abc;
}
int case15() {
  int a = 3;
  int b = 5;
  return a + b;
}

int case16() { return 5; }
int case17() {
  int a = 3;
  return a;
}
int case18() {
  int a = 3;
  int b = 5 * 6 - 8;
  return a + b / 2;
}

int case19() {
  if(5)
    return 11;
}
int case20() {
  if(0)
    return 11;
  else
    return 5;
}
int case21() {
  if(0)
    return 11;
  else if(1)
    return 2;
  else
    return 0;
}
int case22() {
  if(0)
    return 11;
  else if(0)
    return 2;
  else
    return 0;
}
int case23() {
  int i;
  i = 4;
  if(i < 5)
    return 10;
}
int case24() {
  if(1)
    if(1)
      return 11;
    else if(0)
      return 2;
    else
      return 0;
}
int case25() {
  int a = 6;
  if(a == 6)
    a = a + 3;
  return a;
}
int case26() {
  int a = 0;
  while(a != 6)
    a = a + 3;
  return a;
}
int case27() {
  int a = 0;
  while(a != 12)
    a = a + 3;
  return a;
}
int case28() {
  int a = 0;
  for(int i = 0; i < 5; i = i + 1)
    a = a + 1;
  return a;
}
int case29() {
  int a = 0;
  for(int i = 0; i < 12; i = i + 1)
    a = a + 2;
  return a;
}
int case30() {
  int a = 0;
  {
    a = 5;
    a = 4;
    a = 3;
    a = 2;
    a = 1;
  }
  return a;
}
int case31() {
  arg0();
  return 0;
}

int case32() {
  arg1(1);
  return 0;
}

int case33() {
  arg2(1, 2);
  return 0;
}
int case34() {
  arg3(1, 2, 3);
  return 0;
}

int case35() {
  arg4(1, 2, 3, 4);
  return 0;
}

int case36() {
  arg5(1, 2, 3, 4, 5);
  return 0;
}

int case37() {
  arg6(1, 2, 3, 4, 5, 6);
  return 0;
}

int foo38(int a, int b) { return a; }

int case38() { return foo38(1, 3 + 18); }

int foo39() {
  int z = 2;
  return z;
}
int case39() {
  int result;
  result = foo39();
  return result;
}

int foo40() {
  int z = 2;
  int a = 8;
  return a + z;
}

int case40() {
  int r;
  r = foo40();
  return r;
}

int foo41(int a) { return a; }

int case41() {
  int b = foo41(6);
  return b;
}

int foo42(int ab, int cd, int ef) { return ab + cd + ef; }

int case42() {
  int result;
  result = foo42(1, 2, 3);
  55;
  return result + 33;
}

int foo43(int abc, int def, int ghi, int jkl) {
  int z;
  z = abc + def + ghi - jkl;
  return z;
}

int case43() {
  int result;
  result = foo43(1, 2, 3, 4);
  return result;
}

int foo44(int a, int b, int c, int d, int e) {
  int z;
  z = a - b + c - d + e;
  return z + 2;
}

int case44() {
  int result;
  result = foo44(1, 2, 3, 4, 5);
  return result;
}

int foo45(int a, int b, int c, int d, int e, int f) {
  int z;
  z = a - b + c - d + e - f;
  return z + 5;
}

int case45() {
  int result;
  result = foo45(1, 2, 3, 4, 5, 6);
  return result;
}

int fibonacci46(int n) {
  int a;
  int b;
  a = 0;
  b = 1;
  if(n == 1) {
    return a;
  }
  while(n > 1) {
    int c;
    c = b;
    b = a + b;
    a = c;
    n = n - 1;
  }
  return b;
}

int case46() {
  int result;
  result = fibonacci46(10);
  return result;
}

int case47() {
  int a = 0;
  int b = 0;
  int n = 1;
  int c = 10;
  if(a == 0) {
    if(n == 1) {
      b = 33;
      c = 11;
    }
  }
  return c;
}

int foo48(int a, int b, int n) {
  if(a == 0) {
    if(n == 1) {
      return 7;
    }
  }
  return 12;
}

int case48() {
  int result;
  result = foo48(0, 2, 1);
  return result;
}

int foo49(int a, int b, int n) {
  if(a == 0) {
    if(n == 1) {
      return 7;
    } else {
      return 77;
    }
  }
  return 12;
}

int case49() {
  int result;
  result = foo49(0, 2, 10);
  return result;
}

int foo50(int a, int b, int n) {
  if(a == 0) {
    if(n == 1) {
      return 7;
    } else {
      a = 12;
    }
  }
  return a;
}

int case50() {
  int result;
  result = foo50(0, 2, 10);
  return result;
}

int foo51(int a, int n) {
  if(a == 0) {
    if(n == 1) {
      return a;
    }
  }
  int x;
  x = n - 1;
  return x;
}

int case51() {
  int result;
  result = foo51(5, 15);
  return result;
}

int case52() {
  if(0 == 0) {
  }
  return 5;
}

int foo53() {
  if(0 == 0) {
  }
  return 5;
}

int case53() {
  1;
  2;
  3;
  return 33;
}

int bar54() {
  for(int i = 0; i < 5; i = i + 1) {
  }
  return 5;
}
int case54() {
  int result;
  result = bar54();
  return result;
}

int case55() {
  if(0 == 0) {
    5;
  } else {
  }
  return 7;
}

int foo56(int n) { return 14; }

int case56() {
  int n;
  n = 10;
  if(0 == 0) {
    if(n == 0) {
    }
  }
  return 14;
}

int bar57(int n) { return n; }

int case57() {
  int a;
  a = 10;
  return bar57(a - 1);
}

int rec58(int n, int a) {
  if(n == 0) {
    return a;
  }
  return rec58(n - 1, a + 1);
}

int case58() { return rec58(3, 5); }

int rec59(int a, int b) {
  if(a == 0) {
    return b;
  }
  return rec59(a - 1, b + 1);
}

int case59() { return rec59(100, 20); }

int foo60(int a, int b, int c) {
  if(a == 1) {
    if(b == 2) {
      return c;
    }
    return b;
  }
  int r;
  r = foo60(a - 1, b - 1, c + 1);
  return r;
}

int case60() {
  int result;
  result = foo60(5, 20, 3);
  return result;
}

int fibonacci61(int a, int b, int n) {
  if(n == 1) {
    if(a == 0) {
      return a;
    }
    return b;
  }
  int r;
  r = fibonacci61(b, a + b, n - 1);
  return r;
}

int case61() {
  int result;
  result = fibonacci61(0, 1, 10);
  return result;
}

int case62() {
  int a;
  a = 10;
  int *addr;
  addr = &a;
  return *addr;
}

int case63() {
  int x;
  int *y;
  y = &x;
  *y = 7;
  return x;
}
int case64() {
  int x;
  int *y;
  y = &x;
  *y = 3;
  return x;
}
int case65() {
  int x;
  x = 5;
  int *y;
  y = &x;
  *y = 13;

  int **z;
  z = &y;

  return **z;
}

int case66() {
  int a;
  int b;
  a = 5;
  b = 3;
  int *p;
  p = &a;
  p = p + 1;
  return *p;
}

int case67() {
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

int case68() {
  int *p;
  alloc4(&p, 11, 22, 44, 88);
  return *(p + 0);
}
int case69() {
  int *p;
  alloc4(&p, 11, 22, 44, 88);
  return *(p + 1);
}
int case70() {
  int *p;
  alloc4(&p, 11, 22, 44, 88);
  return *(p + 2);
}
int case71() {
  int *p;
  alloc4(&p, 11, 22, 44, 88);
  return *(p + 3);
}
int case72() {
  int *p;
  alloc4(&p, 11, 22, 44, 88);
  p = p + 3;
  return *(p - 2);
}

int main() {
  test(42, case1());
  test(21, case2());
  test(30, case3());
  test(21, case4());
  test(41, case5());
  test(47, case6());
  test(15, case7());
  test(4, case8());
  test(10, case9());
  test(90, case10());
  test(10, case11());
  test(10, case12());
  test(10, case13());
  test(10, case14());
  test(8, case15());
  test(5, case16());
  test(3, case17());
  test(14, case18());
  test(11, case19());
  test(5, case20());
  test(2, case21());
  test(0, case22());
  test(10, case23());
  test(11, case24());
  test(9, case25());
  test(6, case26());
  test(12, case27());
  test(5, case28());
  test(24, case29());
  test(1, case30());
  test(0, case31());
  test(0, case32());
  test(0, case33());
  test(0, case34());
  test(0, case35());
  test(0, case36());
  test(0, case37());
  test(1, case38());
  test(2, case39());
  test(10, case40());
  test(6, case41());
  test(39, case42());
  test(2, case43());
  test(5, case44());
  test(2, case45());
  test(55, case46());
  test(11, case47());
  test(7, case48());
  test(77, case49());
  test(12, case50());
  test(14, case51());
  test(5, case52());
  test(33, case53());
  test(5, case54());
  test(7, case55());
  test(14, case56());
  test(9, case57());
  test(8, case58());
  test(120, case59());
  test(16, case60());
  test(55, case61());
  test(10, case62());
  test(7, case63());
  test(3, case64());
  test(13, case65());
  test(3, case66());
  test(6, case67());
  test(11, case68());
  test(22, case69());
  test(44, case70());
  test(88, case71());
  test(22, case72());

  return 0;
}
