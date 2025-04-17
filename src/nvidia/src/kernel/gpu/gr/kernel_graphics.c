/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_GRAPHICS_H_PRIVATE_ACCESS_ALLOWED
#define NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED


#include "rmconfig.h"

#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/gpu_user_shared_data.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/rmapi/rmapi_utils.h"
#include "kernel/core/locks.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/mem_mgr/gpu_vaspace.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "nvrm_registry.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mmu/kern_gmmu.h"
#include "platform/sli/sli.h"
#include "rmapi/rs_utils.h"
#include "rmapi/client.h"

#include "vgpu/vgpu_events.h"
#include "vgpu/rpc.h"

#include "class/cla06f.h"
#include "class/cl90f1.h" // FERMI_VASPACE_A
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl50a0.h" // NV50_MEMORY_VIRTUAL
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/clc36f.h" // VOLTA_CHANNEL_GPFIFO_A
#include "class/clc46f.h" // TURING_CHANNEL_GPFIFO_A
#include "class/clc56f.h" // AMPERE_CHANNEL_GPFIFO_A
#include "class/clc86f.h" // HOPPER_CHANNEL_GPFIFO_A
#include "class/clc96f.h" // BLACKWELL_CHANNEL_GPFIFO_A
#include "class/clca6f.h" // BLACKWELL_CHANNEL_GPFIFO_B
#include "class/clc637.h"
#include "class/clc638.h"

//
// We use NV2080_CTRL_INTERNAL_GR_MAX_GPC to statically allocate certain
// GPC related array in ctrl call header file. We will need to adjust
// NV2080_CTRL_INTERNAL_GR_MAX_GPC if some day KGRMGR_MAX_GPC gets changed
//
ct_assert(NV2080_CTRL_INTERNAL_GR_MAX_GPC == KGRMGR_MAX_GPC);

//
// Ensure the external and internal ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT
// will always in sync
//
ct_assert(NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT ==
          NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT);

typedef struct KGRAPHICS_PRIVATE_DATA
{
    NvBool bInitialized;
    KGRAPHICS_STATIC_INFO staticInfo;
} KGRAPHICS_PRIVATE_DATA;
static NV_STATUS _kgraphicsMapGlobalCtxBuffer(OBJGPU *pGpu, KernelGraphics *pKernelGraphics, NvU32 gfid, OBJVASPACE *,
                                       KernelGraphicsContext *, GR_GLOBALCTX_BUFFER, NvBool bIsReadOnly);
static NV_STATUS _kgraphicsPostSchedulingEnableHandler(OBJGPU *, void *);

static void
_kgraphicsInitRegistryOverrides(OBJGPU *pGpu, KernelGraphics *pKernelGraphics)
{
    {
        NvU32 data;

        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_FORCE_GR_SCRUBBER_CHANNEL, &data) == NV_OK)
        {
            if (data == NV_REG_STR_RM_FORCE_GR_SCRUBBER_CHANNEL_DISABLE)
            {
                kgraphicsSetBug4208224WAREnabled(pGpu, pKernelGraphics, NV_FALSE);
            }
            else if (data == NV_REG_STR_RM_FORCE_GR_SCRUBBER_CHANNEL_ENABLE)
            {
                kgraphicsSetBug4208224WAREnabled(pGpu, pKernelGraphics, NV_TRUE);
            }
        }
    }
    return;
}

NV_STATUS
kgraphicsConstructEngine_IMPL
(
    OBJGPU         *pGpu,
    KernelGraphics *pKernelGraphics,
    ENGDESCRIPTOR   engDesc
)
{
    KGRAPHICS_PRIVATE_DATA *pPrivate;
    NvU32 idx;
    GR_GLOBALCTX_BUFFER buf;
    GR_CTX_BUFFER localBuf;

    pKernelGraphics->instance = ENGDESC_FIELD(engDesc, _INST);

    pPrivate = portMemAllocNonPaged(sizeof(*pPrivate));
    if (pPrivate == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pPrivate, 0, sizeof(*pPrivate));
    pKernelGraphics->pPrivate = pPrivate;

    // All local ctx buffers default to uncached FB preferred
    FOR_EACH_IN_ENUM(GR_CTX_BUFFER, localBuf)
    {
        pKernelGraphics->ctxAttr[localBuf].pAllocList = ADDRLIST_FBMEM_PREFERRED;
        pKernelGraphics->ctxAttr[localBuf].cpuAttr = NV_MEMORY_UNCACHED;
    }
    FOR_EACH_IN_ENUM_END;

    // Process instloc overrides
    {
        struct
        {
            GR_CTX_BUFFER buf;
            NvU32 override;
        } instlocOverrides[] =
        {
            { GR_CTX_BUFFER_MAIN,      DRF_VAL(_REG_STR_RM, _INST_LOC,   _GRCTX, pGpu->instLocOverrides) },
            { GR_CTX_BUFFER_PATCH,     DRF_VAL(_REG_STR_RM, _INST_LOC_2, _CTX_PATCH, pGpu->instLocOverrides2) },
            { GR_CTX_BUFFER_ZCULL,     DRF_VAL(_REG_STR_RM, _INST_LOC_2, _ZCULLCTX, pGpu->instLocOverrides2) },
            { GR_CTX_BUFFER_PM,        DRF_VAL(_REG_STR_RM, _INST_LOC_2, _PMCTX, pGpu->instLocOverrides2) },
            { GR_CTX_BUFFER_PREEMPT,   DRF_VAL(_REG_STR_RM, _INST_LOC_3, _PREEMPT_BUFFER, pGpu->instLocOverrides3) },
            { GR_CTX_BUFFER_BETA_CB,   DRF_VAL(_REG_STR_RM, _INST_LOC_3, _GFXP_BETACB_BUFFER, pGpu->instLocOverrides3) },
            { GR_CTX_BUFFER_PAGEPOOL,  DRF_VAL(_REG_STR_RM, _INST_LOC_3, _GFXP_PAGEPOOL_BUFFER, pGpu->instLocOverrides3) },
            { GR_CTX_BUFFER_SPILL,     DRF_VAL(_REG_STR_RM, _INST_LOC_3, _GFXP_SPILL_BUFFER, pGpu->instLocOverrides3) },
            { GR_CTX_BUFFER_RTV_CB,    DRF_VAL(_REG_STR_RM, _INST_LOC_3, _GFXP_RTVCB_BUFFER, pGpu->instLocOverrides3) },
            { GR_CTX_BUFFER_SETUP,     DRF_VAL(_REG_STR_RM, _INST_LOC_4, _GFXP_SETUP_BUFFER, pGpu->instLocOverrides3) } // update to instLocOverrides4 when changes are in
        };

        for (idx = 0; idx < NV_ARRAY_ELEMENTS(instlocOverrides); ++idx)
        {
            memdescOverrideInstLocList(instlocOverrides[idx].override,
                                       NV_ENUM_TO_STRING(GR_CTX_BUFFER, instlocOverrides[idx].buf),
                                       &pKernelGraphics->ctxAttr[instlocOverrides[idx].buf].pAllocList,
                                       &pKernelGraphics->ctxAttr[instlocOverrides[idx].buf].cpuAttr);
        }
    }

    // Most global ctx buffers default to uncached FB preferred
    FOR_EACH_IN_ENUM(GR_GLOBALCTX_BUFFER, buf)
    {
        pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[buf].pAllocList = ADDRLIST_FBMEM_PREFERRED;
        pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[buf].cpuAttr = NV_MEMORY_UNCACHED;
    }
    FOR_EACH_IN_ENUM_END;

    // FECS event buffer defaults to cached SYSMEM
    pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[GR_GLOBALCTX_BUFFER_FECS_EVENT].pAllocList = ADDRLIST_SYSMEM_ONLY;
    pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[GR_GLOBALCTX_BUFFER_FECS_EVENT].cpuAttr    = NV_MEMORY_CACHED;

    // Process instloc overrides
    {
        struct
        {
            GR_GLOBALCTX_BUFFER buf;
            NvU32 override;
        } instlocOverrides[] =
        {
            { GR_GLOBALCTX_BUFFER_FECS_EVENT,                   DRF_VAL(_REG_STR_RM, _INST_LOC_4, _FECS_EVENT_BUF, pGpu->instLocOverrides4) },
            { GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB,                 DRF_VAL(_REG_STR_RM, _INST_LOC_2, _ATTR_CB, pGpu->instLocOverrides2) },
            { GR_GLOBALCTX_BUFFER_BUNDLE_CB,                    DRF_VAL(_REG_STR_RM, _INST_LOC_2, _BUNDLE_CB, pGpu->instLocOverrides2) },
            { GR_GLOBALCTX_BUFFER_PAGEPOOL,                     DRF_VAL(_REG_STR_RM, _INST_LOC_2, _PAGEPOOL, pGpu->instLocOverrides2) },
            { GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP,              DRF_VAL(_REG_STR_RM, _INST_LOC_3, _PRIV_ACCESS_MAP, pGpu->instLocOverrides3) },
            { GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP, DRF_VAL(_REG_STR_RM, _INST_LOC_3, _PRIV_ACCESS_MAP, pGpu->instLocOverrides3) },
            { GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP,          DRF_VAL(_REG_STR_RM, _INST_LOC_3, _PRIV_ACCESS_MAP, pGpu->instLocOverrides3) },
            { GR_GLOBALCTX_BUFFER_RTV_CB,                       DRF_VAL(_REG_STR_RM, _INST_LOC_3, _RTVCB_BUFFER, pGpu->instLocOverrides3) }
        };

        for (idx = 0; idx < NV_ARRAY_ELEMENTS(instlocOverrides); ++idx)
        {
            memdescOverrideInstLocList(instlocOverrides[idx].override,
                                       NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, instlocOverrides[idx].buf),
                                       &pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[instlocOverrides[idx].buf].pAllocList,
                                       &pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[instlocOverrides[idx].buf].cpuAttr);
        }
    }

    // Copy final global buffer attributes for local versions
    FOR_EACH_IN_ENUM(GR_GLOBALCTX_BUFFER, buf)
    {
        // Host RM cannot allocate system memory on behalf of the VF RM, so force FB.
        pKernelGraphics->globalCtxBuffersInfo.vfGlobalCtxAttr[buf].pAllocList = ADDRLIST_FBMEM_ONLY;
        pKernelGraphics->globalCtxBuffersInfo.vfGlobalCtxAttr[buf].cpuAttr = NV_MEMORY_UNCACHED;

        // Local context allocation
        pKernelGraphics->globalCtxBuffersInfo.localCtxAttr[buf] = pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[buf];
    }
    FOR_EACH_IN_ENUM_END;

    //
    // Default context buffers to non size aligned. The attribute buffer is
    // required to be mapped with an offset naturally aligned to the size.
    //
    for (idx = 0; idx < GR_GLOBALCTX_BUFFER_COUNT; idx++)
        pKernelGraphics->globalCtxBuffersInfo.bSizeAligned[idx] = NV_FALSE;
    pKernelGraphics->globalCtxBuffersInfo.bSizeAligned[GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB] = NV_TRUE;

    NV_ASSERT_OK_OR_RETURN(fecsCtxswLoggingInit(pGpu, pKernelGraphics, &pKernelGraphics->pFecsTraceInfo));

    _kgraphicsInitRegistryOverrides(pGpu, pKernelGraphics);
    return NV_OK;
}

void
kgraphicsDestruct_IMPL
(
    KernelGraphics *pKernelGraphics
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelGraphics);

    fecsCtxswLoggingTeardown(pGpu, pKernelGraphics);
    pKernelGraphics->pFecsTraceInfo = NULL;
    kgraphicsInvalidateStaticInfo(pGpu, pKernelGraphics);

    portMemFree(pKernelGraphics->pPrivate);
    pKernelGraphics->pPrivate = NULL;
}

NV_STATUS
kgraphicsStateInitLocked_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    NvU32 nGlobalCtx = 1;
    NvU32 numClasses;

    NV_ASSERT_OK_OR_RETURN(gpuGetClassList(pGpu, &numClasses, NULL, ENG_GR(pKernelGraphics->instance)));

    //
    // Number of supported class can be zero when Graphics engine is disabled, but we still
    // need those classes in ClassDB for KernelGraphics engine operation, thus here we are adding
    // the ENG_GR(X) supported classes back to ClassDB
    //
    if (numClasses == 0)
    {
        GpuEngineOrder *pEngineOrder = &pGpu->engineOrder;
        const CLASSDESCRIPTOR *pClassDesc = &pEngineOrder->pClassDescriptors[0];
        NvU32 i;
        NvU32 classNum;

        for (i = 0; i < pEngineOrder->numClassDescriptors; i++)
        {
            classNum = pClassDesc[i].externalClassId;
            if (classNum == (NvU32)~0)
                continue;

            if (ENG_GR(pKernelGraphics->instance) == pClassDesc[i].engDesc)
            {
                NV_PRINTF(LEVEL_INFO, "Adding class ID 0x%x to ClassDB\n", classNum);
                NV_ASSERT_OK_OR_RETURN(
                    gpuAddClassToClassDBByEngTagClassId(pGpu, ENG_GR(pKernelGraphics->instance), classNum));
            }
        }
    }

    //
    // Allocate guest context db array
    //
    if (gpuIsSriovEnabled(pGpu))
    {
        nGlobalCtx = VMMU_MAX_GFID;
    }

    pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers = portMemAllocNonPaged(sizeof(*pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers) * nGlobalCtx);
    if (pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers, 0,
            sizeof(*pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers) * nGlobalCtx);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        kgraphicsSetBug4208224WAREnabled(pGpu, pKernelGraphics, NV_FALSE);
    }
    pKernelGraphics->bug4208224Info.hClient      = NV01_NULL_OBJECT;
    pKernelGraphics->bug4208224Info.hDeviceId    = NV01_NULL_OBJECT;
    pKernelGraphics->bug4208224Info.hSubdeviceId = NV01_NULL_OBJECT;
    pKernelGraphics->bug4208224Info.bConstructed = NV_FALSE;

    return NV_OK;
}

NV_STATUS
kgraphicsStateUnload_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 flags

)
{
    if (pKernelGraphics->instance != 0)
        return NV_OK;

    kfifoRemoveSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
                                 _kgraphicsPostSchedulingEnableHandler,
                                 (void *)((NvUPtr)(pKernelGraphics->instance)),
                                 NULL, NULL);

    return NV_OK;
}

NV_STATUS
kgraphicsStateLoad_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 flags
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    if (fecsGetCtxswLogConsumerCount(pGpu, pKernelGraphicsManager) > 0)
    {
        fecsBufferMap(pGpu, pKernelGraphics);
        fecsBufferReset(pGpu, pKernelGraphics);
    }

    if (pKernelGraphics->instance == 0)
    {
        //
        // GSP_CLIENT creates the golden context channel GR post load. However,
        // if PMA scrubber is enabled, a scrubber channel must be constructed
        // first as a part of Fifo post load. Hence, add the golden channel
        // creation as a fifo post-scheduling-enablement callback.
        //
        NV_ASSERT_OK_OR_RETURN(
            kfifoAddSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
                                      _kgraphicsPostSchedulingEnableHandler,
                                      (void *)((NvUPtr)(pKernelGraphics->instance)),
                                      NULL, NULL));
    }

    return NV_OK;
}

