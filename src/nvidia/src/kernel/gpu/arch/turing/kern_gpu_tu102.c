/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES
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
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/hwproject.h"

/*!
 * @brief determines whether this GPU mode needs to be initialized with an offset
 *        to access the registers defined in dev_vm.ref.
 *
 * @param[in]  pGpu          OBJGPU pointer
 *
 * @returns NvU32 - physical offset needed to access virtual registers in host
 */
NvU32
gpuGetVirtRegPhysOffset_TU102(OBJGPU *pGpu)
{
    // No offset is needed, only in case of VF in SR-IOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
        return 0;
    else
        return DRF_BASE(NV_VIRTUAL_FUNCTION_FULL_PHYS_OFFSET);
}

/*!
 * @brief Returns the physical address width for the given @ref NV_ADDRESS_SPACE
 */
NvU32 gpuGetPhysAddrWidth_TU102
(
    OBJGPU          *pGpu,
    NV_ADDRESS_SPACE addrSp
)
{
    // Currently this function supports only sysmem addresses
    NV_ASSERT_OR_RETURN(ADDR_SYSMEM == addrSp, 0);

    return NV_CHIP_EXTENDED_SYSTEM_PHYSICAL_ADDRESS_BITS;
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
static const GPUCHILDPRESENT gpuChildrenPresent_TU102[] =
{
    {classId(OBJSWENG), 1},
    {classId(OBJUVM), 1},
    {classId(OBJACR), 1},
    {classId(OBJBIF), 1},
    {classId(KernelBif), 1},
    {classId(OBJBSP), 1},
    {classId(OBJBUS), 1},
    {classId(KernelBus), 1},
    {classId(OBJCE), 9},
    {classId(KernelCE), 9},
    {classId(OBJCIPHER), 1},
    {classId(ClockManager), 1},
    {classId(OBJDISP), 1},
    {classId(KernelDisplay), 1},
    {classId(VirtMemAllocator), 1},
    {classId(OBJDPAUX), 1},
    {classId(OBJFAN), 1},
    {classId(OBJHSHUB), 1},
    {classId(MemorySystem), 1},
    {classId(KernelMemorySystem), 1},
    {classId(MemoryManager), 1},
    {classId(OBJFBFLCN), 1},
    {classId(KernelFifo), 1},
    {classId(OBJFIFO), 1},
    {classId(OBJGMMU), 1},
    {classId(KernelGmmu), 1},
    {classId(OBJGPULOG), 1},
    {classId(OBJGPUMON), 1},
    {classId(GraphicsManager), 1},
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
    {classId(KernelPerf), 1},
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
    {classId(KernelNvdec), 1},
    {classId(KernelSec2), 1},
    {classId(KernelGsp), 1},
};

static const GPUCHILDPRESENT gpuChildrenPresent_TU104[] =
{
    {classId(OBJSWENG), 1},
    {classId(OBJUVM), 1},
    {classId(OBJACR), 1},
    {classId(OBJBIF), 1},
    {classId(KernelBif), 1},
    {classId(OBJBSP), 2},
    {classId(OBJBUS), 1},
    {classId(KernelBus), 1},
    {classId(OBJCE), 9},
    {classId(KernelCE), 9},
    {classId(OBJCIPHER), 1},
    {classId(ClockManager), 1},
    {classId(OBJDISP), 1},
    {classId(KernelDisplay), 1},
    {classId(VirtMemAllocator), 1},
    {classId(OBJDPAUX), 1},
    {classId(OBJFAN), 1},
    {classId(OBJHSHUB), 1},
    {classId(MemorySystem), 1},
    {classId(KernelMemorySystem), 1},
    {classId(MemoryManager), 1},
    {classId(OBJFBFLCN), 1},
    {classId(KernelFifo), 1},
    {classId(OBJFIFO), 1},
    {classId(OBJGMMU), 1},
    {classId(KernelGmmu), 1},
    {classId(OBJGPULOG), 1},
    {classId(OBJGPUMON), 1},
    {classId(GraphicsManager), 1},
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
    {classId(KernelPerf), 1},
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
    {classId(KernelNvdec), 1},
    {classId(KernelSec2), 1},
    {classId(KernelGsp), 1},
};

static const GPUCHILDPRESENT gpuChildrenPresent_TU106[] =
{
    {classId(OBJSWENG), 1},
    {classId(OBJUVM), 1},
    {classId(OBJACR), 1},
    {classId(OBJBIF), 1},
    {classId(KernelBif), 1},
    {classId(OBJBSP), 3},
    {classId(OBJBUS), 1},
    {classId(KernelBus), 1},
    {classId(OBJCE), 9},
    {classId(KernelCE), 9},
    {classId(OBJCIPHER), 1},
    {classId(ClockManager), 1},
    {classId(OBJDISP), 1},
    {classId(KernelDisplay), 1},
    {classId(VirtMemAllocator), 1},
    {classId(OBJDPAUX), 1},
    {classId(OBJFAN), 1},
    {classId(MemorySystem), 1},
    {classId(KernelMemorySystem), 1},
    {classId(MemoryManager), 1},
    {classId(OBJFBFLCN), 1},
    {classId(KernelFifo), 1},
    {classId(OBJFIFO), 1},
    {classId(OBJGMMU), 1},
    {classId(KernelGmmu), 1},
    {classId(OBJGPULOG), 1},
    {classId(OBJGPUMON), 1},
    {classId(GraphicsManager), 1},
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
    {classId(KernelPerf), 1},
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
    {classId(KernelNvdec), 1},
    {classId(KernelSec2), 1},
    {classId(KernelGsp), 1},
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_TU102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS32(gpuChildrenPresent_TU102);
    return gpuChildrenPresent_TU102;
}

const GPUCHILDPRESENT *
gpuGetChildrenPresent_TU104(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS32(gpuChildrenPresent_TU104);
    return gpuChildrenPresent_TU104;
}

const GPUCHILDPRESENT *
gpuGetChildrenPresent_TU106(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS32(gpuChildrenPresent_TU106);
    return gpuChildrenPresent_TU106;
}

