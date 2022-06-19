#include <stdio.h>
#include <stdlib.h>

void alloc(int **p) {
  *p = malloc(sizeof(int) * 1);
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;
}

void alloc4(int **p, int a, int b, int c, int d) {
  *p = malloc(sizeof(int) * 4);
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;
}

int main() {

  int *p;
  alloc4(&p, 1, 2, 4, 8);
  printf("%d\n", *(p + 2));

  return 0;
}
