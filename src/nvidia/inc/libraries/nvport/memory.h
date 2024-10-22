/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Memory module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_MEMORY_H_
#define _NVPORT_MEMORY_H_

/**
 * Platform-specific inline implementations
 */
#if NVOS_IS_LIBOS
#include "nvport/inline/memory_libos.h"
#endif

// Go straight at the memory or hardware.
#define PORT_MEM_RD08(p) (*(p))
#define PORT_MEM_RD16(p) (*(p))
#define PORT_MEM_RD32(p) (*(p))
#define PORT_MEM_RD64(p) (*(p))
#define PORT_MEM_WR08(p, v) (*(p) = (v))
#define PORT_MEM_WR16(p, v) (*(p) = (v))
#define PORT_MEM_WR32(p, v) (*(p) = (v))
#define PORT_MEM_WR64(p, v) (*(p) = (v))

/**
 * @defgroup NVPORT_MEMORY Memory
 * @brief This module contains memory management related functionality.
 *
 * @{
 */

/**
 * @brief Single allocation description - forward reference.
 */
struct PORT_MEM_TRACK_ALLOC_INFO;
typedef struct PORT_MEM_TRACK_ALLOC_INFO PORT_MEM_TRACK_ALLOC_INFO;


/**
 * @name Core Functions
 * @{
 */


/**
 * @brief Initializes global Memory tracking structures.
 *
 * This function is called by @ref portInitialize. It is available here in case
 * it is needed to initialize the MEMORY module without initializing all the
 * others. e.g. for unit tests.
 */
void portMemInitialize(void);
/**
 * @brief Destroys global Memory tracking structures, and checks for leaks
 *
 * This function is called by @ref portShutdown. It is available here in case
 * it is needed to initialize the MEMORY module without initializing all the
 * others. e.g. for unit tests.
 *
 * @param bForceSilent - Will not print the report, even if
 * @ref PORT_MEM_TRACK_PRINT_LEVEL isn't PORT_MEM_TRACK_PRINT_LEVEL_SILENT
 */
void portMemShutdown(NvBool bForceSilent);


/**
 * @brief Allocates pageable virtual memory of given size.
 *
 * Will allocate at least lengthBytes bytes and return a pointer to the
 * allocated virtual memory. The caller will be able to both read and write
 * the returned memory via standard pointer accesses.
 *
 * The memory is not guaranteed to be initialized before being returned to the
 * caller.
 *
 * An allocation request of size 0 will result in a return value of NULL.
 *
 * @par Checked builds only:
 * Requests of size 0 will breakpoint/assert.
 *
 * @par Undefined:
 * It is possible this function will consume more than lengthBytes of virtual
 * address space. However behavior is undefined if the caller attempts to read
 * or write addresses beyond lengthBytes.
 *
 * @return  Pointer to requested memory, NULL if allocation fails.
 *
 * @note Calling this function is identical to calling
 * @ref PORT_ALLOC ( @ref portMemAllocatorGetGlobalPaged() , lengthBytes)
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
NV_FORCERESULTCHECK void *portMemAllocPaged(NvLength lengthBytes);

/**
 * @brief Allocates non-paged (i.e. pinned) memory.
 *
 * This function is essentially the same to @ref portMemAllocPaged except that
 * the virtual memory once returned will always be resident in CPU memory.
 *
 * @return  Pointer to requested memory, NULL if allocation fails.
 *
 * @note Calling this function is identical to calling
 * @ref PORT_ALLOC ( @ref portMemAllocatorGetGlobalNonPaged()  , lengthBytes)
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
NV_FORCERESULTCHECK void *portMemAllocNonPaged(NvLength lengthBytes);

/**
 * @brief Allocates non-paged (i.e. pinned) memory on the stack or the heap
 *
 * USE ONLY FOR MEMORY THAT ALLOCATED AND FREED IN THE SAME FUNCTION!
 *
 * This function allocates memory on the stack for platforms with a large stack.
 * Otherwise it is defined to @ref portMemAllocNonPaged and @ref portMemFree.
 */
#define portMemExAllocStack(lengthBytes) __builtin_alloca(lengthBytes)
#define portMemExAllocStack_SUPPORTED PORT_COMPILER_IS_GCC

#if portMemExAllocStack_SUPPORTED && NVOS_IS_LIBOS
#define portMemAllocStackOrHeap(lengthBytes) portMemExAllocStack(lengthBytes)
#define portMemFreeStackOrHeap(pData)
#else
#define portMemAllocStackOrHeap(size) portMemAllocNonPaged(size)
#define portMemFreeStackOrHeap(pData) portMemFree(pData)
#endif

