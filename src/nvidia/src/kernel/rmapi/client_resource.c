/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "core/locks.h"
#include "core/system.h"
#include "os/os.h"
#include "rmapi/client_resource.h"
#include "rmapi/param_copy.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "gpu/gpu_uuid.h"
#include "gpu_mgr/gpu_mgr.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "resserv/rs_access_map.h"
#include "nvBldVer.h"
#include "nvVer.h"
#include "platform/nvpcf.h"
#include "mem_mgr/mem.h"
#include "nvsecurityinfo.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "resource_desc.h"
#include "platform/sli/sli.h"

#include "mem_mgr/fla_mem.h"

#include "vgpu/vgpu_version.h"
#include "virtualization/kernel_vgpu_mgr.h"

#include "platform/chipset/chipset_info.h"
#include "platform/chipset/chipset.h"
#include "platform/cpu.h"
#include "platform/platform.h"
#include "platform/p2p/p2p_caps.h"
#include "platform/acpi_common.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "vgpu/rpc.h"
#include "jt.h"
#include "nvop.h"
#include "diagnostics/gpu_acct.h"
#include "platform/platform_request_handler.h"
#include "gpu/external_device/gsync.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "diagnostics/journal.h"
#include "ctrl/ctrl0000/ctrl0000nvd.h"
#include "nvdump.h"
#include "gpu/gsp/kernel_gsp.h"
#include "power/gpu_boost_mgr.h"

#define CONFIG_2X_BUFF_SIZE_MIN                                             (2)

//
// Controller Table v2.2 has removed some params, set them using these
// default values instead
//
// EWMA retention weight (232/256) results in tau being 10x the sampling period
//
#define CONTROLLER_GRP_DEFAULT_BASE_SAMPLING_PERIOD_MS                    (100)
#define CONTROLLER_GRP_DEFAULT_SAMPLING_MULTIPLIER                          (1)
#define CONTROLLER_GRP_DEFAULT_EWMA_WEIGHT                                (232)
#define CONTROLLER_GRP_DEFAULT_INCREASE_GAIN_UFXP4_12                    (3686)
#define CONTROLLER_GRP_DEFAULT_DECREASE_GAIN_UFXP4_12                    (4096)

/*!
 * Define the filter types.
 */
#define NVPCF0100_CTRL_CONTROLLER_FILTER_TYPE_EMWA                           (0)
#define NVPCF0100_CTRL_CONTROLLER_FILTER_TYPE_MOVING_MAX                     (1)

ct_assert(NV_GPU_UUID_LEN == VM_UUID_SIZE);

static NV_STATUS
CliGetSystemP2pCaps_GSPCLIENT
(
    NvU32 *gpuIds,
    NvU32 gpuCount,
    NvU32 *p2pCaps,
    NvU32 *p2pOptimalReadCEs,
    NvU32 *p2pOptimalWriteCEs,
    NvU8  *p2pCapsStatus,
    NvU32 *busPeerIds,
    NvU32 *busEgmPeerIds
);

static
NV_STATUS
CliGetSystemP2pCapsMatrix_GSPCLIENT
(
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pP2PParams
);

NV_STATUS
cliresConstruct_IMPL
(
    RmClientResource *pRmCliRes,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL* pParams
)
{
    return NV_OK;
}

void
cliresDestruct_IMPL
(
    RmClientResource *pRmCliRes
)
{
}

NvBool
cliresAccessCallback_IMPL
(
    RmClientResource *pRmCliRes,
    RsClient *pInvokingClient,
    void *pAllocParams,
    RsAccessRight accessRight
)
{
    // Client resource's access callback will grant any rights here to any resource it owns
    switch (accessRight)
    {
        case RS_ACCESS_NICE:
        {
            // Grant if the caller satisfies osAllowPriorityOverride
            return osAllowPriorityOverride();
        }
    }

    // Delegate to superclass
    return resAccessCallback_IMPL(staticCast(pRmCliRes, RsResource), pInvokingClient, pAllocParams, accessRight);
}

NvBool
cliresShareCallback_IMPL
(
    RmClientResource *pRmCliRes,
    RsClient *pInvokingClient,
    RsResourceRef *pParentRef,
    RS_SHARE_POLICY *pSharePolicy
)
{
    RmClient *pSrcClient = dynamicCast(RES_GET_CLIENT(pRmCliRes), RmClient);
    RmClient *pDstClient = dynamicCast(pInvokingClient, RmClient);
    NvBool bDstKernel = (pDstClient != NULL) &&
                        (rmclientGetCachedPrivilege(pDstClient) >= RS_PRIV_LEVEL_KERNEL);

    // Client resource's share callback will also share rights it shares here with any resource it owns
    //
    // If a kernel client is validating share policies, that means it's most likely duping on behalf of
    // a user space client. For this case, we check against the current process instead of the kernel
    // client object's process.
    //
    switch (pSharePolicy->type)
    {
        case RS_SHARE_TYPE_OS_SECURITY_TOKEN:
            if ((pSrcClient != NULL) && (pDstClient != NULL) &&
                (pSrcClient->pSecurityToken != NULL))
            {
                if (bDstKernel)
                {
                    NV_STATUS status;
                    PSECURITY_TOKEN *pCurrentToken;

                    pCurrentToken = osGetSecurityToken();
                    if (pCurrentToken == NULL)
                    {
                        NV_ASSERT_FAILED("Cannot get the security token for the current user");
                        return NV_FALSE;
                    }

                    status = osValidateClientTokens(pSrcClient->pSecurityToken, pCurrentToken);
                    portMemFree(pCurrentToken);
                    if (status == NV_OK)
                    {
                        return NV_TRUE;
                    }
                }
                else if (pDstClient->pSecurityToken != NULL)
                {
                    if (osValidateClientTokens(pSrcClient->pSecurityToken, pDstClient->pSecurityToken) == NV_OK)
                        return NV_TRUE;
                }
            }
            break;
        case RS_SHARE_TYPE_PID:
            if ((pSrcClient != NULL) && (pDstClient != NULL))
            {
                if ((pParentRef != NULL) && bDstKernel)
                {
                    if (pSrcClient->ProcID == osGetCurrentProcess())
                        return NV_TRUE;
                }
                else
                {
                    if (pSrcClient->ProcID == pDstClient->ProcID)
                        return NV_TRUE;
                }
            }
            break;
        case RS_SHARE_TYPE_SMC_PARTITION:
        case RS_SHARE_TYPE_GPU:
            // Require exceptions, since RmClientResource is not an RmResource
            if (pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE)
                return NV_TRUE;
            break;
    }

    // Delegate to superclass
    return resShareCallback_IMPL(staticCast(pRmCliRes, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

NV_STATUS
cliresControl_Prologue_IMPL
(
    RmClientResource *pRmCliRes,
    CALL_CONTEXT *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = serverDeserializeCtrlDown(pCallContext, pParams->cmd, &pParams->pParams, &pParams->paramsSize, &pParams->flags);

    return status;
}

void
cliresControl_Epilogue_IMPL
(
    RmClientResource *pRmCliRes,
    CALL_CONTEXT *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_ASSERT_OK(serverSerializeCtrlUp(pCallContext, pParams->cmd, &pParams->pParams, &pParams->paramsSize, &pParams->flags));
    serverFreeSerializeStructures(pCallContext, pParams->pParams);
}

// ****************************************************************************
//                              Helper functions
// ****************************************************************************


static NV_STATUS
CliControlSystemEvent
(
    NvHandle hClient,
    NvU32    event,
    NvU32    action
)
{
    NV_STATUS status = NV_OK;
    RmClient *pClient;
    PEVENTNOTIFICATION *pEventNotification = NULL;

    if (event >= NV0000_NOTIFIERS_MAXCOUNT)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pClient = serverutilGetClientUnderLock(hClient);
    if (pClient == NULL)
        return NV_ERR_INVALID_CLIENT;

    CliGetEventNotificationList(hClient, hClient, NULL, &pEventNotification);
    if (pEventNotification != NULL)
    {
        switch (action)
        {
            case NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE:
            case NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT:
            {
                if (pClient->CliSysEventInfo.notifyActions[event] != NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
                {
                    status = NV_ERR_INVALID_STATE;
                    break;
                }

            //fall through
            }
            case NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE:
            {
                pClient->CliSysEventInfo.notifyActions[event] = action;
                break;
            }

            default:
            {
                status = NV_ERR_INVALID_ARGUMENT;
                break;
            }
        }
    }
    else
    {
        status = NV_ERR_INVALID_STATE;
    }

    return status;
}



static NV_STATUS
CliGetSystemEventStatus
(
    NvHandle  hClient,
    NvU32    *pEvent,
    NvU32    *pStatus
)
{
    NvU32 Head, Tail;
    RmClient *pClient = serverutilGetClientUnderLock(hClient);

    if (pClient == NULL)
        return NV_ERR_INVALID_CLIENT;

    Head = pClient->CliSysEventInfo.systemEventsQueue.Head;
    Tail = pClient->CliSysEventInfo.systemEventsQueue.Tail;

    if (Head == Tail)
    {
        *pEvent = NV0000_NOTIFIERS_EVENT_NONE_PENDING;
        *pStatus = 0;
    }
    else
    {
        *pEvent  = pClient->CliSysEventInfo.systemEventsQueue.EventQueue[Tail].event;
        *pStatus = pClient->CliSysEventInfo.systemEventsQueue.EventQueue[Tail].status;
        pClient->CliSysEventInfo.systemEventsQueue.Tail = (Tail + 1) % NV_SYSTEM_EVENT_QUEUE_SIZE;
    }

    return NV_OK;
}



NV_STATUS
CliGetSystemP2pCaps
(
    NvU32  *gpuIds,
    NvU32   gpuCount,
    NvU32  *p2pCaps,
    NvU32  *p2pOptimalReadCEs,
    NvU32  *p2pOptimalWriteCEs,
    NvU8   *p2pCapsStatus,
    NvU32  *pBusPeerIds,
    NvU32  *pBusEgmPeerIds
)
{
    OBJGPU       *pGpuLocal          = NULL;
    OBJGPU       *pGpuLocalLoop      = NULL;
    OBJGPU       *pGpuPeer           = NULL;
    KernelNvlink *pLocalKernelNvlink = NULL;
    NvU32         gpuMask = 0;
    NvU32         localGpuIndex, peerGpuIndex;
    NvU32         i;
    NvU8          p2pWriteCapStatus, p2pReadCapStatus;
    NV_STATUS     rmStatus = NV_OK;
    P2P_CONNECTIVITY connectivity;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if ((gpuCount ==  0) || (gpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
    {
        NV_PRINTF(LEVEL_WARNING, "Invalid GPU count\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Assume no peer to peer capabilities.
    *p2pOptimalReadCEs = 0;
    *p2pOptimalWriteCEs = 0;
    if (p2pCaps != NULL)
    {
        *p2pCaps = 0;
    }

    if (p2pCapsStatus != NULL)
    {
        for (i = 0; i < NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE; i++)
        {
            p2pCapsStatus[i] = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        }
    }

    // Construct the GPU mask
    for (localGpuIndex = 0; localGpuIndex < gpuCount; localGpuIndex++)
    {
        pGpuLocalLoop = gpumgrGetGpuFromId(gpuIds[localGpuIndex]);
        if (pGpuLocalLoop == NULL)
        {
            NV_PRINTF(LEVEL_WARNING, "GPU ID not found: 0x%x\n",
                      gpuIds[localGpuIndex]);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        gpuMask |= NVBIT(gpuGetInstance(pGpuLocalLoop));

        for (peerGpuIndex = 0; peerGpuIndex < gpuCount; peerGpuIndex++)
        {
            pGpuPeer = gpumgrGetGpuFromId(gpuIds[peerGpuIndex]);
            if (pGpuPeer == NULL)
            {
                NV_PRINTF(LEVEL_WARNING, "GPU ID not found: 0x%x\n",
                          gpuIds[peerGpuIndex]);
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            if (pBusPeerIds != NULL)
            {
                pBusPeerIds[(localGpuIndex * gpuCount) + peerGpuIndex] =
                    kbusGetPeerId_HAL(pGpuLocalLoop, GPU_GET_KERNEL_BUS(pGpuLocalLoop), pGpuPeer);
            }

            if (pBusEgmPeerIds != NULL)
            {
                pBusEgmPeerIds[(localGpuIndex * gpuCount) + peerGpuIndex] =
                    kbusGetEgmPeerId_HAL(pGpuLocalLoop, GPU_GET_KERNEL_BUS(pGpuLocalLoop), pGpuPeer);
            }
        }
    }

    // Local GPU needs to be the first in the list
    pGpuLocal = gpumgrGetGpuFromId(gpuIds[0]);
    if (pGpuLocal == NULL)
    {
        NV_PRINTF(LEVEL_WARNING, "GPU ID not found: 0x%x\n",
                  gpuIds[localGpuIndex]);
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    rmStatus = p2pGetCapsStatus(gpuMask, &p2pWriteCapStatus,
                                &p2pReadCapStatus, &connectivity
                                );
    if (rmStatus != NV_OK)
    {
        goto done;
    }

    if (connectivity == P2P_CONNECTIVITY_NVLINK)
    {
        pLocalKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpuLocal);
        NV_ASSERT_OR_RETURN(pLocalKernelNvlink != NULL, NV_ERR_INVALID_STATE);

        if (p2pCaps != NULL)
        {
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED, _TRUE);
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED, _TRUE);
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED, _TRUE);
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED, _TRUE);
        }

        if (p2pCapsStatus != NULL)
        {
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_NVLINK] = NV0000_P2P_CAPS_STATUS_OK;
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_ATOMICS] = NV0000_P2P_CAPS_STATUS_OK;
        }

        // Get the optimal CEs for P2P read/write for 1 or 2 gpu masks only
        if (gpuCount <= 2)
        {
            knvlinkGetP2POptimalCEs_HAL(pGpuLocal, pLocalKernelNvlink, gpuMask,
                                        NULL, NULL,
                                        p2pOptimalReadCEs,
                                        p2pOptimalWriteCEs);
        }

        if (gpuCount == 1 &&
            knvlinkIsP2pLoopbackSupported(pGpuLocal, pLocalKernelNvlink))
        {
            if (p2pCaps != NULL)
            {
                *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED, _TRUE);
            }

            if (p2pCapsStatus != NULL)
                p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_LOOPBACK] = NV0000_P2P_CAPS_STATUS_OK;
        }
    }
    else if ((connectivity == P2P_CONNECTIVITY_PCIE_BAR1) ||
             (connectivity == P2P_CONNECTIVITY_PCIE_PROPRIETARY))
    {
        if (p2pCaps != NULL)
        {
            *p2pCaps |= (p2pReadCapStatus == NV0000_P2P_CAPS_STATUS_OK) ?
                REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED, _TRUE) : 0;
            *p2pCaps |= (p2pWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK) ?
                REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED, _TRUE) : 0;
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED, _TRUE);

            if (connectivity == P2P_CONNECTIVITY_PCIE_BAR1)
            {
                *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED, _TRUE);
            }
            else
            {
                *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED, _TRUE);
            }
        }

        if (p2pCapsStatus != NULL)
        {
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_PCI] = NV0000_P2P_CAPS_STATUS_OK;

            if (connectivity == P2P_CONNECTIVITY_PCIE_BAR1)
            {
                p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_PCI_BAR1] = NV0000_P2P_CAPS_STATUS_OK;
            }
            else
            {
                p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_PROP] = NV0000_P2P_CAPS_STATUS_OK;
            }
        }

        if ((gpuCount == 1) && (connectivity == P2P_CONNECTIVITY_PCIE_PROPRIETARY))
        {
            if (p2pCaps != NULL)
            {
                *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED, _TRUE);
            }

            if (p2pCapsStatus != NULL)
            {
                p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_LOOPBACK] = NV0000_P2P_CAPS_STATUS_OK;
            }
        }
    }
    else if (connectivity == P2P_CONNECTIVITY_NVLINK_INDIRECT)
    {
        pLocalKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpuLocal);
        NV_ASSERT_OR_RETURN(pLocalKernelNvlink != NULL, NV_ERR_INVALID_STATE);

        if (p2pCaps != NULL)
        {
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED, _TRUE);
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED, _TRUE);
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED, _TRUE);
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED, _TRUE);
        }

        if (p2pCapsStatus != NULL)
        {
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_ATOMICS] = NV0000_P2P_CAPS_STATUS_OK;
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_NVLINK] = NV0000_P2P_CAPS_STATUS_OK;
        }

        // Use sysmem CEs as optimal CEs for indirect peers.
        knvlinkGetP2POptimalCEs_HAL(pGpuLocal, pLocalKernelNvlink, gpuMask,
                                    p2pOptimalReadCEs, p2pOptimalWriteCEs, NULL, NULL);
    }
    else if (connectivity == P2P_CONNECTIVITY_C2C)
    {

        if (p2pCaps != NULL)
        {
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED, _TRUE);
            *p2pCaps |= (p2pReadCapStatus == NV0000_P2P_CAPS_STATUS_OK) ?
                REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED, _TRUE) : 0;
            *p2pCaps |= (p2pWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK) ?
                REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED, _TRUE) : 0;
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED, _TRUE);
        }

        if (p2pCapsStatus != NULL)
        {
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_ATOMICS] = NV0000_P2P_CAPS_STATUS_OK;
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_C2C] = NV0000_P2P_CAPS_STATUS_OK;
        }
        if (gpuCount == 1)
        {
            if (p2pCaps != NULL)
            {
                *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED, _TRUE);
            }

            if (p2pCapsStatus != NULL)
            {
                p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_LOOPBACK] = NV0000_P2P_CAPS_STATUS_OK;
            }
        }
    }

    if (p2pCapsStatus != NULL)
    {
        p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_READ] = p2pReadCapStatus;
        p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_WRITE] = p2pWriteCapStatus;
    }

    // Honor the regkey for loopback status.
    if (p2pCapsStatus != NULL &&
        pGpuLocal->getProperty(pGpuLocal, PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED))
    {
        p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_LOOPBACK] = NV0000_P2P_CAPS_STATUS_DISABLED_BY_REGKEY;
    }

done:
    return rmStatus;
}

