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
  char *x = "hello literal!";
  char *y = "hello literal!";
  testchar(x, y);
  return 0;
}

int main() { case92(); }
