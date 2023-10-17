/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef UTIL_H
#define UTIL_H

#include "phys_mem_allocator.h"
#include "nvport/nvport.h"
#include "regmap.h"
#include "nvmisc.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO See if this can be added to NvPort
#define pmaPortAtomicGet(ptr) portAtomicOrSize((ptr), 0)

NvU32 findRegionID(PMA *pPma, NvU64 address);
void pmaPrintBlockStatus(PMA_PAGESTATUS blockStatus);
void pmaRegionPrint(PMA *pPma, PMA_REGION_DESCRIPTOR *pRegion, void *pMap);
NvBool pmaStateCheck(PMA *pPma);
NV_STATUS pmaCheckRangeAgainstRegionDesc(PMA *pPma, NvU64 base, NvU64 size);

// Temporary putting these here. TODO refactor them in the next CL.
NV_STATUS _pmaEvictContiguous(PMA *pPma, void *pMap, NvU64 evictStart, NvU64 evictEnd,
                              MEMORY_PROTECTION prot);
NV_STATUS _pmaEvictPages(PMA *pPma, void *pMap, NvU64 *evictPages, NvU64 evictPageCount,
                         NvU64 *allocPages, NvU64 allocPageCount, NvU64 pageSize,
                         NvU64 physBegin, NvU64 physEnd, MEMORY_PROTECTION prot);
void      _pmaClearScrubBit(PMA *pPma, SCRUB_NODE *pPmaScrubList, NvU64 count);
NV_STATUS _pmaCheckScrubbedPages(PMA *pPma, NvU64 chunkSize, NvU64 *pPages, NvU32 pageCount);
NV_STATUS _pmaPredictOutOfMemory(PMA *pPma, NvLength allocationCount, NvU64 pageSize,
                                 PMA_ALLOCATION_OPTIONS *allocationOptions);
NV_STATUS pmaSelector(PMA *pPma, PMA_ALLOCATION_OPTIONS *allocationOptions, NvS32 *regionList);
void      _pmaReallocBlacklistPages (PMA  *pPma, NvU32 regId, NvU64 rangeBegin, NvU64 rangeSize);
void      _pmaFreeBlacklistPages (PMA  *pPma, NvU32 regId, NvU64 rangeBegin, NvU64 rangeSize);
NvBool    _pmaLookupBlacklistFrame (PMA  *pPma, NvU32 regId, NvU64 frameNum);

/*!
 * @brief Marks a list of pages with the specified state and attributes.
 * This operation is also used by RM to mark pages as "scrubbed" for the
 * initial ECC sweep. This function does not fail.
 *
 * @param[in] base:
 *      Start of address range to pin.
 *
 * @param[in] size:
 *      Region size in bytes
 *      Will try to align to 64KB page. For desired behavior, pass in aligned
 *      sizes.
 *
 * @param[in] pmaState:
 *      FREE, ALLOC_UNPIN, ALLOC_PIN
 *      PERSISTENT
 *      SCRUBBING
 *      EVICTING
 *
 * @param[in] pmaStateWriteMask:
 *      ALLOC_MASK
 *      PERSISTENT
 *      SCRUBBING
 *      EVICTING
 *
 * @return
 *      NV_ERR_GENERIC:
 *          Unexpected error. We try hard to avoid returning this error
 *          code,because it is not very informative.
 *
 */
void pmaSetBlockStateAttribUnderPmaLock(PMA *pPma, NvU64 base, NvU64 size,
    PMA_PAGESTATUS pmaState, PMA_PAGESTATUS pmaStateWriteMask);

/*!
 * @brief Marks a list of pages with the specified state and attributes.
 * This operation is also used by RM to mark pages as "scrubbed" for the
 * initial ECC sweep. This function does not fail.
 *
 * @param[in] base:
 *      Start of address range to pin.
 *
 * @param[in] size:
 *      Region size in bytes
 *      Will try to align to 64KB page. For desired behavior, pass in aligned
 *      sizes.
 *
 * @param[in] pmaState:
 *      FREE, ALLOC_UNPIN, ALLOC_PIN
 *      PERSISTENT
 *      SCRUBBING
 *      EVICTING
 *
 * @param[in] pmaStateWriteMask:
 *      ALLOC_MASK
 *      PERSISTENT
 *      SCRUBBING
 *      EVICTING
 *
 * @return
 *      NV_ERR_GENERIC:
 *          Unexpected error. We try hard to avoid returning this error
 *          code,because it is not very informative.
 *
 */
