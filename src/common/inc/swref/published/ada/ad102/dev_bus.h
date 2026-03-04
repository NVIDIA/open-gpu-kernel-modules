/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef ad102_dev_nv_bus_h
#define ad102_dev_nv_bus_h

#define NV_PBUS_SW_SCRATCH(i)                     (0x00001400+(i)*4) /* RW-4A */
#define NV_PBUS_SW_SCRATCH__SIZE_1                                64 /*       */
#define NV_PBUS_SW_SCRATCH_FIELD                                31:0 /* RWIVF */
#define NV_PBUS_SW_SCRATCH_FIELD_INIT                     0x00000000 /* RWI-V */
#define NV_PBUS_BAR1_BLOCK                               0x00001704 /* RW-4R */
#define NV_PBUS_BAR1_BLOCK_MAP                                 29:0 /*       */
#define NV_PBUS_BAR1_BLOCK_PTR                                 27:0 /* RWIUF */
#define NV_PBUS_BAR1_BLOCK_PTR_0                         0x00000000 /* RWI-V */
#define NV_PBUS_BAR1_BLOCK_TARGET                             29:28 /* RWIUF */
#define NV_PBUS_BAR1_BLOCK_TARGET_VID_MEM                0x00000000 /* RWI-V */
#define NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_COHERENT       0x00000002 /* RW--V */
#define NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_NONCOHERENT    0x00000003 /* RW--V */
#define NV_PBUS_BAR1_BLOCK_MODE                               31:31 /* RWIUF */
#define NV_PBUS_BAR1_BLOCK_MODE_PHYSICAL                 0x00000000 /* RWI-V */
#define NV_PBUS_BAR1_BLOCK_MODE_VIRTUAL                  0x00000001 /* RW--V */
#define NV_PBUS_BAR1_BLOCK_PTR_SHIFT                             12 /*       */

#endif // ad102_dev_nv_bus_h