NV_STATUS
kgraphicsStatePreUnload_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 flags
)
{
    if (pKernelGraphics->bug4208224Info.bConstructed)
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS params = {0};

        params.bTeardown = NV_TRUE;
        NV_ASSERT_OK(pRmApi->Control(pRmApi,
                     pKernelGraphics->bug4208224Info.hClient,
                     pKernelGraphics->bug4208224Info.hSubdeviceId,
                     NV2080_CTRL_CMD_INTERNAL_KGR_INIT_BUG4208224_WAR,
                     &params,
                     sizeof(params)));
        NV_ASSERT_OK(pRmApi->Free(pRmApi, pKernelGraphics->bug4208224Info.hClient, pKernelGraphics->bug4208224Info.hClient));
        pKernelGraphics->bug4208224Info.bConstructed = NV_FALSE;
    }

    fecsBufferUnmap(pGpu, pKernelGraphics);

    // Release global buffers used as part of the gr context, when not in S/R
    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
        kgraphicsFreeGlobalCtxBuffers(pGpu, pKernelGraphics, GPU_GFID_PF);

    return NV_OK;
}

void
kgraphicsStateDestroy_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    fecsBufferTeardown(pGpu, pKernelGraphics);

    portMemFree(pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers);
    pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers = NULL;
}

NvBool kgraphicsIsPresent_IMPL
(
    OBJGPU         *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 unused;

    if (IsDFPGA(pGpu))
        return NV_FALSE;

    if (IS_MODS_AMODEL(pGpu))
        return NV_TRUE;

    return kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                    ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32)RM_ENGINE_TYPE_GR(pKernelGraphics->instance),
                                    ENGINE_INFO_TYPE_INVALID, &unused) == NV_OK;
}

NV_STATUS
kgraphicsStatePostLoad_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 flags
)
{
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgraphicsLoadStaticInfo(pGpu, pKernelGraphics, KMIGMGR_SWIZZID_INVALID));
    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);

    if ((!IS_VIRTUAL(pGpu)) &&
        (pKernelGraphicsStaticInfo != NULL) &&
        (pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL) &&
        (kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, GPU_GFID_PF)->memDesc[GR_GLOBALCTX_BUFFER_FECS_EVENT] == NULL))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsAllocGlobalCtxBuffers_HAL(pGpu, pKernelGraphics, GPU_GFID_PF));
    }

    return NV_OK;
}

/*!
 * @brief Create a golden image channel after Fifo post load
 * Instead of lazily waiting until first client request, we proactively create a
 * golden channel here.
 */
static NV_STATUS
_kgraphicsPostSchedulingEnableHandler
(
    OBJGPU *pGpu,
    void *pGrIndex
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, ((NvU32)(NvUPtr)pGrIndex));
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);


    // Nothing to do for non-GSPCLIENT
    if (!IS_GSP_CLIENT(pGpu) && !kgraphicsIsBug4208224WARNeeded_HAL(pGpu, pKernelGraphics))
        return NV_OK;

    // Defer golden context channel creation to GPU instance configuration
    if (IS_MIG_ENABLED(pGpu))
        return NV_OK;

    // Skip for MIG engines with 0 GPCs
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    if (pKernelGraphicsStaticInfo->floorsweepingMasks.gpcMask == 0x0)
        return NV_OK;

    if (memmgrIsPmaInitialized(pMemoryManager))
    {
        Heap *pHeap = GPU_GET_HEAP(pGpu);
        NvU32 pmaConfig = PMA_QUERY_SCRUB_ENABLED | PMA_QUERY_SCRUB_VALID;

        NV_ASSERT_OK_OR_RETURN(pmaQueryConfigs(&pHeap->pmaObject, &pmaConfig));

        //
        // Scrubber is also constructed from the same Fifo post scheduling
        // enable callback queue. This check enforces the dependency that
        // scrubber must be initialized first
        //
        if ((pmaConfig & PMA_QUERY_SCRUB_ENABLED) &&
            !(pmaConfig & PMA_QUERY_SCRUB_VALID))
        {
            return NV_WARN_MORE_PROCESSING_REQUIRED;
        }
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgraphicsCreateGoldenImageChannel(pGpu, pKernelGraphics));
    if (kgraphicsIsBug4208224WARNeeded_HAL(pGpu, pKernelGraphics))
    {
        return kgraphicsInitializeBug4208224WAR_HAL(pGpu, pKernelGraphics);
    }

    return NV_OK;
}

void
kgraphicsInvalidateStaticInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    portMemFree(pKernelGraphics->pPrivate->staticInfo.pGrInfo);
    pKernelGraphics->pPrivate->staticInfo.pGrInfo = NULL;

    portMemFree(pKernelGraphics->pPrivate->staticInfo.pPpcMasks);
    pKernelGraphics->pPrivate->staticInfo.pPpcMasks = NULL;

    portMemFree(pKernelGraphics->pPrivate->staticInfo.pZcullInfo);
    pKernelGraphics->pPrivate->staticInfo.pZcullInfo = NULL;

    portMemFree(pKernelGraphics->pPrivate->staticInfo.pRopInfo);
    pKernelGraphics->pPrivate->staticInfo.pRopInfo = NULL;

    portMemFree(pKernelGraphics->pPrivate->staticInfo.pContextBuffersInfo);
    pKernelGraphics->pPrivate->staticInfo.pContextBuffersInfo = NULL;

    portMemFree(pKernelGraphics->pPrivate->staticInfo.pSmIssueRateModifier);
    pKernelGraphics->pPrivate->staticInfo.pSmIssueRateModifier = NULL;

    portMemFree(pKernelGraphics->pPrivate->staticInfo.pSmIssueRateModifierV2);
    pKernelGraphics->pPrivate->staticInfo.pSmIssueRateModifierV2 = NULL;

    portMemFree(pKernelGraphics->pPrivate->staticInfo.pFecsTraceDefines);
    pKernelGraphics->pPrivate->staticInfo.pFecsTraceDefines = NULL;

    portMemSet(&pKernelGraphics->pPrivate->staticInfo, 0, sizeof(pKernelGraphics->pPrivate->staticInfo));
    pKernelGraphics->pPrivate->bInitialized = NV_FALSE;
}

const KGRAPHICS_STATIC_INFO *
kgraphicsGetStaticInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_PRIVATE_DATA *pPrivate = pKernelGraphics->pPrivate;
    return ((pPrivate != NULL) && pPrivate->bInitialized) ? &pPrivate->staticInfo : NULL;
}

static NV_STATUS
_kgraphicsInternalClientAlloc
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 swizzId,
    NvHandle *phClient,
    NvHandle *phDevice,
    NvHandle *phSubdevice
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL); //FIXME = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32 grIdx = pKernelGraphics->instance;

    NV_ASSERT_OR_RETURN(phClient != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(phDevice != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(phSubdevice != NULL, NV_ERR_INVALID_ARGUMENT);

    if (IS_MIG_IN_USE(pGpu))
    {
        NvHandle hSubscription;

        // Delay initialization to GPU instance configuration
        if (swizzId == KMIGMGR_SWIZZID_INVALID)
            return NV_ERR_NOT_READY;

        // With MIG enabled, we need to use a client subscribed to the correct GPU instance.
        NV_ASSERT_OK_OR_RETURN(
            rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, phClient, phDevice, phSubdevice));

        {
            NVC637_ALLOCATION_PARAMETERS params;
            NV_ASSERT_OK(
                serverutilGenResourceHandle(*phClient, &hSubscription));
            portMemSet(&params, 0, sizeof(params));
            params.swizzId = swizzId;
            NV_ASSERT_OK(
                pRmApi->AllocWithHandle(pRmApi, *phClient, *phSubdevice, hSubscription, AMPERE_SMC_PARTITION_REF, &params, sizeof(params)));
        }

    }
    else if (grIdx != 0)
    {
        // Static data is only defined for GR0 in legacy mode
        return NV_ERR_NOT_READY;
    }
    else
    {
        NV_ASSERT_OK_OR_RETURN(
            rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, phClient, phDevice, phSubdevice));
    }

    return NV_OK;
}

/*!
 * @brief Initialize static data that isn't collected right away
 */
NV_STATUS
kgraphicsInitializeDeferredStaticData_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvHandle hClient,
    NvHandle hSubdevice
)
{
    NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *pParams;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KGRAPHICS_PRIVATE_DATA *pPrivate = pKernelGraphics->pPrivate;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvU32 grIdx = pKernelGraphics->instance;
    NV_STATUS status = NV_OK;
    NvBool bInternalClientAllocated = NV_FALSE;
    NvU32 gfid;
    NvBool bCallingContextPlugin;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
    NV_ASSERT_OK_OR_RETURN(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin));

    if (bCallingContextPlugin)
    {
        gfid = GPU_GFID_PF;
    }

    //
    // Most of GR is stub'd so context related things are not needed in AMODEL.
    // But this function can be called in some MODS test, so return OK directly
    // to avoid failing the test.
    //
    if (IS_MODS_AMODEL(pGpu))
        return NV_OK;

    // Not ready
    if (!pPrivate->bInitialized)
        return NV_OK;

    // Already done
    if (pPrivate->staticInfo.pContextBuffersInfo != NULL)
        return NV_OK;

    // In progress
    if (pKernelGraphics->bCollectingDeferredStaticData)
        return NV_OK;

    if (hClient == NV01_NULL_OBJECT)
    {
        NvHandle hDevice = NV01_NULL_OBJECT;
        NvU32 swizzId = KMIGMGR_SWIZZID_INVALID;

        if (IS_MIG_IN_USE(pGpu))
        {
            MIG_INSTANCE_REF ref;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetMIGReferenceFromEngineType(pGpu, pKernelMIGManager,
                                                     RM_ENGINE_TYPE_GR(pKernelGraphics->instance), &ref));

            swizzId = ref.pKernelMIGGpuInstance->swizzId;
        }

        status = _kgraphicsInternalClientAlloc(pGpu, pKernelGraphics, swizzId, &hClient, &hDevice, &hSubdevice);
        if (status == NV_ERR_NOT_READY)
        {
            return NV_OK;
        }
        NV_ASSERT_OR_RETURN(status == NV_OK, status);
        NV_ASSERT_OR_RETURN(hClient != NV01_NULL_OBJECT, NV_ERR_INVALID_STATE);
        bInternalClientAllocated = NV_TRUE;
    }

    // Prevent recursion when deferred static data collection is ON
    pKernelGraphics->bCollectingDeferredStaticData = NV_TRUE;

    if (IS_MIG_IN_USE(pGpu))
    {
        MIG_INSTANCE_REF ref;
        RM_ENGINE_TYPE localRmEngineType;
        Subdevice *pSubdevice;
        RsClient *pClient;

        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            serverGetClientUnderLock(&g_resServ, hClient, &pClient),
            cleanup);

        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            subdeviceGetByHandle(pClient, hSubdevice, &pSubdevice),
            cleanup);

        // Physical RM will fill with local indices, so localize the index
        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                            GPU_RES_GET_DEVICE(pSubdevice),
                                            &ref),
            cleanup);
        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              RM_ENGINE_TYPE_GR(grIdx),
                                              &localRmEngineType),
            cleanup);
        grIdx = RM_ENGINE_TYPE_GR_IDX(localRmEngineType);
    }

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pParams, 0, sizeof(*pParams));
    NV_CHECK_OK_OR_GOTO(
        status,
        LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_CONTEXT_BUFFERS_INFO,
                        pParams,
                        sizeof(*pParams)),
        cleanup_context_buffers_info);

    pPrivate->staticInfo.pContextBuffersInfo =
        portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pContextBuffersInfo));

    if (pPrivate->staticInfo.pContextBuffersInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto cleanup_context_buffers_info;
    }

    portMemCopy(pPrivate->staticInfo.pContextBuffersInfo,
                sizeof(*pPrivate->staticInfo.pContextBuffersInfo),
                &pParams->engineContextBuffersInfo[grIdx],
                sizeof(pParams->engineContextBuffersInfo[grIdx]));

cleanup_context_buffers_info:
    portMemFree(pParams);

    //
    // We are not cleaning pContextBuffersInfo here since it's used after this
    // function so has to be cleaned after used.
    //

cleanup:
    if (bInternalClientAllocated)
    {
        pRmApi->Free(pRmApi, hClient, hClient);
    }

    pKernelGraphics->bCollectingDeferredStaticData = NV_FALSE;

    if (status == NV_OK)
    {
        //
        // Allocate Ctx Buffers that are global to all channels if they have yet
        // to be allocated. We delay them until now to save memory when runs
        // are done without using graphics contexts!
        //
        if (!pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid].bAllocated &&
            (!gpuIsClientRmAllocatedCtxBufferEnabled(pGpu) ||
             (gpuIsSriovEnabled(pGpu) && IS_GFID_PF(gfid))))
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kgraphicsAllocGrGlobalCtxBuffers_HAL(pGpu, pKernelGraphics, gfid, NULL));
        }
    }

    return status;
}

