---
sidebar_position: 2
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🧩 Level 00

<MissionObjective 
  level="Level 00"
  target="level01 privileges"
  method="Reverse engineering a setuid binary"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Weak Authentication & Privilege Escalation"
  severity="high"
  description="The binary uses a simple numeric comparison for authentication and calls system('/bin/sh') upon success. Combined with setuid permissions, this allows privilege escalation to the level01 user."
  techniques={[
    "GDB Analysis",
    "Assembly Disassembly", 
    "SUID Exploitation"
  ]}
/>

## Reconnaissance

Let's start by gathering information about our target environment.

<CodeBlock 
  title="File System Analysis"
  command="ls -la"
  output={`total 17
dr-xr-x---+ 1 level00 level00   120 Sep 10  2016 .
dr-x--x--x  1 root    root      340 Sep 23  2015 ..
-rw-r--r--  1 level00 level00   220 Sep 10  2016 .bash_logout
-rw-r--r--  1 level00 level00  3489 Sep 10  2016 .bashrc
-rw-r--r--  1 level00 level00   675 Sep 10  2016 .profile
-rwsr-x---+ 1 level01 users    7280 Sep 10  2016 level00`}
/>

**Key Observations:**
- Found a setuid binary `level00` owned by `level01`
- The `s` bit indicates elevated privileges when executed


## Binary Analysis

<CodeBlock 
  title="File Information"
  command="file level00"
  output="level00: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked, for GNU/Linux 2.6.24, BuildID[sha1]=d2d5ca9c99d46f5b1f7a3c4c8542c9f6e4b9f2d5, not stripped"
/>

<CodeBlock 
  title="Permissions Check"
  command="ls -l level00"
  output="-rwsr-s---+ 1 level01 users 7280 Sep 10  2016 level00"
/>

**Key Observations:**
- Binary is owned by `level01` and has the setuid bit set
- When executed, it runs with `level01` privileges
- 32-bit ELF binary, not stripped (good for analysis)

## Reverse Engineering with GDB

<CodeBlock 
  title="GDB Disassembly"
  command="gdb level00"
  output={`(gdb) disas main
Dump of assembler code for function main:
   0x08048494 <+0>:     push   %ebp
   0x08048495 <+1>:     mov    %esp,%ebp
   ...
   0x080484e7 <+83>:    cmp    $0x149c,%eax
   0x080484ec <+88>:    jne    0x80484f8 <main+100>
   0x080484ee <+90>:    movl   $0x80485e0,(%esp)
   0x080484f5 <+97>:    call   0x8048390 <system@plt>
   ...`}
/>

**Critical Discovery:** The program compares input with `0x149c` (5276 in decimal)!

<CodeBlock 
  title="Convert Hex to Decimal"
  command="python3 -c 'print(0x149c)'"
  output="5276"
/>

## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/bXLfMcqqNwKsw1SUErGhrgU3z.js" 
  id="asciicast-bXLfMcqqNwKsw1SUErGhrgU3z" 
/>

## Step-by-Step Solution

<StepsList steps={[
  {
    title: "Launch the binary",
    description: "Execute the setuid binary to start the authentication process.",
    command: "./level00",
    output: "Password: "
  },
  {
    title: "Enter the discovered password",
    description: "Use the numeric password found through GDB analysis.",
    command: "5276",
    output: "Authenticated!\n$ "
  },
  {
    title: "Verify elevated privileges",
    description: "Check that we now have level01 effective privileges.",
    command: "whoami && id",
    output: "level00\nuid=1000(level00) gid=1000(level00) euid=1001(level01) egid=100(users)"
  },
  {
    title: "Access the password file",
    description: "Use full path to cat to ensure it runs with elevated privileges.",
    command: "/bin/cat /home/users/level01/.pass",
    output: "uvar42khalfholfkek"
  },
  {
    title: "Alternative: Get shell access",
    description: "The binary calls system('/bin/sh') which gives a shell with level01 privileges.",
    command: "# After entering 5276, you get a shell",
    output: "$ # Now you have level01 effective UID"
  }
]} />

## Key Takeaways

- **GDB disassembly** reveals program logic and comparison values
- **Setuid binaries** run with the owner's privileges (level01 in this case)
- **Simple numeric passwords** are easily discovered through reverse engineering
- **system() calls** in setuid programs can provide shell access with elevated privileges
- **Full paths** to commands (like `/bin/cat`) ensure proper privilege inheritance

**Tools used:** `gdb`, `file`, `ls`, assembly analysis, privilege escalation

:::warning Important Note
Using `cat` alone might fail because the real UID is still level00, while the effective UID is level01. The `.pass` file is only readable by level01. Using `/bin/cat` with the full path ensures the command executes with the inherited effective UID.
:::
