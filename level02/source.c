#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char password[0x29] = {0};
    char username[100] = {0};
    char input_password[100] = {0};
    FILE *fp;

    fp = fopen("/home/users/level02/.pass", "r");
    if (!fp) {
        fwrite("ERROR: failed to open password file\n", 1, 0x24, stderr);
        exit(1);
    }

    int bytes_read = fread(password, 1, 0x28, fp);
    password[strcspn(password, "\n")] = '\0';

    if (bytes_read != 0x29)
    {
        fwrite("ERROR: failed to read password\n", 1, 0x24, stderr);
        fwrite("ERROR: failed to read password\n", 1, 0x24, stderr);
        exit(1);
    }

    fclose(fp);

    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");
    printf("--[ Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("--[ Password: ");
    fgets(input_password, sizeof(input_password), stdin);
    input_password[strcspn(input_password, "\n")] = '\0';

    puts("*****************************************");

    if (strncmp(password, input_password, 0x29) == 0) {
        printf("Greetings, ");
        printf(username); // VULNERABILITY: format string
        printf("!\n");
        system("/bin/sh");
    } else {
        printf(username); // VULNERABILITY: format string again
        puts(" does not have access!");
        exit(1);
    }

    return 0;
}