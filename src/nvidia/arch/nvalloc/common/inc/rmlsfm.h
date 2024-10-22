/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RMLSFM_H
#define RMLSFM_H

/*****************************************************************************/
/*             This file is shared between ACR, SEC2 Binaries                */
/*             Do not update this file without updating ACR/SEC2             */
/*****************************************************************************/

/*!
 * @file   rmlsfm.h
 * @brief  Top-level header-file that defines Light Secure Falcon Managment
           SW shared interfaces.
 */

/*!
 * READ/WRITE masks for WPR region
 */
#define LSF_WPR_REGION_RMASK                    (0xCU) // Readable only from level 2 and 3 client
#define LSF_WPR_REGION_WMASK                    (0xCU) // Writable only from level 2 and 3 client
#define LSF_WPR_REGION_RMASK_SUB_WPR_ENABLED    (0x8) // Readable only from level 3 client
#define LSF_WPR_REGION_WMASK_SUB_WPR_ENABLED    (0x8) // Writable only from level 3 client
#define LSF_WPR_REGION_ALLOW_READ_MISMATCH_NO   (0x0) // Disallow read mis-match for all clients
#define LSF_WPR_REGION_ALLOW_WRITE_MISMATCH_NO  (0x0) // Disallow write mis-match for all clients

/*!
 * READ mask for WPR region on Tegra
 * This is required until we update tegra binaries, Bug 200281517
 * TODO: dgoyal - Remove this once tegra binaries are updated
 */
#define LSF_WPR_REGION_RMASK_FOR_TEGRA   (0xFU)

/*!
 * Expected REGION ID to be used for the unprotected FB region (region that
 * does not have read or write protections)
 */
#define LSF_UNPROTECTED_REGION_ID   (0x0U)

/*!
 * Expected REGION ID to be used for the WPR region for the falcon microcode (includes data).
 * ACR allots client requests to each region based on read/write masks and it is supposed
 * to provide first priority to requests from LSFM. Providing first priority will naturally assign
 * region ID 1 to LSFM and this define will provide a way for different parties to sanity check
 * this fact. Also there are other falcons (FECS/video falcons) which depends on this define, so please
 * be aware while modifying this.
 */
#define LSF_WPR_EXPECTED_REGION_ID  (0x1U)

/*!
 * Expected REGION ID to be used for the unused WPR region.
 */
#define LSF_WPR_UNUSED_REGION_ID    (0x2U)

/*!
 * Invalid LS falcon subWpr ID
 */
#define LSF_SUB_WPR_ID_INVALID      (0xFFFFFFFFU)

/*!
 * Expected REGION ID to be used for the VPR region.
 */
#define LSF_VPR_REGION_ID           (0x3U)

/*!
 * Expected REGION ID to be used for the CPR region with conf compute.
 */
#define LSF_CPR_REGION_ID           (0x3U)

/*!
 * Size of the separate bootloader data that could be present in WPR region.
 */
#define LSF_LS_BLDATA_EXPECTED_SIZE (0x100U)

/*!
 * since we dont check signatures in GC6 exit, we need to hardcode the WPR offset
 */
#define LSF_WPR_EXPECTED_OFFSET     (0x0U)

/*!
 * CTXDMA to be used while loading code/data in target falcons
 */
#define LSF_BOOTSTRAP_CTX_DMA_FECS                (0x0)

/*!
 * Context DMA ID 6 is reserved for Video UCODE
 */
#define LSF_BOOTSTRAP_CTX_DMA_VIDEO               (0x6)
#define LSF_BOOTSTRAP_CTX_DMA_BOOTSTRAP_OWNER     (0x0)
#define LSF_BOOTSTRAP_CTX_DMA_FBFLCN              (0x0)

/*!
 * Falcon Id Defines
 * Defines a common Light Secure Falcon identifier.
 * Codesigning infra. assumes LSF_FALCON_ID_ prefix for units,
 * Changes to the define needs to be reflected in path [1]
 * For new Falcon Id adding, we need to append to the end;
 * don't insert the new falcon Id in the middle.
 *
 * @note If a newly added Falcon has multiple instances sharing
 * the same Falcon Id, the LSF_FALCON_USES_INSTANCE macro
 * need to be updated.
 */
