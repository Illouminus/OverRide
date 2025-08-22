// Reconstructed source for level09 (x86_64)
// Based on the disassembly you shared. Function shapes, buffer sizes,
// and the exploitable bug match the binary. Comments point back to
// notable instructions/offsets from the asm.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 0x8c (140) bytes for message text,
// 0x28 (40) bytes for username,
// then a 4-byte length field.
typedef struct s_message {
    char text[140];       // offset 0x00 .. 0x8b
    char username[40];    // offset 0x8c .. 0xb3
    int  len;             // offset 0xb4 .. 0xb7
} t_message;

// 000000000000088c <secret_backdoor>
// Reads a line and passes it to system(). Not referenced by normal flow.
static void secret_backdoor(void) {
    char cmd[0x80]; // 128 bytes, matches sub rsp, 0x80
    if (fgets(cmd, sizeof(cmd), stdin)) {
        // No trimming of newline – identical to asm behavior
        system(cmd);
    }
}

// 0000000000000932 <set_msg>
// Uses m->len to decide how many bytes to copy into text via strncpy.
static void set_msg(t_message *m) {
    char buf[0x400]; // 1024 bytes; stack frame sub rsp, 0x410
    memset(buf, 0, sizeof(buf));

    puts(": Msg @Unix-Dude");              // 0x400bcd
    printf(">>: ");                        // 0x400bdf
    fgets(buf, sizeof(buf), stdin);         // fgets(..., 0x400)

    // mov eax, [rdi + 0xb4] -> m->len, sign-extended to rdx
    // strncpy(dst=text, src=buf, n=m->len)
    strncpy(m->text, buf, m->len);
}

// 00000000000009cd <set_username>
// Copies up to 41 bytes (index 0..0x28 inclusive) from a local buffer
// into m->username starting at offset 0x8c. That single extra byte can
// overwrite the first (most significant) byte of m->len (at 0xb4) — the
// classic 1-byte overflow that fuels the exploit.
static void set_username(t_message *m) {
    char local[0x80]; // 128 bytes
    memset(local, 0, sizeof(local));

    puts(": Enter your username");         // 0x400be4
    printf(">>: ");                        // 0x400bdf reused
    fgets(local, sizeof(local), stdin);     // fgets(..., 0x80)

    // i is compared with 0x28 (40). Loop runs while i <= 40 and until NUL.
    // That allows writing a 41st byte at username[40], which sits at
    // struct offset 0x8c + 40 = 0xb4, the first byte of m->len (little-endian).
    for (int i = 0; i <= 40 && local[i] != '\0'; i++) {
        m->username[i] = local[i];          // byte writes at 0x8c + i
    }

    printf(": Welcome, %s", m->username);  // 0x400bfb
}

// 00000000000008c0 <handle_msg>
// Initializes the struct, sets m.len = 0x8c (140), then calls the two setters.
static void handle_msg(void) {
    t_message m;                             // lives at rbp-0xc0
    // The asm zeroes from &m.username through a handful of qwords,
    // effectively clearing username and len. Clearing all is fine here.
    memset(&m, 0, sizeof(m));

    m.len = 0x8c;                            // mov dword ptr [rbp-0xc], 0x8c

    set_username(&m);                        // copies into m.username (may clobber high byte of len)
    set_msg(&m);                             // strncpy(m.text, buf, m.len)

    puts(">: Msg sent!");                  // 0x400bc0
}

int main(void) {
    puts("--------------------------------------------");
    puts("|   ~Welcome to l33t-m$n ~    v1337        |");
    puts("--------------------------------------------");

    handle_msg();
    return 0;
}

// Notes for auditors:
// - NX is disabled in this challenge binary, but the intended path is a RET
//   overwrite to secret_backdoor() using the enlarged m->len and an overlong
//   message. PIE is disabled, so secret_backdoor has a stable address.
// - The exact output strings may differ slightly; they don’t affect the bug.
// - This source mirrors sizes and offsets: text=140, username=40, len at 0xb4.
//   That layout matches your disassembly and the working exploit payload.
