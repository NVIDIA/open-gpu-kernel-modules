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
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/mem_mgr/mem.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "platform/sli/sli.h"
#include "containers/eheap_old.h"

#include "class/cl906f.h"

#include "published/maxwell/gm107/dev_ram.h"
#include "published/maxwell/gm107/dev_mmu.h"

static NV_STATUS _kchannelCreateRMUserdMemDesc(OBJGPU *pGpu, KernelChannel *pKernelChannel);

static NV_STATUS _kchannelDestroyRMUserdMemDesc(OBJGPU *pGpu, KernelChannel *pKernelChannel);

/*!
 * The reason this is a hal method is because it primarily concerns with returning EngineID
 * and unfortunately, the defines for these are not standard across chips.
 *
 * Reason we have a classEngineID concat is to present an opaque handle that clients can do
 * setobject with directly. Some of them may also require to know the class, engine IDs.
 */
NV_STATUS
kchannelGetClassEngineID_GM107
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    NvHandle        handle,
    NvU32          *pClassEngineID,
    NvU32          *pClassID,
    RM_ENGINE_TYPE *pRmEngineID
)
{
    NV_STATUS          status = NV_OK;
    NvU32              halEngineTag;
    NvU32              classID;
    ChannelDescendant *pObject = NULL;

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        kchannelFindChildByHandle(pKernelChannel, handle, &pObject));
    NV_ASSERT_OR_RETURN(pObject != NULL, NV_ERR_OBJECT_NOT_FOUND);

    *pClassID = classID = RES_GET_EXT_CLASS_ID(pObject);
    halEngineTag = pObject->resourceDesc.engDesc;

    if (halEngineTag == ENG_SW)
    {
        classID = pObject->classID;
    }

    status = gpuXlateEngDescToClientEngineId(pGpu, halEngineTag, pRmEngineID);

    if (status == NV_OK)
    {
        *pClassEngineID = DRF_NUM(906F, _SET_OBJECT, _NVCLASS, classID);
    }

    NV_PRINTF(LEVEL_INFO,
              "class ID: 0x%08x classEngine ID: 0x%08x\n",
              classID, *pClassEngineID);

    return status;
}

NV_STATUS
kchannelEnableVirtualContext_GM107
(
    KernelChannel *pKernelChannel
)
{
    pKernelChannel->bSkipCtxBufferAlloc = NV_TRUE;

    return NV_OK;
}

/*!
 * Create the sub memory descriptor from preallocated USERD memory
 * allocated by RM for a channel
 */
static NV_STATUS
_kchannelCreateRMUserdMemDesc
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    NV_STATUS            status = NV_OK;
    NvU32                userdSize;
    KernelFifo          *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);
    NvU32                subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    MEMORY_DESCRIPTOR  **ppUserdSubdevMemDesc =
                         &pKernelChannel->pUserdSubDeviceMemDesc[subdevInst];

    kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdSize, NULL);

    status = memdescCreateSubMem(ppUserdSubdevMemDesc,
                                 pUserdInfo->userdPhysDesc[subdevInst],
                                 pGpu,
                                 pKernelChannel->ChID * userdSize,
                                 userdSize );
    return status;
}

/*! Delete prealloc userd submemdesc for the channel */
static NV_STATUS
_kchannelDestroyRMUserdMemDesc
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    MEMORY_DESCRIPTOR  **ppUserdSubdevMemDesc =
                         &pKernelChannel->pUserdSubDeviceMemDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
    if ((ppUserdSubdevMemDesc != NULL) && (*ppUserdSubdevMemDesc != NULL))
    {
        memdescFree(*ppUserdSubdevMemDesc);
        memdescDestroy(*ppUserdSubdevMemDesc);
        *ppUserdSubdevMemDesc = NULL;
    }

    return NV_OK;
}

