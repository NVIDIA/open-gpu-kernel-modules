/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/diagnostics/nv_debug_dump.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/bus/p2p_api.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/gpu_fabric_probe.h"
#include "events/gpu/nvlink/nvlink_events.h"
#include "nvoc/event_bus.h"
#include "rmapi/rs_utils.h"

#define NVLINK_FABRIC_HEALTH_MASK_TIMER_DELAY_NS    60000000000ULL

static void _knvlinkP2PIdleCallback(OBJGPU *pGpu, void *pArgs);
void knvlinkABM_WORKITEM(OBJGPU *pGpu, void *pArgs);

/*!
 * @brief Report a link training failure and dump error info to logs
 */
NV_STATUS
knvlinkLogAliDebugMessages_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bFinal
)
{
    NV_STATUS status;
    NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *pParams;
    NvU32 link;
    NvU8 goeFailureLinkIds[NVLINK_ALI_TRAINING_FAILURE_MAX_ENTRIES];
    NvU32 goeFailures[NVLINK_ALI_TRAINING_FAILURE_MAX_ENTRIES];
    NvU32 goeFailureCount = 0;

    pParams = portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS));
    if (pParams == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pParams, 0, sizeof(*pParams));
    pParams->ErrInfoFlags |= NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_ALI_STATUS;

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                            NV2080_CTRL_CMD_NVLINK_GET_ERR_INFO,
                            (void *)pParams,
                            sizeof(*pParams)),
        portMemFree(pParams);
        return status; );

    FOR_EACH_IN_BITVECTOR(&pKernelNvlink->postRxDetLinkMask, link)
    {
        if ((pParams->linkErrInfo[link].DLStatMN00 & 0xffff) != 0x0)
        {
            NV_PRINTF(LEVEL_ERROR, "ALI Error for GPU %d::linkId %d: 0x%x\n",
                      pGpu->gpuInstance,
                      link,
                      pParams->linkErrInfo[link].DLStatMN00);

            if (goeFailureCount < NVLINK_ALI_TRAINING_FAILURE_MAX_ENTRIES)
            {
                goeFailures[goeFailureCount] = pParams->linkErrInfo[link].DLStatMN00;
                goeFailureLinkIds[goeFailureCount] = (NvU8)link;
                goeFailureCount++;
            }
        }
    }
    FOR_EACH_IN_BITVECTOR_END();

    if (bFinal)
    {
        eventEmit(NvlinkAliTrainingFailure,
                  pKernelNvlink,
                  OPERATIONAL_EVENT_SEVERITY_FATAL,
                  goeFailureCount,
                  1,
                  goeFailureLinkIds,
                  goeFailures);
        eventEmit(NvlinkAliTrainingFailureLegacy,
                  pKernelNvlink,
                  OPERATIONAL_EVENT_SEVERITY_FATAL,
                  goeFailureCount,
                  1,
                  goeFailureLinkIds,
                  goeFailures,
                  ALI_TRAINING_FAIL);
        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_NVLINK_ERROR_FATAL, NULL, 0, 0x0, ALI_TRAINING_FAIL);
    }

    portMemFree(pParams);
    return NV_OK;
}

/*!
 * @brief Registers Callback to the existing dump framework
 */
void
knvlinkDumpCallbackRegister_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvDebugDump         *pNvd               = GPU_GET_NVD(pGpu);
    NVD_ENGINE_CALLBACK *pEngineCallback    = NULL;

    // Return if pNvd is NULL
    if (pNvd == NULL)
        return;

    // See if already registered (can be if have multiple)
    if ((nvdFindEngine(pGpu, pNvd, NVDUMP_COMPONENT_ENG_NVLINK, &pEngineCallback) == NV_OK) &&
        (pEngineCallback != NULL))
        return;

    // Required to sign up for nvlink callback in Kernel RM in order to dump contents in GSP RM
    nvdEngineSignUp(pGpu,
                    pNvd,
                    NULL,
                    NVDUMP_COMPONENT_ENG_NVLINK,
                    REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _MED) |
                    REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _GSP),
                    (void *)pKernelNvlink);
}

static NvU8
_nvlinkLinkCountToRbmMode
(
    NvU8 linkCount
)
{
    NvU8 rbmMode = 0;

    rbmMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE,
                              GPU_NVLINK_BW_MODE_LINK_COUNT,
                              rbmMode);
    rbmMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE_LINK_COUNT,
                              linkCount, rbmMode);
    return rbmMode;
}

