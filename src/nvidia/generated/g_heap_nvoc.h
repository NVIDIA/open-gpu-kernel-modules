
#ifndef _G_HEAP_NVOC_H_
#define _G_HEAP_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_heap_nvoc.h"

#ifndef _OBJHEAP_H_
#define _OBJHEAP_H_

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Defines and structures used for the Heap Object. The heap object    *
*       is responsible for allocating memory based on usage and memory      *
*       configuration.                                                      *
*                                                                           *
\***************************************************************************/

#include "nvlimits.h" // NV_MAX_SUBDEVICES
#include "gpu/mem_mgr/heap_base.h"
#include "core/core.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator.h"
#include "ctrl/ctrl2080/ctrl2080fb.h" // NV2080_CTRL_FB_OFFLINED_ADDRESS_INFO
#include "resserv/resserv.h"
#include "resserv/rs_resource.h"
#include "containers/btree.h"


struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



typedef struct
{
    NvU64 alignment;
    NvU64 allocLo;
    NvU64 allocAl;
    NvU64 allocHi;
    NvU64 allocSize;
    NvBool ignoreBankPlacement;
} OBJHEAP_ALLOC_DATA;

// New Stuff for WDDM
typedef struct
{
    NvU32   type;
    NvU32   flags;
    NvU32  *pHeight;
    NvU32  *pWidth;
    NvU32  *pPitch;
    NvU64  *pSize;
    NvU64  *pAlignment;
    NvU32  *pAttr;
    NvU32  *pAttr2;
    NvU32  *pKind;
    NvU64   pad;
} HEAP_ALLOC_HINT_PARAMS;

typedef struct
{
    NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS *pUserParams;
    NvU32            pad;
    NvU32            hwResId;
    void            *bindResultFunc;
    void            *pHandle;
    HWRESOURCE_INFO  hwResource;
} MEMORY_HW_RESOURCES_ALLOCATION_REQUEST;

//
// Growth placement and direction modifiers - the grow direction if a bank placement
// fails (bit 7) | grow direction within a bank (bit 6) | bank number (bits 0..5)
// gives the algorithm the information to try placing in the specified bank with the
// specified direction within a bank.  If a bank placement fails, use the grow direction
// to search for free space anywhere.  Each bank placement group (image, depth, ..)
// gets MEM_NUM_BANKS_TO_TRY such bytes in a NvU32.
//

// grow direction within a bank
#define BANK_MEM_GROW_UP                  0x00
#define BANK_MEM_GROW_DOWN                0x40
#define BANK_MEM_GROW_MASK                0x40

// grow direction if a bank placement fails
#define MEM_GROW_UP                       0x00
#define MEM_GROW_DOWN                     0x80
#define MEM_GROW_MASK                     0x80

// other defines
#define MEM_NUM_BANKS_TO_TRY              0x1 // check a max of 1 bank
#define MEM_BANK_DATA_SIZE                0x8 // store everything in a byte

//
// defines the number of NvU32's in the bank placement array, which defines
// what kinds of allocations go where (see heap.c)
// If more types need to be handled separately in terms of bank placement,
// increase this number, add another define, and add some code to heapCreate
// and heapAlloc
//
#define BANK_PLACEMENT_IMAGE                0
#define BANK_PLACEMENT_DEPTH                1
#define BANK_PLACEMENT_TEX_OVERLAY_FONT     2
#define BANK_PLACEMENT_OTHER                3
#define BANK_PLACEMENT_NUM_GROUPS           0x00000004

//
// Currently the HEAP_MEM_BLOCK refCount field is only 8 bits.
//
#define HEAP_MAX_REF_COUNT      0xFFFFFFFF

//
// any allocations done for internal RM data structures from the heap should be
// marked as one of the following values.
// this is required so the RM can report back the internal scratch memory allocations
// in NVOS32_FUNCTION_INFO_TYPE_ALLOC_BLOCKS::NVOS32_TYPE_RM_SCRATCH
//
#define HEAP_OWNER_RM_SCRATCH_BEGIN         0xDEAF0000
#define HEAP_OWNER_RM_CHANNEL_CTX_BUFFER    (HEAP_OWNER_RM_SCRATCH_BEGIN + 1)
#define HEAP_OWNER_RM_RESERVED_REGION       (HEAP_OWNER_RM_SCRATCH_BEGIN + 2)
#define HEAP_OWNER_RM_SCRATCH_END           (HEAP_OWNER_RM_SCRATCH_BEGIN + 3)   // make this the last

