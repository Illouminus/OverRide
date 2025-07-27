---
sidebar_position: 1
---

# 🛠️ Intro

Welcome to the documentation for the **Override** project — a step-by-step walkthrough of all **9 security levels** from the `OverRide` series at [School 42](https://42.fr).

Each level involves:
- Analyzing a vulnerable binary
- Understanding how the exploit works
- Gaining access to the next user
- Extracting the flag

---

## 📚 How to Use This Documentation

All levels are listed in the **sidebar** on the left.  
Each page provides:

✅ A short description of the challenge  
🧠 An explanation of the vulnerability  
🔓 A clean, minimal step-by-step exploitation  
📸 Screenshots (when needed)  
📁 Source files or scripts (if applicable)

---

## 🧱 What's Inside

- 🧩 Level 00 — Simple password in binary
- 🔐 Level 01 — Username/password logic bypass
- 💣 Level 02 — Buffer overflow (get shell)
- 🕳️ Level 03 — Format string vuln
- 🧼 Level 04 — TOCTOU race condition
- 🧞‍♂️ Level 05 — Environment variable injection
- 🧪 Level 06 — Dangerous system calls
- 🧃 Level 07 — Heap exploitation
- 🧠 Level 08 — Full ASLR & stack protection bypass

---

## 🐳 How to Run This Locally (With Docker)

If you are on a school machine with no `Node.js` installed — don’t worry.

You can launch the full documentation locally using **Docker Compose**.

```bash
docker compose up