
#ifndef _G_DISP_INST_MEM_NVOC_H_
#define _G_DISP_INST_MEM_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_disp_inst_mem_nvoc.h"

#ifndef DISPLAY_INSTANCE_MEMORY_H
#define DISPLAY_INSTANCE_MEMORY_H

/* ------------------------ Includes --------------------------------------- */
#include "nvtypes.h"
#include "nvoc/utility.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "gpu/mem_mgr/mem_desc.h"

/* ------------------------ Forward Declaration ---------------------------- */
typedef struct OBJEHEAP OBJEHEAP;

struct DispChannel;

#ifndef __NVOC_CLASS_DispChannel_TYPEDEF__
#define __NVOC_CLASS_DispChannel_TYPEDEF__
typedef struct DispChannel DispChannel;
#endif /* __NVOC_CLASS_DispChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3
#endif /* __nvoc_class_id_DispChannel */



struct ContextDma;

#ifndef __NVOC_CLASS_ContextDma_TYPEDEF__
#define __NVOC_CLASS_ContextDma_TYPEDEF__
typedef struct ContextDma ContextDma;
#endif /* __NVOC_CLASS_ContextDma_TYPEDEF__ */

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441b
#endif /* __nvoc_class_id_ContextDma */



/* ------------------------ Macros & Defines ------------------------------- */
#define KERNEL_DISPLAY_GET_INST_MEM(p)    ((p)->pInst)
#define DISP_INST_MEM_ALIGN               0x10000

/* ------------------------ Types definitions ------------------------------ */
/*!
 * A software hash table entry
 */
typedef struct
{
    struct ContextDma *pContextDma;
    struct DispChannel *pDispChannel;
} SW_HASH_TABLE_ENTRY;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_INST_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DisplayInstanceMemory {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct DisplayInstanceMemory *__nvoc_pbase_DisplayInstanceMemory;    // instmem

    // Vtable with 7 per-object function pointers
    void (*__instmemGetSize__)(OBJGPU *, struct DisplayInstanceMemory * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NvU32 (*__instmemGetHashTableBaseAddr__)(OBJGPU *, struct DisplayInstanceMemory * /*this*/);  // halified (2 hals) body
    NvBool (*__instmemIsValid__)(OBJGPU *, struct DisplayInstanceMemory * /*this*/, NvU32);  // halified (2 hals) body
    NvU32 (*__instmemGenerateHashTableData__)(OBJGPU *, struct DisplayInstanceMemory * /*this*/, NvU32, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__instmemHashFunc__)(OBJGPU *, struct DisplayInstanceMemory * /*this*/, NvHandle, NvHandle, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__instmemCommitContextDma__)(OBJGPU *, struct DisplayInstanceMemory * /*this*/, struct ContextDma *);  // halified (2 hals) body
    NV_STATUS (*__instmemUpdateContextDma__)(OBJGPU *, struct DisplayInstanceMemory * /*this*/, struct ContextDma *, NvU64 *, NvU64 *, NvHandle, NvU32);  // halified (2 hals) body

    // Data members
    NV_ADDRESS_SPACE instMemAddrSpace;
    NvU32 instMemAttr;
    NvU64 instMemBase;
    NvU32 instMemSize;
    MEMORY_DESCRIPTOR *pAllocedInstMemDesc;
    MEMORY_DESCRIPTOR *pInstMemDesc;
    void *pInstMem;
    NvU32 nHashTableEntries;
    NvU32 hashTableBaseAddr;
    SW_HASH_TABLE_ENTRY *pHashTable;
    OBJEHEAP *pInstHeap;
};

#ifndef __NVOC_CLASS_DisplayInstanceMemory_TYPEDEF__
#define __NVOC_CLASS_DisplayInstanceMemory_TYPEDEF__
typedef struct DisplayInstanceMemory DisplayInstanceMemory;
#endif /* __NVOC_CLASS_DisplayInstanceMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_DisplayInstanceMemory
#define __nvoc_class_id_DisplayInstanceMemory 0x8223e2
#endif /* __nvoc_class_id_DisplayInstanceMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayInstanceMemory;

#define __staticCast_DisplayInstanceMemory(pThis) \
    ((pThis)->__nvoc_pbase_DisplayInstanceMemory)

#ifdef __nvoc_disp_inst_mem_h_disabled
#define __dynamicCast_DisplayInstanceMemory(pThis) ((DisplayInstanceMemory*)NULL)
#else //__nvoc_disp_inst_mem_h_disabled
#define __dynamicCast_DisplayInstanceMemory(pThis) \
    ((DisplayInstanceMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DisplayInstanceMemory)))