/**
 * @brief Frees memory allocated by @ref portMemAllocPaged or @ref portMemAllocNonPaged.
 *
 * Frees either paged or non-paged virtual memory. The pointer passed in must
 * have been the exact value returned by the allocation routine.
 *
 * Calling with NULL has no effect.
 *
 * @par Checked builds only:
 * Will fill the memory with a pattern to help detect use after free. <br>
 * Will assert/breakpoint if the memory fenceposts have been corrupted
 *
 * @par Undefined:
 * Freeing the same address multiple times results in undefined behavior. <br>
 * Accessing memory in the region freed by this function results in undefined
 * behavior. It may generate a page fault, or if the memory has been
 * reallocated (or kept around to optimize subsequent allocation requests) then
 * the access may unexpectedly work.
 *
 * @pre Windows: IRQL <= APC_LEVEL (DISPATCH_LEVEL if freeing NonPaged memory)
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
void portMemFree(void *pData);

/**
 * @brief Copies data from one address to another.
 *
 * Copies srcSize bytes from pSource to pDestination, returning pDestination.
 * pDestination should be at least destSize bytes, pSource at least srcSize.
 * destSize should be equal or greater to srcSize.
 *
 * If destSize is 0, it is guaranteed to not access either buffer.
 *
 * @par Undefined:
 * Behavior is undefined if memory regions referred to by pSource and
 * pDestination overlap.
 *
 * @par Checked builds only:
 * Will assert/breakpoint if the regions overlap. <br>
 * Will assert/breakpoint if destSize < srcSize <br>
 * Will assert/breakpoint if either pointer is NULL
 *
 * @return pDestination on success, NULL if the operation failed.
 *
 */
void *portMemCopy(void *pDestination, NvLength destSize, const void *pSource, NvLength srcSize);

/**
 * @brief Moves data from one address to another.
 *
 * Copies memory from pSource to pDestination, returning pDestination.
 * pDestination should be at least destSize bytes, pSource at least srcSize.
 * srcSize should be equal or greater to destSize.
 *
 * If destSize is 0, it is guaranteed to not access either buffer.
 *
 * Unlike @ref portMemCopy this function allows the regions to overlap.
 *
 * @par Checked builds only:
 * Will assert/breakpoint if destSize < srcSize <br>
 * Will assert/breakpoint if either pointer is NULL
 *
 * @return pDestination on success, NULL if the operation failed.
 *
 */
void *portMemMove(void *pDestination, NvLength destSize, const void *pSource, NvLength srcSize);

/**
 * @brief Sets given memory to specified value.
 *
 * Writes lengthBytes bytes of data starting at pData with value.
 * The buffer is assumed to have the size of at least lengthBytes.
 *
 * if lengthBytes is 0 it is guaranteed to not access pData.
 *
 * @return pData
 */
void *portMemSet(void *pData, NvU8 value, NvLength lengthBytes);

/**
 * @brief Sets given memory to specified pattern
 *
 * Fills lengthBytes of pData repeating the pPattern pattern.
 * The pData buffer is assumed to have the size of at least lengthBytes.
 * The pPattern buffer is assumed to have the size of at least patternBytes.
 *
 * If lengthBytes is 0 it is guaranteed to not access pData.
 * @par Undefined:
 * Behavior is undefined if patternBytes is zero. <br>
 * Behavior is undefined if pPattern and pData overlap.
 *
 * @return pData
 */
void *portMemSetPattern(void *pData, NvLength lengthBytes, const NvU8 *pPattern, NvLength patternBytes);

/**
 * @brief Compares two memory regions.
 *
 * This function does a byte by byte comparison of the 2 memory regions provided.
 *
 * It simultaneously scans pData0 and pData1 starting from byte 0 and going
 * until lengthBytes bytes have been scanned or the bytes in pData0 and pData1
 * are not equal.
 *
 * The return value will be
 * - 0 if all lengthBytes bytes are equal.
 * - <0 if pData0 is less than pData1 for the first unequal byte.
 * - >0 if pData0 is greater than pData1 for the first unequal byte.
 *
 * Both buffers are assumed to have the size of at least lengthBytes.
 *
 * @par Undefined:
 * Behavior is undefined if memory regions referred to by pData0 and pData1
 * overlap. <br>
 * Behavior is undefined if lengthBytes is 0.
 *
 * @par Checked builds only:
 * The function will return 0 and breakpoint/assert if there is overlap. <br>
 * The function will return 0 and breakpoint/assert if the length is 0.
 */
NvS32 portMemCmp(const void *pData0, const void *pData1, NvLength lengthBytes);


typedef struct PORT_MEM_ALLOCATOR PORT_MEM_ALLOCATOR;

/**
 * @brief Function signature for PORT_MEM_ALLOCATOR::alloc.
 *
 * Basic behavior is similar to @ref portMemAllocPaged. What type of memory
 * is returned depends on the type of allocator that was created.
 *
 * Must be given the same instance of @ref PORT_MEM_ALLOCATOR as that which
 * contains the calling function pointer. A different copy returned by the
 * same function is not sufficient. Behavior is undefined if this is not done.
 */
