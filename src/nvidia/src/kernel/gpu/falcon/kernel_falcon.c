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
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/sec2/kernel_sec2.h"
#include "gpu/gsp/kernel_gsp.h"

#include "gpu/fifo/kernel_fifo.h"
#include "gpu/fifo/kernel_channel.h"
#include "gpu/fifo/kernel_channel_group.h"
#include "gpu/fifo/kernel_channel_group_api.h"
#include "gpu/intr/intr.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/device/device.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/video/kernel_video_engine.h"
#include "mem_mgr/gpu_vaspace.h"
#include "mem_mgr/ctx_buf_pool.h"
#include "rmapi/rmapi.h"

void kflcnConfigureEngine_IMPL(OBJGPU *pGpu, KernelFalcon *pKernelFalcon, KernelFalconEngineConfig *pFalconConfig)
{
    pKernelFalcon->registerBase       = pFalconConfig->registerBase;
    pKernelFalcon->riscvRegisterBase  = pFalconConfig->riscvRegisterBase;
    pKernelFalcon->fbifBase           = pFalconConfig->fbifBase;
    pKernelFalcon->bBootFromHs        = pFalconConfig->bBootFromHs;
    pKernelFalcon->pmcEnableMask      = pFalconConfig->pmcEnableMask;
    pKernelFalcon->bIsPmcDeviceEngine = pFalconConfig->bIsPmcDeviceEngine;
    pKernelFalcon->physEngDesc        = pFalconConfig->physEngDesc;
    pKernelFalcon->ctxAttr            = pFalconConfig->ctxAttr;
    pKernelFalcon->ctxBufferSize      = pFalconConfig->ctxBufferSize;
    pKernelFalcon->addrSpaceList      = pFalconConfig->addrSpaceList;

    // Configure CrashCat with caller arguments (disabled by default)
    kcrashcatEngineConfigure(staticCast(pKernelFalcon, KernelCrashCatEngine),
                             &pFalconConfig->crashcatEngConfig);

    NV_PRINTF(LEVEL_INFO, "for physEngDesc 0x%x\n", pKernelFalcon->physEngDesc);
}

KernelFalcon *kflcnGetKernelFalconForEngine_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR physEngDesc)
{
    //
    // Check for any special objects that are instantiated as GPU children.
    // Otherwise, OBJGPU keeps track of all falcons as reported by GSP
    //
    switch (physEngDesc)
    {
        // this list is mirrored in subdeviceCtrlCmdInternalGetConstructedFalconInfo_IMPL
        case ENG_SEC2:
        {
            KernelFalcon *pKernelSec2 = staticCast(GPU_GET_KERNEL_SEC2(pGpu), KernelFalcon);
            if (pKernelSec2 != NULL)
                return pKernelSec2;
            break; // If KernelSec2 does not exist on this chip, fall back to GKF list
        }
        case ENG_GSP:      return staticCast(GPU_GET_KERNEL_GSP(pGpu), KernelFalcon);
    }

    return staticCast(gpuGetGenericKernelFalconForEngine(pGpu, physEngDesc), KernelFalcon);
}


static NvBool _kflcnNeedToAllocContext(OBJGPU *pGpu, KernelChannel *pKernelChannel)
{
    NvU32 gfid = kchannelGetGfid(pKernelChannel);

    //
    // In case of vGPU, when client allocated ctx buffer feature enabled, vGPU guest
    // RM will alloc all FLCN context buffers for VF channels.
    // But, for PF channels (IS_GFID_PF(gfid) is TRUE), host RM needs to allocate the
    // FLCN buffers.
    //
    if (!gpuIsClientRmAllocatedCtxBufferEnabled(pGpu) || IS_GFID_VF(gfid))
        return NV_FALSE;

    return NV_TRUE;
}

