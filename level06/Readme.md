# 🧠 OverRide - Level06 Walkthrough

## 🔐 Goal

Exploit the `level06` binary by bypassing its protection and discovering the correct serial number corresponding to a login input, without reversing the full hashing algorithm.

Once the correct serial is found, the program prints:

```
Authenticated!
$ cat /home/users/level07/.pass
```

---

## 🧩 What the Binary Does

* Takes two inputs:

  * A login string (`char*`)
  * A serial number (`unsigned int`)

* The `auth()` function:

  1. Trims newline from the login.
  2. Checks that the login is at least 6 characters long.
  3. Ensures all characters are printable (ASCII > 31).
  4. Calls `ptrace(PTRACE_TRACEME)` — exits if being debugged.
  5. Hashes the login into an integer.
  6. Compares the hash with the serial input.

     * If **equal** → returns `0` → main runs `/bin/sh`.
     * Else → returns `1` → program exits.

---

## ⚠️ Why It’s Tricky

The binary calls `ptrace(PTRACE_TRACEME)` to detect if it’s being debugged. If so, it exits early and shows a “New account detected” message.

To bypass this:

* We place a breakpoint **before** the call to `ptrace`
* And **jump over** the ptrace check when it hits

---

## 🛠️ Exploitation Strategy

Instead of reverse engineering the full hash algorithm, we:

* Run the program in GDB
* Bypass the `ptrace` anti-debug check
* Let the program compute the hash
* Read the resulting hash (expected serial) from the stack
* Restart the binary normally with this serial to pass the check

---

## ✅ Step-by-Step Exploit

### 1. Launch the binary with GDB

```bash
gdb ./level06
```

---

### 2. Set breakpoints

```gdb
b *0x080487b5        # ptrace call in auth()
b *0x08048866        # just before comparison of hash with serial
```

You can confirm these addresses with `disas auth` if needed.

---

### 3. Run the program

```gdb
run
```

The binary will prompt for login and serial. Enter:

```
-> Enter Login: helloworld
-> Enter Serial: 0
```

The serial doesn’t matter — we’ll extract the correct one in a moment.

---

### 4. Bypass ptrace

When the first breakpoint (at `0x080487b5`) hits:

```gdb
jump *0x080487ed
```

📌 This jumps over the ptrace call and its check, allowing the program to continue.

---

### 5. Wait for second breakpoint

When the second breakpoint (at `0x08048866`) hits, the hash has been calculated and stored.

Now print the computed hash:

```gdb
p *(int*)($ebp - 0x10)
```

📌 This reads the value from the local variable that holds the computed hash.
Example result:

```
$1 = 106020184
```

---

### 6. Exit GDB

```gdb
quit
```

---

### 7. Run the binary normally with the correct serial

```bash
./level06
-> Enter Login: helloworld
-> Enter Serial: 106020184
```

✅ You should see:

```
Authenticated!
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

---

## 🧠 Explanation: Why This Works

* The `auth()` function calculates a hash of the login.
* This hash is compared to your serial input.
* Instead of replicating the hash logic, we let the program do the work.
* Then we "steal" the result from the stack using GDB.

---

## 🔎 Bonus: How to Find Key Offsets (if changed)

To confirm the ptrace or comparison offset if you're unsure:

```gdb
disas auth
```

Look for:

```asm
0x080487b5: call   0x80485f0 <ptrace@plt>
...
0x08048866: cmp    -0x10(%ebp), %eax
```

Use those to set breakpoints accordingly.

---

## 📌 Summary of Commands

```bash
gdb ./level06
(gdb) b *0x080487b5
(gdb) b *0x08048866
(gdb) run
# Login: helloworld
# Serial: 0
(gdb) jump *0x080487ed
(gdb) p *(int*)($ebp - 0x10)
(gdb) quit
./level06
# Login: helloworld
# Serial: <value printed above>
```

---

## 🏁 Congrats

You’ve successfully bypassed anti-debugging and recovered the required serial to pass the check without reverse engineering the full algorithm.

```
cat /home/users/level07/.pass
```
