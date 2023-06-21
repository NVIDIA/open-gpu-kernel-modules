/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/gpu/subdevice/subdevice_diag.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "kernel/core/locks.h"
#include "lib/base_utils.h"

#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"

#include "class/cl0080.h"
#include "class/cl2080.h"
#include "class/cl208f.h"

#include "ctrl/ctrl0080/ctrl0080fifo.h"

static NV_STATUS _kfifoGetCaps(OBJGPU *pGpu, NvU8 *pKfifoCaps);

/*!
 * @brief deviceCtrlCmdFifoGetChannelList
 */
NV_STATUS
deviceCtrlCmdFifoGetChannelList_IMPL
(
    Device *pDevice,
    NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *pChannelParams
)
{
    OBJGPU  *pGpu               = GPU_RES_GET_GPU(pDevice);
    NvU32   *pChannelHandleList = NvP64_VALUE(pChannelParams->pChannelHandleList);
    NvU32   *pChannelList       = NvP64_VALUE(pChannelParams->pChannelList);
    NvU32    counter;

    // Validate input / Size / Args / Copy args
    if (pChannelParams->numChannels == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid Params for command NV0080_CTRL_CMD_FIFO_GET_CHANNELLIST\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    for (counter = 0; counter < pChannelParams->numChannels; counter++)
    {
        KernelChannel *pKernelChannel;
        NvU32 chid = NV0080_CTRL_FIFO_GET_CHANNELLIST_INVALID_CHANNEL;
        NV_STATUS status;

        // Searching through the rm client db.
        status = CliGetKernelChannel(RES_GET_CLIENT(pDevice),
            pChannelHandleList[counter], &pKernelChannel);

        if (status == NV_OK)
        {
            chid = pKernelChannel->ChID;

            // Amodel-specific : Encode runlist ID
            if (pGpu && (IS_MODS_AMODEL(pGpu)))
            {
                chid |= ((kchannelGetRunlistId(pKernelChannel) & 0xffff) << 16);
            }
        }

        pChannelList[counter] = chid;
    }

    return NV_OK;
}

NV_STATUS
deviceCtrlCmdFifoIdleChannels_IMPL
(
    Device *pDevice,
    NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS *pParams
)
{
    NvBool        isGpuLockAcquired = NV_FALSE;
    NV_STATUS     status = NV_OK;
    OBJGPU       *pGpu = GPU_RES_GET_GPU(pDevice);
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

    // Check buffer size against maximum
    if (pParams->numChannels > NV0080_CTRL_CMD_FIFO_IDLE_CHANNELS_MAX_CHANNELS)
        return NV_ERR_INVALID_ARGUMENT;

    //
    // Acquire GPU lock manually in control call body instead of letting Resource
    // Server do it to ensure that RM_LOCK_MODULES_FIFO is used.
    //
    if (!rmGpuLockIsOwner())
    {
        status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_FIFO);

        if (status != NV_OK)
            goto done;

        isGpuLockAcquired = NV_TRUE;
    }

    //
    // Send RPC if running in Guest/CPU-RM. Do this manually instead of ROUTE_TO_PHYSICAL
    // so that we can acquire the GPU lock in CPU-RM first.
    //
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
    }
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

done:

    if (isGpuLockAcquired)
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

    return status;
}

NV_STATUS
subdeviceCtrlCmdGetPhysicalChannelCount_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *pParams
)
{
    OBJGPU   *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32     numChannelsInUse = 0;
    NvU32     numChannels;
    NvU32     i;
    NvU32     chGrpID;

    pParams->physChannelCount      = NV_U32_MAX;
    pParams->physChannelCountInUse = 0;

    // TODO: Follow up with clients before turning on per esched chidmgr
    for (i = 0; i < pKernelFifo->numChidMgrs; i++)
    {
        if (pKernelFifo->ppChidMgr[i] != NULL)
        {
            // Get the max number of HW channels on the runlist
            numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pKernelFifo->ppChidMgr[i]);

            // Get the number of channels already in use
            for (chGrpID = 0; chGrpID < numChannels; chGrpID++)
            {
                if (nvBitFieldTest(pKernelFifo->ppChidMgr[i]->channelGrpMgr.pHwIdInUse,
                                   pKernelFifo->ppChidMgr[i]->channelGrpMgr.hwIdInUseSz,
                                   chGrpID))
                {
                    numChannelsInUse++;
                }
            }

            pParams->physChannelCount      = NV_MIN(pParams->physChannelCount, numChannels);
            pParams->physChannelCountInUse = NV_MAX(pParams->physChannelCountInUse, numChannelsInUse);
        }
    }
    return NV_OK;
}

