/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**************************************************************************************************************
*
*   Description:
*       Sanity test the system environment to verify our driver can run properly
*
**************************************************************************************************************/

#include <core/core.h>
#include <os/os.h>
#include <gpu/gpu.h>
#include <gpu_mgr/gpu_mgr.h>
#include "kernel/gpu/intr/intr.h"
#include <gpu/bif/kernel_bif.h>

/*!
 * @brief Wait for interrupt
 *
 * @param[in]   pGpu        GPU Object
 * @param[in]   serviced    NV_TRUE if a GPU interrupt was serviced
 *
 * @returns NV_FALSE???
 */

static int interrupt_triggered;

static NvBool osWaitForInterrupt(
    OBJGPU *pGpu,
    NvBool *serviced
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);
    NvU32 intAMode;
    NvBool swPending;

    intrGetStallInterruptMode_HAL(pGpu, pIntr, &intAMode, &swPending);

    //
    // If we see a GPU here that has interrupts disabled, it must
    // share its IRQ with some other device. Don't do anything,
    // if there are additional GPUs, they will be probed soon enough.
    //
    if ((intAMode == INTERRUPT_TYPE_DISABLED) ||
        ((intAMode == INTERRUPT_TYPE_SOFTWARE) && !swPending))
    {
        if (serviced) *serviced = NV_FALSE;
        return NV_FALSE;
    }

    //
    // this should never happen, but hey, lots of things are like that in SW
    // (this references the assert below these comments)
    // The reason it can't happen:
    //
    // If osWaitForInterrupt is wired up, then following the logic of _osVerifyInterrupts()
    // this GPU must be constrained to generating interrupts from the software interrupt
    // pending bit only.
    //
    // If osWaitForInterrupt() is wired up, and we get here, and we are not in software interrupt mode
    // then there is a re-entrancy problem that needs to be addressed in your flavor of the RM.
    //
    if (intAMode != INTERRUPT_TYPE_SOFTWARE)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "INTR_EN_0_INTA_SOFTWARE was not set on gpuInstance: 0x%x\n",
                  pGpu->gpuInstance);
        NV_ASSERT(0);
    }

    intrClearStallSWIntr_HAL(pGpu, pIntr);

    NV_PRINTF(LEVEL_INFO, "Triggered for gpuInstance: 0x%x\n",
              pGpu->gpuInstance);

    interrupt_triggered = 1;
    if (serviced) *serviced = NV_TRUE;

    return NV_FALSE;
}

NV_STATUS osSanityTestIsr(
    OBJGPU *pGpu
)
{
    NvBool serviced = NV_FALSE;

    osWaitForInterrupt(pGpu, &serviced);

    return (serviced) ? NV_OK : NV_ERR_INVALID_STATE;
}

