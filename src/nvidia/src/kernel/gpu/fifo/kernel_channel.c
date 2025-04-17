/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_CHANNEL_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/fifo/kernel_channel.h"

#include "kernel/core/locks.h"
#include "gpu/subdevice/subdevice.h"
#include "kernel/diagnostics/gpu_acct.h"
#include "kernel/gpu/conf_compute/conf_compute.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/fifo/kernel_ctxshare.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/mem_mgr/context_dma.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/mem_mgr/ctx_buf_pool.h"
#include "kernel/mem_mgr/gpu_vaspace.h"
#include "kernel/rmapi/event.h"
#include "kernel/rmapi/rmapi.h"
#include "kernel/rmapi/rs_utils.h"
#include "kernel/rmapi/mapping_list.h"
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/timer/objtmr.h"
#include "platform/sli/sli.h"

#include "class/cl0090.h"   // KERNEL_GRAPHICS_CONTEXT
#include "class/cl906fsw.h" // GF100_GPFIFO
#include "class/cla06c.h"   // KEPLER_CHANNEL_GROUP_A
#include "class/cla06f.h"   // KEPLER_CHANNEL_GPFIFO_A
#include "class/cla06fsw.h" // KEPLER_CHANNEL_GPFIFO_A
#include "class/cla16f.h"   // KEPLER_CHANNEL_GPFIFO_B
#include "class/cla16fsw.h" // KEPLER_CHANNEL_GPFIFO_B
#include "class/clb06f.h"   // MAXWELL_CHANNEL_GPFIFO_A
#include "class/clb06fsw.h" // MAXWELL_CHANNEL_GPFIFO_A
#include "class/clc06f.h"   // PASCAL_CHANNEL_GPFIFO_A
#include "class/clc06fsw.h" // PASCAL_CHANNEL_GPFIFO_A
#include "class/clc36f.h"   // VOLTA_CHANNEL_GPFIFO_A
#include "class/clc36fsw.h" // VOLTA_CHANNEL_GPFIFO_A
#include "class/clc46f.h"   // TURING_CHANNEL_GPFIFO_A
#include "class/clc46fsw.h" // TURING_CHANNEL_GPFIFO_A
#include "class/clc56f.h"   // AMPERE_CHANNEL_GPFIFO_A
#include "class/clc56fsw.h" // AMPERE_CHANNEL_GPFIFO_A
#include "class/clc572.h"   // PHYSICAL_CHANNEL_GPFIFO
#include "class/clc86f.h"   // HOPPER_CHANNEL_GPFIFO_A
#include "class/clc86fsw.h" // HOPPER_CHANNEL_GPFIFO_A

#include "class/clc96f.h"   // BLACKWELL_CHANNEL_GPFIFO_A
#include "class/clc96fsw.h" // BLACKWELL_CHANNEL_GPFIFO_A

#include "class/clca6f.h"   // BLACKWELL_CHANNEL_GPFIFO_B
#include "class/clca6fsw.h" // BLACKWELL_CHANNEL_GPFIFO_B

#include "ctrl/ctrl906f.h"
#include "ctrl/ctrlc46f.h"
#include "ctrl/ctrlc86f.h"

#include "ctrl/ctrlc96f.h"

#include "ctrl/ctrlca6f.h"

#include "Nvcm.h"
#include "libraries/resserv/resserv.h"
#include "libraries/resserv/rs_client.h"
#include "libraries/resserv/rs_resource.h"
#include "libraries/resserv/rs_server.h"
#include "nvRmReg.h"
#include "nvstatuscodes.h"
#include "vgpu/rpc.h"

// Instmem static functions
static NV_STATUS _kchannelAllocHalData(OBJGPU *pGpu, KernelChannel *pKernelChannel);
static void      _kchannelFreeHalData(OBJGPU *pGpu, KernelChannel *pKernelChannel);
static NV_STATUS _kchannelAllocOrDescribeInstMem(
    KernelChannel  *pKernelChannel,
    NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams);
static NV_STATUS _kchannelDescribeMemDescsFromParams(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams);
static NV_STATUS _kchannelDescribeMemDescsHeavySriov(OBJGPU *pGpu, KernelChannel *pKernelChannel);
static NV_STATUS _kchannelSendChannelAllocRpc(
    KernelChannel *pKernelChannel,
    NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams,
    KernelChannelGroup *pKernelChannelGroup,
    NvBool bFullSriov);

static NV_STATUS _kchannelNotifyOfChid(OBJGPU *pGpu, KernelChannel *pKernelChannel, RsClient *pRsClient);
static NV_STATUS _kchannelGetUserMemDesc(OBJGPU *pGpu, KernelChannel *pKernelChannel, PMEMORY_DESCRIPTOR *ppMemDesc);
static void _kchannelUpdateFifoMapping(KernelChannel    *pKernelChannel,
                                       OBJGPU           *pGpu,
                                       NvBool            bKernel,
                                       NvP64             cpuAddress,
                                       NvP64             priv,
                                       NvU64             cpuMapLength,
                                       NvU32             flags,
                                       NvHandle          hSubdevice,
                                       RsCpuMapping     *pMapping);
static NvNotification*
_kchannelGetKeyRotationNotifier(KernelChannel *pKernelChannel);

/*!
 * @brief Construct a new KernelChannel, which also creates a Channel.
 *
 * @param[in,out]  pCallContext     The call context
 * @param[in,out]  pParams          Params for the *_CHANNEL_GPFIFO class
 *                                  object being created
 *
 * @returns NV_OK on success, specific error code on failure.
 */
