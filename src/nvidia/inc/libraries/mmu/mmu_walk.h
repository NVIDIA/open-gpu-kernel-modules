/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_MMU_WALK_H_
#define _NV_MMU_WALK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @file mmu_walk.h
 *
 * @brief Defines high-level utilities to manage, update, and query general MMU page tables.
 *
 * The MMU walk library provides a set of routines to manage and modify
 * the page directories and tables of an MMU page level hierarchy.
 *
 * An instance of an MMU level hierarchy may be associated with a
 * virtual address space (VAS) at a higher level, but this association
 * is not handled by the library.
 *
 * @par State:
 * The library requires SW state for each hierarchy being managed.
 * Library users refer to this state through an opaque
 * @ref MMU_WALK pointer, initialized by @ref mmuWalkCreate
 * and destroyed by @ref mmuWalkDestroy.
 *
 * @par Locking:
 * The library requires exclusive access to its @ref MMU_WALK
 * state during each operation.
 * It does NOT support concurrent operations on the same
 * @ref MMU_WALK state, but each state is independent.
 * Callers must also ensure any resources accessed by its
 * callbacks are synchronized appropriately.
 *
 * @par Synchrony:
 * While concurrent updates on the same hiearachy are not supported,
 * the walker does support asynchronous/buffered updates.
 * If the user callbacks support this strategy, it is possible to
 * pipeline update operations with VAS access.
 *
 * @par Flushing:
 * All access to page level memory and HW is abstracted by
 * user callbacks. Therefore the walker does not enforce
 * or schedule flushes or invalidation of caches/TLBs
 * required for each operation. It is up to user callbacks to
 * handle these appropriately.
 *
 * The basic operations of the MMU walker are mapping and unmapping
 * ranges of VA, provided by @ref mmuWalkMap and @ref mmuWalkUnmap.
 *
 * These two operations have few restrictions on the allowed state transitions.
 * The goal is to provide flexible primitives that enable the user
 * to enforce specific (safer) policies above the walker.
 *
 * For example, it is possible to unmap a range that only partially
 * overlaps existing mapped ranges. Similarly, existing mappings may be
 * clobbered by new ones or moved from one page size to another.
 *
 * However, the walker does ensure that state transitions are complete -
 * stale mappings of different page size are cleared and
 * intermediate states are minimized (though not fully atomic).
 *
 * The remaining APIs are for special features and/or tuning.
 *
 * Sparse textures (a.k.a. tiled resources) is supported through
 * @ref mmuWalkSparsify and @ref mmuWalkUnsparsify.
 *
 * Pre-reservation (lock-down) of page level memory for external use
 * and/or to force non-lazy page level allocation is supported through
 * @ref mmuWalkReserveEntries and @ref mmuWalkReleaseEntries.
 *
 * External migration of page level memory is supported through
 * @ref mmuWalkMigrateLevelInstance. This is needed only to migrate
 * page level instances. The target physical memory mapped by the levels
 * can be migrated with @ref mmuWalkMap (user must handle the copy part).
 */

/* ------------------------ Includes --------------------------------------- */
#include "nvtypes.h"
#include "nvstatus.h"
#include "mmu_fmt.h"

/* ------------------------ Version --------------------------------------- */
/*!
 * MMU Walk Api version number.
 * version 2 added bIgnoreChannelBusy parameter in MmuWalkCBUpdatePdb
 * and mmuWalkMigrateLevelInstance.
 * @note - Whenever any of this API changes increment this version number. This
 * is required to maintain compatibility with external clients.
 */
#define MMU_WALK_API_VERSION 2

/* --------------------------- Datatypes ------------------------------------ */

/*!
 * Opaque library-defined state for a single page level hierarchy backing a VAS.
 */
typedef struct MMU_WALK             MMU_WALK;

/*!
 * Opaque user-defined state describing a block of physical memory.
 * The library references these as the backing memory for page level instances.
 */
typedef struct MMU_WALK_MEMDESC     MMU_WALK_MEMDESC;

/*!
 * Opaque user-defined state passed to all user callbacks.
 */
typedef struct MMU_WALK_USER_CTX    MMU_WALK_USER_CTX;

/*!
 * State that a range of MMU page level entries can be filled to.
 * @see MmuWalkCBFillEntries.
 */
