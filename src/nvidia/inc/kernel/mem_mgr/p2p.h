/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _P2P_H_
#define _P2P_H_

#include "core/core.h"

NV_STATUS  RmP2PGetPages            (NvU64, NvU32, NvU64, NvU64, NvU64 *, NvU32 *, NvU32 *, NvU32 *, OBJGPU **, void *, void (*)(void *), void *);
NV_STATUS  RmP2PGetPagesWithoutCallbackRegistration (NvU64, NvU32, NvU64, NvU64, NvU64 *, NvU32 *, NvU32 *, NvU32 *, OBJGPU **, void *);
NV_STATUS  RmP2PGetPagesPersistent  (NvU64, NvU64, void **, NvU64 *, NvU32 *, void *, void *, void **);
NV_STATUS  RmP2PRegisterCallback    (NvU64, NvU64, NvU64, void *, void (*)(void *), void *);
NV_STATUS  RmP2PPutPages            (NvU64, NvU32, NvU64, void *);
NV_STATUS  RmP2PGetGpuByAddress     (NvU64, NvU64, OBJGPU **);
NV_STATUS  RmP2PPutPagesPersistent  (void *, void *, void *);

#endif
