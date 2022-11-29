/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVCM.H                                                            *
*   Windows Configuration Manager defines and prototypes.                   *
*                                                                           *
* ***IMPORTANT***  The interfaces defined in this file are *deprecated*     *
* ***IMPORTANT***  in favor of RmControl.                                   *
* ***IMPORTANT***  Try hard to not use this file at all and definitely      *
* ***IMPORTANT***  do not add or modify interfaces here.                    *
*                  Ref: bug 488474: delete CFG and CFG_EX                   *
*                                                                           *
\***************************************************************************/

#ifndef _NVCM_H_
#define _NVCM_H_

#include "nvdeprecated.h"

#if NV_DEPRECATED_COMPAT(RM_CONFIG_GET_SET)

#ifdef __cplusplus
extern "C" {
#endif

#include "nvgputypes.h"
#ifndef _H2INC
#include "rmcd.h"
#endif

#include "nverror.h"

#define NV_ROBUST_CHANNEL_ALLOCFAIL_CLIENT      0x00000001
#define NV_ROBUST_CHANNEL_ALLOCFAIL_DEVICE      0x00000002
#define NV_ROBUST_CHANNEL_ALLOCFAIL_SUBDEVICE   0x00000004
#define NV_ROBUST_CHANNEL_ALLOCFAIL_CHANNEL     0x00000008
#define NV_ROBUST_CHANNEL_ALLOCFAIL_CTXDMA      0x00000010
#define NV_ROBUST_CHANNEL_ALLOCFAIL_EVENT       0x00000020
#define NV_ROBUST_CHANNEL_ALLOCFAIL_MEMORY      0x00000040
#define NV_ROBUST_CHANNEL_ALLOCFAIL_OBJECT      0x00000080
#define NV_ROBUST_CHANNEL_ALLOCFAIL_HEAP        0x00000100

#define NV_ROBUST_CHANNEL_BREAKONERROR_DEFAULT  0x00000000
#define NV_ROBUST_CHANNEL_BREAKONERROR_DISABLE  0x00000001
#define NV_ROBUST_CHANNEL_BREAKONERROR_ENABLE   0x00000002

#endif // NV_DEPRECATED_RM_CONFIG_GET_SET

#endif // _NVCM_H_
