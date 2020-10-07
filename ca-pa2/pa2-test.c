//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2020)
//
//  Project #2: FP12 (12-bit floating point) Representation
//
//  September 28, 2020
//
//  Injae Kang (abcinje@snu.ac.kr)
//  Sunmin Jeong (sunnyday0208@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#define RED   "\033[0;31m"
#define GREEN "\033[0;32m"
#define CYAN  "\033[0;36m"
#define RESET "\033[0m"

#include "pa2.h"

#define BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BINARY(BYTE)			\
	(BYTE & 0x80 ? '1' : '0'),	\
	(BYTE & 0x40 ? '1' : '0'),	\
	(BYTE & 0x20 ? '1' : '0'),	\
	(BYTE & 0x10 ? '1' : '0'),	\
	(BYTE & 0x08 ? '1' : '0'),	\
	(BYTE & 0x04 ? '1' : '0'),	\
	(BYTE & 0x02 ? '1' : '0'),	\
	(BYTE & 0x01 ? '1' : '0')

#define PRINT_BYTE(BYTE) printf(BINARY_PATTERN, BINARY(BYTE))
#define PRINT(DATATYPE, TYPENAME, NUM)				\
	do {							\
		size_t typesize = sizeof(DATATYPE);		\
		DATATYPE data = NUM;				\
		uint8_t *ptr = (uint8_t *)&data;		\
								\
		printf("%s(", TYPENAME);			\
		PRINT_BYTE(*(ptr + typesize - 1));		\
		for (ssize_t i = typesize - 2; i >= 0; i--) {	\
			printf(" ");				\
			PRINT_BYTE(*(ptr + i));			\
		}						\
		printf(")");					\
	} while (0)

#define CHECK(RES, ANS) printf("%s"RESET, (RES) == (ANS) ? GREEN"" : RED"WRONG")
#define COMP(RES, ANS, TYPENAME) comp_##TYPENAME(RES, ANS)

static void comp_int(uint32_t result, uint32_t answer)
{
	CHECK(result, answer);
}

static void comp_fp12(uint16_t result, uint16_t answer)
{
	uint16_t exp = 0x7e0 & result;
	uint16_t frac = 0x1f & result;
	if (exp == 0x7e0 && frac != 0) {
		result &= 0xffe0;
		result++;
	}
	CHECK(result, answer);
}

static void comp_float(uint32_t result, uint32_t answer)
{
	uint32_t exp = 0x7f800000 & result;
	uint32_t frac = 0x7fffff & result;
	if (exp == 0x7f800000 && frac != 0) {
		result &= 0xff800000;
		result++;
	}
	CHECK(result, answer);
}

#define N 6

/* int -> fp12 */
uint32_t test1[N] =	{0x00000000, 0x00000001, 0x000007e4, 0xffffff9b, 0x7fffffff, 0x80000000};
uint16_t ans1[N] =	{0x0000, 0x03e0, 0x053f, 0xfcb2, 0x07c0, 0xffc0};

/* fp12 -> int */
uint16_t test2[N] =	{0xf800, 0x04d6, 0x044c, 0xf81f, 0x07e0, 0xffff};
uint32_t ans2[N] =	{0x00000000, 0x000000d8, 0x0000000b, 0x00000000, 0x80000000, 0x80000000};