/*!
 * @brief Create and alloc channel instance mem,
 *        ramfc and userd subdevice memdescs.
 *
 * @param pGpu[in]           OBJGPU pointer
 * @param pKernelChannel[in] KernelChannel pointer
 * @param flags[in]          Flags
 * @param verifFlags[in]     verifFlags
 *
 * @returns NV_STATUS
 */
NV_STATUS kchannelAllocMem_GM107
(
    OBJGPU             *pGpu,
    KernelChannel      *pKernelChannel,
    NvU32               Flags,
    NvU32               verifFlags
)
{
    KernelFifo             *pKernelFifo     = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS               status          = NV_OK;
    NvU32                   CpuCacheAttrib;
    FIFO_INSTANCE_BLOCK    *pInstanceBlock  = NULL;
    NvU32                   userdSize;
    NvU64                   instMemSize;
    NvU64                   instMemAlign;
    NvBool                  bInstProtectedMem;
    const NV_ADDRESS_SPACE *pInstAllocList;
    CTX_BUF_POOL_INFO      *pChannelBufPool = NULL;
    NvU64                   allocFlags      = MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    NvU32                   scgType;
    NvU32                   runqueue;
    KernelChannelGroup     *pKernelChannelGroup;
    NvU32                   subdevInst;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);
    pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;

    scgType     = DRF_VAL(OS04, _FLAGS, _GROUP_CHANNEL_THREAD, Flags);
    runqueue    = DRF_VAL(OS04, _FLAGS, _GROUP_CHANNEL_RUNQUEUE, Flags);

    if (!kfifoValidateSCGTypeAndRunqueue_HAL(pKernelFifo, scgType, runqueue))
        return NV_ERR_INVALID_ARGUMENT;

    kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdSize, NULL);

    status = kfifoGetInstMemInfo_HAL(pKernelFifo, &instMemSize, &instMemAlign,
                                    &bInstProtectedMem, &CpuCacheAttrib, &pInstAllocList);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to get instance memory info!\n");
        goto fail;
    }

    ///  Alloc Instance block
    if (IsSLIEnabled(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        pInstAllocList = ADDRLIST_FBMEM_ONLY;
        CpuCacheAttrib = NV_MEMORY_UNCACHED;
    }

    // check for allocating VPR memory
    if (bInstProtectedMem)
        allocFlags |= MEMDESC_ALLOC_FLAGS_PROTECTED;

    pChannelBufPool = pKernelChannelGroup->pChannelBufPool;
    if (pChannelBufPool != NULL)
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    pInstanceBlock = pKernelChannel->pFifoHalData[subdevInst];

    if (pInstanceBlock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Instance block is NULL for hClient 0x%x Channel 0x%x!\n",
                  RES_GET_CLIENT_HANDLE(pKernelChannel),
                  RES_GET_HANDLE(pKernelChannel));
        SLI_LOOP_BREAK;
    }

    status = memdescCreate(&pInstanceBlock->pInstanceBlockDesc, pGpu,
                           instMemSize, instMemAlign, NV_TRUE,
                           ADDR_UNKNOWN, CpuCacheAttrib, allocFlags);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to allocate instance memory descriptor!\n");
        SLI_LOOP_BREAK;
    }

    if ((memdescGetAddressSpace(pInstanceBlock->pInstanceBlockDesc) == ADDR_SYSMEM) &&
        (gpuIsInstanceMemoryAlwaysCached(pGpu)))
    {
        memdescSetGpuCacheAttrib(pInstanceBlock->pInstanceBlockDesc, NV_MEMORY_CACHED);
    }

    if (pChannelBufPool != NULL)
    {
        status = memdescSetCtxBufPool(pInstanceBlock->pInstanceBlockDesc, pChannelBufPool);
        if (status != NV_OK)
        {
            NV_ASSERT(status == NV_OK);
            SLI_LOOP_BREAK;
        }
    }

    memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_116, 
                        pInstanceBlock->pInstanceBlockDesc, pInstAllocList);
    if (status == NV_OK)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        // Initialize the instance block of the channel with zeros
        status = memmgrMemDescMemSet(pMemoryManager,
                                     pInstanceBlock->pInstanceBlockDesc,
                                     0,
                                     TRANSFER_FLAGS_NONE);
        if (status != NV_OK)
        {
            NV_ASSERT(status == NV_OK);
            SLI_LOOP_BREAK;
        }

        memdescSetName(pGpu, pInstanceBlock->pInstanceBlockDesc, NV_RM_SURF_NAME_INSTANCE_BLOCK, NULL);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Instance block allocation for hClient 0x%x hChannel 0x%x failed\n",
                  RES_GET_CLIENT_HANDLE(pKernelChannel), RES_GET_HANDLE(pKernelChannel));
        SLI_LOOP_BREAK;
    }

    /// Alloc RAMFC Desc
    status = memdescCreateSubMem(&pInstanceBlock->pRamfcDesc,
                                 pInstanceBlock->pInstanceBlockDesc,
                                 pGpu, 0, DRF_SIZE( NV_RAMIN_RAMFC ) / 8);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate memdesc for RAMFC\n");
        SLI_LOOP_BREAK;
    }

    // TODO: Move this elsewhere.
    if (!pKernelChannel->bClientAllocatedUserD)
    {
        NV_ASSERT(pKernelChannel->pUserdSubDeviceMemDesc[subdevInst] == NULL);

        status = _kchannelCreateRMUserdMemDesc(pGpu, pKernelChannel);
        if (status  != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Could not allocate sub memdesc for USERD\n"); // TODO SLI BREAK
            SLI_LOOP_BREAK;
        }
    }

    status = kchannelCreateUserMemDesc_HAL(pGpu, pKernelChannel);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "kchannelCreateUserMemDesc failed \n");
        SLI_LOOP_BREAK;
    }

    NV_PRINTF(LEVEL_INFO,
              "hChannel 0x%x hClient 0x%x, Class ID 0x%x "
              "Instance Block @ 0x%llx (%s %x) "
              "USERD @ 0x%llx "
              "for subdevice %d\n",
              RES_GET_HANDLE(pKernelChannel), RES_GET_CLIENT_HANDLE(pKernelChannel), RES_GET_EXT_CLASS_ID(pKernelChannel),
              memdescGetPhysAddr(pInstanceBlock->pInstanceBlockDesc, AT_GPU, 0),
              memdescGetApertureString(memdescGetAddressSpace(pInstanceBlock->pInstanceBlockDesc)),
              (NvU32)(memdescGetAddressSpace(pInstanceBlock->pInstanceBlockDesc)),
              (pKernelChannel->pUserdSubDeviceMemDesc[subdevInst] == NULL) ? 0x0LL :
              memdescGetPhysAddr(pKernelChannel->pUserdSubDeviceMemDesc[subdevInst], AT_GPU, 0LL),
              subdevInst);

    SLI_LOOP_END

