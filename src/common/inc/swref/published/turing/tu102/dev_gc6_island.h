/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __tu102_dev_gc6_island_h__
#define __tu102_dev_gc6_island_h__

#define NV_PGC6                                                                          0x118fff:0x118000 /* RW--D */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK                                     0x00118128 /* RW-4R */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK_READ_PROTECTION                            3:0 /* RWIVF */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0                     0:0 /*       */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_ENABLE       0x00000001 /*       */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_DISABLE      0x00000000 /*       */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_03(i)                                          (0x00118214+(i)*4) /* RW-4A */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05(i)                                          (0x00118234+(i)*4) /* RW-4A */
#define NV_PGC6_BSI_SECURE_SCRATCH_14                                                           0x001180f8 /* RW-4R */

#endif // __tu102_dev_gc6_island_h__
