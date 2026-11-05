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
*/

/*!
* Provides GR100+ specific KernelGsp ECC HAL implementations.
*/

#include "rmconfig.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/rc/kernel_rc.h"

#include "events/gpu/ras/ras_events.h"
#include "nvoc/event_bus.h"

#include "ctrl/ctrl2080/ctrl2080gpu.h"

#include "published/rubin/gr100/dev_gsp.h"

/*!
 * Check if there is a GSP ECC error pending
 */
NvBool
kgspEccIsErrorPending_GR100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU32      intrStatus
)
{
    NvU32  missionErrStatus = GPU_REG_RD32(pGpu, NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS);
    NvU32  latentErrStatus  = GPU_REG_RD32(pGpu, NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS);
    NvBool bPriError        = ((missionErrStatus & GPU_READ_PRI_ERROR_MASK) == GPU_READ_PRI_ERROR_CODE) ||
                              ((latentErrStatus & GPU_READ_PRI_ERROR_MASK)  == GPU_READ_PRI_ERROR_CODE);

    if (bPriError)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot read GSP EC registers: missionErrStatus 0x%x, latentErrStatus 0x%x\n",
                  missionErrStatus, latentErrStatus);
        return NV_FALSE;
    }

    return (missionErrStatus != 0 || latentErrStatus != 0);
}

/*!
* GSP ECC error service routine
*/
void
kgspEccServiceEvent_GR100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32  missionErrStatus    = GPU_REG_RD32(pGpu, NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS);
    NvU32  latentErrStatus     = GPU_REG_RD32(pGpu, NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS);
    NvU32  errorIdx;
    NvBool bUncorrErrorPending = NV_TRUE;

