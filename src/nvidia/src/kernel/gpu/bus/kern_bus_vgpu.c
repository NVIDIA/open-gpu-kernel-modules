/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"

/*!
 * @brief Setup BAR2 in physical mode.
 *
 * 1. Setup Bar2 in physical mode.
 * 2. Create CPU Mapping for BAR2.
 * 3. Use PA mode to setup RPC buffers in FB memory.
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pKernelBus  KernelBus pointer
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusBar2BootStrapInPhysicalMode_VF
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS status = NV_OK;

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
    {
        //
        // Bind in physical mode so that we can allocate
        //  RPC buffers in BAR2 PA mode.
        //
        status = kbusBindBar2_HAL(pGpu, pKernelBus, BAR2_MODE_PHYSICAL);

        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR, "Unable to bind BAR2 to physical mode.\n");
            return status;
        }
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    status = kbusSetupBar2CpuAperture_HAL(pGpu, pKernelBus, GPU_GFID_PF);
    NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

    pKernelBus->bIsBar2SetupInPhysicalMode = NV_TRUE;

cleanup:
    if (status != NV_OK)
    {
        kbusTeardownBar2CpuAperture_HAL(pGpu, pKernelBus, GPU_GFID_PF);
    }

    return status;
}

/*!
 * @brief Initialize pciBarSizes[], set pKernelBus->bPciBarSizesValid
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns void
 */
NV_STATUS
kbusInitBarsSize_VF(OBJGPU *pGpu, KernelBus *pKernelBus)
{
    if (!pKernelBus->bPciBarSizesValid)
    {
        pKernelBus->pciBarSizes[0] = kbusGetVfBar0SizeBytes(pGpu, pKernelBus);
        pKernelBus->pciBarSizes[1] = pGpu->fbLength;
        pKernelBus->pciBarSizes[2] = BUS_BAR2_APERTURE_MB;

        pKernelBus->bPciBarSizesValid = NV_TRUE;
    }
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusSetP2pMapping_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJGPU *pRemoteGpu = gpumgrGetGpuFromId(pParams->remoteGpuId);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RS_RES_CONTROL_PARAMS_INTERNAL *pControlParams = pCallContext->pControlParams;
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS shimParams = {0};

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pRemoteGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pParams->bUseUuid == NV_FALSE, NV_ERR_NOT_SUPPORTED);
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       pParams->connectionType != NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_INVALID,
                       NV_ERR_INVALID_ARGUMENT);

    portMemCopy(&shimParams, sizeof(NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS),
                pParams, sizeof(NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS));

    portMemCopy(shimParams.remoteGpuUuid, sizeof(shimParams.remoteGpuUuid),
                pRemoteGpu->gpuUuid.uuid, sizeof(pRemoteGpu->gpuUuid.uuid));

    shimParams.bUseUuid = NV_TRUE;

    NV_RM_RPC_CONTROL(pGpu,
                      pControlParams->hClient,
                      pControlParams->hObject,
                      pControlParams->cmd,
                      &shimParams,
                      sizeof(shimParams),
                      status);

    return status;
}

NV_STATUS
subdeviceCtrlCmdBusUnsetP2pMapping_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJGPU *pRemoteGpu = gpumgrGetGpuFromId(pParams->remoteGpuId);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RS_RES_CONTROL_PARAMS_INTERNAL *pControlParams = pCallContext->pControlParams;
    NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS shimParams = {0};

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pRemoteGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pParams->bUseUuid == NV_FALSE, NV_ERR_NOT_SUPPORTED);
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       pParams->connectionType != NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_INVALID,
                       NV_ERR_INVALID_ARGUMENT);

    portMemCopy(&shimParams, sizeof(NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS),
                pParams, sizeof(NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS));

    portMemCopy(shimParams.remoteGpuUuid, sizeof(shimParams.remoteGpuUuid),
                pRemoteGpu->gpuUuid.uuid, sizeof(pRemoteGpu->gpuUuid.uuid));

    shimParams.bUseUuid = NV_TRUE;

    NV_RM_RPC_CONTROL(pGpu,
                      pControlParams->hClient,
                      pControlParams->hObject,
                      pControlParams->cmd,
                      &shimParams,
                      sizeof(shimParams),
                      status);

    return status;
}
