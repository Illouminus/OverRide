# Level08 – Exploit Guide

## 1. Understanding the Program

When you run the binary:

```bash
./level08 filename
```

It expects a **file name** as an argument.

Inside the code (simplified C pseudocode):

```c
FILE *log = fopen("./backups/.log", "w");
fprintf(log, "Starting back up: %s\n", argv[1]);

FILE *source = fopen(argv[1], "r");

char dest[100];
strcpy(dest, "./backups/");
strncat(dest, argv[1], 99 - strlen(dest));

int fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0640);

while ((c = fgetc(source)) != EOF) {
    write(fd, &c, 1);
}

fprintf(log, "Finished back up: %s\n", argv[1]);
```

### Key points:

* It **reads** the file given in `argv[1]` with the program’s **setuid** privileges (it runs as `level09`).
* It **writes** to `"./backups/" + argv[1]` — this is a **relative path** based on your **current working directory (CWD)**.
* There is **no path sanitization** — your argument is used directly in both the read and write path.
* Because the destination is relative, we can control **where** it writes the copied file by changing CWD and creating the right folder structure.

---

## 2. The Vulnerability

* **Source file path**: taken directly from `argv[1]` → can be an **absolute path** like `/home/users/level09/.pass`.
  Since the binary is setuid, it can read files that you normally cannot.

* **Destination file path**: `"./backups/" + argv[1]` is **relative** to CWD.
  Even if `argv[1]` starts with `/`, that slash is inside the string, not at the start of the full path — so the OS treats it as a subdirectory of `./backups`.

Example:
If you run:

```bash
./level08 /home/users/level09/.pass
```

The program will try to write to:

```
./backups//home/users/level09/.pass
```

relative to your **current directory**.

---

## 3. The Exploit Plan

1. Choose a writable directory (e.g., `/tmp`).
2. Create the destination folder tree `./backups/home/users/level09/` **inside /tmp**.
3. Run `level08` from `/tmp`, giving `/home/users/level09/.pass` as the source.
4. The binary will read the secret file with its setuid privileges and write it into your `/tmp/backups/...` path.
5. Read the copied file — now you have the password.

---

## 4. Step-by-Step Commands

```bash
# Step 1: Go to a writable directory
cd /tmp

# Step 2: Create the folder structure for the destination
mkdir -p ./backups/home/users/level09/

# Step 3: Run level08 from here, asking it to copy the secret file
~/level08 /home/users/level09/.pass

# Step 4: Read the file we just got
cat ./backups/home/users/level09/.pass
```

---

## 5. Why It Works

* The **read path** is absolute → unaffected by your CWD, so the binary can read the real `/home/users/level09/.pass`.
* The **write path** is relative → starts from your current directory, so you can make it point anywhere you have permissions.
* Since we pre-create the folders under `/tmp`, the binary can successfully write there.
* This copies the contents of a restricted file into a location you can read.

---

## 6. Important Notes

* If you try to run `level08` from your home directory without making the right `./backups/...` structure there, it will fail — you don’t have permissions to create those directories under `/home/users/...`.
* That’s why we use `/tmp`: it’s world-writable.
* This is not a memory corruption exploit — it’s **path manipulation** and **privilege abuse** via setuid.

---

**Result**:
You get the `level09` password in `/tmp/backups/home/users/level09/.pass`.
