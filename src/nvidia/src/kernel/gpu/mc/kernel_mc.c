/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/mc/kernel_mc.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "gpu/gpu.h"
#include "diagnostics/nv_debug_dump.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mmu/kern_gmmu.h"

#include "lib/protobuf/prb_util.h"
#include "g_nvdebug_pb.h"

//
// MC RM SubDevice Controls
//
// This rmctrl MUST NOT touch hw since it's tagged as NO_GPUS_ACCESS in ctrl2080.def
// RM allow this type of rmctrl to go through when GPU is not available.
//
NV_STATUS
subdeviceCtrlCmdMcGetArchInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *pArchInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    if (IsTEGRA(pGpu))
    {
        pArchInfoParams->architecture = pGpu->chipInfo.platformId;
        pArchInfoParams->implementation = pGpu->chipInfo.implementationId;
        pArchInfoParams->revision = pGpu->chipInfo.revisionId;
    }
    else
    {
        if (pGpu->idInfo.ArchitectureExternal && pGpu->idInfo.ImplementationExternal)
        {
            pArchInfoParams->architecture = pGpu->idInfo.ArchitectureExternal;
            pArchInfoParams->implementation = pGpu->idInfo.ImplementationExternal;
        }
        else
        {
            pArchInfoParams->architecture = pGpu->chipInfo.pmcBoot0.arch;
            pArchInfoParams->implementation = pGpu->chipInfo.pmcBoot0.impl;
        }
        pArchInfoParams->revision = (pGpu->chipInfo.pmcBoot0.majorRev << 4) | pGpu->chipInfo.pmcBoot0.minorRev;
        pArchInfoParams->subRevision = pGpu->chipInfo.subRevision;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdMcGetManufacturer_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *pManufacturerParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    pManufacturerParams->manufacturer = pGpu->idInfo.Manufacturer;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_MC_QUERY_HOSTCLK_SLOWDOWN_STATUS_PARAMS *pGetStatusParams
)
{
    pGetStatusParams->bDisabled = NV_FALSE;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdMcSetHostclkSlowdownStatus_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_MC_SET_HOSTCLK_SLOWDOWN_STATUS_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *pReplayableFaultOwnrshpParams
)
{
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pSubdevice);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    if (pKernelGmmu != NULL)
    {
        kgmmuChangeReplayableFaultOwnership_HAL(pGpu, pKernelGmmu, pReplayableFaultOwnrshpParams->bOwnedByRm);
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

//
// Routine to dump Engine State on Error Conditions
//
static NV_STATUS
_kmcDumpEngineFunc(OBJGPU *pGpu, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, void *pvData)
{
    NV_STATUS rmStatus = NV_OK;
    NvU8  startingDepth = prbEncNestingLevel(pPrbEnc);

    switch (DRF_VAL(_NVD, _ERROR_CODE, _MAJOR, pNvDumpState->internalCode))
    {
    case NVD_GPU_GENERATED:
    case NVD_SKIP_ZERO:
        // don't report on these internal codes.
        return NV_OK;
    }

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_GPUINFO_ENG_MC),
        External_Cleanup);

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_ENG_MC_RM_DATA),
        External_Cleanup);

    prbEncAddUInt32(pPrbEnc, NVDEBUG_ENG_MC_RMDATA_PMCBOOT0, pGpu->chipId0);

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR, // NVDEBUG_ENG_MC_RM_DATA
        prbEncNestedEnd(pPrbEnc),
        External_Cleanup);

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        NvU32       i = 0;

        for (i = 0; i < pKernelBus->totalPciBars; ++i)
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, NVDEBUG_ENG_MC_PCI_BARS),
                External_Cleanup);

            // Memory variables only.  RM lock not needed.
            prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_MC_PCIBARINFO_OFFSET,
                            kbusGetPciBarOffset(pKernelBus, i));

            prbEncAddUInt64(pPrbEnc, NVDEBUG_ENG_MC_PCIBARINFO_LENGTH,
                            kbusGetPciBarSize(pKernelBus, i));

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR, // NVDEBUG_ENG_MC_PCI_BARS
                prbEncNestedEnd(pPrbEnc),
                External_Cleanup);
        }
    }

External_Cleanup:
    // Unwind the protobuff to inital depth
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(rmStatus, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return rmStatus;
}

NV_STATUS
kmcStateInitLocked_IMPL
(
    OBJGPU *pGpu,
    KernelMc *pKernelMc
)
{
    NvDebugDump *pNvd = GPU_GET_NVD(pGpu);
    if(pNvd != NULL)
    {
        nvdEngineSignUp(pGpu,
                        pNvd,
                        _kmcDumpEngineFunc,
                        NVDUMP_COMPONENT_ENG_MC,
                        REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _CRITICAL) |
                        REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _BOTH),
                        (void *)pKernelMc);
    }
    return NV_OK;
}
