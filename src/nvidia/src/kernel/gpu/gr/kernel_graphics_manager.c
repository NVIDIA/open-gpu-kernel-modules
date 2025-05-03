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

#define NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/gr/fecs_event_list.h"

#include "kernel/gpu/device/device.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"

#include "kernel/rmapi/client.h"
#include "kernel/rmapi/client_resource.h"
#include "nvRmReg.h"

// COMPUTE
#include "class/clb0c0.h"
#include "class/clb1c0.h"
#include "class/clc0c0.h"
#include "class/clc1c0.h"
#include "class/clc3c0.h"
#include "class/clc5c0.h"
#include "class/clc6c0.h"
#include "class/clc7c0.h"
#include "class/clc9c0.h"
#include "class/clcbc0.h"
#include "class/clcdc0.h"
#include "class/clcec0.h"
// GFX
#include "class/clb097.h"
#include "class/clb197.h"
#include "class/clc097.h"
#include "class/clc197.h"
#include "class/clc397.h"
#include "class/clc597.h"
#include "class/clc697.h"
#include "class/clc797.h"
#include "class/clc997.h"
#include "class/clcb97.h"
#include "class/clcd97.h"
#include "class/clce97.h"
// TWOD
#include "class/cl902d.h"

// MEM2MEM
#include "class/cla140.h"
#include "class/clcd40.h"

static NvBool
_kgrmgrGPUInstanceHasComputeInstances
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_FALSE);
    NvU32 computeInstanceIdx;

    for (computeInstanceIdx = 0;
         computeInstanceIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
         ++computeInstanceIdx)
    {
        if (pKernelMIGGpuInstance->MIGComputeInstance[computeInstanceIdx].bValid)
            return NV_TRUE;
    }

    return NV_FALSE;
}

/*!
 * @brief Get GR object type from the class number
 *
 * @param[IN]   classNum        external class number
 * @param[OUT]  pObjectType     GR class subtype
 */
void
kgrmgrGetGrObjectType_IMPL
(
    NvU32 classNum,
    NvU32 *pObjectType
)
{
    switch (classNum)
    {
        case MAXWELL_COMPUTE_A:
        case MAXWELL_COMPUTE_B:
        case PASCAL_COMPUTE_A:
        case PASCAL_COMPUTE_B:
        case VOLTA_COMPUTE_A:
        case TURING_COMPUTE_A:
        case AMPERE_COMPUTE_A:
        case AMPERE_COMPUTE_B:
        case ADA_COMPUTE_A:
        case HOPPER_COMPUTE_A:
        case BLACKWELL_COMPUTE_A:
        case BLACKWELL_COMPUTE_B:
            *pObjectType = GR_OBJECT_TYPE_COMPUTE;
            break;
        case MAXWELL_A:
        case MAXWELL_B:
        case PASCAL_A:
        case PASCAL_B:
        case VOLTA_A:
        case TURING_A:
        case AMPERE_A:
        case AMPERE_B:
        case ADA_A:
        case HOPPER_A:
        case BLACKWELL_A:
        case BLACKWELL_B:
            *pObjectType = GR_OBJECT_TYPE_3D;
            break;
        case FERMI_TWOD_A:
            *pObjectType = GR_OBJECT_TYPE_2D;
            break;
        case KEPLER_INLINE_TO_MEMORY_B:
        case BLACKWELL_INLINE_TO_MEMORY_A:
            *pObjectType = GR_OBJECT_TYPE_MEM;
            break;
        default:
            *pObjectType = GR_OBJECT_TYPE_INVALID;
            break;
    }
}

/*!
 * @brief Is local ctx buffer supported
 *
 * @param[IN]   bufId             buffer Id
 * @param[IN]   bClassSupported2D Is 2D class supported
 */
NvBool
kgrmgrIsCtxBufSupported_IMPL
(
    GR_CTX_BUFFER bufId,
    NvBool bClassSupported2D
)
{
    NvBool bSupported = NV_FALSE;

    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_CTX_BUFFER, bufId), NV_FALSE);

    // All buffers are supported when 2D class is supported
    if (bClassSupported2D)
    {
        return NV_TRUE;
    }

    switch (bufId)
    {
        case GR_CTX_BUFFER_ZCULL:
            // fall-through
        case GR_CTX_BUFFER_PREEMPT:
            // fall-through
        case GR_CTX_BUFFER_SPILL:
            // fall-through
        case GR_CTX_BUFFER_BETA_CB:
            // fall-through
        case GR_CTX_BUFFER_PAGEPOOL:
            // fall-through
        case GR_CTX_BUFFER_RTV_CB:
            // fall-through
        case GR_CTX_BUFFER_SETUP:
            bSupported = NV_FALSE;
            break;

        case GR_CTX_BUFFER_PM:
            // fall-through
        case GR_CTX_BUFFER_MAIN:
            // fall-through
        case GR_CTX_BUFFER_PATCH:
            bSupported = NV_TRUE;
            break;

        // No default case - compiler enforces update if enum changes
    }
    return bSupported;
}