#define LSF_FALCON_ID_PMU               (0U)
#define LSF_FALCON_ID_DPU               (1U)
#define LSF_FALCON_ID_GSPLITE           LSF_FALCON_ID_DPU
#define LSF_FALCON_ID_FECS              (2U)
#define LSF_FALCON_ID_GPCCS             (3U)
#define LSF_FALCON_ID_NVDEC             (4U)
#define LSF_FALCON_ID_NVENC             (5U)
#define LSF_FALCON_ID_NVENC0            (5U)
#define LSF_FALCON_ID_NVENC1            (6U)
#define LSF_FALCON_ID_SEC2              (7U)
#define LSF_FALCON_ID_NVENC2            (8U)
#define LSF_FALCON_ID_MINION            (9U)
#define LSF_FALCON_ID_FBFALCON          (10U)
#define LSF_FALCON_ID_XUSB              (11U)
#define LSF_FALCON_ID_GSP_RISCV         (12U)
#define LSF_FALCON_ID_PMU_RISCV         (13U)
#define LSF_FALCON_ID_SOE               (14U)
#define LSF_FALCON_ID_NVDEC1            (15U)
#define LSF_FALCON_ID_OFA               (16U)
#define LSF_FALCON_ID_SEC2_RISCV        (17U)
#define LSF_FALCON_ID_NVDEC_RISCV       (18U)
#define LSF_FALCON_ID_NVDEC_RISCV_EB    (19U)
#define LSF_FALCON_ID_NVJPG             (20U)
#define LSF_FALCON_ID_FECS_RISCV        (21U)
#define LSF_FALCON_ID_GPCCS_RISCV       (22U)
#define LSF_FALCON_ID_NVJPG_RISCV_EB    (23U)
#define LSF_FALCON_ID_OFA_RISCV_EB      (24U)
#define LSF_FALCON_ID_NVENC_RISCV_EB    (25U)
#define LSF_FALCON_ID_PMU_RISCV_EB      (26U)
#define LSF_FALCON_ID_NVDEC0_RISCV_EB   (27U)
#define LSF_FALCON_ID_GSPLITE_RISCV_EB  (28U)
#define LSF_FALCON_ID_DISPLAY_RISCV_EB  (29U)
#define LSF_FALCON_ID_FBFALCON_RISCV_EB (30U)
#define LSF_FALCON_ID_END               (31U)

#define LSF_FALCON_ID_INVALID   (0xFFFFFFFFU)

//
// TODO: Remove below Alias and add _EB Patching to macro LSF_FALCON_ID_FECS_RISCV, similarly for GPCCS,
// and similar cleanups in RM since RISCV based CTXSW engines are to be booted externally.
// Tracking in Bug 3808599
//
#define LSF_FALCON_ID_FECS_RISCV_EB    (LSF_FALCON_ID_FECS_RISCV)
#define LSF_FALCON_ID_GPCCS_RISCV_EB   (LSF_FALCON_ID_GPCCS_RISCV)

//
// ************************ NOTIFICATION *********************************
// In case anyone needs to add new LSF falconId, please must calculate
// WPR header size per LSF_FALCON_ID_END. RM needs to call lsfmGetWprHeaderSizeMax_HAL
// to align with acrReadSubWprHeader_HAL in ACR. Otherwise, ACR can't get correct
// address to read sub wpr headers.
// We observer in case LSF_FALCON_ID_END > 32 will cause  SEC2 IMEM tag missing error;
// but don't get the root cause yet.
//
#define LSF_FALCON_ID_END_15           (15U)
#define LSF_FALCON_ID_END_17           (17U)
#define LSF_FALCON_ID_END_18           (18U)
#define LSF_FALCON_ID_END_21           (21U)
#define LSF_FALCON_ID_END_30           (30U)