/* float -> fp12 */
uint32_t test3[31] =	{
	0x00000001, // 0 00000000 00000000000000000000001 (0.0000... * 2^(1-127))
	0x3fe00000, // 0 01111111 11000000000000000000000 (1.1100... * 2^(127-127))
	0xe0000000, // 1 11000000 00000000000000000000000 (1.0000... * 2^(192-127))
	0x80b5840c, // 1 00000001 01101011000010000001100 (1.011010. * 2^(1-127))
	0x7f800000, 
	0xff800001,

	0x80000000, // -0
	0x007FFFFF, // 0 00000000 11111111111111111111111 (0.1111... * 2^(1-127))

	0x2D800000, // 0 01011010 00000000000000000000000 (1.0000... * 2^(90-127)) 
	0x2D800000, // 0 01011011 00000000000000000000000 (1.0000... * 2^(91-127)) 
	0x2D800001, // 0 01011011 00000000000000000000001 (1.0000... * 2^(91-127))
	0x2DFFFFFF, // 0 01011011 10000000000000000000000 (1.1000... * 2^(91-127))
	0x2DFFFFFF, // 0 01011011 11111111111111111111111 (1.1111... * 2^(91-127))

	0x2E000000, // 0 01011100 00000000000000000000000 (1.0000... * 2^(92-127))
	0x2E7FFFFF, // 0 01011100 11111111111111111111111 (1.1111... * 2^(92-127)) 5
	0x2E800000, // 0 01011101 00000000000000000000000 (1.0000... * 2^(93-127))
	0x2EFFFFFF, // 0 01011101 11111111111111111111111 (1.1111... * 2^(93-127))

	0x30000000, // 0 01100000 00000000000000000000000 (1.0000... * 2^(96-127))
	0x30380000, // 0 01100000 01110000000000000000000 (1.0111... * 2^(96-127)) 9
	0x307C0000, // 0 01100000 11111000000000000000000 (1.111110. * 2^(96-127))
	0x307FFFFF, // 0 01100000 11111111111111111111111 (1.111111. * 2^(96-127)) 

	0x30800000, // 0 01100001 00000000000000000000000 (1.0000... * 2^(97-127))
	0x30FFFFFF, // 0 01100001 11111111111111111111111 (1.1111... * 2^(97-127))

	0x4E800000, // 0 10011101 00000000000000000000000 (1.0000... * 2^(157-127))
	0x4EFFFFFF, // 0 10011101 11111111111111111111111 (1.1111... * 2^(157-127))

	0x4F000000, // 0 10011110 00000000000000000000000 (1.0000... * 2^(158-127))
	0x4F040000, // 0 10011110 00001000000000000000000 (1.00001.. * 2^(158-127))
	0x4F060000, // 0 10011110 00001100000000000000000 (1.000011. * 2^(158-127))
	0x4F7FFFFF, // 0 10011110 11111111111111111111111 (1.1111... * 2^(158-127))
	0xCF7FFFFF, // 1 10011110 11111111111111111111111 (1.1111... * 2^(158-127))
	0xCF800000, // 1 10011111 00000000000000000000000 (1.0000... * 2^(159-127))
};
uint16_t ans3[31] =	{
	0x0000, 
	0x03f8, // 00000 011111 11000 (1.11000 * 2^(31-31))
	0xffe0, // 11111 111111 00000 -inf
	0xf800, // 11111 000000 00000 -0
	0x07e0, 
	0xffe1, 

	0xf800, // 11111 000000 00000 
	0x0000, // 00000 000000 00000 

	0x0000, // 00000 000000 00000 
	0x0000, // 00000 000000 00000 
	0x0001, // 00000 000000 00001
	0x0001, // 00000 000000 00001
	0x0001, // 00000 000000 00001

	0x0001, // 00000 000000 00001
	0x0002, // 00000 000000 00010 5
	0x0002, // 00000 000000 00010
	0x0004, // 00000 000000 00100

	0x0010, // 00000 000000 10000
	0x0017, // 00000 000000 10111 9
	0x0020, // 00000 000001 00000 
	0x0020, // 00000 000001 00000

	0x0020, // 00000 000001 00000 
	0x0040, // 00000 000010 00000 
	
	0x07A0, // 00000 111101 00000 1.00000 * 2^30 157
	0x07c0, // 00000 111110 00000 1.00000 * 2^31 157

	0x07c0, // 00000 111110 00000 1.00000 * 2^31 158
	0x07c1, // 00000 111110 00001 1.00001 * 2^31 158
	0x07c2, // 00000 111110 00010 1.00010 * 2^31 158
	0x07e0, // 00000 111111 00000 +inf 158
	0xFFE0, // 11111 111111 00000 -inf 158
	0xFFE0, // 11111 111111 00000 -inf 159
};

