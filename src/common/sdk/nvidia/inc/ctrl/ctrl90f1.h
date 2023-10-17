/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl90f1.finn
//

#include "ctrl/ctrlxxxx.h"
#include "mmu_fmt_types.h"
#include "nvcfg_sdk.h"

#define GMMU_FMT_MAX_LEVELS  6U

/* Fermi+ GPU VASpace control commands and parameters */
#define NV90F1_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x90F1, NV90F1_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NV90F1_CTRL_RESERVED (0x00U)
#define NV90F1_CTRL_VASPACE  (0x01U)

/*!
 * Does nothing.
 */
#define NV90F1_CTRL_CMD_NULL (0x90f10000U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*!
 * Get VAS GPU MMU format.
 */
#define NV90F1_CTRL_CMD_VASPACE_GET_GMMU_FORMAT (0x90f10101U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS_MESSAGE_ID" */

#define NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS {
    /*!
     * [in] GPU sub-device handle - this API only supports unicast.
     *      Pass 0 to use subDeviceId instead.
     */
    NvHandle hSubDevice;

    /*!
     * [in] GPU sub-device ID. Ignored if hSubDevice is non-zero.
     */
    NvU32    subDeviceId;

    /*!
     * [out] GMMU format struct. This is of RM-internal type "struct GMMU_FMT*"
     *       which can only be accessed by kernel builds since this is a kernel
     *       only API.
     */
    NV_DECLARE_ALIGNED(NvP64 pFmt, 8);
} NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS;

/*!
 * Get VAS page level information.
 */
#define NV90F1_CTRL_CMD_VASPACE_GET_PAGE_LEVEL_INFO (0x90f10102U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS_MESSAGE_ID" */

typedef struct NV_CTRL_VASPACE_PAGE_LEVEL {
    /*!
     * Format of this level.
     */
    NV_DECLARE_ALIGNED(struct MMU_FMT_LEVEL *pFmt, 8);

    /*!
     * Level/Sublevel Formats flattened
     */
    NV_DECLARE_ALIGNED(MMU_FMT_LEVEL levelFmt, 8);
    NV_DECLARE_ALIGNED(MMU_FMT_LEVEL sublevelFmt[MMU_FMT_MAX_SUB_LEVELS], 8);

    /*!
     * Physical address of this page level instance.
     */
    NV_DECLARE_ALIGNED(NvU64 physAddress, 8);

    /*!
     * Aperture in which this page level instance resides.
     */
    NvU32 aperture;

    /*!
     * Size in bytes allocated for this level instance.
     */
    NV_DECLARE_ALIGNED(NvU64 size, 8);

    /*!
     * Entry Index for this offset.
     */
    NvU32 entryIndex;
} NV_CTRL_VASPACE_PAGE_LEVEL;

#define NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS {
    /*!
     * [in] GPU sub-device handle - this API only supports unicast.
     *      Pass 0 to use subDeviceId instead.
     */
    NvHandle hSubDevice;

    /*!
     * [in] GPU sub-device ID. Ignored if hSubDevice is non-zero.
     */
    NvU32    subDeviceId;

    /*!
     * [in] GPU virtual address to query.
     */
    NV_DECLARE_ALIGNED(NvU64 virtAddress, 8);

    /*!
     * [in] Page size to query.
     */
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);

    /*!
     * [in] Flags
     * Contains flags to control various aspects of page level info.
     */
    NV_DECLARE_ALIGNED(NvU64 flags, 8);

    /*!
     * [out] Number of levels populated.
     */
    NvU32    numLevels;

    /*!
     * [out] Per-level information.
     */
    NV_DECLARE_ALIGNED(NV_CTRL_VASPACE_PAGE_LEVEL levels[GMMU_FMT_MAX_LEVELS], 8);
} NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS;

/* valid flags parameter values */
#define NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_FLAG_NONE 0x0ULL
#define NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_FLAG_BAR1 NVBIT64(0)


