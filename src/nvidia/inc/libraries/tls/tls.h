/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Thread local storage public interface
 */

#include "nvport/nvport.h"

#ifndef _NV_TLS_H_
#define _NV_TLS_H_

/**
 * @defgroup Thread local storage operations
 *
 * @brief This module contains thread local storage functionality used by other
 * modules.
 *
 * @par Module dependencies:
 *   - NvPort (UTIL, ATOMIC, MEMORY, SYNC and THREAD modules)
 *   - NvContainers (Map)
 *   - NvUtils (NV_PRINTF and NV_ASSERT)
 *
 * @par TLS architecture:
 *   A base TLS allocation unit is an Entry (@ref TLS_ENTRY). Entries are local
 *   to a thread and are identified by a 64bit ID. Entries are lazy-allocated
 *   and refcounted. All entries for a given thread are organized in one Map -
 *   i.e. TLS has as many Maps active as there are threads; each map is
 *   inherently single-threaded. The Map for a given thread ID is obtained by
 *   searching a map of all threads with thread ID as key.
 *   The whole TLS system can be thought of as:
 *       map<thread_id, map<entry_id, void*>>
 *
 * @par Complexity:
 *   All operations are O(log(numActiveThreads) + log(numEntriesForGivenThread))
 *
 * @par A note on ISRs and DPCs
 *   Interrupt Service Routines (and in some cases Deferred Procedure Calls) do
 *   not have their own thread IDs - they can have the same ID as a regular
 *   thread. Because of this, they are kept in a separate map indexed by their
 *   stack pointer instead of thread ID. Because getting the exact base of the
 *   ISR stack can be difficult, when searching we use the closest one, in the
 *   direction of stack growth. This assumes that the given entry always exists,
 *   so ISR thread entries are preallocated with @ref tlsIsrInit.
 *
 *   An example of how this works:
 * ~~~{.c}
 *    if (is_isr())
 *        return isr_map.find(get_approx_sp());
 *    else
 *        return thread_map.find(get_thread_id());
 * ~~~
 *   The exact definition of is_isr() varies by platform, but generally means
 *   "if it does not have a unique thread ID". Threaded IRQs are not ISRs.
 *
 * @par Locking:
 *   Currently, TLS has two spinlocks - separate locks for ISR and passive
 *   thread maps. This will be changed to RW-spinlocks in the future.
 *   We cannot use RW sleeper locks in passive threads, since they may modify
 *   their IRQL and thus be unable to acquire them, even conditionally.
 *
 *   In cases where ISRs are not allowed to acquire a spinlock at all, the ISR
 *   map is implemented in a lockless fashion. This is slower than the locked
 *   implementation (O(maxIsrs)), but works in cases where all locks are banned.
 *
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note Only returned in cases of irregular order of public API calls.
 */
#define TLS_ERROR_VAL ~0

/**
 * @brief Global TLS structure initialization.
 *
 * Must be called before any TLS functions can be called.
 *
 * If this function returns an error then calling any TLS function will result
 * in undefined behavior.
 *
 * Called on RmInitRm().
 * @return NV_OK if successful;
 * @return Error code  otherwise.
 *
 */
NV_STATUS tlsInitialize(void);

/**
 * @brief Global TLS structure termination.
 *
 * It frees resources allocated by tlsInitialize.
 * Called on RmDestroyRm().
 *
 */
void tlsShutdown(void);

enum {
    TLS_ENTRY_ID_THREADSTATE,
    TLS_ENTRY_ID_RESSERV_1,
    TLS_ENTRY_ID_CURRENT_GPU_INSTANCE,
    TLS_ENTRY_ID_PRIORITY,
    TLS_ENTRY_ID_GPUMGR_EXPANDED_GPU_VISIBILITY,
    TLS_ENTRY_ID_DYNAMIC, // dynamic allocations start here
    TLS_ENTRY_ID_TAG_START = 0x100000 // Custom tags start here
};
/**
 * @brief Allocates a new entry spot and returns a unique entry ID.
 *
 * Ids are unique for all threads.
 *
 * @return 0 if all ids are used;
 * @return unique id otherwise.
 *
 */
