/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides GB100+ specific KernelGsp ECC HAL implementations.
 */

#include "rmconfig.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/rc/kernel_rc.h"
#include "gpu/gpu_mods_error.h"

#include "ctrl/ctrl2080/ctrl2080gpu.h"

#include "published/blackwell/gb100/dev_gsp.h"

/*!
 * Check if there is a GSP ECC error pending
 */
NvBool
kgspEccIsErrorPending_GB100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU32      intrStatus
)
{
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);

    return (intrStatus & kflcnGetEccInterruptMask_HAL(pGpu, pKernelFalcon) ) != 0;
}

/*!
 * GSP ECC error service routine
 */
void
kgspEccServiceEvent_GB100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 eccIntrStatus = GPU_REG_RD32(pGpu, NV_PGSP_ECC_INTR_STATUS);

    NV_ASSERT_OR_RETURN_VOID(eccIntrStatus != 0);

    if (FLD_TEST_DRF(_PGSP, _ECC_INTR_STATUS, _UNCORRECTED, _PENDING, eccIntrStatus))
    {
        kgspEccServiceUncorrError_HAL(pGpu, pKernelGsp);
    }

    // Only parity is supported right now
    if (FLD_TEST_DRF(_PGSP, _ECC_INTR_STATUS, _CORRECTED, _PENDING, eccIntrStatus))
    {
        NV_ASSERT_FAILED("Corrected errors are not supported");
    }

    GPU_REG_WR32(pGpu, NV_PGSP_FALCON_ECC_STATUS, DRF_DEF(_PGSP, _FALCON_ECC_STATUS, _RESET, _TASK));
}

/*!
 * GSP ECC uncorrected error service routine
 */
void
kgspEccServiceUncorrError_GB100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    NvU32     eccStatus = GPU_REG_RD32(pGpu, NV_PGSP_FALCON_ECC_STATUS);

    NV_ASSERT_OR_RETURN_VOID(eccStatus != 0);

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_IMEM, _PENDING, eccStatus))
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_IMEM PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_IMEM\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_IMEM, eccStatus);
    }

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_DMEM, _PENDING, eccStatus))
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DMEM PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DMEM\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_DMEM, eccStatus);
    }

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_ICACHE, _PENDING, eccStatus))
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_ICACHE PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_ICACHE\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_ICACHE, eccStatus);
    }

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_DCACHE, _PENDING, eccStatus))
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCACHE PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCACHE\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_DCACHE, eccStatus);
    }

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_MPU_RAM, _PENDING, eccStatus))
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_MPU_RAM PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_MPU_RAM\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_MPU_RAM, eccStatus);
    }

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_DCLS, _PENDING, eccStatus))
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCLS PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCLS\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_DCLS, eccStatus);
    }

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_REG, _PENDING, eccStatus))
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_REG PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_REG\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_REG, eccStatus);
    }

    if (FLD_TEST_DRF(_PGSP, _FALCON_ECC_STATUS, _UNCORRECTED_ERR_EMEM, _PENDING, eccStatus))
    {
        //
        // The the other ECC errors have a counterpart in FAULT_CONTAINMENT_SRCSTAT where
        // recovery happens but this one doesn't so it needs handling here
        //
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_EMEM PENDING\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_EMEM\n");
        MODS_REPORT_GSP_ERROR(pGpu, MODSDRV_ERROR_SEVERITY_FATAL,
                              MODSDRV_FALCON_ERROR_CODE_ECC_EMEM, eccStatus);

        pKernelGsp->bFatalError = NV_TRUE;

        if (pKernelRc != NULL)
        {
            krcRcAndNotifyAllChannels(pGpu, pKernelRc, ROBUST_CHANNEL_GPU_ECC_DBE, NV_TRUE);
        }

        NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
    }

    // Notify of and log the error
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_ECC_DBE, NULL, 0, 0,
                            (NvU16)NV2080_CTRL_GPU_ECC_UNIT_GSP);
    nvErrorLog_va((void *)pGpu, ROBUST_CHANNEL_GPU_ECC_DBE, "An uncorrectable ECC error has been detected on GPU in GSP-RISCV");
    nvErrorLog_va((void *)pGpu, UNCORRECTABLE_SRAM_ERROR, "GSP-RISCV, Uncorrectable SRAM error");
}
