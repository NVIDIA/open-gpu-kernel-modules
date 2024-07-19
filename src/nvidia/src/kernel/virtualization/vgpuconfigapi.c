/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "virtualization/vgpuconfigapi.h"

#include "core/core.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/heap.h"
#include "kernel/gpu/fifo/kernel_sched_mgr.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "rmapi/control.h"
#include "nv-hypervisor.h"
#include "ctrl/ctrla081.h"
#include "nvrm_registry.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

NV_STATUS
vgpuconfigapiConstruct_IMPL
(
    VgpuConfigApi                *pVgpuConfigApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NvU32                        i, pgpuIndex;
    NV_STATUS                    rmStatus        = NV_OK;
    VGPU_CONFIG_EVENT_INFO_NODE *pVgpuConfigEventInfoNode = NULL;
    OBJGPU                      *pGpu            = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS                      *pSys            = SYS_GET_INSTANCE();
    KernelVgpuMgr               *pKernelVgpuMgr  = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvHandle                     hClient         = pParams->hClient;
    NvHandle                     hVgpuConfig     = pParams->hResource;

    // Forbid allocation of this class on GSP-RM and Guest-RM
    // to avoid fuzzing this class in such cases. See bug 3529160.
    if (!RMCFG_FEATURE_KERNEL_RM || IS_VIRTUAL(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    for (i = 0; i < NVA081_NOTIFIERS_MAXCOUNT; i++)
    {
        pVgpuConfigApi->notifyActions[i] = NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    }

    // Add config event info to vgpuMgr DB
    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex) != NV_OK)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pVgpuConfigEventInfoNode = listAppendNew(&(pKernelVgpuMgr->pgpuInfo[pgpuIndex].listVgpuConfigEventsHead));
    if (pVgpuConfigEventInfoNode != NULL)
    {
        portMemSet(pVgpuConfigEventInfoNode, 0, sizeof(VGPU_CONFIG_EVENT_INFO_NODE));

        pVgpuConfigEventInfoNode->hVgpuConfig       = hVgpuConfig;
        pVgpuConfigEventInfoNode->hClient           = hClient;
        pVgpuConfigEventInfoNode->pVgpuConfigApi    = pVgpuConfigApi;
    }

    return rmStatus;
}

void
vgpuconfigapiDestruct_IMPL
(
    VgpuConfigApi *pVgpuConfigApi
)
{
    NV_STATUS                    rmStatus        = NV_OK;
    OBJGPU                      *pGpu            = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS                      *pSys            = SYS_GET_INSTANCE();
    KernelVgpuMgr               *pKernelVgpuMgr  = SYS_GET_KERNEL_VGPUMGR(pSys);
    VGPU_CONFIG_EVENT_INFO_NODE *pVgpuConfigEventInfoNode = NULL;
    NvHandle                     hClient;
    NvHandle                     hVgpuConfig;
    NvU32                        pgpuIndex;
    CALL_CONTEXT                *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;

    resGetFreeParams(staticCast(pVgpuConfigApi, RsResource), &pCallContext, &pParams);
    hClient     = pParams->hClient;
    hVgpuConfig = pParams->hResource;

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex) != NV_OK)
    {
        pParams->status = NV_ERR_OBJECT_NOT_FOUND;
        return;
    }

    rmStatus = kvgpumgrGetConfigEventInfoFromDb(hClient,
                                               hVgpuConfig,
                                               &pVgpuConfigEventInfoNode,
                                               pgpuIndex);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to remove config event info from DB\n");
        NV_ASSERT(0);
        pParams->status = rmStatus;
        return;
    }
    else
    {
        listRemove(&(pKernelVgpuMgr->pgpuInfo[pgpuIndex].listVgpuConfigEventsHead),
                   pVgpuConfigEventInfoNode);
    }

    pParams->status = rmStatus;
}