#define LSF_FALCON_INSTANCE_DEFAULT_0           (0x0U)
#define LSF_FALCON_INSTANCE_COUNT_DEFAULT_1     (0x1U)

// Currently max supported instance is 8 for FECS/GPCCS SMC
#define LSF_FALCON_INSTANCE_FECS_GPCCS_MAX      (0x8U)
#define LSF_FALCON_INSTANCE_INVALID             (0xFFFFFFFFU)
#define LSF_FALCON_INDEX_MASK_DEFAULT_0         (0x0U)

/*!
 * Checks if the LSF Falcon specified by falconId uses a falconInstance to uniquely identify itself.
 * Some Falcons (eg: NVENC) use separate FalconId for each instance while some (eg: NVJPG)
 * shares the same falconId across all instances of that engine. Those engines require a falconInstance
 * to uniquely identify it.
 * @note this macro should be updated as needed whenever LSF_FALCON_ID* defines are added. See Bug: 3833461
 */
#define LSF_FALCON_USES_INSTANCE(falconId)  ((falconId == LSF_FALCON_ID_NVDEC_RISCV_EB)   ||  \
                                             (falconId == LSF_FALCON_ID_NVJPG)            ||  \
                                             (falconId == LSF_FALCON_ID_NVJPG_RISCV_EB)   ||  \
                                             (falconId == LSF_FALCON_ID_NVENC_RISCV_EB)   ||  \
                                             (falconId == LSF_FALCON_ID_FECS_RISCV_EB)    ||  \
                                             (falconId == LSF_FALCON_ID_GPCCS_RISCV_EB)   ||  \
                                             (falconId == LSF_FALCON_ID_GSPLITE_RISCV_EB) ||  \
                                             (falconId == LSF_FALCON_ID_OFA_RISCV_EB))


/*!
 * Size in entries of the ucode descriptor's dependency map.
 * This used to be LSF_FALCON_ID_END, but since that had to grow and we did not want to break any
 * existing binaries, they had to be split.
 *
 * Increasing this number should be done with care.
 */
#define LSF_FALCON_DEPMAP_SIZE  (11U)

/*!
 * Falcon Binaries version defines
 */
#define LSF_FALCON_BIN_VERSION_INVALID (0xFFFFFFFFU)


/*!
 * Light Secure Falcon Ucode Description Defines
 * This stucture is prelim and may change as the ucode signing flow evolves.
 */
typedef struct
{
    NvU8  prdKeys[2][16];
    NvU8  dbgKeys[2][16];
    NvU32 bPrdPresent;
    NvU32 bDbgPresent;
    NvU32 falconId;
    NvU32 bSupportsVersioning;
    NvU32 version;
    NvU32 depMapCount;
    NvU8  depMap[LSF_FALCON_DEPMAP_SIZE * 2 * 4];
    NvU8  kdf[16];
} LSF_UCODE_DESC, *PLSF_UCODE_DESC;

/*!
 * Light Secure WPR Header
 * Defines state allowing Light Secure Falcon bootstrapping.
 *
 * falconId       - LS falcon ID
 * lsbOffset      - Offset into WPR region holding LSB header
 * bootstrapOwner - Bootstrap OWNER (either PMU or SEC2)
 * bLazyBootstrap - Skip bootstrapping by ACR
 * status         - Bootstrapping status
 */
typedef struct
{
    NvU32  falconId;
    NvU32  lsbOffset;
    NvU32  bootstrapOwner;
    NvU32  bLazyBootstrap;
    NvU32  binVersion;
    NvU32  status;
} LSF_WPR_HEADER, *PLSF_WPR_HEADER;

/*!
 * LSF shared SubWpr Header
 *
 * useCaseId  - Shared SubWpr se case ID (updated by RM)
 * startAddr  - start address of subWpr (updated by RM)
 * size4K     - size of subWpr in 4K (updated by RM)
 */
typedef struct
{
    NvU32 useCaseId;
    NvU32 startAddr;
    NvU32 size4K;
} LSF_SHARED_SUB_WPR_HEADER, *PLSF_SHARED_SUB_WPR_HEADER;

