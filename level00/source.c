#include <stdio.h>
#include <stdlib.h>

int main() {
    int input;

    puts("***********************************");
    puts("* \t     -Level00 -\t\t  *");
    puts("***********************************");
    printf("Password:");

    scanf("%d", &input);  // Read an input

    if (input == 0x149c) {  // 5276 in decimal
        puts("Authenticated!");
        system("/bin/sh");  // shell euid = level01
    } else {
        puts("Invalid Password!");
    }

    return 0;
}