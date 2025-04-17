/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __NVRMCONTEXT_H__
#define __NVRMCONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "rs_access.h"

/*
 * An RM wrapping structure to make RMAPI accessible from the RM abstraction
 * in one location/context to other locations/contexts in a unified way.
 *
 * An nvRMContext can be created on the fly or stored preinitialized in
 * objects. It has no mutable state, it's just a collection of static
 * parameters to make access to RM possible.
 *
 * This is a C interface for maximal compatibility, and it is intended to be
 * used both in kernel and userspace.
 *
 */
typedef struct nvRMContextRec nvRMContext, *nvRMContextPtr;

struct nvRMContextRec {
    /*
     * The RM client of the calling context. In normal circumstances, the
     * callee should pass this as the hClient argument.
     */
    NvU32 clientHandle;

    /* User data field for the caller: to be freely used. */
    void *owner;

    /*
     * RMAPI function wrappers: it's enough to only fill in those functions
     * that the caller will know that will be needed in each case. For now,
     * we list function pointers needed by nv_smg.c -- feel free to add more
     * RMAPI functions when necessary.
     */
    NvU32 (*allocRoot)          (nvRMContextPtr rmctx, NvU32 *phClient);
    NvU32 (*alloc)              (nvRMContextPtr rmctx, NvU32 hClient, NvU32 hParent, NvU32 hObject, NvU32 hClass, void *pAllocParms);
    NvU32 (*free)               (nvRMContextPtr rmctx, NvU32 hClient, NvU32 hParent, NvU32 hObject);
    NvU32 (*control)            (nvRMContextPtr rmctx, NvU32 hClient, NvU32 hObject, NvU32 cmd, void *pParams, NvU32 paramsSize);
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __NVRMCONTEXT_H__
