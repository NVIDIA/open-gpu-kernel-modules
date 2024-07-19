/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * GSP Client (CPU RM) specific GPU routines reside in this file.
 */

#include "core/core.h"
#include "gpu/gpu.h"
#include "ctrl/ctrl2080.h"

#include "gpu/gsp/gsp_static_config.h"  // CORERM-3199

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

NV_STATUS gpuInitBranding_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    pGpu->bIsQuadro    = pGSCI->bIsQuadroAd || pGSCI->bIsQuadroGeneric;
    pGpu->bIsQuadroAD  = pGSCI->bIsQuadroAd;
    pGpu->bIsNvidiaNvs = pGSCI->bIsNvidiaNvs;
    pGpu->bIsVgx       = pGSCI->bIsVgx;
    pGpu->bGeforceSmb  = pGSCI->bGeforceSmb;
    pGpu->bIsTitan     = pGSCI->bIsTitan;
    pGpu->bIsTesla     = pGSCI->bIsTesla;
    pGpu->bIsGeforce   = !(pGpu->bIsQuadro || pGpu->bIsTesla || pGpu->bIsNvidiaNvs);

    return NV_OK;
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
             NV_PRINTF(LEVEL_INFO, "GSP Static Info has not been initialized yet for UUID\n");
             return NV_ERR_INVALID_STATE;
        }

        portMemCopy(pGidData, RM_SHA1_GID_SIZE, pGSCI->gidInfo.data, RM_SHA1_GID_SIZE);
        return NV_OK;
    }
    return NV_ERR_NOT_SUPPORTED;
}

NvU64 gpuGetActiveFBIOs_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    return pGSCI->fbio_mask;
}

NvBool gpuIsGlobalPoisonFuseEnabled_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    return pGSCI->poisonFuseEnabled;
}

void gpuGetRtd3GC6Data_FWCLIENT(OBJGPU *pGpu)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    pGpu->gc6State.GC6PerstDelay      = pGSCI->RTD3GC6PerstDelay;
    pGpu->gc6State.GC6TotalBoardPower = pGSCI->RTD3GC6TotalBoardPower;
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

    NV_ASSERT_OR_RETURN(pParams->numEntries <= NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES,
                        NV_ERR_INVALID_STATE);

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

NV_STATUS
gpuGetNameString_FWCLIENT
(
    OBJGPU *pGpu,
    NvU32 type,
    void *nameStringBuffer
)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    if (type == NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII)
    {
        portMemCopy(nameStringBuffer, sizeof(pGSCI->gpuNameString),
                    pGSCI->gpuNameString, sizeof(pGSCI->gpuNameString));
    }
    else
    {
        portMemCopy(nameStringBuffer, sizeof(pGSCI->gpuNameString_Unicode),
                    pGSCI->gpuNameString_Unicode, sizeof(pGSCI->gpuNameString_Unicode));
    }

    return NV_OK;
}

NV_STATUS
gpuGetShortNameString_FWCLIENT
(
    OBJGPU *pGpu,
    NvU8 *nameStringBuffer
)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    portMemCopy(nameStringBuffer, sizeof(pGSCI->gpuShortNameString),
                pGSCI->gpuShortNameString, sizeof(pGSCI->gpuShortNameString));

    return NV_OK;
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