#define HEAP_OWNER_RM_KERNEL_CLIENT         (HEAP_OWNER_RM_SCRATCH_END + 1)
#define HEAP_OWNER_PMA_RESERVED_REGION      (HEAP_OWNER_RM_SCRATCH_END + 2)
#define HEAP_OWNER_RM_CLIENT_GENERIC        (HEAP_OWNER_RM_SCRATCH_END + 3)

//
// size of the texture buffer array, when more than 4 clients detected,
// kill one of the clients listed in the client texture buffer
//
#define MAX_TEXTURE_CLIENT_IDS  4


//
// HEAP object is being created for multiple usecases now. Initial heap object created during RM init manages the whole FB
// and there are usecases such as PhysicalMemorySuballocator which uses HEAP to manage its internal allocations. We need to
// differentiate these heaps to allow/block certain features such as scrub/PMA etc.
//
typedef enum
{
    HEAP_TYPE_RM_GLOBAL                    = 0x0,       // HEAP created by RM to manage entire FB
    HEAP_TYPE_PHYS_MEM_SUBALLOCATOR        = 0x1,       // HEAP created by clients to manage Physical Memory Suballocations
    HEAP_TYPE_PARTITION_LOCAL              = 0x2,       // HEAP created by RM to manage memory assigned to a SMC partition
} HEAP_TYPE_INTERNAL;

/*!
 * Structure to hold references to PhysMemSubAlloc resource
 */
typedef struct _def_physmemsuballoc_data
{
    void               *pObject;       // PMSA object
    MEMORY_DESCRIPTOR *pMemDesc;       // Parent memdesc from which memory managed by PMSA is allocated
} PHYS_MEM_SUBALLOCATOR_DATA;

typedef struct MEM_BLOCK MEM_BLOCK;
struct MEM_BLOCK
{
    NvBool allocedMemDesc;
    NvU8 reserved0;
    NvU8 reserved1;
    NvU32 owner;
    NvHandle mhandle;
    NvU64 begin;
    NvU64 align;
    NvU64 alignPad; // padding to beginning of surface from aligned start (hack for NV50 perf work)
    NvU64 end;
    NvU32 textureId;
    NvU32 format;
    NvU32 pitch;     // allocated surface pitch, needed for realloc
    NvU32 height;    // allocated surface height, needed for realloc
    NvU32 width;     // allocated surface width, needed for realloc
    NvU32 refCount;
    NODE node;
    MEMORY_DESCRIPTOR *pMemDesc;    // Back pointer to the memory descriptor for this allocation
    HWRESOURCE_INFO hwResource;
    union
    {
        NvU32     type;
        MEM_BLOCK *prevFree;
    } u0;
    union
    {
        MEM_BLOCK *nextFree;
    } u1;
    MEM_BLOCK *prev;
    MEM_BLOCK *next;

    // hooks into noncontig block freelist
    MEM_BLOCK *nextFreeNoncontig;
    MEM_BLOCK *prevFreeNoncontig;
    MEM_BLOCK *noncontigAllocListNext;
};

typedef struct TEX_INFO
{
    NvU32 clientId;                 // texture client id
    NvU32 refCount;                 // how many textures have been allocated wrt this client
    NvU8 placementFlags;            // how texture is grown
    NvBool mostRecentAllocatedFlag; // most recently allocated client
} TEX_INFO;

#define NV_HEAP_PAGE_OFFLINE_TYPE           31:29
#define NV_HEAP_PAGE_OFFLINE_PAGE_NUMBER    27:0

typedef struct
{
    MEMORY_DESCRIPTOR  *pMemDesc;                // memory descriptor for the blacklisted page
    NvU64               physOffset;              // physical offset of blacklisted FB address
    NvU64               size;                    // size of the blacklisted page
    NvBool              bIsValid;                // If the blacklisted address is still managed by RM
    NvBool              bPendingRetirement;      // if the dynamically blacklisted pages is pending to be retired.
} BLACKLIST_CHUNK;

typedef struct
{
    BLACKLIST_CHUNK    *pBlacklistChunks;
    NvU32               count;
} BLACKLIST;