/*!
 * @brief subdeviceCtrlCmdFifoGetInfo
 *
 * Lock Requirements:
 *      Assert that both the GPUs lock and API lock are held on entry.
 */
NV_STATUS
subdeviceCtrlCmdFifoGetInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FIFO_GET_INFO_PARAMS *pFifoInfoParams
)
{
    OBJGPU        *pGpu           = GPU_RES_GET_GPU(pSubdevice);
    KernelFifo    *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS      status         = NV_OK;
    NvU32          runlistId;
    CHID_MGR      *pChidMgr;
    NvU32          i;
    NvU32          data;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    // error checck
    if (pFifoInfoParams->fifoInfoTblSize > NV2080_CTRL_FIFO_GET_INFO_MAX_ENTRIES)
        return NV_ERR_INVALID_PARAM_STRUCT;

    // step thru list
    for (i = 0; i < pFifoInfoParams->fifoInfoTblSize; i++)
    {
        switch (pFifoInfoParams->fifoInfoTbl[i].index)
        {
            case NV2080_CTRL_FIFO_INFO_INDEX_INSTANCE_TOTAL:
                data = memmgrGetRsvdMemorySize(pMemoryManager);
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNEL_GROUPS:
                //
                // TODO: Follow up with clients using this control call before
                // turning on per esched chidmgr
                //
                data = kfifoGetMaxChannelGroupsInSystem(pGpu, pKernelFifo);
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNELS_PER_GROUP:
                data = kfifoGetMaxChannelGroupSize_HAL(pKernelFifo);
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_CHANNEL_GROUPS_IN_USE:
                //
                // TODO: Follow up with clients using this control call before
                // turning on per esched chidmgr
                //
                data = kfifoGetChannelGroupsInUse(pGpu, pKernelFifo);
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_MAX_SUBCONTEXT_PER_GROUP:
                //
                // RM-SMC AMPERE-TODO This data is incompatible with SMC, where
                // different engines can have different max VEID counts
                //
                data = kfifoGetMaxSubcontext_HAL(pGpu, pKernelFifo, NV_FALSE);
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_BAR1_USERD_START_OFFSET:
            {
                NvU64 userdAddr;
                NvU32 userdSize;
                NvU32 gfid;

                NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
                if (hypervisorIsVgxHyper() && IS_GFID_PF(gfid))
                {
                    status = kfifoGetUserdBar1MapInfo_HAL(pGpu, pKernelFifo, &userdAddr, &userdSize);
                    if (status == NV_OK)
                        data = (NvU32)(userdAddr >> NV2080_CTRL_FIFO_GET_INFO_USERD_OFFSET_SHIFT);
                }
                else
                {
                    data = 0;
                    status = NV_ERR_INVALID_REQUEST;
                }
                break;
            }
            case NV2080_CTRL_FIFO_INFO_INDEX_DEFAULT_CHANNEL_TIMESLICE:
                {
                    NvU64 timeslice = kfifoChannelGroupGetDefaultTimeslice_HAL(pKernelFifo);
                    data = NvU64_LO32(timeslice);
                    NV_ASSERT_OR_RETURN((NvU64_HI32(timeslice) == 0), NV_ERR_INVALID_PARAM_STRUCT);
                }
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_IS_PER_RUNLIST_CHANNEL_RAM_SUPPORTED:
                data = (NvU32) kfifoIsPerRunlistChramEnabled(pKernelFifo);
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNEL_GROUPS_PER_ENGINE:
                // Get runlist ID for Engine type.
                NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                                ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                                                gpuGetRmEngineType(pFifoInfoParams->engineType),
                                                                ENGINE_INFO_TYPE_RUNLIST,
                                                                &runlistId));
                pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);
                data = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
                break;
            case NV2080_CTRL_FIFO_INFO_INDEX_CHANNEL_GROUPS_IN_USE_PER_ENGINE:
                // Get runlist ID for Engine type.
                NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                                ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                                                gpuGetRmEngineType(pFifoInfoParams->engineType),
                                                                ENGINE_INFO_TYPE_RUNLIST, &runlistId));
                data = kfifoGetRunlistChannelGroupsInUse(pGpu, pKernelFifo, runlistId);
                break;
            default:
                data = 0;
                status = NV_ERR_INVALID_ARGUMENT;
                break;
        }

        if (status != NV_OK)
            break;

        // save off data value
        pFifoInfoParams->fifoInfoTbl[i].data = data;
    }

    return status;
}