typedef void *PortMemAllocatorAlloc(PORT_MEM_ALLOCATOR *pAlloc, NvLength length);

/**
 * @brief Function signature for PORT_MEM_ALLOCATOR::free.
 *
 * See @ref portMemFree for details.
 *
 * Must be given the same instance of @ref PORT_MEM_ALLOCATOR as that which
 * contains the calling function pointer. A different copy returned by the
 * same function is not sufficient. Behavior is undefined if this is not done.
 *
 * @par Checked builds only:
 * Will assert if given a different pointer than the one the memory
 * was allocated with.
 */
typedef void PortMemAllocatorFree(PORT_MEM_ALLOCATOR *pAlloc, void *pMemory);

/**
 * @brief Function signature for PORT_MEM_ALLOCATOR::release.
 *
 * This function is called by @ref portMemAllocatorRelease when the allocator is
 * released. This is only needed when implementing custom allocators, to be able
 * to clean up as necessary.
 */
typedef void PortMemAllocatorRelease(PORT_MEM_ALLOCATOR *pAlloc);


/**
 * @brief Platform specific allocator implementation.
 */
typedef struct PORT_MEM_ALLOCATOR_IMPL PORT_MEM_ALLOCATOR_IMPL;

/**
 * @brief Opaque structure to hold all memory tracking information.
 */
typedef struct PORT_MEM_ALLOCATOR_TRACKING PORT_MEM_ALLOCATOR_TRACKING;

/**
 * @brief Initializes an allocator tracking structures.
 *
 * You only need to call this when creating a custom allocator. The functions
 * declared in this file call this internally.
 *
 * @param pTracking - Pointer to an already allocated tracking structure.
 */
void portMemInitializeAllocatorTracking(PORT_MEM_ALLOCATOR *pAllocator, PORT_MEM_ALLOCATOR_TRACKING *pTracking);

/**
 * @brief A set of functions that can be used to manage a specific type of memory.
 *
 * The intent of the allocator paradigm is to allow for generic code to be
 * given an instance of PORT_MEM_ALLOCATOR for use to create memory so it does
 * not have to embed a policy decision in its implementation. It can also
 * allow for the implementation of specialized allocators that can be leveraged
 * through a generic interface.
 *
 * Don't call these functions directly, use @ref PORT_ALLOC and @ref PORT_FREE
 * This is done to provide full tracking support for these calls.
 */
struct PORT_MEM_ALLOCATOR {
    /**
     * @brief see @ref PortMemAllocatorAlloc for documentation
     */
    PortMemAllocatorAlloc *_portAlloc;
    /**
     * @brief see @ref PortMemAllocatorFree for documentation
     */
    PortMemAllocatorFree *_portFree;
    /**
     * @brief see @ref PortMemAllocatorRelease for documentation
     */
    PortMemAllocatorRelease *_portRelease;
    /**
     * @brief Pointer to tracking structure.
     */
    PORT_MEM_ALLOCATOR_TRACKING *pTracking;
    /**
     * @brief Pointer to the platform specific implementation.
     */
    PORT_MEM_ALLOCATOR_IMPL *pImpl;
};

/**
 * @brief Macro for calling the alloc method of an allocator object.
 *
 * Please use this instead of calling the methods directly, to ensure proper
 * memory tracking in all cases.
 *
 * @pre Windows: IRQL <= APC_LEVEL(DISPATCH_LEVEL if allocating NonPaged memory)
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
#define PORT_ALLOC(pAlloc, length) _portMemAllocatorAlloc(pAlloc, length)
/**
 * @brief Macro for calling the free method of an allocator object
 *
 * Please use this instead of calling the methods directly, to ensure proper
 * memory tracking in all cases.
 *
 * @pre Windows: IRQL <= APC_LEVEL (DISPATCH_LEVEL if freeing NonPaged memory)
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
#define PORT_FREE(pAlloc, pMem) _portMemAllocatorFree(pAlloc, pMem)

/**
 * @brief Creates an allocator for paged memory.
 *
 * Returns an allocator instance where @ref PORT_ALLOC will behave
 * like @ref portMemAllocPaged. Note the memory holding the PORT_MEM_ALLOCATOR
 * instance may also be paged.
 *
 * @return NULL if creation failed.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
NV_FORCERESULTCHECK PORT_MEM_ALLOCATOR *portMemAllocatorCreatePaged(void);

/**
 * @brief Creates an allocator for non-paged memory.
 *
 * Returns an allocator instance where @ref PORT_ALLOC will
 * behave like @ref portMemAllocNonPaged. Note the memory holding the
 * PORT_MEM_ALLOCATOR instance will also be non-paged.
 *
 * @return NULL if creation failed.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
NV_FORCERESULTCHECK PORT_MEM_ALLOCATOR *portMemAllocatorCreateNonPaged(void);

/**
 * @brief Creates an allocator over an existing block of memory.
 *
 * Adds allocator bookkeeping information to an existing memory block, so that
 * it can be used with the standard allocator interface. Some of the space of
 * the preallocated block will be consumed for bookkeeping, so not all of the
 * memory will be allocatable.
 *
 * Use this to create an allocator object on an ISR stack, so memory allocations
 * can be done at DIQRL.
 *
 * @par Implementation details:
 * The allocator allocates in chunks of 16 bytes, and uses a 2bit-vector to keep
 * track of free chunks. Thus, the bookkeeping structures for a block of size N
 * will take about N/64+sizeof(PORT_MEM_ALLOCATOR) bytes.
 * Use @ref PORT_MEM_PREALLOCATED_BLOCK if you want to specify useful(allocable)
 * size instead of total size.
 *
 * The allocator is only valid while the memory it was created on is valid.
 * @ref portMemAllocatorRelease must be called on the allocator before the
 * memory lifecycle ends.
 *
 * @return NULL if creation failed.
 *
 * @pre Usable at any IRQL/interrupt context
 * @note Will not put the thread to sleep.
 * @note This allocator is not thread safe.
 */