/*!
 * Check if requested RBM mode is supported by GPU
 */
NvBool
knvlinkIsBwModeSupported_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU16    mode
)
{
    NvU32 i;
    NvU8 linkCount = DRF_VAL(_GPU, _NVLINK, _BW_MODE_LINK_COUNT, mode);

    // Legacy BW modes are not supported except _FULL
    if ((DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) != GPU_NVLINK_BW_MODE_LINK_COUNT) &&
        (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) != GPU_NVLINK_BW_MODE_FULL))
    {
        NV_PRINTF(LEVEL_ERROR, "Legacy BW modes are not supported on this platform.\n");
        return NV_FALSE;
    }

    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_FULL)
    {
        // Requesting full bandwidth on GPU
        return NV_TRUE;
    }

    // Check if requested BW mode link count is supported by GFM
    if (linkCount > pKernelNvlink->maxRbmLinks)
    {
        NV_PRINTF(LEVEL_ERROR, "RBM not supported by GFM. LinkCount: %d; MaxLinkCount: %d\n",
                  linkCount,
                  pKernelNvlink->maxRbmLinks);
        return NV_FALSE;
    }

    // Check if requested BW mode link count is supported by HSHUB
    for (i = 0; i < pKernelNvlink->totalRbmModes; i++)
    {
        if (mode == pKernelNvlink->hshubSupportedRbmModesList[i])
        {
            return NV_TRUE;
        }
    }

    NV_PRINTF(LEVEL_ERROR, "RBM requested is not supported. LinkCount: %d\n",
              linkCount);
    return NV_FALSE;
}

/*!
 * Retrieve list of HSHUB supported RBM Modes
 */
NV_STATUS
knvlinkGetHshubSupportedRbmModes_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;

    //
    // TODO: Update hardcoded list with list retrieved from HSHUB query rpc.
    // Current supported link counts: 0, 2, 4, 6, 8, 12
    //
    const NvU8 gpuNvlinkHshubSupportedRbmList[] =
    {
        _nvlinkLinkCountToRbmMode(0),
        _nvlinkLinkCountToRbmMode(2),
        _nvlinkLinkCountToRbmMode(4),
        _nvlinkLinkCountToRbmMode(6),
        _nvlinkLinkCountToRbmMode(8),
        _nvlinkLinkCountToRbmMode(12)
    };

    for (i = 0; i < NV_ARRAY_ELEMENTS(gpuNvlinkHshubSupportedRbmList); i++)
    {
        pKernelNvlink->hshubSupportedRbmModesList[i] = gpuNvlinkHshubSupportedRbmList[i];
    }
    pKernelNvlink->totalRbmModes = NV_ARRAY_ELEMENTS(gpuNvlinkHshubSupportedRbmList);

    return status;
}

/**
 * @brief Calculate the effective peer link mask for HS_HUB configuration
 *
 * @param[in]   pGpu               OBJGPU pointer of local GPU
 * @param[in]   pKernelNvlink      reference of KernelNvlink
 * @param[in]   pRemoteGpu         OBJGPU pointer of remote GPU
 * @param[in/out] pPeerLinkMask    reference of peerLinkMask
 */
void
knvlinkGetEffectivePeerLinkMask_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU *pRemoteGpu,
    NVLINK_BIT_VECTOR *pPeerLinkMask
)
{
    NVLINK_BIT_VECTOR linkMaskToBeReduced;
    NV_STATUS status = NV_OK;

    if (knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        if (gpuFabricProbeGetlinkMaskToBeReduced(pGpu->pGpuFabricProbeInfoKernel,
                                                 &linkMaskToBeReduced) == NV_OK)
        {
            NVLINK_BIT_VECTOR complementLinkMaskToBeReducedVec;
            NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
                bitVectorComplement(&complementLinkMaskToBeReducedVec, &linkMaskToBeReduced),
                return; );
            NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
                bitVectorAnd(pPeerLinkMask, pPeerLinkMask, &complementLinkMaskToBeReducedVec),
                return; );
            NV_PRINTF(LEVEL_INFO, "Reducing nvlinkMask from "NV_BITVECTOR_INLINE_FMTX" to updated "NV_BITVECTOR_INLINE_FMTX"\n",
                NV_BITVECTOR_INLINE_PRINTF_ARG(&linkMaskToBeReduced), NV_BITVECTOR_INLINE_PRINTF_ARG(pPeerLinkMask));
        }
    }
}

