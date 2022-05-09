/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga102_dev_ce_h__
#define __ga102_dev_ce_h__
#define NV_CE_PCE_MAP                                           0x00104028 /* R--4R */
#define NV_CE_PCE_MAP_VALUE                                           23:0 /* R-XVF */
#define NV_CE_HSH_PCE_MASK                                      0x0010404c /* C--4R */
#define NV_CE_HSH_PCE_MASK_VALUE                                      23:0 /* C--VF */
#define NV_CE_PCE2LCE_CONFIG__SIZE_1                                     6 /*       */
#define NV_CE_PCE2LCE_CONFIG_PCE_ASSIGNED_LCE_NONE              0x0000000f /* RW--V */
#define NV_CE_GRCE_CONFIG__SIZE_1                                       2 /*       */
#define NV_CE_GRCE_CONFIG_SHARED_LCE                                  3:0 /* RWIVF */
#define NV_CE_GRCE_CONFIG_SHARED_LCE_NONE                             0xf /* RW--V */
#define NV_CE_GRCE_CONFIG_SHARED                                    30:30 /* RWIVF */
#endif
