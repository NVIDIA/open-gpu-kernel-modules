/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * IEEE754 floating point
 * common internal header file
 */
/*
 * MIPS floating point support
 * Copyright (C) 1994-2000 Algorithmics Ltd.
 */
#ifndef __IEEE754INT_H
#define __IEEE754INT_H

#include "ieee754.h"

#define CLPAIR(x, y)	((x)*6+(y))

enum maddf_flags {
	MADDF_NEGATE_PRODUCT	= 1 << 0,
	MADDF_NEGATE_ADDITION	= 1 << 1,
};

static inline void ieee754_clearcx(void)
{
	ieee754_csr.cx = 0;
}

static inline void ieee754_setcx(const unsigned int flags)
{
	ieee754_csr.cx |= flags;
	ieee754_csr.sx |= flags;
}

static inline int ieee754_setandtestcx(const unsigned int x)
{
	ieee754_setcx(x);

	return ieee754_csr.mx & x;
}

static inline int ieee754_class_nan(int xc)
{
	return xc >= IEEE754_CLASS_SNAN;
}

#define COMPXSP \
	unsigned int xm; int xe; int xs __maybe_unused; int xc

#define COMPYSP \
	unsigned int ym; int ye; int ys; int yc

#define COMPZSP \
	unsigned int zm; int ze; int zs; int zc

#define EXPLODESP(v, vc, vs, ve, vm)					\
{									\
	vs = SPSIGN(v);							\
	ve = SPBEXP(v);							\
	vm = SPMANT(v);							\
	if (ve == SP_EMAX+1+SP_EBIAS) {					\
		if (vm == 0)						\
			vc = IEEE754_CLASS_INF;				\
		else if (ieee754_csr.nan2008 ^ !(vm & SP_MBIT(SP_FBITS - 1))) \
			vc = IEEE754_CLASS_QNAN;			\
		else							\
			vc = IEEE754_CLASS_SNAN;			\
	} else if (ve == SP_EMIN-1+SP_EBIAS) {				\
		if (vm) {						\
			ve = SP_EMIN;					\
			vc = IEEE754_CLASS_DNORM;			\
		} else							\
			vc = IEEE754_CLASS_ZERO;			\
	} else {							\
		ve -= SP_EBIAS;						\
		vm |= SP_HIDDEN_BIT;					\
		vc = IEEE754_CLASS_NORM;				\
	}								\
}
#define EXPLODEXSP EXPLODESP(x, xc, xs, xe, xm)
#define EXPLODEYSP EXPLODESP(y, yc, ys, ye, ym)
#define EXPLODEZSP EXPLODESP(z, zc, zs, ze, zm)


#define COMPXDP \
	u64 xm; int xe; int xs __maybe_unused; int xc

#define COMPYDP \
	u64 ym; int ye; int ys; int yc

#define COMPZDP \
	u64 zm; int ze; int zs; int zc

#define EXPLODEDP(v, vc, vs, ve, vm)					\
{									\
	vm = DPMANT(v);							\
	vs = DPSIGN(v);							\
	ve = DPBEXP(v);							\
	if (ve == DP_EMAX+1+DP_EBIAS) {					\
		if (vm == 0)						\
			vc = IEEE754_CLASS_INF;				\
		else if (ieee754_csr.nan2008 ^ !(vm & DP_MBIT(DP_FBITS - 1))) \
			vc = IEEE754_CLASS_QNAN;			\
		else							\
			vc = IEEE754_CLASS_SNAN;			\
	} else if (ve == DP_EMIN-1+DP_EBIAS) {				\
		if (vm) {						\
			ve = DP_EMIN;					\
			vc = IEEE754_CLASS_DNORM;			\
		} else							\
			vc = IEEE754_CLASS_ZERO;			\
	} else {							\
		ve -= DP_EBIAS;						\
		vm |= DP_HIDDEN_BIT;					\
		vc = IEEE754_CLASS_NORM;				\
	}								\
}
#define EXPLODEXDP EXPLODEDP(x, xc, xs, xe, xm)
#define EXPLODEYDP EXPLODEDP(y, yc, ys, ye, ym)
#define EXPLODEZDP EXPLODEDP(z, zc, zs, ze, zm)

#define FLUSHDP(v, vc, vs, ve, vm)					\
	if (vc==IEEE754_CLASS_DNORM) {					\
		if (ieee754_csr.nod) {					\
			ieee754_setcx(IEEE754_INEXACT);			\
			vc = IEEE754_CLASS_ZERO;			\
			ve = DP_EMIN-1+DP_EBIAS;			\
			vm = 0;						\
			v = ieee754dp_zero(vs);				\
		}							\
	}

#define FLUSHSP(v, vc, vs, ve, vm)					\
	if (vc==IEEE754_CLASS_DNORM) {					\
		if (ieee754_csr.nod) {					\
			ieee754_setcx(IEEE754_INEXACT);			\
			vc = IEEE754_CLASS_ZERO;			\
			ve = SP_EMIN-1+SP_EBIAS;			\
			vm = 0;						\
			v = ieee754sp_zero(vs);				\
		}							\
	}

#define FLUSHXDP FLUSHDP(x, xc, xs, xe, xm)
#define FLUSHYDP FLUSHDP(y, yc, ys, ye, ym)
#define FLUSHZDP FLUSHDP(z, zc, zs, ze, zm)
#define FLUSHXSP FLUSHSP(x, xc, xs, xe, xm)
#define FLUSHYSP FLUSHSP(y, yc, ys, ye, ym)
#define FLUSHZSP FLUSHSP(z, zc, zs, ze, zm)

#endif /* __IEEE754INT_H  */