typedef enum
{
    /*!
     * The range is not valid and will generate a fault on access.
     */
    MMU_WALK_FILL_INVALID,

    /*!
     * Also known as the "zero" state.
     * Writes are dropped and reads return zero when the range is accessed.
     *
     * @note Not all MMUs support this state/feature.
     */
    MMU_WALK_FILL_SPARSE,

    /**
     * No valid aligned 4K PTE state for a 64K PTE
     * 64K big PTE state indicating that there is no valid 4K aligned PTEs
     *
     * @note This is not supported pre Volta.
     */
    MMU_WALK_FILL_NV4K,
} MMU_WALK_FILL_STATE;

/*!
 * See @ref mmuWalkMap.
 */
typedef struct MMU_MAP_TARGET       MMU_MAP_TARGET;

/*!
 * See @ref mmuWalkMap.
 */
typedef struct MMU_MAP_ITERATOR     MMU_MAP_ITERATOR;

/*!
 * User callback to allocate backing physical memory for a page level instance.
 *
 * The contents of the memory need not be initialized.
 * The walker will initialize entries before use.
 *
 * The walker calls this lazily when a page level instance is
 * required for the operation taking place.
 * It is up to user implementation whether to allocate new memory,
 * pre-allocate, pool, etc.
 *
 * It is also up to user to determine the best physical aperture and
 * attributes for the memory (e.g. for GPU whether to place in vidmem/sysmem).
 * The walker only modifies the memory contents through the remaining
 * callbacks below so access is entirely opaque.
 *
 * This interface has several parameters that are required for
 * specialized tuning of particular MMU formats, but for a simple
 * user implementation most can be ignored.
 *
 * @param[in]  vaBase       First absolute VA covered by this level instance.
 *                            This (+pLevelFmt) uniquely identifies the instance.
 * @param[in]  vaLimit      Last absolute VA covered by this level instance
 *                            required for the current operation.
 *                            This may be used to implement dynamic growth
 *                            for levels that support it (e.g. root page dir).
 * @param[in]  bTarget      Indicates if this level instance is the target
 *                            for the current operation.
 *                            If bTarget is false it is usually not required
 *                            to allocate memory, but it is required to maintain
 *                            parallel partial size sub-levels.
 * @param[in,out] ppMemDesc On input, the existing memory descriptor for this instance
 *                            (may be NULL).
 *                            This must NOT be modified or freed during this
 *                            callback. The walker will call @ref MmuWalkCBLevelFree
 *                            when this memory is no longer required.
 *                          On output, new memory descriptor for this instance.
 *                            Leaving the old memory descriptor is allowed if it
 *                            already provides sufficient backing memory for the given VA range.
 *                            If bTarget is true, this MUST be non-NULL on success.
 * @param[in,out] pMemSize  Old/new memory size in bytes.
 *                            Can be used for dynamic root page directory growth
 *                            or partial-size page tables.
 * @param[in,out] pBChanged Indicates if the backing memory behind *ppMemDesc has
 *                            changed (initially NV_FALSE).
 *                            This must be set if either the *ppMemDesc pointer or
 *                            *pMemSize change, but also allows for changes in
 *                            physical aperture or location to be updated properly.
 *
 * @returns On failure the current walk operation will be aborted
 *          and the SW state rolled back.
 * @returns The walker will only call this function
 *          prior to page level and HW state modifications.
 */
typedef NV_STATUS
MmuWalkCBLevelAlloc
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const NvU64              vaBase,
    const NvU64              vaLimit,
    const NvBool             bTarget,
    MMU_WALK_MEMDESC       **ppMemDesc,
    NvU32                   *pMemSize,
    NvBool                  *pBChanged
);

/*!
 * User callback to free backing physical memory of an unused page level instance.
 *
 * The contents of the memory when freed are undefined.
 * It is up to the user to zero the memory if required.
 *
 * The walker calls this aggressively when a page level instance is no longer
 * required (on a commit or discard operation).
 * It is up to user implementation whether to free immediately,
 * cache for later re-use, etc.
 *
 * @param[in]  vaBase    First absolute VA covered by this level instance.
 *                         This (+pLevelFmt) uniquely identifies the instance.
 * @param[in]  pOldMem   Memory descriptor to free.
 */
typedef void
MmuWalkCBLevelFree
(
    MMU_WALK_USER_CTX   *pUserCtx,
    const MMU_FMT_LEVEL *pLevelFmt,
    const NvU64          vaBase,
    MMU_WALK_MEMDESC    *pOldMem
);

/*!
 * User callback to initialize the HW root page directory pointer (PDB).
 * In this context "PDB" stands for "page directory base (address)."
 *
 * Example: For GPU MMU this should update the instance blocks
 *          associated with the VAS.
 *
 * @returns NV_TRUE if the operation completed.
 * @returns NV_FALSE if the operation must be retried later. See @ref mmuWalkContinue.
 */
