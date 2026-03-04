/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "platform/sli/sli.h"

#include "diagnostics/nv_debug_dump.h"

#include "lib/protobuf/prb_util.h"
#include "g_nvdebug_pb.h"

//
// Routine to dump gpu engine common fields
//
static
NV_STATUS
_gpuDumpEngine_CommonFields
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE   *pNvDumpState
)
{
    NV_STATUS rmStatus    = NV_OK;

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_ENG_GPU_GPU_ID,
                    pGpu->gpuId);

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_SLI,
                  IsSLIEnabled(pGpu));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_NOTEBOOK,
                  IsMobile(pGpu));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_VIRTUAL,
                  IS_VIRTUAL(pGpu));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_FULL_POWER,
                  gpuIsGpuFullPower(pGpu));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_IN_FULLCHIP_RESET,
                  pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_IN_SEC_BUS_RESET,
                  pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_IN_GC6_RESET,
                  pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_GC6_RESET));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_SUSPENDED,
                  pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_LOST,
                  pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST));

    prbEncAddBool(pPrbEnc,
                  NVDEBUG_ENG_GPU_IS_ACCESSIBLE,
                  pNvDumpState->bGpuAccessible);

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_ENG_GPU_RUSD_MASK,
                    pGpu->userSharedData.lastPolledDataMask);

    return rmStatus;
}

//
// Routine to dump gpu engine debug info
//
static
NV_STATUS
_gpuDumpEngineFunc
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE   *pNvDumpState,
    void        *pvData
)
{
    NV_STATUS nvStatus = NV_OK;
    NvU8 startingDepth = prbEncNestingLevel(pPrbEnc);

    // Dump basic GPU info for all error types.
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_GPUINFO_ENG_GPU));

    // Dump common fields.
    NV_CHECK_OK(nvStatus, LEVEL_ERROR,
        _gpuDumpEngine_CommonFields(pGpu, pPrbEnc, pNvDumpState));

    // Unwind the protobuf to the correct depth.
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return nvStatus;
}

void
gpuDumpCallbackRegister_IMPL
(
    OBJGPU   *pGpu
)
{
    NvDebugDump *pNvd = GPU_GET_NVD(pGpu);
    if (pNvd != NULL)
    {
        nvdEngineSignUp(pGpu,
                        pNvd,
                        _gpuDumpEngineFunc,
                        NVDUMP_COMPONENT_ENG_GPU,
                        REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _MED) |
                        REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _BOTH),
                        (void *)pGpu);

        if (IS_GSP_CLIENT(pGpu))
        {
            //
            // Sign up all the GSP-only NVDUMP engines with a NULL dump function.
            // This will make the CPU-RM call NV_RM_RPC_DUMP_PROTOBUF_COMPONENT
            // for each of these to collect the data from GSP and then put it
            // in the overall protobuf payload.
            //
            NvU32 gspNvdEngines[] =
            {
                NVDUMP_COMPONENT_ENG_FIFO,
                NVDUMP_COMPONENT_ENG_GRAPHICS,
                NVDUMP_COMPONENT_ENG_FB,
                NVDUMP_COMPONENT_ENG_DISP,
                NVDUMP_COMPONENT_ENG_FAN,
                NVDUMP_COMPONENT_ENG_THERMAL,
                NVDUMP_COMPONENT_ENG_FUSE,
                NVDUMP_COMPONENT_ENG_VBIOS,
                NVDUMP_COMPONENT_ENG_PERF,
                NVDUMP_COMPONENT_ENG_BUS,
                NVDUMP_COMPONENT_ENG_PMU,
                NVDUMP_COMPONENT_ENG_CE,
                NVDUMP_COMPONENT_ENG_LPWR,
                NVDUMP_COMPONENT_ENG_VOLT,
                NVDUMP_COMPONENT_ENG_CLK,
                NVDUMP_COMPONENT_ENG_SEC2,
                NVDUMP_COMPONENT_ENG_NVDEC,
                NVDUMP_COMPONENT_ENG_DPU,
                NVDUMP_COMPONENT_ENG_FBFLCN,
                NVDUMP_COMPONENT_ENG_HDA,
                NVDUMP_COMPONENT_ENG_NVENC,
                NVDUMP_COMPONENT_ENG_GSP
            };
            NvU32 flags = REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _LOW) |
                          REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _GSP);
            for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(gspNvdEngines); i++)
            {
                nvdEngineSignUp(pGpu, pNvd, NULL, gspNvdEngines[i], flags, (void*)pGpu);
            }
        }
    }
}