NV_STATUS
kgraphicsLoadStaticInfo_VF
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 swizzId
)
{
    KGRAPHICS_PRIVATE_DATA *pPrivate = pKernelGraphics->pPrivate;
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NvU32 grIdx = pKernelGraphics->instance;
    NVOS_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pPrivate != NULL, NV_ERR_INVALID_STATE);

    if (pPrivate->bInitialized)
        return status;

    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

        //
        // Delay initialization to GPU instance configuration, unless MODS is using
        // legacy VGPU mode, in which case the guest never receives a
        // configuration call
        //
        if ((swizzId == KMIGMGR_SWIZZID_INVALID) && !kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager))
            return status;

        pPrivate->staticInfo.pGrInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pGrInfo));
        if (pPrivate->staticInfo.pGrInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pGrInfo->infoList,
                    NV0080_CTRL_GR_INFO_MAX_SIZE * sizeof(*pPrivate->staticInfo.pGrInfo->infoList),
                    pVSI->grInfoParams.engineInfo[grIdx].infoList,
                    NV0080_CTRL_GR_INFO_MAX_SIZE * sizeof(*pVSI->grInfoParams.engineInfo[grIdx].infoList));

        portMemCopy(&pPrivate->staticInfo.globalSmOrder, sizeof(pPrivate->staticInfo.globalSmOrder),
                    &pVSI->globalSmOrder.globalSmOrder[grIdx], sizeof(pVSI->globalSmOrder.globalSmOrder[grIdx]));

        // grCaps are the same for all GR and can be copied from VGPU static info
        portMemCopy(pPrivate->staticInfo.grCaps.capsTbl, sizeof(pPrivate->staticInfo.grCaps.capsTbl),
                    pVSI->grCapsBits, sizeof(pVSI->grCapsBits));

        // Initialize PDB properties synchronized with physical RM
        pPrivate->staticInfo.pdbTable.bPerSubCtxheaderSupported = pVSI->bPerSubCtxheaderSupported;
        kgraphicsSetPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics, pPrivate->staticInfo.pdbTable.bPerSubCtxheaderSupported);

        pPrivate->staticInfo.pSmIssueRateModifier =
                portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pSmIssueRateModifier));
        if (pPrivate->staticInfo.pSmIssueRateModifier == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pSmIssueRateModifier, sizeof(*pPrivate->staticInfo.pSmIssueRateModifier),
                    &pVSI->smIssueRateModifier.smIssueRateModifier[grIdx], sizeof(pVSI->smIssueRateModifier.smIssueRateModifier[grIdx]));

        pPrivate->staticInfo.pPpcMasks = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pPpcMasks));
        if (pPrivate->staticInfo.pPpcMasks == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pPpcMasks, sizeof(*pPrivate->staticInfo.pPpcMasks),
                    &pVSI->ppcMaskParams.enginePpcMasks[grIdx], sizeof(pVSI->ppcMaskParams.enginePpcMasks[grIdx]));

        portMemCopy(&pPrivate->staticInfo.floorsweepingMasks, sizeof(pPrivate->staticInfo.floorsweepingMasks),
                    &pVSI->floorsweepMaskParams.floorsweepingMasks[grIdx], sizeof(pVSI->floorsweepMaskParams.floorsweepingMasks[grIdx]));

        pPrivate->staticInfo.pContextBuffersInfo =
            portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pContextBuffersInfo));

        if (pPrivate->staticInfo.pContextBuffersInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pContextBuffersInfo,
                    sizeof(*pPrivate->staticInfo.pContextBuffersInfo),
                    &pVSI->ctxBuffInfo.engineContextBuffersInfo[grIdx],
                    sizeof(pVSI->ctxBuffInfo.engineContextBuffersInfo[grIdx]));

        if (pKernelMIGManager->bIsSmgEnabled)
        {
           pPrivate->staticInfo.pZcullInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pZcullInfo));
            if (pPrivate->staticInfo.pZcullInfo == NULL)
            {
                status = NV_ERR_NO_MEMORY;
                goto cleanup;
            }
            portMemCopy(pPrivate->staticInfo.pZcullInfo, sizeof(*pPrivate->staticInfo.pZcullInfo),
                        &pVSI->zcullInfoParams.engineZcullInfo[grIdx], sizeof(pVSI->zcullInfoParams.engineZcullInfo[grIdx]));
        }

        pPrivate->staticInfo.fecsRecordSize.fecsRecordSize = pVSI->fecsRecordSize.fecsRecordSize[grIdx].fecsRecordSize;

        pPrivate->staticInfo.pFecsTraceDefines =
                portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pFecsTraceDefines));

        if (pPrivate->staticInfo.pFecsTraceDefines == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pFecsTraceDefines,
                    sizeof(*pPrivate->staticInfo.pFecsTraceDefines),
                    &pVSI->fecsTraceDefines.fecsTraceDefines[grIdx],
                    sizeof(pVSI->fecsTraceDefines.fecsTraceDefines[grIdx]));

        portMemCopy(&pPrivate->staticInfo.pdbTable, sizeof(pPrivate->staticInfo.pdbTable),
                    &pVSI->pdbTableParams.pdbTable[grIdx], sizeof(pVSI->pdbTableParams.pdbTable[grIdx]));
    }
    else if (grIdx == 0)
    {
        portMemCopy(pPrivate->staticInfo.grCaps.capsTbl, sizeof(pPrivate->staticInfo.grCaps.capsTbl),
                    pVSI->grCapsBits, sizeof(pVSI->grCapsBits));

        pPrivate->staticInfo.pGrInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pGrInfo));
        if (pPrivate->staticInfo.pGrInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pGrInfo->infoList,
                    NV0080_CTRL_GR_INFO_MAX_SIZE * sizeof(*pPrivate->staticInfo.pGrInfo->infoList),
                    pVSI->grInfoParams.engineInfo[grIdx].infoList,
                    NV0080_CTRL_GR_INFO_MAX_SIZE * sizeof(*pVSI->grInfoParams.engineInfo[grIdx].infoList));

        // Initialize PDB properties synchronized with physical RM
        pPrivate->staticInfo.pdbTable.bPerSubCtxheaderSupported = pVSI->bPerSubCtxheaderSupported;
        kgraphicsSetPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics, pPrivate->staticInfo.pdbTable.bPerSubCtxheaderSupported);

        portMemCopy(&pPrivate->staticInfo.globalSmOrder, sizeof(pPrivate->staticInfo.globalSmOrder),
                    &pVSI->globalSmOrder.globalSmOrder[grIdx], sizeof(pVSI->globalSmOrder.globalSmOrder[grIdx]));

        pPrivate->staticInfo.pSmIssueRateModifier =
                portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pSmIssueRateModifier));
        if (pPrivate->staticInfo.pSmIssueRateModifier == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pSmIssueRateModifier, sizeof(*pPrivate->staticInfo.pSmIssueRateModifier),
                    &pVSI->smIssueRateModifier.smIssueRateModifier[grIdx], sizeof(pVSI->smIssueRateModifier.smIssueRateModifier[grIdx]));

        pPrivate->staticInfo.pPpcMasks = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pPpcMasks));
        if (pPrivate->staticInfo.pPpcMasks == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pPpcMasks, sizeof(*pPrivate->staticInfo.pPpcMasks),
                    &pVSI->ppcMaskParams.enginePpcMasks[grIdx], sizeof(pVSI->ppcMaskParams.enginePpcMasks[grIdx]));

        pPrivate->staticInfo.pContextBuffersInfo =
            portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pContextBuffersInfo));

        if (pPrivate->staticInfo.pContextBuffersInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pContextBuffersInfo,
                    sizeof(*pPrivate->staticInfo.pContextBuffersInfo),
                    &pVSI->ctxBuffInfo.engineContextBuffersInfo[grIdx],
                    sizeof(pVSI->ctxBuffInfo.engineContextBuffersInfo[grIdx]));

        portMemCopy(&pPrivate->staticInfo.floorsweepingMasks, sizeof(pPrivate->staticInfo.floorsweepingMasks),
                    &pVSI->floorsweepMaskParams.floorsweepingMasks[grIdx], sizeof(pVSI->floorsweepMaskParams.floorsweepingMasks[grIdx]));

        pPrivate->staticInfo.pRopInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pRopInfo));
        if (pPrivate->staticInfo.pRopInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pRopInfo, sizeof(*pPrivate->staticInfo.pRopInfo),
                    &pVSI->ropInfoParams.engineRopInfo[grIdx], sizeof(pVSI->ropInfoParams.engineRopInfo[grIdx]));

        pPrivate->staticInfo.pZcullInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pZcullInfo));
        if (pPrivate->staticInfo.pZcullInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pZcullInfo, sizeof(*pPrivate->staticInfo.pZcullInfo),
                    &pVSI->zcullInfoParams.engineZcullInfo[grIdx], sizeof(pVSI->zcullInfoParams.engineZcullInfo[grIdx]));

        pPrivate->staticInfo.fecsRecordSize.fecsRecordSize = pVSI->fecsRecordSize.fecsRecordSize[grIdx].fecsRecordSize;

        pPrivate->staticInfo.pFecsTraceDefines =
                portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pFecsTraceDefines));
        if (pPrivate->staticInfo.pFecsTraceDefines == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pFecsTraceDefines,
                    sizeof(*pPrivate->staticInfo.pFecsTraceDefines),
                    &pVSI->fecsTraceDefines.fecsTraceDefines[grIdx],
                    sizeof(pVSI->fecsTraceDefines.fecsTraceDefines[grIdx]));

        portMemCopy(&pPrivate->staticInfo.pdbTable, sizeof(pPrivate->staticInfo.pdbTable),
                    &pVSI->pdbTableParams.pdbTable[grIdx], sizeof(pVSI->pdbTableParams.pdbTable[grIdx]));
    }
    else
    {
        // if MIG disabled, only GR0 static data needs to be published
        return status;
    }

    if (status == NV_OK)
    {
        // Publish static configuration
        pPrivate->bInitialized = NV_TRUE;
    }

    if (!IS_MIG_IN_USE(pGpu) && (grIdx == 0))
    {
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

        // Cache legacy GR mask info (i.e. GR0 with MIG disabled) to pKernelGraphicsManager->legacyFsMaskState
        kgrmgrSetLegacyKgraphicsStaticInfo(pGpu, pKernelGraphicsManager, pKernelGraphics);
    }

    // FECS ctxsw logging is consumed when profiling support is available in guest
    if (!pVSI->vgpuStaticProperties.bProfilingTracingEnabled)
    {
        kgraphicsSetCtxswLoggingSupported(pGpu, pKernelGraphics, NV_FALSE);
        NV_PRINTF(LEVEL_NOTICE, "Profiling support not requested. Disabling ctxsw logging\n");
    }

cleanup :

    if (status != NV_OK)
    {
        portMemFree(pPrivate->staticInfo.pGrInfo);
        pPrivate->staticInfo.pGrInfo = NULL;

        portMemFree(pPrivate->staticInfo.pPpcMasks);
        pPrivate->staticInfo.pPpcMasks = NULL;

        portMemFree(pPrivate->staticInfo.pZcullInfo);
        pPrivate->staticInfo.pZcullInfo = NULL;

        portMemFree(pPrivate->staticInfo.pRopInfo);
        pPrivate->staticInfo.pRopInfo = NULL;

        portMemFree(pPrivate->staticInfo.pContextBuffersInfo);
        pPrivate->staticInfo.pContextBuffersInfo = NULL;

        portMemFree(pPrivate->staticInfo.pSmIssueRateModifier);
        pPrivate->staticInfo.pSmIssueRateModifier = NULL;

        portMemFree(pPrivate->staticInfo.pFecsTraceDefines);
        pPrivate->staticInfo.pFecsTraceDefines = NULL;
    }

    return status;
}

NV_STATUS
kgraphicsLoadStaticInfo_KERNEL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 swizzId
)
{
    KGRAPHICS_PRIVATE_DATA *pPrivate = pKernelGraphics->pPrivate;
    NvHandle hClient = NV01_NULL_OBJECT;
    NvHandle hDevice;
    NvHandle hSubdevice;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvU32 grIdx = pKernelGraphics->instance;
    NV_STATUS status = NV_OK;
    NvBool bBcState = gpumgrGetBcEnabledStatus(pGpu);
    union
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS                      caps;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS                      info;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS           globalSmOrder;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS       floorsweepingMasks;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS                 ppcMasks;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS                zcullInfo;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS                  ropInfo;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS    smIssueRateModifier;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS smIssueRateModifierV2;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS          fecsRecordSize;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS        fecsTraceDefines;
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS            pdbProperties;
    } *pParams = NULL;

    NV_ASSERT_OR_RETURN(pPrivate != NULL, NV_ERR_INVALID_STATE);

    if (pPrivate->bInitialized)
        return NV_OK;

    status = _kgraphicsInternalClientAlloc(pGpu, pKernelGraphics, swizzId, &hClient, &hDevice, &hSubdevice);

    if (status == NV_ERR_NOT_READY)
    {
        return NV_OK;
    }
    NV_ASSERT_OR_RETURN(status == NV_OK, status);
    NV_ASSERT_OR_RETURN(hClient != NV01_NULL_OBJECT, NV_ERR_INVALID_STATE);

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto cleanup;
    }

    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;
        RM_ENGINE_TYPE localRmEngineType;
        RsClient *pClient;
        Device *pDevice;

        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            serverGetClientUnderLock(&g_resServ, hClient, &pClient),
            cleanup);

        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            deviceGetByHandle(pClient, hDevice, &pDevice),
            cleanup);

        // Physical RM will fill with local indices, so localize the index
        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref),
            cleanup);
        NV_CHECK_OK_OR_GOTO(
            status,
            LEVEL_ERROR,
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              RM_ENGINE_TYPE_GR(grIdx),
                                              &localRmEngineType),
            cleanup);
        grIdx = RM_ENGINE_TYPE_GR_IDX(localRmEngineType);
    }

    // GR Caps
    portMemSet(pParams, 0, sizeof(*pParams));
    NV_CHECK_OK_OR_GOTO(
        status,
        LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_CAPS,
                        pParams,
                        sizeof(pParams->caps)),
        cleanup);

    portMemCopy(&pPrivate->staticInfo.grCaps, sizeof(pPrivate->staticInfo.grCaps),
                &pParams->caps.engineCaps[grIdx], sizeof(pParams->caps.engineCaps[grIdx]));

    // GR Info
    portMemSet(pParams, 0, sizeof(*pParams));
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_INFO,
                             pParams,
                             sizeof(pParams->info));

    if (status == NV_OK)
    {
        pPrivate->staticInfo.pGrInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pGrInfo));
        if (pPrivate->staticInfo.pGrInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pGrInfo->infoList,
                    NV0080_CTRL_GR_INFO_MAX_SIZE * sizeof(*pPrivate->staticInfo.pGrInfo->infoList),
                    pParams->info.engineInfo[grIdx].infoList,
                    NV0080_CTRL_GR_INFO_MAX_SIZE * sizeof(*pParams->info.engineInfo[grIdx].infoList));

    }

    // Floorsweeping masks
    portMemSet(pParams, 0, sizeof(*pParams));
    NV_CHECK_OK_OR_GOTO(
        status,
        LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FLOORSWEEPING_MASKS,
                        pParams,
                        sizeof(pParams->floorsweepingMasks)),
        cleanup);

    portMemCopy(&pPrivate->staticInfo.floorsweepingMasks, sizeof(pPrivate->staticInfo.floorsweepingMasks),
                &pParams->floorsweepingMasks.floorsweepingMasks[grIdx], sizeof(pParams->floorsweepingMasks.floorsweepingMasks[grIdx]));

    //
    // Most of GR is stub'd in AMODEL. However, some tests still need the CAPS/INFO data,
    // so we still need to generate CAPS/INFO data for AMODEL
    //
    if (IS_MODS_AMODEL(pGpu))
    {
        pPrivate->bInitialized = NV_TRUE;
        if (!IS_MIG_IN_USE(pGpu) && (grIdx == 0))
        {
            KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

            // Cache legacy GR mask info (i.e. GR0 with MIG disabled) to pKernelGraphicsManager->legacyFsMaskState
            kgrmgrSetLegacyKgraphicsStaticInfo(pGpu, pKernelGraphicsManager, pKernelGraphics);
        }
        status = NV_OK;
        goto cleanup;
    }

    // GR Global SM Order
    portMemSet(pParams, 0, sizeof(*pParams));
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_GLOBAL_SM_ORDER,
                        pParams,
                        sizeof(pParams->globalSmOrder)),
        cleanup);

    portMemCopy(&pPrivate->staticInfo.globalSmOrder, sizeof(pPrivate->staticInfo.globalSmOrder),
                &pParams->globalSmOrder.globalSmOrder[grIdx], sizeof(pParams->globalSmOrder.globalSmOrder[grIdx]));

    // PPC Mask
    portMemSet(pParams, 0, sizeof(*pParams));
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_PPC_MASKS,
                             pParams,
                             sizeof(pParams->ppcMasks));

    if (status == NV_OK)
    {
        pPrivate->staticInfo.pPpcMasks = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pPpcMasks));
        if (pPrivate->staticInfo.pPpcMasks == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pPpcMasks, sizeof(*pPrivate->staticInfo.pPpcMasks),
                    &pParams->ppcMasks.enginePpcMasks[grIdx], sizeof(pParams->ppcMasks.enginePpcMasks[grIdx]));
    }
    else if (status == NV_ERR_NOT_SUPPORTED)
    {
        //
        // Some chips don't support this call, so just keep the pPpcMasks
        // pointer as NULL, but don't return error
        //
        status = NV_OK;
    }

    portMemSet(pParams, 0, sizeof(*pParams));
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_ZCULL_INFO,
                             pParams,
                             sizeof(pParams->zcullInfo));

    if (status == NV_OK)
    {
        pPrivate->staticInfo.pZcullInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pZcullInfo));
        if (pPrivate->staticInfo.pZcullInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pZcullInfo, sizeof(*pPrivate->staticInfo.pZcullInfo),
                    &pParams->zcullInfo.engineZcullInfo[grIdx], sizeof(pParams->zcullInfo.engineZcullInfo[grIdx]));
    }
    else if (status == NV_ERR_NOT_SUPPORTED)
    {
        // It's expected to get this error when MIG is enabled, thus don't return error
        if (IS_MIG_ENABLED(pGpu))
        {
            status = NV_OK;
        }
    }

    // ROP Info
    portMemSet(pParams, 0, sizeof(*pParams));
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_ROP_INFO,
                             pParams,
                             sizeof(pParams->ropInfo));

    if (status == NV_OK)
    {
        pPrivate->staticInfo.pRopInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pRopInfo));
        if (pPrivate->staticInfo.pRopInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pRopInfo, sizeof(*pPrivate->staticInfo.pRopInfo),
                    &pParams->ropInfo.engineRopInfo[grIdx], sizeof(pParams->ropInfo.engineRopInfo[grIdx]));
    }
    else if (status == NV_ERR_NOT_SUPPORTED)
    {
        // It's expected to get this error when MIG is enabled, thus don't return error
        if (IS_MIG_ENABLED(pGpu))
        {
            status = NV_OK;
        }
    }

    // SM Issue Rate Modifier
    portMemSet(pParams, 0, sizeof(*pParams));
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER,
                             pParams,
                             sizeof(pParams->smIssueRateModifier));

    if (status == NV_OK)
    {
        pPrivate->staticInfo.pSmIssueRateModifier = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pSmIssueRateModifier));
        if (pPrivate->staticInfo.pSmIssueRateModifier == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pSmIssueRateModifier, sizeof(*pPrivate->staticInfo.pSmIssueRateModifier),
                    &pParams->smIssueRateModifier.smIssueRateModifier[grIdx], sizeof(pParams->smIssueRateModifier.smIssueRateModifier[grIdx]));
    }
    else if (status == NV_ERR_NOT_SUPPORTED)
    {
        status = NV_OK;
    }

    // SM Issue Rate Modifier V2
    portMemSet(pParams, 0, sizeof(*pParams));
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_V2,
                             pParams,
                             sizeof(pParams->smIssueRateModifierV2));

    if (status == NV_OK)
    {
        pPrivate->staticInfo.pSmIssueRateModifierV2 = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pSmIssueRateModifierV2));
        if (pPrivate->staticInfo.pSmIssueRateModifierV2 == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        portMemCopy(pPrivate->staticInfo.pSmIssueRateModifierV2, sizeof(*pPrivate->staticInfo.pSmIssueRateModifierV2),
                    &pParams->smIssueRateModifierV2.smIssueRateModifierV2[grIdx], sizeof(pParams->smIssueRateModifierV2.smIssueRateModifierV2[grIdx]));
    }
    else if (status == NV_ERR_NOT_SUPPORTED)
    {
        status = NV_OK;
    }

    // FECS Record Size
    portMemSet(pParams, 0, sizeof(*pParams));
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_RECORD_SIZE,
                        pParams,
                        sizeof(pParams->fecsRecordSize)),
        cleanup);

    pPrivate->staticInfo.fecsRecordSize.fecsRecordSize = pParams->fecsRecordSize.fecsRecordSize[grIdx].fecsRecordSize;

    // FECS Trace Defines
    portMemSet(pParams, 0, sizeof(*pParams));
    status = pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES,
                        pParams,
                        sizeof(pParams->fecsTraceDefines));
    if (status == NV_OK)
    {
        pPrivate->staticInfo.pFecsTraceDefines = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pFecsTraceDefines));
        if (pPrivate->staticInfo.pFecsTraceDefines == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }
        portMemCopy(pPrivate->staticInfo.pFecsTraceDefines, sizeof(*pPrivate->staticInfo.pFecsTraceDefines),
                    &pParams->fecsTraceDefines.fecsTraceDefines[grIdx], sizeof(pParams->fecsTraceDefines.fecsTraceDefines[grIdx]));
    }
    else if (status == NV_ERR_NOT_SUPPORTED)
    {
        status = NV_OK;
    }

    // PDB Properties
    portMemSet(pParams, 0, sizeof(*pParams));
    NV_CHECK_OK_OR_GOTO(
        status,
        LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_PDB_PROPERTIES,
                        pParams,
                        sizeof(pParams->pdbProperties)),
        cleanup);

    portMemCopy(&pPrivate->staticInfo.pdbTable, sizeof(pPrivate->staticInfo.pdbTable),
                &pParams->pdbProperties.pdbTable[grIdx], sizeof(pParams->pdbProperties.pdbTable[grIdx]));
    kgraphicsSetPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics, pPrivate->staticInfo.pdbTable.bPerSubCtxheaderSupported);

    // Publish static configuration
    pPrivate->bInitialized = NV_TRUE;

    // The deferred data is ready after MIG is enabled, so no need to defer the initialization
    if (IS_MIG_IN_USE(pGpu) ||
        !kgraphicsShouldDeferContextInit(pGpu, pKernelGraphics))
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kgraphicsInitializeDeferredStaticData(pGpu, pKernelGraphics, hClient, hSubdevice), cleanup);
    }

    if (!IS_MIG_IN_USE(pGpu) && (grIdx == 0))
    {
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

        // Cache legacy GR mask info (i.e. GR0 with MIG disabled) to pKernelGraphicsManager->legacyFsMaskState
        kgrmgrSetLegacyKgraphicsStaticInfo(pGpu, pKernelGraphicsManager, pKernelGraphics);
    }

