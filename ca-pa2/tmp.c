/* Convert 32-bit single-precision floating point to 12-bit floating point */
fp12 float_fp12(float f) {
	if (f == 0.0)
		return 0;

	u2f value = { .f = f };

	unsigned int sign = ((value.u & 0x80000000) == 0) ? 0 : 0xf800;
	unsigned int exp = (value.u & 0x7f800000) >> 23;
	unsigned int exp_result = exp - 96;
	unsigned int frac = value.u & 0x007fffff;

	float twoThirty = 1;
	for (int i = 0; i < 30; i++) {
		twoThirty *= 0.5;
	}

	if (f > 0 && f < twoThirty) {
		if ((frac & 0x00010000) != 0) { // R17 == 1
	    	unsigned int sticky = frac & 0x8000; // S16
		    for (int i = 15; i >= 0; i--) {
		    	sticky |= frac & (1 << i);
		    }

		    // printf("%d \n", sticky);

		    if (sticky != 0 || ((frac & 0x00020000) != 0 && sticky == 0)) {
		    	frac += 1;

		    	// PRINT(uint16_t, "fracPlusOne", frac);
				// printf("\n");
				if ((frac & 0x0020) != 0) {
		    		exp_result += 1;
		    	}
		    	else {
		    		return 0;
		    	}
		    }
	    }
	}
}