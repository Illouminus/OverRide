#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>

int auth(char *login, unsigned int serial) {
    login[strcspn(login, "\n")] = 0; // remove newline

    int len = strnlen(login, 32);
    if (len <= 5)
        return 1;

    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        puts(".---------------------------.");
        puts("| !! TAMPERING DETECTED !!  |");
        puts("'---------------------------'");
        return 1;
    }

    int hash = ((login[3] ^ 0x1337) + 0x5eeded);

    for (int i = 0; i < len; i++) {
        if (login[i] <= 31)
            return 1;

        int c = login[i] ^ hash;
        int tmp = c;
        tmp = tmp - ((tmp * 0x88233b2b) >> 10);
        tmp *= 0x539;
        hash += (c - tmp);
    }

    return serial != hash;
}

int main(void) {
    char login[32];
    unsigned int serial;

    puts("***********************************");
    puts("*\t\tlevel06\t  \t  *");
    puts("***********************************");

    printf("-> Enter Login: ");
    fgets(login, 32, stdin);

    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");

    printf("-> Enter Serial: ");
    scanf("%u", &serial);

    if (!auth(login, serial)) {
        puts("Authenticated!");
        system("/bin/sh");
    } else {
        puts("Authentication failed.");
    }

    return 0;
}