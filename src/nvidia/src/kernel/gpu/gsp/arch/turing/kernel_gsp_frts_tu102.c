/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * KernelGsp functions and helpers for executing FWSEC ucode for FRTS.
 *
 * Note: Other than those suffixed by a chip name, functions here
 *       do not actually need to be HAL'd; we are simply keeping them all in
 *       one file to try to keep it self-contained.
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "published/turing/tu102/dev_bus.h" // for NV_PBUS_VBIOS_SCRATCH
#include "published/turing/tu102/dev_fb.h"  // for NV_PFB_PRI_MMU_WPR2_ADDR_HI
#include "published/turing/tu102/dev_gc6_island.h"
#include "published/turing/tu102/dev_gc6_island_addendum.h"

/*!
 * Get size of FRTS data.
 *
 * Currently, FRTS data size is hard-coded to be 1MB
 * (if FRTS exists for the chip).
 */
NvU32
kgspGetFrtsSize_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 sizeIn4k = NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K;
    return sizeIn4k * 0x1000;
}


// ---------------------------------------------------------------------------
// Structures and defines for FWSEC commands
// ---------------------------------------------------------------------------

typedef struct
{
    NvU8 version;
    NvU8 headerSize;
    NvU8 entrySize;
    NvU8 entryCount;
} FALCON_APPLICATION_INTERFACE_HEADER_V1;

typedef struct
{
    NvU32 id;
    NvU32 dmemOffset;
} FALCON_APPLICATION_INTERFACE_ENTRY_V1;

#define FALCON_APPLICATION_INTERFACE_ENTRY_ID_DMEMMAPPER     (0x4)

typedef struct
{
    NvU32 signature;
    NvU16 version;
    NvU16 size;
    NvU32 cmd_in_buffer_offset;
    NvU32 cmd_in_buffer_size;
    NvU32 cmd_out_buffer_offset;
    NvU32 cmd_out_buffer_size;
    NvU32 nvf_img_data_buffer_offset;
    NvU32 nvf_img_data_buffer_size;
    NvU32 printfBufferHdr;
    NvU32 ucode_build_time_stamp;
    NvU32 ucode_signature;
    NvU32 init_cmd;
    NvU32 ucode_feature;
    NvU32 ucode_cmd_mask0;
    NvU32 ucode_cmd_mask1;
    NvU32 multiTgtTbl;
} FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3;

#define FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS (0x15)
#define FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB   (0x19)

typedef struct
{
    NvU32 version;
    NvU32 size;
    NvU64 gfwImageOffset;
    NvU32 gfwImageSize;
    NvU32 flags;
} FWSECLIC_READ_VBIOS_DESC;

#define FWSECLIC_READ_VBIOS_STRUCT_FLAGS (2)

typedef struct
{
    NvU32 version;
    NvU32 size;
    NvU32 frtsRegionOffset4K;
    NvU32 frtsRegionSize;
    NvU32 frtsRegionMediaType;
} FWSECLIC_FRTS_REGION_DESC;

#define FWSECLIC_FRTS_REGION_MEDIA_FB (2)
#define FWSECLIC_FRTS_REGION_SIZE_1MB_IN_4K (0x100)

typedef struct
{
    FWSECLIC_READ_VBIOS_DESC readVbiosDesc;
    FWSECLIC_FRTS_REGION_DESC frtsRegionDesc;
} FWSECLIC_FRTS_CMD;

#define NV_VBIOS_FWSECLIC_SCRATCH_INDEX_0E      0x0E
#define NV_VBIOS_FWSECLIC_FRTS_ERR_CODE         31:16
#define NV_VBIOS_FWSECLIC_FRTS_ERR_CODE_NONE    0x00000000

#define NV_VBIOS_FWSECLIC_SCRATCH_INDEX_15      0x15
#define NV_VBIOS_FWSECLIC_SB_ERR_CODE           15:0
#define NV_VBIOS_FWSECLIC_SB_ERR_CODE_NONE      0x00000000