/*!
 * Retrieve list of supported BW modes
 */
NV_STATUS
knvlinkGetSupportedBwMode_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams
)
{
    NvU32 i;

    for (i = 0; i < pKernelNvlink->totalRbmModes; i++)
    {
        // Need to filter HSHUB supported list with maxRbmLinks received from probe response
        if (pKernelNvlink->hshubSupportedRbmModesList[i] <= _nvlinkLinkCountToRbmMode(pKernelNvlink->maxRbmLinks))
        {
            pParams->rbmModesList[i] = pKernelNvlink->hshubSupportedRbmModesList[i];
        }
        else
        {
            // GFM no longer supports mode in supported list. Do not copy to client.
            break;
        }
    }
    pParams->rbmTotalModes = i;

    return NV_OK;
}

NV_STATUS
knvlinkSetDirectConnectBaseAddress_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    if (gpuIsSelfHosted(pGpu))
    {
        //
        // There are 64 entries in the GPA remap table of size 4TB each.
        // For direct-connect nvlink5 systems, we use zero-based EGM GPA addresses,
        // So remap slot 0 is used by EGM, and remap slot 1 is use for vidmem
        // that requires 4TB as base address.
        //
        pKernelNvlink->vidmemDirectConnectBaseAddr = NVBIT64(42);
    }

    return NV_OK;
}

NV_STATUS
knvlinkValidateFabricBaseAddress_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricBaseAddr
)
{
    // Check if fabric address is aligned to mapslot size.
    if (fabricBaseAddr & (NVBIT64(42) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;

}

void
knvlinkABMIdle_WORKITEM
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NVLINK_BIT_VECTOR linkMask;

    NVLINK_BIT_VECTOR *pEnabledLinksVec = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);

    if (knvlinkIsP2PActive_IMPL(pGpu, pKernelNvlink))
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_TRUE);
        gpuSetRecoveryDrainP2P(pGpu, NV_TRUE);
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_FALSE);
        return;
    }

    bitVectorAnd(&linkMask, pEnabledLinksVec, &pKernelNvlink->pendingAbmLinkMaskToBeReduced);
    NV_PRINTF(LEVEL_NOTICE, "GPU%u Detected fabric idle. Applying linkMask "NV_BITVECTOR_INLINE_FMTX" and Unmarking Drain P2P.\n",
                gpuInstance, NV_BITVECTOR_INLINE_PRINTF_ARG(&linkMask));

    // Reuse linkMaskToBeReduced so RBM/ABM flows are the same
    gpuFabricProbeSetlinkMaskToBeReduced(pGpu->pGpuFabricProbeInfoKernel, &pKernelNvlink->pendingAbmLinkMaskToBeReduced);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_TRUE);
    gpuUnmarkDeviceForDrainP2P(pGpu);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_FALSE);

    osRemove1HzCallback(pGpu, knvlinkABM_WORKITEM, pArgs);

    knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
        NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_TRUE,
        NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
        NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_FALSE);
}

void
knvlinkABM_WORKITEM
(
    OBJGPU *pGpu,
    void *pArgs
)
{
    // Queue a work item to check P2P with proper locks
    NV_STATUS status = osQueueWorkItem(pGpu,
                                      knvlinkABMIdle_WORKITEM,
                                      NULL,
                                      (OsQueueWorkItemFlags){
                                          .bLockSema = NV_TRUE,
                                          .bLockGpus = NV_TRUE,
                                          .apiLock = WORKITEM_FLAGS_API_LOCK_READ_ONLY
                                      });

    if (status != NV_OK)
    {
        knvlinkSetAmapUpdateStatus(pGpu, GPU_GET_KERNEL_NVLINK(pGpu),
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_TRUE);
        NV_PRINTF(LEVEL_ERROR, "Failed to queue P2P idle check.\n");
    }
}

NV_STATUS
knvlinkABMLinkMaskUpdate_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool bNeedsRCRecovery
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    // Need to kill channels with AMAP of 16 on switch tray removal
    if (bNeedsRCRecovery)
    {
        // Perform PF GFID RC error recovery on all usermode channels.
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_NVLINK_RC_USER_MODE_CHANNELS,
                            NULL,
                            0));
    }

    // Launch repeated 1Hz workitem to await drainP2P completion and apply link mask
    (void)osSchedule1HzCallback(pGpu, knvlinkABM_WORKITEM, NULL, NV_OS_1HZ_REPEAT);

    return NV_OK;
}