NV_FORCERESULTCHECK PORT_MEM_ALLOCATOR *portMemAllocatorCreateOnExistingBlock(void *pPreallocatedBlock, NvLength blockSizeBytes);

/**
 * @brief Extends the given size to fit the required bookkeeping information
 *
 * To be used when preallocating blocks that will be used to create an allocator
 * Consider these two preallocated memory blocks:
 * ~~~{.c}
 *  NvU8 xxx[1024];
 *  NvU8 yyy[PORT_MEM_PREALLOCATED_BLOCK(1024)];
 * ~~~
 * Block @c xxx has a size of 1024, but only ~950 of that can be allocated.
 * Block @c yyy has a size of ~1100, and exactly 1024 bytes can be allocated.
 */
#define PORT_MEM_PREALLOCATED_BLOCK(size)                       \
    (size + PORT_MEM_PREALLOCATED_BLOCK_EXTRA_SIZE(size))

/**
 * @brief releases an allocator instance.
 *
 * This must be called to release any resources associated with the allocator.
 *
 * @par Checked builds only:
 * Will assert if pAllocator has unfreed allocations
 *
 * @par Undefined:
 * pAllocator must be an instance of PORT_MEM_ALLOCATOR that was provided by one
 * of the portMemAllocatorCreate* functions.
 *
 * These limitations don't apply to allocators created using @ref portMemAllocatorCreateOnExistingBlock and
 * @ref portMemExAllocatorCreateLockedOnExistingBlock.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
void portMemAllocatorRelease(PORT_MEM_ALLOCATOR *pAllocator);

/**
 * @brief Returns the pointer to the global nonpaged allocator.
 *
 * This allocator is always initialized does not need to be released.
 *
 * Allocations performed using this allocator are identical to the ones done
 * by @ref portMemAllocNonPaged
 */
PORT_MEM_ALLOCATOR *portMemAllocatorGetGlobalNonPaged(void);
/**
 * @brief Returns the pointer to the global paged allocator.
 *
 * This allocator is always initialized does not need to be released.
 *
 * Allocations performed using this allocator are identical to the ones done
 * by @ref portMemAllocPaged
 */
PORT_MEM_ALLOCATOR *portMemAllocatorGetGlobalPaged(void);
/**
 * @brief Prints the memory details gathered by whatever tracking mechanism is
 * enabled. If pTracking is NULL, aggregate tracking information from all
 * allocators will be printed.
 *
 * @note Printing is done using portDbgPrintf, which prints regardless of
 * build type and debug levels.
 */
void portMemPrintTrackingInfo(const PORT_MEM_ALLOCATOR_TRACKING *pTracking);
/**
 * @brief Calls @ref portMemPrintTrackingInfo for all current allocator trackers.
 */
void portMemPrintAllTrackingInfo(void);

// @} End core functions


/**
 * @name Extended Functions
 * @{
 */

/**
 * @brief Returns true if it is safe to allocate paged memory.
 */
NvBool portMemExSafeForPagedAlloc(void);
#define portMemExSafeForPagedAlloc_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Returns true if it is safe to allocate non-paged memory.
 */
NvBool portMemExSafeForNonPagedAlloc(void);
#define portMemExSafeForNonPagedAlloc_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Public allocator tracking information
 */
typedef struct PORT_MEM_TRACK_ALLOCATOR_STATS
{
    /** @brief Total number of allocations */
    NvU32 numAllocations;
    /** @brief Total allocated bytes, including all staging */
    NvLength allocatedSize;
    /** @brief Useful size of allocations - What was actually requested */
    NvLength usefulSize;
    /** @brief Extra size allocated for tracking/debugging purposes */
    NvLength metaSize;
} PORT_MEM_TRACK_ALLOCATOR_STATS;

