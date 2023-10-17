/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief   GSP Client (CPU RM) specific GPU routines reside in this file.
 */

#include "core/core.h"
#include "gpu/gpu.h"
#include "ctrl/ctrl2080.h"

#include "gpu/gr/kernel_graphics_manager.h"

#include "gpu/gsp/kernel_gsp.h"

/*!
 * @brief Determines if the GPU has INTERNAL SKU FUSE parts by checking the GSP
 * static info
 *
 * @param[in]  pGpu   OBJGPU pointer
 *
 * @returns NV_TRUE if the GPU has INTERNAL SKU FUSE parts, NV_FALSE otherwise.
 */
NvBool
gpuIsInternalSku_FWCLIENT
(
    OBJGPU *pGpu
)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    return pGSCI->bGpuInternalSku;
}

NV_STATUS
gpuInitSriov_FWCLIENT
(
    OBJGPU *pGpu
)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    NvU32 totalPcieFns = 0;

    pGpu->sriovState.vfBarSize[0] = pGSCI->sriovCaps.bar0Size;
    pGpu->sriovState.vfBarSize[1] = pGSCI->sriovCaps.bar1Size;
    pGpu->sriovState.vfBarSize[2] = pGSCI->sriovCaps.bar2Size;

    pGpu->sriovState.maxGfid      = pGSCI->sriovMaxGfid;

    // note: pGpu->sriovState.virtualRegPhysOffset is initialized separately

    // owned by physical RM, so leave uninitialized
    pGpu->sriovState.pP2PInfo      = NULL;
    pGpu->sriovState.bP2PAllocated = NV_FALSE;
    pGpu->sriovState.maxP2pGfid    = 0;

    // Include Physical function that occupies GFID 0
    totalPcieFns = pGpu->sriovState.totalVFs + 1;

    pGpu->sriovState.pAllocatedGfids = portMemAllocNonPaged(totalPcieFns);

    if (pGpu->sriovState.pAllocatedGfids == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Memory allocation failed for GFID tracking\n");
        DBG_BREAKPOINT();
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pGpu->sriovState.pAllocatedGfids, 0, totalPcieFns);
    pGpu->sriovState.maxGfid = pGpu->sriovState.totalVFs;

    // Set GFID 0 in use.
    gpuSetGfidUsage(pGpu, GPU_GFID_PF, NV_TRUE);

    return NV_OK;
}

NV_STATUS
gpuDeinitSriov_FWCLIENT
(
    OBJGPU *pGpu
)
{
    if (pGpu->sriovState.pAllocatedGfids != NULL)
    {
        // Clear GFID 0 usage.
        gpuSetGfidUsage(pGpu, GPU_GFID_PF, NV_FALSE);

        portMemFree(pGpu->sriovState.pAllocatedGfids);
        pGpu->sriovState.pAllocatedGfids = NULL;
    }

    return NV_OK;
}

NvBool
gpuCheckPageRetirementSupport_GSPCLIENT
(
    OBJGPU *pGpu
)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    return pGSCI->bPageRetirementSupported;
}

void gpuInitBranding_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    pGpu->bIsQuadro    = pGSCI->bIsQuadroAd || pGSCI->bIsQuadroGeneric;
    pGpu->bIsNvidiaNvs = pGSCI->bIsNvidiaNvs;
    pGpu->bIsVgx       = pGSCI->bIsVgx;
    pGpu->bGeforceSmb  = pGSCI->bGeforceSmb;
    pGpu->bIsTitan     = pGSCI->bIsTitan;
    pGpu->bIsTesla     = pGSCI->bIsTesla;

    pGpu->bIsGeforce = !(pGpu->bIsQuadro || pGpu->bIsTesla || pGpu->bIsNvidiaNvs);
}

BRANDING_TYPE gpuDetectBranding_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    if (pGSCI->bIsQuadroGeneric)
        return BRANDING_TYPE_QUADRO_GENERIC;
    if (pGSCI->bIsQuadroAd)
        return BRANDING_TYPE_QUADRO_AD;
    if (pGSCI->bIsNvidiaNvs)
        return BRANDING_TYPE_NVS_NVIDIA;

    return BRANDING_TYPE_NONE;
}

COMPUTE_BRANDING_TYPE
gpuDetectComputeBranding_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    return pGSCI->computeBranding;
}

BRANDING_TYPE
gpuDetectVgxBranding_FWCLIENT(OBJGPU *pGpu)
{
    return pGpu->bIsVgx ? BRANDING_TYPE_VGX : BRANDING_TYPE_NONE;
}

