---
sidebar_position: 8
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🧪 Level 06

<MissionObjective 
  level="Level 06"
  target="level07 privileges"
  method="Anti-debugging bypass and hash extraction"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Anti-Debug Bypass & Logic Exploitation"
  severity="medium"
  description="Binary uses ptrace() anti-debugging protection and hash-based authentication. Instead of reversing the hash algorithm, we bypass ptrace and extract the computed hash directly from memory."
  techniques={[
    "Anti-Debug Bypass",
    "Memory Extraction", 
    "GDB Jump Exploitation",
    "Hash Recovery"
  ]}
/>

## Program Behavior Analysis

<CodeBlock 
  title="Binary Authentication Flow"
  command="./level06"
  output={`-> Enter Login: test
-> Enter Serial: 123
Nope.`}
/>

**Authentication Logic:**
1. **Input Validation**: Login must be ≥6 chars, all printable
2. **Anti-Debug Check**: `ptrace(PTRACE_TRACEME)` detects debugger
3. **Hash Calculation**: Login string → integer hash
4. **Serial Comparison**: Hash vs user-provided serial
5. **Shell Access**: If match → `/bin/sh`, else exit

## Anti-Debugging Protection

<CodeBlock 
  title="Ptrace Detection Mechanism"
  command="gdb ./level06"
  output={`(gdb) run
-> Enter Login: test
-> Enter Serial: 123
Starting program: /home/level06/level06 
New account detected
[Inferior 1 (process 1234) exited normally]`}
/>

**Protection Mechanism:**
- `ptrace(PTRACE_TRACEME)` returns -1 if already being traced
- Program detects GDB and exits with "New account detected"
- Must bypass this check to analyze the hash algorithm

## Strategy: Hash Extraction Without Reversing

<CodeBlock 
  title="Exploitation Approach"
  command="# Instead of reverse engineering hash algorithm"
  output={`1. Use GDB to bypass ptrace() check
2. Let program calculate hash naturally
3. Extract computed hash from memory
4. Use extracted hash as correct serial
5. Run program normally with correct credentials`}
/>


## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/FWDCXErHfqnV2PGcAOl6dtJRS.js" 
  id="asciicast-FWDCXErHfqnV2PGcAOl6dtJRS" 
/>


## Step-by-Step Exploitation

<StepsList
  title="Anti-Debug Bypass and Hash Extraction"
  steps={[
    {
      title: "Setup GDB Breakpoints",
      description: "Place strategic breakpoints around ptrace and comparison",
      command: "b *0x080487b5  # ptrace call\nb *0x08048866  # hash comparison",
      result: "Breakpoints set for anti-debug bypass"
    },
    {
      title: "Initial Program Run",
      description: "Start program with any login/serial combination",
      command: "run\nLogin: helloworld\nSerial: 0",
      result: "Program hits first breakpoint at ptrace"
    },
    {
      title: "Bypass Ptrace Check",
      description: "Jump over the ptrace call and its validation",
      command: "jump *0x080487ed",
      result: "Anti-debug protection bypassed"
    },
    {
      title: "Extract Computed Hash",
      description: "Read hash value from stack at comparison point",
      command: "p *(int*)($ebp - 0x10)",
      result: "Hash value: 106020184"
    }
  ]}
/>

### Detailed GDB Session

<CodeBlock 
  title="Complete GDB Bypass Session"
  command="gdb ./level06"
  output={`(gdb) b *0x080487b5
Breakpoint 1 at 0x80487b5
(gdb) b *0x08048866  
Breakpoint 2 at 0x8048866
(gdb) run
Starting program: ./level06
-> Enter Login: helloworld
-> Enter Serial: 0

Breakpoint 1, 0x080487b5 in auth ()
(gdb) jump *0x080487ed
Continuing at 0x80487ed.

Breakpoint 2, 0x08048866 in auth ()
(gdb) p *(int*)($ebp - 0x10)
$1 = 106020184
(gdb) quit`}
/>

