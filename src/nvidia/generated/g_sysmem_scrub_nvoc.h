
#ifndef _G_SYSMEM_SCRUB_NVOC_H_
#define _G_SYSMEM_SCRUB_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#pragma once
#include "g_sysmem_scrub_nvoc.h"

#ifndef SYSMEM_SCRUB_H
#define SYSMEM_SCRUB_H

#include "core/core.h"
#include "gpu/gpu.h"


struct CeUtils;

#ifndef __NVOC_CLASS_CeUtils_TYPEDEF__
#define __NVOC_CLASS_CeUtils_TYPEDEF__
typedef struct CeUtils CeUtils;
#endif /* __NVOC_CLASS_CeUtils_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtils
#define __nvoc_class_id_CeUtils 0x8b8bae
#endif /* __nvoc_class_id_CeUtils */



struct SysmemScrubber;

#ifndef __NVOC_CLASS_SysmemScrubber_TYPEDEF__
#define __NVOC_CLASS_SysmemScrubber_TYPEDEF__
typedef struct SysmemScrubber SysmemScrubber;
#endif /* __NVOC_CLASS_SysmemScrubber_TYPEDEF__ */

#ifndef __nvoc_class_id_SysmemScrubber
#define __nvoc_class_id_SysmemScrubber 0x266962
#endif /* __nvoc_class_id_SysmemScrubber */



typedef struct
{
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 semaphoreValue;
    NODE listNode;
} SysScrubEntry;

MAKE_INTRUSIVE_LIST(SysScrubList, SysScrubEntry, listNode);

typedef struct
{
    // semaphore event handle doesn't take GPU lock
    PORT_SPINLOCK *pSpinlock;

    // spinlock needs to be taken to use pSysmemScrubber
    struct SysmemScrubber *pSysmemScrubber;

    NvU32 refCount;
    NvU32 bWorkerQueued;
} SysmemScrubberWorkerParams;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SYSMEM_SCRUB_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SysmemScrubber;
struct NVOC_METADATA__Object;


struct SysmemScrubber {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SysmemScrubber *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct SysmemScrubber *__nvoc_pbase_SysmemScrubber;    // sysmemscrub

    // Data members
    struct OBJGPU *pGpu;
    struct CeUtils *pCeUtils;
    SysScrubList asyncScrubList;
    NvBool bAsync;
    SysmemScrubberWorkerParams *pWorkerParams;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SysmemScrubber {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_SysmemScrubber_TYPEDEF__
#define __NVOC_CLASS_SysmemScrubber_TYPEDEF__
typedef struct SysmemScrubber SysmemScrubber;
#endif /* __NVOC_CLASS_SysmemScrubber_TYPEDEF__ */

#ifndef __nvoc_class_id_SysmemScrubber
#define __nvoc_class_id_SysmemScrubber 0x266962
#endif /* __nvoc_class_id_SysmemScrubber */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SysmemScrubber;

#define __staticCast_SysmemScrubber(pThis) \
    ((pThis)->__nvoc_pbase_SysmemScrubber)

#ifdef __nvoc_sysmem_scrub_h_disabled
#define __dynamicCast_SysmemScrubber(pThis) ((SysmemScrubber*) NULL)
#else //__nvoc_sysmem_scrub_h_disabled
#define __dynamicCast_SysmemScrubber(pThis) \
    ((SysmemScrubber*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SysmemScrubber)))
#endif //__nvoc_sysmem_scrub_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SysmemScrubber(SysmemScrubber**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SysmemScrubber(SysmemScrubber**, Dynamic*, NvU32, struct OBJGPU *arg_pGpu);
#define __objCreate_SysmemScrubber(ppNewObj, pParent, createFlags, arg_pGpu) \
    __nvoc_objCreate_SysmemScrubber((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pGpu)


// Wrapper macros for implementation functions
NV_STATUS sysmemscrubConstruct_IMPL(struct SysmemScrubber *arg_pSysmemScrubber, struct OBJGPU *arg_pGpu);
#define __nvoc_sysmemscrubConstruct(arg_pSysmemScrubber, arg_pGpu) sysmemscrubConstruct_IMPL(arg_pSysmemScrubber, arg_pGpu)

NV_STATUS sysmemscrubScrubAndFree_IMPL(struct SysmemScrubber *pSysmemScrubber, MEMORY_DESCRIPTOR *pMemDesc);
#ifdef __nvoc_sysmem_scrub_h_disabled
static inline NV_STATUS sysmemscrubScrubAndFree(struct SysmemScrubber *pSysmemScrubber, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("SysmemScrubber was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_sysmem_scrub_h_disabled
#define sysmemscrubScrubAndFree(pSysmemScrubber, pMemDesc) sysmemscrubScrubAndFree_IMPL(pSysmemScrubber, pMemDesc)
#endif // __nvoc_sysmem_scrub_h_disabled

void sysmemscrubDestruct_IMPL(struct SysmemScrubber *pSysmemScrubber);
#define __nvoc_sysmemscrubDestruct(pSysmemScrubber) sysmemscrubDestruct_IMPL(pSysmemScrubber)


// Wrapper macros for halified functions

// Dispatch functions
#undef PRIVATE_FIELD


#endif // SYSMEM_SCRUB_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SYSMEM_SCRUB_NVOC_H_