fail:
    // Just a note about our failure path, null and unallocated
    // memdescFrees are allowed so this is not a bug.
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not create Channel\n");
        DBG_BREAKPOINT();
    }

    return status;
}

/*!
 * @brief Free and destroy channel memdescs
 *        created during channel alloc mem
 *
 * @param pGpu[in]            OBJGPU pointer
 * @param pKernelChannel[in]  KernelChannel pointer
 *
 * @return void
 */
void
kchannelDestroyMem_GM107
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    FIFO_INSTANCE_BLOCK *pInstanceBlock  = NULL;
    NvU32                subdevInst;

    NV_ASSERT_OR_RETURN_VOID(pKernelChannel != NULL);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    if (!pKernelChannel->bClientAllocatedUserD)
    {
        _kchannelDestroyRMUserdMemDesc(pGpu, pKernelChannel);
    }

    pInstanceBlock = pKernelChannel->pFifoHalData[subdevInst];
    if (pInstanceBlock != NULL)
    {
        // Release RAMFC sub memdesc
        if (pInstanceBlock->pRamfcDesc != NULL)
        {
            memdescFree(pInstanceBlock->pRamfcDesc);
            memdescDestroy(pInstanceBlock->pRamfcDesc);
            pInstanceBlock->pRamfcDesc = NULL;
        }

        // Release Inst block Desc
        if (pInstanceBlock->pInstanceBlockDesc != NULL)
        {
            memdescFree(pInstanceBlock->pInstanceBlockDesc);
            memdescDestroy(pInstanceBlock->pInstanceBlockDesc);
            pInstanceBlock->pInstanceBlockDesc = NULL;
        }
    }

    // Remove USERD memDescs
    memdescDestroy(pKernelChannel->pInstSubDeviceMemDesc[subdevInst]);
    pKernelChannel->pInstSubDeviceMemDesc[subdevInst] = NULL;

    SLI_LOOP_END

    return;
}

