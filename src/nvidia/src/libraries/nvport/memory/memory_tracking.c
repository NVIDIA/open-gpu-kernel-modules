/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief MEMORY module tracking functions implementation
 *
 */

#include "nvport/nvport.h"
#include "nvtypes.h"
#include <nvport/safe.h>

#if NVOS_IS_LIBOS && defined(DEBUG)
#include "gsp_error_injection.h"
#endif

#if !PORT_IS_MODULE_SUPPORTED(debug)
#error "DEBUG module must be present for memory tracking"
#endif

#if PORT_MEM_TRACK_USE_LIMIT
#include "os/os.h"
#endif

#if NVOS_IS_LIBOS
#define PORT_MEM_THREAD_SAFE_ALLOCATIONS 0
#else
#define PORT_MEM_THREAD_SAFE_ALLOCATIONS 1
#endif

#if PORT_MEM_THREAD_SAFE_ALLOCATIONS && !PORT_IS_MODULE_SUPPORTED(atomic)
#error "ATOMIC module must be present for memory tracking"
#endif

#if PORT_MEM_THREAD_SAFE_ALLOCATIONS
#define PORT_MEM_ATOMIC_ADD_SIZE portAtomicAddSize
#define PORT_MEM_ATOMIC_SUB_SIZE portAtomicSubSize
#define PORT_MEM_ATOMIC_DEC_U32 portAtomicDecrementU32
#define PORT_MEM_ATOMIC_INC_U32 portAtomicIncrementU32
#define PORT_MEM_ATOMIC_SET_U32 portAtomicSetU32
#define PORT_MEM_ATOMIC_CAS_SIZE portAtomicCompareAndSwapSize
#define PORT_MEM_ATOMIC_CAS_U32 portAtomicCompareAndSwapU32
#else
//
// We can just stub out the atomic operations for non-atomic ones and not waste
// cycles on synchronization
//
#define PORT_MEM_ATOMIC_ADD_SIZE(pVal, val) (*((volatile NvSPtr *)pVal) += val)
#define PORT_MEM_ATOMIC_SUB_SIZE(pVal, val) (*((volatile NvSPtr *)pVal) -= val)
#define PORT_MEM_ATOMIC_DEC_U32(pVal)      (--(*((volatile NvU32 *)pVal)))
#define PORT_MEM_ATOMIC_INC_U32(pVal)      (++(*((volatile NvU32 *)pVal)))
#define PORT_MEM_ATOMIC_SET_U32(pVal, val) (*((volatile NvU32 *)pVal) = val)
#define PORT_MEM_ATOMIC_CAS_SIZE(pVal, newVal, oldVal) \
    ((*pVal == oldVal) ? ((*((volatile NvSPtr *)pVal) = newVal), NV_TRUE) : NV_FALSE)
#define PORT_MEM_ATOMIC_CAS_U32(pVal, newVal, oldVal) \
    ((*pVal == oldVal) ? ((*((volatile NvU32 *)pVal) = newVal), NV_TRUE) : NV_FALSE)
#endif // !PORT_MEM_THREAD_SAFE_ALLOCATIONS

struct PORT_MEM_ALLOCATOR_IMPL
{
    PORT_MEM_ALLOCATOR_TRACKING tracking;
};

//
// Debug print macros
//
#if PORT_MEM_TRACK_PRINT_LEVEL == PORT_MEM_TRACK_PRINT_LEVEL_SILENT
#define PORT_MEM_PRINT_ERROR(...)
#define PORT_MEM_PRINT_INFO(...)
#elif PORT_MEM_TRACK_PRINT_LEVEL == PORT_MEM_TRACK_PRINT_LEVEL_BASIC
#define PORT_MEM_PRINT_ERROR(...) portDbgPrintf(__VA_ARGS__)
#define PORT_MEM_PRINT_INFO(...)
#else
#define PORT_MEM_PRINT_ERROR(...) portDbgPrintf(__VA_ARGS__)
#define PORT_MEM_PRINT_INFO(...)  portDbgPrintf(__VA_ARGS__)
#endif

// Simple implementation of a spinlock that is going to be used where sync module is not included.
#if !PORT_IS_MODULE_SUPPORTED(sync)

#if NVCPU_IS_RISCV64
#error "Sync module should be enabled for RISC-V builds"
#endif

typedef volatile NvU32 PORT_SPINLOCK;
static NvLength portSyncSpinlockSize = sizeof(PORT_SPINLOCK);
static NV_STATUS portSyncSpinlockInitialize(PORT_SPINLOCK *pSpinlock)
{
    *pSpinlock = 0;
    return NV_OK;
}
static void portSyncSpinlockAcquire(PORT_SPINLOCK *pSpinlock)
{
    while (!PORT_MEM_ATOMIC_CAS_U32(pSpinlock, 1, 0));
}
static void portSyncSpinlockRelease(PORT_SPINLOCK *pSpinlock)
{
    PORT_MEM_ATOMIC_SET_U32(pSpinlock, 0);
}
static void portSyncSpinlockDestroy(PORT_SPINLOCK *pSpinlock)
{
    PORT_UNREFERENCED_VARIABLE(pSpinlock);
}
#endif

#define PORT_MEM_LOCK_INIT(lock)                                               \
    do {                                                                       \
        lock = _portMemAllocNonPagedUntracked(portSyncSpinlockSize);           \
        portSyncSpinlockInitialize(lock);                                      \
    } while (0)
#define PORT_MEM_LOCK_DESTROY(lock)                                            \
    do {                                                                       \
        portSyncSpinlockDestroy(lock);                                         \
        _portMemFreeUntracked(lock);                                           \
    } while(0)
#define PORT_MEM_LOCK_ACQUIRE(lock) portSyncSpinlockAcquire(lock)
#define PORT_MEM_LOCK_RELEASE(lock) portSyncSpinlockRelease(lock)


//
// List link operation that operates on structures that have pNext and pPrev
// fields. Assumes the root always exists.
//
#define PORT_LOCKED_LIST_LINK(pRoot, pNode, lock)                              \
    do {                                                                       \
        PORT_MEM_LOCK_ACQUIRE(lock);                                           \
          (pNode)->pNext = (pRoot);                                            \
          (pNode)->pPrev = (pRoot)->pPrev;                                     \
          (pRoot)->pPrev = (pNode);                                            \
          (pNode)->pPrev->pNext = (pNode);                                     \
        PORT_MEM_LOCK_RELEASE(lock);                                           \
    } while(0)

#define PORT_LOCKED_LIST_UNLINK(pRoot, pNode, lock)                            \
    do {                                                                       \
        PORT_MEM_LOCK_ACQUIRE(lock);                                           \
          (pNode)->pNext->pPrev = (pNode)->pPrev;                              \
          (pNode)->pPrev->pNext = (pNode)->pNext;                              \
        PORT_MEM_LOCK_RELEASE(lock);                                           \
    } while (0)

//
// All memory tracking globals are contained in this structure
//
static struct PORT_MEM_GLOBALS
{
    PORT_MEM_ALLOCATOR_TRACKING mainTracking;
    void *trackingLock;
    struct
    {
        PORT_MEM_ALLOCATOR paged;
        PORT_MEM_ALLOCATOR nonPaged;
        PORT_MEM_ALLOCATOR_IMPL pagedImpl;
        PORT_MEM_ALLOCATOR_IMPL nonPagedImpl;
    } alloc;
    NvU32 initCount;
    NvU32 totalAllocators;
#if PORT_MEM_TRACK_USE_LIMIT
    NvBool bLimitEnabled;
    PORT_MEM_ALLOCATOR_TRACKING *pGfidTracking[PORT_MEM_LIMIT_MAX_GFID];
#endif
} portMemGlobals;