#endif //__nvoc_disp_inst_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DisplayInstanceMemory(DisplayInstanceMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DisplayInstanceMemory(DisplayInstanceMemory**, Dynamic*, NvU32);
#define __objCreate_DisplayInstanceMemory(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_DisplayInstanceMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define instmemGetSize_FNPTR(pInstMem) pInstMem->__instmemGetSize__
#define instmemGetSize(pGpu, pInstMem, pTotalInstMemSize, pHashTableSize) instmemGetSize_DISPATCH(pGpu, pInstMem, pTotalInstMemSize, pHashTableSize)
#define instmemGetSize_HAL(pGpu, pInstMem, pTotalInstMemSize, pHashTableSize) instmemGetSize_DISPATCH(pGpu, pInstMem, pTotalInstMemSize, pHashTableSize)
#define instmemGetHashTableBaseAddr_FNPTR(pInstMem) pInstMem->__instmemGetHashTableBaseAddr__
#define instmemGetHashTableBaseAddr(pGpu, pInstMem) instmemGetHashTableBaseAddr_DISPATCH(pGpu, pInstMem)
#define instmemGetHashTableBaseAddr_HAL(pGpu, pInstMem) instmemGetHashTableBaseAddr_DISPATCH(pGpu, pInstMem)
#define instmemIsValid_FNPTR(pInstMem) pInstMem->__instmemIsValid__
#define instmemIsValid(pGpu, pInstMem, offset) instmemIsValid_DISPATCH(pGpu, pInstMem, offset)
#define instmemIsValid_HAL(pGpu, pInstMem, offset) instmemIsValid_DISPATCH(pGpu, pInstMem, offset)
#define instmemGenerateHashTableData_FNPTR(pInstMem) pInstMem->__instmemGenerateHashTableData__
#define instmemGenerateHashTableData(pGpu, pInstMem, hClient, offset, dispChannelNum) instmemGenerateHashTableData_DISPATCH(pGpu, pInstMem, hClient, offset, dispChannelNum)
#define instmemGenerateHashTableData_HAL(pGpu, pInstMem, hClient, offset, dispChannelNum) instmemGenerateHashTableData_DISPATCH(pGpu, pInstMem, hClient, offset, dispChannelNum)
#define instmemHashFunc_FNPTR(pInstMem) pInstMem->__instmemHashFunc__
#define instmemHashFunc(pGpu, pInstMem, hClient, hContextDma, dispChannelNum, result) instmemHashFunc_DISPATCH(pGpu, pInstMem, hClient, hContextDma, dispChannelNum, result)
#define instmemHashFunc_HAL(pGpu, pInstMem, hClient, hContextDma, dispChannelNum, result) instmemHashFunc_DISPATCH(pGpu, pInstMem, hClient, hContextDma, dispChannelNum, result)
#define instmemCommitContextDma_FNPTR(pInstMem) pInstMem->__instmemCommitContextDma__
#define instmemCommitContextDma(pGpu, pInstMem, pContextDma) instmemCommitContextDma_DISPATCH(pGpu, pInstMem, pContextDma)
#define instmemCommitContextDma_HAL(pGpu, pInstMem, pContextDma) instmemCommitContextDma_DISPATCH(pGpu, pInstMem, pContextDma)
#define instmemUpdateContextDma_FNPTR(pInstMem) pInstMem->__instmemUpdateContextDma__
#define instmemUpdateContextDma(pGpu, pInstMem, pContextDma, pNewAddress, pNewLimit, hMemory, comprInfo) instmemUpdateContextDma_DISPATCH(pGpu, pInstMem, pContextDma, pNewAddress, pNewLimit, hMemory, comprInfo)
#define instmemUpdateContextDma_HAL(pGpu, pInstMem, pContextDma, pNewAddress, pNewLimit, hMemory, comprInfo) instmemUpdateContextDma_DISPATCH(pGpu, pInstMem, pContextDma, pNewAddress, pNewLimit, hMemory, comprInfo)