NV_STATUS
kchannelConstruct_IMPL
(
    KernelChannel *pKernelChannel,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU                 *pGpu             = GPU_RES_GET_GPU(pKernelChannel);
    OBJSYS                 *pSys             = SYS_GET_INSTANCE();
    KernelMIGManager       *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KernelFifo             *pKernelFifo      = GPU_GET_KERNEL_FIFO(pGpu);
    RsClient               *pRsClient        = pCallContext->pClient;
    RmClient               *pRmClient        = NULL;
    RsResourceRef          *pResourceRef     = pCallContext->pResourceRef;
    RsResourceRef          *pKernelCtxShareRef = NULL;
    NV_STATUS               status;
    RM_API                 *pRmApi           = NULL;
    NvHandle                hClient          = pRsClient->hClient;
    NvHandle                hParent          = pResourceRef->pParentRef->hResource;
    NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams = pParams->pAllocParams;
    RsResourceRef          *pChanGrpRef      = NULL;
    KernelChannelGroupApi  *pKernelChannelGroupApi = NULL;
    NvHandle                hKernelCtxShare  = pChannelGpfifoParams->hContextShare;
    NvBool                  bTsgAllocated    = NV_FALSE;
    NvHandle                hChanGrp         = NV01_NULL_OBJECT;
    RsResourceRef          *pDeviceRef       = NULL;
    RsResourceRef          *pVASpaceRef      = NULL;
    KernelGraphicsContext  *pKernelGraphicsContext = NULL;
    NvBool                  bMIGInUse;
    KernelChannelGroup     *pKernelChannelGroup = NULL;
    NvU32                   chID             = ~0;
    NvU32                   flags            = pChannelGpfifoParams->flags;
    RM_ENGINE_TYPE          globalRmEngineType = RM_ENGINE_TYPE_NULL;
    NvU32                   verifFlags2      = 0;
    NvBool                  bChidAllocated   = NV_FALSE;
    NvBool                  bLockAcquired    = NV_FALSE;
    CTX_BUF_POOL_INFO      *pChannelBufPool  = NULL;
    CTX_BUF_INFO            bufInfo          = {0};
    NvBool                  bRpcAllocated    = NV_FALSE;
    NvBool                  bFullSriov       = IS_VIRTUAL_WITH_SRIOV(pGpu) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu);
    NvBool                  bAddedToGroup    = NV_FALSE;
    NvU32                   callingContextGfid;
    Device                 *pDevice;
    NvBool                  bUvmOwnedFlag;

    if (rmapiLockIsOwner())
    {
        pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    }
    else if (rmapiInRtd3PmPath())
    {
        pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Caller missing proper locks\n");
        return NV_ERR_INVALID_LOCK_STATE;
    }

    // We only support physical channels.
    NV_ASSERT_OR_RETURN(FLD_TEST_DRF(OS04, _FLAGS, _CHANNEL_TYPE, _PHYSICAL, flags),
        NV_ERR_NOT_SUPPORTED);

    pKernelChannel->refCount = 1;
    pKernelChannel->bIsContextBound = NV_FALSE;
    pKernelChannel->nextObjectClassID = 0;
    pKernelChannel->subctxId = 0;
    pKernelChannel->vaSpaceId = 0;
    pKernelChannel->bSkipCtxBufferAlloc = FLD_TEST_DRF(OS04, _FLAGS,
                                                       _SKIP_CTXBUFFER_ALLOC, _TRUE, flags);
    pKernelChannel->cid = portAtomicIncrementU32(&pSys->currentChannelUniqueId);
    pKernelChannel->runqueue = DRF_VAL(OS04, _FLAGS, _GROUP_CHANNEL_RUNQUEUE, flags);
    pKernelChannel->engineType = RM_ENGINE_TYPE_NULL;
    pChannelGpfifoParams->cid = pKernelChannel->cid;
    NV_ASSERT_OK_OR_RETURN(refFindAncestorOfType(pResourceRef, classId(Device), &pDeviceRef));
    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &callingContextGfid));

    pDevice = dynamicCast(pDeviceRef->pResource, Device);

    // Save the UVM owned flag.
    bUvmOwnedFlag = FLD_TEST_DRF(_KERNELCHANNEL, _ALLOC_INTERNALFLAGS, _UVM_OWNED, _YES, pChannelGpfifoParams->internalFlags);

    // Internal fields must be cleared when RMAPI call is from client
    if (!hypervisorIsVgxHyper() || IS_GSP_CLIENT(pGpu))
        pChannelGpfifoParams->hPhysChannelGroup = NV01_NULL_OBJECT;
    pChannelGpfifoParams->internalFlags = 0;
    portMemSet(&pChannelGpfifoParams->errorNotifierMem, 0,
               sizeof pChannelGpfifoParams->errorNotifierMem);
    portMemSet(&pChannelGpfifoParams->eccErrorNotifierMem, 0,
               sizeof pChannelGpfifoParams->eccErrorNotifierMem);
    pChannelGpfifoParams->ProcessID = 0;
    pChannelGpfifoParams->SubProcessID = 0;
    portMemSet(pChannelGpfifoParams->encryptIv, 0, sizeof(pChannelGpfifoParams->encryptIv));
    portMemSet(pChannelGpfifoParams->decryptIv, 0, sizeof(pChannelGpfifoParams->decryptIv));
    portMemSet(pChannelGpfifoParams->hmacNonce, 0, sizeof(pChannelGpfifoParams->hmacNonce));

    pRmClient = dynamicCast(pRsClient, RmClient);
    if (pRmClient == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    pKernelChannel->pUserInfo = pRmClient->pUserInfo;

    pKernelChannel->bGspOwned =
        FLD_TEST_DRF(_KERNELCHANNEL, _ALLOC_INTERNALFLAGS, _GSP_OWNED, _YES, pChannelGpfifoParams->internalFlags);
    NV_ASSERT_OR_RETURN(!pKernelChannel->bGspOwned || RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_INVALID_ARGUMENT);

    //
    // GSP-RM needs privilegeLevel passed in as an alloc param because it cannot
    // check pRmClient for kernel/admin.
    // Other platforms check pRmClient to determine privilegeLevel.
    //
    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        // Guest-RM clients can allocate a privileged channel to perform
        // actions such as updating page tables in physical mode or scrubbing.
        // Security for these channels is enforced by VMMU and IOMMU
        if (gpuIsSriovEnabled(pGpu) && IS_GFID_VF(callingContextGfid) &&
                FLD_TEST_DRF(OS04, _FLAGS, _PRIVILEGED_CHANNEL, _TRUE, flags))
        {
            pKernelChannel->privilegeLevel = NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_ADMIN;
        }
        else
        {
            pKernelChannel->privilegeLevel =
                DRF_VAL(_KERNELCHANNEL, _ALLOC_INTERNALFLAGS, _PRIVILEGE, pChannelGpfifoParams->internalFlags);
        }

        // In GSP, all vGPU channel's will simply consider GFID as the processID
        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_GFID_VF(callingContextGfid))
        {
            pKernelChannel->ProcessID = callingContextGfid;
        }
        else
        {
            pKernelChannel->ProcessID = pChannelGpfifoParams->ProcessID;
        }

        pKernelChannel->SubProcessID = pChannelGpfifoParams->SubProcessID;
    }
    else
    {
        RS_PRIV_LEVEL privLevel = pCallContext->secInfo.privLevel;
        if (privLevel >= RS_PRIV_LEVEL_KERNEL)
        {
            pKernelChannel->privilegeLevel = NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_KERNEL;
            pChannelGpfifoParams->flags = FLD_SET_DRF(OS04, _FLAGS, _PRIVILEGED_CHANNEL, _TRUE, pChannelGpfifoParams->flags);
        }
        else if (rmclientIsAdmin(pRmClient, privLevel) || hypervisorCheckForObjectAccess(hClient))
        {
            pKernelChannel->privilegeLevel = NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_ADMIN;
            pChannelGpfifoParams->flags = FLD_SET_DRF(OS04, _FLAGS, _PRIVILEGED_CHANNEL, _TRUE, pChannelGpfifoParams->flags);
        }
        else
        {
            pKernelChannel->privilegeLevel = NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_USER;
        }

        pKernelChannel->ProcessID = pRmClient->ProcID;
        pKernelChannel->SubProcessID = pRmClient->SubProcessID;
    }

    //
    // Mark the channel as UVM owned only if the channel is kernel privileged
    // and flagged as UVM owned.
    //
    if (pKernelChannel->privilegeLevel == NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_KERNEL)
    {
        pKernelChannel->bUvmOwned = bUvmOwnedFlag;
    }

    // Context share and vaspace handles can't be active at the same time.
    if ((hKernelCtxShare != NV01_NULL_OBJECT) && (pChannelGpfifoParams->hVASpace != NV01_NULL_OBJECT))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Both context share and vaspace handles can't be valid at the same time\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    bMIGInUse = IS_MIG_IN_USE(pGpu);

    //
    // The scrubber is allocated by Kernel RM in offload mode, and is disabled
    // completely on GSP, so it is not possible for GSP to determine whether
    // this allocation should be allowed or not. CPU RM can and should properly
    // check this.
    //
    if (IS_MIG_ENABLED(pGpu) && !RMCFG_FEATURE_PLATFORM_GSP && !bMIGInUse)
    {
        NvBool bTopLevelScrubberEnabled = NV_FALSE;
        NvBool bTopLevelScrubberConstructed = NV_FALSE;
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        if (memmgrIsPmaInitialized(pMemoryManager))
        {
            Heap *pHeap = GPU_GET_HEAP(pGpu);
            NvU32 pmaConfigs = PMA_QUERY_SCRUB_ENABLED | PMA_QUERY_SCRUB_VALID;
            NV_ASSERT_OK(pmaQueryConfigs(&pHeap->pmaObject, &pmaConfigs));
            bTopLevelScrubberEnabled = (pmaConfigs & PMA_QUERY_SCRUB_ENABLED) != 0x0;
            bTopLevelScrubberConstructed = (pmaConfigs & PMA_QUERY_SCRUB_VALID) != 0x0;
        }

        //
        // Exception: Top level scrubber must be initialized before
        // GPU instances can be created, and therefore must be allowed to
        // create a CE context if the scrubber is supported.
        //

        if (!bTopLevelScrubberEnabled || bTopLevelScrubberConstructed ||
            !kchannelCheckIsKernel(pKernelChannel))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Channel allocation not allowed when MIG is enabled without GPU instancing\n");
            return NV_ERR_INVALID_STATE;
        }
    }

    // Find the TSG, or create the TSG if we need to wrap it
    status = clientGetResourceRefByType(pRsClient, hParent,
                                        classId(KernelChannelGroupApi),
                                        &pChanGrpRef);
    if (status != NV_OK)
    {
        NV_CHANNEL_GROUP_ALLOCATION_PARAMETERS tsgParams = { 0 };

        // Context share can only be used with a TSG channel
        if (hKernelCtxShare != NV01_NULL_OBJECT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Non-TSG channels can't use context share\n");
            status = NV_ERR_INVALID_ARGUMENT;
            goto cleanup;
        }

        tsgParams.hVASpace = pChannelGpfifoParams->hVASpace;
        tsgParams.engineType = pChannelGpfifoParams->engineType;
        // vGpu plugin context flag should only be set if context is plugin
        if (gpuIsSriovEnabled(pGpu))
        {
            tsgParams.bIsCallingContextVgpuPlugin = FLD_TEST_DRF(OS04, _FLAGS, _CHANNEL_VGPU_PLUGIN_CONTEXT, _TRUE, pChannelGpfifoParams->flags);
        }
        //
        // Internally allocate a TSG to wrap this channel. There is no point
        // in mirroring this allocation in the host, as the channel is
        // already mirrored.
        //
        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->AllocWithSecInfo(pRmApi,
                                     hClient,
                                     hParent,
                                     &pChannelGpfifoParams->hPhysChannelGroup,
                                     KEPLER_CHANNEL_GROUP_A,
                                     NV_PTR_TO_NvP64(&tsgParams),
                                     sizeof(tsgParams),
                                     RMAPI_ALLOC_FLAGS_SKIP_RPC,
                                     NvP64_NULL,
                                     &pRmApi->defaultSecInfo),
                          cleanup);
        bTsgAllocated = NV_TRUE;
        hChanGrp = pChannelGpfifoParams->hPhysChannelGroup;

        NV_ASSERT_OK_OR_GOTO(status,
            clientGetResourceRefByType(pRsClient, hChanGrp,
                                       classId(KernelChannelGroupApi),
                                       &pChanGrpRef),
                             cleanup);

        pKernelChannelGroupApi = dynamicCast(pChanGrpRef->pResource,
                                             KernelChannelGroupApi);
        pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;
        pKernelChannelGroup->bAllocatedByRm = NV_TRUE;
    }
    else
    {
        hChanGrp = hParent;
        pKernelChannelGroupApi = dynamicCast(pChanGrpRef->pResource,
                                             KernelChannelGroupApi);
        if (pKernelChannelGroupApi == NULL ||
            pKernelChannelGroupApi->pKernelChannelGroup == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid KernelChannelGroup* for channel 0x%x\n",
                      pResourceRef->hResource);
            status = NV_ERR_INVALID_POINTER;
            NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);
        }
        pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;

        // TSG channel should specify a context share object, rather than vaspace directly
        if (pChannelGpfifoParams->hVASpace != NV01_NULL_OBJECT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "TSG channels can't use an explicit vaspace\n");
            status = NV_ERR_INVALID_ARGUMENT;
            NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);
        }
    }
    pKernelChannel->pKernelChannelGroupApi = pKernelChannelGroupApi;

    NV_ASSERT_OR_RETURN(pKernelChannelGroupApi != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_STATE);

    //
    // Reserve memory for channel instance block from PMA
    // into a pool tied to channel's parent TSG.
    // RM will later allocate memory for instance block from this pool.
    //
    pChannelBufPool = pKernelChannelGroup->pChannelBufPool;
    if (pChannelBufPool != NULL)
    {
        NvBool bIsScrubSkipped;
        NvBool bRequestScrubSkip = FLD_TEST_DRF(OS04, _FLAGS, _CHANNEL_SKIP_SCRUBBER, _TRUE, pChannelGpfifoParams->flags);

        if (bRequestScrubSkip)
        {
            if (!kchannelCheckIsKernel(pKernelChannel))
            {
                status = NV_ERR_INVALID_ARGUMENT;
                NV_PRINTF(LEVEL_ERROR, "Only kernel priv clients can skip scrubber\n");
                goto cleanup;
            }

            //
            // If this is first channel in the TSG then setup ctx buf pool to skip scrubbing.
            // For subsequent channels, setting should match with ctx buf pool's state.
            //
            if (pKernelChannelGroup->chanCount == 0)
            {
                ctxBufPoolSetScrubSkip(pChannelBufPool, NV_TRUE);
                NV_PRINTF(LEVEL_INFO, "Skipping scrubber for all allocations on this context\n");
            }
        }

        bIsScrubSkipped = ctxBufPoolIsScrubSkipped(pChannelBufPool);
        if (bIsScrubSkipped ^ bRequestScrubSkip)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            NV_PRINTF(LEVEL_ERROR, "Mismatch between channel and parent TSG's policy on skipping scrubber\n");
            NV_PRINTF(LEVEL_ERROR, "scrubbing %s skipped for TSG and %s for channel\n", (bIsScrubSkipped ? "is" : "is not"),
                (bRequestScrubSkip ? "is" : "is not"));
            goto cleanup;
        }
        NV_ASSERT_OK_OR_GOTO(status,
                             kfifoGetInstMemInfo_HAL(pKernelFifo, &bufInfo.size, &bufInfo.align, NULL, NULL, NULL),
                             cleanup);
        bufInfo.attr = RM_ATTR_PAGE_SIZE_DEFAULT;
        NV_ASSERT_OK_OR_GOTO(status, ctxBufPoolReserve(pGpu, pChannelBufPool, &bufInfo, 1), cleanup);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "Not using ctx buf pool\n");
    }

    //--------------------------------------------------------------------------
    // we acquire the GPU lock below.
    // From here down do not return early, use goto cleanup
    //--------------------------------------------------------------------------

    NV_ASSERT_OK_OR_GOTO(status,
        rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_FIFO),
        cleanup);
    bLockAcquired = NV_TRUE;

    //
    // Initialize the notification indices used for different notifications
    //
    pKernelChannel->notifyIndex[NV_CHANNELGPFIFO_NOTIFICATION_TYPE_ERROR]
        = NV_CHANNELGPFIFO_NOTIFICATION_TYPE_ERROR;
    pKernelChannel->notifyIndex[NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN]
        = NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN;

    // Bake channel group error handlers into the channel
    pKernelChannel->hErrorContext = pChannelGpfifoParams->hObjectError;
    pKernelChannel->hEccErrorContext = pChannelGpfifoParams->hObjectEccError;

    if (pKernelChannel->hErrorContext == NV01_NULL_OBJECT)
    {
        pKernelChannel->hErrorContext = (
            pKernelChannel->pKernelChannelGroupApi->hErrorContext);
    }
    if (pKernelChannel->hEccErrorContext == NV01_NULL_OBJECT)
    {
        pKernelChannel->hEccErrorContext = (
            pKernelChannel->pKernelChannelGroupApi->hEccErrorContext);
    }

    if (pKernelChannel->hErrorContext == NV01_NULL_OBJECT)
    {
        pKernelChannel->errorContextType = ERROR_NOTIFIER_TYPE_NONE;
    }
    else if (!(RMCFG_FEATURE_PLATFORM_GSP && !pKernelChannel->bGspOwned))
    {
        NV_ASSERT_OK(kchannelGetNotifierInfo(pGpu, pDevice,
            pKernelChannel->hErrorContext,
            &pKernelChannel->pErrContextMemDesc,
            &pKernelChannel->errorContextType,
            &pKernelChannel->errorContextOffset));
        NV_ASSERT(pKernelChannel->errorContextType !=
                  ERROR_NOTIFIER_TYPE_NONE);
    }
    if (pKernelChannel->hEccErrorContext == NV01_NULL_OBJECT)
    {
        pKernelChannel->eccErrorContextType = ERROR_NOTIFIER_TYPE_NONE;
    }
    else if (!(RMCFG_FEATURE_PLATFORM_GSP && !pKernelChannel->bGspOwned))
    {
        NV_ASSERT_OK(kchannelGetNotifierInfo(pGpu, pDevice,
            pKernelChannel->hEccErrorContext,
            &pKernelChannel->pEccErrContextMemDesc,
            &pKernelChannel->eccErrorContextType,
            &pKernelChannel->eccErrorContextOffset));
        NV_ASSERT(pKernelChannel->eccErrorContextType !=
                  ERROR_NOTIFIER_TYPE_NONE);
    }

    if (IS_GSP_CLIENT(pGpu) || bFullSriov)
    {
        if (pKernelChannel->hErrorContext != NV01_NULL_OBJECT)
        {
            NV_ASSERT_TRUE_OR_GOTO(status,
                pKernelChannel->pErrContextMemDesc,
                NV_ERR_INVALID_STATE,
                cleanup);
            pChannelGpfifoParams->errorNotifierMem.base = (
                memdescGetPhysAddr(pKernelChannel->pErrContextMemDesc,
                                   AT_GPU, 0) +
                pKernelChannel->errorContextOffset);
            pChannelGpfifoParams->errorNotifierMem.size = (
                pKernelChannel->pErrContextMemDesc->Size -
                pKernelChannel->errorContextOffset);
            pChannelGpfifoParams->errorNotifierMem.addressSpace =
                memdescGetAddressSpace(pKernelChannel->pErrContextMemDesc);
            pChannelGpfifoParams->errorNotifierMem.cacheAttrib =
                memdescGetCpuCacheAttrib(pKernelChannel->pErrContextMemDesc);

        }
        if (pKernelChannel->hEccErrorContext != NV01_NULL_OBJECT)
        {
            NV_ASSERT_TRUE_OR_GOTO(status,
                pKernelChannel->pEccErrContextMemDesc,
                NV_ERR_INVALID_STATE,
                cleanup);
            pChannelGpfifoParams->eccErrorNotifierMem.base = (
                memdescGetPhysAddr(pKernelChannel->pEccErrContextMemDesc,
                                   AT_GPU, 0) +
                pKernelChannel->eccErrorContextOffset);
            pChannelGpfifoParams->eccErrorNotifierMem.size = (
                pKernelChannel->pEccErrContextMemDesc->Size -
                pKernelChannel->eccErrorContextOffset);
            pChannelGpfifoParams->eccErrorNotifierMem.addressSpace =
                memdescGetAddressSpace(pKernelChannel->pEccErrContextMemDesc);
            pChannelGpfifoParams->eccErrorNotifierMem.cacheAttrib =
                memdescGetCpuCacheAttrib(pKernelChannel->pEccErrContextMemDesc);
        }

        pChannelGpfifoParams->internalFlags = FLD_SET_DRF_NUM(
            _KERNELCHANNEL_ALLOC, _INTERNALFLAGS, _ERROR_NOTIFIER_TYPE,
            pKernelChannel->errorContextType,
            pChannelGpfifoParams->internalFlags);
        pChannelGpfifoParams->internalFlags = FLD_SET_DRF_NUM(
            _KERNELCHANNEL_ALLOC, _INTERNALFLAGS, _ECC_ERROR_NOTIFIER_TYPE,
            pKernelChannel->eccErrorContextType,
            pChannelGpfifoParams->internalFlags);
    }

    //
    // The error context types should be set on all RM configurations
    // (GSP/baremetal/CPU-GSP client)
    //
    NV_ASSERT(pKernelChannel->errorContextType != ERROR_NOTIFIER_TYPE_UNKNOWN);
    NV_ASSERT(pKernelChannel->eccErrorContextType !=
              ERROR_NOTIFIER_TYPE_UNKNOWN);


    if ((pKernelChannelGroup->chanCount != 0) &&
        (( pKernelChannelGroup->bLegacyMode && (hKernelCtxShare != NV01_NULL_OBJECT)) ||
         (!pKernelChannelGroup->bLegacyMode && (hKernelCtxShare == NV01_NULL_OBJECT))))
    {
        //
        // Check if this channnel allocation specifying (or not) a user
        // allocated context share matches with previous channel allocations (if
        // any) in this group specifiying (or not) a user allocated context
        // share.
        //
        // A channel group cannot have a mix of channels with some of them
        // specifying a user allocated context share and some having RM
        // allocated context share.
        //
        NV_PRINTF(LEVEL_NOTICE,
            "All channels in a channel group must specify a CONTEXT_SHARE if any one of them specifies it\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto cleanup;
    }

    // Get KernelCtxShare (supplied or legacy)
    if (hKernelCtxShare != NV01_NULL_OBJECT)
    {
        // Get object pointers from supplied hKernelCtxShare.
        NV_ASSERT_OK_OR_GOTO(status,
            clientGetResourceRefByType(pRsClient,
                                       hKernelCtxShare,
                                       classId(KernelCtxShareApi),
                                       &pKernelCtxShareRef),
            cleanup);

        //
        // If hKernelCtxShare is nonzero, the ChannelGroup is not internal
        // either, so it should have the same parent as hParent.
        //
        NV_ASSERT_TRUE_OR_GOTO(status,
            pKernelCtxShareRef->pParentRef != NULL &&
                pKernelCtxShareRef->pParentRef->hResource == hParent,
            NV_ERR_INVALID_OBJECT_PARENT,
            cleanup);
    }
    else
    {
        NvU32 subctxFlag;
        NvHandle hLegacyKernelCtxShare;

        if (!pKernelChannelGroup->bLegacyMode)
        {
            //
            // Set this ChannelGroup to legacy mode and get the KernelCtxShare
            // from it.
            //
            NV_ASSERT_OK_OR_GOTO(status,
                kchangrpapiSetLegacyMode(pKernelChannelGroupApi,
                                         pGpu, pKernelFifo, hClient),
                cleanup);
        }

        subctxFlag = DRF_VAL(OS04, _FLAGS, _GROUP_CHANNEL_THREAD, flags);
        hLegacyKernelCtxShare = (subctxFlag ==
                           NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SYNC) ?
                              pKernelChannelGroupApi->hLegacykCtxShareSync :
                              pKernelChannelGroupApi->hLegacykCtxShareAsync;

        NV_ASSERT_OK_OR_GOTO(status,
            clientGetResourceRefByType(pRsClient,
                                       hLegacyKernelCtxShare,
                                       classId(KernelCtxShareApi),
                                       &pKernelCtxShareRef),
            cleanup);
    }

    pKernelChannel->pKernelCtxShareApi = dynamicCast(
        pKernelCtxShareRef->pResource,
        KernelCtxShareApi);
    NV_ASSERT_TRUE_OR_GOTO(status,
                           pKernelChannel->pKernelCtxShareApi != NULL,
                           NV_ERR_INVALID_OBJECT,
                           cleanup);
    NV_ASSERT_TRUE_OR_GOTO(status,
                           pKernelChannel->pKernelCtxShareApi->pShareData !=
                               NULL,
                           NV_ERR_INVALID_OBJECT,
                           cleanup);
    pKernelChannel->pVAS = pKernelChannel->pKernelCtxShareApi->pShareData->pVAS;
    NV_ASSERT_TRUE_OR_GOTO(status,
                           pKernelChannel->pVAS != NULL,
                           NV_ERR_INVALID_OBJECT,
                           cleanup);

    if (kfifoIsPerRunlistChramSupportedInHw(pKernelFifo))
    {
        // TSG should always have a valid engine Id.
        NV_ASSERT_TRUE_OR_GOTO(status,
            RM_ENGINE_TYPE_IS_VALID(pKernelChannelGroup->engineType),
            NV_ERR_INVALID_STATE,
            cleanup);

        if (NV2080_ENGINE_TYPE_IS_VALID(pChannelGpfifoParams->engineType))
        {
            globalRmEngineType = gpuGetRmEngineType(pChannelGpfifoParams->engineType);
            // Convert it to global engine id if MIG is enabled
            if (bMIGInUse)
            {
                MIG_INSTANCE_REF ref;

                NV_CHECK_OK_OR_GOTO(
                    status,
                    LEVEL_ERROR,
                    kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                    pDevice, &ref),
                    cleanup);

                NV_CHECK_OK_OR_GOTO(
                    status,
                    LEVEL_ERROR,
                    kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                                      globalRmEngineType,
                                                      &globalRmEngineType),
                    cleanup);
            }

            // Throw an error if TSG engine Id does NOT match with channel engine Id
            if (globalRmEngineType != pKernelChannelGroup->engineType)
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Engine type of channel = 0x%x (0x%x) not compatible with engine type of TSG = 0x%x (0x%x)\n",
                    gpuGetNv2080EngineType(pChannelGpfifoParams->engineType),
                    pChannelGpfifoParams->engineType,
                    gpuGetNv2080EngineType(pKernelChannelGroup->engineType),
                    pKernelChannelGroup->engineType);

                status = NV_ERR_INVALID_ARGUMENT;
                goto cleanup;
            }
        }

        // Assign the engine type from the parent TSG
        pKernelChannel->engineType = pKernelChannelGroup->engineType;
    }

    // Determine initial runlist ID (based on engine type if provided or inherited from TSG)
    pKernelChannel->runlistId = kfifoGetDefaultRunlist_HAL(pGpu, pKernelFifo, pKernelChannel->engineType);

    pKernelChannel->bCCSecureChannel = FLD_TEST_DRF(OS04, _FLAGS, _CC_SECURE, _TRUE, flags);
    pKernelChannel->bUseScrubKey = FLD_TEST_DRF(OS04, _FLAGS, _CHANNEL_SKIP_SCRUBBER, _TRUE, pChannelGpfifoParams->flags);
    if (pKernelChannel->bCCSecureChannel)
    {
        ConfidentialCompute* pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);

        // return early if gpu is not ready to accept work
        if ((pConfCompute != NULL) && kchannelCheckIsUserMode(pKernelChannel)
            && !confComputeAcceptClientRequest(pGpu, pConfCompute))
        {
            return NV_ERR_NOT_READY;
        }

        if ((pConfCompute != NULL) && pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED))
        {
            KEY_ROTATION_STATUS state;
            NvU32 h2dKey;
            NV_ASSERT_OK_OR_GOTO(status,
                                 confComputeGetKeyPairByChannel(pGpu, pConfCompute, pKernelChannel, &h2dKey, NULL),
                                 cleanup);
            NV_ASSERT_OK_OR_GOTO(status,
                                 confComputeGetKeyRotationStatus(pConfCompute, h2dKey, &state),
                                 cleanup);
            if (state != KEY_ROTATION_STATUS_IDLE)
            {
                status = NV_ERR_KEY_ROTATION_IN_PROGRESS;
                goto cleanup;
            }
        }
        status = kchannelDeriveAndRetrieveKmb_HAL(pGpu, pKernelChannel, ROTATE_IV_ALL_VALID,
                                                  NV_TRUE, &pKernelChannel->clientKmb);
        NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

        portMemCopy(pChannelGpfifoParams->encryptIv,
                    sizeof(pChannelGpfifoParams->encryptIv),
                    pKernelChannel->clientKmb.encryptBundle.iv,
                    sizeof(pKernelChannel->clientKmb.encryptBundle.iv));

        portMemCopy(pChannelGpfifoParams->decryptIv,
                    sizeof(pChannelGpfifoParams->decryptIv),
                    pKernelChannel->clientKmb.decryptBundle.iv,
                    sizeof(pKernelChannel->clientKmb.decryptBundle.iv));

        portMemCopy(pChannelGpfifoParams->hmacNonce,
                    sizeof(pChannelGpfifoParams->hmacNonce),
                    pKernelChannel->clientKmb.hmacBundle.nonce,
                    sizeof(pKernelChannel->clientKmb.hmacBundle.nonce));

    }

    // Set TLS state and BAR0 window if we are working with Gr
    if (bMIGInUse && RM_ENGINE_TYPE_IS_GR(pKernelChannel->engineType))
    {
        NV_ASSERT_OK(kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                     pDevice, &pKernelChannel->partitionRef));
    }

    // Allocate the ChId (except legacy VGPU which allocates ChID on the host)
    if (!IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        status = kchannelAllocHwID_HAL(pGpu, pKernelChannel, hClient,
                                       flags, verifFlags2, chID);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Error in Allocating channel id 0x%x for hClient 0x%x hKernelChannel 0x%x \n",
                                   chID, hClient, pResourceRef->hResource);
            DBG_BREAKPOINT();
            goto cleanup;

        }

        chID = pKernelChannel->ChID;
        bChidAllocated = NV_TRUE;
    }

    //
    // RPC alloc the channel in legacy VGPU / Heavy SRIOV so that instmem details can be gotten from it
    //
    if (IS_VIRTUAL(pGpu) && (!bFullSriov))
    {
        NV_ASSERT_OK_OR_GOTO(status,
                             _kchannelSendChannelAllocRpc(pKernelChannel,
                                                          pChannelGpfifoParams,
                                                          pKernelChannelGroup,
                                                          bFullSriov),
                             cleanup);
        bRpcAllocated = NV_TRUE;
    }

    // Legacy VGPU: allocate chid that the host provided
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        chID = pKernelChannel->ChID;

        status = kchannelAllocHwID_HAL(pGpu, pKernelChannel, hClient,
                                       flags, verifFlags2, chID);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Error in Allocating channel id 0x%x for hClient 0x%x hKernelChannel 0x%x \n",
                      chID, hClient, pResourceRef->hResource);
            chID = ~0;
            DBG_BREAKPOINT();
            goto cleanup;
        }

        bChidAllocated = NV_TRUE;
    }

    //
    // Do instmem setup here
    // (Requires the channel to be created on the host if legacy VGPU / Heavy SRIOV.
    // Does not require a Channel object.)
    //
    NV_ASSERT_OK_OR_GOTO(status,
        _kchannelAllocOrDescribeInstMem(pKernelChannel, pChannelGpfifoParams),
        cleanup);

    // Join the channel group here
    NV_ASSERT_OK_OR_GOTO(status,
        kchangrpAddChannel(pGpu, pKernelChannelGroup, pKernelChannel),
        cleanup);
    bAddedToGroup = NV_TRUE;

    // Assign to the same runlistId as the KernelChannelGroup if it's already determined
    if (pKernelChannelGroup->bRunlistAssigned)
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        {
            NV_ASSERT_OK_OR_ELSE(status,
                kfifoRunlistSetId_HAL(pGpu,
                                      GPU_GET_KERNEL_FIFO(pGpu),
                                      pKernelChannel,
                                      pKernelChannelGroup->runlistId),
                SLI_LOOP_GOTO(cleanup));
        }
        SLI_LOOP_END
     }

    // Allocate the physical channel
    NV_ASSERT_OK_OR_GOTO(status,
        kchannelAllocChannel_HAL(pKernelChannel, pChannelGpfifoParams),
        cleanup);

    // Initialize the userd length
    if (!pKernelChannel->bClientAllocatedUserD)
    {
        NvU64 temp_offset;

        kchannelGetUserdInfo_HAL(pGpu,
                                 pKernelChannel,
                                 NULL,
                                 &temp_offset,
                                 &pKernelChannel->userdLength);
    }
    else
    {
        kfifoGetUserdSizeAlign_HAL(pKernelFifo, (NvU32*)&pKernelChannel->userdLength, NULL);
    }

    // Set GPU accounting
    if (RMCFG_MODULE_GPUACCT &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON))
    {
        GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(SYS_GET_INSTANCE());

        gpuacctSetProcType(pGpuAcct,
                           pGpu->gpuInstance,
                           pRmClient->ProcID,
                           pRmClient->SubProcessID,
                           NV_GPUACCT_PROC_TYPE_GPU);
    }

    //
    // RPC to allocate the channel on GSPFW/host.
    // (Requires a Channel object but only for hPhysChannel.)
    //
    if (IS_GSP_CLIENT(pGpu) || bFullSriov)
    {
        NV_ASSERT_OK_OR_GOTO(status,
                             _kchannelSendChannelAllocRpc(pKernelChannel,
                                                          pChannelGpfifoParams,
                                                          pKernelChannelGroup,
                                                          bFullSriov),
                             cleanup);
        bRpcAllocated = NV_TRUE;
    }

    if (kfifoIsPerRunlistChramEnabled(pKernelFifo) ||
        (gpuIsCCorApmFeatureEnabled(pGpu) || bMIGInUse))
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        {
            KernelFifo *pTempKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
            //
            // If we have a separate channel RAM for each runlist then we need to set
            // runlistId as we already picked a chID from channel RAM based on this runlistId.
            // This will also ensure runlistId is not overridden later to a different value
            //
            NV_ASSERT_OK_OR_GOTO(status,
                kfifoRunlistSetId_HAL(pGpu, pTempKernelFifo, pKernelChannel, pKernelChannel->runlistId),
                cleanup);
        }
        SLI_LOOP_END;
    }

    //
    // If we alloced this group, we want to free KernelChannel first,
    // so we should set KernelChannel as its dependent.
    //
    if (bTsgAllocated)
    {
        NV_ASSERT_OK_OR_GOTO(status, refAddDependant(pChanGrpRef, pResourceRef), cleanup);
    }

    // We depend on VASpace if it was provided
    if (pChannelGpfifoParams->hVASpace != NV01_NULL_OBJECT)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            clientGetResourceRef(pRsClient,
                                 pChannelGpfifoParams->hVASpace,
                                 &pVASpaceRef),
            cleanup);
        NV_ASSERT_TRUE_OR_GOTO(status,
                               pVASpaceRef != NULL,
                               NV_ERR_INVALID_OBJECT,
                               cleanup);
        NV_ASSERT_OK_OR_GOTO(status,
                             refAddDependant(pVASpaceRef, pResourceRef),
                             cleanup);
    }

    //
    // If KernelCtxShare was provided, we depend on it (and if we created it then we
    // also want KernelChannel to be freed first.)
    //
    if (pKernelChannel->pKernelCtxShareApi != NULL)
    {
        NV_ASSERT_OK_OR_GOTO(
            status,
            refAddDependant(RES_GET_REF(pKernelChannel->pKernelCtxShareApi), pResourceRef),
            cleanup);
    }

    pKernelChannel->hKernelGraphicsContext = pKernelChannelGroupApi->hKernelGraphicsContext;
    if (pKernelChannel->hKernelGraphicsContext != NV01_NULL_OBJECT)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            kgrctxFromKernelChannel(pKernelChannel, &pKernelGraphicsContext),
            cleanup);

        NV_ASSERT_OK_OR_GOTO(status,
            refAddDependant(RES_GET_REF(pKernelGraphicsContext), pResourceRef),
            cleanup);
    }

    if (pChannelGpfifoParams->hObjectError != 0)
    {
        NV_ASSERT_OK_OR_GOTO(
            status,
            _kchannelNotifyOfChid(pGpu, pKernelChannel, pRsClient),
            cleanup);
    }

    // Cache the hVASpace for this channel in the KernelChannel object
    pKernelChannel->hVASpace = pKernelChannel->pKernelCtxShareApi->hVASpace;

    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    if ((pConfCompute != NULL) &&
        (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED)) &&
        (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED)) &&
        (pKernelChannel->bCCSecureChannel))
    {
        // Create persistent mapping to key rotation notifier
        NV_ASSERT_OK_OR_GOTO(
            status,
            kchannelSetKeyRotationNotifier_HAL(pGpu, pKernelChannel, NV_TRUE),
            cleanup);
    }