/**
 * @brief Returns the statistics of currently active allocations for the given
 * allocator.
 *
 * If pAllocator is NULL, it returns stats for all allocators, as well as the
 * memory allocated with @ref portMemAllocPaged and @ref portMemAllocNonPaged
 */
NV_STATUS portMemExTrackingGetActiveStats(const PORT_MEM_ALLOCATOR *pAllocator, PORT_MEM_TRACK_ALLOCATOR_STATS *pStats);

/**
 * @brief Returns the statistics of currently active allocations made with the
 * given gfid.
 *
 * If the corresponding pTracking is not found, it returns
 * NV_ERR_OBJECT_NOT_FOUND
 */
NV_STATUS portMemExTrackingGetGfidActiveStats(
    NvU32 gfid,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
);

/**
 * @brief Returns the statistics of all allocations made with the given
 * allocator since it was created.
 *
 * If pAllocator is NULL, it returns stats for all allocators, as well as the
 * memory allocated with @ref portMemAllocPaged and @ref portMemAllocNonPaged
 */
NV_STATUS portMemExTrackingGetTotalStats(const PORT_MEM_ALLOCATOR *pAllocator, PORT_MEM_TRACK_ALLOCATOR_STATS *pStats);

/**
 * @brief Returns the statistics of all allocations made with the given
 * gfid.
 *
 * If the corresponding pTracking is not found, it returns
 * NV_ERR_OBJECT_NOT_FOUND
 */
NV_STATUS portMemExTrackingGetGfidTotalStats(
    NvU32 gfid,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
);

/**
 * @brief Returns the statistics of peak allocations made with the given
 * allocator since it was created.
 *
 * Peak data reports the high-water mark based on the maximum size (the peak
 * allocations doesn't report the largest number of allocations, it reports
 * the number of allocations at the time the peak size was achieved). This is
 * done so that the other peak stats, which are derived from peak size and
 * peak allocations, are consistent with each other.
 *
 * If pAllocator is NULL, it returns stats for all allocators, as well as the
 * memory allocated with @ref portMemAllocPaged and @ref portMemAllocNonPaged
 */
NV_STATUS portMemExTrackingGetPeakStats(const PORT_MEM_ALLOCATOR *pAllocator, PORT_MEM_TRACK_ALLOCATOR_STATS *pStats);

/**
 * @brief Returns the statistics of peak allocations made with the given
 * gfid since it was created.
 *
 * Peak data reports the high-water mark based on the maximum size (the peak
 * allocations doesn't report the largest number of allocations, it reports
 * the number of allocations at the time the peak size was achieved). This is
 * done so that the other peak stats, which are derived from peak size and
 * peak allocations, are consistent with each other.
 *
 * If the corresponding pTracking is not found, it returns
 * NV_ERR_OBJECT_NOT_FOUND
 */
NV_STATUS portMemExTrackingGetGfidPeakStats(
    NvU32 gfid,
    PORT_MEM_TRACK_ALLOCATOR_STATS *pStats
);

/**
 * @brief Cycles through the tracking infos for allocations by pAllocator
 * If pAllocator is NULL, it will cycle through all allocations.
 *
 * @param [out]     pInfo      The info will be written to this buffer.
 * @param [in, out] pIterator
 *    Should point to NULL the first time it is called.
 *    Every next call should pass the value returned by previous.
 *    To reset the loop, set the iterator to NULL.
 *    Upon writing the last range, the iterator will be set to NULL.
 *    The iterator is only valid until the next alloc/free from this allocator.
 *    There is no need to release the iterator in any way.
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if no allocations exist.
 */
NV_STATUS portMemExTrackingGetNext(const PORT_MEM_ALLOCATOR *pAllocator, PORT_MEM_TRACK_ALLOC_INFO *pInfo, void **pIterator);

/**
 * @brief Gets the total size of the underlying heap, in bytes.
 */
NvLength portMemExTrackingGetHeapSize(void);

/**
 * @brief Gets the usable size in bytes (sans metadata/padding) of the given allocation.
 */
NvLength portMemExTrackingGetAllocUsableSize(void *pMem);

/**
 * @brief Copies from user memory to kernel memory.
 *
 * When accepting data as input from user space it is necessary to take
 * additional precautions to access it safely and securely. This means copy
 * the user data into a kernel buffer and then using that kernel buffer for all
 * needed accesses.
 *
 * The function will fail if pUser is an invalid user space pointer or if the
 * memory it refers to is less than length bytes long. A valid kernel pointer
 * is interpreted as an invalid user pointer.
 * @par Checked builds only:
 * Will trigger a breakpoint if pUser is invalid userspace pointer
 *
 * The function will fail if pKernel is NULL.
 *
 * The function will fail if lengthBytes is 0.
 *
 * @return
 *   - NV_OK if successful
 *   - NV_ERR_INVALID_POINTER if pUser is invalid or pKernel is NULL
 *   - NV_ERR_INVALID_ARGUMENT if lengthBytes is 0
 */
