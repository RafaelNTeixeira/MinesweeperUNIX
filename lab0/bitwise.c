#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BIT(n) (1<<(n))

char *byte2bin(uint8_t n, char *binstr) {
    uint8_t binary[8];

    int i = 0;
    for(i = 0; i < 8; i++) {
        binary[i] = n % 2;
        n = n / 2;
    }

    for (int j = 7, k= 0; j >= 0; j--, k++)
        sprintf(binstr + k, "%d", binary[j]);
        
	return binstr;
}

void print_usage(char *name) {
	printf("Usage: %s <action> <byte> <bit no>\n", name);
	printf("\tWhere: <action> one of 'h'|'l'|'r'|'s'|'t'\n"
		"\t\t <byte> 8-bit integer in hexadecimal\n"
		"\t\t <bit no> of bit to operate on\n");
}

int main(int argc, char *argv[]) {
	char letter;	
	unsigned long number; 
	int bit; 
    char binary[9];

    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    letter = argv[1][0];

    if (letter != 'h' && letter != 'l' && letter != 'r' && letter != 's' && letter != 't') {
        printf("Action must be one of 'h'|'l'|'r'|'s'|'t'\n");
        return 1;
    }    

    bit = atoi(argv[3]);
    if (bit < 0 || bit > 7) {
        printf("Bit number must be between 0 and 7\n");
        return 1;
    }
    
    char *endptr; 
    number = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || number > 255) {
        printf("Byte must be between 0 and 255\n");
        return 1;
    }

    byte2bin(number, binary);
    printf("Initial value: %s\n", binary); 
    printf("Action: %c\n", letter);
    printf("Bit: %d\n\n", bit);

    switch (letter) {
        case 'h':
            if (number & BIT(bit)) {
                printf("Bit %d is high\n", bit);
            }
            else {
                printf("Bit %d is low\n", bit);
            }
            return 0;
        case 'l':
            if (!(number & BIT(bit))) {
                printf("Bit %d is low\n", bit);
            }
            else {
                printf("Bit %d is high\n", bit);
            }
            return 0;
        case 's':
            number |= BIT(bit);
            break;
        case 'r':
            number &= ~BIT(bit);
            break;
        case 't':
            number ^= BIT(bit);
    }

    byte2bin(number, binary);
    printf("Final result: %s\n", binary);
	
    return 0;
}