cleanup:
    if (bLockAcquired)
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    // These fields are only needed internally; clear them here
    pChannelGpfifoParams->hPhysChannelGroup = 0;
    pChannelGpfifoParams->internalFlags = 0;
    portMemSet(&pChannelGpfifoParams->errorNotifierMem, 0,
               sizeof pChannelGpfifoParams->errorNotifierMem);
    portMemSet(&pChannelGpfifoParams->eccErrorNotifierMem, 0,
               sizeof pChannelGpfifoParams->eccErrorNotifierMem);
    pChannelGpfifoParams->ProcessID = 0;
    pChannelGpfifoParams->SubProcessID = 0;
    portMemSet(pChannelGpfifoParams->encryptIv, 0, sizeof(pChannelGpfifoParams->encryptIv));
    portMemSet(pChannelGpfifoParams->decryptIv, 0, sizeof(pChannelGpfifoParams->decryptIv));
    portMemSet(pChannelGpfifoParams->hmacNonce, 0, sizeof(pChannelGpfifoParams->hmacNonce));

    // Free the allocated resources if there was an error
    if (status != NV_OK)
    {
        // Remove any dependencies we may have added; we don't want our destructor called when freeing anything below
        if (pKernelGraphicsContext != NULL)
        {
            refRemoveDependant(RES_GET_REF(pKernelGraphicsContext), pResourceRef);
        }
        if (pKernelChannel->pKernelCtxShareApi != NULL)
        {
            refRemoveDependant(RES_GET_REF(pKernelChannel->pKernelCtxShareApi), pResourceRef);
        }
        if (pVASpaceRef != NULL)
        {
            refRemoveDependant(pVASpaceRef, pResourceRef);
        }
        if (bTsgAllocated)
        {
            refRemoveDependant(pChanGrpRef, pResourceRef);
        }

        if (bAddedToGroup)
        {
            kchangrpRemoveChannel(pGpu, pKernelChannelGroup, pKernelChannel);
        }

        if (RMCFG_FEATURE_PLATFORM_GSP && !pKernelChannel->bGspOwned)
        {
            // Free memdescs created during construct on GSP path.
            memdescFree(pKernelChannel->pErrContextMemDesc);
            memdescDestroy(pKernelChannel->pErrContextMemDesc);
            memdescFree(pKernelChannel->pEccErrContextMemDesc);
            memdescDestroy(pKernelChannel->pEccErrContextMemDesc);
        }
        pKernelChannel->pErrContextMemDesc = NULL;
        pKernelChannel->pEccErrContextMemDesc = NULL;

        if (bRpcAllocated)
        {
            NV_RM_RPC_FREE_ON_ERROR(pGpu, hClient, hParent, RES_GET_HANDLE(pKernelChannel));
        }

        _kchannelFreeHalData(pGpu, pKernelChannel);

        if (pChannelBufPool != NULL)
        {
            ctxBufPoolRelease(pChannelBufPool);
        }

        if (bTsgAllocated)
        {
            pRmApi->Free(pRmApi, hClient, hChanGrp);
        }

        if (bChidAllocated)
        {
            kchannelFreeHwID_HAL(pGpu, pKernelChannel);
        }
    }

    return status;
}

void
kchannelDestruct_IMPL
(
    KernelChannel *pKernelChannel
)
{
    CALL_CONTEXT                *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    NvHandle                     hClient;
    RM_API                      *pRmApi = NULL;
    OBJGPU                      *pGpu   = GPU_RES_GET_GPU(pKernelChannel);
    NV_STATUS                    status = NV_OK;
    KernelChannelGroup          *pKernelChannelGroup = NULL;

    NV_ASSERT(pKernelChannel->pKernelChannelGroupApi != NULL);
    pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;
    NV_ASSERT(pKernelChannelGroup != NULL);

    resGetFreeParams(staticCast(pKernelChannel, RsResource), &pCallContext, &pParams);
    hClient = pCallContext->pClient->hClient;

    if (rmapiLockIsOwner())
    {
        pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    }
    else if (rmapiInRtd3PmPath())
    {
        pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Caller missing proper locks\n");
        return;
    }

    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    NvBool bCheckKeyRotation = NV_FALSE;
    NvU32 h2dKey, d2hKey;
    if ((pConfCompute != NULL) &&
        (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED)) &&
        (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED)) &&
        (pKernelChannel->bCCSecureChannel))
    {
        NV_ASSERT_OK(confComputeUpdateFreedChannelStats(pGpu, pConfCompute, pKernelChannel));

        // check if we need to trigger key rotation after freeing this channel
        KEY_ROTATION_STATUS state;
        NV_ASSERT_OK(confComputeGetKeyPairByChannel(pGpu, pConfCompute, pKernelChannel, &h2dKey, &d2hKey));
        NV_ASSERT_OK(confComputeGetKeyRotationStatus(pConfCompute, h2dKey, &state));
        if ((state == KEY_ROTATION_STATUS_PENDING) ||
            (state == KEY_ROTATION_STATUS_PENDING_TIMER_SUSPENDED))
        {
            bCheckKeyRotation = NV_TRUE;
        }

        NV_ASSERT_OK(kchannelSetEncryptionStatsBuffer_HAL(pGpu, pKernelChannel, NULL, NV_FALSE));
        pRmApi->Free(pRmApi, hClient, pKernelChannel->hEncryptStatsBuf);
        NV_ASSERT_OK(kchannelSetKeyRotationNotifier_HAL(pGpu, pKernelChannel, NV_FALSE));
    }

    if (RMCFG_FEATURE_PLATFORM_GSP && !pKernelChannel->bGspOwned)
    {
        // Free memdescs created during construct on GSP path.
        memdescFree(pKernelChannel->pErrContextMemDesc);
        memdescDestroy(pKernelChannel->pErrContextMemDesc);
        memdescFree(pKernelChannel->pEccErrContextMemDesc);
        memdescDestroy(pKernelChannel->pEccErrContextMemDesc);
    }
    pKernelChannel->pErrContextMemDesc = NULL;
    pKernelChannel->pEccErrContextMemDesc = NULL;

    // GSP and vGPU support
    if ((IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu)))
    {
        //
        // GSP:
        //
        // Method buffer is allocated by CPU-RM during TSG construct
        // but mapped to invisible BAR2 in GSP during channel construct
        // During Free, first the BAR2 mapping must be unmapped in GSP
        // and then freeing of method buffer should be done on CPU.
        // This RPC call is especially required for the internal channel case
        // where channelDestruct calls free for its TSG
        //
        NV_RM_RPC_FREE(pGpu,
                       hClient,
                       RES_GET_PARENT_HANDLE(pKernelChannel),
                       RES_GET_HANDLE(pKernelChannel),
                       status);
    }

    {
        KernelGraphicsContext *pKernelGraphicsContext;

        // Perform GR ctx cleanup tasks on channel destruction
        if ((kgrctxFromKernelChannel(pKernelChannel, &pKernelGraphicsContext) == NV_OK) &&
            kgrctxIsValid(pGpu, pKernelGraphicsContext, pKernelChannel))
        {
            shrkgrctxDetach(pGpu, kgrctxGetShared(pGpu, pKernelGraphicsContext), pKernelGraphicsContext, pKernelChannel);
        }
    }

    _kchannelFreeHalData(pGpu, pKernelChannel);

    NV_ASSERT(pKernelChannel->pKernelChannelGroupApi != NULL);

    pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;

    NV_ASSERT(pKernelChannelGroup != NULL);

    // remove channel from the group
    kchangrpRemoveChannel(pGpu, pKernelChannelGroup, pKernelChannel);

    // Free the Ctx Buf pool
    if (pKernelChannelGroup->pChannelBufPool != NULL)
    {
        ctxBufPoolRelease(pKernelChannelGroup->pChannelBufPool);
    }

    // Free the channel group, if we alloced it
    if (pKernelChannelGroup->bAllocatedByRm)
    {
        pRmApi->Free(pRmApi, hClient,
                     RES_GET_HANDLE(pKernelChannel->pKernelChannelGroupApi));
        pKernelChannelGroup = NULL;
        pKernelChannel->pKernelChannelGroupApi = NULL;
    }

    kchannelFreeHwID_HAL(pGpu, pKernelChannel);
    kchannelFreeMmuExceptionInfo(pKernelChannel);

    NV_ASSERT(pKernelChannel->refCount == 1);

    if (bCheckKeyRotation)
    {
        //
        // If key rotation is pending on this key because the channel being freed hasn't reported idle yet then,
        // we wait until this channel's SW state is cleared out before triggerring key rotation
        // so that the key rotation code doesn't try to notify this channel or check its idle state.
        //
        NV_ASSERT_OK(confComputeCheckAndPerformKeyRotation(pGpu, pConfCompute, h2dKey, d2hKey));
    }
}

NV_STATUS
kchannelMap_IMPL
(
    KernelChannel     *pKernelChannel,
    CALL_CONTEXT      *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping      *pCpuMapping
)
{
    OBJGPU *pGpu;
    NV_STATUS rmStatus;
    RsClient *pRsClient = pCallContext->pClient;
    RmClient *pRmClient = dynamicCast(pRsClient, RmClient);
    GpuResource *pGpuResource;

    NV_ASSERT_OR_RETURN(!pKernelChannel->bClientAllocatedUserD, NV_ERR_INVALID_REQUEST);

    rmStatus = gpuresGetByDeviceOrSubdeviceHandle(pRsClient,
                                                  pCpuMapping->pContextRef->hResource,
                                                  &pGpuResource);
    if (rmStatus != NV_OK)
        return rmStatus;

    pGpu = GPU_RES_GET_GPU(pGpuResource);
    GPU_RES_SET_THREAD_BC_STATE(pGpuResource);

    // If the flags are fifo default then offset/length passed in
    if (DRF_VAL(OS33, _FLAGS, _FIFO_MAPPING, pCpuMapping->flags) == NVOS33_FLAGS_FIFO_MAPPING_DEFAULT)
    {
        // Validate the offset and limit passed in.
        if (pCpuMapping->offset >= pKernelChannel->userdLength)
            return NV_ERR_INVALID_BASE;
        if (pCpuMapping->length == 0)
            return NV_ERR_INVALID_LIMIT;
        if (pCpuMapping->offset + pCpuMapping->length > pKernelChannel->userdLength)
            return NV_ERR_INVALID_LIMIT;
    }
    else
    {
        pCpuMapping->offset = 0x0;
        pCpuMapping->length = pKernelChannel->userdLength;
    }

    rmStatus = kchannelMapUserD(pGpu, pKernelChannel,
                                rmclientGetCachedPrivilege(pRmClient),
                                pCpuMapping->offset,
                                pCpuMapping->pPrivate->protect,
                                &pCpuMapping->pLinearAddress,
                                &(pCpuMapping->pPrivate->pPriv));

    if (rmStatus != NV_OK)
        return rmStatus;

    // Save off the mapping
    _kchannelUpdateFifoMapping(pKernelChannel,
                               pGpu,
                               (pRsClient->type == CLIENT_TYPE_KERNEL),
                               pCpuMapping->pLinearAddress,
                               pCpuMapping->pPrivate->pPriv,
                               pCpuMapping->length,
                               pCpuMapping->flags,
                               pCpuMapping->pContextRef->hResource,
                               pCpuMapping);

    return NV_OK;
}

NV_STATUS
kchannelUnmap_IMPL
(
    KernelChannel *pKernelChannel,
    CALL_CONTEXT  *pCallContext,
    RsCpuMapping  *pCpuMapping
)
{
    OBJGPU   *pGpu;
    RsClient *pRsClient = pCallContext->pClient;
    RmClient *pRmClient = dynamicCast(pRsClient, RmClient);

    if (pKernelChannel->bClientAllocatedUserD)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }

    pGpu = pCpuMapping->pPrivate->pGpu;

    kchannelUnmapUserD(pGpu,
                       pKernelChannel,
                       rmclientGetCachedPrivilege(pRmClient),
                       &pCpuMapping->pLinearAddress,
                       &pCpuMapping->pPrivate->pPriv);

    return NV_OK;
}

NV_STATUS
kchannelGetMapAddrSpace_IMPL
(
    KernelChannel    *pKernelChannel,
    CALL_CONTEXT     *pCallContext,
    NvU32             mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 userdAperture;
    NvU32 userdAttribute;

    NV_ASSERT_OK_OR_RETURN(kfifoGetUserdLocation_HAL(pKernelFifo,
                                                     &userdAperture,
                                                     &userdAttribute));
    if (pAddrSpace)
        *pAddrSpace = userdAperture;

    return NV_OK;
}

NV_STATUS
kchannelGetMemInterMapParams_IMPL
(
    KernelChannel              *pKernelChannel,
    RMRES_MEM_INTER_MAP_PARAMS *pParams
)
{
    OBJGPU            *pGpu = pParams->pGpu;
    KernelFifo        *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    MEMORY_DESCRIPTOR *pSrcMemDesc = NULL;
    NV_STATUS          status;

    if (pParams->bSubdeviceHandleProvided)
    {
        NV_PRINTF(LEVEL_ERROR, "Unicast DMA mappings of USERD not supported.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!kfifoIsUserdMapDmaSupported(pKernelFifo))
        return NV_ERR_INVALID_OBJECT_HANDLE;

    status = _kchannelGetUserMemDesc(pGpu, pKernelChannel, &pSrcMemDesc);
    if (status != NV_OK)
        return status;

    pParams->pSrcMemDesc = pSrcMemDesc;
    pParams->pSrcGpu = pSrcMemDesc->pGpu;

    return NV_OK;
}

NV_STATUS
kchannelCheckMemInterUnmap_IMPL
(
    KernelChannel *pKernelChannel,
    NvBool         bSubdeviceHandleProvided
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (bSubdeviceHandleProvided)
    {
        NV_PRINTF(LEVEL_ERROR, "Unicast DMA mappings of channels not supported.\n");
        return NV_ERR_NOT_SUPPORTED;
    }


    if (!kfifoIsUserdMapDmaSupported(pKernelFifo))
        return NV_ERR_INVALID_OBJECT_HANDLE;

    return NV_OK;
}

/**
 * @brief Creates an iterator to iterate all channels in a given scope.
 *
 * Iterates over all channels under a given scope.  For a device it will loop
 * through all channels that are descendants of the device (including children
 * of channel groups).  For a channel group it will only iterate over the
 * channels within that group.  Ordering is ensured for channel group.
 * All channels within a channel group will be iterated together before moving to
 * another channel group or channel.
 *
 * @param[in]  pClient
 * @param[in]  pScopeRef The resource that defines the scope of iteration
 */
RS_ORDERED_ITERATOR
kchannelGetIter
(
    RsClient      *pClient,
    RsResourceRef *pScopeRef
)
{
    return clientRefOrderedIter(pClient, pScopeRef, classId(KernelChannel), NV_TRUE);
}

/**
 * @brief Given a client, parent, and KernelChannel handle retrieves the
 * KernelChannel object
 *
 * @param[in]  hClient
 * @param[in]  hParent              Device or Channel Group parent
 * @param[in]  hKernelChannel
 * @param[out] ppKernelChannel      Valid iff NV_OK is returned.
 *
 * @return  NV_OK if successful, appropriate error otherwise
 */
NV_STATUS
CliGetKernelChannelWithDevice
(
    RsClient       *pClient,
    NvHandle        hParent,
    NvHandle        hKernelChannel,
    KernelChannel **ppKernelChannel
)
{
    RsResourceRef *pParentRef;
    RsResourceRef *pResourceRef;
    KernelChannel *pKernelChannel;

    if (ppKernelChannel == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    *ppKernelChannel = NULL;

    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(pClient, hKernelChannel, &pResourceRef));

    pKernelChannel = dynamicCast(pResourceRef->pResource, KernelChannel);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pKernelChannel != NULL, NV_ERR_OBJECT_NOT_FOUND);

    pParentRef = pResourceRef->pParentRef;
    NV_CHECK_OR_RETURN(LEVEL_INFO, pParentRef != NULL, NV_ERR_OBJECT_NOT_FOUND);

    //
    // Check that the parent matches requested handle.  Parent handle can be a
    // device or a ChannelGroup.  The first case can match either, the second
    // matches a Device when the parent is a ChannelGroup.
    //
    NV_CHECK_OR_RETURN(LEVEL_INFO, (pParentRef->hResource == hParent) ||
                     (RES_GET_HANDLE(GPU_RES_GET_DEVICE(pKernelChannel)) == hParent),
                         NV_ERR_OBJECT_NOT_FOUND);

    *ppKernelChannel = pKernelChannel;
    return NV_OK;
} // end of CliGetKernelChannelWithDevice()


/**
 * @brief Given a classNum this routine returns various sdk specific values for
 * that class.
 *
 * @param[in]   classNum
 * @param[out]  pClassInfo
 */
void
CliGetChannelClassInfo
(
    NvU32 classNum,
    CLI_CHANNEL_CLASS_INFO *pClassInfo
)
{
    switch (classNum)
    {
        case GF100_CHANNEL_GPFIFO:
        {
            pClassInfo->notifiersMaxCount  = NV906F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NV906F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case KEPLER_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVA06F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVA06F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case KEPLER_CHANNEL_GPFIFO_B:
        {
            pClassInfo->notifiersMaxCount  = NVA16F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVA16F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case MAXWELL_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVB06F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVB06F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case PASCAL_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVC06F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVC06F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case VOLTA_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVC36F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVC36F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case TURING_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVC46F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVC46F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case AMPERE_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVC56F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVC56F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }
        case HOPPER_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVC86F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVC86F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }

        case BLACKWELL_CHANNEL_GPFIFO_A:
        {
            pClassInfo->notifiersMaxCount  = NVC96F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVC96F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }

        case BLACKWELL_CHANNEL_GPFIFO_B:
        {
            pClassInfo->notifiersMaxCount  = NVCA6F_NOTIFIERS_MAXCOUNT;
            pClassInfo->rcNotifierIndex    = NVCA6F_NOTIFIERS_RC;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_GPFIFO;
            break;
        }

        //
        // Does not make sense. Call with the class type from the client not the
        // internal type
        //
        case PHYSICAL_CHANNEL_GPFIFO:
            NV_PRINTF(LEVEL_ERROR,
                      "Invalid class for CliGetChannelClassInfo\n");

        default:
        {
            pClassInfo->notifiersMaxCount  = 0;
            pClassInfo->rcNotifierIndex    = 0;
            pClassInfo->classType          = CHANNEL_CLASS_TYPE_DMA;
            break;
        }
    }
}


/**
 * @brief Returns the next KernelChannel from the iterator.
 *
 * Iterates over runlist IDs and ChIDs and returns the next KernelChannel found
 * on the heap, if any.
 *
 * (error guaranteed if pointer is NULL; non-NULL pointer guaranteed if NV_OK)
 *
 * @param[in] pGpu
 * @param[in] pIt                   the channel iterator
 * @param[out] ppKernelChannel      returns a KernelChannel *
 *
 * @return NV_OK if the returned pointer is valid or error
 */
NV_STATUS kchannelGetNextKernelChannel
(
    OBJGPU              *pGpu,
    CHANNEL_ITERATOR    *pIt,
    KernelChannel      **ppKernelChannel
)
{
    KernelChannel *pKernelChannel;
    KernelFifo    *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (ppKernelChannel == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    *ppKernelChannel = NULL;

    while (pIt->runlistId < pIt->numRunlists)
    {
        CHID_MGR *pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, pIt->runlistId);

        if (pChidMgr == NULL)
        {
            pIt->runlistId++;
            continue;
        }

        pIt->numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);
        while (pIt->physicalChannelID < pIt->numChannels)
        {
            pKernelChannel = kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo,
                pChidMgr, pIt->physicalChannelID);
            pIt->physicalChannelID++;

            //
            // This iterator can be used during an interrupt, when a KernelChannel may
            // be in the process of being destroyed. Don't return it if so.
            //
            if (pKernelChannel == NULL)
                continue;
            if (!kchannelIsValid_HAL(pKernelChannel))
                continue;

            *ppKernelChannel = pKernelChannel;
            return NV_OK;
        }

        pIt->runlistId++;
        // Reset channel index to 0 for next runlist
        pIt->physicalChannelID = 0;
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/**
 * @brief Finds the corresponding KernelChannel given client object and channel handle
 *
 * Looks in client object store for the channel handle.  Scales with total
 * number of registered objects in the client, not just the number of channels.
 *
 * @param[in]  pClient
 * @param[in]  hKernelChannel a KernelChannel Channel handle
 * @param[out] ppKernelChannel
 *
 * @return NV_STATUS
 */
NV_STATUS
CliGetKernelChannel
(
    RsClient       *pClient,
    NvHandle        hKernelChannel,
    KernelChannel **ppKernelChannel
)
{
    NV_STATUS      status;
    RsResourceRef *pResourceRef;

    *ppKernelChannel = NULL;

    status = clientGetResourceRef(pClient, hKernelChannel, &pResourceRef);
    if (status != NV_OK)
    {
        return status;
    }

    *ppKernelChannel = dynamicCast(pResourceRef->pResource, KernelChannel);
    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       *ppKernelChannel != NULL,
                       NV_ERR_INVALID_CHANNEL);
    return NV_OK;
}

