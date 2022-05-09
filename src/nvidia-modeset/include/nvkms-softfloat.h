/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_SOFTFLOAT_H__
#define __NVKMS_SOFTFLOAT_H__

/*
 * This header file provides utility code built on top of the softfloat floating
 * point emulation library.
 */

#include "nv-softfloat.h"
#include "nvkms-api-types.h"

/*
 * A 3x3 row-major matrix of float32_t's.
 */
struct NvKmsMatrixF32 {
    float32_t m[3][3];
};

/*
 * A 3x4 row-major matrix of float32_t's.
 */
struct NvKms3x4MatrixF32 {
    float32_t m[3][4];
};

/*
 * Convert from an NvKmsMatrix (stores floating point values in NvU32s) to an
 * NvKmsMatrixF32 (stores floating point values in float32_t).
 */
static inline struct NvKmsMatrixF32 NvKmsMatrixToNvKmsMatrixF32(
    const struct NvKmsMatrix in)
{
    struct NvKmsMatrixF32 out =  { };
    int i, j;

    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            out.m[i][j] = NvU32viewAsF32(in.m[i][j]);
        }
    }

    return out;
}

/*
 * Compute the matrix product A * B, where A is a 3x3 matrix and B is a 3x4 matrix,
 * and return the resulting 3x4 matrix.
 */
static inline struct NvKms3x4MatrixF32 nvMultiply3x4Matrix(const struct NvKmsMatrixF32 *A,
                                                           const struct NvKms3x4MatrixF32 *B)
{
    struct NvKms3x4MatrixF32 C = { };
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 3; ++k) {
                C.m[i][j] = f32_mulAdd(A->m[i][k], B->m[k][j], C.m[i][j]);
            }
        }
    }

    return C;
}

/* return x**y */
float64_t nvKmsPow(float64_t x, float64_t y);

#endif /* __NVKMS_SOFTFLOAT_H__ */