NV_STATUS
gpuGenGidData_FWCLIENT
(
    OBJGPU *pGpu,
    NvU8   *pGidData,
    NvU32   gidSize,
    NvU32   gidFlags
)
{
    if (FLD_TEST_DRF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1, gidFlags))
    {
        GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
        NvU8 zeroGid[RM_SHA1_GID_SIZE] = { 0 };

        if (portMemCmp(pGSCI->gidInfo.data, zeroGid, RM_SHA1_GID_SIZE) == 0)
        {
             NV_PRINTF(LEVEL_ERROR, "GSP Static Info has not been initialized yet for UUID\n");
             return NV_ERR_INVALID_STATE;
        }

        portMemCopy(pGidData, RM_SHA1_GID_SIZE, pGSCI->gidInfo.data, RM_SHA1_GID_SIZE);
        return NV_OK;
    }
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 gpuGetActiveFBIOs_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    return pGSCI->fbio_mask;
}

NvBool gpuIsGlobalPoisonFuseEnabled_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    return pGSCI->poisonFuseEnabled;
}

void gpuInitProperties_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_MOBILE, pGSCI->bIsMobile);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_SUPPORTED, pGSCI->bIsGc6Rtd3Allowed);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED, pGSCI->bIsGcOffRtd3Allowed);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_UEFI, pGSCI->bIsGpuUefi);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_LEGACY_GCOFF_SUPPORTED, pGSCI->bIsGcoffLegacyAllowed);
}

/*!
 * @brief These functions are used on CPU RM when pGpu is a GSP client.
 * Data is fetched from GSP using subdeviceCtrlCmdInternalGetChipInfo and cached,
 * then retrieved through the internal gpuGetChipInfo.
 *
 * Functions either return value directly, or through a second [out] param, depending
 * on the underlying function.
 *
 * @param[in]  pGpu
 */
NvU8
gpuGetChipSubRev_FWCLIENT
(
    OBJGPU *pGpu
)
{
    const NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pChipInfo = gpuGetChipInfo(pGpu);
    NV_ASSERT_OR_RETURN(pChipInfo != NULL, 0);

    return pChipInfo->chipSubRev;
}

NvU32
gpuGetEmulationRev1_FWCLIENT
(
    OBJGPU *pGpu
)
{
    const NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pChipInfo = gpuGetChipInfo(pGpu);
    NV_ASSERT_OR_RETURN(pChipInfo != NULL, 0);

    return pChipInfo->emulationRev1;
}

NV_STATUS
gpuConstructDeviceInfoTable_FWCLIENT
(
    OBJGPU *pGpu
)
{
    NV_STATUS status;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams;

    if (pGpu->pDeviceInfoTable) // already initialized
        return NV_OK;

    pParams = portMemAllocNonPaged(sizeof *pParams);
    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_NO_MEMORY);

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_GET_DEVICE_INFO_TABLE,
                             pParams,
                             sizeof *pParams);
    if (status != NV_OK)
        goto done;

    if (pParams->numEntries == 0)
        goto done;

    pGpu->pDeviceInfoTable = portMemAllocNonPaged(
        pParams->numEntries * (sizeof *pGpu->pDeviceInfoTable));
    NV_ASSERT_TRUE_OR_GOTO(status,
                           pGpu->pDeviceInfoTable != NULL,
                           NV_ERR_NO_MEMORY,
                           done);

    pGpu->numDeviceInfoEntries = pParams->numEntries;
    portMemCopy(pGpu->pDeviceInfoTable,
                pGpu->numDeviceInfoEntries * (sizeof *pGpu->pDeviceInfoTable),
                pParams->deviceInfoTable,
                pParams->numEntries * (sizeof pParams->deviceInfoTable[0]));

done:
    portMemFree(pParams);
    return status;
}

NvU32
gpuGetLitterValues_FWCLIENT
(
    OBJGPU *pGpu,
    NvU32 index
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    const NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;
    NvU32 i;

    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, 0);
    pGrInfo = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo;
    NV_ASSERT_OR_RETURN(pGrInfo != NULL, 0);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGrInfo->infoList); i++)
    {
        if (pGrInfo->infoList[i].index == index)
            return pGrInfo->infoList[i].data;
    }
    return 0;
}

NV_STATUS
gpuGetRegBaseOffset_FWCLIENT
(
    OBJGPU *pGpu,
    NvU32 regBase,
    NvU32 *pOffset
)
{
    const NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pChipInfo = gpuGetChipInfo(pGpu);
    NV_ASSERT_OR_RETURN(pChipInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(regBase < NV_ARRAY_ELEMENTS(pChipInfo->regBases), NV_ERR_NOT_SUPPORTED);

    if (pChipInfo->regBases[regBase] != 0xFFFFFFFF)
    {
        *pOffset = pChipInfo->regBases[regBase];
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NvU32
gpuReadBAR1Size_FWCLIENT
(
    OBJGPU *pGpu
)
{
    const NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pChipInfo = gpuGetChipInfo(pGpu);
    NV_ASSERT_OR_RETURN(pChipInfo != NULL, 0);

    return pChipInfo->bar1Size;
}