NV_STATUS portMemExCopyFromUser(const NvP64 pUser, void *pKernel, NvLength lengthBytes);
#define portMemExCopyFromUser_SUPPORTED PORT_IS_KERNEL_BUILD


/**
 * @brief Copies from kernel memory to user memory.
 *
 * This is the reverse of @ref portMemExCopyFromUser. The copy in this case is
 * from pKernel to pUser.
 *
 * See @ref portMemExCopyFromUser for more details.
 *
 */
NV_STATUS portMemExCopyToUser(const void *pKernel, NvP64 pUser, NvLength lengthBytes);
#define portMemExCopyToUser_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Returns the size (in bytes) of a single memory page.
 */
NvLength portMemExGetPageSize(void);
#define portMemExGetPageSize_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Opaque container holding an allocation of physical system memory.
 */
typedef struct PORT_PHYSICAL_MEMDESC PORT_PHYSICAL_MEMDESC;

/**
 * @brief Creates a handle used to manage and manipulate a physical memory
 * allocation.
 *
 * @param pAllocator the allocator to use the create the allocation's tracking
 * structures. This allocator is *not* used to allocate physical memory.
 *
 * @return NULL if the allocation failed.
 */
PORT_PHYSICAL_MEMDESC *portMemExPhysicalDescCreate(PORT_MEM_ALLOCATOR *pAllocator);
#define portMemExPhysicalDescCreate_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Types of caching for physical memory mappings.
 *
 * In case a target architecture does not support a specific caching mode,
 * the mapping call will fail.
 * Specifying PORT_MEM_ANYCACHE lets the implementation pick a caching mode that
 * is present on the target architecture. This way the mapping will not fail.
 */
typedef enum
{
    PORT_MEM_UNCACHED,
    PORT_MEM_CACHED,
    PORT_MEM_WRITECOMBINED,
    PORT_MEM_ANYCACHE
} PortMemCacheMode;

/**
 * @brief Types of access protections for physical memory mappings.
 */
typedef enum
{
    PORT_MEM_PROT_NO_ACCESS       = 0,
    PORT_MEM_PROT_READ            = 1,
    PORT_MEM_PROT_WRITE           = 2,
    PORT_MEM_PROT_READ_WRITE      = 3,
    PORT_MEM_PROT_EXEC            = 4,
    PORT_MEM_PROT_READ_EXEC       = 5,
    PORT_MEM_PROT_WRITE_EXEC      = 6,
    PORT_MEM_PROT_READ_WRITE_EXEC = 7
} PortMemProtectMode;

/**
 * @brief Populates a physical memory descriptor with backing pages.
 *
 * Populates a descriptor with physical pages. Pages will be zeroed.
 */
NV_STATUS portMemExPhysicalDescPopulate(PORT_PHYSICAL_MEMDESC *pPmd, NvLength sizeBytes, NvBool bContiguous);
#define portMemExPhysicalDescPopulate_SUPPORTED PORT_IS_KERNEL_BUILD


/**
 * @brief allocates a PMD and populates it with memory
 *
 * This is a combination of @ref portMemExPhysicalDescCreate and @ref
 * portMemExPhysicalDescPopulate. It should be the preferred method to allocate
 * physical memory when it is possible to do it as a single step. Not only
 * does the caller require less code and error handling but it allows the
 * implementation the option to combine the tracking data into fewer
 * allocations since it knows the size up front.
 *
 * @param [out] ppPmd      - Pointer to the allocated PMD.
 * @param       pAllocator - Allocator to use when allocating the PMD
 */
NV_STATUS portMemExPhysicalDescCreateAndPopulate(PORT_MEM_ALLOCATOR *pAllocator,
        PORT_PHYSICAL_MEMDESC **ppPmd, NvLength sizeBytes, NvBool bContiguous);
#define portMemExPhysicalDescCreateAndPopulate_SUPPORTED PORT_IS_KERNEL_BUILD


/**
 * @brief Adds a contiguous memory range to the physical memory descriptor
 *
 * To describe a non-contiguous memory range, call this function once for every
 * contiguous range. Range order will be determined by function call order,
 * not the range addresses.
 */