cleanup:
    if (status != NV_OK)
    {
        // Redact static configuration
        pPrivate->bInitialized = NV_FALSE;

        portMemFree(pPrivate->staticInfo.pGrInfo);
        pPrivate->staticInfo.pGrInfo = NULL;

        portMemFree(pPrivate->staticInfo.pPpcMasks);
        pPrivate->staticInfo.pPpcMasks = NULL;

        portMemFree(pPrivate->staticInfo.pZcullInfo);
        pPrivate->staticInfo.pZcullInfo = NULL;

        portMemFree(pPrivate->staticInfo.pRopInfo);
        pPrivate->staticInfo.pRopInfo = NULL;

        portMemFree(pPrivate->staticInfo.pContextBuffersInfo);
        pPrivate->staticInfo.pContextBuffersInfo = NULL;

        portMemFree(pPrivate->staticInfo.pSmIssueRateModifier);
        pPrivate->staticInfo.pSmIssueRateModifier = NULL;

        portMemFree(pPrivate->staticInfo.pSmIssueRateModifierV2);
        pPrivate->staticInfo.pSmIssueRateModifierV2 = NULL;

        portMemFree(pPrivate->staticInfo.pFecsTraceDefines);
        pPrivate->staticInfo.pFecsTraceDefines = NULL;
    }

    // If we had to subscribe specifically, free the hclient we allocated
    if (hClient != NV01_NULL_OBJECT)
        pRmApi->Free(pRmApi, hClient, hClient);

    if (gpumgrGetBcEnabledStatus(pGpu) != bBcState)
    {
        // Corrupted broadcast state!
        NV_ASSERT(gpumgrGetBcEnabledStatus(pGpu) != bBcState);
        gpumgrSetBcEnabledStatus(pGpu, bBcState);
    }

    portMemFree(pParams);

    return status;
}

/*! Return if GFX is supported for the given kernel graphics engine */
NvBool
kgraphicsIsGFXSupported_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NvU32 gfxCapabilites;

    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pGrInfo != NULL, NV_FALSE);

    gfxCapabilites = pKernelGraphicsStaticInfo->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_GFX_CAPABILITIES].data;

    return (FLD_TEST_DRF(2080_CTRL_GR, _INFO_GFX_CAPABILITIES, _2D, _TRUE, gfxCapabilites) &&
            FLD_TEST_DRF(2080_CTRL_GR, _INFO_GFX_CAPABILITIES, _3D, _TRUE, gfxCapabilites) &&
            FLD_TEST_DRF(2080_CTRL_GR, _INFO_GFX_CAPABILITIES, _I2M, _TRUE, gfxCapabilites));
}

/*! Retrieve ctxbufpool parameters for given local ctx buffer */
const CTX_BUF_INFO *
kgraphicsGetCtxBufferInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    GR_CTX_BUFFER buf
)
{
    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_CTX_BUFFER, buf), NULL);
    return &pKernelGraphics->maxCtxBufSize[buf];
}

/*! Set ctxbufpool parameters for given local ctx buffer */
void
kgraphicsSetCtxBufferInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    GR_CTX_BUFFER buf,
    NvU64 size,
    NvU64 align,
    RM_ATTR_PAGE_SIZE attr,
    NvBool bContiguous
)
{
    CTX_BUF_INFO *pInfo;
    NV_ASSERT_OR_RETURN_VOID(NV_ENUM_IS(GR_CTX_BUFFER, buf));

    pInfo = &pKernelGraphics->maxCtxBufSize[buf];
    pInfo->size    = size;
    pInfo->align   = align;
    pInfo->attr    = attr;
    pInfo->bContig = bContiguous;
}

/*! Clear ctxbufpool parameters for all local ctx buffers */
void
kgraphicsClearCtxBufferInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    portMemSet(pKernelGraphics->maxCtxBufSize, 0, sizeof(pKernelGraphics->maxCtxBufSize));
}

/*! Initialize ctxbufpool for this engine */
NV_STATUS
kgraphicsInitCtxBufPool_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    Heap *pHeap
)
{
    return ctxBufPoolInit(pGpu, pHeap, &pKernelGraphics->pCtxBufPool);
}

/*! Retrieve ctxbufpool for this engine */
CTX_BUF_POOL_INFO *
kgraphicsGetCtxBufPool_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    return pKernelGraphics->pCtxBufPool;
}

/*! destroy ctxbufpool for this engine */
void
kgraphicsDestroyCtxBufPool_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    if (pKernelGraphics->pCtxBufPool == NULL)
        return;

    ctxBufPoolRelease(pKernelGraphics->pCtxBufPool);
    ctxBufPoolDestroy(&pKernelGraphics->pCtxBufPool);
    pKernelGraphics->pCtxBufPool = NULL;
}

/*! Get the global ctx buffers for the given GFID */
GR_GLOBALCTX_BUFFERS *
kgraphicsGetGlobalCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid
)
{
    if (pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers == NULL)
        return NULL;
    return &pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid];
}

/*! Should this global ctx buffer be mapped as size aligned? */
NvBool
kgraphicsIsGlobalCtxBufferSizeAligned_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    GR_GLOBALCTX_BUFFER buf
)
{
    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_GLOBALCTX_BUFFER, buf), NV_FALSE);
    return pKernelGraphics->globalCtxBuffersInfo.bSizeAligned[buf];
}

/*! Get ctx buf attr for global priv access map */
const GR_BUFFER_ATTR *
kgraphicsGetGlobalPrivAccessMapAttr_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    return &pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr[GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP];
}

/*
 * @brief Get Main context buffer size
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphics
 * @param[in]  bIncludeSubctxHdrs If subctx headers should be included in size calculation
 * @param[out] pSize              Main Context buffer size
 */
NV_STATUS
kgraphicsGetMainCtxBufferSize_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvBool bIncludeSubctxHdrs,
    NvU32 *pSize
)
{
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NvU32 size;

    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pGrInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    size = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS].size;

    // Allocate space for per VEID headers in the golden context buffer.
    if (bIncludeSubctxHdrs &&
        kgraphicsIsPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics))
    {
        // TODO size this down to max per-engine subcontexts
        NvU32 maxSubctx = pKernelGraphicsStaticInfo->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_MAX_SUBCONTEXT_COUNT].data;

        // The header needs to start at a 4 KB aligned address
        size = RM_ALIGN_UP(size, RM_PAGE_SIZE);

        // The header is only 256 bytes; but needs to be 4KB aligned.
        size += (RM_PAGE_SIZE * maxSubctx);
    }

   *pSize = size;
    return NV_OK;
}

NV_STATUS
kgraphicsAllocKgraphicsBuffers_KERNEL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelChannel *pKernelChannel
)
{
    NvU32 gfid;
    OBJGVASPACE *pGVAS;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);
    pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);
    gfid = kchannelGetGfid(pKernelChannel);

    // Deferred static info is necessary at this point for FECS buffer allocation. Skip for guest RM
    if (!IS_VIRTUAL(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsInitializeDeferredStaticData(pGpu, pKernelGraphics, NV01_NULL_OBJECT, NV01_NULL_OBJECT));
    }

    //
    // Allocate global context buffers for this gfid, if they haven't been
    // already
    //
    if (kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid)->memDesc[GR_GLOBALCTX_BUFFER_FECS_EVENT] == NULL)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsAllocGlobalCtxBuffers_HAL(pGpu, pKernelGraphics, gfid));
    }

    if (kgraphicsIsCtxswLoggingSupported(pGpu, pKernelGraphics) &&
        (kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid)->memDesc[GR_GLOBALCTX_BUFFER_FECS_EVENT] != NULL))
    {
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

        if (!gvaspaceIsExternallyOwned(pGVAS) && !IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        {
            //
            // We map CTXSW buffer on each object allocation including compute object
            // Other global context buffers are not mapped during compute object alloc with subcontexts since
            // those are GR-only buffers and not needed for compute-only contexts
            //
            _kgraphicsMapGlobalCtxBuffer(pGpu, pKernelGraphics, gfid, pKernelChannel->pVAS, pKernelGraphicsContext,
                                         GR_GLOBALCTX_BUFFER_FECS_EVENT, NV_FALSE);
        }

        if (!fecsBufferIsMapped(pGpu, pKernelGraphics))
        {
            fecsBufferMap(pGpu, pKernelGraphics);
        }

        if (fecsGetCtxswLogConsumerCount(pGpu, pKernelGraphicsManager) > 0)
            fecsBufferReset(pGpu, pKernelGraphics);
    }

    return NV_OK;
}

static NV_STATUS
_kgraphicsMapGlobalCtxBuffer
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    OBJVASPACE *pVAS,
    KernelGraphicsContext *pKernelGraphicsContext,
    GR_GLOBALCTX_BUFFER buffId,
    NvBool bIsReadOnly
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status = NV_OK;
    NvU64 vaddr = 0;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvBool bSizeAligned;

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    bSizeAligned = pKernelGraphics->globalCtxBuffersInfo.bSizeAligned[buffId];
    pMemDesc = pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid].memDesc[buffId];

    if (pMemDesc == NULL)
    {
        NvU32 buffSize;
        NvU32 fifoEngineId;
        const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;

        pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

        NV_ASSERT_OK_OR_RETURN(kgrctxGlobalCtxBufferToFifoEngineId(buffId, &fifoEngineId));
        buffSize = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[fifoEngineId].size;
        if (buffSize == 0)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Could not map %s Buffer as buffer is not supported\n",
                      NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId));
            return NV_OK;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not map %s Buffer, no memory allocated for it!\n",
                      NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId));
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // Unconditionally call map for refcounting
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgraphicsMapCtxBuffer(pGpu, pKernelGraphics, pMemDesc, pVAS,
                              &pKernelGraphicsContextUnicast->globalCtxBufferVaList[buffId],
                              bSizeAligned,
                              bIsReadOnly));

    NV_ASSERT_OK(vaListFindVa(&pKernelGraphicsContextUnicast->globalCtxBufferVaList[buffId], pVAS, &vaddr));

    NV_PRINTF(LEVEL_INFO,
              "GPU:%d %s Buffer PA @ 0x%llx VA @ 0x%llx of Size 0x%llx\n",
              pGpu->gpuInstance, NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId),
              memdescGetPhysAddr(memdescGetMemDescFromGpu(pMemDesc, pGpu), AT_GPU, 0),
              vaddr, pMemDesc->Size);

    return status;
}

