#include <stdio.h>

int main() {
    int a,b;

    a = (b = 10) + 20;
    printf("%d %d\n", a, b);
    a++;
    ++a;
    a;
}