# Level00

## Summary

This level introduces a simple password-protected binary that is owned by `level01` and has the `setuid` bit set. If the correct password is entered, the program calls `system("/bin/sh")` and gives a shell with effective UID = level01.

## Steps

### 1. Analyze the binary

```bash
$ file level00
→ ELF 32-bit, setuid, not stripped

$ ls -l level00
→ -rwsr-s---+ 1 level01 users ...

```

This shows the binary is owned by level01 and runs with its privileges.


2. Reverse with GDB

Using GDB to disassemble main:

```bash

0x080484e7 <+83>: cmp $0x149c, %eax  ; compare input

0x149c = 5276

```

The program compares your input with 5276. If it matches, it calls:

```bash

system("/bin/sh")

```

3. Exploitation

Simply run:

```bash

$ ./level00
Password: 5276
Authenticated!
$ /bin/cat /home/users/level01/.pass

```

Note: cat alone might fail because real UID = level00, and .pass is readable only by level01.
Using /bin/cat ensures the command is executed with inherited euid = level01.