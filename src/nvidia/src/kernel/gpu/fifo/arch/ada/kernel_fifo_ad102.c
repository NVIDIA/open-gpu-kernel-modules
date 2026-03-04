/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/mmu/kern_gmmu.h"

#include "published/ada/ad102/dev_fault.h"

/*!
 * @brief Converts a mmu engine id (NV_PFAULT_MMU_ENG_ID_*) into a string.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] engineID NV_PFAULT_MMU_ENG_ID_*
 *
 * @returns a string (always non-null)
 */
const char*
kfifoPrintInternalEngine_AD102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineID
)
{
    NV_STATUS   status = NV_OK;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32       engTag;

    if (kfifoIsMmuFaultEngineIdPbdma(pGpu, pKernelFifo, engineID))
    {
        return kfifoPrintFaultingPbdmaEngineName(pGpu, pKernelFifo, engineID);
    }

    if (kgmmuIsFaultEngineBar1_HAL(pKernelGmmu, engineID))
    {
        return "BAR1";
    }
    else if (kgmmuIsFaultEngineBar2_HAL(pKernelGmmu, engineID))
    {
        return "BAR2";
    }

    switch (engineID)
    {
        case NV_PFAULT_MMU_ENG_ID_DISPLAY:
            return "DISPLAY";
        case NV_PFAULT_MMU_ENG_ID_IFB:
            return "IFB";
        case NV_PFAULT_MMU_ENG_ID_SEC:
            return "SEC";
        case NV_PFAULT_MMU_ENG_ID_PERF:
            return "PERF";
        case NV_PFAULT_MMU_ENG_ID_NVDEC0:
            return "NVDEC0";
        case NV_PFAULT_MMU_ENG_ID_NVDEC1:
            return "NVDEC1";
        case NV_PFAULT_MMU_ENG_ID_NVDEC2:
            return "NVDEC2";
        case NV_PFAULT_MMU_ENG_ID_NVDEC3:
            return "NVDEC3";
        case NV_PFAULT_MMU_ENG_ID_CE0:
            return "CE0";
        case NV_PFAULT_MMU_ENG_ID_CE1:
            return "CE1";
        case NV_PFAULT_MMU_ENG_ID_CE2:
            return "CE2";
        case NV_PFAULT_MMU_ENG_ID_CE3:
            return "CE3";
        case NV_PFAULT_MMU_ENG_ID_CE4:
            return "CE4";
        case NV_PFAULT_MMU_ENG_ID_CE5:
            return "CE5";
        case NV_PFAULT_MMU_ENG_ID_PWR_PMU:
            return "PMU";
        case NV_PFAULT_MMU_ENG_ID_PTP:
            return "PTP";
        case NV_PFAULT_MMU_ENG_ID_NVENC0:
            return "NVENC0";
        case NV_PFAULT_MMU_ENG_ID_NVENC1:
            return "NVENC1";
        case NV_PFAULT_MMU_ENG_ID_NVENC2:
            return "NVENC2";
        case NV_PFAULT_MMU_ENG_ID_PHYSICAL:
            return "PHYSICAL";
        case NV_PFAULT_MMU_ENG_ID_NVJPG0:
            return "NVJPG0";
        case NV_PFAULT_MMU_ENG_ID_NVJPG1:
            return "NVJPG1";
        case NV_PFAULT_MMU_ENG_ID_NVJPG2:
            return "NVJPG2";
        case NV_PFAULT_MMU_ENG_ID_NVJPG3:
            return "NVJPG3";
        case NV_PFAULT_MMU_ENG_ID_OFA0:
            return "OFA";
        case NV_PFAULT_MMU_ENG_ID_FLA:
            return "FLA";
        default:
        {
            const char *engine =
                    kfifoPrintInternalEngineCheck_HAL(pGpu, pKernelFifo, engineID);
            if (engine != NULL)
            {
                return engine;
            }
        }
    }

    status = kfifoEngineInfoXlate_HAL(pGpu,
                                      pKernelFifo,
                                      ENGINE_INFO_TYPE_MMU_FAULT_ID,
                                      engineID,
                                      ENGINE_INFO_TYPE_ENG_DESC,
                                      &engTag);
    if ((NV_OK == status) && IS_GR(engTag))
    {
        switch (engTag)
        {
            case ENG_GR(0):
                return "GRAPHICS";
            case ENG_GR(1):
                return "GR1";
            case ENG_GR(2):
                return "GR2";
            case ENG_GR(3):
                return "GR3";
            case ENG_GR(4):
                return "GR4";
            case ENG_GR(5):
                return "GR5";
            case ENG_GR(6):
                return "GR6";
            case ENG_GR(7):
                return "GR7";
        }
    }

    return "UNKNOWN";
}

/**
 * @brief Converts a subid/clientid into a client string
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pMmuExceptData

 * @returns a string (always non-null)
 */
const char*
kfifoGetClientIdString_AD102
(
    OBJGPU                  *pGpu,
    KernelFifo              *pKernelFifo,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptInfo
)
{
    if (!pMmuExceptInfo->bGpc)
    {
        switch (pMmuExceptInfo->clientId)
        {
            case NV_PFAULT_CLIENT_HUB_NVJPG1:
                return "HUBCLIENT_NVJPG1";
            case NV_PFAULT_CLIENT_HUB_NVJPG2:
                return "HUBCLIENT_NVJPG2";
            case NV_PFAULT_CLIENT_HUB_NVJPG3:
                return "HUBCLIENT_NVJPG3";
        }
    }
    // Fallback to GA100 HAL if above did not hit
    return kfifoGetClientIdString_GA100(pGpu, pKernelFifo, pMmuExceptInfo);
}
