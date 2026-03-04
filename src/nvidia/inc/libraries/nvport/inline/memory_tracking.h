/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief NvPort MEMORY module extension to track memory allocations
 *
 * This file is internal to NvPort MEMORY module.
 * @cond NVPORT_INTERNAL
 */

#ifndef _NVPORT_MEMORY_INTERNAL_H_
#define _NVPORT_MEMORY_INTERNAL_H_

#include "nvtypes.h"

#define portMemExTrackingGetActiveStats_SUPPORTED       PORT_MEM_TRACK_USE_COUNTER
#define portMemExTrackingGetTotalStats_SUPPORTED        PORT_MEM_TRACK_USE_COUNTER
#define portMemExTrackingGetPeakStats_SUPPORTED         PORT_MEM_TRACK_USE_COUNTER
#define portMemExTrackingGetNext_SUPPORTED              \
    (PORT_MEM_TRACK_USE_FENCEPOSTS & PORT_MEM_TRACK_USE_ALLOCLIST)
#define portMemExTrackingGetHeapSize_SUPPORTED          (NVOS_IS_LIBOS)
#define portMemGetLargestFreeChunkSize_SUPPORTED        (NVOS_IS_LIBOS)
#define portMemExValidate_SUPPORTED            0
#define portMemExValidateAllocations_SUPPORTED 0
#define portMemExFreeAll_SUPPORTED             0

/** @brief Untracked paged memory allocation, platform specific */
void *_portMemAllocPagedUntracked(NvLength lengthBytes);
/** @brief Untracked nonpaged memory allocation, platform specific */
void *_portMemAllocNonPagedUntracked(NvLength lengthBytes);
/** @brief Untracked memory free, platform specific */
void _portMemFreeUntracked(void *pMemory);
/** @brief Wrapper around pAlloc->_portAlloc() that tracks the allocation */
void *_portMemAllocatorAlloc(PORT_MEM_ALLOCATOR *pAlloc, NvLength length);
/** @brief Wrapper around pAlloc->_portFree() that tracks the allocation */
void _portMemAllocatorFree(PORT_MEM_ALLOCATOR *pAlloc, void *pMem);

#if PORT_MEM_TRACK_USE_LIMIT
/** @brief Initialize per VF tracking limit **/
void portMemInitializeAllocatorTrackingLimit(NvU32 gfid, NvLength limit, NvBool bLimitEnabled);
/** @brief Init per Gfid mem tracking **/
void portMemGfidTrackingInit(NvU32 gfid);
/** @brief Free per Gfid mem tracking **/
void portMemGfidTrackingFree(NvU32 gfid);
/** @brief Increment per Gfid LibOS mem tracking **/
void portMemLibosLimitInc(NvU32 gfid, NvLength size);
/** @brief Decrement per Gfid LibOS mem tracking **/
void portMemLibosLimitDec(NvU32 gfid, NvLength size);
/** @brief Check if per Gfid LibOS mem limit is exceeded by allocation **/
NvBool portMemLibosLimitExceeded(NvU32 gfid, NvLength size);
/** @brief Initialize per Gfid LibOS tracking limit **/
void portMemInitializeAllocatorTrackingLibosLimit(NvU32 gfid, NvLength limit);
#endif

#if PORT_MEM_TRACK_USE_LIMIT
#define PORT_MEM_LIMIT_MAX_GFID                 64
#define LIBOS_RW_LOCK_SIZE                      144
#endif

typedef struct PORT_MEM_COUNTER
{
    volatile NvU32    activeAllocs;
    volatile NvU32    totalAllocs;
    volatile NvU32    peakAllocs;
    volatile NvLength activeSize;
    volatile NvLength totalSize;
    volatile NvLength peakSize;
} PORT_MEM_COUNTER;

typedef struct PORT_MEM_FENCE_HEAD
{
    PORT_MEM_ALLOCATOR *pAllocator;
    NvU32 magic;
} PORT_MEM_FENCE_HEAD;

typedef struct PORT_MEM_FENCE_TAIL
{
    NvU32 magic;
} PORT_MEM_FENCE_TAIL;

typedef struct PORT_MEM_LIST
{
    struct PORT_MEM_LIST *pPrev;
    struct PORT_MEM_LIST *pNext;
} PORT_MEM_LIST;

#if PORT_MEM_TRACK_USE_CALLERINFO

#if PORT_MEM_TRACK_USE_CALLERINFO_IP

typedef NvUPtr PORT_MEM_CALLERINFO;
#define PORT_MEM_CALLERINFO_MAKE         portUtilGetIPAddress()

#else // PORT_MEM_TRACK_USE_CALLERINFO_IP

