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

#ifndef DAC_P2061_H
#define DAC_P2061_H

/* ------------------------ Macros & Defines ------------------------------- */
// P2061 uses P2060's object.

// P2061 hal ifaces
NV_STATUS gsyncGetHouseSyncMode_P2061(OBJGPU *, DACEXTERNALDEVICE *, NvU8*);
NV_STATUS gsyncSetHouseSyncMode_P2061(OBJGPU *, DACEXTERNALDEVICE *, NvU8);
NV_STATUS gsyncGetCplStatus_P2061    (OBJGPU *, DACEXTERNALDEVICE *, GSYNCSTATUS, NvU32 *);
NV_STATUS gsyncSetSyncSkew_P2061_V204(OBJGPU *, DACEXTERNALDEVICE *, NvU32);
NV_STATUS gsyncGetSyncSkew_P2061_V204(OBJGPU *, DACEXTERNALDEVICE *, NvU32 *);

#endif // DAC_P2061_H