// Shared SubWpr use case IDs
typedef enum
{
    LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_FRTS_VBIOS_TABLES     = 1,
    LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_PLAYREADY_SHARED_DATA = 2,
    LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_APM_RTS               = 3
} LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_ENUM;

#define LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_MAX         LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_APM_RTS
#define LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_INVALID     (0xFFFFFFFFU)

#define MAX_SUPPORTED_SHARED_SUB_WPR_USE_CASES          LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_MAX

//
// Static sizes of shared subWPRs
// Minimum granularity supported is 4K
//
#define LSF_SHARED_DATA_SUB_WPR_FRTS_VBIOS_TABLES_SIZE_IN_4K             (0x100)   // 1MB in 4K
#define LSF_SHARED_DATA_SUB_WPR_PLAYREADY_SHARED_DATA_SIZE_IN_4K         (0x1)     // 4K
#define LSF_SHARED_DATA_SUB_WPR_APM_RTS_SIZE_IN_4K                       (0x1)     // 4K

/*!
 * Bootstrap Owner Defines
 */
#define LSF_BOOTSTRAP_OWNER_PMU     (LSF_FALCON_ID_PMU)
#define LSF_BOOTSTRAP_OWNER_SEC2    (LSF_FALCON_ID_SEC2)
#define LSF_BOOTSTRAP_OWNER_GSPLITE (LSF_FALCON_ID_GSPLITE)
#define LSF_BOOTSTRAP_OWNER_DEFAULT LSF_BOOTSTRAP_OWNER_PMU

/*!
 * Image Status Defines
 */
#define LSF_IMAGE_STATUS_NONE                           (0U)
#define LSF_IMAGE_STATUS_COPY                           (1U)
#define LSF_IMAGE_STATUS_VALIDATION_CODE_FAILED         (2U)
#define LSF_IMAGE_STATUS_VALIDATION_DATA_FAILED         (3U)
#define LSF_IMAGE_STATUS_VALIDATION_DONE                (4U)
#define LSF_IMAGE_STATUS_VALIDATION_SKIPPED             (5U)
#define LSF_IMAGE_STATUS_BOOTSTRAP_READY                (6U)
#define LSF_IMAGE_STATUS_REVOCATION_CHECK_FAILED        (7U)

/*!
 * Light Secure Bootstrap Header
 * Defines state allowing Light Secure Falcon bootstrapping.
 *
 * signature     - Code/data signature details for this LS falcon
 * ucodeOffset   - Offset into WPR region where UCODE is located
 * ucodeSize     - Size of ucode
 * dataSize      - Size of ucode data
 * blCodeSize    - Size of bootloader that needs to be loaded by bootstrap owner
 * blImemOffset  - BL starting virtual address. Need for tagging.
 * blDataOffset  - Offset into WPR region holding the BL data
 * blDataSize    - Size of BL data
 * appCodeOffset - Offset into WPR region where Application UCODE is located
 * appCodeSize   - Size of Application UCODE
 * appDataOffset - Offset into WPR region where Application DATA is located
 * appDataSize   - Size of Application DATA
 * blLoadCodeAt0 - Load BL at 0th IMEM offset
 * bSetVACtx     - Make sure to set the code/data loading CTX DMA to be virtual before exiting
 * bDmaReqCtx    - This falcon requires a ctx before issuing DMAs
 * bForcePrivLoad- Use priv loading method instead of bootloader/DMAs
 */

