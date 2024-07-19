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

/****************************************************************************
*
*       Kernel Perf Module
*       This file contains functions managing perf on CPU RM
*
****************************************************************************/

/* ------------------------ Includes --------------------------------------- */
#include "gpu/perf/kern_perf.h"
#include "gpu/perf/kern_perf_boost.h"
#include "gpu/timer/objtmr.h"
#include "platform/platform_request_handler.h"
#include "platform/chipset/chipset.h"

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Functions ------------------------------- */
/*!
 * @copydoc kperfConstructEngine
 */
NV_STATUS
kperfConstructEngine_IMPL(OBJGPU *pGpu, KernelPerf *pKernelPerf, ENGDESCRIPTOR engDesc)
{
    return NV_OK;
}

/*!
 * @copydoc kperfStateInitLocked
 */
NV_STATUS
kperfStateInitLocked_IMPL(OBJGPU *pGpu, KernelPerf *pKernelPerf)
{

    NV_STATUS status = NV_OK;

    // Initialize SW state corresponding to SLI GPU Boost synchronization.
    status = kperfGpuBoostSyncStateInit(pGpu, pKernelPerf);

    {
        OBJSYS                  *pSys                    = SYS_GET_INSTANCE();
        PlatformRequestHandler  *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);


        // Initialize PFM_REQ_HNDLR module which is a child of OBJSYS
        if (pPlatformRequestHandler != NULL)
        {
            pfmreqhndlrStateInit(pPlatformRequestHandler);
        }
    }

    return status;
}

/*!
 * @copydoc kperfStateLoad
 */
NV_STATUS
kperfStateLoad_IMPL(OBJGPU *pGpu, KernelPerf *pKernelPerf, NvU32 flags)
{
    // Initialize SW state corresponding to SLI GPU Boost synchronization.
    kperfGpuBoostSyncStateInit(pGpu, pKernelPerf);

    {
        OBJSYS                  *pSys                    = SYS_GET_INSTANCE();
        OBJCL                   *pCl                     = SYS_GET_CL(pSys);
        PlatformRequestHandler  *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);


        // Load PFM_REQ_HNDLR module which is a child of OBJSYS
        // Skip pfmreqhndlrStateLoad on eGPU
        if (!(pCl->getProperty(pCl, PDB_PROP_CL_IS_EXTERNAL_GPU)) &&
             (pPlatformRequestHandler != NULL))
        {
            pfmreqhndlrStateLoad(pPlatformRequestHandler);
        }
    }

    return NV_OK;
}

/*!
 * @copydoc kperfStateUnload
*/
NV_STATUS
kperfStateUnload_IMPL
(
    OBJGPU        *pGpu,
    KernelPerf    *pKernelPerf,
    NvU32          flags
)
{
    {
        OBJSYS                  *pSys                    = SYS_GET_INSTANCE();
        PlatformRequestHandler  *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);

        if ((pPlatformRequestHandler != NULL))
        {
            // Unload PFM_REQ_HNDLR module which is a child of OBJSYS
            pfmreqhndlrStateUnload(pPlatformRequestHandler);
        }
    }

    return NV_OK;
}

/*!
 * @copydoc kperfStateDestroy
*/
void
kperfStateDestroy_IMPL
(
    OBJGPU        *pGpu,
    KernelPerf    *pKernelPerf
)
{
    {
        OBJSYS                  *pSys                    = SYS_GET_INSTANCE();
        PlatformRequestHandler  *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);

        // destroy PFM_REQ_HNDLR module state
        if (pPlatformRequestHandler != NULL)
        {
            pfmreqhndlrStateDestroy(pPlatformRequestHandler);
        }
    }
}

/*!
 * @copydoc kperfReentrancy
 *
 * @note This is 1:1 copy from Physical RM to the Kernel RM
 */
NV_STATUS kperfReentrancy_IMPL(OBJGPU *pGpu, KernelPerf *pKernelPerf, NvU32 function, NvBool bSet)
{
    // This routine and all routines that this routine calls are forbidden.
    NvU32 reentrancyMask = function | (function - 1);

    if (bSet)
    {
        // Catches routines that reentered.
        reentrancyMask &= pKernelPerf->reentrancyMask;
        pKernelPerf->reentrancyMask |= function;
    }
    else
    {
        // Catches routines that were not exited or were exited twice.
        reentrancyMask &= (pKernelPerf->reentrancyMask ^ function);
        pKernelPerf->reentrancyMask &= ~function;
    }

    if (pKernelPerf->reentrancyMask & reentrancyMask)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Code reentered.  function %02x, reentered %02x set %d\n",
                  function, reentrancyMask, bSet);

        // This assert causes failures in MODS Sanity PVS.
        //NV_ASSERT(reentrancyMask, 0);   // Log all reentrancy errors.

        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

/* ------------------------- Private Functions ------------------------------ */