## Assembly Analysis

<CodeBlock 
  title="Key Assembly Points"
  command="(gdb) disas auth"
  output={`0x080487b5: call   0x80485f0 <ptrace@plt>    ; Anti-debug check
0x080487ba: test   %eax,%eax                    ; Check ptrace return
0x080487bc: jns    0x80487ed                    ; Jump if not traced
...
0x08048866: cmp    -0x10(%ebp),%eax            ; Compare hash with serial
0x08048869: je     0x8048872                    ; Jump if equal (success)`}
/>

**Critical Memory Layout:**
- `%ebp - 0x10`: Computed hash storage location
- `%eax`: User-provided serial for comparison
- Jump targets for bypass and success conditions

## Final Exploitation

<CodeBlock 
  title="Successful Authentication"
  command="./level06"
  output={`-> Enter Login: helloworld
-> Enter Serial: 106020184
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8`}
/>

## Alternative Hash Discovery

<CodeBlock 
  title="Quick Reference Commands"
  command="# Complete exploit in one session"
  output={`gdb ./level06
(gdb) b *0x080487b5
(gdb) b *0x08048866
(gdb) run
# Login: helloworld, Serial: 0
(gdb) jump *0x080487ed
(gdb) p *(int*)($ebp - 0x10)
# Note the hash value
(gdb) quit
./level06
# Login: helloworld, Serial: <extracted_hash>`}
/>

## Technical Deep Dive

### Ptrace Anti-Debug Analysis

:::tip **How Ptrace Protection Works**
- `ptrace(PTRACE_TRACEME)` attaches process to its parent as debugger
- Returns `-1` if process is already being traced (like by GDB)
- Program checks return value and exits if debugging detected
- Bypass: Jump over the ptrace call entirely
:::

### Hash Algorithm Insights

<CodeBlock 
  title="Hash Function Characteristics"
  command="# Different logins produce different hashes"
  output={`Login: "hello"     → Hash: 2134567890
Login: "helloworld" → Hash: 106020184
Login: "admin"      → Hash: 3456789012

Each login produces deterministic hash
Algorithm remains opaque but predictable`}
/>

## Debugging Offset Discovery

<CodeBlock 
  title="Finding Key Assembly Offsets"
  command="(gdb) disas auth"
  output={`# Look for these patterns:
call   <ptrace@plt>     ; Breakpoint 1 location
test   %eax,%eax        ; Ptrace return check
jns    <address>        ; Jump target for bypass
...
cmp    -0x10(%ebp),%eax ; Breakpoint 2 location`}
/>

## Key Learning Points

:::warning **Anti-Analysis Techniques**
- **Ptrace Detection**: Common anti-debugging protection
- **Dynamic Analysis Bypass**: Jump instruction to skip checks
- **Memory Inspection**: Reading computed values without algorithm reversal
- **Staged Exploitation**: Use debugger info in normal execution
:::

## Alternative Approaches

<CodeBlock 
  title="Other Potential Methods"
  command="# If GDB bypass fails"
  output={`1. Binary patching: NOP out ptrace call
2. LD_PRELOAD: Hook ptrace to return success
3. Static analysis: Reverse hash algorithm completely
4. Timing attacks: Brute force with performance analysis`}
/>

## Technical Summary

This level demonstrates:
1. **Anti-Debug Evasion** - Bypassing ptrace protection mechanisms
2. **Dynamic Analysis Techniques** - Using debuggers despite protection
3. **Memory Extraction** - Reading computed values from stack
4. **Staged Attack Strategy** - Combining debug info with normal execution
5. **Creative Problem Solving** - Avoiding complex reverse engineering

The vulnerability showcases how anti-debugging measures can be circumvented through careful debugger manipulation, and how sometimes the simplest approach (letting the program do the work) is more effective than complex reverse