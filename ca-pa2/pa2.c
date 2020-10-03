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

#include <stdio.h>
#include <stdint.h>

#include "pa2.h"

typedef union {
    uint32_t u;
    float f;
} u2f;

/* Convert 32-bit signed integer to 12-bit floating point */
fp12 int_fp12(int n)
{
	if (n == 0)
		return 0;

	// Get sign bit and the absolute value
	unsigned int sign = 0;
	unsigned int value = (unsigned int) n;

	// printf("%d\n", n);

	if (n < 0) {
		sign = 0xf800;
		value = (unsigned int) -n;
	}

	// PRINT(uint32_t, "val", value);
	// printf("\n");
	// PRINT(uint16_t, "sign", sign);
	// printf("\n");

	// Get exponent
    unsigned int exp = 62; // 62 - (32 - digits) = 30 + digits
	while ((value & (1 << 31)) == 0) {
        value <<= 1;
        if (exp == 0)
			return sign | 0x07E0;
        exp--;
    }
    // PRINT(uint16_t, "result", sign | 0x07e0);
    // printf("\n");
    // PRINT(uint16_t, "exp", exp);
	// printf(" %d\n", exp);

    // Get fraction with round-to-even
    unsigned int frac = value >> 26;

    // PRINT(uint32_t, "val", value);
    // printf("\n");

    // PRINT(uint16_t, "frac", frac);
    // printf("\n");

    if ((value & (1 << 25)) != 0) {
    	unsigned int sticky = value & (1 << 24);
	    for (int i = 8; i < exp - 30; i++) {
	    	sticky |= value & (1 << (31 - i));
	    }

	    if (sticky != 0 || ((value & (1 << 26)) != 0 && sticky == 0)) {
	    	frac += 1;
	    	// PRINT(uint32_t, "val", value + 1);
    		// printf("\n");

	    	if ((frac & 0x0040) != 0) { // ((value + 1) & (1 << (exp - 30))) != 0
	    		frac >>= 1;
    			exp += 1;
    			if (exp > 62)
    				return sign | 0x07E0;
	    	}
	    }
    }

    // PRINT(uint16_t, "frac", frac);
    // printf("\n");

	return sign | ((exp << 5) & 0x07e0) | (frac & 0x001f);
}

/* Convert 12-bit floating point to 32-bit signed integer */
int fp12_int(fp12 x)
{
	unsigned int value = x & 0x07FF;

	if (value == 0)
		return 0;

	if (value >> 5 == 0x003f)
		return 0x80000000;

	int man = (value & 0x001f) + 0x0020;
	int exp = (value & 0x07e0) >> 5;

	// PRINT(uint32_t, "man", man);
 //    printf(" %d\n", man);
 //    PRINT(uint32_t, "exp", exp);
 //    printf(" %d\n", exp);

	if (exp - 31 >= 15) // exp - 31 > 15
		return 0x80000000;

    if (exp > 36) { // exp - 31 > 5
		for (int i = 0; i < exp - 36; i++) // exp - 5 - 31
			man *= 2;
    }
    else { // exp - 31 <= 5
    	for (int i = 0; i < 36 - exp; i++)
    		man /= 2;
    }

	// PRINT(uint32_t, "man", man);
 //    printf("\n");

	int result = (int) man;

	if ((x & (1 << 15)) != 0)
		result *= -1;

	return result;
}


fp12 float_fp12(float f)
{
	u2f value = { .f = f };

	unsigned int sign = ((value.u & 0x80000000) == 0) ? 0 : 0xf800;
	unsigned int exp = (value.u & 0x7f800000) >> 23;

	if (exp < 91) {
		return sign | 0;
	}

	unsigned int frac = value.u & 0x007fffff;

	if (exp == 0x00ff) {
		if (frac == 0)
			return sign | 0x07e0;
		else {
			return sign | 0x07e1;
		}
	}
	else if (exp >= 159) {
		return sign | 0x07e0;
	}

	unsigned int exp_result = exp - 96;
	unsigned int sticky = 0;

	frac |= 0x00800000;
	
    if (exp < 97) {
		exp_result = 0;
		for (int i = 0; i < 97 - exp; i++) {
			sticky |= (frac & (1 >> i));
			frac >>= 1;
		}
	}
	
	unsigned int man = frac;
	frac >>= 18;

	if ((man & 0x00020000) != 0) { 
		sticky |= man & 0x0001FFFF;

	    if (sticky != 0 || ((man & 0x00040000) != 0 && sticky == 0)) { // L18
	    	frac += 1;

	    	if ((frac & 0x0040) != 0) { 
	    		frac >>= 1;
    			exp_result += 1;

				if (exp_result >= 63) {
					return sign | 0x07e0;
				}
	    	}
	    	else if (exp == 96 && (frac & 0x0020) != 0) {
	    		exp_result = 1;
	    	}
	    }
    }

	return sign | (exp_result << 5) | (frac & 0x001f);
}

/* Convert 12-bit floating point to 32-bit single-precision floating point */
float fp12_float(fp12 x)
{
	unsigned int sign = (x & 0x0800) << 20;
	unsigned int exp = (x & 0x07e0) >> 5;
	unsigned int frac = (x & 0x001f);

	// PRINT(uint32_t, "sign", sign);
	// printf("\n");
	// PRINT(uint16_t, "frac", frac);
	// printf("\n");
	// PRINT(uint16_t, "exp", exp);
	// printf(" %d\n", exp);

	if (exp == 0x003f) {
		if (frac == 0) { // infinity
			u2f infinity = { .u = (sign | 0x7f800000) };
			// printf("infinity: %f\n", infinity.f);
			return infinity.f;
		}
		else { // NaN
			u2f nan = { .u = (sign | 0x7f800001) };
			// printf("nan: %f\n", nan.f);
			return nan.f;
		}
	}
	else if (exp == 0) { // denormalized fp12
		if (frac == 0) {
			u2f zero = { .u = ( sign | 0x0000 )};
			return zero.f;
		}
		int shifted = 0;
		while ((frac & 0x0020) == 0) {
			frac <<= 1;
			shifted++;
			// PRINT(uint16_t, "frac", frac);
			// printf("\n");
		}

		// printf("%d\n", shifted);

		// frac = frac & 0x001f;
		exp = 97 - shifted;
	}
	else { // normalized
		exp += 96;
	}

	

	u2f result = { .u = (sign | ((exp << 23) & 0x7f800000) | ((frac << 18) & 0x007fffff)) };

	// PRINT(uint32_t, "sign", sign);
	// printf("\n");
	// PRINT(uint32_t, "frac", frac);
	// printf("\n");
	// PRINT(uint16_t, "exp", exp);
	// printf(" %d\n", exp);
	// printf("%f\n", result.f);

	return result.f;
}