/*!
 * Reserve (allocate and bind) page directory/table entries up to
 * a given level of the MMU format. Also referred to as "lock-down".
 *
 * Each range that has been reserved must be released
 * eventually with @ref NV90F1_CTRL_CMD_VASPACE_RELEASE_ENTRIES.
 * A particular VA range and level (page size) combination may only be
 * locked down once at a given time, but each level is independent.
 */
#define NV90F1_CTRL_CMD_VASPACE_RESERVE_ENTRIES           (0x90f10103U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS_MESSAGE_ID" */

#define NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS {
    /*!
     * [in] GPU sub-device handle - this API only supports unicast.
     *      Pass 0 to use subDeviceId instead.
     */
    NvHandle hSubDevice;

    /*!
     * [in] GPU sub-device ID. Ignored if hSubDevice is non-zero.
     */
    NvU32    subDeviceId;

    /*!
     * [in] Page size (VA coverage) of the level to reserve.
     *      This need not be a leaf (page table) page size - it can be
     *      the coverage of an arbitrary level (including root page directory).
     */
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);

    /*!
     * [in] First GPU virtual address of the range to reserve.
     *      This must be aligned to pageSize.
     */
    NV_DECLARE_ALIGNED(NvU64 virtAddrLo, 8);

    /*!
     * [in] Last GPU virtual address of the range to reserve.
     *      This (+1) must be aligned to pageSize.
     */
    NV_DECLARE_ALIGNED(NvU64 virtAddrHi, 8);
} NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS;

/*!
 * Release (unbind and free) page directory/table entries up to
 * a given level of the MMU format that has been reserved through a call to
 * @ref NV90F1_CTRL_CMD_VASPACE_RESERVE_ENTRIES. Also referred to as "unlock".
 */
#define NV90F1_CTRL_CMD_VASPACE_RELEASE_ENTRIES (0x90f10104U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS_MESSAGE_ID" */

#define NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS {
    /*!
     * [in] GPU sub-device handle - this API only supports unicast.
     *      Pass 0 to use subDeviceId instead.
     */
    NvHandle hSubDevice;

    /*!
     * [in] GPU sub-device ID. Ignored if hSubDevice is non-zero.
     */
    NvU32    subDeviceId;

    /*!
     * [in] Page size (VA coverage) of the level to release.
     *      This need not be a leaf (page table) page size - it can be
     *      the coverage of an arbitrary level (including root page directory).
     */
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);

    /*!
     * [in] First GPU virtual address of the range to release.
     *      This must be aligned to pageSize.
     */
    NV_DECLARE_ALIGNED(NvU64 virtAddrLo, 8);

    /*!
     * [in] Last GPU virtual address of the range to release.
     *      This (+1) must be aligned to pageSize.
     */
    NV_DECLARE_ALIGNED(NvU64 virtAddrHi, 8);
} NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS;

/*!
 * Get VAS page level information without kernel priviledge. This will internally call
 * NV90F1_CTRL_CMD_VASPACE_GET_PAGE_LEVEL_INFO.
 */
#define NV90F1_CTRL_CMD_VASPACE_GET_PAGE_LEVEL_INFO_VERIF (0x90f10105U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_VERIF_PARAMS_MESSAGE_ID" */

#define NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_VERIF_PARAMS_MESSAGE_ID (0x5U)

typedef NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_VERIF_PARAMS;

/*!
 * Pin PDEs for a given VA range on the server RM and then mirror the client's page 
 * directory/tables in the server. 
 *  
 * @ref
 */
#define NV90F1_CTRL_CMD_VASPACE_COPY_SERVER_RESERVED_PDES (0x90f10106U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_MESSAGE_ID" */