//
// Memory counter implementation
//
#if PORT_MEM_TRACK_USE_COUNTER
#if PORT_MEM_TRACK_ALLOC_SIZE
static NV_INLINE NvLength
_portMemExTrackingGetAllocUsableSizeWrapper
(
    void *pMem
)
{
#if PORT_MEM_HEADER_HAS_BLOCK_SIZE
    return PORT_MEM_SUB_HEADER_PTR(pMem)->blockSize;
#endif
}
static NV_INLINE void
_portMemExTrackingSetOrGetAllocUsableSize
(
    void    *pMem,
    NvLength *pSize
)
{
#if PORT_MEM_HEADER_HAS_BLOCK_SIZE
    PORT_MEM_SUB_HEADER_PTR(pMem)->blockSize = *pSize;
#else
    *pSize = _portMemExTrackingGetAllocUsableSizeWrapper(pMem);
#endif
}
#endif // PORT_MEM_TRACK_ALLOC_SIZE
static NV_INLINE void
_portMemCounterInit
(
    PORT_MEM_COUNTER *pCounter
)
{
    portMemSet(pCounter, 0, sizeof(*pCounter));
}
static NV_INLINE void
_portMemCounterInc
(
    PORT_MEM_COUNTER *pCounter,
    NvLength          size
)
{
    NvU32 activeAllocs;
    NvLength activeSize = 0;

    activeAllocs = PORT_MEM_ATOMIC_INC_U32(&pCounter->activeAllocs);
    PORT_MEM_ATOMIC_INC_U32(&pCounter->totalAllocs);
#if PORT_MEM_TRACK_ALLOC_SIZE
    //
    // activeSize is only tracked on configurations where we can retrieve the
    // allocation size from allocation metadata in _portMemCounterDec.
    //
    activeSize = PORT_MEM_ATOMIC_ADD_SIZE(&pCounter->activeSize, size);
#endif

    //
    // Note: this can overflow on 32-bit platforms if we exceed 4GB cumulative
    // allocations. It's not trivial to fix, since NvPort doesn't emulate 64-bit
    // atomics on 32-bit platforms, so just assume this doesn't happen (or
    // doesn't matter too much if it does, since it's only for reporting).
    //
    PORT_MEM_ATOMIC_ADD_SIZE(&pCounter->totalSize, size);

    // Update the peak stats, if we're updating the peakSize
    {
        NvU32 peakAllocs;
        NvLength peakSize = pCounter->peakSize;
        while (activeSize > peakSize)
        {
            PORT_MEM_ATOMIC_CAS_SIZE(&pCounter->peakSize, activeSize, peakSize);
            peakSize = pCounter->peakSize;
        }

        //
        // Ensure peakAllocs stays (approximately) in sync with peakSize, rather
        // than always taking the greatest peakAllocs, so that the peak stats
        // report is consistent.
        //
        do
        {
            peakAllocs = pCounter->peakAllocs;

            //
            // Only attempt to update the peakAllocs if activeSize is still the
            // peakSize.
            //
            if (activeSize != pCounter->peakSize)
                break;
        } while (!PORT_MEM_ATOMIC_CAS_U32(&pCounter->peakAllocs, activeAllocs, peakAllocs));
    }
}
static NV_INLINE void
_portMemCounterDec
(
    PORT_MEM_COUNTER *pCounter,
    NvLength          size
)
{
    PORT_MEM_ATOMIC_DEC_U32(&pCounter->activeAllocs);
#if PORT_MEM_TRACK_ALLOC_SIZE
    PORT_MEM_ATOMIC_SUB_SIZE(&pCounter->activeSize, size);
#else
    PORT_UNREFERENCED_VARIABLE(size);
#endif
}

#define PORT_MEM_COUNTER_INIT(pCounter)      _portMemCounterInit(pCounter)
#define PORT_MEM_COUNTER_INC(pCounter, size) _portMemCounterInc(pCounter, size)
#define PORT_MEM_COUNTER_DEC(pCounter, size) _portMemCounterDec(pCounter, size)
#else
#define PORT_MEM_COUNTER_INIT(x)
#define PORT_MEM_COUNTER_INC(x, y)  PORT_UNREFERENCED_VARIABLE(y)
#define PORT_MEM_COUNTER_DEC(x, y)  PORT_UNREFERENCED_VARIABLE(y)
#endif // COUNTER


//
// Memory fenceposts implementation
//
#if PORT_MEM_TRACK_USE_FENCEPOSTS
#define PORT_MEM_FENCE_HEAD_MAGIC 0x68656164 // 'head'
#define PORT_MEM_FENCE_TAIL_MAGIC 0x7461696c // 'tail'

static NV_INLINE void
_portMemFenceInit
(
    PORT_MEM_ALLOCATOR *pAlloc,
    void               *pMem,
    NvLength            size
)
{
    PORT_MEM_HEADER *pHead = (PORT_MEM_HEADER*)pMem - 1;
    PORT_MEM_FOOTER *pTail = (PORT_MEM_FOOTER*)((NvU8*)pMem + size);

    pHead->fence.pAllocator = pAlloc;
    pHead->fence.magic      = PORT_MEM_FENCE_HEAD_MAGIC;
    pTail->fence.magic      = PORT_MEM_FENCE_TAIL_MAGIC;
}

static NV_INLINE void
_portMemFenceCheck
(
    PORT_MEM_ALLOCATOR *pAlloc,
    void               *pMem,
    NvLength            size
)
{
    PORT_MEM_HEADER *pHead = (PORT_MEM_HEADER*)pMem - 1;
    PORT_MEM_FOOTER *pTail = (PORT_MEM_FOOTER*)((NvU8*)pMem + size);

    if (pHead->fence.magic != PORT_MEM_FENCE_HEAD_MAGIC ||
        pTail->fence.magic != PORT_MEM_FENCE_TAIL_MAGIC)
    {
        PORT_MEM_PRINT_ERROR("Memory corruption detected on block %p\n", pMem);
        PORT_ASSERT_CHECKED(pHead->fence.magic   == PORT_MEM_FENCE_HEAD_MAGIC);
        PORT_ASSERT_CHECKED(pTail->fence.magic   == PORT_MEM_FENCE_TAIL_MAGIC);
    }
    if (pHead->fence.pAllocator != pAlloc)
    {
        PORT_MEM_PRINT_ERROR("Freeing block %p using a wrong allocator (%p instead of %p)\n",
                             pMem, pAlloc, pHead->fence.pAllocator);
        PORT_ASSERT_CHECKED(pHead->fence.pAllocator == pAlloc);

    }
}

#define PORT_MEM_FENCE_CHECK(pAlloc, pMem, size)    _portMemFenceCheck(pAlloc, pMem, size)
#define PORT_MEM_FENCE_INIT(pAlloc, pMem, size)     _portMemFenceInit(pAlloc, pMem, size)
#else
#define PORT_MEM_FENCE_INIT(x, y, z)
#define PORT_MEM_FENCE_CHECK(x, y, z)
#endif // FENCEPOSTS


//
// Memory allocation lists implementation
//
#if PORT_MEM_TRACK_USE_ALLOCLIST
static NV_INLINE void
_portMemListAdd
(
    PORT_MEM_ALLOCATOR_TRACKING *pTracking,
    void                        *pMem
)
{
    PORT_MEM_HEADER *pHead = (PORT_MEM_HEADER*)pMem - 1;
    PORT_MEM_LIST *pList = &pHead->list;
    pList->pNext = pList;
    pList->pPrev = pList;
    if (!PORT_MEM_ATOMIC_CAS_SIZE(&pTracking->pFirstAlloc, pList, NULL))
    {
        PORT_LOCKED_LIST_LINK(pTracking->pFirstAlloc, pList, pTracking->listLock);
    }
}
static NV_INLINE void
_portMemListRemove
(
    PORT_MEM_ALLOCATOR_TRACKING *pTracking,
    void                        *pMem
)
{
    PORT_MEM_HEADER *pHead = (PORT_MEM_HEADER*)pMem - 1;
    PORT_MEM_LIST *pList = &pHead->list;

    if (!PORT_MEM_ATOMIC_CAS_SIZE(&pList->pNext, NULL, pList))
    {
        PORT_LOCKED_LIST_UNLINK(pTracking->pFirstAlloc, pList, pTracking->listLock);
    }
    PORT_MEM_ATOMIC_CAS_SIZE(&pTracking->pFirstAlloc, pList->pNext, pList);
}

static NV_INLINE PORT_MEM_HEADER *
_portMemListGetHeader
(
    PORT_MEM_LIST *pList
)
{
    return (PORT_MEM_HEADER*)((NvU8*)pList - (NvUPtr)(&((PORT_MEM_HEADER*)NULL)->list));
}
#define PORT_MEM_LIST_INIT(pTracking)                                          \
    do {                                                                       \
        (pTracking)->pFirstAlloc = NULL;                                       \
        PORT_MEM_LOCK_INIT((pTracking)->listLock);                               \
    } while (0)
#define PORT_MEM_LIST_DESTROY(pTracking)   PORT_MEM_LOCK_DESTROY((pTracking)->listLock)
#define PORT_MEM_LIST_ADD(pTracking, pMem)   _portMemListAdd(pTracking, pMem)
#define PORT_MEM_LIST_REMOVE(Tracking, pMem) _portMemListRemove(pTracking, pMem)
#else
#define PORT_MEM_LIST_INIT(x)
#define PORT_MEM_LIST_DESTROY(x)
#define PORT_MEM_LIST_ADD(x, y)
#define PORT_MEM_LIST_REMOVE(x, y)
#endif // ALLOCLIST



//
// Memory allocation-caller info implementation
//
#if PORT_MEM_TRACK_USE_CALLERINFO

static NV_INLINE void
_portMemCallerInfoInitMem
(
    void                *pMem,
    PORT_MEM_CALLERINFO callerInfo
)
{
    PORT_MEM_HEADER *pHead = (PORT_MEM_HEADER*)pMem - 1;
    portMemCopy(&pHead->callerInfo, sizeof(callerInfo),
                &callerInfo,        sizeof(callerInfo));
}
static NV_INLINE void
_portMemCallerInfoInitTracking
(
    PORT_MEM_ALLOCATOR_TRACKING *pTracking,
    PORT_MEM_CALLERINFO          callerInfo
)
{
    portMemCopy(&pTracking->callerInfo, sizeof(callerInfo),
                &callerInfo,            sizeof(callerInfo));
}

#define PORT_MEM_CALLERINFO_INIT_TRACKING(pTracking)                           \
    _portMemCallerInfoInitTracking(pTracking, PORT_MEM_CALLERINFO_PARAM)
#define PORT_MEM_CALLERINFO_INIT_MEM(pMem)                                     \
    _portMemCallerInfoInitMem(pMem, PORT_MEM_CALLERINFO_PARAM)

