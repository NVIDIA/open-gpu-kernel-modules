/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 *  @brief This file exposes the PMA interfaces.
 *  The PMA module interacts with the RM and UVM components. UVM will use the
 *  ops layer to call out to PMA, while RM directly calls into PMA.
 *  The PMA module takes a global lock to protect its internal structure. It
 *  uses a bitmap structure called regmap.
 *
 *  @bug
 *  1. status code -- decide if we need to add to global
 *  2. suspend/resume -- might add one more function to support
 *
 *  @TODO
 *  1. external fragmentation
 *  2. use new scrubber API and remove the initScrubbing atomic variable
 */

#ifndef PHYS_MEM_ALLOCATOR_H
#define PHYS_MEM_ALLOCATOR_H

#include "nvport/nvport.h"
#include "regmap.h"
#include "nvmisc.h"

#if defined(SRT_BUILD)
#define RMCFG_MODULE_x 1
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OBJMEMSCRUB OBJMEMSCRUB;
typedef struct SCRUB_NODE SCRUB_NODE;

#define PMA_REGION_SIZE 32
#define PMA_ADDR2FRAME(addr, base)  (((addr) - (base)) >> PMA_PAGE_SHIFT)
#define PMA_FRAME2ADDR(frame, base) ((base) + ((frame) << PMA_PAGE_SHIFT))

//
// These flags are used for initialization in order to set global PMA states,
// in case we need to wait for scrubber to be initialized or wait for a NUMA
// node being onlined, etc.
//
#define PMA_INIT_NONE                   NVBIT(0)
#define PMA_INIT_SCRUB_ON_FREE          NVBIT(1)
#define PMA_INIT_NUMA                   NVBIT(2)
#define PMA_INIT_INTERNAL               NVBIT(3) // Used after heap is removed
#define PMA_INIT_FORCE_PERSISTENCE      NVBIT(4)
// unused
#define PMA_INIT_NUMA_AUTO_ONLINE       NVBIT(6)

// These flags are used for querying PMA's config and/or state.
#define PMA_QUERY_SCRUB_ENABLED         NVBIT(0)
#define PMA_QUERY_SCRUB_VALID           NVBIT(1)
#define PMA_QUERY_NUMA_ENABLED          NVBIT(2)
#define PMA_QUERY_NUMA_ONLINED          NVBIT(3)

//
// When modifying flags, make sure they are compatible with the mirrored
// UVM_PMA_* flags in nv_uvm_types.h.
//
// Input flags
#define PMA_ALLOCATE_DONT_EVICT             NVBIT(0)
#define PMA_ALLOCATE_PINNED                 NVBIT(1)
#define PMA_ALLOCATE_SPECIFY_MINIMUM_SPEED  NVBIT(2)
#define PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE  NVBIT(3)
#define PMA_ALLOCATE_SPECIFY_REGION_ID      NVBIT(4)
#define PMA_ALLOCATE_PREFER_SLOWEST         NVBIT(5)
#define PMA_ALLOCATE_CONTIGUOUS             NVBIT(6)
#define PMA_ALLOCATE_PERSISTENT             NVBIT(7)
#define PMA_ALLOCATE_PROTECTED_REGION       NVBIT(8)
#define PMA_ALLOCATE_FORCE_ALIGNMENT        NVBIT(9)
#define PMA_ALLOCATE_NO_ZERO                NVBIT(10)
#define PMA_ALLOCATE_TURN_BLACKLIST_OFF     NVBIT(11)
#define PMA_ALLOCATE_ALLOW_PARTIAL          NVBIT(12)
#define PMA_ALLOCATE_REVERSE_ALLOC          NVBIT(13)
#define PMA_ALLOCATE_LOCALIZED_UGPU0        NVBIT(14)
#define PMA_ALLOCATE_LOCALIZED_UGPU1        NVBIT(15)

// Output flags
#define PMA_ALLOCATE_RESULT_IS_ZERO         NVBIT(0)

// These are flags input to the pmaFreePages call
#define PMA_FREE_SKIP_SCRUB           NVBIT(0)

// State bits for debugging utilities like nvwatch
#define PMA_SCRUB_INITIALIZE   0
#define PMA_SCRUB_IN_PROGRESS  1
#define PMA_SCRUB_DONE         2

#define PMA_SCRUBBER_VALID     1
#define PMA_SCRUBBER_INVALID   0

#define PMA_NUMA_NO_NODE      -1

// Maximum blacklist entries possible
#define PMA_MAX_BLACKLIST_ENTRIES 512