/*!
 * @brief Map a GR ctx buffer
 */
NV_STATUS
kgraphicsMapCtxBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    MEMORY_DESCRIPTOR *pMemDesc,
    OBJVASPACE *pVAS,
    VA_LIST *pVaList,
    NvBool bAlignSize,
    NvBool bIsReadOnly
)
{
    NV_STATUS status = NV_OK;
    NvU64 vaddr = 0;
    OBJGVASPACE *pGVAS;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_PRINTF(LEVEL_INFO, "gpu:%d isBC=%d\n", pGpu->gpuInstance,
              gpumgrGetBcEnabledStatus(pGpu));

    pGVAS = dynamicCast(pVAS, OBJGVASPACE);
    NV_ASSERT_OR_RETURN(!gvaspaceIsExternallyOwned(pGVAS), NV_ERR_INVALID_OPERATION);

    status = vaListFindVa(pVaList, pVAS, &vaddr);
    if (status == NV_ERR_OBJECT_NOT_FOUND)
    {
        // create a new subcontext mapping
        NvU32 allocFlags  = bAlignSize ? DMA_ALLOC_VASPACE_SIZE_ALIGNED : DMA_ALLOC_VASPACE_NONE;
        NvU32 updateFlags = bIsReadOnly ? (DMA_UPDATE_VASPACE_FLAGS_READ_ONLY |
            DMA_UPDATE_VASPACE_FLAGS_SHADER_READ_ONLY) : DMA_UPDATE_VASPACE_FLAGS_NONE;

        if (pGVAS->flags & VASPACE_FLAGS_RESTRICTED_RM_INTERNAL_VALIMITS)
        {
            allocFlags |= DMA_ALLOC_VASPACE_USE_RM_INTERNAL_VALIMITS;
        }

        if (kgraphicsIsPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics))
        {
            status = dmaMapBuffer_HAL(pGpu, GPU_GET_DMA(pGpu), pVAS, pMemDesc, &vaddr,
                allocFlags, updateFlags);
        }
        else
        {
            //
            // Per subcontext headers not enabled.
            // If subcontext is not supported, create a new mapping.
            // If subcontext is supported, create an identity mapping to the existing one.
            //

            if (vaListMapCount(pVaList) == 0)
            {
                status = dmaMapBuffer_HAL(pGpu, GPU_GET_DMA(pGpu), pVAS,
                                          pMemDesc,
                                          &vaddr,
                                          allocFlags,
                                          updateFlags);
            }
            else
            {
                OBJVASPACE *pVas;
                NvU32 mapFlags = 0x0;
                NvU64 vaddrCached;

                FOR_EACH_IN_VADDR_LIST(pVaList, pVas, vaddr)
                {
                    // Find the first virtual address in any VAS
                    break;
                }
                FOR_EACH_IN_VADDR_LIST_END(pVaList, pVas, vaddr);

                if (bIsReadOnly)
                {
                    mapFlags = FLD_SET_DRF(OS46, _FLAGS, _ACCESS, _READ_ONLY, mapFlags);
                    mapFlags = FLD_SET_DRF(OS46, _FLAGS, _SHADER_ACCESS, _READ_ONLY, mapFlags);
                }
                mapFlags = FLD_SET_DRF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _FALSE, mapFlags);
                mapFlags = FLD_SET_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, mapFlags);

                NV_ASSERT(!bAlignSize); // TODO: Add support for size align
                vaddrCached = vaddr;
                NV_ASSERT_OK_OR_ELSE(status,
                    dmaAllocMapping_HAL(pGpu, GPU_GET_DMA(pGpu), pVAS, pMemDesc,
                                        &vaddr,
                                        mapFlags,
                                        NULL,
                                        KMIGMGR_SWIZZID_INVALID),
                    /* do nothing on error, but make sure we overwrite status */;);
                NV_ASSERT(vaddr == vaddrCached);
            }
        }

        NV_PRINTF(LEVEL_INFO, "New ctx buffer mapping at VA 0x%llx\n", vaddr);
    }

    if (status == NV_OK)
        NV_ASSERT_OK_OR_RETURN(vaListAddVa(pVaList, pVAS, vaddr));

    return status;
}

/*!
 * @brief Unmap a GR ctx buffer
 */
void
kgraphicsUnmapCtxBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    OBJVASPACE *pVAS,
    VA_LIST *pVaList
)
{
    NV_STATUS status = NV_OK;
    NvU64     vaddr = 0;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_PRINTF(LEVEL_INFO, "gpu:%d isBC=%d\n", pGpu->gpuInstance,
              gpumgrGetBcEnabledStatus(pGpu));

    status = vaListFindVa(pVaList, pVAS, &vaddr);
    if (status == NV_OK)
    {
        NV_ASSERT_OK(vaListRemoveVa(pVaList, pVAS));

        status = vaListFindVa(pVaList, pVAS, &vaddr);

        NV_ASSERT((NV_OK == status) || (NV_ERR_OBJECT_NOT_FOUND == status));
        if (NV_ERR_OBJECT_NOT_FOUND == status)
        {
            if (vaListGetManaged(pVaList))
            {
                dmaUnmapBuffer_HAL(pGpu, GPU_GET_DMA(pGpu), pVAS, vaddr);
            }

            NV_PRINTF(LEVEL_INFO, "Freed ctx buffer mapping at VA 0x%llx\n",
                      vaddr);
        }
    }
}

/*!
 * @brief Get the Class number for a given gr object type
 *
 * @param[in]   pGpu
 * @param[in]   pKernelGraphics
 * @param[in]   wantObjectType   GR object type to lookup
 * @param[out]  pClass           class number
 */
NV_STATUS
kgraphicsGetClassByType_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 wantObjectType,
    NvU32 *pClass
)
{
    NV_STATUS status = NV_OK;
    NvU32 objectType;
    NvU32 i;
    NvU32 numClasses;
    NvU32 *pClassesSupported;

    *pClass = 0;

    if (wantObjectType >= GR_OBJECT_TYPE_INVALID)
    {
        NV_PRINTF(LEVEL_ERROR, "bad requested object type : %d\n",
                  wantObjectType);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // find out how many classes of type ENG_GR(0) we have
    NV_ASSERT_OK_OR_RETURN(
        gpuGetClassList(pGpu, &numClasses, NULL, ENG_GR(pKernelGraphics->instance)));

    pClassesSupported = portMemAllocNonPaged(sizeof(NvU32) * numClasses);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pClassesSupported != NULL, NV_ERR_NO_MEMORY);

    status = gpuGetClassList(pGpu, &numClasses, pClassesSupported, ENG_GR(pKernelGraphics->instance));

    if (status == NV_OK)
    {
        for (i = 0; i < numClasses; i++)
        {
            kgrmgrGetGrObjectType(pClassesSupported[i], &objectType);

            NV_PRINTF(LEVEL_INFO, "classNum=0x%08x, type=%d\n",
                      pClassesSupported[i], objectType);

            if (objectType == wantObjectType)
                *pClass = pClassesSupported[i];
        }
    }

    portMemFree(pClassesSupported);

    return (*pClass != 0) ? NV_OK : NV_ERR_INVALID_CLASS;
}

/*!
 * @brief retrieve the ctx attributes for the given buffer
 */
const GR_BUFFER_ATTR *
kgraphicsGetContextBufferAttr_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    GR_CTX_BUFFER buf
)
{
    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_CTX_BUFFER, buf), NULL);
    return &pKernelGraphics->ctxAttr[buf];
}

/*!
 * @brief Creates a VEID0 channel for Golden Image creation
 *
 * @return NV_OK if channel and golden image created successfully
 */
NV_STATUS
kgraphicsCreateGoldenImageChannel_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    NV_STATUS                              status = NV_OK;
    NvHandle                               hClientId = NV01_NULL_OBJECT;
    NvHandle                               hDeviceId;
    NvHandle                               hSubdeviceId;
    NvHandle                               hVASpace = 0xbaba0042;
    NvHandle                               hPBVirtMemId = 0xbaba0043;
    NvHandle                               hPBPhysMemId = 0xbaba0044;
    NvHandle                               hChannelId = 0xbaba0045;
    NvHandle                               hObj3D = 0xbaba0046;
    NvHandle                               hUserdId = 0xbaba0049;
    NvU32                                  gpFifoEntries = 32;       // power-of-2 random choice
    NvU64                                  gpFifoSize = NVA06F_GP_ENTRY__SIZE * gpFifoEntries;
    NvU64                                  chSize = gpFifoSize;
    RM_API                                *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RsClient                              *pClientId;
    KernelMIGManager                      *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool                                 bNeedMIGWar;
    NvBool                                 bBcStatus;
    NvBool                                 bClientUserd = IsVOLTAorBetter(pGpu);
    NvBool                                 bAcquireLock = NV_FALSE;
    NvU32                                  sliLoopReentrancy;
    NV_VASPACE_ALLOCATION_PARAMETERS       vaParams;
    NV_MEMORY_ALLOCATION_PARAMS            memAllocParams;
    NV_CHANNEL_ALLOC_PARAMS channelGPFIFOAllocParams;
    NvU32                                  classNum;
    MIG_INSTANCE_REF                       ref;
    NvU32                                  objectType;
    NvU32                                  primarySliSubDeviceInstance;

    // XXX This should be removed when broadcast SLI support is deprecated
    if (!gpumgrIsParentGPU(pGpu))
    {
        return NV_OK;
    }

    bBcStatus = gpumgrGetBcEnabledStatus(pGpu);

    // FIXME these allocations corrupt BC state
    NV_ASSERT_OK_OR_RETURN(
        rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, &hClientId, &hDeviceId, &hSubdeviceId));
    // rmapiutilAllocClientAndDeviceHandles allocates a subdevice object for this subDeviceInstance
    primarySliSubDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClientId, &pClientId));

    gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);

    // As we have forced here SLI broadcast mode, temporarily reset the reentrancy count
    sliLoopReentrancy = gpumgrSLILoopReentrancyPop(pGpu);

    bNeedMIGWar = IS_MIG_IN_USE(pGpu);

    // Allocate subdevices for secondary GPUs
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        NvHandle hSecondary;
        NV2080_ALLOC_PARAMETERS nv2080AllocParams;
        NvU32 thisSubDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

        // Skip if already allocated by rmapiutilAllocClientAndDeviceHandles()
        if (thisSubDeviceInstance == primarySliSubDeviceInstance)
            SLI_LOOP_CONTINUE;

        // Allocate a subDevice
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            clientGenResourceHandle(pClientId, &hSecondary),
            cleanup);

        portMemSet(&nv2080AllocParams, 0, sizeof(nv2080AllocParams));
        nv2080AllocParams.subDeviceId = thisSubDeviceInstance;

        NV_CHECK_OK(status, LEVEL_SILENT,
            pRmApi->AllocWithHandle(pRmApi,
                                    hClientId,
                                    hDeviceId,
                                    hSecondary,
                                    NV20_SUBDEVICE_0,
                                    &nv2080AllocParams,
                                    sizeof(nv2080AllocParams)));
    }
    SLI_LOOP_END;

    if (bNeedMIGWar)
    {
        NvHandle hPartitionRef = 0xbaba0048;
        NvHandle hExecPartitionRef = 0xbaba004a;
        NVC637_ALLOCATION_PARAMETERS nvC637AllocParams = {0};

        // Get swizzId for this GR
        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGetMIGReferenceFromEngineType(pGpu, pKernelMIGManager,
                                                 RM_ENGINE_TYPE_GR(pKernelGraphics->instance), &ref),
            cleanup);

        portMemSet(&nvC637AllocParams, 0, sizeof(NVC637_ALLOCATION_PARAMETERS));
        nvC637AllocParams.swizzId = ref.pKernelMIGGpuInstance->swizzId;

        // allocate partition reference
        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->AllocWithHandle(pRmApi,
                                    hClientId,
                                    hSubdeviceId,
                                    hPartitionRef,
                                    AMPERE_SMC_PARTITION_REF,
                                    &nvC637AllocParams,
                                    sizeof(nvC637AllocParams)),
            cleanup);

        if (ref.pMIGComputeInstance != NULL)
        {
            NVC638_ALLOCATION_PARAMETERS nvC638AllocParams = {0};
            nvC638AllocParams.execPartitionId = ref.pMIGComputeInstance->id;
            NV_ASSERT_OK_OR_GOTO(status,
                pRmApi->AllocWithHandle(pRmApi,
                                        hClientId,
                                        hPartitionRef,
                                        hExecPartitionRef,
                                        AMPERE_SMC_EXEC_PARTITION_REF,
                                        &nvC638AllocParams,
                                        sizeof(nvC638AllocParams)),
                cleanup);
        }
    }

    //
    // VidHeapControl and vaspace creation calls should happen outside GPU locks
    // UVM/CUDA may be holding the GPU locks here and the allocation may subsequently fail
    // So explicitly release GPU locks before RmVidHeapControl
    // See Bug 1735851-#24
    //
    if (rmapiLockIsOwner())
    {
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        bAcquireLock = NV_TRUE;
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

    // Create a new VAspace for channel
    portMemSet(&vaParams, 0, sizeof(NV_VASPACE_ALLOCATION_PARAMETERS));
    vaParams.flags |= NV_VASPACE_ALLOCATION_FLAGS_PTETABLE_HEAP_MANAGED;

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hVASpace, FERMI_VASPACE_A, &vaParams, sizeof(vaParams)),
        cleanup);

    // Allocate gpfifo entries
    portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = chSize;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.hVASpace  = 0; // Physical allocations don't expect vaSpace handles

    //
    // When APM feature is enabled all RM internal sysmem allocations must
    // be in unprotected memory
    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // Other sysmem allocations that are not required to be accessed from GPU
    // must be in protected memory
    //
    memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _MEMORY_PROTECTION, _UNPROTECTED);

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hPBPhysMemId, NV01_MEMORY_SYSTEM, &memAllocParams, sizeof(memAllocParams)),
        cleanup);

    portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = chSize;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.flags     = NVOS32_ALLOC_FLAGS_VIRTUAL;
    memAllocParams.hVASpace  = hVASpace; // Virtual allocation expect vaSpace handles
                                         // 0 handle = allocations on gpu default vaSpace

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hPBVirtMemId, NV50_MEMORY_VIRTUAL, &memAllocParams, sizeof(memAllocParams)),
        cleanup);

    // Allocate Userd
    if (bClientUserd)
    {
        NvU32 userdMemClass = NV01_MEMORY_LOCAL_USER;
        NvU32 ctrlSize;

        if (gpuIsClassSupported(pGpu, VOLTA_CHANNEL_GPFIFO_A))
        {
            ctrlSize = sizeof(Nvc36fControl);
        }
        else if (gpuIsClassSupported(pGpu, TURING_CHANNEL_GPFIFO_A))
        {
            ctrlSize = sizeof(Nvc46fControl);
        }
        else if (gpuIsClassSupported(pGpu, AMPERE_CHANNEL_GPFIFO_A))
        {
            ctrlSize = sizeof(Nvc56fControl);
        }
        else if (gpuIsClassSupported(pGpu, HOPPER_CHANNEL_GPFIFO_A))
        {
            ctrlSize = sizeof(Nvc86fControl);
        }
        else if (gpuIsClassSupported(pGpu, BLACKWELL_CHANNEL_GPFIFO_A))
        {
            ctrlSize = sizeof(Nvc96fControl);
        }
        else if (gpuIsClassSupported(pGpu, BLACKWELL_CHANNEL_GPFIFO_B))
        {
            ctrlSize = sizeof(Nvca6fControl);
        }
        else
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto cleanup;
        }

        portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));
        memAllocParams.owner = HEAP_OWNER_RM_CLIENT_GENERIC;
        memAllocParams.size  = ctrlSize;
        memAllocParams.type  = NVOS32_TYPE_IMAGE;

        // Apply registry overrides to USERD.
        switch (DRF_VAL(_REG_STR_RM, _INST_LOC, _USERD, pGpu->instLocOverrides))
        {
            case NV_REG_STR_RM_INST_LOC_USERD_NCOH:
            case NV_REG_STR_RM_INST_LOC_USERD_COH:
                userdMemClass = NV01_MEMORY_SYSTEM;
                memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
                break;

            case NV_REG_STR_RM_INST_LOC_USERD_VID:
            case NV_REG_STR_RM_INST_LOC_USERD_DEFAULT:
                memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);
                memAllocParams.attr2 = DRF_DEF(OS32, _ATTR2, _INTERNAL, _YES);
                memAllocParams.flags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;
                break;
        }

        //
        // When APM is enabled all RM internal allocations must to go to
        // unprotected memory irrespective of vidmem or sysmem
        // When Hopper CC is enabled all RM internal sysmem allocations that
        // are required to be accessed from GPU should be in unprotected memory
        // and all vidmem allocations must go to protected memory
        //
        if (gpuIsApmFeatureEnabled(pGpu) ||
            FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr))
        {
            memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _MEMORY_PROTECTION,
                                            _UNPROTECTED);
        }
        memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _ALLOCATE_FROM_RESERVED_HEAP, _YES);

        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hUserdId,
                                    userdMemClass, &memAllocParams, sizeof(memAllocParams)),
            cleanup);
    }

    // Get fifo channel class Id
    classNum = kfifoGetChannelClassId(pGpu, GPU_GET_KERNEL_FIFO(pGpu));
    NV_ASSERT_OR_GOTO(classNum != 0, cleanup);

    // Allocate a bare channel
    portMemSet(&channelGPFIFOAllocParams, 0, sizeof(NV_CHANNEL_ALLOC_PARAMS));
    channelGPFIFOAllocParams.hVASpace      = hVASpace;
    channelGPFIFOAllocParams.hObjectBuffer = hPBVirtMemId;
    channelGPFIFOAllocParams.gpFifoEntries = gpFifoEntries;
    //
    // Set the gpFifoOffset to zero intentionally since we only need this channel
    // to be created, but will not submit any work to it. So it's fine not to
    // provide a valid offset here.
    //
    channelGPFIFOAllocParams.gpFifoOffset  = 0;
    if (bClientUserd)
    {
        channelGPFIFOAllocParams.hUserdMemory[0] = hUserdId;
    }

    if (bNeedMIGWar)
    {
        RM_ENGINE_TYPE localRmEngineType;
        Device *pDevice;

        NV_ASSERT_OK_OR_GOTO(status,
            deviceGetByHandle(pClientId, hDeviceId, &pDevice),
            cleanup);

        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref),
            cleanup);

        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref, RM_ENGINE_TYPE_GR(pKernelGraphics->instance), &localRmEngineType),
            cleanup);

        channelGPFIFOAllocParams.engineType = gpuGetNv2080EngineType(localRmEngineType);
    }
    else
    {
        channelGPFIFOAllocParams.engineType = gpuGetNv2080EngineType(RM_ENGINE_TYPE_GR0);
    }

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hChannelId,
                                classNum, &channelGPFIFOAllocParams, sizeof(channelGPFIFOAllocParams)),
        cleanup);

    //
    // When using split VAS, we need to reserve enough pagepool memory to
    // sustain large context buffer mappings. For GSPCLIENT where the golden
    // context buffer channel is initialized on boot, the pagepool does not have
    // enough reserved memory to accommodate these buffers, so we need to
    // reserve extra here.
    //
    if (IS_GSP_CLIENT(pGpu))
    {
        KernelChannel *pKernelChannel;
        NvU64 reserveSize;
        const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NvU32 i;

        NV_ASSERT_OK(CliGetKernelChannel(pClientId, hChannelId, &pKernelChannel));

        NV_ASSERT_OR_ELSE(pKernelGraphicsStaticInfo != NULL,
            status = NV_ERR_INVALID_STATE;
            goto cleanup;);
        NV_ASSERT_OR_ELSE(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL,
            status = NV_ERR_INVALID_STATE;
            goto cleanup;);

        reserveSize = 0;
        for (i = 0; i < NV_ARRAY_ELEMENTS(pKernelGraphicsStaticInfo->pContextBuffersInfo->engine); ++i)
        {
            if (pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[i].size != NV_U32_MAX)
                reserveSize += pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[i].size;
        }

        NV_ASSERT_OK(
            vaspaceReserveMempool(pKernelChannel->pVAS, pGpu,
                                  GPU_RES_GET_DEVICE(pKernelChannel),
                                  reserveSize, RM_PAGE_SIZE,
                                  VASPACE_RESERVE_FLAGS_NONE));
    }

    // Reaquire the GPU locks
    NV_ASSERT_OK_OR_GOTO(status,
        rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GR),
        cleanup);
    bAcquireLock = NV_FALSE;
    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (kgraphicsIsGFXSupported(pGpu, pKernelGraphics))
    {
        objectType = GR_OBJECT_TYPE_3D;
    }
    else
    {
        objectType = GR_OBJECT_TYPE_COMPUTE;
    }

    // Get KernelGraphicsObject class Id
    NV_ASSERT_OK_OR_GOTO(status,
        kgraphicsGetClassByType(pGpu, pKernelGraphics, objectType, &classNum),
        cleanup);
    NV_ASSERT_OR_GOTO(classNum != 0, cleanup);

    // Allocate a GR object on the channel
    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hChannelId, hObj3D, classNum, NULL, 0),
        cleanup);

