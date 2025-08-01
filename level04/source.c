#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

void run() {
    char buffer[128];
    printf("Give me some shellcode, k\n");
    gets(buffer); // vulnerable
    puts("child is exiting...");
}

int main() {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // child
        run();
    } else {
        // parent
        wait(&status);
        if (WIFEXITED(status)) {
            if (ptrace(PTRACE_PEEKDATA, pid, NULL, NULL) == -1) {
                puts("no exec() for you");
            }
        }
    }

    return 0;
}