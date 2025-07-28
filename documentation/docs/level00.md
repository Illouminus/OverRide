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
  target="flag00"
  method="a vulnerable setuid binary"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Hardcoded Password Vulnerability"
  severity="high"
  description="The target binary contains hardcoded credentials that can be discovered through static analysis. This represents a critical security flaw where sensitive authentication data is embedded directly in the executable code."
  techniques={[
    "Static Analysis",
    "String Extraction", 
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

<CodeBlock 
  title="User Information"
  command="cat /etc/passwd | grep -E 'flag00|level00|level01'"
  output={`level00:x:1000:1000:level00,,,:/home/level00:/bin/bash
level01:x:1001:1001:level01,,,:/home/level01:/bin/bash
flag00:x:3000:3000:flag00,,,:/home/flag00:/bin/bash`}
/>

## Binary Analysis

<CodeBlock 
  title="String Analysis"
  command="strings ./level00"
  output={`/lib64/ld-linux-x86-64.so.2
libc.so.6
puts
printf
getchar
strcmp
__libc_start_main
__gmon_start__
GLIBC_2.2.5
UH-H
AWAVA
AUATL
[]A\A]A^A_
***********************************
*            -Level00 -           *
***********************************
Username: %s
Password:
Nope.
woupa2yuojokd
/bin/sh
;*3$"`}
/>

**Critical Discovery:** Found hardcoded password `woupa2yuojokd` in the binary!

## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/wM9Vo07lS9fiblYICSvSNpxrQ.js" 
  id="asciicast-wM9Vo07lS9fiblYICSvSNpxrQ" 
/>

## Step-by-Step Solution

<StepsList steps={[
  {
    title: "Launch the binary",
    description: "Execute the setuid binary to start the authentication process.",
    command: "./level00",
    output: `***********************************
*            -Level00 -           *
***********************************
Username: `
  },
  {
    title: "Enter username",
    description: "Provide the target username we want to escalate to.",
    command: "flag00",
    output: "Password: "
  },
  {
    title: "Use discovered password",
    description: "Enter the hardcoded password we found in the binary strings.",
    command: "woupa2yuojokd",
    output: "$ "
  },
  {
    title: "Verify access",
    description: "Confirm we have successfully escalated privileges.",
    command: "whoami",
    output: "flag00"
  },
  {
    title: "Find the flag",
    description: "Navigate to the flag directory and retrieve the flag.",
    command: "cd /home/flag00 && cat .flag",
    output: "uvar42khalfholfkek"
  }
]} />

## Key Takeaways

- **Never hardcode credentials** in application binaries
- **Static analysis** can reveal sensitive information in compiled code  
- **SUID binaries** are high-value targets for privilege escalation
- **String extraction** is a fundamental reverse engineering technique

**Tools used:** `strings`, `ls`, `grep`, static analysis
