#include <stdio.h>

int arg0() { printf("no args\n"); }

int arg1(int num) { printf("%d\n", num); }

int arg2(int n1, int n2) { printf("%d/%d!!\n", n1, n2); }

int arg3(int n1, int n2, int n3) { printf("%d/%d/%d!!\n", n1, n2, n3); }

int arg4(int n1, int n2, int n3, int n4) {
  printf("%d/%d/%d/%d!!\n", n1, n2, n3, n4);
}

int arg5(int n1, int n2, int n3, int n4, int n5) {
  printf("%d/%d/%d/%d/%d!!\n", n1, n2, n3, n4, n5);
}

int arg6(int n1, int n2, int n3, int n4, int n5, int n6) {
  printf("%d/%d/%d/%d/%d/%d!!\n", n1, n2, n3, n4, n5, n6);
}
