/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "tls/tls.h"
#include "containers/map.h"
#include "nvport/nvport.h"

/// @todo Figure out which builds have upward stack. Looks like none?
#define STACK_GROWS_DOWNWARD 1

#if NVOS_IS_LIBOS
//
// On LibOS we have at most one passive thread (task_rm) and one ISR
// (task_interrupt) active at once (on same CPU core). Since these two will
// use different maps, we don't need to protect them with spinlocks.
// TLS structures are pre-allocated for single-threaded (1 passive, 1 ISR thread) runtimes.
//
#define TLS_IS_SINGLE_THREADED 1
#else
#define TLS_IS_SINGLE_THREADED 0
#endif // NVOS_IS_LIBOS

/**
 *  @brief Entry which counts how many times some data in TLS has been referenced.
 */
typedef struct TlsEntry
{
    NvU32 refCount;
    NvP64 pUserData;
    MapNode node;
} TlsEntry;

MAKE_INTRUSIVE_MAP(TlsEntryMap, TlsEntry, node);

/**
 *  @brief Single thread's TLS information
 */
typedef struct ThreadEntry
{
    union {
        NvU64 threadId; /// < For passive threads
        NvU64 sp;       /// < For ISR threads
    } key;              /// @todo Use node.key instead?
    TlsEntryMap map;
    MapNode node;

    NvU8 *pTlsBlock; // backing memory for TlsEntryMap nodes
    PORT_MEM_ALLOCATOR *pTlsEntryAllocator;
} ThreadEntry;

#define TLS_THREAD_ENTRY_BLOCK_SZ PORT_MEM_PREALLOCATED_BLOCK(sizeof(TlsEntry[16]))

MAKE_INTRUSIVE_MAP(ThreadEntryMap, ThreadEntry, node);

/**
 * @brief Stores all necessary data for TLS mechanism.
 *
 * @todo Use RW Spinlocks instead. Nice perf boost.
 */
typedef struct TlsDatabase
{
    /// @brief Allocator which allocates all necessary data for current @ref TlsDatabase.
    PORT_MEM_ALLOCATOR *pAllocator;
    /// @brief Last allocated entry id.
    NvU64 lastEntryId;

#if TLS_IS_SINGLE_THREADED
    ThreadEntry passiveThreadEntry;
    ThreadEntry isrThreadEntry;
#else
    /// @brief Lock for the passive thread entry map
    PORT_SPINLOCK *pLock;
    /// @brief Map of thread entries of non ISR threads.
    ThreadEntryMap threadEntries;

#if TLS_ISR_CAN_USE_LOCK
    /// @brief Lock which controls access to ISR-specific structures
    PORT_SPINLOCK *pIsrLock;
    /// @brief Map of thread entries of ISR threads.
    ThreadEntryMap isrEntries;
#else
#if !defined(TLS_ISR_UNIT_TEST)
#define TLS_MAX_ISRS 64
#else
#define TLS_MAX_ISRS 1024
#endif
    struct {
        volatile NvU64 sp;
        ThreadEntry   *pThreadEntry;
    } isrEntries[TLS_MAX_ISRS];
#endif // TLS_ISR_CAN_USE_LOCK
#endif // TLS_IS_SINGLE_THREADED

#if TLS_THREADS_CAN_RAISE_IRQL
    /**
     * @brief Number of ISRs / DPCs active on a given CPU.
     *
     * Every time an ISR starts, it increments this, and decrements on end.
     * Since ISRs never get rescheduled, and passive threads will never preempt
     * them, (isrCount[current_cpu] == 0) will be true IFF we're in ISR/DPC.
     */
    NvU32 *isrCount;
#endif

    volatile NvU32 initCount;
} TlsDatabase;

TlsDatabase tlsDatabase; // Zero initialized

// Helper function prototypes
static NvBool              _tlsIsIsr(void);
static ThreadEntry        *_tlsThreadEntryGet(void);
static ThreadEntry        *_tlsThreadEntryGetOrAlloc(void);
static NvP64               *_tlsEntryAcquire(ThreadEntry *pThreadEntry, NvU64 entryId, PORT_MEM_ALLOCATOR *pCustomAllocator);
static NvU32               _tlsEntryRelease(ThreadEntry *pThreadEntry, TlsEntry *pTlsEntry, PORT_MEM_ALLOCATOR *pCustomAllocator);
static PORT_MEM_ALLOCATOR *_tlsIsrAllocatorGet(void);
static PORT_MEM_ALLOCATOR *_tlsAllocatorGet(ThreadEntry *pThreadEntry);
static NV_STATUS           _tlsPassiveThreadEntryInit(ThreadEntry *pThreadEntry);

