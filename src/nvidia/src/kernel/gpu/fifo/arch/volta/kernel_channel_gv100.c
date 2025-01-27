/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/mem_mgr/mem.h"
#include "kernel/mem_mgr/os_desc_mem.h"
#include "kernel/gpu/mmu/kern_gmmu.h"
#include "platform/sli/sli.h"

#include "published/volta/gv100/dev_pbdma.h"
#include "rmapi/rs_utils.h"

/**
 * @brief Verify that the given userd physical address is of the correct size
 *
 * @param[in] pKernelChannel    KernelChannel pointer
 * @param[in] userdAddrLo       low USERD physical address bits
 * @param[in] userdAddrHi       high USERD physical address bits
 *
 * @returns NV_TRUE if the given userd physical address is of the correct size
            NV_FALSE otherwise
 */
NvBool
kchannelIsUserdAddrSizeValid_GV100
(
    KernelChannel *pKernelChannel,
    NvU32          userdAddrLo,
    NvU32          userdAddrHi
)
{
    return ((userdAddrLo & DRF_MASK(NV_PPBDMA_USERD_ADDR)) == userdAddrLo) &&
           ((userdAddrHi & DRF_MASK(NV_PPBDMA_USERD_HI_ADDR)) == userdAddrHi);
}

/**
 * @brief Create the memory descriptor for USERD memory allocated
 *        by client using memory handle
 *
 *
 * @param[in]  pGpu
 * @param[in]  pKernelChannel
 * @param[in]  hClient
 * @param[in]  pUserdMemory
 * @param[in]  pUserdOffset
 *
 * @returns    NV_STATUS
 */
NV_STATUS
kchannelCreateUserdMemDescBc_GV100
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    NvHandle        hClient,
    NvHandle       *phUserdMemory,
    NvU64          *pUserdOffset
)
{
    NV_STATUS rmStatus = NV_OK;

    if (phUserdMemory[0] != 0)
    {
        NvU32 iter = 0;

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

        iter      = IsSLIEnabled(pGpu) ? gpumgrGetSubDeviceInstanceFromGpu(pGpu) : 0;

        NvHandle hUserdMemory = phUserdMemory[iter];
        NvU64    userdOffset  = pUserdOffset[iter];

        if (!hUserdMemory)
        {
            //
            // pUserdMemory[iter] may be 0 for non-zero iter in case USERD is in FB
            // The following hack will be removed once clients fix this.
            // See bug 1659362, comment 26.
            //
            NV_PRINTF(LEVEL_ERROR,
                      "User provided memory info for index %d is NULL\n",
                      iter);
            NV_PRINTF(LEVEL_ERROR,
                      "NV_CHANNEL_ALLOC_PARAMS needs to have all subdevice info\n");

            hUserdMemory = phUserdMemory[0];
            userdOffset  = pUserdOffset[0];

        }

        rmStatus = kchannelCreateUserdMemDesc_HAL(pGpu, pKernelChannel, hClient,
                                                  hUserdMemory,
                                                  userdOffset,
                                                  NULL, NULL);

        if (rmStatus != NV_OK)
        {
            SLI_LOOP_BREAK;
        }

        SLI_LOOP_END


        if (rmStatus != NV_OK)
        {
            SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
            kchannelDestroyUserdMemDesc_HAL(pGpu, pKernelChannel);
            SLI_LOOP_END
        }
        else
        {
            pKernelChannel->bClientAllocatedUserD = NV_TRUE;
        }
    }
    return rmStatus;
}

/**
 * @brief Create the memory descriptor for USERD memory allocated
 *        by client using memory handle
 *
 *
 * @param[in]  pGpu
 * @param[in]  pKernelChannel
 * @param[in]  hClient
 * @param[in]  pUserdMemory
 * @param[in]  pUserdOffset
 * @param[out] pUserdAddr   (optional) returns the USERD PA
 * @param[out] pUserdAper   (optional) returns the USERD aperture
 *
 * @returns    NV_STATUS
 */