/*!
 * @brief Is globalctx buffer supported
 *
 * @param[IN]   bufId             buffer Id
 * @param[IN]   bClassSupported2D Is 2D class supported
 */
NvBool
kgrmgrIsGlobalCtxBufSupported_IMPL
(
    GR_GLOBALCTX_BUFFER bufId,
    NvBool bClassSupported2D
)
{
    NvBool bSupported = NV_FALSE;

    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_GLOBALCTX_BUFFER, bufId), NV_FALSE);

    // All buffers are supported when 2D class is supported
    if (bClassSupported2D)
    {
        return NV_TRUE;
    }

    switch (bufId)
    {
        case GR_GLOBALCTX_BUFFER_BUNDLE_CB:
            // fall-through
        case GR_GLOBALCTX_BUFFER_PAGEPOOL:
            // fall-through
        case GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB:
            // fall-through
        case GR_GLOBALCTX_BUFFER_RTV_CB:
            // fall-through
        case GR_GLOBALCTX_BUFFER_GFXP_POOL:
            // fall-through
        case GR_GLOBALCTX_BUFFER_GFXP_CTRL_BLK:
            bSupported = NV_FALSE;
            break;

        case GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP:
            // fall-through
        case GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP:
            // fall-through
        case GR_GLOBALCTX_BUFFER_FECS_EVENT:
            // fall-through
        case GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP:
            bSupported = NV_TRUE;
            break;

        // No default case - compiler enforces update if enum changes
    }

    return bSupported;
}

/*!
 * @brief Sets swizzID and engineID on routing info if they don't already exist.
 *
 * @param[in]      engID            GR engine ID
 * @param[in, out] pRouteInfo       Client provided routing info
 */
void
kgrmgrCtrlSetEngineID_IMPL
(
    NvU32 engID,
    NV2080_CTRL_GR_ROUTE_INFO *pRouteInfo
)
{
    if (NULL == pRouteInfo)
    {
        return;
    }
    else if (NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_NONE ==
             DRF_VAL(2080_CTRL_GR, _ROUTE_INFO_FLAGS, _TYPE, pRouteInfo->flags))
    {
        pRouteInfo->flags = DRF_DEF(2080_CTRL_GR, _ROUTE_INFO_FLAGS, _TYPE, _ENGID);
        pRouteInfo->route = DRF_NUM64(2080_CTRL_GR, _ROUTE_INFO_DATA, _ENGID, engID);
    }
}

/*!
 * @brief Sets channel handle on routing info if it doesn't already exist.
 *
 * @param[in]      hChannel         Channel handle
 * @param[in, out] pRouteInfo       Client provided routing info
 */
void
kgrmgrCtrlSetChannelHandle_IMPL
(
    NvHandle hChannel,
    NV2080_CTRL_GR_ROUTE_INFO *pRouteInfo
)
{
    if (NULL == pRouteInfo)
    {
        return;
    }
    else if (NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_NONE ==
             DRF_VAL(2080_CTRL_GR, _ROUTE_INFO_FLAGS, _TYPE, pRouteInfo->flags))
    {
        pRouteInfo->flags = DRF_DEF(2080_CTRL_GR, _ROUTE_INFO_FLAGS, _TYPE, _CHANNEL);
        pRouteInfo->route = DRF_NUM64(2080_CTRL_GR, _ROUTE_INFO_DATA, _CHANNEL_HANDLE, hChannel);
    }
}

static void
_kgrmgrInitRegistryOverrides
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
}

NV_STATUS
kgrmgrConstructEngine_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    ENGDESCRIPTOR          engDesc
)
{
    _kgrmgrInitRegistryOverrides(pGpu, pKernelGraphicsManager);

    fecsGlobalLoggingInit(pGpu, pKernelGraphicsManager, &pKernelGraphicsManager->pFecsGlobalTraceInfo);

    return NV_OK;
}