#if !TLS_IS_SINGLE_THREADED
static NV_STATUS           _tlsIsrEntriesInit(void);
static void                _tlsIsrEntriesDestroy(void);
static void                _tlsIsrEntriesInsert(ThreadEntry *pThreadEntry);
static ThreadEntry        *_tlsIsrEntriesRemove(NvU64 sp);
static ThreadEntry        *_tlsIsrEntriesFind(NvU64 approxSp);
#endif

#if TLS_THREADS_CAN_RAISE_IRQL
/// @todo move to NvPort (bug 1583359)
NvU32 osGetCurrentProcessorNumber(void);
#if defined(NVRM)
NvU32 osGetMaximumCoreCount(void);
#else
#define osGetMaximumCoreCount() 0x0
#endif
#endif

#if !PORT_IS_FUNC_SUPPORTED(portSyncExSafeToSleep)
#define portSyncExSafeToSleep() NV_TRUE
#endif

#if !PORT_IS_FUNC_SUPPORTED(portMemExSafeForNonPagedAlloc)
#define portMemExSafeForNonPagedAlloc() NV_TRUE
#endif

#if defined(TLS_PROFILING)
#include "tls_profiling.h"
#endif

NV_STATUS tlsInitialize(void)
{
    NV_STATUS status;

    if (portAtomicIncrementU32(&tlsDatabase.initCount) != 1)
    {
        return NV_OK; /// @todo Maybe return NV_WARN_NOTHING_TO_DO?
    }

    status = portInitialize();
    if (status != NV_OK)
        return status;

    tlsDatabase.pAllocator = portMemAllocatorCreateNonPaged();
    if (tlsDatabase.pAllocator == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

#if TLS_IS_SINGLE_THREADED
    portMemSet(&tlsDatabase.passiveThreadEntry, 0, sizeof(tlsDatabase.passiveThreadEntry));
    portMemSet(&tlsDatabase.isrThreadEntry, 0, sizeof(tlsDatabase.isrThreadEntry));

    status = _tlsPassiveThreadEntryInit(&tlsDatabase.passiveThreadEntry);
    if (status != NV_OK)
        goto done;

    mapInitIntrusive(&tlsDatabase.passiveThreadEntry.map);
#else
    tlsDatabase.pLock = portSyncSpinlockCreate(tlsDatabase.pAllocator);
    if (tlsDatabase.pLock == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }

    mapInitIntrusive(&tlsDatabase.threadEntries);

    status = _tlsIsrEntriesInit();
    if (status != NV_OK)
        goto done;
#endif

    tlsDatabase.lastEntryId = TLS_ENTRY_ID_DYNAMIC;

#if TLS_THREADS_CAN_RAISE_IRQL
{
    NvU32 maxCoreCount = osGetMaximumCoreCount();
    if (maxCoreCount == 0)
        maxCoreCount = 1; // MODS reports only 1 CPU at index 0.

    tlsDatabase.isrCount = PORT_ALLOC(tlsDatabase.pAllocator, maxCoreCount * sizeof(NvU32));
    if (tlsDatabase.isrCount == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }
    portMemSet(tlsDatabase.isrCount, 0, maxCoreCount * sizeof(NvU32));
}
#endif // TLS_THREADS_CAN_RAISE_IRQL

done:
    if (status != NV_OK)
    {
        tlsShutdown();
    }
    return status;
}

void tlsShutdown(void)
{
    if (portAtomicDecrementU32(&tlsDatabase.initCount) != 0)
    {
        return;
    }

#if defined(TLS_PROFILING)
    _tlsProfilePrint();
#endif

#if TLS_IS_SINGLE_THREADED
    portMemAllocatorRelease(tlsDatabase.passiveThreadEntry.pTlsEntryAllocator);
    PORT_FREE(tlsDatabase.pAllocator, tlsDatabase.passiveThreadEntry.pTlsBlock);
    mapDestroy(&tlsDatabase.passiveThreadEntry.map);
#else
    mapDestroy(&tlsDatabase.threadEntries);
    if (tlsDatabase.pLock)
        portSyncSpinlockDestroy(tlsDatabase.pLock);

    _tlsIsrEntriesDestroy();
#endif

    if (tlsDatabase.pAllocator)
    {
#if TLS_THREADS_CAN_RAISE_IRQL
        PORT_FREE(tlsDatabase.pAllocator, tlsDatabase.isrCount);
#endif
        portMemAllocatorRelease(tlsDatabase.pAllocator);
    }
    portMemSet(&tlsDatabase, 0, sizeof(tlsDatabase));
    portShutdown();
}

void tlsIsrInit(PORT_MEM_ALLOCATOR *pIsrAllocator)
{
    ThreadEntry * pThreadEntry = NULL;
    NV_ASSERT_OR_RETURN_VOID(tlsDatabase.initCount > 0);

    //
    // If TLS_THREADS_CAN_RAISE_IRQL we treat anything that calls tlsIsrInit as
    // ISR, and cannot perform this check. Will be moved to ASSERT later.
    // See CORERM-96
    //
    if (!TLS_THREADS_CAN_RAISE_IRQL && !_tlsIsIsr())
    {
        static NvBool bAlreadyPrinted = NV_FALSE;
        if (!bAlreadyPrinted)
        {
            NV_PRINTF(LEVEL_WARNING,
                "TLS: Unnecessary tlsIsrInit() call at %p. Will stop reporting further violations.\n",
                  (void*)portUtilGetReturnAddress());
            bAlreadyPrinted = NV_TRUE;
        }
        return;
    }

#if TLS_IS_SINGLE_THREADED
    pThreadEntry = &tlsDatabase.isrThreadEntry;
#else
    pThreadEntry = PORT_ALLOC(pIsrAllocator, sizeof(*pThreadEntry));
    NV_ASSERT_OR_RETURN_VOID(pThreadEntry != NULL);
#endif

    pThreadEntry->key.sp = (NvU64)(NvUPtr)pIsrAllocator;
    pThreadEntry->pTlsEntryAllocator = NULL;

    mapInitIntrusive(&pThreadEntry->map);

#if !TLS_IS_SINGLE_THREADED
    _tlsIsrEntriesInsert(pThreadEntry);
#endif

#if TLS_THREADS_CAN_RAISE_IRQL
    portAtomicIncrementU32(&tlsDatabase.isrCount[osGetCurrentProcessorNumber()]);
#endif
}

void tlsIsrDestroy(PORT_MEM_ALLOCATOR *pIsrAllocator)
{
    ThreadEntry *pThreadEntry = NULL;
    PORT_UNREFERENCED_VARIABLE(pIsrAllocator);
    NV_ASSERT_OR_RETURN_VOID(tlsDatabase.initCount > 0);

    if (!_tlsIsIsr())
    {
        if (TLS_THREADS_CAN_RAISE_IRQL)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "TLS: Calling tlsIsrDestroy() without accompanying tlsIsrInit at %p\n",
                    (void*)portUtilGetReturnAddress());
        }
        return;
    }