/*!
 * @brief Notify client that channel is stopped.
 *
 * @param[in] pKernelChannnel
 */
NV_STATUS
kchannelNotifyRc_IMPL
(
    KernelChannel *pKernelChannel
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    RM_ENGINE_TYPE rmEngineType = RM_ENGINE_TYPE_NULL;
    NV_STATUS rmStatus = NV_OK;

    if (IS_GFID_VF(kchannelGetGfid(pKernelChannel)))
    {
        NV_PRINTF(LEVEL_INFO, "Notification for channel 0x%x stop is already performed on guest-RM\n",
                  kchannelGetDebugTag(pKernelChannel));
        return NV_OK;
    }

    if (pKernelChannel->hErrorContext == NV01_NULL_OBJECT &&
        pKernelChannel->hEccErrorContext == NV01_NULL_OBJECT)
    {
        NV_PRINTF(LEVEL_WARNING, "Channel 0x%x has no notifier set\n",
                  kchannelGetDebugTag(pKernelChannel));
        return NV_OK;
    }

    if (RM_ENGINE_TYPE_IS_VALID(kchannelGetEngineType(pKernelChannel)))
    {
        rmEngineType = kchannelGetEngineType(pKernelChannel);
    }
    rmStatus = krcErrorSetNotifier(pGpu, GPU_GET_KERNEL_RC(pGpu),
                                   pKernelChannel,
                                   ROBUST_CHANNEL_PREEMPTIVE_REMOVAL,
                                   rmEngineType,
                                   RC_NOTIFIER_SCOPE_CHANNEL);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to set error notifier for channel 0x%x with error 0x%x.\n",
            kchannelGetDebugTag(pKernelChannel), rmStatus);
    }
    return rmStatus;
}

/**
 * @brief Sends event corresponding to index to notify clients
 *
 * @param[in] pKernelChannel
 * @param[in] notifyIndex
 * @param[in] pNotifyParams
 * @parms[in] notifyParamsSize
 */
void kchannelNotifyEvent_IMPL
(
    KernelChannel *pKernelChannel,
    NvU32          notifyIndex,
    NvU32          info32,
    NvU16          info16,
    void          *pNotifyParams,
    NvU32          notifyParamsSize
)
{
    OBJGPU                 *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    EVENTNOTIFICATION      *pEventNotification;
    CLI_CHANNEL_CLASS_INFO  classInfo;

    CliGetChannelClassInfo(RES_GET_EXT_CLASS_ID(pKernelChannel), &classInfo);

    // validate notifyIndex
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, notifyIndex < classInfo.notifiersMaxCount);

    // handle event if client wants it
    pEventNotification = inotifyGetNotificationList(staticCast(pKernelChannel, INotifier));
    if (pEventNotification != NULL)
    {
        NV_PRINTF(LEVEL_INFO, "Posting event on channel = 0x%x with info16 = 0x%x\n",
           kchannelGetDebugTag(pKernelChannel), (NvU32)info16);
        // ping any events on the list of type notifyIndex
        osEventNotificationWithInfo(pGpu, pEventNotification, notifyIndex, info32, info16,
                                    pNotifyParams, notifyParamsSize);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "No event on channel = 0x%x\n", kchannelGetDebugTag(pKernelChannel));
    }

    return;
}

/**
 * @brief Writes notifier memory at given index with given info
 *
 * @param[in] pKernelChannel
 * @param[in] notifyIndex
 * @param[in] info32
 * @param[in] info16
 * @parms[in] notifierStatus
 */
NV_STATUS kchannelUpdateNotifierMem_IMPL
(
    KernelChannel *pKernelChannel,
    NvU32 notifyIndex,
    NvU32 info32,
    NvU16 info16,
    NvU32 notifierStatus
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    MEMORY_DESCRIPTOR *pNotifierMemDesc = pKernelChannel->pErrContextMemDesc;
    NV_ADDRESS_SPACE addressSpace;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64 time;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    TRANSFER_SURFACE surf = {0};
    NvNotification *pNotifier = NULL;
    NvBool bMemEndTransfer = NV_FALSE;

    if (pNotifierMemDesc == NULL)
        return NV_OK;

    addressSpace = memdescGetAddressSpace(pNotifierMemDesc);
    if (RMCFG_FEATURE_PLATFORM_GSP)
        NV_ASSERT_OR_RETURN(addressSpace == ADDR_FBMEM, NV_ERR_INVALID_STATE);

    //
    // If clients did not allocate enough memory for the doorbell
    // notifier, return NV_OK so as not to regress older clients
    //
    NV_CHECK_OR_RETURN(LEVEL_INFO, memdescGetSize(pNotifierMemDesc) >= (notifyIndex + 1) * sizeof(NvNotification), NV_OK);

    //
    // we rely on persistent mapping for key rotation notifier
    // since this may be called in top half and mappings are not allowed
    // in contexts that can't sleep on KVM or similar HCC systems.
    //
    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    if ((pConfCompute != NULL) &&
        (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED)) &&
        (notifyIndex == NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS))
    {
        pNotifier = _kchannelGetKeyRotationNotifier(pKernelChannel);
        NV_ASSERT_OR_RETURN(pNotifier != NULL, NV_ERR_INVALID_STATE);
        bMemEndTransfer = NV_FALSE;
    }
    else
    {
        pNotifier = (NvNotification *)memdescGetKernelMapping(pNotifierMemDesc);
        if (pNotifier == NULL)
        {
            surf.pMemDesc = pNotifierMemDesc;
            surf.offset = notifyIndex * sizeof(NvNotification);

            pNotifier =
                (NvNotification *) memmgrMemBeginTransfer(pMemoryManager, &surf,
                                                          sizeof(NvNotification),
                                                          TRANSFER_FLAGS_SHADOW_ALLOC);
            NV_ASSERT_OR_RETURN(pNotifier != NULL, NV_ERR_INVALID_STATE);
            bMemEndTransfer = NV_TRUE;
        }
        else
        {
            //
            // If a CPU pointer has been passed by caller ensure that the notifier
            // is in sysmem or in case it in vidmem, BAR access to the same is not
            // blocked (for HCC)
            //
            NV_ASSERT_OR_RETURN(
                memdescGetAddressSpace(pNotifierMemDesc) == ADDR_SYSMEM ||
                !kbusIsBarAccessBlocked(pKernelBus), NV_ERR_INVALID_ARGUMENT);
            pNotifier = &pNotifier[notifyIndex];
        }
    }

    tmrGetCurrentTime(pTmr, &time);

    notifyFillNvNotification(pGpu, pNotifier, info32, info16,
                             notifierStatus, NV_TRUE, time);

    if (bMemEndTransfer)
    {
        memmgrMemEndTransfer(pMemoryManager, &surf, sizeof(NvNotification), 0);
    }
    return NV_OK;
}

/*!
 * @brief Stop channel and notify client
 *
 * @param[in] pKernelChannnel
 * @param[in] pStopChannelParams
 */
NV_STATUS
kchannelCtrlCmdStopChannel_IMPL
(
    KernelChannel *pKernelChannel,
    NVA06F_CTRL_STOP_CHANNEL_PARAMS *pStopChannelParams
)
{
    NV_STATUS     rmStatus      = NV_OK;
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pKernelChannel);
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          RES_GET_HANDLE(pKernelChannel),
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          rmStatus);
        if (rmStatus != NV_OK)
            return rmStatus;
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
            kchannelFwdToInternalCtrl_HAL(pGpu,
                                          pKernelChannel,
                                          NVA06F_CTRL_CMD_INTERNAL_STOP_CHANNEL,
                                          pRmCtrlParams));
    }

    NV_ASSERT_OK_OR_RETURN(kchannelNotifyRc_HAL(pKernelChannel));

    return NV_OK;
}

/*!
 * @brief Helper to get type and memdesc of a channel notifier (memory/ctxdma)
 */
NV_STATUS
kchannelGetNotifierInfo
(
    OBJGPU             *pGpu,
    Device             *pDevice,
    NvHandle            hErrorContext,
    MEMORY_DESCRIPTOR **ppMemDesc,
    ErrorNotifierType  *pNotifierType,
    NvU64              *pOffset
)
{
    RsClient   *pRsClient   = RES_GET_CLIENT(pDevice);
    ContextDma *pContextDma = NULL;
    Memory     *pMemory     = NULL;

    NV_ASSERT_OR_RETURN(ppMemDesc != NULL, NV_ERR_INVALID_PARAMETER);
    NV_ASSERT_OR_RETURN(pNotifierType != NULL, NV_ERR_INVALID_PARAMETER);

    *ppMemDesc = NULL;
    *pNotifierType = ERROR_NOTIFIER_TYPE_UNKNOWN;
    *pOffset = 0;

    if (hErrorContext == NV01_NULL_OBJECT)
    {
        *pNotifierType = ERROR_NOTIFIER_TYPE_NONE;
        return NV_OK;
    }

    // Checking by device is not required because the device may be duplicated for uGPU steering
    if (memGetByHandle(pRsClient, hErrorContext, &pMemory) ==
        NV_OK)
    {
        if (memdescGetAddressSpace(pMemory->pMemDesc) == ADDR_VIRTUAL)
        {
            //
            // GPUVA case: Get the underlying DMA mapping in this case. In GSP
            // client mode + SLI, GSP won't be able to write to notifiers on
            // other GPUs.
            //
            NvU64 offset;
            NvU32 subdeviceInstance;
            NvU64 notifyGpuVA = memdescGetPhysAddr(pMemory->pMemDesc,
                                                   AT_GPU_VA, 0);
            CLI_DMA_MAPPING_INFO *pDmaMappingInfo;
            NvBool bFound;

            bFound = CliGetDmaMappingInfo(
                pRsClient,
                RES_GET_HANDLE(pDevice),
                RES_GET_HANDLE(pMemory),
                notifyGpuVA,
                gpumgrGetDeviceGpuMask(pGpu->deviceInstance),
                &pDmaMappingInfo);

            if (!bFound)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Cannot find DMA mapping for GPU_VA notifier\n");
                return NV_ERR_INVALID_STATE;
            }

            offset = notifyGpuVA - pDmaMappingInfo->DmaOffset;
            if (offset + sizeof(NOTIFICATION) > pDmaMappingInfo->pMemDesc->Size)
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Notifier does not fit within DMA mapping for GPU_VA\n");
                return NV_ERR_INVALID_STATE;
            }

            subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(
                gpumgrGetParentGPU(pGpu));
            SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)
            if (IsSLIEnabled(pGpu) && IS_GSP_CLIENT(pGpu))
            {
                NV_PRINTF(LEVEL_ERROR, "GSP does not support SLI\n");
                return NV_ERR_NOT_SUPPORTED;
            }
            SLI_LOOP_END

            if (!pDmaMappingInfo->KernelVAddr[subdeviceInstance])
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Kernel VA addr mapping not present for notifier\n");
                return NV_ERR_INVALID_STATE;
            }
            *ppMemDesc = pDmaMappingInfo->pMemDesc;
            // The notifier format here is struct NOTIFICATION, same as ctxdma
            *pNotifierType = ERROR_NOTIFIER_TYPE_CTXDMA;
            *pOffset = offset;
        }
        else
        {
            *ppMemDesc = pMemory->pMemDesc;
            *pNotifierType = ERROR_NOTIFIER_TYPE_MEMORY;
        }
        return NV_OK;
    }

    if (ctxdmaGetByHandle(pRsClient, hErrorContext, &pContextDma) == NV_OK)
    {
        *ppMemDesc = pContextDma->pMemDesc;
        *pNotifierType = ERROR_NOTIFIER_TYPE_CTXDMA;
        return NV_OK;
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief  Check if the client that owns this channel is in user mode.
 *
 * This replaces using call context for privilege checking,
 * and is callable from both CPU and GSP.
 *
 * @param[in] pGpu
 * @param[in] pKernelChannel
 *
 * @returns NV_TRUE if owned by user mode or NV_FALSE.
 */
NvBool
kchannelCheckIsUserMode_IMPL
(
    KernelChannel *pKernelChannel
)
{
    return (pKernelChannel->privilegeLevel == NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_USER) ||
           (pKernelChannel->privilegeLevel == NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_ADMIN);
}

/*!
 * @brief  Check if the client that owns this channel is kernel.
 *
 * This replaces using call context for privilege checking,
 * and is callable from both CPU and GSP.
 *
 * @param[in] pGpu
 * @param[in] pKernelChannel
 *
 * @returns NV_TRUE if owned by kernel or NV_FALSE.
 */
NvBool
kchannelCheckIsKernel_IMPL
(
    KernelChannel *pKernelChannel
)
{
    return pKernelChannel->privilegeLevel == NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_KERNEL;
}

/*!
 * @brief  Check if the client that owns this channel is admin.
 *
 * This replaces using call context for admin privilege checking,
 * but is callable from both CPU and GSP.
 *
 * @param[in] pGpu
 * @param[in] pKernelChannel
 *
 * @returns NV_TRUE if owned by admin or NV_FALSE.
 */
NvBool
kchannelCheckIsAdmin_IMPL
(
    KernelChannel *pKernelChannel
)
{
    return (pKernelChannel->privilegeLevel == NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_KERNEL) ||
           (pKernelChannel->privilegeLevel == NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_ADMIN);
}


/*!
 * @brief  Check if the channel is bound to its resources.
 *
 * This is to make sure channel went through the UVM registration step before it can be scheduled.
 * This applies only to UVM owned channels.
 *
 * @param[in] pKernelChannel
 * @param[in] pGVAS
 *
 * @returns NV_TRUE if bound.
 */
NvBool
kchannelIsSchedulable_IMPL
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel
)
{
    OBJGVASPACE *pGVAS = NULL;
    NvU32        engineDesc = 0;
    NvU32        gfId;

    gfId = kchannelGetGfid(pKernelChannel);
    if (IS_GFID_VF(gfId))
    {
        NV_PRINTF(LEVEL_INFO, "Check for channel schedulability for channel 0x%x is already performed on guest-RM\n",
                  kchannelGetDebugTag(pKernelChannel));
        return NV_TRUE;
    }

    pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);

    //
    // It should be an error to have allocated and attempt to schedule a
    // channel without having allocated a GVAS. We ignore this check on
    // AMODEL, which has its own dummy AVAS.
    //
    NV_ASSERT_OR_RETURN(pGVAS != NULL || IS_MODS_AMODEL(pGpu), NV_FALSE);

    NV_ASSERT_OR_RETURN(kchannelGetEngine_HAL(pGpu, pKernelChannel, &engineDesc) == NV_OK, NV_FALSE);

    if (pGVAS != NULL && gvaspaceIsExternallyOwned(pGVAS) && IS_GR(engineDesc) && !pKernelChannel->bIsContextBound)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Cannot schedule externally-owned channel with unbound allocations :0x%x!\n",
                  kchannelGetDebugTag(pKernelChannel));
        return NV_FALSE;
    }
    return NV_TRUE;
}

// Alloc pFifoHalData
static NV_STATUS
_kchannelAllocHalData
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    portMemSet(pKernelChannel->pFifoHalData, 0, sizeof(pKernelChannel->pFifoHalData));

    // Alloc 1 page of instmem per GPU instance
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = portMemAllocNonPaged(sizeof(FIFO_INSTANCE_BLOCK));

    NV_ASSERT_OR_ELSE(pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] != NULL,
            SLI_LOOP_GOTO(failed));

    portMemSet(pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)], 0, sizeof(FIFO_INSTANCE_BLOCK));

    SLI_LOOP_END

    return NV_OK;

failed:
    DBG_BREAKPOINT();
    _kchannelFreeHalData(pGpu, pKernelChannel);
    return NV_ERR_NO_MEMORY;
}

// Free memdescs and pFifoHalData, if any
static void
_kchannelFreeHalData
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    // Unmap / delete memdescs
    kchannelDestroyMem_HAL(pGpu, pKernelChannel);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    kchannelDestroyUserdMemDesc(pGpu, pKernelChannel);

    // Free pFifoHalData
    portMemFree(pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)]);
    pKernelChannel->pFifoHalData[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = NULL;
    SLI_LOOP_END
}

// Returns the proper VerifFlags for kchannelAllocMem
static NvU32
_kchannelgetVerifFlags
(
    OBJGPU                                    *pGpu,
    NV_CHANNEL_ALLOC_PARAMS    *pChannelGpfifoParams
)
{
    NvU32 verifFlags = 0;

    return verifFlags;
}

// Allocate and describe instance memory
static NV_STATUS
_kchannelAllocOrDescribeInstMem
(
    KernelChannel  *pKernelChannel,
    NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams
)
{
    OBJGPU                *pGpu        = GPU_RES_GET_GPU(pKernelChannel);
    KernelFifo            *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    KernelChannelGroupApi *pKernelChannelGroupApi = pKernelChannel->pKernelChannelGroupApi;
    KernelChannelGroup    *pKernelChannelGroup    = pKernelChannelGroupApi->pKernelChannelGroup;
    NvU32                  gfid       = pKernelChannelGroup->gfid;
    NV_STATUS              status;
    NvHandle               hClient = RES_GET_CLIENT_HANDLE(pKernelChannel);

    // Alloc pFifoHalData
    NV_ASSERT_OK_OR_RETURN(_kchannelAllocHalData(pGpu, pKernelChannel));

    //
    // GSP RM and host RM on full SRIOV setup will not be aware of the client allocated userd handles,
    // translate the handle on client GSP. GSP RM or host RM on full SRIOV setup will get the translated
    // addresses which it will later memdescribe.
    //
    // However it is still client allocated userd from GSP RM or host RM on full SRIOV setup
    // perspective so set the flag accordingly.
    //
    if (!(RMCFG_FEATURE_PLATFORM_GSP && !pKernelChannel->bGspOwned) &&
        !(IS_GFID_VF(gfid) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        pKernelChannel->bClientAllocatedUserD = NV_FALSE;
        NV_ASSERT_OK_OR_GOTO(status,
                kchannelCreateUserdMemDescBc_HAL(pGpu, pKernelChannel, hClient,
                    pChannelGpfifoParams->hUserdMemory,
                    pChannelGpfifoParams->userdOffset),
                failed);
    }
    else
    {
        pKernelChannel->bClientAllocatedUserD = NV_TRUE;
    }

    // Alloc/describe instmem memdescs depending on platform
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        // On Heavy SRIOV, describe memdescs using RPC
        NV_ASSERT_OK_OR_GOTO(status,
                _kchannelDescribeMemDescsHeavySriov(pGpu, pKernelChannel),
                failed);
    }
    else if ((RMCFG_FEATURE_PLATFORM_GSP && !pKernelChannel->bGspOwned) ||
        (IS_GFID_VF(gfid) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        // On GSPFW or non-heavy SRIOV, describe memdescs from params
        NV_ASSERT_OK_OR_GOTO(status,
                _kchannelDescribeMemDescsFromParams(pGpu, pKernelChannel, pChannelGpfifoParams),
                failed);
    }
    else if (!IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        // On baremetal, GSP client, or SRIOV host, alloc mem
        NV_ASSERT_OK_OR_GOTO(status,
                kchannelAllocMem_HAL(pGpu,
                                     pKernelChannel,
                                     pChannelGpfifoParams->flags,
                                     _kchannelgetVerifFlags(pGpu, pChannelGpfifoParams)),
                failed);
    }

    // Setup USERD
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        PMEMORY_DESCRIPTOR pUserdSubDeviceMemDesc =
                pKernelChannel->pUserdSubDeviceMemDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
        NvBool bFullSriov = IS_VIRTUAL_WITH_SRIOV(pGpu) &&
            !gpuIsWarBug200577889SriovHeavyEnabled(pGpu);

        // Clear Userd if it is in FB for SRIOV environment without BUG 200577889 or if in SYSMEM
        if (pUserdSubDeviceMemDesc != NULL &&
                ((memdescGetAddressSpace(pUserdSubDeviceMemDesc) == ADDR_SYSMEM)
                || ((memdescGetAddressSpace(pUserdSubDeviceMemDesc) == ADDR_FBMEM) && bFullSriov)))
        {
            kfifoSetupUserD_HAL(pGpu, pKernelFifo, pUserdSubDeviceMemDesc);
        }
    }
    return NV_OK;

failed:
    _kchannelFreeHalData(pGpu, pKernelChannel);
    return status;
}

/**
 * @brief Create and describe channel instance memory ramfc and userd memdescs
 *        Done using info in pChanGpfifoParams
 *
 * @param pGpu                  : OBJGPU pointer
 * @param pKernelChannel        : KernelChannel pointer
 * @param pChanGpfifoParams     : Pointer to channel allocation params
 */