static NvBool
_knvlinkIsGfmLinkMaskActive
(
    OBJGPU *pGpu
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NVLINK_BIT_VECTOR linkMaskToBeReduced;
    NVLINK_BIT_VECTOR complementLinkMaskToBeReducedVec;
    NVLINK_BIT_VECTOR activeLinkMaskVec;
    NvU32  i;
    NvBool bIsGfmLinkMaskActive = NV_TRUE;
    NV_STATUS status = NV_OK;

    NVLINK_BIT_VECTOR *pEnabledLinksVec = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    bitVectorClrAll(&activeLinkMaskVec);
    bitVectorClrAll(&complementLinkMaskToBeReducedVec);
    bitVectorClrAll(&linkMaskToBeReduced);

    // Check if active link mask matches link mask from GFM
    gpuFabricProbeGetlinkMaskToBeReduced(pGpu->pGpuFabricProbeInfoKernel, &linkMaskToBeReduced);
    bitVectorComplement(&complementLinkMaskToBeReducedVec, &linkMaskToBeReduced);
    bitVectorAnd(&activeLinkMaskVec, pEnabledLinksVec, &complementLinkMaskToBeReducedVec);

    NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS linkTrainedParams;

    portMemSet(&linkTrainedParams, 0, sizeof(linkTrainedParams));
    linkTrainedParams.bActiveOnly = NV_TRUE;

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        convertBitVectorToLinkMasks(&activeLinkMaskVec, NULL, 0, &linkTrainedParams.linkMask), return NV_FALSE;);


    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_ARE_LINKS_TRAINED,
                                     &linkTrainedParams, sizeof(linkTrainedParams)),
                                     return NV_FALSE;);

    FOR_EACH_IN_BITVECTOR(&activeLinkMaskVec, i)
    {
        if (!linkTrainedParams.bIsLinkActive[i])
        {
            bIsGfmLinkMaskActive = NV_FALSE;
            break;
        }
    }
    FOR_EACH_IN_BITVECTOR_END();

    return bIsGfmLinkMaskActive;
}

static NV_STATUS
knvlinkFabricHealthMask_WORKITEM
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU32 healthStatusMask = 0;

    // Get current fabric health status mask
    gpuFabricProbeGetFabricHealthStatus(pGpu->pGpuFabricProbeInfoKernel, &healthStatusMask);
    healthStatusMask = FLD_SET_DRF(LINK, _INBAND_FABRIC_HEALTH_MASK, _ROUTE_UPDATE,
        _FALSE, healthStatusMask);

    if (!_knvlinkIsGfmLinkMaskActive(pGpu))
    {
        healthStatusMask = FLD_SET_DRF(LINK, _INBAND_FABRIC_HEALTH_MASK, _INCORRECT_CONFIGURATION,
                                           _INSUFFICIENT_NVLINKS, healthStatusMask);
        gpuFabricProbeDegradeCliques(pGpu);
    }

    gpuFabricProbeOverrideFabricHealthStatus(pGpu->pGpuFabricProbeInfoKernel, healthStatusMask);
    tmrEventDestroy(pTmr, pEvent);
    pKernelNvlink->pFabricHealthMaskTmrEvent = NULL;
    return NV_OK;
}

