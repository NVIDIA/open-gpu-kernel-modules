/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "gpu_mgr/gpu_mgr.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "resserv/rs_access_map.h"
#include "nvBldVer.h"
#include "nvVer.h"
#include "mem_mgr/mem.h"
#include "nvsecurityinfo.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "resource_desc.h"

#include "mem_mgr/fla_mem.h"
#include "platform/chipset/chipset_info.h"
#include "platform/chipset/chipset.h"
#include "platform/cpu.h"
#include "platform/platform.h"
#include "platform/p2p/p2p_caps.h"
#include "platform/acpi_common.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "vgpu/rpc.h"
#include "diagnostics/gpu_acct.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "diagnostics/journal.h"
#include "ctrl/ctrl0000/ctrl0000nvd.h"
#include "nvdump.h"
#include "gpu/gsp/kernel_gsp.h"
#include "power/gpu_boost_mgr.h"

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

    if (NV_OK != serverutilGetClientUnderLock(hClient, &pClient))
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
    RmClient *pClient;

    if (NV_OK != serverutilGetClientUnderLock(hClient, &pClient))
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
    NvU32  *pBusPeerIds
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

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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

        // Get the optimal CEs for P2P read/write for 2 gpu masks only
        if (gpuCount == 2)
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
    else if (connectivity == P2P_CONNECTIVITY_PCIE_BAR1 || (connectivity == P2P_CONNECTIVITY_PCIE))
    {
        if (p2pCaps != NULL)
        {
            *p2pCaps |= (p2pReadCapStatus == NV0000_P2P_CAPS_STATUS_OK) ?
                REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED, _TRUE) : 0;
            *p2pCaps |= (p2pWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK) ?
                REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED, _TRUE) : 0;
            *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED, _TRUE);

            if (connectivity == P2P_CONNECTIVITY_PCIE_BAR1)
            {
                *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED, _TRUE);
            }
            else
            {
                *p2pCaps |= REF_DEF(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED, _TRUE);
            }
        }

        if (p2pCapsStatus != NULL)
        {
            p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_PROP] = NV0000_P2P_CAPS_STATUS_OK;

            if (connectivity == P2P_CONNECTIVITY_PCIE_BAR1)
            {
                p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_PCI_BAR1] = NV0000_P2P_CAPS_STATUS_OK;
            }
            else
            {
                p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_PCI] = NV0000_P2P_CAPS_STATUS_OK;
            }
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
    NvU32      featuresMask = 0;

    NV_ASSERT_OR_RETURN(pSys != NULL, NV_ERR_INVALID_STATE);

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    if (pSys->getProperty(pSys, PDB_PROP_SYS_IS_UEFI))
    {
         featuresMask = FLD_SET_DRF(0000, _CTRL_SYSTEM_GET_FEATURES,
            _UEFI, _TRUE, featuresMask);
    }

    // Don't update EFI init on non Display system
    if (pSys->getProperty(pSys, PDB_PROP_SYS_IS_EFI_INIT))
    {
        featuresMask = FLD_SET_DRF(0000, _CTRL_SYSTEM_GET_FEATURES,
            _IS_EFI_INIT, _TRUE, featuresMask);
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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    ct_assert(sizeof(NV_VERSION_STRING) <= sizeof(pParams->driverVersionBuffer));
    ct_assert(sizeof(NV_BUILD_BRANCH_VERSION) <= sizeof(pParams->versionBuffer));
    ct_assert(sizeof(NV_DISPLAY_DRIVER_TITLE) <= sizeof(pParams->titleBuffer));

    portMemCopy(pParams->driverVersionBuffer, sizeof(pParams->driverVersionBuffer),
                NV_VERSION_STRING, sizeof(NV_VERSION_STRING));
    portMemCopy(pParams->versionBuffer, sizeof(pParams->versionBuffer),
                NV_BUILD_BRANCH_VERSION, sizeof(NV_BUILD_BRANCH_VERSION));
    portMemCopy(pParams->titleBuffer, sizeof(pParams->titleBuffer),
                NV_DISPLAY_DRIVER_TITLE, sizeof(NV_DISPLAY_DRIVER_TITLE));

    pParams->changelistNumber = NV_BUILD_CHANGELIST_NUM;
    pParams->officialChangelistNumber = NV_LAST_OFFICIAL_CHANGELIST_NUM;

    return NV_OK;
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

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
// cliresCtrlCmdSystemSetMemorySize
//
// Set system memory size in pages.
//
// Lock Requirements:
//      Assert that API and GPUs locks held on entry
//
NV_STATUS
cliresCtrlCmdSystemSetMemorySize_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS = SYS_GET_OS(pSys);

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner() && rmGpuLockIsOwner());

    pOS->SystemMemorySize = pParams->memorySize;

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

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner() && rmGpuLockIsOwner());

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

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    return gpumgrGetGpuIdInfo(pGpuIdInfoParams);
}