/*!
 * @brief Get bitmask of allocated channels
 */
NV_STATUS subdeviceCtrlCmdFifoGetAllocatedChannels_IMPL
(
    Subdevice                                      *pSubdevice,
    NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS *pParams
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(GPU_RES_GET_GPU(pSubdevice));
    NV_STATUS status;

    status = kfifoGetAllocatedChannelMask(GPU_RES_GET_GPU(pSubdevice),
                                          pKernelFifo,
                                          pParams->runlistId,
                                          pParams->bitMask,
                                          sizeof pParams->bitMask);
    switch(status)
    {
    case NV_ERR_BUFFER_TOO_SMALL:
    case NV_ERR_INVALID_ARGUMENT:
        //
        // Update the ctrl call structure to have sufficient space for 1 bit per
        // possible channels in a runlist. This is a driver bug.
        //
        NV_ASSERT_OK(status);
        return NV_ERR_NOT_SUPPORTED;
    default:
        return status;
    }
}


/*!
 * @brief subdeviceCtrlCmdFifoGetUserdLocation
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdFifoGetUserdLocation_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *pUserdLocationParams
)
{
    NvU32      userdAperture;
    NvU32      userdAttribute;
    NV_STATUS  rmStatus = NV_OK;
    OBJGPU    *pGpu  = GPU_RES_GET_GPU(pSubdevice);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    rmStatus = kfifoGetUserdLocation_HAL(pKernelFifo,
                                         &userdAperture,
                                         &userdAttribute);

    if (rmStatus != NV_OK)
        return rmStatus;

    // Support for NVLINK coherent memory is not yet available in RM

    if (userdAperture == ADDR_FBMEM)
    {
        pUserdLocationParams->aperture = NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_APERTURE_VIDMEM;
    }
    else if (userdAperture == ADDR_SYSMEM)
    {
        pUserdLocationParams->aperture = NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_APERTURE_SYSMEM;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid userdAperture value = 0x%08x\n",
                  userdAperture);
        return NV_ERR_INVALID_STATE;
    }

    if (userdAttribute == NV_MEMORY_CACHED)
    {
        pUserdLocationParams->attribute = NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_ATTRIBUTE_CACHED;
    }
    else if (userdAttribute == NV_MEMORY_UNCACHED)
    {
        pUserdLocationParams->attribute = NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_ATTRIBUTE_UNCACHED;
    }
    else if (userdAttribute == NV_MEMORY_WRITECOMBINED)
    {
        pUserdLocationParams->attribute = NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_ATTRIBUTE_WRITECOMBINED;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid userdAttribute value = 0x%08x\n",
                  userdAttribute);
        return NV_ERR_INVALID_STATE;
    }

    return rmStatus;
}

/*!
 * @brief subdeviceCtrlCmdFifoGetChannelMemInfo
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *pChannelMemParams
)
{
    OBJGPU    *pGpu     = GPU_RES_GET_GPU(pSubdevice);
    NvHandle   hDevice  = RES_GET_PARENT_HANDLE(pSubdevice);
    RsClient  *pClient  = RES_GET_CLIENT(pSubdevice);
    NV_STATUS  rmStatus = NV_OK;
    NvU32      index;
    NvU32      runqueues;
    KernelFifo *pKernelFifo     = GPU_GET_KERNEL_FIFO(pGpu);
    KernelChannel *pKernelChannel;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NV2080_CTRL_FIFO_CHANNEL_MEM_INFO chMemInfo;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    rmStatus = CliGetKernelChannelWithDevice(pClient,
                                             hDevice,
                                             pChannelMemParams->hChannel,
                                             &pKernelChannel);
    if (rmStatus != NV_OK)
    {
        return NV_ERR_INVALID_CHANNEL;
    }

    portMemSet((void *)&chMemInfo, 0, sizeof(NV2080_CTRL_FIFO_CHANNEL_MEM_INFO));

    // Get Inst Block Mem Info
    rmStatus = kfifoChannelGetFifoContextMemDesc_HAL(pGpu,
                                                     pKernelFifo,
                                                     pKernelChannel,
                                                     FIFO_CTX_INST_BLOCK,
                                                     &pMemDesc);
    if (rmStatus != NV_OK)
        return rmStatus;

    kfifoFillMemInfo(pKernelFifo, pMemDesc, &chMemInfo.inst);

    // Get RAMFC mem Info
    pMemDesc = NULL;
    rmStatus = kfifoChannelGetFifoContextMemDesc_HAL(pGpu,
                                          pKernelFifo,
                                          pKernelChannel,
                                          FIFO_CTX_RAMFC,
                                          &pMemDesc);

    if (rmStatus != NV_OK)
        return rmStatus;

    kfifoFillMemInfo(pKernelFifo, pMemDesc, &chMemInfo.ramfc);

    // Get Method buffer mem info
    runqueues = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
    NV_ASSERT((runqueues <= NV2080_CTRL_FIFO_GET_CHANNEL_MEM_INFO_MAX_COUNT));
    for (index = 0; index < runqueues; index++)
    {
        pMemDesc = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pMthdBuffers[index].pMemDesc;
        if (pMemDesc != NULL)
        {
            kfifoFillMemInfo(pKernelFifo, pMemDesc, &chMemInfo.methodBuf[index]);
            chMemInfo.methodBufCount++;
        }
    }

    // copy into the kernel structure, there is no userland pointer
    // maybe later structure is copied out to userland
    portMemCopy(&pChannelMemParams->chMemInfo,
                sizeof(NV2080_CTRL_FIFO_CHANNEL_MEM_INFO),
                &chMemInfo,
                sizeof(NV2080_CTRL_FIFO_CHANNEL_MEM_INFO));

    return rmStatus;
}

NV_STATUS
diagapiCtrlCmdFifoEnableVirtualContext_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS *pEnableVCParams
)
{
    NV_STATUS      rmStatus = NV_OK;
    KernelChannel *pKernelChannel = NULL;
    RsClient      *pClient = RES_GET_CLIENT(pDiagApi);
    Device        *pDevice = GPU_RES_GET_DEVICE(pDiagApi);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        CliGetKernelChannelWithDevice(pClient,
                                      RES_GET_HANDLE(pDevice),
                                      pEnableVCParams->hChannel,
                                      &pKernelChannel));

    rmStatus = kchannelEnableVirtualContext_HAL(pKernelChannel);
    return rmStatus;
}

/*!
 * @brief subdeviceCtrlCmdFifoUpdateChannelInfo
 *
 * This function is broken for SLI.
 * Will be fixed after instance block and userd
 * is made unicast.
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdFifoUpdateChannelInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pChannelInfo
)
{
    CALL_CONTEXT             *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams             *pRmCtrlParams = pCallContext->pControlParams;
    OBJGPU                   *pGpu           = GPU_RES_GET_GPU(pSubdevice);
    RsClient                 *pChannelClient;
    NvHandle                  hClient        = RES_GET_CLIENT_HANDLE(pSubdevice);
    KernelChannel            *pKernelChannel = NULL;
    NV_STATUS                 status         = NV_OK;
    NvU64                     userdAddr      = 0;
    NvU32                     userdAper      = 0;

    // Bug 724186 -- Skip this check for deferred API
    LOCK_ASSERT_AND_RETURN(pRmCtrlParams->bDeferredApi || rmGpuLockIsOwner());

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
                          serverGetClientUnderLock(&g_resServ,
                                                   pChannelInfo->hClient,
                                                   &pChannelClient));

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
                          CliGetKernelChannel(pChannelClient,
                                              pChannelInfo->hChannel,
                                              &pKernelChannel));
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    if (!pChannelInfo->hUserdMemory)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!pKernelChannel->bClientAllocatedUserD)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        if (status != NV_OK)
            return status;

        // Destroy the submemdescriptor of the previous USERD
        kchannelDestroyUserdMemDesc_HAL(pGpu, pKernelChannel);

        // Get the userd hMemory and create a submemdescriptor
        // Store it in pKernelChannel
        status = kchannelCreateUserdMemDesc_HAL(pGpu, pKernelChannel, hClient,
                                   pChannelInfo->hUserdMemory,
                                   pChannelInfo->userdOffset,
                                   &userdAddr, &userdAper);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                        "kchannelCreateUserdMemDesc_HAL"
                        "failed for hClient 0x%x and channel 0x%x status 0x%x\n",
                        hClient, kchannelGetDebugTag(pKernelChannel), status);
        }
    }
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

    return status;
}

NV_STATUS
diagapiCtrlCmdFifoGetChannelState_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    RsClient *pChannelClient;
    KernelChannel *pKernelChannel;

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        serverGetClientUnderLock(&g_resServ, pChannelStateParams->hClient,
            &pChannelClient));

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        CliGetKernelChannel(pChannelClient, pChannelStateParams->hChannel, &pKernelChannel));
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        kchannelGetChannelPhysicalState(pGpu, pKernelChannel, pChannelStateParams));

    // Fill out kernel state here
    pChannelStateParams->bCpuMap     = kchannelIsCpuMapped(pGpu, pKernelChannel);
    pChannelStateParams->bRunlistSet = kchannelIsRunlistSet(pGpu, pKernelChannel);

    return NV_OK;
}

static NV_STATUS
_kfifoGetCaps
(
    OBJGPU *pGpu,
    NvU8   *pKfifoCaps
)
{
    NV_STATUS   rmStatus         = NV_OK;
    NvBool      bCapsInitialized = NV_FALSE;
    KernelFifo *pKernelFifo      = GPU_GET_KERNEL_FIFO(pGpu);

    VERIFY_OBJ_PTR(pKernelFifo);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
        if (pKernelFifo == NULL)
        {
            rmStatus = NV_ERR_INVALID_POINTER;
            SLI_LOOP_BREAK;
        }
        kfifoGetDeviceCaps(pGpu, pKernelFifo, pKfifoCaps, bCapsInitialized);
        bCapsInitialized = NV_TRUE;
    }
    SLI_LOOP_END

    return rmStatus;
}

/*!
 * @brief deviceCtrlCmdFifoGetCaps
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
deviceCtrlCmdFifoGetCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_FIFO_GET_CAPS_PARAMS *pKfifoCapsParams
)
{
    OBJGPU  *pGpu      = GPU_RES_GET_GPU(pDevice);
    NvU8    *pKfifoCaps = NvP64_VALUE(pKfifoCapsParams->capsTbl);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    // sanity check array size
    if (pKfifoCapsParams->capsTblSize != NV0080_CTRL_FIFO_CAPS_TBL_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR, "size mismatch: client 0x%x rm 0x%x\n",
                  pKfifoCapsParams->capsTblSize,
                  NV0080_CTRL_FIFO_CAPS_TBL_SIZE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // now accumulate caps for entire device
    return _kfifoGetCaps(pGpu, pKfifoCaps);
}

/*!
 * @brief deviceCtrlCmdFifoGetCapsV2
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
deviceCtrlCmdFifoGetCapsV2_IMPL
(
    Device *pDevice,
    NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS *pKfifoCapsParams
)
{
    OBJGPU    *pGpu      = GPU_RES_GET_GPU(pDevice);
    NvU8      *pKfifoCaps = pKfifoCapsParams->capsTbl;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    // now accumulate caps for entire device
    return _kfifoGetCaps(pGpu, pKfifoCaps);
}

/**
 * @brief Disables or enables the given channels.
 */
NV_STATUS
subdeviceCtrlCmdFifoDisableChannels_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pDisableChannelParams
)
{
    NV_STATUS       status        = NV_OK;
    OBJGPU         *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    CALL_CONTEXT   *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams   *pRmCtrlParams = pCallContext->pControlParams;

    // Validate use of pRunlistPreemptEvent to allow use by Kernel clients only
    if ((pDisableChannelParams->pRunlistPreemptEvent != NULL) &&
        (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // Send RPC to handle message on Host-RM
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
    }
    // Send internal control call to actually disable channels
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

    return status;
}