NV_STATUS
kchannelCreateUserdMemDesc_GV100
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    NvHandle        hClient,
    NvHandle        hUserdMemory,
    NvU64           userdOffset,
    NvU64          *pUserdAddr,
    NvU32          *pUserdAper
)
{
    KernelFifo             *pKernelFifo            = GPU_GET_KERNEL_FIFO(pGpu);
    RsResourceRef          *pUserdMemoryRef;
    Memory                 *pUserdMemory           = NULL;
    PMEMORY_DESCRIPTOR      pUserdMemDescForSubDev = NULL;
    PMEMORY_DESCRIPTOR      pUserdSubMemDesc       = NULL;
    RmPhysAddr              userdAddr              = 0;
    NvU32                   userdSize              = 0;
    NV_STATUS               status                 = NV_OK;
    NvU32                   userdShift;
    NvU32                   userdAddrLo;
    NvU32                   userdAddrHi;
    NvU32                   userdAlignment;
    NvU64                   pageSize;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);
    pKernelChannel->pUserdSubDeviceMemDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = NULL;

    // Get the userd size
    kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdSize, &userdShift);
    userdAlignment = (NvU32) (1ULL << userdShift);

    if (serverutilGetResourceRefWithType(hClient,
                                         hUserdMemory,
                                         classId(Memory),
                                         &pUserdMemoryRef) != NV_OK)
    {
        NV_ASSERT(pUserdMemoryRef);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pUserdMemory = dynamicCast(pUserdMemoryRef->pResource, Memory);

    // Get the userd memdesc for the gpu
    pUserdMemDescForSubDev = memdescGetMemDescFromGpu(pUserdMemory->pMemDesc, pGpu);

    // check that the memory is not VPR
    if (memdescGetFlag(pUserdMemDescForSubDev, MEMDESC_ALLOC_FLAGS_PROTECTED))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    userdAddr = memdescGetPhysAddr(pUserdMemDescForSubDev,
                               AT_GPU,
                               userdOffset);

    userdAddrLo = NvU64_LO32(userdAddr) >> userdShift;
    userdAddrHi = NvU64_HI32(userdAddr);

    // Check that the physical address is of the correct size
    if (!kchannelIsUserdAddrSizeValid_HAL(pKernelChannel, userdAddrLo, userdAddrHi))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "physical addr size of userdAddrHi=0x%08x, userAddrLo=0x%08x is incorrect!\n",
                  userdAddrHi, userdAddrLo);

        NV_ASSERT(0);
        return NV_ERR_INVALID_ADDRESS;
    }

    //
    // USERD page size should be RM_PAGE_SIZE for BAR2 mapping to not waste size.
    // Client allocated userds  can be of larger pagesizes
    // submemdesc uses parent page size to determine "actual size" which gets
    // used by bar2 mapper.
    // Therefore override the size here and restore it later below.
    //
    pageSize = memdescGetPageSize(pUserdMemDescForSubDev, AT_GPU);
    memdescSetPageSize(pUserdMemDescForSubDev, AT_GPU, RM_PAGE_SIZE);

    // The userd memory descriptor may be shared across channels or gpus.
    // Create a Sub-memory descriptor and ref count the base memory desc.
    status = memdescCreateSubMem(&pUserdSubMemDesc,
                            pUserdMemDescForSubDev, pGpu,
                            userdOffset,
                            userdSize);
    // restore the pagesize
    memdescSetPageSize(pUserdMemDescForSubDev, AT_GPU, pageSize);

    if (status != NV_OK)
    {
        return status;
    }

    //
    // For some memory types, just creating submem is not enough to ensure
    // proper lifetimes. Make the channel dependant on this memory object,
    // so that it gets released before memory itself.
    //
    if (dynamicCast(pUserdMemoryRef->pResource, OsDescMemory) != NULL)
    {
        refAddDependant(pUserdMemoryRef, RES_GET_REF(pKernelChannel));
    }

    // check alignment
    if ((pUserdMemory->pMemDesc->Alignment < userdAlignment) &&
        (pUserdMemory->pMemDesc->Alignment  != 0))
    {
        memdescDestroy(pUserdSubMemDesc);
        return NV_ERR_INVALID_ADDRESS;
    }

    pKernelChannel->pUserdSubDeviceMemDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = pUserdSubMemDesc;

    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
    }

    if (pUserdAddr != NULL)
        *pUserdAddr = userdAddr;

    if (pUserdAper != NULL)
        *pUserdAper = kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pUserdMemDescForSubDev);

    return status;
}

/**
 * @brief Delete the memory descriptors for userd memory allocated
 *        by client
 */
void
kchannelDestroyUserdMemDesc_GV100
(
    OBJGPU           *pGpu,
    KernelChannel    *pKernelChannel
)
{
    NvU32 subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    memdescDestroy(pKernelChannel->pUserdSubDeviceMemDesc[subdevInst]);
    pKernelChannel->pUserdSubDeviceMemDesc[subdevInst] = NULL;
}