#if PORT_MEM_TRACK_USE_CALLERINFO_IP
#if NVOS_IS_LIBOS
//
// Libos has custom %a format specifier that decodes an instruction pointer into
// a function / file / line reference when the binary output is decoded.
//
#define PORT_MEM_CALLERINFO_PRINT_ARGS(x)  "@ %a\n", x
#else
#define PORT_MEM_CALLERINFO_PRINT_ARGS(x)  "@ 0x%016x\n", x
#endif // NVOS_IS_LIBOS
#else
#define PORT_MEM_CALLERINFO_PRINT_ARGS(x)  "@ %s:%u (%s)\n", x.file, x.line, x.func
#endif // PORT_MEM_TRACK_USE_CALLERINFO_IP

#else // PORT_MEM_TRACK_USE_CALLERINFO
#define PORT_MEM_CALLERINFO_INIT_TRACKING(x)
#define PORT_MEM_CALLERINFO_INIT_MEM(x)
#define PORT_MEM_CALLERINFO_PRINT_ARGS(x)  "\n"
#endif // PORT_MEM_TRACK_USE_CALLERINFO


#if PORT_MEM_TRACK_USE_LOGGING
#include "nvlog/nvlog.h"
/** @brief Single log entry. Uses 64bit values even on 32bit systems. */
typedef struct PORT_MEM_LOG_ENTRY
{
    NvP64 address;
    NvP64 allocator;
    NvLength size;     // if size is 0, it is a free() call, not alloc()
} PORT_MEM_LOG_ENTRY;

#define PORT_MEM_TRACK_LOG_TAG 0x70726d74
#define PORT_MEM_LOG_ENTRIES 4096

static void
_portMemLogInit(void)
{
    NVLOG_BUFFER_HANDLE hBuffer;
    nvlogAllocBuffer(PORT_MEM_LOG_ENTRIES * sizeof(PORT_MEM_LOG_ENTRY),
                     DRF_DEF(LOG, _BUFFER_FLAGS, _FORMAT, _MEMTRACK),
                     PORT_MEM_TRACK_LOG_TAG, &hBuffer);
}

static void
_portMemLogDestroy(void)
{
    NVLOG_BUFFER_HANDLE hBuffer;
    nvlogGetBufferHandleFromTag(PORT_MEM_TRACK_LOG_TAG, &hBuffer);
    nvlogDeallocBuffer(hBuffer);
}

static void
_portMemLogAdd
(
    PORT_MEM_ALLOCATOR *pAllocator,
    void               *pMem,
    NvLength            lengthBytes
)
{
    NVLOG_BUFFER_HANDLE hBuffer;
    PORT_MEM_LOG_ENTRY entry = {0};
    entry.address = NV_PTR_TO_NvP64(pMem);
    entry.address = NV_PTR_TO_NvP64(pAllocator);
    entry.size    = lengthBytes;
    nvlogGetBufferHandleFromTag(PORT_MEM_TRACK_LOG_TAG, &hBuffer);
    nvlogWriteToBuffer(hBuffer, &entry, sizeof(entry));
}

#define PORT_MEM_LOG_INIT()     _portMemLogInit()
#define PORT_MEM_LOG_DESTROY()  _portMemLogDestroy()
#define PORT_MEM_LOG_ALLOC(pAlloc, pMem, size)                                 \
    _portMemLogAdd(pAlloc, pMem, size)
#define PORT_MEM_LOG_FREE(pAlloc, pMem)                                        \
    _portMemLogAdd(pAlloc, pMem, 0)
#else
#define PORT_MEM_LOG_INIT()
#define PORT_MEM_LOG_DESTROY()
#define PORT_MEM_LOG_ALLOC(x, y, z)
#define PORT_MEM_LOG_FREE(x, y)
#endif // LOGGING


////////////////////////////////////////////////////////////////////////////////
//
// Main memory tracking implementation
//
////////////////////////////////////////////////////////////////////////////////

//
// All static functions declarations. Definitions at the end of file.
//
static void *_portMemAllocatorAllocPagedWrapper(PORT_MEM_ALLOCATOR *pAlloc, NvLength length);
static void *_portMemAllocatorAllocNonPagedWrapper(PORT_MEM_ALLOCATOR *pAlloc, NvLength length);
static void _portMemAllocatorFreeWrapper(PORT_MEM_ALLOCATOR *pAlloc, void *pMem);
static void _portMemAllocatorReleaseWrapper(PORT_MEM_ALLOCATOR *pAlloc);

static PORT_MEM_ALLOCATOR *_portMemAllocatorCreateOnExistingBlock(void *pAlloc, NvLength blockSizeBytes, void *pSpinlock PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM);
static void    *_portMemAllocatorAllocExistingWrapper(PORT_MEM_ALLOCATOR *pAlloc, NvLength length);
static void     _portMemAllocatorFreeExistingWrapper(PORT_MEM_ALLOCATOR *pAlloc, void *pMem);

static void _portMemTrackingRelease(PORT_MEM_ALLOCATOR_TRACKING *pTracking, NvBool bReportLeaks);
static void _portMemTrackAlloc(
    PORT_MEM_ALLOCATOR_TRACKING *pTracking,
    void *pMem,
    NvLength size,
    NvU32 gfid
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
);
static void _portMemTrackFree(PORT_MEM_ALLOCATOR_TRACKING *pTracking, void *pMem);



#if PORT_MEM_TRACK_USE_CALLERINFO
#undef portMemAllocPaged
#undef portMemAllocNonPaged
#undef portMemAllocatorCreatePaged
#undef portMemAllocatorCreateNonPaged
#undef portMemInitializeAllocatorTracking
#undef _portMemAllocatorAlloc
#undef portMemAllocatorCreateOnExistingBlock
#undef portMemExAllocatorCreateLockedOnExistingBlock
// These functions have different names if CallerInfo is enabled.
#define portMemAllocPaged              portMemAllocPaged_CallerInfo
#define portMemAllocNonPaged           portMemAllocNonPaged_CallerInfo
#define portMemAllocatorCreatePaged    portMemAllocatorCreatePaged_CallerInfo
#define portMemAllocatorCreateNonPaged portMemAllocatorCreateNonPaged_CallerInfo
#define portMemInitializeAllocatorTracking portMemInitializeAllocatorTracking_CallerInfo
#define _portMemAllocatorAlloc         _portMemAllocatorAlloc_CallerInfo
#define portMemAllocatorCreateOnExistingBlock portMemAllocatorCreateOnExistingBlock_CallerInfo
#define portMemExAllocatorCreateLockedOnExistingBlock portMemExAllocatorCreateLockedOnExistingBlock_CallerInfo
#endif

//
// Per-process heap limiting implementation
//
#if PORT_MEM_TRACK_USE_LIMIT
static inline NvBool isGfidValid(NvU32 gfid)
{
    return (gfid > 0) && (gfid <= PORT_MEM_LIMIT_MAX_GFID);
}

static void _portMemTrackingGfidRelease(void)
{
    int gfidIdx;

    for (gfidIdx = 0; gfidIdx < PORT_MEM_LIMIT_MAX_GFID; ++gfidIdx)
    {
        if (portMemGlobals.pGfidTracking[gfidIdx] != NULL)
        {
            _portMemTrackingRelease(portMemGlobals.pGfidTracking[gfidIdx], NV_FALSE);
            portMemGlobals.pGfidTracking[gfidIdx] = NULL;
        }
    }
}

static NV_INLINE void
_portMemLimitInc(NvU32 gfid, void *pMem, NvLength size)
{
    PORT_MEM_HEADER *pMemHeader = PORT_MEM_SUB_HEADER_PTR(pMem);
    pMemHeader->gfid = gfid;
    if (portMemGlobals.bLimitEnabled)
    {
        if (isGfidValid(gfid))
        {
            NvU32 gfidIdx = gfid - 1;
            PORT_MEM_ATOMIC_ADD_SIZE(&portMemGlobals.pGfidTracking[gfidIdx]->counterGfid, size);
        }
    }
}

static NV_INLINE void
_portMemLimitDec(void *pMem, NvLength size)
{
    if (portMemGlobals.bLimitEnabled)
    {
        PORT_MEM_HEADER *pMemHeader = PORT_MEM_SUB_HEADER_PTR(pMem);
        NvU32 gfid = pMemHeader->gfid;

        if (isGfidValid(gfid))
        {
            NvU32 gfidIdx = gfid - 1;
            if (portMemGlobals.pGfidTracking[gfidIdx]->counterGfid < size)
            {
                PORT_MEM_PRINT_ERROR("memory free error: counter underflow\n");
                PORT_BREAKPOINT_CHECKED();
            }
            else
            {
                PORT_MEM_ATOMIC_SUB_SIZE(
                    &portMemGlobals.pGfidTracking[gfidIdx]->counterGfid,
                    size);
            }
        }
    }
}

static NV_INLINE NvBool
_portMemLimitExceeded(NvU32 gfid, NvLength size)
{
    NvBool bExceeded = NV_FALSE;

    if (portMemGlobals.bLimitEnabled)
    {
        if (isGfidValid(gfid))
        {
            NvU32 gfidIdx = gfid - 1;
            if ((size + portMemGlobals.pGfidTracking[gfidIdx]->counterGfid) >
                portMemGlobals.pGfidTracking[gfidIdx]->limitGfid)
            {
                PORT_MEM_PRINT_ERROR(
                    "memory allocation denied; GFID %d exceeded per-process heap limit of "
                    "%"NvUPtr_fmtu"\n",
                    gfid, portMemGlobals.pGfidTracking[gfidIdx]->limitGfid
                );
                bExceeded = NV_TRUE;
            }
        }
    }
    return bExceeded;
}

