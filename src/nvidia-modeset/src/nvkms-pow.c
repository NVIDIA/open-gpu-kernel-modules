/*
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* Adapted from https://www.netlib.org/fdlibm/fdlibm.h 1.5 04/04/22 */
/* Adapted from https://www.netlib.org/fdlibm/e_pow.c 1.5 04/04/22 SMI */
/* Adapted from https://www.netlib.org/fdlibm/s_fabs.c 1.3 95/01/18 */
/* Adapted from https://www.netlib.org/fdlibm/s_scalbn.c 1.3 95/01/18 */
/* Adapted from https://www.netlib.org/fdlibm/s_copysign.c 1.3 95/01/18 */

#include "nvkms-softfloat.h"
#include "nv-float.h"

static const float64_t
bp[]     = {{0x3FF0000000000000}, {0x3FF8000000000000},}, // 1.0, 1.5
dp_h[]   = {{0x0000000000000000}, {0x3FE2B80340000000},}, // 5.84962487220764160156e-01
dp_l[]   = {{0x0000000000000000}, {0x3E4CFDEB43CFD006},}, // 1.35003920212974897128e-08
nan      = {0xFFF8000000000000}, // NaN
zero     = {0x0000000000000000}, // 0.0
quarter  = {0x3FD0000000000000}, // 0.25
third    = {0x3FD5555555555555}, // 0.3333333333333333333333
half     = {0x3FE0000000000000}, // 0.5
one      = {0x3FF0000000000000}, // 1.0
two      = {0x4000000000000000}, // 2.0
three    = {0x4008000000000000}, // 3.0
two53    = {0x4340000000000000}, // 9007199254740992.0
two54    = {0x4350000000000000}, // 1.80143985094819840000e+16
twom54   = {0x3C90000000000000}, // 5.55111512312578270212e-17
huge     = {0x7E37E43C8800759C}, // 1.0e300
tiny     = {0x01A56E1FC2F8F359}, // 1.0e-300
/* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
L1       = {0x3FE3333333333303}, //  5.99999999999994648725e-01}
L2       = {0x3FDB6DB6DB6FABFF}, //  4.28571428578550184252e-01}
L3       = {0x3FD55555518F264D}, //  3.33333329818377432918e-01}
L4       = {0x3FD17460A91D4101}, //  2.72728123808534006489e-01}
L5       = {0x3FCD864A93C9DB65}, //  2.30660745775561754067e-01}
L6       = {0x3FCA7E284A454EEF}, //  2.06975017800338417784e-01}
P1       = {0x3FC555555555553E}, //  1.66666666666666019037e-01}
P2       = {0xBF66C16C16BEBD93}, // -2.77777777770155933842e-03}
P3       = {0x3F11566AAF25DE2C}, //  6.61375632143793436117e-05}
P4       = {0xBEBBBD41C5D26BF1}, // -1.65339022054652515390e-06}
P5       = {0x3E66376972BEA4D0}, //  4.13813679705723846039e-08}
lg2      = {0x3FE62E42FEFA39EF}, //  6.93147180559945286227e-01}
lg2_h    = {0x3FE62E4300000000}, //  6.93147182464599609375e-01}
lg2_l    = {0xBE205C610CA86C39}, // -1.90465429995776804525e-09}
ovt      = {0x3C971547652B82FE}, // -(1024-log2(ovfl+.5ulp)),  8.0085662595372944372e-0017
cp       = {0x3FEEC709DC3A03FD}, // 2/(3ln2),                  9.61796693925975554329e-01
cp_h     = {0x3FEEC709E0000000}, // (float)cp,                 9.61796700954437255859e-01
cp_l     = {0xBE3E2FE0145B01F5}, // tail of cp_h,             -7.02846165095275826516e-09
ivln2    = {0x3FF71547652B82FE}, // 1/ln2,                     1.44269504088896338700e+00
ivln2_h  = {0x3FF7154760000000}, // 24b 1/ln2,                 1.44269502162933349609e+00
ivln2_l  = {0x3E54AE0BF85DDF44}; // 1/ln2 tail,                1.92596299112661746887e-08

// Little endian only
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x

/*
 * F64_copysign(float64_t x, float64_t y)
 * F64_copysign(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */
static float64_t F64_copysign(float64_t x, float64_t y)
{
    __HI(x) = (__HI(x) & 0x7fffffff) | (__HI(y) & 0x80000000);
    return x;
}

/*
 * F64_scalbn (float64_t x, int n)
 * F64_scalbn(x,n) returns x* 2**n  computed by  exponent
 * manipulation rather than by actually performing an
 * exponentiation or a multiplication.
 */
