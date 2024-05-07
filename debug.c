#include <stdio.h>
#include <string.h>
#include "halloc.h"
int main(int argc, char **argv[]) {
printf("\nPhase 1: Success\n\n\n");
MEM_init(50);
printf("\nPhase 2: Success\n\n\n");
char *str = (char *)halloc(strlen("YES W!") +1); // plus one for the null terminator.
printf("Phase 3: Success");


    strcpy(str, "YES W!");
    printf("\nPhase 4: Success\n\n\n");
    printf("\naddress: %p\n\n\n", &str);
    printf("\nstring: %s\n\n\n", str);

    hfree(str);
    return 0;

}