static NV_STATUS
_kchannelDescribeMemDescsFromParams
(
    OBJGPU                                 *pGpu,
    KernelChannel                          *pKernelChannel,
    NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams
)
{
    NV_STATUS               status         = NV_OK;
    FIFO_INSTANCE_BLOCK    *pInstanceBlock = NULL;
    NvU32                   subDevInst;
    NvU32                   gfid           = GPU_GFID_PF;
    NvU32                   runqueue;
    KernelChannelGroupApi *pKernelChannelGroupApi =
        pKernelChannel->pKernelChannelGroupApi;

    NV_ASSERT_OR_RETURN((pKernelChannelGroupApi != NULL), NV_ERR_INVALID_STATE);
    gfid = pKernelChannelGroupApi->pKernelChannelGroup->gfid;

    NV_ASSERT_OR_RETURN((RMCFG_FEATURE_PLATFORM_GSP && !pKernelChannel->bGspOwned) ||
                        (IS_GFID_VF(gfid) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu)),
                        NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN((pChannelGpfifoParams != NULL), NV_ERR_INVALID_ARGUMENT);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    subDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    pInstanceBlock = (FIFO_INSTANCE_BLOCK*) pKernelChannel->pFifoHalData[subDevInst];

    // Create memory descriptor for the instance memory
    status = memdescCreate(&pInstanceBlock->pInstanceBlockDesc, pGpu,
                           pChannelGpfifoParams->instanceMem.size, 1 , NV_TRUE,
                           pChannelGpfifoParams->instanceMem.addressSpace,
                           pChannelGpfifoParams->instanceMem.cacheAttrib,
                           MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to allocate instance memory descriptor!\n");
        SLI_LOOP_RETURN(status);
    }

    memdescDescribe(pInstanceBlock->pInstanceBlockDesc, pChannelGpfifoParams->instanceMem.addressSpace,
                    pChannelGpfifoParams->instanceMem.base, pChannelGpfifoParams->instanceMem.size);


    // Create memory descriptor for the ramfc
    status = memdescCreate(&pInstanceBlock->pRamfcDesc, pGpu,
                           pChannelGpfifoParams->ramfcMem.size, 1 , NV_TRUE,
                           pChannelGpfifoParams->ramfcMem.addressSpace,
                           pChannelGpfifoParams->ramfcMem.cacheAttrib,
                           MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to allocate instance memory descriptor!\n");
        SLI_LOOP_RETURN(status);
    }

    memdescDescribe(pInstanceBlock->pRamfcDesc, pChannelGpfifoParams->ramfcMem.addressSpace,
                    pChannelGpfifoParams->ramfcMem.base, pChannelGpfifoParams->ramfcMem.size);

    // Create userd memory descriptor
    status = memdescCreate(&pKernelChannel->pUserdSubDeviceMemDesc[subDevInst], pGpu,
                           pChannelGpfifoParams->userdMem.size, 1 , NV_TRUE,
                           pChannelGpfifoParams->userdMem.addressSpace,
                           pChannelGpfifoParams->userdMem.cacheAttrib,
                           MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to allocate instance memory descriptor!\n");
        SLI_LOOP_RETURN(status);
    }

    memdescDescribe(pKernelChannel->pUserdSubDeviceMemDesc[subDevInst],
                    pChannelGpfifoParams->userdMem.addressSpace,
                    pChannelGpfifoParams->userdMem.base, pChannelGpfifoParams->userdMem.size);

    if (IS_GFID_VF(gfid) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        /*
         * For full SRIOV, guest RM allocates and sends istance, ramfc and userd memory.
         * Set MEMDESC_FLAGS_GUEST_ALLOCATED flag in memory descriptor
         */
        memdescSetFlag(pInstanceBlock->pInstanceBlockDesc, MEMDESC_FLAGS_GUEST_ALLOCATED, NV_TRUE);
        memdescSetFlag(pInstanceBlock->pRamfcDesc, MEMDESC_FLAGS_GUEST_ALLOCATED, NV_TRUE);
        memdescSetFlag(pKernelChannel->pUserdSubDeviceMemDesc[subDevInst], MEMDESC_FLAGS_GUEST_ALLOCATED, NV_TRUE);
    }

    // Create method buffer memory descriptor
    runqueue = DRF_VAL(OS04, _FLAGS, _GROUP_CHANNEL_RUNQUEUE, pChannelGpfifoParams->flags);
    if (IS_GFID_VF(gfid) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        pKernelChannelGroupApi->pKernelChannelGroup->pMthdBuffers[runqueue]
            .bar2Addr = pChannelGpfifoParams->mthdbufMem.base;
    }
    else if (pKernelChannelGroupApi->pKernelChannelGroup
                 ->pMthdBuffers[runqueue].pMemDesc == NULL)
    {
        NV_ASSERT(pChannelGpfifoParams->mthdbufMem.size > 0);
        NV_ASSERT(pChannelGpfifoParams->mthdbufMem.base != 0);
        status = memdescCreate(&pKernelChannelGroupApi->pKernelChannelGroup
                                    ->pMthdBuffers[runqueue].pMemDesc,
                               pGpu,
                               pChannelGpfifoParams->mthdbufMem.size,
                               1,
                               NV_TRUE,
                               pChannelGpfifoParams->mthdbufMem.addressSpace,
                               pChannelGpfifoParams->mthdbufMem.cacheAttrib,
                               MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to allocate instance memory descriptor!\n");
            SLI_LOOP_RETURN(status);
        }
        memdescDescribe(pKernelChannelGroupApi->pKernelChannelGroup
                            ->pMthdBuffers[runqueue].pMemDesc,
                        pChannelGpfifoParams->mthdbufMem.addressSpace,
                        pChannelGpfifoParams->mthdbufMem.base,
                        pChannelGpfifoParams->mthdbufMem.size);
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
              (pKernelChannel->pUserdSubDeviceMemDesc[subDevInst] == NULL) ? 0x0LL :
              memdescGetPhysAddr(pKernelChannel->pUserdSubDeviceMemDesc[subDevInst], AT_GPU, 0LL), subDevInst);

    SLI_LOOP_END

    return status;
}

/**
 * @brief Create and describe channel instance memory ramfc and userd memdescs
 *        Done using RPC for Heavy SRIOV guest
 *
 * @param pGpu                  : OBJGPU pointer
 * @param pKernelChannel        : KernelChannel pointer
 */
static NV_STATUS
_kchannelDescribeMemDescsHeavySriov
(
    OBJGPU               *pGpu,
    KernelChannel        *pKernelChannel
)
{
    NV_STATUS               status         = NV_OK;
    FIFO_INSTANCE_BLOCK    *pInstanceBlock = NULL;
    NvU32                   subDevInst;
    Subdevice              *pSubDevice;
    NvHandle                hSubDevice     = 0;
    NvU32                   apert          = ADDR_UNKNOWN;
    NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS memInfoParams;
    Device                 *pDevice = GPU_RES_GET_DEVICE(pKernelChannel);

    NV_ASSERT_OR_RETURN(IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu),
            NV_ERR_INVALID_STATE);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    subDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    pInstanceBlock = (FIFO_INSTANCE_BLOCK*) pKernelChannel->pFifoHalData[subDevInst];

    //
    // In SRIOV enabled systems, MMU fault interrupts for guest contexts are received and handled in guests.
    // Inorder to correctly find the faulting channel, faulting instance address has be compared with list of allocated channels.
    // But since contexts are currently allocated in host during channelConstruct, we need
    // context info from host and save it locally for the above channel lookup to pass. This piece of code uses GET_CHANNEL_MEM_INFO
    // to fetch the info and update pFifoHalData with the relevant details.
    //

    portMemSet(&memInfoParams, 0, sizeof(NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS));
    memInfoParams.hChannel = RES_GET_HANDLE(pKernelChannel);

    status = subdeviceGetByInstance(RES_GET_CLIENT(pKernelChannel),
                                    RES_GET_HANDLE(pDevice),
                                    subDevInst,
                                    &pSubDevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to get subdevice object.\n");
        DBG_BREAKPOINT();
        SLI_LOOP_RETURN(status);
    }

    GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

    hSubDevice = RES_GET_HANDLE(pSubDevice);

    NV_RM_RPC_CONTROL(pGpu,
                      RES_GET_CLIENT_HANDLE(pKernelChannel),
                      hSubDevice,
                      NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO,
                      &memInfoParams,
                      sizeof(NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS),
                      status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RM Control call to fetch channel meminfo failed, hKernelChannel 0x%x\n",
                  RES_GET_HANDLE(pKernelChannel));
        DBG_BREAKPOINT();
        SLI_LOOP_RETURN(status);
    }

    // Find the aperture
    if (memInfoParams.chMemInfo.inst.aperture == NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_VIDMEM)
    {
        apert = ADDR_FBMEM;
    }
    else if ((memInfoParams.chMemInfo.inst.aperture == NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_COH) ||
             (memInfoParams.chMemInfo.inst.aperture == NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_NCOH))
    {
        apert = ADDR_SYSMEM;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unknown aperture, hClient 0x%x, hKernelChannel 0x%x\n",
                  RES_GET_CLIENT_HANDLE(pKernelChannel),
                  RES_GET_HANDLE(pKernelChannel));
        status = NV_ERR_INVALID_ARGUMENT;
        DBG_BREAKPOINT();
        SLI_LOOP_RETURN(status);
    }

    status = memdescCreate(&pInstanceBlock->pInstanceBlockDesc, pGpu,
                           memInfoParams.chMemInfo.inst.size, 1 , NV_TRUE,
                           apert, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to allocate instance memory descriptor!\n");
        SLI_LOOP_RETURN(status);
    }

    memdescDescribe(pInstanceBlock->pInstanceBlockDesc, apert, memInfoParams.chMemInfo.inst.base, memInfoParams.chMemInfo.inst.size);

    NV_PRINTF(LEVEL_INFO,
              "hChannel 0x%x hClient 0x%x, Class ID 0x%x "
              "Instance Block @ 0x%llx (%s %x) "
              "USERD @ 0x%llx "
              "for subdevice %d\n",
              RES_GET_HANDLE(pKernelChannel), RES_GET_CLIENT_HANDLE(pKernelChannel), RES_GET_EXT_CLASS_ID(pKernelChannel),
              memdescGetPhysAddr(pInstanceBlock->pInstanceBlockDesc, AT_GPU, 0),
              memdescGetApertureString(memdescGetAddressSpace(pInstanceBlock->pInstanceBlockDesc)),
              (NvU32)(memdescGetAddressSpace(pInstanceBlock->pInstanceBlockDesc)),
              (pKernelChannel->pUserdSubDeviceMemDesc[subDevInst] == NULL) ? 0x0LL :
              memdescGetPhysAddr(pKernelChannel->pUserdSubDeviceMemDesc[subDevInst], AT_GPU, 0LL), subDevInst);

    SLI_LOOP_END

    return status;
}

static NV_STATUS
_kchannelSendChannelAllocRpc
(
    KernelChannel *pKernelChannel,
    NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams,
    KernelChannelGroup *pKernelChannelGroup,
    NvBool bFullSriov
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    NV_CHANNEL_ALLOC_PARAMS *pRpcParams;
    NV_STATUS status = NV_OK;

    pRpcParams = portMemAllocNonPaged(sizeof(*pRpcParams));
    NV_ASSERT_OR_RETURN(pRpcParams != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pRpcParams, 0, sizeof(*pRpcParams));

    pRpcParams->hObjectError      = pChannelGpfifoParams->hObjectError;
    pRpcParams->hObjectBuffer     = 0;
    pRpcParams->gpFifoOffset      = pChannelGpfifoParams->gpFifoOffset;
    pRpcParams->gpFifoEntries     = pChannelGpfifoParams->gpFifoEntries;
    pRpcParams->flags             = pChannelGpfifoParams->flags;
    pRpcParams->hContextShare     = pChannelGpfifoParams->hContextShare;
    pRpcParams->hVASpace          = pChannelGpfifoParams->hVASpace;
    pRpcParams->engineType        = pChannelGpfifoParams->engineType;
    pRpcParams->subDeviceId       = pChannelGpfifoParams->subDeviceId;
    pRpcParams->hObjectEccError   = pChannelGpfifoParams->hObjectEccError;
    pRpcParams->hPhysChannelGroup = pChannelGpfifoParams->hPhysChannelGroup;
    pRpcParams->internalFlags     = pChannelGpfifoParams->internalFlags;

    portMemCopy((void*)pRpcParams->hUserdMemory,
                sizeof(NvHandle) * NV2080_MAX_SUBDEVICES,
                (const void*)pChannelGpfifoParams->hUserdMemory,
                sizeof(NvHandle) * NV2080_MAX_SUBDEVICES);

    portMemCopy((void*)pRpcParams->userdOffset,
                sizeof(NvU64) * NV2080_MAX_SUBDEVICES,
                (const void*)pChannelGpfifoParams->userdOffset,
                sizeof(NvU64) * NV2080_MAX_SUBDEVICES);

    if (pKernelChannel->bCCSecureChannel)
    {
        portMemCopy((void*)pRpcParams->encryptIv,
                    sizeof(pRpcParams->encryptIv),
                    (const void*)pChannelGpfifoParams->encryptIv,
                    sizeof(pChannelGpfifoParams->encryptIv));

        portMemCopy((void*)pRpcParams->decryptIv,
                    sizeof(pRpcParams->decryptIv),
                    (const void*)pChannelGpfifoParams->decryptIv,
                    sizeof(pChannelGpfifoParams->decryptIv));

        portMemCopy((void*)pRpcParams->hmacNonce,
                    sizeof(pRpcParams->hmacNonce),
                    (const void*)pChannelGpfifoParams->hmacNonce,
                    sizeof(pChannelGpfifoParams->hmacNonce));
    }

    //
    // These fields are only filled out for GSP client or full SRIOV
    // i.e. the guest independently allocs ChID and instmem
    //
    if (IS_GSP_CLIENT(pGpu) || bFullSriov)
    {
        NvU32 subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        FIFO_INSTANCE_BLOCK *pInstanceBlock = pKernelChannel->pFifoHalData[subdevInst];
        NvU32 runqueue  = DRF_VAL(OS04, _FLAGS, _GROUP_CHANNEL_RUNQUEUE, pChannelGpfifoParams->flags);

        NV_ASSERT_TRUE_OR_GOTO(status,
                               pInstanceBlock != NULL,
                               NV_ERR_INVALID_STATE,
                               cleanup);

        portMemCopy(&pRpcParams->errorNotifierMem,
                    sizeof pRpcParams->errorNotifierMem,
                    &(pChannelGpfifoParams->errorNotifierMem),
                    sizeof pChannelGpfifoParams->errorNotifierMem);
        portMemCopy(&pRpcParams->eccErrorNotifierMem,
                    sizeof pRpcParams->eccErrorNotifierMem,
                    &(pChannelGpfifoParams->eccErrorNotifierMem),
                    sizeof pChannelGpfifoParams->eccErrorNotifierMem);

        // Fill the instance block
        if (pInstanceBlock)
        {
            pRpcParams->instanceMem.base =
                            memdescGetPhysAddr(pInstanceBlock->pInstanceBlockDesc, AT_GPU, 0);
            pRpcParams->instanceMem.size = pInstanceBlock->pInstanceBlockDesc->Size;
            pRpcParams->instanceMem.addressSpace =
                            memdescGetAddressSpace(pInstanceBlock->pInstanceBlockDesc);
            pRpcParams->instanceMem.cacheAttrib =
                            memdescGetCpuCacheAttrib(pInstanceBlock->pInstanceBlockDesc);

            pRpcParams->ramfcMem.base =
                            memdescGetPhysAddr(pInstanceBlock->pRamfcDesc,  AT_GPU, 0);
            pRpcParams->ramfcMem.size = pInstanceBlock->pRamfcDesc->Size;
            pRpcParams->ramfcMem.addressSpace =
                            memdescGetAddressSpace(pInstanceBlock->pRamfcDesc);
            pRpcParams->ramfcMem.cacheAttrib =
                            memdescGetCpuCacheAttrib(pInstanceBlock->pRamfcDesc);
        }

        // Fill the userd memory descriptor
        if (pKernelChannel->pUserdSubDeviceMemDesc[subdevInst])
        {
            pRpcParams->userdMem.base =
                            memdescGetPhysAddr(pKernelChannel->pUserdSubDeviceMemDesc[subdevInst], AT_GPU, 0);
            pRpcParams->userdMem.size = pKernelChannel->pUserdSubDeviceMemDesc[subdevInst]->Size;
            pRpcParams->userdMem.addressSpace =
                            memdescGetAddressSpace(pKernelChannel->pUserdSubDeviceMemDesc[subdevInst]);
            pRpcParams->userdMem.cacheAttrib =
                            memdescGetCpuCacheAttrib(pKernelChannel->pUserdSubDeviceMemDesc[subdevInst]);
        }

        // Fill the method buffer memory descriptor
        if (pKernelChannelGroup->pMthdBuffers != NULL &&
            pKernelChannelGroup->pMthdBuffers[runqueue].pMemDesc != NULL)
        {
            if (bFullSriov)
            {
                pRpcParams->mthdbufMem.base =
                    pKernelChannelGroup->pMthdBuffers[runqueue].bar2Addr;
                pRpcParams->mthdbufMem.size =
                    pKernelChannelGroup->pMthdBuffers[runqueue].pMemDesc->Size;
                pRpcParams->mthdbufMem.addressSpace = ADDR_VIRTUAL;
                pRpcParams->mthdbufMem.cacheAttrib = 0;
            }
            else
            {
                pRpcParams->mthdbufMem.base = memdescGetPhysAddr(
                    pKernelChannelGroup->pMthdBuffers[runqueue].pMemDesc,
                    AT_GPU, 0);
                pRpcParams->mthdbufMem.size =
                    pKernelChannelGroup->pMthdBuffers[runqueue].pMemDesc->Size;
                pRpcParams->mthdbufMem.addressSpace = memdescGetAddressSpace(
                    pKernelChannelGroup->pMthdBuffers[runqueue].pMemDesc);
                pRpcParams->mthdbufMem.cacheAttrib = memdescGetCpuCacheAttrib(
                    pKernelChannelGroup->pMthdBuffers[runqueue].pMemDesc);
            }
        }

        if (IS_GSP_CLIENT(pGpu))
        {
            //
            // Setting these param flags will make the Physical RMAPI use our
            // ChID (which is already decided)
            //

            NvU32 numChannelsPerUserd = NVBIT(DRF_SIZE(NVOS04_FLAGS_CHANNEL_USERD_INDEX_VALUE)); //  1<<3 -> 4K / 512B

            pRpcParams->flags = FLD_SET_DRF(OS04, _FLAGS,
                    _CHANNEL_USERD_INDEX_FIXED, _FALSE, pRpcParams->flags);
            pRpcParams->flags = FLD_SET_DRF(OS04, _FLAGS,
                    _CHANNEL_USERD_INDEX_PAGE_FIXED, _TRUE, pRpcParams->flags);
            pRpcParams->flags = FLD_SET_DRF_NUM(OS04, _FLAGS,
                    _CHANNEL_USERD_INDEX_VALUE, pKernelChannel->ChID % numChannelsPerUserd, pRpcParams->flags);
            pRpcParams->flags = FLD_SET_DRF_NUM(OS04, _FLAGS,
                    _CHANNEL_USERD_INDEX_PAGE_VALUE, pKernelChannel->ChID / numChannelsPerUserd, pRpcParams->flags);

            // GSP client needs to pass in privilege level as an alloc param since GSP-RM cannot check this
            pRpcParams->internalFlags =
                FLD_SET_DRF_NUM(_KERNELCHANNEL, _ALLOC_INTERNALFLAGS, _PRIVILEGE,
                    pKernelChannel->privilegeLevel, pRpcParams->internalFlags);
            if (pKernelChannel->bUvmOwned)
            {
                pRpcParams->internalFlags =
                    FLD_SET_DRF(_KERNELCHANNEL, _ALLOC_INTERNALFLAGS, _UVM_OWNED,
                        _YES, pRpcParams->internalFlags);
            }
            pRpcParams->ProcessID = pKernelChannel->ProcessID;
            pRpcParams->SubProcessID= pKernelChannel->SubProcessID;
        }
    }

    NV_RM_RPC_ALLOC_CHANNEL(pGpu,
                            RES_GET_CLIENT_HANDLE(pKernelChannel),
                            RES_GET_PARENT_HANDLE(pKernelChannel),
                            RES_GET_HANDLE(pKernelChannel),
                            RES_GET_EXT_CLASS_ID(pKernelChannel),
                            pRpcParams,
                            &pKernelChannel->ChID,
                            status);
    NV_ASSERT_OK_OR_GOTO(status, status, cleanup);

    NV_PRINTF(LEVEL_INFO,
        "Alloc Channel chid %d, hClient:0x%x, hParent:0x%x, hObject:0x%x, hClass:0x%x\n",
        pKernelChannel->ChID,
        RES_GET_CLIENT_HANDLE(pKernelChannel),
        RES_GET_PARENT_HANDLE(pKernelChannel),
        RES_GET_HANDLE(pKernelChannel),
        RES_GET_EXT_CLASS_ID(pKernelChannel));

cleanup:
    portMemFree(pRpcParams);

    return status;
}

/*!
 * @brief Bind a single channel to a runlist
 *
 * This is a helper function for kchannelCtrlCmdBind and kchangrpapiCtrlCmdBind
 */
NV_STATUS kchannelBindToRunlist_IMPL
(
    KernelChannel *pKernelChannel,
    RM_ENGINE_TYPE localRmEngineType,
    ENGDESCRIPTOR  engineDesc
)
{
    OBJGPU *pGpu;
    KernelFifo *pKernelFifo;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);
    pGpu = GPU_RES_GET_GPU(pKernelChannel);
    pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    // copied from setRunlistIdByEngineType
    if ((engineDesc == ENG_SW) || (engineDesc == ENG_BUS))
    {
        return NV_OK;
    }

    if (IS_GSP_CLIENT(pGpu) ||
        (IS_VIRTUAL(pGpu) &&
         !(IS_VIRTUAL_WITH_FULL_SRIOV(pGpu) &&
           kfifoIsPerRunlistChramEnabled(pKernelFifo))))
    {
        NVA06F_CTRL_BIND_PARAMS params;

        params.engineType = gpuGetNv2080EngineType(localRmEngineType);

        NV_RM_RPC_CONTROL(pGpu,
                          RES_GET_CLIENT_HANDLE(pKernelChannel),
                          RES_GET_HANDLE(pKernelChannel),
                          NVA06F_CTRL_CMD_BIND,
                          &params,
                          sizeof(params),
                          status);

        NV_ASSERT_OK_OR_RETURN(status);
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    status = kfifoRunlistSetIdByEngine_HAL(pGpu, pKernelFifo, pKernelChannel, engineDesc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to set RunlistID 0x%08x for channel 0x%08x\n",
                  engineDesc, kchannelGetDebugTag(pKernelChannel));
        SLI_LOOP_BREAK;
    }

    SLI_LOOP_END;

    return status;
}