typedef struct PORT_MEM_CALLERINFO
{
    const char *file;
    const char *func;
    NvU32 line;
} PORT_MEM_CALLERINFO;

/** @note Needed since not all compilers support automatic struct creation */
static NV_INLINE PORT_MEM_CALLERINFO
_portMemCallerInfoMake
(
    const char *file,
    const char *func,
    NvU32       line
)
{
    PORT_MEM_CALLERINFO callerInfo;
    callerInfo.file = file;
    callerInfo.func = func;
    callerInfo.line = line;
    return callerInfo;
}

#define PORT_MEM_CALLERINFO_MAKE              \
    _portMemCallerInfoMake(__FILE__, __FUNCTION__, __LINE__)
#endif // PORT_MEM_TRACK_USE_CALLERINFO_IP

void *portMemAllocPaged_CallerInfo(NvLength, PORT_MEM_CALLERINFO);
void *portMemAllocNonPaged_CallerInfo(NvLength, PORT_MEM_CALLERINFO);
PORT_MEM_ALLOCATOR *portMemAllocatorCreatePaged_CallerInfo(PORT_MEM_CALLERINFO);
PORT_MEM_ALLOCATOR *portMemAllocatorCreateNonPaged_CallerInfo(PORT_MEM_CALLERINFO);
void portMemInitializeAllocatorTracking_CallerInfo(PORT_MEM_ALLOCATOR *, PORT_MEM_ALLOCATOR_TRACKING *, PORT_MEM_CALLERINFO);
void *_portMemAllocatorAlloc_CallerInfo(PORT_MEM_ALLOCATOR*, NvLength, PORT_MEM_CALLERINFO);
PORT_MEM_ALLOCATOR *portMemAllocatorCreateOnExistingBlock_CallerInfo(void *, NvLength, PORT_MEM_CALLERINFO);
#if portMemExAllocatorCreateLockedOnExistingBlock_SUPPORTED
PORT_MEM_ALLOCATOR *portMemExAllocatorCreateLockedOnExistingBlock_CallerInfo(void *, NvLength, void *, PORT_MEM_CALLERINFO);
#endif //portMemExAllocatorCreateLockedOnExistingBlock_SUPPORTED
#undef  PORT_ALLOC
#define PORT_ALLOC(pAlloc, length) \
    _portMemAllocatorAlloc_CallerInfo(pAlloc, length, PORT_MEM_CALLERINFO_MAKE)

#define portMemAllocPaged(size)                                                \
    portMemAllocPaged_CallerInfo((size), PORT_MEM_CALLERINFO_MAKE)
#define portMemAllocNonPaged(size)                                             \
    portMemAllocNonPaged_CallerInfo((size), PORT_MEM_CALLERINFO_MAKE)
#define portMemAllocatorCreatePaged()                                          \
    portMemAllocatorCreatePaged_CallerInfo(PORT_MEM_CALLERINFO_MAKE)
#define portMemAllocatorCreateNonPaged()                                       \
    portMemAllocatorCreateNonPaged_CallerInfo(PORT_MEM_CALLERINFO_MAKE)

#define portMemInitializeAllocatorTracking(pAlloc, pTrack)                     \
    portMemInitializeAllocatorTracking_CallerInfo(pAlloc, pTrack, PORT_MEM_CALLERINFO_MAKE)

#define portMemAllocatorCreateOnExistingBlock(pMem, size)                      \
    portMemAllocatorCreateOnExistingBlock_CallerInfo(pMem, size, PORT_MEM_CALLERINFO_MAKE)
#if portMemExAllocatorCreateLockedOnExistingBlock_SUPPORTED
#define portMemExAllocatorCreateLockedOnExistingBlock(pMem, size, pLock)        \
    portMemExAllocatorCreateLockedOnExistingBlock_CallerInfo(pMem, size, pLock, \
                                                    PORT_MEM_CALLERINFO_MAKE)
#endif //portMemExAllocatorCreateLockedOnExistingBlock_SUPPORTED
#else
#define PORT_MEM_CALLERINFO_MAKE
#endif // CALLERINFO


#if PORT_MEM_TRACK_USE_FENCEPOSTS || PORT_MEM_TRACK_USE_ALLOCLIST || PORT_MEM_TRACK_USE_CALLERINFO || PORT_MEM_TRACK_USE_LIMIT

//
// The blockSize of the allocation is tracked in PORT_MEM_HEADER::blockSize
// when fenceposts or per-GFID limit tracking is enabled.
//
#define PORT_MEM_HEADER_HAS_BLOCK_SIZE  \
    (PORT_MEM_TRACK_USE_FENCEPOSTS || PORT_MEM_TRACK_USE_LIMIT)

