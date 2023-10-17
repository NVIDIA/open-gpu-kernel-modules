/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_MMU_WALK_PRIVATE_H_
#define _NV_MMU_WALK_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------ Includes --------------------------------------- */
#include "mmu/mmu_walk.h"
#include "containers/btree.h"

/* --------------------------- Macros ---------------------------------------- */
#define HI_PRI_SUBLEVEL_INDEX     0
#define LO_PRI_SUBLEVEL_INDEX     1

/* --------------------------- Datatypes ------------------------------------ */

typedef struct MMU_WALK_LEVEL_INST     MMU_WALK_LEVEL_INST;
typedef struct MMU_WALK_LEVEL          MMU_WALK_LEVEL;
typedef struct MMU_WALK_OP_PARAMS      MMU_WALK_OP_PARAMS;

/*!
 * Higher-level PDE/PTE states.
 */
typedef enum
{
    /*!
     * The entry is not valid and will generate an MMU fault on access.
     */
    MMU_ENTRY_STATE_INVALID,

    /*!
     * Indicates to drop writes and return 0 on reads
     * when this entry is translated, instead of generating a fault.
     *
     * @note Only supported if MMU_FMT_CAPS.bSparse is set.
     */
    MMU_ENTRY_STATE_SPARSE,

    /*!
     * Indicates the entry is being used as PTE, mapping to a valid page.
     */
    MMU_ENTRY_STATE_IS_PTE,

    /*!
     * Indicates the entry is being used as PDE, pointing to one
     * or more valid sub-levels.
     */
    MMU_ENTRY_STATE_IS_PDE,

    /**
     * Indicates that there is no aligned valid 4K PTE for the 64K PTE.
     * It is a state only for 64K PTE when ATS is enabled
     *
     * @note Only supported if pWalk->flags.bAtsEnabled
     */
    MMU_ENTRY_STATE_NV4K,

    /*!
     * Should be kept at the end of the list so that it can be used
     * as a count for the number of states.
     */
    MMU_ENTRY_STATE_COUNT
} MMU_ENTRY_STATE;

typedef struct
{
    NvU8 state     : 6;
    NvU8 bHybrid   : 1;
    NvU8 bReserved : 1;
} MMU_ENTRY_INFO;

/*!
 * Information about the current level iteration state.
 * Used for converting the MMU walker to be iterative.
 */
typedef struct
{
    /*!
     * Variables cached for traversal/pdeRelease.
     */
    MMU_WALK_LEVEL_INST *pLevelInst;
    NvU64 vaLo;
    NvU64 vaHi;
    NvU64 vaLevelBase;
    NvU32 entryIndexHi;
    NvU32 entryIndex;
    NvU32 entryIndexFillStart;
    NvU32 entryIndexFillEnd;
    NvU32 pendingFillCount;
    /*!
     * Variable only cached for pdeRelease and not restored
     * on the stack
     */
    NvU64 entryVaLo;
} MMU_WALK_ITER_INFO;


/*!
 * Describes an entire (horizontal) level of an MMU level hiearchy.
 */
struct MMU_WALK_LEVEL
{
    /*!
     * Format of this level.
     */
    const MMU_FMT_LEVEL  *pFmt;

    /*!
     * Back-pointer to parent level.
     */
    MMU_WALK_LEVEL       *pParent;

    /*!
     * Sub-level array of length pFmt->numSubLevels.
     */
    MMU_WALK_LEVEL       *subLevels;

    /*!
     * Level instance tree keyed by VA range.
     */
    MMU_WALK_LEVEL_INST  *pInstances;

    /*!
     * Struct storing all variables needed for iterative MMU walker
     */
    MMU_WALK_ITER_INFO    iterInfo;

    /*!
     * Tree tracking ranges of VA that are reserved (locked down)
     * for this level. @see mmuWalkReserveEntries.
     */
    NODE                 *pReservedRanges;
};

/*!
 * Describes the physical aspects of a single page level instance.
 */
struct MMU_WALK_LEVEL_INST
{
    /*!
     * Embedded btree node anchor (must be first).
     */
    NODE                node;

    /*!
     * Memory descriptor for the physical memory backing this page level
     */
    MMU_WALK_MEMDESC   *pMemDesc;

    /*!
     * Current size in bytes of the physical level memory.
     */
    NvU32               memSize;

    /*!
     * Number of valid entries within this page level.
     */
    NvU32               numValid;

    /*!
     * Number of sparse entries within this page level.
     */
    NvU32               numSparse;

    /*!
     * Number of reserved entries (in any state) - @see mmuWalkReserveEntries.
     */
    NvU32               numReserved;

    /*!
     * Number of hybrid entries - mixed PTE/PDE above reserved sub-levels.
     */
    NvU32               numHybrid;

    /**
     * Number of NV4K (no aligned valid 4K PTE for a 64K PTE) entries
     */
    NvU32               numNv4k;

    /*!
     * State tracker for entries of this level instance.
     */
    MMU_ENTRY_INFO     *pStateTracker;
};

struct MMU_WALK
{
    MMU_WALK_USER_CTX        *pUserCtx;
    const MMU_WALK_CALLBACKS *pCb;
    MMU_WALK_FLAGS            flags;
    MMU_WALK_LEVEL            root;
    MMU_WALK_MEMDESC         *pStagingBuffer;
    NvBool                    bUseStagingBuffer;
    NvBool                    bInvalidateOnReserve;
};

