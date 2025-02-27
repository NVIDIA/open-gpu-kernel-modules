/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/subdevice/subdevice.h"

#include "nvrm_registry.h"
#include "os/os.h"
#include "vgpu/rpc.h"

NV_STATUS
subdeviceCtrlCmdBiosGetSKUInfo_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pBiosGetSKUInfoParams
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(pBiosGetSKUInfoParams, sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS), &pVSI->SKUInfo, sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS));

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdGpuQueryEccConfiguration_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *pConfig
)
{
    OBJGPU     *pGpu     = GPU_RES_GET_GPU(pSubdevice);
    OBJVGPU    *pVGpu    = GPU_GET_VGPU(pGpu);
    NvU32       eccState = 0;

    NV_ASSERT_OR_RETURN(pVGpu != NULL, NV_ERR_INVALID_STATE);

    if (!pVGpu->bECCSupported)
        return NV_ERR_NOT_SUPPORTED;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GUEST_ECC_STATE, &eccState) == NV_OK)
    {
        pConfig->currentConfiguration = eccState;
    }
    else
    {
        pConfig->currentConfiguration = pVGpu->bECCEnabled;
    }

    // If ECC supported on vGPU, default ECC state is enabled.
    pConfig->defaultConfiguration = NV_REG_STR_RM_GUEST_ECC_STATE_DEFAULT;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdGpuSetEccConfiguration_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *pConfig
)
{
    OBJGPU     *pGpu     = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS   rmStatus = NV_OK;
    OBJVGPU    *pVGpu    = GPU_GET_VGPU(pGpu);
    NvU32       eccState;

    NV_ASSERT_OR_RETURN(pVGpu != NULL, NV_ERR_INVALID_STATE);

    if (!pVGpu->bECCSupported)
        return NV_ERR_NOT_SUPPORTED;

    eccState = !!pConfig->newConfiguration;

    rmStatus = osWriteRegistryDword(pGpu, NV_REG_STR_RM_GUEST_ECC_STATE, eccState);

    return rmStatus;
}

/*!
 * @brief  Returns the status of the HW scrubber - running/idle.
 *         If the scrubber is running, it also returns the range
 *         that is being scrubbed.
 */
NV_STATUS
subdeviceCtrlCmdGpuQueryScrubberStatus_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *pParams
)
{
    pParams->scrubberStatus = NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_SCRUBBER_IDLE;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBiosGetPostTime_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *pBiosPostTime
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    pBiosPostTime->vbiosPostTime = pGpu->gpuVbiosPostTime;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdGpuResetEccErrorStatus_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *pParams
)
{
    OBJGPU           *pGpu        = GPU_RES_GET_GPU(pSubdevice);
    OBJVGPU          *pVGpu       = GPU_GET_VGPU(pGpu);
    VGPU_STATIC_INFO *pVSI        = GPU_GET_STATIC_INFO(pGpu);
    NvBool            bMigEnabled = IS_MIG_ENABLED(pGpu);
    NvU32 i;

    NV_CHECK_OR_RETURN(LEVEL_INFO, !bMigEnabled, NV_ERR_NOT_SUPPORTED);

    if ((pVGpu == NULL) || (!pVGpu->bECCSupported) ||
        (pParams->statuses & NV2080_CTRL_GPU_ECC_ERROR_STATUS_AGGREGATE))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT; i++)
    {
        if (pVSI->eccStatus.units[i].supported)
        {
            pVSI->eccStatus.units[i].sbe.count = 0;
            pVSI->eccStatus.units[i].dbe.count = 0;
        }
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdGpuQueryEccStatus_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *pParams
)
{
    OBJGPU           *pGpu   = GPU_RES_GET_GPU(pSubdevice);
    OBJVGPU          *pVGpu  = GPU_GET_VGPU(pGpu);
    VGPU_STATIC_INFO *pVSI   = GPU_GET_STATIC_INFO(pGpu);
    NV_STATUS         status = NV_OK;

    if ((pVGpu == NULL) || !pVGpu->bECCSupported)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pVGpu->bECCEnabled)
    {
        portMemCopy(pParams, sizeof(*pParams), &pVSI->eccStatus, sizeof(pVSI->eccStatus));
    }
    else
    {
        portMemSet(pParams, 0, sizeof(*pParams));
    }

    return status;
}

/*!
 * @brief   This Command is used to report if the specified logo illumination attribute
 *          is supported
 *
 * @param[in,out]   pConfigParams
 *                  attribute:  The attribute whose support is to be determined.
 *                  bSupported: indicator if the specified attribute is supported.
 *
 * @return  Returns NV_STATUS
 *          NV_OK                     Success
 *
 */
NV_STATUS
subdeviceCtrlCmdGpuQueryIllumSupport_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *pConfigParams
)
{
    pConfigParams->bSupported = NV_FALSE;

    return NV_OK;
}

NV_STATUS subdeviceCtrlCmdBiosGetInfoV2_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *pBiosInfoParams
)
{
    NV2080_CTRL_BIOS_INFO *pBiosInfos = pBiosInfoParams->biosInfoList;
    NvU32 i;

    if ((pBiosInfoParams->biosInfoListSize == 0) ||
        (pBiosInfoParams->biosInfoListSize > NV2080_CTRL_BIOS_INFO_MAX_SIZE))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    for (i = 0; i < pBiosInfoParams->biosInfoListSize; i++)
    {
        pBiosInfos[i].data = 0;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_VF
(
    Subdevice                                                           *pSubdevice,
    NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS   *pConfig
)
{
    // TODO: Will implement the functionality for VF in a subsequent changelist (Bug 4308325)
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_VF
(
    Subdevice                                                   *pSubdevice,
    NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS     *pConfig
)
{
    // TODO: Will implement the functionality for VF in a subsequent changelist (Bug 4308325)
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_VF
(
    Subdevice                                           *pSubdevice,
    NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS  *pConfig
)
{
    // TODO: Will implement the functionality for VF in a subsequent changelist (Bug 4308325)
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdCcuGetSampleInfo_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *pParams
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(pParams, sizeof(*pParams), &pVSI->ccuSampleInfo, sizeof(pVSI->ccuSampleInfo));

    return NV_OK;
}
