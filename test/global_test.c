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

int a91;
int case91() {
  a91 = 5;
  return a91;
}

int main() { test(5, case91()); }