#define NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS {
    /*!
     * [in] GPU sub-device handle - this API only supports unicast.
     *      Pass 0 to use subDeviceId instead.
     */
    NvHandle hSubDevice;

    /*!
     * [in] GPU sub-device ID. Ignored if hSubDevice is non-zero.
     */
    NvU32    subDeviceId;

    /*!
     * [in] Page size (VA coverage) of the level to reserve.
     *      This need not be a leaf (page table) page size - it can be
     *      the coverage of an arbitrary level (including root page directory).
     */
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);

    /*!
     * [in] First GPU virtual address of the range to reserve.
     *      This must be aligned to pageSize.
     */
    NV_DECLARE_ALIGNED(NvU64 virtAddrLo, 8);

    /*!
     * [in] Last GPU virtual address of the range to reserve.
     *      This (+1) must be aligned to pageSize.
     */
    NV_DECLARE_ALIGNED(NvU64 virtAddrHi, 8);

    /*! 
     * [in] Number of PDE levels to copy.
     */
    NvU32    numLevelsToCopy;

   /*!
     * [in] Per-level information.
     */
    struct {
        /*!
         * Physical address of this page level instance.
         */
        NV_DECLARE_ALIGNED(NvU64 physAddress, 8);

        /*!
         * Size in bytes allocated for this level instance.
         */
        NV_DECLARE_ALIGNED(NvU64 size, 8);

        /*!
         * Aperture in which this page level instance resides.
         */
        NvU32 aperture;

        /*!
         * Page shift corresponding to the level
         */
        NvU8  pageShift;
    } levels[GMMU_FMT_MAX_LEVELS];
} NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS;

/*!
 * Retrieve extra VA range that RM needs to reserve from the OS
 */
#define NV90F1_CTRL_CMD_VASPACE_GET_HOST_RM_MANAGED_SIZE (0x90f10107U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS_MESSAGE_ID" */
#define NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS {
    /*!
     * [in] GPU sub-device handle - this API only supports unicast.
     *      Pass 0 to use subDeviceId instead.
     */
    NvHandle hSubDevice;

    /*!
     * [in] GPU sub-device ID. Ignored if hSubDevice is non-zero.
     */
    NvU32    subDeviceId;

    /*!
     * [out]  The required VA range, in Megabytes
     */
    NV_DECLARE_ALIGNED(NvU64 requiredVaRange, 8);
} NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS;

/*!
 * Retrieve info on a VAS heap - used only for the MODS test RandomVATest
 */
#define NV90F1_CTRL_CMD_VASPACE_GET_VAS_HEAP_INFO (0x90f10108U) /* finn: Evaluated from "(FINN_FERMI_VASPACE_A_VASPACE_INTERFACE_ID << 8) | NV90F1_CTRL_VASPACE_GET_VAS_HEAP_INFO_PARAMS_MESSAGE_ID" */
#define NV90F1_CTRL_VASPACE_GET_VAS_HEAP_INFO_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV90F1_CTRL_VASPACE_GET_VAS_HEAP_INFO_PARAMS {
    /*!
     * [in] GPU sub-device handle - this API only supports unicast.
     *      Pass 0 to use subDeviceId instead.
     */
    NvHandle hSubDevice;

    /*!
     * [in] GPU sub-device ID. Ignored if hSubDevice is non-zero.
     */
    NvU32    subDeviceId;

    /*!
     * [out] Number of free bytes in the heap
     */
    NV_DECLARE_ALIGNED(NvU64 bytesFree, 8);

    /*!
     * [out] Number of bytes in the heap
     */
    NV_DECLARE_ALIGNED(NvU64 bytesTotal, 8);

    /*!
     * [out] Offset of largest free block
     */
    NV_DECLARE_ALIGNED(NvU64 largestFreeOffset, 8);

    /*!
     * [out] Size of the largest free block
     */
    NV_DECLARE_ALIGNED(NvU64 largestFreeSize, 8);

    /*!
     * [out] Number of usable free bytes
     */
    NV_DECLARE_ALIGNED(NvU64 usableBytesFree, 8);

    /*!
     * [out] Number of free blocks
     */
    NvU32    numFreeBlocks;
} NV90F1_CTRL_VASPACE_GET_VAS_HEAP_INFO_PARAMS;

/* _ctrl90f1_h_ */