void
portMemLibosLimitInc(NvU32 gfid, NvLength size)
{
    if (portMemGlobals.bLimitEnabled)
    {
        if (isGfidValid(gfid))
        {
            NvU32 gfidIdx = gfid - 1;
            PORT_MEM_ATOMIC_ADD_SIZE(&portMemGlobals.pGfidTracking[gfidIdx]->counterLibosGfid, size);
        }
    }
}

void
portMemLibosLimitDec(NvU32 gfid, NvLength size)
{
    if (portMemGlobals.bLimitEnabled)
    {
        if (isGfidValid(gfid))
        {
            NvU32 gfidIdx = gfid - 1;
            if (portMemGlobals.pGfidTracking[gfidIdx]->counterLibosGfid < size)
            {
                PORT_MEM_PRINT_ERROR("GFID %d memory free error: counter underflow\n"
                                     "counter = %llu\nsize = %llu\n",
                                     gfid,
                                     portMemGlobals.pGfidTracking[gfidIdx]->counterLibosGfid,
                                     size);
                PORT_BREAKPOINT_CHECKED();
            }
            else
            {
                PORT_MEM_ATOMIC_SUB_SIZE(
                    &portMemGlobals.pGfidTracking[gfidIdx]->counterLibosGfid,
                    size);
            }
        }
    }
}

NvBool
portMemLibosLimitExceeded(NvU32 gfid, NvLength size)
{
    NvBool bExceeded = NV_FALSE;

    if (portMemGlobals.bLimitEnabled)
    {
        if (isGfidValid(gfid))
        {
            NvU32 gfidIdx = gfid - 1;
            if ((size + portMemGlobals.pGfidTracking[gfidIdx]->counterLibosGfid) >
                portMemGlobals.pGfidTracking[gfidIdx]->limitLibosGfid)
            {
                PORT_MEM_PRINT_ERROR(
                    "LibOS memory allocation denied; GFID %d exceeded per-VF LibOS heap limit of "
                    "%"NvUPtr_fmtu"\n"
                    "counter = %llu\nsize = %llu\n",
                    gfid,
                    portMemGlobals.pGfidTracking[gfidIdx]->limitLibosGfid,
                    portMemGlobals.pGfidTracking[gfidIdx]->counterLibosGfid,
                    size
                );
                bExceeded = NV_TRUE;
            }
        }
        else
        {
            // Also fail for invalid GFID
            PORT_MEM_PRINT_ERROR("LibOS memory allocation denied; GFID %d invalid\n", gfid);
            bExceeded = NV_TRUE;
        }
    }
    return bExceeded;
}

#define PORT_MEM_LIMIT_INC(gfid, pMem, size) _portMemLimitInc(gfid, pMem, size)
#define PORT_MEM_LIMIT_DEC(pMem, size)      _portMemLimitDec(pMem, size)
#define PORT_MEM_LIMIT_EXCEEDED(gfid, size)  _portMemLimitExceeded(gfid, size)
#else
#define PORT_MEM_LIMIT_INC(gfid, pMem, size) \
    do {                                    \
        PORT_UNREFERENCED_VARIABLE(gfid);    \
        PORT_UNREFERENCED_VARIABLE(pMem);   \
        PORT_UNREFERENCED_VARIABLE(size);   \
    } while (0)
#define PORT_MEM_LIMIT_DEC(pMem, size)      \
    do {                                    \
        PORT_UNREFERENCED_VARIABLE(pMem);   \
        PORT_UNREFERENCED_VARIABLE(size);   \
    } while (0)
#define PORT_MEM_LIMIT_EXCEEDED(gfid, size)  (NV_FALSE)
#endif // PORT_MEM_TRACK_USE_LIMIT

static NV_INLINE PORT_MEM_ALLOCATOR_TRACKING *
_portMemGetTracking
(
    const PORT_MEM_ALLOCATOR *pAlloc
)
{
    if (pAlloc == NULL)
        return &portMemGlobals.mainTracking;
    else
        return pAlloc->pTracking;
}


void
portMemInitialize(void)
{
#if PORT_MEM_TRACK_USE_CALLERINFO
    PORT_MEM_CALLERINFO_TYPE_PARAM = PORT_MEM_CALLERINFO_MAKE;
#endif
    if (PORT_MEM_ATOMIC_INC_U32(&portMemGlobals.initCount) != 1)
        return;

    portMemGlobals.mainTracking.pAllocator = NULL;
    portMemGlobals.mainTracking.pNext = &portMemGlobals.mainTracking;
    portMemGlobals.mainTracking.pPrev = &portMemGlobals.mainTracking;
    PORT_MEM_COUNTER_INIT(&portMemGlobals.mainTracking.counter);
    PORT_MEM_LIST_INIT(&portMemGlobals.mainTracking);
    PORT_MEM_LOCK_INIT(portMemGlobals.trackingLock);

#if PORT_MEM_TRACK_USE_LIMIT
    // Initialize process heap limit to max int (i.e. no limit)
    portMemGlobals.bLimitEnabled = NV_FALSE;
#endif

    portMemGlobals.alloc.paged._portAlloc      = _portMemAllocatorAllocPagedWrapper;
    portMemGlobals.alloc.nonPaged._portAlloc   = _portMemAllocatorAllocNonPagedWrapper;
    portMemGlobals.alloc.paged._portFree       = _portMemAllocatorFreeWrapper;
    portMemGlobals.alloc.nonPaged._portFree    = _portMemAllocatorFreeWrapper;
    portMemGlobals.alloc.paged._portRelease    = NULL;
    portMemGlobals.alloc.nonPaged._portRelease = NULL;

    if (PORT_MEM_TRACK_USE_FENCEPOSTS)
    {
        //
        // Distinct paged and non-paged allocators require PORT_MEM_TRACK_USE_FENCEPOSTS
        // so that the correct allocator can be looked up from the fenceposts in the
        // portMemFree path.
        //
        portMemGlobals.alloc.paged.pImpl    = &portMemGlobals.alloc.pagedImpl;
        portMemGlobals.alloc.nonPaged.pImpl = &portMemGlobals.alloc.nonPagedImpl;

        portMemInitializeAllocatorTracking(&portMemGlobals.alloc.paged,
                                     &portMemGlobals.alloc.paged.pImpl->tracking
                                     PORT_MEM_CALLERINFO_COMMA_PARAM);
        portMemInitializeAllocatorTracking(&portMemGlobals.alloc.nonPaged,
                                  &portMemGlobals.alloc.nonPaged.pImpl->tracking
                                  PORT_MEM_CALLERINFO_COMMA_PARAM);
    }
    else
    {
        // Use the same impl for both paged and nonpaged.
        portMemGlobals.alloc.paged.pImpl    = &portMemGlobals.alloc.pagedImpl;
        portMemGlobals.alloc.nonPaged.pImpl = &portMemGlobals.alloc.pagedImpl;
        portMemInitializeAllocatorTracking(&portMemGlobals.alloc.paged,
                             &portMemGlobals.alloc.pagedImpl.tracking
                             PORT_MEM_CALLERINFO_COMMA_PARAM);
        portMemGlobals.alloc.paged.pTracking    = &portMemGlobals.alloc.pagedImpl.tracking;
        portMemGlobals.alloc.nonPaged.pTracking = &portMemGlobals.alloc.pagedImpl.tracking;
    }
    PORT_MEM_LOG_INIT();
}
void
portMemShutdown(NvBool bForceSilent)
{
    PORT_UNREFERENCED_VARIABLE(bForceSilent);
    if (PORT_MEM_ATOMIC_DEC_U32(&portMemGlobals.initCount) != 0)
        return;

#if (PORT_MEM_TRACK_PRINT_LEVEL > PORT_MEM_TRACK_PRINT_LEVEL_SILENT)
    if (!bForceSilent)
    {
        portMemPrintAllTrackingInfo();
    }
#endif
    PORT_MEM_LOG_DESTROY();

    if (PORT_MEM_TRACK_USE_FENCEPOSTS)
    {
        _portMemTrackingRelease(&portMemGlobals.alloc.nonPaged.pImpl->tracking, NV_FALSE);
        _portMemTrackingRelease(&portMemGlobals.alloc.paged.pImpl->tracking, NV_FALSE);
    }
    else
    {
        _portMemTrackingRelease(&portMemGlobals.alloc.pagedImpl.tracking, NV_FALSE);
    }
#if PORT_MEM_TRACK_USE_LIMIT
    _portMemTrackingGfidRelease();
#endif
    PORT_MEM_LOCK_DESTROY(portMemGlobals.trackingLock);
    PORT_MEM_LIST_DESTROY(&portMemGlobals.mainTracking);
    portMemSet(&portMemGlobals, 0, sizeof(portMemGlobals));
}