NV_STATUS portMemExPhysicalDescribeRange(PORT_PHYSICAL_MEMDESC *pPmd, NvU64 start, NvLength length);
#define portMemExPhysicalDescribeRange_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Hands back the next contiguous memory range in the memory descriptor
 *
 * @param [out]     pStart    - Physical address of the range
 * @param [out]     pLength   - Length of the range
 * @param [in, out] pIterator
 *    Should point to NULL the first time it is called.
 *    Every next call should pass the value returned by previous.
 *    To reset the loop, set the iterator to NULL.
 *    Upon writing the last range, the iterator will be set to NULL.
 *    The iterator is valid until pPmd is destroyed.
 *    There is no need to release the iterator in any way.
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if no ranges exist.
 */
NV_STATUS portMemExPhysicalGetNextRange(PORT_PHYSICAL_MEMDESC *pPmd,
        NvU64 *pStart, NvLength *pLength, void **pIterator);
#define portMemExPhysicalGetNextRange_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Frees the memory descriptor and all tracking data. The descriptor must
 * have been allocated with @ref portMemExPhysicalDescCreate or
 * @ref portMemExPhysicalDescCreateAndPopulate
 *
 * Freed memory is not automatically unmapped.
 *
 * It is guaranteed that after memory has been freed, the original data can no
 * longer be read in any way.
 * @par Undefined:
 * Accessing a mapping that has been freed results in undefined behavior.
 */
void portMemExPhysicalDescFree(PORT_PHYSICAL_MEMDESC *pPmd);
#define portMemExPhysicalDescFree_SUPPORTED PORT_IS_KERNEL_BUILD


/**
 * @brief Frees physical memory allocated with @ref portMemExPhysicalDescPopulate
 */
void portMemExPhysicalFree(PORT_PHYSICAL_MEMDESC *pPmd);
#define portMemExPhysicalFree_SUPPORTED PORT_IS_KERNEL_BUILD


/**
 * @brief Maps a region of a @ref PORT_PHYSICAL_MEMDESC
 *
 * @param [out] ppMapping - Virtual address where the physical memory is mapped
 * @param offset    - Offset of the physical memory where the region starts.
 *                    The region must start on a page boundary.
 * @param length    - Length of the physical memory region.
 *                    Needs to be a multiple of page size.
 * @param protect   - Mapping protections
 * @param cacheMode - Mapping cache mode.
 *                    Only PORT_MEM_ANYCACHE is guaranteed to be supported.
 *
 * @return NV_ERR_NOT_SUPPORTED if the specified cache mode is not supported by
 *             the current architecture.
 */
NV_STATUS portMemExPhysicalMap(PORT_PHYSICAL_MEMDESC *pPmd,
                        void **ppMapping, NvU64 offset, NvU64 length,
                        PortMemProtectMode protect, PortMemCacheMode cacheMode);
#define portMemExPhysicalMap_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Unmaps a region created with @ref portMemExPhysicalMap.
 *
 * @par Undefined:
 * Accessing an unmapped memory is undefined, but it is guaranteed that the
 * actual data can't be read/overwritten.
 */
NV_STATUS portMemExPhysicalUnmap(PORT_PHYSICAL_MEMDESC *pPmd, void *pMapping);
#define portMemExPhysicalUnmap_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Creates a thread safe allocator over an existing block of memory.
 *
 * @note See @ref portMemAllocatorCreateOnExistingBlock for other limitations.
 * @note User should initialize @p pSpinlock and destroy it after it
 * has finished using this allocator.
 */
PORT_MEM_ALLOCATOR *portMemExAllocatorCreateLockedOnExistingBlock(void *pPreallocatedBlock, NvLength blockSizeBytes, void *pSpinlock);
#define portMemExAllocatorCreateLockedOnExistingBlock_SUPPORTED \
                            (PORT_IS_MODULE_SUPPORTED(sync))


/**
 * @brief Maps the given physical address range to nonpaged system space.
 *
 * @param[in] start     Specifies the starting physical address of the I/O
 *                      range to be mapped.
 * @param[in] byteSize  Specifies the number of bytes to be mapped.
 *
 * @return    The base virtual address that maps the base physical address for
 *            the range
 */
void *portMemExMapIOSpace(NvU64 start, NvU64 byteSize);
#define portMemExMapIOSpace_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS)

/**
 * @brief Unmaps a specified range of physical addresses previously mapped by
 *        portMapIOSpace
 *
 * @param[in] addr      Pointer to the base virtual address to which the
 *                      physical pages were mapped.
 * @param[in] byteSize  Specifies the number of bytes that were mapped.
 */
void portMemExUnmapIOSpace(void *addr, NvU64 byteSize);
#define portMemExUnmapIOSpace_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS)

// @} End extended functions


/**
 * @note Memory tracking is controlled through the following compile-time flags.
 * The PORT_MEM_TRACK_USE_* constants should be defined to 0 or 1.
 * If nothing is defined, the default values are assigned here.
 */