typedef NvBool
MmuWalkCBUpdatePdb
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pRootFmt,
    const MMU_WALK_MEMDESC  *pRootMem,
    const NvBool             bIgnoreChannelBusy
);

/*!
 * User callback to initialize a page directory entry to point to one or more
 * sub-levels.
 *
 * @param[in]  pLevelFmt  Format of the parent level.
 * @param[in]  pLevelMem  Memory descriptor of the parent level.
 * @param[in]  entryIndex Index of the PDE being initialized.
 * @param[in]  pSubLevels Array of sub-level memory descriptors of length
 *                        pLevelFmt->numSubLevels.
 *
 * @returns NV_TRUE if the operation completed.
 * @returns NV_FALSE if the operation must be retried later. See @ref mmuWalkContinue.
 */
typedef NvBool
MmuWalkCBUpdatePde
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const MMU_WALK_MEMDESC  *pLevelMem,
    const NvU32              entryIndex,
    const MMU_WALK_MEMDESC **pSubLevels
);

/*!
 * User callback to fill a range of entries with a constant state.
 *
 * @param[in]     pLevelFmt    Format of the level.
 * @param[in]     pLevelMem    Memory descriptor of the level.
 * @param[in]     entryIndexLo First entry index to initialize.
 * @param[in]     entryIndexHi Last entry index to initialize.
 * @param[in]     fillState    Constant state to initialize to.
 * @param[in,out] pProgress    Number of entries filled successfully (0 on input).
 *
 * @returns If (*pProgress == entryIndexHi - entryIndexLo + 1) then
 *          the operation completed successfully.
 * @returns Otherwise the operation must be retried later for
 *          the remaining entries. See @ref mmuWalkContinue.
 */
typedef void
MmuWalkCBFillEntries
(
    MMU_WALK_USER_CTX         *pUserCtx,
    const MMU_FMT_LEVEL       *pLevelFmt,
    const MMU_WALK_MEMDESC    *pLevelMem,
    const NvU32                entryIndexLo,
    const NvU32                entryIndexHi,
    const MMU_WALK_FILL_STATE  fillState,
    NvU32                     *pProgress
);

/*!
 * User callback to copy a range of entries between backing page level memory.
 *
 * @note This interface is only required if dynamic page level growth is
 *       supported (e.g. for the root page directory or partial page tables)
 *       or for page level migration (@ref mmuWalkMigrateLevelInstance).
 *
 * @param[in]     pLevelFmt    Format of the level.
 * @param[in]     pMemSrc      Source memory descriptor of the level.
 * @param[in]     pMemDst      Destination memory descriptor of the level.
 * @param[in]     entryIndexLo First entry index to copy.
 * @param[in]     entryIndexHi Last entry index to copy.
 * @param[in,out] pProgress    Number of entries copied successfully (0 on input).
 *
 * @returns If (*pProgress == entryIndexHi - entryIndexLo + 1) then
 *          the operation completed successfully.
 * @returns Otherwise the operation must be retried later for
 *          the remaining entries. See @ref mmuWalkContinue.
 */
typedef void
MmuWalkCBCopyEntries
(
    MMU_WALK_USER_CTX         *pUserCtx,
    const MMU_FMT_LEVEL       *pLevelFmt,
    const MMU_WALK_MEMDESC    *pMemSrc,
    const MMU_WALK_MEMDESC    *pMemDst,
    const NvU32                entryIndexLo,
    const NvU32                entryIndexHi,
    NvU32                     *pProgress
);

/*!
 * User callback to copy staging buffer to its final destination.
 *
 * If NULL is passed as the staging buffer on walker creation,
 * this callback is ignored. Otherwise, this callback should
 * perform a memcopy from the table located at the staging buffer
 * memdesc to its final location in the buffer allocated for the
 * actual table (in FB or otherwise).
 *
 * @param[in] pStagingBuffer Staging buffer PD/PTs are written to
 * @param[in] pLevelBuffer   Memdesc containing final location for
 *                           PD/PT
 * @param[in] entryIndexLo   Start index of entries to be copied.
 * @param[in] entryIndexHi   End index (inclusive) of entries to be
 *                           copied.
 * @param[in] tableSize      Size of the current level of PD/PT, in
 *                           entries. The offsets into the staging
 *                           buffer are the entry indices taken
 *                           modulo tableSize.
 * @param[in] entrySize      Size of each entry, in bytes
 */