void pmaSetBlockStateAttrib(PMA *pPma, NvU64 base, NvU64 size,
    PMA_PAGESTATUS pmaState, PMA_PAGESTATUS pmaStateWriteMask);

/*
 * @brief Update the per region specific frame state statistics. This function helps
 * keep a running count of number of frames that are STATE_FREE, STATE_UNPIN, STATE_PIN.
 * It also keeps the 64KB free frame & 2MB pages statistics via pNumFree statistics.
 *
 * @param[in/out] pNumFree:
 *      Pointer to the statistic counter to update
 * @param[in/out] numPages:
 *      The number of pages to adjust pNumFree by
 * @param[in] oldState:
 *      The state the page was in
 *
 * @param[in] newState:
 *      The state the page will be in
 */
void pmaStatsUpdateState(NvU64 *pNumFree, NvU64 numPages, PMA_PAGESTATUS oldState,
	                     PMA_PAGESTATUS newState);

NvBool pmaIsEvictionPending(PMA *pPma);

void pmaOsSchedule(void);

/*!
 * @brief Returns a list of PMA-managed blocks with the specified state and
 *        attributes.
 *
 * @param[in]     pPma          PMA pointer
 * @param[in/out] ppList        Pointer to list of segments having specified
 *                              state and attributes
 * @param[in]     pageStatus    PMA page state and attribute
 *
 * @return
 *      NV_OK                   Success
 *      NV_ERR_NO_MEMORY        Failure to allocate list
 */
NV_STATUS pmaBuildList(PMA *pPma, PRANGELISTTYPE *ppList,
                       PMA_PAGESTATUS pageStatus);

/*!
 * @brief Frees previously generated list of PMA-managed blocks with
 *        function pmaBuildList()
 *
 * @param[in]     pPma      PMA pointer
 * @param[in/out] ppList    Pointer to list of PMA segments
 *
 * @return
 *      None
 */
void pmaFreeList(PMA *pPma, PRANGELISTTYPE *ppList);

/*!
 * @brief Registers blacklisting information
 * Called during pmaRegisterRegion to set the attribute for blacklisted pages
 *
 * @param[in] pPma                  PMA pointer
 * @param[in] physAddrBase          The base address of this address tree
 * @param[in] pBlacklistPageBase    Structure that contains the blacklisted pages
 * @param[in] blacklistCount        Number of blacklisted pages
 * @param[in] bBlacklistFromInforom Whether the blacklisted pages are coming from
 *                                  inforom (i.e., from heap/PMA init) or not
 *                                  (i.e., from ECC interrupt handling)
 *
 * @return NV_OK
 *         NV_ERR_NO_MEMORY if memory allocation fails
 */
NV_STATUS pmaRegisterBlacklistInfo(PMA *pPma, NvU64 physAddrBase,
                                      PPMA_BLACKLIST_ADDRESS pBlacklistPageBase, NvU32 blacklistCount,
                                      NvBool bBlacklistFromInforom);

/*!
 * @brief Query blacklisting states tracked by PMA
 *
 * @param[in] pPma                      PMA pointer
 * @param[in] pBlacklistCount           Pointer to store count of blacklisted pages
 * @param[in] pbClientManagedBlacklist  Pointer to store whether client manages blacklisting
 * @param[in] ppBlacklistChunks         Pointer to store the blacklisted chunks
 *
 * @return void
 */
void pmaQueryBlacklistInfo(PMA *pPma, NvU32 *pBlacklistCount, NvBool *pbClientManagedBlacklist,
                              PMA_BLACKLIST_CHUNK **ppBlacklistChunks);

/*!
 * @brief Update whether PMA has client managed blacklisting or not
 *
 * @param[in] pPma                     PMA pointer
 * @param[in] bClientManagedBlacklist  Whether PMA has client managed blacklisting
 *                                     When set to TRUE, PMA hands over control of blacklisted
 *                                     pages to the OS
 *                                     When set to FALSE, blacklisted pages are managed by PMA
 *
 * @return void
 */
void pmaSetClientManagedBlacklist(PMA *pPma, NvBool bClientManagedBlacklist);


/*!
 * @brief Checks if the address is already present in the blacklist info
 *
 * @param[in] pPma                     PMA pointer
 * @param[in] physAddress              Physical address that needs to be blacklisted

 * @return NvBool
 */
NvBool pmaIsBlacklistingAddrUnique(PMA *pPma, NvU64 physAddress);

#endif // UTIL_H
