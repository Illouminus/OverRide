#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int data[100];

void clear_stdin() {
    char c = 0;
    while (c != '\n' && c != EOF)
        c = getchar();
}

unsigned int get_unum() {
    unsigned int num = 0;
    fflush(stdout);
    scanf("%u", &num);
    clear_stdin();
    return num;
}

int store_number(unsigned int *data) {
    unsigned int number, index;

    printf(" Number: ");
    number = get_unum();

    printf(" Index: ");
    index = get_unum();

    // Ограничение: нельзя писать в кратные 3 индексы
    if ((index % 3) == 0 || (index >> 24) == 0xb7) {
        puts("This index is reserved for wil!");
        return 1;
    }

    data[index] = number;

    return 0;
}

int read_number(unsigned int *data) {
    unsigned int index;

    printf(" Index: ");
    index = get_unum();

    printf(" Number at data[%u] is %u\n", index, data[index]);
    return 0;
}

int main() {
    char buffer[20];
    memset(data, 0, sizeof(data));

    puts("----------------------------------------------------");
    puts("  Welcome to wil's crappy number storage service!   ");
    puts("----------------------------------------------------");
    puts(" Commands:                                          ");
    puts("    store - store a number into the data storage    ");
    puts("    read  - read a number from the data storage     ");
    puts("    quit  - exit the program                        ");
    puts("----------------------------------------------------");
    puts("   wil has reserved some storage :>                 ");
    puts("----------------------------------------------------");

    while (1) {
        printf("\nInput command: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (strncmp(buffer, "store", 5) == 0)
            store_number(data);
        else if (strncmp(buffer, "read", 4) == 0)
            read_number(data);
        else if (strncmp(buffer, "quit", 4) == 0)
            break;
        else
            puts(" Failed to do em command");
    }

    return 0;
}
