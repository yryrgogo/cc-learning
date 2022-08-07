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

int case73() { return sizeof 4; }
int case74() {
  int a = 10;
  return sizeof a;
}
int case75() {
  int x = 10;
  int *y = &x;
  return sizeof y;
}
int case76() {
  int x;
  x = 10;
  int *y;
  y = &x;
  return sizeof(y);
}
int case77() {
  int a[2];
  a[0] = 1;
  a[1] = 2;
  return a[0] + a[1];
}
int case78() {
  int a[10];
  a[1] = 7;
  a[2] = 8;
  return a[1] + a[2];
}
int case79() {
  int a[10];
  return sizeof a;
}
int case80() {
  int *a[10];
  return sizeof a;
}
int case81() {
  int **a[10];
  return sizeof a;
}
int case82() {
  int *****a[10];
  return sizeof a;
}
int case83() {
  int a[2];
  *a = 1;
  return *a;
}
int case84() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  return *a;
}
int case85() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  return *(a + 1);
}
int case86() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  return *a + *(a + 1);
}
int case87() {
  int a[2];
  a[0] = 6;
  a[1] = 8;
  int *p;
  p = a;
  return *p;
}
int case88() {
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  int *p;
  p = a;
  return *p + *(p + 1);
}
int case89() {
  int a[3];
  a[0] = 0;
  a[1] = 1;
  a[2] = 2;
  return a[2];
}
int case90() {
  int a[3];
  a[0] = 0;
  a[1] = 1;
  a[2] = 2;
  return 2 [a];
}

int main() {
  test(4, case73());
  test(4, case74());
  test(8, case75());
  test(8, case76());
  test(3, case77());
  test(15, case78());
  test(40, case79());
  test(80, case80());
  test(80, case81());
  test(80, case82());
  test(1, case83());
  test(1, case84());
  test(2, case85());
  test(3, case86());
  test(6, case87());
  test(3, case88());
  test(2, case89());
  test(2, case90());
}