// static functions used by NVPCF controls
static void
_configCalculateSizes
(
    const char *pFormat,
    NvU32      *pPackedSize,
    NvU32      *pUnpackedSize
)
{
    NvU32 unpkdSize = 0;
    NvU32 pkdSize = 0;
    NvU32 count;
    char fmt;

    while ((fmt = *pFormat++))
    {
        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *pFormat++;
        }
        if (count == 0)
            count = 1;

        switch (fmt)
        {
            case 'b':
                pkdSize += count * 1;
                unpkdSize += count * sizeof(NvU32);
                break;

            case 's':    // signed byte
                pkdSize += count * 1;
                unpkdSize += count * sizeof(NvU32);
                break;

            case 'w':
                pkdSize += count * 2;
                unpkdSize += count * sizeof(NvU32);
                break;

            case 'd':
                pkdSize += count * 4;
                unpkdSize += count * sizeof(NvU32);
                break;
        }
    }

    if (pPackedSize != NULL)
        *pPackedSize = pkdSize;

    if (pUnpackedSize != NULL)
        *pUnpackedSize = unpkdSize;
}


static NV_STATUS
_configUnpackStructure
(
    const char *pFormat,
    const NvU8 *pPackedData,
    NvU32      *pUnpackedData,
    NvU32      *pUnpackedSize,
    NvU32      *pFieldsCount
)
{
    NvU32 unpkdSize = 0;
    NvU32 fields = 0;
    NvU32 count;
    NvU32 data;
    char fmt;

    while ((fmt = *pFormat++))
    {
        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *pFormat++;
        }
        if (count == 0)
            count = 1;

        while (count--)
        {
            switch (fmt)
            {
                case 'b':
                    data = *pPackedData++;
                    unpkdSize += 1;
                    break;

                case 's':    // signed byte
                    data = *pPackedData++;
                    if (data & 0x80)
                        data |= ~0xff;
                    unpkdSize += 1;
                    break;

                case 'w':
                    data  = *pPackedData++;
                    data |= *pPackedData++ << 8;
                    unpkdSize += 2;
                    break;

                case 'd':
                    data  = *pPackedData++;
                    data |= *pPackedData++ << 8;
                    data |= *pPackedData++ << 16;
                    data |= *pPackedData++ << 24;
                    unpkdSize += 4;
                    break;

                default:
                    return NV_ERR_GENERIC;
            }
            *pUnpackedData++ = data;
            fields++;
        }
    }

    if (pUnpackedSize != NULL)
        *pUnpackedSize = unpkdSize;

    if (pFieldsCount != NULL)
        *pFieldsCount = fields;

    return NV_OK;
}

static NV_STATUS
configReadStructure
  (
      NvU8       *pData,
      void        *pStructure,
      NvU32        offset,
      const char  *pFormat
  )
  {
      NvU32  packed_size;
      NvU8  *pPacked_data;
      NvU32  unpacked_bytes;

      // calculate the size of the data as indicated by its packed format.
      _configCalculateSizes(pFormat, &packed_size, &unpacked_bytes);
      pPacked_data = &pData[offset];

      return _configUnpackStructure(pFormat, pPacked_data, pStructure,
          &unpacked_bytes, NULL);
}

// ****************************************************************************
//                              Other functions
// ****************************************************************************

//
// cliresCtrlCmdSystemGetFeatures
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdSystemGetFeatures_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS *pFeaturesParams
)
{
    OBJSYS    *pSys         = SYS_GET_INSTANCE();
    OBJGPU    *pGpu            = NULL;
    NvU32      featuresMask = 0;
    NvU32      gpuMask  = 0U;
    NvU32      gpuIndex = 0U;
    NvBool     bIsEfiInit = NV_FALSE;

    NV_ASSERT_OR_RETURN(pSys != NULL, NV_ERR_INVALID_STATE);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        // Don't update EFI init on non Display system
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_EFI_INIT))
        {
            bIsEfiInit = NV_TRUE;
            break;
        }
    }

    if (bIsEfiInit)
    {
        featuresMask = FLD_SET_DRF(0000, _CTRL_SYSTEM_GET_FEATURES,
            _IS_EFI_INIT, _TRUE, featuresMask);
    }

    if (osImexChannelIsSupported() && (osImexChannelCount() != 0))
    {
        featuresMask = FLD_SET_DRF(0000, _CTRL_SYSTEM_GET_FEATURES,
            _UUID_BASED_MEM_SHARING, _TRUE, featuresMask);
    }

    if (pSys->getProperty(pSys, PDB_PROP_SYS_ENABLE_RM_TEST_ONLY_CODE))
    {
        featuresMask = FLD_SET_DRF(0000, _CTRL_SYSTEM_GET_FEATURES,
            _RM_TEST_ONLY_CODE_ENABLED, _TRUE, featuresMask);
    }

    pFeaturesParams->featuresMask = featuresMask;

    return NV_OK;
}

//
// cliresCtrlCmdSystemGetBuildVersionV2
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdSystemGetBuildVersionV2_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS *pParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    ct_assert(sizeof(NV_VERSION_STRING) <= sizeof(pParams->driverVersionBuffer));
    ct_assert(sizeof(NV_BUILD_BRANCH_VERSION) <= sizeof(pParams->versionBuffer));
    ct_assert(sizeof(NV_DISPLAY_DRIVER_TITLE) <= sizeof(pParams->titleBuffer));
    ct_assert(sizeof(STRINGIZE(NV_BUILD_BRANCH)) <= sizeof(pParams->driverBranch));

    portMemCopy(pParams->driverVersionBuffer, sizeof(pParams->driverVersionBuffer),
                NV_VERSION_STRING, sizeof(NV_VERSION_STRING));
    portMemCopy(pParams->versionBuffer, sizeof(pParams->versionBuffer),
                NV_BUILD_BRANCH_VERSION, sizeof(NV_BUILD_BRANCH_VERSION));
    portMemCopy(pParams->titleBuffer, sizeof(pParams->titleBuffer),
                NV_DISPLAY_DRIVER_TITLE, sizeof(NV_DISPLAY_DRIVER_TITLE));
    portMemCopy(pParams->driverBranch, sizeof(pParams->driverBranch),
                STRINGIZE(NV_BUILD_BRANCH), sizeof(STRINGIZE(NV_BUILD_BRANCH)));

    pParams->changelistNumber = NV_BUILD_CHANGELIST_NUM;
    pParams->officialChangelistNumber = NV_LAST_OFFICIAL_CHANGELIST_NUM;

    return NV_OK;
}

/*!
 * @brief Allows clients to execute ACPI methods for various purpose
 *
 * @return if the client request support:
 *           NV_OK                   if the ACPI method is executed successfully
 *           NV_ERR_INVALID_ARGUMENT if the arguments are not proper
 *           NV_ERR_BUFFER_TOO_SMALL if the output buffer is smaller than expected
 */
NV_STATUS
cliresCtrlCmdSystemExecuteAcpiMethod_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams
)
{
    OBJGPU     *pGpu            = NULL;
    NvU32       method          = (NvU32) pAcpiMethodParams->method;
    NvU32       outStatus       = 0;
    NvU32       inOutDataSize;
    NvU16       inDataSize;
    NvU16       outDataSize;
    NvBool      bDoCopyOut      = NV_FALSE;
    void*       pInOutData      = NULL;
    NV_STATUS   status          = NV_OK;

    pGpu = gpumgrGetSomeGpu();
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_REQUEST;
    }
    inDataSize      = pAcpiMethodParams->inDataSize;
    outDataSize     = pAcpiMethodParams->outDataSize;
    inOutDataSize   = (NvU32) NV_MAX(inDataSize, outDataSize);

    // Verify size
    if ((outDataSize == 0) ||
        (pAcpiMethodParams->inData == NvP64_NULL) ||
        (pAcpiMethodParams->outData == NvP64_NULL))
    {

        NV_PRINTF(LEVEL_WARNING,
                  "ERROR: NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: Parameter validation failed: outDataSize=%d  method = %ud\n",
                  (NvU32)outDataSize, method);

        return NV_ERR_INVALID_ARGUMENT;
    }

    // Allocate memory for the combined in/out buffer
    pInOutData = portMemAllocNonPaged(inOutDataSize);
    if (pInOutData == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_WARNING,
                  "ERROR: NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: mem alloc failed\n");
        goto done;
    }

    if (inDataSize)
    {
        portMemCopy(pInOutData, inDataSize, NvP64_VALUE(pAcpiMethodParams->inData), inDataSize);
    }

    // jump to the method to be executed
    switch (method)
    {
        case NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSCAPS:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       ACPI_DSM_FUNCTION_NVOP,
                                       NVOP_FUNC_OPTIMUSCAPS,
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSFLAG:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       ACPI_DSM_FUNCTION_NVOP,
                                       NVOP_FUNC_OPTIMUSFLAG,
                                       (NvU32*) pInOutData,
                                       (NvU16*) &outDataSize);
            break;
        }
        case NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_JT_CAPS:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       ACPI_DSM_FUNCTION_JT,
                                       JT_FUNC_CAPS,
                                       (NvU32*) pInOutData,
                                       (NvU16*) &outDataSize);
            break;
        }
        case NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_JT_PLATPOLICY:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       ACPI_DSM_FUNCTION_JT,
                                       JT_FUNC_PLATPOLICY,
                                       (NvU32*) pInOutData,
                                       (NvU16*) &outDataSize);
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_WARNING,
                      "ERROR: NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: Unrecognized Api Code: 0x%x\n",
                      method);

            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
    }

    if (outStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "ERROR: NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: Execution failed for method: 0x%x, status=0x%x\n",
                  method, outStatus);

        // if it was a command we tried to return the real status.
        if (status == NV_OK)
        {
            status = outStatus;
        }

        pAcpiMethodParams->outStatus = outStatus;
        goto done;
    }

    // NOTE: 'outDataSize' may have changed above.
    if (outDataSize > pAcpiMethodParams->outDataSize)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "ERROR: NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: output buffer is smaller then expected!\n");

        // pAcpiMethodParams->outStatus = outStatus; //XXX64 check
        // status = outStatus; //XXX64 check
        status = NV_ERR_BUFFER_TOO_SMALL;
        goto done;
    }

    // all ok - so copy 'outdata' back to client
    bDoCopyOut = NV_TRUE;

done:

    // pass data out to client's output buffer
    if (bDoCopyOut)
    {
        pAcpiMethodParams->outDataSize = outDataSize;

        portMemCopy(NvP64_VALUE(pAcpiMethodParams->outData), outDataSize, pInOutData, outDataSize);
    }

    // release client's input buffer
    portMemFree(pInOutData);

    return status;
}

//
// cliresCtrlCmdSystemGetCpuInfo
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdSystemGetCpuInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS *pCpuInfoParams
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pCpuInfoParams->type = pSys->cpuInfo.type;
    pCpuInfoParams->capabilities = pSys->cpuInfo.caps;
    pCpuInfoParams->clock = pSys->cpuInfo.clock;
    pCpuInfoParams->L1DataCacheSize = pSys->cpuInfo.l1DataCacheSize;
    pCpuInfoParams->L2DataCacheSize = pSys->cpuInfo.l2DataCacheSize;
    pCpuInfoParams->dataCacheLineSize = pSys->cpuInfo.dataCacheLineSize;
    pCpuInfoParams->numLogicalCpus = pSys->cpuInfo.numLogicalCpus;
    pCpuInfoParams->numPhysicalCpus = pSys->cpuInfo.numPhysicalCpus;
    pCpuInfoParams->coresOnDie = pSys->cpuInfo.coresOnDie;
    pCpuInfoParams->family = pSys->cpuInfo.family;
    pCpuInfoParams->model = pSys->cpuInfo.model;
    pCpuInfoParams->stepping = pSys->cpuInfo.stepping;
    pCpuInfoParams->bCCEnabled = (sysGetStaticConfig(pSys))->bOsCCEnabled;
    portMemCopy(pCpuInfoParams->name,
                sizeof (pCpuInfoParams->name), pSys->cpuInfo.name,
                sizeof (pCpuInfoParams->name));

    return NV_OK;
}

//
// cliresCtrlCmdSystemGetChipsetInfo
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdSystemGetChipsetInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS *pChipsetInfo
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pChipsetInfo->flags = 0;

    if (pCl->chipsetIDBusAddr.valid)
    {
        pChipsetInfo->vendorId = pCl->chipsetIDInfo.vendorID;
        pChipsetInfo->deviceId = pCl->chipsetIDInfo.deviceID;
        pChipsetInfo->subSysVendorId = pCl->chipsetIDInfo.subvendorID;
        pChipsetInfo->subSysDeviceId = pCl->chipsetIDInfo.subdeviceID;
    }
    else
    {
        pChipsetInfo->vendorId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
        pChipsetInfo->deviceId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
        pChipsetInfo->subSysVendorId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
        pChipsetInfo->subSysDeviceId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
    }

    if (pCl->FHBAddr.valid)
    {
        pChipsetInfo->HBvendorId = pCl->FHBBusInfo.vendorID;
        pChipsetInfo->HBdeviceId = pCl->FHBBusInfo.deviceID;
        pChipsetInfo->HBsubSysVendorId = pCl->FHBBusInfo.subvendorID;
        pChipsetInfo->HBsubSysDeviceId = pCl->FHBBusInfo.subdeviceID;
    }
    else
    {
        pChipsetInfo->HBvendorId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
        pChipsetInfo->HBdeviceId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
        pChipsetInfo->HBsubSysVendorId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
        pChipsetInfo->HBsubSysDeviceId = NV0000_SYSTEM_CHIPSET_INVALID_ID;
    }

    pChipsetInfo->sliBondId = pCl->ChipsetSliBondType;

    csGetInfoStrings(pCl,
                     &pChipsetInfo->chipsetNameString[0],
                     &pChipsetInfo->vendorNameString[0],
                     &pChipsetInfo->sliBondNameString[0],
                     &pChipsetInfo->subSysVendorNameString[0],
                     NV0000_SYSTEM_MAX_CHIPSET_STRING_LENGTH);

    if (pCl->getProperty(pCl, PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE))
    {
        pChipsetInfo->flags = FLD_SET_DRF(0000, _CTRL_SYSTEM_CHIPSET_FLAG, _HAS_RESIZABLE_BAR_ISSUE, _YES, pChipsetInfo->flags);
    }
    else
    {
        pChipsetInfo->flags = FLD_SET_DRF(0000, _CTRL_SYSTEM_CHIPSET_FLAG, _HAS_RESIZABLE_BAR_ISSUE, _NO, pChipsetInfo->flags);
    }

    return NV_OK;
}

//
// cliresCtrlCmdSystemGetLockTimes
//
// Get API and GPU lock hold/wait times.
//
// Lock Requirements:
//      None
//
NV_STATUS
cliresCtrlCmdSystemGetLockTimes_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    // Check if lock time collection is enabled first
    if (!pSys->getProperty(pSys, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT))
        return NV_ERR_NOT_SUPPORTED;

    // Fetch API lock hold/wait times
    rmapiLockGetTimes(pParams);

    // Fetch GPU lock hold/wait times
    rmGpuLockGetTimes(pParams);

    return NV_OK;
}

static NV_STATUS
classGetSystemClasses(NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS *pParams)
{
    NvU32 i;
    NvU32 numResources;
    const RS_RESOURCE_DESC *resources;
    NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS params;

    NV_ASSERT_OR_RETURN(pParams, NV_ERR_INVALID_ARGUMENT);

    RsResInfoGetResourceList(&resources, &numResources);

    portMemSet(&params, 0x0, sizeof(params));

    for (i = 0; i < numResources; i++)
    {
        if ((resources[i].pParentList[0] == classId(RmClientResource)) &&
            (resources[i].pParentList[1] == 0x0))
        {
            NV_ASSERT_OR_RETURN(params.numClasses < NV0000_CTRL_SYSTEM_MAX_CLASSLIST_SIZE,
                                NV_ERR_INVALID_STATE);

            params.classes[params.numClasses] = resources[i].externalClassId;
            params.numClasses++;
        }
    }

    portMemCopy(pParams, sizeof(*pParams), &params, sizeof(params));

    return NV_OK;
}

//
// cliresCtrlCmdSystemGetClassList
//
// Get list of supported system classes.
//
// Lock Requirements:
//      Assert that API and GPUs locks held on entry
//
NV_STATUS
cliresCtrlCmdSystemGetClassList_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS *pParams
)
{
    NV_STATUS status;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    status = classGetSystemClasses(pParams);

    return status;
}

//
// cliresCtrlCmdSystemNotifyEvent
//
// This function exists to allow the RM Client to notify us when they receive
// a system event message.  We generally will store off the data, but in some
// cases, we'll trigger our own handling of that code.  Prior to Vista, we
// would just poll a scratch bit for these events.  But for Vista, we get them
// directly from the OS.
//
// Added Support for notifying power change event to perfhandler
//
NV_STATUS
cliresCtrlCmdSystemNotifyEvent_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS *pParams
)
{
    NV_STATUS   status    = NV_OK;

    switch(pParams->eventType)
    {
        case NV0000_CTRL_SYSTEM_EVENT_TYPE_LID_STATE:
        case NV0000_CTRL_SYSTEM_EVENT_TYPE_DOCK_STATE:
        case NV0000_CTRL_SYSTEM_EVENT_TYPE_TRUST_LID:
        case NV0000_CTRL_SYSTEM_EVENT_TYPE_TRUST_DOCK:
        {
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }

        case NV0000_CTRL_SYSTEM_EVENT_TYPE_POWER_SOURCE:
            status = NV_ERR_NOT_SUPPORTED;
            break;

        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

    return status;
}

//
// cliresCtrlCmdSystemGetPlatformType
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdSystemGetPlatformType_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS *pSysParams
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJPFM     *pPfm = SYS_GET_PFM(pSys);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if (pPfm->getProperty(pPfm, PDB_PROP_PFM_IS_TOSHIBA_MOBILE))
    {
        pSysParams->systemType = NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_MOBILE_TOSHIBA;
    }
    else if (pPfm->getProperty(pPfm, PDB_PROP_PFM_IS_MOBILE))
    {
        pSysParams->systemType = NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_MOBILE_GENERIC;
    }
    else
    {
        pSysParams->systemType = NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_DESKTOP;
    }

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemDebugCtrlRmMsg_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS *pParams
)
{
// NOTE: RmMsg is only available when NV_PRINTF_STRINGS_ALLOWED is true.
#if NV_PRINTF_STRINGS_ALLOWED
    NvU32 len = 0;

    extern char RmMsg[NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE];

    switch (pParams->cmd)
    {
        case NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_CMD_GET:
        {
            len = (NvU32)portStringLength(RmMsg);
            portMemCopy(pParams->data, len, RmMsg, len);
            pParams->count = len;
            break;
        }
        case NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_CMD_SET:
        {
#if !(defined(DEBUG) || defined(DEVELOP))
            RmClient *pRmClient = dynamicCast(RES_GET_CLIENT(pRmCliRes), RmClient);
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

            NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);
            NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

            if (!rmclientIsAdmin(pRmClient, pCallContext->secInfo.privLevel))
            {
                NV_PRINTF(LEVEL_WARNING, "Non-privileged context issued privileged cmd\n");
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }
#endif
            portMemCopy(RmMsg, NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE, pParams->data, NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE);
            break;
        }
        default:
            return NV_ERR_INVALID_ARGUMENT;
            break;
    }

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS
cliresCtrlCmdSystemGetRmInstanceId_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS *pRmInstanceIdParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    pRmInstanceIdParams->rm_instance_id = pSys->rmInstanceId;

    return NV_OK;
}

