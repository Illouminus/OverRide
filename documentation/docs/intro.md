---
sidebar_position: 1
---

import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🛡️ OverRide Documentation

<MissionObjective 
  level="OverRide Series"
  target="9 Security Challenges"
  method="Binary Exploitation & Reverse Engineering"
/>

Welcome to the comprehensive documentation for the **OverRide** project — a complete walkthrough of all **9 security levels** from the prestigious [School 42](https://42.fr) cybersecurity curriculum.

## What You'll Learn

<div style={{display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))', gap: '1rem', margin: '2rem 0'}}>
  <VulnerabilityCard 
    type="Static Analysis"
    severity="beginner"
    description="Learn to extract hardcoded credentials and analyze binary strings to find authentication bypasses."
    techniques={["strings", "objdump", "hexdump"]}
  />
  
  <VulnerabilityCard 
    type="Buffer Overflow"
    severity="medium"
    description="Master stack-based buffer overflows to redirect program execution and gain shell access."
    techniques={["gdb", "pattern_create", "shellcode"]}
  />
  
  <VulnerabilityCard 
    type="Format String"
    severity="high"
    description="Exploit format string vulnerabilities to read/write arbitrary memory locations."
    techniques={["printf exploitation", "GOT overwrite", "stack reading"]}
  />
  
  <VulnerabilityCard 
    type="Advanced Exploitation"
    severity="expert"
    description="Bypass modern protections like ASLR, stack canaries, and heap exploitation techniques."
    techniques={["ROP chains", "heap overflow", "ASLR bypass"]}
  />
</div>

## Level Overview

<StepsList steps={[
  {
    title: "🧩 Level 00 - Weak Authentication",
    description: "Simple numeric password through GDB analysis",
    command: "gdb level00 && disas main",
    output: "Reverse engineering fundamentals"
  },
  {
    title: "🔐 Level 01 - Buffer Overflow", 
    description: "Stack-based buffer overflow with return address overwrite",
    command: "fgets() overflow + ROP chain",
    output: "Classic buffer overflow exploitation"
  },
  {
    title: "💣 Level 02 - Format String",
    description: "Memory disclosure through printf format string vulnerability",
    command: "printf(buffer) + %p stack scanning",
    output: "Password extraction from memory"
  },
  {
    title: "🕳️ Level 03 - Format String",
    description: "Format string vulnerability exploitation",
    command: "printf format specifiers abuse",
    output: "Arbitrary read/write primitives"
  },
  {
    title: "🧼 Level 04 - Race Condition",
    description: "Time-of-check to time-of-use (TOCTOU) exploitation",
    command: "Timing attack implementation", 
    output: "File system race condition"
  },
  {
    title: "🧞‍♂️ Level 05 - Environment Injection",
    description: "Environment variable manipulation attacks",
    command: "PATH hijacking & LD_PRELOAD",
    output: "Privilege escalation via env vars"
  },
  {
    title: "🧪 Level 06 - System Call Abuse",
    description: "Dangerous system call exploitation",
    command: "syscall analysis & abuse",
    output: "System-level privilege escalation"
  },
  {
    title: "🧃 Level 07 - Heap Exploitation", 
    description: "Heap-based memory corruption attacks",
    command: "Heap layout manipulation",
    output: "Dynamic memory exploitation"
  },
  {
    title: "🧠 Level 08 - Full Protection Bypass",
    description: "Advanced techniques against modern protections",
    command: "ASLR/Stack canary bypass",
    output: "Complete system compromise"
  }
]} />

## Quick Start Guide

<CodeBlock 
  title="Launch Documentation Locally"
  command="docker-compose up -d"
  output={`[+] Running 2/2
 ✓ Network override-network          Created
 ✓ Container override-documentation  Started

🌐 Documentation available at: http://localhost:3000`}
/>

Each level provides:
- **🎯 Mission Objective** - Clear goals and targets
- **🔍 Vulnerability Analysis** - Technical breakdown of security flaws  
- **🛠️ Step-by-Step Exploitation** - Detailed attack methodology
- **💡 Key Takeaways** - Security lessons and prevention techniques
- **🎬 Live Demonstrations** - Interactive terminal recordings

---

:::tip Ready to Begin?
Start with [**Level 00**](./level00) to learn the fundamentals of binary analysis and work your way up to advanced exploitation techniques.
:::

:::info Prerequisites
Basic knowledge of C programming, Linux command line, and x86 assembly is recommended but not required. Each level builds upon the previous concepts.
:::
