int testchar(char *expected, char *actual) {
  if(expected == actual) {
    pass();
    return 0;
  } else {
    failchar(expected, actual);
    return 1;
  }
  return 0;
}

int case92() {
  // char 型を戻り値とする関数のコンパイルがまだできないため、テストが書けない
  char *x = "hello literal!";
  char *y = "hello literal!";
  testchar(x, y);
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

int main() {
  case92();
  case93();
}