NV_STATUS
knvlinkAbmFabricHealthMaskUpdate_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NV_STATUS status;

    if (!gpuFabricProbeIsReceived(pGpu->pGpuFabricProbeInfoKernel))
    {
        return NV_OK;
    }

    if (_knvlinkIsGfmLinkMaskActive(pGpu))
    {
        return NV_OK;
    }

    if (pKernelNvlink->pFabricHealthMaskTmrEvent == NULL)
    {
        NvU32 healthStatusMask = 0;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            tmrEventCreate(pTmr, &pKernelNvlink->pFabricHealthMaskTmrEvent,
                           knvlinkFabricHealthMask_WORKITEM, NULL, TMR_FLAGS_NONE));

        status = tmrEventScheduleRel(pTmr, pKernelNvlink->pFabricHealthMaskTmrEvent,
                                     NVLINK_FABRIC_HEALTH_MASK_TIMER_DELAY_NS);
        if (status != NV_OK)
        {
            tmrEventDestroy(pTmr, pKernelNvlink->pFabricHealthMaskTmrEvent);
            pKernelNvlink->pFabricHealthMaskTmrEvent = NULL;
            return status;
        }

        gpuFabricProbeGetFabricHealthStatus(pGpu->pGpuFabricProbeInfoKernel, &healthStatusMask);
        healthStatusMask = FLD_SET_DRF(LINK, _INBAND_FABRIC_HEALTH_MASK, _ROUTE_UPDATE,
                                           _TRUE, healthStatusMask);
        gpuFabricProbeOverrideFabricHealthStatus(pGpu->pGpuFabricProbeInfoKernel, healthStatusMask);
    }
    else
    {
        // Cancel running timer event if new link error occurs before 60 seconds and kick off new timer event
        tmrEventCancel(pTmr, pKernelNvlink->pFabricHealthMaskTmrEvent);
        status = tmrEventScheduleRel(pTmr, pKernelNvlink->pFabricHealthMaskTmrEvent,
                                     NVLINK_FABRIC_HEALTH_MASK_TIMER_DELAY_NS);
        if (status != NV_OK)
        {
            tmrEventDestroy(pTmr, pKernelNvlink->pFabricHealthMaskTmrEvent);
            pKernelNvlink->pFabricHealthMaskTmrEvent = NULL;
            return status;
        }
    }

    return NV_OK;
}

/*!
 * @brief   Validates fabric EGM base address.
 *
 * @param[in]  pGpu              OBJGPU pointer
 * @param[in]  pKernelNvlink     KernelNvlink pointer
 * @param[in]  fabricEgmBaseAddr Address to be validated
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkValidateFabricEgmBaseAddress_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricEgmBaseAddr
)
{
    // SW WAR for HW bug 4851258 requires fabric address to use only bits 46:44
    if ((fabricEgmBaseAddr & ~(((NvU64)0x7ULL) << 44)) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid EGM fabric address: 0x%llx\n", fabricEgmBaseAddr);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/**
 * @brief Check if ENCRYPT_EN bit is set
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_TRUE is ENCRYPT_EN is set, else NV_FALSE
 */
NV_STATUS
knvlinkGetEncryptionBits_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS params;
    NV_STATUS status;
    portMemSet(&params, 0, sizeof(params));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                NV2080_CTRL_CMD_NVLINK_GET_NVLE_ENCRYPT_EN_INFO,
                                (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute RPC to get Nvlink Encrypt Enable Info\n");
        return status;
    }

    pKernelNvlink->bMmuNvlinkEncryptEn = params.bMmuNvlinkEncryptEn;
    pKernelNvlink->bNvlinkTlwEncryptEn = params.bNvlinkTlwEncryptEn;

    return NV_OK;
}

/*!
 * @brief  Check if NVLE PDB Property is set
 *
 * @param[in]  pGpu              OBJGPU pointer
 * @param[in]  pKernelNvlink     KernelNvlink pointer
 *
 */
NvBool
knvlinkIsNvleEnabled_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS params;
    NV_STATUS status;

    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
    {
        //
        // On MODS and for NVLE Qual mode verification, just check for PDB_PROP_KNVLINK_ENCRYPTION_ENABLED,
        // for other use-cases, check the following settings as well
        //
        if (!RMCFG_FEATURE_MODS_FEATURES && !pKernelNvlink->bNvleQualModeRegkey)
        {
            //
            // Disable Nvlink encryption if :
            //   1. Nvlink TLW Encrypt Enable Bit is not set by FSP, OR
            //   2. Nvlink encryption needs to be enabled with CC, but both NVLE and CC are disabled, OR
            //   3. Nvlink encryption can be enabled without CC, but both NVLE and TLW Encrypt Enable bit are disabled
            //
            if (!pKernelNvlink->bNvlinkTlwEncryptEn
                || (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLE_ENCRYPTION_WITH_CC) &&
                    !gpuIsNvleModeEnabledInHw_HAL(pGpu) && !gpuIsCCEnabledInHw_HAL(pGpu))
                || (!pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLE_ENCRYPTION_WITH_CC) &&
                   (!gpuIsNvleModeEnabledInHw_HAL(pGpu) && !pKernelNvlink->bNvlinkTlwEncryptEn))
                )
            {
                // This is an error case
                NV_PRINTF(LEVEL_ERROR,
                          "Disabling Nvlink encryption, since, either TLW Encrypt Enable bit is not set"
                          " OR CC is disabled OR NVLE bit is not set\n");
                pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED, NV_FALSE);
            }
        }
    }

    params.bIsNvleEnabled = pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED);

    // Update NVLE enablement status in GSP-RM
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_SET_NVLE_ENABLED_STATE,
                                 (void *)&params, sizeof(params));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute RPC to set Nvlink Enablement Status\n");
        return NV_FALSE;
    }

    return pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED);
}