void
kgrmgrDestruct_IMPL
(
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelGraphicsManager);

    fecsGlobalLoggingTeardown(pGpu, pKernelGraphicsManager);

    portMemSet(&pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks, 0,
           sizeof(pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks));
    portMemFree(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks);
    pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks = NULL;
    portMemFree(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo);
    pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo = NULL;
    pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized = NV_FALSE;
}

void
kgrmgrStateDestroy_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    fecsRemoveAllBindpointsForGpu(pGpu);
}

/*!
 * @brief Set legacy Kgraphics Static Info (i.e. state of GR0)
 */
void
kgrmgrSetLegacyKgraphicsStaticInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    KernelGraphics *pKernelGraphics
)
{
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, !pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized);
    NV_ASSERT_OR_RETURN_VOID((pKernelGraphics != NULL) && (kgraphicsGetInstance(pGpu, pKernelGraphics) == 0));
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN_VOID(pKernelGraphicsStaticInfo != NULL);

    portMemCopy(&pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks, sizeof(pKernelGraphicsStaticInfo->floorsweepingMasks),
                &pKernelGraphicsStaticInfo->floorsweepingMasks, sizeof(pKernelGraphicsStaticInfo->floorsweepingMasks));

    if (pKernelGraphicsStaticInfo->pPpcMasks != NULL)
    {
        pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks = portMemAllocNonPaged(sizeof(*pKernelGraphicsStaticInfo->pPpcMasks));
        NV_ASSERT_OR_GOTO(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks != NULL, cleanup);

        portMemCopy(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks, sizeof(*pKernelGraphicsStaticInfo->pPpcMasks),
                    pKernelGraphicsStaticInfo->pPpcMasks, sizeof(*pKernelGraphicsStaticInfo->pPpcMasks));
    }

    if (pKernelGraphicsStaticInfo->pGrInfo != NULL)
    {
        pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo = portMemAllocNonPaged(sizeof(*pKernelGraphicsStaticInfo->pGrInfo));
        NV_ASSERT_OR_GOTO(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo != NULL, cleanup);

        portMemCopy(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo, sizeof(*pKernelGraphicsStaticInfo->pGrInfo),
                    pKernelGraphicsStaticInfo->pGrInfo, sizeof(*pKernelGraphicsStaticInfo->pGrInfo));
    }

    pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized = NV_TRUE;
    return;

cleanup:
    portMemFree(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks);
    pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks = NULL;
    portMemFree(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo);
    pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo = NULL;
}

/*!
 * @brief Retrieves associated KernelGraphics engine for given device / route info
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphicsManager
 * @param[in]  pDevice
 * @param[in]  grRouteInfo                   Client-provided info to direct GR accesses
 * @param[out] ppKernelGraphics (Optional)   Ptr to store appropriate KernelGraphics *, if desired.
 */