/**
 * @brief reserves a hardware channel slot
 *
 * Only responsible for indicating a hardware channel is in use, does not set
 * any other software state.
 *
 * @param     pGpu
 * @param[in] pKernelChannel  the pre-allocated KernelChannel
 * @param[in] hClient
 * @param[in] allocMode CHANNEL_HW_ID_ALLC_MODE_*
 * @param[in] ChID
 * @param[in] bForceInternalIdx true if requesting specific index within USERD page
 * @param[in] internalIdx requested index within USERD page when bForceInternalIdx
 *                        true
 */
NV_STATUS
kchannelAllocHwID_GM107
(
    OBJGPU   *pGpu,
    KernelChannel *pKernelChannel,
    NvHandle  hClient,
    NvU32     Flags,
    NvU32     verifFlags2,
    NvU32     ChID
)
{
    NV_STATUS status;
    KernelFifo              *pKernelFifo       = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR                *pChidMgr          = NULL;
    NvU32                    internalIdx       = 0;
    NvU32                    userdPageIdx      = 0;
    NvBool                   bForceInternalIdx = NV_FALSE;
    NvBool                   bForceUserdPage   = NV_FALSE;
    CHANNEL_HW_ID_ALLOC_MODE allocMode         = CHANNEL_HW_ID_ALLOC_MODE_GROW_UP;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        // per runlist channel heap is supported for sriov only
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
                           !kfifoIsPerRunlistChramEnabled(pKernelFifo),
                           NV_ERR_INVALID_STATE);

        pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo,
                                CHIDMGR_RUNLIST_ID_LEGACY);

        NV_CHECK_OR_RETURN(LEVEL_INFO,
                           kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo,
                                                        pChidMgr, ChID) == NULL,
                           NV_OK);
        allocMode = CHANNEL_HW_ID_ALLOC_MODE_PROVIDED;
    }
    else
    {
        if (FLD_TEST_DRF(OS04, _FLAGS, _CHANNEL_USERD_INDEX_PAGE_FIXED, _TRUE, Flags))
        {
            bForceUserdPage = NV_TRUE;
            userdPageIdx = DRF_VAL(OS04, _FLAGS, _CHANNEL_USERD_INDEX_PAGE_VALUE, Flags);
            internalIdx = DRF_VAL(OS04, _FLAGS, _CHANNEL_USERD_INDEX_VALUE, Flags);

            NV_ASSERT_OR_RETURN(FLD_TEST_DRF(OS04,
                                             _FLAGS,
                                             _CHANNEL_USERD_INDEX_FIXED,
                                             _FALSE,
                                             Flags),
                                NV_ERR_INVALID_STATE);
        }

        if (FLD_TEST_DRF(OS04, _FLAGS, _CHANNEL_USERD_INDEX_FIXED, _TRUE, Flags))
        {
            bForceInternalIdx = NV_TRUE;
            internalIdx = DRF_VAL(OS04, _FLAGS, _CHANNEL_USERD_INDEX_VALUE, Flags);
        }
    }

    pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, kchannelGetRunlistId(pKernelChannel));

    status = kfifoChidMgrAllocChid(pGpu,
                                   pKernelFifo,
                                   pChidMgr,
                                   hClient,
                                   allocMode,
                                   bForceInternalIdx,
                                   internalIdx,
                                   bForceUserdPage,
                                   userdPageIdx,
                                   ChID,
                                   pKernelChannel);

    return status;
}