//
// cliresCtrlCmdGpuGetAttachedIds
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuGetAttachedIds_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS *pGpuAttachedIds
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    return gpumgrGetAttachedGpuIds(pGpuAttachedIds);
}

//
// cliresCtrlCmdGpuGetIdInfo
//
// Lock Requirements:
//      Assert that API lock and Gpus lock held on entry
//
NV_STATUS
cliresCtrlCmdGpuGetIdInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_ID_INFO_PARAMS *pGpuIdInfoParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    return gpumgrGetGpuIdInfo(pGpuIdInfoParams);
}

NV_STATUS
cliresCtrlCmdGpuGetIdInfoV2_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS *pGpuIdInfoParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    return gpumgrGetGpuIdInfoV2(pGpuIdInfoParams);
}

//
// cliresCtrlCmdGpuGetInitStatus
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuGetInitStatus_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS *pGpuInitStatusParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    return gpumgrGetGpuInitStatus(pGpuInitStatusParams);
}

//
// cliresCtrlCmdGpuGetDeviceIds
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuGetDeviceIds_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS *pDeviceIdsParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pDeviceIdsParams->deviceIds = gpumgrGetDeviceInstanceMask();

    return NV_OK;
}

//
// cliresCtrlCmdGpuGetActiveDeviceIds
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuGetActiveDeviceIds_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS *pActiveDeviceIdsParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    return gpumgrCacheGetActiveDeviceIds(pActiveDeviceIdsParams);
}

//
// cliresCtrlCmdGpuGetPciInfo
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuGetPciInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS *pPciInfoParams
)
{
    NV_STATUS status;
    NvU64 gpuDomainBusDevice;

    NV_ASSERT(rmapiLockIsOwner());

    status = gpumgrGetProbedGpuDomainBusDevice(pPciInfoParams->gpuId, &gpuDomainBusDevice);
    if (status != NV_OK)
        return status;

    pPciInfoParams->domain = gpuDecodeDomain(gpuDomainBusDevice);
    pPciInfoParams->bus = gpuDecodeBus(gpuDomainBusDevice);
    pPciInfoParams->slot = gpuDecodeDevice(gpuDomainBusDevice);

    return NV_OK;
}

//
// cliresCtrlCmdGpuGetProbedIds
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuGetProbedIds_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *pGpuProbedIds
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    return gpumgrGetProbedGpuIds(pGpuProbedIds);
}

//
// _cliresValidateGpuIdAgainstProbed
//
// Lock Requirements: none (only operates on arguments)
//
static NV_STATUS
_cliresValidateGpuIdAgainstProbed
(
    NvU32 gpuId,
    const NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *pGpuProbedIds
)
{
    NvU32 j;

    for (j = 0; j < NV0000_CTRL_GPU_MAX_PROBED_GPUS; j++)
    {
        if (pGpuProbedIds->gpuIds[j] == NV0000_CTRL_GPU_INVALID_ID)
            break;

        if (gpuId == pGpuProbedIds->gpuIds[j])
            return NV_OK;
    }

    return NV_ERR_INVALID_ARGUMENT;
}

//
// cliresCtrlCmdGpuAttachIds
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuAttachIds_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_ATTACH_IDS_PARAMS *pGpuAttachIds
)
{
    NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *pGpuProbedIds = NULL;
    NvU32 i;
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if (pGpuAttachIds->gpuIds[0] == NV0000_CTRL_GPU_ATTACH_ALL_PROBED_IDS)
    {
        // XXX add callback to attach logic on Windows
        status = NV_OK;
        goto done;
    }

    pGpuProbedIds = portMemAllocNonPaged(sizeof(*pGpuProbedIds));
    if (pGpuProbedIds == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    status = gpumgrGetProbedGpuIds(pGpuProbedIds);
    if (status != NV_OK)
    {
        goto done;
    }

    for (i = 0; (i < NV0000_CTRL_GPU_MAX_PROBED_GPUS) &&
                (pGpuAttachIds->gpuIds[i] != NV0000_CTRL_GPU_INVALID_ID); i++)
    {
        status = _cliresValidateGpuIdAgainstProbed(pGpuAttachIds->gpuIds[i],
                                                   pGpuProbedIds);
        if (status != NV_OK)
            break;
    }

    // XXX add callback to attach logic on Windows
done:
    portMemFree(pGpuProbedIds);
    return status;
}

//
// cliresCtrlCmdGpuAsyncAttachId
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuAsyncAttachId_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_ASYNC_ATTACH_ID_PARAMS *pAsyncAttachIdParams
)
{
    //
    // Similar to non-async attach, async attach is mostly handled by
    // libnvrmapi in userspace. Here, Core RM just does validation.
    //

    NV_STATUS status = NV_OK;
    NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *pGpuProbedIds = NULL;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pGpuProbedIds = portMemAllocNonPaged(sizeof(*pGpuProbedIds));
    if (pGpuProbedIds == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    status = gpumgrGetProbedGpuIds(pGpuProbedIds);
    if (status != NV_OK)
    {
        goto done;
    }

    status = _cliresValidateGpuIdAgainstProbed(pAsyncAttachIdParams->gpuId,
                                               pGpuProbedIds);
    if (status != NV_OK)
    {
        goto done;
    }

done:
    portMemFree(pGpuProbedIds);
    return status;
}

//
// cliresCtrlCmdGpuWaitAttachId
//
// Lock Requirements: none
//
NV_STATUS
cliresCtrlCmdGpuWaitAttachId_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_WAIT_ATTACH_ID_PARAMS *pWaitAttachIdParams
)
{
    //
    // Similar to non-async attach, async attach is mostly handled by
    // libnvrmapi in userspace. That includes the logic for waiting for
    // background attach operations to complete.
    //
    // Since background attach operations are not tracked by Core RM
    // (that is the responsibility of libnvrmapi and the kernel interface
    // layer), there is nothing to do here.
    //
    // Note: libnvrmapi on UNIX skips calling into Core RM entirely for this
    // command, so this path (and the unneeded API lock acquire) is not taken.
    //

    return NV_OK;
}

//
// cliresCtrlCmdGpuDetachIds
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuDetachIds_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_DETACH_IDS_PARAMS *pGpuDetachIds
)
{
    NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS *pGpuAttachedIds = NULL;
    NvU32 i, j;
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if (pGpuDetachIds->gpuIds[0] == NV0000_CTRL_GPU_DETACH_ALL_ATTACHED_IDS)
    {
        // XXX add callback to detach logic on Windows
        status = NV_OK;
        goto done;
    }
    else
    {
        pGpuAttachedIds = portMemAllocNonPaged(sizeof(*pGpuAttachedIds));
        if (pGpuAttachedIds == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        status = gpumgrGetAttachedGpuIds(pGpuAttachedIds);
        if (status != NV_OK)
        {
            goto done;
        }

        for (i = 0; (i < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS) &&
                    (pGpuDetachIds->gpuIds[i] != NV0000_CTRL_GPU_INVALID_ID); i++)
        {
            for (j = 0; (j < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS) &&
                        (pGpuAttachedIds->gpuIds[j] != NV0000_CTRL_GPU_INVALID_ID); j++)
            {
                if (pGpuDetachIds->gpuIds[i] == pGpuAttachedIds->gpuIds[j])
                    break;
            }

            if ((j == NV0000_CTRL_GPU_MAX_ATTACHED_GPUS) ||
                (pGpuAttachedIds->gpuIds[j] == NV0000_CTRL_GPU_INVALID_ID))
            {
                status = NV_ERR_INVALID_ARGUMENT;
                break;
            }
            else
            {
                // XXX add callback to detach logic on Windows
                break;
            }
        }
    }

done:
    portMemFree(pGpuAttachedIds);
    return status;
}

NV_STATUS
cliresCtrlCmdGpuGetSvmSize_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS *pSvmSizeGetParams
)
{
    OBJGPU *pGpu = NULL;

    // error check incoming gpu id
    pGpu = gpumgrGetGpuFromId(pSvmSizeGetParams->gpuId);
    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_WARNING, "GET_SVM_SIZE: bad gpuid: 0x%x\n",
                  pSvmSizeGetParams->gpuId);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Get the SVM size in MB.
    pSvmSizeGetParams->svmSize = 0;
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdGsyncGetAttachedIds_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS *pGsyncAttachedIds
)
{
    return gsyncGetAttachedIds(pGsyncAttachedIds);
}

NV_STATUS
cliresCtrlCmdGsyncGetIdInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS *pGsyncIdInfoParams
)
{
    return gsyncGetIdInfo(pGsyncIdInfoParams);
}

NV_STATUS
cliresCtrlCmdEventSetNotification_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pEventSetNotificationParams
)
{
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);

    return CliControlSystemEvent(hClient, pEventSetNotificationParams->event, pEventSetNotificationParams->action);
}

NV_STATUS
cliresCtrlCmdEventGetSystemEventStatus_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS *pSystemEventStatusParams
)
{
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);

    return CliGetSystemEventStatus(hClient, &pSystemEventStatusParams->event, &pSystemEventStatusParams->status);
}

NV_STATUS
cliresCtrlCmdGpuAcctGetProcAccountingInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS *pAcctInfoParams
)
{
    OBJSYS        *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU        *pGpu;
    NV_STATUS      status = NV_OK;
    CALL_CONTEXT  *pCallContext;
    RmCtrlParams  *pRmCtrlParams;
    RM_API        *pRmApi;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pAcctInfoParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (IS_VIRTUAL(pGpu))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_GET_ENGINE_UTILIZATION(pGpu,
                                         pRmCtrlParams->hClient,
                                         pRmCtrlParams->hObject,
                                         pRmCtrlParams->cmd,
                                         pAcctInfoParams,
                                         sizeof(NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS),
                                         status);
        return status;
    }
    else if (IS_GSP_CLIENT(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) &&
             (pAcctInfoParams->subPid != 0))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams;
        pRmApi        = GPU_GET_PHYSICAL_RMAPI(pGpu);

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return gpuacctGetProcAcctInfo(pGpuAcct, pAcctInfoParams);
}

NV_STATUS
cliresCtrlCmdGpuAcctSetAccountingState_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *pParams
)
{
    OBJGPU       *pGpu = NULL;
    NV_STATUS     status = NV_OK;
    CALL_CONTEXT *pCallContext;
    RmCtrlParams *pRmCtrlParams;
    RM_API       *pRmApi;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    OBJSYS     *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);

    if (IS_VIRTUAL(pGpu))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_GET_ENGINE_UTILIZATION(pGpu,
                                         pRmCtrlParams->hClient,
                                         pRmCtrlParams->hObject,
                                         pRmCtrlParams->cmd,
                                         pParams,
                                         sizeof(NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS),
                                         status);
        return status;
    }
    else if (IS_GSP_CLIENT(pGpu))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams;
        pRmApi        = GPU_GET_PHYSICAL_RMAPI(pGpu);

        status = pRmApi->Control(pRmApi,
                                 pRmCtrlParams->hClient,
                                 pRmCtrlParams->hObject,
                                 pRmCtrlParams->cmd,
                                 pRmCtrlParams->pParams,
                                 pRmCtrlParams->paramsSize);

        if (status != NV_OK)
            return status;
    }

    if (NV0000_CTRL_GPU_ACCOUNTING_STATE_ENABLED == pParams->newState)
    {
        status = gpuacctEnableAccounting(pGpuAcct,
            pGpu->gpuInstance, pParams);
    }
    else if (NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED == pParams->newState)
    {
        status = gpuacctDisableAccounting(pGpuAcct,
            pGpu->gpuInstance, pParams);
    }
    else
    {
        status = NV_ERR_INVALID_ARGUMENT;
    }

    return status;
}

NV_STATUS
cliresCtrlCmdGpuAcctClearAccountingData_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS *pParams
)
{
    OBJSYS         *pSys = SYS_GET_INSTANCE();
    GpuAccounting  *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU         *pGpu = NULL;
    CALL_CONTEXT   *pCallContext;
    RmCtrlParams   *pRmCtrlParams;
    NV_STATUS       status = NV_OK;
    RM_API         *pRmApi;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (IS_VIRTUAL(pGpu))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_GET_ENGINE_UTILIZATION(pGpu,
                                         pRmCtrlParams->hClient,
                                         pRmCtrlParams->hObject,
                                         pRmCtrlParams->cmd,
                                         &pParams,
                                         sizeof(NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS),
                                         status);
        return status;
    }
    else if (IS_GSP_CLIENT(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && (pParams->pid != 0))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams;
        pRmApi        = GPU_GET_PHYSICAL_RMAPI(pGpu);

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return gpuacctClearAccountingData(pGpuAcct, pGpu->gpuInstance, pParams);
}

NV_STATUS
cliresCtrlCmdGpuAcctGetAccountingState_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS *pParams
)
{
    OBJSYS         *pSys = SYS_GET_INSTANCE();
    GpuAccounting  *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU         *pGpu = NULL;
    CALL_CONTEXT   *pCallContext;
    RmCtrlParams   *pRmCtrlParams;
    NV_STATUS       status = NV_OK;
    RM_API         *pRmApi;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (IS_VIRTUAL(pGpu))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_GET_ENGINE_UTILIZATION(pGpu,
                                         pRmCtrlParams->hClient,
                                         pRmCtrlParams->hObject,
                                         pRmCtrlParams->cmd,
                                         pParams,
                                         sizeof(NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS),
                                         status);
        return status;
    }
    else if (IS_GSP_CLIENT(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && (pParams->pid != 0))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams;
        pRmApi        = GPU_GET_PHYSICAL_RMAPI(pGpu);

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return gpuacctGetAccountingMode(pGpuAcct, pGpu->gpuInstance, pParams);
}

NV_STATUS
cliresCtrlCmdGpuAcctGetAccountingPids_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS *pAcctPidsParams
)
{
    OBJSYS         *pSys = SYS_GET_INSTANCE();
    GpuAccounting  *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU         *pGpu;
    CALL_CONTEXT   *pCallContext;
    RmCtrlParams   *pRmCtrlParams;
    NV_STATUS       status = NV_OK;
    RM_API         *pRmApi;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pAcctPidsParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (IS_VIRTUAL(pGpu))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_GET_ENGINE_UTILIZATION(pGpu,
                                         pRmCtrlParams->hClient,
                                         pRmCtrlParams->hObject,
                                         pRmCtrlParams->cmd,
                                         pAcctPidsParams,
                                         sizeof(NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS),
                                         status);
        return status;
    }
    else if (IS_GSP_CLIENT(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) &&
             (pAcctPidsParams->pid != 0))
    {
        pCallContext  = resservGetTlsCallContext();
        pRmCtrlParams = pCallContext->pControlParams;
        pRmApi        = GPU_GET_PHYSICAL_RMAPI(pGpu);

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return gpuacctGetAcctPids(pGpuAcct, pAcctPidsParams);
}

NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrControl_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS *controlParams
)
{
    OBJSYS                  *pSys                    = NULL;
    PlatformRequestHandler  *pPlatformRequestHandler = NULL;
    NV_STATUS  ret   = NV_OK;
    NvU32      data  = 0;

    pSys = SYS_GET_INSTANCE();
    if (!pSys)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    data = controlParams->data;

    pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);
    if (!pPlatformRequestHandler)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    ret = pfmreqhndlrControl(pPlatformRequestHandler, controlParams->command, controlParams->locale, &data);

    if (NV_OK == ret)
    {
        controlParams->data = data;
    }
    else
    {
        controlParams->data = NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INVALID;
    }

    return ret;
}

NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrBatchControl_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS *controlParams
)
{
    OBJSYS    *pSys  = NULL;
    PlatformRequestHandler       *pPlatformRequestHandler  = NULL;
    NvU32      data  = 0;
    NvU32      i     = 0;
    NvU32      cnt   = 0;

    if (controlParams->cmdCount > NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_BATCH_COMMAND_MAX)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    pSys = SYS_GET_INSTANCE();
    if (!pSys)
    {
        NV_ASSERT(pSys);
        return NV_ERR_INVALID_REQUEST;
    }

    pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);
    if (!pPlatformRequestHandler)
    {
        NV_ASSERT(pPlatformRequestHandler);
        return NV_ERR_INVALID_REQUEST;
    }

    for (i = 0; i < controlParams->cmdCount; i++)
    {
        data = controlParams->cmdData[i].data;
        if (pfmreqhndlrControl(pPlatformRequestHandler,
                       controlParams->cmdData[i].command,
                       controlParams->cmdData[i].locale, &data) == NV_OK)
        {
            controlParams->cmdData[i].data = data;
            cnt++;
        }
        else
        {
            controlParams->cmdData[i].data = NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INVALID;
        }
    }

    controlParams->succeeded = cnt;

    return NV_OK;
}

/*!
 * Helper to build config data from unpacked table data,
 * static config v2.0/2.1.
 *
 * @param[in]  pEntry          Unpacked data from static table
 * @param[out] pParams         Structure to fill parsed info
 *
 */
static void
_controllerBuildConfig_StaticTable_v20
(
    CONTROLLER_STATIC_TABLE_ENTRY_V20 *pEntry,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    pParams->samplingMulti   =
        (NvU16)pEntry->samplingMulti;
    pParams->filterType      =
        (NvU8)DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _FILTER, _TYPE,
                      pEntry->filterParams);
    pParams->filterReserved  =
        (NvU16)DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _FILTER, _RESERVED,
                       pEntry->filterParams);

    // Get the filter param based on filter type
    switch (pParams->filterType)
    {
        case NVPCF0100_CTRL_CONTROLLER_FILTER_TYPE_EMWA:
        {
            pParams->filterParam.weight =
                (NvU8)DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _FILTERPARAM, _EWMA_WEIGHT,
                              pEntry->filterParams);
            break;
        }

        case NVPCF0100_CTRL_CONTROLLER_FILTER_TYPE_MOVING_MAX:
        default:
        {
            pParams->filterParam.windowSize =
                (NvU8)DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _FILTERPARAM, _WINDOW_SIZE,
                              pEntry->filterParams);
            break;
        }
    }
}