static NV_STATUS _kflcnAllocAndMapCtxBuffer
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFalcon,
    KernelChannel *pKernelChannel
)
{
    MEMORY_DESCRIPTOR  *pCtxMemDesc = NULL;
    CTX_BUF_POOL_INFO  *pCtxBufPool = NULL;
    KernelChannelGroup *pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;
    OBJGVASPACE        *pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);
    NV_STATUS           status = NV_OK;
    NvU64               flags = MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;

    if (kchannelIsCtxBufferAllocSkipped(pKernelChannel))
        return NV_OK;

    kchangrpGetEngineContextMemDesc(pGpu, pKernelChannelGroup, &pCtxMemDesc);
    if (pCtxMemDesc != NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "This channel already has a falcon engine instance on engine %d:%d\n",
                  ENGDESC_FIELD(pKernelFalcon->physEngDesc, _CLASS),
                  ENGDESC_FIELD(pKernelFalcon->physEngDesc, _INST));
        return NV_OK;
    }

    if (ctxBufPoolIsSupported(pGpu) && pKernelChannelGroup->pCtxBufPool != NULL)
    {
        flags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
        pCtxBufPool = pKernelChannelGroup->pCtxBufPool;
    }

    //
    // Setup an engine context and initialize.
    //
    NV_ASSERT_OK_OR_RETURN(memdescCreate(&pCtxMemDesc, pGpu,
               pKernelFalcon->ctxBufferSize,
               FLCN_BLK_ALIGNMENT,
               NV_TRUE,
               ADDR_UNKNOWN,
               pKernelFalcon->ctxAttr,
               flags));
    NV_ASSERT_OK_OR_GOTO(status,
        memdescSetCtxBufPool(pCtxMemDesc, pCtxBufPool),
        done);
    memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_115, 
                        pCtxMemDesc, memdescU32ToAddrSpaceList(pKernelFalcon->addrSpaceList));
    NV_ASSERT_OK_OR_GOTO(status, status, done);

    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemDescMemSet(GPU_GET_MEMORY_MANAGER(pGpu), pCtxMemDesc, 0,
                            TRANSFER_FLAGS_NONE),
        done);

    NV_ASSERT_OK_OR_GOTO(status,
        kchannelSetEngineContextMemDesc(pGpu, pKernelChannel, pKernelFalcon->physEngDesc, pCtxMemDesc),
        done);

    if (!gvaspaceIsExternallyOwned(pGVAS))
    {
        NV_ASSERT_OK_OR_GOTO(status,
            kchannelMapEngineCtxBuf(pGpu, pKernelChannel, pKernelFalcon->physEngDesc),
            done);
    }

done:
    if (status != NV_OK)
    {
        memdescFree(pCtxMemDesc);
        memdescDestroy(pCtxMemDesc);
    }

    return status;
}

static NV_STATUS _kflcnPromoteContext
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFalcon,
    KernelChannel *pKernelChannel
)
{
    RM_API                *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    RsClient              *pClient = RES_GET_CLIENT(pKernelChannel);
    Device                *pDevice = GPU_RES_GET_DEVICE(pKernelChannel);
    Subdevice             *pSubdevice;
    RM_ENGINE_TYPE         rmEngineType;
    ENGINE_CTX_DESCRIPTOR *pEngCtx;
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS rmCtrlParams = {0};
    OBJGVASPACE           *pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);

    NV_ASSERT_OR_RETURN(gpumgrGetSubDeviceInstanceFromGpu(pGpu) == 0, NV_ERR_INVALID_STATE);
    NV_ASSERT_OK_OR_RETURN(subdeviceGetByInstance(pClient, RES_GET_HANDLE(pDevice), 0, &pSubdevice));

    pEngCtx = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[0];
    NV_ASSERT_OR_RETURN(pEngCtx != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
                            ENGINE_INFO_TYPE_ENG_DESC, pKernelFalcon->physEngDesc,
                            ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType));

    rmCtrlParams.hClient     = pClient->hClient;
    rmCtrlParams.hObject     = RES_GET_HANDLE(pKernelChannel);
    rmCtrlParams.hChanClient = pClient->hClient;
    rmCtrlParams.size        = pKernelFalcon->ctxBufferSize;
    rmCtrlParams.engineType  = gpuGetNv2080EngineType(rmEngineType);
    rmCtrlParams.ChID        = pKernelChannel->ChID;

    // Promote physical address only. VA will be promoted later as part of nvgpuBindChannelResources
    if (gvaspaceIsExternallyOwned(pGVAS))
    {
        MEMORY_DESCRIPTOR *pMemDesc = NULL;
        NvU32 physAttr = 0x0;

        NV_ASSERT_OK_OR_RETURN(kchangrpGetEngineContextMemDesc(pGpu,
                                   pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup, &pMemDesc));
        NV_ASSERT_OR_RETURN(memdescGetContiguity(pMemDesc, AT_GPU), NV_ERR_INVALID_STATE);

        switch (memdescGetAddressSpace(pMemDesc))
        {
            case ADDR_FBMEM:
                physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX,
                           _APERTURE, _VIDMEM, physAttr);
                break;

            case ADDR_SYSMEM:
                if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
                {
                    physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX,
                                _APERTURE, _COH_SYS, physAttr);
                }
                else if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED)
                {
                    physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX,
                               _APERTURE, _NCOH_SYS, physAttr);
                }
                else
                {
                    return NV_ERR_INVALID_STATE;
                }
                break;

            default:
                return NV_ERR_INVALID_STATE;
        }

        physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX, _GPU_CACHEABLE, _NO, physAttr);

        rmCtrlParams.entryCount = 1;
        rmCtrlParams.promoteEntry[0].gpuPhysAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
        rmCtrlParams.promoteEntry[0].size = pMemDesc->Size;
        rmCtrlParams.promoteEntry[0].physAttr = physAttr;
        rmCtrlParams.promoteEntry[0].bufferId = 0; // unused for flcn
        rmCtrlParams.promoteEntry[0].bInitialize = NV_TRUE;
        rmCtrlParams.promoteEntry[0].bNonmapped = NV_TRUE;
    }
    else
    {
        NvU64 addr;
        NV_ASSERT_OK_OR_RETURN(vaListFindVa(&pEngCtx->vaList, pKernelChannel->pVAS, &addr));
        rmCtrlParams.virtAddress = addr;
    }

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pClient->hClient, RES_GET_HANDLE(pSubdevice),
        NV2080_CTRL_CMD_GPU_PROMOTE_CTX, &rmCtrlParams, sizeof(rmCtrlParams)));

    return NV_OK;
}


