/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/timer/objtmr.h"
#include "os/os.h"
#include "gpu/gpu.h"
#include "class/cl0080.h"
#include "vgpu/vgpu_util.h"

/*!
 * Generate GID data for vGPU.
 * For SHA-1, we return uuid cached in the vGPU object
 * We do not support SHA-256
 *
 * @param  [in]  pGpu      OBJGPU pointer
 * @param  [out] pGidData  data array into which GID should be written
 * @param  [in]  gidSize   size of data array
 * @param  [in]  gidFlags  selects either the SHA-1 or SHA-256 GID
 *
 * @return       NV_OK if the GID if SHA1 GID is requested
 */
NV_STATUS
gpuGenGidData_VF
(
    OBJGPU *pGpu,
    NvU8   *pGidData,
    NvU32   gidSize,
    NvU32   gidFlags
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    if (pVSI != NULL && FLD_TEST_DRF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1, gidFlags))
    {
        portMemCopy(pGidData, RM_SHA1_GID_SIZE, pVSI->gidInfo.data, RM_SHA1_GID_SIZE);
        return NV_OK;
    }
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief       Returns FBIO Floorsweeping Mask
 *
 * @param[in]   pGpu            OBJGPU pointer
 * @returns     FBIO Floorsweeping Mask - On is enabled
 *
 */
NvU64 gpuGetActiveFBIOs_VF
(
    OBJGPU *pGpu
)
{
    // Cache is not valid.
    if (pGpu->activeFBIOs == 0)
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
        if (pVSI)
            pGpu->activeFBIOs = pVSI->fbioMask;
    }
    // Return the cached map of available FBIOs
    return pGpu->activeFBIOs;
}

NV_STATUS
gpuCreateDefaultClientShare_VF
(
    OBJGPU *pGpu
)
{
    NvHandle                hClient    = NV01_NULL_OBJECT;
    NvHandle                hDevice    = NV01_NULL_OBJECT;
    NvHandle                hSubDevice = NV01_NULL_OBJECT;
    NV_STATUS               status;
    NV0080_ALLOC_PARAMETERS deviceAllocParams;
    NV2080_ALLOC_PARAMETERS subdeviceAllocParams;
    NvU32                   deviceInstance;
    RM_API                 *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT_OK_OR_RETURN(pRmApi->AllocWithHandle(pRmApi,
                                                   hClient,
                                                   hClient,
                                                   hClient,
                                                   NV01_ROOT,
                                                   &hClient,
                                                   sizeof(hClient)));

    pGpu->hDefaultClientShare = hClient;

    // Which device are we?
    deviceInstance = gpuGetDeviceInstance(pGpu);

    portMemSet(&deviceAllocParams, 0, sizeof(NV0080_ALLOC_PARAMETERS));
    deviceAllocParams.deviceId = deviceInstance;
    deviceAllocParams.hClientShare = pGpu->hDefaultClientShare;

    // Add a device.
    NV_ASSERT_OK_OR_GOTO(status, pRmApi->Alloc(pRmApi,
                                               hClient,
                                               hClient,
                                               &hDevice,
                                               NV01_DEVICE_0,
                                               &deviceAllocParams,
                                               sizeof(deviceAllocParams)), failed);

    pGpu->hDefaultClientShareDevice = hDevice;

    portMemSet(&subdeviceAllocParams, 0, sizeof(NV2080_ALLOC_PARAMETERS));
    subdeviceAllocParams.subDeviceId = 0;

    NV_ASSERT_OK_OR_GOTO(status, pRmApi->Alloc(pRmApi,
                                               hClient,
                                               hDevice,
                                               &hSubDevice,
                                               NV20_SUBDEVICE_0,
                                               &subdeviceAllocParams,
                                               sizeof(subdeviceAllocParams)), failed);

    pGpu->hDefaultClientShareSubDevice = hSubDevice;

    return NV_OK;

failed:
    pRmApi->Free(pRmApi, hClient, hClient);
    pGpu->hDefaultClientShare = NV01_NULL_OBJECT;

    return status;
}

void
gpuDestroyDefaultClientShare_VF
(
    OBJGPU *pGpu
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (pGpu->hDefaultClientShare != NV01_NULL_OBJECT)
    {
        // Frees everything allocated under this client.
        pRmApi->Free(pRmApi, pGpu->hDefaultClientShare,
                     pGpu->hDefaultClientShare);
    }
}

/*!
 *  Determines if the board/GPU supports page retirement i.e. removal of
 *  blacklisted pages from FB heap. For vGPU guest, it is only
 *  supported for vGPU types which have ECC enabled
 *
 * @param[in]   pGpu        GPU object pointer
 *
 * @return NV_TRUE
 *      If page retirement is supported.
 * @return NV_FALSE
 *      If page retirement is not supported.
 */

NvBool
gpuCheckPageRetirementSupport_VF
(
    OBJGPU *pGpu
)
{
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_POINTER);
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);

    return pVGpu->page_retirement_enabled;
}