typedef struct
{
    NvU32 count;
    BLACKLIST_ADDRESS* data;
} BLACKLIST_ADDRESSES;

#define SHUFFLE_STRIDE_MAX 5


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_HEAP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Heap;
struct NVOC_METADATA__Object;


struct Heap {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Heap *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct Heap *__nvoc_pbase_Heap;    // heap

    // 2 PDB properties
    NvBool PDB_PROP_HEAP_NONCONTIG_ALLOC_BY_DEFAULT;
    NvBool PDB_PROP_HEAP_PAGE_SHUFFLE;

    // Data members
    HEAP_TYPE_INTERNAL heapType;
    void *pHeapTypeSpecificData;
    NvU64 refCount;
    NvBool bHasFbRegions;
    NvU64 base;
    NvU64 total;
    NvU64 free;
    NvU64 reserved;
    struct MEM_BLOCK *pBlockList;
    struct MEM_BLOCK *pFreeBlockList;
    NODE *pBlockTree;
    TEX_INFO textureData[4];
    struct MEM_BLOCK *pNoncontigFreeBlockList;
    BLACKLIST_ADDRESSES blackListAddresses;
    BLACKLIST blackList;
    NvU32 dynamicBlacklistSize;
    NvU32 staticBlacklistSize;
    NvU32 placementStrategy[4];
    NvU32 shuffleStrides[5];
    NvU32 shuffleStrideIndex;
    PMA pmaObject;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Heap {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_Heap_TYPEDEF__
#define __NVOC_CLASS_Heap_TYPEDEF__
typedef struct Heap Heap;
#endif /* __NVOC_CLASS_Heap_TYPEDEF__ */

#ifndef __nvoc_class_id_Heap
#define __nvoc_class_id_Heap 0x556e9a
#endif /* __nvoc_class_id_Heap */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Heap;

#define __staticCast_Heap(pThis) \
    ((pThis)->__nvoc_pbase_Heap)

#ifdef __nvoc_heap_h_disabled
#define __dynamicCast_Heap(pThis) ((Heap*) NULL)
#else //__nvoc_heap_h_disabled
#define __dynamicCast_Heap(pThis) \
    ((Heap*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Heap)))
#endif //__nvoc_heap_h_disabled

// Property macros
#define PDB_PROP_HEAP_NONCONTIG_ALLOC_BY_DEFAULT_BASE_CAST
#define PDB_PROP_HEAP_NONCONTIG_ALLOC_BY_DEFAULT_BASE_NAME PDB_PROP_HEAP_NONCONTIG_ALLOC_BY_DEFAULT
#define PDB_PROP_HEAP_PAGE_SHUFFLE_BASE_CAST
#define PDB_PROP_HEAP_PAGE_SHUFFLE_BASE_NAME PDB_PROP_HEAP_PAGE_SHUFFLE

NV_STATUS __nvoc_objCreateDynamic_Heap(Heap**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Heap(Heap**, Dynamic*, NvU32);
#define __objCreate_Heap(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_Heap((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS heapInit_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4, HEAP_TYPE_INTERNAL arg5, NvU32 arg6, void *arg7);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapInit(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4, HEAP_TYPE_INTERNAL arg5, NvU32 arg6, void *arg7) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapInit(arg1, arg2, arg3, arg4, arg5, arg6, arg7) heapInit_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapInitInternal_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4, HEAP_TYPE_INTERNAL arg5, void *arg6);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapInitInternal(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4, HEAP_TYPE_INTERNAL arg5, void *arg6) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapInitInternal(arg1, arg2, arg3, arg4, arg5, arg6) heapInitInternal_IMPL(arg1, arg2, arg3, arg4, arg5, arg6)
#endif //__nvoc_heap_h_disabled

void heapDestruct_IMPL(struct Heap *arg1);

#define __nvoc_heapDestruct(arg1) heapDestruct_IMPL(arg1)
NV_STATUS heapAlloc_IMPL(struct OBJGPU *arg1, NvHandle arg2, struct Heap *arg3, MEMORY_ALLOCATION_REQUEST *arg4, NvHandle arg5, OBJHEAP_ALLOC_DATA *arg6, FB_ALLOC_INFO *arg7, HWRESOURCE_INFO **arg8, NvBool *arg9, NvBool arg10, NvBool arg11);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapAlloc(struct OBJGPU *arg1, NvHandle arg2, struct Heap *arg3, MEMORY_ALLOCATION_REQUEST *arg4, NvHandle arg5, OBJHEAP_ALLOC_DATA *arg6, FB_ALLOC_INFO *arg7, HWRESOURCE_INFO **arg8, NvBool *arg9, NvBool arg10, NvBool arg11) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapAlloc(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) heapAlloc_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapFree_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvHandle hClient, NvHandle hDevice, NvU32 owner, MEMORY_DESCRIPTOR *pMemDesc);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapFree(struct OBJGPU *arg1, struct Heap *arg2, NvHandle hClient, NvHandle hDevice, NvU32 owner, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapFree(arg1, arg2, hClient, hDevice, owner, pMemDesc) heapFree_IMPL(arg1, arg2, hClient, hDevice, owner, pMemDesc)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapReference_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvU32 arg3, MEMORY_DESCRIPTOR *arg4);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapReference(struct OBJGPU *arg1, struct Heap *arg2, NvU32 arg3, MEMORY_DESCRIPTOR *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapReference(arg1, arg2, arg3, arg4) heapReference_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapInfo_IMPL(struct Heap *arg1, NvU64 *arg2, NvU64 *arg3, NvU64 *arg4, NvU64 *arg5, NvU64 *arg6);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapInfo(struct Heap *arg1, NvU64 *arg2, NvU64 *arg3, NvU64 *arg4, NvU64 *arg5, NvU64 *arg6) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapInfo(arg1, arg2, arg3, arg4, arg5, arg6) heapInfo_IMPL(arg1, arg2, arg3, arg4, arg5, arg6)
#endif //__nvoc_heap_h_disabled

void heapGetClientAddrSpaceSize_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvU64 *arg3);

#ifdef __nvoc_heap_h_disabled
static inline void heapGetClientAddrSpaceSize(struct OBJGPU *arg1, struct Heap *arg2, NvU64 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
}
#else //__nvoc_heap_h_disabled
#define heapGetClientAddrSpaceSize(arg1, arg2, arg3) heapGetClientAddrSpaceSize_IMPL(arg1, arg2, arg3)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapGetSize_IMPL(struct Heap *arg1, NvU64 *arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapGetSize(struct Heap *arg1, NvU64 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapGetSize(arg1, arg2) heapGetSize_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapGetFree_IMPL(struct Heap *arg1, NvU64 *arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapGetFree(struct Heap *arg1, NvU64 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapGetFree(arg1, arg2) heapGetFree_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapGetUsableSize_IMPL(struct Heap *arg1, NvU64 *arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapGetUsableSize(struct Heap *arg1, NvU64 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapGetUsableSize(arg1, arg2) heapGetUsableSize_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapGetBase_IMPL(struct Heap *arg1, NvU64 *arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapGetBase(struct Heap *arg1, NvU64 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapGetBase(arg1, arg2) heapGetBase_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapGetBlock_IMPL(struct Heap *arg1, NvU64 arg2, struct MEM_BLOCK **arg3);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapGetBlock(struct Heap *arg1, NvU64 arg2, struct MEM_BLOCK **arg3) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapGetBlock(arg1, arg2, arg3) heapGetBlock_IMPL(arg1, arg2, arg3)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapAllocHint_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvHandle arg3, NvHandle arg4, HEAP_ALLOC_HINT_PARAMS *arg5);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapAllocHint(struct OBJGPU *arg1, struct Heap *arg2, NvHandle arg3, NvHandle arg4, HEAP_ALLOC_HINT_PARAMS *arg5) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapAllocHint(arg1, arg2, arg3, arg4, arg5) heapAllocHint_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapHwAlloc_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvHandle arg3, NvHandle arg4, NvHandle arg5, MEMORY_HW_RESOURCES_ALLOCATION_REQUEST *arg6, NvU32 *arg7, NvU32 *arg8);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapHwAlloc(struct OBJGPU *arg1, struct Heap *arg2, NvHandle arg3, NvHandle arg4, NvHandle arg5, MEMORY_HW_RESOURCES_ALLOCATION_REQUEST *arg6, NvU32 *arg7, NvU32 *arg8) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapHwAlloc(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) heapHwAlloc_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
#endif //__nvoc_heap_h_disabled

void heapHwFree_IMPL(struct OBJGPU *arg1, struct Heap *arg2, struct Memory *arg3, NvU32 arg4);

#ifdef __nvoc_heap_h_disabled
static inline void heapHwFree(struct OBJGPU *arg1, struct Heap *arg2, struct Memory *arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
}
#else //__nvoc_heap_h_disabled
#define heapHwFree(arg1, arg2, arg3, arg4) heapHwFree_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapInitRegistryOverrides_IMPL(struct OBJGPU *arg1, struct Heap *arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapInitRegistryOverrides(struct OBJGPU *arg1, struct Heap *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapInitRegistryOverrides(arg1, arg2) heapInitRegistryOverrides_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapBlackListPages_IMPL(struct OBJGPU *arg1, struct Heap *arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapBlackListPages(struct OBJGPU *arg1, struct Heap *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapBlackListPages(arg1, arg2) heapBlackListPages_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapFreeBlackListedPages_IMPL(struct OBJGPU *arg1, struct Heap *arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapFreeBlackListedPages(struct OBJGPU *arg1, struct Heap *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapFreeBlackListedPages(arg1, arg2) heapFreeBlackListedPages_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapAddPageToBlackList_IMPL(struct OBJGPU *pGpu, struct Heap *pHeap, NvU64 pageNumber, NvU32 type);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapAddPageToBlackList(struct OBJGPU *pGpu, struct Heap *pHeap, NvU64 pageNumber, NvU32 type) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapAddPageToBlackList(pGpu, pHeap, pageNumber, type) heapAddPageToBlackList_IMPL(pGpu, pHeap, pageNumber, type)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapStoreBlackList_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvU64 *arg3, NvU64 *arg4, NvU32 arg5);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapStoreBlackList(struct OBJGPU *arg1, struct Heap *arg2, NvU64 *arg3, NvU64 *arg4, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapStoreBlackList(arg1, arg2, arg3, arg4, arg5) heapStoreBlackList_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_heap_h_disabled

NvBool heapIsPmaManaged_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4);