#define NV_FLCN_ACR_LSF_FLAG_LOAD_CODE_AT_0             0:0
#define NV_FLCN_ACR_LSF_FLAG_LOAD_CODE_AT_0_FALSE       0
#define NV_FLCN_ACR_LSF_FLAG_LOAD_CODE_AT_0_TRUE        1
#define NV_FLCN_ACR_LSF_FLAG_SET_VA_CTX                 1:1
#define NV_FLCN_ACR_LSF_FLAG_SET_VA_CTX_FALSE           0
#define NV_FLCN_ACR_LSF_FLAG_SET_VA_CTX_TRUE            1
#define NV_FLCN_ACR_LSF_FLAG_DMACTL_REQ_CTX             2:2
#define NV_FLCN_ACR_LSF_FLAG_DMACTL_REQ_CTX_FALSE       0
#define NV_FLCN_ACR_LSF_FLAG_DMACTL_REQ_CTX_TRUE        1
#define NV_FLCN_ACR_LSF_FLAG_FORCE_PRIV_LOAD            3:3
#define NV_FLCN_ACR_LSF_FLAG_FORCE_PRIV_LOAD_FALSE      0
#define NV_FLCN_ACR_LSF_FLAG_FORCE_PRIV_LOAD_TRUE       1
typedef struct
{
    LSF_UCODE_DESC signature;
    NvU32 ucodeOffset;
    NvU32 ucodeSize;
    NvU32 dataSize;
    NvU32 blCodeSize;
    NvU32 blImemOffset;
    NvU32 blDataOffset;
    NvU32 blDataSize;
    NvU32 appCodeOffset;
    NvU32 appCodeSize;
    NvU32 appDataOffset;
    NvU32 appDataSize;
    NvU32 flags;
    /*
     * TODO: Uncomment this once Sanket's changes
     * of HALifying is done
    NvU32 monitorCodeOffset;
    NvU32 monitorDataOffset;
    NvU32 manifestOffset;
    */
} LSF_LSB_HEADER, *PLSF_LSB_HEADER;


/*!
 * Light Secure WPR Content Alignments
 */
#define LSF_WPR_HEADER_ALIGNMENT        (256U)
#define LSF_SUB_WPR_HEADER_ALIGNMENT    (256U)
#define LSF_LSB_HEADER_ALIGNMENT        (256U)
#define LSF_BL_DATA_ALIGNMENT           (256U)
#define LSF_BL_DATA_SIZE_ALIGNMENT      (256U)
#define LSF_BL_CODE_SIZE_ALIGNMENT      (256U)
#define LSF_DATA_SIZE_ALIGNMENT         (256U)
#define LSF_CODE_SIZE_ALIGNMENT         (256U)

// MMU excepts subWpr sizes in units of 4K
#define SUB_WPR_SIZE_ALIGNMENT          (4096U)

/*!
 * Maximum WPR Header size
 */
#define LSF_WPR_HEADERS_TOTAL_SIZE_MAX      (NV_ALIGN_UP((sizeof(LSF_WPR_HEADER) * LSF_FALCON_ID_END), LSF_WPR_HEADER_ALIGNMENT))
#define LSF_LSB_HEADER_TOTAL_SIZE_MAX       (NV_ALIGN_UP(sizeof(LSF_LSB_HEADER), LSF_LSB_HEADER_ALIGNMENT))

//
// PMU OBJACR_ALIGNED_256 size will vary with LSF_FALCON_ID_END.
// PMU could run out of DMEM in case we increase LSF_FALCON_ID_END more and more.
// The PMU supports the ACR task on GM20X_thru_VOLTA profiles only.
// In order to prevent LSF_FALCON_ID_END changes to affect older / shipped PMU ucodes (increase of DMEM footprint)
// adding PMU specific ***_END define capturing value covering all PMU profiles that this with the ACR task.
//
#define LSF_FALCON_ID_END_PMU               (LSF_FALCON_ID_FBFALCON + 1)
#define LSF_WPR_HEADERS_TOTAL_SIZE_MAX_PMU  (NV_ALIGN_UP((sizeof(LSF_WPR_HEADER) * LSF_FALCON_ID_END_PMU), LSF_WPR_HEADER_ALIGNMENT))

