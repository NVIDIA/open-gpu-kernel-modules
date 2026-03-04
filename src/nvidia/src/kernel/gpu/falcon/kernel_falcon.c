/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/falcon/kernel_falcon_core_dump.h"
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

NvU32 kflcnGetPendingHostInterrupts(OBJGPU *pGpu, KernelFalcon *pKernelFalcon)
{
    if (kflcnIsRiscvMode(pGpu, pKernelFalcon))
        return kflcnRiscvReadIntrStatus(pGpu, pKernelFalcon);
    else
        return kflcnReadIntrStatus(pGpu, pKernelFalcon);
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
            FMT_CHANNEL_DEBUG_TAG " does not have a falcon engine instance for engDesc=0x%x\n",
            kchannelGetDebugTag(pKernelChannel),
            pKernelFalcon->physEngDesc);
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

NV_STATUS kflcnCoreDumpNondestructive(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 verbosity
)
{
    CoreDumpRegs PeregrineCoreRegisters = { 0 };

    kflcnDumpCoreRegs_HAL(pGpu, pKernelFlcn, &PeregrineCoreRegisters);
    if (verbosity >= 1)
    {
        kflcnDumpPeripheralRegs_HAL(pGpu, pKernelFlcn, &PeregrineCoreRegisters);
    }
    if (verbosity >= 2)
    {
        kflcnDumpTracepc(pGpu, pKernelFlcn, &PeregrineCoreRegisters);
    }

    NV_PRINTF(LEVEL_ERROR, "PRI: riscvPc               : %08x\n", PeregrineCoreRegisters.riscvPc);
    if (verbosity >= 1)
    {
        NV_PRINTF(LEVEL_ERROR, "PRI: riscvCpuctl           : %08x\n", PeregrineCoreRegisters.riscvCpuctl);
        NV_PRINTF(LEVEL_ERROR, "PRI: riscvIrqmask          : %08x\n", PeregrineCoreRegisters.riscvIrqmask);
        NV_PRINTF(LEVEL_ERROR, "PRI: riscvIrqdest          : %08x\n", PeregrineCoreRegisters.riscvIrqdest);
        NV_PRINTF(LEVEL_ERROR, "PRI: riscvPrivErrStat      : %08x\n", PeregrineCoreRegisters.riscvPrivErrStat);
        NV_PRINTF(LEVEL_ERROR, "PRI: riscvPrivErrInfo      : %08x\n", PeregrineCoreRegisters.riscvPrivErrInfo);
        NV_PRINTF(LEVEL_ERROR, "PRI: riscvPrivErrAddr      : %016" NvU64_fmtx "\n", (((NvU64)PeregrineCoreRegisters.riscvPrivErrAddrH << 32ull) | PeregrineCoreRegisters.riscvPrivErrAddrL));
        NV_PRINTF(LEVEL_ERROR, "PRI: riscvHubErrStat       : %08x\n", PeregrineCoreRegisters.riscvHubErrStat);
        NV_PRINTF(LEVEL_ERROR, "PRI: falconMailbox         : 0:%08x 1:%08x\n", PeregrineCoreRegisters.falconMailbox[0], PeregrineCoreRegisters.falconMailbox[1]);
        NV_PRINTF(LEVEL_ERROR, "PRI: falconIrqstat         : %08x\n", PeregrineCoreRegisters.falconIrqstat);
        NV_PRINTF(LEVEL_ERROR, "PRI: falconIrqmode         : %08x\n", PeregrineCoreRegisters.falconIrqmode);
        NV_PRINTF(LEVEL_ERROR, "PRI: fbifInstblk           : %08x\n", PeregrineCoreRegisters.fbifInstblk);
        NV_PRINTF(LEVEL_ERROR, "PRI: fbifCtl               : %08x\n", PeregrineCoreRegisters.fbifCtl);
        NV_PRINTF(LEVEL_ERROR, "PRI: fbifThrottle          : %08x\n", PeregrineCoreRegisters.fbifThrottle);
        NV_PRINTF(LEVEL_ERROR, "PRI: fbifAchkBlk           : 0:%08x 1:%08x\n", PeregrineCoreRegisters.fbifAchkBlk[0], PeregrineCoreRegisters.fbifAchkBlk[1]);
        NV_PRINTF(LEVEL_ERROR, "PRI: fbifAchkCtl           : 0:%08x 1:%08x\n", PeregrineCoreRegisters.fbifAchkCtl[0], PeregrineCoreRegisters.fbifAchkCtl[1]);
        NV_PRINTF(LEVEL_ERROR, "PRI: fbifCg1               : %08x\n", PeregrineCoreRegisters.fbifCg1);
    }
    if (verbosity >= 2)
    {
        for (unsigned int n = 0; n < PeregrineCoreRegisters.tracePCEntries; n++)
        {
            NV_PRINTF(LEVEL_ERROR, "TRACE: %02u = 0x%016" NvU64_fmtx "\n", n, PeregrineCoreRegisters.tracePC[n]);
        }
    }

    return NV_OK;
}

