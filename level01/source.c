#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int verify_user_name(char *input) {
    return strncmp(input, "dat_wil", 8);
}

int verify_user_pass(char *input) {
    char buffer[16];

    // Here we have a buffer overflow vulnerability
    // The input is copied into a fixed-size buffer without checking its length
    // This allows an attacker to overwrite the return address on the stack
    strcpy(buffer, input);

    return 0; // ALWAYS returns success (0) — regardless of password
}

int main() {
    char user_name[100];
    char user_pass[100];
    int check = 0;

    puts("********* ADMIN LOGIN PROMPT *********");
    printf("Enter Username: ");
    fgets(user_name, sizeof(user_name), stdin);

    if (verify_user_name(user_name) != 0) {
        puts("nope, incorrect username...");
        return 1;
    }

    printf("Enter Password: ");
    fgets(user_pass, sizeof(user_pass), stdin);

    check = verify_user_pass(user_pass);

    if (check == 0) {
        // This branch is never reached in normal logic
        // because verify_user_pass always returns 0
        puts("nope, incorrect password...");
    } else {
        // This branch is never reached in normal logic
        puts("Access granted!");
        system("/bin/sh");
    }

    return 1;
}