typedef struct
{
    NvU32 flags;
    NvU32 minimumSpeed;         // valid if flags & PMA_ALLOCATE_SPECIFY_MININUM_SPEED
    NvU64 physBegin, physEnd;   // valid if flags & PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE
    NvU32 regionId;             // valid if flags & PMA_ALLOCATE_SPECIFY_REGION_ID
    NvU64 alignment;            // valid if flags & PMA_ALLOCATE_FORCE_ALIGNMENT
    NvLength numPagesAllocated; // valid if flags & PMA_ALLOCATE_ALLOW_PARTIAL

    NvU32 resultFlags;          // valid if the allocation function returns NV_OK
} PMA_ALLOCATION_OPTIONS;

//
// Explanation: This struct will be provided when UVM/RM registers a region with PMA,
// after which the struct is stored locally in PMA. The internal "filter" function will
// use the information everytime a request comes in.
//
typedef struct
{
    NvU64   base;               // Base/start address of the region
    NvU64   limit;              // Last/end address of region
    NvU32   performance;        // Relative performance.  Higher is faster
    NvBool  bSupportCompressed; // Support compressed kinds
    NvBool  bSupportISO;        // Support ISO (display, cursor, video) surfaces
    NvBool  bProtected;         // Represents a protected region of memory.
} PMA_REGION_DESCRIPTOR;

typedef struct _PMA_MAP_INFO PMA_MAP_INFO;
typedef struct _PMA PMA;

// Range descriptors for managing persistent range lists
typedef struct _RANGELISTTYPE
{
    NvU64           base;
    NvU64           limit;
    struct _RANGELISTTYPE  *pNext;
} RANGELISTTYPE, *PRANGELISTTYPE;

typedef enum
{
    MEMORY_PROTECTION_UNPROTECTED = 0,
    MEMORY_PROTECTION_PROTECTED   = 1
} MEMORY_PROTECTION;

/*!
 * @brief Callback to update stats in RUSD
 */
typedef void (*pmaUpdateStatsCb_t)(void *pCtx, NvU64 freeFrames);

/*!
 * @brief Callbacks to UVM for eviction
 */
typedef NV_STATUS (*pmaEvictPagesCb_t)(void *ctxPtr, NvU64 pageSize, NvU64 *pPages,
                                       NvU32 count, NvU64 physBegin, NvU64 physEnd,
                                       MEMORY_PROTECTION prot);
typedef NV_STATUS (*pmaEvictRangeCb_t)(void *ctxPtr, NvU64 physBegin, NvU64 physEnd,
                                       MEMORY_PROTECTION prot);

/*!
 * @brief Pluggable data structure management. Currently we have regmap.
 */
typedef void *(*pmaMapInit_t)(NvU64 numFrames, NvU64 addrBase, PMA_STATS *pPmaStats, NvBool bProtected);
typedef void  (*pmaMapDestroy_t)(void *pMap);
typedef void  (*pmaMapChangeStateAttrib_t)(void *pMap, NvU64 frameNum, PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);
typedef void  (*pmaMapChangePageStateAttrib_t)(void *pMap, NvU64 startFrame, NvU64 pageSize, PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);
typedef void  (*pmaMapChangeBlockStateAttrib_t)(void *pMap, NvU64 frameNum, NvU64 numFrames, PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);
typedef PMA_PAGESTATUS (*pmaMapRead_t)(void *pMap, NvU64 frameNum, NvBool readAttrib);
typedef NV_STATUS (*pmaMapScanContiguous_t)(void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
                                            NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment,
                                            NvU64 stride, NvU32 strideStart,
                                            NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);
typedef NV_STATUS (*pmaMapScanDiscontiguous_t)(void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
                                               NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment,
                                               NvU64 stride, NvU32 strideStart,
                                               NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);
typedef void (*pmaMapGetSize_t)(void *pMap, NvU64 *pBytesTotal);
typedef void (*pmaMapGetLargestFree_t)(void *pMap, NvU64 *pLargestFree);
typedef NV_STATUS (*pmaMapScanContiguousNumaEviction_t)(void *pMap, NvU64 addrBase, NvLength actualSize,
                                                        NvU64 pageSize, NvU64 *evictStart, NvU64 *evictEnd);
typedef NvU64 (*pmaMapGetEvictingFrames_t)(void *pMap);
typedef void (*pmaMapSetEvictingFrames_t)(void *pMap, NvU64 frameEvictionsInProcess);