NV_STATUS
cliresCtrlCmdGpuGetIdInfoV2_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS *pGpuIdInfoParams
)
{
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    pDeviceIdsParams->deviceIds = gpumgrGetDeviceInstanceMask();

    return NV_OK;
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

    NV_ASSERT(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    return gpumgrGetProbedGpuIds(pGpuProbedIds);
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
    NvU32 i, j;
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
        for (j = 0; (j < NV0000_CTRL_GPU_MAX_PROBED_GPUS) &&
                    (pGpuProbedIds->gpuIds[j] != NV0000_CTRL_GPU_INVALID_ID); j++)
        {
            if (pGpuAttachIds->gpuIds[i] == pGpuProbedIds->gpuIds[j])
                break;
        }

        if ((j == NV0000_CTRL_GPU_MAX_PROBED_GPUS) ||
            (pGpuProbedIds->gpuIds[j] == NV0000_CTRL_GPU_INVALID_ID))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    // XXX add callback to attach logic on Windows
done:
    portMemFree(pGpuProbedIds);
    return status;
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

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS32(pGsyncAttachedIds->gsyncIds); i++)
    {
        pGsyncAttachedIds->gsyncIds[i] = NV0000_CTRL_GSYNC_INVALID_ID;
    }

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdGsyncGetIdInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS *pGsyncIdInfoParams
)
{
    return NV_ERR_NOT_SUPPORTED;
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
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU     *pGpu;

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pAcctInfoParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
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
    OBJGPU     *pGpu = NULL;
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    OBJSYS     *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);

    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
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
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU     *pGpu = NULL;

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
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
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU     *pGpu = NULL;

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
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
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    OBJGPU     *pGpu;

    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    pGpu = gpumgrGetGpuFromId(pAcctPidsParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return gpuacctGetAcctPids(pGpuAcct, pAcctPidsParams);
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
                kgspDumpGspLogs(pGpu, pKernelGsp, NV_TRUE);
            }
        }

        pParams->version    = NvLogLogger.version;

        portMemSet(pParams->bufferTags, 0, sizeof(pParams->bufferTags));
        for (i = 0; i < NVLOG_MAX_BUFFERS; i++)
        {
            if (NvLogLogger.pBuffers[i] != NULL)
                pParams->bufferTags[i] = NvLogLogger.pBuffers[i]->tag;
        }
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
        NvBool bPause;

        if (pParams->tag != 0)
        {
            status = nvlogGetBufferHandleFromTag(pParams->tag, &hBuffer);
            NV_ASSERT_OR_RETURN(status == NV_OK, status);
        }
        else
        {
            NV_ASSERT_OR_RETURN(pParams->buffer < NVLOG_MAX_BUFFERS, NV_ERR_INVALID_ARGUMENT);
            hBuffer = pParams->buffer;
        }

        pBuffer = NvLogLogger.pBuffers[hBuffer];
        NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_OBJECT_NOT_FOUND);

        bPause = pParams->flags & DRF_DEF(0000, _CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS, _PAUSE, _YES);
        nvlogPauseLoggingToBuffer(hBuffer, bPause);

        pParams->tag        = pBuffer->tag;
        pParams->size       = pBuffer->size;
        pParams->flags      = pBuffer->flags;
        pParams->pos        = pBuffer->pos;
        pParams->overflow   = pBuffer->extra.ring.overflow;
        status = NV_OK;
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
    }

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

    if ((pP2PParams->gpuCount ==  0) || (pP2PParams->gpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIds[0]);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return CliGetSystemP2pCaps(pP2PParams->gpuIds,
                               pP2PParams->gpuCount,
                              &pP2PParams->p2pCaps,
                              &pP2PParams->p2pOptimalReadCEs,
                              &pP2PParams->p2pOptimalWriteCEs,
                               NvP64_VALUE(pP2PParams->p2pCapsStatus),
                               NvP64_VALUE(pP2PParams->busPeerIds));
}