/*!
 * @brief Returns the context ID of a channel
 *
 */
NV_STATUS
kchannelCtrlCmdGetContextId_IMPL
(
    KernelChannel *pKernelChannel,
    NVA06F_CTRL_GET_CONTEXT_ID_PARAMS *pParams
)
{
    pParams->contextId = kchannelGetCid(pKernelChannel);
    return NV_OK;
}

NV_STATUS
kchannelCtrlCmdGetClassEngineid_IMPL
(
    KernelChannel *pKernelChannel,
    NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_STATUS status = NV_OK;
    RM_ENGINE_TYPE rmEngineType;

    //
    // MODS uses hObject 0 to figure out if this call is supported or not.
    // In SRIOV VF scenario, plugin asserts if host returns an error code
    // for a control call. Adding a temporary work around till MODS submits
    // a proper fix.
    //
    if (pParams->hObject == NV01_NULL_OBJECT)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       pParams->hObject != RES_GET_CLIENT_HANDLE(pKernelChannel),
                       NV_ERR_INVALID_ARGUMENT);

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          RES_GET_HANDLE(pKernelChannel),
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    NV_ASSERT_OK_OR_RETURN(
        kchannelGetClassEngineID_HAL(pGpu, pKernelChannel, pParams->hObject,
                                 &pParams->classEngineID,
                                 &pParams->classID,
                                 &rmEngineType));

    pParams->engineID = gpuGetNv2080EngineType(rmEngineType);

    if (IS_MIG_IN_USE(pGpu) &&
        kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, rmEngineType))
    {
        MIG_INSTANCE_REF ref;
        RM_ENGINE_TYPE localRmEngineType;

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                            GPU_RES_GET_DEVICE(pKernelChannel),
                                            &ref));

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &localRmEngineType));

        NV_PRINTF(LEVEL_INFO, "Overriding global engine type 0x%x to local engine type 0x%x (0x%x) due to MIG\n",
                  pParams->engineID, gpuGetNv2080EngineType(localRmEngineType), localRmEngineType);

        pParams->engineID = gpuGetNv2080EngineType(localRmEngineType);
    }

    return status;
}

NV_STATUS
kchannelCtrlCmdResetIsolatedChannel_IMPL
(
    KernelChannel *pKernelChannel,
    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pResetIsolatedChannelParams
)
{
    NV_STATUS  status    = NV_OK;
    OBJGPU    *pGpu      = GPU_RES_GET_GPU(pKernelChannel);
    RM_API    *pRmApi    = GPU_GET_PHYSICAL_RMAPI(pGpu);


    // This ctrl sets bIsRcPending in the KernelChannel object. Because Kernel-RM is
    // the source of truth on this, it's important that this ctrl is called from CPU-RM
    NV_ASSERT_OR_RETURN(!RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_INVALID_OPERATION);

    // Call internal RMCTRL on physical-RM, kchannelFwdToInternalCtrl() is not
    // used because no conversion from KernelChannel to Channel is required
    status = pRmApi->Control(pRmApi,
                             resservGetTlsCallContext()->pControlParams->hClient,
                             RES_GET_HANDLE(pKernelChannel),
                             NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL,
                             pResetIsolatedChannelParams,
                             sizeof(NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS));

    // If physical RM successfully reset the isolated channel,
    // mark that the RC is no longer pending
    if (status == NV_OK)
        pKernelChannel->bIsRcPending[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = NV_FALSE;

    return status;
}

// This ctrl accesses bIsRcPending in the KernelChannel object to populate
// information required by physical RM. Because Kernel-RM is the source of
// truth on this, it's important that this ctrl be called originally from CPU-RM.
NV_STATUS
kchannelCtrlCmdResetChannel_IMPL
(
    KernelChannel *pKernelChannel,
    NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams
)
{
    NV_STATUS status    = NV_OK;
    OBJGPU   *pGpu      = GPU_RES_GET_GPU(pKernelChannel);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    if (!(pRmCtrlParams->bInternal ||
          pResetChannelParams->resetReason <
              NV906F_CTRL_CMD_RESET_CHANNEL_REASON_ENUM_MAX))
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    // Send physical RM info on if an RC is pending
    pResetChannelParams->bIsRcPending =
        pKernelChannel->bIsRcPending[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

    //
    // All real hardware management is done in the host.
    // Do an RPC to the host to do the hardware update and return.
    //
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          RES_GET_HANDLE(pKernelChannel),
                          NV906F_CTRL_CMD_RESET_CHANNEL,
                          pResetChannelParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    //
    // Do an internal control call to do channel reset
    // on Host (Physical) RM
    //
    return kchannelFwdToInternalCtrl_HAL(pGpu,
                                         pKernelChannel,
                                         NV906F_CTRL_CMD_RESET_CHANNEL,
                                         pRmCtrlParams);
}

NV_STATUS
kchannelCtrlCmdGpFifoSchedule_IMPL
(
    KernelChannel *pKernelChannel,
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pKernelChannel);
    NV_STATUS     rmStatus      = NV_OK;
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    //
    // Bug 1737765: Prevent Externally Owned Channels from running unless bound
    //  It is possible for clients to allocate and schedule channels while
    //  skipping the UVM registration step which binds the appropriate
    //  allocations in RM. We need to fail channel scheduling if the channels
    //  have not been registered with UVM.
    //  This check is performed on baremetal, CPU-RM and guest-RM
    //
    NV_ASSERT_OR_RETURN(kchannelIsSchedulable_HAL(pGpu, pKernelChannel), NV_ERR_INVALID_STATE);

    //
    // If this was a host-only channel we'll have never set the runlist id, so
    // force it here to ensure it is immutable now that the channel is scheduled.
    //
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    kchannelSetRunlistSet(pGpu, pKernelChannel, NV_TRUE);
    SLI_LOOP_END


    //
    // All real hardware management is done in the host.
    // Do an RPC to the host to do the hardware update and return.
    //
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          RES_GET_CLIENT_HANDLE(pKernelChannel),
                          RES_GET_HANDLE(pKernelChannel),
                          NVA06F_CTRL_CMD_GPFIFO_SCHEDULE,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          rmStatus);

        return rmStatus;
    }

    //
    // Do an internal control call to do channel reset
    // on Host (Physical) RM
    //
    return kchannelFwdToInternalCtrl_HAL(pGpu,
                                         pKernelChannel,
                                         NVA06F_CTRL_CMD_INTERNAL_GPFIFO_SCHEDULE,
                                         pRmCtrlParams);
}

NV_STATUS
kchannelCtrlCmdSetErrorNotifier_IMPL
(
    KernelChannel *pKernelChannel,
    NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS *pSetErrorNotifierParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    RC_NOTIFIER_SCOPE scope;
    NV_STATUS rmStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO,
              "calling setErrorNotifier on channel: 0x%x, broadcast to TSG: %s\n",
              kchannelGetDebugTag(pKernelChannel),
              pSetErrorNotifierParams->bNotifyEachChannelInTSG ? "true" : "false");

    scope = pSetErrorNotifierParams->bNotifyEachChannelInTSG ?
                RC_NOTIFIER_SCOPE_TSG :
                RC_NOTIFIER_SCOPE_CHANNEL;

    rmStatus = krcErrorSetNotifier(pGpu, GPU_GET_KERNEL_RC(pGpu),
                                   pKernelChannel,
                                   ROBUST_CHANNEL_GR_EXCEPTION,
                                   kchannelGetEngineType(pKernelChannel),
                                   scope);
    return rmStatus;
}

NV_STATUS
kchannelCtrlCmdBind_IMPL
(
    KernelChannel *pKernelChannel,
    NVA06F_CTRL_BIND_PARAMS *pParams
)
{
    RM_ENGINE_TYPE globalRmEngineType;
    RM_ENGINE_TYPE localRmEngineType;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);
    NV_STATUS rmStatus = NV_OK;
    ENGDESCRIPTOR engineDesc;

    if (!pParams)
        return NV_ERR_INVALID_ARGUMENT;

    // Check if channel belongs to TSG that is not internal RM TSG
    if (!pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bAllocatedByRm)
    {
        // This may be valid request if we added new channel to TSG that is
        // already running. In that case we just have to check that it uses
        // the same runlist as whole TSG.
        // We do that in fifoRunlistSetId()
        NV_PRINTF(LEVEL_INFO,
                  "Bind requested for channel %d belonging to TSG %d.\n",
                  kchannelGetDebugTag(pKernelChannel),
                  pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->grpID);
    }

    localRmEngineType = globalRmEngineType = gpuGetRmEngineType(pParams->engineType);

    if (bMIGInUse)
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                            GPU_RES_GET_DEVICE(pKernelChannel),
                                            &ref));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref, localRmEngineType,
                                              &globalRmEngineType));

    }

    NV_PRINTF(LEVEL_INFO, "Binding Channel %d to Engine %d\n",
              kchannelGetDebugTag(pKernelChannel), globalRmEngineType);

    // Translate globalRmEngineType -> enginedesc
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
        gpuXlateClientEngineIdToEngDesc(pGpu, globalRmEngineType, &engineDesc));

    if (rmStatus == NV_OK)
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            kchannelBindToRunlist(pKernelChannel, localRmEngineType, engineDesc));
    }

    return rmStatus;
}

NV_STATUS
kchannelCtrlCmdSetInterleaveLevel_IMPL
(
    KernelChannel *pKernelChannel,
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams
)
{
    OBJGPU          *pGpu         = GPU_RES_GET_GPU(pKernelChannel);
    NV_STATUS        status       = NV_OK;

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          RES_GET_CLIENT_HANDLE(pKernelChannel),
                          RES_GET_HANDLE(pKernelChannel),
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, NV_ERR_NOT_SUPPORTED);
    }

    status = kchangrpSetInterleaveLevel(pGpu, pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup, pParams->channelInterleaveLevel);

    return status;
}

NV_STATUS
kchannelCtrlCmdGetInterleaveLevel_IMPL
(
    KernelChannel *pKernelChannel,
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);

    pParams->channelInterleaveLevel =
        pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pInterleaveLevel[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

    return NV_OK;
}

NV_STATUS
kchannelCtrlCmdGpfifoGetWorkSubmitToken_IMPL
(
    KernelChannel *pKernelChannel,
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pKernelChannel);
    KernelFifo   *pKernelFifo   = GPU_GET_KERNEL_FIFO(pGpu);
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
    NvBool bIsMIGEnabled        = IS_MIG_ENABLED(pGpu);

    NvBool bIsModsVgpu          = NV_FALSE;

    NvBool bIsVgpuRpcNeeded     = (bIsModsVgpu || (IS_VIRTUAL(pGpu) &&
                                  !(IS_VIRTUAL_WITH_SRIOV(pGpu) && !bIsMIGEnabled &&
                                    kfifoIsPerRunlistChramEnabled(pKernelFifo)))) &&
                                    (!pKernelFifo->bGuestGenenratesWorkSubmitToken);

    NvBool bGenerateWorkSubmitToken = !bIsModsVgpu || pKernelFifo->bGuestGenenratesWorkSubmitToken;

    //
    // vGPU:
    // If required call into the host to get the worksubmit token.
    //
    if (bIsVgpuRpcNeeded)
    {
        NV_STATUS rmStatus = NV_OK;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          RES_GET_HANDLE(pKernelChannel),
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          rmStatus);
        //
        // All done if error or for non-MODS vGPU guest (host did notification in RPC).
        // GSP FW is not able to perform the notification, nor is MODS vGPU host,
        // so it still needs to be handled by the client/guest outside the RPC.
        //
        if (rmStatus != NV_OK)
        {
            return rmStatus;
        }

        if (IS_VIRTUAL(pGpu))
        {
            return rmStatus;
        }
    }

    if (bGenerateWorkSubmitToken)
    {
        NV_ASSERT_OR_RETURN(pKernelChannel->pKernelChannelGroupApi != NULL, NV_ERR_INVALID_STATE);

        KernelChannelGroup *pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;
        NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_STATE);

        NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
            kfifoGenerateWorkSubmitToken(pGpu, pKernelFifo, pKernelChannel,
                                         &pTokenParams->workSubmitToken,
                                         pKernelChannelGroup->bIsCallingContextVgpuPlugin));
    }

    return kchannelNotifyWorkSubmitToken(pGpu, pKernelChannel, pTokenParams->workSubmitToken);
}

NV_STATUS
kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_IMPL
(
    KernelChannel *pKernelChannel,
    NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams
)
{
    NV_STATUS   rmStatus    = NV_OK;
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pKernelChannel);

    //
    // vGPU:
    //
    // Since vgpu plugin is required to update notifier for guest, send an RPC
    // to host RM for the plugin to hook.
    // RPC not needed for SR-IOV vGpu.
    //
    // GSP-RM:
    //
    // Notification is done in CPU-RM, so RPC is not made to FW-RM.
    //
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvBool bIsMIGEnabled    = IS_MIG_ENABLED(pGpu);
    NvBool bIsVgpuRpcNeeded = IS_VIRTUAL(pGpu) &&
                              !(IS_VIRTUAL_WITH_SRIOV(pGpu) && !bIsMIGEnabled &&
                                kfifoIsPerRunlistChramEnabled(pKernelFifo)) &&
                              (!pKernelFifo->bGuestGenenratesWorkSubmitToken);
    if (bIsVgpuRpcNeeded)
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          RES_GET_HANDLE(pKernelChannel),
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          rmStatus);
        return rmStatus;
    }

    rmStatus = kchannelUpdateWorkSubmitTokenNotifIndex(pGpu, pKernelChannel, pParams->index);
    return rmStatus;
}

NV_STATUS
kchannelRegisterChild_IMPL
(
    KernelChannel     *pKernelChannel,
    ChannelDescendant *pObject
)
{
    NvU16 firstObjectClassID;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    //
    // On recent GPU architectures such as FERMI, SetObject operations
    // require an EngineID:ClassID tuple as an argument, rather than
    // an object handle. In order to be able to differentiate between
    // different instances of any given software class, the ClassID
    // field needs to be unique within the FIFO context. The code below
    // attempts to find a qualifying 16-bit ClassID.
    //
    if (pObject->resourceDesc.engDesc == ENG_SW)
    {
        RS_ORDERED_ITERATOR it;
        RsClient *pClient = RES_GET_CLIENT(pKernelChannel);
        ChannelDescendant *pMatchingObject = NULL;

        firstObjectClassID = pKernelChannel->nextObjectClassID;

        do
        {
            if (++pKernelChannel->nextObjectClassID == firstObjectClassID)
            {
                NV_PRINTF(LEVEL_ERROR, "channel %08x:%08x: out of handles!\n",
                          RES_GET_CLIENT_HANDLE(pKernelChannel), RES_GET_HANDLE(pKernelChannel));
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }
            if (pKernelChannel->nextObjectClassID == 0)
                continue;

            it = clientRefOrderedIter(pClient, RES_GET_REF(pKernelChannel), classId(ChannelDescendant), NV_FALSE);

            while (clientRefOrderedIterNext(pClient, &it))
            {
                pMatchingObject = dynamicCast(it.pResourceRef->pResource, ChannelDescendant);
                NV_ASSERT_OR_ELSE(pMatchingObject != NULL, continue);

                if ((pMatchingObject->resourceDesc.engDesc == ENG_SW) &&
                    (pMatchingObject->classID == pKernelChannel->nextObjectClassID))
                {
                    break;
                }

                pMatchingObject = NULL;
            }
        }
        while (pMatchingObject != NULL);

        pObject->classID = pKernelChannel->nextObjectClassID;
    }

    return kfifoAddObject_HAL(pGpu, pKernelFifo, pObject);
}

NV_STATUS
kchannelDeregisterChild_IMPL
(
    KernelChannel     *pKernelChannel,
    ChannelDescendant *pObject
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    status = kfifoDeleteObject_HAL(pGpu, pKernelFifo, pObject);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not delete hal resources with object\n");
        DBG_BREAKPOINT();
    }

    return status;
}

void
kchannelGetChildIterator
(
    KernelChannel *pKernelChannel,
    NvU32 classID,
    RM_ENGINE_TYPE engineID,
    KernelChannelChildIterator *pIter
)
{
    RsClient *pClient = RES_GET_CLIENT(pKernelChannel);
    NV_ASSERT_OR_RETURN_VOID(pIter != NULL);

    portMemSet(pIter, 0, sizeof(*pIter));
    pIter->classID = classID;
    pIter->engineID = engineID;
    pIter->rsIter = clientRefOrderedIter(pClient, RES_GET_REF(pKernelChannel), classId(ChannelDescendant), NV_FALSE);
}

ChannelDescendant *
kchannelGetNextChild
(
    KernelChannelChildIterator *pIter
)
{
    ChannelDescendant *pChild;

    NV_ASSERT_OR_RETURN(pIter != NULL, NULL);

    while (clientRefOrderedIterNext(pIter->rsIter.pClient, &pIter->rsIter))
    {
        pChild = dynamicCast(pIter->rsIter.pResourceRef->pResource, ChannelDescendant);
        NV_ASSERT_OR_RETURN(pChild != NULL, NULL);

        // Continue to the next child if it doesn't match these filters:
        if (pIter->engineID != pChild->resourceDesc.engDesc)
            continue;
        if (pIter->classID != 0)
        {
            if ((RES_GET_EXT_CLASS_ID(pChild) != pIter->classID) &&
                (pChild->classID != pIter->classID))
                continue;
        }

        // Yield this matching child
        return pChild;
    }

    return NULL;
}

ChannelDescendant *
kchannelGetOneChild
(
    KernelChannel *pKernelChannel,
    NvU32          classID,
    NvU32          engineID
)
{
    KernelChannelChildIterator iter;

    kchannelGetChildIterator(pKernelChannel, classID, engineID, &iter);
    return kchannelGetNextChild(&iter);
}

/**
 * @brief Gets object iterator for a channel or channel group
 *
 * @param[in] pKernelChannel
 * @param[in] classNum
 * @param[in] engDesc
 * @param[out] pIt
 *
 */
void
kchannelGetChildIterOverGroup
(
    KernelChannel                   *pKernelChannel,
    NvU32                            classNum,
    NvU32                            engDesc,
    KernelChannelChildIterOverGroup *pIt
)
{
    NV_ASSERT_OR_RETURN_VOID(pIt != NULL);
    portMemSet(pIt, 0, sizeof(*pIt));

    NV_ASSERT_OR_RETURN_VOID(pKernelChannel != NULL);

    pIt->classNum = classNum;
    pIt->engDesc = engDesc;

    pIt->channelNode.pKernelChannel =
        pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pChanList->pHead->pKernelChannel;
    pIt->channelNode.pNext =
        pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pChanList->pHead->pNext;

    kchannelGetChildIterator(pIt->channelNode.pKernelChannel, pIt->classNum, pIt->engDesc, &pIt->kchannelIter);
}

/**
 * @brief Get the next object based on given class/engine tag.
 * When the class number is 0, it is ignored.
 *
 * @param[in] pIt
 *
 * Returns: found child or NULL
 *
 */
ChannelDescendant *
kchannelGetNextChildOverGroup
(
    KernelChannelChildIterOverGroup *pIt
)
{
    PCHANNEL_NODE pHead = NULL;
    ChannelDescendant *pObject = NULL;

    NV_ASSERT_OR_RETURN(pIt != NULL, NULL);

    // Start iterating from the given object (if any) of the given channel.
    pHead = &pIt->channelNode;

    while ((pHead != NULL) && (pHead->pKernelChannel != NULL))
    {
        pObject = kchannelGetNextChild(&pIt->kchannelIter);

        if (pObject != NULL)
            break;

        //
        // If there are no more objects to inspect in the given channel,
        // move to the next channel (if any, for TSGs).
        //
        pHead = pHead->pNext;
        if (pHead != NULL)
        {
            NV_ASSERT_OR_ELSE(pHead->pKernelChannel != NULL, break);
            // Re-initialize the channeldescendant iterator based on this channel
            kchannelGetChildIterator(pHead->pKernelChannel, pIt->classNum, pIt->engDesc, &pIt->kchannelIter);
        }
    }

    // Cache off the next channel to start searching from in future iterations.
    pIt->channelNode.pKernelChannel = pHead ? pHead->pKernelChannel : NULL;
    pIt->channelNode.pNext = pHead ? pHead->pNext : NULL;

    return pObject;
}