struct _PMA_MAP_INFO
{
    NvU32                       mode;
    pmaMapInit_t                pmaMapInit;
    pmaMapDestroy_t             pmaMapDestroy;
    pmaMapChangeStateAttrib_t      pmaMapChangeStateAttrib;
    pmaMapChangePageStateAttrib_t  pmaMapChangePageStateAttrib;
    pmaMapChangeBlockStateAttrib_t pmaMapChangeBlockStateAttrib;
    pmaMapRead_t                pmaMapRead;
    pmaMapScanContiguous_t      pmaMapScanContiguous;
    pmaMapScanDiscontiguous_t   pmaMapScanDiscontiguous;
    pmaMapGetSize_t             pmaMapGetSize;
    pmaMapGetLargestFree_t      pmaMapGetLargestFree;
    pmaMapScanContiguousNumaEviction_t pmaMapScanContiguousNumaEviction;
    pmaMapGetEvictingFrames_t  pmaMapGetEvictingFrames;
    pmaMapSetEvictingFrames_t  pmaMapSetEvictingFrames;
};

struct _PMA
{
    PORT_SPINLOCK           *pPmaLock;                          // PMA-wide lock
    PORT_MUTEX              *pEvictionCallbacksLock;            // Eviction callback registration lock

    // Only used when free scrub-on-free feature is turned on
    PORT_RWLOCK             *pScrubberValidLock;                // A reader-writer lock to protect the scrubber valid bit
    PORT_MUTEX              *pAllocLock;                        // Used to protect page stealing in the allocation path

    // Region related states
    NvU32                   regSize;                            // Actual size of regions array
    void *                  pRegions[PMA_REGION_SIZE];          // All the region maps stored as opaque pointers
    NvU32                   *pSortedFastFirst;                  // Pre-sorted array of region IDs
    PMA_REGION_DESCRIPTOR   *pRegDescriptors [PMA_REGION_SIZE]; // Stores the descriptions of each region
    PMA_MAP_INFO            *pMapInfo;                          // The pluggable layer for managing scanning

    // Allocation related states
    void *                  evictCtxPtr;                        // Opaque context pointer for eviction callback
    pmaEvictPagesCb_t       evictPagesCb;                       // Discontiguous eviction callback
    pmaEvictRangeCb_t       evictRangeCb;                       // Contiguous eviction callback
    NvU64                   frameAllocDemand;                   // Frame count of allocations in-process
    NvBool                  bForcePersistence;                  // Force all allocations to persist across suspend/resume
    PMA_STATS               pmaStats;                           // PMA statistics used for client heuristics

    // Scrubber related states
    NvSPtr                  initScrubbing;                      // If the init scrubber has finished in this PMA
    NvBool                  bScrubOnFree;                       // If "scrub on free" is enabled for this PMA object
    NvSPtr                  scrubberValid;                      // If scrubber object is valid, using atomic variable to prevent races
    OBJMEMSCRUB            *pScrubObj;                          // Object to store the FreeScrub header

    // NUMA states
    NvBool                  bNuma;                              // If we are allocating for a NUMA system
    NvBool                  nodeOnlined;                        // If node is onlined
    NvS32                   numaNodeId;                         // Current Node ID, set at initialization. -1 means invalid
    NvU64                   coherentCpuFbBase;                  // Used to calculate FB offset from bus address
    NvU64                   coherentCpuFbSize;                  // Used for error checking only
    NvU32                   numaReclaimSkipThreshold;           // percent value below which __GFP_RECLAIM will not be used.
    NvBool                  bNumaAutoOnline;                    // If NUMA memory is auto-onlined

    // Blacklist related states
    PMA_BLACKLIST_CHUNK    *pBlacklistChunks;                   // Tracking for blacklist pages
    NvU32                   blacklistCount;                     // Number of blacklist pages
    NvBool                  bClientManagedBlacklist;            // Blacklisted pages in PMA that will be taken over by Client

    // RUSD Callback
    pmaUpdateStatsCb_t      pStatsUpdateCb;                     // RUSD update free pages
    void                   *pStatsUpdateCtx;                    // Context for RUSD update
};

/*!
 * @brief This must be called before any other PMA functions. Returns a PMA
 * object for later use
 *
 * @param[in] gpuId         The UVM global GPU ID. Defined in nvCpuUuid.h as a 16
 *                          byte digest. PMA will only store reference to it.
 * @param[in] pma           Pointer to PMA object being initialized
 * @param[in] initFlags     PMA initialization flags for special modes
 * @return
 *      NV_ERR_INVALID_ARGUMENT:
 *          If the combination of initFlags is invalid.
 *      NV_ERR_NO_MEMORY:
 *          Internal memory allocation failed.
 *      NV_ERR_GENERIC:
 *          Unexpected error. We try hard to avoid returning this error
 *          code, because it is not very informative.
 *
 */