NV_STATUS
kgrmgrCtrlRouteKGRWithDevice_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    Device *pDevice,
    const NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo,
    KernelGraphics **ppKernelGraphics
)
{
    MIG_INSTANCE_REF ref;
    KernelGraphics *pKernelGraphics;
    NvU32 type;
    NV_STATUS status = NV_OK;
    NvU32 grIdx;
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = *pGrRouteInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    RsClient *pClient;

    if (!IS_MIG_IN_USE(pGpu))
    {
        grIdx = 0;
        goto done;
    }

    pClient = RES_GET_CLIENT(pDevice);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

    //
    // Compute instances always have 1 GR engine, so automatically fill in
    // the route info when subscribed to a compute instance
    //
    if (ref.pMIGComputeInstance != NULL)
    {
        portMemSet(&grRouteInfo, 0, sizeof(NV2080_CTRL_GR_ROUTE_INFO));
        kgrmgrCtrlSetEngineID(0, &grRouteInfo);
    }
    else
    {
        RmClient *pRmClient = dynamicCast(pClient, RmClient);

        if (pRmClient == NULL)
            return NV_ERR_INVALID_OBJECT_HANDLE;

        RS_PRIV_LEVEL privLevel = rmclientGetCachedPrivilege(pRmClient);
        if (!rmclientIsAdmin(pRmClient, privLevel) &&
             _kgrmgrGPUInstanceHasComputeInstances(pGpu, pKernelGraphicsManager, ref.pKernelMIGGpuInstance))
        {
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }

    type = DRF_VAL(2080_CTRL_GR, _ROUTE_INFO_FLAGS, _TYPE, grRouteInfo.flags);
    switch (type)
    {
        case NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_NONE:
            NV_PRINTF(LEVEL_ERROR,
                      "Cannot give GR Route flag of TYPE_NONE with MIG enabled!\n");
            return NV_ERR_INVALID_ARGUMENT;

        case NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_ENGID:
        {
            NvU32 localGrIdx = DRF_VAL64(2080_CTRL_GR, _ROUTE_INFO_DATA,
                                         _ENGID, grRouteInfo.route);
            RM_ENGINE_TYPE globalRmEngType;

            NV_CHECK_OK_OR_RETURN(
                LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                                  RM_ENGINE_TYPE_GR(localGrIdx),
                                                  &globalRmEngType));
            NV_ASSERT_OR_RETURN(RM_ENGINE_TYPE_IS_GR(globalRmEngType), NV_ERR_INVALID_STATE);
            grIdx = RM_ENGINE_TYPE_GR_IDX(globalRmEngType);

            break;
        }

        case NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_CHANNEL:
        {
            KernelChannel *pKernelChannel;
            RM_ENGINE_TYPE rmEngineType;
            NvHandle hChannel = DRF_VAL64(2080_CTRL_GR, _ROUTE_INFO_DATA,
                                          _CHANNEL_HANDLE, grRouteInfo.route);

            status = CliGetKernelChannel(pClient, hChannel, &pKernelChannel);
            if (status != NV_OK)
            {
                RsResourceRef         *pChanGrpRef;
                KernelChannelGroupApi *pKernelChannelGroupApi = NULL;
                KernelChannelGroup    *pKernelChannelGroup    = NULL;

                //
                // If retrieving a channel with the given hChannel doesn't work,
                // try interpreting it as a handle to a channel group instead.
                //
                status = CliGetChannelGroup(pClient->hClient, hChannel,
                    &pChanGrpRef, NULL);
                if (NV_OK != status)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to find a channel or TSG with given handle 0x%08x associated with hClient=0x%08x\n",
                              hChannel, pClient->hClient);
                    return NV_ERR_INVALID_ARGUMENT;
                }

                pKernelChannelGroupApi = dynamicCast(pChanGrpRef->pResource,
                                                     KernelChannelGroupApi);
                NV_ASSERT_OR_RETURN(
                    (pKernelChannelGroupApi != NULL &&
                     pKernelChannelGroupApi->pKernelChannelGroup != NULL),
                    NV_ERR_INVALID_STATE);
                pKernelChannelGroup =
                    pKernelChannelGroupApi->pKernelChannelGroup;

                NV_PRINTF(LEVEL_INFO,
                          "Found TSG with given handle 0x%08x, using this to determine GR engine ID\n",
                          hChannel);
                rmEngineType = pKernelChannelGroup->engineType;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO,
                          "Found channel with given handle 0x%08x, using this to determine GR engine ID\n",
                          hChannel);
                rmEngineType = kchannelGetEngineType(pKernelChannel);
            }

            if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to route GR using non-GR engine type 0x%x (0x%x)\n",
                          gpuGetNv2080EngineType(rmEngineType), rmEngineType);
                return NV_ERR_INVALID_ARGUMENT;
            }

            grIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);
            break;
        }
        default:
            NV_PRINTF(LEVEL_ERROR,
                      "Unrecognized GR Route flag type 0x%x!\n", type);
            return NV_ERR_INVALID_ARGUMENT;
    }

done:
    pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
    NV_ASSERT_OR_RETURN(pKernelGraphics != NULL, NV_ERR_INVALID_STATE);

    if (ppKernelGraphics != NULL)
        *ppKernelGraphics = pKernelGraphics;

    return status;
}

/*!
 * @return legacy GPC mask enumerated by this chip
 */
NvU32
kgrmgrGetLegacyGpcMask_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized, 0);

    return pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks.gpcMask;
}

/*!
 * @return legacy physical GFX GPC mask enumerated by this chip
 */
NvU32
kgrmgrGetLegacyPhysGfxGpcMask_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized, 0);

    return pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks.physGfxGpcMask;
}

/*!
 * @return legacy TPC mask for certain GPC
 *
 * @param[in]  pGpu
 * @param[in]  KernelGraphicsManager
 * @param[in]  gpcId                 Indicates logical GPC ID when MIG enabled or physical
 *                                   GPC ID when MIG disabled
 */
NvU32
kgrmgrGetLegacyTpcMask_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 gpcId
)
{
    NvU32 maxNumGpcs;

    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized, 0);
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo != NULL, 0);

    maxNumGpcs = pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS].data;
    NV_CHECK_OR_RETURN(LEVEL_ERROR, (gpcId < maxNumGpcs), 0);

    return pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks.tpcMask[gpcId];
}