/**
 * @brief Releases a hardware channel ID.
 *
 * Not responsible for freeing any software state beyond that which indicates a
 * hardware channel is in use.
 *
 * @param pGpu
 * @param pKernelChannel
 */
NV_STATUS
kchannelFreeHwID_GM107
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    NV_STATUS   status;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR   *pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo,
                                           kchannelGetRunlistId(pKernelChannel));
    EMEMBLOCK  *pFifoDataBlock;

    NV_ASSERT_OR_RETURN(pChidMgr != NULL, NV_ERR_OBJECT_NOT_FOUND);
    pFifoDataBlock = pChidMgr->pFifoDataHeap->eheapGetBlock(
        pChidMgr->pFifoDataHeap,
        pKernelChannel->ChID,
        NV_FALSE);
    NV_ASSERT_OR_RETURN(pFifoDataBlock != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT(pFifoDataBlock->pData == pKernelChannel);

    status = kfifoChidMgrFreeChid(pGpu, pKernelFifo, pChidMgr, pKernelChannel->ChID);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to Free Channel From Heap: %d\n",
                  pKernelChannel->ChID);
        DBG_BREAKPOINT();
    }

    return status;
}

NV_STATUS
kchannelGetUserdInfo_GM107
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    NvU64          *userBase,
    NvU64          *offset,
    NvU64          *length
)
{
    NV_STATUS              status;
    NvU64                  bar1MapOffset;
    NvU32                  bar1MapSize;
    CLI_CHANNEL_CLASS_INFO classInfo;
    KernelMemorySystem     *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    NvBool bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

    CliGetChannelClassInfo(RES_GET_EXT_CLASS_ID(pKernelChannel), &classInfo);

    switch (classInfo.classType)
    {
        case CHANNEL_CLASS_TYPE_GPFIFO:
            NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

            // USERD is not pre-allocated in BAR1 so there is no offset/userBase
            NV_ASSERT_OR_RETURN(!pKernelChannel->bClientAllocatedUserD,
                                NV_ERR_INVALID_REQUEST);

            status = kchannelGetUserdBar1MapOffset_HAL(pGpu,
                                                       pKernelChannel,
                                                       &bar1MapOffset,
                                                       &bar1MapSize);
            if (status == NV_OK)
            {
                *offset = bar1MapOffset;
                *length = bar1MapSize;

                if (userBase)
                {
                    if (bCoherentCpuMapping)
                    {
                        NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
                        *userBase = pKernelMemorySystem->coherentCpuFbBase;
                    }
                    else
                    {
                        *userBase = gpumgrGetGpuPhysFbAddr(pGpu);
                    }
                }
            }
            break;

        default:
            NV_PRINTF(LEVEL_ERROR,
                      "class = %x not supported for user base mapping\n",
                      RES_GET_EXT_CLASS_ID(pKernelChannel));
            status = NV_ERR_GENERIC;
            break;
    }
    return status;
}