static float64_t F64_scalbn(float64_t x, int n)
{
    int  k,hx,lx;
    hx = __HI(x);
    lx = __LO(x);

    k = (hx & 0x7ff00000) >> 20; // extract exponent
    if (k == 0) { // 0 or subnormal x
        if ((lx | (hx & 0x7fffffff)) == 0) {
             return x; // +-0
        }
        x = f64_mul(x, two54);
        hx = __HI(x);
        k = ((hx & 0x7ff00000) >> 20) - 54;
        if (n < -50000) {
             return f64_mul(tiny, x); // underflow
        }
    }
    if (k == 0x7ff) {
         return f64_add(x, x); // NaN or Inf
    }
    k = k + n;
    if (k > 0x7fe) {
        return f64_mul(huge, F64_copysign(huge,x)); // overflow
    }
    if (k > 0) { // normal result
        __HI(x) = (hx & 0x800fffff) | (k << 20);
        return x;
    }
    if (k <= -54) {
        if (n > 50000) { // in case integer overflow in n+k
            return f64_mul(huge, F64_copysign(huge,x)); // overflow
        }
    } else {
        return f64_mul(tiny, F64_copysign(tiny,x)); // underflow
    }
    k += 54; // subnormal result
    __HI(x) = (hx & 0x800fffff) | (k << 20);
    return f64_mul(x, twom54);
}


/*
 * F64_fabs(x) returns the absolute value of x.
 */
static float64_t F64_fabs(float64_t x)
{
    __HI(x) &= 0x7fffffff;
    return x;
}

/*
 * nvKmsPow(x,y) return x**y
 *
 *            n
 * Method:  Let x =  2   * (1+f)
 *  1. Compute and return log2(x) in two pieces:
 *      log2(x) = w1 + w2,
 *     where w1 has 53-24 = 29 bit trailing zeros.
 *  2. Perform y*log2(x) = n+y' by simulating muti-precision
 *     arithmetic, where |y'|<=0.5.
 *  3. Return x**y = 2**n*exp(y'*log2)
 *
 * Special cases:
 *  1.  (anything) ** 0  is 1
 *  2.  (anything) ** 1  is itself
 *  3.  (anything) ** NAN is NAN
 *  4.  NAN ** (anything except 0) is NAN
 *  5.  +-(|x| > 1) **  +INF is +INF
 *  6.  +-(|x| > 1) **  -INF is +0
 *  7.  +-(|x| < 1) **  +INF is +0
 *  8.  +-(|x| < 1) **  -INF is +INF
 *  9.  +-1         ** +-INF is NAN
 *  10. +0 ** (+anything except 0, NAN)               is +0
 *  11. -0 ** (+anything except 0, NAN, odd integer)  is +0
 *  12. +0 ** (-anything except 0, NAN)               is +INF
 *  13. -0 ** (-anything except 0, NAN, odd integer)  is +INF
 *  14. -0 ** (odd integer) = -( +0 ** (odd integer) )
 *  15. +INF ** (+anything except 0,NAN) is +INF
 *  16. +INF ** (-anything except 0,NAN) is +0
 *  17. -INF ** (anything)  = -0 ** (-anything)
 *  18. (-anything) ** (integer) is (-1)**(integer)*(+anything**integer)
 *  19. (-anything except 0 and inf) ** (non-integer) is NAN
 *
 * Accuracy:
 *  pow(x,y) returns x**y nearly rounded. In particular, pow(integer,integer)
 *  always returns the correct integer provided it is representable.
 */
