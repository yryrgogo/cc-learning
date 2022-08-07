int testchar(char expected, char actual) {
  printchar(actual);
  if(expected == actual) {
    pass();
    return 0;
  } else {
    failchar(expected, actual);
    return 1;
  }
  return 0;
}

char case92() {
  char x[3];
  x[0] = 5;
  return x[0];
}
char case93() {
  char x[3];
  x[0] = 5;
  x[1] = 3;
  x[2] = 1;
  return x[0] + x[1] + x[2];
}
char case94() {
  char a = 1;
  char b = 3;
  return a;
}

int main() {
  testchar(5, case92());
  testchar(8, case93());
  testchar(4, case94());
}