typedef struct PORT_MEM_HEADER
{
#if PORT_MEM_HEADER_HAS_BLOCK_SIZE
                                    NvLength blockSize;
#endif
#if PORT_MEM_TRACK_USE_CALLERINFO
                                    PORT_MEM_CALLERINFO callerInfo;
#endif
#if PORT_MEM_TRACK_USE_ALLOCLIST
                                    PORT_MEM_LIST list;
#endif
#if PORT_MEM_TRACK_USE_FENCEPOSTS
                                    PORT_MEM_FENCE_HEAD fence;
#endif
#if PORT_MEM_TRACK_USE_LIMIT
                                    NV_DECLARE_ALIGNED(NvU32 gfid, 8);
#endif
} PORT_MEM_HEADER;

typedef struct PORT_MEM_FOOTER
{
#if PORT_MEM_TRACK_USE_FENCEPOSTS
                                    PORT_MEM_FENCE_TAIL fence;
#endif
} PORT_MEM_FOOTER;

#define PORT_MEM_ADD_HEADER_PTR(p)   ((PORT_MEM_HEADER*)p + 1)
#define PORT_MEM_SUB_HEADER_PTR(p)   ((PORT_MEM_HEADER*)p - 1)
#define PORT_MEM_STAGING_SIZE (sizeof(PORT_MEM_HEADER)+sizeof(PORT_MEM_FOOTER))
#else
#define PORT_MEM_ADD_HEADER_PTR(p)              p
#define PORT_MEM_SUB_HEADER_PTR(p)              p
#define PORT_MEM_STAGING_SIZE                   0
#define PORT_MEM_HEADER_HAS_BLOCK_SIZE          0
#endif

#define PORT_MEM_TRACK_ALLOC_SIZE                                   \
    PORT_MEM_TRACK_USE_COUNTER &&                                   \
    PORT_MEM_HEADER_HAS_BLOCK_SIZE

struct PORT_MEM_ALLOCATOR_TRACKING
{
    PORT_MEM_ALLOCATOR                 *pAllocator;
    struct PORT_MEM_ALLOCATOR_TRACKING *pPrev;
    struct PORT_MEM_ALLOCATOR_TRACKING *pNext;

#if PORT_MEM_TRACK_USE_COUNTER
    PORT_MEM_COUNTER                    counter;
#endif
#if PORT_MEM_TRACK_USE_ALLOCLIST
    PORT_MEM_LIST                      *pFirstAlloc;
    void                               *listLock;
#endif
#if PORT_MEM_TRACK_USE_CALLERINFO
    PORT_MEM_CALLERINFO                 callerInfo;
#endif
#if PORT_MEM_TRACK_USE_LIMIT
    NvLength                            limitGfid;
    NvLength                            counterGfid;
    NvU32                               gfid;
    NvLength                            limitLibosGfid;
    NvLength                            counterLibosGfid;
#endif
};

/// @brief Actual size of an allocator structure, including internals
#define PORT_MEM_ALLOCATOR_SIZE \
    (sizeof(PORT_MEM_ALLOCATOR) + sizeof(PORT_MEM_ALLOCATOR_TRACKING))

#if defined(BIT)
#define NVIDIA_UNDEF_LEGACY_BIT_MACROS
#endif
#include "nvmisc.h"

//
// Internal bitvector structures for allocators over existing blocks
//
#define PORT_MEM_BITVECTOR_CHUNK_SIZE 16U
typedef NvU8 PORT_MEM_BITVECTOR_CHUNK[PORT_MEM_BITVECTOR_CHUNK_SIZE];
typedef struct
{
    //
    // Points to a PORT_SPINLOCK that make memory thread safe.
    // If this is not thread safe variant, then it is NULL.
    //
    void *pSpinlock;
    // Points to after the bitvector, aligned to first chunk.
    PORT_MEM_BITVECTOR_CHUNK *pChunks;
    NvU32 numChunks;
    //
    // What follows are two bitvectors one next to another:
    //  - The first represents availability of chunks: 0=free, 1=allocated
    //  - The second represents allocation sizes: 1=last chunk of an allocation
    // So the total size of this array is 2*numChunks bits
    // The second vector continues immediately after the first, no alignment
    //
    // Example: numChunks = 8, 2 allocations of 3 chunks each:
    // bits == |11111100| <- 2*3 chunks allocated, 2 free
    //         |00100100| <- Chunks 2 and 5 are last in allocation
    //
    NvU32 bits[NV_ANYSIZE_ARRAY];
} PORT_MEM_BITVECTOR;

