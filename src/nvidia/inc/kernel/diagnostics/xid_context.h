/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef XID_CONTEXT_H
#define XID_CONTEXT_H 1

#include "nvtypes.h"

//!
//! Root cause information to print in specific cases.
//!
//! Some Xid strings must be kept "stable", so this information is
//! only printed in certain cases where we can break the stability, or where
//! that particular Xid string was not stable.
//!
//! It will always be okay to pass an all zero struct { 0 }
//!
typedef struct
{
    //!
    //! If nonzero, print this as a root cause of the current
    //! ROBUST_CHANNEL_PREEMPTIVE_REMOVAL
    //!
    NvU32 preemptiveRemovalPreviousXid;
} RcRootCause;


//!
//! Xid and context information about an Xid passed to KernelRM.
//!
//! This is NOT stable. The Xid printing mechanism must take care that Xid
//! strings which must remain stable continue to do so even some information
//! changes here.
//!
typedef struct
{
    //! Xid number.
    NvU32 xid;

    //! Additional root cause information valid only for certain Xids.
    RcRootCause rootCause;
} XidContext;

#endif // XID_CONTEXT_H