// Dispatch functions
static inline void instmemGetSize_DISPATCH(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 *pTotalInstMemSize, NvU32 *pHashTableSize) {
    pInstMem->__instmemGetSize__(pGpu, pInstMem, pTotalInstMemSize, pHashTableSize);
}

static inline NvU32 instmemGetHashTableBaseAddr_DISPATCH(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem) {
    return pInstMem->__instmemGetHashTableBaseAddr__(pGpu, pInstMem);
}

static inline NvBool instmemIsValid_DISPATCH(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 offset) {
    return pInstMem->__instmemIsValid__(pGpu, pInstMem, offset);
}

static inline NvU32 instmemGenerateHashTableData_DISPATCH(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 hClient, NvU32 offset, NvU32 dispChannelNum) {
    return pInstMem->__instmemGenerateHashTableData__(pGpu, pInstMem, hClient, offset, dispChannelNum);
}

static inline NV_STATUS instmemHashFunc_DISPATCH(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvHandle hClient, NvHandle hContextDma, NvU32 dispChannelNum, NvU32 *result) {
    return pInstMem->__instmemHashFunc__(pGpu, pInstMem, hClient, hContextDma, dispChannelNum, result);
}

static inline NV_STATUS instmemCommitContextDma_DISPATCH(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma) {
    return pInstMem->__instmemCommitContextDma__(pGpu, pInstMem, pContextDma);
}

static inline NV_STATUS instmemUpdateContextDma_DISPATCH(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma, NvU64 *pNewAddress, NvU64 *pNewLimit, NvHandle hMemory, NvU32 comprInfo) {
    return pInstMem->__instmemUpdateContextDma__(pGpu, pInstMem, pContextDma, pNewAddress, pNewLimit, hMemory, comprInfo);
}

static inline void instmemDecommitContextDma_b3696a(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma) {
    return;
}


