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

#ifndef __tu102_dev_bus_h__
#define __tu102_dev_bus_h__

#define NV_PBUS_VBIOS_SCRATCH(i)                  (0x00001400+(i)*4) /*       */

#define NV_PBUS_IFR_FMT_FIXED0                        0x00000000 /*       */
#define NV_PBUS_IFR_FMT_FIXED0_SIGNATURE                    31:0 /*       */
#define NV_PBUS_IFR_FMT_FIXED0_SIGNATURE_VALUE        0x4947564E /*       */
#define NV_PBUS_IFR_FMT_FIXED1                        0x00000004 /*       */
#define NV_PBUS_IFR_FMT_FIXED1_VERSIONSW                    15:8 /*       */
#define NV_PBUS_IFR_FMT_FIXED1_FIXED_DATA_SIZE             30:16 /*       */
#define NV_PBUS_IFR_FMT_FIXED2                        0x00000008 /*       */
#define NV_PBUS_IFR_FMT_FIXED2_TOTAL_DATA_SIZE              19:0 /*       */

#endif // __tu102_dev_bus_h__
