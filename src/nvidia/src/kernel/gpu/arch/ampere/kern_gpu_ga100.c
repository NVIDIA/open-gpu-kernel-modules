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

#include "gpu/gpu.h"
#include "kernel/gpu/intr/intr.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "published/ampere/ga100/dev_fb.h"
#include "published/ampere/ga100/dev_vm.h"
#include "published/ampere/ga100/dev_fuse.h"

/*!
 * @brief Read fuse for display supported status.
 *        Some chips not marked displayless do not support display
 */
NvBool
gpuFuseSupportsDisplay_GA100
(
    OBJGPU *pGpu
)
{
    return GPU_FLD_TEST_DRF_DEF(pGpu, _FUSE, _STATUS_OPT_DISPLAY, _DATA, _ENABLE);
}

/*!
 * @brief Clear FBHUB POISON Interrupt state for Bug 2924523.
 * This HAL handles the CPU interrupt tree
 *
 * @param[in]      pGpu           OBJGPU pointer
 *
 * @return NV_OK if success, else appropriate NV_STATUS code
 */
NV_STATUS
gpuClearFbhubPoisonIntrForBug2924523_GA100_KERNEL
(
    OBJGPU *pGpu
)
{
    NvU32 intrVector = NV_PFB_FBHUB_POISON_INTR_VECTOR_HW_INIT;
    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    NvU32 bit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);
    NvV32 intr = 0;

    if (pGpu == NULL)
        return NV_OK;

    //
    // Check if FBHUB Poison interrupt got triggered before RM Init due
    // to VBIOS IFR on GA100. If yes, clear the FBHUB Interrupt. This WAR is
    // required for Bug 2924523 as VBIOS IFR causes FBHUB Poison intr.
    //
    intr = GPU_VREG_RD32_EX(pGpu,
                            NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(reg),
                            NULL) &
                            NVBIT(bit);

    if (intr != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "FBHUB Interrupt detected = 0x%X. Clearing it.\n", intr);

        // Clear FBHUB Poison interrupt
        GPU_VREG_WR32_EX(pGpu,
                         NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(reg),
                         NVBIT(bit),
                         NULL);
    }

    return NV_OK;
}

/*!
 * @brief Returns FLA VASpace Size for Ampere
 *
 * @param[in] pGpu                        OBJGPU pointer
 * @param[in] bNvSwitchVirtualization     boolean
 *
 * @returns NvU64 -> size of FLA VASpace
 */
NvU64
gpuGetFlaVasSize_GA100
(
    POBJGPU pGpu,
    NvBool  bNvswitchVirtualization
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64  totalFbSize = (pMemoryManager->Ram.fbTotalMemSizeMb << 20);

    if (bNvswitchVirtualization || totalFbSize <= NVBIT64(36))
    {
        return 0x2000000000;  // 128GB
    }
    else
    {
        return (totalFbSize * 2);
    }
}

//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//

static const GPUCHILDPRESENT gpuChildrenPresent_GA100[] =
{
    { classId(OBJSWENG), 1 },
    { classId(OBJUVM), 1 },
    { classId(OBJACR), 1 },
    { classId(OBJBIF), 1 },
    { classId(KernelBif), 1 },
    { classId(OBJBSP), 5 },
    { classId(OBJBUS), 1 },
    { classId(KernelBus), 1 },
    { classId(OBJCE), 10 },
    { classId(KernelCE), 10 },
    { classId(OBJCIPHER), 1 },
    { classId(ClockManager), 1 },
    { classId(OBJDISP), 1 },
    { classId(KernelDisplay), 1 },
    { classId(VirtMemAllocator), 1 },
    { classId(OBJDPAUX), 1 },
    { classId(OBJFAN), 1 },
    { classId(OBJHSHUB), 2 },
    { classId(MemorySystem), 1 },
    { classId(KernelMemorySystem), 1 },
    { classId(MemoryManager), 1 },
    { classId(OBJFBFLCN), 1 },
    { classId(KernelFifo), 1 },
    { classId(OBJFIFO), 1 },
    { classId(OBJGMMU), 1 },
    { classId(KernelGmmu), 1},
    { classId(OBJGPULOG), 1 },
    { classId(OBJGPUMON), 1 },
    { classId(GraphicsManager), 1 },
    { classId(MIGManager), 1},
    { classId(KernelMIGManager), 1 },
    { classId(KernelGraphicsManager), 1 },
    { classId(Graphics), 8 },
    { classId(KernelGraphics), 8 },
    { classId(OBJHDACODEC), 1 },
    { classId(OBJHWPM), 1 },
    { classId(OBJINFOROM), 1 },
    { classId(Intr), 1 },
    { classId(Lpwr   ), 1 },
    { classId(OBJLSFM), 1 },
    { classId(OBJMC), 1 },
    { classId(KernelMc), 1 },
    { classId(PrivRing), 1 },
    { classId(SwIntr), 1 },
    { classId(OBJNVJPG), 1 },
    { classId(NvDebugDump), 1 },
    { classId(KernelNvlink), 1 },
    { classId(Nvlink), 1 },
    { classId(Perf), 1 },
    { classId(KernelPerf), 1 },
    { classId(Pmgr), 1 },
    { classId(Pmu), 1 },
    { classId(KernelPmu), 1 },
    { classId(OBJSEC2), 1 },
    { classId(Gsp), 1 },
    { classId(Therm), 1 },
    { classId(OBJTMR), 1 },
    { classId(OBJVOLT), 1 },
    { classId(OBJGRIDDISPLAYLESS), 1 },
    { classId(OBJFAS), 1 },
    { classId(OBJVMMU), 1 },
    { classId(OBJOFA), 1 },
    { classId(KernelNvdec), 1 },
    { classId(KernelSec2), 1 },
    { classId(KernelGsp), 1 },
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GA100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS32(gpuChildrenPresent_GA100);
    return gpuChildrenPresent_GA100;
}