/*!
 * @brief Get legacy PPC mask for certain GPC
 */
NV_STATUS
kgrmgrGetLegacyPpcMask_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 physGpcId,
    NvU32 *pPpcMask
)
{
    NvU32 maxNumGpcs;

    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo != NULL, NV_ERR_INVALID_STATE);
    maxNumGpcs = pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS].data;
    NV_CHECK_OR_RETURN(LEVEL_ERROR, (physGpcId < maxNumGpcs), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pPpcMask != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks != NULL), NV_ERR_NOT_SUPPORTED);

    *pPpcMask = pKernelGraphicsManager->legacyKgraphicsStaticInfo.pPpcMasks->mask[physGpcId];

    return NV_OK;
}

/*!
 * @brief Returns legacy zcull mask for specific gpc
 */
NvU32
kgrmgrGetLegacyZcullMask_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 physGpcId
)
{
    NvU32 maxNumGpcs;

    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized, 0);
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo != NULL, NV_ERR_INVALID_STATE);
    maxNumGpcs = pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS].data;
    NV_CHECK_OR_RETURN(LEVEL_ERROR, (physGpcId < maxNumGpcs), NV_ERR_INVALID_ARGUMENT);

    return pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks.zcullMask[physGpcId];
}

/*!
 * @brief   Function to Alloc VEIDs for a GR engine
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelGraphicsManager
 * @param[IN]   grIdx                  phys gr idx
 * @param[IN]   veidSpanOffset         Offset of spans localized to a GPU instance to make the VEID request from
 * @param[IN]   veidCount              Total VEIDs connected to this GR engine
 * @param[IN]   pKernelMIGGPUInstance
 */
NV_STATUS
kgrmgrAllocVeidsForGrIdx_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 grIdx,
    NvU32 veidSpanOffset,
    NvU32 veidCount,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance
)
{
    NvU32 veidSizePerSpan;
    NvU32 veidStart = 0;
    NvU32 veidEnd = 0;
    NvU32 GPUInstanceVeidEnd;
    NvU64 GPUInstanceVeidMask;
    NvU64 GPUInstanceFreeVeidMask;
    NvU64 reqVeidMask;

    // This GR should not be already configured to use any VEIDs
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->grIdxVeidMask[grIdx] == 0, NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(
        kgrmgrGetVeidSizePerSpan(pGpu, pKernelGraphicsManager, &veidSizePerSpan));

    // We statically assign VEIDs to a GR based on the number of GPCs connected to it
    if (veidCount % veidSizePerSpan != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "veidCount %d is not aligned to veidSizePerSpan=%d\n", veidCount, veidSizePerSpan);
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT_OR_RETURN(veidSpanOffset != KMIGMGR_SPAN_OFFSET_INVALID, NV_ERR_INVALID_ARGUMENT);

    veidStart = (veidSpanOffset * veidSizePerSpan) + pKernelMIGGPUInstance->resourceAllocation.veidOffset;
    veidEnd = veidStart + veidCount - 1;

    NV_ASSERT_OR_RETURN(veidStart < veidEnd, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(veidStart < 64, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(veidEnd < 64, NV_ERR_INVALID_ARGUMENT);

    reqVeidMask = DRF_SHIFTMASK64(veidEnd:veidStart);

    NV_ASSERT_OR_RETURN(reqVeidMask != 0x0, NV_ERR_INVALID_STATE);

    // Create a mask for VEIDs associated with this GPU instance
    GPUInstanceVeidEnd = pKernelMIGGPUInstance->resourceAllocation.veidOffset + pKernelMIGGPUInstance->resourceAllocation.veidCount - 1;
    GPUInstanceVeidMask = DRF_SHIFTMASK64(GPUInstanceVeidEnd:pKernelMIGGPUInstance->resourceAllocation.veidOffset);

    NV_ASSERT_OR_RETURN(GPUInstanceVeidMask != 0x0, NV_ERR_INVALID_STATE);

    GPUInstanceFreeVeidMask = ~(pKernelGraphicsManager->veidInUseMask) & GPUInstanceVeidMask;

    // VEID range should not overlap with existing VEIDs in use
    NV_ASSERT_OR_RETURN((pKernelGraphicsManager->veidInUseMask & reqVeidMask) == 0, NV_ERR_STATE_IN_USE);

    //
    // For swizzId 0, client can use any range of VEIDs if not used, so we don't
    // need to do the range check
    //
    NV_ASSERT_OR_RETURN(((pKernelMIGGPUInstance->swizzId == 0) ||
                         ((GPUInstanceFreeVeidMask & reqVeidMask) == reqVeidMask)),
                        NV_ERR_INSUFFICIENT_RESOURCES);

    // mark each VEID in the range as "in use"
    pKernelGraphicsManager->veidInUseMask |= reqVeidMask;
    pKernelGraphicsManager->grIdxVeidMask[grIdx] |= reqVeidMask;

    return NV_OK;
}

/*!
 * @brief   Function to Clear Gr Engine to VEIDs mapping
 */
void
kgrmgrClearVeidsForGrIdx_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 grIdx
)
{
    NvU64 veidMask = pKernelGraphicsManager->grIdxVeidMask[grIdx];

    // mark all VEIDs of this GR engine as "not in use"
    NV_ASSERT((pKernelGraphicsManager->veidInUseMask & veidMask) == veidMask);
    pKernelGraphicsManager->veidInUseMask &= ~veidMask;
    pKernelGraphicsManager->grIdxVeidMask[grIdx] = 0;
}

/*! 
 * @brief Get VEID step size
 */
NV_STATUS
kgrmgrGetVeidStepSize_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 *pVeidStepSize
)
{
    NV_ASSERT_OR_RETURN(pVeidStepSize != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo != NULL, NV_ERR_INVALID_STATE);

    *pVeidStepSize = pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_LITTER_MIN_SUBCTX_PER_SMC_ENG].data;

    return NV_OK;
}