/*!
 * Helper to build Qboost's config data from unpacked table data,
 * static config v2.0/2.1.
 *
 * @param[in]  pEntry          Unpacked data from static table
 * @param[out] pParams         Structure to fill parsed info
 *
 */
static void
_controllerBuildQboostConfig_StaticTable_v20
(
    CONTROLLER_STATIC_TABLE_ENTRY_V20 *pEntry,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{

    pParams->bIsBoostController = NV_TRUE;

    // Type-specific param0
    pParams->incRatio =
        (NvUFXP4_12)DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _PARAM0, _QBOOST_INCREASE_GAIN,
            pEntry->param0);
    pParams->decRatio =
        (NvUFXP4_12)DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _PARAM0, _QBOOST_DECREASE_GAIN,
            pEntry->param0);

    // Type-specific param1
    pParams->bSupportBatt =
        (NvBool)(DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _PARAM1, _QBOOST_DC_SUPPORT,
            pEntry->param1));

}

/*!
 * Helper to build config data from unpacked table data,
 * static config v2.2.
 *
 * @param[in]  pEntry          Unpacked data from static table
 * @param[out] pParams         Structure to fill parsed info
 *
 */
static void
_controllerBuildConfig_StaticTable_v22
(
    CONTROLLER_STATIC_TABLE_ENTRY_V22 *pEntry,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    pParams->samplingMulti          = CONTROLLER_GRP_DEFAULT_SAMPLING_MULTIPLIER;
    pParams->filterType             = NVPCF0100_CTRL_CONTROLLER_FILTER_TYPE_EMWA;
    pParams->filterParam.weight     = CONTROLLER_GRP_DEFAULT_EWMA_WEIGHT;
}

/*!
 * Helper to build Qboost's config data from unpacked table data,
 * static config v2.2.
 *
 * @param[in]  pEntry          Unpacked data from static table
 * @param[out] pParams         Structure to fill parsed info
 *
 */
static void
_controllerBuildQboostConfig_StaticTable_v22
(
    CONTROLLER_STATIC_TABLE_ENTRY_V22 *pEntry,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    pParams->bIsBoostController = NV_TRUE;

    // Use increase gain of 90%, decrease gain of 100%
    pParams->incRatio = CONTROLLER_GRP_DEFAULT_INCREASE_GAIN_UFXP4_12;
    pParams->decRatio = CONTROLLER_GRP_DEFAULT_DECREASE_GAIN_UFXP4_12;

    // Type-specific param0
    pParams->bSupportBatt =
        (NvBool)(DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V22, _PARAM0, _QBOOST_DC_SUPPORT,
            pEntry->param0));
}

/*!
 * Helper to build CTGP controller's config data from unpacked table data,
 * static config 2x version.  Re-uses struct types from normal Qboost
 * controller.
 *
 * @param[out] pParams         Structure to fill parsed info
 *
 */
static void
_controllerBuildCtgpConfig_StaticTable_2x
(
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    //
    // Sampling period only really only affects the delay in handling
    // CTGP changes, so just set sampling period multiplier to 1
    //
    // Force EWMA filter type with weight 0, since currently the reading
    // and filtering of CPU power is still done
    //
    pParams->samplingMulti       = CONTROLLER_GRP_DEFAULT_SAMPLING_MULTIPLIER;
    pParams->filterType          = NVPCF0100_CTRL_CONTROLLER_FILTER_TYPE_EMWA;
    pParams->filterParam.weight  = 0;

    // Inform apps that there is no Dynamic Boost support
    pParams->bIsBoostController = NV_FALSE;
    pParams->incRatio = 0;
    pParams->decRatio = 0;
    pParams->bSupportBatt = NV_FALSE;
}

/*!
 * Attempts to parse the static controller table, as v2.0 or v2.1.
 *
 * @param[in]  pData                 Pointer to start (header) of the table
 * @param[in]  dataSize              Size of entire table, including header
 * @param[out] pEntryCount           Number of controller entries found
 * @param[out] pParams               Structure to fill parsed info
 *
 * @return NV_OK
 *     Table was successfully parsed; caller should remember to free object array
 * @return NV_ERR_NOT_SUPPORTED
 *     Failed to detect correct table version, no output
 * @return Other errors
 *     NV_ERR_INVALID_DATA or errors propogated up from functions called
 */
static NV_STATUS
_controllerParseStaticTable_v20
(
    NvU8 *pData,
    NvU32 dataSize,
    NvU8 *pEntryCount,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    const char *pHeaderFmt = NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V20_FMT_SIZE_05;
    const char *pCommonFmt = NVPCF_CONTROLLER_STATIC_TABLE_COMMON_V20_FMT_SIZE_02;
    const char *pEntryFmt  = NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FMT_SIZE_0F;
    NvU32 loop               = 0;
    NV_STATUS   status     = NV_OK;

    CONTROLLER_STATIC_TABLE_HEADER_V20 header = { 0 };
    CONTROLLER_STATIC_TABLE_COMMON_V20 common = { 0 };

    // Check if we can safely parse the header
    if (dataSize < NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V20_SIZE_05)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto _controllerParseStaticTable_v20_exit;
    }

    // Unpack the table header
    configReadStructure(pData, &header, 0, pHeaderFmt);

    switch (header.version)
    {
        case NVPCF_CONTROLLER_STATIC_TABLE_VERSION_20:
        case NVPCF_CONTROLLER_STATIC_TABLE_VERSION_21:
        {
            NvU32 expectedSize;

            // check rest of header
            if ((header.headerSize != NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V20_SIZE_05)
                || (header.commonSize != NVPCF_CONTROLLER_STATIC_TABLE_COMMON_V20_SIZE_02)
                || (header.entrySize != NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_SIZE_0F))
            {
                status = NV_ERR_INVALID_DATA;
                goto _controllerParseStaticTable_v20_exit;
            }

            // must have at least one entry
            if (header.entryCount == 0)
            {
                status = NV_ERR_INVALID_DATA;
                goto _controllerParseStaticTable_v20_exit;
            }

            // check data size
            expectedSize = header.headerSize + header.commonSize
                + (header.entryCount * header.entrySize);
            if (expectedSize != dataSize)
            {
                status = NV_ERR_INVALID_DATA;
                goto _controllerParseStaticTable_v20_exit;
            }
            break;
        }
        default:
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto _controllerParseStaticTable_v20_exit;
        }
    }

    // Unpack the common data, base sampling period cannot be 0
    configReadStructure(pData, &common, header.headerSize, pCommonFmt);

    if (common.samplingPeriodms == 0)
    {
        status = NV_ERR_INVALID_DATA;
        goto _controllerParseStaticTable_v20_exit;
    }
    pParams->samplingPeriodmS = (NvU16)common.samplingPeriodms;

    // Parse each entry
    for (loop = 0; loop < header.entryCount; loop++)
    {
        CONTROLLER_STATIC_TABLE_ENTRY_V20 entry = { 0 };

        NvU32 offset = header.headerSize + header.commonSize +
            (loop * NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_SIZE_0F);

        // Unpack the controller entry
        configReadStructure(pData, &entry, offset, pEntryFmt);

        switch (DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V20, _FLAGS0, _CLASS,
            entry.flags0))
        {
            case NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FLAGS0_CLASS_PPAB:
            {
                _controllerBuildConfig_StaticTable_v20(&entry, pParams);
                _controllerBuildQboostConfig_StaticTable_v20(&entry, pParams);
                break;
            }
            case NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FLAGS0_CLASS_CTGP:
            {
                _controllerBuildCtgpConfig_StaticTable_2x(pParams);
                break;
            }
            case NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FLAGS0_CLASS_DISABLED:
            default:
            {
            }
        }
    }

    pParams->version = (NvU8)header.version;
    *pEntryCount = (NvU8)header.entryCount;

_controllerParseStaticTable_v20_exit:
    return status;
}

/*!
 * Attempts to parse the static controller table, as v2.2.
 *
 * @param[in]  pData                 Pointer to start (header) of the table
 * @param[in]  dataSize              Size of entire table, including header
 * @param[out] pEntryCount           Number of controller entries found
 * @param[out] pParams               Structure to fill parsed info
 *
 * @return NV_OK
 *     Table was successfully parsed; caller should remember to free object array
 * @return NV_ERR_NOT_SUPPORTED
 *     Failed to detect correct table version, no output
 * @return Other errors
 *     NV_ERR_INVALID_DATA or errors propogated up from functions called
 */
static NV_STATUS
_controllerParseStaticTable_v22
(
    NvU8 *pData,
    NvU32 dataSize,
    NvU8 *pEntryCount,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    const char *pHeaderFmt = NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V22_FMT_SIZE_04;
    const char *pEntryFmt  = NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FMT_SIZE_05;
    NV_STATUS   status     = NV_OK;
    NvU32 loop               = 0;

    CONTROLLER_STATIC_TABLE_HEADER_V22 header = { 0 };

    // Check if we can safely parse the header
    if (dataSize < NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V22_SIZE_04)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto _controllerParseStaticTable_v22_exit;
    }

    // Unpack the table header
    configReadStructure(pData, &header, 0, pHeaderFmt);

    switch (header.version)
    {
        case NVPCF_CONTROLLER_STATIC_TABLE_VERSION_24:
        case NVPCF_CONTROLLER_STATIC_TABLE_VERSION_23:
        case NVPCF_CONTROLLER_STATIC_TABLE_VERSION_22:
        {
            NvU32 expectedSize;

            // check rest of header
            if ((header.headerSize != NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V22_SIZE_04)
                || (header.entrySize != NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_SIZE_05))
            {
                status = NV_ERR_INVALID_DATA;
                goto _controllerParseStaticTable_v22_exit;
            }

            // must have at least one entry
            if (header.entryCount == 0)
            {
                status = NV_ERR_INVALID_DATA;
                goto _controllerParseStaticTable_v22_exit;
            }

            // check data size
            expectedSize = header.headerSize + (header.entryCount * header.entrySize);
            if (expectedSize != dataSize)
            {
                status = NV_ERR_INVALID_DATA;
                goto _controllerParseStaticTable_v22_exit;
            }

            break;
        }
        default:
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto _controllerParseStaticTable_v22_exit;
        }
    }

    // Parse each entry
    for (loop = 0; loop < header.entryCount; loop++)
    {
        CONTROLLER_STATIC_TABLE_ENTRY_V22 entry = { 0 };

        NvU32 offset = header.headerSize +
            (loop * NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_SIZE_05);

        // Unpack the controller entry
        configReadStructure(pData, &entry, offset, pEntryFmt);

        switch (DRF_VAL(PCF_CONTROLLER_STATIC_TABLE_ENTRY_V22, _FLAGS0, _CLASS,
            entry.flags0))
        {
            case NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FLAGS0_CLASS_PPAB:
            {
                _controllerBuildConfig_StaticTable_v22(&entry, pParams);
                _controllerBuildQboostConfig_StaticTable_v22(&entry, pParams);
                break;
            }
            case NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FLAGS0_CLASS_CTGP:
            {
                _controllerBuildCtgpConfig_StaticTable_2x(pParams);
                break;
            }
            case NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FLAGS0_CLASS_DISABLED:
            default:
            {
            }
        }
    }

    pParams->version    = (NvU8)header.version;
    pParams->samplingPeriodmS = CONTROLLER_GRP_DEFAULT_BASE_SAMPLING_PERIOD_MS;
    *pEntryCount = (NvU8)header.entryCount;

_controllerParseStaticTable_v22_exit:
    return status;
}

static NV_STATUS
_sysDeviceParseStaticTable_2x
(
    NvU8 *pData,
    NvU32 *dataSize,
    NvU32 *controllerTableOffset,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    NV_STATUS                       status              = NV_OK;
    NvU32                           deviceTableOffset   = 0;
    SYSDEV_STATIC_TABLE_HEADER_2X   sysdevHeader        = { 0 };
    SYSDEV_STATIC_TABLE_COMMON_2X   common              = { 0 };
    const char                     *pSzSysDevHeaderFmt  =
        NVPCF_SYSDEV_STATIC_TABLE_HEADER_2X_FMT_SIZE_03;
    const char                     *pSzCommonFmt        =
        NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_FMT_SIZE_01;

    // Unpack the table header
    configReadStructure(pData, &sysdevHeader, deviceTableOffset, pSzSysDevHeaderFmt);

    // Check the header version and sizes
    if ((sysdevHeader.version != NVPCF_SYSDEV_STATIC_TABLE_VERSION_2X) ||
        (sysdevHeader.headerSize != NVPCF_SYSDEV_STATIC_TABLE_HEADER_2X_SIZE_03) ||
        (sysdevHeader.commonSize != NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_SIZE_01))
    {
        NV_PRINTF(LEVEL_ERROR, "NVPCF: %s: Unsupported header\n",
            __FUNCTION__);

        status = NV_ERR_INVALID_DATA;
        goto _sysDeviceParseStaticTable_2x_exit;
    }

    // Update controller table pointer based on sysdev header data
    *controllerTableOffset = deviceTableOffset + sysdevHeader.headerSize + sysdevHeader.commonSize;

    configReadStructure(pData,
                        &common,
                        deviceTableOffset + sysdevHeader.headerSize,
                        pSzCommonFmt);

    pParams->cpuType = (DRF_VAL(PCF_SYSDEV_STATIC_TABLE_COMMON_2X, _PARAM0, _CPU_TYPE,
                       common.param0));

    pParams->gpuType = (DRF_VAL(PCF_SYSDEV_STATIC_TABLE_COMMON_2X, _PARAM0, _GPU_TYPE,
                       common.param0));

_sysDeviceParseStaticTable_2x_exit:
    return status;
}

static NV_STATUS
_controllerParseStaticTable_2x
(
    NvU8 *pData,
    NvU32 dataSize,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    NvU32       controllerTableOffset   = 0;
    NvU8        entryCount              = 0;
    NV_STATUS   status                  = NV_OK;

    // Make sure we can safely parse the sysdev header
    if (dataSize < NVPCF_SYSDEV_STATIC_TABLE_HEADER_2X_SIZE_03)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto _controllerParseStaticTable_exit;
    }

    _sysDeviceParseStaticTable_2x(pData, &dataSize, &controllerTableOffset, pParams);

    // Make sure data size is at least the controller table offset
    if (dataSize < controllerTableOffset)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto _controllerParseStaticTable_exit;
    }

    pData    += controllerTableOffset;
    dataSize -= controllerTableOffset;

    status = _controllerParseStaticTable_v22(pData,
                                            dataSize,
                                            &entryCount,
                                            pParams);
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        status = _controllerParseStaticTable_v20(pData,
                                                dataSize,
                                                &entryCount,
                                                pParams);
    }

    if (status != NV_OK)
    {
        goto _controllerParseStaticTable_exit;
    }

_controllerParseStaticTable_exit:
    return status;
}

/*!
 * Helper function to validate the config static data that can be
 * received from various sources, using one byte two's complement
 * checksum. And match is against the last byte the original
 * checksum byte is stored in the data.
 *
 * @param[in/out]  pData              NvU8           data buffer pointer
 * @param[in]      pDataSize          NvU32          pointer to the data size in bytes
 *
 * @return NV_OK
 *     Checksum successfully matched.
 *
 * @return NV_ERR_INVALID_POINTER
 *     Invalid input data buffer pointer. *
 * @return NV_ERR_INVALID_DATA
 *     Checksum failure or wrong data size.
 */
static NV_STATUS
_validateConfigStaticTable_2x
(
    NvU8    *pData,
    NvU16   *pDataSize
)
{
    NV_STATUS  status    = NV_OK;
    NvU8       checkSum;
    NvU16       idx;

    NV_ASSERT_OR_RETURN(pData     != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pDataSize != NULL, NV_ERR_INVALID_POINTER);

    //
    // Check data size length for static2x data. Must be min 2 bytes
    // (CONFIG_2X_BUFF_SIZE_MIN) including 1 byte for checksum. The
    // max allowed for static2x is CONFIG_2X_BUFF_SIZE_MAX.
    //
    if ((*pDataSize < CONFIG_2X_BUFF_SIZE_MIN) ||
        (*pDataSize > NVPCF0100_CTRL_CONFIG_2X_BUFF_SIZE_MAX))
    {
        status = NV_ERR_INVALID_DATA;
        goto validateConfigStaticTable_2x_exit;
    }

    checkSum = 0;
    for (idx = 0; idx < (*pDataSize - 1); idx++)
    {
        checkSum += pData[idx];
    }
    checkSum = (~checkSum) + 1;

    // Match with the original checksum
    if (checkSum != pData[*pDataSize - 1])
    {
        status = NV_ERR_INVALID_DATA;
        goto validateConfigStaticTable_2x_exit;
    }

validateConfigStaticTable_2x_exit:
    return status;
}

