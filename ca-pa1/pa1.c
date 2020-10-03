//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2020)
//
//  Project #1: Compressing Data with Huffman Coding
//
//  September 9, 2020
//
//  Injae Kang (abcinje@snu.ac.kr)
//  Sunmin Jeong (sunnyday0208@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------

// #include <stdio.h>

typedef struct symbol {
	char *binary;
	char *code;
	int freq;
	int originalIndex;
}symbol;

int compare(const char *s1, const char *s2) {
  const unsigned char *us1 = (const unsigned char *) s1;
  const unsigned char *us2 = (const unsigned char *) s2;

  while (*us1 == *us2 && *us1 != '\0') {
    us1++;
    us2++;
  }

  return (*us1 > *us2) - (*us1 < *us2);
}

char* concat(char *result, const char *str, int i) {
	int j;
	for (j = 0; str[j] != '\0'; j++) {
		result[i + j] = str[j];
	}
	result[i + j] = '\0';
	return result;
}

/* TODO: Implement this function */
int encode(const char *inp, int inbytes, char *outp, int outbytes)
{
	char *binaries[16] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};
	char *codes[16] = {"000", "001", "010", "011", "1000", "1001", "1010", "1011", "11000", "11001", "11010", "11011", "11100", "11101", "11110", "11111"};
	struct symbol symbols[16];

	if (inbytes <= 0)
		return 0;

	// initialize outp
	// for (int i = 0; i < outbytes; i++)
 //  		outp[i] = 0;

  	unsigned char hex[inbytes * 2];
	// change each character into hex representation
	for (int i = 0; i < inbytes; i++) {
		hex[2 * i] = (unsigned char) inp[i] / 16;
		hex[2 * i + 1] = (unsigned char) inp[i] % 16;
	}

	// for (int i = 0; i < inbytes * 2; i++) {
	// 	printf("%d: %d\n", i, hex[i]);
	// }
	// initialize array of symbols
	for (int i = 0; i < 16; i++) {
		symbols[i].binary = binaries[15 - i];
		symbols[i].freq = 0;
		symbols[i].originalIndex = 15 - i;
	}

	// read each of the 4-bit value
	for (int i = 0; i < inbytes * 2; i++) {
		symbols[15 - hex[i]].freq++;
	}

	// sort by frequency
	// sort(symbols, 16);
	struct symbol tmp;
	for (int i = 0; i < 16; i++) {
		for (int j = i+1; j < 16; j++) {
			if (symbols[i].freq <= symbols[j].freq) {
				tmp = symbols[i];
				symbols[i] = symbols[j];
				symbols[j] = tmp;
			}
		}
	}
	// sortHalf(symbols, 8, 16);
	for (int i = 8; i < 16; i++) {
		for (int j = i + 1; j < 16; j++) {
			if (symbols[i].originalIndex > symbols[j].originalIndex) {
				tmp = symbols[i];
				symbols[i] = symbols[j];
				symbols[j] = tmp;
			}
		}
	}

	// assign codes to the array
	for (int i = 0; i < 16; i++) {
		symbols[i].code = codes[i];
		// printf("%d\t%s\n", symbols[i].freq, symbols[i].binary);
	}
	
	// compress the original input data 
	char compressed[outbytes * 8];
	int size = 36;
	// struct symbol tmp;

	for (int i = 0; i < inbytes * 2; i++) {

		// printf("%d: %d\t%d\n", i, hex[i], inbytes);
		for (int j = 0; j < 16; j++) {
			
			if (symbols[j].originalIndex == hex[i]) {
				// printf("- %s\n", symbols[j].code);

				// concat the code to the compressed
				int codesize;
				for (codesize = 0; symbols[j].code[codesize] != '\0'; codesize++);
				for (int k = 0; k < codesize; k++) {
					compressed[size] = symbols[j].code[k];
					size++;
				}
				break;
			}
		}
		// printf("- i: %d\t%d\n", i, size);
	}
	compressed[size] = '\0';
	// printf("outbytes: %d\n", outbytes);

	// puts("*rank table");

	// print rank table
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			compressed[i * 4 + j] = symbols[i].binary[j];
		}
	}

	// puts("*end info");

	// end info = get size of the compressed and % it with 4
	int endinfo = (8 - size % 8) % 8;
	for (int i = 0; i < 4; i++) {
		compressed[32 + i] = binaries[endinfo][i];
	}

	// puts("*padded bits");

	// padded bits
	for (int i = 0; i < endinfo; i++) {
		compressed[size] = '0';
		size++;
	}
	// printf("%d", size);

	// puts(compressed);

	unsigned char result[size / 8];

	for (int i = 0; i < size / 8; i++) {
		result[i] = 0;
		for (int j = 0; j < 8; j++) {
			result[i] |= (compressed[i * 8 + j] == '1') << (7 - j);
		}
	}
	// for (int i = 0; i < size / 8; i++)
	// 	printf("%02x ", result[i]);

	if (size / 8 > outbytes) {
		return -1;
	}

	for (int i = 0; i < size / 8; i++) {
		outp[i] = result[i];
	}

	return size / 8;
}
