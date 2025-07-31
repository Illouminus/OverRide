# OverRide - Level03 Walkthrough

## 🧠 Goal
Get a shell as `level04` by exploiting the `level03` binary.

---

## 📂 Step 1: Understand the Binary

```bash
$ ./level03
***********************************
*           level03              **
***********************************
Password:
Invalid Password

The program asks for a password (a number). Let’s analyze it with gdb:

$ gdb ./level03
(gdb) disas main

We see this logic:

movl $0x8048a7b, %eax        ; printf("Password:")
scanf("%d", &input)          ; user enters number -> stored at [esp+0x1c]
movl $322424845, 0x4(%esp)   ; hardcoded number
mov input, (%esp)
call test

So the program calls test(input, 322424845).

⸻

## 🔍 Step 2: Analyzing test

Inside test, we see:

mov param2, %eax        ; eax = 322424845
sub param1, %eax        ; eax = 322424845 - input
cmp eax, 21             ; if difference <= 21, go to decrypt

That means only if (322424845 - input) <= 21, the function decrypt is called.

Let’s call this difference delta:

delta = 322424845 - input

If delta <= 21, we continue.

So:

if ((322424845 - input) <= 21)
    decrypt(322424845 - input);


⸻

## 🔐 Step 3: Reverse decrypt

In decrypt, we see a buffer being XOR’ed with the input param:

movl $0x757c7d51,-0x1d(%ebp)
movl $0x67667360,-0x19(%ebp)
movl $0x7b66737e,-0x15(%ebp)
movl $0x33617c7d,-0x11(%ebp)

This is the encrypted string, byte by byte. Let’s concatenate it:

echo -n 517d7c75 60736667 7e73667b 7d7c6133 | xxd -r -p

Output:

Q}|u`sfg~sf{}|a3

This is the XOR-encoded string. The decrypt function then does:

for (i = 0; i < strlen(buffer); i++)
    buffer[i] ^= param;

And compares it to:

"Congratulations!"

So we can reverse the XOR ourselves to find what param gives the correct result.

Take the first character:

'Q' ^ x = 'C' → x = 'Q' ^ 'C' = 0x51 ^ 0x43 = 0x12 (18)

✅ Bingo!

So, the correct param = 18, and we know:

322424845 - input = 18 → input = 322424827


⸻

✅ Final Exploit

$ ./level03
Password: 322424827
$ whoami
level04


⸻

💬 Notes
	•	The program fails if you try to run it via echo $num | ./level03 or here-docs due to how scanf works.
	•	You must manually input the password or use tools that simulate stdin properly.
	•	Trying to use GDB run crashes the program because of missing file descriptors or anti-debug protection.

So brute-force with a loop like this will not work:

for ((i = 0; i <= 21; i++)); do
  echo $((322424845 - i)) | ./level03
done

Instead, we manually deduce the correct password from the XOR logic.

⸻

🏁 Conclusion

This level teaches you to:
	•	Analyze parameters passed between functions
	•	Read XOR-based obfuscation in memory
	•	Deduce values by reversing logic