cleanup:

    if (bAcquireLock)
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
            rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GR));
    }

    // Free all handles
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
        pRmApi->Free(pRmApi, hClientId, hClientId));

    // Restore the reentrancy count
    gpumgrSLILoopReentrancyPush(pGpu, sliLoopReentrancy);

    gpumgrSetBcEnabledStatus(pGpu, bBcStatus);

    return status;
}

/*!
 * @brief Free context buffers shared by all/most graphics contexts
 */
void kgraphicsFreeGlobalCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    GR_GLOBALCTX_BUFFERS *pCtxBuffers;
    GR_GLOBALCTX_BUFFER buff;
    NvBool bEvict = NV_FALSE;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    if (pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers == NULL)
        return;

    pCtxBuffers = &pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid];

    if (!pCtxBuffers->bAllocated)
        return;

    FOR_EACH_IN_ENUM(GR_GLOBALCTX_BUFFER, buff)
    {
        if (pCtxBuffers->memDesc[buff] != NULL)
        {
            bEvict = NV_TRUE;

            memdescFree(pCtxBuffers->memDesc[buff]);
            memdescDestroy(pCtxBuffers->memDesc[buff]);
            pCtxBuffers->memDesc[buff] = NULL;
            pCtxBuffers->bInitialized[buff] = NV_FALSE;
        }
    }
    FOR_EACH_IN_ENUM_END;

    pCtxBuffers->bAllocated = NV_FALSE;

    // make sure all L2 cache lines using CB buffers are clear after we free them
    if (bEvict)
    {
        NV_STATUS status;
        status = kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, NULL, FB_CACHE_VIDEO_MEMORY, FB_CACHE_EVICT);
        NV_ASSERT((status == NV_OK) || (status == NV_ERR_GPU_IN_FULLCHIP_RESET));
    }
}

NV_STATUS
kgraphicsGetCaps_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU8 *pGrCaps
)
{
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN(pGrCaps != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(pGrCaps,
                NV0080_CTRL_GR_CAPS_TBL_SIZE * sizeof(*pGrCaps),
                pKernelGraphicsStaticInfo->grCaps.capsTbl,
                NV0080_CTRL_GR_CAPS_TBL_SIZE * sizeof(*pGrCaps));

    return NV_OK;
}

/*!
 * @brief Return whether unrestricted register access bufffer is supported or not.
 */
NvBool
kgraphicsIsUnrestrictedAccessMapSupported_PF
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    return !hypervisorIsVgxHyper();
}

/*!
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 */
void
kgraphicsRegisterIntrService_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    NvU32 engineIdx = MC_ENGINE_IDX_GRn_FECS_LOG(pKernelGraphics->instance);

    NV_ASSERT(pRecords[engineIdx].pInterruptService == NULL);
    pRecords[engineIdx].pInterruptService = staticCast(pKernelGraphics, IntrService);

    engineIdx = MC_ENGINE_IDX_GRn(pKernelGraphics->instance);

    NV_ASSERT(pRecords[engineIdx].pNotificationService == NULL);
    pRecords[engineIdx].bFifoWaiveNotify = NV_FALSE;
    pRecords[engineIdx].pNotificationService = staticCast(pKernelGraphics, IntrService);
}

/*!
 * @brief Services the nonstall interrupt.
 */
NvU32
kgraphicsServiceNotificationInterrupt_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    IntrServiceServiceNotificationInterruptArguments *pParams
)
{
    NvU32 grIdx = pKernelGraphics->instance;

    NV_ASSERT_OR_RETURN(pParams != NULL, 0);
    NV_ASSERT_OR_RETURN(pParams->engineIdx == MC_ENGINE_IDX_GRn(grIdx), 0);

    MODS_ARCH_REPORT(NV_ARCH_EVENT_NONSTALL_GR, "%s", "processing GR nonstall interrupt\n");

    kgraphicsNonstallIntrCheckAndClear_HAL(pGpu, pKernelGraphics, pParams->pThreadState);
    engineNonStallIntrNotify(pGpu, RM_ENGINE_TYPE_GR(pKernelGraphics->instance));
    return NV_OK;
}

/*!
 * KernelGraphics RM Device Controls
 */

/*!
 * deviceCtrlCmdKGrGetCaps_IMPL
 *
 * Lock Requirements:
 *      Assert that API lock held on entry
 *
 * TODO: remove once all uses have been migrated to V2
 */
NV_STATUS
deviceCtrlCmdKGrGetCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GR_GET_CAPS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);
    NvU8 *pGrCaps = NvP64_VALUE(pParams->capsTbl);
    NvBool bCapsPopulated = NV_FALSE;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IsDFPGA(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGrCaps != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pParams->capsTblSize == NV0080_CTRL_GR_CAPS_TBL_SIZE, NV_ERR_INVALID_ARGUMENT);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        KernelGraphics *pKernelGraphics;
        NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
        NV_STATUS status;

        portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
        kgrmgrCtrlSetEngineID(0, &grRouteInfo);
        NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics),
            SLI_LOOP_RETURN(status););

        if (!bCapsPopulated)
        {
            NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
                kgraphicsGetCaps(pGpu, pKernelGraphics, pGrCaps),
                SLI_LOOP_RETURN(status););

            bCapsPopulated = NV_TRUE;
        }
    }
    SLI_LOOP_END

    return NV_OK;
}

/*!
 * deviceCtrlCmdKGrGetCapsV2_IMPL
 *
 * Lock Requirements:
 *      Assert that API lock held on entry
 */
NV_STATUS
deviceCtrlCmdKGrGetCapsV2_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GR_GET_CAPS_V2_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IsDFPGA(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        KernelGraphics *pKernelGraphics;
        NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = pParams->grRouteInfo;
        NV_STATUS status;

        kgrmgrCtrlSetEngineID(0, &grRouteInfo);
        NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics),
            SLI_LOOP_RETURN(status););

        if (!pParams->bCapsPopulated)
        {
            NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
                kgraphicsGetCaps(pGpu, pKernelGraphics, pParams->capsTbl),
                SLI_LOOP_RETURN(status););

            pParams->bCapsPopulated = NV_TRUE;
        }
    }
    SLI_LOOP_END

    return NV_OK;
}

static NV_STATUS
_kgraphicsCtrlCmdGrGetInfoV2
(
    OBJGPU *pGpu,
    Device *pDevice,
    NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NvU32 grInfoListSize = pParams->grInfoListSize;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;
    NvU32 i;

    if (pKernelGraphicsManager == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((0 == grInfoListSize) ||
        (grInfoListSize > NV2080_CTRL_GR_INFO_MAX_SIZE))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid grInfoList size: 0x%x\n", grInfoListSize);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo != NULL, NV_ERR_NOT_SUPPORTED);

        pGrInfo = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo;
    }
    else
    {
        KernelGraphics *pKernelGraphics;
        const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo, &pKernelGraphics));

        pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pGrInfo != NULL, NV_ERR_NOT_SUPPORTED);

        pGrInfo = pKernelGraphicsStaticInfo->pGrInfo;
    }

    for (i = 0; i < grInfoListSize; i++)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pParams->grInfoList[i].index < NV2080_CTRL_GR_INFO_MAX_SIZE, NV_ERR_INVALID_ARGUMENT);
        pParams->grInfoList[i].data = pGrInfo->infoList[pParams->grInfoList[i].index].data;
    }

    return status;
}

/*!
 * deviceCtrlCmdKGrGetInfo
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 *
 * TODO: remove once all uses have been migrated to V2
 */
NV_STATUS
deviceCtrlCmdKGrGetInfo_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GR_GET_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);
    NV0080_CTRL_GR_GET_INFO_V2_PARAMS grInfoParamsV2;
    NV0080_CTRL_GR_INFO *pGrInfos = NvP64_VALUE(pParams->grInfoList);
    NvU32 grInfoListSize = NV_MIN(pParams->grInfoListSize,
                                  NV0080_CTRL_GR_INFO_MAX_SIZE);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGrInfos != NULL, NV_ERR_INVALID_ARGUMENT);

    portMemSet(&grInfoParamsV2, 0, sizeof(grInfoParamsV2));
    portMemCopy(grInfoParamsV2.grInfoList, grInfoListSize * sizeof(*pGrInfos),
                pGrInfos, grInfoListSize * sizeof(*pGrInfos));
    grInfoParamsV2.grInfoListSize = grInfoListSize;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgraphicsCtrlCmdGrGetInfoV2(pGpu, pDevice, &grInfoParamsV2));

    portMemCopy(pGrInfos, grInfoListSize * sizeof(*pGrInfos),
                grInfoParamsV2.grInfoList, grInfoListSize * sizeof(*pGrInfos));
    return NV_OK;
}