/*!
 * @brief Get VEID size for each span
 */
NV_STATUS
kgrmgrGetVeidSizePerSpan_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 *pVeidSizePerSpan
)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE computeProfile;
    NvU32 computeSize;

    NV_ASSERT_OR_RETURN(pVeidSizePerSpan != NULL, NV_ERR_INVALID_ARGUMENT);

    computeSize = kmigmgrSmallestComputeProfileSize(pGpu, pKernelMIGManager);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, computeSize != KMIGMGR_COMPUTE_SIZE_INVALID, NV_ERR_INVALID_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager, computeSize, &computeProfile));

    // VEIDs for each span should be the VEID size we assign to the smallest GPC count
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrGetVeidsFromGpcCount_HAL(pGpu, pKernelGraphicsManager, computeProfile.gpcCount, pVeidSizePerSpan));

    return NV_OK;
}

/*!
 * @brief   Function to get starting VEID for a Gr Engine
 */
NV_STATUS
kgrmgrGetVeidBaseForGrIdx_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 grIdx,
    NvU32 *pVeidStart
)
{
    NvU64 veidMask;
    NV_ASSERT_OR_RETURN(pVeidStart != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(grIdx != KGRMGR_MAX_GR, NV_ERR_INVALID_ARGUMENT);

    *pVeidStart = 0;

    veidMask = pKernelGraphicsManager->grIdxVeidMask[grIdx];

    //
    // If a GR is not configured, VEID mask for it will be "0" and counting
    // "0" in a zero-based mask will result in max bit-width size.
    //
    if (veidMask != 0x0)
        *pVeidStart = portUtilCountTrailingZeros64(veidMask);

    return NV_OK;
}

/*!
 * @brief   Function to get GR index for a VEID
 */
NV_STATUS
kgrmgrGetGrIdxForVeid_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 veid,
    NvU32 *pGrIdx
)
{
    NvU32 i;
    NvU64 veidMask = NVBIT64(veid);

    NV_ASSERT_OR_RETURN(pGrIdx != NULL, NV_ERR_INVALID_ARGUMENT);
    for (i = 0; i < KGRMGR_MAX_GR; ++i)
    {
        if ((pKernelGraphicsManager->grIdxVeidMask[i] & veidMask) != 0)
            break;
    }
    NV_ASSERT_OR_RETURN(i != KGRMGR_MAX_GR, NV_ERR_OBJECT_NOT_FOUND);
    *pGrIdx = i;
    return NV_OK;
}

/*!
 * @brief discovers maximum size of local ctx buffers.
 */
NV_STATUS
kgrmgrDiscoverMaxLocalCtxBufInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    KernelGraphics *pKernelGraphics,
    NvU32 swizzId
)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    //
    // Most of GR is stub'd so context related things are not needed in AMODEL.
    // But this function can be called in some MODS test, so return OK directly
    // to avoid failing the test.
    //
    if (IS_MODS_AMODEL(pGpu))
        return NV_OK;

    NV_CHECK_OR_RETURN(LEVEL_SILENT,
        kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, swizzId), NV_OK);

    return kgraphicsDiscoverMaxLocalCtxBufferSize(pGpu, pKernelGraphics);
}

