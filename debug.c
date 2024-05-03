#include <stdio.h>
#include "halloc.h"
int main(int argc, char **argv[]) {
    int a = halloc(5);
    printf("address: 0x%p", &a);
    return 0;
}