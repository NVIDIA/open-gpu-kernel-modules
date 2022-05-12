// SPDX-License-Identifier: GPL-2.0
#include "libgcc.h"

#define __ll_B ((UWtype) 1 << (W_TYPE_SIZE / 2))
#define __ll_lowpart(t) ((UWtype) (t) & (__ll_B - 1))
#define __ll_highpart(t) ((UWtype) (t) >> (W_TYPE_SIZE / 2))

#define umul_ppmm(w1, w0, u, v) \
	do {			   \
		UWtype __x0, __x1, __x2, __x3;	\
		UHWtype __ul, __vl, __uh, __vh; \
		__ul = __ll_lowpart(u);	\
		__uh = __ll_highpart(u);	\
		__vl = __ll_lowpart(v);	\
		__vh = __ll_highpart(v);	\
		__x0 = (UWtype) __ul * __vl;	\
		__x1 = (UWtype) __ul * __vh;	\
		__x2 = (UWtype) __uh * __vl;	\
		__x3 = (UWtype) __uh * __vh;	\
		__x1 += __ll_highpart(__x0);	\
		__x1 += __x2;			\
		if (__x1 < __x2)		\
			__x3 += __ll_B;		\
		(w1) = __x3 + __ll_highpart(__x1);	       \
		(w0) = __ll_lowpart(__x1) * __ll_B + __ll_lowpart(__x0); \
	} while (0)

#define __umulsidi3(u, v) (			\
		{				\
			DWunion __w;		\
			umul_ppmm(__w.s.high, __w.s.low, u, v);	\
			__w.ll; }					\
		)

DWtype __muldi3(DWtype u, DWtype v)
{
	const DWunion uu = {.ll = u};
	const DWunion vv = {.ll = v};
	DWunion w = {.ll = __umulsidi3(uu.s.low, vv.s.low)};

	w.s.high += ((UWtype) uu.s.low * (UWtype) vv.s.high
		     + (UWtype) uu.s.high * (UWtype) vv.s.low);

	return w.ll;
}
