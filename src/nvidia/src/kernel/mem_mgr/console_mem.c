/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr_internal.h"
#include "mem_mgr/console_mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "rmapi/client.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/bus/kern_bus.h"

#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER

NV_STATUS
conmemConstruct_IMPL
(
    ConsoleMemory                *pConsoleMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS          status         = NV_OK;
    Memory            *pMemory        = staticCast(pConsoleMemory, Memory);
    OBJGPU            *pGpu           = pMemory->pGpu;
    MemoryManager     *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    MEMORY_DESCRIPTOR *pMemDesc       = memmgrGetReservedConsoleMemDesc(pGpu, pMemoryManager);

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    if (pMemDesc == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT(pMemDesc->Allocated == 0);
    memdescAddRef(pMemDesc);
    pMemDesc->DupCount++;

    //
    // NV01_MEMORY_FRAMEBUFFER_CONSOLE is just a way to get at the reserved
    // framebuffer console memDesc rather than allocating a new one. Otherwise,
    // it's treated as normal memory.
    //
    status = memConstructCommon(pMemory, NV01_MEMORY_LOCAL_USER, 0, pMemDesc,
                                0, NULL, 0, 0, 0, 0, NVOS32_MEM_TAG_NONE,
                                (HWRESOURCE_INFO *)NULL);
    if (status != NV_OK)
    {
        memdescDestroy(pMemDesc);
    }
    return status;
}

NvBool
conmemCanCopy_IMPL
(
    ConsoleMemory *pConsoleMemory
)
{
    return NV_TRUE;
}

NV_STATUS conmemCtrlCmdNotifyConsoleDisabled_IMPL(ConsoleMemory *pConsoleMemory)
{
    Memory            *pMemory        = staticCast(pConsoleMemory, Memory);
    OBJGPU            *pGpu           = pMemory->pGpu;
    MEMORY_DESCRIPTOR *pMemDesc       = pMemory->pMemDesc;
    KernelBus         *pKernelBus     = GPU_GET_KERNEL_BUS(pGpu);
    NvU32              gfid           = pMemDesc->gfid;

    // Remove the BAR1 mapping of the framebuffer console.
    kbusUnmapPreservedConsole(pGpu, pKernelBus, gfid);

    // Inform OS layer, to no longer save/restore console.
    osDisableConsoleManagement(pGpu);

    return NV_OK;
}
