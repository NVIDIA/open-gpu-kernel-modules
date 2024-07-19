/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga100_dev_top_h__
#define __ga100_dev_top_h__


#define NV_PTOP_DEVICE_INFO2(i)                     (0x00022800+(i)*4) /* R--4A */
#define NV_PTOP_DEVICE_INFO_CFG                     0x000224FC         /* R--4R */
#define NV_PTOP_DEVICE_INFO_CFG_MAX_DEVICES         15:4               /* C--UF */
#define NV_PTOP_DEVICE_INFO_CFG_MAX_ROWS_PER_DEVICE 19:16              /* C--UF */
#define NV_PTOP_DEVICE_INFO_CFG_NUM_ROWS            31:20              /* C--UF */

#endif // __ga100_dev_top_h__
