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

#ifndef __UVM_TEST_RNG_H__
#define __UVM_TEST_RNG_H__

#include "uvm_test_ioctl.h"
#include "uvm_linux.h"

// Seed-able RNG for generating test data

typedef struct
{
    unsigned int z;
    unsigned int w;
    unsigned int jsr;
    unsigned int jcong;
} uvm_test_rng_t;

void uvm_test_rng_init(uvm_test_rng_t *rng, NvU32 seed);

// Fill the input buffer with random data
void uvm_test_rng_memset(uvm_test_rng_t *rng, void *ptr, size_t size);

// Uniform distribution

NvU32  uvm_test_rng_32(uvm_test_rng_t *rng);
NvU64  uvm_test_rng_64(uvm_test_rng_t *rng);
NvUPtr uvm_test_rng_ptr(uvm_test_rng_t *rng);

NvU32  uvm_test_rng_range_32(uvm_test_rng_t *rng, NvU32 lo, NvU32 hi);
NvU64  uvm_test_rng_range_64(uvm_test_rng_t *rng, NvU64 lo, NvU64 hi);
NvUPtr uvm_test_rng_range_ptr(uvm_test_rng_t *rng, NvUPtr lo, NvUPtr hi);

// Logarithmic distribution

NvU64 uvm_test_rng_log64(uvm_test_rng_t *rng);
NvU64 uvm_test_rng_range_log64(uvm_test_rng_t *rng, NvU64 lo, NvU64 hi);

NV_STATUS uvm_test_rng_sanity(UVM_TEST_RNG_SANITY_PARAMS *params, struct file *file);

#endif // __UVM_TEST_RNG_H__
