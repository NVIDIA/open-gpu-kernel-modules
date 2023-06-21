/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_dev_gc6_island_h__
#define __gh100_dev_gc6_island_h__

#define NV_PGC6_SCI_SEC_TIMER_TIME_0                             0x00118f54 /* RW-4R */
#define NV_PGC6_SCI_SEC_TIMER_TIME_0_NSEC                              31:5 /* RWEUF */
#define NV_PGC6_SCI_SEC_TIMER_TIME_0_NSEC_ZERO                   0x00000000 /* RWE-V */

#define NV_PGC6_SCI_SEC_TIMER_TIME_1                             0x00118f58 /* RW-4R */
#define NV_PGC6_SCI_SEC_TIMER_TIME_1_NSEC                              28:0 /* RWEUF */
#define NV_PGC6_SCI_SEC_TIMER_TIME_1_NSEC_ZERO                   0x00000000 /* RWE-V */

#define NV_PGC6_SCI_SYS_TIMER_OFFSET_0                           0x00118df4 /* RW-4R */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_0_UPDATE                           0:0 /* RWEVF */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_0_UPDATE_DONE               0x00000000 /* R-E-V */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_0_UPDATE_TRIGGER            0x00000001 /* -W--T */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_0_NSEC                            31:5 /* RWEUF */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_0_NSEC_ZERO                 0x00000000 /* RWE-V */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_1                           0x00118df8 /* RW-4R */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_1_NSEC                            28:0 /* RWEUF */
#define NV_PGC6_SCI_SYS_TIMER_OFFSET_1_NSEC_ZERO                 0x00000000 /* RWE-V */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20                      0x001182cc /* RW-4R */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_DATA                       31:0 /* RWIVF */
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_20_DATA_INIT            0x00000000 /* RWI-V */

#endif // __gh100_dev_gc6_island_h__