float64_t nvKmsPow(float64_t x, float64_t y)
{
    float64_t z, ax, z_h, z_l, p_h, p_l;
    float64_t y1, t1, t2, r, s, t, u, v, w;
    int i, j, k, yisint, n;
    int hx, hy, ix, iy;
    unsigned lx, ly;

    hx = __HI(x);
    lx = __LO(x);

    hy = __HI(y);
    ly = __LO(y);

    ix = hx & 0x7fffffff;
    iy = hy & 0x7fffffff;

    /* y==zero: x**0 = 1 */
    if ((iy | ly) == 0) {
        return one;
    }

    /* +-NaN return x+y */
    if ((ix > 0x7ff00000) || ((ix == 0x7ff00000) && (lx != 0)) ||
        (iy > 0x7ff00000) || ((iy == 0x7ff00000) && (ly != 0))) {
        return f64_add(x, y);
    }

    /*
     * Determine if y is an odd int when x < 0:
     * yisint = 0   ... y is not an integer
     * yisint = 1   ... y is an odd int
     * yisint = 2   ... y is an even int
     */
    yisint = 0;
    if (hx < 0) {
        if (iy >= 0x43400000) {
            yisint = 2; // even integer y
        } else if (iy >= 0x3ff00000) {
            k = (iy >> 20) - 0x3ff; // exponent
            if (k > 20) {
                j = ly >> (52 - k);
                if ((j << (52 - k)) == ly) {
                    yisint = 2 - (j & 1);
                }
            } else if (ly == 0) {
                j = iy >> (20 - k);
                if ((j << (20 -k )) == iy) {
                    yisint = 2 - (j & 1);
                }
            }
        }
    }

    /* special value of y */
    if (ly == 0) {
        if (iy == 0x7ff00000) { // y is +-inf
            if (((ix - 0x3ff00000) | lx) == 0) { // inf**+-1 is NaN
                return f64_sub(y, y);
            } else if (ix >= 0x3ff00000) { // (|x|>1)**+-inf = inf,0
                return (hy >= 0) ? y: zero;
            } else { // (|x|<1)**-,+inf = inf,0
                return (hy < 0) ? F64_negate(y): zero;
            }
        }
        if (iy == 0x3ff00000) { // y is +-1
            if (hy < 0) {
                return f64_div(one, x);
            } else {
                return x;
            }
        }
        if (hy == 0x40000000) { // y is 2
            return f64_mul(x, x);
        }
        if (hy == 0x3fe00000) { // y is 0.5
            if (hx >= 0) { // x >= +0
                return f64_sqrt(x);
            }
        }
    }

    ax = F64_fabs(x);
    /* special value of x */
    if (lx == 0) {
        // x is +-0,+-inf,+-1
        if ((ix == 0x7ff00000) || (ix == 0) || (ix == 0x3ff00000)) {
            z = ax;
            if (hy < 0) {
                z = f64_div(one, z); // z = (1/|x|)
            }
            if (hx < 0) {
                if (((ix - 0x3ff00000) | yisint) == 0) { // (-1)**non-int is NaN
                    z = nan;
                } else if (yisint == 1) { // (x<0)**odd = -(|x|**odd)
                    z = F64_negate(z);
                }
            }
            return z;
        }
    }

    n = (hx >> 31) + 1;

    /* (x<0)**(non-int) is NaN */
    if ((n | yisint) == 0) {
        return nan;
    }

    s = one; // s (sign of result -ve**odd) = -1 else = 1
    if ((n | (yisint - 1)) == 0) {
        s = F64_negate(one); // (-ve)**(odd int)
    }

    /* |y| is huge */
    if (iy > 0x41e00000) { // if |y| > 2**31
        if (iy > 0x43f00000){  // if |y| > 2**64, must o/uflow
            if (ix <= 0x3fefffff) {
                return (hy < 0) ? f64_mul(huge, huge) : f64_mul(tiny, tiny);
            }
            if (ix >= 0x3ff00000) {
                return (hy > 0) ? f64_mul(huge, huge) : f64_mul(tiny, tiny);
            }
        }
        /* over/underflow if x is not close to one */
        if (ix < 0x3fefffff) {
            return (hy < 0) ? f64_mul(f64_mul(s, huge), huge) :
                              f64_mul(f64_mul(s, tiny), tiny);
        }
        if (ix > 0x3ff00000) {
            return (hy > 0) ? f64_mul(f64_mul(s, huge), huge) :
                              f64_mul(f64_mul(s, tiny), tiny);
        }
        /*
         * now |1-x| is tiny <= 2**-20, suffice to compute
         * log(x) by x-x^2/2+x^3/3-x^4/4
         */
        t = f64_sub(ax, one); // t has 20 trailing zeros
        w = f64_mul(f64_mul(t, t), f64_sub(half, f64_mul(t, f64_sub(third, f64_mul(t, quarter)))));
        u = f64_mul(ivln2_h, t); // ivln2_h has 21 sig. bits
        v = f64_sub(f64_mul(t, ivln2_l), f64_mul(w, ivln2));
        t1 = f64_add(u, v);
        __LO(t1) = 0;
        t2 = f64_sub(v, f64_sub(t1, u));
    } else {
        float64_t ss, s2, s_h, s_l, t_h, t_l;
        n = 0;
        /* take care subnormal number */
        if (ix < 0x00100000) {
            ax = f64_mul(ax, two53);
            n -= 53;
            ix = __HI(ax);
        }
        n += ((ix) >> 20) - 0x3ff;
        j = ix & 0x000fffff;
        /* determine interval */
        ix = j | 0x3ff00000; // normalize ix
        if (j <= 0x3988E) { // |x|<sqrt(3/2)
            k = 0;
        } else if (j<0xBB67A) { // |x|<sqrt(3)
            k = 1;
        } else {
            k = 0;
            n += 1;
            ix -= 0x00100000;
        }
        __HI(ax) = ix;

        /* compute ss = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
        u = f64_sub(ax, bp[k]); // bp[0]=1.0, bp[1]=1.5
        v = f64_div(one, f64_add(ax, bp[k]));
        ss = f64_mul(u, v);
        s_h = ss;
        __LO(s_h) = 0;
        /* t_h=ax+bp[k] High */
        t_h = zero;
        __HI(t_h) = ((ix >> 1) | 0x20000000) + 0x00080000 + (k << 18);
        t_l = f64_sub(ax, f64_sub(t_h, bp[k]));
        s_l = f64_mul(v, f64_sub(f64_sub(u, f64_mul(s_h, t_h)), f64_mul(s_h, t_l)));
        /* compute log(ax) */
        s2 = f64_mul(ss, ss);
        r = f64_mul(f64_mul(s2, s2),f64_add(L1, f64_mul(s2, f64_add(L2, f64_mul(s2, f64_add(L3, f64_mul(s2, f64_add(L4, f64_mul(s2, f64_add(L5, f64_mul(s2, L6)))))))))));
        r = f64_add(r, f64_mul(s_l, f64_add(s_h, ss)));
        s2 = f64_mul(s_h, s_h);
        t_h = f64_add(f64_add(three, s2), r);
        __LO(t_h) = 0;
        t_l = f64_sub(r, (f64_sub(f64_sub(t_h, three), s2)));
        /* u+v = ss*(1+...) */
        u = f64_mul(s_h, t_h);
        v = f64_add(f64_mul(s_l, t_h), f64_mul(t_l, ss));
        /* 2/(3log2)*(ss+...) */
        p_h = f64_add(u, v);
        __LO(p_h) = 0;
        p_l = f64_sub(v, f64_sub(p_h, u));
        z_h = f64_mul(cp_h, p_h); // cp_h+cp_l = 2/(3*log2)
        z_l = f64_add(f64_add(f64_mul(cp_l, p_h), f64_mul(p_l, cp)), dp_l[k]);
        /* log2(ax) = (ss+..)*2/(3*log2) = n + dp_h + z_h + z_l */
        t = i32_to_f64(n);
        t1 = f64_add(f64_add(f64_add(z_h, z_l), dp_h[k]), t);
        __LO(t1) = 0;
        t2 = f64_sub(z_l, f64_sub(f64_sub(f64_sub(t1, t), dp_h[k]), z_h));
    }

    /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
    y1 = y;
    __LO(y1) = 0;
    p_l = f64_add(f64_mul(f64_sub(y, y1), t1), f64_mul(y, t2));
    p_h = f64_mul(y1, t1);
    z = f64_add(p_l, p_h);
    j = __HI(z);
    i = __LO(z);
    if (j >= 0x40900000) { // z >= 1024
        if (((j - 0x40900000) | i) != 0) { // if z > 1024
            return f64_mul(f64_mul(s, huge), huge); // overflow
        } else {
            if (f64_lt(f64_sub(z, p_h), f64_add(p_l, ovt))) {
                return f64_mul(f64_mul(s, huge), huge); // overflow
            }
        }
    } else if ((j & 0x7fffffff) >= 0x4090cc00) { // z <= -1075
        if (((j - 0xc090cc00) | i) != 0) { // z < -1075
            return f64_mul(f64_mul(s, tiny), tiny); // underflow
        } else {
            if (f64_le(p_l, f64_sub(z, p_h))) {
                return f64_mul(f64_mul(s, tiny), tiny); // underflow
            }
        }
    }
    /*
     * compute 2**(p_h+p_l)
     */
    i = j & 0x7fffffff;
    k = (i >> 20) - 0x3ff;
    n = 0;
    if (i > 0x3fe00000) { // if |z| > 0.5, set n = [z+0.5]
        n = j + (0x00100000 >> (k + 1));
        k = ((n & 0x7fffffff) >> 20) - 0x3ff; // new k for n
        t = zero;
        __HI(t) = (n & ~(0x000fffff >> k));
        n = ((n & 0x000fffff) | 0x00100000) >> (20 - k);
        if (j < 0) {
            n = -n;
        }
        p_h = f64_sub(p_h, t);
    }
    t = f64_add(p_l, p_h);
    __LO(t) = 0;
    u = f64_mul(t, lg2_h);
    v = f64_add(f64_mul(f64_sub(p_l, f64_sub(t, p_h)), lg2), f64_mul(t, lg2_l));
    z = f64_add(u, v);
    w = f64_sub(v, f64_sub(z, u));
    t = f64_mul(z, z);
    t1 = f64_sub(z, f64_mul(t, f64_add(P1, f64_mul(t, f64_add(P2, f64_mul(t, f64_add(P3, f64_mul(t, f64_add(P4, f64_mul(t, P5))))))))));
    r = f64_sub(f64_div(f64_mul(z, t1), f64_sub(t1, two)), f64_add(w, f64_mul(z, w)));
    z = f64_sub(one, f64_sub(r, z));
    j = __HI(z);
    j += (n << 20);
    if ((j >> 20) <= 0) {
        z = F64_scalbn(z,n); // subnormal output
    } else {
        __HI(z) += (n << 20);
    }
    return f64_mul(s, z);
}
