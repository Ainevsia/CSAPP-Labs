#include <stdio.h>

int main(int argc, char const *argv[], char const *env[]) {
    printf("Command-line arguments:\n");
    int i;
    for (i = 0; i < argc; i++) {
        printf("\targv[%2d]: %s\n", i, argv[i]);
    }
    printf("Environment arguments:\n");
    i = 0;
    while (env[i] != NULL) {
        printf("\tenvp[%2d]: %s\n", i, env[i]);
        i++;
    }
}