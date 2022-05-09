/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "published/turing/tu102/dev_gc6_island_addendum.h"  // for NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K

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


// ---------------------------------------------------------------------------
// Functions for preparing and executing FWSEC commands
// ---------------------------------------------------------------------------

/*!
 * Patch DMEM of FWSEC for FRTS command
 *
 * @param[inout]  pMappedData      Pointer to mapped DMEM of FWSEC
 * @param[in]     mappedDataSize   Number of bytes valid under pMappedData
 * @param[in]     pFrtsCmd         FRTS command to patch in
 * @param[in]     interfaceOffset  Interface offset given by VBIOS for FWSEC
 */
NV_STATUS
s_vbiosPatchFrtsInterfaceData
(
    NvU8 *pMappedData,  // inout
    const NvU32 mappedDataSize,
    const FWSECLIC_FRTS_CMD *pFrtsCmd,
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
        NV_PRINTF(LEVEL_ERROR, "too few interface entires found for FRTS\n");
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
        NV_PRINTF(LEVEL_ERROR, "failed to find required interface entry for FRTS\n");
        return NV_ERR_INVALID_DATA;
    }

    pDmemMapper->init_cmd = FALCON_APPLICATION_INTERFACE_DMEM_MAPPER_V3_CMD_FRTS;

    if (pDmemMapper->cmd_in_buffer_size < sizeof(FWSECLIC_FRTS_CMD))
    {
        NV_PRINTF(LEVEL_ERROR, "insufficient cmd buffer for FRTS interface\n");
    }

    if (pDmemMapper->cmd_in_buffer_offset >= mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(pIntFaceEntry->dmemOffset, sizeof(*pFrtsCmd), &nextOffset);
    if (!bSafe || nextOffset > mappedDataSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    portMemCopy(pMappedData + pDmemMapper->cmd_in_buffer_offset, sizeof(*pFrtsCmd),
                pFrtsCmd, sizeof(*pFrtsCmd));

    return NV_OK;
}

/*!
 * Excecute FWSEC for FRTS and wait for completion.
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelGsp     KernelGsp pointer
 * @param[in]   pFwsecUcode    KernelGspFlcnUcode structure of FWSEC ucode
 * @param[in]   frtsOffset     Desired offset in FB of FRTS data and WPR2
 */
NV_STATUS
kgspExecuteFwsecFrts_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode *pFwsecUcode,
    const NvU64 frtsOffset
)
{
    NV_STATUS status;

    NvU32 blockSizeIn4K;
    FWSECLIC_FRTS_CMD frtsCmd;

    NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pFwsecUcode != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(frtsOffset > 0, NV_ERR_INVALID_ARGUMENT);

    // Build up FRTS args
    blockSizeIn4K = NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K;

    frtsCmd.frtsRegionDesc.version = 1;
    frtsCmd.frtsRegionDesc.size = sizeof(frtsCmd.frtsRegionDesc);
    frtsCmd.frtsRegionDesc.frtsRegionOffset4K = (NvU32) (frtsOffset >> 12);
    frtsCmd.frtsRegionDesc.frtsRegionSize = blockSizeIn4K;
    frtsCmd.frtsRegionDesc.frtsRegionMediaType = FWSECLIC_FRTS_REGION_MEDIA_FB;

    frtsCmd.readVbiosDesc.version = 1;
    frtsCmd.readVbiosDesc.size = sizeof(frtsCmd.readVbiosDesc);
    frtsCmd.readVbiosDesc.gfwImageOffset = 0;
    frtsCmd.readVbiosDesc.gfwImageSize = 0;
    frtsCmd.readVbiosDesc.flags = FWSECLIC_READ_VBIOS_STRUCT_FLAGS;

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

        status = s_vbiosPatchFrtsInterfaceData(pMappedData, pUcode->dmemSize,
                                               &frtsCmd, pUcode->interfaceOffset);

        portMemCopy(pMappedData + pUcode->hsSigDmemAddr, pUcode->sigSize,
                    ((NvU8 *) pUcode->pSignatures) + sigOffset, pUcode->sigSize);

        memdescUnmapInternal(pGpu, pUcode->pUcodeMemDesc,
                             TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedImage = NULL;
        pMappedData = NULL;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to prepare interface data for FRTS: 0x%x\n", status);
            return status;
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

        status = s_vbiosPatchFrtsInterfaceData(pMappedData, pUcode->dmemSize,
                                               &frtsCmd, pUcode->interfaceOffset);

        memdescUnmapInternal(pGpu, pUcode->pDataMemDesc,
                            TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedData = NULL;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to prepare interface data for FRTS: 0x%x\n", status);
            return status;
        }
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    status = kgspExecuteHsFalcon_HAL(pGpu, pKernelGsp, pFwsecUcode,
                                     staticCast(pKernelGsp, KernelFalcon), NULL, NULL);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for FRTS: status 0x%x\n", status);
        return status;
    }

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
            return NV_ERR_GENERIC;
        }

        data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_HI);
        wpr2HiVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_HI, _VAL, data);
        if (wpr2HiVal == 0)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC for FRTS: no initialized WPR2 found\n");
            return NV_ERR_GENERIC;
        }

        data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_LO);
        wpr2LoVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_LO, _VAL, data);
        expectedLoVal = (NvU32) (frtsOffset >> NV_PFB_PRI_MMU_WPR2_ADDR_LO_ALIGNMENT);
        if (wpr2LoVal != expectedLoVal)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to execute FWSEC for FRTS: WPR2 initialized at an unexpected location: 0x%08x (expected 0x%08x)\n",
                      wpr2LoVal, expectedLoVal);
            return NV_ERR_GENERIC;
        }
    }

    return status;
}