typedef void
MmuWalkCBWriteBuffer
(
    MMU_WALK_USER_CTX    *pUserCtx,
    MMU_WALK_MEMDESC     *pStagingBuffer,
    MMU_WALK_MEMDESC     *pLevelBuffer,
    NvU64                 entryIndexLo,
    NvU64                 entryIndexHi,
    NvU64                 tableSize,
    NvU64                 entrySize
);

/*!
 * Bundles user-implemented callback pointers.
 */
typedef struct
{
    MmuWalkCBLevelAlloc  *LevelAlloc;
    MmuWalkCBLevelFree   *LevelFree;
    MmuWalkCBUpdatePdb   *UpdatePdb;
    MmuWalkCBUpdatePde   *UpdatePde;
    MmuWalkCBFillEntries *FillEntries;
    MmuWalkCBCopyEntries *CopyEntries;
    MmuWalkCBWriteBuffer *WriteBuffer;
} MMU_WALK_CALLBACKS;



/*!
 * Flags that affect walk library behavior.
 */
typedef struct
{
    /*!
     * Indicates if the user implementation supports asynchronous/buffered
     * updates, such that all callbacks that modify page level and/or HW state
     * are buffered (e.g. to be committed by a later DMA/copy).
     *
     * The primary advantage of asynchronous mode is the potential to pipeline
     * updates with other work.
     *
     * The main drawback of asynchronous mode is the amount of space required for
     * the buffers is generally not known ahead of time (bounded, but potentially large).
     * The walker library supports splitting a given operation into multiple
     * pieces, each piece continuing where it left off until an operation is complete.
     * This way the user can use a fixed or limited size buffer.
     *
     * Synchronous update mode (default) *requires* the callbacks to modify page level
     * and HW state immediately. This is usually simpler to implement but
     * less efficient.
     */
    NvBool bAsynchronous : 1;
    /**
     * @brief      Indicates if ATS is enabled.
     * @details    Should be setup as:
     *             gvaspaceIsAtsEnabled(pWalk->pUserCtx->pGVAS)
     *             Currently, from 8/2016, it is used to enable NV4K (no valid
     *             4K PTE) in MMU walker
     */
    NvBool bAtsEnabled : 1;
} MMU_WALK_FLAGS;

/*!
 * User callback to map a batch of entries during an @ref mmuWalkMap operation.
 *
 * A "batch" is a contiguous range of entries within a single page level instance.
 * It is the responsibility of the callback to track the current
 * page index into the target physical memory (if applicable).
 *
 * @note The reason this interface enforces batching is to amortize the cost
 *       of the function pointer (callback) flexibility.
 *       Some architectures (e.g. ARM) have performance issues with indirect
 *       function calls, and PTE init loop should be the critical path.
 *
 * @param[in]     entryIndexLo First entry index to map.
 * @param[in]     entryIndexHi Last entry index to map.
 * @param[in,out] pProgress    Number of entries mapped successfully (0 on input).
 *
 * @returns If (*pProgress == entryIndexHi - entryIndexLo + 1) then
 *          the operation completed successfully.
 * @returns Otherwise the operation must be retried later for
 *          the remaining entries. See @ref mmuWalkContinue.
 */
typedef void
MmuWalkCBMapNextEntries
(
    MMU_WALK_USER_CTX        *pUserCtx,
    const MMU_MAP_TARGET     *pTarget,
    const MMU_WALK_MEMDESC   *pLevelMem,
    const NvU32               entryIndexLo,
    const NvU32               entryIndexHi,
    NvU32                    *pProgress
);

/*!
 * Describes the physical memory (target) of an @ref mmuWalkMap operation.
 */
struct MMU_MAP_TARGET
{
    /*!
     * Target level format.
     */
    const MMU_FMT_LEVEL     *pLevelFmt;

    /*!
     * User-defined iterator for the physical pages being mapped.
     * This may be context sensitive - e.g. it can contain a counter to track
     * the current page index. The walker will always call this for consecutive
     * increasing page indices across a single map operation.
     *
     * @note The lifetime of this pointer extends until the operation
     *       completes. Take care that it is not stack allocated if
     *       using @ref mmuWalkContinue from a different call stack later.
     */
    MMU_MAP_ITERATOR        *pIter;

    /*!
     * Callback to map the batch of entries.
     */
    MmuWalkCBMapNextEntries *MapNextEntries;

    /*!
     * Page array granularity of the physical target memory
     */
    NvU64 pageArrayGranularity;
};