// ---------------------------------------------------------------------------
// Functions for preparing and executing FWSEC commands
// ---------------------------------------------------------------------------

/*!
 * Patch DMEM of FWSEC for a given command
 *
 * @param[inout]  pMappedData      Pointer to mapped DMEM of FWSEC
 * @param[in]     mappedDataSize   Number of bytes valid under pMappedData
 * @param[in]     cmd              FWSEC command to invoke
 * @param[in]     pCmdBuffer       Buffer containing command arguments to patch in
 * @param[in]     cmdBufferSize    Size of buffer pointed by pCmdBuffer
 * @param[in]     interfaceOffset  Interface offset given by VBIOS for FWSEC
 */
static NV_STATUS
s_vbiosPatchInterfaceData
(
    NvU8 *pMappedData,  // inout
    const NvU32 mappedDataSize,
    const NvU32 cmd,
    const void *pCmdBuffer,
    const NvU32 cmdBufferSize,
    const NvU32 interfaceOffset
)
{
    FALCON_APPLICATION_INTERFACE_HEADER_V1 *pIntFaceHdr = NULL;
    FALCON_APPLICATION_INTERFACE_ENTRY_V1 *pIntFaceEntry = NULL;
    FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3 *pDmemMapper = NULL;

    NvBool bSafe;
    NvU32 index;

    NvU32 curOffset;
    NvU32 nextOffset;

    if (interfaceOffset >= mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(interfaceOffset, sizeof(*pIntFaceHdr), &nextOffset);
    if (!bSafe || nextOffset > mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    pIntFaceHdr = (FALCON_APPLICATION_INTERFACE_HEADER_V1 *) (pMappedData + interfaceOffset);
    if (pIntFaceHdr->entryCount < 2)
    {
        NV_PRINTF(LEVEL_ERROR, "too few interface entires found for FWSEC cmd 0x%x\n", cmd);
        return NV_ERR_INVALID_DATA;
    }

    curOffset = nextOffset;
    for (index = 0; index < pIntFaceHdr->entryCount; index++)
    {
        if (curOffset >= mappedDataSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        bSafe = portSafeAddU32(curOffset, sizeof(*pIntFaceEntry), &nextOffset);
        if (!bSafe || nextOffset > mappedDataSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        pIntFaceEntry = (FALCON_APPLICATION_INTERFACE_ENTRY_V1 *) (pMappedData + curOffset);
        curOffset = nextOffset;

        if (pIntFaceEntry->id == FALCON_APPLICATION_INTERFACE_ENTRY_ID_DMEMMAPPER)
        {
            NvU32 dmemMapperMaxOffset;

            if (pIntFaceEntry->dmemOffset >= mappedDataSize)
            {
                return NV_ERR_INVALID_OFFSET;
            }

            bSafe = portSafeAddU32(pIntFaceEntry->dmemOffset, sizeof(*pDmemMapper),
                                   &dmemMapperMaxOffset);
            if (!bSafe || dmemMapperMaxOffset > mappedDataSize)
            {
                return NV_ERR_INVALID_OFFSET;
            }

            pDmemMapper = (FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3 *)
                                (pMappedData + pIntFaceEntry->dmemOffset);
        }
    }

    if (!pDmemMapper)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to find required interface entry for FWSEC cmd 0x%x\n", cmd);
        return NV_ERR_INVALID_DATA;
    }

    pDmemMapper->init_cmd = cmd;

    if (pDmemMapper->cmd_in_buffer_size < cmdBufferSize)
    {
        NV_PRINTF(LEVEL_ERROR, "insufficient cmd buffer for FWSEC interface cmd 0x%x\n", cmd);
    }

    if (pDmemMapper->cmd_in_buffer_offset >= mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(pIntFaceEntry->dmemOffset, cmdBufferSize, &nextOffset);
    if (!bSafe || nextOffset > mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    portMemCopy(pMappedData + pDmemMapper->cmd_in_buffer_offset, cmdBufferSize,
                pCmdBuffer, cmdBufferSize);

    return NV_OK;
}

/*!
 * Prepare to execute a given FWSEC cmd.
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelGsp     KernelGsp pointer
 * @param[in]   pFwsecUcode    KernelGspFlcnUcode structure of FWSEC ucode
 * @param[in]   cmd            FWSEC cmd (FRTS or SB)
 * @param[in]   frtsOffset     (if cmd is FRTS) desired FB offset of FRTS data
 * @param[out]  pPreparedCmd   Prepared command state to pass to kgspExecuteFwsec_TU102
 */
static NV_STATUS
s_prepareForFwsec_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode *pFwsecUcode,
    const NvU32 cmd,
    const NvU64 frtsOffset,
    KernelGspPreparedFwsecCmd *pPreparedCmd
)
{
    NV_STATUS status;

    FWSECLIC_READ_VBIOS_DESC readVbiosDesc;
    FWSECLIC_FRTS_CMD frtsCmd;

    void *pCmdBuffer;
    NvU32 cmdBufferSize;

    NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pFwsecUcode != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pPreparedCmd != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((cmd != FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS) ||
                        (frtsOffset > 0), NV_ERR_INVALID_ARGUMENT);

    if ((cmd != FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS) &&
        (cmd != FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB))
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pPreparedCmd->pFwsecUcode = pFwsecUcode;
    pPreparedCmd->cmd = cmd;
    pPreparedCmd->frtsOffset = frtsOffset;

    readVbiosDesc.version = 1;
    readVbiosDesc.size = sizeof(readVbiosDesc);
    readVbiosDesc.gfwImageOffset = 0;
    readVbiosDesc.gfwImageSize = 0;
    readVbiosDesc.flags = FWSECLIC_READ_VBIOS_STRUCT_FLAGS;

    if (cmd == FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS)
    {
        // FRTS takes an FRTS_CMD, here we build that up
        NvU32 blockSizeIn4K = NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K;

        frtsCmd.frtsRegionDesc.version = 1;
        frtsCmd.frtsRegionDesc.size = sizeof(frtsCmd.frtsRegionDesc);
        frtsCmd.frtsRegionDesc.frtsRegionOffset4K = (NvU32) (frtsOffset >> 12);
        frtsCmd.frtsRegionDesc.frtsRegionSize = blockSizeIn4K;
        frtsCmd.frtsRegionDesc.frtsRegionMediaType = FWSECLIC_FRTS_REGION_MEDIA_FB;

        frtsCmd.readVbiosDesc = readVbiosDesc;

        pCmdBuffer = &frtsCmd;
        cmdBufferSize = sizeof(frtsCmd);

    }
    else  // i.e. FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB
    {
        // SB takes READ_VBIOS_DESC directly
        pCmdBuffer = &readVbiosDesc;
        cmdBufferSize = sizeof(readVbiosDesc);
    }

    if (pFwsecUcode->bootType == KGSP_FLCN_UCODE_BOOT_FROM_HS)
    {
        KernelGspFlcnUcodeBootFromHs *pUcode = &pFwsecUcode->ucodeBootFromHs;
        NvU8 *pMappedImage;
        NvU8 *pMappedData;

        NvBool bSafe;

        NvU32 ucodeVersionVal;
        NvU32 hsSigVersions;
        NvU32 sigOffset;
        NvU32 sigMaxOffset;

        NV_ASSERT_OR_RETURN(pUcode->pUcodeMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pUcode->pSignatures != NULL, NV_ERR_INVALID_ARGUMENT);

        ucodeVersionVal = kgspReadUcodeFuseVersion_HAL(pGpu, pKernelGsp, pUcode->ucodeId);

        ucodeVersionVal = 1 << ucodeVersionVal;
        hsSigVersions = pUcode->vbiosSigVersions;

        if ((ucodeVersionVal & hsSigVersions) == 0)
        {
            return NV_ERR_NOT_SUPPORTED;;
        }

        sigOffset = 0;
        while ((ucodeVersionVal & hsSigVersions & 1) == 0)
        {
            sigOffset += (hsSigVersions & 1) * pUcode->sigSize;
            hsSigVersions >>= 1;
            ucodeVersionVal >>= 1;
        }

        if (sigOffset >= pUcode->signaturesTotalSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        bSafe = portSafeAddU32(sigOffset, pUcode->sigSize, &sigMaxOffset);
        if (!bSafe || sigMaxOffset > pUcode->signaturesTotalSize)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        pMappedImage = memdescMapInternal(pGpu, pUcode->pUcodeMemDesc, TRANSFER_FLAGS_NONE);
        if (pMappedImage == NULL)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
        pMappedData = pMappedImage + pUcode->dataOffset;

        status = s_vbiosPatchInterfaceData(pMappedData, pUcode->dmemSize, cmd,
                                           pCmdBuffer, cmdBufferSize, pUcode->interfaceOffset);

        portMemCopy(pMappedData + pUcode->hsSigDmemAddr, pUcode->sigSize,
                    ((NvU8 *) pUcode->pSignatures) + sigOffset, pUcode->sigSize);

        memdescUnmapInternal(pGpu, pUcode->pUcodeMemDesc,
                             TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedImage = NULL;
        pMappedData = NULL;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to prepare interface data for FWSEC cmd 0x%x: 0x%x\n",
                      cmd, status);
            goto out;
        }
    }
    else if (pFwsecUcode->bootType == KGSP_FLCN_UCODE_BOOT_WITH_LOADER)
    {
        KernelGspFlcnUcodeBootWithLoader *pUcode = &pFwsecUcode->ucodeBootWithLoader;
        NvU8 *pMappedData;

        NV_ASSERT_OR_RETURN(pUcode->pCodeMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pUcode->pDataMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

        pMappedData = memdescMapInternal(pGpu, pUcode->pDataMemDesc, TRANSFER_FLAGS_NONE);
        if (pMappedData == NULL)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        status = s_vbiosPatchInterfaceData(pMappedData, pUcode->dmemSize, cmd,
                                           pCmdBuffer, cmdBufferSize, pUcode->interfaceOffset);

        memdescUnmapInternal(pGpu, pUcode->pDataMemDesc,
                            TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedData = NULL;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to prepare interface data for FWSEC cmd 0x%x: 0x%x\n",
                      cmd, status);
            goto out;
        }
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

out:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "(note: VBIOS version %s)\n", pKernelGsp->vbiosVersionStr);
    }

    return status;
}

/*!
 * Execute a given FWSEC cmd and wait for completion.
 * KernelGspPreparedFwsecCmd should be set by s_prepareForFwsec_TU102 and
 * not filled in manually
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelGsp     KernelGsp pointer
 * @param[in]   pPreparedCmd   Prepared command state from s_prepareForFwsec_TU102
 */
NV_STATUS
kgspExecuteFwsec_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspPreparedFwsecCmd *pPreparedCmd
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pPreparedCmd != NULL, NV_ERR_INVALID_ARGUMENT);

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        return NV_ERR_GPU_IN_FULLCHIP_RESET;

    status = kgspExecuteHsFalcon_HAL(pGpu, pKernelGsp, pPreparedCmd->pFwsecUcode,
                                     staticCast(pKernelGsp, KernelFalcon), NULL, NULL);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC cmd 0x%x: status 0x%x\n", pPreparedCmd->cmd, status);
        goto out;
    }

    if (pPreparedCmd->cmd == FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS)
    {
        NvU32 data;
        NvU32 frtsErrCode;
        NvU32 wpr2HiVal;
        NvU32 wpr2LoVal;
        NvU32 expectedLoVal;

        data = GPU_REG_RD32(pGpu, NV_PBUS_VBIOS_SCRATCH(NV_VBIOS_FWSECLIC_SCRATCH_INDEX_0E));
        frtsErrCode = DRF_VAL(_VBIOS, _FWSECLIC, _FRTS_ERR_CODE, data);
        if (frtsErrCode != NV_VBIOS_FWSECLIC_FRTS_ERR_CODE_NONE)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for FRTS: FRTS error code 0x%x\n", frtsErrCode);
            status = NV_ERR_GENERIC;
            goto out;
        }

        data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_HI);
        wpr2HiVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_HI, _VAL, data);
        if (wpr2HiVal == 0)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for FRTS: no initialized WPR2 found\n");
            status = NV_ERR_GENERIC;
            goto out;
        }

        data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_LO);
        wpr2LoVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_LO, _VAL, data);
        expectedLoVal = (NvU32) (pPreparedCmd->frtsOffset >> NV_PFB_PRI_MMU_WPR2_ADDR_LO_ALIGNMENT);
        if (wpr2LoVal != expectedLoVal)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to execute FWSEC for FRTS: WPR2 initialized at an unexpected location: 0x%08x (expected 0x%08x)\n",
                      wpr2LoVal, expectedLoVal);
            status = NV_ERR_GENERIC;
            goto out;
        }
    }
    else  // i.e. FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB
    {
        NvU32 data;
        NvU32 sbErrCode;

        if (!GPU_FLD_TEST_DRF_DEF(pGpu, _PGC6, _AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK,
                                  _READ_PROTECTION_LEVEL0, _ENABLE))
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for SB: GFW PLM not lowered\n");
            status = NV_ERR_GENERIC;
            goto out;
        }

        if (!GPU_FLD_TEST_DRF_DEF(pGpu, _PGC6, _AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT,
                                  _PROGRESS, _COMPLETED))
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for SB: GFW progress not completed\n");
            status = NV_ERR_GENERIC;
            goto out;
        }

        data = GPU_REG_RD32(pGpu, NV_PBUS_VBIOS_SCRATCH(NV_VBIOS_FWSECLIC_SCRATCH_INDEX_15));
        sbErrCode = DRF_VAL(_VBIOS, _FWSECLIC, _SB_ERR_CODE, data);
        if (sbErrCode != NV_VBIOS_FWSECLIC_SB_ERR_CODE_NONE)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for SB: SB error code 0x%x\n", sbErrCode);
            status = NV_ERR_GENERIC;
            goto out;
        }
    }

out:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "(note: VBIOS version %s)\n", pKernelGsp->vbiosVersionStr);
    }

    return status;
}

/*!
 * Prepare to execute FWSEC FRTS ucode to setup FRTS
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelGsp     KernelGsp pointer
 * @param[in]   pFwsecUcode    KernelGspFlcnUcode structure of FWSEC ucode
 * @param[in]   frtsOffset     Desired offset in FB of FRTS data and WPR2
 * @param[out]  pPreparedCmd   Prepared command state to pass to kgspExecuteFwsec_TU102
 */
NV_STATUS
kgspPrepareForFwsecFrts_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode *pFwsecUcode,
    const NvU64 frtsOffset,
    KernelGspPreparedFwsecCmd *pPreparedCmd
)
{
    return s_prepareForFwsec_TU102(pGpu, pKernelGsp, pFwsecUcode,
                                   FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS,
                                   frtsOffset, pPreparedCmd);
}

/*!
 * Prepare to execute FWSEC SB ucode to setup FRTS
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelGsp     KernelGsp pointer
 * @param[in]   pFwsecUcode    KernelGspFlcnUcode structure of FWSEC ucode
 * @param[out]  pPreparedCmd   Prepared command state to pass to kgspExecuteFwsec_TU102
 */
NV_STATUS
kgspPrepareForFwsecSb_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode *pFwsecUcode,
    KernelGspPreparedFwsecCmd *pPreparedCmd
)
{
    return s_prepareForFwsec_TU102(pGpu, pKernelGsp, pFwsecUcode,
                                   FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_SB,
                                   0, pPreparedCmd);
}