NV_STATUS kflcnCoreDumpDestructive(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    // Initialise state - nothing succeeded yet.
    NvU64 pc = 1;
    NvU64 traceRa = 0;
    NvU64 traceS0 = 0;
    NvU32 unwindDepth = 0;
    NvU64 regValue64;
    NvU64 riscvCoreRegisters[32];
    NvU32 anySuccess = 0;

    // Check if PRI is alive / core is booted.
    {
        if (kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn)) // If core is not booted, abort - nothing to do.
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: Core is booted.\n");
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: [ERROR] Core is not booted.\n");
            return NV_OK;
        }
    }

    // Check if ICD RSTAT works.
    {
        for (int i = 0; i < 8; i++)
        {
            if (kflcnRiscvIcdRstat_HAL(pGpu, pKernelFlcn, i, &regValue64) == NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "ICD: RSTAT%d 0x%016" NvU64_fmtx "\n", i, regValue64);
                anySuccess++;
            }
        }
        if (!anySuccess)
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: [ERROR] Unable to retrieve any RSTAT register.\n");
            return NV_OK; // Failed to read ANY RSTAT value. This means ICD is dead.
        }
    }

    // ATTEMPT ICD HALT, and dump state. Check if ICD commands work.
    {
        if (kflcnRiscvIcdHalt_HAL(pGpu, pKernelFlcn) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: [ERROR] ICD Halt command failed.\n");
            return NV_OK; // Failed to halt core. Typical end point for "core is hung" scenario.
        }
    }

    // Dump PC, as much as we can get.
    if (kflcnRiscvIcdRpc_HAL(pGpu, pKernelFlcn, &pc) != NV_OK)
    {
        if (kflcnCoreDumpPc_HAL(pGpu, pKernelFlcn, &pc) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: [WARN] Cannot retrieve PC.\n");
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: PC = 0x--------%08llx\n", pc & 0xffffffff);
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "ICD: PC = 0x%016" NvU64_fmtx "\n", pc);
    }

    // Dump registers
    for (int a = 0; a < 32; a++)
    {
        if (kflcnRiscvIcdReadReg_HAL(pGpu, pKernelFlcn, a, &regValue64) == NV_OK)
        {
            riscvCoreRegisters[a] = regValue64;

            // Save off registers needed for unwinding.
            if (a == 1)
                traceRa = regValue64;
            if (a == 8)
                traceS0 = regValue64;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: register read failed for x%02d\n", a);
            riscvCoreRegisters[a] = 0xbaadbaadbaadbaad;
        }
    }
    NV_PRINTF(LEVEL_ERROR,
        "ICD: ra:0x%016" NvU64_fmtx "   sp:0x%016" NvU64_fmtx "   gp:0x%016" NvU64_fmtx "   tp:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[1], riscvCoreRegisters[2], riscvCoreRegisters[3], riscvCoreRegisters[4]);
    NV_PRINTF(LEVEL_ERROR,
        "ICD: a0:0x%016" NvU64_fmtx "   a1:0x%016" NvU64_fmtx "   a2:0x%016" NvU64_fmtx "   a3:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[5], riscvCoreRegisters[6], riscvCoreRegisters[7], riscvCoreRegisters[8]);
    NV_PRINTF(LEVEL_ERROR,
        "ICD: a4:0x%016" NvU64_fmtx "   a5:0x%016" NvU64_fmtx "   a6:0x%016" NvU64_fmtx "   a7:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[9], riscvCoreRegisters[10], riscvCoreRegisters[11], riscvCoreRegisters[12]);
    NV_PRINTF(LEVEL_ERROR,
        "ICD: s0:0x%016" NvU64_fmtx "   s1:0x%016" NvU64_fmtx "   s2:0x%016" NvU64_fmtx "   s3:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[13], riscvCoreRegisters[14], riscvCoreRegisters[15], riscvCoreRegisters[16]);
    NV_PRINTF(LEVEL_ERROR,
        "ICD: s4:0x%016" NvU64_fmtx "   s5:0x%016" NvU64_fmtx "   s6:0x%016" NvU64_fmtx "   s7:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[17], riscvCoreRegisters[18], riscvCoreRegisters[19], riscvCoreRegisters[20]);
    NV_PRINTF(LEVEL_ERROR,
        "ICD: s8:0x%016" NvU64_fmtx "   s9:0x%016" NvU64_fmtx "  s10:0x%016" NvU64_fmtx "  s11:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[21], riscvCoreRegisters[22], riscvCoreRegisters[23], riscvCoreRegisters[24]);
    NV_PRINTF(LEVEL_ERROR,
        "ICD: t0:0x%016" NvU64_fmtx "   t1:0x%016" NvU64_fmtx "   t2:0x%016" NvU64_fmtx "   t3:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[25], riscvCoreRegisters[26], riscvCoreRegisters[27], riscvCoreRegisters[28]);
    NV_PRINTF(LEVEL_ERROR,
        "ICD: t4:0x%016" NvU64_fmtx "   t5:0x%016" NvU64_fmtx "   t6:0x%016" NvU64_fmtx "\n",
        riscvCoreRegisters[29], riscvCoreRegisters[30], riscvCoreRegisters[31]);

    // Dump CSRs
    for (int a = 0; a < 4096; a++)
    {
        if (kflcnRiscvIcdRcsr_HAL(pGpu, pKernelFlcn, a, &regValue64) == NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "ICD: csr[%03x] = 0x%016" NvU64_fmtx "\n", a, regValue64);
        }
    }

    //
    // Attempt core unwind. For various reasons, may fail very early.
    // To unwind, we use s0 as the frame pointer and ra as the return address (adding that to the callstack).
    // s0[-2] contains the previous stack pointer, and s0[-1] contains the previous return address.
    // We continue until the memory is not readable, or we hit some "very definitely wrong" values like zero or
    // misaligned stack. If we unwind even once, we declare our unwind a great success and move on.
    //
    {
        if ((!traceRa) || (!traceS0))
            return NV_OK; // Fail to unwind - the ra/s0 registers are not valid.

        do
        {
            if ((!traceS0) ||  // S0 cannot be zero
                (!traceRa) ||  // RA cannot be zero
                (traceS0 & 7)) // stack cannot be misaligned
                goto abortUnwind;

            traceS0 -= 16;
            if (kflcnRiscvIcdReadMem_HAL(pGpu, pKernelFlcn, traceS0 + 8, 8, &traceRa) != NV_OK)
                goto abortUnwind;
            if (kflcnRiscvIcdReadMem_HAL(pGpu, pKernelFlcn, traceS0 + 0, 8, &traceS0) != NV_OK)
                goto abortUnwind;

            NV_PRINTF(LEVEL_ERROR, "ICD: unwind%02u: 0x%016" NvU64_fmtx "\n", unwindDepth, traceRa);
            unwindDepth++;
        } while (unwindDepth < __RISCV_MAX_UNWIND_DEPTH);

        // Core unwind attempt finished. The call stack was too deep.
        NV_PRINTF(LEVEL_ERROR, "ICD: [WARN] unwind greater than max depth...\n");
        goto unwindFull;
    }
abortUnwind:
    // Core unwind attempt finished. No unwind past the register (ra) was possible.
    if (unwindDepth == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "ICD: [WARN] unwind retrieved zero values :(\n");
        return NV_OK;
    }

    // Core unwind attempt finished. Unwind successfully got 1 or more entries.
unwindFull:
    NV_PRINTF(LEVEL_ERROR, "ICD: unwind complete.\n");
    return NV_OK;
}