/*!
 * @brief Get ctxbufpool info for the global ctx buffer
 */
const CTX_BUF_INFO *
kgrmgrGetGlobalCtxBufInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    GR_GLOBALCTX_BUFFER buf
)
{
    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_GLOBALCTX_BUFFER, buf), NULL);
    return &pKernelGraphicsManager->globalCtxBufInfo[buf];
}

/*!
 * @brief Set ctxbufpool parameters for the global ctx buffer
 */
void
kgrmgrSetGlobalCtxBufInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    GR_GLOBALCTX_BUFFER buf,
    NvU64 size,
    NvU64 align,
    RM_ATTR_PAGE_SIZE attr,
    NvBool bContiguous
)
{
    CTX_BUF_INFO *pInfo;
    NV_ASSERT_OR_RETURN_VOID(NV_ENUM_IS(GR_GLOBALCTX_BUFFER, buf));

    pInfo = &pKernelGraphicsManager->globalCtxBufInfo[buf];
    pInfo->size    = size;
    pInfo->align   = align;
    pInfo->attr    = attr;
    pInfo->bContig = bContiguous;
}

/*!
 * @brief Gets maximum size of GR global ctx buffers.
 *        These are sizes of buffer for GR0 in legacy mode with all GPCs
 *        connected to GR0.
 */
NV_STATUS
kgrmgrDiscoverMaxGlobalCtxBufSizes_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    KernelGraphics *pKernelGraphics,
    NvBool bMemoryPartitioningNeeded
)
{
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    GR_GLOBALCTX_BUFFER bufId;

    //
    // Most of GR is stub'd so context related things are not needed in AMODEL.
    // But this function can be called in some MODS test, so return OK directly
    // to avoid failing the test.
    //
    if (IS_MODS_AMODEL(pGpu))
        return NV_OK;

    NV_ASSERT_OR_RETURN(!IS_MIG_IN_USE(pGpu), NV_ERR_INVALID_STATE);

    //
    // Bug 2915422: Eventually we expect this check to be replaced by ctxBufPoolIsSupported
    // we can't use that check today because PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA is not enabled
    // when this function is called because
    // kgraphicsInitializeDeferredStaticData below will eventually lead to
    // global ctx buffer allocation from ctxBufPools even before these pools are
    // populated which happens later during GPU instance creation. Once we are
    // able to rip out global buffer alloc from
    // kgraphicsInitializeDeferredStaticData, we can enable the above property
    // early.
    //
    NV_CHECK_OR_RETURN(LEVEL_SILENT, bMemoryPartitioningNeeded, NV_OK);

    // Make sure sizes of all buffers are setup
    NV_ASSERT_OK_OR_RETURN(
        kgraphicsInitializeDeferredStaticData(pGpu, pKernelGraphics, NV01_NULL_OBJECT, NV01_NULL_OBJECT));

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    // Get sizes of global ctx buffers
    FOR_EACH_IN_ENUM(GR_GLOBALCTX_BUFFER, bufId)
    {
        NvU32 fifoEngineId;
        NV_ASSERT_OK_OR_RETURN(
            kgrctxGlobalCtxBufferToFifoEngineId(bufId, &fifoEngineId));

        //
        // contiguity is determined later before reservation as it depends on settings
        // that take effect after this point.
        //
        kgrmgrSetGlobalCtxBufInfo(pGpu, pKernelGraphicsManager, bufId,
                                  pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[fifoEngineId].size,
                                  pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[fifoEngineId].alignment,
                                  RM_ATTR_PAGE_SIZE_4KB, NV_FALSE);
    }
    FOR_EACH_IN_ENUM_END;

    return NV_OK;
}

/*!
 * @return legacy TPC count for certain GPC
 *
 * @param[in]  pGpu
 * @param[in]  KernelGraphicsManager
 * @param[in]  gpcId                 Indicates logical GPC ID
 */
NvU32
kgrmgrGetLegacyGpcTpcCount_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 gpcId
)
{
    NvU32 maxNumGpcs;

    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized, 0);
    NV_ASSERT_OR_RETURN(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo != NULL, 0);

    maxNumGpcs = nvPopCount32(pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks.gpcMask);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, (gpcId < maxNumGpcs), 0);

    return pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks.tpcCount[gpcId];
}

