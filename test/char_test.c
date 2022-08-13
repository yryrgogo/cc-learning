
int test_char(int expected, int actual) {
  if(expected == actual) {
    pass();
    return 0;
  } else {
    fail_char(expected, actual);
    return 1;
  }
  return 0;
}

int test_str(char *expected, char *actual) {
  if(expected == actual) {
    pass();
    return 0;
  } else {
    fail_str(expected, actual);
    return 1;
  }
  return 0;
}

int case92() {
  // char 型を戻り値とする関数のコンパイルがまだできないため、テストが書けない
  char *x = "hello literal!";
  char *y = "hello literal!";
  test_str(x, y);
  return 0;
}

int case93() {
  /*
    block comment test
abc
    123456
    x + y = z;
    int a = 1;
    printf("%d", a); */

  return 0;
}

char case94() {
  char a = 'a';
  return a;
}

int main() {
  case92();
  case93();
  char result_94 = case94();
  if(result_94 == 'a') {
    pass();
  } else {
    char a = 'a';
    fail_char(a, result_94);
  }

  return 0;
}