/*!
 * @brief callback after FIFO is done initializing and we are ready to handle RC
 */
static NV_STATUS
_knvlinkHandlePostSchedulingEnableCallback_GB100
(
    OBJGPU *pGpu,
    void *pUnusedData
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // Trigger supressed error replay in gsp-rm
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                            NV2080_CTRL_CMD_INTERNAL_NVLINK_REPLAY_SUPPRESSED_ERRORS,
                            NULL,
                            0));
    return NV_OK;
}

/*!
 * @brief Registers Callback to process supressed link errors on boot
 */
void
knvlinkPostSchedulingEnableCallbackRegister_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_ASSERT_OK(
        kfifoAddSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            _knvlinkHandlePostSchedulingEnableCallback_GB100, NULL, NULL, NULL));
}

/*!
 * @brief Unregisters Callback to process supressed link errors on boot
 */
void
knvlinkPostSchedulingEnableCallbackUnregister_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    kfifoRemoveSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
        _knvlinkHandlePostSchedulingEnableCallback_GB100, NULL, NULL, NULL);
}

/*!
 * @brief Get supported core link states for this device
 */
NvU32
knvlinkGetSupportedCoreLinkStateMask_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)
    return NVBIT32(NVLINK_LINKSTATE_OFF) |
           NVBIT32(NVLINK_LINKSTATE_HS) |
           NVBIT32(NVLINK_LINKSTATE_SLEEP) |
           NVBIT32(NVLINK_LINKSTATE_DETECT) |
           NVBIT32(NVLINK_LINKSTATE_ALI) |
           NVBIT32(NVLINK_LINKSTATE_FAULT);
#else
    return 0x0;
#endif // defined(INCLUDE_NVLINK_LIB)
}

void
knvlinkP2PIdleCheck_WORKITEM
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (knvlinkIsP2PActive_IMPL(pGpu, pKernelNvlink))
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_TRUE);
        gpuSetRecoveryDrainP2P(pGpu, NV_TRUE);
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_FALSE);
        return;
    }

    // Invalidate/Suspend probe
    gpuFabricProbeSuspend(pGpu->pGpuFabricProbeInfoKernel);
    gpuFabricProbeInvalidate(pGpu->pGpuFabricProbeInfoKernel);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_TRUE);
    gpuUnmarkDeviceForDrainP2P(pGpu);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_FALSE);

    // Send requested probe
    NV_ASSERT_OK(gpuFabricProbeResume(pGpu->pGpuFabricProbeInfoKernel));

    osRemove1HzCallback(pGpu, _knvlinkP2PIdleCallback, NULL);
}

static void
_knvlinkP2PIdleCallback
(
    OBJGPU *pGpu,
    void *pArgs
)
{
    // Queue a work item to check P2P with proper locks
    NV_STATUS status = osQueueWorkItem(pGpu,
                                      knvlinkP2PIdleCheck_WORKITEM,
                                      NULL,
                                      (OsQueueWorkItemFlags){
                                          .bLockSema = NV_TRUE,
                                          .apiLock = WORKITEM_FLAGS_API_LOCK_READ_ONLY,
                                          .bLockGpus = NV_TRUE
                                      });

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to queue P2P idle check.\n");
    }
}

NV_STATUS
knvlinkTriggerProbeRequest_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    (void)osSchedule1HzCallback(pGpu, _knvlinkP2PIdleCallback, NULL, NV_OS_1HZ_REPEAT);

    return NV_OK;
}

/*!
 * Gets the platform info for the GPU, this is needed while getting port mappings
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
NV_STATUS
knvlinkGetPlatformInfo_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS params;
    portMemSet(&params, 0, sizeof(params));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                              NV2080_CTRL_CMD_NVLINK_GET_PLATFORM_INFO,
                                              (void *)&params, sizeof(params)));

    portMemCopy(&pKernelNvlink->platformInfo, sizeof(NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS),
                &params, sizeof(NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS));

    return NV_OK;
}
