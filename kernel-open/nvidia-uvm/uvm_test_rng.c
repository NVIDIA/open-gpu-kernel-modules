/*******************************************************************************
    Copyright (c) 2015 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "uvm_test_rng.h"
#include "uvm_linux.h"

#include "uvm_test.h"

// George Marsaglia's RNG:
// https://groups.google.com/forum/#!msg/sci.stat.math/5yb0jwf1stw/ApaXM3IRy-0J
// https://groups.google.com/forum/#!msg/sci.math.num-analysis/yoaCpGWKEk0/UXCxgufdTesJ
//
// This is intended for testing purposes ONLY, not for anything which needs to
// be secure. get_random_bytes is not sufficient for testing purposes because we
// need reproducible sequences for testing. The prandom family would work fine
// but they aren't available on kernels < 2.6.35.

void uvm_test_rng_init(uvm_test_rng_t *rng, NvU32 seed)
{
    rng->z     = 362436069;
    rng->w     = 521288629;
    rng->jcong = 380116160;
    rng->jsr   = seed;
}

NvU32 uvm_test_rng_32(uvm_test_rng_t *rng)
{
    unsigned int mwc;

    rng->z = 36969*(rng->z & 65535) + (rng->z >> 16);

    rng->w = 18000*(rng->w & 65535) + (rng->w >> 16);

    rng->jcong = 69069*rng->jcong + 1234567;

    rng->jsr ^= (rng->jsr << 17);
    rng->jsr ^= (rng->jsr >> 13);
    rng->jsr ^= (rng->jsr <<  5);

    mwc = (rng->z << 16) + rng->w;

    return (mwc ^ rng->jcong) + rng->jsr;
}

NvU64 uvm_test_rng_64(uvm_test_rng_t *rng)
{
    NvU64 val64;
    val64 = uvm_test_rng_32(rng);
    val64 <<= 32;
    val64 |= uvm_test_rng_32(rng);
    return val64;
}

NvUPtr uvm_test_rng_ptr(uvm_test_rng_t *rng)
{
    if (sizeof(NvUPtr) == sizeof(NvU32))
        return uvm_test_rng_32(rng);
    return (NvUPtr)uvm_test_rng_64(rng);
}

// These range-based computations are subject to modulo bias, depending on the
// range. As described above, this is good enough for testing purposes.
NvU32 uvm_test_rng_range_32(uvm_test_rng_t *rng, NvU32 lo, NvU32 hi)
{
    if (lo == 0 && hi == ~0U)
        return uvm_test_rng_32(rng);
    return lo + (uvm_test_rng_32(rng) % (hi - lo + 1));
}

NvU64 uvm_test_rng_range_64(uvm_test_rng_t *rng, NvU64 lo, NvU64 hi)
{
    if (lo == 0 && hi == ~0ULL)
        return uvm_test_rng_64(rng);
    return lo + (uvm_test_rng_64(rng) % (hi - lo + 1));
}

NvUPtr uvm_test_rng_range_ptr(uvm_test_rng_t *rng, NvUPtr lo, NvUPtr hi)
{
    if (sizeof(NvUPtr) == sizeof(NvU32))
        return uvm_test_rng_range_32(rng, lo, hi);
    return (NvUPtr)uvm_test_rng_range_64(rng, lo, hi);
}


// Logarithmic distribution

NvU64 uvm_test_rng_log64(uvm_test_rng_t *rng)
{
    return uvm_test_rng_range_log64(rng, 0, ~0ULL);
}

NvU64 uvm_test_rng_range_log64(uvm_test_rng_t *rng, NvU64 lo, NvU64 hi)
{
    NvU32 log2_lo, log2_hi, rand_exp;
    NvU64 rand_lo, rand_hi;

    // This is a very rough approximation of a logarithmic distribution. It
    // weights each power of 2 covered in the range [lo, hi] equally, then
    // uses a uniform distribution to select a value with that power of 2.
    //
    // This means that if the input range is for example [32, 64], 64 will be
    // selected 50% of the time. The other 50% will be equally distributed among
    // the range [32, 63].
    //
    // A more mathematically-correct distribution requires doing fixed-point
    // exponentiation. That's more trouble than it's worth for the purposes of
    // selecting random ranges for testing, which is the current goal of this
    // implementation.

    if (hi == 0)
        return 0; // lo must also be 0

    // Compute the log2 floor of both lo and hi
    if (lo == 0)
        log2_lo = 0;
    else
        log2_lo = ilog2(lo);
    log2_hi = ilog2(hi);

    // Pick a random exponent. If lo is 0, offset things so we can use an
    // "exponent" value of 0 to return 0.
    rand_exp = uvm_test_rng_range_32(rng, log2_lo, log2_hi + (lo == 0));
    if (lo == 0) {
        if (rand_exp == 0)
            return 0;
        --rand_exp; // Didn't pick 0 so re-normalize the exponent
    }

    // Pick a random number in the range [2^rand_exp, 2^(rand_exp+1))
    rand_lo = 1ULL << rand_exp;
    if (rand_exp == 63) // Overflow on left-shift is undefined
        rand_hi = ~0ULL;
    else
        rand_hi = (1ULL << (rand_exp + 1)) - 1;

    // Clamp
    rand_lo = max(rand_lo, lo);
    rand_hi = min(rand_hi, hi);

    return uvm_test_rng_range_64(rng, rand_lo, rand_hi);
}

void uvm_test_rng_memset(uvm_test_rng_t *rng, void *ptr, size_t size)
{
    // This implementation is optimized to generate as few random numbers as
    // possible, and to write to memory in natively-aligned chunks. This means
    // the code is somewhat ugly because it has to handle all starting
    // alignments and sizes.

    // Easy casting
    union
    {
        NvUPtr     u;
        void       *pv;
        NvU8       *p8;
        NvUPtr     *p_native;
    } p, p_end;

    NvUPtr val;
    p.pv = ptr;
    p_end.u = p.u + size;

    // Initial bytes until we get aligned
    if ((p.u % sizeof(*p.p_native)) && p.u < p_end.u) {
        val = uvm_test_rng_ptr(rng);
        do {
            *p.p8++ = (NvU8)(val & 0xFF);
            val >>= 8;
        } while ((p.u % sizeof(*p.p_native)) && p.u < p_end.u);
    }

    // Aligned steady state
    while (p.p_native + 1 <= p_end.p_native) {
        val = uvm_test_rng_ptr(rng);
        *p.p_native++ = val;
    }

    // Unaligned cleanup at end
    if (p.p8 < p_end.p8) {
        val = uvm_test_rng_ptr(rng);
        do {
            *p.p8++ = (NvU8)(val & 0xFF);
            val >>= 8;
        } while (p.p8 < p_end.p8);
    }
}

// -------- Unit test --------

#define RNG_RANGE_TRIALS 10

typedef struct test_range32_struct
{
    NvU32 lo, hi;
} test_range32_t;

typedef struct test_range64_struct
{
    NvU64 lo, hi;
} test_range64_t;

static const test_range32_t test_ranges32[] =
{
    {0,             0},
    {0,             1},
    {0,             100},
    {0,             0x7fffffff},
    {0,             0x80000000},
    {0,             0xffffffff},
    {1,             1},
    {1,             2},
    {100,           100},
    {100,           0x80000000},
    {0xfffffffe,    0xffffffff},
    {0xffffffff,    0xffffffff},
};

static const test_range64_t test_ranges64[] =
{
    {0,                     0},
    {0,                     1},
    {0,                     100},
    {0,                     0xffffffff},
    {0,                     0x100000000ull},
    {0,                     0xffffffffffffffffull},
    {1,                     1},
    {1,                     2},
    {100,                   100},
    {100,                   0x800000000000ull},
    {0xfffffffffffffffeull, 0xffffffffffffffffull},
    {0xffffffffffffffffull, 0xffffffffffffffffull},
};

// Known initial sequences with seed == 0
static const NvU32 test_vals32[] =
{
    0xfa0ad9e5,
    0x50328964,
    0x68745401,
    0x346765d1
};

static const NvU64 test_vals64[] =
{
    0xfa0ad9e550328964,
    0x68745401346765d1,
    0x5ce392ad7cdff94e,
    0x4c75b15ad18c8d81
};

static const NvU64 test_vals_log64[] =
{
    0x68745401,
    0x34e,
    0x23f4ea57,
    0x587e5f3fc99332b
};

NV_STATUS uvm_test_rng_sanity(UVM_TEST_RNG_SANITY_PARAMS *params, struct file *file)
{
    uvm_test_rng_t rng;
    size_t i, j;
    NvU32 seed = 0;

    // Check known initial sequences
    uvm_test_rng_init(&rng, seed);
    for (i = 0; i < ARRAY_SIZE(test_vals32); i++)
        TEST_CHECK_RET(uvm_test_rng_32(&rng) == test_vals32[i]);

    uvm_test_rng_init(&rng, seed);
    for (i = 0; i < ARRAY_SIZE(test_vals64); i++)
        TEST_CHECK_RET(uvm_test_rng_64(&rng) == test_vals64[i]);

    uvm_test_rng_init(&rng, seed);
    for (i = 0; i < ARRAY_SIZE(test_vals64); i++)
        TEST_CHECK_RET(uvm_test_rng_ptr(&rng) == (NvUPtr)test_vals64[i]);

    uvm_test_rng_init(&rng, seed);
    for (i = 0; i < ARRAY_SIZE(test_vals_log64); i++)
        TEST_CHECK_RET(uvm_test_rng_log64(&rng) == test_vals_log64[i]);

    // Check memset
    uvm_test_rng_init(&rng, seed);
    for (i = 0; i < ARRAY_SIZE(test_vals64); i++) {
        NvU64 r;
        uvm_test_rng_memset(&rng, &r, sizeof(r));
        TEST_CHECK_RET(r == test_vals64[i]);
    }

    // Check that values fall within specified ranges
    uvm_test_rng_init(&rng, seed);
    for (i = 0; i < ARRAY_SIZE(test_ranges32); i++) {
        NvU32 lo = test_ranges32[i].lo;
        NvU32 hi = test_ranges32[i].hi;
        for (j = 0; j < RNG_RANGE_TRIALS; j++) {
            NvU32 r = uvm_test_rng_range_32(&rng, lo, hi);
            TEST_CHECK_RET(r >= lo && r <= hi);
        }
    }

    uvm_test_rng_init(&rng, seed);
    for (i = 0; i < ARRAY_SIZE(test_ranges64); i++) {
        NvU64 lo = test_ranges64[i].lo;
        NvU64 hi = test_ranges64[i].hi;
        for (j = 0; j < RNG_RANGE_TRIALS; j++) {
            NvU64 r;

            r = uvm_test_rng_range_64(&rng, lo, hi);
            TEST_CHECK_RET(r >= lo && r <= hi);

            r = uvm_test_rng_range_ptr(&rng, lo, hi);
            TEST_CHECK_RET((NvUPtr)r >= (NvUPtr)lo && (NvUPtr)r <= (NvUPtr)hi);

            r = uvm_test_rng_range_log64(&rng, lo, hi);
            TEST_CHECK_RET(r >= lo && r <= hi);
        }
    }

    return NV_OK;
}