//
// Takes as input a Channel * and returns the BAR1 offset that this channel's
// USERD has been mapped to. Also returns the size of the BAR1 mapping that
// pertains to this channel. The BAR1 map of all USERDs should have already
// been setup before the first channel was created.
//
// For example, USERD of 40 channels have been mapped at BAR1 offset 0x100.
// USERD of one channel is of size 4k. In which case this function will return
// ( 0x100 + ( 0x1000 * 0xa ) ) if the input ChID = 0xa.
//
NV_STATUS
kchannelGetUserdBar1MapOffset_GM107
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel,
    NvU64         *bar1MapOffset,
    NvU32         *bar1MapSize
)
{
    KernelFifo        *pKernelFifo     = GPU_GET_KERNEL_FIFO(pGpu);
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    //
    // only supported when bUsePerRunlistChannelRam is disabled.
    // We don't pre-allocate userd for all channels across all runlists; we expect
    // clients to have moved to client allocated userd.
    //
    NV_ASSERT_OR_RETURN(!kfifoIsPerRunlistChramEnabled(pKernelFifo),
                        NV_ERR_NOT_SUPPORTED);

    if (pUserdInfo->userdBar1MapSize == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "fifoGetUserdBar1Offset_GF100: BAR1 map of USERD has not "
                  "been setup yet\n");
        NV_ASSERT(0);
        return NV_ERR_GENERIC;
    }

    kfifoGetUserdSizeAlign_HAL(pKernelFifo, bar1MapSize, NULL);

    *bar1MapOffset = pKernelChannel->ChID * *bar1MapSize +
                     pUserdInfo->userdBar1MapStartOffset;

    NV_ASSERT((*bar1MapOffset + *bar1MapSize) <=
              (pUserdInfo->userdBar1MapStartOffset +
               pUserdInfo->userdBar1MapSize));

    return NV_OK;
}

/*!
 * @brief Creates a memory descriptor to be used for creating a GPU mapped MMIO
 *        region for a given channel.
 */
NV_STATUS
kchannelCreateUserMemDesc_GM107
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelFifo    *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pSubDevInstMemDesc = NULL;
    MEMORY_DESCRIPTOR **ppMemDesc =
         &pKernelChannel->pInstSubDeviceMemDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    status = kfifoChannelGetFifoContextMemDesc_HAL(pGpu, pKernelFifo, pKernelChannel,
                                                   FIFO_CTX_INST_BLOCK, &pSubDevInstMemDesc);

    if (status != NV_OK)
        return status;

    NV_ASSERT_OR_RETURN(pSubDevInstMemDesc, NV_ERR_OBJECT_NOT_FOUND);

    status = memdescCreate(ppMemDesc, pGpu, RM_PAGE_SIZE, 0,
                           memdescGetContiguity(pSubDevInstMemDesc, AT_GPU),
                           memdescGetAddressSpace(pSubDevInstMemDesc),
                           memdescGetCpuCacheAttrib(pSubDevInstMemDesc),
                           MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

    if (status != NV_OK)
        return status;

    NV_ASSERT(*ppMemDesc);

    memdescDescribe(*ppMemDesc, memdescGetAddressSpace(pSubDevInstMemDesc),
                    memdescGetPhysAddr(pSubDevInstMemDesc, AT_GPU, 0), RM_PAGE_SIZE);
    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);

    memdescSetPteKind(*ppMemDesc, NV_MMU_PTE_KIND_SMHOST_MESSAGE);

    return NV_OK;
}

/**
 * @brief Retrieves the engine ID (NV_PFIFO_ENGINE_*) a given channel is operating on.
 *
 * This value will not be valid for a channel that has not been scheduled.
 *
 * @param       pGpu
 * @param       pKernelChannel
 * @param[out]  pEngDesc
 */
NV_STATUS
kchannelGetEngine_GM107
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel,
    NvU32         *pEngDesc
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (pEngDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    NV_PRINTF(LEVEL_INFO, "0x%x\n", kchannelGetDebugTag(pKernelChannel));

    *pEngDesc = kchannelGetRunlistId(pKernelChannel);

    // This will pick the first engine on this runlist (may not be the only one).
    NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                ENGINE_INFO_TYPE_RUNLIST, *pEngDesc,
                ENGINE_INFO_TYPE_ENG_DESC, pEngDesc));

    return NV_OK;
}
