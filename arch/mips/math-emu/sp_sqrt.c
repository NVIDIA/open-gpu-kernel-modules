// SPDX-License-Identifier: GPL-2.0-only
/* IEEE754 floating point arithmetic
 * single precision square root
 */
/*
 * MIPS floating point support
 * Copyright (C) 1994-2000 Algorithmics Ltd.
 */

#include "ieee754sp.h"

union ieee754sp ieee754sp_sqrt(union ieee754sp x)
{
	int ix, s, q, m, t, i;
	unsigned int r;
	COMPXSP;

	/* take care of Inf and NaN */

	EXPLODEXSP;
	ieee754_clearcx();
	FLUSHXSP;

	/* x == INF or NAN? */
	switch (xc) {
	case IEEE754_CLASS_SNAN:
		return ieee754sp_nanxcpt(x);

	case IEEE754_CLASS_QNAN:
		/* sqrt(Nan) = Nan */
		return x;

	case IEEE754_CLASS_ZERO:
		/* sqrt(0) = 0 */
		return x;

	case IEEE754_CLASS_INF:
		if (xs) {
			/* sqrt(-Inf) = Nan */
			ieee754_setcx(IEEE754_INVALID_OPERATION);
			return ieee754sp_indef();
		}
		/* sqrt(+Inf) = Inf */
		return x;

	case IEEE754_CLASS_DNORM:
	case IEEE754_CLASS_NORM:
		if (xs) {
			/* sqrt(-x) = Nan */
			ieee754_setcx(IEEE754_INVALID_OPERATION);
			return ieee754sp_indef();
		}
		break;
	}

	ix = x.bits;

	/* normalize x */
	m = (ix >> 23);
	if (m == 0) {		/* subnormal x */
		for (i = 0; (ix & 0x00800000) == 0; i++)
			ix <<= 1;
		m -= i - 1;
	}
	m -= 127;		/* unbias exponent */
	ix = (ix & 0x007fffff) | 0x00800000;
	if (m & 1)		/* odd m, double x to make it even */
		ix += ix;
	m >>= 1;		/* m = [m/2] */

	/* generate sqrt(x) bit by bit */
	ix += ix;
	s = 0;
	q = 0;			/* q = sqrt(x) */
	r = 0x01000000;		/* r = moving bit from right to left */

	while (r != 0) {
		t = s + r;
		if (t <= ix) {
			s = t + r;
			ix -= t;
			q += r;
		}
		ix += ix;
		r >>= 1;
	}

	if (ix != 0) {
		ieee754_setcx(IEEE754_INEXACT);
		switch (ieee754_csr.rm) {
		case FPU_CSR_RU:
			q += 2;
			break;
		case FPU_CSR_RN:
			q += (q & 1);
			break;
		}
	}
	ix = (q >> 1) + 0x3f000000;
	ix += (m << 23);
	x.bits = ix;
	return x;
}