NV_STATUS
kchannelFindChildByHandle
(
    KernelChannel *pKernelChannel,
    NvHandle hResource,
    ChannelDescendant **ppObject
)
{
    RsClient *pClient = RES_GET_CLIENT(pKernelChannel);
    RsResourceRef *pResourceRef = NULL;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, clientGetResourceRef(pClient, hResource, &pResourceRef));

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pResourceRef->pParentRef->hResource == RES_GET_HANDLE(pKernelChannel), NV_ERR_OBJECT_NOT_FOUND);

    *ppObject = dynamicCast(pResourceRef->pResource, ChannelDescendant);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, *ppObject != NULL, NV_ERR_OBJECT_NOT_FOUND);

    return NV_OK;
}

static NV_STATUS
_kchannelClearVAList
(
    OBJGPU          *pGpu,
    VA_LIST         *pVaList,
    NvBool           bUnmap
)
{
    //
    // Subcontext handling
    // We need to unmap the mappings on all the subcontext, since the this call will be made only on one of the TSG channels.
    //
    if (bUnmap)
    {
        OBJVASPACE *pVas;
        NvU64 vAddr;

        FOR_EACH_IN_VADDR_LIST(pVaList, pVas, vAddr)
        {
            dmaUnmapBuffer_HAL(pGpu, GPU_GET_DMA(pGpu), pVas, vAddr);
        }
        FOR_EACH_IN_VADDR_LIST_END(pVaList, pVas, vAddr);
    }

    vaListClear(pVaList);

    return NV_OK;
}

/**
 * @brief Set or clear the Engine Context Memdesc.
 *
 * Should be committed to hardware after this using channelCommitEngineContext().
 * Should be unmapped before cleared/changed using kchannelUnmapEngineCtxBuf()
 *
 * @param[in] pGpu
 * @param[in] pKernelChannel
 * @param[in] engDesc
 * @param[in] pMemDesc                the new memdesc to assign, or NULL to clear
 *
 * Returns: status
 */
NV_STATUS
kchannelSetEngineContextMemDesc_IMPL
(
    OBJGPU             *pGpu,
    KernelChannel      *pKernelChannel,
    NvU32               engDesc,
    MEMORY_DESCRIPTOR  *pMemDesc
)
{
    NV_STATUS status = NV_OK;
    ENGINE_CTX_DESCRIPTOR *pEngCtxDesc;
    KernelChannelGroup *pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;

    NV_PRINTF(LEVEL_INFO,
              "ChID %x engDesc 0x%x pMemDesc %p\n",
              kchannelGetDebugTag(pKernelChannel), engDesc, pMemDesc);

    NV_ASSERT_OR_RETURN(engDesc != ENG_FIFO, NV_ERR_INVALID_PARAMETER);

    if (IS_GR(engDesc))
    {
        NV_ASSERT_OK_OR_RETURN(kchannelCheckBcStateCurrent(pGpu, pKernelChannel));
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    // Get or allocate the EngCtxDesc
    pEngCtxDesc = pKernelChannelGroup->ppEngCtxDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

    if (pEngCtxDesc == NULL && pMemDesc == NULL)
    {
        // There is no need to clean up or alloc anything.
        SLI_LOOP_CONTINUE;
    }

    if (pEngCtxDesc != NULL)
    {
        // Cleanup for the engDesc context that existed before
        if (pEngCtxDesc->pMemDesc != NULL)
        {
            memdescFree(pEngCtxDesc->pMemDesc);
            memdescDestroy(pEngCtxDesc->pMemDesc);
        }

        //
    }
    else
    {
        NV_ASSERT_OK_OR_ELSE(status,
            kchangrpAllocEngineContextDescriptor(pGpu, pKernelChannelGroup),
            SLI_LOOP_GOTO(fail));
        pEngCtxDesc = pKernelChannelGroup->ppEngCtxDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
        NV_ASSERT_OR_ELSE(pEngCtxDesc != NULL, status = NV_ERR_NO_MEMORY; SLI_LOOP_GOTO(fail));
    }

    if (pMemDesc != NULL)
    {
        // We are setting a memdesc
        if (pMemDesc->Allocated > 0)
            pMemDesc->Allocated++;
        memdescAddRef(pMemDesc);

        if (memdescGetAddressSpace(pMemDesc) == ADDR_VIRTUAL)
        {
            NvU64 virtAddr;

            // Since the memdesc is already virtual, we do not manage it
            status = vaListSetManaged(&pEngCtxDesc->vaList, NV_FALSE);
            NV_ASSERT_OR_ELSE(status == NV_OK, SLI_LOOP_GOTO(fail));

            // memdescGetPhysAddr of a virtual memdesc is a virtual addr
            virtAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
            status = vaListAddVa(&pEngCtxDesc->vaList, pKernelChannel->pVAS, virtAddr);
            NV_ASSERT_OR_ELSE(status == NV_OK, SLI_LOOP_GOTO(fail));
        }
    }

    // Assign the memdesc (or NULL)
    pEngCtxDesc->pMemDesc = pMemDesc;
    pEngCtxDesc->engDesc = engDesc;

    SLI_LOOP_END

fail:
    return status;
}

/**
 * @brief Unmaps everything from the Engine Context Memdesc.
 *
 * @param[in] pGpu
 * @param[in] pKernelChannel
 * @param[in] engDesc
 *
 * Returns: status
 */
NV_STATUS
kchannelUnmapEngineCtxBuf_IMPL
(
    OBJGPU             *pGpu,
    KernelChannel      *pKernelChannel,
    NvU32               engDesc
)
{
    NV_STATUS status = NV_OK;
    ENGINE_CTX_DESCRIPTOR *pEngCtxDesc;

    NV_PRINTF(LEVEL_INFO,
              "ChID %x engDesc 0x%x\n",
              kchannelGetDebugTag(pKernelChannel), engDesc);

    NV_ASSERT_OR_RETURN(engDesc != ENG_FIFO, NV_ERR_INVALID_PARAMETER);

    if (IS_GR(engDesc))
    {
        NV_ASSERT_OK_OR_RETURN(kchannelCheckBcStateCurrent(pGpu, pKernelChannel));
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    pEngCtxDesc = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

    // EngCtxDesc and MemDesc will be here, or else nothing can be mapped
    if ((pEngCtxDesc == NULL) || (pEngCtxDesc->pMemDesc == NULL))
    {
        SLI_LOOP_CONTINUE;
    }

    // Clear VA list, including unmap if managed
    status = _kchannelClearVAList(pGpu, &pEngCtxDesc->vaList, vaListGetManaged(&pEngCtxDesc->vaList));
    NV_ASSERT_OR_ELSE(status == NV_OK, SLI_LOOP_GOTO(fail));

    SLI_LOOP_END

fail:
    return status;
}

// Check that BcState stays consistent for GR channel engine context
NV_STATUS
kchannelCheckBcStateCurrent_IMPL
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
#define KERNEL_CHANNEL_BCSTATE_UNINITIALIZED (0)
#define KERNEL_CHANNEL_BCSTATE_DISABLED (1)
#define KERNEL_CHANNEL_BCSTATE_ENABLED (2)

    NvBool bBcState = gpumgrGetBcEnabledStatus(pGpu);
    NvU8   channelBcStateEnum = bBcState ? KERNEL_CHANNEL_BCSTATE_ENABLED : KERNEL_CHANNEL_BCSTATE_DISABLED;

    NV_PRINTF(
        LEVEL_INFO,
        "GPU = %d, ChID = %d, bcStateCurrent = %d, channelBcStateEnum = %d\n",
        pGpu->gpuInstance,
        kchannelGetDebugTag(pKernelChannel),
        pKernelChannel->bcStateCurrent,
        channelBcStateEnum);

    // Check that the BC status did not change - 0 = first call, 1 = disable, 2 = enable.
    if (pKernelChannel->bcStateCurrent == KERNEL_CHANNEL_BCSTATE_UNINITIALIZED)
    {
        pKernelChannel->bcStateCurrent = channelBcStateEnum;
    }
    NV_ASSERT_OR_RETURN(pKernelChannel->bcStateCurrent == channelBcStateEnum, NV_ERR_INVALID_STATE);

    return NV_OK;
}

// Map the Engine Context Memdesc and add it's VAddr
NV_STATUS
kchannelMapEngineCtxBuf_IMPL
(
    OBJGPU      *pGpu,
    KernelChannel *pKernelChannel,
    NvU32        engDesc
)
{
    OBJVASPACE            *pVAS           = NULL;
    NV_STATUS              status         = NV_OK;
    ENGINE_CTX_DESCRIPTOR *pEngCtx;
    NvU64                  addr;
    MEMORY_DESCRIPTOR     *pTempMemDesc;
    OBJGVASPACE           *pGVAS;
    KernelFifo            *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);

    NV_ASSERT_OR_RETURN(engDesc != ENG_FIFO, NV_ERR_INVALID_ARGUMENT);

    if (IS_GR(engDesc))
    {
        NV_ASSERT_OK_OR_RETURN(kchannelCheckBcStateCurrent(pGpu, pKernelChannel));
    }

    NV_PRINTF(LEVEL_INFO, "ChID %d engDesc %s (0x%x) \n",
              kchannelGetDebugTag(pKernelChannel),
              kfifoGetEngineName_HAL(GPU_GET_KERNEL_FIFO(pGpu), ENGINE_INFO_TYPE_ENG_DESC, engDesc),
              engDesc);

    pVAS = pKernelChannel->pVAS;
    pGVAS = dynamicCast(pVAS, OBJGVASPACE);
    NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_INVALID_STATE);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

    pEngCtx = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
    NV_ASSERT_OR_ELSE(pEngCtx != NULL, status = NV_ERR_INVALID_STATE; goto fail);

    pTempMemDesc = pEngCtx->pMemDesc;
    NV_ASSERT_OR_ELSE(pTempMemDesc != NULL, status = NV_ERR_INVALID_STATE; goto fail);

    //
    // For virtual context, UMD has already alloced/mapped the engine context.
    // So simply get the vaddr
    //

    status = vaListFindVa(&pEngCtx->vaList, pVAS, &addr);
    if (status == NV_OK)
    {
        // VAddr already exists and needs no action
        SLI_LOOP_CONTINUE;
    }
    else if (status == NV_ERR_OBJECT_NOT_FOUND)
    {
        NvU32 flags = DMA_ALLOC_VASPACE_NONE;
        if (gvaspaceIsExternallyOwned(pGVAS))
        {
            // We should never land up here if VA space is externally owned!
            NV_ASSERT_FAILED("Externally owned object not found");
            status = NV_ERR_INVALID_OPERATION;
            goto fail;
        }

        kfifoGetCtxBufferMapFlags_HAL(pGpu, pKernelFifo, engDesc, &flags);

        status = dmaMapBuffer_HAL(pGpu, GPU_GET_DMA(pGpu), pVAS, pTempMemDesc, &addr,
            flags, DMA_UPDATE_VASPACE_FLAGS_NONE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not map context buffer for engDesc 0x%x\n",
                      engDesc);
            goto fail;
        }
        else
        {
            status = vaListAddVa(&pEngCtx->vaList, pVAS, addr);
            NV_ASSERT(status == NV_OK);
        }
    }
    else
    {
        NV_ASSERT_OK_FAILED("vaListFindVa", status);
        goto fail;
    }

fail:
    if (status != NV_OK)
    {
        SLI_LOOP_BREAK;
    }
    SLI_LOOP_END

    return status;
}

/**
 * @brief Updates the notifier index with which to update the work submit
 *        notifier on request.
 *
 * @param[IN] pGpu              OBJGPU
 * @param[in] pKernelChannel    KernelChannel
 * @param[in] index             Updated notifier index
 *
 * @return NV_OK
 *         NV_ERR_OUT_OF_RANGE if index is beyond the bounds of the notifier
 */
NV_STATUS
kchannelUpdateWorkSubmitTokenNotifIndex_IMPL
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NvU32 index
)
{
    NvHandle hNotifier;
    RsClient *pClient = RES_GET_CLIENT(pKernelChannel);
    Memory *pMemory;
    ContextDma *pContextDma;
    NvU32 addressSpace;
    NvU64 notificationBufferSize;
    Device *pDevice;

    hNotifier = pKernelChannel->hErrorContext;

    // Clobbering error notifier index is illegal
    NV_CHECK_OR_RETURN(LEVEL_INFO, index != NV_CHANNELGPFIFO_NOTIFICATION_TYPE_ERROR,
                     NV_ERR_INVALID_ARGUMENT);

    // If key rotation is enabled then clobbering key rotation notifier is disallowed
    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    if ((pConfCompute != NULL) &&
        (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED)))
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, index != NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS,
                           NV_ERR_INVALID_ARGUMENT);
    }

    // Check for integer overflows
    if (((index + 1) < index) ||
        !portSafeMulU64(index + 1, sizeof(NvNotification), &notificationBufferSize))
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    pDevice = GPU_RES_GET_DEVICE(pKernelChannel);

    if (NV_OK == memGetByHandleAndDevice(pClient, hNotifier, RES_GET_HANDLE(pDevice), &pMemory))
    {
        addressSpace = memdescGetAddressSpace(pMemory->pMemDesc);

        NV_CHECK_OR_RETURN(LEVEL_INFO, pMemory->Length >= notificationBufferSize,
                         NV_ERR_OUT_OF_RANGE);
        switch (addressSpace)
        {
            case ADDR_VIRTUAL:
            {
                NvU64 physAddr = memdescGetPhysAddr(pMemory->pMemDesc, AT_GPU_VA, 0);
                PCLI_DMA_MAPPING_INFO pDmaMappingInfo;

                NV_CHECK_OR_RETURN(LEVEL_INFO,
                    CliGetDmaMappingInfo(pClient,
                                         RES_GET_HANDLE(pDevice),
                                         RES_GET_HANDLE(pMemory),
                                         physAddr,
                                         gpumgrGetDeviceGpuMask(pGpu->deviceInstance),
                                         &pDmaMappingInfo),
                    NV_ERR_GENERIC);

                NV_CHECK_OR_RETURN(LEVEL_INFO, pDmaMappingInfo->pMemDesc->Size >= notificationBufferSize,
                                 NV_ERR_OUT_OF_RANGE);
                break;
            }
            case ADDR_FBMEM:
                // fall through
            case ADDR_SYSMEM:
                // Covered by check prior to switch/case
                break;
            default:
                return NV_ERR_NOT_SUPPORTED;
        }
    }
    else if (NV_OK == ctxdmaGetByHandle(pClient, hNotifier, &pContextDma))
    {
        NV_CHECK_OR_RETURN(LEVEL_INFO, pContextDma->Limit >= (notificationBufferSize - 1),
                         NV_ERR_OUT_OF_RANGE);
    }
    else
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pKernelChannel->notifyIndex[NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN]
        = index;

    return NV_OK;
}

/**
 * @brief Updates the work submit notifier passed to the channel during channel
 *        creation with the new work submit token.
 *
 * @param[IN] pGpu              OBJGPU
 * @param[in] pKernelChannel    KernelChannel
 * @param[in] token             Work submit token to notify clients of
 *
 * @return NV_OK on successful notify
 *         NV_OK if client has not set up the doorbell notifier. This should
 *         be an error once all clients have been updated.
 */
NV_STATUS
kchannelNotifyWorkSubmitToken_IMPL
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NvU32 token
)
{
    NvU16 notifyStatus = 0x0;
    NvU32 index = pKernelChannel->notifyIndex[NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN];

    notifyStatus =
        FLD_SET_DRF(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _IN_PROGRESS, _TRUE, notifyStatus);
    notifyStatus =
        FLD_SET_DRF_NUM(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _VALUE, 0xFFFF, notifyStatus);

    return kchannelUpdateNotifierMem(pKernelChannel, index, token, 0, notifyStatus);
}

static NV_STATUS
_kchannelNotifyOfChid
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    RsClient *pRsClient
)
{
    ContextDma *pContextDma;

    //
    // Return the chid to the drivers in the error context DMA
    //
    // We need to update this when virtual channel gets mapped in.
    //

    if ((ctxdmaGetByHandle(pRsClient, pKernelChannel->hErrorContext, &pContextDma)) == NV_OK)
    {
        NV_CHECK_OR_RETURN(LEVEL_INFO, pContextDma->Limit >= sizeof(NvNotification) - 1, NV_ERR_INVALID_ARGUMENT);
        notifyFillNotifier(pGpu, pContextDma, pKernelChannel->ChID, 0, NV_OK);
    }

    return NV_OK;
}

NvU32
kchannelGetGfid_IMPL
(
    KernelChannel *pKernelChannel
)
{
    return pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->gfid;
}

NvBool
kchannelIsCpuMapped
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel
)
{
    return !!(pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &
              KERNEL_CHANNEL_SW_STATE_CPU_MAP);
}

void
kchannelSetCpuMapped
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NvBool bCpuMapped
)
{
    if (bCpuMapped)
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] |=
              KERNEL_CHANNEL_SW_STATE_CPU_MAP;
    }
    else
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &=
              ~(KERNEL_CHANNEL_SW_STATE_CPU_MAP);
    }
}

NvBool
kchannelIsRunlistSet
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel
)
{
    return !!(pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &
              KERNEL_CHANNEL_SW_STATE_RUNLIST_SET);
}

void
kchannelSetRunlistSet
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NvBool bRunlistSet
)
{
    if (bRunlistSet)
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] |=
              KERNEL_CHANNEL_SW_STATE_RUNLIST_SET;
    }
    else
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &=
              ~(KERNEL_CHANNEL_SW_STATE_RUNLIST_SET);
    }
}

NV_STATUS
kchannelGetChannelPhysicalState_KERNEL
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams
)
{
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    // Get the physical state from GSP
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                           pRmCtrlParams->hClient,
                                           pRmCtrlParams->hObject,
                                           NV208F_CTRL_CMD_FIFO_GET_CHANNEL_STATE,
                                           pChannelStateParams,
                                           sizeof(*pChannelStateParams)));

    return NV_OK;
}

NV_STATUS
kchannelMapUserD_IMPL
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    RS_PRIV_LEVEL   privLevel,
    NvU64           offset,
    NvU32           protect,
    NvP64          *ppCpuVirtAddr,
    NvP64          *ppPriv
)
{
    NV_STATUS status      = NV_OK;
    NvU64     userBase;
    NvU64     userOffset;
    NvU64     userSize;
    NvU32     cachingMode = NV_MEMORY_UNCACHED;

    // if USERD is allocated by client
    if (pKernelChannel->bClientAllocatedUserD)
    {
        return NV_OK;
    }

    status = kchannelGetUserdInfo_HAL(pGpu, pKernelChannel,
                                      &userBase, &userOffset, &userSize);

    if (status != NV_OK)
        return status;


    if (userBase == pGpu->busInfo.gpuPhysAddr)
    {
        // Create a mapping of BAR0
        status = osMapGPU(pGpu, privLevel, NvU64_LO32(userOffset+offset),
                 NvU64_LO32(userSize), protect, ppCpuVirtAddr, ppPriv);
        goto done;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        cachingMode = NV_MEMORY_CACHED;
    }

    //
    // If userBase is not bar0, then it is bar1 and we create a regular memory
    // mapping.
    //
    if (privLevel >= RS_PRIV_LEVEL_KERNEL)
    {
        status = osMapPciMemoryKernel64(pGpu, userBase + userOffset + offset,
                                        userSize, protect, ppCpuVirtAddr, cachingMode);
    }
    else
    {
        status = osMapPciMemoryUser(pGpu->pOsGpuInfo,
                                    userBase + userOffset + offset,
                                    userSize, protect, ppCpuVirtAddr,
                                    ppPriv, cachingMode);
    }
    if (!((status == NV_OK) && *ppCpuVirtAddr))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "BAR1 offset 0x%llx for USERD of channel %x could not be cpu mapped\n",
                  userOffset, kchannelGetDebugTag(pKernelChannel));
    }

done:

    // Indicate channel is mapped
    if (status == NV_OK)
    {
            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
            kchannelSetCpuMapped(pGpu, pKernelChannel, NV_TRUE);
            SLI_LOOP_END
    }

    return status;
}

void
kchannelUnmapUserD_IMPL
(
    OBJGPU         *pGpu,
    KernelChannel  *pKernelChannel,
    RS_PRIV_LEVEL   privLevel,
    NvP64          *ppCpuVirtAddr,
    NvP64          *ppPriv
)
{
    NV_STATUS status;
    NvU64     userBase;
    NvU64     userOffset;
    NvU64     userSize;

    if (pKernelChannel->bClientAllocatedUserD)
    {
        return;
    }

    status = kchannelGetUserdInfo_HAL(pGpu, pKernelChannel,
                                      &userBase, &userOffset, &userSize);

    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);

    if (userBase == pGpu->busInfo.gpuPhysAddr)
    {
        osUnmapGPU(pGpu->pOsGpuInfo, privLevel, *ppCpuVirtAddr,
                   NvU64_LO32(userSize), *ppPriv);
    }
    else
    {
        // GF100+
        // Unmap Cpu virt mapping
        if (privLevel >= RS_PRIV_LEVEL_KERNEL)
        {
            osUnmapPciMemoryKernel64(pGpu, *ppCpuVirtAddr);
        }
        else
        {
            osUnmapPciMemoryUser(pGpu->pOsGpuInfo, *ppCpuVirtAddr,
                                 userSize, *ppPriv);
        }
    }

    // Indicate channel is !mapped
    kchannelSetCpuMapped(pGpu, pKernelChannel, NV_FALSE);
    return;
}

