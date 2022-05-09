/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clc671_h_
#define _clc671_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVC671_DISP_SF_USER (0x000C671)

typedef volatile struct _clc671_tag0 {
    NvU32 dispSfUserOffset[0x400];
} _NvC671DispSfUser, NvC671DispSfUserMap;

#define NVC671_SF_HDMI_INFO_IDX_GENERIC_INFOFRAME                         0x00000001 /*       */
#define NVC671_SF_HDMI_INFO_IDX_VSI                                       0x00000004 /*       */
#define NVC671_SF_HDMI_INFO_CTRL(i,j)                 (0x000E0000-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC671_SF_HDMI_INFO_CTRL__SIZE_1                                         4 /*       */
#define NVC671_SF_HDMI_INFO_CTRL__SIZE_2                                         5 /*       */

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif // _clc671_h_