/* fp12 -> float */
uint16_t test4[13] =	{0x0000, 0xf801, 0x0555, 0x07e0, 0xffe0, 0x07e1, 
	0xF800, // 11111 000000 00000 -0
	0x0001, // 00000 000000 00001 (0.00001 * 2^(1-31))
	0x0017, // 00000 000000 10111 (0.10111 * 2^(1-31))
	0x001f, // 00000 000000 11111 (0.11111 * 2^(1-31))
	0x0020, // 00000 000001 00000 (1.00000 * 2^(1-31))
	0x07DF, // 00000 111110 11111 (1.11111 * 2^(62-31))
	0xffe1, // 11111 111111 00000 NaN
};
uint32_t ans4[13] =	{0x00000000, 0xae000000, 0x45540000, 0x7f800000, 0xff800000, 0x7f800001, 
	0x80000000, // -0
	0x2e000000, // 0 01011100 00000000000000000000000 (1.000000. * 2^(92-127))
	0x30380000, // 0 01100000 01110000000000000000000 (1.011100. * 2^(96-127))
	0x30780000, // 0 01100000 11110000000000000000000 (1.111100. * 2^(96-127))
	0x30800000, // 0 01100001 00000000000000000000000 (1.000000. * 2^(97-127))
	0x4F7C0000, // 0 10011110 11111000000000000000000 (1.111110. * 2^(158-127))
	0xFF800001, // 1 11111111 00000000000000000000001 NaN
};

int main(void)
{
	// printf("\n%sTest 1: Casting from int to fp12%s\n", CYAN, RESET);
	// for (int i = 0; i < N; i++) {
	// 	uint16_t result = (uint16_t)int_fp12(test1[i]);

	// 	PRINT(uint32_t, "int", test1[i]);
	// 	printf(" => ");
	// 	PRINT(uint16_t, "fp12", result);
	// 	printf(", ");
	// 	PRINT(uint16_t, "ans", ans1[i]);
	// 	printf(", ");
	// 	COMP(result, ans1[i], fp12);
	// 	printf("\n");
	// }

	// printf("\n%sTest 2: Casting from fp12 to int%s\n", CYAN, RESET);
	// for (int i = 0; i < N; i++) {
	// 	uint32_t result = (uint32_t)fp12_int(test2[i]);

	// 	PRINT(uint16_t, "fp12", test2[i]);
	// 	printf(" => ");
	// 	PRINT(uint32_t, "int", result);
	// 	printf(", ");
	// 	PRINT(uint32_t, "ans", ans2[i]);
	// 	printf(", ");
	// 	COMP(result, ans2[i], int);
	// 	printf("\n");
	// }

	printf("\n%sTest 3: Casting from float to fp12%s\n", CYAN, RESET);
	struct timeval t0, t1;
	float sum = 0;

	for (int k = 0; k < 100; k++) {
		gettimeofday(&t0, NULL);

		for (int j = 0; j < 300; j++) {
			for (int i = 0; i < 29; i++) {
				float *p = (float *)&test3[i];
				float f = *p;
				uint16_t result = (uint16_t)float_fp12(f);

				// PRINT(uint32_t, "float", test3[i]);
				// printf(" => ");
				// PRINT(uint16_t, "fp12", result);
				// printf(", ");
				// PRINT(uint16_t, "ans", ans3[i]);
				// printf(", ");
				COMP(result, ans3[i], fp12);
				// printf("\n");
			}
		}

		gettimeofday(&t1, NULL);
		float diff = (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
		// printf("took %f ms\n", diff); 
		sum += diff;
	}
	printf("Average: %f ms\n", sum / 100.0);

	// printf("\n%sTest 4: Casting from fp12 to float%s\n", CYAN, RESET);
	// for (int i = 0; i < 13; i++) {
	// 	float f = fp12_float(test4[i]);
	// 	uint32_t *p = (uint32_t *)&f;
	// 	uint32_t result = *p;

	// 	PRINT(uint16_t, "fp12", test4[i]);
	// 	printf(" => ");
	// 	PRINT(uint32_t, "float", result);
	// 	printf(", ");
	// 	PRINT(uint32_t, "ans", ans4[i]);
	// 	printf(", ");
	// 	COMP(result, ans4[i], float);
	// 	printf("\n");
	// }

	// printf("\n");
	return 0;
}