NV_STATUS kflcnAllocContext_IMPL
(
    OBJGPU        *pGpu,
    KernelFalcon  *pKernelFalcon,
    KernelChannel *pKernelChannel,
    NvU32          classNum
)
{
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    if (!_kflcnNeedToAllocContext(pGpu, pKernelChannel))
        return NV_OK;

    NV_ASSERT_OR_RETURN(gpuIsClassSupported(pGpu, classNum), NV_ERR_INVALID_OBJECT);

    NV_ASSERT_OK_OR_RETURN(_kflcnAllocAndMapCtxBuffer(pGpu, pKernelFalcon, pKernelChannel));

    NV_CHECK(LEVEL_ERROR, videoEventTraceCtxInit(pGpu, pKernelChannel, pKernelFalcon->physEngDesc) == NV_OK);

    return _kflcnPromoteContext(pGpu, pKernelFalcon, pKernelChannel);
}

NV_STATUS kflcnFreeContext_IMPL
(
    OBJGPU        *pGpu,
    KernelFalcon  *pKernelFalcon,
    KernelChannel *pKernelChannel,
    NvU32          classNum
)
{
    NV_STATUS status = NV_OK;
    MEMORY_DESCRIPTOR *pCtxMemDesc = NULL;
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    if (!_kflcnNeedToAllocContext(pGpu, pKernelChannel))
        return NV_OK;

    if (kchannelIsCtxBufferAllocSkipped(pKernelChannel))
        return NV_OK;

    kchangrpGetEngineContextMemDesc(pGpu,
        pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup,
        &pCtxMemDesc);

    if (pCtxMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "The channel 0x%x does not have a falcon engine instance for engDesc=0x%x\n",
                  kchannelGetDebugTag(pKernelChannel), pKernelFalcon->physEngDesc);
        return NV_OK;
    }

    kchannelUnmapEngineCtxBuf(pGpu, pKernelChannel, pKernelFalcon->physEngDesc);
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
        kchannelSetEngineContextMemDesc(pGpu, pKernelChannel, pKernelFalcon->physEngDesc, NULL));
    memdescFree(pCtxMemDesc);
    memdescDestroy(pCtxMemDesc);

    return status;
}

NV_STATUS gkflcnConstruct_IMPL
(
    GenericKernelFalcon *pGenericKernelFalcon,
    OBJGPU *pGpu,
    KernelFalconEngineConfig *pFalconConfig
)
{
    KernelFalcon *pKernelFalcon = staticCast(pGenericKernelFalcon, KernelFalcon);
    if (pFalconConfig != NULL)
    {
        kflcnConfigureEngine(pGpu, pKernelFalcon, pFalconConfig);
    }
    return NV_OK;
}

NV_STATUS gkflcnResetHw_IMPL(OBJGPU *pGpu, GenericKernelFalcon *pGenKernFlcn)
{
    NV_ASSERT_FAILED("This should only be called on full KernelFalcon implementations");
    return NV_ERR_NOT_SUPPORTED;
}