#ifdef __nvoc_disp_inst_mem_h_disabled
static inline void instmemDecommitContextDma(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemDecommitContextDma(pGpu, pInstMem, pContextDma) instmemDecommitContextDma_b3696a(pGpu, pInstMem, pContextDma)
#endif //__nvoc_disp_inst_mem_h_disabled

#define instmemDecommitContextDma_HAL(pGpu, pInstMem, pContextDma) instmemDecommitContextDma(pGpu, pInstMem, pContextDma)

void instmemGetSize_v03_00(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 *pTotalInstMemSize, NvU32 *pHashTableSize);

static inline void instmemGetSize_f2d351(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 *pTotalInstMemSize, NvU32 *pHashTableSize) {
    NV_ASSERT_PRECOMP(0);
}

NvU32 instmemGetHashTableBaseAddr_v03_00(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem);

static inline NvU32 instmemGetHashTableBaseAddr_4a4dee(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem) {
    return 0;
}

NvBool instmemIsValid_v03_00(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 offset);

static inline NvBool instmemIsValid_491d52(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 offset) {
    return ((NvBool)(0 != 0));
}

NvU32 instmemGenerateHashTableData_v03_00(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 hClient, NvU32 offset, NvU32 dispChannelNum);

static inline NvU32 instmemGenerateHashTableData_4a4dee(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 hClient, NvU32 offset, NvU32 dispChannelNum) {
    return 0;
}

NV_STATUS instmemHashFunc_v03_00(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvHandle hClient, NvHandle hContextDma, NvU32 dispChannelNum, NvU32 *result);

static inline NV_STATUS instmemHashFunc_46f6a7(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvHandle hClient, NvHandle hContextDma, NvU32 dispChannelNum, NvU32 *result) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS instmemCommitContextDma_v03_00(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma);

static inline NV_STATUS instmemCommitContextDma_46f6a7(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS instmemUpdateContextDma_v03_00(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma, NvU64 *pNewAddress, NvU64 *pNewLimit, NvHandle hMemory, NvU32 comprInfo);

static inline NV_STATUS instmemUpdateContextDma_46f6a7(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma, NvU64 *pNewAddress, NvU64 *pNewLimit, NvHandle hMemory, NvU32 comprInfo) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS instmemConstruct_IMPL(struct DisplayInstanceMemory *arg_pInstMem);

#define __nvoc_instmemConstruct(arg_pInstMem) instmemConstruct_IMPL(arg_pInstMem)
void instmemDestruct_IMPL(struct DisplayInstanceMemory *pInstMem);

#define __nvoc_instmemDestruct(pInstMem) instmemDestruct_IMPL(pInstMem)
NV_STATUS instmemStateInitLocked_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline NV_STATUS instmemStateInitLocked(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemStateInitLocked(pGpu, pInstMem) instmemStateInitLocked_IMPL(pGpu, pInstMem)
#endif //__nvoc_disp_inst_mem_h_disabled

void instmemStateDestroy_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline void instmemStateDestroy(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemStateDestroy(pGpu, pInstMem) instmemStateDestroy_IMPL(pGpu, pInstMem)
#endif //__nvoc_disp_inst_mem_h_disabled

NV_STATUS instmemStateLoad_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 flags);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline NV_STATUS instmemStateLoad(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemStateLoad(pGpu, pInstMem, flags) instmemStateLoad_IMPL(pGpu, pInstMem, flags)
#endif //__nvoc_disp_inst_mem_h_disabled

NV_STATUS instmemStateUnload_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 flags);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline NV_STATUS instmemStateUnload(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemStateUnload(pGpu, pInstMem, flags) instmemStateUnload_IMPL(pGpu, pInstMem, flags)
#endif //__nvoc_disp_inst_mem_h_disabled

void instmemSetMemory_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NV_ADDRESS_SPACE dispInstMemAddrSpace, NvU32 dispInstMemAttr, NvU64 dispInstMemBase, NvU32 dispInstMemSize);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline void instmemSetMemory(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, NV_ADDRESS_SPACE dispInstMemAddrSpace, NvU32 dispInstMemAttr, NvU64 dispInstMemBase, NvU32 dispInstMemSize) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemSetMemory(pGpu, pInstMem, dispInstMemAddrSpace, dispInstMemAttr, dispInstMemBase, dispInstMemSize) instmemSetMemory_IMPL(pGpu, pInstMem, dispInstMemAddrSpace, dispInstMemAttr, dispInstMemBase, dispInstMemSize)
#endif //__nvoc_disp_inst_mem_h_disabled

NV_STATUS instmemBindContextDma_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma, struct DispChannel *pDispChannel);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline NV_STATUS instmemBindContextDma(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemBindContextDma(pGpu, pInstMem, pContextDma, pDispChannel) instmemBindContextDma_IMPL(pGpu, pInstMem, pContextDma, pDispChannel)
#endif //__nvoc_disp_inst_mem_h_disabled

NV_STATUS instmemUnbindContextDma_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma, struct DispChannel *pDispChannel);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline NV_STATUS instmemUnbindContextDma(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemUnbindContextDma(pGpu, pInstMem, pContextDma, pDispChannel) instmemUnbindContextDma_IMPL(pGpu, pInstMem, pContextDma, pDispChannel)
#endif //__nvoc_disp_inst_mem_h_disabled

void instmemUnbindContextDmaFromAllChannels_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline void instmemUnbindContextDmaFromAllChannels(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct ContextDma *pContextDma) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemUnbindContextDmaFromAllChannels(pGpu, pInstMem, pContextDma) instmemUnbindContextDmaFromAllChannels_IMPL(pGpu, pInstMem, pContextDma)
#endif //__nvoc_disp_inst_mem_h_disabled

void instmemUnbindDispChannelContextDmas_IMPL(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct DispChannel *pDispChannel);

#ifdef __nvoc_disp_inst_mem_h_disabled
static inline void instmemUnbindDispChannelContextDmas(OBJGPU *pGpu, struct DisplayInstanceMemory *pInstMem, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("DisplayInstanceMemory was disabled!");
}
#else //__nvoc_disp_inst_mem_h_disabled
#define instmemUnbindDispChannelContextDmas(pGpu, pInstMem, pDispChannel) instmemUnbindDispChannelContextDmas_IMPL(pGpu, pInstMem, pDispChannel)
#endif //__nvoc_disp_inst_mem_h_disabled

#undef PRIVATE_FIELD


#endif // DISPLAY_INSTANCE_MEMORY_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DISP_INST_MEM_NVOC_H_
