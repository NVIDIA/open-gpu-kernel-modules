
#ifndef _G_FBSR_NVOC_H_
#define _G_FBSR_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_fbsr_nvoc.h"

#ifndef FBSR_H
#define FBSR_H

#include "core/core.h"
#include "gpu/gpu.h"



/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: OBJFBSR.H                                                         *
*       Defines and structures used for the FB Save/Restore Engine Object.  *
\***************************************************************************/

typedef enum
{
    FBSR_OP_SIZE_BUF = 0,
    FBSR_OP_SAVE,
    FBSR_OP_RESTORE,
    FBSR_OP_DESTROY,
} FBSR_OP_TYPE;

typedef struct _def_fbsr_node
{
    struct _def_fbsr_node *pNext;
    NvU32                  data[1];
} FBSR_NODE, *PFBSR_NODE;

typedef struct
{
    void*   sectionHandle;
    void*   pMdl;               // MDL
    NvP64   sysAddr;
    NvU64   maxLength;          // MAX VA size allocated
    NvU64   avblViewSz;         // Chunk of mapped view that's unprocessed i.e., not restored or can be saved.
} BACKINGSTORE_SECTION_INFO;

// Struct to hold info on a region being saved/restored
typedef struct
{
    MEMORY_DESCRIPTOR *pVidMemDesc;     // MEMORY_DESCRIPTOR of FBMEM region being saved/restored
    MEMORY_DESCRIPTOR *pSysMemDesc;     // MEMORY_DESCRIPTOR of SYSMEM region used to save FB region to/restore from
    NvU64              vidOffset;       // Offset in FBMEM region to save/restore
    NvU64              sysOffset;       // Offset in SYSMEM region to save to/restore from
    NvU64              size;            // Size of region being saved/restored
} FBSR_REGION_RECORD;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_FBSR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJFBSR {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJFBSR *__nvoc_pbase_OBJFBSR;    // fbsr

    // Vtable with 3 per-object function pointers
    NV_STATUS (*__fbsrBegin__)(struct OBJGPU *, struct OBJFBSR * /*this*/, FBSR_OP_TYPE);  // halified (2 hals) body
    NV_STATUS (*__fbsrEnd__)(struct OBJGPU *, struct OBJFBSR * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__fbsrSendMemsysProgramRawCompressionMode__)(struct OBJGPU *, struct OBJFBSR * /*this*/, NvBool);  // halified (2 hals) body

    // Data members
    NvU32 type;
    struct OBJCE *pCe;
    FBSR_OP_TYPE op;
    MEMORY_DESCRIPTOR *pSysMemDesc;
    FBSR_NODE *pSysMemNodeHead;
    FBSR_NODE *pSysMemNodeCurrent;
    BACKINGSTORE_SECTION_INFO pagedBufferInfo;
    FBSR_REGION_RECORD *pRegionRecords;
    NvU32 *pPinnedBuffer;
    NvU8 *pDmaBuffer;
    void *pMapCookie;
    NvU64 length;
    NvU64 sysOffset;
    NvU32 numRegions;
    NvBool bOperationFailed;
    NvBool bValid;
    NvBool bInitialized;
    NvBool bRawModeWasEnabled;
    MEMORY_DESCRIPTOR *pSysReservedMemDesc;
};

#ifndef __NVOC_CLASS_OBJFBSR_TYPEDEF__
#define __NVOC_CLASS_OBJFBSR_TYPEDEF__
typedef struct OBJFBSR OBJFBSR;
#endif /* __NVOC_CLASS_OBJFBSR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJFBSR
#define __nvoc_class_id_OBJFBSR 0xa30fe6
#endif /* __nvoc_class_id_OBJFBSR */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJFBSR;

#define __staticCast_OBJFBSR(pThis) \
    ((pThis)->__nvoc_pbase_OBJFBSR)

#ifdef __nvoc_fbsr_h_disabled
#define __dynamicCast_OBJFBSR(pThis) ((OBJFBSR*)NULL)
#else //__nvoc_fbsr_h_disabled
#define __dynamicCast_OBJFBSR(pThis) \
    ((OBJFBSR*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJFBSR)))