NvU64 tlsEntryAlloc(void);

/**
 * @brief Get pointer to TLS entry for given @p entryId.
 *
 * This function increments the refCount of the given entry.
 *
 * @return NULL if @p entryId is invalid (Not returned by @ref tlsEntryAlloc),
 * in case of not enough memory.
 * @return Pointer to a void* the users can use to point to custom structure.
 *
 * Example usage:
 * ~~~{.c}
 * NvU64 id = tlsEntryAlloc();
 * MY_THREAD_DATA **ppData = tlsEntryAcquire(id);
 * if (**ppData == NULL)
 *     *ppData = portMemAllocNonPaged(sizeof(MY_THREAD_DATA))
 * ~~~
 *
 * @note On first call for given @p entryId, the dereferenced (user) pointer
 * will be set to NULL - (*tlsEntryAcquire(x) == NULL)
 *
 */
NvP64 *tlsEntryAcquire(NvU64 entryId);

/**
 * @brief Like @ref tlsEntryAcquire, but  memory is allocated using @p pAllocator.
 *
 * @note Should be used only when performance is important in allocation or
 * when a spinlock is acquired in a non ISR thread and there is a need for the tls.
 *
 * @note pAllocator should be thread safe.
 */
NvP64 *tlsEntryAcquireWithAllocator(NvU64 entryId, PORT_MEM_ALLOCATOR *pAllocator);

/**
 * @brief Release the TLS entry for given @p entryId.
 *
 * This functions decrements the refCount of the given entry.
 *
 * @return refCount after releasing the structure if @p entryId is valid,
 * @return TLS_ERROR_VAL if TLS entry for given @p entryId doesn't exist.
 *
 * ~~~{.c}
 * if (tlsEntryRelease(id) == 0)
 *     portMemFree(*ppData);
 * ~~~
 */
NvU32 tlsEntryRelease(NvU64 entryId);

/**
 * @brief Like @ref tlsEntryRelease, but  memory is allocated using @p pAllocator.
 *
 * @note Should be used only when performance is important in allocation or
 * when a spinlock is acquired in a non ISR thread and there is a need for the tls.
 *
 * @note @p pAllocator should be thread safe.
 */
NvU32 tlsEntryReleaseWithAllocator(NvU64 entryId, PORT_MEM_ALLOCATOR *pAllocator);

/**
 * @brief Get pointer to TLS data for given entryId.
 *
 * This function will not modify the refCount, and does not return a double
 * pointer required to set the entry value.
 *
 * @return NULL if the entry doesn't exist.
 * @return Otherwise pointer on user's custom structure.
 *
 * Example usage:
 * ~~~{.c}
 * NvU64 id = tlsEntryAlloc();
 * MY_THREAD_DATA **ppData = tlsEntryAcquire(id);
 * if (**ppData == NULL)
 * {
 *     *ppData = portMemAllocNonPaged(sizeof(MY_THREAD_DATA))
 *     *ppData->myData = 1;
 * }
 * MY_THREAD_DATA *pData = tlsEntryGet(id);
 * if (pData->myData == 1)
 * {
 *  ...
 * }
 * ~~~
 *
 */
NvP64 tlsEntryGet(NvU64 entryId);

/**
 * @brief Increment the refCount of given TLS entry.
 *
 * If an entry with given entryId doesn't exist, this function does nothing.
 *
 * This is useful when the code requires a call to a function that might call
 * @ref tlsEntryRelease, but TLS should not be freed. An example might be when
 * calling a function that acquires the GPU lock while already holding the lock.
 * Currently, the code will temporarily release the lock, so the nested function
 * acquires it again. Since rmGpuLock{Acquire,Release} acquires/releases TLS,
 * this release could cause the data to be freed.
 *
 * @return TLS_ERROR_VAL if the entry doesn't exist.
 * @return New TLS entry refCount, after increment.
 *
 */
