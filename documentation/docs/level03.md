---
sidebar_position: 5
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🔐 Level 03

<MissionObjective 
  level="Level 03"
  target="level04 shell access"
  method="XOR decryption and reverse engineering"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Weak Cryptography & Logic Flaw"
  severity="medium"
  description="The binary uses simple XOR encryption with a predictable key derived from user input. The decryption logic can be reversed through static analysis to find the correct password."
  techniques={[
    "Static Analysis",
    "XOR Reversal", 
    "Assembly Inspection"
  ]}
/>

## Initial Program Analysis

<CodeBlock 
  title="Program Execution"
  command="./level03"
  output={`***********************************
*           level03              **
***********************************
Password:
[Enter anything]
Invalid Password`}
/>

**Key Observations:**
- Program expects a numeric password
- Shows "Invalid Password" for wrong input
- No obvious brute-force opportunity

## Binary Analysis with GDB

<CodeBlock 
  title="Main Function Disassembly"
  command="gdb ./level03"
  output={`(gdb) disas main
...
movl $0x8048a7b, %eax        ; printf("Password:")
scanf("%d", &input)          ; user input -> [esp+0x1c]
movl $322424845, 0x4(%esp)   ; hardcoded constant
mov input, (%esp)
call test                    ; test(input, 322424845)
...`}
/>

**Key Findings:**
- Program calls `test(input, 322424845)` with user input and hardcoded constant
- The magic number `322424845` is crucial for the algorithm

## Test Function Analysis

<CodeBlock 
  title="Test Function Logic"
  command="(gdb) disas test"
  output={`mov param2, %eax        ; eax = 322424845
sub param1, %eax        ; eax = 322424845 - input
cmp eax, 21             ; if difference <= 21, continue
...
call decrypt            ; decrypt(322424845 - input)`}
/>

**Algorithm Logic:**
```c
delta = 322424845 - input
if (delta <= 21) {
    decrypt(delta);
}
```

This means our input must be in range: `322424824` to `322424845`

## Decrypt Function Reverse Engineering

<CodeBlock 
  title="Encrypted Data in Memory"
  command="(gdb) disas decrypt"
  output={`movl $0x757c7d51,-0x1d(%ebp)   ; Encrypted bytes
movl $0x67667360,-0x19(%ebp)
movl $0x7b66737e,-0x15(%ebp)
movl $0x33617c7d,-0x11(%ebp)`}
/>


## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/blO98TyOPnHSV469YafAMkCjy.js" 
  id="asciicast-blO98TyOPnHSV469YafAMkCjy" 
/>



### XOR Decryption Analysis

<StepsList
  title="Reversing the XOR Encryption"
  steps={[
    {
      title: "Extract Encrypted Bytes",
      description: "Convert little-endian hex values to string",
      command: "echo -n 517d7c75 60736667 7e73667b 7d7c6133 | xxd -r -p",
      result: "Q}|u`sfg~sf{}|a3"
    },
    {
      title: "Identify Target String", 
      description: "The program compares decrypted result to known string",
      command: "# Target string from binary analysis",
      result: "Congratulations!"
    },
    {
      title: "Calculate XOR Key",
      description: "Find what XOR key transforms encrypted to target",
      command: "# First character: 'Q' ^ x = 'C'",
      result: "x = 'Q' ^ 'C' = 0x51 ^ 0x43 = 0x12 (18)"
    },
    {
      title: "Solve for Input",
      description: "Work backwards from XOR key to input",
      command: "# 322424845 - input = 18",
      result: "input = 322424827"
    }
  ]}
/>

## XOR Reversal Mathematics

<CodeBlock 
  title="XOR Key Calculation"
  command="# Manual XOR reversal"
  output={`Encrypted: Q}|u\`sfg~sf{}|a3
Target:    Congratulations!

First char: 'Q' (0x51) ^ key = 'C' (0x43)
Key = 0x51 ^ 0x43 = 0x12 = 18

Verification with other characters:
'}' (0x7D) ^ 18 = 'o' ✓
'|' (0x7C) ^ 18 = 'n' ✓
'u' (0x75) ^ 18 = 'g' ✓`}
/>

## Final Exploitation

<CodeBlock 
  title="Successful Password Entry"
  command="./level03"
  output={`***********************************
*           level03              **
***********************************
Password: 322424827
$ whoami
level04
$ cat /home/users/level04/.pass
[REDACTED]`}
/>

## Key Learning Points

:::tip **Cryptographic Weaknesses**
- **Weak XOR Implementation**: Using a single-byte key makes the cipher trivial to break
- **Predictable Key Derivation**: The XOR key is directly derived from user input
- **Static Analysis Vulnerability**: All encryption constants are visible in the binary
:::

:::warning **Anti-Analysis Notes**
- Program may fail when run through pipes or redirects due to stdin handling
- Some anti-debugging measures may interfere with dynamic analysis
- Manual input required - automated brute force approaches may not work
:::

## Technical Summary

This level demonstrates:
1. **Static Binary Analysis** - Extracting hardcoded constants and logic flow
2. **XOR Cryptanalysis** - Reversing simple XOR encryption when plaintext is known
3. **Mathematical Problem Solving** - Working backwards from desired output to required input
4. **Assembly Code Reading** - Understanding x86 instructions and function calling conventions

The vulnerability lies in using a weak XOR cipher with a user-controlled key, combined with the ability to reverse-engineer the