#ifdef __nvoc_heap_h_disabled
static inline NvBool heapIsPmaManaged(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_FALSE;
}
#else //__nvoc_heap_h_disabled
#define heapIsPmaManaged(arg1, arg2, arg3, arg4) heapIsPmaManaged_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_heap_h_disabled

NvU32 heapAddRef_IMPL(struct Heap *arg1);

#ifdef __nvoc_heap_h_disabled
static inline NvU32 heapAddRef(struct Heap *arg1) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return 0;
}
#else //__nvoc_heap_h_disabled
#define heapAddRef(arg1) heapAddRef_IMPL(arg1)
#endif //__nvoc_heap_h_disabled

NvU32 heapRemoveRef_IMPL(struct Heap *arg1);

#ifdef __nvoc_heap_h_disabled
static inline NvU32 heapRemoveRef(struct Heap *arg1) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return 0;
}
#else //__nvoc_heap_h_disabled
#define heapRemoveRef(arg1) heapRemoveRef_IMPL(arg1)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapResize_IMPL(struct Heap *arg1, NvS64 arg2);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapResize(struct Heap *arg1, NvS64 arg2) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapResize(arg1, arg2) heapResize_IMPL(arg1, arg2)
#endif //__nvoc_heap_h_disabled

void heapFilterBlackListPages_IMPL(struct Heap *arg1, NvU64 arg2, NvU64 arg3);

#ifdef __nvoc_heap_h_disabled
static inline void heapFilterBlackListPages(struct Heap *arg1, NvU64 arg2, NvU64 arg3) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
}
#else //__nvoc_heap_h_disabled
#define heapFilterBlackListPages(arg1, arg2, arg3) heapFilterBlackListPages_IMPL(arg1, arg2, arg3)
#endif //__nvoc_heap_h_disabled

NV_STATUS heapStorePendingBlackList_IMPL(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4);

#ifdef __nvoc_heap_h_disabled
static inline NV_STATUS heapStorePendingBlackList(struct OBJGPU *arg1, struct Heap *arg2, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_FAILED_PRECOMP("Heap was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_heap_h_disabled
#define heapStorePendingBlackList(arg1, arg2, arg3, arg4) heapStorePendingBlackList_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_heap_h_disabled

#undef PRIVATE_FIELD


#endif // _OBJHEAP_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HEAP_NVOC_H_