/*!
 * @brief This function type implements the core operation to be
 * performed on the page levels. Typically each operation will expose an
 * external function to handle the root case. This external function will call
 * into an instance of this function type.
 *
 * @param[in]     pWalkParams   Pointer to @ref MMU_WALK.
 * @param[in]     pOpParams     Pointer to @ref MMU_WALK_OP_PARAMS.
 * @param[in]     pFmt          Pointer to @ref MMU_FMT_LEVEL.
 * @param[in/out] pDesc         Pointer to @ref MMU_WALK_LEVEL_INST.
 * @param[in]     vaLo          Lower end of the operational VA range.
 * @param[in]     vaHi          Higher end of the operational VA range.
 *
 * @return NV_STATUS_SUCCESS if mapping succeeds. Other errors if not.
 */
typedef NV_STATUS
MmuWalkOp(const MMU_WALK           *pWalk,
          const MMU_WALK_OP_PARAMS *pOpParams,
          MMU_WALK_LEVEL           *pLevel,
          MMU_WALK_LEVEL_INST      *pLevelInst,
          NvU64                     vaLo,
          NvU64                     vaHi);

/*!
 * @brief This function type is used to select the right sublevel for the operation
 *
 * @param[in]  pOpCtx       Pointer to user supplied operation context needed by the
 *                          implementing function.
 * @param[in]  pFmt         Pointer to @ref MMU_FMT_LEVEL of the level processed.
 * @param[out] pSubLevel    Pointer to @ref MMU_WALK_LEVEL_INST of the sublevel that
 *                          should be selected for processing.
 * @param[in]  vaLo         Lower end of the operational VA range.
 * @param[in]  vaHi         Higher end of the operational VA range.
 *
 * @return NV_STATUS_SUCCESS of allocation succeeds. Other errors if not.
 */
typedef NV_STATUS
MmuWalkOpSelectSubLevel(const void           *pOpCtx,
                        const MMU_WALK_LEVEL *pLevel,
                        NvU32                *pSubLevel,
                        NvU64                 vaLo,
                        NvU64                 vaHi);

/*!
 * This structure is used to represent parameteres needed  per operation.
 */
struct MMU_WALK_OP_PARAMS
{
    /*!
     * The context needed for the page level operation call back.
     * Example: Params for callbacks.
     */
    const void              *pOpCtx;

    /*!
     * @copydoc MmuWalkCBOpFunc
     */
    MmuWalkOp               *opFunc;

    /*!
     * @copydoc MmuWalkCBSelectSubLevel
     */
    MmuWalkOpSelectSubLevel *selectSubLevel;

    /*!
     * Indicates if this is a fill operation.
     */
    NvBool                   bFill : 1;

    /*!
     * Indicates if this is a release operation.
     */
    NvBool                   bRelease : 1;

    /*!
     * Indicates to ignore sub-level conflicts.
     */
    NvBool                   bIgnoreSubLevelConflicts : 1;

    /*!
     * Commit already allocated page directory entries.
     * This is needed in suspend-resume scenarios, where
     * the page directory contents are lost during suspend.
     * During restore, we need to rewrite the PDEs to the original values.
     */
    NvBool                   bCommit : 1;
};

/*!
 * Op context for fill operations (unmap and sparse).
 */
typedef struct
{
    MMU_ENTRY_STATE      entryState;
    MMU_WALK_FILL_STATE  fillState;
} MMU_FILL_TARGET;

extern const MMU_WALK_OP_PARAMS g_opParamsSparsify;
extern const MMU_WALK_OP_PARAMS g_opParamsUnmap;

/*----------------------------Private Interface--------------------------------*/
const MMU_WALK_LEVEL *
mmuWalkFindLevel(const MMU_WALK      *pWalk,
                 const MMU_FMT_LEVEL *pLevelFmt);

NV_STATUS
mmuWalkProcessPdes(const MMU_WALK           *pWalk,
                   const MMU_WALK_OP_PARAMS *pOpParams,
                   MMU_WALK_LEVEL           *pLevel,
                   MMU_WALK_LEVEL_INST      *pLevelInst,
                   NvU64                     vaLo,
                   NvU64                     vaHi);

NV_STATUS
mmuWalkRootAcquire(MMU_WALK   *pWalk,
                   NvU64       vaLo,
                   NvU64       vaHi,
                   NvBool      bForceCommit);

void
mmuWalkRootRelease(MMU_WALK *pWalk);

MMU_ENTRY_STATE
mmuWalkGetEntryState(MMU_WALK_LEVEL_INST *pLevelInst, NvU32 entryIndex);

void
mmuWalkSetEntryState(MMU_WALK_LEVEL_INST *pLevelInst,
                     NvU32                entryIndex,
                     MMU_ENTRY_STATE      state);

void
mmuWalkSetEntryReserved(MMU_WALK_LEVEL_INST *pLevelInst,
                        NvU32                entryIndex,
                        NvBool               bReserved);

void
mmuWalkSetEntryHybrid(MMU_WALK_LEVEL_INST *pLevelInst,
                      NvU32                entryIndex,
                      NvBool               bHybrid);

void
mmuFmtCalcAlignedEntryIndices(const MMU_FMT_LEVEL *pPageFmtIn,
                              const NvU32 indexLoIn,
                              const NvU32 indexHiIn,
                              const MMU_FMT_LEVEL *pPageFmtOut,
                              NvU32 *pIndexLoOut,
                              NvU32 *pIndexHiOut);

MmuWalkOp               mmuWalkFill;
MmuWalkOpSelectSubLevel mmuWalkFillSelectSubLevel;

#ifdef __cplusplus
}
#endif

#endif