NV_STATUS
cliresCtrlCmdSystemNVPCFGetPowerModeInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams
)
{
    NvU32     rc = NV_OK;
    OBJGPU   *pGpu   = NULL;
    NV_STATUS status = NV_OK;
    NvU16     dsmDataSize;
    ACPI_DSM_FUNCTION acpiDsmFunction = ACPI_DSM_FUNCTION_NVPCF_2X;
    NvU32     acpiDsmSubFunction = NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_SUPPORTED;

    if (pParams == NULL)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);

    if (pGpu == NULL)
    {
        NV_ASSERT(pGpu);
        return NV_ERR_INVALID_REQUEST;
    }

    switch (pParams->subFunc)
    {
        case NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_SUPPORTED_CASE:
        {
            NvU32       supportedFuncs;
            dsmDataSize = sizeof(supportedFuncs);

            if ((rc = osCallACPI_DSM(pGpu,
                                     acpiDsmFunction,
                                     acpiDsmSubFunction,
                                     &supportedFuncs,
                                     &dsmDataSize)) != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING,
                    "Unable to retrieve NVPCF supported functions. Possibly not supported by SBIOS "
                    "rc = %x\n", rc);
                status =  NV_ERR_NOT_SUPPORTED;
            }
            else
            {
                if ((FLD_TEST_DRF(PCF0100, _CTRL_CONFIG_DSM,
                        _FUNC_GET_SUPPORTED_IS_SUPPORTED, _NO, supportedFuncs)) ||
                        (dsmDataSize != sizeof(supportedFuncs)))
                {
                    NV_PRINTF(LEVEL_WARNING,
                        " NVPCF FUNC_GET_SUPPORTED is not supported"
                        "rc = %x\n", rc);
                    status = NV_ERR_NOT_SUPPORTED;
                }
            }
            break;

        }

        case NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_CASE:
        {
            NvU8 *pData = NULL;
            DYNAMIC_PARAMS_HEADER_2X_PACKED header = {0};
            DYNAMIC_PARAMS_COMMON_2X_PACKED common = {0};
            DYNAMIC_PARAMS_ENTRY_2X_PACKED  entries[NVPCF_DYNAMIC_PARAMS_2X_ENTRY_MAX]= {0};

            NvU16 dataSize = sizeof(header) + sizeof(common) + sizeof(entries);
            pData = portMemAllocNonPaged(dataSize);

            portMemSet(&header, 0, sizeof(header));

            header.version    = pParams->version;
            header.headerSize = NVPCF_DYNAMIC_PARAMS_2X_HEADER_SIZE_05;
            header.commonSize = NVPCF_DYNAMIC_PARAMS_2X_COMMON_SIZE_10;
            header.entrySize  = NVPCF_DYNAMIC_PARAMS_2X_ENTRY_SIZE_1C;
            header.entryCount = 0;

            pParams->bRequireDcSysPowerLimitsTable =
                (pParams->version >= NVPCF_CONTROLLER_STATIC_TABLE_VERSION_22);
            pParams->bAllowDcRestOfSystemReserveOverride =
                (pParams->version >= NVPCF_CONTROLLER_STATIC_TABLE_VERSION_23);
            pParams->bSupportDcTsp =
                (pParams->version >= NVPCF_CONTROLLER_STATIC_TABLE_VERSION_24);

            common.param0 = FLD_SET_DRF(PCF_DYNAMIC_PARAMS_COMMON_2X,
                _INPUT_PARAM0, _CMD, _GET, common.param0);

            portMemCopy(pData, sizeof(header), &header, sizeof(header));
            portMemCopy(pData + sizeof(header), sizeof(common), &common, sizeof(common));
            portMemCopy(pData + sizeof(header) + sizeof(common), sizeof(entries), entries, sizeof(entries));

            if ((rc = osCallACPI_DSM(pGpu,
                            ACPI_DSM_FUNCTION_NVPCF_2X,
                            NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_PARAMS,
                            (NvU32 *)pData,
                            &dataSize)) != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING,
                "Unable to retrieve NVPCF dynamic data. Possibly not supported by SBIOS"
                "rc = %x\n", rc);
                status =  NV_ERR_NOT_SUPPORTED;
                goto nvpcf2xGetDynamicParams_exit;
            }

            if (dataSize < (sizeof(header) + sizeof(common)))
            {
                status = NV_ERR_INVALID_DATA;
                goto nvpcf2xGetDynamicParams_exit;
            }

            DYNAMIC_PARAMS_HEADER_2X   headerOut     = { 0 };
            DYNAMIC_PARAMS_COMMON_2X   commonOut     = { 0 };
            DYNAMIC_PARAMS_ENTRY_2X    entriesOut    = { 0 };
            NvU32 idx;

            const char  *pSzHeaderFmt     =
                NVPCF_DYNAMIC_PARAMS_2X_HEADER_FMT_SIZE_05;
            const char  *pSzCommonFmt     =
                NVPCF_DYNAMIC_PARAMS_2X_COMMON_FMT_SIZE_10;
            const char  *pSzEntryFmt      =
                NVPCF_DYNAMIC_PARAMS_2X_ENTRY_FMT_SIZE_1C;

            // Unpack the header part
            configReadStructure(pData, (void *)&headerOut, 0, pSzHeaderFmt);

            if (headerOut.version != pParams->version)
            {
                status = NV_ERR_INVALID_DATA;
                goto nvpcf2xGetDynamicParams_exit;
            }

            if ((headerOut.headerSize != NVPCF_DYNAMIC_PARAMS_2X_HEADER_SIZE_05) ||
                (headerOut.commonSize != NVPCF_DYNAMIC_PARAMS_2X_COMMON_SIZE_10) ||
                (headerOut.entrySize != NVPCF_DYNAMIC_PARAMS_2X_ENTRY_SIZE_1C))
            {
                status = NV_ERR_INVALID_DATA;
                goto nvpcf2xGetDynamicParams_exit;
            }

            // Check total size
            if (dataSize < (
                NVPCF_DYNAMIC_PARAMS_2X_HEADER_SIZE_05 +
                NVPCF_DYNAMIC_PARAMS_2X_COMMON_SIZE_10 +
                (headerOut.entryCount * NVPCF_DYNAMIC_PARAMS_2X_ENTRY_SIZE_1C)))
            {
                status = NV_ERR_INVALID_DATA;
                goto nvpcf2xGetDynamicParams_exit;
            }

            // Unpack the common part
            configReadStructure(pData, (void *)&commonOut,
                NVPCF_DYNAMIC_PARAMS_2X_HEADER_SIZE_05, pSzCommonFmt);

            for (idx = 0; idx < headerOut.entryCount; idx++)
            {
                NvU32                    dataOffset;

                dataOffset = NVPCF_DYNAMIC_PARAMS_2X_HEADER_SIZE_05 +
                     NVPCF_DYNAMIC_PARAMS_2X_COMMON_SIZE_10 +
                     (idx * NVPCF_DYNAMIC_PARAMS_2X_ENTRY_SIZE_1C);

                // Unpack the controller entry
                configReadStructure(pData, (void *)&entriesOut,
                     dataOffset, pSzEntryFmt);

                // Enable/disable Dynamic Boost (SBIOS uses 1 = disable)
                pParams->bEnableForAC = !DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                     _OUTPUT_PARAM0, _CMD0_DISABLE_AC, entriesOut.param0);

                // Configurable TGP is from common
                pParams->ctgpOffsetmW = (NvS32)NVPCF_DYNAMIC_PARAMS_2X_POWER_UNIT_MW *
                    (NvS16)DRF_VAL(PCF_DYNAMIC_PARAMS_COMMON_2X,
                        _OUTPUT_PARAM0, _CMD0_CTGP_AC_OFFSET, commonOut.param0);

                // Rest of AC DB params
                pParams->targetTppOffsetmW = (NvS32)NVPCF_DYNAMIC_PARAMS_2X_POWER_UNIT_MW *
                    (NvS16)DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                        _OUTPUT_PARAM1, _CMD0_SIGNED0, entriesOut.param1);
                pParams->maxOutputOffsetmW = (NvS32)NVPCF_DYNAMIC_PARAMS_2X_POWER_UNIT_MW *
                    (NvS16)DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                        _OUTPUT_PARAM2, _CMD0_SIGNED0, entriesOut.param2);
                pParams->minOutputOffsetmW = (NvS32)NVPCF_DYNAMIC_PARAMS_2X_POWER_UNIT_MW *
                    (NvS16)DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                        _OUTPUT_PARAM2, _CMD0_SIGNED0, entriesOut.param3);

                // DC_ENABLE command
                pParams->bEnableForDC = !DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                    _OUTPUT_PARAM0, _CMD0_DISABLE_DC, entriesOut.param0);

                if (!pParams->bRequireDcSysPowerLimitsTable)
                {
                    // DC CTGP offset
                    pParams->ctgpBattOffsetmW = (NvS32)NVPCF_DYNAMIC_PARAMS_2X_POWER_UNIT_MW *
                        (NvS16)DRF_VAL(PCF_DYNAMIC_PARAMS_COMMON_2X,
                            _OUTPUT_PARAM0, _CMD0_CTGP_DC_OFFSET, common.param0);

                    // DC TPP target offset
                    pParams->targetTppBattOffsetmW = (NvS32)NVPCF_DYNAMIC_PARAMS_2X_POWER_UNIT_MW *
                        (NvS16)DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                            _OUTPUT_PARAM1, _CMD0_SIGNED1, entriesOut.param1);
                }

                if (pParams->bAllowDcRestOfSystemReserveOverride)
                {
                    pParams->dcRosReserveOverridemW = (NvU32)DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                            _OUTPUT_PARAM4, _CMD0_UNSIGNED, entriesOut.param4);
                }

                if (pParams->bSupportDcTsp)
                {
                    pParams->dcTspLongTimescaleLimitmA = (NvU32)DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                            _OUTPUT_PARAM5, _CMD0_UNSIGNED, entriesOut.param5);
                    pParams->dcTspShortTimescaleLimitmA = (NvU32)DRF_VAL(PCF_DYNAMIC_PARAMS_ENTRY_2X,
                            _OUTPUT_PARAM6, _CMD0_UNSIGNED, entriesOut.param6);
                }
            }

nvpcf2xGetDynamicParams_exit:
            portMemFree(pData);

            break;
        }
        case NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CASE:
        {
            NvU8 *pData = NULL;
            NvU16 dataSize =  NVPCF0100_CTRL_CONFIG_2X_BUFF_SIZE_MAX;

            pData = portMemAllocNonPaged(dataSize);

            if ((rc = osCallACPI_DSM(pGpu,
                            ACPI_DSM_FUNCTION_NVPCF_2X,
                            NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CONFIG_TABLES,
                            (NvU32 *)pData,
                            &dataSize)) != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING,
                "Unable to retrieve NVPCF Static data. Possibly not supported by SBIOS"
                "rc = %x\n", rc);
                status =  NV_ERR_NOT_SUPPORTED;
                goto nvpcf2xGetStaticParams_exit;
            }

            status = _validateConfigStaticTable_2x(pData, &dataSize);
            if (NV_OK != status)
            {
                NV_PRINTF(LEVEL_WARNING, "Config Static Data checksum failed\n");
                status =  NV_ERR_NOT_SUPPORTED;
                goto nvpcf2xGetStaticParams_exit;
            }

            // Subtract 1 byte for the checksum
            dataSize--;

            status = _controllerParseStaticTable_2x(pData, dataSize, pParams);
            if (NV_OK != status)
            {
                status =  NV_ERR_NOT_SUPPORTED;
            }

nvpcf2xGetStaticParams_exit:
            portMemFree(pData);
            break;

        }
        default:
        {
            NV_PRINTF(LEVEL_INFO, "Inavlid NVPCF subFunc : 0x%x\n", pParams->subFunc);
            status =  NV_ERR_NOT_SUPPORTED;
        }

    }
    return status;

}

static void
getHwbcInfo
(
    NV0000_CTRL_SYSTEM_HWBC_INFO *pHwbcInfo,
    OBJHWBC *pHWBC,
    NvU32 *pIndex
)
{
    if (pHWBC->pFirstChild)
        getHwbcInfo(pHwbcInfo, pHWBC->pFirstChild, pIndex);
    if (pHWBC->pSibling)
        getHwbcInfo(pHwbcInfo, pHWBC->pSibling, pIndex);

    if (HWBC_NVIDIA_BR04 == pHWBC->bcRes)
    {
        if (*pIndex >= NV0000_CTRL_SYSTEM_MAX_HWBCS)
        {
            //
            // Should never happen! Return whatever info we've
            // gathered till now.
            //
            NV_ASSERT(*pIndex < NV0000_CTRL_SYSTEM_MAX_HWBCS);
            return;
        }
        pHwbcInfo[*pIndex].hwbcId = pHWBC->hwbcId;
        pHwbcInfo[*pIndex].firmwareVersion = pHWBC->fwVersion;
        pHwbcInfo[*pIndex].subordinateBus = pHWBC->maxBus;
        pHwbcInfo[*pIndex].secondaryBus = pHWBC->minBus;
        (*pIndex)++;
    }
}

NV_STATUS
cliresCtrlCmdSystemGetHwbcInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS *pParams
)
{
    NV0000_CTRL_SYSTEM_HWBC_INFO *pHwbcInfo = pParams->hwbcInfo;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);
    NvU32   index = 0;

    if (pCl->pHWBC)
        getHwbcInfo(pHwbcInfo, pCl->pHWBC, &index);

    for (; index < NV0000_CTRL_SYSTEM_MAX_HWBCS; index++)
    {
        pHwbcInfo[index].hwbcId = NV0000_CTRL_SYSTEM_HWBC_INVALID_ID;
    }

    return NV_OK;
}

/*!
 * @brief Get Dump Size. Returns an estimate of the number of bytes in the dump
 * that can be used to allocate a buffer. The size is based on the component
 * argument.
 *
 * @returns NV_OK on success
 */
NV_STATUS
cliresCtrlCmdNvdGetDumpSize_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams
)
{
    OBJSYS       *pSys  = SYS_GET_INSTANCE();
    Journal      *pRcDB = SYS_GET_RCDB(pSys);
    NVDUMP_BUFFER nvDumpBuffer = {0};
    NV_STATUS     rmStatus;

    nvDumpBuffer.size = NV0000_CTRL_NVD_MAX_DUMP_SIZE;

    rmStatus = rcdbDumpComponent(pRcDB, pDumpSizeParams->component,
                                 &nvDumpBuffer,
                                 NVDUMP_BUFFER_COUNT,
                                 NULL);

    pDumpSizeParams->size = nvDumpBuffer.curNumBytes;

    return rmStatus;
}

/*!
 * @brief Get Dump. Returns a dump that includes the component specified
 * when the conditions in the trigger are set.
 *
 * @returns NV_OK on success
 */
NV_STATUS
cliresCtrlCmdNvdGetDump_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams
)
{
    OBJSYS       *pSys          = SYS_GET_INSTANCE();
    Journal      *pRcDB         = SYS_GET_RCDB(pSys);
    NVDUMP_BUFFER nvDumpBuffer  = {0};
    NV_STATUS     rmStatus;

    NV_ASSERT_OR_RETURN(pDumpParams->size <= NV0000_CTRL_NVD_MAX_DUMP_SIZE, NV_ERR_INVALID_ARGUMENT);

    nvDumpBuffer.size    = pDumpParams->size;
    nvDumpBuffer.address = pDumpParams->pBuffer;

    // Dump the component
    rmStatus = rcdbDumpComponent(pRcDB, pDumpParams->component,
                                 &nvDumpBuffer,
                                 NVDUMP_BUFFER_PROVIDED,
                                 NULL);

    pDumpParams->size = nvDumpBuffer.curNumBytes;

    return rmStatus;
}

/*!
 * @brief Get Timestamp. Returns a standard timestamp, osGetCurrentTime.
 *
 * @returns NV_OK
 */
NV_STATUS
cliresCtrlCmdNvdGetTimestamp_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS *pTimestampParams
)
{
    NV_STATUS status = NV_OK;

    switch (pTimestampParams->cpuClkId)
    {
        case NV0000_NVD_CPU_TIME_CLK_ID_DEFAULT:
        case NV0000_NVD_CPU_TIME_CLK_ID_OSTIME:
        {
            NvU32 sec;
            NvU32 uSec;

            osGetCurrentTime(&sec, &uSec);
            pTimestampParams->timestamp = (((NvU64)sec) * 1000000) + uSec;
            break;
        }

        case NV0000_NVD_CPU_TIME_CLK_ID_PLATFORM_API:
        {
            osGetPerformanceCounter(&pTimestampParams->timestamp);
            break;
        }

        case NV0000_NVD_CPU_TIME_CLK_ID_TSC:
        {
#if PORT_IS_FUNC_SUPPORTED(portUtilExReadTimestampCounter)
            pTimestampParams->timestamp = portUtilExReadTimestampCounter();
#else
            status = NV_ERR_NOT_SUPPORTED;
#endif
            break;
        }

        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
        }
    }

    return status;
}

/**
 * These control handlers can be called:
 * - with pRmCliRes (i.e. this) == NULL
 * - with RS TLS state missing.
 */

/*!
 * @brief Get Nvlog Info. Returns the current state of the NVLOG subsystem.
 *
 * @returns NV_OK on success
 */
NV_STATUS
cliresCtrlCmdNvdGetNvlogInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NvU32 i;

    NV_ASSERT_OR_RETURN(pParams->component >= NV0000_CTRL_NVD_DUMP_COMPONENT_NVLOG,    NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pParams->component <  NV0000_CTRL_NVD_DUMP_COMPONENT_RESERVED, NV_ERR_INVALID_ARGUMENT);

    if ((pParams->component == NVDUMP_COMPONENT_NVLOG_RM) || (pParams->component == NVDUMP_COMPONENT_NVLOG_ALL))
    {
        //
        // Copy a snapshot of the GSP log buffers into the NvLog buffers for all
        // GPUs.  This code assumes that GetNvlogInfo is called just before
        // GetNvlogBufferInfo and GetNvlog.
        //
        //
        // If pRmCliRes==NULL, we are not called from the ResourceServer path, and
        // therefore cannot safely dereference OBJGPU in the below block.
        //
        if (pRmCliRes != NULL)
        {
            NvU32           gpuMask = 0;
            NvU32           gpuInstance = 0;
            OBJGPU         *pGpu;

            (void)gpumgrGetGpuAttachInfo(NULL, &gpuMask);

            for (;;)
            {
                pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);

                if (pGpu == NULL)
                    break;

                if (IS_GSP_CLIENT(pGpu))
                {
                    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
                    kgspDumpGspLogs(pKernelGsp, NV_TRUE);
                }
            }
        }
        else
        {
            // Flush any nvlog buffers if needed
            nvlogRunFlushCbs();
        }

        pParams->version    = NvLogLogger.version;

        portMemSet(pParams->bufferTags, 0, sizeof(pParams->bufferTags));

        portSyncMutexAcquire(NvLogLogger.buffersLock);
        for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
        {
            if (NvLogLogger.pBuffers[i] != NULL)
                pParams->bufferTags[i] = NvLogLogger.pBuffers[i]->tag;
        }
        portSyncMutexRelease(NvLogLogger.buffersLock);
        status = NV_OK;
    }

    return status;
}

/*!
 * @brief Get Nvlog Buf Info. Returns  current state of a specific buffer in
 * the nvlog buffer system.
 *
 * @returns NV_OK on success
 */