//
// In order to prevent LSF_FALCON_ID_END changes to affect older / shipped SEC2/ACR ucodes (increase of DMEM footprint)
// adding SEC2/ACR specific ***_END define covering all supported falcons in pre-hopper SEC2-RTOS/ACR ucode.
//
#define LSF_FALCON_ID_END_ACR_ON_SEC2               (LSF_FALCON_ID_NVJPG + 1)
#define LSF_WPR_HEADERS_TOTAL_SIZE_MAX_ACR_ON_SEC2  (NV_ALIGN_UP((sizeof(LSF_WPR_HEADER) * LSF_FALCON_ID_END_ACR_ON_SEC2), LSF_WPR_HEADER_ALIGNMENT))

// Maximum SUB WPR header size
#define LSF_SUB_WPR_HEADERS_TOTAL_SIZE_MAX  (NV_ALIGN_UP((sizeof(LSF_SHARED_SUB_WPR_HEADER) * LSF_SHARED_DATA_SUB_WPR_USE_CASE_ID_MAX), LSF_SUB_WPR_HEADER_ALIGNMENT))

/*!
 * For the ucode surface alignment, We align to RM_PAGE_SIZE because of
 * busMapRmAperture issues, not because of Falcon ucode alignment requirements
 * which currently are that it be at least 256.
 */
#define LSF_UCODE_DATA_ALIGNMENT RM_PAGE_SIZE

/*!
 * ACR Descriptors used by ACR UC
 */

/*!
 * Supporting maximum of 2 regions.
 * This is needed to pre-allocate space in DMEM
 */
#define RM_FLCN_ACR_MAX_REGIONS                  (2)
#define LSF_BOOTSTRAP_OWNER_RESERVED_DMEM_SIZE   (0x200)

/*!
 * startAddress   - Starting address of region
 * endAddress     - Ending address of region
 * regionID       - Region ID
 * readMask       - Read Mask
 * writeMask      - WriteMask
 * clientMask     - Bit map of all clients currently using this region
 * shadowMemStartAddress- FB location from where contents need to be copied to startAddress
 */
typedef struct _def_acr_dmem_region_prop
{
    NvU32   startAddress;
    NvU32   endAddress;
    NvU32   regionID;
    NvU32   readMask;
    NvU32   writeMask;
    NvU32   clientMask;
    NvU32   shadowMemStartAddress;
} RM_FLCN_ACR_REGION_PROP, *PRM_FLCN_ACR_REGION_PROP;


/*!
 * noOfRegions   - Number of regions used by RM.
 * regionProps   - Region properties
 */
typedef struct _def_acr_regions
{
    NvU32                     noOfRegions;
    RM_FLCN_ACR_REGION_PROP   regionProps[RM_FLCN_ACR_MAX_REGIONS];
} RM_FLCN_ACR_REGIONS, *PRM_FLCN_ACR_REGIONS;

/*!
 * bVprEnabled      : When set, ACR_LOCKDOWN phase programs VPR range. Needs to be
                    : NvU32 because of alignment
 * vprStartAddress  : Start address of VPR region. SEC2 binary updates this value
 * vprEndAddress    : End address of VPR region. SEC2 binary updates this value
 * hdcpPolicies     : VPR display policies. SEC2 binary updates this value
 */
typedef  struct _def_acr_vpr_dmem_desc
{
    NvU32 bVprEnabled;
    NvU32 vprStartAddress;
    NvU32 vprEndAddress;
    NvU32 hdcpPolicies;
} ACR_BSI_VPR_DESC, *PACR_BSI_VPR_DESC;

/*!
 * reservedDmem - When the bootstrap owner has done bootstrapping other falcons,
 *                and need to switch into LS mode, it needs to have its own actual
 *                DMEM image copied into DMEM as part of LS setup. If ACR desc is at
 *                location 0, it will definitely get overwritten causing data corruption.
 *                Hence we are reserving 0x200 bytes to give room for any loading data.
 *                NOTE: This has to be the first member always
 * signature    - Signature of ACR ucode.
 * wprRegionID  - Region ID holding the WPR header and its details
 * wprOffset    - Offset from the WPR region holding the wpr header
 * regions      - Region descriptors
 * ucodeBlobBase- Used for Tegra, stores non-WPR start address where kernel stores ucode blob
 * ucodeBlobSize- Used for Tegra, stores the size of the ucode blob
 */