//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//
static const GPUCHILDPRESENT gpuChildrenPresent_GA102[] =
{
    {classId(OBJSWENG), 1},
    {classId(OBJUVM), 1},
    {classId(OBJACR), 1},
    {classId(OBJBIF), 1},
    {classId(KernelBif), 1},
    {classId(OBJNNE), 1},
    {classId(OBJBSP), 2},
    {classId(OBJBUS), 1},
    {classId(KernelBus), 1},
    {classId(OBJCE), 5},
    {classId(KernelCE), 5},
    {classId(OBJCIPHER), 1},
    {classId(ClockManager), 1},
    {classId(OBJDISP), 1},
    {classId(KernelDisplay), 1},
    {classId(VirtMemAllocator), 1},
    {classId(OBJDPAUX), 1},
    {classId(OBJFAN), 1},
    {classId(OBJHSHUB), 2 },
    {classId(MemorySystem), 1},
    {classId(KernelMemorySystem), 1},
    {classId(MemoryManager), 1},
    {classId(OBJFBFLCN), 1},
    {classId(KernelFifo), 1 },
    {classId(OBJFIFO), 1},
    {classId(OBJGMMU), 1},
    {classId(KernelGmmu), 1},
    {classId(OBJGPULOG), 1},
    {classId(OBJGPUMON), 1},
    {classId(GraphicsManager), 1 },
    {classId(MIGManager), 1},
    {classId(KernelMIGManager), 1},
    {classId(KernelGraphicsManager), 1},
    {classId(Graphics), 1},
    {classId(KernelGraphics), 1},
    {classId(OBJHDACODEC), 1},
    {classId(OBJHWPM), 1},
    {classId(OBJINFOROM), 1},
    {classId(Intr), 1},
    {classId(Lpwr   ), 1},
    {classId(OBJLSFM), 1},
    {classId(OBJMC), 1},
    {classId(KernelMc), 1},
    {classId(PrivRing), 1},
    {classId(SwIntr), 1},
    {classId(OBJMSENC), 1},
    {classId(NvDebugDump), 1},
    {classId(KernelNvlink), 1},
    {classId(Nvlink), 1},
    {classId(Perf), 1},
    {classId(KernelPerf), 1 },
    {classId(Pmgr), 1},
    {classId(Pmu), 1},
    {classId(KernelPmu), 1},
    {classId(OBJSEC2), 1},
    {classId(Gsp), 1},
    {classId(Therm), 1},
    {classId(OBJTMR), 1},
    {classId(OBJVOLT), 1},
    {classId(OBJGRIDDISPLAYLESS), 1},
    {classId(OBJFAS), 1},
    {classId(OBJVMMU), 1},
    {classId(OBJOFA), 1 },
    {classId(KernelNvdec), 1},
    {classId(KernelSec2), 1},
    {classId(KernelGsp), 1},
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GA102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS32(gpuChildrenPresent_GA102);
    return gpuChildrenPresent_GA102;
}