NvU32 tlsEntryReference(NvU64 entryId);

/**
 * @brief Decrement the refCount of given TLS entry.
 *
 * If an entry with given entryId doesn't exist, this function does nothing.
 * See @ref tlsEntryReference for details.
 *
 * @return TLS_ERROR_VAL if the entry doesn't exist.
 * @return New TLS entry refCount, after decrement.
 *
 */
NvU32 tlsEntryUnreference(NvU64 entryId);

/// @brief Size of memory to preallocate on ISR stack for TLS
#if PORT_IS_CHECKED_BUILD
// Checked builds have per-allocation overhead for tracking
#define TLS_ISR_ALLOCATOR_SIZE 512
#else
#if defined(LOCK_VAL_ENABLED)
    #define TLS_ISR_ALLOCATOR_SIZE 512
#else
    #define TLS_ISR_ALLOCATOR_SIZE 256
#endif
#endif

/**
 * @brief Allocates thread id for current ISR thread.
 *
 * @note Function should be called on the beginning of ISR, as early as possible
 *
 */
void tlsIsrInit(PORT_MEM_ALLOCATOR *pIsrAllocator);

/**
 * @brief Destroys thread id for current ISR thread.
 *
 * @note should be called at end of ISR. Must be NOINLINE because if it gets
 * inlined and tlsIsrInit doesn't, SP order can be wrong.
 */
NV_NOINLINE void tlsIsrDestroy(PORT_MEM_ALLOCATOR *pIsrAllocator);

/**
 * @brief Returns allocator that can be used for allocations of memory in ISR
 * threads.
 * In case this function is called outside of ISR NULL will be returned.
 * @note Should be called between tlsIsrInit and tlsIsrDestroy if you are in ISR,
 * otherwise it will ASSERT and return NULL.
 */
PORT_MEM_ALLOCATOR *tlsIsrAllocatorGet(void);

/**
 * @brief Set if DPCs have a unique thread ID that can be acquired by
 * @ref portThreadGetCurrentThreadId. Windows DPCs have the same thread ID
 * as the thread they preempted, so they are treated like ISRs.
 *
 * This isn't used by the TLS implementation, but is needed to decide whether
 * the DPCs should call @ref tlsIsrInit
 */
#if PORT_IS_KERNEL_BUILD && !defined(NV_MODS) && NVOS_IS_WINDOWS
#define TLS_DPC_HAVE_UNIQUE_ID 0
#else
#define TLS_DPC_HAVE_UNIQUE_ID 1
#endif

/**
 * @brief Set if threads can modify their own IRQL/interrupt context.
 * On such builds, we cannot use @ref portUtilIsInterruptContext to decide
 * whether a given thread is an ISR or a passive thread, and instead use a
 * per-CPU ISR counter.
 */
#if PORT_IS_KERNEL_BUILD && (defined(NV_MODS) || NVOS_IS_WINDOWS)
#define TLS_THREADS_CAN_RAISE_IRQL 1
#else
#define TLS_THREADS_CAN_RAISE_IRQL 0
#endif

/**
 * @brief Set if ISRs are allowed to acquire a spinlock. On VMWare, the top
 * level interrupt handler (ACK function) is not allowed to hold the spinlock
 * for any amount of time (enforced by validation suite), so it uses a slower
 * lockless implementation.
 */
#if PORT_IS_KERNEL_BUILD && NVOS_IS_VMWARE
#define TLS_ISR_CAN_USE_LOCK 0
#else
#define TLS_ISR_CAN_USE_LOCK 1
#endif

/// @brief If set, a copy of THREAD_STATE_NODE pointer will be kept in TLS.
#ifndef TLS_MIRROR_THREADSTATE
#define TLS_MIRROR_THREADSTATE 0
#endif

#ifdef __cplusplus
}
#endif

///@}

#endif