typedef struct _def_acr_dmem_desc
{
    NvU32                signatures[4];
    NvU32                wprRegionID;
    NvU32                wprOffset;
    NvU32                mmuMemoryRange;
    RM_FLCN_ACR_REGIONS  regions;
    NvU32                ucodeBlobSize;
    // uCodeBlobBase is moved after ucodeBlobSize to inherently align to qword (8 bytes)
    NvU64                NV_DECLARE_ALIGNED(ucodeBlobBase, 8);

    /*!
     * Do not change the offset of this descriptor as it shared between
     * ACR_REGION_LOCKDOWN HS binary and SEC2. Any change in this structure
     * need recompilation of SEC2 and ACR_LOCKDOWN HS binary
     */
    ACR_BSI_VPR_DESC     vprDesc;
} RM_FLCN_ACR_DESC, *PRM_FLCN_ACR_DESC;

/*!
* Hub keys/nonce Structure in BSI
*/
#define MAX_SFBHUB_ENCRYPTION_REGION_KEY_SIZE 4

typedef struct _def_acr_hub_scratch_data
{
    NvU32    key[MAX_SFBHUB_ENCRYPTION_REGION_KEY_SIZE];
    NvU32    nonce[MAX_SFBHUB_ENCRYPTION_REGION_KEY_SIZE];
} ACR_BSI_HUB_DESC, *PACR_BSI_HUB_DESC;

#define MAX_HUB_ENCRYPTION_REGION_COUNT 3
typedef struct _def_acr_hub_scratch_array
{
    ACR_BSI_HUB_DESC entries[MAX_HUB_ENCRYPTION_REGION_COUNT];
} ACR_BSI_HUB_DESC_ARRAY, *PACR_BSI_HUB_DESC_ARRAY;

typedef struct _def_acr_reserved_dmem
{
    NvU32            reservedDmem[(LSF_BOOTSTRAP_OWNER_RESERVED_DMEM_SIZE/4)];  // Always first..
} ACR_RESERVED_DMEM, *PACR_RESERVED_DMEM;

typedef struct _def_booter_reserved_dmem
{
    NvU32            reservedDmem[(LSF_BOOTSTRAP_OWNER_RESERVED_DMEM_SIZE/4)];  // Always first..
} BOOTER_RESERVED_DMEM;

#define NV_FLCN_ACR_DESC_FLAGS_SIG_VERIF           0:0
#define NV_FLCN_ACR_DESC_FLAGS_SIG_VERIF_DISABLE   0
#define NV_FLCN_ACR_DESC_FLAGS_SIG_VERIF_ENABLE    1

// Macro defines to be consumed by RM to get GH100 GSP Inst_in_sys FMC boot status.
#define GSP_INST_IN_SYS_COMPLETION_STATUS_OK          0x55
#define GSP_INST_IN_SYS_COMPLETION_STATUS_ERROR       0xAA
#define GSP_INST_IN_SYS_COMPLETION_STATUS_IN_PROGRESS 0x00

/*!
 * Size of ACR phase in dword
 */
#define ACR_PHASE_SIZE_DWORD                       sizeof(RM_FLCN_ACR_DESC)/sizeof(NvU32)

/*!
 * Falcon Mode Tokens
 * This is the value logged to a mailbox register to indicate that the
 * falcon isn't booted in secure mode.
 */
#define LSF_FALCON_MODE_TOKEN_FLCN_INSECURE   (0xDEADDEADU)


// LS encryption context, to store data to decrypt LS images.
#define LS_ENCRYPTION_AES128_CBC_IV_SIZE_BYTE (16)

typedef struct
{
    NvU8   bLsEncrypted;
    NvU8   rsvd[3];
    NvU8   lsEncIV[LS_ENCRYPTION_AES128_CBC_IV_SIZE_BYTE];
} LSF_ENCRYPTION_CONTEXT, *PLSF_ENCRYPTION_CONTEXT;

#endif // RMLSFM_H

