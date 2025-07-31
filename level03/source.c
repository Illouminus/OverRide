#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decrypt(int param) {
	char buffer[] = "Q}|u`sfg~sf{}|a3"; // Encrypted string
    // The string is encrypted using XOR with a key
	int len = strlen(buffer);

	for (int i = 0; i < len; i++) {
		buffer[i] ^= param; // XOR decryption
	}

	if (strcmp(buffer, "Congratulations!") == 0) {
		system("/bin/sh");
	} else {
		puts("Invalid Password");
	}
}

void test(int input, int magic) {
	if ((magic - input) <= 21) {
		decrypt(magic - input);
	} else {
		puts("Invalid Password");
	}
}

int main() {
	int input;

	puts("***********************************");
	puts("*		level03		**");
	puts("***********************************");
	printf("Password:");

	scanf("%d", &input);

	test(input, 322424845);

	return 0;
}