#if TLS_IS_SINGLE_THREADED
    pThreadEntry = &tlsDatabase.isrThreadEntry;
#else
    pThreadEntry = _tlsIsrEntriesRemove((NvU64)(NvUPtr)pIsrAllocator);
#endif

    mapDestroy(&pThreadEntry->map);

#if !TLS_IS_SINGLE_THREADED
    PORT_FREE(pIsrAllocator, pThreadEntry);
#endif

#if TLS_THREADS_CAN_RAISE_IRQL
    portAtomicDecrementU32(&tlsDatabase.isrCount[osGetCurrentProcessorNumber()]);
#endif
}

PORT_MEM_ALLOCATOR *tlsIsrAllocatorGet(void)
{

    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, NULL);
    return _tlsIsrAllocatorGet();
}

NvU64 tlsEntryAlloc(void)
{
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, TLS_ERROR_VAL);
    return portAtomicExIncrementU64(&tlsDatabase.lastEntryId);
}

NvBool tlsIsRefcounted(void)
{
    return NV_TRUE;
}

NvP64 *tlsEntryAcquire(NvU64 entryId)
{
    ThreadEntry *pThreadEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, NULL);

    // User tries allocation of unallocated entryId.
    NV_ASSERT_OR_RETURN(entryId <= tlsDatabase.lastEntryId ||
                        entryId >= TLS_ENTRY_ID_TAG_START, NULL);

    pThreadEntry = _tlsThreadEntryGetOrAlloc();
    NV_ASSERT_OR_RETURN(pThreadEntry != NULL, NULL);

    return _tlsEntryAcquire(pThreadEntry, entryId, NULL);
}