#endif //__nvoc_fbsr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJFBSR(OBJFBSR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJFBSR(OBJFBSR**, Dynamic*, NvU32);
#define __objCreate_OBJFBSR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJFBSR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define fbsrBegin_FNPTR(pFbsr) pFbsr->__fbsrBegin__
#define fbsrBegin(pGpu, pFbsr, op) fbsrBegin_DISPATCH(pGpu, pFbsr, op)
#define fbsrBegin_HAL(pGpu, pFbsr, op) fbsrBegin_DISPATCH(pGpu, pFbsr, op)
#define fbsrEnd_FNPTR(pFbsr) pFbsr->__fbsrEnd__
#define fbsrEnd(pGpu, pFbsr) fbsrEnd_DISPATCH(pGpu, pFbsr)
#define fbsrEnd_HAL(pGpu, pFbsr) fbsrEnd_DISPATCH(pGpu, pFbsr)
#define fbsrSendMemsysProgramRawCompressionMode_FNPTR(pFbsr) pFbsr->__fbsrSendMemsysProgramRawCompressionMode__
#define fbsrSendMemsysProgramRawCompressionMode(pGpu, pFbsr, bRawMode) fbsrSendMemsysProgramRawCompressionMode_DISPATCH(pGpu, pFbsr, bRawMode)
#define fbsrSendMemsysProgramRawCompressionMode_HAL(pGpu, pFbsr, bRawMode) fbsrSendMemsysProgramRawCompressionMode_DISPATCH(pGpu, pFbsr, bRawMode)

// Dispatch functions
static inline NV_STATUS fbsrBegin_DISPATCH(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, FBSR_OP_TYPE op) {
    return pFbsr->__fbsrBegin__(pGpu, pFbsr, op);
}

static inline NV_STATUS fbsrEnd_DISPATCH(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr) {
    return pFbsr->__fbsrEnd__(pGpu, pFbsr);
}

static inline NV_STATUS fbsrSendMemsysProgramRawCompressionMode_DISPATCH(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, NvBool bRawMode) {
    return pFbsr->__fbsrSendMemsysProgramRawCompressionMode__(pGpu, pFbsr, bRawMode);
}

NV_STATUS fbsrInit_GM107(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr);