/*!
 * deviceCtrlCmdKGrGetInfoV2
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
deviceCtrlCmdKGrGetInfoV2_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GR_GET_INFO_V2_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgraphicsCtrlCmdGrGetInfoV2(pGpu, pDevice, pParams));

    return NV_OK;
}

NV_STATUS
kgraphicsDiscoverMaxLocalCtxBufferSize_IMPL
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    NvU32 bufId = 0;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);

    if (IS_MODS_AMODEL(pGpu))
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(
    kgraphicsInitializeDeferredStaticData(pGpu, pKernelGraphics, NV01_NULL_OBJECT, NV01_NULL_OBJECT));

    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    FOR_EACH_IN_ENUM(GR_CTX_BUFFER, bufId)
    {
        if (bufId == GR_CTX_BUFFER_MAIN)
        {
            NvU32 size;

            NV_ASSERT_OK_OR_RETURN(kgraphicsGetMainCtxBufferSize(pGpu, pKernelGraphics, NV_TRUE, &size));
            kgraphicsSetCtxBufferInfo(pGpu, pKernelGraphics, bufId,
                                      size,
                                      RM_PAGE_SIZE,
                                      RM_ATTR_PAGE_SIZE_4KB,
                                      kgraphicsShouldForceMainCtxContiguity_HAL(pGpu, pKernelGraphics));
        }
        else
        {
            NvU32 fifoEngineId;

            NV_ASSERT_OK_OR_RETURN(
                kgrctxCtxBufferToFifoEngineId(bufId, &fifoEngineId));

            kgraphicsSetCtxBufferInfo(pGpu, pKernelGraphics, bufId,
                                      pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[fifoEngineId].size,
                                      RM_PAGE_SIZE,
                                      RM_ATTR_PAGE_SIZE_4KB,
                                      ((bufId == GR_CTX_BUFFER_PATCH) || (bufId == GR_CTX_BUFFER_PM)));
        }
    }
    FOR_EACH_IN_ENUM_END;
    return NV_OK;
}

/*!
 * KernelGraphics RM SubDevice Controls
 */

/*!
 * subdeviceCtrlCmdKGrGetCapsV2
 *
 * Lock Requirements:
 *      Assert that API lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetCapsV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = pParams->grRouteInfo;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);

    kgrmgrCtrlSetEngineID(0, &grRouteInfo);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    if (pKernelGraphicsStaticInfo == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    if (!pParams->bCapsPopulated)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsGetCaps(pGpu, pKernelGraphics, pParams->capsTbl));

        pParams->bCapsPopulated = NV_TRUE;
    }

    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetInfo
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 *
 * TODO: remove once all uses have been migrated to V2
 */
NV_STATUS
subdeviceCtrlCmdKGrGetInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_INFO_PARAMS *pParams
)
{
    NV2080_CTRL_GR_GET_INFO_V2_PARAMS grInfoParamsV2;
    NV2080_CTRL_GR_INFO *pGrInfos = NvP64_VALUE(pParams->grInfoList);
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = pParams->grRouteInfo;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NvU32 grInfoListSize = NV_MIN(pParams->grInfoListSize,
                                  NV2080_CTRL_GR_INFO_MAX_SIZE);

    //
    // Adding the null check as engine GRMGR is missing for DFPGA.
    //
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelGraphicsManager != NULL, NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGrInfos != NULL, NV_ERR_INVALID_ARGUMENT);

    portMemSet(&grInfoParamsV2, 0, sizeof(grInfoParamsV2));
    grInfoParamsV2.grInfoListSize = grInfoListSize;
    portMemCopy(grInfoParamsV2.grInfoList, grInfoListSize * sizeof(*pGrInfos),
                pGrInfos, grInfoListSize * sizeof(*pGrInfos));
    grInfoParamsV2.grRouteInfo = grRouteInfo;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgraphicsCtrlCmdGrGetInfoV2(pGpu, pDevice, &grInfoParamsV2));

    portMemCopy(pGrInfos, grInfoListSize * sizeof(*pGrInfos),
                grInfoParamsV2.grInfoList, grInfoListSize * sizeof(*pGrInfos));
    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetInfoV2
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetInfoV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kgraphicsCtrlCmdGrGetInfoV2(pGpu, pDevice, pParams));

    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetSmToGpcTpcMappings
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphics *pKernelGraphics;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    const KGRAPHICS_STATIC_INFO *pStaticInfo;
    NvU32 i;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo, &pKernelGraphics));
    }

    // Verify static info is available
    pStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);

    // Verify limits are within bounds
    NV_ASSERT_OR_RETURN(pStaticInfo->globalSmOrder.numSm <= NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_MAX_SM_COUNT,
                        NV_ERR_INVALID_LIMIT);

    // Populate output data
    pParams->smCount = pStaticInfo->globalSmOrder.numSm;
    for (i = 0; i < pStaticInfo->globalSmOrder.numSm; ++i)
    {
        pParams->smId[i].gpcId = pStaticInfo->globalSmOrder.globalSmId[i].gpcId;
        pParams->smId[i].tpcId = pStaticInfo->globalSmOrder.globalSmId[i].localTpcId;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphics *pKernelGraphics;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    const KGRAPHICS_STATIC_INFO *pStaticInfo;
    NvU32 i;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo, &pKernelGraphics));
    }

    // Verify static info is available
    pStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);

    // Verify limits are within bounds
    NV_ASSERT_OR_RETURN(pStaticInfo->globalSmOrder.numSm <= NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER_MAX_SM_COUNT,
                        NV_ERR_INVALID_LIMIT);

    // Populate output data
    pParams->numSm = pStaticInfo->globalSmOrder.numSm;
    pParams->numTpc = pStaticInfo->globalSmOrder.numTpc;
    for (i = 0; i < pStaticInfo->globalSmOrder.numSm; ++i)
    {
        pParams->globalSmId[i].gpcId           = pStaticInfo->globalSmOrder.globalSmId[i].gpcId;
        pParams->globalSmId[i].localTpcId      = pStaticInfo->globalSmOrder.globalSmId[i].localTpcId;
        pParams->globalSmId[i].localSmId       = pStaticInfo->globalSmOrder.globalSmId[i].localSmId;
        pParams->globalSmId[i].globalTpcId     = pStaticInfo->globalSmOrder.globalSmId[i].globalTpcId;
        pParams->globalSmId[i].virtualGpcId    = pStaticInfo->globalSmOrder.globalSmId[i].virtualGpcId;
        pParams->globalSmId[i].migratableTpcId = pStaticInfo->globalSmOrder.globalSmId[i].migratableTpcId;
        pParams->globalSmId[i].ugpuId          = pStaticInfo->globalSmOrder.globalSmId[i].ugpuId;
    }

    return status;
}

/*!
 * subdeviceCtrlCmdKGrGetSmIssueRateModifier
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphics *pKernelGraphics;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    const KGRAPHICS_STATIC_INFO *pStaticInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        NvU32 grIdx;
        for (grIdx = 0; grIdx < GPU_MAX_GRS; grIdx++)
        {
            pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
            if (pKernelGraphics != NULL)
                break;
        }
        if (pKernelGraphics == NULL)
            return NV_ERR_INVALID_STATE;
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo, &pKernelGraphics));
    }

    // Verify static info is available
    pStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pStaticInfo->pSmIssueRateModifier != NULL, NV_ERR_NOT_SUPPORTED);

    pParams->imla0 = pStaticInfo->pSmIssueRateModifier->imla0;
    pParams->fmla16 = pStaticInfo->pSmIssueRateModifier->fmla16;
    pParams->dp = pStaticInfo->pSmIssueRateModifier->dp;
    pParams->fmla32 = pStaticInfo->pSmIssueRateModifier->fmla32;
    pParams->ffma = pStaticInfo->pSmIssueRateModifier->ffma;
    pParams->imla1 = pStaticInfo->pSmIssueRateModifier->imla1;
    pParams->imla2 = pStaticInfo->pSmIssueRateModifier->imla2;
    pParams->imla3 = pStaticInfo->pSmIssueRateModifier->imla3;
    pParams->imla4 = pStaticInfo->pSmIssueRateModifier->imla4;

    return NV_OK;
}

static NvU8
findSmIssueRateModifier
(
    NvU32 index,
    NvU32 *pData,
    NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER_V2 *pSmIssueRateModifierV2
)
{
    for (NvU32 i = 0; i < pSmIssueRateModifierV2->smIssueRateModifierListSize; i++)
    {
        if (pSmIssueRateModifierV2->smIssueRateModifierList[i].index == index)
        {
            *pData = pSmIssueRateModifierV2->smIssueRateModifierList[i].data;
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ARGUMENT;
}

/*!
 * subdeviceCtrlCmdKGrGetSmIssueRateModifierV2
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphics *pKernelGraphics;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    const KGRAPHICS_STATIC_INFO *pStaticInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvU32 fuseValue = 0;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (!IS_MIG_IN_USE(pGpu) || kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        NvU32 grIdx;
        for (grIdx = 0; grIdx < GPU_MAX_GRS; grIdx++)
        {
            pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
            if (pKernelGraphics != NULL)
                break;
        }
        if (pKernelGraphics == NULL)
            return NV_ERR_INVALID_STATE;
    }
    else
    {
        MIG_INSTANCE_REF ref;
        RM_ENGINE_TYPE globalGrEngine;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));
        NV_ASSERT_OR_RETURN(ref.pMIGComputeInstance != NULL && ref.pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_STATE);

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref, RM_ENGINE_TYPE_GR(0), &globalGrEngine));

        pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, RM_ENGINE_TYPE_GR_IDX(globalGrEngine));
    }

    // Verify static info is available
    pStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pStaticInfo->pSmIssueRateModifierV2 != NULL, NV_ERR_NOT_SUPPORTED);

    if (pParams->smIssueRateModifierListSize >= NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_MAX_LIST_SIZE)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    else if (pParams->smIssueRateModifierListSize != 0)
    {
        // Discarding fuse values. Will collect agn after validating all fuse indexes are valid.
        for (NvU32 i = 0; i < pParams->smIssueRateModifierListSize; i++)
        {
            NV_ASSERT_OK_OR_RETURN(findSmIssueRateModifier(pParams->smIssueRateModifierList[i].index, &fuseValue, pStaticInfo->pSmIssueRateModifierV2));
        }
    }
    else if (pParams->smIssueRateModifierListSize == 0)
    {
        pParams->smIssueRateModifierListSize = pStaticInfo->pSmIssueRateModifierV2->smIssueRateModifierListSize;
        for (NvU32 i = 0; i < pParams->smIssueRateModifierListSize; i++)
            pParams->smIssueRateModifierList[i].index = pStaticInfo->pSmIssueRateModifierV2->smIssueRateModifierList[i].index;
    }

    for (NvU32 i = 0; i < pParams->smIssueRateModifierListSize; i++)
    {
        NV_ASSERT_OK_OR_RETURN(findSmIssueRateModifier(pParams->smIssueRateModifierList[i].index, &(pParams->smIssueRateModifierList[i].data), pStaticInfo->pSmIssueRateModifierV2));
    }

    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetGpcMask
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetGpcMask_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (!IS_MIG_IN_USE(pGpu) ||
        kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        pParams->gpcMask = kgrmgrGetLegacyGpcMask(pGpu, pKernelGraphicsManager);
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo, &pKernelGraphics));

        pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

        pParams->gpcMask = pKernelGraphicsStaticInfo->floorsweepingMasks.gpcMask;
    }

    return status;
}

/*!
 * subdeviceCtrlCmdKGrGetTpcMask
 *
 * Note:
 *   pParams->gpcId is physical GPC id for non-MIG case, but logical GPC id for
 *   MIG case.
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetTpcMask_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvU32 gpcCount;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (!IS_MIG_IN_USE(pGpu) ||
        kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        pParams->tpcMask = kgrmgrGetLegacyTpcMask(pGpu, pKernelGraphicsManager, pParams->gpcId);
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo, &pKernelGraphics));

        pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

        gpcCount = nvPopCount32(pKernelGraphicsStaticInfo->floorsweepingMasks.gpcMask);
        if (pParams->gpcId >= gpcCount)
        {
            NV_PRINTF(LEVEL_ERROR, "Incorrect GPC-Idx provided = %d\n", pParams->gpcId);
            return NV_ERR_INVALID_ARGUMENT;
        }

        pParams->tpcMask = pKernelGraphicsStaticInfo->floorsweepingMasks.tpcMask[pParams->gpcId];
    }

    return status;
}

NV_STATUS
subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    NvU32 gpcCount;

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    gpcCount = nvPopCount32(pKernelGraphicsStaticInfo->floorsweepingMasks.gpcMask);
    if (pParams->gpcId >= gpcCount)
    {
        NV_PRINTF(LEVEL_ERROR, "Incorrect GPC-Idx provided = %d\n", pParams->gpcId);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pParams->numTpcs = pKernelGraphicsStaticInfo->floorsweepingMasks.tpcCount[pParams->gpcId];

    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetPpcMask
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetPpcMask_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgrmgrGetLegacyPpcMask(pGpu, pKernelGraphicsManager, pParams->gpcId, &pParams->ppcMask));
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo, &pKernelGraphics));

        pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pPpcMasks != NULL, NV_ERR_NOT_SUPPORTED);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pGrInfo != NULL, NV_ERR_NOT_SUPPORTED);

        if (pParams->gpcId >=
            pKernelGraphicsStaticInfo->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS].data)
        {
            NV_PRINTF(LEVEL_ERROR, "Incorrect GPC-Idx provided = %d\n", pParams->gpcId);
            return NV_ERR_INVALID_ARGUMENT;
        }

        pParams->ppcMask = pKernelGraphicsStaticInfo->pPpcMasks->mask[pParams->gpcId];
    }

    return NV_OK;
}

//
// subdeviceCtrlCmdKGrFecsBindEvtbufForUid
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *pParams
)
{
    NV_STATUS status;
    RmClient *pClient;
    RsResourceRef *pEventBufferRef = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    NV_ASSERT_OK_OR_RETURN(
        serverutilGetResourceRefWithType(hClient, pParams->hEventBuffer, classId(EventBuffer), &pEventBufferRef));

    pClient = serverutilGetClientUnderLock(hClient);
    NV_ASSERT_OR_RETURN(pClient != NULL, NV_ERR_INVALID_CLIENT);

    if (bMIGInUse)
        return NV_ERR_NOT_SUPPORTED;

    status = fecsAddBindpoint(pGpu,
                              pClient,
                              pEventBufferRef,
                              pSubdevice,
                              pParams->bAllUsers,
                              pParams->levelOfDetail,
                              pParams->eventFilter,
                              1,
                              NULL);

    return status;
}

//
// subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *pParams
)
{
    NV_STATUS status;
    RmClient *pClient;
    RsResourceRef *pEventBufferRef = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    pParams->reasonCode = NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NONE;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    NV_ASSERT_OK_OR_RETURN(
        serverutilGetResourceRefWithType(hClient, pParams->hEventBuffer, classId(EventBuffer), &pEventBufferRef));

    pClient = serverutilGetClientUnderLock(hClient);
    NV_ASSERT_OR_RETURN(pClient != NULL, NV_ERR_INVALID_CLIENT);

    status = fecsAddBindpoint(pGpu,
                              pClient,
                              pEventBufferRef,
                              pSubdevice,
                              pParams->bAllUsers,
                              pParams->levelOfDetail,
                              pParams->eventFilter,
                              2,
                              &pParams->reasonCode);
    return status;
}

/*!
 * subdeviceCtrlCmdKGrGetPhysGpcMask
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvU32 grIdx = 0;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (!IS_MIG_ENABLED(pGpu))
    {
        grIdx = 0;
    }
    //
    // if MIG is enabled we follow below policies:
    // For device level monitoring with no subscription - Return GPC mask for
    //                                                    requested syspipe
    // For valid subscription - Return physical GPC mask after validating that
    //                           a physical syspipe exist in given GPU instance
    //
    else if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        NV_ASSERT_OR_RETURN(pParams->physSyspipeId < GPU_MAX_GRS, NV_ERR_INVALID_ARGUMENT);
        grIdx = pParams->physSyspipeId;
    }
    else
    {
        MIG_INSTANCE_REF ref;
        RM_ENGINE_TYPE localRmEngineType;

        //
        // Get the relevant subscription and see if provided physicalId is
        // valid in defined GPU instance
        //
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                              pDevice, &ref));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                                                RM_ENGINE_TYPE_GR(pParams->physSyspipeId),
                                                                &localRmEngineType));
        // Not failing above means physSyspipeId is valid in GPU instance
        grIdx = pParams->physSyspipeId;
    }

    pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
    NV_ASSERT_OR_RETURN(pKernelGraphics != NULL, NV_ERR_INVALID_STATE);
    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    pParams->gpcMask = pKernelGraphicsStaticInfo->floorsweepingMasks.physGpcMask;

    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetZcullMask_IMPL
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetZcullMask_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        pParams->zcullMask = kgrmgrGetLegacyZcullMask(pGpu, pKernelGraphicsManager, pParams->gpcId);
    }
    else
    {
        portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

        pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

        if (pParams->gpcId >=
            pKernelGraphicsStaticInfo->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS].data)
        {
            NV_PRINTF(LEVEL_ERROR, "Incorrect GPC-Idx provided = %d\n", pParams->gpcId);
            return NV_ERR_INVALID_ARGUMENT;
        }

        if (pKernelGraphicsStaticInfo->floorsweepingMasks.zcullMask[pParams->gpcId] == NV_U32_MAX)
        {
            return NV_ERR_NOT_SUPPORTED;
        }
        else
        {
            pParams->zcullMask = pKernelGraphicsStaticInfo->floorsweepingMasks.zcullMask[pParams->gpcId];
        }
    }

    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetZcullInfo
 *
 * Lock Requirements:
 *      Assert that API lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetZcullInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (pKernelGraphicsManager == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pZcullInfo != NULL, NV_ERR_NOT_SUPPORTED);

    portMemCopy(pParams,
                sizeof(*pParams),
                pKernelGraphicsStaticInfo->pZcullInfo,
                sizeof(*pKernelGraphicsStaticInfo->pZcullInfo));

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdKGrCtxswPmMode_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status = NV_OK;

    if (IS_GSP_CLIENT(pGpu))
    {
        NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = pParams->grRouteInfo;
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        KernelGraphics *pKernelGraphics;
        KernelChannel *pKernelChannel;
        KernelGraphicsContext *pKernelGraphicsContext;
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
            NV_ERR_INVALID_LOCK_STATE);

        if (pParams->pmMode != NV2080_CTRL_CTXSW_PM_MODE_NO_CTXSW)
        {
            kgrmgrCtrlSetChannelHandle(pParams->hChannel, &grRouteInfo);
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager,
                                             GPU_RES_GET_DEVICE(pSubdevice),
                                             &grRouteInfo,
                                             &pKernelGraphics));

            // Retrieve channel from either bare channel or TSG handle
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kchannelGetFromDualHandleRestricted(RES_GET_CLIENT(pSubdevice),
                    pParams->hChannel, &pKernelChannel));

            NV_ASSERT_OK_OR_RETURN(
                kgrctxFromKernelChannel(pKernelChannel, &pKernelGraphicsContext));

            // Setup / promote the PM ctx buffer if required
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kgrctxSetupDeferredPmBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel));
        }

        status = pRmApi->Control(pRmApi,
                                 RES_GET_CLIENT_HANDLE(pSubdevice),
                                 RES_GET_HANDLE(pSubdevice),
                                 NV2080_CTRL_CMD_GR_CTXSW_PM_MODE,
                                 pParams,
                                 sizeof(*pParams));
    }

    return status;
}

/*!
 * @brief Gets information about ROPs.
 *
 *        Lock Requirements:
 *          Assert that API and Gpus lock held on entry
 *
 * @return    NV_OK if success. Error otherwise.
 */