void *
portMemAllocPaged
(
    NvLength length
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
#if defined(__COVERITY__)
    return __coverity_alloc__(length);
#endif
    PORT_MEM_ALLOCATOR *pAlloc = portMemAllocatorGetGlobalPaged();
    return _portMemAllocatorAlloc(pAlloc, length PORT_MEM_CALLERINFO_COMMA_PARAM);
}

void *
portMemAllocNonPaged
(
    NvLength length
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
#if defined(__COVERITY__)
    return __coverity_alloc__(length);
#endif
    PORT_MEM_ALLOCATOR *pAlloc = portMemAllocatorGetGlobalNonPaged();
    return _portMemAllocatorAlloc(pAlloc, length PORT_MEM_CALLERINFO_COMMA_PARAM);
}

void
portMemFree
(
    void *pMem
)
{
    if (pMem != NULL)
    {
#if PORT_MEM_TRACK_USE_FENCEPOSTS
        PORT_MEM_HEADER *pHead = (PORT_MEM_HEADER*)pMem - 1;
        PORT_FREE(pHead->fence.pAllocator, pMem);
#else
        // Paged/nonpaged are logged together if we don't have fenceposts.
        PORT_FREE(portMemAllocatorGetGlobalPaged(), pMem);
#endif
    }

#if defined(__COVERITY__)
    __coverity_free__(pMem);
#endif
}

PORT_MEM_ALLOCATOR *
portMemAllocatorCreatePaged(PORT_MEM_CALLERINFO_TYPE_PARAM)
{
    PORT_MEM_ALLOCATOR *pAllocator;

    pAllocator = portMemAllocPaged(PORT_MEM_ALLOCATOR_SIZE
                                   PORT_MEM_CALLERINFO_COMMA_PARAM);
    if (pAllocator == NULL)
        return NULL;

    pAllocator->pImpl         = (PORT_MEM_ALLOCATOR_IMPL*)(pAllocator + 1);
    pAllocator->_portAlloc    = _portMemAllocatorAllocPagedWrapper;
    pAllocator->_portFree     = _portMemAllocatorFreeWrapper;
    pAllocator->_portRelease  = _portMemAllocatorReleaseWrapper;
    portMemInitializeAllocatorTracking(pAllocator, &pAllocator->pImpl->tracking
                                       PORT_MEM_CALLERINFO_COMMA_PARAM);

    PORT_MEM_PRINT_INFO("Acquired paged allocator %p ", pAllocator);
    PORT_MEM_PRINT_INFO(PORT_MEM_CALLERINFO_PRINT_ARGS(PORT_MEM_CALLERINFO_PARAM));

    return pAllocator;
}

PORT_MEM_ALLOCATOR *
portMemAllocatorCreateNonPaged(PORT_MEM_CALLERINFO_TYPE_PARAM)
{
    PORT_MEM_ALLOCATOR *pAllocator;

    pAllocator = portMemAllocNonPaged(PORT_MEM_ALLOCATOR_SIZE
                                      PORT_MEM_CALLERINFO_COMMA_PARAM);
    if (pAllocator == NULL)
        return NULL;

    pAllocator->pImpl         = (PORT_MEM_ALLOCATOR_IMPL*)(pAllocator + 1);
    pAllocator->_portAlloc    = _portMemAllocatorAllocNonPagedWrapper;
    pAllocator->_portFree     = _portMemAllocatorFreeWrapper;
    pAllocator->_portRelease  = _portMemAllocatorReleaseWrapper;
    portMemInitializeAllocatorTracking(pAllocator, &pAllocator->pImpl->tracking
                                       PORT_MEM_CALLERINFO_COMMA_PARAM);

    PORT_MEM_PRINT_INFO("Acquired nonpaged allocator %p ", pAllocator);
    PORT_MEM_PRINT_INFO(PORT_MEM_CALLERINFO_PRINT_ARGS(PORT_MEM_CALLERINFO_PARAM));
    return pAllocator;
}


PORT_MEM_ALLOCATOR *
portMemAllocatorCreateOnExistingBlock
(
    void *pPreallocatedBlock,
    NvLength blockSizeBytes
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
    return _portMemAllocatorCreateOnExistingBlock(pPreallocatedBlock, blockSizeBytes,
                                                  NULL PORT_MEM_CALLERINFO_COMMA_PARAM);
}

PORT_MEM_ALLOCATOR *
portMemExAllocatorCreateLockedOnExistingBlock
(
    void *pPreallocatedBlock,
    NvLength blockSizeBytes,
    void *pSpinlock
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
    return _portMemAllocatorCreateOnExistingBlock(pPreallocatedBlock, blockSizeBytes,
                                                  pSpinlock PORT_MEM_CALLERINFO_COMMA_PARAM);
}

void
portMemAllocatorRelease
(
    PORT_MEM_ALLOCATOR *pAllocator
)
{
    if (pAllocator == NULL)
    {
        PORT_BREAKPOINT_CHECKED();
        return;
    }
    _portMemTrackingRelease(pAllocator->pTracking, NV_TRUE);
    PORT_MEM_PRINT_INFO("Released allocator %p\n", pAllocator);

    if (pAllocator->_portRelease != NULL)
        pAllocator->_portRelease(pAllocator);
}


PORT_MEM_ALLOCATOR *
portMemAllocatorGetGlobalNonPaged(void)
{
    return &portMemGlobals.alloc.nonPaged;
}
PORT_MEM_ALLOCATOR *
portMemAllocatorGetGlobalPaged(void)
{
    return &portMemGlobals.alloc.paged;
}

void
portMemInitializeAllocatorTracking
(
    PORT_MEM_ALLOCATOR          *pAlloc,
    PORT_MEM_ALLOCATOR_TRACKING *pTracking
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
    if (portMemGlobals.initCount == 0)
    {
        portMemSet(pTracking, 0, sizeof(*pTracking));
        if (pAlloc != NULL)
            pAlloc->pTracking = NULL;
        return;
    }

    pTracking->pAllocator = pAlloc;
    if (pAlloc != NULL)
        pAlloc->pTracking = pTracking;
    PORT_LOCKED_LIST_LINK(&portMemGlobals.mainTracking, pTracking, portMemGlobals.trackingLock);
    PORT_MEM_COUNTER_INIT(&pTracking->counter);
    PORT_MEM_LIST_INIT(pTracking);
    PORT_MEM_CALLERINFO_INIT_TRACKING(pTracking);
    PORT_MEM_ATOMIC_INC_U32(&portMemGlobals.totalAllocators);
}

#if PORT_MEM_TRACK_USE_LIMIT
void
portMemInitializeAllocatorTrackingLimit(NvU32 gfid, NvLength limit, NvBool bLimitEnabled)
{
    if (!isGfidValid(gfid))
        return;

    NvU32 gfidIdx = gfid - 1;

    portMemGlobals.pGfidTracking[gfidIdx]->limitGfid = limit;
    portMemGlobals.bLimitEnabled = bLimitEnabled;
}

void
portMemInitializeAllocatorTrackingLibosLimit(NvU32 gfid, NvLength limit)
{
    if (!isGfidValid(gfid))
        return;

    NvU32 gfidIdx = gfid - 1;

    if (portMemGlobals.pGfidTracking[gfidIdx] != NULL)
        portMemGlobals.pGfidTracking[gfidIdx]->limitLibosGfid = limit;
}

void portMemGfidTrackingInit(NvU32 gfid)
{
    if (!isGfidValid(gfid))
    {
        PORT_BREAKPOINT_CHECKED();
        return;
    }

    NvU32 gfidIdx = gfid - 1;

    if (portMemGlobals.pGfidTracking[gfidIdx] != NULL)
        return;

    PORT_MEM_ALLOCATOR_TRACKING *pTracking =
        _portMemAllocNonPagedUntracked(sizeof(PORT_MEM_ALLOCATOR_TRACKING));

    if (pTracking == NULL)
    {
        portDbgPrintf("!!! Failed memory allocation for pTracking !!!\n");
        PORT_BREAKPOINT_CHECKED();
        return;
    }

    portMemSet(pTracking, 0, sizeof(*pTracking));
    pTracking->limitGfid = NV_U64_MAX;
    pTracking->counterGfid = 0;
    pTracking->gfid = gfid;
    pTracking->limitLibosGfid = NV_U64_MAX;
    pTracking->counterLibosGfid = 0;
    PORT_LOCKED_LIST_LINK(&portMemGlobals.mainTracking, pTracking, portMemGlobals.trackingLock);
    PORT_MEM_COUNTER_INIT(&pTracking->counter);
    portMemGlobals.pGfidTracking[gfidIdx] = pTracking;
}

void portMemGfidTrackingFree(NvU32 gfid)
{
    if (!isGfidValid(gfid))
    {
        PORT_BREAKPOINT_CHECKED();
        return;
    }

    NvU32 gfidIdx = gfid - 1;
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = portMemGlobals.pGfidTracking[gfidIdx];

    if (pTracking == NULL)
    {
        PORT_BREAKPOINT_CHECKED();
        return;
    }

    if (pTracking->counter.activeAllocs != 0)
    {
        portDbgPrintf("  !!! MEMORY LEAK DETECTED (%u blocks) !!!\n",
                      pTracking->counter.activeAllocs);

    }

    portMemPrintTrackingInfo(pTracking);
}

#endif

