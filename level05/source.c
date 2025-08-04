#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[100];
    int i = 0;

    fgets(buffer, 100, stdin); 

    while (i < strlen(buffer)) {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            buffer[i] ^= 0x20;  
        }
        i++;
    }
    printf(buffer);
    exit(0);
}