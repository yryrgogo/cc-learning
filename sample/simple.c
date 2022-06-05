#include <stdio.h>

int foo(int n)
{
	int a = n + 3;
	printf("foo!! %d", a);

	return a;
}

int bar()
{
	return 2;
}

int main()
{
	foo(2);
	bar();

	return 0;
}