/*!
 * @brief   Function to Alloc VEIDs with either a specific placement or first-fit placement
 *          depending upon the value passed into pSpanStart
 *
 * @param[IN]     pGpu
 * @param[IN]     pKernelGraphicsManager
 * @param[IN/OUT] pInUseMask             Mask of VEIDs in-use, updated with newly set VEIDs
 * @param[IN]     veidCount              Total VEIDs connected to this GR engine
 * @param[IN/OUT] pSpanStart             GPU Instance local span offset to begin assigning VEIDs from
 * @param[IN]     pKernelMIGGPUInstance
 */
NV_STATUS
kgrmgrCheckVeidsRequest_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU64 *pInUseMask,
    NvU32 veidCount,
    NvU32 *pSpanStart,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance
)
{
    NvU32 veidSizePerSpan;
    NvU32 veidStart = 0;
    NvU32 veidEnd = 0;
    NvU32 GPUInstanceVeidEnd;
    NvU64 GPUInstanceVeidMask;
    NvU64 GPUInstanceFreeVeidMask;
    NvU64 veidMask;

    NV_ASSERT_OK_OR_RETURN(
        kgrmgrGetVeidSizePerSpan(pGpu, pKernelGraphicsManager, &veidSizePerSpan));

    NV_ASSERT_OR_RETURN(pSpanStart != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pInUseMask != NULL, NV_ERR_INVALID_ARGUMENT);

    // We statically assign VEIDs to a GR based on the number of GPCs connected to it
    if (veidCount % veidSizePerSpan != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "veidCount %d is not aligned to veidSizePerSpan=%d\n", veidCount, veidSizePerSpan);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Create a mask for VEIDs associated with this GPU instance
    GPUInstanceVeidEnd = pKernelMIGGPUInstance->resourceAllocation.veidOffset + pKernelMIGGPUInstance->resourceAllocation.veidCount - 1;
    GPUInstanceVeidMask = DRF_SHIFTMASK64(GPUInstanceVeidEnd:pKernelMIGGPUInstance->resourceAllocation.veidOffset);

    NV_ASSERT_OR_RETURN(GPUInstanceVeidMask != 0x0, NV_ERR_INVALID_STATE);

    GPUInstanceFreeVeidMask = ~(*pInUseMask) & GPUInstanceVeidMask;

    if (*pSpanStart != KMIGMGR_SPAN_OFFSET_INVALID)
    {
        veidStart = (*pSpanStart * veidSizePerSpan) + pKernelMIGGPUInstance->resourceAllocation.veidOffset;
        veidEnd = veidStart + veidCount - 1;

        NV_ASSERT_OR_RETURN(veidStart < veidEnd, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(veidStart < 64, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(veidEnd < 64, NV_ERR_INVALID_ARGUMENT);
    }
    else
    {
        NvU64 reqVeidMask = DRF_SHIFTMASK64(veidCount - 1:0);
        NvU32 i;

        for (i = pKernelMIGGPUInstance->resourceAllocation.veidOffset; i <= GPUInstanceVeidEnd; i += veidSizePerSpan)
        {
            // See if requested slots are available within this range
            if (((GPUInstanceFreeVeidMask >> i) & reqVeidMask) == reqVeidMask)
            {
                veidStart = i;
                veidEnd = veidStart + veidCount - 1;
                break;
            }
        }

        NV_CHECK_OR_RETURN(LEVEL_SILENT, i <= GPUInstanceVeidEnd,
                           NV_ERR_INSUFFICIENT_RESOURCES);
    }

    veidMask = DRF_SHIFTMASK64(veidEnd:veidStart);
    NV_ASSERT_OR_RETURN(veidMask != 0x0, NV_ERR_INVALID_STATE);

    //
    // For swizzId 0, client can use any range of VEIDs if not used, so we don't
    // need to do the range check
    //
    NV_CHECK_OR_RETURN(LEVEL_ERROR, 
                       ((pKernelMIGGPUInstance->swizzId == 0) ||
                        (GPUInstanceVeidMask & veidMask) == veidMask),
                       NV_ERR_INSUFFICIENT_RESOURCES);

    // VEID range should not overlap with existing VEIDs in use
    NV_ASSERT_OR_RETURN((*pInUseMask & veidMask) == 0, NV_ERR_STATE_IN_USE);

    NV_ASSERT(veidStart >= pKernelMIGGPUInstance->resourceAllocation.veidOffset);

    *pSpanStart = (veidStart - pKernelMIGGPUInstance->resourceAllocation.veidOffset) / veidSizePerSpan;
    *pInUseMask |= veidMask;
    return NV_OK;
}

