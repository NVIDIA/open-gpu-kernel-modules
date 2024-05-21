/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga100_dev_nv_xve_h__
#define __ga100_dev_nv_xve_h__
#define NV_PCFG                             0x00088FFF:0x00088000 /* RW--D */
#define NV_XVE_LINK_CONTROL_STATUS                     0x00000088 /* RW-4R */
#define NV_XVE_LINK_CONTROL_STATUS_LINK_SPEED               19:16 /* R--VF */
#define NV_XVE_DBG0                                                     0x0000084C /* RW-4R */
#define NV_XVE_DBG0_OUTSTANDING_DOWNSTREAM_READ_CNTR_RESET                     9:9 /* RWCVF */
#define NV_XVE_DBG0_OUTSTANDING_DOWNSTREAM_READ_CNTR_RESET_TRIGGER      0x00000001 /* -W--T */
#define NV_XVE_DBG0_OUTSTANDING_DOWNSTREAM_READ_CNTR_RESET_DONE         0x00000000 /* R-C-V */
#define NV_XVE_DBG0_OUTSTANDING_DOWNSTREAM_READ_CNTR_RESET_PENDING      0x00000001 /* R---V */
#define NV_XVE_DBG_CYA_0                               0x00000898 /* RW-4R */
#define NV_XVE_DBG_CYA_0_BAR0_ADDR_WIDTH                    19:19 /* RWCVF */
#define NV_XVE_DBG_CYA_0_BAR0_ADDR_WIDTH_32BIT         0x00000000 /* RWC-V */
#define NV_XVE_DBG_CYA_0_BAR0_ADDR_WIDTH_64BIT         0x00000001 /* RW--V */
#define NV_XVE_RESIZE_BAR1_CTRL                        0x00000BC0 /* RW-4R */
#define NV_XVE_RESIZE_BAR1_CTRL_BAR_SIZE                     12:8 /* RWIVF */
#define NV_XVE_RESIZE_BAR1_CTRL_BAR_SIZE_MIN           0x00000006 /* ----V */
#define NV_XVE_RESIZE_BAR1_CTRL_BAR_SIZE_MAX           0x00000013 /* ----V */
#endif