NvP64 *tlsEntryAcquireWithAllocator(NvU64 entryId, PORT_MEM_ALLOCATOR *pCustomAllocator)
{
    ThreadEntry *pThreadEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, NULL);

    // User tries allocation of unallocated entryId.
    NV_ASSERT_OR_RETURN(entryId <= tlsDatabase.lastEntryId ||
                        entryId >= TLS_ENTRY_ID_TAG_START, NULL);
    NV_ASSERT_OR_RETURN(pCustomAllocator != NULL, NULL);

    pThreadEntry = _tlsThreadEntryGetOrAlloc();
    NV_ASSERT_OR_RETURN(pThreadEntry != NULL, NULL);

    return _tlsEntryAcquire(pThreadEntry, entryId, pCustomAllocator);
}

NvU32 tlsEntryRelease(NvU64 entryId)
{
    ThreadEntry *pThreadEntry;
    TlsEntry *pTlsEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, TLS_ERROR_VAL);

    pThreadEntry = _tlsThreadEntryGet();
    NV_ASSERT_OR_RETURN(pThreadEntry != NULL, TLS_ERROR_VAL);

    pTlsEntry = mapFind(&pThreadEntry->map, entryId);
    NV_ASSERT_OR_RETURN(pTlsEntry != NULL, TLS_ERROR_VAL);

    return _tlsEntryRelease(pThreadEntry, pTlsEntry, NULL);
}

NvU32 tlsEntryReleaseWithAllocator(NvU64 entryId, PORT_MEM_ALLOCATOR *pCustomAllocator)
{
    ThreadEntry *pThreadEntry;
    TlsEntry *pTlsEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, TLS_ERROR_VAL);
    NV_ASSERT_OR_RETURN(pCustomAllocator != NULL, TLS_ERROR_VAL);

    pThreadEntry = _tlsThreadEntryGet();
    NV_ASSERT_OR_RETURN(pThreadEntry != NULL, TLS_ERROR_VAL);

    pTlsEntry = mapFind(&pThreadEntry->map, entryId);
    NV_ASSERT_OR_RETURN(pTlsEntry != NULL, TLS_ERROR_VAL);

    return _tlsEntryRelease(pThreadEntry, pTlsEntry, pCustomAllocator);
}

NvP64 tlsEntryGet(NvU64 entryId)
{
    ThreadEntry *pThreadEntry;
    TlsEntry *pTlsEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, NvP64_NULL);

    pThreadEntry = _tlsThreadEntryGet();
    if (pThreadEntry == NULL)
        return NvP64_NULL;

    pTlsEntry = mapFind(&pThreadEntry->map, entryId);
    return pTlsEntry ? pTlsEntry->pUserData : NvP64_NULL;
}

NvBool tlsEntrySet(NvU64 entryId, NvP64 entryData)
{
    ThreadEntry *pThreadEntry;
    TlsEntry *pTlsEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, NV_FALSE);

    pThreadEntry = _tlsThreadEntryGet();
    if (pThreadEntry == NULL)
        return NV_FALSE;

    pTlsEntry = mapFind(&pThreadEntry->map, entryId);

    if (pTlsEntry != NULL)
    {
        pTlsEntry->pUserData = entryData;
        return NV_TRUE;
    }

    return NV_FALSE;
}