/*----------------------------Public Interface--------------------------------*/

/*!
 * Create an initial walker library SW state.
 *
 * @param[in]  pRootFmt       MMU format of the root page level.
 * @param[in]  pUserCtx       User-defined context passed to callbacks.
 * @param[in]  pCb            User-implemented callback bundle.
 * @param[in]  flags          Flags applying to this walker instance.
 * @param[out] ppWalk         Returned walker state.
 * @param[in]  pStagingBuffer Optional memdesc to stage PD/PT writes to.
 */
NV_STATUS
mmuWalkCreate
(
    const MMU_FMT_LEVEL      *pRootFmt,
    MMU_WALK_USER_CTX        *pUserCtx,
    const MMU_WALK_CALLBACKS *pCb,
    const MMU_WALK_FLAGS      flags,
    MMU_WALK                **ppWalk,
    MMU_WALK_MEMDESC         *pStagingBuffer
);

/*!
 * Destroy a walker library SW state.
 *
 * This will free all remaining memory referenced by the walker, but it
 * is recommended to enforce symmetric operations at a higher level
 * to catch/report memory leaks.
 */
void
mmuWalkDestroy
(
    MMU_WALK *pWalk
);

/*!
 * Map a range of VA to physical memory at an arbitrary page level.
 *
 * The VA range must be aligned to the MMU's smallest page size and
 * to the largest page size of any previous mapping that overlaps.
 * The VA range cannot cross a sparse boundary.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkMap
(
    MMU_WALK             *pWalk,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const MMU_MAP_TARGET *pTarget
);

/*!
 * Return a range of VA to its unmapped state (invalid or sparse).
 *
 * The VA range must be aligned to the MMU's smallest page size and
 * to the largest page size of any previous mappings that overlap.
 * The VA range cannot cross a sparse boundary.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkUnmap
(
    MMU_WALK             *pWalk,
    const NvU64           vaLo,
    const NvU64           vaHi
);

/*!
 * Set the unmapped state of a VA range to the sparse (zero) state.
 *
 * The VA range must be aligned to the MMU's smallest page size.
 * The previous state of the entire range must be unmapped and non-sparse.
 * The last parameter indicates whether the staging buffer and the WriteBuffer
 * callback should be used.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkSparsify
(
    MMU_WALK             *pWalk,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const NvBool          bUseStagingBuffer
);

/*!
 * Return a range of VA to the invalid unmapped state.
 *
 * The VA range must exactly match a previously sparsified range.
 * Any mappings remaining within the range are cleared to invalid.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkUnsparsify
(
    MMU_WALK             *pWalk,
    const NvU64           vaLo,
    const NvU64           vaHi
);

/*!
 * Reserve (lock-down) page level entries for a VA range.
 *
 * The VA range must be aligned to the target page size.
 * The range may not overlap with an existing reserved range for the
 * target page level, but reservation state between levels is independent.
 *
 * @note This does not change the effective state of the VA range.
 *       It only changes the state of the backing page level memory.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkReserveEntries
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const NvBool          bInvalidate
);

/*!
 * Release page level entries previously reserved.
 *
 * The VA range must exactly match an existing reserved range for
 * the target page level.
 *
 * @note This does not change the effective state of the VA range.
 *       It only changes the state of the backing page level memory.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkReleaseEntries
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaLo,
    const NvU64           vaHi
);

/*!
 * Commit the page directory entries for a VA range.
 *
 * Traverse the walker and rewrite the PDEs from the SW state.
 * This won't trigger any new PDE allocations or state change.
 *
 * This call won't affect the PTEs. If needed, support can be added later.
 *
 * The VA range must be aligned to the MMU's smallest page size and
 * to the largest page size of any previous mappings that overlap.
 * The VA range cannot cross a sparse boundary.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkCommitPDEs
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelTarget,
    const NvU64           vaLo,
    const NvU64           vaHi
);

/*!
 * Switches a page level instance from one memory location to another.
 *
 * This function commits the PDB to the HW if the level instance being
 * migrated happens to be the PDB.
 *
 * @note This differs from @ref mmuWalkMigrateLevelInstance in that it
 * copies/does not copy and updates/does not update parent PDE as
 * specified by the caller.
 *
 * @returns
 */
NV_STATUS
mmuWalkModifyLevelInstance
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaBase,
    MMU_WALK_MEMDESC     *pNewMem,
    const NvU32           memSize,
    const NvBool          bCopyEntries,
    const NvBool          bUpdatePde,
    const NvBool          bIgnoreChannelBusy
);