NV_STATUS
subdeviceCtrlCmdKGrGetROPInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *pRopInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pRopInfo != NULL, NV_ERR_NOT_SUPPORTED);

    portMemCopy(pRopInfoParams,
                sizeof(*pRopInfoParams),
                pKernelGraphicsStaticInfo->pRopInfo,
                sizeof(*pKernelGraphicsStaticInfo->pRopInfo));

    return NV_OK;
}

/*!
 * @brief Gets the current attribute buffer size.
 *
 * Lock Requirements:
 *  Assert that API lock held on entry
 *
 * @return    NV_OK if success. Error otherwise.
 */
NV_STATUS
subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *pAttribBufferSizeParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NvU32 engineId;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

    // Verify static info is available
    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    if (pKernelGraphicsStaticInfo->pContextBuffersInfo == NULL)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsInitializeDeferredStaticData(pGpu, pKernelGraphics, NV01_NULL_OBJECT, NV01_NULL_OBJECT));

        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);
    }

    engineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ATTRIBUTE_CB;
    pAttribBufferSizeParams->attribBufferSize = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[engineId].size;

    return NV_OK;
}

/*!
 * subdeviceCtrlCmdKGrGetEngineContextProperties
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NvU32 size = 0;
    NvU32 alignment = RM_PAGE_SIZE;
    NvU32 engineId;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    engineId = DRF_VAL(0080_CTRL_FIFO, _GET_ENGINE_CONTEXT_PROPERTIES, _ENGINE_ID, pParams->engineId);

    if (engineId >= NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice,
                                     &pParams->grRouteInfo, &pKernelGraphics));

    // Verify static info is available
    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    if (pKernelGraphicsStaticInfo->pContextBuffersInfo == NULL)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsInitializeDeferredStaticData(pGpu, pKernelGraphics, NV01_NULL_OBJECT, NV01_NULL_OBJECT));

        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);
    }

    size = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[engineId].size;
    alignment = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[engineId].alignment;

    if (size == NV_U32_MAX)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pParams->bInfoPopulated)
    {
        size = NV_MAX(size, pParams->size);
        alignment = NV_MAX(alignment, pParams->alignment);
    }

    pParams->size = size;
    pParams->alignment = alignment;
    pParams->bInfoPopulated = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Gets the Graphics Context buffer size and alignment
 *
 *        Lock Requirements:
 *          Assert that API and Gpus lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphics *pKernelGraphics;
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV2080_CTRL_GR_CTX_BUFFER_INFO *pCtxBufferInfo;
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    KernelChannel *pKernelChannel;
    KernelGraphicsContext *pKernelGraphicsContext;
    NvU32 bufferCount;
    NvU64 totalBufferSize;
    NvU64 prevAlignment;
    NvU32 i;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to fetch the total GR Context Buffer Size.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    kgrmgrCtrlSetChannelHandle(pParams->hChannel, &grRouteInfo);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice,
                                     &grRouteInfo, &pKernelGraphics));

    // Get channel from provided handle and owner client
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        CliGetKernelChannel(pClient, pParams->hChannel, &pKernelChannel));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxFromKernelChannel(pKernelChannel, &pKernelGraphicsContext));

    // Get the total buffer count
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetBufferCount(pGpu, pKernelGraphicsContext, pKernelGraphics, &bufferCount));

    pCtxBufferInfo = portMemAllocNonPaged(bufferCount * sizeof(NV2080_CTRL_GR_CTX_BUFFER_INFO));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
                     pCtxBufferInfo != NULL,
                     status = NV_ERR_NO_MEMORY;
                     goto done;);
    portMemSet(pCtxBufferInfo, 0, bufferCount * sizeof(NV2080_CTRL_GR_CTX_BUFFER_INFO));

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        kgrctxGetCtxBufferInfo(pGpu,
                               pKernelGraphicsContext,
                               pKernelGraphics,
                               kchannelGetGfid(pKernelChannel),
                               bufferCount,
                               &bufferCount,
                               pCtxBufferInfo),
        done);

    //
    // Calculate total size by walking thru all buffers & alignments. Adjust the total size
    // by adding the respective alignment so that the mapping VA can be adjusted.
    //
    totalBufferSize = 0;
    prevAlignment = 0x0;
    for (i = 0; i < bufferCount; i++)
    {
        if (prevAlignment != pCtxBufferInfo[i].alignment)
        {
            totalBufferSize += pCtxBufferInfo[i].alignment;
            prevAlignment = pCtxBufferInfo[i].alignment;
        }

        totalBufferSize += (pCtxBufferInfo[i].alignment != 0x0) ?
                NV_ALIGN_UP(pCtxBufferInfo[i].size, pCtxBufferInfo[i].alignment) : pCtxBufferInfo[i].size;
    }

    pParams->totalBufferSize = totalBufferSize;

done:
    portMemFree(pCtxBufferInfo);
    return status;
}

/*!
 * @brief Gets the Graphics Context buffer info like opaque buffer pointer
 *        size, alignment, aperture, allocation contiguity etc.
 *
 *        Lock Requirements:
 *          Assert that API and Gpus lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    RsClient *pUserClient;
    KernelGraphics *pKernelGraphics;
    KernelChannel *pKernelChannel;
    KernelGraphicsContext *pKernelGraphicsContext;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get Graphics context buffers information.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        serverGetClientUnderLock(&g_resServ, pParams->hUserClient, &pUserClient));

    // Get channel from provided handle and owner client
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        CliGetKernelChannel(pUserClient, pParams->hChannel, &pKernelChannel));

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    kgrmgrCtrlSetChannelHandle(pParams->hChannel, &grRouteInfo);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager,
                                     GPU_RES_GET_DEVICE(pKernelChannel),
                                     &grRouteInfo, &pKernelGraphics));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxFromKernelChannel(pKernelChannel, &pKernelGraphicsContext));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetCtxBufferInfo(pGpu,
                               pKernelGraphicsContext,
                               pKernelGraphics,
                               kchannelGetGfid(pKernelChannel),
                               NV_ARRAY_ELEMENTS(pParams->ctxBufferInfo),
                               &pParams->bufferCount,
                               pParams->ctxBufferInfo));

    return status;
}

/*!
 * subdeviceCtrlCmdKGrInternalGetCtxBufferPtes
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    RsClient *pUserClient;
    KernelGraphics *pKernelGraphics;
    KernelChannel *pKernelChannel;
    KernelGraphicsContext *pKernelGraphicsContext;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    //
    // Currently, ROUTE_TO_VGPU_HOST instructs resource server to call the RPC
    // on all vGPU configurations including SRIOV Standard which is not required.
    // Hence, manually dispatching the RPC for required vGPU configs.
    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get Graphics context buffers PTEs information.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
        NvHandle hObject = RES_GET_HANDLE(pSubdevice);

        NV_RM_RPC_CONTROL(pGpu,
                          hClient,
                          hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        serverGetClientUnderLock(&g_resServ, pParams->hUserClient, &pUserClient));

    // Get channel from provided handle and owner client
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        CliGetKernelChannel(pUserClient, pParams->hChannel, &pKernelChannel));

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    kgrmgrCtrlSetChannelHandle(pParams->hChannel, &grRouteInfo);
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager,
                                     GPU_RES_GET_DEVICE(pKernelChannel),
                                     &grRouteInfo, &pKernelGraphics));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxFromKernelChannel(pKernelChannel, &pKernelGraphicsContext));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetCtxBufferPtes(pGpu,
                               pKernelGraphicsContext,
                               pKernelGraphics,
                               kchannelGetGfid(pKernelChannel),
                               pParams->bufferType,
                               pParams->firstPage,
                               pParams->physAddrs,
                               NV_ARRAY_ELEMENTS(pParams->physAddrs),
                               &pParams->numPages,
                               &pParams->bNoMorePages));

    return status;
}

/*!
 * subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 */
NV_STATUS
subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice,
                                     &pParams->grRouteInfo, &pKernelGraphics));

    // Verify static info is available
    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    pParams->physGfxGpcMask = pKernelGraphicsStaticInfo->floorsweepingMasks.physGfxGpcMask;
    pParams->numGfxTpc = pKernelGraphicsStaticInfo->floorsweepingMasks.numGfxTpc;

    return NV_OK;
}

#define KGR_DO_WITH_GR(pGpu, pKernelGraphics, body) do                                            \
    {                                                                                             \
        (body);                                                                                   \
    } while (0);

NV_STATUS
subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NV_STATUS status = NV_OK;
    KernelGraphics *pKernelGraphics;

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), 
        NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice,
                                     &pParams->grRouteInfo, &pKernelGraphics));

    KGR_DO_WITH_GR(pGpu, pKernelGraphics,
                   kgraphicsSetFecsTraceHwEnable_HAL(pGpu, pKernelGraphics, pParams->bEnable));
    pKernelGraphics->bCtxswLoggingEnabled = pParams->bEnable;

    return status;
}

NV_STATUS
subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NV_STATUS status = NV_OK;
    KernelGraphics *pKernelGraphics;

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), 
        NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo,
                                     &pKernelGraphics));

    KGR_DO_WITH_GR(pGpu, pKernelGraphics,
                   pParams->bEnable = kgraphicsIsCtxswLoggingEnabled(pGpu, pKernelGraphics));

    return status;
}

NV_STATUS
subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NV_STATUS status = NV_OK;
    KernelGraphics *pKernelGraphics;

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), 
        NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo,
                                     &pKernelGraphics));

    KGR_DO_WITH_GR(pGpu, pKernelGraphics,
                   kgraphicsSetFecsTraceRdOffset_HAL(pGpu, pKernelGraphics, pParams->offset));

    return status;
}

NV_STATUS
subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NV_STATUS status = NV_OK;
    KernelGraphics *pKernelGraphics;

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), 
        NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo,
                                     &pKernelGraphics));

    KGR_DO_WITH_GR(pGpu, pKernelGraphics,
                   pParams->offset = kgraphicsGetFecsTraceRdOffset_HAL(pGpu, pKernelGraphics));

    return status;
}

NV_STATUS
subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NV_STATUS status = NV_OK;
    KernelGraphics *pKernelGraphics;

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), 
        NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &pParams->grRouteInfo,
                                     &pKernelGraphics));

    KGR_DO_WITH_GR(pGpu, pKernelGraphics,
                   kgraphicsSetFecsTraceWrOffset_HAL(pGpu, pKernelGraphics, pParams->offset));

    return status;
}

NvBool kgraphicsIsCtxswLoggingEnabled_FWCLIENT(OBJGPU *pGpu, KernelGraphics *pKernelGraphics)
{
    NvBool bEnabled = NV_FALSE;

    // Skip on CC
    if (pGpu->userSharedData.pMapBuffer != NULL)
    {
        RUSD_GR_INFO grInfo;

        RUSD_READ_DATA((NV00DE_SHARED_DATA*)(pGpu->userSharedData.pMapBuffer), grInfo, &grInfo);
        bEnabled = grInfo.bCtxswLoggingEnabled;
    }

    pKernelGraphics->bCtxswLoggingEnabled = bEnabled;
    return pKernelGraphics->bCtxswLoggingEnabled;
}