NV_STATUS pmaInitialize(PMA *pPma, NvU32 initFlags);


/*!
 * @brief Release a PMA object. Also frees memory.
 *
 * All eviction handlers must have been unregistered by this point.
 *
 * @param[in] pPma      Pointer to PMA object being destroyed.
 */
void  pmaDestroy(PMA *pPma);


/*!
 * @brief Queries PMA configuration and state.
 *
 * Any clients of PMA can query config and state with a valid PMA object.
 * Querying at different times may return different values when states change.
 *
 * @param[in]     pPma      Pointer to PMA object
 * @param[in/out] pConfigs  Configs/states to query. See PMA_QUERY_* above.
 */
NV_STATUS pmaQueryConfigs(PMA* pPma, NvU32 *pConfigs);

/*!
 * @brief Identify if an FB range is PMA-managed
 *
 *
 * @param[in] pPma    Pointer to PMA object
 * @param[in] offset  FB block offset
 * @param[in] limit   FB block limit
 *
 * @return NV_TRUE      offset is PMA-managed
 *         NV_FALSE     offset is not managed by PMA
*/
NvBool pmaIsPmaManaged(PMA* pPma, NvU64 offset, NvU64 limit);

/*!
 * @brief Attaches a region of physical memory to be managed by the PMA.
 *
 * Systems with floorswept memory configurations will have (multi gigabyte)
 * holes in memory. Each contiguous region should be reported by this
 * registration function. This is also intended to be used by systems with
 * two speed memories.
 *
 * Note: Some 0FB configurations may choose to skip registering any regions.
 *       At most 64 regions may be registered with a single PMA.
 *
 * @param[in] id
 *      A unique value in the range [0, 64) that uniquely identifies this
 *      region. Passed in by RM in region-order. Should be continuous for
 *      best performance.
 *
 * @param[in] bAsyncEccScrub
 *      RM will set this when it is performing the initial ECC scrub
 *      asynchronously.  All pages in the pma will be marked 'allocated pinned'.
 *      RM will call pmaFreeContiguous as memory is scrubbed.
 *
 *      Until the scrub is complete (by calling pmaScrubComplete), no
 *      allocation calls will fail with out of memory. Instead, it will hang
 *      until scrubbing is complete. One exception is when the client passes
 *      in the PMA_DONT_EVICT flag, in which case the call will actually fail
 *      regardless of whether RM is scrubbing memory.
 *
 *      CAUTION! RM is responsible for ensuring black-listed pages are not
 *      marked free during the scrub.
 *
 * @param[in] regionState:
 *      Contains physical information about the region.
 *
 * @param[in] pBlacklistPageBase:
 *      List of base addresses of bad GPU pages.
 *      Each address is assumed to reference to a page of size
 *      PMA_GRANULARITY (64kb).
 *
 * Implementors note: PMA will simply mark these pages as "allocatedPinned".
 * This list should be saved so that during pmaDestroy we can verify that only
 * these pages are still allocated.
 *
 * @param[in] blacklistCount:
 *      Number of pages in above list.
 *
 * @return
 *      NV_ERR_NO_MEMORY:
 *          Internal memory allocation failed.
 *      NV_ERR_GENERIC:
 *          Unexpected error. We try hard to avoid returning this error code,
 *          because it is not very informative.
 *
 */
NV_STATUS pmaRegisterRegion(PMA *pPma, NvU32 id, NvBool bAsyncEccScrub,
    PMA_REGION_DESCRIPTOR *pRegionDesc, NvU32 blacklistCount, PPMA_BLACKLIST_ADDRESS pBlacklistPage);