#ifdef __nvoc_fbsr_h_disabled
static inline NV_STATUS fbsrInit(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr) {
    NV_ASSERT_FAILED_PRECOMP("OBJFBSR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fbsr_h_disabled
#define fbsrInit(pGpu, pFbsr) fbsrInit_GM107(pGpu, pFbsr)
#endif //__nvoc_fbsr_h_disabled

#define fbsrInit_HAL(pGpu, pFbsr) fbsrInit(pGpu, pFbsr)

void fbsrDestroy_GM107(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr);


#ifdef __nvoc_fbsr_h_disabled
static inline void fbsrDestroy(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr) {
    NV_ASSERT_FAILED_PRECOMP("OBJFBSR was disabled!");
}
#else //__nvoc_fbsr_h_disabled
#define fbsrDestroy(pGpu, pFbsr) fbsrDestroy_GM107(pGpu, pFbsr)
#endif //__nvoc_fbsr_h_disabled

#define fbsrDestroy_HAL(pGpu, pFbsr) fbsrDestroy(pGpu, pFbsr)

void fbsrCopyMemoryMemDesc_GM107(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, MEMORY_DESCRIPTOR *pVidMemDesc);


#ifdef __nvoc_fbsr_h_disabled
static inline void fbsrCopyMemoryMemDesc(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, MEMORY_DESCRIPTOR *pVidMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("OBJFBSR was disabled!");
}
#else //__nvoc_fbsr_h_disabled
#define fbsrCopyMemoryMemDesc(pGpu, pFbsr, pVidMemDesc) fbsrCopyMemoryMemDesc_GM107(pGpu, pFbsr, pVidMemDesc)
#endif //__nvoc_fbsr_h_disabled

#define fbsrCopyMemoryMemDesc_HAL(pGpu, pFbsr, pVidMemDesc) fbsrCopyMemoryMemDesc(pGpu, pFbsr, pVidMemDesc)

static inline NV_STATUS fbsrExecuteSaveRestore_46f6a7(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_fbsr_h_disabled
static inline NV_STATUS fbsrExecuteSaveRestore(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr) {
    NV_ASSERT_FAILED_PRECOMP("OBJFBSR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fbsr_h_disabled
#define fbsrExecuteSaveRestore(pGpu, pFbsr) fbsrExecuteSaveRestore_46f6a7(pGpu, pFbsr)
#endif //__nvoc_fbsr_h_disabled

#define fbsrExecuteSaveRestore_HAL(pGpu, pFbsr) fbsrExecuteSaveRestore(pGpu, pFbsr)

NV_STATUS fbsrBegin_GA100(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, FBSR_OP_TYPE op);

NV_STATUS fbsrBegin_GM107(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, FBSR_OP_TYPE op);

NV_STATUS fbsrEnd_GA100(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr);

NV_STATUS fbsrEnd_GM107(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr);

NV_STATUS fbsrSendMemsysProgramRawCompressionMode_GA100(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, NvBool bRawMode);

static inline NV_STATUS fbsrSendMemsysProgramRawCompressionMode_56cd7a(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, NvBool bRawMode) {
    return NV_OK;
}

NV_STATUS fbsrObjectInit_IMPL(struct OBJFBSR *pFbsr, NvU32 arg2);

#ifdef __nvoc_fbsr_h_disabled
static inline NV_STATUS fbsrObjectInit(struct OBJFBSR *pFbsr, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJFBSR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fbsr_h_disabled
#define fbsrObjectInit(pFbsr, arg2) fbsrObjectInit_IMPL(pFbsr, arg2)
#endif //__nvoc_fbsr_h_disabled

NV_STATUS fbsrReserveSysMemoryForPowerMgmt_IMPL(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, NvU64 arg3);

#ifdef __nvoc_fbsr_h_disabled
static inline NV_STATUS fbsrReserveSysMemoryForPowerMgmt(struct OBJGPU *pGpu, struct OBJFBSR *pFbsr, NvU64 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJFBSR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fbsr_h_disabled
#define fbsrReserveSysMemoryForPowerMgmt(pGpu, pFbsr, arg3) fbsrReserveSysMemoryForPowerMgmt_IMPL(pGpu, pFbsr, arg3)
#endif //__nvoc_fbsr_h_disabled

void fbsrFreeReservedSysMemoryForPowerMgmt_IMPL(struct OBJFBSR *pFbsr);

#ifdef __nvoc_fbsr_h_disabled
static inline void fbsrFreeReservedSysMemoryForPowerMgmt(struct OBJFBSR *pFbsr) {
    NV_ASSERT_FAILED_PRECOMP("OBJFBSR was disabled!");
}
#else //__nvoc_fbsr_h_disabled
#define fbsrFreeReservedSysMemoryForPowerMgmt(pFbsr) fbsrFreeReservedSysMemoryForPowerMgmt_IMPL(pFbsr)
#endif //__nvoc_fbsr_h_disabled

#undef PRIVATE_FIELD


// Method used for copying
#define FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED     0   // Pre-reserve paged region during boot. Map and pin the region and DMA copy to it from FB and unpin it.
#define FBSR_TYPE_WDDM_SLOW_CPU_PAGED                 1   // Pre-reserve paged region during boot. Map in chunks of 64K and DMA copy to a scratch space of 64K and cpu copy to the paged region
#define FBSR_TYPE_PAGED_DMA                           2   // Copy using DMA approach, allocated from paged pool
#define FBSR_TYPE_PERSISTENT                          3   // Copy using DMA approach, memory will be persistent after allocation to avoid from system VM fragmentation.
#define FBSR_TYPE_DMA                                 4   // Copy using DMA. Fastest.
#define FBSR_TYPE_CPU                                 5   // CPU. Used when we don't have enough resources for DMA.
#define FBSR_TYPE_FILE                                6   // DMA from FB to scratch sysmem buffer of 64K size , which in turn copies to temporary file backed system memory
#define FBSR_TYPE_GSP                                 7   // FBSR for GSP regions

#define NUM_FBSR_TYPES              (FBSR_TYPE_GSP + 1)   // FBSR_TYPE_GSP to be last FBSR Type, insert any new FBSR types above FBSR_TYPE_GSP

#endif // FBSR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FBSR_NVOC_H_