NV_STATUS
gpuInitSriov_VF
(
    OBJGPU *pGpu
)
{
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
        pGpu->bSplitVasManagementServerClientRm = pVSI->bSplitVasBetweenServerClientRm;
    }

    return NV_OK;
}

NV_STATUS
gpuGetNameString_VF
(
    OBJGPU *pGpu,
    NvU32 type,
    void *nameStringBuffer
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    if (type == NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII)
    {
        portMemCopy(nameStringBuffer, sizeof(pVSI->adapterName),
                    pVSI->adapterName, sizeof(pVSI->adapterName));
    }
    else
    {
        portMemCopy(nameStringBuffer, sizeof(pVSI->adapterName_Unicode),
                    pVSI->adapterName_Unicode, sizeof(pVSI->adapterName_Unicode));
    }

    return NV_OK;
}

NV_STATUS
gpuGetShortNameString_VF
(
    OBJGPU *pGpu,
    NvU8 *nameStringBuffer
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(nameStringBuffer, sizeof(pVSI->shortGpuNameString),
                pVSI->shortGpuNameString, sizeof(pVSI->shortGpuNameString));

    return NV_OK;
}

NvBool
gpuIsGlobalPoisonFuseEnabled_VF
(
    OBJGPU *pGpu
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_FALSE);

    return pVSI->poisonFuseEnabled;
}

NV_STATUS
gpuConstructDeviceInfoTable_VF
(
    OBJGPU *pGpu
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    if (pGpu->pDeviceInfoTable) // already initialized
        return NV_OK;

    if (pVSI->deviceInfoTable.numEntries == 0)
        return NV_OK;

    NV_ASSERT_OR_RETURN(pVSI->deviceInfoTable.numEntries <= NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES,
                        NV_ERR_INVALID_STATE);

    pGpu->pDeviceInfoTable = portMemAllocNonPaged(
        pVSI->deviceInfoTable.numEntries * (sizeof *pGpu->pDeviceInfoTable));

    NV_ASSERT_OR_RETURN(pGpu->pDeviceInfoTable != NULL, NV_ERR_NO_MEMORY);

    pGpu->numDeviceInfoEntries = pVSI->deviceInfoTable.numEntries;
    for (NvU32 i = 0; i < pGpu->numDeviceInfoEntries; i++)
    {
        NV2080_CTRL_INTERNAL_DEVICE_INFO *pSrc =
            &pVSI->deviceInfoTable.deviceInfoTable[i];

        pGpu->pDeviceInfoTable[i] = (DEVICE_INFO2_ENTRY){
            .faultId                = pSrc->faultId,
            .instanceId             = pSrc->instanceId,
            .typeEnum               = pSrc->typeEnum,
            .resetId                = pSrc->resetId,
            .devicePriBase          = pSrc->devicePriBase,
            .isEngine               = pSrc->isEngine,
            .rlEngId                = pSrc->rlEngId,
            .groupId                = pSrc->groupId,
            .runlistPriBase         = pSrc->runlistPriBase,
            .groupId                = pSrc->groupId,
            .ginTargetId            = pSrc->ginTargetId,
            .deviceBroadcastPriBase = pSrc->deviceBroadcastPriBase,
            .groupLocalInstanceId   = pSrc->groupLocalInstanceId
        };
    }

    return NV_OK;
}

/*!
 * @brief Initialize GPU branding properties
 */
NV_STATUS gpuInitBranding_VF(OBJGPU *pGpu)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);

    NV_ASSERT_OR_RETURN(pVGpu != NULL, NV_ERR_INVALID_STATE);

    pGpu->bIsQuadro    = (pVGpu->vgpuConfigUsmType == NV_VGPU_CONFIG_USM_TYPE_QUADRO);
    pGpu->bIsQuadroAD  = NV_FALSE;
    pGpu->bIsNvidiaNvs = (pVGpu->vgpuConfigUsmType == NV_VGPU_CONFIG_USM_TYPE_NVS);
    pGpu->bIsVgx       = NV_TRUE;
    pGpu->bGeforceSmb  = NV_FALSE;
    pGpu->bIsTitan     = NV_FALSE;
    pGpu->bIsTesla     = (pVGpu->vgpuConfigUsmType == NV_VGPU_CONFIG_USM_TYPE_COMPUTE);
    pGpu->bIsGeforce   = !(pGpu->bIsQuadro || pGpu->bIsTesla || pGpu->bIsNvidiaNvs);

    return NV_OK;
}

NV_STATUS
gpuGetSkuInfo_VF
(
    OBJGPU *pGpu,
    NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pParams
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(pParams,
                sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS),
                &pVSI->SKUInfo,
                sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS));

    return NV_OK;
}

NvBool
gpuValidateMIGSupport_VF
(
    OBJGPU *pGpu
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    return pVSI->gpuPartitionInfo.swizzId != KMIGMGR_SWIZZID_INVALID;
}