/*!
 * @brief Synchronous API for allocating pages from the PMA.
 * PMA will decide which pma regions to allocate from based on the provided
 * flags.  PMA will also initiate UVM evictions to make room for this
 * allocation unless prohibited by PMA_FLAGS_DONT_EVICT.  UVM callers must pass
 * this flag to avoid deadlock.  Only UVM may allocate unpinned memory from this
 * API and note that eviction callbacks need to be registered before that
 * happens.
 *
 * Alignment of the allocated pages is guaranteed to be greater or equal to the
 * requested page size. For contiguous allocations, a greater alignment can be
 * specified with the PMA_ALLOCATE_FORCE_ALIGNMENT flag and the alignment
 * allocation option. For non-contiguous allocations, it's an error to specify
 * an alignment larger than the page size.
 *
 * For broadcast methods, PMA will guarantee the same physical frames are
 * allocated on multiple GPUs, specified by the PMA objects passed in.
 *
 * Implementors note:
 *      If region registered with asyncEccScrub and pmaScrubComplete
 *      has not yet been issued then we cannot return NV_ERR_NO_MEMORY.
 *      We must instead drop the lock and wait until the next call to
 *      either pmaScrubComplete or pmaFreeContiguous/Pages to retry.
 *      Exception: PMA_ALLOCATE_DONT_EVICT
 *
 * @param[in] pPma
 *      The input PMA object
 *
 * @param[in] pageCount
 *      Number of pages to allocate.
 *
 * @param[in] pageSize
 *      64kb, 128kb or 2mb.  No other values are permissible.
 *
 * @param[in/out] allocationOptions
 * Input flags:
 *      PMA_ALLOCATE_DONT_EVICT
 *          Do not evict in order to service this allocation.
 *          Do not wait for ECC scrub completion if out of memory.
 *      PMA_ALLOCATE_PINNED
 *          The allocation is pinned (RM must pass this)
 *      PMA_ALLOCATE_SPECIFY_MININUM_SPEED
 *          Only examines regions whose speed is greater than
 *          minimumSpeed.
 *      PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE
 *          Restrict the allowable physical address range for
 *          allocation to [physBegin, physEnd).
 *      PMA_ALLOCATE_SPECIFY_REGION_ID
 *          Only service allocations out of 'regionId'.  The
 *          regionId is assigned in pmaRegisterRegion.
 *      PMA_ALLOCATE_PREFER_SLOWEST
 *          Prefer slower memory over faster.
 *      PMA_ALLOCATE_CONTIGUOUS
 *          If this allocation is a contiguous allocation
 *      PMA_ALLOCATE_PERSISTENT
 *          If this allocation should persist across suspend/resume
 *      PMA_ALLOCATE_FORCE_ALIGNMENT
 *          Force a specific alignment of the allocation. For non-contiguous
 *          allocations has to be less or equal to the page size.
 *
 * Output flags:
 *      PMA_ALLOCATE_RESULT_IS_ZERO
 *          If the allocated pages have been scrubbed.
 *
 * @param[out] pPages
 *      Return array of base addresses of allocated pages.
 *
 * @return
 *      NV_ERR_NO_MEMORY:
 *          Internal memory allocation failed.
 *      NV_ERR_GENERIC:
 *          Unexpected error. We try hard to avoid returning this error
 *          code,because it is not very informative.
 *
 */
NV_STATUS pmaAllocatePages(PMA *pPma, NvLength pageCount, NvU64 pageSize,
    PMA_ALLOCATION_OPTIONS *pAllocationOptions, NvU64 *pPages);

// allocate on multiple GPU, thus pmaCount
NV_STATUS pmaAllocatePagesBroadcast(PMA **pPma, NvU32 pmaCount, NvLength allocationCount,
    NvU64 pageSize, PMA_ALLOCATION_OPTIONS *pAllocationOptions, NvU64 *pPages);


/*!
 * @brief Marks previously unpinned pages as pinned.
 *
 * It will return an error and rollback any change if any page is not
 * previously marked "unpinned".
 *
 * @param[in] pPages
 *      Array of base addresses of pages to pin
 *
 * @param[in] pageCount
 *      Number of pages to pin
 *
 * @param[in] pageSize
 *      Page size of each page being pinned
 *
 * @return
 *      NV_OK:
 *          The pages have been pinned successfully.
 *
 *      NV_ERR_IN_USE:
 *          Some of the pages requested to be pinned are being evicted and thus
 *          cannot be pinned. None of the pages have been pinned and the caller
 *          needs to make sure the pages can be successfully evicted.
 *
 *      NV_ERR_INVALID_STATE:
 *          Some of the pages requested to be pinned weren't in the allocated
 *          unpinned state.
 *
 *      TODO some error for rollback
 *
 */
NV_STATUS pmaPinPages(PMA *pPma, NvU64 *pPages, NvLength pageCount, NvU64 pageSize);

/*!
 * @brief Marks a list of pages as free.
 * This operation is also used by RM to mark pages as "scrubbed" for the
 * initial ECC sweep. This function does not fail.
 *
 * @param[in] pPages
 *      Array of base addresses of pages to free
 *
 * @param[in] pageCount
 *      Number of pages to free
 *      If the value is 1, this is a contiguous free
 *
 * @param[in] size
 *      When freeing contiguous memory, this is the total size;
 *      When freeing discontiguous memory, this is page size of each page.
 *
 * @param[in] flag
 *      PMA_FREE_SKIP_SCRUB
 *          This flag is used to disable scrub on free when PMA frees the page
 *
 * @return Void
 *
 */
