#include <stdio.h>
#include <stdlib.h>

void print_byte(unsigned char byte) {
    printf("%d\n", byte >> 7);
	for (int i = 7; i >= 0; i--) {
		printf("%d", (byte >> i) & 1);
	}
	printf(" \n");
}

int main() {
    print_byte(5);

    return 0;
}