// source.c — reconstructed from level08 disassembly
// Notes:
// - Behavior matches the binary you analyzed.
// - Vulnerability: log_wrapper() uses strcpy() + snprintf() with a fixed
//   0xFE limit, and main builds a *relative* path "./backups/<argv[1]>".
//   Running the program from a writable directory lets you control where
//   that relative path lands (e.g., under /tmp).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// ---------------------------------------------------------------------
// log_wrapper(FILE *log, const char *prefix, const char *arg)
//
// Builds a message = prefix + arg into a stack buffer, strips the first
// newline, then writes "LOG: %s\n" to the given FILE*.
// Buffer is ~0x110 bytes in the binary; we keep it the same size here
// to mirror the layout.
// ---------------------------------------------------------------------
static void log_wrapper(FILE *logf, const char *prefix, const char *arg)
{
    char buf[0x110];          // 272 bytes on stack, like in the binary

    // strcpy(dest, prefix)
    // (exactly what the binary does: no length check on prefix)
    strcpy(buf, prefix);

    // Append arg, but cap the total written size to 0xFE (254) bytes.
    // The assembly computed: avail = 0xFE - strlen(buf)
    size_t len = strlen(buf);
    size_t avail = (len < 0xFE) ? (0xFE - len) : 0;  // bytes left before 0xFE
    if (avail > 0) {
        // snprintf(buf+len, avail, "%s", arg)
        // (snprintf will write at most 'avail-1' chars plus NUL)
        snprintf(buf + len, avail, "%s", arg);
    }

    // Strip newline (strcspn with "\n", then place '\0')
    buf[strcspn(buf, "\n")] = '\0';

    // fprintf(logf, "LOG: %s\n", buf)
    fprintf(logf, "LOG: %s\n", buf);
}

// ---------------------------------------------------------------------
// main
// Usage: ./level08 <filename>
//
// 1) Opens "./backups/.log" for writing and logs "Starting back up: <arg>".
// 2) Opens <filename> for reading.
// 3) Creates/opens the destination "./backups/<filename>" (relative path!)
//    with flags 0xC1 and mode 0660 (as in the binary).
// 4) Copies the file byte-by-byte using write(fd, &buf, 1).
// 5) Logs "Finished back up <arg>", closes handles, returns.
// ---------------------------------------------------------------------
int main(int argc, char **argv)
{
    FILE *logf = NULL;
    FILE *in = NULL;
    char dest[100];            // local buffer used for path in the binary
    unsigned char byte = 0xFF; // matches the single-byte write loop

    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        printf("ERROR: Failed to open (null)\n");
        exit(1);
    }

    // Open ./backups/.log ("w")
    logf = fopen("./backups/.log", "w");
    if (!logf) {
        printf("ERROR: Failed to open %s\n", "./backups/.log");
        exit(1);
    }

    // Log start
    log_wrapper(logf, "Starting back up: ", argv[1]);

    // Open input file (argv[1], "r")
    in = fopen(argv[1], "r");
    if (!in) {
        printf("ERROR: Failed to open %s\n", argv[1]);
        exit(1);
    }

    // Build destination path: "./backups/" + argv[1]
    // Uses strcpy + strncat with 99 - strlen(dest), as per disassembly.
    strcpy(dest, "./backups/");
    strncat(dest, argv[1], 99 - (int)strlen(dest));

    // open(dest, flags=0xC1, mode=0660)
    // 0xC1 == O_WRONLY(1) | O_CREAT(64) | O_NOCTTY(128)
    int fd = open(dest, 0xC1, 0660);
    if (fd < 0) {
        // The binary prints: "ERROR: Failed to open %s%s\n", "./backups/", argv[1]
        printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
        exit(1);
    }

    // Copy loop: fgetc -> write(fd, &byte, 1) until EOF (-1)
    for (;;) {
        int c = fgetc(in);
        if (c == EOF)
            break;
        byte = (unsigned char)c;
        write(fd, &byte, 1);
    }

    // Log finish
    log_wrapper(logf, "Finished back up ", argv[1]);

    // Cleanup
    fclose(in);
    close(fd);

    return 0;
}