void pmaFreePages(PMA *pPma, NvU64 *pPages, NvU64 pageCount, NvU64 size, NvU32 flag);

/*!
 * @brief Clears scrubbing bit on PMA pages within the supplied range.
 *
 * @param[in] pma
 *      PMA object
 *
 * @param[in] rangeBase
 *      The start address
 *
 * @param[in] rangeLimit
 *      The end address
 *
 * @return
 *      void
 */
void pmaClearScrubRange(PMA *pPma, NvU64 rangeBase, NvU64 rangeLimit);


/*!
 * @brief Notifies the PMA that the ECC scrub is complete.
 *
 * Until this function is called no PMA allocations will fail with
 * "insufficient memory". They will hang and wait for the scrubbing.
 *
 * TODO consider suspend/resume behavior!
 *
 * Design Note:
 *      pmaRegisterRegion(w/ asyncEccScrub) leaves all pages as
 *      "allocated and pinned".
 *
 *      As the ECC scrub progresses, RM will call PMA and "pmaFreeContiguous"
 *      the regions as they are scrubbed.
 *
 * @param[in] pma
 *      PMA object
 *
 * @return
 *      NV_ERR_GENERIC:
 *          Unexpected error. We try hard to avoid returning this error
 *          code,because it is not very informative.
 *
 */
NV_STATUS pmaScrubComplete(PMA *pPma);


/*!
 * Register the eviction callbacks.
 *
 * Only one set of callbacks can be registered at a time and they need to be
 * unregistered with pmaUnregisterEvictionCb() before a new set can be
 * registered.
 *
 * Note that eviction callbacks need to be registered before allocating any unpinned memory.
 *
 * See the documentation of the callbacks above for details of the eviction.
 *
 * @param[in] pma
 *      PMA object
 *
 * @param[in] evictPageCb
 *      The callback function for evicting pages at a time
 *
 * @param[in] evictRangeCb
 *      The callback function for evicting a range
 *
 * @param[in] ctxPtr
 *      The callback context pointer to be passed back on callback
 *
 * @return
 *      NV_ERR_INVALID_ARGUMENT:
 *          One of the callbacks or PMA object was NULL.
 *
 *      NV_ERR_INVALID_STATE:
 *          Callbacks already registered.
 */
NV_STATUS pmaRegisterEvictionCb(PMA *pPma, pmaEvictPagesCb_t evictPagesCb, pmaEvictRangeCb_t evictRangeCb, void *ctxPtr);

/*!
 * Register the stats update callback.
 *
 * Register callback to call when number of free pages changes. Currently only used for RUSD.
 *
 * @param[in] pma
 *      PMA object
 *
 * @param[in] pUpdateCb
 *      The callback to call when updating free page count
 *
 * @param[in] ctxPtr
 *      The callback context pointer to be passed back on callback
 */
void pmaRegisterUpdateStatsCb(PMA *pPma, pmaUpdateStatsCb_t pUpdateCb, void *ctxPtr);


/*!
 * Unregister the eviction callbacks.
 *
 * Guarantees that all pending eviction callbacks complete before returning.
 *
 * All unpinned allocations must be freed before the callbacks are unregistered
 * and the caller needs to guarantee that any pending eviction callbacks won't
 * block on the thread unregistering the callbacks.
 *
 * The call does nothing if the PMA object is NULL.
 *
 * @param[in] pma
 *      PMA object.
 */
void pmaUnregisterEvictionCb(PMA *pPma);

/*!
 * @brief Returns information about the total FB memory.
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pBytesTotal    Pointer that will return the total FB memory size.
 *
 * @return
 *      void
 */
void pmaGetTotalMemory(PMA *pPma, NvU64 *pBytesTotal);

/*!
 * @brief Returns information about each region managed by PMA
 *
 * @param[in]  pPma           PMA pointer
 * @param[out] pRegSize       Pointer to size of region descriptor array
 * @param[out] ppRegionDesc   Pointer to the array of region descriptors
 *
 * @return
 *      NV_STATUS codes based on convention
 */
NV_STATUS pmaGetRegionInfo(PMA *pPma, NvU32 *pRegSize, PMA_REGION_DESCRIPTOR **ppRegionDesc);

/*!
 * @brief Returns information about the total free FB memory.
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pBytesFree     Pointer that will return the free FB memory size.
 *
 * @return
 *      void
 */
void pmaGetFreeMemory(PMA *pPma, NvU64 *pBytesFree);

/*!
 * @brief Returns information about the client address space size
 *        that can be allocated
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pSize          Pointer that will return the size of the
 *                            client-allocatable address space
 *
 * @return
 *      void
 */