#if !defined(PORT_MEM_TRACK_USE_COUNTER)
/**
 * @brief Use allocations counter for all allocators
 *
 * Allocation counter is lightweight and can detect if a leak is present.
 * Default is always on.
 */
#define PORT_MEM_TRACK_USE_COUNTER 1
#endif
#if !defined(PORT_MEM_TRACK_USE_FENCEPOSTS)
/**
 * @brief Use fenceposts around all allocated blocks
 *
 * Fenceposts can detect out of bounds writes and improper free calls
 * Default is on for checked builds (where it will assert if an error occurs)
 */
#define PORT_MEM_TRACK_USE_FENCEPOSTS PORT_IS_CHECKED_BUILD
#endif
#if !defined(PORT_MEM_TRACK_USE_ALLOCLIST)
/**
 * @brief Keep a list of all allocations.
 *
 * Allocation lists can give more details about detected leaks, and allow
 * cycling through all allocations.
 * Default is off.
 * @todo Perhaps enable for checked builds?
 */
#define PORT_MEM_TRACK_USE_ALLOCLIST 0
#endif
#if !defined(PORT_MEM_TRACK_USE_CALLERINFO)
/**
 * @brief Track file:line information for all allocations
 *
 * On release builds the filename hash is passed instead of the string. This
 * requires NvLog to be enabled.
 * Default is off.
 */
#define PORT_MEM_TRACK_USE_CALLERINFO 0
#endif
/**
 * @brief Track instruction pointer instead of function/file/line information
 *        for all allocations
 *
 * Has no effect unless PORT_MEM_TRACK_USE_CALLERINFO is also set.
 */
#if !defined(PORT_MEM_TRACK_USE_CALLERINFO_IP)
#if NVOS_IS_LIBOS
#define PORT_MEM_TRACK_USE_CALLERINFO_IP 1
#else
#define PORT_MEM_TRACK_USE_CALLERINFO_IP 0
#endif
#endif
#if !defined(PORT_MEM_TRACK_USE_LOGGING)
/**
 * @brief Log all alloc and free calls to a binary NvLog buffer
 * Requires NvLog to be enabled.
 *
 * Default is off.
 */
#define PORT_MEM_TRACK_USE_LOGGING 0
#endif
#if !defined(PORT_MEM_TRACK_USE_LIMIT)
/**
 * @brief Track and enforce a heap memory usage limit on processes
 *        running in GSP-RM.
 *
 * Default is on in GSP-RM only.
 */
#ifndef GSP_PLUGIN_BUILD
#define PORT_MEM_TRACK_USE_LIMIT (NVOS_IS_LIBOS)
#else
#define PORT_MEM_TRACK_USE_LIMIT 0
#endif
#endif // !defined(PORT_MEM_TRACK_USE_LIMIT)

// Memory tracking header can redefine some functions declared here.
#include "nvport/inline/memory_tracking.h"

/** @brief Nothing is printed unless @ref portMemPrintTrackingInfo is called */
#define PORT_MEM_TRACK_PRINT_LEVEL_SILENT  0
/** @brief Print when an error occurs and at shutdown */
#define PORT_MEM_TRACK_PRINT_LEVEL_BASIC   1
/** @brief Print at every alloc and free, and at any abnormal situation */
#define PORT_MEM_TRACK_PRINT_LEVEL_VERBOSE 2

#if !defined(PORT_MEM_TRACK_PRINT_LEVEL)
#if PORT_IS_CHECKED_BUILD || PORT_MEM_TRACK_ALLOC_SIZE
#define PORT_MEM_TRACK_PRINT_LEVEL PORT_MEM_TRACK_PRINT_LEVEL_BASIC
#else
#define PORT_MEM_TRACK_PRINT_LEVEL PORT_MEM_TRACK_PRINT_LEVEL_SILENT
#endif // PORT_IS_CHECKED_BUILD
#endif // !defined(PORT_MEM_TRACK_PRINT_LEVEL)

/**
 * @brief Single allocation description.
 *
 * Must be defined after memory_tracking.h is included for PORT_MEM_CALLERINFO.
 */
struct PORT_MEM_TRACK_ALLOC_INFO
{
#if PORT_MEM_TRACK_USE_CALLERINFO
    /**
     * @brief Function / file / line or instruction pointer.
     */
    PORT_MEM_CALLERINFO callerInfo;
#endif
    /**
     * @brief pointer to the allocated memory block.
     */
    void *pMemory;
    /**
     * @brief Size of the allocated memory block
     */
    NvLength size;
    /**
     * @brief Pointer to the allocator that allocated the memory.
     * If the memory was allocated globally, this will be NULL
     */
    PORT_MEM_ALLOCATOR *pAllocator;
    /**
     * @brief Timestamp of the allocation. Will be 0 if it wasn't logged.
     */
    NvU64 timestamp;
};

/**
 * @}
 */

#endif // _NVPORT_MEMORY_H_