static NV_STATUS
_kchannelGetUserMemDesc
(
    OBJGPU             *pGpu,
    KernelChannel      *pKernelChannel,
    PMEMORY_DESCRIPTOR *ppMemDesc
)
{
    NV_ASSERT_OR_RETURN(ppMemDesc != NULL, NV_ERR_INVALID_STATE);
    *ppMemDesc = NULL;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_STATE);

    *ppMemDesc = pKernelChannel->pInstSubDeviceMemDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

    return *ppMemDesc ? NV_OK : NV_ERR_INVALID_STATE;
}

/*!
 * @brief Retrieve a KernelChannel from either a KernelChannel or TSG handle. KernelChannel is
 * checked first. If TSG is provided, the head of the TSG is returned.
 *
 * @param[in]  pClient            Client object
 * @param[in]  hDual              NvHandle either to TSG or to KernelChannel
 * @param[out] ppKernelChannel    Referenced KernelChannel
 */
NV_STATUS
kchannelGetFromDualHandle_IMPL
(
    RsClient        *pClient,
    NvHandle         hDual,
    KernelChannel  **ppKernelChannel
)
{
    KernelChannel *pKernelChannel;
    RsResourceRef *pChanGrpRef;

    NV_ASSERT_OR_RETURN(ppKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    *ppKernelChannel = NULL;

    if (CliGetKernelChannel(pClient, hDual, &pKernelChannel) == NV_OK)
    {
        *ppKernelChannel = pKernelChannel;
        return NV_OK;
    }

    if (CliGetChannelGroup(pClient->hClient, hDual, &pChanGrpRef, NULL) == NV_OK)
    {
        KernelChannelGroupApi *pKernelChannelGroupApi = dynamicCast(
            pChanGrpRef->pResource,
            KernelChannelGroupApi);

        NV_ASSERT_OR_RETURN(
            (pKernelChannelGroupApi != NULL) &&
                (pKernelChannelGroupApi->pKernelChannelGroup != NULL),
            NV_ERR_INVALID_ARGUMENT);

        if (pKernelChannelGroupApi->pKernelChannelGroup->chanCount == 0)
            return NV_ERR_INVALID_ARGUMENT;

        *ppKernelChannel =
            pKernelChannelGroupApi->pKernelChannelGroup->pChanList->pHead->pKernelChannel;
        NV_ASSERT_OR_RETURN(*ppKernelChannel != NULL, NV_ERR_INVALID_STATE);

        return NV_OK;
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief Retrieve a KernelChannel from either a KernelChannel or TSG handle. KernelChannel is
 * checked first. If TSG is provided, the head of the TSG is returned. If
 * KernelChannel handle is provided, it must not be part of a client-allocated TSG.
 *
 * @param[in]  pClient            Client object
 * @param[in]  hDual              NvHandle either to TSG or to bare Channel
 * @param[out] ppKernelChannel    Referenced KernelChannel
 */
NV_STATUS
kchannelGetFromDualHandleRestricted_IMPL
(
    RsClient        *pClient,
    NvHandle         hDual,
    KernelChannel  **ppKernelChannel
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kchannelGetFromDualHandle(pClient, hDual, ppKernelChannel));
    if ((RES_GET_HANDLE(*ppKernelChannel) == hDual) &&
        (((*ppKernelChannel)->pKernelChannelGroupApi->pKernelChannelGroup != NULL) &&
         !(*ppKernelChannel)->pKernelChannelGroupApi->pKernelChannelGroup->bAllocatedByRm))
    {
        NV_PRINTF(LEVEL_ERROR, "channel handle 0x%08x is part of a channel group, not allowed!\n",
                  RES_GET_HANDLE(*ppKernelChannel));
        return NV_ERR_INVALID_ARGUMENT;
    }
    return NV_OK;
}

static void
_kchannelUpdateFifoMapping
(
    KernelChannel    *pKernelChannel,
    OBJGPU           *pGpu,
    NvBool            bKernel,
    NvP64             cpuAddress,
    NvP64             priv,
    NvU64             cpuMapLength,
    NvU32             flags,
    NvHandle          hSubdevice,
    RsCpuMapping     *pMapping
)
{
    pMapping->pPrivate->pGpu      = pGpu;
    pMapping->pPrivate->bKernel   = bKernel;
    pMapping->processId = osGetCurrentProcess();
    pMapping->pLinearAddress      = cpuAddress;
    pMapping->pPrivate->pPriv     = priv;
    pMapping->length              = cpuMapLength;
    pMapping->flags               = flags;
    pMapping->pContext            = (void*)(NvUPtr)pKernelChannel->ChID;
}

NV_STATUS kchannelDeriveAndRetrieveKmb_KERNEL
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    ROTATE_IV_TYPE rotateOperation,
    NvBool bIncludeIvOrNonce,
    CC_KMB *keyMaterialBundle
)
{
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    NV_ASSERT(pCC != NULL);

    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveViaChannel_HAL(pCC, pKernelChannel, rotateOperation,
                                                                   bIncludeIvOrNonce, keyMaterialBundle));

    return (confComputeKeyStoreRetrieveViaChannel_HAL(pCC, pKernelChannel, rotateOperation,
                                                      bIncludeIvOrNonce, keyMaterialBundle));
}

/*!
 * @brief Get KMB for secure channel
 *
 * @param[in] pKernelChannnel
 * @param[out] pGetKmbParams
 */
NV_STATUS
kchannelCtrlCmdGetKmb_KERNEL
(
    KernelChannel *pKernelChannel,
    NVC56F_CTRL_CMD_GET_KMB_PARAMS *pGetKmbParams
)
{
    NV_STATUS status = NV_OK;
    if (!pKernelChannel->bCCSecureChannel)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    if ((pConfCompute != NULL) && pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED))
    {
        KEY_ROTATION_STATUS state;
        NvU32 h2dKey;
        NV_ASSERT_OK_OR_RETURN(confComputeGetKeyPairByChannel(pGpu, pConfCompute, pKernelChannel, &h2dKey, NULL));
        NV_ASSERT_OK_OR_RETURN(confComputeGetKeyRotationStatus(pConfCompute, h2dKey, &state));
        if ((state != KEY_ROTATION_STATUS_IDLE) ||
            (kchannelIsDisabledForKeyRotation(pGpu, pKernelChannel)))
        {
            return NV_ERR_KEY_ROTATION_IN_PROGRESS;
        }
    }

    portMemCopy((void*)(&pGetKmbParams->kmb), sizeof(CC_KMB),
                (const void*)(&pKernelChannel->clientKmb), sizeof(CC_KMB));
    confComputeGetKeyRotationThreshold(pConfCompute,
                                       &pGetKmbParams->keyRotationLowerThreshold,
                                       &pGetKmbParams->keyRotationUpperThreshold);

    if ((pConfCompute != NULL) && pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED))
    {
        RM_API            *pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        RsClient          *pRsClient      = NULL;
        RsResourceRef     *pResourceRef   = NULL;
        NvHandle           hClient        = RES_GET_CLIENT_HANDLE(pKernelChannel);
        NvHandle           hDevice        = RES_GET_HANDLE(GPU_RES_GET_DEVICE(pKernelChannel));

        NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClient, &pRsClient));
        // check if a valid memory handle was passed to us
        if (clientGetResourceRef(pRsClient, pGetKmbParams->hMemory, &pResourceRef) == NV_OK)
        {
            //
            // If a buffer already exists then remove it as we
            // will replace it with a new buffer.
            //
            if (pKernelChannel->pEncStatsBuf != NULL)
            {
                NV_ASSERT_OK_OR_RETURN(kchannelSetEncryptionStatsBuffer_HAL(pGpu, pKernelChannel, NULL, NV_FALSE));
                pRmApi->Free(pRmApi, hClient, pKernelChannel->hEncryptStatsBuf);
            }

            // dup the memory handle under the same client
            NV_ASSERT_OK_OR_RETURN(pRmApi->DupObject(pRmApi, hClient, hDevice,
                                                     &pKernelChannel->hEncryptStatsBuf,
                                                     hClient, pGetKmbParams->hMemory, 0));
            NV_ASSERT_OK_OR_GOTO(status, 
                clientGetResourceRef(pRsClient, pKernelChannel->hEncryptStatsBuf, &pResourceRef),
                cleanup);
            Memory *pMemory = dynamicCast(pResourceRef->pResource, Memory);
            NV_ASSERT_OR_ELSE((pMemory != NULL) && (pMemory->pMemDesc != NULL),
                status = NV_ERR_INVALID_ARGUMENT;
                goto cleanup);
            NV_ASSERT_OK_OR_GOTO(status, 
                kchannelSetEncryptionStatsBuffer_HAL(pGpu, pKernelChannel, pMemory->pMemDesc, NV_TRUE),
                cleanup);
        }

        //
        // Reset statistics every time GET_KMB is called irrespective of whether
        // a new encrypt stats buffer was created or not.
        //
        if (pKernelChannel->pEncStatsBuf != NULL)
        {
            portMemSet(pKernelChannel->pEncStatsBuf, 0, sizeof(CC_CRYPTOBUNDLE_STATS));
        }

cleanup:
        if (status != NV_OK)
        {
            pRmApi->Free(pRmApi, hClient, pKernelChannel->hEncryptStatsBuf);
        }
    }

    return status;
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief      Rotate the IVs for the given secure channel
 *
 * @param[in]  pKernelChannel
 * @param[out] pRotateIvParams
 *
 * @return     NV_OK on success
 * @return     NV_ERR_NOT_SUPPORTED if channel is not a secure channel.
 */
NV_STATUS
kchannelCtrlRotateSecureChannelIv_KERNEL
(
    KernelChannel *pKernelChannel,
    NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *pRotateIvParams
)
{
    NV_STATUS            status            = NV_OK;
    OBJGPU              *pGpu              = GPU_RES_GET_GPU(pKernelChannel);
    RM_API              *pRmApi            = GPU_GET_PHYSICAL_RMAPI(pGpu);
    ConfidentialCompute *pCC               = GPU_GET_CONF_COMPUTE(pGpu);
    ROTATE_IV_TYPE       rotateIvOperation = pRotateIvParams->rotateIvType;

    if (!pKernelChannel->bCCSecureChannel)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_PRINTF(LEVEL_INFO, "Rotating IV in CPU-RM.\n");

    status = confComputeKeyStoreRetrieveViaChannel_HAL(
        pCC, pKernelChannel, rotateIvOperation, NV_TRUE, &pKernelChannel->clientKmb);

    if (status != NV_OK)
    {
        return status;
    }

    portMemSet(pRotateIvParams, 0, sizeof(*pRotateIvParams));

    portMemCopy(pRotateIvParams->updatedKmb.encryptBundle.iv,
                sizeof(pRotateIvParams->updatedKmb.encryptBundle.iv),
                pKernelChannel->clientKmb.encryptBundle.iv,
                sizeof(pKernelChannel->clientKmb.encryptBundle.iv));

    portMemCopy(pRotateIvParams->updatedKmb.decryptBundle.iv,
                sizeof(pRotateIvParams->updatedKmb.decryptBundle.iv),
                pKernelChannel->clientKmb.decryptBundle.iv,
                sizeof(pKernelChannel->clientKmb.decryptBundle.iv));

    pRotateIvParams->rotateIvType = rotateIvOperation;

    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pKernelChannel),
                             RES_GET_HANDLE(pKernelChannel),
                             NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV,
                             pRotateIvParams,
                             sizeof(*pRotateIvParams));

    if (status != NV_OK)
    {
        return status;
    }

    if ((rotateIvOperation == ROTATE_IV_ALL_VALID) || (rotateIvOperation == ROTATE_IV_ENCRYPT))
    {
        portMemCopy(&pRotateIvParams->updatedKmb.encryptBundle,
                    sizeof(pRotateIvParams->updatedKmb.encryptBundle),
                    &pKernelChannel->clientKmb.encryptBundle,
                    sizeof(pKernelChannel->clientKmb.encryptBundle));
    }

    if ((rotateIvOperation == ROTATE_IV_ALL_VALID) || (rotateIvOperation == ROTATE_IV_DECRYPT))
    {
        portMemCopy(&pRotateIvParams->updatedKmb.decryptBundle,
                    sizeof(pRotateIvParams->updatedKmb.decryptBundle),
                    &pKernelChannel->clientKmb.decryptBundle,
                    sizeof(pKernelChannel->clientKmb.decryptBundle));
    }

    return NV_OK;
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kchannelCtrlRotateSecureChannelIv_PHYSICAL
(
    KernelChannel *pKernelChannel,
    NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *pRotateIvParams
)
{
    NV_STATUS status;

    NV_PRINTF(LEVEL_INFO, "Rotating IV in GSP-RM.\n");

    // CPU-side encrypt IV corresponds to GPU-side decrypt IV.
    // CPU-side decrypt IV corresponds to GPU-side encrypt IV.
    status =
        kchannelRotateSecureChannelIv_HAL(pKernelChannel,
                                          pRotateIvParams->rotateIvType,
                                          pRotateIvParams->updatedKmb.decryptBundle.iv,
                                          pRotateIvParams->updatedKmb.encryptBundle.iv);
    if (status != NV_OK)
    {
        return status;
    }

    return NV_OK;
}

/*!
 * Fill in per-channel MMU exception data and allocate memory for this data if
 * necessary
 *
 * @param[inout]    pKernelChannel
 * @param[in]       pMmuExceptionData MMU exception data to be copied
 */
void
kchannelFillMmuExceptionInfo_IMPL
(
    KernelChannel           *pKernelChannel,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN_VOID(pKernelChannel);

    if (pKernelChannel->pMmuExceptionData == NULL)
    {
        pKernelChannel->pMmuExceptionData = portMemAllocNonPaged(sizeof(FIFO_MMU_EXCEPTION_DATA));
        if (pKernelChannel->pMmuExceptionData == NULL)
            status = NV_ERR_NO_MEMORY;
    }

    if (status == NV_OK)
    {
        portMemCopy(pKernelChannel->pMmuExceptionData,
                    sizeof(FIFO_MMU_EXCEPTION_DATA),
                    pMmuExceptionData,
                    sizeof(FIFO_MMU_EXCEPTION_DATA));
    }
}

/*!
 * Free per-channel MMU exception data if it exists
 *
 * @param[inout]    pKernelChannel
 */
void
kchannelFreeMmuExceptionInfo_IMPL
(
    KernelChannel           *pKernelChannel
)
{
    portMemFree(pKernelChannel->pMmuExceptionData);
    pKernelChannel->pMmuExceptionData = NULL;
}

/*!
 * Check if channel is disabled for key rotation
 */
NvBool kchannelIsDisabledForKeyRotation
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel
)
{
    return !!(pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &
              KERNEL_CHANNEL_SW_STATE_DISABLED_FOR_KEY_ROTATION);
}

/*!
 * Mark channel disabled for key rotation
 */
void kchannelDisableForKeyRotation
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NvBool bDisable
)
{
    if (bDisable)
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] |=
        KERNEL_CHANNEL_SW_STATE_DISABLED_FOR_KEY_ROTATION;
    }
    else
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &=
        ~KERNEL_CHANNEL_SW_STATE_DISABLED_FOR_KEY_ROTATION;
    }
}

/*!
 * Check if channel needs to be enabled after key rotation
 */
NvBool kchannelIsEnableAfterKeyRotation
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel
)
{
    return !!(pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &
              KERNEL_CHANNEL_SW_STATE_ENABLE_AFTER_KEY_ROTATION);
}

/*!
 * Mark channel to be re-enabled after key rotation completes
 */
void kchannelEnableAfterKeyRotation
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NvBool bEnable
)
{
    if (bEnable)
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] |=
        KERNEL_CHANNEL_SW_STATE_ENABLE_AFTER_KEY_ROTATION;
    }
    else
    {
        pKernelChannel->swState[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] &=
        ~KERNEL_CHANNEL_SW_STATE_ENABLE_AFTER_KEY_ROTATION;
    }
}

/*!
 * Creates/destroys persistent mappings for key rotation notifier
 */
NV_STATUS
kchannelSetKeyRotationNotifier_KERNEL
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    NvBool bSet
)
{
    NV_STATUS status = NV_OK;
    MEMORY_DESCRIPTOR *pNotifierMemDesc = pKernelChannel->pErrContextMemDesc;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_ASSERT_OR_RETURN(pNotifierMemDesc != NULL, NV_ERR_INVALID_STATE);
    NV_ADDRESS_SPACE addressSpace = memdescGetAddressSpace(pNotifierMemDesc);
    NvU32 notifyIndex = NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS;
    if (bSet)
    {
        NV_ASSERT_OR_RETURN(memdescGetSize(pNotifierMemDesc) >= ((notifyIndex + 1) * sizeof(NvNotification)),
                            NV_ERR_INVALID_ARGUMENT);

        NV_ASSERT_OR_RETURN(addressSpace == ADDR_SYSMEM, NV_ERR_NOT_SUPPORTED);
        if (pKernelChannel->pKeyRotationNotifierMemDesc == NULL)
        {
            NV_ASSERT_OK_OR_RETURN(memdescCreateSubMem(&pKernelChannel->pKeyRotationNotifierMemDesc,
                                                       pNotifierMemDesc, pGpu, notifyIndex * sizeof(NvNotification),
                                                       sizeof(NvNotification)));

            pKernelChannel->pKeyRotationNotifier =
                (NvNotification *) memmgrMemDescBeginTransfer(pMemoryManager,
                                                              pKernelChannel->pKeyRotationNotifierMemDesc,
                                                              TRANSFER_FLAGS_SHADOW_ALLOC);
            NV_ASSERT_OR_ELSE(pKernelChannel->pKeyRotationNotifier != NULL, status = NV_ERR_INVALID_STATE; goto done;);

            portMemSet((void*)pKernelChannel->pKeyRotationNotifier, 0, sizeof(NvNotification));
        }
    }
    else
    {
        if (pKernelChannel->pKeyRotationNotifierMemDesc != NULL)
        {
            if (pKernelChannel->pKeyRotationNotifier != NULL)
            {
                memmgrMemDescEndTransfer(pMemoryManager,
                                         pKernelChannel->pKeyRotationNotifierMemDesc,
                                         TRANSFER_FLAGS_SHADOW_ALLOC);
                pKernelChannel->pKeyRotationNotifier = NULL;
            }
            memdescDestroy(pKernelChannel->pKeyRotationNotifierMemDesc);
            pKernelChannel->pKeyRotationNotifierMemDesc = NULL;
        }
    }

done:
    if (status != NV_OK)
    {
        if (pKernelChannel->pKeyRotationNotifierMemDesc != NULL)
        {
            memdescDestroy(pKernelChannel->pKeyRotationNotifierMemDesc);
            pKernelChannel->pKeyRotationNotifierMemDesc = NULL;
        }
    }
    return status;
}

/*!
 * Creates/destroys persistent mappings for encryption stats buffer
 */
NV_STATUS
kchannelSetEncryptionStatsBuffer_KERNEL
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool bSet
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    if (bSet)
    {
        NV_ASSERT_OR_RETURN(pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pKernelChannel->pEncStatsBuf == NULL, NV_ERR_INVALID_STATE);

        if (kchannelCheckIsKernel(pKernelChannel))
        {
            //
            // kernel channels use ccslContextInitViaChannel and don't supply a mem handle to be duped.
            // so we need to refcount in this case.
            //
            NV_ASSERT_OK_OR_RETURN(memdescCreateSubMem(&pKernelChannel->pEncStatsBufMemDesc, pMemDesc, pGpu,
                                                       0, memdescGetSize(pMemDesc)));
        }
        else
        {
            pKernelChannel->pEncStatsBufMemDesc = pMemDesc;
        }
        //
        // we rely on persistent mapping for encryption statistics buffer
        // since these will be used in top half and mappings are not allowed
        // in contexts that can't sleep on KVM or similar HCC systems.
        //
        pKernelChannel->pEncStatsBuf =
            (CC_CRYPTOBUNDLE_STATS*)memmgrMemDescBeginTransfer(pMemoryManager, pKernelChannel->pEncStatsBufMemDesc,
                                                               TRANSFER_FLAGS_SHADOW_ALLOC);
        if (pKernelChannel->pEncStatsBuf == NULL)
        {
            if (kchannelCheckIsKernel(pKernelChannel))
            {
                memdescDestroy(pKernelChannel->pEncStatsBufMemDesc);
            }
            pKernelChannel->pEncStatsBufMemDesc = NULL;
            return NV_ERR_INVALID_STATE;
        }
        portMemSet(pKernelChannel->pEncStatsBuf, 0, sizeof(CC_CRYPTOBUNDLE_STATS));
    }
    else
    {
        // Free persistent mappings for encryption stats buffer
        NV_ASSERT_OR_RETURN(pKernelChannel->pEncStatsBufMemDesc != NULL, NV_ERR_INVALID_STATE);
        memmgrMemDescEndTransfer(pMemoryManager, pKernelChannel->pEncStatsBufMemDesc,
                                 TRANSFER_FLAGS_SHADOW_ALLOC);
        pKernelChannel->pEncStatsBuf = NULL;
        if (kchannelCheckIsKernel(pKernelChannel))
        {
            NV_ASSERT(memdescIsSubMemoryMemDesc(pKernelChannel->pEncStatsBufMemDesc));
            memdescDestroy(pKernelChannel->pEncStatsBufMemDesc);
        }
        pKernelChannel->pEncStatsBufMemDesc = NULL;
    }
    return NV_OK;
}

static NvNotification*
_kchannelGetKeyRotationNotifier(KernelChannel *pKernelChannel)
{
    return pKernelChannel->pKeyRotationNotifier;
}
