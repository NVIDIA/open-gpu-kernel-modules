/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RM_POOL_ALLOC_H_
#define _RM_POOL_ALLOC_H_

/*!
 * @file pool_alloc.h
 *
 * @brief Describes utilities for creating pools for RM internal usage,
 *        allocating memory from the pools, freeing memory to the pools
 *        and destroying the pools.
 *        Uses:
 *        1. RM allocations for client page tables
 *        2. Context-specific RM allocations like context buffers, instance memory, fault buffers, GR local buffers
 *        3. Engine-specific RM allocations like runlist buffers and GR global buffers
 */

/* ---------------------------------Includes ------------------------------------ */
#include "core/core.h"

/* ------------------------------- Public Interface ----------------------------- */
/*!
 * Static configurations for various pools.
 */

typedef enum
{
    POOL_CONFIG_GMMU_FMT_1,     // configure pool for client page tables with version = GMMU_FMT_VERSION_1
    POOL_CONFIG_GMMU_FMT_2,     // configure pool for client page tables with version = GMMU_FMT_VERSION_2
    POOL_CONFIG_CTXBUF_256G,    // configure pool for RM internal allocations like ctx buffers with 256GB page size
    POOL_CONFIG_CTXBUF_512M,    // configure pool for RM internal allocations like ctx buffers with 512MB page size
    POOL_CONFIG_CTXBUF_2M,      // configure pool for RM internal allocations like ctx buffers with 2MB page size
    POOL_CONFIG_CTXBUF_64K,     // configure pool for RM internal allocations like ctx buffers with 64KB page size
    POOL_CONFIG_CTXBUF_4K,      // configure pool for RM internal allocations like ctx buffers with 4KB page size
    POOL_CONFIG_MAX_SUPPORTED

}POOL_CONFIG_MODE;
/* ------------------------------------ Datatypes ---------------------------------- */

/*!
 * Opaque library-defined state of a pool reserve.
 */
typedef struct RM_POOL_ALLOC_MEM_RESERVE_INFO   RM_POOL_ALLOC_MEM_RESERVE_INFO;

/*!
 * Opaque user-defined state describing a block of physical memory.
 * We reference these as the backing memory for the allocation.
 */
typedef struct RM_POOL_ALLOC_MEMDESC            RM_POOL_ALLOC_MEMDESC;

/* ------------------------------- Public Interface ----------------------------- */

/*!
 * @brief Sets up the memory pool and the tracking structure.
 *
 * @param[in] pCtx              Pointer to some user context data
 * @param[in] ppMemReserveInfo  Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] configMode        Mode to configure the pool
 *
 * @return
 *      NV_OK:
 *          Internal memory allocation failed.
 */
NV_STATUS rmMemPoolSetup(void *pCtx, RM_POOL_ALLOC_MEM_RESERVE_INFO **ppMemReserve, POOL_CONFIG_MODE configMode);

/*!
 * @brief Reserve memory for the allocation in vidmem. Physical frames are
 *        added to the memory pool on need basis only when a mapping is
 *        made in the VAS.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] poolSize        pool size
 * @param[in] flags           VASpace flags to skip scrubbing in PMA for internal clients
 *
 * @return
 *      NV_ERR_NO_MEMORY:
 *          Internal memory allocation failed
 *      NV_ERR_INVALID_ARGUMENT:
 *          Invalid argument
 */
NV_STATUS rmMemPoolReserve(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserve,
                               NvU64 poolSize, NvU32 flags);

/*!
 * @brief Releases the memory pool memory to the PMA once all allocations are
 *        returned back to it.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] flags           VASpace flags to skip scrubbing in PMA for internal clients
 *
 * @return
 */
void      rmMemPoolRelease(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo, NvU32 flags);

/*!
 * @brief Returns any unused nodes from the topmost level of a pool hierarchy
 *        back to PMA.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] nodesToPreserve Number of nodes to preserve in the topmost pool
 * @param[in] flags           VASpace flags to skip scrubbing in PMA for internal clients

 * @return
 */
void      rmMemPoolTrim (RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
                             NvU32 nodesToPreserve, NvU32 flags);

/*!
 * @brief Suballocate memory for an allocation from the pool created
 *        by @see rmMemPoolReserve.
 *
 * @param[in] pMemReserveInfo    Pointer to RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] pMemDesc           Pointer to the allocations mem descriptor
 *
 * @return
 *      NV_ERR_NO_MEMORY:
 *          Internal memory allocation failed.
 *      NV_ERR_GENERIC:
 *          Unexpected error.
 */
NV_STATUS rmMemPoolAllocate(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
                                RM_POOL_ALLOC_MEMDESC *pPoolMemDesc);

/*!
 * @brief Returns the allocation's memory back to the pool from
 *        which it was borrowed.
 *
 * @param[in] pMemReserveInfo    Pointer to RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] pMemDesc           Pointer to the allocations mem descriptor
 * @param[in] flags              VASpace flags to skip scrubbing in PMA for internal clients
 *
 * @return
 */
void      rmMemPoolFree(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
                            RM_POOL_ALLOC_MEMDESC *pPoolMemDesc, NvU32 flags);

/*!
 * @brief Destroys the memory pool once all allocations are returned
 *        back to it.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 *
 * @return
 */
void      rmMemPoolDestroy(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo);

/*!
 * @brief Setup pool to skip scrubber.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] bSkipScrub      skip scrubber
 *
 * @return
 */
void      rmMemPoolSkipScrub(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo, NvBool bSkipScrub);

/*!
 * @brief Get pool setting for skipping scrubber.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 *
 * @return
 *      NV_TRUE  Scrubbing is skipped
 *      NV_FALSE Scrubbing not skipped
 */
NvBool    rmMemPoolIsScrubSkipped(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo);

/*!
 * @brief Get page size and chunk size for a pool
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[out] chunkSize
 * @param[out] pageSize
 *
 * @return
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_OK
 */
NV_STATUS      rmMemPoolGetChunkAndPageSize(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo, NvU64*, NvU64*);

/*!
 * @brief Indicate that pool should be allocated in protected video memory in
 *        case memory protection is enabled
 *
 * @param[in] pMemReserveInfo  Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] bProtected       Allocate in protected memory
 *
 * @return
 */
void           rmMemPoolAllocateProtectedMemory(RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo, NvBool bProtected);

#endif //_RM_POOL_ALLOC_
