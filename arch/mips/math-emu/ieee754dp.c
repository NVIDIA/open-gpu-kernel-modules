// SPDX-License-Identifier: GPL-2.0-only
/* IEEE754 floating point arithmetic
 * double precision: common utilities
 */
/*
 * MIPS floating point support
 * Copyright (C) 1994-2000 Algorithmics Ltd.
 */

#include <linux/compiler.h>

#include "ieee754dp.h"

int ieee754dp_class(union ieee754dp x)
{
	COMPXDP;
	EXPLODEXDP;
	return xc;
}

static inline int ieee754dp_isnan(union ieee754dp x)
{
	return ieee754_class_nan(ieee754dp_class(x));
}

static inline int ieee754dp_issnan(union ieee754dp x)
{
	int qbit;

	assert(ieee754dp_isnan(x));
	qbit = (DPMANT(x) & DP_MBIT(DP_FBITS - 1)) == DP_MBIT(DP_FBITS - 1);
	return ieee754_csr.nan2008 ^ qbit;
}


/*
 * Raise the Invalid Operation IEEE 754 exception
 * and convert the signaling NaN supplied to a quiet NaN.
 */
union ieee754dp __cold ieee754dp_nanxcpt(union ieee754dp r)
{
	assert(ieee754dp_issnan(r));

	ieee754_setcx(IEEE754_INVALID_OPERATION);
	if (ieee754_csr.nan2008) {
		DPMANT(r) |= DP_MBIT(DP_FBITS - 1);
	} else {
		DPMANT(r) &= ~DP_MBIT(DP_FBITS - 1);
		if (!ieee754dp_isnan(r))
			DPMANT(r) |= DP_MBIT(DP_FBITS - 2);
	}

	return r;
}

static u64 ieee754dp_get_rounding(int sn, u64 xm)
{
	/* inexact must round of 3 bits
	 */
	if (xm & (DP_MBIT(3) - 1)) {
		switch (ieee754_csr.rm) {
		case FPU_CSR_RZ:
			break;
		case FPU_CSR_RN:
			xm += 0x3 + ((xm >> 3) & 1);
			/* xm += (xm&0x8)?0x4:0x3 */
			break;
		case FPU_CSR_RU:	/* toward +Infinity */
			if (!sn)	/* ?? */
				xm += 0x8;
			break;
		case FPU_CSR_RD:	/* toward -Infinity */
			if (sn) /* ?? */
				xm += 0x8;
			break;
		}
	}
	return xm;
}


/* generate a normal/denormal number with over,under handling
 * sn is sign
 * xe is an unbiased exponent
 * xm is 3bit extended precision value.
 */
union ieee754dp ieee754dp_format(int sn, int xe, u64 xm)
{
	assert(xm);		/* we don't gen exact zeros (probably should) */

	assert((xm >> (DP_FBITS + 1 + 3)) == 0);	/* no excess */
	assert(xm & (DP_HIDDEN_BIT << 3));

	if (xe < DP_EMIN) {
		/* strip lower bits */
		int es = DP_EMIN - xe;

		if (ieee754_csr.nod) {
			ieee754_setcx(IEEE754_UNDERFLOW);
			ieee754_setcx(IEEE754_INEXACT);

			switch(ieee754_csr.rm) {
			case FPU_CSR_RN:
			case FPU_CSR_RZ:
				return ieee754dp_zero(sn);
			case FPU_CSR_RU:    /* toward +Infinity */
				if (sn == 0)
					return ieee754dp_min(0);
				else
					return ieee754dp_zero(1);
			case FPU_CSR_RD:    /* toward -Infinity */
				if (sn == 0)
					return ieee754dp_zero(0);
				else
					return ieee754dp_min(1);
			}
		}

		if (xe == DP_EMIN - 1 &&
		    ieee754dp_get_rounding(sn, xm) >> (DP_FBITS + 1 + 3))
		{
			/* Not tiny after rounding */
			ieee754_setcx(IEEE754_INEXACT);
			xm = ieee754dp_get_rounding(sn, xm);
			xm >>= 1;
			/* Clear grs bits */
			xm &= ~(DP_MBIT(3) - 1);
			xe++;
		}
		else {
			/* sticky right shift es bits
			 */
			xm = XDPSRS(xm, es);
			xe += es;
			assert((xm & (DP_HIDDEN_BIT << 3)) == 0);
			assert(xe == DP_EMIN);
		}
	}
	if (xm & (DP_MBIT(3) - 1)) {
		ieee754_setcx(IEEE754_INEXACT);
		if ((xm & (DP_HIDDEN_BIT << 3)) == 0) {
			ieee754_setcx(IEEE754_UNDERFLOW);
		}

		/* inexact must round of 3 bits
		 */
		xm = ieee754dp_get_rounding(sn, xm);
		/* adjust exponent for rounding add overflowing
		 */
		if (xm >> (DP_FBITS + 3 + 1)) {
			/* add causes mantissa overflow */
			xm >>= 1;
			xe++;
		}
	}
	/* strip grs bits */
	xm >>= 3;

	assert((xm >> (DP_FBITS + 1)) == 0);	/* no excess */
	assert(xe >= DP_EMIN);

	if (xe > DP_EMAX) {
		ieee754_setcx(IEEE754_OVERFLOW);
		ieee754_setcx(IEEE754_INEXACT);
		/* -O can be table indexed by (rm,sn) */
		switch (ieee754_csr.rm) {
		case FPU_CSR_RN:
			return ieee754dp_inf(sn);
		case FPU_CSR_RZ:
			return ieee754dp_max(sn);
		case FPU_CSR_RU:	/* toward +Infinity */
			if (sn == 0)
				return ieee754dp_inf(0);
			else
				return ieee754dp_max(1);
		case FPU_CSR_RD:	/* toward -Infinity */
			if (sn == 0)
				return ieee754dp_max(0);
			else
				return ieee754dp_inf(1);
		}
	}
	/* gen norm/denorm/zero */

	if ((xm & DP_HIDDEN_BIT) == 0) {
		/* we underflow (tiny/zero) */
		assert(xe == DP_EMIN);
		if (ieee754_csr.mx & IEEE754_UNDERFLOW)
			ieee754_setcx(IEEE754_UNDERFLOW);
		return builddp(sn, DP_EMIN - 1 + DP_EBIAS, xm);
	} else {
		assert((xm >> (DP_FBITS + 1)) == 0);	/* no excess */
		assert(xm & DP_HIDDEN_BIT);

		return builddp(sn, xe + DP_EBIAS, xm & ~DP_HIDDEN_BIT);
	}
}