void
pmaGetClientAddrSpaceSize(PMA *pPma, NvU64 *pSize);

/*!
 * @brief Returns information about the largest free FB memory chunk across all regions.
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pLargestFree   Pointer that will return the largest free FB memory size.
 * @param[in]  pRegionBase    Pointer that will return the region base of largest free FB memory.
 * @param[in]  pLargestOffset Pointer that will return the offset in region for largest free FB memory.
 *
 * @return
 *      void
 */
void pmaGetLargestFree(PMA *pPma, NvU64 *pLargestFree, NvU64 *pRegionBase, NvU64 *pLargestOffset);

/*!
 * @brief Returns a list of PMA allocated blocks which has ATTRIB_PERSISTENT
 *        attribute set. It will be used by FBSR module to save/restore
 *        clients PMA allocations during system suspend/resume.
 *
 * @param[in]     pPma              PMA pointer
 * @param[in/out] ppPersistList     Pointer to list of persistent segments
 *
 * @return
 *      NV_OK                   Success
 *      NV_ERR_NO_MEMORY        Failure to allocate list
 */
NV_STATUS pmaBuildPersistentList(PMA *pPma, PRANGELISTTYPE *ppPersistList);


/*!
 * @brief Returns a list of all PMA allocated blocks. For all the PMA
 *        allocated blocks, either STATE_PIN or STATE_UNPIN attribute will
 *        be set. It will be used by FBSR module to save/restore clients
 *        PMA allocations for Unix GC-OFF based power management.
 *
 * @param[in]     pPma      PMA pointer
 * @param[in/out] ppList    Pointer to list of all the PMA allocated blocks.
 *
 * @return
 *      NV_OK                   Success
 *      NV_ERR_NO_MEMORY        Failure to allocate list
 */
NV_STATUS pmaBuildAllocatedBlocksList(PMA *pPma, PRANGELISTTYPE *ppList);


/*!
 * @brief Frees previously generated list by function pmaBuildPersistentList().
 *
 * @param[in]       pPma            PMA pointer
 * @param[in/out]   ppPersistList   Pointer to list of persistent segments
 *
 * @return
 *      void
 */
void pmaFreePersistentList(PMA *pPma, PRANGELISTTYPE *ppPersistList);


/*!
 * @brief Frees previously generated list by function
 *        pmaBuildAllocatedBlocksList().
 *
 * @param[in]     pPma      PMA pointer
 * @param[in/out] ppList    Pointer to list of all the PMA allocated blocks.
 *
 * @return
 *      void
 */
void pmaFreeAllocatedBlocksList(PMA *pPma, PRANGELISTTYPE *ppList);

/*!
 * @brief Registers a memory scrubber to PMA. Currently only used for
 * the scrub-on-free feature.
 *
 * This function will take the PMA lock to protect against races between
 * the the use of the MemScrub object and any Reg/Unreg calls.
 *
 * @param[in]  pPma             PMA pointer
 * @param[in]  pScrubObj        Pointer to the scrubber
 *
 * @return
 *     NV_OK                    Success
 *     NV_INVALID_STATE         When PMA is NULL or bMemScrub is NV_FALSE
 */
NV_STATUS pmaRegMemScrub(PMA *pPma, OBJMEMSCRUB *pScrubObj);


/*!
 * @brief Unregisters the memory scrubber, when the scrubber is torn
 * down. If any feature is turned on that would require this scrubber to be
 * present, after this call, the PMA object will be unavailable to give out
 * any free pages for use, until pmaRegMemScrub is called.
 *
 * This function will take the PMA lock to protect against races between
 * the use of the MemScrub object and any Reg/Unreg calls.
 *
 * @param[in]  pPma             PMA pointer
 *
 * @return
 *     void
 */
void pmaUnregMemScrub(PMA *pPma);


/*!
 * @brief Notifies PMA that node onlining is complete and we can make pass-through
 * calls to OS.
 *
 * This function will take the PMA lock to protect against races between
 * the use of online/offline calls.
 *
 * TODO: Under development!
 * Currently:
 * - We online at hardcoded offset and size of PMA memory
 * - Normal PMA allocation will go to the part of PMA that is not onlined
 *
 * Future:
 * - Online all of PMA memory
 * - PMA requests will suballocate from OS
 * - Any mapping created over the PMA range will have to use kmap instead of ioremap
 *
 * Bug #1973975, 1858365, 1943187
 *
 * @param[in]  pPma              PMA pointer
 * @param[in]  numaNodeId        NUMA node ID that PMA is managing
 * @param[in]  coherentCpuFbBase The ATS aperture base corresponding to start of FB
 * @param[in]  coherentCpuFbSize The ATS aperture size. The actual size we
 *                               onlined. This could be different from the HW
 *                               ATS aperture size.
 *
 * @return
 *     NV_OK                    Success
 *     NV_INVALID_STATE         When PMA is NULL or bNuma is NV_FALSE or nodeId is too big
 */