void
CliNotifyVgpuConfigEvent
(
    OBJGPU     *pGpu,
    NvU32       notifyIndex
)
{
    VGPU_CONFIG_EVENT_INFO_NODE *pVgpuConfigEventInfoNode = NULL;
    PEVENTNOTIFICATION           pEventNotification;
    VgpuConfigApi               *pVgpuConfigApi;
    OBJSYS                      *pSys           = SYS_GET_INSTANCE();
    KernelVgpuMgr               *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32                        pgpuIndex;

    // Get pgpuIndex, for which we need to notify events
    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex) != NV_OK)
        return;

    for (pVgpuConfigEventInfoNode = listHead(&(pKernelVgpuMgr->pgpuInfo[pgpuIndex].listVgpuConfigEventsHead));
         pVgpuConfigEventInfoNode != NULL;
         pVgpuConfigEventInfoNode = listNext(&(pKernelVgpuMgr->pgpuInfo[pgpuIndex].listVgpuConfigEventsHead), pVgpuConfigEventInfoNode))
    {
        pVgpuConfigApi = pVgpuConfigEventInfoNode->pVgpuConfigApi;
        if (pVgpuConfigApi->notifyActions[notifyIndex] == NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
        {
            continue;
        }

        pEventNotification = inotifyGetNotificationList(staticCast(pVgpuConfigApi, INotifier));
        // ping all events on the list of type notifyIndex
        while (pEventNotification)
        {
            if (pEventNotification->NotifyIndex == notifyIndex)
            {
                if (osNotifyEvent(pGpu, pEventNotification, 0, 0, 0) != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "CliNotifyVgpuEvent: failed to deliver event 0x%x\n",
                              notifyIndex);
                }
            }
            pEventNotification = pEventNotification->Next;
        }

        // reset if single shot notify action
        if (pVgpuConfigApi->notifyActions[notifyIndex] == NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE)
        {
            pVgpuConfigApi->notifyActions[notifyIndex] = NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
        }
    }
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams
)
{
    NV_STATUS rmStatus = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo = NULL;
    NvU32 index;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);
    if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to find pGpu info entry.\n");
        return rmStatus;
    }

    pPhysGpuInfo = &pKernelVgpuMgr->pgpuInfo[index];

    // Set vgpu config state
    if (pPhysGpuInfo == NULL)
        return NV_ERR_INVALID_STATE;
    else
        pPhysGpuInfo->vgpuConfigState = pParams->vgpuConfigState;

    rmStatus = kvgpumgrPgpuAddVgpuType(pGpu, pParams->discardVgpuTypes, &pParams->vgpuInfo);
    if (rmStatus != NV_OK)
    {
        pPhysGpuInfo->vgpuConfigState = NVA081_CTRL_VGPU_CONFIG_STATE_UNINITIALIZED;
        NV_PRINTF(LEVEL_ERROR, "Failed to add vGPU type to pGPU info.\n");
        return rmStatus;
    }

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);

    return kvgpumgrGetVgpuFbUsage(pGpu, pParams);
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams
)
{
    NVA081_VGPU_GUEST       *pVgpuGuest;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = NULL;
    KERNEL_VGPU_GUEST       *pVgpuGuestTmp         = NULL;
    KERNEL_PHYS_GPU_INFO    *pPhysGpuInfo;
    OBJGPU                  *pGpu                  = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS                  *pSys                  = SYS_GET_INSTANCE();
    KernelVgpuMgr           *pKernelVgpuMgr        = SYS_GET_KERNEL_VGPUMGR(pSys);
    NV_STATUS               rmStatus               = NV_OK;
    NvU32                   index[] = {3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15};
    NvU32                   i, j;
    NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pVgpuPerPgpuParams;
    NV2080_VGPU_GUEST       *pVgpuGuestGsp;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    // This structure can't be allocated on stack because it will result function stack usage > 4KB
    pVgpuPerPgpuParams = portMemAllocNonPaged(sizeof(*pVgpuPerPgpuParams));
    if (pVgpuPerPgpuParams == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pVgpuPerPgpuParams, 0, sizeof(*pVgpuPerPgpuParams));

    NV_ASSERT_OK_OR_GOTO(rmStatus, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i), failed);

    NV_ASSERT_OK_OR_GOTO(rmStatus, kvgpumgrEnumerateVgpuPerPgpu(pGpu, pVgpuPerPgpuParams), failed);

    pPhysGpuInfo = &pKernelVgpuMgr->pgpuInfo[i];

    if (pVgpuPerPgpuParams->numVgpu != pPhysGpuInfo->numActiveVgpu)
    {
        rmStatus = NV_ERR_INVALID_STATE;
        goto failed;
    }

    i = 0;
    for (pKernelHostVgpuDevice = listHead(&(pPhysGpuInfo->listHostVgpuDeviceHead));
         pKernelHostVgpuDevice != NULL;
         pKernelHostVgpuDevice = listNext(&(pPhysGpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice), i++)
    {
        pVgpuGuestTmp = pKernelHostVgpuDevice->vgpuGuest;
        if (pVgpuGuestTmp == NULL)
        {
            rmStatus = NV_ERR_INVALID_STATE;
            goto failed;
        }

        if (i >= NV_ARRAY_ELEMENTS(pParams->vgpuGuest))
        {
            rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
            goto failed;
        }
        pVgpuGuest = &pParams->vgpuGuest[i];

        portMemSet(pVgpuGuest->guestVmInfo.vmName, 0, NVA081_VM_NAME_SIZE);
        portMemSet(pVgpuGuest->vgpuDevice.mdevUuid, 0, VGPU_UUID_SIZE);

        if (osIsVgpuVfioPresent() == NV_OK)
        {
            portMemCopy(pVgpuGuest->guestVmInfo.vmName,
                        NVA081_VM_NAME_SIZE,
                        pVgpuGuestTmp->guestVmInfo.vmName,
                        NVA081_VM_NAME_SIZE);

            if (pKernelHostVgpuDevice->pRequestVgpuInfoNode == NULL)
            {
                rmStatus = NV_ERR_INVALID_POINTER;
                goto failed;
            }

            /*
             * SRIOV vGPUs using vfio-pci-core framework on KVM do not use MDEV framework,
             * so don't expose MDEV UUID to NVML.
             */
            if (!osIsVfioPciCorePresent() || !gpuIsSriovEnabled(pGpu))
            {
                for (j = 0; j < VGPU_UUID_SIZE; j++)
                    pVgpuGuest->vgpuDevice.mdevUuid[j] = pKernelHostVgpuDevice->pRequestVgpuInfoNode->mdevUuid[index[j]];
            }
        }

        // Copy VM's information
        if (!IS_GSP_CLIENT(pGpu))
        {
            pVgpuGuestGsp = &pVgpuPerPgpuParams->vgpuGuest[i];
        }
        else
        {
            pVgpuGuestGsp = NULL;
            for (j = 0; j < pVgpuPerPgpuParams->numVgpu; j++)
            {
                if (pVgpuPerPgpuParams->vgpuGuest[j].vgpuDevice.gfid == pKernelHostVgpuDevice->gfid)
                {
                    pVgpuGuestGsp = &pVgpuPerPgpuParams->vgpuGuest[j];
                    break;
                }
            }
            if (pVgpuGuestGsp == NULL)
            {
                rmStatus = NV_ERR_OBJECT_NOT_FOUND;
                goto failed;
            }
        }

        pVgpuGuest->guestVmInfo.vmPid                       = pVgpuGuestGsp->guestVmInfo.vmPid;
        pVgpuGuest->guestVmInfo.guestOs                     = pVgpuGuestGsp->guestVmInfo.guestOs;
        pVgpuGuest->guestVmInfo.migrationProhibited         = pVgpuGuestGsp->guestVmInfo.migrationProhibited;
        pVgpuGuest->guestVmInfo.guestNegotiatedVgpuVersion  = pVgpuGuestGsp->guestVmInfo.guestNegotiatedVgpuVersion;
        pVgpuGuest->guestVmInfo.licensed                    = pVgpuGuestGsp->guestVmInfo.licensed;
        pVgpuGuest->guestVmInfo.licenseState                = pVgpuGuestGsp->guestVmInfo.licenseState;
        pVgpuGuest->guestVmInfo.guestVmInfoState            = pVgpuGuestGsp->guestVmInfo.guestVmInfoState;
        pVgpuGuest->guestVmInfo.licenseExpiryTimestamp      = pVgpuGuestGsp->guestVmInfo.licenseExpiryTimestamp;
        pVgpuGuest->guestVmInfo.licenseExpiryStatus         = pVgpuGuestGsp->guestVmInfo.licenseExpiryStatus;
        pVgpuGuest->guestVmInfo.frameRateLimit              = pVgpuGuestGsp->guestVmInfo.frameRateLimit;

        portStringCopy((char *) pVgpuGuest->guestVmInfo.guestDriverVersion,
                       sizeof(pVgpuGuest->guestVmInfo.guestDriverVersion),
                       (char *) pVgpuGuestGsp->guestVmInfo.guestDriverVersion,
                       NVA081_VGPU_STRING_BUFFER_SIZE);

        portStringCopy((char *) pVgpuGuest->guestVmInfo.guestDriverBranch,
                       sizeof(pVgpuGuest->guestVmInfo.guestDriverBranch),
                       (char *) pVgpuGuestGsp->guestVmInfo.guestDriverBranch,
                       NVA081_VGPU_STRING_BUFFER_SIZE);

        pVgpuGuest->guestVmInfo.vmIdType                    = pVgpuGuestTmp->guestVmInfo.vmIdType;
        if (pVgpuGuestTmp->guestVmInfo.vmIdType == VM_ID_DOMAIN_ID)
        {
            pVgpuGuest->guestVmInfo.guestVmId.vmId          = pVgpuGuestTmp->guestVmInfo.guestVmId.vmId;
        }
        else if (pVgpuGuestTmp->guestVmInfo.vmIdType == VM_ID_UUID)
        {
            portMemCopy(pVgpuGuest->guestVmInfo.guestVmId.vmUuid,
                        NVA081_VM_UUID_SIZE,
                        pVgpuGuestTmp->guestVmInfo.guestVmId.vmUuid,
                        NVA081_VM_UUID_SIZE);
        }
        else
        {
            rmStatus = NV_ERR_INVALID_STATE;
            goto failed;
        }

        pVgpuGuest->vgpuDevice.vgpuType             = pKernelHostVgpuDevice->vgpuType;
        pVgpuGuest->vgpuDevice.vgpuDeviceInstanceId = pVgpuGuestGsp->vgpuDevice.vgpuDeviceInstanceId;
        pVgpuGuest->vgpuDevice.encoderCapacity      = pVgpuGuestGsp->vgpuDevice.encoderCapacity;
        pVgpuGuest->vgpuDevice.fbUsed               = pVgpuGuestGsp->vgpuDevice.fbUsed;
        pVgpuGuest->vgpuDevice.swizzId              = pKernelHostVgpuDevice->swizzId;
        pVgpuGuest->vgpuDevice.eccState             = pVgpuGuestGsp->vgpuDevice.eccState;
        pVgpuGuest->vgpuDevice.bDriverLoaded        = pVgpuGuestGsp->vgpuDevice.bDriverLoaded;
        pVgpuGuest->vgpuDevice.placementId          = pKernelHostVgpuDevice->placementId;

        portMemCopy(pVgpuGuest->vgpuDevice.vgpuUuid, VGPU_UUID_SIZE, pKernelHostVgpuDevice->vgpuUuid, VGPU_UUID_SIZE);

        pVgpuGuest->vgpuDevice.vgpuPciId = pVgpuGuestGsp->vgpuDevice.vgpuPciId;
    }

    pParams->numVgpu  = pPhysGpuInfo->numActiveVgpu;
    pParams->vgpuType = NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE; // This field is unused on the client side.
    // We should remove this field, unless it is required for compatibility reasons.

failed:
    portMemFree(pVgpuPerPgpuParams);
    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams
)
{
    // This code should match hostvgpudeviceapiCtrlCmdGetVgpuTypeInfo

    VGPU_TYPE            *vgpuTypeInfo;
    NV_STATUS             rmStatus         = NV_OK;
    OBJGPU               *pGpu             = GPU_RES_GET_GPU(pVgpuConfigApi);
    NvU32                 data             = 0;
    OBJSYS               *pSys             = SYS_GET_INSTANCE();
    KernelVgpuMgr        *pKernelVgpuMgr   = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU8                  shortGpuNameString[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvU32                 i;
    NvU32                 pgpuIndex;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if ((rmStatus = kvgpumgrGetVgpuTypeInfo(pParams->vgpuType, &vgpuTypeInfo)) != NV_OK)
    {
        return rmStatus;
    }

    if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex)) != NV_OK)
        return rmStatus;

    pPgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    pParams->vgpuTypeInfo.vgpuType           = vgpuTypeInfo->vgpuTypeId;
    pParams->vgpuTypeInfo.maxInstance        = vgpuTypeInfo->maxInstance;
    pParams->vgpuTypeInfo.numHeads           = vgpuTypeInfo->numHeads;
    pParams->vgpuTypeInfo.maxResolutionX     = vgpuTypeInfo->maxResolutionX;
    pParams->vgpuTypeInfo.maxResolutionY     = vgpuTypeInfo->maxResolutionY;
    pParams->vgpuTypeInfo.maxPixels          = vgpuTypeInfo->maxPixels;
    pParams->vgpuTypeInfo.frlConfig          = vgpuTypeInfo->frlConfig;
    pParams->vgpuTypeInfo.cudaEnabled        = vgpuTypeInfo->cudaEnabled;
    pParams->vgpuTypeInfo.eccSupported       = vgpuTypeInfo->eccSupported;
    pParams->vgpuTypeInfo.gpuInstanceSize    = vgpuTypeInfo->gpuInstanceSize;
    pParams->vgpuTypeInfo.vdevId             = vgpuTypeInfo->vdevId;
    pParams->vgpuTypeInfo.pdevId             = vgpuTypeInfo->pdevId;
    pParams->vgpuTypeInfo.profileSize        = vgpuTypeInfo->profileSize;
    pParams->vgpuTypeInfo.fbLength           = vgpuTypeInfo->fbLength;
    pParams->vgpuTypeInfo.gspHeapSize        = vgpuTypeInfo->gspHeapSize;
    pParams->vgpuTypeInfo.fbReservation      = vgpuTypeInfo->fbReservation;
    pParams->vgpuTypeInfo.mappableVideoSize  = vgpuTypeInfo->mappableVideoSize;
    pParams->vgpuTypeInfo.encoderCapacity    = vgpuTypeInfo->encoderCapacity;
    pParams->vgpuTypeInfo.bar1Length         = vgpuTypeInfo->bar1Length;
    pParams->vgpuTypeInfo.gpuDirectSupported = vgpuTypeInfo->gpuDirectSupported;
    pParams->vgpuTypeInfo.nvlinkP2PSupported = vgpuTypeInfo->nvlinkP2PSupported;
    pParams->vgpuTypeInfo.multiVgpuExclusive = vgpuTypeInfo->multiVgpuExclusive;
    pParams->vgpuTypeInfo.frlEnable          = vgpuTypeInfo->frlEnable;
    pParams->vgpuTypeInfo.multiVgpuSupported = vgpuTypeInfo->multiVgpuSupported;

    /* Represents vGPU type level support for heterogeneous timeslice profiles */
    pParams->vgpuTypeInfo.exclusiveType      = !kvgpumgrIsHeterogeneousVgpuSupported();

    /*
     * Represents vGPU type level support for heterogenenous timeslice size.
     * Currently, if a pGpu supports heterogenenous timeslice size, all vGPU
     * types support it. Set to false if supported
     */
    pParams->vgpuTypeInfo.exclusiveSize      = !pPgpuInfo->heterogeneousTimesliceSizesSupported;

    // Disable FRL if we are using sched_sw (PVMRL) in timesliced mode
    if (IsPASCALorBetter(pGpu))
    {
        KernelSchedMgr *pKernelSchedMgr = GPU_GET_KERNEL_SCHEDMGR(pGpu);
        if (pKernelSchedMgr &&
            kschedmgrIsPvmrlEnabled(pKernelSchedMgr))
        {
            pParams->vgpuTypeInfo.frlEnable = 0;
        }
    }

    portStringCopy((char *) pParams->vgpuTypeInfo.vgpuName, sizeof(pParams->vgpuTypeInfo.vgpuName), (char *) vgpuTypeInfo->vgpuName, VGPU_STRING_BUFFER_SIZE);
    portStringCopy((char *) pParams->vgpuTypeInfo.vgpuClass, sizeof(pParams->vgpuTypeInfo.vgpuClass), (char *) vgpuTypeInfo->vgpuClass, VGPU_STRING_BUFFER_SIZE);
    portStringCopy((char *) pParams->vgpuTypeInfo.license, sizeof(pParams->vgpuTypeInfo.license), (char *) vgpuTypeInfo->license, NV_GRID_LICENSE_INFO_MAX_LENGTH);
    portStringCopy((char *) pParams->vgpuTypeInfo.vgpuExtraParams, sizeof(pParams->vgpuTypeInfo.vgpuExtraParams), (char *) vgpuTypeInfo->vgpuExtraParams,
                   sizeof(pParams->vgpuTypeInfo.vgpuExtraParams));
    portStringCopy((char *) pParams->vgpuTypeInfo.licensedProductName, sizeof(pParams->vgpuTypeInfo.licensedProductName), (char *) vgpuTypeInfo->licensedProductName,
                   NV_GRID_LICENSE_INFO_MAX_LENGTH);

    if (NV_OK == osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_PLUGIN_IN_FTRACE_BUFFER, &data))
    {
        if (data == NV_REG_STR_RM_ENABLE_PLUGIN_IN_FTRACE_BUFFER_ENABLED)
            pParams->vgpuTypeInfo.ftraceEnable = data;
    }

    rmStatus = gpuGetShortNameString(pGpu, (void *)shortGpuNameString);
    if (rmStatus != NV_OK)
        return rmStatus;

    portMemCopy(pParams->vgpuTypeInfo.shortGpuNameString, sizeof(shortGpuNameString), shortGpuNameString, sizeof(shortGpuNameString));

    portStringCopy((char *) pParams->vgpuTypeInfo.adapterName, sizeof(pParams->vgpuTypeInfo.adapterName),
                   (char *) vgpuTypeInfo->vgpuName, VGPU_STRING_BUFFER_SIZE);
    portStringCopy((char *) pParams->vgpuTypeInfo.adapterName_Unicode, sizeof(pParams->vgpuTypeInfo.adapterName_Unicode),
                   (char *) vgpuTypeInfo->vgpuName, VGPU_STRING_BUFFER_SIZE);

    portStringConvertAsciiToUtf16(pParams->vgpuTypeInfo.adapterName_Unicode, VGPU_STRING_BUFFER_SIZE,
                                  (char *)pParams->vgpuTypeInfo.adapterName_Unicode, VGPU_STRING_BUFFER_SIZE);

    // used only by NVML
    if (vgpuTypeInfo->gpuInstanceSize != 0)
    {
        NV_ASSERT_OK_OR_RETURN(
            kvgpumgrGetPartitionFlag(vgpuTypeInfo->vgpuTypeId, &pParams->vgpuTypeInfo.gpuInstanceProfileId));
    }
    else
    {
        pParams->vgpuTypeInfo.gpuInstanceProfileId = PARTITIONID_INVALID;
    }

    if (pPgpuInfo->heterogeneousTimesliceSizesSupported == NV_TRUE)
    {
        pParams->vgpuTypeInfo.placementSize     = vgpuTypeInfo->placementSize;
        pParams->vgpuTypeInfo.placementCount    = vgpuTypeInfo->placementCount;

        for (i = 0; i < vgpuTypeInfo->placementCount; i++)
        {
            pParams->vgpuTypeInfo.placementIds[i] = vgpuTypeInfo->supportedPlacementIds[i];
        }
    }
    else
    {
        pParams->vgpuTypeInfo.placementSize     = 0;
        pParams->vgpuTypeInfo.placementCount    = 0;
    }

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams
)
{
    NvU32 pgpuIndex, i;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NV_STATUS rmStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (pGpu == NULL ||
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex)) != NV_OK)
        return rmStatus;

    pPgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    pParams->vgpuConfigState = pPgpuInfo->vgpuConfigState;

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pParams->vgpuTypes[i] = pPgpuInfo->vgpuTypes[i]->vgpuTypeId;
    }
    pParams->numVgpuTypes = pPgpuInfo->numVgpuTypes;

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU32 pgpuIndex;
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NV_STATUS rmStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (pGpu == NULL ||
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex)) != NV_OK)
        return rmStatus;

    rmStatus = kvgpumgrGetCreatableVgpuTypes(pGpu, pKernelVgpuMgr, pgpuIndex,
                                            &pParams->numVgpuTypes, pParams->vgpuTypes);

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pVgpuConfigApi);
    NvHandle hObject = RES_GET_HANDLE(pVgpuConfigApi);
    VGPU_CONFIG_EVENT_INFO_NODE *pVgpuConfigEventInfoNode = NULL;
    VgpuConfigApi *pVgpuConfigInfo;
    NV_STATUS rmStatus = NV_OK;
    NvU32 pgpuIndex;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (pSetEventParams->event >= NVA081_NOTIFIERS_MAXCOUNT)
    {
        NV_PRINTF(LEVEL_INFO, "Bad event 0x%x\n", pSetEventParams->event);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex)) != NV_OK)
        return rmStatus;

    rmStatus = kvgpumgrGetConfigEventInfoFromDb(hClient,
                                               hObject,
                                               &pVgpuConfigEventInfoNode,
                                               pgpuIndex);
    if (rmStatus != NV_OK || pVgpuConfigEventInfoNode == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to find config event info for %x from DB\n",
                  hClient);
        return rmStatus;
    }
    pVgpuConfigInfo = pVgpuConfigEventInfoNode->pVgpuConfigApi;

    if (inotifyGetNotificationList(staticCast(pVgpuConfigInfo, INotifier)) == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "Cmd 0x%x: no event list\n",
                  NVA081_CTRL_CMD_VGPU_CONFIG_EVENT_SET_NOTIFICATION);
        return NV_ERR_INVALID_STATE;
    }

    switch (pSetEventParams->action)
    {
        case NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE:
        case NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT:
        {
            // must be in disabled state to transition to an active state
            if (pVgpuConfigInfo->notifyActions[pSetEventParams->event] != NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
            {
                rmStatus = NV_ERR_INVALID_STATE;
                break;
            }
            pVgpuConfigInfo->notifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }

        case NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE:
        {
            pVgpuConfigInfo->notifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS *pParams
)
{
    OBJGPU          *pGpu           = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS          *pSys           = SYS_GET_INSTANCE();
    KernelVgpuMgr   *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NV_STATUS        rmStatus       = NV_OK;
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 pgpuIndex;
    NvU32 i, j, count = 0;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (pGpu == NULL || (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex));

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[pgpuIndex]);

    for (i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPhysGpuInfo->vgpuTypes[i];
        if (pParams->vgpuTypeId == pVgpuTypeInfo->vgpuTypeId)
        {
            for (j = 0; j < pVgpuTypeInfo->placementCount; j++)
            {
                if (pPhysGpuInfo->creatablePlacementIds[i][j] != NVA081_PLACEMENT_ID_INVALID)
                {
                    pParams->placementIds[count] = pPhysGpuInfo->creatablePlacementIds[i][j];
                    count++;
                }
            }
            pParams->placementSize = pVgpuTypeInfo->placementSize;
            pParams->count = count;
            break;
        }
    }

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigSetCapability_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS *pSetCapabilityParams
)
{
    OBJGPU *pGpu                         = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys                         = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr        = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;
    NV_STATUS rmStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
    {
         return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);

    switch (pSetCapabilityParams->capability)
    {
        case NVA081_CTRL_VGPU_CAPABILITY_MINI_QUARTER_GPU:
        {
            pPhysGpuInfo->miniQuarterEnabled = pSetCapabilityParams->state;
            break;
        }
        case NVA081_CTRL_VGPU_CAPABILITY_COMPUTE_MEDIA_ENGINE_GPU:
        {
            pPhysGpuInfo->computeMediaEngineEnabled = pSetCapabilityParams->state;
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetCapability_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS *pGetCapabilityParams
)
{
    OBJGPU *pGpu                         = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys                         = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr        = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 index;
    NV_STATUS rmStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index) != NV_OK)
    {
         return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    switch (pGetCapabilityParams->capability)
    {
        case NVA081_CTRL_VGPU_CAPABILITY_MINI_QUARTER_GPU:
        {
            pGetCapabilityParams->state = pPhysGpuInfo->miniQuarterEnabled;
            break;
        }
        case NVA081_CTRL_VGPU_CAPABILITY_COMPUTE_MEDIA_ENGINE_GPU:
        {
            pGetCapabilityParams->state = pPhysGpuInfo->computeMediaEngineEnabled;
            break;
        }
        case NVA081_CTRL_VGPU_CAPABILITY_WARM_UPDATE:
        {
            /*
             * As per our current requirement, the capability is supported on all GPUs
             * and hence we are turning true always without checking for input device.
             * If we decided not to support any GPU, this needs to be modified.
             */
            pGetCapabilityParams->state = NV_TRUE;
            if (IS_MIG_ENABLED(pGpu))
            {
                pGetCapabilityParams->state = NV_FALSE;
            }
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi
)
{
    NV_STATUS rmStatus = NV_OK;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);

    if (pGpu != NULL)
    {
        rmStatus = kvgpumgrSetSupportedPlacementIds(pGpu);
        if (rmStatus != NV_OK)
            return rmStatus;
    }
    else
        rmStatus = NV_ERR_INVALID_STATE;

    return rmStatus;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);

    return kvgpumgrSetVgpuEncoderCapacity(pGpu, pEncoderParams->vgpuUuid, pEncoderParams->encoderCapacity);
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pMigrationCapParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    NvU32 data = 0;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (pHypervisor == NULL)
        return NV_ERR_INVALID_REQUEST;

    if (pGpu == NULL)
        return NV_ERR_INVALID_STATE;

    if (osIsVgpuVfioPresent() == NV_OK)
    {
        if (NV_OK == osReadRegistryDword(pGpu,
                                         NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION,
                                         &data))
        {
            if (data == NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION_FALSE)
            {
                pMigrationCapParams->bMigrationCap = NV_FALSE;
            }
            else
            {
                pMigrationCapParams->bMigrationCap = kvgpumgrCheckPgpuMigrationSupport(pGpu);
            }
        }
        else // Regkey isn't set explicitly, check for default value
        {
            pMigrationCapParams->bMigrationCap = ((NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION_DEFAULT ==
                                                  NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION_TRUE) &&
                                                 kvgpumgrCheckPgpuMigrationSupport(pGpu));
        }
    }
    else
        pMigrationCapParams->bMigrationCap = kvgpumgrCheckPgpuMigrationSupport(pGpu);

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    NvU32 written_bytes = 0;
    NvU32 total_written_bytes = 0;
    NvBool pgpuMigrationCap = NV_FALSE;
    NvU8  *pGidString = NULL;
    NvU32  flags = 0;
    NvU32  gidStrlen;
    NV_STATUS rmStatus = NV_OK;

    if (pHypervisor == NULL)
        return NV_ERR_INVALID_REQUEST;

    if (pGpu == NULL)
        return NV_ERR_INVALID_STATE;

    portMemSet(pGpuMetadataStringParams->pGpuString, 0,
             NVA081_PGPU_METADATA_STRING_SIZE);

    pgpuMigrationCap = kvgpumgrCheckPgpuMigrationSupport(pGpu);

    if (pgpuMigrationCap == NV_FALSE)
    {
        // Bug 2063867
        // For migration unsupported GPUs, get the GPU's UUID
        // and save it in the string.
        // To perform offline compatibility checks for vGPU migration,
        // when the strings are compared, there is mismatch and migration
        // can be avoided for these migration unsupported GPUs.
        rmStatus = gpuGetGidInfo(pGpu, &pGidString, &gidStrlen, flags);
        if (rmStatus != NV_OK)
            return rmStatus;

        if (NVA081_PGPU_METADATA_STRING_SIZE >= gidStrlen)
        {
            portMemCopy(pGpuMetadataStringParams->pGpuString, gidStrlen, pGidString, gidStrlen);
        }
        else
        {
            rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
            portMemFree(pGidString);
            return rmStatus;
        }
        portMemFree(pGidString);
    }
    else
    {
        written_bytes = kvgpumgrGetPgpuDevIdEncoding(pGpu,
                            pGpuMetadataStringParams->pGpuString,
                            NVA081_PGPU_METADATA_STRING_SIZE);

        if (written_bytes == NV_U32_MAX)
            return NV_ERR_INSUFFICIENT_RESOURCES;
        total_written_bytes += written_bytes;

        written_bytes = kvgpumgrGetPgpuSubdevIdEncoding(pGpu,
                            pGpuMetadataStringParams->pGpuString + total_written_bytes,
                            NVA081_PGPU_METADATA_STRING_SIZE - total_written_bytes);

        if (written_bytes == NV_U32_MAX)
            return NV_ERR_INSUFFICIENT_RESOURCES;
        total_written_bytes += written_bytes;


        /*
         * Dynamic Floor Sweeping support for hopper+ allows migration on GPUs
         * with different floor sweep. Floor sweep info encoding for checking
         * migration capability is not needed hopper+.
         */
        if (!IsdHOPPERorBetter(pGpu))
        {
            written_bytes = kvgpumgrGetPgpuFSEncoding(pGpu,
                                pGpuMetadataStringParams->pGpuString + total_written_bytes,
                                NVA081_PGPU_METADATA_STRING_SIZE - total_written_bytes);

            if (written_bytes == NV_U32_MAX)
                return NV_ERR_INSUFFICIENT_RESOURCES;
            total_written_bytes += written_bytes;
        }


        written_bytes = kvgpumgrGetPgpuCapEncoding(pGpu,
                                                  pGpuMetadataStringParams->pGpuString + total_written_bytes,
                                                  NVA081_PGPU_METADATA_STRING_SIZE - total_written_bytes);

        if (written_bytes == NV_U32_MAX)
            return NV_ERR_INSUFFICIENT_RESOURCES;
        total_written_bytes += written_bytes;
    }

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams
)
{
    OBJGPU        *pGpu                     = GPU_RES_GET_GPU(pVgpuConfigApi);
    MemoryManager *pMemoryManager           = GPU_GET_MEMORY_MANAGER(pGpu);

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (pParams->vgpuTypeId == NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE)
    {
        pParams->hostReservedFb     = 0;
        pParams->eccAndPrReservedFb = 0;
        pParams->totalReservedFb    = 0;

        return NV_ERR_INVALID_ARGUMENT;
    }

    pParams->hostReservedFb = memmgrGetVgpuHostRmReservedFb_HAL(pGpu, pMemoryManager, pParams->vgpuTypeId);

    pParams->eccAndPrReservedFb = kvgpumgrGetEccAndPrReservedFb(pGpu);

    pParams->totalReservedFb = pParams->hostReservedFb + pParams->eccAndPrReservedFb;

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams
)
{

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams
)
{
    OBJGPU *pGpu                         = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys                         = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr        = SYS_GET_KERNEL_VGPUMGR(pSys);
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
    {
         return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);

    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       IS_MIG_IN_USE(pGpu),
                       NV_ERR_INVALID_OPERATION);

    if (osIsVgpuVfioPresent() == NV_OK)
    {
        for (pRequestVgpu = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
             pRequestVgpu != NULL;
             pRequestVgpu = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpu))
        {
            /* Check for VF's bdf */
            if (pRequestVgpu->gpuPciBdf == pParams->gpuPciId)
                break;
        }
        if (pRequestVgpu == NULL)
        {
            return NV_ERR_OBJECT_NOT_FOUND;
        }

        pParams->swizzId = pRequestVgpu->swizzId;
    }
    else
    {
        NvU32 partitionFlag = PARTITIONID_INVALID;

        NV_ASSERT_OK_OR_RETURN(
            kvgpumgrGetPartitionFlag(pParams->vgpuTypeId, &partitionFlag));

        NV_ASSERT_OK_OR_RETURN(
            kvgpumgrGetSwizzId(pGpu, pPhysGpuInfo, partitionFlag, &pParams->swizzId));
    }

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu                         = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys                         = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr        = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
    {
         return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);
    pParams->fractionalmultiVgpuSupported = pPhysGpuInfo->fractionalMultiVgpu;

    // We are not currently limiting the feature based on the pgpu.
    // Return the system level value here.
    pParams->heterogeneousTimesliceProfilesSupported = kvgpumgrIsHeterogeneousVgpuSupported();

    pParams->heterogeneousTimesliceSizesSupported = pPhysGpuInfo->heterogeneousTimesliceSizesSupported;

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    pParams->streamingCapability = NV_FALSE;

    if (gpuIsSriovEnabled(pGpu))
    {
        pParams->streamingCapability = NV_TRUE;
    }

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams
)
{
    pParams->heterogeneousMultiVgpuSupported    = kvgpumgrIsHeterogeneousVgpuSupported();
    pParams->warmUpdateSupported                = kvgpumgrIsVgpuWarmUpdateSupported();

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu                         = GPU_RES_GET_GPU(pVgpuConfigApi);
    OBJSYS *pSys                         = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr        = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
    {
         return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);
    pPhysGpuInfo->fractionalMultiVgpu = pParams->fractionalMultiVgpu;

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams
)
{
    OBJGPU *pGpu                         = GPU_RES_GET_GPU(pVgpuConfigApi);

    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       IS_MIG_IN_USE(pGpu),
                       NV_ERR_INVALID_OPERATION);

    NV_ASSERT_OK_OR_RETURN(
        kvgpumgrValidateSwizzId(pGpu, pParams->vgpuTypeId, pParams->swizzId));

    return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_CONFIG_UPDATE_HETEROGENEOUS_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    NV_STATUS rmStatus = NV_OK;

    if (pGpu == NULL)
        return NV_ERR_INVALID_STATE;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE))
    {
        pParams->isHeterogeneousEnabled = NV_FALSE;
        pParams->placementId = NVA081_PLACEMENT_ID_INVALID;
        return NV_OK;
    }

    pParams->isHeterogeneousEnabled = NV_TRUE;

    rmStatus = kvgpumgrUpdateHeterogeneousInfo(pGpu, pParams->vgpuTypeId,
                                               &pParams->placementId,
                                               &pParams->guestFbLength,
                                               &pParams->guestFbOffset,
                                               &pParams->gspHeapOffset);
    if (rmStatus != NV_OK)
        return rmStatus;

   return NV_OK;
}

NV_STATUS
vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL
(
    VgpuConfigApi *pVgpuConfigApi,
    NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuConfigApi);
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = NULL;
    NV_STATUS status;

    status = kvgpumgrGetHostVgpuDeviceFromMdevUuid(pGpu->gpuId,
                                                  pParams->vgpuName,
                                                  &pKernelHostVgpuDevice);
    if (status != NV_OK)
        return status;

    portStringCopy((char *)pKernelHostVgpuDevice->vgpuGuest->guestVmInfo.vmName,
                   NVA081_VM_NAME_SIZE, (const char *)pParams->vmName, NVA081_VM_NAME_SIZE);

    return NV_OK;
}