#define REPORT_EC_STATUS_CASE(type, idx)                                         \
    case DRF_BASE(NV_PGSP_EC_ERRSLICE0_##type##_STATUS_ERR##idx):                \
    {                                                                            \
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_EC_ERRSLICE0_%s_STATUS_ERR%d PENDING\n", \
                  NV_STRINGIFY(type),                                            \
                  errorIdx);                                                     \
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_EC_ERRSLICE0_%s_STATUS_ERR%d\n",         \
                  NV_STRINGIFY(type),                                            \
                  errorIdx);                                                     \
        break;                                                                   \
    }

    // Service mission errors
    FOR_EACH_INDEX_IN_MASK(64, errorIdx, missionErrStatus)
    {
        switch (errorIdx)
        {
            REPORT_EC_STATUS_CASE(MISSIONERR, 0)
            REPORT_EC_STATUS_CASE(MISSIONERR, 1)
            REPORT_EC_STATUS_CASE(MISSIONERR, 2)
            REPORT_EC_STATUS_CASE(MISSIONERR, 3)
            REPORT_EC_STATUS_CASE(MISSIONERR, 4)
            REPORT_EC_STATUS_CASE(MISSIONERR, 5)
            REPORT_EC_STATUS_CASE(MISSIONERR, 6)
            REPORT_EC_STATUS_CASE(MISSIONERR, 7)
            REPORT_EC_STATUS_CASE(MISSIONERR, 8)
            REPORT_EC_STATUS_CASE(MISSIONERR, 9)
            REPORT_EC_STATUS_CASE(MISSIONERR, 10)
            REPORT_EC_STATUS_CASE(MISSIONERR, 11)
            REPORT_EC_STATUS_CASE(MISSIONERR, 12)
            REPORT_EC_STATUS_CASE(MISSIONERR, 14)
            REPORT_EC_STATUS_CASE(MISSIONERR, 15)
            REPORT_EC_STATUS_CASE(MISSIONERR, 16)
            REPORT_EC_STATUS_CASE(MISSIONERR, 17)
            REPORT_EC_STATUS_CASE(MISSIONERR, 18)
            case DRF_BASE(NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR13):
            {
                NV_PRINTF(LEVEL_ERROR, "NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR%d PENDING\n",
                          errorIdx);
                MODS_ARCH_ERROR_PRINTF("NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR%d\n", errorIdx);

                bUncorrErrorPending = NV_FALSE;
                break;
            }
            default:
            {
                NV_PRINTF(LEVEL_ERROR, "Invalid mission error: 0x%x\n", errorIdx);
                bUncorrErrorPending = NV_FALSE;
                break;
            }
        }

        if (bUncorrErrorPending)
        {
            kgspEccServiceUncorrErrorIndex_HAL(pGpu, pKernelGsp, errorIdx);
        }

        // Reset error pending flag
        bUncorrErrorPending = NV_TRUE;
    }
    FOR_EACH_INDEX_IN_MASK_END

    // Service latent errors
    FOR_EACH_INDEX_IN_MASK(64, errorIdx, latentErrStatus)
    {
        switch (errorIdx)
        {
            REPORT_EC_STATUS_CASE(LATENTERR, 0)
            REPORT_EC_STATUS_CASE(LATENTERR, 1)
            REPORT_EC_STATUS_CASE(LATENTERR, 2)
            REPORT_EC_STATUS_CASE(LATENTERR, 3)
            REPORT_EC_STATUS_CASE(LATENTERR, 4)
            REPORT_EC_STATUS_CASE(LATENTERR, 5)
            REPORT_EC_STATUS_CASE(LATENTERR, 6)
            REPORT_EC_STATUS_CASE(LATENTERR, 7)
            REPORT_EC_STATUS_CASE(LATENTERR, 8)
            REPORT_EC_STATUS_CASE(LATENTERR, 9)
            REPORT_EC_STATUS_CASE(LATENTERR, 10)
            REPORT_EC_STATUS_CASE(LATENTERR, 11)
            REPORT_EC_STATUS_CASE(LATENTERR, 12)
            REPORT_EC_STATUS_CASE(LATENTERR, 14)
            REPORT_EC_STATUS_CASE(LATENTERR, 15)
            REPORT_EC_STATUS_CASE(LATENTERR, 16)
            REPORT_EC_STATUS_CASE(LATENTERR, 17)
            REPORT_EC_STATUS_CASE(LATENTERR, 18)
            case DRF_BASE(NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR13):
            {
                NV_PRINTF(LEVEL_ERROR, "NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR%d PENDING\n",
                          errorIdx);
                MODS_ARCH_ERROR_PRINTF("NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR%d\n", errorIdx);

                // Not an ECC error
                bUncorrErrorPending = NV_FALSE;
                break;
            }
            default:
            {
                NV_PRINTF(LEVEL_ERROR, "Invalid latent error: 0x%x\n", errorIdx);
                bUncorrErrorPending = NV_FALSE;
                break;
            }
        }

        if (bUncorrErrorPending)
        {
            kgspEccServiceUncorrErrorIndex_HAL(pGpu, pKernelGsp, errorIdx);
        }

        // Reset error pending flag
        bUncorrErrorPending = NV_TRUE;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    GPU_REG_WR32(pGpu, NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS, missionErrStatus);
    GPU_REG_WR32(pGpu, NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS, latentErrStatus);

#undef REPORT_EC_STATUS_CASE
}

/*!
* GSP ECC uncorrected error service routine
*/
void
kgspEccServiceUncorrErrorIndex_GR100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU32      errorIdx
)
{
    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

    // Notify of and log the error
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_ECC_DBE, NULL, 0, 1,
                            (NvU16)NV2080_CTRL_GPU_ECC_UNIT_GSP);
    eventEmit(GspUncorrectableError, pGpu, NV_TRUE, errorIdx);

    if (errorIdx == DRF_BASE(NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR18))
    {
        //
        // The the other ECC errors have a counterpart in FAULT_CONTAINMENT_SRCSTAT where
        // recovery happens but this one doesn't so it needs handling here
        //
        pKernelGsp->bFatalError = NV_TRUE;

        if (pKernelRc != NULL)
        {
            krcRcAndNotifyAllChannels(pGpu, pKernelRc, ROBUST_CHANNEL_GPU_ECC_DBE, NV_TRUE);
        }

        NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
    }
}
