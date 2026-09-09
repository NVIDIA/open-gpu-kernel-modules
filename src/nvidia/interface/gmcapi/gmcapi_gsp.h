/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_GSP_H
#define GMCAPI_GSP_H

#include "gmcapi/gmcapi_gsp_config.h"
#include "gmcapi/gmcapi_gsp_config_unstable.h"

//
// GMCAPI_CMD_GSP_SUSPEND
// Kernel RM tells GSP-RM to suspend itself (driver unload, PM transition,
// GC6 entry, etc.).
//
// Fire and forget. The kernel does not wait for a GMCAPI response; it
// synchronizes by polling for the GSP processor to reach the suspended
// state via its existing chip-specific mechanism.
//
typedef struct GmcApiGspSuspendParams
{
    NvU64 flags;
} GmcApiGspSuspendParams;

//
// GSP is being suspended as part of a PM transition (D3, hibernate, GC6 entry, etc).
// When clear, this is a "regular" unload (driver unbind, fast-unload, etc).
//
#define GMCAPI_GSP_SUSPEND_FLAGS_PM_TRANSITION  (1ULL << 0)
// GSP is entering GC6.
#define GMCAPI_GSP_SUSPEND_FLAGS_GC6_ENTERING   (1ULL << 1)
// Hibernate / suspend-to-disk
#define GMCAPI_GSP_SUSPEND_FLAGS_HIBERNATE      (1ULL << 2)
// Skip non-essential teardown (e.g. Windows restart fast unload)
#define GMCAPI_GSP_SUSPEND_FLAGS_FAST           (1ULL << 3)
// Force GSP unload even when a previous step has flagged an error.
#define GMCAPI_GSP_SUSPEND_FLAGS_FORCE          (1ULL << 4)

//
// GMCAPI_CMD_GSP_RESUME_DONE
// GSP-RM event delivered to Kernel RM when GSP-RM has finished its resume
// work and is ready to service requests again.
//
typedef struct GmcApiGspResumeDoneParams
{
    NvU64     flags;
} GmcApiGspResumeDoneParams;

// GPU was resumed from D3-hot
#define GMCAPI_GSP_RESUME_DONE_FLAGS_D3HOT      (1ULL << 0)

#endif // GMCAPI_GSP_H