NV_STATUS
cliresCtrlCmdNvdGetNvlogBufferInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OR_RETURN(pParams->component >= NV0000_CTRL_NVD_DUMP_COMPONENT_NVLOG,   NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pParams->component < NV0000_CTRL_NVD_DUMP_COMPONENT_RESERVED, NV_ERR_INVALID_ARGUMENT);

    if ((pParams->component == NVDUMP_COMPONENT_NVLOG_RM) || (pParams->component == NVDUMP_COMPONENT_NVLOG_ALL))
    {
        NVLOG_BUFFER *pBuffer;
        NVLOG_BUFFER_HANDLE hBuffer;

        portSyncMutexAcquire(NvLogLogger.buffersLock);

        if (pParams->tag != 0)
        {
            status = nvlogGetBufferHandleFromTag(pParams->tag, &hBuffer);
            if (status != NV_OK)
                goto done;
        }
        else
        {
            if (pParams->buffer >= NVLOG_MAX_BUFFERS)
            {
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }
            hBuffer = pParams->buffer;
        }

        pBuffer = NvLogLogger.pBuffers[hBuffer];
        NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_OBJECT_NOT_FOUND);

        NvBool bPause = pParams->flags & DRF_DEF(0000, _CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS, _PAUSE, _YES);
        nvlogPauseLoggingToBuffer(hBuffer, bPause);

        pParams->tag        = pBuffer->tag;
        pParams->size       = pBuffer->size;
        pParams->flags      = pBuffer->flags;
        pParams->pos        = pBuffer->pos;
        pParams->overflow   = pBuffer->extra.ring.overflow;
        status = NV_OK;

done:
        portSyncMutexRelease(NvLogLogger.buffersLock);
    }

    return status;
}

/*!
 * @brief Get Nvlog. Returns a dump that includes the buffer specified
 * by the caller.
 *
 * @returns NV_OK on success
 */
NV_STATUS
cliresCtrlCmdNvdGetNvlog_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_NVD_GET_NVLOG_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OR_RETURN(pParams->component >= NV0000_CTRL_NVD_DUMP_COMPONENT_NVLOG,    NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pParams->component <  NV0000_CTRL_NVD_DUMP_COMPONENT_RESERVED, NV_ERR_INVALID_ARGUMENT);

    if ((pParams->component == NVDUMP_COMPONENT_NVLOG_RM) || (pParams->component == NVDUMP_COMPONENT_NVLOG_ALL))
    {
        NVLOG_BUFFER_HANDLE hBuffer = pParams->buffer;
        NvU32 oldSize = pParams->size;

        NV_ASSERT_OR_RETURN(pParams->size <= NV0000_CTRL_NVLOG_MAX_BLOCK_SIZE, NV_ERR_INVALID_ARGUMENT);

        portSyncMutexAcquire(NvLogLogger.buffersLock);
        nvlogPauseLoggingToBuffer(hBuffer, NV_TRUE);
        status = nvlogExtractBufferChunk(hBuffer, pParams->blockNum, &pParams->size, pParams->data);

        //
        // If size returned is different than asked, entire buffer has been
        // extracted, and logging can be resumed.
        //
        if (oldSize != pParams->size)
        {
            nvlogPauseLoggingToBuffer(hBuffer, NV_FALSE);
        }
        portSyncMutexRelease(NvLogLogger.buffersLock);
    }

    return status;
}

static NV_STATUS
CliGetSystemP2pCaps_GSPCLIENT
(
    NvU32 *gpuIds,
    NvU32 gpuCount,
    NvU32 *p2pCaps,
    NvU32 *p2pOptimalReadCEs,
    NvU32 *p2pOptimalWriteCEs,
    NvU8  *p2pCapsStatus,
    NvU32 *busPeerIds,
    NvU32 *pBusEgmPeerIds
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;
    NV2080_CTRL_GET_P2P_CAPS_PARAMS *pGetParams = NULL;

    if ((gpuIds == NULL) ||
        (gpuCount == 0) ||
        (gpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS) ||
        (p2pOptimalReadCEs == NULL) ||
        (p2pOptimalWriteCEs == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGetParams = portMemAllocNonPaged(sizeof *pGetParams);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pGetParams != NULL, NV_ERR_NO_MEMORY);

    // Initialize caps to empty
    *p2pOptimalReadCEs = 0;
    *p2pOptimalWriteCEs = 0;
    if (p2pCaps != NULL)
    {
        *p2pCaps = 0;
    }

    if (p2pCapsStatus != NULL)
    {
        for (i = 0; i < NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE; i++)
        {
            p2pCapsStatus[i] = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        }
    }

    if (busPeerIds != NULL)
    {
        for (i = 0; i < (gpuCount * gpuCount); i++)
        {
            busPeerIds[i] = NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER;
        }
    }

    if (pBusEgmPeerIds != NULL)
    {
        for (i = 0; i < (gpuCount * gpuCount); i++)
        {
            pBusEgmPeerIds[i] = NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER;
        }
    }

    // Initialize internal call params
    pGetParams->bAllCaps = NV_FALSE;
    pGetParams->bUseUuid = NV_TRUE;
    pGetParams->peerGpuCount = gpuCount;
    for (i = 0; i < gpuCount; i++)
    {
        OBJGPU *pGpu = gpumgrGetGpuFromId(gpuIds[i]);

        NV_CHECK_OR_ELSE(LEVEL_INFO, pGpu != NULL,
                         status = NV_ERR_INVALID_ARGUMENT; goto done);

        portMemCopy(pGetParams->peerGpuCaps[i].gpuUuid,
                    VM_UUID_SIZE,
                    pGpu->gpuUuid.uuid,
                    NV_GPU_UUID_LEN);
    }

    // Retrieve caps and peer IDs
    for (i = 0; i < gpuCount; i++)
    {
        NvU32 j;
        NvU32 gpuMaskRelease = 0;
        NvBool bLockAcquired = NV_FALSE;
        OBJGPU *pGpu = gpumgrGetGpuFromId(gpuIds[i]);

        NV_CHECK_OR_ELSE(LEVEL_INFO, pGpu != NULL,
                         status = NV_ERR_INVALID_ARGUMENT; goto done);

        if (!rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskRelease))
        {
            // Acquire lock
            NV_ASSERT_OK_OR_GOTO(status,
                                 rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                       GPU_LOCK_GRP_SUBDEVICE,
                                                       GPUS_LOCK_FLAGS_NONE,
                                                       RM_LOCK_MODULES_RPC,
                                                       &gpuMaskRelease),
                                 done);

            bLockAcquired = NV_TRUE;
        }

        NV_RM_RPC_CONTROL(pGpu,
                          pGpu->hInternalClient,
                          pGpu->hInternalSubdevice,
                          NV2080_CTRL_CMD_GET_P2P_CAPS,
                          pGetParams,
                          sizeof *pGetParams,
                          status);

        // Release lock
        if (bLockAcquired && gpuMaskRelease != 0)
        {
            rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
        }

        if (status != NV_OK)
            goto done;

        //
        // Populate caps only once and according to local gpu's view.
        // If gpuCount == 1, retrieve gpu caps of local gpu (allows read of loopback cap).
        // When gpuCount > 1, retrieve gpu caps of one other remote:
        //     Don't use caps of local gpu as it may contain loopback cap.
        //     All local-to-remote pairs have identical caps except CEs data.
        //
        if (i == 0)
        {
            NvU32 ref = gpuCount > 1 ? 1 : 0;
            NV2080_CTRL_GPU_P2P_PEER_CAPS_PEER_INFO localGpuCaps = pGetParams->peerGpuCaps[ref];

            if (p2pCaps != NULL)
            {
                *p2pCaps = localGpuCaps.p2pCaps;
            }

            // Retrieve CEs only for a pair of GPUs (local to itself or to a remote)
            if (gpuCount <= 2)
            {
                *p2pOptimalReadCEs = localGpuCaps.p2pOptimalReadCEs;
                *p2pOptimalWriteCEs = localGpuCaps.p2pOptimalWriteCEs;
            }

            if (p2pCapsStatus != NULL)
            {
                portMemCopy(p2pCapsStatus,
                            NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE * sizeof(p2pCapsStatus[0]),
                            &localGpuCaps.p2pCapsStatus,
                            NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE * sizeof(localGpuCaps.p2pCapsStatus[0]));
            }
        }

        // Fill out gpu peerId matrix
        if (busPeerIds != NULL)
        {
            for (j = 0; j < gpuCount; ++j)
                busPeerIds[i * gpuCount + j] = pGetParams->peerGpuCaps[j].busPeerId;
        }

        // Fill out gpu EGM peerId matrix
        if (pBusEgmPeerIds != NULL)
        {
            for (j = 0; j < gpuCount; ++j)
                pBusEgmPeerIds[i * gpuCount + j] = pGetParams->peerGpuCaps[j].busEgmPeerId;
        }
    }

done:
    portMemFree(pGetParams);

    return status;
}

static NV_STATUS
CliGetSystemP2pCapsMatrix_GSPCLIENT
(
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pP2PParams
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;
    NvBool bReflexive = NV_FALSE;
    NV2080_CTRL_GET_P2P_CAPS_PARAMS *pGetParams = NULL;

    if ((pP2PParams == NULL) ||
        (pP2PParams->grpACount == 0) ||
        (pP2PParams->grpACount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS) ||
        (pP2PParams->grpBCount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGetParams = portMemAllocNonPaged(sizeof *pGetParams);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pGetParams != NULL, NV_ERR_NO_MEMORY);

    if (pP2PParams->grpBCount == 0)
    {
        bReflexive = NV_TRUE;
    }

    // Initialize output to empty
    portMemSet(&pP2PParams->p2pCaps, 0, sizeof(pP2PParams->p2pCaps));
    portMemSet(&pP2PParams->a2bOptimalReadCes, 0, sizeof(pP2PParams->a2bOptimalReadCes));
    portMemSet(&pP2PParams->a2bOptimalWriteCes, 0, sizeof(pP2PParams->a2bOptimalWriteCes));
    portMemSet(&pP2PParams->b2aOptimalReadCes, 0, sizeof(pP2PParams->b2aOptimalReadCes));
    portMemSet(&pP2PParams->b2aOptimalWriteCes, 0, sizeof(pP2PParams->b2aOptimalWriteCes));

    // Initialize internal call params for group A
    pGetParams->bAllCaps = NV_FALSE;
    pGetParams->bUseUuid = NV_TRUE;
    for (i = 0; bReflexive ? i < (pP2PParams->grpACount) : (i < pP2PParams->grpBCount); i++)
    {
        if (bReflexive)
        {
            OBJGPU *pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIdGrpA[i]);

            NV_CHECK_OR_ELSE(LEVEL_INFO, pGpu != NULL,
                             status = NV_ERR_INVALID_ARGUMENT; goto done);

            portMemCopy(pGetParams->peerGpuCaps[i].gpuUuid,
                        VM_UUID_SIZE,
                        pGpu->gpuUuid.uuid,
                        NV_GPU_UUID_LEN);
        }
        else
        {
            OBJGPU *pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIdGrpB[i]);

            NV_CHECK_OR_ELSE(LEVEL_INFO, pGpu != NULL,
                             status = NV_ERR_INVALID_ARGUMENT; goto done);

            portMemCopy(pGetParams->peerGpuCaps[i].gpuUuid,
                        VM_UUID_SIZE,
                        pGpu->gpuUuid.uuid,
                        NV_GPU_UUID_LEN);
        }
    }

    // Cycle through group A to set caps and A2B CEs
    for (i = 0; i < pP2PParams->grpACount; i++)
    {
        NvU32 b;
        NvU32 gpuMaskRelease = 0;
        NvBool bLockAcquired = NV_FALSE;
        OBJGPU *pGpu;

        if (bReflexive)
        {
            // Set gpuCount equal to (i + 1) to get # GPUs up to and including matrix diag
            pGetParams->peerGpuCount = i + 1;
        }
        else
        {
            pGetParams->peerGpuCount = pP2PParams->grpBCount;
        }

        pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIdGrpA[i]);

        NV_CHECK_OR_ELSE(LEVEL_INFO, pGpu != NULL,
                         status = NV_ERR_INVALID_ARGUMENT; goto done);

        if (!rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskRelease))
        {
            // Acquire lock
            NV_ASSERT_OK_OR_GOTO(status,
                                 rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                       GPU_LOCK_GRP_SUBDEVICE,
                                                       GPUS_LOCK_FLAGS_NONE,
                                                       RM_LOCK_MODULES_RPC,
                                                       &gpuMaskRelease),
                                 done);

            bLockAcquired = NV_TRUE;
        }

        NV_RM_RPC_CONTROL(pGpu,
                          pGpu->hInternalClient,
                          pGpu->hInternalSubdevice,
                          NV2080_CTRL_CMD_GET_P2P_CAPS,
                          pGetParams,
                          sizeof *pGetParams,
                          status);

        // Release lock
        if (bLockAcquired && gpuMaskRelease != 0)
        {
            rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
        }

        if (status != NV_OK)
            goto done;

        // Set Caps and A2B CEs for this row of the matrix
        for (b = 0; b < pGetParams->peerGpuCount; ++b)
        {
            pP2PParams->p2pCaps[i][b] = pGetParams->peerGpuCaps[b].p2pCaps;
            pP2PParams->a2bOptimalReadCes[i][b] = pGetParams->peerGpuCaps[b].p2pOptimalReadCEs;
            pP2PParams->a2bOptimalWriteCes[i][b] = pGetParams->peerGpuCaps[b].p2pOptimalWriteCEs;

            // If reflexive (including identity) mirror A2B results to B2A
            if (bReflexive)
            {
                pP2PParams->p2pCaps[b][i] = pP2PParams->p2pCaps[i][b];
                pP2PParams->b2aOptimalReadCes[b][i] = pP2PParams->a2bOptimalReadCes[i][b];
                pP2PParams->b2aOptimalWriteCes[b][i] = pP2PParams->a2bOptimalWriteCes[i][b];
            }
        }
    }

    //
    // Initialize internal call params for group B.
    // Regardless of bReflexive, logically always should use grpACount and gpuIdGrpA here.
    //
    portMemSet(pGetParams, 0, sizeof *pGetParams);
    pGetParams->bAllCaps = NV_FALSE;
    pGetParams->bUseUuid = NV_TRUE;
    for (i = 0; i < pP2PParams->grpACount; i++)
    {
        OBJGPU *pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIdGrpA[i]);

        NV_CHECK_OR_ELSE(LEVEL_INFO, pGpu != NULL,
                         status = NV_ERR_INVALID_ARGUMENT; goto done);

        portMemCopy(pGetParams->peerGpuCaps[i].gpuUuid,
                    VM_UUID_SIZE,
                    pGpu->gpuUuid.uuid,
                    NV_GPU_UUID_LEN);
    }

    // Cycle through group B and set B2A CEs
    for (i = 0; bReflexive ? (i < pP2PParams->grpACount) : (i < pP2PParams->grpBCount); i++)
    {
        NvU32 a;
        NvU32 gpuMaskRelease = 0;
        NvBool bLockAcquired = NV_FALSE;
        OBJGPU *pGpu;

        if (bReflexive)
        {
            // set count to number of elems up to and including diagonal.
            pGetParams->peerGpuCount = i + 1;
            pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIdGrpA[i]);
        }
        else
        {
            pGetParams->peerGpuCount = pP2PParams->grpACount;
            pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIdGrpB[i]);
        }

        NV_CHECK_OR_ELSE(LEVEL_INFO, pGpu != NULL,
                         status = NV_ERR_INVALID_ARGUMENT; goto done);

        if (!rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskRelease))
        {
            // Acquire lock
            NV_ASSERT_OK_OR_GOTO(status,
                                 rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                       GPU_LOCK_GRP_SUBDEVICE,
                                                       GPUS_LOCK_FLAGS_NONE,
                                                       RM_LOCK_MODULES_RPC,
                                                       &gpuMaskRelease),
                                  done);

            bLockAcquired = NV_TRUE;
        }

        // Retrieve B2A optimal CEs
        NV_RM_RPC_CONTROL(pGpu,
                          pGpu->hInternalClient,
                          pGpu->hInternalSubdevice,
                          NV2080_CTRL_CMD_GET_P2P_CAPS,
                          pGetParams,
                          sizeof *pGetParams,
                          status);

        // Release lock
        if (bLockAcquired && gpuMaskRelease != 0)
        {
            rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
        }

        if (status != NV_OK)
            goto done;

        // Set B2A CEs
        for (a = 0; a < pGetParams->peerGpuCount; ++a)
        {
            if (bReflexive)
            {
                if (a == i)
                {
                    // Identity work all done in A loop.
                    continue;
                }

                // Set B->A CEs (which are a2bOptimal*Ces for GPU i in group B)
                pP2PParams->a2bOptimalReadCes[a][i] = pGetParams->peerGpuCaps[a].p2pOptimalReadCEs;
                pP2PParams->a2bOptimalWriteCes[a][i] = pGetParams->peerGpuCaps[a].p2pOptimalWriteCEs;

                // Set A->B CEs (which are b2aOptimal*Ces for GPU i in group A)
                pP2PParams->b2aOptimalReadCes[i][a] = pP2PParams->a2bOptimalReadCes[a][i];
                pP2PParams->b2aOptimalWriteCes[i][a] = pP2PParams->a2bOptimalWriteCes[a][i];
            }
            else
            {
                pP2PParams->b2aOptimalReadCes[a][i] = pGetParams->peerGpuCaps[a].p2pOptimalReadCEs;
                pP2PParams->b2aOptimalWriteCes[a][i] = pGetParams->peerGpuCaps[a].p2pOptimalWriteCEs;
            }
        }
    }

done:
    portMemFree(pGetParams);

    return status;
}