NvU32 tlsEntryReference(NvU64 entryId)
{
    ThreadEntry *pThreadEntry;
    TlsEntry *pTlsEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, TLS_ERROR_VAL);

    pThreadEntry = _tlsThreadEntryGet();
    NV_ASSERT_OR_RETURN(pThreadEntry != NULL, TLS_ERROR_VAL);

    pTlsEntry = mapFind(&pThreadEntry->map, entryId);
    NV_ASSERT_OR_RETURN(pTlsEntry != NULL, TLS_ERROR_VAL);

    return ++pTlsEntry->refCount;
}

NvU32 tlsEntryUnreference(NvU64 entryId)
{
    ThreadEntry *pThreadEntry;
    TlsEntry *pTlsEntry;
    NV_ASSERT_OR_RETURN(tlsDatabase.initCount > 0, TLS_ERROR_VAL);

    pThreadEntry = _tlsThreadEntryGet();
    NV_ASSERT_OR_RETURN(pThreadEntry != NULL, TLS_ERROR_VAL);

    pTlsEntry = mapFind(&pThreadEntry->map, entryId);
    NV_ASSERT_OR_RETURN(pTlsEntry != NULL, TLS_ERROR_VAL);

    return --pTlsEntry->refCount;
}

static NV_STATUS _tlsPassiveThreadEntryInit(ThreadEntry *pThreadEntry)
{
    pThreadEntry->key.threadId = portThreadGetCurrentThreadId();
    pThreadEntry->pTlsBlock = PORT_ALLOC(tlsDatabase.pAllocator, TLS_THREAD_ENTRY_BLOCK_SZ);
    if (pThreadEntry->pTlsBlock == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    pThreadEntry->pTlsEntryAllocator = portMemAllocatorCreateOnExistingBlock(pThreadEntry->pTlsBlock, TLS_THREAD_ENTRY_BLOCK_SZ);
    if (pThreadEntry->pTlsEntryAllocator == NULL)
    {
        PORT_FREE(tlsDatabase.pAllocator, pThreadEntry->pTlsBlock);
        pThreadEntry->pTlsBlock = NULL;
        return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

#if TLS_IS_SINGLE_THREADED

static ThreadEntry *
_tlsThreadEntryGet(void)
{
    if (!_tlsIsIsr())
        return &tlsDatabase.passiveThreadEntry;
    else
        return &tlsDatabase.isrThreadEntry;
}

static ThreadEntry *
_tlsThreadEntryGetOrAlloc(void)
{
    return _tlsThreadEntryGet();
}

#else

static ThreadEntry *
_tlsThreadEntryGet(void)
{
    ThreadEntry *pThreadEntry;

    if (_tlsIsIsr())
    {
        pThreadEntry = _tlsIsrEntriesFind((NvU64)(NvUPtr)&pThreadEntry);
    }
    else
    {
        NvU64 threadId = portThreadGetCurrentThreadId();
        portSyncSpinlockAcquire(tlsDatabase.pLock);
        pThreadEntry = mapFind(&tlsDatabase.threadEntries, threadId);
        portSyncSpinlockRelease(tlsDatabase.pLock);
    }
    return pThreadEntry;
}


static ThreadEntry *
_tlsThreadEntryGetOrAlloc(void)
{
    ThreadEntry* pThreadEntry = NULL;

    pThreadEntry = _tlsThreadEntryGet();
    if (pThreadEntry == NULL) // Only non-ISRs can be missing
    {
        NV_ASSERT(portMemExSafeForNonPagedAlloc());
        pThreadEntry = PORT_ALLOC(tlsDatabase.pAllocator, sizeof(*pThreadEntry));
        if (pThreadEntry != NULL)
        {
            if (_tlsPassiveThreadEntryInit(pThreadEntry) != NV_OK)
            {
                PORT_FREE(tlsDatabase.pAllocator, pThreadEntry);
                return NULL;
            }

            mapInitIntrusive(&pThreadEntry->map);
            portSyncSpinlockAcquire(tlsDatabase.pLock);
            mapInsertExisting(&tlsDatabase.threadEntries,
                              pThreadEntry->key.threadId,
                              pThreadEntry);
            portSyncSpinlockRelease(tlsDatabase.pLock);
        }
    }

    return pThreadEntry;
}
#endif // TLS_IS_SINGLE_THREADED

static NvP64*
_tlsEntryAcquire
(
    ThreadEntry *pThreadEntry,
    NvU64 entryId,
    PORT_MEM_ALLOCATOR *pCustomAllocator
)
{
    TlsEntry *pTlsEntry;
    PORT_MEM_ALLOCATOR *pAllocator;

    pAllocator = (pCustomAllocator != NULL) ? pCustomAllocator : _tlsAllocatorGet(pThreadEntry);
    pTlsEntry = mapFind(&pThreadEntry->map, entryId);
    if (pTlsEntry != NULL)
    {
        pTlsEntry->refCount++;
    }
    else
    {
        pTlsEntry = PORT_ALLOC(pAllocator, sizeof(*pTlsEntry));
        NV_ASSERT_OR_RETURN(pTlsEntry != NULL, NULL);
        mapInsertExisting(&pThreadEntry->map, entryId, pTlsEntry);

        pTlsEntry->refCount = 1;
        pTlsEntry->pUserData = NvP64_NULL;
    }
    return &pTlsEntry->pUserData;
}

static NvU32
_tlsEntryRelease
(
    ThreadEntry* pThreadEntry,
    TlsEntry *pTlsEntry,
    PORT_MEM_ALLOCATOR *pCustomAllocator
)
{
    NvU32 refCount;
    PORT_MEM_ALLOCATOR *pAllocator;
    pAllocator = (pCustomAllocator != NULL) ? pCustomAllocator : _tlsAllocatorGet(pThreadEntry);

    refCount = --pTlsEntry->refCount;
    if (refCount == 0)
    {
        mapRemove(&pThreadEntry->map, pTlsEntry);
        PORT_FREE(pAllocator, pTlsEntry);

#if !TLS_IS_SINGLE_THREADED
        // Only non ISR Thread Entry can be deallocated.
        if (!_tlsIsIsr() && (mapCount(&pThreadEntry->map) == 0))
        {
            NV_ASSERT(portMemExSafeForNonPagedAlloc());

            portMemAllocatorRelease(pThreadEntry->pTlsEntryAllocator);
            PORT_FREE(tlsDatabase.pAllocator, pThreadEntry->pTlsBlock);

            mapDestroy(&pThreadEntry->map);
            portSyncSpinlockAcquire(tlsDatabase.pLock);
            mapRemove(&tlsDatabase.threadEntries, pThreadEntry);
            portSyncSpinlockRelease(tlsDatabase.pLock);
            PORT_FREE(tlsDatabase.pAllocator, pThreadEntry);
        }
#endif
    }
    return refCount;
}

static PORT_MEM_ALLOCATOR *_tlsIsrAllocatorGet(void)
{
    ThreadEntry *pThreadEntry;

    if (!_tlsIsIsr()) { return NULL; }
    pThreadEntry = _tlsThreadEntryGet();

    return (PORT_MEM_ALLOCATOR*)(NvUPtr)pThreadEntry->key.sp;
}

static PORT_MEM_ALLOCATOR *_tlsAllocatorGet(ThreadEntry *pThreadEntry)
{
    PORT_MEM_ALLOCATOR *pAllocator = NULL;

    if (_tlsIsIsr())
    {
        pAllocator = _tlsIsrAllocatorGet();
    }
    else
    {
        pAllocator = pThreadEntry->pTlsEntryAllocator;
    }

    if (pAllocator == NULL)
        pAllocator = tlsDatabase.pAllocator;

    return pAllocator;
}

#if !TLS_IS_SINGLE_THREADED
#if TLS_ISR_CAN_USE_LOCK

static NV_STATUS _tlsIsrEntriesInit(void)
{
    tlsDatabase.pIsrLock = portSyncSpinlockCreate(tlsDatabase.pAllocator);
    if (tlsDatabase.pLock == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    mapInitIntrusive(&tlsDatabase.isrEntries);
    return NV_OK;
}
static void _tlsIsrEntriesDestroy(void)
{
    if (tlsDatabase.pIsrLock)
        portSyncSpinlockDestroy(tlsDatabase.pIsrLock);
    mapDestroy(&tlsDatabase.isrEntries);
}
static void _tlsIsrEntriesInsert(ThreadEntry *pThreadEntry)
{
    portSyncSpinlockAcquire(tlsDatabase.pIsrLock);
    mapInsertExisting(&tlsDatabase.isrEntries, pThreadEntry->key.sp, pThreadEntry);
    portSyncSpinlockRelease(tlsDatabase.pIsrLock);
}
static ThreadEntry *_tlsIsrEntriesRemove(NvU64 sp)
{
    ThreadEntry *pThreadEntry;
    portSyncSpinlockAcquire(tlsDatabase.pIsrLock);
    pThreadEntry = mapFind(&tlsDatabase.isrEntries, sp);
    mapRemove(&tlsDatabase.isrEntries, pThreadEntry);
    portSyncSpinlockRelease(tlsDatabase.pIsrLock);
    return pThreadEntry;
}
static ThreadEntry *_tlsIsrEntriesFind(NvU64 approxSp)
{
    ThreadEntry *pThreadEntry;
    portSyncSpinlockAcquire(tlsDatabase.pIsrLock);
#if STACK_GROWS_DOWNWARD
    pThreadEntry = mapFindGEQ(&tlsDatabase.isrEntries, approxSp);
#else
    pThreadEntry = mapFindLEQ(&tlsDatabase.isrEntries, approxSp);
#endif
    portSyncSpinlockRelease(tlsDatabase.pIsrLock);
    return pThreadEntry;
}

#else // Lockless

static NV_STATUS _tlsIsrEntriesInit(void)
{
    portMemSet(tlsDatabase.isrEntries, 0, sizeof(tlsDatabase.isrEntries));
    return NV_OK;
}
static void _tlsIsrEntriesDestroy(void)
{
    portMemSet(tlsDatabase.isrEntries, 0, sizeof(tlsDatabase.isrEntries));
}
static void _tlsIsrEntriesInsert(ThreadEntry *pThreadEntry)
{
    NvU32 i = 0;

    while (!portAtomicExCompareAndSwapU64(&tlsDatabase.isrEntries[i].sp,
                                         pThreadEntry->key.sp, 0))
    {
        i = (i + 1) % TLS_MAX_ISRS;
    }
    tlsDatabase.isrEntries[i].pThreadEntry = pThreadEntry;
}
static ThreadEntry *_tlsIsrEntriesRemove(NvU64 sp)
{
    ThreadEntry *pThreadEntry;
    NvU32 i = 0;

    while (tlsDatabase.isrEntries[i].sp != sp)
    {
        i++;
    }
    pThreadEntry = tlsDatabase.isrEntries[i].pThreadEntry;
    portAtomicExSetU64(&tlsDatabase.isrEntries[i].sp, 0);

    return pThreadEntry;
}
static ThreadEntry *_tlsIsrEntriesFind(NvU64 approxSp)
{
    NvU32 i;
    NvU32 closestIdx = ~0x0;
    NvU64 closestSp = STACK_GROWS_DOWNWARD ? ~0ULL : 0;

    for (i = 0; i < TLS_MAX_ISRS; i++)
    {
        NvU64 sp = tlsDatabase.isrEntries[i].sp;
#if STACK_GROWS_DOWNWARD
        if (sp != 0 && sp >= approxSp && sp < closestSp)
#else
        if (sp != 0 && sp <= approxSp && sp > closestSp)
#endif
        {
            closestSp = sp;
            closestIdx = i;
        }
    }
    NV_ASSERT_OR_RETURN(closestIdx != ~0x0, NULL);
    return tlsDatabase.isrEntries[closestIdx].pThreadEntry;
}

#endif // TLS_ISR_CAN_USE_LOCK
#endif // TLS_IS_SINGLE_THREADED


static NvBool _tlsIsIsr(void)
{
#if defined (TLS_ISR_UNIT_TEST)
    // In unit tests we simulate ISR tests in different ways, so tests define this
    extern NvBool tlsTestIsIsr(void);
    return tlsTestIsIsr();
#elif TLS_THREADS_CAN_RAISE_IRQL
    NvU64 preempt = portSyncExDisablePreemption();
    NvBool bIsIsr = (tlsDatabase.isrCount[osGetCurrentProcessorNumber()] > 0);
    portSyncExRestorePreemption(preempt);
    return bIsIsr;
#else // Usermode and most kernelmode platforms
    return portUtilIsInterruptContext();
#endif // TLS_ISR_UNIT_TEST
}