/// @note the following can be used as arguments for static array size, so
/// they must be fully known at compile time - macros, not inline functions

/// @brief Total number of chunks in a preallocated block of given size
#define PORT_MEM_PREALLOCATED_BLOCK_NUM_CHUNKS(size) \
    NV_DIV_AND_CEIL(size, PORT_MEM_BITVECTOR_CHUNK_SIZE)

/// @brief Minimal nonaligned bookkeeping size required for a preallocated block
#define PORT_MEM_PREALLOCATED_BLOCK_MINIMAL_NONALIGNED_EXTRA_SIZE \
    sizeof(PORT_MEM_ALLOCATOR) + sizeof(PORT_MEM_BITVECTOR)

/// @brief Minimal bookkeeping size required for a preallocated block
#define PORT_MEM_PREALLOCATED_BLOCK_MINIMAL_EXTRA_SIZE                     \
    NV_ALIGN_UP(PORT_MEM_PREALLOCATED_BLOCK_MINIMAL_NONALIGNED_EXTRA_SIZE, \
                PORT_MEM_BITVECTOR_CHUNK_SIZE)

/// @brief Number of chunks that can be tracked in the minimal bookkeeping size
#define PORT_MEM_PREALLOCATED_BLOCK_CHUNKS_GRATIS        \
    ((                                                   \
        PORT_MEM_PREALLOCATED_BLOCK_MINIMAL_EXTRA_SIZE - \
        sizeof(PORT_MEM_ALLOCATOR)                     - \
        NV_OFFSETOF(PORT_MEM_BITVECTOR, bits)            \
    )*4U)

// Although we can never execute the underflow branch, the compiler will complain
// if any constant expression results in underflow, even in dead code.
// Note: Skipping (parens) around a and b on purpose here.
#define _PORT_CEIL_NO_UNDERFLOW(a, b) (NV_DIV_AND_CEIL(b + a, b) - 1)

/// @brief Required additional size for a given number of chunks
#define PORT_MEM_PREALLOCATED_BLOCK_SIZE_FOR_NONGRATIS_CHUNKS(num_chunks)                \
    ((num_chunks > PORT_MEM_PREALLOCATED_BLOCK_CHUNKS_GRATIS)                            \
       ? _PORT_CEIL_NO_UNDERFLOW(num_chunks - PORT_MEM_PREALLOCATED_BLOCK_CHUNKS_GRATIS, \
                                 4*PORT_MEM_BITVECTOR_CHUNK_SIZE)                        \
         * PORT_MEM_BITVECTOR_CHUNK_SIZE                                                 \
       : 0)

/// @brief Total required bookkeeping size for a block of given useful size
#define PORT_MEM_PREALLOCATED_BLOCK_EXTRA_SIZE(size)         \
    PORT_MEM_PREALLOCATED_BLOCK_MINIMAL_EXTRA_SIZE         + \
    PORT_MEM_PREALLOCATED_BLOCK_SIZE_FOR_NONGRATIS_CHUNKS(   \
        PORT_MEM_PREALLOCATED_BLOCK_NUM_CHUNKS(size))

/**
 * Macros for defining memory allocation wrappers.
 *
 * The function / file / line reference is not useful when portMemAlloc
 * is called from a generic memory allocator function, such as the memCreate
 * function in resman.
 *
 * These macros can be used to push the function /file / line reference up one
 * level when defining a memory allocator function.  In other words, log who
 * calls memCreate instead of logging memCreate.
 *
 *  These macros are also used throughout memory-tracking.c
 */
#if PORT_MEM_TRACK_USE_CALLERINFO

#define PORT_MEM_CALLERINFO_PARAM             _portMemCallerInfo
#define PORT_MEM_CALLERINFO_TYPE_PARAM                                         \
    PORT_MEM_CALLERINFO PORT_MEM_CALLERINFO_PARAM
#define PORT_MEM_CALLERINFO_COMMA_PARAM       ,PORT_MEM_CALLERINFO_PARAM
#define PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM  ,PORT_MEM_CALLERINFO_TYPE_PARAM
#define PORT_MEM_CALLINFO_FUNC(f)             f##_CallerInfo

#else // PORT_MEM_TRACK_USE_CALLERINFO

#define PORT_MEM_CALLERINFO_PARAM
#define PORT_MEM_CALLERINFO_TYPE_PARAM void
#define PORT_MEM_CALLERINFO_COMMA_PARAM
#define PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
#define PORT_MEM_CALLINFO_FUNC(f)             f

#endif // PORT_MEM_TRACK_USE_CALLERINFO

#endif // _NVPORT_MEMORY_INTERNAL_H_
/// @endcond