NV_STATUS
cliresCtrlCmdSystemGetP2pCaps_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *pP2PParams
)
{
    OBJGPU *pGpu;

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        // Unsupported in GSP-RM; use NV2080_CTRL_CMD_GET_P2P_CAPS directly.
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((pP2PParams->gpuCount ==  0) || (pP2PParams->gpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIds[0]);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get blacklist information from host RM
    //
    if (IS_VIRTUAL(pGpu))
    {
        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        {
            return CliGetSystemP2pCaps_GSPCLIENT(pP2PParams->gpuIds,
                                                 pP2PParams->gpuCount,
                                                 &pP2PParams->p2pCaps,
                                                 &pP2PParams->p2pOptimalReadCEs,
                                                 &pP2PParams->p2pOptimalWriteCEs,
                                                 pP2PParams->p2pCapsStatus,
                                                 pP2PParams->busPeerIds,
                                                 pP2PParams->busEgmPeerIds);
        }
    }

    return CliGetSystemP2pCaps(pP2PParams->gpuIds,
                               pP2PParams->gpuCount,
                               &pP2PParams->p2pCaps,
                               &pP2PParams->p2pOptimalReadCEs,
                               &pP2PParams->p2pOptimalWriteCEs,
                               NvP64_VALUE(pP2PParams->p2pCapsStatus),
                               NvP64_VALUE(pP2PParams->busPeerIds),
                               NvP64_VALUE(pP2PParams->busEgmPeerIds));
}

NV_STATUS
cliresCtrlCmdSystemGetP2pCapsV2_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *pP2PParams
)
{
    OBJGPU *pGpu;
    NvBool bLoopback = pP2PParams->gpuCount == 2 && pP2PParams->gpuIds[0] == pP2PParams->gpuIds[1];

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        // Unsupported in GSP-RM; use NV2080_CTRL_CMD_GET_P2P_CAPS directly.
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((pP2PParams->gpuCount ==  0) || (pP2PParams->gpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIds[0]);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get blacklist information from host RM
    //
    if (IS_VIRTUAL(pGpu))
    {
        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        {
            //
            // NOTE: If in the future we'd like to enable this path for baremetal,
            // the guest GPU ID to UUID conversion needs to be extracted to here.
            //
            return CliGetSystemP2pCaps_GSPCLIENT(pP2PParams->gpuIds,
                                                 pP2PParams->gpuCount,
                                                 &pP2PParams->p2pCaps,
                                                 &pP2PParams->p2pOptimalReadCEs,
                                                 &pP2PParams->p2pOptimalWriteCEs,
                                                 pP2PParams->p2pCapsStatus,
                                                 pP2PParams->busPeerIds,
                                                 pP2PParams->busEgmPeerIds);
        }
    }

    return CliGetSystemP2pCaps(pP2PParams->gpuIds,
                               bLoopback ? 1 : pP2PParams->gpuCount,
                              &pP2PParams->p2pCaps,
                              &pP2PParams->p2pOptimalReadCEs,
                              &pP2PParams->p2pOptimalWriteCEs,
                               NvP64_VALUE(pP2PParams->p2pCapsStatus),
                               NvP64_VALUE(pP2PParams->busPeerIds),
                               NvP64_VALUE(pP2PParams->busEgmPeerIds));
}

NV_STATUS
cliresCtrlCmdSystemGetP2pCapsMatrix_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pP2PParams
)
{
    NvU32 grpAIdx, grpBIdx;
    NV_STATUS status = NV_OK;
    NvBool bReflexive = NV_FALSE;
    NvU32 *groupA = NULL;
    NvU32 *groupB = NULL;
    OBJGPU *pGpu;

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        // Unsupported in GSP-RM; use NV2080_CTRL_CMD_GET_P2P_CAPS directly.
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pP2PParams->grpACount == 0 ||
        pP2PParams->grpACount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS ||
        pP2PParams->grpBCount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIdGrpA[0]);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get blacklist information from host RM
    //
    if (IS_VIRTUAL(pGpu))
    {
        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        {
            return CliGetSystemP2pCapsMatrix_GSPCLIENT(pP2PParams);
        }
    }

    groupA = pP2PParams->gpuIdGrpA;
    groupB = pP2PParams->gpuIdGrpB;

    // Check for the reflexive case
    if (pP2PParams->grpBCount == 0)
    {
        bReflexive = NV_TRUE;
        groupB = groupA;
    }

    for (grpAIdx = 0; grpAIdx < pP2PParams->grpACount; grpAIdx++)
    {
        //
        // When group A == group B, skip the last half of the queries. These are
        // the same as the backward-direction queries done inside the inner loop
        // during the first half in this case.
        //
        for (grpBIdx = 0;
             bReflexive ? grpBIdx <= grpAIdx : grpBIdx < pP2PParams->grpBCount;
             grpBIdx++)
        {
            //
            // Get *both* directions of P2P capabilities. This is necessary to
            // reduce the number of crossings into the RM. The caller invokes
            // the control once for two sets of GPUs and gets both directions of
            // P2P caps for all pairs between the sets.
            //

            // Get the A-to-B directional caps
            status = CliGetSystemP2pCaps((NvU32[]){groupA[grpAIdx],
                                                   groupB[grpBIdx]},
                                         (groupA[grpAIdx] == groupB[grpBIdx]) ? 1 : 2,
                                         &pP2PParams->p2pCaps[grpAIdx][grpBIdx],
                                         &pP2PParams->a2bOptimalReadCes[grpAIdx][grpBIdx],
                                         &pP2PParams->a2bOptimalWriteCes[grpAIdx][grpBIdx],
                                         NULL,
                                         NULL,
                                         NULL);
            if (status != NV_OK)
            {
                return status;
            }

            //
            // Note that when we're in the reflexive case *and* we're looking at
            // a single GPU against itself, we can skip the backward direction
            // query as well because the above query gave us total information.
            //
            if (bReflexive && grpAIdx == grpBIdx)
            {
                continue;
            }

            // Get the B-to-A (asymmetric) CEs, skipping (symmetric) p2pCaps
            status = CliGetSystemP2pCaps((NvU32[]){groupB[grpBIdx],
                                                   groupA[grpAIdx]},
                                         (groupA[grpAIdx] == groupB[grpBIdx]) ? 1 : 2,
                                         NULL, // Skip p2pCaps
                                         &pP2PParams->b2aOptimalReadCes[grpAIdx][grpBIdx],
                                         &pP2PParams->b2aOptimalWriteCes[grpAIdx][grpBIdx],
                                         NULL,
                                         NULL,
                                         NULL);
            if (status != NV_OK)
            {
                return status;
            }

            //
            // If a set is being compared against itself, we can copy any result
            // from the a2b query as the b2a result in the opposite direction.
            // This is not true when two different sets are being compared.
            //
            if (bReflexive)
            {
                pP2PParams->p2pCaps[grpBIdx][grpAIdx] = pP2PParams->p2pCaps[grpAIdx][grpBIdx];
                pP2PParams->a2bOptimalReadCes[grpBIdx][grpAIdx] = pP2PParams->b2aOptimalReadCes[grpAIdx][grpBIdx];
                pP2PParams->a2bOptimalWriteCes[grpBIdx][grpAIdx] = pP2PParams->b2aOptimalWriteCes[grpAIdx][grpBIdx];
                pP2PParams->b2aOptimalReadCes[grpBIdx][grpAIdx] = pP2PParams->a2bOptimalReadCes[grpAIdx][grpBIdx];
                pP2PParams->b2aOptimalWriteCes[grpBIdx][grpAIdx] = pP2PParams->a2bOptimalWriteCes[grpAIdx][grpBIdx];
            }
        }
    }

    return status;
}

NV_STATUS
cliresCtrlCmdSystemGetVgxSystemInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS *pParams
)
{
    NV_STATUS         status = NV_OK;
    OBJGPU           *pGpu   = NULL;
    VGPU_STATIC_INFO *pVSI;

    // Get the pGpu object
    pGpu = gpumgrGetSomeGpu();
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    pVSI = GPU_GET_STATIC_INFO(pGpu);
    if (pVSI)
    {
        portMemCopy(pParams, sizeof(pVSI->vgxSystemInfo), &pVSI->vgxSystemInfo, sizeof(pVSI->vgxSystemInfo));
    }
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

    return status;
}

/*!
 * @brief get the GPUs Power status.
 *
 * @returns NV_OK, or some error.
 * @note Possible errors include no initialized GPU (invalid request)
 */
NV_STATUS
cliresCtrlCmdSystemGetGpusPowerStatus_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS *pGpusPowerStatus
)
{
    NV_STATUS status = NV_OK;
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPU    *pGpu = NULL;
    NvU32      gpuAttachCnt = 0;
    NvU32      gpuAttachMask = 0;
    NvU32      i = 0;
    NvU32      gpuIndex = 0;

    portMemSet(pGpusPowerStatus, 0, sizeof(*pGpusPowerStatus));

    // Loop though the GPUs with power disconnected
    for (gpuIndex = 0; gpuIndex < pGpuMgr->powerDisconnectedGpuCount; gpuIndex++)
    {
        pGpusPowerStatus->gpuBus[gpuIndex] = pGpuMgr->powerDisconnectedGpuBus[gpuIndex];
        pGpusPowerStatus->gpuExternalPowerStatus[gpuIndex] = NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_NOT_CONNECTED;
    }

    pGpusPowerStatus->gpuCount = pGpuMgr->powerDisconnectedGpuCount;

    // Loop though the attached GPU and not already in the list.
    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    while ((pGpu = gpumgrGetNextGpu(gpuAttachMask, &i)))
    {
        for (gpuIndex = 0; gpuIndex < pGpusPowerStatus->gpuCount; gpuIndex++)
        {
            if (gpuGetBus(pGpu) == pGpusPowerStatus->gpuBus[gpuIndex])
            {
                break;
            }
        }
        if ((gpuIndex == pGpusPowerStatus->gpuCount) && (pGpusPowerStatus->gpuCount < NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
        {
             pGpusPowerStatus->gpuBus[gpuIndex] = gpuGetBus(pGpu);
             pGpusPowerStatus->gpuExternalPowerStatus[gpuIndex] = NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_CONNECTED;
             pGpusPowerStatus->gpuCount++;
        }
    }

    return status;
}

NV_STATUS
cliresCtrlCmdSystemGetPrivilegedStatus_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS *pParams
)
{
    RmClient *pClient = dynamicCast(RES_GET_CLIENT(pRmCliRes), RmClient);
    NvU8     privStatus = 0;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN (pClient != NULL, NV_ERR_INVALID_CLIENT);

    if (pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL)
    {
        privStatus |= NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_KERNEL_HANDLE_FLAG;
    }

    if (pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_USER_ROOT)
    {
        privStatus |= NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PRIV_USER_FLAG;
    }

    if (rmclientIsAdmin(pClient, pCallContext->secInfo.privLevel))
    {
        privStatus |= NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PRIV_HANDLE_FLAG;
    }

    pParams->privStatusFlags = privStatus;

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGetFabricStatus_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU32 fabricStatus = NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_SKIP;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        fabricStatus = NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_UNINITIALIZED;

        if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED))
        {
            fabricStatus = NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_IN_PROGRESS;
        }

        if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED))
        {
            fabricStatus = NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_INITIALIZED;
        }
    }

    pParams->fabricStatus = fabricStatus;

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdGpuGetUuidInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = NULL;

    pGpu = gpumgrGetGpuFromUuid(pParams->gpuUuid, pParams->flags);

    if (NULL == pGpu)
        return NV_ERR_OBJECT_NOT_FOUND;

    pParams->gpuId = pGpu->gpuId;
    pParams->deviceInstance = gpuGetDeviceInstance(pGpu);
    pParams->subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdGpuGetUuidFromGpuId_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS *pParams
)
{
    OBJGPU   *pGpu = NULL;
    NvU8     *pGidString = NULL;
    NvU32     gidStrLen = 0;
    NV_STATUS rmStatus;

    // First check for UUID cached by gpumgr
    rmStatus = gpumgrGetGpuUuidInfo(pParams->gpuId, &pGidString, &gidStrLen, pParams->flags);

    if (rmStatus != NV_OK)
    {
        // If UUID not cached by gpumgr then try to query device
        pGpu = gpumgrGetGpuFromId(pParams->gpuId);

        if (NULL == pGpu)
            return NV_ERR_OBJECT_NOT_FOUND;

        // get the UUID of this GPU
        rmStatus = gpuGetGidInfo(pGpu, &pGidString, &gidStrLen, pParams->flags);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "gpumgrGetGpuInfo: getting gpu GUID failed\n");
            return rmStatus;
        }
    }

    if (gidStrLen <= NV0000_GPU_MAX_GID_LENGTH)
    {
        portMemCopy(pParams->gpuUuid, gidStrLen, pGidString, gidStrLen);
        pParams->uuidStrLen = gidStrLen;
    }

    // cleanup the allocated gidstring
    portMemFree(pGidString);

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdGpuModifyGpuDrainState_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS *pParams
)
{
    NV_STATUS status;
    NvBool    bEnable;
    NvBool    bRemove = NV_FALSE;
    NvBool    bLinkDisable = NV_FALSE;
    OBJGPU   *pGpu = gpumgrGetGpuFromId(pParams->gpuId);

    if (NV0000_CTRL_GPU_DRAIN_STATE_ENABLED == pParams->newState)
    {
        if ((pGpu != NULL) && IsSLIEnabled(pGpu))
        {
            // "drain" state not supported in SLI configurations
            return NV_ERR_NOT_SUPPORTED;
        }

        bEnable = NV_TRUE;
        bRemove =
            ((pParams->flags & NV0000_CTRL_GPU_DRAIN_STATE_FLAG_REMOVE_DEVICE) != 0);
        bLinkDisable =
            ((pParams->flags & NV0000_CTRL_GPU_DRAIN_STATE_FLAG_LINK_DISABLE) != 0);

        if (bLinkDisable && !bRemove)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else if (NV0000_CTRL_GPU_DRAIN_STATE_DISABLED ==
            pParams->newState)
    {
        bEnable = NV_FALSE;
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Set/Clear GPU manager drain state
    status = gpumgrModifyGpuDrainState(pParams->gpuId, bEnable, bRemove, bLinkDisable);

    //
    // For now, assume that when drain state is set up by a client, it is in
    // anticipation of some form of external GPU reset.
    //
    if ((pGpu != NULL) && (status == NV_OK))
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

        if (pKernelNvlink != NULL)
        {
            pKernelNvlink->setProperty(pKernelNvlink,
                                       PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD,
                                       bEnable);

            status = knvlinkSyncLaneShutdownProps(pGpu, pKernelNvlink);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING, "Failed to sync lane shutdown properties\n");
                return status;
            }
        }
    }

    return status;
}

NV_STATUS
cliresCtrlCmdGpuQueryGpuDrainState_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS *pParams
)
{
    NvBool    bDrainState;
    NvBool    bRemove;
    NV_STATUS status;

    status = gpumgrQueryGpuDrainState(pParams->gpuId, &bDrainState, &bRemove);

    if (status != NV_OK)
    {
        return status;
    }

    pParams->drainState = bDrainState ? NV0000_CTRL_GPU_DRAIN_STATE_ENABLED
                                      : NV0000_CTRL_GPU_DRAIN_STATE_DISABLED;

    pParams->flags = bRemove ? NV0000_CTRL_GPU_DRAIN_STATE_FLAG_REMOVE_DEVICE : 0;

    return NV_OK;
}

/*
 * Associate sub process ID with client handle
 *
 * @return 'NV_OK' on success. Otherwise return NV_ERR_INVALID_CLIENT
 */
NV_STATUS
cliresCtrlCmdSetSubProcessID_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS *pParams
)
{
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    RmClient *pClient = serverutilGetClientUnderLock(hClient);

    if (pClient == NULL)
        return NV_ERR_INVALID_CLIENT;

    pClient->SubProcessID = pParams->subProcessID;
    portStringCopy(pClient->SubProcessName, sizeof(pClient->SubProcessName), pParams->subProcessName, sizeof(pParams->subProcessName));

    return NV_OK;
}

/*
 * Disable USERD isolation among all the sub processes within a user process
 *
 * @return 'NV_OK' on success. Otherwise return NV_ERR_INVALID_CLIENT
 */
NV_STATUS
cliresCtrlCmdDisableSubProcessUserdIsolation_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS *pParams
)
{
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    RmClient *pClient = serverutilGetClientUnderLock(hClient);

    if (pClient == NULL)
        return NV_ERR_INVALID_CLIENT;

    pClient->bIsSubProcessDisabled = pParams->bIsSubProcessDisabled;

    return NV_OK;
}

/*!
 * @brief Queries whether Sync Gpu Boost Manager is enabled or not.
 */
NV_STATUS
cliresCtrlCmdSyncGpuBoostInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_SYNC_GPU_BOOST_INFO_PARAMS *pParams
)
{
    OBJSYS         *pSys      = SYS_GET_INSTANCE();
    OBJGPUBOOSTMGR *pBoostMgr = SYS_GET_GPUBOOSTMGR(pSys);

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_ARGUMENT);
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if (NULL == pBoostMgr)
    {
        pParams->bEnabled = NV_FALSE;
        return NV_OK;
    }

    pParams->bEnabled = NV_TRUE;
    return NV_OK;
}

/*!
 * @brief Creates a Sync GPU Boost Group containing the GPUs
 * specified
 *
 * @returns NV_OK                    Success
 * @returns other value              Failure
 */
NV_STATUS
cliresCtrlCmdSyncGpuBoostGroupCreate_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS *pParams
)
{
    NV_STATUS       status    = NV_OK;
    OBJSYS         *pSys      = SYS_GET_INSTANCE();
    OBJGPUBOOSTMGR *pBoostMgr = SYS_GET_GPUBOOSTMGR(pSys);

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_ARGUMENT);
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // Start off with invalid boost group ID
    pParams->boostConfig.boostGroupId = NV0000_SYNC_GPU_BOOST_INVALID_GROUP_ID;

    // Create the boost group
    status = gpuboostmgrCreateGroup(pBoostMgr, &pParams->boostConfig);
    NV_ASSERT(NV_OK == status);
    return status;
}

/*!
 * @brief  Destroys a previously created GPU Boost Group.
 *
 * @returns   NV_OK                  Success
 */
NV_STATUS
cliresCtrlCmdSyncGpuBoostGroupDestroy_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS *pParams
)
{
    NV_STATUS       status    = NV_OK;
    OBJSYS         *pSys      = SYS_GET_INSTANCE();
    OBJGPUBOOSTMGR *pBoostMgr = SYS_GET_GPUBOOSTMGR(pSys);

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_ARGUMENT);
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // Destroy the boost group
    status = gpuboostmgrDestroyGroup(pBoostMgr, pParams->boostGroupId);
    NV_ASSERT(NV_OK == status);
    return status;
}

/*!
 * @brief Queries information for a specified Sync Gpu Boost Group.
 */
NV_STATUS
cliresCtrlCmdSyncGpuBoostGroupInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS *pParams
)
{

    NV_STATUS       status    = NV_OK;
    OBJSYS         *pSys      = SYS_GET_INSTANCE();
    OBJGPUBOOSTMGR *pBoostMgr = SYS_GET_GPUBOOSTMGR(pSys);

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_ARGUMENT);
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    status = gpuboostmgrQueryGroups(pBoostMgr, pParams);
    NV_ASSERT(NV_OK == status);
    return status;
}

NV_STATUS
cliresCtrlCmdVgpuVfioNotifyRMStatus_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_VGPU_VFIO_NOTIFY_RM_STATUS_PARAMS *pVgpuStatusParams
)
{

    if (osIsVgpuVfioPresent() != NV_OK)
        return NV_ERR_NOT_SUPPORTED;

    osWakeRemoveVgpu(pVgpuStatusParams->gpuId, pVgpuStatusParams->returnStatus);

    return NV_OK;
}