//
// IRQ routing verification  is currently done on Windows and Linux
// For Windows this is done both during bootUp and resume whereas for linux
// this is done only during bootup.
//
static NV_STATUS _osVerifyInterrupts(
    OBJGPU *pGpu
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    OBJGPU *pGpuSaved = pGpu;
    NvU32  *pIntrEn0, *pIntrEn1;
    MC_ENGINE_BITVECTOR intrMask;
    MC_ENGINE_BITVECTOR engines;
    NvU32  Bailout;
    NvU32  gpuAttachCnt, gpuAttachMask, gpuInstance, i;

    //
    // We're adding the PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY check since none of the
    // support required to run this interrupt sanity test has been brought up
    // yet for T234D SOC display.
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_IGPU))
    {
        //
        // Nothing to verify here for the time being
        //
        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO, "called with gpuInstance: 0x%x\n",
              pGpu->gpuInstance);

    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    pIntrEn0 = portMemAllocNonPaged(gpuAttachCnt * sizeof(NvU32));
    if (pIntrEn0 == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pIntrEn0 portMemAllocNonPaged failed!\n");
        return NV_ERR_NO_MEMORY;
    }
    pIntrEn1 = portMemAllocNonPaged(gpuAttachCnt * sizeof(NvU32));
    if (pIntrEn1 == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pIntrEn1 portMemAllocNonPaged failed!\n");
        portMemFree(pIntrEn0);
        return NV_ERR_NO_MEMORY;
    }

    //
    // Normally, interrupts are disabled while we have the semaphore.  However in locking
    // models that support altIntr or intrMask, intrs may be left enabled.  So we really
    // need to disable intrs on all GPUs before running this test and switching over
    // our ISR to the dummy one.
    //
    // In this case, we enable interrupts so we can catch a specifically triggered interrupt
    // then we restore the initial state. this should be early enough that no other
    // interrupts are triggered than the software one we explicitly throw.
    // note that we only enable the software interrupt, so we won't get any unexpected hw interrupts
    //
    // Concerning HW interrupts while software interrupts
    // are enabled.
    //
    // "Yes, if INTERRUPT_TYPE is set to SOFTWARE, all the other
    // [hardware] interrupts can no longer assert.
    //
    // I'm surprised the manual doesn't have a #define for BOTH (0x3).
    // That would allow both SW and HW interrupts to assert."
    //
    gpuInstance = i = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        if (gpuIsGpuFullPower(pGpu) == NV_FALSE)
        {
            continue;
        }
        pIntr = GPU_GET_INTR(pGpu);

        // Save off the current state of the non stall interrupt tree
        pIntrEn1[i] = intrGetNonStallEnable_HAL(pGpu, pIntr, NULL /* threadstate */);

        // Disable the non stall interrupt tree
        intrDisableNonStall_HAL(pGpu, pIntr, NULL /* threadstate */);

        pIntr = GPU_GET_INTR(pGpu);
        pIntrEn0[i] = intrGetIntrEnFromHw_HAL(pGpu, pIntr, NULL /* threadstate */);
        intrSetIntrEnInHw_HAL(pGpu, pIntr, INTERRUPT_TYPE_DISABLED, NULL /* threadstate */);
        i++;
    }
    pGpu = pGpuSaved;
    pIntr = GPU_GET_INTR(pGpu);

    intrSetIntrEnInHw_HAL(pGpu, pIntr, INTERRUPT_TYPE_SOFTWARE, NULL /* threadstate */);
    intrDisableStallSWIntr_HAL(pGpu, pIntr);
    intrEnableStallSWIntr_HAL(pGpu, pIntr);
    if (pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING))
    {
        //
        // Save off the IntrMask and enable all engines to be reflected
        // in PMC_INTR_0
        //
        intrGetIntrMask_HAL(pGpu, pIntr, &intrMask, NULL /* threadstate */);
        bitVectorSetAll(&engines);
        intrSetIntrMask_HAL(pGpu, pIntr, &engines, NULL /* threadstate */);
    }

    //
    // Hook up our dummy ISR to catch the SW triggered test after
    // we've set INTR_EN_0 to _SOFTWARE and enabled all intrs
    // to be seen via the IntrMask
    //

    pGpu->testIntr = NV_TRUE;

    interrupt_triggered = 0;

    intrSetStallSWIntr_HAL(pGpu, pIntr);

    Bailout = 0;

    while (!interrupt_triggered)
    {
        osDelayUs(5);
        Bailout += 5;

        if (Bailout > pGpu->timeoutData.defaultus)
            break;
    }

    //
    // Message Signalled Interrupt (MSI) support
    // This call checks if MSI is enabled and if it is, we need re-arm it.
    //
    kbifCheckAndRearmMSI(pGpu, pKernelBif);

    pGpu->testIntr = NV_FALSE;

    if (pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING))
    {
        // Restore the IntrMask that we saved off
        intrSetIntrMask_HAL(pGpu, pIntr, &intrMask, NULL /* threadstate */);
    }

    // Disable the SW interrupt explicitly before restoring interrupt enables
    intrDisableStallSWIntr_HAL(pGpu, pIntr);

    // Restore NV_PMC_INTR_EN_0 and INTR_EN_1 on all GPUs
    gpuInstance = i = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        if (gpuIsGpuFullPower(pGpu) == NV_FALSE)
        {
            continue;
        }
        pIntr = GPU_GET_INTR(pGpu);

        // Restore the non stall interrupt tree enable
        intrRestoreNonStall_HAL(pGpu, pIntr, pIntrEn1[i], NULL /* threadState */);

        // Restore the stall interrupt tree enable
        intrSetIntrEnInHw_HAL(pGpu, pIntr, pIntrEn0[i], NULL /* threadstate */);
        i++;
    }

    portMemFree(pIntrEn0);
    portMemFree(pIntrEn1);

    NV_PRINTF(LEVEL_INFO, "Finishing with %d\n", interrupt_triggered);

    return interrupt_triggered ? NV_OK : NV_ERR_IRQ_NOT_FIRING;
}

//
// add various system environment start-up tests here
// currently, just verify interrupt hookup, but could also verify other details
//
NV_STATUS osVerifySystemEnvironment(
    OBJGPU *pGpu
)
{
    NV_STATUS status = NV_OK;

    // PCIE GEN4 fmodel MSI is broken as per bug 2076928 comment 42 and will not be fixed
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    if (IS_FMODEL(pGpu)                                                        &&
        pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN) &&
        kbifIsMSIEnabled(pGpu, pKernelBif))
    {
        return NV_OK;
    }

    status = _osVerifyInterrupts(pGpu);

    return status;
}