void gkflcnRegisterIntrService_IMPL(OBJGPU *pGpu, GenericKernelFalcon *pGenericKernelFalcon, IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX])
{
    KernelFalcon *pKernelFalcon = staticCast(pGenericKernelFalcon, KernelFalcon);
    NV_ASSERT_OR_RETURN_VOID(pKernelFalcon);

    NV_PRINTF(LEVEL_INFO, "physEngDesc 0x%x\n", pKernelFalcon->physEngDesc);

    if (!IS_VIDEO_ENGINE(pKernelFalcon->physEngDesc) && pKernelFalcon->physEngDesc != ENG_SEC2)
        return;

    // Register to handle nonstalling interrupts of the corresponding physical falcon in kernel rm
    if (pKernelFalcon->physEngDesc != ENG_INVALID)
    {
        NvU32 mcIdx = MC_ENGINE_IDX_NULL;

        NV_STATUS status = kfifoEngineInfoXlate_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            ENGINE_INFO_TYPE_ENG_DESC, pKernelFalcon->physEngDesc,
            ENGINE_INFO_TYPE_MC, &mcIdx);

        if (IS_VIRTUAL(pGpu) && (status == NV_ERR_OBJECT_NOT_FOUND))
        {
            //
            // In vGPU MIG, the GI does not own all possible engine instances,
            // so engine list search returns NV_ERR_OBJECT_NOT_FOUND.
            //
            return;
        }
        else
        {
            NV_ASSERT_OR_RETURN_VOID(status == NV_OK);
        }

        NV_PRINTF(LEVEL_INFO, "Registering 0x%x/0x%x to handle nonstall intr\n", pKernelFalcon->physEngDesc, mcIdx);

        NV_ASSERT(pRecords[mcIdx].pNotificationService == NULL);
        pRecords[mcIdx].bFifoWaiveNotify = NV_FALSE;
        pRecords[mcIdx].pNotificationService = staticCast(pGenericKernelFalcon, IntrService);
    }
}

NV_STATUS gkflcnServiceNotificationInterrupt_IMPL(OBJGPU *pGpu, GenericKernelFalcon *pGenericKernelFalcon, IntrServiceServiceNotificationInterruptArguments *pParams)
{
    NvU32 idxMc = pParams->engineIdx;
    RM_ENGINE_TYPE rmEngineType = RM_ENGINE_TYPE_NULL;

    NV_PRINTF(LEVEL_INFO, "nonstall intr for MC 0x%x\n", idxMc);

    if (MC_ENGINE_IDX_NVDECn(0) <= idxMc &&
        idxMc < MC_ENGINE_IDX_NVDECn(RM_ENGINE_TYPE_NVDEC_SIZE))
    {
        NvU32 nvdecIdx = idxMc - MC_ENGINE_IDX_NVDECn(0);
        rmEngineType = RM_ENGINE_TYPE_NVDEC(nvdecIdx);
    }
    else if (MC_ENGINE_IDX_OFA(0) <= idxMc &&
             idxMc < MC_ENGINE_IDX_OFA(RM_ENGINE_TYPE_OFA_SIZE))
    {
        NvU32 ofaIdx = idxMc - MC_ENGINE_IDX_OFA(0);
        rmEngineType = RM_ENGINE_TYPE_OFA(ofaIdx);
    }
    else if (MC_ENGINE_IDX_NVJPEGn(0) <= idxMc &&
             idxMc < MC_ENGINE_IDX_NVJPEGn(RM_ENGINE_TYPE_NVJPEG_SIZE))
    {
        NvU32 nvjpgIdx = idxMc - MC_ENGINE_IDX_NVJPEGn(0);
        rmEngineType = RM_ENGINE_TYPE_NVJPEG(nvjpgIdx);
    }
    else if (MC_ENGINE_IDX_NVENCn(0) <= idxMc &&
             idxMc < MC_ENGINE_IDX_NVENCn(RM_ENGINE_TYPE_NVENC_SIZE))
    {
        NvU32 msencIdx = idxMc - MC_ENGINE_IDX_NVENCn(0);
        rmEngineType = RM_ENGINE_TYPE_NVENC(msencIdx);
    }
    else if (idxMc == MC_ENGINE_IDX_SEC2)
    {
        rmEngineType = RM_ENGINE_TYPE_SEC2;
    }

    NV_ASSERT_OR_RETURN(rmEngineType != RM_ENGINE_TYPE_NULL, NV_ERR_INVALID_STATE);

    // Wake up channels waiting on this event
    engineNonStallIntrNotify(pGpu, rmEngineType);

    return NV_OK;
}
