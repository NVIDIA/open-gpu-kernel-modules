/*
* SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *
 */

/*!
 * Provides GR100 and GR102 specific TdispMgr HAL implementations.
 *
 * NV_VIRTUAL_FUNCTION_REGION5_GSP_SCRATCH_RW is only defined on GR100 and GR102;
 * do not use these implementations for other Rubin chips.
 */

#include "rmconfig.h"

#include "gpu/conf_compute/conf_compute.h"

#include "gpu/tdisp/tdisp_mgr.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "os/os.h"
#include "spdm/rmspdmtransport.h"

#include "published/rubin/gr100/dev_gsp.h"
#include "published/rubin/gr100/dev_vm.h"

static NvBool
_tdispmgrIsTdiInErrorState
(
    OBJGPU *pGpu,
    void   *pVoid
)
{
    NvU32 dvsecTdiState = 0;
    NV_STATUS status;

    (void)pVoid;

    status = gpuReadBusConfigReg_HAL(
        pGpu,
        NV_TDISP_DVSEC0_HEADER_2_AND_GENERAL,
        &dvsecTdiState);
    if (status != NV_OK)
    {
        return NV_FALSE;
    }

    return (NV_TDISP_DVSEC0_TDI_STATE(dvsecTdiState) ==
            NV_TDISP_DVSEC0_TDI_STATE_ERROR);
}

static NV_STATUS
_tdispmgrWriteTdispUnloadCommand_GR100
(
    OBJGPU   *pGpu,
    TdispMgr *pTdispMgr,
    NvU32     command
)
{
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    IoAperture *pProtectedGspScratchAperture = NULL;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pTdispMgr != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGsp != NULL, NV_ERR_INVALID_STATE);

    status = kgspGetGspScratchAperture_HAL(
        pGpu, pKernelGsp, &pProtectedGspScratchAperture);
    if ((status != NV_OK) || (pProtectedGspScratchAperture == NULL))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get GSP_SCRATCH aperture for TDISP "
                  "unload command: 0x%x\n",
                  status);
        return (status != NV_OK) ? status : NV_ERR_INSUFFICIENT_RESOURCES;
    }

    REG_WR32(pProtectedGspScratchAperture,
             NV_VIRTUAL_FUNCTION_REGION5_GSP_SCRATCH_RW(0),
             command);

    kgspReleaseGspScratchAperture_HAL(
        pGpu, pKernelGsp, &pProtectedGspScratchAperture);
    return NV_OK;
}

/*!
 * Ask ACR to enter the TDISP unload path before the GSP-RM suspend command
 * starts ACR shutdown. TDISP waits for a later PROCEED command before moving
 * the TDI to ERROR.
 */
NV_STATUS
tdispmgrTriggerTdispUnload_GR100
(
    OBJGPU   *pGpu,
    TdispMgr *pTdispMgr
)
{
    if (_tdispmgrIsTdiInErrorState(pGpu, NULL))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Cannot start TDISP unload because TDI is already ERROR\n");
        return NV_ERR_INVALID_STATE;
    }

    NV_PRINTF(LEVEL_NOTICE, "Starting TDISP unload handoff\n");
    return _tdispmgrWriteTdispUnloadCommand_GR100(
        pGpu, pTdispMgr, NV_TDISP_DSM_UNLOAD_START);
}

/*!
 * Allow TDISP to complete unload after GSP-RM processor suspend is observed.
 */
NV_STATUS
tdispmgrProceedTdispUnload_GR100
(
    OBJGPU   *pGpu,
    TdispMgr *pTdispMgr
)
{
    if (_tdispmgrIsTdiInErrorState(pGpu, NULL))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Cannot proceed with TDISP unload because TDI is already ERROR\n");
        return NV_ERR_INVALID_STATE;
    }

    NV_PRINTF(LEVEL_NOTICE, "Proceeding with TDISP unload\n");
    return _tdispmgrWriteTdispUnloadCommand_GR100(
        pGpu, pTdispMgr, NV_TDISP_DSM_UNLOAD_PROCEED);
}

/*!
 * Wait for TDISP to publish a terminal unload result through the unprotected
 * lower GSP_SCRATCH0 alias.
 */
static NvBool
_tdispmgrIsTdispUnloadComplete
(
    OBJGPU *pGpu,
    void   *pVoid
)
{
    NvU32 *pUnprotectedGspScratch0 = (NvU32 *)pVoid;
    NvU32 unloadResult;

    (void)pGpu;

    unloadResult = MEM_RD32(pUnprotectedGspScratch0);

    return (unloadResult == NV_TDISP_DSM_UNLOAD_SUCCESS) ||
           (unloadResult == NV_TDISP_DSM_UNLOAD_FAILURE);
}

/*!
 * Wait for the terminal TDISP unload result.
 *
 * Decoder-trap slot 16 redirects reads of the lower GSP_SCRATCH0 alias to the
 * original CPU-RM GSP_SCRATCH0 range in RUN and ERROR states. Kernel-RM uses
 * that lower alias so it can poll the terminal unload result after TDI ERROR,
 * then releases the mapping before returning.
 */
NV_STATUS
tdispmgrWaitForTdispUnload_GR100
(
    OBJGPU   *pGpu,
    TdispMgr *pTdispMgr
)
{
    NV_STATUS status;
    NvU32 *pUnprotectedGspScratch0 = NULL;
    NvU32 unloadResult;

    NV_ASSERT_OR_RETURN(pTdispMgr != NULL, NV_ERR_INVALID_STATE);

    // Map GSP_SCRATCH0's lower alias to poll across the RUN-to-ERROR transition.
    pUnprotectedGspScratch0 = osMapKernelSpace(
        pGpu->busInfo.gpuPhysAddr +
            NV_TDISP_GSP_SCRATCH_PF_RW_ALIAS_REG(0),
        sizeof(*pUnprotectedGspScratch0),
        NV_MEMORY_UNCACHED,
        NV_PROTECT_READABLE);
    if (pUnprotectedGspScratch0 == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to create unprotected GSP_SCRATCH alias mapping "
                  "for TDISP unload result\n");
        return NV_ERR_OPERATING_SYSTEM;
    }

    status = gpuTimeoutCondWait(
        pGpu,
        _tdispmgrIsTdispUnloadComplete,
        pUnprotectedGspScratch0,
        NULL);
    unloadResult = MEM_RD32(pUnprotectedGspScratch0);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Timed out waiting for TDISP unload result: "
                  "GSP_SCRATCH0=0x%08x\n",
                  unloadResult);
        goto done;
    }

    if (unloadResult != NV_TDISP_DSM_UNLOAD_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "TDISP unload failed: GSP_SCRATCH0=0x%08x\n",
                  unloadResult);
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    if (!_tdispmgrIsTdiInErrorState(pGpu, NULL))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "TDI did not reach ERROR state after TDISP unload: "
                  "GSP_SCRATCH0=0x%08x\n",
                  unloadResult);
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    NV_PRINTF(LEVEL_NOTICE, "TDISP unload completed successfully\n");

done:
    if (pUnprotectedGspScratch0 != NULL)
    {
        osUnmapKernelSpace(pUnprotectedGspScratch0,
                           sizeof(*pUnprotectedGspScratch0));
    }
    return status;
}