NV_STATUS
cliresCtrlCmdSystemGetP2pCapsV2_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *pP2PParams
)
{
    OBJGPU *pGpu;

    if ((pP2PParams->gpuCount ==  0) || (pP2PParams->gpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = gpumgrGetGpuFromId(pP2PParams->gpuIds[0]);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return CliGetSystemP2pCaps(pP2PParams->gpuIds,
                               pP2PParams->gpuCount,
                              &pP2PParams->p2pCaps,
                              &pP2PParams->p2pOptimalReadCEs,
                              &pP2PParams->p2pOptimalWriteCEs,
                               NvP64_VALUE(pP2PParams->p2pCapsStatus),
                               NvP64_VALUE(pP2PParams->busPeerIds));
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
                                         2,
                                         &pP2PParams->p2pCaps[grpAIdx][grpBIdx],
                                         &pP2PParams->a2bOptimalReadCes[grpAIdx][grpBIdx],
                                         &pP2PParams->a2bOptimalWriteCes[grpAIdx][grpBIdx],
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
                                         2,
                                         NULL, // Skip p2pCaps
                                         &pP2PParams->b2aOptimalReadCes[grpAIdx][grpBIdx],
                                         &pP2PParams->b2aOptimalWriteCes[grpAIdx][grpBIdx],
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
    NV_STATUS  status   = NV_OK;
    OBJSYS    *pSys     = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr  = SYS_GET_GPUMGR(pSys);
    OBJGPU    *pGpu     = NULL;
    NvU32      gpuAttachCnt = 0, gpuAttachMask = 0, i = 0;
    NvU32      gpuIndex     = 0;
    RM_API    *pRmApi;
    NV0080_CTRL_INTERNAL_PERF_GET_UNDERPOWERED_GPU_COUNT_PARAMS params = {0};

    pGpu = gpumgrGetSomeGpu();

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_CHECK_OK_OR_RETURN(
        LEVEL_INFO,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalDevice,
                        NV0080_CTRL_CMD_INTERNAL_PERF_GET_UNDERPOWERED_GPU_COUNT,
                        &params,
                        sizeof(params)));

    pGpuMgr->powerDisconnectedGpuCount = params.powerDisconnectedGpuCount;
    portMemCopy(pGpuMgr->powerDisconnectedGpuBus, sizeof(*pGpuMgr->powerDisconnectedGpuBus) * NV_MAX_DEVICES,
            &params.powerDisconnectedGpuBus, sizeof(*params.powerDisconnectedGpuBus) * NV_MAX_DEVICES);

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
    RmClient *pClient;

    if (NV_OK != serverutilGetClientUnderLock(hClient, &pClient))
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
    RmClient *pClient;

    if (NV_OK != serverutilGetClientUnderLock(hClient, &pClient))
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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

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
    LOCK_ASSERT_AND_RETURN(rmApiLockIsOwner());

    status = gpuboostmgrQueryGroups(pBoostMgr, pParams);
    NV_ASSERT(NV_OK == status);
    return status;
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

        // Soon FlaMemory will be moved to ADDR_FABRIC. For now, this WAR.
        if ((memType == ADDR_FBMEM) && (dynamicCast(pMemory, FlaMemory) != NULL))
        {
            memType = ADDR_FABRIC;
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
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_SYSMEM;
            break;
        case ADDR_FBMEM:
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM;
            break;
        case ADDR_REGMEM:
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_REGMEM;
            break;
        case ADDR_FABRIC:
        case ADDR_FABRIC_V2:
            pParams->addrSpaceType = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_FABRIC;
            break;
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

    resservSwapTlsCallContext(&pOldCallContext, &callContext);
    status = clientShareResource(pClient, pObjectRef, pSharePolicy, &callContext);
    resservRestoreTlsCallContext(pOldCallContext);
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
    NvU32     processId      = osGetCurrentProcess();

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
        pParams->processId = processId;
    }

    if ((status = krcCliresCtrlNvdGetRcerrRptCheckPermissions_HAL(
             GPU_GET_KERNEL_RC(pGpu),
             pRmCliRes,
             pParams)) != NV_OK)
    {
        return status;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        //
        // Pre-GSP, RcDiagRec from all GPUs were stored in kernel sysmem in a
        // single RING_BUFFER_LOG.
        //
        // With GSP, each GPU its own separate RING_BUFFER_LOG. We need to
        // search in all of them.
        //
        // However, we will always return only the first matching record in all
        // cases (similar to pre-GSP behavior)
        //
        for (; pGpu != NULL ; pGpu = gpumgrGetNextGpu(gpuMask, &gpuIdx))
        {
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
            NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS localParams = {0};
            localParams.reqIdx    = pParams->reqIdx;
            localParams.owner     = pParams->owner;
            localParams.processId = pParams->processId;

            status = pRmApi->Control(pRmApi,
                                     RES_GET_CLIENT_HANDLE(pRmCliRes),
                                     RES_GET_HANDLE(pRmCliRes),
                                     NV0000_CTRL_CMD_NVD_GET_RCERR_RPT,
                                     &localParams,
                                     sizeof localParams);
            if (status == NV_OK &&
                (localParams.flags &
                 NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_DATA_VALID))
            {
                //
                // Each RING_BUFFER_LOG can contain MAX_RCDB_RCDIAG_WRAP_BUFF
                // RmRcDiag_RECORD. We will multiply indices returned to the
                // client by this value so the GPU can be uniquely identified
                // (in addition to GPUTag) from
                // NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS.rptIdx
                //
                // Note that this will result in clients receivinga rptIdx value
                // larger than MAX_RCDB_RCDIAG_WRAP_BUFF.
                //
                NvU16 indexOffset = gpuIdx * MAX_RCDB_RCDIAG_WRAP_BUFF;

                *pParams = localParams;
                pParams->startIdx += indexOffset;
                pParams->endIdx   += indexOffset;
                pParams->rptIdx   += indexOffset;

                return NV_OK;
            }

            if (status == NV_ERR_BUSY_RETRY)
            {
                //
                // To avoid the case where we silently fail to find a record
                // because we skipped over to the next Gpu on getting a
                // BUSY_RETRY on one of the Gpus (which might have contained the
                // record).
                //
                return status;
            }
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

NV_STATUS
cliresCtrlCmdSystemGetPerfSensorCounters_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    portMemSet(pParams, 0, sizeof(*pParams));
    return status;
}

NV_STATUS
cliresCtrlCmdSystemGetExtendedPerfSensorCounters_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    portMemSet(pParams, 0, sizeof(*pParams));
    return status;
}