NV_STATUS pmaNumaOnlined(PMA *pPma, NvS32 numaNodeId,
                         NvU64 coherentCpuFbBase, NvU64 coherentCpuFbSize);


/*!
 * @brief Notifies PMA that node offlining has started and PMA should start failing
 * all allocation calls.
 *
 * This function will take the PMA lock to protect against races between
 * the use of online/offline calls.
 *
 * @param[in]  pPma             PMA pointer
 *
 * @return
 *     void
 */
void pmaNumaOfflined(PMA *pPma);

/*!
 * @brief Returns client managed blacklisted pages in the PMA region
 *
 * @param[in]  pPma             PMA pointer
 * @param[in]  pChunks          pointer to blacklist addresses in the PMA region
 * @param[in]  pPageSize        pointer to Size of each blacklist page addresses
 * @param[in]  pNumChunks       pointer to valid client managed blacklist pages
 *
 * @return
 *     void
 */
void pmaGetClientBlacklistedPages(PMA *pPma, NvU64 *pChunks, NvU64 *pPageSize, NvU32 *pNumChunks);

/*!
 * @brief Returns the PMA blacklist size in bytes for
 *        both statically and dynamically blacklisted pages.
 *        pDynamicBlacklistSize and pStaticBlacklistSize are only copied-out if non-NULL.
 *
 * @param[in]  pPma                     PMA pointer
 * @param[in]  pDynamicBlacklistSize    pointer to dynamic blacklist size (bytes)
 * @param[in]  pStaticBlacklistSize     pointer to static blacklist size (bytes)
 *
 * @return
 *     void
 */
void pmaGetBlacklistSize(PMA *pPma, NvU32 *pDynamicBlacklistSize, NvU32 *pStaticBlacklistSize);

/*!
 * @brief Clear scrub bit for pages from the list of scrub items
 *        that fall in the base to base+size range to return these pages to PMA.
 *
 * @param[in]  pPma                     PMA pointer
 * @param[in]  pPmaScrubList            list of scrub items each with a id, base and size
 * @param[in]  count                    count of scrub items
 *
 * @return
 *     void
 */
void pmaClearScrubbedPages(PMA *pPma, SCRUB_NODE *pPmaScrubList, NvU64 count);

/*!
 * @brief Print states of all regions
 */
void pmaPrintMapState(PMA *pPma);

/*!
 * @brief Track the given physical address as blacklisted page in PMA. This call will blacklist
 *        the entire PMA page frame of size 64KB which contains the physical address.
 *
 * @param[in]  pPma                     PMA pointer
 * @param[in]  physAddr                 Address of the blacklisted page
 *
 * Locking:
 * - DO NOT call this function with the PMA lock already held.
 * - This function will internally grab the PMA lock to update the attribute bits.
 *
 * @return
 *     void
 */
NV_STATUS pmaAddToBlacklistTracking(PMA *pPma, NvU64 physBase);

/*!
 * @brief Returns total protected video memory.
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pBytesTotal    Pointer that will return the total FB memory size.
 *
 * @return
 *      void
 */
void pmaGetTotalProtectedMemory(PMA *pPma, NvU64 *pBytesTotal);

/*!
 * @brief Returns total unprotected video memory.
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pBytesTotal    Pointer that will return the total FB memory size.
 *
 * @return
 *      void
 */
void pmaGetTotalUnprotectedMemory(PMA *pPma, NvU64 *pBytesTotal);

/*!
 * @brief Returns information about the total free protected FB memory.
 *        In confidential compute use cases, memory will be split into
 *        protected and unprotected regions
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pBytesFree     Pointer that will return the free protected memory size.
 *
 * @return
 *      void
 */
void pmaGetFreeProtectedMemory(PMA *pPma, NvU64 *pBytesFree);

/*!
 * @brief Returns information about the total free unprotected FB memory.
 *        In confidential compute use cases, memory will be split into
 *        protected and unprotected regions
 *
 * @param[in]  pPma           PMA pointer
 * @param[in]  pBytesFree     Pointer that will return the free unprotected memory size.
 *
 * @return
 *      void
 */
void pmaGetFreeUnprotectedMemory(PMA *pPma, NvU64 *pBytesFree);

#ifdef __cplusplus
}
#endif

#endif // PHYS_MEM_ALLOCATOR_H
