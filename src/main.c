#include <stdio.h>
#include "elloc.h"

int main() {
    char *a = elloc(5 * sizeof(char));
    strcpy(a, "eyad");

    char *b = elloc(10 * sizeof(char));
    strcpy(b, "also eyad");

    printf("Hope for no core dumped: %s, %s\n", a, b);

    efree(a);
    efree(b);

    char *c = elloc(34 * sizeof(char));
    strcpy(c, "and also eyad, for no reason, lol");
    printf("Plz no core dumped: %s\n", c);

    efree(c);
    
    return 0;
}