NV_STATUS
cliresCtrlCmdVgpuGetVgpuVersion_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS *vgpuVersionInfo
)
{
    vgpuVersionInfo->host_min_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);
    vgpuVersionInfo->host_max_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);

    NV_PRINTF(LEVEL_INFO, "User enforced vGPU version = (0x%x, 0x%x)\n",
              vgpuVersionInfo->user_min_supported_version,
              vgpuVersionInfo->user_max_supported_version);

    return (kvgpumgrGetHostVgpuVersion(&(vgpuVersionInfo->user_min_supported_version),
                                       &(vgpuVersionInfo->user_max_supported_version)));
}

NV_STATUS
cliresCtrlCmdVgpuSetVgpuVersion_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS *vgpuVersionInfo
)
{
    return (kvgpumgrSetHostVgpuVersion(vgpuVersionInfo->min_version,
                                       vgpuVersionInfo->max_version));
}

NV_STATUS
cliresCtrlCmdClientGetAddrSpaceType_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS *pParams
)
{
    NvHandle         hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    CALL_CONTEXT     callContext;
    RsClient        *pRsClient;
    RsResourceRef   *pResourceRef;
    Memory          *pMemory = NULL;
    GpuResource     *pGpuResource = NULL;
    NV_ADDRESS_SPACE memType;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClient, &pRsClient));
    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(pRsClient, pParams->hObject, &pResourceRef));

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pClient = pRsClient;
    callContext.pResourceRef = pResourceRef;

    pMemory = dynamicCast(pResourceRef->pResource, Memory);
    if (pMemory != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(memGetMapAddrSpace(pMemory, &callContext, pParams->mapFlags, &memType));

        // Soon FlaMemory is deprecated. This is just a hack to keep compatibility.
        if ((memType == ADDR_FBMEM) && (dynamicCast(pMemory, FlaMemory) != NULL))
        {
            memType = ADDR_FABRIC_V2;
        }
    }
    else
    {
        pGpuResource = dynamicCast(pResourceRef->pResource, GpuResource);
        if (pGpuResource != NULL)
        {
            NV_ASSERT_OK_OR_RETURN(gpuresGetMapAddrSpace(pGpuResource, &callContext, pParams->mapFlags, &memType));
        }
        else
        {
            return NV_ERR_INVALID_OBJECT;
        }
    }

    switch (memType)
    {
        case ADDR_SYSMEM:
        case ADDR_EGM:
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_SYSMEM;
            break;
        case ADDR_FBMEM:
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM;
            break;
        case ADDR_REGMEM:
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_REGMEM;
            break;
        case ADDR_FABRIC_V2:
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_FABRIC;
            break;
        case ADDR_FABRIC_MC:
#ifdef NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_FABRIC_MC
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_FABRIC_MC;
            break;
#else
            NV_ASSERT(0);
            return NV_ERR_INVALID_ARGUMENT;
#endif
        case ADDR_VIRTUAL:
            NV_PRINTF(LEVEL_ERROR,
                      "VIRTUAL (0x%x) is not a valid NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE\n",
                      memType);
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_INVALID;
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
        default:
            NV_PRINTF(LEVEL_ERROR, "Cannot determine address space 0x%x\n",
                      memType);
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_INVALID;
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdClientGetHandleInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS *pParams
)
{
    NvHandle       hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    NV_STATUS      status;
    RsResourceRef *pRsResourceRef;

    status = serverutilGetResourceRef(hClient, pParams->hObject, &pRsResourceRef);
    if (status != NV_OK)
    {
        return status;
    }

    switch (pParams->index)
    {
        case NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO_INDEX_PARENT:
            pParams->data.hResult = pRsResourceRef->pParentRef ? pRsResourceRef->pParentRef->hResource : 0;
            break;
        case NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO_INDEX_CLASSID:
            pParams->data.iResult = pRsResourceRef->externalClassId;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdClientGetAccessRights_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS *pParams
)
{
    NV_STATUS      status;
    RsResourceRef *pRsResourceRef;
    RsResourceRef *pClientRef = RES_GET_REF(pRmCliRes);
    RsClient      *pClient = pClientRef->pClient;

    status = serverutilGetResourceRef(pParams->hClient, pParams->hObject, &pRsResourceRef);
    if (status != NV_OK)
    {
        return status;
    }

    rsAccessUpdateRights(pRsResourceRef, pClient, NULL);

    rsAccessGetAvailableRights(pRsResourceRef, pClient, &pParams->maskResult);

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdClientSetInheritedSharePolicy_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS *pParams
)
{
    NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS params;

    portMemSet(&params, 0, sizeof(params));
    params.sharePolicy = pParams->sharePolicy;
    params.hObject = RES_GET_REF(pRmCliRes)->hResource;

    return cliresCtrlCmdClientShareObject(pRmCliRes, &params);
}

NV_STATUS
cliresCtrlCmdClientShareObject_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS *pParams
)
{
    RS_SHARE_POLICY *pSharePolicy = &pParams->sharePolicy;
    RsClient        *pClient = RES_GET_CLIENT(pRmCliRes);
    RsResourceRef   *pObjectRef;

    CALL_CONTEXT  callContext;
    CALL_CONTEXT *pOldCallContext;

    NV_STATUS status;

    if (pSharePolicy->type >= RS_SHARE_TYPE_MAX)
        return NV_ERR_INVALID_ARGUMENT;

    status = clientGetResourceRef(pClient, pParams->hObject, &pObjectRef);
    if (status != NV_OK)
        return status;

    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    portMemSet(&callContext, 0, sizeof(callContext));
    callContext.pServer = &g_resServ;
    callContext.pClient = pClient;
    callContext.pResourceRef = pObjectRef;
    callContext.secInfo = pCallContext->secInfo;

    NV_ASSERT_OK_OR_RETURN(resservSwapTlsCallContext(&pOldCallContext, &callContext));

    status = clientShareResource(pClient, pObjectRef, pSharePolicy, &callContext);
    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldCallContext));
    if (status != NV_OK)
        return status;

    //
    // Above clientShareResource does everything needed for normal sharing,
    // but we may still need to add a backref if we're sharing with a client,
    // to prevent stale access.
    //
    if (!(pSharePolicy->action & RS_SHARE_ACTION_FLAG_REVOKE) &&
        (pSharePolicy->type == RS_SHARE_TYPE_CLIENT))
    {
        RsClient *pClientTarget;

        // Trying to share with self, nothing to do.
        if (pSharePolicy->target == pClient->hClient)
            return NV_OK;

        status = serverGetClientUnderLock(&g_resServ, pSharePolicy->target, &pClientTarget);
        if (status != NV_OK)
            return status;

        status = clientAddAccessBackRef(pClientTarget, pObjectRef);
        if (status != NV_OK)
            return status;
    }

    return status;
}

NV_STATUS
cliresCtrlCmdClientGetChildHandle_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS *pParams
)
{
    NvHandle       hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    NV_STATUS      status;
    RsResourceRef *pParentRef;
    RsResourceRef *pResourceRef;

    status = serverutilGetResourceRef(hClient, pParams->hParent, &pParentRef);
    if (status != NV_OK)
    {
        return status;
    }

    status = refFindChildOfType(pParentRef, pParams->classId, NV_TRUE, &pResourceRef);
    if (status == NV_OK)
    {
        pParams->hObject = pResourceRef ? pResourceRef->hResource : 0;
    }
    return status;
}

NV_STATUS
cliresCtrlCmdObjectsAreDuplicates_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS *pParams
)
{
    RsResourceRef *pResRef;
    RsClient *pClient = RES_GET_CLIENT(pRmCliRes);

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        clientGetResourceRef(pClient, pParams->hObject1, &pResRef));

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        resIsDuplicate(pResRef->pResource, pParams->hObject2,
                       &pParams->bDuplicates));

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdClientSubscribeToImexChannel_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL_PARAMS *pParams
)
{
    RsClient *pClient = RES_GET_CLIENT(pRmCliRes);
    RmClient *pRmClient = dynamicCast(pClient, RmClient);
    NvS32 channel;

    if (!osImexChannelIsSupported() || (osImexChannelCount() == 0))
        return NV_ERR_NOT_SUPPORTED;

    channel = osImexChannelGet(pParams->devDescriptor);
    if (channel < 0)
        return NV_ERR_INSUFFICIENT_PERMISSIONS;

    // Same subscription
    if (pRmClient->imexChannel == channel)
        return NV_OK;

    // For now, only one channel subscription is allowed per client.
    if (pRmClient->imexChannel != -1)
        return NV_ERR_STATE_IN_USE;

    pRmClient->imexChannel = channel;
    pParams->channel = channel;

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdGpuGetMemOpEnable_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS *pMemOpEnableParams
)
{
    OBJSYS   *pSys = SYS_GET_INSTANCE();
    NV_STATUS status = NV_OK;

    pMemOpEnableParams->enableMask = 0;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_ENABLE_STREAM_MEMOPS))
    {
        NV_PRINTF(LEVEL_INFO, "MemOpOverride enabled\n");
        pMemOpEnableParams->enableMask = NV0000_CTRL_GPU_FLAGS_MEMOP_ENABLE;
    }

    return status;
}

NV_STATUS
cliresCtrlCmdGpuDisableNvlinkInit_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS *pParams
)
{
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    if (!rmclientIsCapableOrAdminByHandle(hClient,
                                          NV_RM_CAP_EXT_FABRIC_MGMT,
                                          pCallContext->secInfo.privLevel))
    {
        NV_PRINTF(LEVEL_WARNING, "Non-privileged context issued privileged cmd\n");
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (pParams->gpuId == NV0000_CTRL_GPU_INVALID_ID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return gpumgrSetGpuInitDisabledNvlinks(pParams->gpuId, pParams->mask, pParams->bSkipHwNvlinkDisable);
}

NV_STATUS
cliresCtrlCmdGpuSetNvlinkBwMode_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS *pParams
)
{
    return gpumgrSetGpuNvlinkBwMode(pParams->mode);
}

NV_STATUS
cliresCtrlCmdGpuGetNvlinkBwMode_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS *pParams
)
{
    pParams->mode = gpumgrGetGpuNvlinkBwMode();
    return NV_OK;
}

/*!
* @brief Get Rcerr Rpt. Returns an Rc Error report form the circular buffer.
*
* @returns NV_OK on success
*/
NV_STATUS
cliresCtrlCmdNvdGetRcerrRpt_IMPL
(
    RmClientResource                         *pRmCliRes,
    NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS *pParams
)
{
    NV_STATUS status         = NV_OK;
    NvU32     gpuMask        = 0;
    NvU32     gpuAttachCount = 0;
    NvU32     gpuIdx         = 0;
    OBJGPU   *pGpu           = NULL;

    NV_ASSERT_OK_OR_RETURN(gpumgrGetGpuAttachInfo(&gpuAttachCount, &gpuMask));

    pGpu = gpumgrGetNextGpu(gpuMask, &gpuIdx);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    // Assume we won't transfer any data, set the rptIdx & flags accordingly.
    pParams->rptIdx    = ~pParams->reqIdx;
    pParams->flags     = 0;
    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        pParams->processId = osGetCurrentProcess();
    }

    if ((status = krcCliresCtrlNvdGetRcerrRptCheckPermissions_HAL(
             GPU_GET_KERNEL_RC(pGpu),
             pRmCliRes,
             pParams)) != NV_OK)
    {
        return status;
    }

    {
        Journal                  *pRcDB = SYS_GET_RCDB(SYS_GET_INSTANCE());
        RmRCCommonJournal_RECORD *pCommon;

        status = rcdbGetRcDiagRecBoundaries(pRcDB,
                                            &pParams->startIdx,
                                            &pParams->endIdx,
                                            pParams->owner,
                                            pParams->processId);
        if (status != NV_OK)
        {
            return status;
        }

        pParams->flags |= NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_RANGE_VALID;

        {
            NV_STATUS localStatus = rcdbGetRcDiagRec(pRcDB,
                                                     pParams->reqIdx,
                                                     &pCommon,
                                                     pParams->owner,
                                                     pParams->processId);
            switch (localStatus)
            {
                case NV_OK:
                    break;
                case NV_ERR_BUSY_RETRY:
                    return localStatus;
                default:
                    return status;
            }
        }

        if (pCommon != NULL)
        {
            NvU32            i       = 0;
            RmRcDiag_RECORD *pRecord = (RmRcDiag_RECORD *)&pCommon[1];

            pParams->GPUTag   = pCommon->GPUTag;
            pParams->rptIdx   = pRecord->idx;
            pParams->rptTime  = pRecord->timeStamp;
            pParams->rptType  = pRecord->type;
            pParams->rptCount = pRecord->count;
            pParams->flags |= pRecord->flags;

            for (i = 0; i < pRecord->count; ++i)
            {
                pParams->report[i].tag       = pRecord->data[i].tag;
                pParams->report[i].value     = pRecord->data[i].value;
                pParams->report[i].attribute = pRecord->data[i].attribute;
            }
            for (; i < NV0000_CTRL_CMD_NVD_RCERR_RPT_MAX_ENTRIES; ++i)
            {
                pParams->report[i].tag =
                    NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_EMPTY;
                pParams->report[i].value     = 0;
                pParams->report[i].attribute = 0;
            }
            pParams->flags |= NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_DATA_VALID;
        }
    }

    return status;
}

NV_STATUS
cliresCtrlCmdLegacyConfig_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS *pParams
)
{
    NvHandle      hClient            = RES_GET_CLIENT_HANDLE(pRmCliRes);
    RsClient     *pClient            = RES_GET_CLIENT(pRmCliRes);
    RmClient     *pRmClient          = dynamicCast(pClient, RmClient);
    NvHandle      hDeviceOrSubdevice = pParams->hContext;
    NvHandle      hDevice;
    OBJGPU       *pGpu;
    GpuResource  *pGpuResource;
    NV_STATUS     rmStatus           = NV_OK;
    CALL_CONTEXT *pCallContext       = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    //
    // Clients pass in device or subdevice as context for NvRmConfigXyz.
    //
    rmStatus = gpuresGetByDeviceOrSubdeviceHandle(pClient,
                                                  hDeviceOrSubdevice,
                                                  &pGpuResource);
    if (rmStatus != NV_OK)
        return rmStatus;

    hDevice = RES_GET_HANDLE(GPU_RES_GET_DEVICE(pGpuResource));
    pGpu    = GPU_RES_GET_GPU(pGpuResource);

    //
    // GSP client builds should have these legacy APIs disabled,
    // but a monolithic build running in offload mode can still reach here,
    // so log those cases and bail early to keep the same behavior.
    //
    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    GPU_RES_SET_THREAD_BC_STATE(pGpuResource);

    pParams->dataType = pParams->opType;

    switch (pParams->opType)
    {
        default:
            PORT_UNREFERENCED_VARIABLE(pGpu);
            PORT_UNREFERENCED_VARIABLE(hDevice);
            PORT_UNREFERENCED_VARIABLE(hClient);
            rmStatus = NV_ERR_NOT_SUPPORTED;
            break;
    }

    return rmStatus;
}

NV_STATUS
cliresCtrlCmdIdleChannels_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS *pParams
)
{
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    NvBool        bUserModeArgs = pCallContext->secInfo.paramLocation != PARAM_LOCATION_KERNEL;
    NvHandle      hClient       = RES_GET_CLIENT_HANDLE(pRmCliRes);

    return RmIdleChannels(hClient, pParams->hDevice, pParams->hChannel, pParams->numChannels,
                          pParams->phClients, pParams->phDevices, pParams->phChannels,
                          pParams->flags, pParams->timeout, bUserModeArgs);
}

NV_STATUS
cliresCtrlCmdSystemSyncExternalFabricMgmt_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS *pExtFabricMgmtParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED,
                      pExtFabricMgmtParams->bExternalFabricMgmt);
    return NV_OK;
}

NV_STATUS cliresCtrlCmdSystemGetClientDatabaseInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS *pParams
)
{
    pParams->clientCount = g_resServ.activeClientCount;
    pParams->resourceCount = g_resServ.activeResourceCount;
    return NV_OK;
}

/*!
 * @brief Used to push the GSP ucode into RM. This function is used only on
 *        VMware
 *
 * @return
 *      NV_OK                   The sent data is stored successfully
 *      NV_ERR_INVALID_ARGUMENT if the arguments are not proper
 *      NV_ERR_NO_MEMORY        if memory allocation failed
 *      NV_ERR_NOT_SUPPORTED    if function is invoked on non-GSP setup or any
 *                              setup other than VMware host
 */
NV_STATUS cliresCtrlCmdPushGspUcode_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_PUSH_GSP_UCODE_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS cliresCtrlCmdSystemRmctrlCacheModeCtrl_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS *pParams
)
{
    switch (pParams->cmd)
    {
        case NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_CMD_SET:
            rmapiControlCacheSetMode(pParams->mode);
            break;
        case NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_CMD_GET:
            pParams->mode = rmapiControlCacheGetMode();
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    status = pfmreqhndlrGetPerfSensorCounters(pParams, PFM_REQ_HNDLR_PSR_PUB_TAG);
    return status;
}

NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    status = pfmreqhndlrGetPerfSensorCounters(pParams, 0);
    return status;
}

// GPS HOSUNGK DELETE after KMD, NvAPI changes are made
NV_STATUS
cliresCtrlCmdSystemGpsControl_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS *controlParams
)
{
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGpsBatchControl_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS *controlParams
)
{
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGetPerfSensorCounters_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGetExtendedPerfSensorCounters_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGetVrrCookiePresent_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT_PARAMS *pParams
)
{
    OBJGPU *pGpu          = NULL;
    NvU32   gpuCount      = 0;
    NvU32   gpuAttachMask = 0;

    NvU32        objSize = 0;
    NvU8        *pObjData = NULL;
    NV_STATUS    status;

    // Get the master GPU to query the cookies
    gpumgrGetGpuAttachInfo(&gpuCount, &gpuAttachMask);
    pGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(gpuAttachMask));

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    status = getAcpiDsmObjectData(pGpu, &pObjData, &objSize,
                                  ACPI_DSM_FUNCTION_CURRENT, NBSI_VALKEY, NBSI_VALIDATE_ALL);

    pParams->bIsPresent = ((status == NV_ERR_BUFFER_TOO_SMALL) && (objSize != 0));

    return NV_OK;
}