void *
_portMemAllocatorAlloc
(
    PORT_MEM_ALLOCATOR *pAlloc,
    NvLength length
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
    NvU32 gfid = 0;
    void *pMem = NULL;
#if NVOS_IS_LIBOS && defined(DEBUG)
    __error_injection_probe(NV2082_CTRL_GSP_INJECT_TARGET_NVPORT_MEM_ALLOC, 0);
    if (g_bMemoryAllocationFailure)
    {
        portDbgPrintf("  !!! MEMORY ALLOCATION FAILURE !!!\n");
        g_bMemoryAllocationFailure = NV_FALSE;
        return NULL;
    }
#endif
    if (pAlloc == NULL)
    {
        PORT_BREAKPOINT_CHECKED();
        return NULL;
    }

#if PORT_MEM_TRACK_USE_LIMIT
    if (portMemGlobals.bLimitEnabled)
    {
        if (osGetCurrentProcessGfid(&gfid) != NV_OK)
        {
            PORT_BREAKPOINT_CHECKED();
            return NULL;
        }
    }
#endif

    // Check if per-process memory limit will be exhausted by this allocation
    if (PORT_MEM_LIMIT_EXCEEDED(gfid, length))
        return NULL;

    if (length > 0)
    {
        NvLength paddedLength;
// RISCV64 requires 64-bit alignment of structures, and length indicates the alignment of the footer
#if defined(__riscv)
        if (PORT_MEM_STAGING_SIZE > 0 && (length & 7))
        {
            if (!portSafeAddLength(length & ~7, 8, &length))
            {
                return NULL;
            }
        }
#endif
        if (!portSafeAddLength(length, PORT_MEM_STAGING_SIZE, &paddedLength))
        {
            return NULL;
        }
        pMem = pAlloc->_portAlloc(pAlloc, paddedLength);
    }
    if (pMem != NULL)
    {
        pMem = PORT_MEM_ADD_HEADER_PTR(pMem);
        _portMemTrackAlloc(_portMemGetTracking(pAlloc), pMem, length, gfid
                           PORT_MEM_CALLERINFO_COMMA_PARAM);
    }
    return pMem;
}
void
_portMemAllocatorFree
(
    PORT_MEM_ALLOCATOR *pAlloc,
    void *pMem
)
{
    if (pAlloc == NULL)
    {
        PORT_BREAKPOINT_CHECKED();
        return;
    }
    if (pMem != NULL)
    {
        _portMemTrackFree(_portMemGetTracking(pAlloc), pMem);
        pMem = PORT_MEM_SUB_HEADER_PTR(pMem);
        pAlloc->_portFree(pAlloc, pMem);
    }
}

void
portMemPrintTrackingInfo
(
    const PORT_MEM_ALLOCATOR_TRACKING *pTracking
)
{
    if (pTracking == NULL)
        pTracking = &portMemGlobals.mainTracking;

    if (pTracking == &portMemGlobals.mainTracking)
        portDbgPrintf("[NvPort] ======== Aggregate Memory Tracking ========\n");
    else if ((pTracking == portMemGlobals.alloc.nonPaged.pTracking) &&
             (pTracking == portMemGlobals.alloc.paged.pTracking))
        portDbgPrintf("[NvPort] ======== Global Allocator Tracking ========\n");
    else if (pTracking == portMemGlobals.alloc.nonPaged.pTracking)
        portDbgPrintf("[NvPort] ======== Global Non-Paged Memory Allocator Tracking ========\n");
    else if (pTracking == portMemGlobals.alloc.paged.pTracking)
        portDbgPrintf("[NvPort] ======== Global Paged Memory Allocator Tracking ========\n");
#if PORT_MEM_TRACK_USE_LIMIT
    else if (isGfidValid(pTracking->gfid))
        portDbgPrintf("[NvPort] ======== GFID %u Tracking ========\n", pTracking->gfid);
#endif
    else
        portDbgPrintf("[NvPort] ======== Memory Allocator %p Tracking ======== \n", pTracking->pAllocator);

    if (pTracking->counter.activeAllocs != 0)
        portDbgPrintf("  !!! MEMORY LEAK DETECTED (%u blocks) !!!\n",
                      pTracking->counter.activeAllocs);

#if PORT_MEM_TRACK_USE_CALLERINFO
    {
        portDbgPrintf("  Allocator acquired "
            PORT_MEM_CALLERINFO_PRINT_ARGS(pTracking->callerInfo));
    }
#endif

#if PORT_IS_FUNC_SUPPORTED(portMemExTrackingGetHeapSize)
    //
    // Heap is shared across all allocators, so only print it with the
    // aggregate stats.
    //
    if (pTracking == _portMemGetTracking(NULL))
        portDbgPrintf("  HEAP:   %"NvUPtr_fmtu" bytes\n", portMemExTrackingGetHeapSize());
#endif

#if PORT_IS_FUNC_SUPPORTED(portMemExTrackingGetActiveStats)
    {
        PORT_MEM_TRACK_ALLOCATOR_STATS stats;

        portMemSet(&stats, 0, sizeof(stats));
#if PORT_MEM_TRACK_USE_LIMIT
        if (isGfidValid(pTracking->gfid))
        {
            portMemExTrackingGetGfidActiveStats(pTracking->gfid, &stats);
        }
        else
#endif
        {
            portMemExTrackingGetActiveStats(pTracking->pAllocator, &stats);
        }

        //
        // rmtest_gsp test script (dvs_gsp_sanity.sh) depends on this print, so do not change
        // format without updating script!
        //
        portDbgPrintf("  ACTIVE: %u allocations, %"NvUPtr_fmtu" bytes allocated (%"NvUPtr_fmtu" useful, %"NvUPtr_fmtu" meta)\n",
                    stats.numAllocations,
                    stats.allocatedSize,
                    stats.usefulSize,
                    stats.metaSize);
    }
#endif

#if PORT_IS_FUNC_SUPPORTED(portMemExTrackingGetTotalStats)
    {
        PORT_MEM_TRACK_ALLOCATOR_STATS stats;

        portMemSet(&stats, 0, sizeof(stats));
#if PORT_MEM_TRACK_USE_LIMIT
        if (isGfidValid(pTracking->gfid))
        {
            portMemExTrackingGetGfidTotalStats(pTracking->gfid, &stats);
        }
        else
#endif
        {
            portMemExTrackingGetTotalStats(pTracking->pAllocator, &stats);
        }
        portDbgPrintf("  TOTAL:  %u allocations, %"NvUPtr_fmtu" bytes allocated (%"NvUPtr_fmtu" useful, %"NvUPtr_fmtu" meta)\n",
                    stats.numAllocations,
                    stats.allocatedSize,
                    stats.usefulSize,
                    stats.metaSize);
    }
#endif

#if PORT_IS_FUNC_SUPPORTED(portMemExTrackingGetPeakStats)
    {
        PORT_MEM_TRACK_ALLOCATOR_STATS stats;

        portMemSet(&stats, 0, sizeof(stats));
#if PORT_MEM_TRACK_USE_LIMIT
        if (isGfidValid(pTracking->gfid))
        {
            portMemExTrackingGetGfidPeakStats(pTracking->gfid, &stats);
        }
        else
#endif
        {
            portMemExTrackingGetPeakStats(pTracking->pAllocator, &stats);
        }
        portDbgPrintf("  PEAK:   %u allocations, %"NvUPtr_fmtu" bytes allocated (%"NvUPtr_fmtu" useful, %"NvUPtr_fmtu" meta)\n",
                    stats.numAllocations,
                    stats.allocatedSize,
                    stats.usefulSize,
                    stats.metaSize);
    }
#endif

#if PORT_IS_FUNC_SUPPORTED(portMemExTrackingGetNext)
    {
        PORT_MEM_TRACK_ALLOC_INFO info;
        NvBool bPrinted = NV_FALSE;
        void *iterator = NULL;

        do
        {
            if (portMemExTrackingGetNext(pTracking->pAllocator, &info, &iterator) != NV_OK)
            {
                portDbgPrintf("  (no active allocations)\n");
                break;
            }
            else if (!bPrinted)
            {
                portDbgPrintf("  Currently active allocations:\n");
                bPrinted = NV_TRUE;
            }
            portDbgPrintf("   - A:%p - 0x%p [%8"NvUPtr_fmtu" bytes] T=%llu ",
                            info.pAllocator,
                            info.pMemory,
                            info.size,
                            info.timestamp);
            portDbgPrintf(PORT_MEM_CALLERINFO_PRINT_ARGS(info.callerInfo));
        } while (iterator != NULL);
    }
#endif
}

void
portMemPrintAllTrackingInfo(void)
{
    const PORT_MEM_ALLOCATOR_TRACKING *pTracking = &portMemGlobals.mainTracking;
    PORT_MEM_LOCK_ACQUIRE(portMemGlobals.trackingLock);
    do
    {
        portMemPrintTrackingInfo(pTracking);
    } while ((pTracking = pTracking->pNext) != &portMemGlobals.mainTracking);
    PORT_MEM_LOCK_RELEASE(portMemGlobals.trackingLock);
}