/*!
 * Migrate a page level instance from one memory location to another.
 *
 * The VA must be aligned to the base of an instance that has been
 * previously allocated by the walker through @ref MmuWalkCBLevelAlloc.
 *
 * @note This does not change the effective state of any VA range.
 *       It only changes the state of the backing page level memory.
 *       It is a wrapper around @ref mmuWalkModifyLevelInstance.
 *
 * @returns See @ref mmuWalkContinue.
 */
NV_STATUS
mmuWalkMigrateLevelInstance
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaBase,
    MMU_WALK_MEMDESC     *pNewMem,
    const NvU32           memSize,
    const NvBool          bIgnoreChannelBusy
);

/*!
 * Query a walker SW state for the page level instance memory descriptors
 * backing a given virtual address and page size.
 * The caller provides an array of memdesc pointers.
 */
NV_STATUS
mmuWalkGetPageLevelInfo
(
    const MMU_WALK          *pWalk,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const NvU64              virtAddr,
    const MMU_WALK_MEMDESC **ppMemDesc,
    NvU32                   *pMemSize
);

/*!
 * Force frees all page level instances. We may have to force free page levels
 * in case of surprise removal. In the surprise removal case, we may end up
 * with many failed unmappings once the GPU is off the bus. This might leave
 * some of the MMU_WALK_LEVEL and MMU_WALK_LEVEL_INST objects to be in an
 * allocated state. This function just iterates over the level instances at
 * each level and force frees everything ignoring any outstanding valid, sparse
 * and reserved entries..
 */
void
mmuWalkLevelInstancesForceFree
(
    MMU_WALK *pWalk
);

/*!
 * Continue a walker operation that was previously started.
 *
 * If a state changing operation on the walker returns
 * NV_WARN_MORE_PROCESSING_REQUIRED, the user must call this function
 * to continue processing once resources (e.g. pushbuffer space)
 * are again available.
 *
 * Any operation-specific context passed to the walker when the operation
 * is started continues to be referenced until the operation completes.
 *
 * @returns NV_OK if the operation has finished. For asynchronous mode,
 *          the user must call @ref mmuWalkCommit once the last update
 *          buffer has committed.
 *
 * @returns NV_WARN_MORE_PROCESSING_REQUIRED if more processing is
 *          required. As a pathological case the walker supports a
 *          1-entry update buffer, but that is obviously not efficient.
 *
 * @returns Any other error codes indicate the walker is either
 *          not in a state that can continue (user bug, ignored) or
 *          there is an interal bug - either in walker or user
 *          callbacks. The latter case is a fatal error - there is no
 *          way for walker to recover from such situations as the
 *          SW/HW state has potentially lost consistency.
 *          This would require fully transactional updates
 *          that would both increase intermediate memory requirements and
 *          the probability of an internal bug :D.
 *          The user must decide how to handle this case (either ignore
 *          and hope for the best or reset/crash the context using
 *          this state).
 */
NV_STATUS
mmuWalkContinue
(
    MMU_WALK *pWalk
);

/*!
 * Commit any pending SW state the walker is tracking and
 * free references to unused page level instances.
 *
 * The user only needs to call this if supporting
 * @ref MMU_WALK_FLAGS::bAsynchronous mode.
 * Otherwise this will be called automatically once an operation completes.
 *
 * For buffered updates, the user must call this after the entire
 * operation has completed - once @ref mmuWalkContinue returns NV_OK and the final
 * update buffer has been committed to memory/HW (only then is it safe
 * to free the unused level instances).
 */
void
mmuWalkCommit
(
    MMU_WALK *pWalk
);

/*!
 * Get the user context of a walker state.
 */
MMU_WALK_USER_CTX *
mmuWalkGetUserCtx
(
    const MMU_WALK *pWalk
);

/*!
 * Set the user context of a walker state.
 */
NV_STATUS
mmuWalkSetUserCtx
(
    MMU_WALK          *pWalk,
    MMU_WALK_USER_CTX *pUserCtx
);

/*!
 * Get the user callbacks of a walker state.
 */
const MMU_WALK_CALLBACKS *
mmuWalkGetCallbacks
(
    const MMU_WALK *pWalk
);

/*!
 * Set the user callbacks of a walker state.
 */
void
mmuWalkSetCallbacks
(
    MMU_WALK                 *pWalk,
    const MMU_WALK_CALLBACKS *pCb
);

#ifdef __cplusplus
}
#endif

#endif