#if portMemExTrackingGetActiveStats_SUPPORTED
NV_STATUS
portMemExTrackingGetActiveStats
(
    const PORT_MEM_ALLOCATOR       *pAllocator,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
)
{
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = _portMemGetTracking(pAllocator);
    if (pTracking == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    pStats->numAllocations = pTracking->counter.activeAllocs;
    pStats->usefulSize     = pTracking->counter.activeSize;
    pStats->metaSize       = pStats->numAllocations * PORT_MEM_STAGING_SIZE;
    pStats->allocatedSize  = pStats->usefulSize + pStats->metaSize;
    return NV_OK;
}
#if PORT_MEM_TRACK_USE_LIMIT
NV_STATUS
portMemExTrackingGetGfidActiveStats
(
    NvU32                           gfid,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
)
{
    if (!isGfidValid(gfid))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NvU32 gfidIdx = gfid - 1;
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = portMemGlobals.pGfidTracking[gfidIdx];

    if (pTracking == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pStats->numAllocations = pTracking->counter.activeAllocs;
    pStats->usefulSize     = pTracking->counter.activeSize;
    pStats->metaSize       = pStats->numAllocations * PORT_MEM_STAGING_SIZE;
    pStats->allocatedSize  = pStats->usefulSize + pStats->metaSize;
    return NV_OK;
}
#endif
#endif

#if portMemExTrackingGetTotalStats_SUPPORTED
NV_STATUS
portMemExTrackingGetTotalStats
(
    const PORT_MEM_ALLOCATOR       *pAllocator,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
)
{
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = _portMemGetTracking(pAllocator);
    if (pTracking == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    pStats->numAllocations = pTracking->counter.totalAllocs;
    pStats->usefulSize     = pTracking->counter.totalSize;
    pStats->metaSize       = pStats->numAllocations * PORT_MEM_STAGING_SIZE;
    pStats->allocatedSize  = pStats->usefulSize + pStats->metaSize;
    return NV_OK;
}
#if PORT_MEM_TRACK_USE_LIMIT
NV_STATUS
portMemExTrackingGetGfidTotalStats
(
    NvU32                           gfid,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
)
{
    if (!isGfidValid(gfid))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NvU32 gfidIdx = gfid - 1;
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = portMemGlobals.pGfidTracking[gfidIdx];

    if (pTracking == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pStats->numAllocations = pTracking->counter.totalAllocs;
    pStats->usefulSize     = pTracking->counter.totalSize;
    pStats->metaSize       = pStats->numAllocations * PORT_MEM_STAGING_SIZE;
    pStats->allocatedSize  = pStats->usefulSize + pStats->metaSize;
    return NV_OK;
}
#endif
#endif

#if portMemExTrackingGetPeakStats_SUPPORTED
NV_STATUS
portMemExTrackingGetPeakStats
(
    const PORT_MEM_ALLOCATOR       *pAllocator,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
)
{
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = _portMemGetTracking(pAllocator);
    if (pTracking == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    pStats->numAllocations = pTracking->counter.peakAllocs;
    pStats->usefulSize     = pTracking->counter.peakSize;
    pStats->metaSize       = pStats->numAllocations * PORT_MEM_STAGING_SIZE;
    pStats->allocatedSize  = pStats->usefulSize + pStats->metaSize;
    return NV_OK;
}
#if PORT_MEM_TRACK_USE_LIMIT
NV_STATUS
portMemExTrackingGetGfidPeakStats
(
    NvU32                           gfid,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
)
{
    if (!isGfidValid(gfid))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NvU32 gfidIdx = gfid - 1;
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = portMemGlobals.pGfidTracking[gfidIdx];

    if (pTracking == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pStats->numAllocations = pTracking->counter.peakAllocs;
    pStats->usefulSize     = pTracking->counter.peakSize;
    pStats->metaSize       = pStats->numAllocations * PORT_MEM_STAGING_SIZE;
    pStats->allocatedSize  = pStats->usefulSize + pStats->metaSize;
    return NV_OK;
}
#endif
#endif

#if portMemExTrackingGetNext_SUPPORTED
NV_STATUS
portMemExTrackingGetNext
(
    const PORT_MEM_ALLOCATOR  *pAllocator,
    PORT_MEM_TRACK_ALLOC_INFO *pInfo,
    void                     **pIterator
)
{
    PORT_MEM_ALLOCATOR_TRACKING *pTracking = _portMemGetTracking(pAllocator);
    PORT_MEM_LIST *pList;
    PORT_MEM_HEADER *pHead;

    if (pTracking == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    if (pTracking->pFirstAlloc == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    if (*pIterator == NULL)
        pList = pTracking->pFirstAlloc;
    else
        pList = (PORT_MEM_LIST*)(*pIterator);

    pHead = _portMemListGetHeader(pList);

    // Advance iterator
    if (pList->pNext == pTracking->pFirstAlloc)
        *pIterator = NULL;
    else
        *pIterator = pList->pNext;

    // Populate pInfo
    pInfo->pMemory    = pHead + 1;
    pInfo->size       = _portMemExTrackingGetAllocUsableSizeWrapper(pInfo->pMemory);
    pInfo->pAllocator = pHead->fence.pAllocator;
    pInfo->timestamp  = 0;

#if PORT_MEM_TRACK_USE_CALLERINFO
    pInfo->callerInfo = pHead->callerInfo;
#endif

    return NV_OK;
}
#endif

static void
_portMemTrackingRelease
(
    PORT_MEM_ALLOCATOR_TRACKING *pTracking,
    NvBool bReportLeaks
)
{
    if (pTracking == NULL) return;

#if (PORT_MEM_TRACK_PRINT_LEVEL > PORT_MEM_TRACK_PRINT_LEVEL_SILENT)
    if (bReportLeaks && (pTracking->counter.activeAllocs != 0))
        portMemPrintTrackingInfo(pTracking);
#else
    PORT_UNREFERENCED_VARIABLE(bReportLeaks);
#endif

    PORT_LOCKED_LIST_UNLINK(&portMemGlobals.mainTracking, pTracking, portMemGlobals.trackingLock);
    PORT_MEM_LIST_DESTROY(pTracking);
#if PORT_MEM_TRACK_USE_LIMIT
    if (isGfidValid(pTracking->gfid))
    {
        _portMemFreeUntracked(pTracking);
    }
    else
#endif
    {
        PORT_MEM_ATOMIC_DEC_U32(&portMemGlobals.totalAllocators);
    }
}

static void
_portMemTrackAlloc
(
    PORT_MEM_ALLOCATOR_TRACKING *pTracking,
    void                        *pMem,
    NvLength                     size,
    NvU32                        gfid
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
    PORT_UNREFERENCED_VARIABLE(pMem);
    if (pTracking == NULL) return;

#if PORT_MEM_TRACK_ALLOC_SIZE
    //
    // Either set the block size in the header, or override it with the value
    // from the underlying allocator (which may be bigger than what was
    // requested). This keeps the counters consistent with the free path.
    //
    _portMemExTrackingSetOrGetAllocUsableSize(pMem, &size);
#endif

    PORT_MEM_PRINT_INFO("Allocated %"NvUPtr_fmtu" bytes at address %p", size, pMem);
    PORT_MEM_PRINT_INFO(PORT_MEM_CALLERINFO_PRINT_ARGS(PORT_MEM_CALLERINFO_PARAM));

    PORT_MEM_COUNTER_INC(&pTracking->counter, size);
    PORT_MEM_COUNTER_INC(&portMemGlobals.mainTracking.counter, size);
    PORT_MEM_LIMIT_INC(gfid, pMem, size);

    PORT_MEM_FENCE_INIT(pTracking->pAllocator, pMem, size);
    PORT_MEM_LIST_ADD(pTracking, pMem);
    PORT_MEM_CALLERINFO_INIT_MEM(pMem);
    PORT_MEM_LOG_ALLOC(pTracking->pAllocator, pMem, size);
#if PORT_MEM_TRACK_USE_LIMIT
    if (isGfidValid(gfid))
    {
        NvU32 gfidIdx = gfid - 1;

        PORT_MEM_COUNTER_INC(&portMemGlobals.pGfidTracking[gfidIdx]->counter, size);
    }
#endif
}

static void
_portMemTrackFree
(
    PORT_MEM_ALLOCATOR_TRACKING *pTracking,
    void                        *pMem
)
{
    NvLength size = 0;

    if (pTracking == NULL) return;

#if PORT_MEM_TRACK_ALLOC_SIZE
    size = _portMemExTrackingGetAllocUsableSizeWrapper(pMem);
    PORT_MEM_PRINT_INFO("Freeing %"NvUPtr_fmtu"-byte block at address %p\n", size, pMem);
#else
    PORT_MEM_PRINT_INFO("Freeing block at address %p\n", pMem);
#endif

    PORT_MEM_COUNTER_DEC(&pTracking->counter, size);
    PORT_MEM_COUNTER_DEC(&portMemGlobals.mainTracking.counter, size);
    PORT_MEM_LIMIT_DEC(pMem, size);

    PORT_MEM_FENCE_CHECK(pTracking->pAllocator, pMem, size);
    PORT_MEM_LIST_REMOVE(pTracking, pMem);
    PORT_MEM_LOG_FREE(pTracking->pAllocator, pMem);
#if PORT_MEM_TRACK_USE_LIMIT
    PORT_MEM_HEADER *pMemHeader = PORT_MEM_SUB_HEADER_PTR(pMem);

    if (isGfidValid(pMemHeader->gfid)) 
    {
        NvU32 gfidIdx = pMemHeader->gfid - 1;

        PORT_MEM_COUNTER_DEC(&portMemGlobals.pGfidTracking[gfidIdx]->counter, size);
    }
#endif
}


static void *
_portMemAllocatorAllocPagedWrapper
(
    PORT_MEM_ALLOCATOR *pAlloc,
    NvLength            length
)
{
    PORT_UNREFERENCED_VARIABLE(pAlloc);
    return _portMemAllocPagedUntracked(length);
}

static void *
_portMemAllocatorAllocNonPagedWrapper
(
    PORT_MEM_ALLOCATOR *pAlloc,
    NvLength            length
)
{
    PORT_UNREFERENCED_VARIABLE(pAlloc);
    return _portMemAllocNonPagedUntracked(length);
}

static void
_portMemAllocatorFreeWrapper
(
    PORT_MEM_ALLOCATOR *pAlloc,
    void               *pMem
)
{
    PORT_UNREFERENCED_VARIABLE(pAlloc);
    _portMemFreeUntracked(pMem);
}

static void
_portMemAllocatorReleaseWrapper
(
    PORT_MEM_ALLOCATOR *pAllocator
)
{
    portMemFree(pAllocator);
}

/// @todo Add these as intrinsics to UTIL module
static NV_INLINE NvBool _isBitSet(NvU32 *vect, NvU32 bit)
{
    return !!(vect[bit/32] & NVBIT32(bit%32));
}
static NV_INLINE void _setBit(NvU32 *vect, NvU32 bit)
{
    vect[bit/32] |= NVBIT32(bit%32);
}
static NV_INLINE void _clearBit(NvU32 *vect, NvU32 bit)
{
    vect[bit/32] &= ~NVBIT32(bit%32);
}

static PORT_MEM_ALLOCATOR *
_portMemAllocatorCreateOnExistingBlock
(
    void *pPreallocatedBlock,
    NvLength blockSizeBytes,
    void *pSpinlock
    PORT_MEM_CALLERINFO_COMMA_TYPE_PARAM
)
{
    PORT_MEM_ALLOCATOR *pAllocator = (PORT_MEM_ALLOCATOR *)pPreallocatedBlock;
    PORT_MEM_BITVECTOR *pBitVector;
    PORT_MEM_BITVECTOR_CHUNK *pLastChunkInBlock;
    NvU32 bitVectorSize;

    if ((pPreallocatedBlock == NULL) ||
        (blockSizeBytes < PORT_MEM_PREALLOCATED_BLOCK_MINIMAL_EXTRA_SIZE) ||
        (blockSizeBytes > NV_S32_MAX))
    {
        return NULL;
    }

    pAllocator->_portAlloc    = _portMemAllocatorAllocExistingWrapper;
    pAllocator->_portFree     = _portMemAllocatorFreeExistingWrapper;
    pAllocator->_portRelease  = NULL;
    pAllocator->pTracking     = NULL; // No tracking for this allocator
    pAllocator->pImpl         = (PORT_MEM_ALLOCATOR_IMPL*)(pAllocator + 1);


    //
    // PORT_MEM_BITVECTOR (pAllocator->pImpl) and PORT_MEM_ALLOCATOR_TRACKING (pAllocator->pImpl->tracking)
    // are mutually exclusively used.
    // When pAllocator->pTracking = NULL the data in pAllocator->pImpl->tracking is not used and instead 
    // pBitVector uses the same meory location. 
    // When pAllocator->pImpl->tracking there is no usage of PORT_MEM_BITVECTOR
    //
    pBitVector = (PORT_MEM_BITVECTOR*)(pAllocator->pImpl);
    pBitVector->pSpinlock = pSpinlock;

    // Calculate total number of chunks available
    pBitVector->pChunks = (PORT_MEM_BITVECTOR_CHUNK *)(pBitVector + 1);
    pBitVector->pChunks = (void*)NV_ALIGN_UP((NvUPtr)pBitVector->pChunks,
                                             (NvUPtr)PORT_MEM_BITVECTOR_CHUNK_SIZE);

    pLastChunkInBlock = (void*)NV_ALIGN_DOWN((NvUPtr)pPreallocatedBlock +
                                                     blockSizeBytes     -
                                                     PORT_MEM_BITVECTOR_CHUNK_SIZE,
                                             (NvUPtr)PORT_MEM_BITVECTOR_CHUNK_SIZE);
    if (pLastChunkInBlock < pBitVector->pChunks)
    {
        pBitVector->numChunks = 0;
    }
    else
    {
        pBitVector->numChunks = (NvU32)(pLastChunkInBlock - pBitVector->pChunks) + 1;
    }
    bitVectorSize = (NvU32)((NvU8*)pBitVector->pChunks - (NvU8*)pBitVector->bits);

    while (bitVectorSize*8 < pBitVector->numChunks*2)
    {
        // If too many chunks to track in current bit vector, increase bitvector by one chunk
        pBitVector->pChunks++;
        pBitVector->numChunks--;
        bitVectorSize = (NvU32)((NvU8*)pBitVector->pChunks - (NvU8*)pBitVector->bits);
    }
    portMemSet(pBitVector->bits, 0, bitVectorSize);

    PORT_MEM_PRINT_INFO("Acquired preallocated block allocator %p (%"NvUPtr_fmtu" bytes) ", pAllocator, blockSizeBytes);
    PORT_MEM_PRINT_INFO(PORT_MEM_CALLERINFO_PRINT_ARGS(PORT_MEM_CALLERINFO_PARAM));
    return pAllocator;
}

static void *
_portMemAllocatorAllocExistingWrapper
(
    PORT_MEM_ALLOCATOR *pAlloc,
    NvLength length
)
{
    NvU32 chunksNeeded = (NvU32)NV_DIV_AND_CEIL(length, PORT_MEM_BITVECTOR_CHUNK_SIZE);
    void *pMem = NULL;
    NvU32 chunksFound = 0;
    NvU32 i;
    PORT_MEM_BITVECTOR *pBitVector = (PORT_MEM_BITVECTOR*)(pAlloc->pImpl);
    PORT_SPINLOCK *pSpinlock = (PORT_SPINLOCK*)(pBitVector->pSpinlock);

    if (chunksNeeded > pBitVector->numChunks)
    {
        return NULL;
    }
    if (pSpinlock != NULL)
    {
        portSyncSpinlockAcquire(pSpinlock);
    }
    for (i = 0; i < pBitVector->numChunks; i++)
    {
        NvBool bWholeWordSet;
        bWholeWordSet = pBitVector->bits[i/32] == ~0U;
        if (bWholeWordSet || (_isBitSet(pBitVector->bits, i)))
        {
            // Chunk not available as whole.
            chunksFound = 0;
            // Skip fully set words
            if (bWholeWordSet)
            {
                i += 31;
            }
            if (chunksNeeded > (pBitVector->numChunks - i - (bWholeWordSet ? 1 : 0)))
            {
                break;
            }
        }
        else
        {
            chunksFound++;
            if (chunksFound == chunksNeeded)
            {
                NvU32 j;
                NvU32 firstAllocatedChunk = i - chunksFound + 1;

                pMem = pBitVector->pChunks[firstAllocatedChunk];
                // Mark all acquired chunks as occupied
                for (j = firstAllocatedChunk; j <= i; j++)
                {
                    _setBit(pBitVector->bits, j);
                }
                // Mark last chunk of allocation
                _setBit(pBitVector->bits, pBitVector->numChunks + i);
                break;
            }
        }
    }
    if (pSpinlock != NULL)
    {
        portSyncSpinlockRelease(pSpinlock);
    }
    if (pMem == NULL)
    {
         PORT_MEM_PRINT_ERROR("Memory allocation failed.\n");
    }
    return pMem;
}

static void
_portMemAllocatorFreeExistingWrapper
(
    PORT_MEM_ALLOCATOR *pAlloc,
    void *pMem
)
{
    PORT_MEM_BITVECTOR_CHUNK *pChunk = (PORT_MEM_BITVECTOR_CHUNK *)pMem;
    NvU32 i;
    PORT_MEM_BITVECTOR *pBitVector = (PORT_MEM_BITVECTOR*)(pAlloc->pImpl);
    PORT_SPINLOCK *pSpinlock = (PORT_SPINLOCK*)(pBitVector->pSpinlock);

    if (((NvUPtr)pMem < (NvUPtr)pBitVector->pChunks) ||
        ((NvUPtr)pMem > (NvUPtr)(pBitVector->pChunks + pBitVector->numChunks)))
    {
        // pMem not inside this allocator.
        PORT_BREAKPOINT_CHECKED();
        return;
    }

    if (pSpinlock != NULL)
    {
        portSyncSpinlockAcquire(pSpinlock);
    }
    for (i = (NvU32)(pChunk - pBitVector->pChunks); i < pBitVector->numChunks; i++)
    {
        // Mark chunk as free
        _clearBit(pBitVector->bits, i);
        if (_isBitSet(pBitVector->bits, pBitVector->numChunks + i))
        {
            // Clear last-allocation-bit and bail
            _clearBit(pBitVector->bits, pBitVector->numChunks + i);
            break;
        }
    }
    if (pSpinlock != NULL)
    {
        portSyncSpinlockRelease(pSpinlock);
    }
}
