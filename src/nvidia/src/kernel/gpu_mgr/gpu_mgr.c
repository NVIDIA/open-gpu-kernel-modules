/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*         GPU Manager                                                       *
\***************************************************************************/



#include "core/system.h"
#include "core/locks.h"
#include "platform/sli/sli.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/device/device.h"
#include "gpu/gpu.h"
#include "tls/tls.h"
#include "nvrm_registry.h"
#include "nv_ref.h"
#include "nvlimits.h"
#include "nv-firmware-registry.h"

#include "kernel/gpu/intr/intr.h"
#include "vgpu/sdk-structures.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "platform/chipset/chipset.h"
#include "published/pcie_switch/pcie_switch_ref.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/conf_compute/conf_compute.h"
#include "gpu/gpu_fabric_probe.h"
#include "gpu/mig_mgr/gpu_instance_subscription.h"
#include "ctrl/ctrlc56f.h"

// local static funcs
static void   gpumgrSetAttachInfo(OBJGPU *, GPUATTACHARG *);
static void   gpumgrGetGpuHalFactor(NvU32 *pChipId0, NvU32 *pChipId1, NvU32 *pSocChipId0, RM_RUNTIME_VARIANT *pRmVariant, TEGRA_CHIP_TYPE *pTegraType, GPUATTACHARG *pAttachArg);
static NvBool _gpumgrGetPcieP2PCapsFromCache(NvU32 gpuMask, NvU8* pP2PWriteCapsStatus, NvU8* pP2PReadCapsStatus);

static void
_gpumgrUnregisterRmCapsForGpuUnderLock(NvU64 gpuDomainBusDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    // SMC partition caps must be destroyed before GPU caps.
    gpumgrUnregisterRmCapsForMIGGI(gpuDomainBusDevice);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        PROBEDGPU *pProbedGpu = &pGpuMgr->probedGpus[i];

        if (pProbedGpu->gpuDomainBusDevice == gpuDomainBusDevice &&
            pProbedGpu->gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            osRmCapUnregister(&pProbedGpu->pOsRmCaps);
            break;
        }
    }
}

static void
_gpumgrUnregisterRmCapsForGpu(NvU64 gpuDomainBusDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);
    _gpumgrUnregisterRmCapsForGpuUnderLock(gpuDomainBusDevice);
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
}

static NV_STATUS
_gpumgrRegisterRmCapsForGpu(OBJGPU *pGpu)
{
    NV_STATUS status = NV_ERR_INVALID_STATE;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        PROBEDGPU *pProbedGpu = &pGpuMgr->probedGpus[i];

        if (((pProbedGpu->gpuDomainBusDevice == 0) && pGpu->bIsSOC) ||
            (pProbedGpu->gpuDomainBusDevice == gpuGetDBDF(pGpu) &&
            pProbedGpu->gpuId != NV0000_CTRL_GPU_INVALID_ID))
        {
            if (pProbedGpu->pOsRmCaps == NULL)
            {
                status = osRmCapRegisterGpu(pGpu->pOsGpuInfo,
                                            &pProbedGpu->pOsRmCaps);
            }
            else
            {
                status = NV_OK;
            }

            pGpu->pOsRmCaps = pProbedGpu->pOsRmCaps;
            break;
        }
    }

    NV_ASSERT(status == NV_OK);

    portSyncMutexRelease(pGpuMgr->probedGpusLock);

    return status;
}

static NV_STATUS
_gpumgrDetermineConfComputeCapabilities
(
    OBJGPUMGR *pGpuMgr,
    OBJGPU    *pGpu
)
{
    NvBool bCCFeatureEnabled = NV_FALSE;
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
    bCCFeatureEnabled = (pCC != NULL) &&
                         pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_ENABLED);

    // First GPU
    if (ONEBITSET(pGpuMgr->gpuAttachMask))
    {
        pGpuMgr->ccCaps.bApmFeatureCapable = NV_FALSE;
        pGpuMgr->ccCaps.bHccFeatureCapable = pGpu->getProperty(pGpu, PDB_PROP_GPU_CC_FEATURE_CAPABLE);
        pGpuMgr->ccCaps.bCCFeatureEnabled = bCCFeatureEnabled;
        pGpuMgr->ccCaps.bDevToolsModeEnabled = gpuIsCCDevToolsModeEnabled(pGpu);
        pGpuMgr->ccCaps.bMultiGpuProtectedPcieModeEnabled = gpuIsCCMultiGpuProtectedPcieModeEnabled(pGpu);
        pGpuMgr->ccCaps.bMultiGpuNvleModeEnabled = gpuIsCCMultiGpuNvleModeEnabled(pGpu);

        if (pGpuMgr->ccCaps.bDevToolsModeEnabled)
        {
            pGpuMgr->ccCaps.bAcceptClientRequest = NV_TRUE;
        }
        else
        {
            pGpuMgr->ccCaps.bAcceptClientRequest = NV_FALSE;
        }
    }
    else
    {
        NV_ASSERT_OR_RETURN(pGpuMgr->ccCaps.bCCFeatureEnabled ==
            bCCFeatureEnabled, NV_ERR_INVALID_STATE);
        if (pGpuMgr->ccCaps.bCCFeatureEnabled)
        {
            NV_ASSERT_OR_RETURN(pGpuMgr->ccCaps.bHccFeatureCapable ==
                pGpu->getProperty(pGpu, PDB_PROP_GPU_CC_FEATURE_CAPABLE),
                NV_ERR_INVALID_STATE);
        }
        //
        // If one of the GPUs is not Hopper CC capable, the system as a whole
        // is not Hopper CC capable
        //
        pGpuMgr->ccCaps.bHccFeatureCapable &=
            pGpu->getProperty(pGpu, PDB_PROP_GPU_CC_FEATURE_CAPABLE);

        NV_ASSERT_OR_RETURN(pGpuMgr->ccCaps.bDevToolsModeEnabled ==
            gpuIsCCDevToolsModeEnabled(pGpu),
            NV_ERR_INVALID_STATE);

        NV_ASSERT_OR_RETURN((pGpuMgr->ccCaps.bMultiGpuProtectedPcieModeEnabled ==
            gpuIsCCMultiGpuProtectedPcieModeEnabled(pGpu) || pGpuMgr->ccCaps.bMultiGpuNvleModeEnabled ==
            gpuIsCCMultiGpuNvleModeEnabled(pGpu)),
            NV_ERR_INVALID_STATE);
    }

    return NV_OK;
}

static NV_STATUS
_gpumgrDetermineNvlinkEncryptionCapabilities
(
    OBJGPUMGR *pGpuMgr,
    OBJGPU    *pGpu
)
{
    NvBool bNvlEncryptionEnabled = NV_FALSE;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    bNvlEncryptionEnabled = (pKernelNvlink != NULL) &&
                             pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED);

    // First GPU
    if (ONEBITSET(pGpuMgr->gpuAttachMask))
    {
        pGpuMgr->nvleCaps.bNvlEncryptionEnabled = bNvlEncryptionEnabled;
    }
    else
    {
        //
        // If one of the GPUs is not NVLE capable, the system as a whole
        // is not NVLE capable
        //
        NV_ASSERT_OR_RETURN(pGpuMgr->nvleCaps.bNvlEncryptionEnabled ==
            bNvlEncryptionEnabled, NV_ERR_INVALID_STATE);
    }

    return NV_OK;
}

static void
_gpumgrCacheClearMIGGpuIdInfo(NvU32 gpuId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    portSyncRwLockAcquireWrite(pGpuMgr->cachedMIGInfoLock);

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (pGpuMgr->cachedMIGInfo[i].gpuId == gpuId)
        {
            portMemSet(&pGpuMgr->cachedMIGInfo[i], 0x0, sizeof(pGpuMgr->cachedMIGInfo[i]));
            break;
        }
    }

    portSyncRwLockReleaseWrite(pGpuMgr->cachedMIGInfoLock);
}

//
// ODB functions
//
NV_STATUS
gpumgrConstruct_IMPL(OBJGPUMGR *pGpuMgr)
{
    NvU32 i;

    NV_PRINTF(LEVEL_INFO, "gpumgrConstruct\n");

    pGpuMgr->numGpuHandles = 0;

    for (i = 0; i < NV_MAX_DEVICES; i++)
        pGpuMgr->gpuHandleIDList[i].gpuInstance  = NV_MAX_DEVICES;

    pGpuMgr->probedGpusLock = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());

    if (pGpuMgr->probedGpusLock == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        portMemSet(&pGpuMgr->probedGpus[i], 0, sizeof(PROBEDGPU));
        pGpuMgr->probedGpus[i].gpuId = NV0000_CTRL_GPU_INVALID_ID;
    }

    pGpuMgr->gpuAttachCount = 0;
    pGpuMgr->gpuAttachMask = 0;

    pGpuMgr->deviceCount = 0;
    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->pGpuGrpTable); i++)
    {
        pGpuMgr->pGpuGrpTable[i] = NULL;
    }

    pGpuMgr->powerDisconnectedGpuCount = 0;

    NV_ASSERT_OK_OR_RETURN(gpumgrInitPcieP2PCapsCache(pGpuMgr));

    pGpuMgr->cachedMIGInfoLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());
    NV_ASSERT_OR_RETURN(pGpuMgr->cachedMIGInfoLock != NULL,
                        NV_ERR_INSUFFICIENT_RESOURCES);

    portMemSet(pGpuMgr->cachedMIGInfo, 0, sizeof(pGpuMgr->cachedMIGInfo));

    pGpuMgr->ccAttackerAdvantage = SECURITY_POLICY_ATTACKER_ADVANTAGE_DEFAULT;

    return NV_OK;
}


void
gpumgrDestruct_IMPL(OBJGPUMGR *pGpuMgr)
{
    NV_PRINTF(LEVEL_INFO, "gpumgrDestruct\n");

    portSyncMutexDestroy(pGpuMgr->probedGpusLock);

    portSyncRwLockDestroy(pGpuMgr->cachedMIGInfoLock);

    gpumgrDestroyPcieP2PCapsCache(pGpuMgr);
}

static NvBool
_gpumgrThreadHasExpandedGpuVisibilityInTls(void)
{
    NvP64 entry = tlsEntryGet(TLS_ENTRY_ID_GPUMGR_EXPANDED_GPU_VISIBILITY);
    return (entry != NvP64_NULL) && (entry == ((NvP64) 1));
}

NvBool
gpumgrThreadHasExpandedGpuVisibility(void)
{
    if (RMCFG_FEATURE_PLATFORM_UNIX)
    {
        return _gpumgrThreadHasExpandedGpuVisibilityInTls();
    }
    else
    {
        //
        // Bug 4376209
        // Non-UNIX platforms have expanded GPU visibility by default for now
        // (this is OK as they do not have parallel init yet).
        //
        return NV_TRUE;
    }
}

NV_STATUS
gpumgrThreadEnableExpandedGpuVisibility(void)
{
    NvP64 *pEntry = tlsEntryAcquire(TLS_ENTRY_ID_GPUMGR_EXPANDED_GPU_VISIBILITY);
    if (pEntry == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    *pEntry = (NvP64) 1;
    return NV_OK;
}

void
gpumgrThreadDisableExpandedGpuVisibility(void)
{
    if (_gpumgrThreadHasExpandedGpuVisibilityInTls())
    {
        NvU32 refCount = tlsEntryRelease(TLS_ENTRY_ID_GPUMGR_EXPANDED_GPU_VISIBILITY);
        NV_ASSERT(refCount == 0);
    }
}

//
// gpumgrAllocGpuInstance
//
// This interface returns the next available gpu number.
//
NV_STATUS
gpumgrAllocGpuInstance(NvU32 *pGpuInstance)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    NvU32 i;
    NvU64 availableIDs = ((1ULL << NV_MAX_DEVICES) - 1);

    if (pGpuMgr->numGpuHandles == 0)
    {
        *pGpuInstance = 0;
        return NV_OK;
    }
    else if (pGpuMgr->numGpuHandles == NV_MAX_DEVICES)
    {
        *pGpuInstance = NV_MAX_DEVICES;
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    for (i = 0; i < pGpuMgr->numGpuHandles; i++)
        availableIDs &= ~NVBIT(pGpuMgr->gpuHandleIDList[i].gpuInstance);

    for (i = 0; ((availableIDs & (1ULL << i)) == 0); i++)
        ;

    *pGpuInstance = i;

    return NV_OK;
}

//
// During destruction of a GPU the handle list needs to be modified.
// Since we cannot guarantee the _last_ GPU will always be the one
// destroyed we have to compact the handle list so we have no gaps
// and can simply decrement numGpuHandles.
//
static void
_gpumgrShiftDownGpuHandles(NvU32 startIndex)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i, lastMovedIndex = startIndex;

    for (i = startIndex; i < (NV_MAX_DEVICES - 1); i++)
    {
        if (pGpuMgr->gpuHandleIDList[i + 1].pGpu != 0)
        {
            lastMovedIndex = i + 1;
            pGpuMgr->gpuHandleIDList[i].gpuInstance =
                pGpuMgr->gpuHandleIDList[i + 1].gpuInstance;

            pGpuMgr->gpuHandleIDList[i].pGpu =
                pGpuMgr->gpuHandleIDList[i + 1].pGpu;
        }
    }

    pGpuMgr->gpuHandleIDList[lastMovedIndex].gpuInstance     = NV_MAX_DEVICES;
    pGpuMgr->gpuHandleIDList[lastMovedIndex].pGpu            = reinterpretCast(NULL, OBJGPU *);
    pGpuMgr->numGpuHandles--;
}

static void
_gpumgrDestroyGpu(NvU32 gpuInstance)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPU    *pGpu;
    NvU32      i;

    osSyncWithGpuDestroy(NV_TRUE);

    pGpu = gpumgrGetGpu(gpuInstance);

    objDelete(pGpu);

    for (i = 0; i < pGpuMgr->numGpuHandles; i++)
    {
        if (pGpuMgr->gpuHandleIDList[i].gpuInstance == gpuInstance)
        {
            pGpuMgr->gpuHandleIDList[i].gpuInstance     = NV_MAX_DEVICES;
            pGpuMgr->gpuHandleIDList[i].pGpu            = reinterpretCast(NULL, OBJGPU *);
            _gpumgrShiftDownGpuHandles(i);
            break;
        }
    }

    osSyncWithGpuDestroy(NV_FALSE);
}

POBJGPU
gpumgrGetGpu(NvU32 gpuInstance)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    for (i = 0; i < pGpuMgr->numGpuHandles; i++)
    {
        if (pGpuMgr->gpuHandleIDList[i].gpuInstance == gpuInstance)
        {
            OBJGPU *pGpu = pGpuMgr->gpuHandleIDList[i].pGpu;
            if (pGpu != NULL)
            {
                if (gpumgrThreadHasExpandedGpuVisibility() ||
                    pGpu->getProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED))
                {
                    return pGpu;
                }
            }
        }
    }

    return NULL;
}

POBJGPU
gpumgrGetSomeGpu(void)
{
    OBJGPU *pGpu        = NULL;
    NvU32   gpuMask     = 0;
    NvU32   gpuIndex    = 0;
    NvU32   gpuCount    = 0;

    // Get some gpu to get the SLI Display Parent
    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);
    pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);

    if (pGpu == NULL)
    {
        // None of the GPUs are initialized - Too early
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to retrieve pGpu - Too early call!.\n");
        NV_ASSERT(NV_FALSE);
        return pGpu;
    }
    return pGpu;
}


//
// gpumgrAllocDeviceInstance
//
// This interface returns the next available broadcast device number.
// This broadcast device number is used to uniquely identify this set
// of gpu(s) both internally in the RM (e.g. OBJGPUGRP handle) as well
// as via the architecture (e.g., for the 'deviceId' parameter of
// NV0080_ALLOC_PARAMETERS).
//
NV_STATUS
gpumgrAllocDeviceInstance(NvU32 *pDeviceInstance)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->pGpuGrpTable); i++)
        if (pGpuMgr->pGpuGrpTable[i] == NULL)
            break;

    if (i == NV_MAX_DEVICES)
    {
        *pDeviceInstance = NV_MAX_DEVICES;
        return NV_ERR_GENERIC;
    }

    *pDeviceInstance = i;

    return NV_OK;
}

//
// gpumgrGetGpuAttachInfo
//
// Returns current gpu attach info.
//
NV_STATUS
gpumgrGetGpuAttachInfo(NvU32 *pGpuCnt, NvU32 *pGpuMask)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    NvU32 outCount = 0;
    NvU32 outMask = 0;

    NvU32 gpuMask = pGpuMgr->gpuAttachMask;
    NvU32 gpuInstance = 0;

    while (gpuInstance != NV_MAX_DEVICES)
    {
        OBJGPU *pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
        if (pGpu == NULL)
            continue;

        if (gpumgrThreadHasExpandedGpuVisibility() ||
            pGpu->getProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED))
        {
            // adjust gpuInstance down one as gpumgrGetNextGpu increments one
            outMask |= NVBIT(gpuInstance - 1);
            outCount++;
        }
    }

    // caller can pass in NULL for outparams that it doesn't need.
    if (pGpuCnt != NULL)  *pGpuCnt  = outCount;
    if (pGpuMask != NULL) *pGpuMask = outMask;

    return NV_OK;
}

NvU32
gpumgrGetDeviceGpuMask(NvU32 deviceInstance)
{
    OBJGPUGRP  *pGpuGrp = gpumgrGetGpuGrpFromInstance(deviceInstance);

    if (pGpuGrp == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Could not find GPU Group for deviceInstance 0x%x!\n",
                  deviceInstance);
        return 0;
    }

    return gpugrpGetGpuMask(pGpuGrp);
}

NV_STATUS
gpumgrIsDeviceInstanceValid(NvU32 deviceInstance)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPUGRP *pGpuGrp = NULL;

    if (deviceInstance >= NV_MAX_DEVICES)
        return NV_ERR_INVALID_ARGUMENT;

    pGpuGrp = pGpuMgr->pGpuGrpTable[deviceInstance];
    if (NULL == pGpuGrp)
        return NV_ERR_INVALID_DATA;

    if (0 == gpugrpGetGpuMask(pGpuGrp))
        return NV_ERR_INVALID_ARGUMENT;

    return NV_OK;
}

NvBool
gpumgrIsSubDeviceInstanceValid(NvU32 subDeviceInstance)
{
    if (subDeviceInstance >= NV2080_MAX_SUBDEVICES)
        return NV_FALSE;

    return NV_TRUE;
}

NvU32 gpumgrGetPrimaryForDevice(NvU32 deviceInstance)
{
    NvU32 gpuMask, gpuInstance = 0;
    OBJGPU *pGpu = NULL;

    gpuMask = gpumgrGetDeviceGpuMask(deviceInstance);

    if (gpumgrIsSubDeviceCountOne(gpuMask))
    {
        pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
        if (pGpu != NULL)
        {
            return pGpu->gpuInstance;
        }
    }
    else
    {
        while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
        {
            if (gpumgrIsParentGPU(pGpu))
            {
                return pGpu->gpuInstance;
            }
        }
    }

    NV_PRINTF(LEVEL_ERROR,
             "deviceInstance 0x%x does not exist!\n", deviceInstance);

    return 0;           // this should not happen, never
}

NvBool
gpumgrIsDeviceEnabled(NvU32 deviceInstance)
{
    NvU32   gpuMask, gpuInstance = 0;
    NvBool  bEnabled;

    gpuMask = gpumgrGetDeviceGpuMask(deviceInstance);

    if (gpuMask == 0)
    {
        return NV_FALSE;
    }
    /*
     * Check if this device
     * - has been disabled via Power-SLI
     * - is in the "drain" state
     */
    if (gpumgrIsSubDeviceCountOne(gpuMask))
    {
        OBJGPU *pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);

        if (pGpu == NULL)
            return NV_FALSE;

        if ((gpumgrQueryGpuDrainState(pGpu->gpuId, &bEnabled, NULL) == NV_OK)
            && bEnabled)
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

//
// gpumgrRegisterGpuId
//
// This interface is used by os-dependent code to insert a probed
// gpu into the table of probed gpus known to the RM.
//
NV_STATUS
gpumgrRegisterGpuId(NvU32 gpuId, NvU64 gpuDomainBusDevice)
{
    NV_STATUS status = NV_ERR_INSUFFICIENT_RESOURCES;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        if (pGpuMgr->probedGpus[i].gpuId == gpuId)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU id 0x%x already registered at index %u\n",
                      gpuId, i);

            // Duplicate gpu
            status = NV_ERR_IN_USE;
            goto done;
        }

        if (pGpuMgr->probedGpus[i].gpuId == NV0000_CTRL_GPU_INVALID_ID)
        {
            pGpuMgr->probedGpus[i].gpuId = gpuId;
            pGpuMgr->probedGpus[i].gpuDomainBusDevice = gpuDomainBusDevice;
            pGpuMgr->probedGpus[i].bInitAttempted = NV_FALSE;
            pGpuMgr->probedGpus[i].bExcluded = NV_FALSE;
            pGpuMgr->probedGpus[i].bUuidValid = NV_FALSE;
            pGpuMgr->probedGpus[i].pOsRmCaps = NULL;
            status = NV_OK;
            goto done;
        }
    }

done:
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return status;
}

//
// gpumgrUnregisterGpuId
//
// This interface is used by os-dependent code to remove a gpu
// from the table of probed gpus known to the RM.
//
NV_STATUS
gpumgrUnregisterGpuId(NvU32 gpuId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        PROBEDGPU *pProbedGpu = &pGpuMgr->probedGpus[i];

        if (pProbedGpu->gpuId == gpuId)
        {
            _gpumgrCacheClearMIGGpuIdInfo(gpuId);
            gpumgrRemovePcieP2PCapsFromCache(pProbedGpu->gpuId);
            _gpumgrUnregisterRmCapsForGpuUnderLock(pProbedGpu->gpuDomainBusDevice);
            pProbedGpu->gpuId = NV0000_CTRL_GPU_INVALID_ID;
            pProbedGpu->bDrainState = NV_FALSE;
            pProbedGpu->bRemoveIdle = NV_FALSE;
            pProbedGpu->bExcluded = NV_FALSE;
            pProbedGpu->bUuidValid = NV_FALSE;
            goto done;
        }
    }

done:
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return NV_OK;
}

//
// gpumgrExcludeGpuId
//
// This interface is used by os-dependent code to 'exclude' a gpu.
//
// gpuId: the device to exclude
//
NV_STATUS
gpumgrExcludeGpuId(NvU32 gpuId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        PROBEDGPU *pProbedGpu = &pGpuMgr->probedGpus[i];

        if (pProbedGpu->gpuId == gpuId)
        {
            pProbedGpu->bExcluded = NV_TRUE;
            goto done;
        }
    }

done:
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return NV_OK;
}

//
// gpumgrSetUuid
//
// This interface is used by os-dependent code to pass the UUID for a gpu.
// The UUID is a 16-byte raw UUID/GID.
//
NV_STATUS
gpumgrSetUuid(NvU32 gpuId, NvU8 *uuid)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    if (uuid == NULL)
        return NV_ERR_INVALID_DATA;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        PROBEDGPU *pProbedGpu = &pGpuMgr->probedGpus[i];

        if (pProbedGpu->gpuId == gpuId)
        {
            portMemCopy(pProbedGpu->uuid, RM_SHA1_GID_SIZE, uuid, RM_SHA1_GID_SIZE);
            pProbedGpu->bUuidValid = NV_TRUE;
            goto done;
        }
    }

done:
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return NV_OK;
}

//
// gpumgrGetCachedUuid
//
// Lookup the cached UUID for a GPU
//
static NV_STATUS
gpumgrGetCachedUuid(NvU32 gpuId, NvU8 *uuid, unsigned int len)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;

    if (uuid == NULL || len < RM_SHA1_GID_SIZE)
        return NV_ERR_INVALID_ARGUMENT;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        PROBEDGPU *pProbedGpu = &pGpuMgr->probedGpus[i];

        if (pProbedGpu->gpuId == gpuId)
        {
            if (pProbedGpu->bUuidValid)
            {
                portMemCopy(uuid, RM_SHA1_GID_SIZE, pProbedGpu->uuid, RM_SHA1_GID_SIZE);
                status = NV_OK;
            }
            else
                status = NV_ERR_NOT_SUPPORTED;
            goto done;
        }
    }

done:
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return status;
}

NV_STATUS
gpumgrGetGpuUuidInfo(NvU32 gpuId, NvU8 **ppUuidStr, NvU32 *pUuidStrLen, NvU32 uuidFlags)
{
    NvU8 *pUuid;
    NV_STATUS status;

    if (ppUuidStr == NULL || pUuidStrLen == NULL)
        return NV_ERR_INVALID_DATA;

    // gpumgr only supports SHA1 format; error out if requesting otherwise
    if (!FLD_TEST_DRF(0000_CTRL_CMD, _GPU_GET_UUID_FROM_GPU_ID_FLAGS, _TYPE, _SHA1, uuidFlags))
        return NV_ERR_INVALID_ARGUMENT;

    pUuid = portMemAllocNonPaged(RM_SHA1_GID_SIZE);
    if (pUuid == NULL)
        return NV_ERR_NO_MEMORY;

    status = gpumgrGetCachedUuid(gpuId, pUuid, RM_SHA1_GID_SIZE);
    if (status != NV_OK)
    {
        portMemFree(pUuid);
        return status;
    }

    if (FLD_TEST_DRF(0000_CTRL_CMD, _GPU_GET_UUID_FROM_GPU_ID_FLAGS, _FORMAT, _BINARY, uuidFlags))
    {
        // Binary case - pUuid is freed by the caller
        *ppUuidStr = pUuid;
        *pUuidStrLen = RM_SHA1_GID_SIZE;
    }
    else
    {
        // Conversion to ASCII or UNICODE
        status = transformGidToUserFriendlyString(pUuid, RM_SHA1_GID_SIZE,
                                                    ppUuidStr, pUuidStrLen, uuidFlags, RM_UUID_PREFIX_GPU);
        portMemFree(pUuid);
    }

    return status;
}

static NV_STATUS
gpumgrGetRegisteredIds
(
    NvU64 gpuDomainBusDevice,
    NvU32 *pGpuId,
    NvUuid *pUuid,
    NvBool *pbUuidValid
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        PROBEDGPU *pProbedGpu = &pGpuMgr->probedGpus[i];
        if (pProbedGpu->gpuId == NV0000_CTRL_GPU_INVALID_ID)
            continue;

        if (pProbedGpu->gpuDomainBusDevice == gpuDomainBusDevice)
        {
            *pGpuId = pProbedGpu->gpuId;
            *pbUuidValid = pProbedGpu->bUuidValid;
            if (pProbedGpu->bUuidValid)
                portMemCopy(pUuid->uuid, sizeof(pUuid->uuid),
                            pProbedGpu->uuid, sizeof(pProbedGpu->uuid));

            status = NV_OK;
            break;
        }
    }

    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return status;
}

static void
gpumgrGetGpuHalFactorOfVirtual(NvBool *pIsVirtual, GPUATTACHARG *pAttachArg)
{
    DEVICE_MAPPING gpuDevMapping = {0};
    NvU32  pmcBoot1;

    gpuDevMapping.gpuNvAddr = pAttachArg->regBaseAddr;
    gpuDevMapping.gpuNvLength = pAttachArg->regLength;

    *pIsVirtual = NV_FALSE;

    // Check register NV_PMC_BOOT_1
    pmcBoot1 = osDevReadReg032(/*pGpu=*/ NULL, &gpuDevMapping, NV_PMC_BOOT_1);

    // VGPU with SRIOV
    if (FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _VF, pmcBoot1))
    {
        *pIsVirtual = NV_TRUE;
    }
}

NvBool gpumgrGetRmFirmwareLogsEnabled
(
    NvU32   enableFirmwareLogsRegVal
)
{
    // Check for logs
    if (enableFirmwareLogsRegVal == NV_REG_ENABLE_GPU_FIRMWARE_LOGS_ENABLE)
    {
        return NV_TRUE;
    }
#if defined(DEBUG) || defined(DEVELOP)
    if (enableFirmwareLogsRegVal == NV_REG_ENABLE_GPU_FIRMWARE_LOGS_ENABLE_ON_DEBUG)
    {
        return NV_TRUE;
    }
#endif // defined(DEBUG) || defined(DEVELOP)

    return NV_FALSE;
}

void gpumgrGetRmFirmwarePolicy
(
    NvU32   pmcBoot42,
    NvBool  bIsVirtualWithSriov,
    NvBool  bIsSoc,
    NvU32   enableFirmwareRegVal,
    NvBool *pbRequestFirmware,
    NvBool *pbAllowFallbackToMonolithicRm,
    WindowsFirmwarePolicyArg  *pWinRmFwPolicyArg
)
{
    NvBool bFirmwareCapable = NV_FALSE;
    NvBool bEnableByDefault = NV_FALSE;
    NvU32  regkeyFirmwareMode;

    regkeyFirmwareMode =
        enableFirmwareRegVal & NV_REG_ENABLE_GPU_FIRMWARE_MODE_MASK;
    *pbAllowFallbackToMonolithicRm =
        !!(enableFirmwareRegVal & NV_REG_ENABLE_GPU_FIRMWARE_POLICY_ALLOW_FALLBACK);

    bFirmwareCapable = gpumgrIsDeviceRmFirmwareCapable(pmcBoot42,
                                                       bIsVirtualWithSriov,
                                                       bIsSoc,
                                                       &bEnableByDefault,
                                                       pWinRmFwPolicyArg);

    *pbRequestFirmware =
        (bFirmwareCapable &&
         ((regkeyFirmwareMode == NV_REG_ENABLE_GPU_FIRMWARE_MODE_ENABLED) ||
          (bEnableByDefault && (regkeyFirmwareMode != NV_REG_ENABLE_GPU_FIRMWARE_MODE_DISABLED))));
}

static NvBool _gpumgrIsRmFirmwareCapableChip(NvU32 pmcBoot42)
{
    return (decodePmcBoot42Architecture(pmcBoot42) >= NV_PMC_BOOT_42_ARCHITECTURE_TU100);
}

NvBool gpumgrIsVgxRmFirmwareCapableChip(NvU32 pmcBoot42)
{
    return (decodePmcBoot42Architecture(pmcBoot42) >= NV_PMC_BOOT_42_ARCHITECTURE_GB100) ||
           (decodePmcBoot42Architecture(pmcBoot42) == NV_PMC_BOOT_42_ARCHITECTURE_GH100) ||
           (decodePmcBoot42Architecture(pmcBoot42) == NV_PMC_BOOT_42_ARCHITECTURE_AD100);
}

static NvBool _gpumgrIsVgxRmFirmwareDefaultChip(NvU32 pmcBoot42)
{
    return gpumgrIsVgxRmFirmwareCapableChip(pmcBoot42);
}

NvBool gpumgrIsDeviceRmFirmwareCapable
(
    NvU32 pmcBoot42,
    NvBool bIsVirtualWithSriov,
    NvBool bIsSoc,
    NvBool *pbEnabledByDefault,
    WindowsFirmwarePolicyArg  *pWinRmFwPolicyArg
)
{
    NvBool bEnabledByDefault = NV_FALSE;
    NvBool bFirmwareCapable  = NV_TRUE;

    // SoC is treated as always firmware capable and not enabled by default
    if (bIsSoc)
        goto finish;

    if (!hypervisorIsVgxHyper() && !_gpumgrIsRmFirmwareCapableChip(pmcBoot42))
    {
        bFirmwareCapable = NV_FALSE;
        goto finish;
    }
    else if (hypervisorIsVgxHyper() && !gpumgrIsVgxRmFirmwareCapableChip(pmcBoot42))
    {
        bFirmwareCapable = NV_FALSE;
        goto finish;
    }

#if NVCPU_IS_FAMILY_PPC
    // Disable default enablement for GSP on PowerPC until it is fully tested
    bEnabledByDefault = NV_FALSE;
    goto finish;
#endif

    if (hypervisorIsVgxHyper())
    {
        if (_gpumgrIsVgxRmFirmwareDefaultChip(pmcBoot42))
        {
            bEnabledByDefault = NV_TRUE;
        }
    }
    else
    {
        bEnabledByDefault = NV_TRUE;
    }

finish:
    if (pbEnabledByDefault != NULL)
    {
        *pbEnabledByDefault = bEnabledByDefault;
    }

    return bFirmwareCapable;
}

static NvBool gpumgrCheckRmFirmwarePolicy
(
    NvU64  nvDomainBusDeviceFunc,
    NvBool bRequestFwClientRm,
    NvU32  pmcBoot42
)
{

    if (!bRequestFwClientRm)
        return NV_FALSE;

    if (!_gpumgrIsRmFirmwareCapableChip(pmcBoot42))
    {
        NV_PRINTF(LEVEL_ERROR, "Disabling GSP offload -- GPU not supported\n");
        return NV_FALSE;
    }

    return NV_TRUE;
}

//
// gpumgrGetGpuHalFactor
//
// Get Gpu Hal factors those are used to init Hal binding
//
static void
gpumgrGetGpuHalFactor
(
    NvU32  *pChipId0,
    NvU32  *pChipId1,
    NvU32  *pSocChipId0,
    RM_RUNTIME_VARIANT *pRmVariant,
    TEGRA_CHIP_TYPE    *pTegraType,
    GPUATTACHARG *pAttachArg
)
{
    NvBool isVirtual;
    NvBool isFwClient;

    *pTegraType = TEGRA_CHIP_TYPE_DEFAULT;

    // get ChipId0 and ChipId1
    if (pAttachArg->socDeviceArgs.specified)
    {
        *pChipId0 = pAttachArg->socDeviceArgs.socChipId0;

        // iGPU has a PMC_BOOT_0, Display does not
        if (pAttachArg->socDeviceArgs.bIsIGPU)
        {
            DEVICE_MAPPING *pGpuDevMapping = NULL;
            pGpuDevMapping = &pAttachArg->socDeviceArgs.deviceMapping[DEVICE_INDEX_GPU];
            *pChipId0 = osDevReadReg032(/*pGpu=*/ NULL, pGpuDevMapping, NV_PMC_BOOT_0);
        }

        *pChipId1 = 0;
        *pSocChipId0 = pAttachArg->socDeviceArgs.socChipId0;
        isVirtual = NV_FALSE;
    }
    else if (pAttachArg->bIsSOC)
    {
        // This path is only taken for ARCH MODS iGPU verification.

        *pChipId0 = pAttachArg->socChipId0;
        *pChipId1 = 0;
        *pSocChipId0 = pAttachArg->socChipId0;
        isVirtual = NV_FALSE;
    }
    else
    {
        DEVICE_MAPPING  gpuDevMapping = {0};
        gpuDevMapping.gpuNvAddr = pAttachArg->regBaseAddr;
        gpuDevMapping.gpuNvLength = pAttachArg->regLength;

        //
        // PMC_BOOT_42 register is added above G94+ chips which is internal to NVIDIA
        // Earlier we used PMC_BOOT_0 as Internal ID which is now exposed to customers
        //
        *pChipId0 = osDevReadReg032(/*pGpu=*/ NULL, &gpuDevMapping, NV_PMC_BOOT_0);
        *pChipId1 = osDevReadReg032(/*pGpu=*/ NULL, &gpuDevMapping, NV_PMC_BOOT_42);
        *pSocChipId0 = 0;

        gpumgrGetGpuHalFactorOfVirtual(&isVirtual, pAttachArg);

        //
        // If socChipId0 has valid value, then running environment is SOCV.
        // The Tegra chip after Ampere arch is using PCIE interface which connects
        // iGPU to SoC for BAR and control accesses (interrupt).
        // The code between TEGRA_CHIP_TYPE_PCIE and TEGRA_CHIP_TYPE_SOC
        // shares same dGPU ARCH specific HAL mostly except manual differences due to
        // latency of manual updates between nvgpu (Standlone iGPU/Full Chip Verification)
        // and nvmobile (SOC) trees.
        //
        if (pAttachArg->socChipId0 != 0)
        {
            *pTegraType = TEGRA_CHIP_TYPE_SOC;
        }
    }

    isFwClient = gpumgrCheckRmFirmwarePolicy(pAttachArg->nvDomainBusDeviceFunc,
                                             pAttachArg->bRequestFwClientRm,
                                             *pChipId1);

    if (RMCFG_FEATURE_PLATFORM_GSP || RMCFG_FEATURE_PLATFORM_DCE)
        *pRmVariant = RM_RUNTIME_VARIANT_UCODE;
    else if (isVirtual)
        *pRmVariant = RM_RUNTIME_VARIANT_VF;
    else if (isFwClient)
        *pRmVariant = RM_RUNTIME_VARIANT_PF_KERNEL_ONLY;
    else
        *pRmVariant = RM_RUNTIME_VARIANT_PF_MONOLITHIC;    // default, monolithic mode

    NV_PRINTF(LEVEL_INFO,
        "ChipId0[0x%x] ChipId1[0x%x] SocChipId0[0x%x] isFwClient[%d] RmVariant[%d] tegraType[%d]\n",
        *pChipId0, *pChipId1, *pSocChipId0, isFwClient, *pRmVariant, *pTegraType);
}


//
// _gpumgrCreateGpu
//
// Former _sysCreateGpu().  The function is moved to Gpumgr for hinding struct
// GPUATTACHARG from SYS.  SYS is still the parent object of both GPUMGR and
// GPU.
//
static NV_STATUS
_gpumgrCreateGpu(NvU32 gpuInstance, GPUATTACHARG *pAttachArg)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPU    *pGpu;
    NV_STATUS  status;
    RM_RUNTIME_VARIANT  rmVariant;
    TEGRA_CHIP_TYPE     tegraType;
    NvU32      chipId0;     // 32-bit chipId (pmcBoot0 on GPU)
    NvU32      chipId1;     // 32-bit chipId (pmcBoot42 on GPU)
    NvU32      socChipId0;  // 32-bit SOC chipId
    NvU32      hidrev, majorRev;
    NvU32      gpuId;
    NvUuid     gpuUuid;
    NvBool     bGpuUuidValid = NV_FALSE;

    gpumgrGetGpuHalFactor(&chipId0, &chipId1, &socChipId0, &rmVariant, &tegraType, pAttachArg);

    hidrev = DRF_VAL(_PAPB_MISC, _GP_HIDREV, _CHIPID, socChipId0);
    majorRev = DRF_VAL(_PAPB_MISC, _GP_HIDREV, _MAJORREV, socChipId0);

    // WAR: The majorrev of t234 shows 0xa on fmodel instead of 0x4
    if ((hidrev == 0x23) && (majorRev == 0xa))
    {
        majorRev = 0x4;
    }

    hidrev = (hidrev << 4) | majorRev;

    //
    // The OS layer must have previously registered the GPU ID, and may have already registered
    // the UUID. Pull out the registered IDs for this device from the probed GPU info to pass to
    // the OBJGPU constructor.
    //
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpumgrGetRegisteredIds(pAttachArg->nvDomainBusDeviceFunc,
                                                              &gpuId, &gpuUuid, &bGpuUuidValid));

    // create OBJGPU with halspec factor initialization value
    status = objCreate(&pGpu, pSys, OBJGPU,
                    /* ChipHal_arch = */ decodePmcBoot42Architecture(chipId1),
                    /* ChipHal_impl = */ DRF_VAL(_PMC, _BOOT_42, _IMPLEMENTATION, chipId1),
                  /* ChipHal_hidrev = */ hidrev,
          /* RmVariantHal_rmVariant = */ rmVariant,
          /* TegraChipHal_tegraType = */ tegraType,
                 /* DispIpHal_ipver = */ 0,  // initialized later
                /* ctor.gpuInstance = */ gpuInstance,
                      /* ctor.gpuId = */ gpuId,
                      /* ctor.pUuid = */ bGpuUuidValid ? &gpuUuid : NULL);
    if (status != NV_OK)
    {
        return status;
    }

    // legacy chip-config Hal registration path
    status = gpuBindHalLegacy(pGpu, chipId0, chipId1, socChipId0);
    if (status != NV_OK)
    {
        objDelete(pGpu);
        return status;
    }

    //
    // Save away the public ID associated with the handle just returned
    // from create object.
    //
    pGpuMgr->gpuHandleIDList[pGpuMgr->numGpuHandles].gpuInstance = gpuInstance;
    pGpuMgr->gpuHandleIDList[pGpuMgr->numGpuHandles].pGpu = pGpu;

    pGpuMgr->numGpuHandles++;

    return status;
}


static void
_gpumgrGetEncSessionStatsReportingState(OBJGPU *pGpu)
{
    NvU32 numClasses = 0;

    // Set NvEnc Session Stats reporting state flag to 'not supported' on GPUs with no encoder engine.
    NV_STATUS nvStatus = gpuGetClassList(pGpu, &numClasses, NULL, MKENGDESC(classId(OBJMSENC), 0));
    if ((nvStatus != NV_OK) || (numClasses == 0))
    {
        pGpu->encSessionStatsReportingState = NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_NOT_SUPPORTED;
        return;
    }

    pGpu->encSessionStatsReportingState = NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_DISABLED;

#if defined(NVCPU_AARCH64)
    pGpu->encSessionStatsReportingState = NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE_DISABLED;
#else
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    if (IS_VIRTUAL(pGpu))
    {
        if (pVSI)
        {
            pGpu->encSessionStatsReportingState = pVSI->vgpuStaticProperties.encSessionStatsReportingState;
        }
        else
        {
            pGpu->encSessionStatsReportingState = NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_ENABLED;
        }
    }
    else
    {
        NvU32 encSessionStatsReportingState = NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE_DISABLED;;

        // Encoder Sessions Stats report is enabled by default
        pGpu->encSessionStatsReportingState = NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_ENABLED;

        // Override the value with the value specified in registry.
        if (osReadRegistryDword(pGpu,
                                NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE,
                                &encSessionStatsReportingState) == NV_OK)
        {
            switch(encSessionStatsReportingState)
            {
                case NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE_DISABLED:
                    pGpu->encSessionStatsReportingState = NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_DISABLED;
                    break;
                case NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE_ENABLED:
                    pGpu->encSessionStatsReportingState = NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_ENABLED;
                    break;
                default:
                    break;
            }
        }
    }
#endif // defined(NVCPU_AARCH64)
}

//
// gpumgrAttachGpu
//
// This interface is used by os-dependent code to attach a new gpu
// to the pool managed by the RM.  Construction of OBJGPU and it's
// descendants is handled here, along with any other necessary prep
// for the subsequent gpu preinit/init stages.
//
NV_STATUS
gpumgrAttachGpu(NvU32 gpuInstance, GPUATTACHARG *pAttachArg)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPU    *pGpu = NULL;
    NV_STATUS  status;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // create the new OBJGPU
    if ((status = _gpumgrCreateGpu(gpuInstance, pAttachArg)) != NV_OK)
    {
        goto gpumgrAttach_error_and_exit;
    }

    // get a pointer to the new OBJGPU
    pGpu = gpumgrGetGpu(gpuInstance);

    // load up attach parameters
    gpumgrSetAttachInfo(pGpu, pAttachArg);

    // Load OOR check address mode based on arch
#if defined(NVCPU_X86_64)
    pGpu->busInfo.oorArch = OOR_ARCH_X86_64;
#elif defined(NVCPU_PPC64LE)
    pGpu->busInfo.oorArch = OOR_ARCH_PPC64LE;
#elif defined(NVCPU_ARM)
    pGpu->busInfo.oorArch = OOR_ARCH_ARM;
#elif defined(NVCPU_AARCH64)
    pGpu->busInfo.oorArch = OOR_ARCH_AARCH64;
#else
    pGpu->busInfo.oorArch = OOR_ARCH_NONE;
#endif

    pGpu->pOS = SYS_GET_OS(pSys);

    // let os fill in dpc details before we get into engine construction
    if ((status = osDpcAttachGpu(pGpu, pAttachArg->pOsAttachArg)) != NV_OK)
    {
        goto gpumgrAttach_error_and_exit;
    }

    // let os fill in what it needs before we get into engine construction
    if ((status = osAttachGpu(pGpu, pAttachArg->pOsAttachArg)) != NV_OK)
    {
        goto gpumgrAttach_error_and_exit;
    }

    NV_ASSERT((pGpuMgr->gpuAttachMask & NVBIT(gpuInstance)) == 0);
    pGpuMgr->gpuAttachMask |= NVBIT(gpuInstance);
    pGpuMgr->gpuAttachCount++;

    status = _gpumgrRegisterRmCapsForGpu(pGpu);
    if (status != NV_OK)
    {
        goto gpumgrAttach_error_and_exit;
    }

    // finish gpu construction
    if ((status = gpuPostConstruct(pGpu, pAttachArg)) != NV_OK)
    {
        goto gpumgrAttach_error_and_exit;
    }

    _gpumgrGetEncSessionStatsReportingState(pGpu);

    Intr *pIntr = GPU_GET_INTR(pGpu);
    // On some boards, vbios enables interrupt early before RM
    // initialize pGpu so that hotplug intrs can be serviced on
    // the mfg line. Disable interrupt here for this case.
    if (pIntr != NULL)
    {
        if (intrGetIntrEnFromHw_HAL(pGpu, pIntr, NULL) != INTERRUPT_TYPE_DISABLED)
        {
            intrSetIntrEnInHw_HAL(pGpu, pIntr, INTERRUPT_TYPE_DISABLED, NULL);
            intrSetStall_HAL(pGpu, pIntr, INTERRUPT_TYPE_DISABLED, NULL);
        }
    }

    // Add entry into system nvlink topo array
    gpumgrAddSystemNvlinkTopo(pAttachArg->nvDomainBusDeviceFunc);
    // Add entry into system partition topo array
    gpumgrAddSystemMIGInstanceTopo(pAttachArg->nvDomainBusDeviceFunc);

    // Determine conf compute params
    NV_ASSERT_OK_OR_RETURN(_gpumgrDetermineConfComputeCapabilities(pGpuMgr, pGpu));

    if (!IS_GSP_CLIENT(pGpu))
        pGpuMgr->gpuMonolithicRmMask |= NVBIT(gpuInstance);

    return status;

gpumgrAttach_error_and_exit:
    if ((pGpuMgr->gpuAttachMask & NVBIT(gpuInstance)) != 0)
    {
        pGpuMgr->gpuAttachMask &= ~NVBIT(gpuInstance);
        pGpuMgr->gpuAttachCount--;
    }

    if (pGpu != NULL)
    {
        _gpumgrUnregisterRmCapsForGpu(gpuGetDBDF(pGpu));
    }

    osDpcDetachGpu(pGpu);
    _gpumgrDestroyGpu(gpuInstance);
    return status;
}

//
// gpumgrDetachGpu
//
// This entry point detaches a gpu from the RM.  The corresponding
// OBJGPU and any of it's offspring are released, etc.
//
NV_STATUS
gpumgrDetachGpu(NvU32 gpuInstance)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    NvBool bDelClientResourcesFromGpuMask = !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TIMEOUT_RECOVERY);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Mark for deletion the stale clients related to the GPU mask
    if (bDelClientResourcesFromGpuMask)
    {
        rmapiSetDelPendingClientResourcesFromGpuMask(NVBIT(gpuInstance));
    }

    osDpcDetachGpu(pGpu);

    pGpu->pOsRmCaps = NULL;

    // release pDev
    _gpumgrDestroyGpu(gpuInstance);

    // Delete the marked clients related to the GPU mask
    if (bDelClientResourcesFromGpuMask)
    {
        rmapiDelPendingDevices(NVBIT(gpuInstance));
        rmapiDelPendingClients();
    }

    pGpuMgr->gpuMonolithicRmMask &= ~NVBIT(gpuInstance);
    NV_ASSERT(pGpuMgr->gpuAttachMask & NVBIT(gpuInstance));
    pGpuMgr->gpuAttachMask &= ~NVBIT(gpuInstance);
    pGpuMgr->gpuAttachCount--;

    return NV_OK;
}

//
// gpumgrCreateDevice
//
// Create a broadcast device.   The set of one or more gpus
// comprising the broadcast device is described by gpuMask.
//
NV_STATUS
gpumgrCreateDevice(NvU32 *pDeviceInstance, NvU32 gpuMask, NvU32 *pGpuIdsOrdinal)
{
    OBJSYS    *pSys       = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr    = SYS_GET_GPUMGR(pSys);
    OBJGPU    *pParentGpu = NULL;
    NvU32      gpuInstance;
    NV_STATUS  status     = NV_ERR_INVALID_REQUEST;
    OBJGPUGRP *pGpuGrp    = NULL;

    pGpuMgr->deviceCount++;

    NV_ASSERT(gpuMask != 0);

    // if only 1 gpu in the set, we're done
    if (gpumgrIsSubDeviceCountOne(gpuMask))
    {
        // alloc new broadcast device instance
        status = gpumgrAllocDeviceInstance(pDeviceInstance);
        if (status != NV_OK)
        {
            goto gpumgrCreateDevice_exit;
        }

        gpumgrConstructGpuGrpObject(pGpuMgr, gpuMask,
                                 &pGpuMgr->pGpuGrpTable[*pDeviceInstance]);
        //
        // Set up parent gpu state.  pParentGpu == NULL during boot when
        // we're first creating this device because the GPU attach process
        // has not yet completed.  pParentGpu != NULL when we're coming
        // out of SLI (unlinking).
        //
        gpuInstance = 0;
        pParentGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
        if (pParentGpu)
        {
            gpumgrSetParentGPU(pParentGpu, pParentGpu);
        }

        gpumgrAddDeviceMaskToGpuInstTable(gpuMask);
        status = NV_OK;
        goto gpumgrCreateDevice_exit;
    }

gpumgrCreateDevice_exit:
    if (status != NV_OK)
    {
        // Device creation failed
        pGpuMgr->deviceCount--;
    }
    else
    {
        pGpuGrp = pGpuMgr->pGpuGrpTable[*pDeviceInstance];
        if (gpugrpGetGpuMask(pGpuGrp) != gpuMask)
        {
            NV_ASSERT(0);
            gpumgrDestroyDevice(*pDeviceInstance);
            return NV_ERR_INVALID_DATA;
        }
        NV_PRINTF(LEVEL_INFO,
                  "gpumgrCreateDevice: deviceInst 0x%x mask 0x%x\n",
                  *pDeviceInstance, gpuMask);
    }
    return status;
}

NV_STATUS
gpumgrDestroyDevice(NvU32 deviceInstance)
{
    OBJSYS    *pSys    = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS  status  = NV_OK;
    OBJGPUGRP *pGpuGrp = pGpuMgr->pGpuGrpTable[deviceInstance];
    NvU32      gpuMask;

    NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NV_ERR_OBJECT_NOT_FOUND);
    gpuMask = gpugrpGetGpuMask(pGpuGrp);

    NV_ASSERT(gpuMask != 0);

    // if we only have one subdevice we're done
    if (gpumgrIsSubDeviceCountOne(gpuMask))
    {
        gpugrpDestroy(pGpuGrp);
        pGpuMgr->pGpuGrpTable[deviceInstance] = NULL;
        gpumgrClearDeviceMaskFromGpuInstTable(gpuMask);
        goto gpumgrDestroyDevice_exit;
    }

gpumgrDestroyDevice_exit:
    pGpuMgr->deviceCount--;

    return status;
}

//
// gpumgrGetDeviceInstanceMask
//
// Returns mask of enabled (or valid) device instances.
// This mask tells clients which NV01_DEVICE class
// instances are valid.
//
NvU32
gpumgrGetDeviceInstanceMask(void)
{
    NvU32 i, deviceInstanceMask = 0;

    // for every broadcast device...
    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        // ...add it to our mask if it's enabled
        if (NV_OK == gpumgrIsDeviceInstanceValid(i))
            deviceInstanceMask |= NVBIT(i);
    }

    return deviceInstanceMask;
}

NvU32
gpumgrGetGpuMask(OBJGPU *pGpu)
{
    NvU32 deviceInstance = gpuGetDeviceInstance(pGpu);

    NV_ASSERT(deviceInstance < NV_MAX_DEVICES);

    return gpumgrGetDeviceGpuMask(deviceInstance);
}

//
// gpumgrGetSubDeviceCount
//
NvU32
gpumgrGetSubDeviceCount(NvU32 gpuMask)
{
    NvU32 subDeviceCount = 0;

    // tally # of gpus in the set
    while (gpuMask != 0)
    {
        subDeviceCount ++;
        gpuMask &= (gpuMask-1); // remove lowest bit in gpuMask
    }
    return subDeviceCount;
}

//
// gpumgrGetSubDeviceCountFromGpu
// ATTENTION: When using with SLI Next / RM Unlinked SLI, the
// subdevice count is always 1 for each GPU. This can cause
// bugs, buffer overflows with arrays based on subdevice instances as
// with RM Unlinked SLI:
//  - subdevice count is always 1 (the GPUs are not linked)
//  - GPU subdevice instance can be non zero
// For subdevice instance arrays, please use
// gpumgrGetSubDeviceMaxValuePlus1()
//
NvU32
gpumgrGetSubDeviceCountFromGpu(OBJGPU *pGpu)
{
    NvU32 gpuMask = gpumgrGetGpuMask(pGpu);
    NvU32 subDeviceCount = gpumgrGetSubDeviceCount(gpuMask);

    NV_ASSERT(subDeviceCount > 0);
    return subDeviceCount;
}

//
// gpumgrGetSubDeviceMaxValuePlus1
// SLI disabled: return 1
// SLI enabled with RM linked in SLI: returns 2 or more
// SLI enabled with RM unlinked: return current subdeviceInstance + 1
// Life of the function: until a full transition to SLI Next / RM Unlinked SLI.
//
NvU32
gpumgrGetSubDeviceMaxValuePlus1(OBJGPU *pGpu)
{
    if (!IsSLIEnabled(pGpu))
    {
        // SLI disabled: return 1 as all GPU subdevice instances are 0
        // Unkinked SLI: returns the current subdevice instance + 1
        return gpumgrGetSubDeviceInstanceFromGpu(pGpu) + 1;
    }
    else
    {
        // SLI Enabled in RM: The count of subdevice instances for that GPU/device
        return gpumgrGetSubDeviceCountFromGpu(pGpu);
    }
}

static void
gpumgrSetAttachInfo(OBJGPU *pGpu, GPUATTACHARG *pAttachArg)
{
    if (pAttachArg->socDeviceArgs.specified)
    {
        NvU32 idx;
        NvU32 maxIdx;
        // This path is taken for Tegra Display and iGPU

        //
        // TODO: This existing field is specifically used to safeguard
        // iGPU-specific code paths within RM, and should actually be NV_FALSE for
        // T234D+.
        //
        // See JIRA TDS-5101 for more details.
        //
        pGpu->bIsSOC = NV_TRUE;
        maxIdx = SOC_DEV_MAPPING_MAX;

        for (idx = 0; idx < maxIdx; idx++)
        {
            pGpu->deviceMappings[idx] = pAttachArg->socDeviceArgs.deviceMapping[idx];
        }

        pGpu->busInfo.iovaspaceId = pAttachArg->socDeviceArgs.iovaspaceId;
        {
            pGpu->busInfo.gpuPhysAddr = pGpu->deviceMappings[SOC_DEV_MAPPING_DISP].gpuNvPAddr;
            pGpu->gpuDeviceMapCount = 1;
        }

        //
        // TODO bug 2100708: a fake DBDF is used on SOC to opt out of some
        // RM paths that cause issues otherwise, see the bug for details.
        //
        pGpu->busInfo.nvDomainBusDeviceFunc = pAttachArg->nvDomainBusDeviceFunc;
        pGpu->busInfo.bNvDomainBusDeviceFuncValid = NV_TRUE;
    }
    else if (pAttachArg->bIsSOC)
    {
        // This path is only taken for ARCH MODS iGPU verification.

        NV_ASSERT(sizeof(pGpu->deviceMappings) == sizeof(pAttachArg->socDeviceMappings));
        pGpu->bIsSOC = NV_TRUE;
        pGpu->idInfo.PCIDeviceID    = pAttachArg->socId;
        pGpu->idInfo.PCISubDeviceID = pAttachArg->socSubId;
        pGpu->busInfo.iovaspaceId   = pAttachArg->iovaspaceId;
        if (RMCFG_FEATURE_PLATFORM_MODS)
        {
            NV_ASSERT(sizeof(pGpu->deviceMappings) == sizeof(pAttachArg->socDeviceMappings));
            portMemCopy(pGpu->deviceMappings, sizeof(pGpu->deviceMappings), pAttachArg->socDeviceMappings, sizeof(pGpu->deviceMappings));
            pGpu->gpuDeviceMapCount = pAttachArg->socDeviceCount;

            //
            // TODO bug 2100708: a fake DBDF is used on SOC to opt out of some
            // RM paths that cause issues otherwise, see the bug for details.
            //
            pGpu->busInfo.nvDomainBusDeviceFunc = pAttachArg->nvDomainBusDeviceFunc;
            pGpu->busInfo.bNvDomainBusDeviceFuncValid = NV_TRUE;
        }
    }
    else
    {
        //
        // Set this gpu's hardware register access address pointers
        // from the contents of mappingInfo.
        //
        pGpu->bIsSOC = NV_FALSE;

        pGpu->deviceMappings[0].gpuNvAddr = pAttachArg->regBaseAddr;
        pGpu->registerAccess.gpuFbAddr = pAttachArg->fbBaseAddr;
        pGpu->busInfo.gpuPhysAddr     = pAttachArg->devPhysAddr;
        pGpu->busInfo.gpuPhysFbAddr   = pAttachArg->fbPhysAddr;
        pGpu->busInfo.gpuPhysInstAddr = pAttachArg->instPhysAddr;
        pGpu->busInfo.gpuPhysIoAddr   = pAttachArg->ioPhysAddr;
        pGpu->busInfo.iovaspaceId     = pAttachArg->iovaspaceId;
        pGpu->busInfo.nvDomainBusDeviceFunc = pAttachArg->nvDomainBusDeviceFunc;
        pGpu->busInfo.bNvDomainBusDeviceFuncValid = NV_TRUE;
        pGpu->deviceMappings[0].gpuNvLength = pAttachArg->regLength;
        pGpu->fbLength                = pAttachArg->fbLength;
        pGpu->busInfo.IntLine         = pAttachArg->intLine;
        pGpu->gpuDeviceMapCount       = 1;
        pGpu->cpuNumaNodeId           = pAttachArg->cpuNumaNodeId;

        if ( ! pAttachArg->instBaseAddr )
        {
            //
            // The OS init goo didn't map a separate region for instmem.
            // So instead use the 1M mapping in bar0.
            //
            pGpu->instSetViaAttachArg = NV_FALSE;
            pGpu->registerAccess.gpuInstAddr = (GPUHWREG*)(((NvU8*)pGpu->deviceMappings[0].gpuNvAddr) + 0x00700000); // aka NV_PRAMIN.
            if (!pGpu->busInfo.gpuPhysInstAddr)
            {
                //
                // Only use the bar0 window physical address if the OS didn't
                // specify a bar2 physical address.
                //
                pGpu->busInfo.gpuPhysInstAddr = pGpu->busInfo.gpuPhysAddr + 0x00700000;  // aka NV_PRAMIN
            }
            pGpu->instLength = 0x100000; // 1MB
        }
        else
        {
            pGpu->instSetViaAttachArg = NV_TRUE;
            pGpu->registerAccess.gpuInstAddr = pAttachArg->instBaseAddr;
            pGpu->instLength = pAttachArg->instLength;
        }
    }
}

//
// gpumgrStatePreInitGpu & gpumgrStateInitGpu
//
// These routines handle unicast gpu initialization.
//
NV_STATUS
gpumgrStatePreInitGpu(OBJGPU *pGpu)
{
    NV_STATUS status;

    // LOCK: acquire GPUs lock
    status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT);
    if (status == NV_OK)
    {
        if (FULL_GPU_SANITY_CHECK(pGpu))
        {
            // pre-init phase done in UC mode
            status = gpuStatePreInit(pGpu);
        }
        else
        {
            status = NV_ERR_GPU_IS_LOST;
            DBG_BREAKPOINT();
        }
        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    // save the init status for later client queries
    gpumgrSetGpuInitStatus(pGpu->gpuId, status);

    return status;
}

NV_STATUS
gpumgrStateInitGpu(OBJGPU *pGpu)
{
    NV_STATUS status;

    // LOCK: acquire GPUs lock
    status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT);
    if (status == NV_OK)
    {
        // init phase
        status = gpuStateInit(pGpu);

        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    // save the init status for later client queries
    gpumgrSetGpuInitStatus(pGpu->gpuId, status);

    return status;
}

NV_STATUS
gpumgrStateLoadGpu(OBJGPU *pGpu, NvU32 flags)
{
    NV_STATUS status;

    // LOCK: acquire GPUs lock
    status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT);
    if (status == NV_OK)
    {
        // Load phase
        status = gpuStateLoad(pGpu, flags);

        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    if (status != NV_OK)
        goto gpumgrStateLoadGpu_exit;

    OBJSYS *pSys       = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    // Determine nvlink encryption params
    NV_ASSERT_OK_OR_RETURN(_gpumgrDetermineNvlinkEncryptionCapabilities(pGpuMgr, pGpu));

gpumgrStateLoadGpu_exit:
    // save the init status for later client queries
    gpumgrSetGpuInitStatus(pGpu->gpuId, status);

    return status;
}

//
// gpumgrGetNextGpu
//
// This routine searches subDeviceMask for the next gpu by using
// the start index value as a beginning bit position.  If a gpu is
// found, the start index value is bumped to the next bit position
// in the mask.
//
POBJGPU
gpumgrGetNextGpu(NvU32 subDeviceMask, NvU32 *pStartIndex)
{
    NvU32 i;

    if (*pStartIndex > NV_MAX_DEVICES)
    {
        *pStartIndex = NV_MAX_DEVICES;
        return NULL;
    }

    for (i = *pStartIndex; i < NV_MAX_DEVICES; i++)
    {
        if (subDeviceMask & NVBIT(i))
        {
            *pStartIndex = i+1;
            return gpumgrGetGpu(i);
        }
    }

    *pStartIndex = NV_MAX_DEVICES;
    return NULL;
}


//
// gpumgrIsGpuPointerValid - Validates pGpu is initialized without dereferencing it.
//
NvBool
gpumgrIsGpuPointerValid(OBJGPU *pGpu)
{
    OBJGPU    *pTempGpu       = NULL;
    NvU32      gpuMask        = 0;
    NvU32      gpuCount       = 0;
    NvU32      gpuIndex       = 0;

    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);
    pTempGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);

    while(pTempGpu)
    {
        if (pTempGpu->getProperty(pTempGpu, PDB_PROP_GPU_STATE_INITIALIZED))
        {
            if (pTempGpu == pGpu)
            {
                return NV_TRUE;
            }
        }

        pTempGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    }

    return NV_FALSE;
}

//
// gpumgrIsGpuPointerAttached - Validates pGpu is attached without dereferencing it.
//
NvBool
gpumgrIsGpuPointerAttached(OBJGPU *pGpu)
{
    OBJGPU    *pTempGpu       = NULL;
    NvU32      gpuMask        = 0;
    NvU32      gpuCount       = 0;
    NvU32      gpuIndex       = 0;

    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);
    pTempGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);

    while(pTempGpu)
    {
        if (pTempGpu == pGpu)
        {
            return NV_TRUE;
        }
        pTempGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    }

    return NV_FALSE;
}


NvBool gpumgrIsGpuDisplayParent(OBJGPU *pGpu)
{
    OBJGPUGRP *pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);
    NvBool     rc      = NV_FALSE;
    NvU32      gpuMask;

    NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NV_FALSE);
    gpuMask = gpugrpGetGpuMask(pGpuGrp);

    // If there's only one GPU in the device, then of course it's the display parent!
    if (gpumgrIsSubDeviceCountOne(gpuMask))
    {
        rc = NV_TRUE;
    }
    //
    // If the gpuInstance argument is the first gpuInstance in the ordering,
    // then it's the display parent!
    //
    else if (pGpu->gpuInstance == pGpuGrp->SliLinkOrder[0].gpuInstance)
    {
        rc = NV_TRUE;
    }

    // Otherwise it isn't.
    return rc;
}

OBJGPU *gpumgrGetDisplayParent(OBJGPU *pGpu)
{
    OBJGPUGRP *pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);
    NvU32      gpuCount;
    NvU32      gpuMask;
    NvU32      gpuInstance;

    NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NULL);
    gpuMask = gpugrpGetGpuMask(pGpuGrp);
    gpuCount = gpumgrGetSubDeviceCount(gpuMask);

    if (gpuCount > 1)
    {
        gpuInstance = pGpuGrp->SliLinkOrder[0].gpuInstance;
        pGpu = gpumgrGetGpu(gpuInstance);
    }

    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

    return pGpu;
}

//
// gpumgrGetProbedGpuIds
//
// This routine services the NV0000_CTRL_GPU_GET_PROBED_IDS command.
// The passed in gpuIds table is filled in with valid gpuId info
// for each probed gpu.  Invalid entries in the table are set to the
// invalid id value.
//
NV_STATUS
gpumgrGetProbedGpuIds(NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *pGpuIdsParams)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i, j, k;

    ct_assert(NV_MAX_DEVICES == NV0000_CTRL_GPU_MAX_PROBED_GPUS);

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0, j = 0, k = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        if (pGpuMgr->probedGpus[i].gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            if (pGpuMgr->probedGpus[i].bExcluded)
                pGpuIdsParams->excludedGpuIds[k++] = pGpuMgr->probedGpus[i].gpuId;
            else
                pGpuIdsParams->gpuIds[j++] = pGpuMgr->probedGpus[i].gpuId;
        }
    }

    portSyncMutexRelease(pGpuMgr->probedGpusLock);

    for (i = j; i < NV_ARRAY_ELEMENTS(pGpuIdsParams->gpuIds); i++)
        pGpuIdsParams->gpuIds[i] = NV0000_CTRL_GPU_INVALID_ID;

    for (i = k; i < NV_ARRAY_ELEMENTS(pGpuIdsParams->excludedGpuIds); i++)
        pGpuIdsParams->excludedGpuIds[i] = NV0000_CTRL_GPU_INVALID_ID;

    return NV_OK;
}

//
// gpumgrGetAttachedGpuIds
//
// This routine services the NV0000_CTRL_GPU_GET_ATTACHED_IDS command.
// The passed in gpuIds table is filled in with valid gpuId info
// for each attached gpu.  Any remaining entries in the table are set to
// the invalid id value.
//
NV_STATUS
gpumgrGetAttachedGpuIds(NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS *pGpuIdsParams)
{
    OBJGPU *pGpu;
    NvU32 gpuAttachCnt, gpuAttachMask, i, cnt;
    NvU32 *pGpuIds = &pGpuIdsParams->gpuIds[0];

    // fill the table w/valid entries
    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    for (cnt = 0, i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (gpuAttachMask & NVBIT(i))
        {
            pGpu = gpumgrGetGpu(i);
            pGpuIds[cnt++] = pGpu->gpuId;
        }
    }

    // invalidate rest of the entries
    while (cnt < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS)
        pGpuIds[cnt++] = NV0000_CTRL_GPU_INVALID_ID;

    return NV_OK;
}

//
// gpumgrGetSubDeviceDeviceInstanceFromGpu
//
// Given a pGpu return the corresponding subdevice instance value.
//
NvU32
gpumgrGetSubDeviceInstanceFromGpu(OBJGPU *pGpu)
{
    return pGpu->subdeviceInstance;
}

//
// gpumgrGetParentGPU
//
POBJGPU
gpumgrGetParentGPU(OBJGPU *pGpu)
{
    OBJGPUGRP *pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);
    NvU32      gpuMask;

    NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NULL);
    gpuMask = gpugrpGetGpuMask(pGpuGrp);

    if (gpumgrIsSubDeviceCountOne(gpuMask))
    {
        return pGpu;
    }
    else
    {
        return gpugrpGetParentGpu(pGpuGrp);
    }
}

//
// gpumgrSetParentGPU
//
void
gpumgrSetParentGPU(OBJGPU *pGpu, OBJGPU *pParentGpu)
{
    OBJGPUGRP *pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);

    NV_ASSERT_OR_RETURN_VOID(pGpuGrp != NULL);
    gpugrpSetParentGpu(pGpuGrp, pParentGpu);
}

//
// gpumgrGetGpuFromId
//
// Find the specified gpu from it's gpuId.
//
POBJGPU
gpumgrGetGpuFromId(NvU32 gpuId)
{
    OBJGPU *pGpu;
    NvU32 gpuAttachCnt, gpuAttachMask;
    NvU32 i;

    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (gpuAttachMask & NVBIT(i))
        {
            pGpu = gpumgrGetGpu(i);

            // found it
            if (pGpu->gpuId == gpuId)
                return pGpu;
        }
    }

    // didn't find it
    return NULL;
}

//
// gpumgrGetGpuFromUuid()
//
// Get GPUOBJECT from UUID. Returns NULL if it cannot find a GPU with the
// requested UUID.
//
POBJGPU
gpumgrGetGpuFromUuid(const NvU8 *pGpuUuid, NvU32 flags)
{
     OBJGPU *pGpu;
     NvU32 attachedGpuCount;
     NvU32 attachedGpuMask;
     NvU32 gpuIndex;
     NvU32 gidStrLen;
     NvU8 *pGidString = NULL;
     NV_STATUS rmStatus;

     // get all attached GPUs
     rmStatus = gpumgrGetGpuAttachInfo(&attachedGpuCount, &attachedGpuMask);

     gpuIndex = 0;

     for(pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex);
         pGpu != NULL;
         pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex))
     {
         //
         // get the GPU's UUID
         //
         // This implementation relies on the fact that gpuGetGidInfo() only
         // allocates memory if it succeeds.
         //
         rmStatus = gpuGetGidInfo(pGpu, &pGidString, &gidStrLen, flags);
         if (NV_OK != rmStatus)
             return NULL;

         // check if it matches
         if (0 == portMemCmp(pGidString, pGpuUuid, gidStrLen))
         {
             portMemFree(pGidString);
             return pGpu;
         }
         else
         {
             // if it doesn't match, clean up allocated memory for next iteration
             portMemFree(pGidString);
         }
     }

     return NULL; // Failed to find a GPU with the requested UUID
}

//
// gpumgrGetGpuFromBusInfo
//
// Find the specified GPU using its PCI bus info.
//
POBJGPU
gpumgrGetGpuFromBusInfo(NvU32 domain, NvU8 bus, NvU8 device)
{
    NV_STATUS status;
    OBJGPU *pGpu;
    NvU32 attachedGpuCount;
    NvU32 attachedGpuMask;
    NvU32 gpuIndex = 0;

    status = gpumgrGetGpuAttachInfo(&attachedGpuCount, &attachedGpuMask);
    NV_ASSERT_OR_RETURN(status == NV_OK, NULL);

    for (pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex);
         pGpu != NULL;
         pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex))
    {
        if ((gpuGetDomain(pGpu) == domain)  &&
            (gpuGetBus(pGpu) == bus)        &&
            (gpuGetDevice(pGpu) == device))
        {
            return pGpu;
        }
    }

    return NULL;
}

//
// gpumgrSetGpuId
//
// This routine assigns the specified gpuId to the specified gpu.
//
void
gpumgrSetGpuId(OBJGPU *pGpu, NvU32 gpuId)
{
    pGpu->gpuId = gpuId;

    // if boardId is unassigned then give it a default value now
    if (pGpu->boardId == 0xffffffff)
    {
        pGpu->boardId = gpuId;
    }
}

//
// gpumgrGetGpuIdInfo
//
// Special purpose routine that handles NV0000_CTRL_CMD_GPU_GET_ID_INFO
// requests from clients.
// NV0000_CTRL_CMD_GPU_GET_ID_INFO is deprecated in favour of
// NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2, per comments in ctrl0000gpu.h
//
NV_STATUS
gpumgrGetGpuIdInfoV2(NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS *pGpuInfo)
{
    OBJGPU *pGpu;
    NvU32 deviceInstance, subDeviceInstance;

    // start by making sure client request specifies a valid gpu
    pGpu = gpumgrGetGpuFromId(pGpuInfo->gpuId);
    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpumgrGetGpuInfoV2: bad gpuid spec: 0x%x\n",
                  pGpuInfo->gpuId);
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT_OR_RETURN(gpumgrIsSafeToReadGpuInfo(), NV_ERR_INVALID_LOCK_STATE);

    //
    // We have a valid gpuInstance, so now let's get the corresponding
    // deviceInstance/subDeviceInstance pair.
    //
    deviceInstance = gpuGetDeviceInstance(pGpu);
    if (deviceInstance == NV_MAX_DEVICES)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "gpumgrGetGpuInfoV2: deviceInstance not found\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    {
        subDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    }

    pGpuInfo->gpuInstance = pGpu->gpuInstance;
    pGpuInfo->deviceInstance = deviceInstance;
    pGpuInfo->subDeviceInstance = subDeviceInstance;
    pGpuInfo->boardId = pGpu->boardId;

    //
    // Setup gpu info flags; see ctrl0000gpu.h for list of flags.
    //
    pGpuInfo->gpuFlags = 0;
    pGpuInfo->numaId = NV0000_CTRL_NO_NUMA_NODE;
    if (osGpuSupportsAts(pGpu))
    {
        pGpuInfo->gpuFlags |= DRF_NUM(0000, _CTRL_GPU_ID_INFO, _ATS_ENABLED,
                                      NV0000_CTRL_GPU_ID_INFO_ATS_ENABLED_TRUE);

        if (gpuIsSelfHosted(pGpu) && IS_MIG_IN_USE(pGpu))
        {
            //
            // We do not have enough information here to route to correct MIG instance.
            // There may even not be enough state to do this lookup at all as this is a 0x0000 control
            // but we need to get to Subdevice child. Try extra hard to preserve originally
            // implemented behavior. See bug 4251227.
            //
            CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
            RS_ITERATOR it = clientRefIter(pCallContext->pClient, NULL, classId(GPUInstanceSubscription),
                                           RS_ITERATE_DESCENDANTS, NV_TRUE);

            while (clientRefIterNext(it.pClient, &it))
            {
                NvU32 swizzId;
                GPUInstanceSubscription *pGPUInstanceSubscription =
                    dynamicCast(it.pResourceRef->pResource, GPUInstanceSubscription);

                if (GPU_RES_GET_GPU(pGPUInstanceSubscription) != pGpu)
                {
                    continue;
                }

                swizzId = gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription)->swizzId;

                if (swizzId == NVC637_DEVICE_PROFILING_SWIZZID)
                {
                    continue;
                }
                if (GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)->memPartitionNumaInfo[swizzId].bInUse)
                {
                    pGpuInfo->numaId = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)->memPartitionNumaInfo[swizzId].numaNodeId;
                }
                break;
            }
        }
        else
        {
            pGpuInfo->numaId = pGpu->numaNodeId;
        }
    }

    // is this gpu in use?
    pGpuInfo->gpuFlags |= DRF_NUM(0000, _CTRL_GPU_ID_INFO, _IN_USE, gpuIsInUse(pGpu));

    // is this gpu part of a sli device?
    pGpuInfo->gpuFlags |= DRF_NUM(0000, _CTRL_GPU_ID_INFO, _LINKED_INTO_SLI_DEVICE, IsSLIEnabled(pGpu));

    // is this gpu a mobile gpu?
    if (IsMobile(pGpu))
    {
        pGpuInfo->gpuFlags |= DRF_DEF(0000, _CTRL_GPU_ID_INFO, _MOBILE, _TRUE);
    }

    // is this gpu the boot primary?
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_PRIMARY_DEVICE))
    {
        pGpuInfo->gpuFlags |= DRF_DEF(0000, _CTRL_GPU_ID_INFO, _BOOT_MASTER, _TRUE);
    }

    // is this GPU part of an SOC
    if (pGpu->bIsSOC || pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_SOC_SDM))
    {
        pGpuInfo->gpuFlags |= DRF_DEF(0000, _CTRL_GPU_ID_INFO, _SOC, _TRUE);
    }

    // GPU specific SLI status
    pGpuInfo->sliStatus = pGpu->sliStatus;

    NV_PRINTF(LEVEL_INFO,
              "gpumgrGetGpuInfoV2: gpu[0x%x]: device 0x%x subdevice 0x%x\n",
              pGpuInfo->gpuId, pGpuInfo->deviceInstance,
              pGpuInfo->subDeviceInstance);

    return NV_OK;
}
NV_STATUS
gpumgrGetGpuIdInfo(NV0000_CTRL_GPU_GET_ID_INFO_PARAMS *pGpuInfo)
{
    NV_STATUS status;
    NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS pGpuInfoV2 = {0};
    pGpuInfoV2.gpuId = pGpuInfo->gpuId;

    status = gpumgrGetGpuIdInfoV2(&pGpuInfoV2);

    if (status != NV_OK)
    {
        return status;
    }
    pGpuInfo->gpuFlags = pGpuInfoV2.gpuFlags;
    pGpuInfo->deviceInstance = pGpuInfoV2.deviceInstance;
    pGpuInfo->subDeviceInstance = pGpuInfoV2.subDeviceInstance;
    pGpuInfo->sliStatus = pGpuInfoV2.sliStatus;
    pGpuInfo->boardId = pGpuInfoV2.boardId;
    pGpuInfo->gpuInstance = pGpuInfoV2.gpuInstance;
    pGpuInfo->numaId = pGpuInfoV2.numaId;

    return status;
}

//
// gpumgrGetGpuInitStatus
//
// Special purpose routine that handles NV0000_CTRL_CMD_GET_GPU_INIT_STATUS
// requests from clients.
//
NV_STATUS
gpumgrGetGpuInitStatus(NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS *pGpuInitStatus)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;
    NV_STATUS rmStatus;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        if (pGpuMgr->probedGpus[i].gpuId == pGpuInitStatus->gpuId)
        {
            if (pGpuMgr->probedGpus[i].bInitAttempted)
            {
                pGpuInitStatus->status = pGpuMgr->probedGpus[i].initStatus;
                rmStatus = NV_OK;
            }
            else
            {
                //
                // No init has been attempted on this GPU yet, so this request
                // doesn't make any sense.
                //
                rmStatus = NV_ERR_INVALID_STATE;
            }
            goto done;
        }
    }

    // We couldn't find a probed gpuId matching the requested one.
    rmStatus = NV_ERR_INVALID_ARGUMENT;
done:
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return rmStatus;
}

NV_STATUS
gpumgrGetProbedGpuDomainBusDevice(NvU32 gpuId, NvU64 *gpuDomainBusDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;
    NV_STATUS rmStatus;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        if (pGpuMgr->probedGpus[i].gpuId == gpuId)
        {
            *gpuDomainBusDevice = pGpuMgr->probedGpus[i].gpuDomainBusDevice;
            rmStatus = NV_OK;
            goto done;
        }
    }

    //
    // We couldn't find a probed gpuId matching the requested one.
    //
    // This used to return a generic NV_ERR_INVALID_ARGUMENT, but we want to be
    // more specific as at least nvml wants to be able to tell this case apart
    // from other errors. This case is expected when GPUs are removed from the
    // driver (e.g. through unbind on Linux) after a client queries for the
    // probed GPUs, but before getting the PCI info for all of them.
    //
    rmStatus = NV_ERR_OBJECT_NOT_FOUND;

done:
    portSyncMutexRelease(pGpuMgr->probedGpusLock);
    return rmStatus;
}

//
// gpumgrSetGpuInitStatus
//
// Marks initialization of the gpu in question as attempted and stores the
// status.
//
void
gpumgrSetGpuInitStatus(NvU32 gpuId, NV_STATUS status)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); i++)
    {
        if (pGpuMgr->probedGpus[i].gpuId == gpuId)
        {
            // Overwrite any previous init status
            pGpuMgr->probedGpus[i].bInitAttempted = NV_TRUE;
            pGpuMgr->probedGpus[i].initStatus = status;
            break;
        }
    }

    portSyncMutexRelease(pGpuMgr->probedGpusLock);
}

//
// gpumgrUpdateBoardId
//
// Update gpu boardIds.  By default the boardId will already be set to
// the gpuId.  This routine fetches the upstream bridge port and sets the
// new boardId to the pGpu
//
void
gpumgrUpdateBoardId_IMPL(OBJGPU *pGpu)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);
    void *handleUp;
    NvU16 vendorIDUp, deviceIDUp;
    NvU8 busUp, deviceUp, funcUp;
    NvU32 domain;
    NvU32 boardId;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE))
    {
        return;
    }

    domain = gpuGetDomain(pGpu);
    handleUp = clFindBrdgUpstreamPort(pGpu, pCl, NV_TRUE,
                                      &busUp, &deviceUp, &funcUp,
                                      &vendorIDUp, &deviceIDUp,
                                      NULL);
    if (!handleUp)
        return;

    if (!(IS_SUPPORTED_PCIE_SWITCH(vendorIDUp, deviceIDUp)))
    {
        return;
    }

    boardId = gpuGenerate32BitId(domain, busUp, deviceUp);
    pGpu->boardId = boardId;
}

//
// gpumgrGetDefaultPrimaryGpu
//
// This routine looks at the set of GPUs and picks a the primary (parent)
// with the following rules, in this order:
// 1- If a primary GPU has been passed in an SLI config by a client
// 2- If there is a boot primary in the GPU mask
// 3- The first VGA device attached (not 3d controller)
//
NvU32
gpumgrGetDefaultPrimaryGpu
(
    NvU32 gpuMask
)
{
    OBJGPU     *pGpu = NULL;
    NvU32       gpuInstance;

    if (gpuMask == 0)
    {
        NV_ASSERT(gpuMask);
        return 0;
    }

    // Find  masterFromSLIConfig, set when a RM client passes a primary GPU
    // index from a SLI config
    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (pGpu->masterFromSLIConfig)
        {
            break;
        }
    }

    // default to boot primary
    if (pGpu == NULL)
    {
        gpuInstance = 0;
        while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_PRIMARY_DEVICE))
            {
                break;
            }
        }
    }

    if (pGpu)
    {
        return gpuGetInstance(pGpu);
    }

    // otherwise the primary is the first non 3d controller in the set attached to the RM
    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_3D_CONTROLLER))
        {
            break;
        }
    }

    if (!pGpu)
    {
        // The GPU mask contains only 3d Controllers.
        // Choose first one in the set attached to the RM.
        gpuInstance = 0;
        pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
    }

    if (pGpu == NULL)
    {
        return 0; // This should never happen
    }

    return gpuGetInstance(pGpu);
}

void
gpumgrServiceInterrupts_IMPL(NvU32 gpuMask, MC_ENGINE_BITVECTOR *engineMask, NvBool bExhaustive)
{
    OBJGPU *pGpu;
    NvU32 gpuInstance = 0;

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (gpuIsGpuFullPower(pGpu))
        {
            Intr *pIntr = GPU_GET_INTR(pGpu);

            //
            // On SLI, one OBJGPU's StateInit functions could attempt to service
            // interrupts on another OBJGPU which has not yet started StateInit.
            // However Intr is not prepared to service interrupts until after
            // intrStateInit. To avoid calling in too early, check that the
            // interrupt table is initialized.
            //
            if (vectIsEmpty(&pIntr->intrTable))
            {
                continue;
            }

            //
            // Service interrupts for the specified engines now.
            // A non-false value for 'bExhaustive' indicates that intrServiceStallList
            // should repreatedly service all specified engines, until it finds
            // no further pending interrupt work remains on those engines.
            //
            intrServiceStallList_HAL(pGpu, pIntr, engineMask, bExhaustive);
        }
    }
}

NV_STATUS
gpumgrGetGpuLockAndDrPorts
(
    OBJGPU *pGpu,
    OBJGPU *pPeerGpu,
    NvU32 *pPinsetOut,
    NvU32 *pPinsetIn
)
{

    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPUGRP *pGpuGrp = NULL;
    NvU32      pinsetIndex = 0, childPinset = drPinSet_None;
    NvU32      i;
    NvU32      gpuInstance = pGpu->gpuInstance;
    NV_STATUS  rmStatus;

    *pPinsetOut = drPinSet_None;
    *pPinsetIn = drPinSet_None;

    if (pPeerGpu == NULL)
    {
        pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);
        NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NV_ERR_INVALID_DATA);

        for (i = 0; i < NV2080_MAX_SUBDEVICES; i++)
        {
            if (pGpuGrp->SliLinkOrder[i].gpuInstance == gpuInstance)
            {
                break;
            }
        }

        if (i == NV2080_MAX_SUBDEVICES)
        {
            *pPinsetOut = 0;
            *pPinsetIn  = 0;
        }
        else
        {
            *pPinsetOut = pGpuGrp->SliLinkOrder[i].ChildDrPort;
            *pPinsetIn  = pGpuGrp->SliLinkOrder[i].ParentDrPort;
        }

        return NV_OK;
    }
    else
    {
        NvU32 childPinsetIndex;

        for (pinsetIndex = 0; pinsetIndex < DR_PINSET_COUNT; pinsetIndex++)
        {
            if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _INPUT, _PASSED,
                pGpuMgr->gpuSliLinkRoute[pGpuMgr->gpuBridgeType][gpuInstance][pPeerGpu->gpuInstance][pinsetIndex]))
            {
                continue;
            }

            childPinset = DRF_VAL(_SLILINK, _ROUTE, _INPUT_CHILD_PORT,
                pGpuMgr->gpuSliLinkRoute[pGpuMgr->gpuBridgeType][gpuInstance][pPeerGpu->gpuInstance][pinsetIndex]);

            rmStatus = gpumgrPinsetToPinsetTableIndex(childPinset, &childPinsetIndex);
            if (rmStatus != NV_OK)
            {
                return rmStatus;
            }
            if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _OUTPUT, _PASSED,
                pGpuMgr->gpuSliLinkRoute[pGpuMgr->gpuBridgeType][pPeerGpu->gpuInstance][gpuInstance][childPinsetIndex]))
            {
                continue;
            }
            *pPinsetOut |= childPinset;
            *pPinsetIn |= NVBIT(pinsetIndex);
        }
        return ((*pPinsetOut != drPinSet_None) && (*pPinsetIn != drPinSet_None)) ? NV_OK : NV_ERR_INVALID_ARGUMENT;
    }

}

//
// Stores the address of the boot primary in pGpu
// Returns NV_OK on success NV_ERR_GENERIC otherwise.
//
NV_STATUS
gpumgrGetBootPrimary(OBJGPU **ppGpu)
{
    NvU32 gpuCount, gpuMask, idx1;
    OBJGPU *pGpu = NULL;

    // Find boot primary
    idx1 = 0;
    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &idx1)) != NULL)
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_PRIMARY_DEVICE))
            break;
    }
    *ppGpu = pGpu;

    // No boot primary
    if (pGpu == NULL)
    {
        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

//
// Returns the mGpu
//
OBJGPU *gpumgrGetMGpu (void)
{
    OBJGPU *pGpu;
    NvU32 gpuCount, gpuMask, gpuIndex = 0;
    // Parse through all the GPUs

    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)))
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_HYBRID_MGPU))
        {
            break;
        }
    }

    return pGpu;
}

//
// Get PhysFbAddr for the given GPU which may be different if
// the GPU is broadcast or chipset broadcast are enabled or not:
// - BC GPU + no CL BC -> returns gpu address
// - UC GPU -> returns GPU address
// - BC GPU + CL BC -> returns broadcast address
//
RmPhysAddr gpumgrGetGpuPhysFbAddr(OBJGPU *pGpu)
{
    RmPhysAddr physFbAddr;

    physFbAddr = pGpu->busInfo.gpuPhysFbAddr;

    NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB) || physFbAddr);
    return physFbAddr;
}


//
// Get GPU object from subdevice instance
//
POBJGPU
gpumgrGetGpuFromSubDeviceInst(NvU32 deviceInst, NvU32 subDeviceInst)
{
    OBJGPU    *pGpu     = NULL;
    OBJGPUGRP *pGpuGrp  = NULL;
    NvU32 gpuInst = 0;
    NvU32 gpuMask;

    pGpuGrp = gpumgrGetGpuGrpFromInstance(deviceInst);
    NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NULL);

    gpuMask = gpugrpGetGpuMask(pGpuGrp);

    // check for single GPU case
   if (gpumgrGetSubDeviceCount(gpuMask) == 1)
        return gpumgrGetNextGpu(gpuMask, &gpuInst);

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInst)) != NULL)
    {
        if (gpumgrGetSubDeviceInstanceFromGpu(pGpu) == subDeviceInst)
        {
            break;
        }
    }

    NV_ASSERT(pGpu);

    return pGpu;
}

/*!
 * @brief sets the device instance pGpu->deviceInstance for the GPUs indicated by the gpu mask
 *
 * Only remove the device instance if it is the last GPU to be removed.
 *
 * At RM initialization we fill in the software feature values for this GPU.
 * The values are determined from the software feature database
 *
 * @param[in]   gpuMask   NvU32 value
 *
 * @return NV_OK or NV_ERR_OBJECT_NOT_FOUND if no GPU has been found
 *
 */
NV_STATUS
gpumgrAddDeviceInstanceToGpus(NvU32 gpuMask)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS rmStatus = NV_ERR_OBJECT_NOT_FOUND;
    OBJGPU *pGpu = NULL;
    NvU32 i, gpuIndex = 0;
    OBJGPUGRP *pGpuGrp = NULL;

    // Add the device instance to the GPU objects in the mask
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)))
    {
        rmStatus = NV_ERR_OBJECT_NOT_FOUND;
        for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->pGpuGrpTable); i++)
        {
            pGpuGrp = pGpuMgr->pGpuGrpTable[i];
            // if it contains the specified gpu...
            if ((pGpuGrp != NULL) &&
                (gpugrpGetGpuMask(pGpuGrp) & NVBIT(pGpu->gpuInstance)))
            {
                pGpu->deviceInstance = i;
                rmStatus = NV_OK;
                break;
            }
        }
        NV_ASSERT_OK_OR_RETURN(rmStatus);
    }

    return rmStatus;
}

/*!
 * @brief Retrieves the OBJGPUGRP pointer given the instance
 *
 * @param[in]  gpugrpInstance GPUGRP instance
 *
 * @return GPUGRP pointer on success, NULL on error
 *
 */
OBJGPUGRP *
gpumgrGetGpuGrpFromInstance
(
    NvU32      gpugrpInstance
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_ASSERT_OR_RETURN(gpugrpInstance < NV_MAX_DEVICES, NULL);
    return pGpuMgr->pGpuGrpTable[gpugrpInstance];
}

/*!
 * @brief Retrieves the OBJGPUGRP pointer given the GPU pointer.
 *
 * @param[in]  pGpu GPU object pointer
 *
 * @return OBJGPUGRP pointer on success, NULL on error
 *
 */
OBJGPUGRP *
gpumgrGetGpuGrpFromGpu
(
    OBJGPU    *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 gpugrpInstance = gpuGetDeviceInstance(pGpu);
    NV_ASSERT_OR_RETURN(gpugrpInstance < NV_MAX_DEVICES, NULL);

    return pGpuMgr->pGpuGrpTable[gpugrpInstance];
}

/*!
 * @brief Constructs the GPUGRP object for the given instance
 *
 * @param[in]  pGpu      GPU object pointer
 * @param[in]  gpuMask   GpuMask corresponding to this GPUGRP
 * @param[out] ppGpuGrp  Newly created gpugrp object pointer
 *
 * @return NV_OK on success, appropriate error on failure.
 *
 */
NV_STATUS
gpumgrConstructGpuGrpObject
(
    OBJGPUMGR *pGpuMgr,
    NvU32      gpuMask,
    OBJGPUGRP **ppGpuGrp
)
{
    NV_STATUS  status;

    status = objCreate(ppGpuGrp, pGpuMgr, OBJGPUGRP);
    if (NV_OK != status)
    {
        return status;
    }

    status = gpugrpCreate(*ppGpuGrp, gpuMask);
    if (NV_OK != status)
    {
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Enter/exit "drain" state on a given GPU
 *
 * @param[in]  gpuId        Platform specific GPU Id
 * @param[in]  bEnable      NV_TRUE: enter, NV_FALSE: exit
 * @param[in]  bRemove      Ask the OS to forget the GPU, once quiescent
 * @param[in]  bLinkDisable Shut down the upstream PCIe link after the removal.
 *                          This is done in user-land, we just check that the
 *                          GPU is in the right state.
 *
 * @return NV_OK on success, appropriate error on failure.
 */
NV_STATUS
gpumgrModifyGpuDrainState

(
    NvU32   gpuId,
    NvBool  bEnable,
    NvBool  bRemove,
    NvBool  bLinkDisable
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPU     *pGpu;
    NvBool      bAttached;
    NvBool      bStateChange = NV_FALSE;
    NvU32       i;
    NvU32       domain = 0;
    NvU8        bus = 0;
    NvU8        device = 0;

    if (bRemove && !osRemoveGpuSupported())
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    bAttached = ((pGpu = gpumgrGetGpuFromId(gpuId)) != NULL);

    if (bEnable && bLinkDisable && bAttached)
    {
        return NV_ERR_IN_USE;
    }

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); ++i)
    {
        if (pGpuMgr->probedGpus[i].gpuId == gpuId)
        {
            bStateChange = pGpuMgr->probedGpus[i].bDrainState != bEnable;
            pGpuMgr->probedGpus[i].bDrainState = bEnable;
            pGpuMgr->probedGpus[i].bRemoveIdle = bEnable && bRemove;
            domain = gpuDecodeDomain(pGpuMgr->probedGpus[i].gpuDomainBusDevice);
            bus = gpuDecodeBus(pGpuMgr->probedGpus[i].gpuDomainBusDevice);
            device = gpuDecodeDevice(pGpuMgr->probedGpus[i].gpuDomainBusDevice);
            break;
        }
    }

    portSyncMutexRelease(pGpuMgr->probedGpusLock);

    if (i == NV_MAX_DEVICES)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // If the initial drain state (characterized by enabling draining without
    // setting the remove flag) is already enabled, multiple clients may be
    // trying to simultaneously manage drain state. Only return success for the
    // first to allow them to filter out the others.
    //
    if (bEnable && !bRemove && !bStateChange)
    {
        return NV_ERR_IN_USE;
    }

    if (bEnable && bRemove && !bAttached)
    {
        osRemoveGpu(domain, bus, device);
    }

    return NV_OK;
}

/*!
 * @brief Query "drain"/remove state on a given GPU
 *
 * @param[in]   gpuId   Platform specific GPU Id
 * @param[out]  pBEnable Drain state ptr
 * @param[out]  pBRemove Remove flag ptr
 *
 * @return NV_OK on success, appropriate error on failure.
 */
NV_STATUS
gpumgrQueryGpuDrainState

(
    NvU32   gpuId,
    NvBool *pBEnable,
    NvBool *pBRemove
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32       i;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); ++i)
    {
        if (pGpuMgr->probedGpus[i].gpuId == gpuId)
        {
            if (pBEnable != NULL)
            {
                *pBEnable = pGpuMgr->probedGpus[i].bDrainState;
            }

            if (pBRemove != NULL)
            {
                *pBRemove = pGpuMgr->probedGpus[i].bRemoveIdle;
            }

            break;
        }
    }

    portSyncMutexRelease(pGpuMgr->probedGpusLock);

    //
    // This used to return a generic NV_ERR_INVALID_ARGUMENT on error, but we
    // want to be more specific as at least nvml wants to be able to tell this
    // case apart from other errors. This case is expected when GPUs are
    // removed from the driver (e.g. through unbind on Linux) after a client
    // queries for the probed GPUs, but before getting the PCI info for all of
    // them.
    //
    return (i == NV_MAX_DEVICES) ? NV_ERR_OBJECT_NOT_FOUND : NV_OK;
}

/*!
* @brief Retrieves the group gpuMask that contains this gpuInstance.
*        Used for locking all gpus under the same device together
*
* @param[in] gpuInstance: unique Index per GPU
*
* @return        gpuMask: mask of all GPU that are in the same group
*
*/
NvU32
gpumgrGetGrpMaskFromGpuInst
(
    NvU32      gpuInst
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    NV_ASSERT_OR_RETURN(gpuInst < NV_MAX_DEVICES, 0);

    return pGpuMgr->gpuInstMaskTable[gpuInst];
}

/*!
* @brief Updates per GPU isnstance table to contain correct group mask
*
* @param[in] gpuMask: mask of all GPUs that are in the same group
*
*/
void
gpumgrAddDeviceMaskToGpuInstTable
(
    NvU32 gpuMask
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    int gpuInst;
    NvU32 tempGpuMask = gpuMask;

    for (gpuInst = 0; (tempGpuMask != 0) && (gpuInst < NV_MAX_DEVICES); gpuInst++)
    {
        if (NVBIT(gpuInst) & gpuMask)
            pGpuMgr->gpuInstMaskTable[gpuInst] = gpuMask;

        tempGpuMask &= ~NVBIT(gpuInst);
    }
}

/*!
* @brief Clear group mask from per GPU isnstance table (when group is destroyed)
*
* @param[in] gpuMask: gpu group mask being teared down
*
*/
void
gpumgrClearDeviceMaskFromGpuInstTable
(
    NvU32 gpuMask
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    int gpuInst;
    NvU32 tempGpuMask = gpuMask;

    for (gpuInst = 0; (tempGpuMask != 0) && (gpuInst < NV_MAX_DEVICES); gpuInst++)
    {
        if (NVBIT(gpuInst) & gpuMask)
            pGpuMgr->gpuInstMaskTable[gpuInst] = 0;

        tempGpuMask &= ~NVBIT(gpuInst);
    }
}

/*!
* @brief Add an nvlink topo entry by a GPU's PCI DomainBusDevice if not already present
*
* @param[in] DomainBusDevice: the PCI DomainBusDevice for the gpu to be registered
*
*/
void
gpumgrAddSystemNvlinkTopo_IMPL
(
    NvU64 DomainBusDevice
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    if (gpumgrGetSystemNvlinkTopo(DomainBusDevice, NULL))
    {
        // This gpu is already registered
        return;
    }

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        // add new gpu entry into the first slot available
        if (!pGpuMgr->nvlinkTopologyInfo[i].valid)
        {
            pGpuMgr->nvlinkTopologyInfo[i].valid = NV_TRUE;
            pGpuMgr->nvlinkTopologyInfo[i].DomainBusDevice = DomainBusDevice;
            return;
        }
    }
}

/*!
* @brief Finds nvlinkTopologyInfo entry info based on DomainBusDevice
*
* @param[in]  DomainBusDevice: the PCI DomainBusDevice for the gpu to be registered
* @param[out] pTopoParams: topology params found for the specified GPU
*
* @returns NV_TRUE if entry found
*          NV_FALSE otherwise
*/
NvBool
gpumgrGetSystemNvlinkTopo_IMPL
(
    NvU64                   DomainBusDevice,
    PNVLINK_TOPOLOGY_PARAMS pTopoParams
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i, idx;

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        //
        // Choose the correct GPU by comparing PCI BusDomainDevice
        // This ensures we are using the same GPU across gpu load/unload
        //
        if ((pGpuMgr->nvlinkTopologyInfo[i].valid) && (pGpuMgr->nvlinkTopologyInfo[i].DomainBusDevice == DomainBusDevice))
        {
            if (pTopoParams)
            {
                pTopoParams->sysmemLinks     = pGpuMgr->nvlinkTopologyInfo[i].params.sysmemLinks;
                pTopoParams->maxLinksPerPeer = pGpuMgr->nvlinkTopologyInfo[i].params.maxLinksPerPeer;
                pTopoParams->bSymmetric      = pGpuMgr->nvlinkTopologyInfo[i].params.bSymmetric;
                // Pascal Only
                pTopoParams->numLinks        = pGpuMgr->nvlinkTopologyInfo[i].params.numLinks;
                // Volta+
                pTopoParams->numPeers        = pGpuMgr->nvlinkTopologyInfo[i].params.numPeers;
                pTopoParams->bSwitchConfig   = pGpuMgr->nvlinkTopologyInfo[i].params.bSwitchConfig;
                // Ampere+
                for (idx = 0; idx < NV2080_CTRL_MAX_PCES; idx++)
                {
                    pTopoParams->maxPceLceMap[idx] = pGpuMgr->nvlinkTopologyInfo[i].params.maxPceLceMap[idx];
                }
                for (idx = 0; idx < NV2080_CTRL_MAX_GRCES; idx++)
                {
                    pTopoParams->maxGrceConfig[idx] = pGpuMgr->nvlinkTopologyInfo[i].params.maxGrceConfig[idx];
                }
                pTopoParams->maxExposeCeMask = pGpuMgr->nvlinkTopologyInfo[i].params.maxExposeCeMask;
                pTopoParams->maxTopoIdx = pGpuMgr->nvlinkTopologyInfo[i].params.maxTopoIdx;
                for (idx = 0; idx < NV2080_CTRL_CE_MAX_HSHUBS; idx++)
                {
                    pTopoParams->pceAvailableMaskPerConnectingHub[idx] =
                        pGpuMgr->nvlinkTopologyInfo[i].params.pceAvailableMaskPerConnectingHub[idx];
                }
            }
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
* @brief Update the nvlinkTopologyInfo entry
*
* @param[in] DomainBusDevice: the PCI DomainBusDevice for the gpu to be registered
* @param[in] pTopoParams: topology params to update the cache with
*
*/
void
gpumgrUpdateSystemNvlinkTopo_IMPL
(
    NvU64                   DomainBusDevice,
    PNVLINK_TOPOLOGY_PARAMS pTopoParams
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i, idx;

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        //
        // Choose the correct GPU by comparing PCI BusDomainDevice
        // This ensures we are using the same GPU across gpu load/unload
        //
        if ((pGpuMgr->nvlinkTopologyInfo[i].valid) && (pGpuMgr->nvlinkTopologyInfo[i].DomainBusDevice == DomainBusDevice))
        {
            pGpuMgr->nvlinkTopologyInfo[i].params.sysmemLinks     = pTopoParams->sysmemLinks;
            pGpuMgr->nvlinkTopologyInfo[i].params.maxLinksPerPeer = pTopoParams->maxLinksPerPeer;
            pGpuMgr->nvlinkTopologyInfo[i].params.numLinks        = pTopoParams->numLinks; // Pascal only
            pGpuMgr->nvlinkTopologyInfo[i].params.numPeers        = pTopoParams->numPeers;
            pGpuMgr->nvlinkTopologyInfo[i].params.bSymmetric      = pTopoParams->bSymmetric;
            pGpuMgr->nvlinkTopologyInfo[i].params.bSwitchConfig   = pTopoParams->bSwitchConfig;
            // Ampere + only
            for (idx = 0; idx < NV2080_CTRL_MAX_PCES; idx++)
            {
                pGpuMgr->nvlinkTopologyInfo[i].params.maxPceLceMap[idx] = pTopoParams->maxPceLceMap[idx];
            }
            for (idx = 0; idx < NV2080_CTRL_MAX_GRCES; idx++)
            {
                pGpuMgr->nvlinkTopologyInfo[i].params.maxGrceConfig[idx] = pTopoParams->maxGrceConfig[idx];
            }
            pGpuMgr->nvlinkTopologyInfo[i].params.maxExposeCeMask = pTopoParams->maxExposeCeMask;
            pGpuMgr->nvlinkTopologyInfo[i].params.maxTopoIdx = pTopoParams->maxTopoIdx;
            for (idx = 0; idx < NV2080_CTRL_CE_MAX_HSHUBS; idx++)
            {
                pGpuMgr->nvlinkTopologyInfo[i].params.pceAvailableMaskPerConnectingHub[idx] =
                    pTopoParams->pceAvailableMaskPerConnectingHub[idx];
            }
            return;
        }
    }
}

NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *
gpumgrGetNvlinkRecoveryInfo_IMPL
(
    NvU64 DomainBusDevice
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        //
        // Choose the correct GPU by comparing PCI BusDomainDevice
        // If no matching entry is found, pick the first invalid one.
        //
        if (!pGpuMgr->nvlinkUncontainedErrorRecoveryInfo[i].bValid ||
            (pGpuMgr->nvlinkUncontainedErrorRecoveryInfo[i].DomainBusDevice == DomainBusDevice))
        {
            return &pGpuMgr->nvlinkUncontainedErrorRecoveryInfo[i];
        }
    }

    // If no entries available, bail.
    return NULL;
}

/*!
* @brief Check if GPUs are indirect peers
*
* @param[in] pGpu
* @param[in] pRemoteGpu
* @returns  NV_TRUE if GPUs are indirect peers
*
*/
NvBool
gpumgrCheckIndirectPeer_IMPL
(
    OBJGPU *pGpu,
    OBJGPU *pRemoteGpu
)
{
#if defined(NVCPU_PPC64LE)
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (pKernelNvlink == NULL)
    {
        return NV_FALSE;
    }

    // If it's the same GPU or GPUs belonging to same SLI group, return early
    if ((pGpu == pRemoteGpu) ||
        (pGpu->deviceInstance == pRemoteGpu->deviceInstance))
    {
        return NV_FALSE;
    }

    // If we are not on P9 + NVLINK2 systems then we don't support indirect peers
    if ((GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)->coherentCpuFbBase == 0) ||
        (GPU_GET_KERNEL_MEMORY_SYSTEM(pRemoteGpu)->coherentCpuFbBase == 0))
    {
        return NV_FALSE;
    }

    return !(knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu));
#else
    return NV_FALSE;
#endif
}

/*!
 * @brief Set NVLinks (mask) for which initialization is disabled.
 *
 * @param[in]  gpuId        Platform specific GPU Id.
 * @param[in]  mask         Mask representing the links to be disabled.
 *
 * @return NV_OK on success, appropriate error on failure.
 */
NV_STATUS
gpumgrSetGpuInitDisabledNvlinks_IMPL
(
    NvU32   gpuId,
    NvU32   mask,
    NvBool  bSkipHwNvlinkDisable
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS   status = NV_ERR_INVALID_DEVICE;
    NvU32 i;

    if (gpumgrGetGpuFromId(gpuId) != NULL)
    {
        return NV_ERR_IN_USE;
    }

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); ++i)
    {
        if (pGpuMgr->probedGpus[i].gpuId == gpuId)
        {
            // Mask will be validated during Nvlink construct.
            pGpuMgr->probedGpus[i].initDisabledNvlinksMask = mask;
            pGpuMgr->probedGpus[i].bSkipHwNvlinkDisable = bSkipHwNvlinkDisable;
            status = NV_OK;
            break;
        }
    }

    portSyncMutexRelease(pGpuMgr->probedGpusLock);

    return status;
}

/*!
 * @brief Get NVLinks (mask) for which initialization is disabled.
 *
 * @param[in]  gpuId        Platform specific GPU Id.
 * @param[out]  mask        Mask representing the links to be disabled.
 *
 * @return NV_OK on success, appropriate error on failure.
 */
NV_STATUS
gpumgrGetGpuInitDisabledNvlinks_IMPL
(
    NvU32   gpuId,
    NvU32   *pMask,
    NvBool  *pbSkipHwNvlinkDisable
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS   status = NV_ERR_INVALID_DEVICE;
    NvU32 i;

    portSyncMutexAcquire(pGpuMgr->probedGpusLock);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->probedGpus); ++i)
    {
        if (pGpuMgr->probedGpus[i].gpuId == gpuId)
        {
            *pMask = pGpuMgr->probedGpus[i].initDisabledNvlinksMask;
            *pbSkipHwNvlinkDisable = pGpuMgr->probedGpus[i].bSkipHwNvlinkDisable;
            status = NV_OK;
            break;
        }
    }

    portSyncMutexRelease(pGpuMgr->probedGpusLock);

    return status;
}

/*!
 * @brief Get nvlink bandwidth mode
 *
 * @return mode     reduced bandwidth mode.
 */
NvU8
gpumgrGetGpuNvlinkBwMode_IMPL(void)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);

    return pGpuMgr->nvlinkBwMode;
}

/*!
 * @brief Get nvlink bandwidth mode scope
 *
 * @return bwModeScope     reduced bandwidth mode scope.
 */
NvU8
gpumgrGetGpuNvlinkBwModeScope_IMPL(void)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);

    return pGpuMgr->bwModeScope;
}

/*!
 * @brief Set nvlink bandwidth mode from Registry
 *
 * @param[in]  pGpu   reference of OBJGPU
 *
 */
void
gpumgrSetGpuNvlinkBwModeFromRegistry_IMPL
(
    OBJGPU *pGpu
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    const char *pStrChar;
    NvU32 strLength = 32;
    NvU8 pStr[32];
    NvU32 linkCount;

    //
    // An RM client can set NVLink BW mode using
    // NV0000_CTRL_CMD_GPU_SET_NVLINK_BW_MODE control call.
    // If the value is not default i.e. `GPU_NVLINK_BW_MODE_FULL`, then skip.
    // The scope would also be set to GPU_NVLINK_BW_MODE_SCOPE_UNSET if no
    // PER_GPU setting has been made via NV2080_CTRL_CMD_GPU_SET_NVLINK_BW_MODE.
    // Skip if it is not.
    //
    if (pGpuMgr->nvlinkBwMode != GPU_NVLINK_BW_MODE_FULL ||
        pGpuMgr->bwModeScope != GPU_NVLINK_BW_MODE_SCOPE_UNSET)
    {

        NV_PRINTF(LEVEL_ERROR, "BW mode already set. Cannot override with regkey.\n");
        return;
    }

    // sysInitRegistryOverrides should pass in valid pGpu
    NV_ASSERT (pGpu != NULL);

    if (osReadRegistryString(pGpu, NV_REG_STR_RM_NVLINK_BW, pStr, &strLength) != NV_OK)
    {
        goto out;
    }

    pStrChar = (const char *)pStr;
    strLength = portStringLength(pStrChar);
    if (portStringCompare(pStrChar, "OFF", strLength) == 0)
    {
        pGpuMgr->nvlinkBwMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE,
                                                GPU_NVLINK_BW_MODE_OFF,
                                                pGpuMgr->nvlinkBwMode);
    }
    else if (portStringCompare(pStrChar, "MIN", strLength) == 0)
    {
        pGpuMgr->nvlinkBwMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE,
                                                GPU_NVLINK_BW_MODE_MIN,
                                                pGpuMgr->nvlinkBwMode);
    }
    else if (portStringCompare(pStrChar, "HALF", strLength) == 0)
    {
        pGpuMgr->nvlinkBwMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE,
                                                GPU_NVLINK_BW_MODE_HALF,
                                                pGpuMgr->nvlinkBwMode);
    }
    else if (portStringCompare(pStrChar, "3QUARTER", strLength) == 0)
    {
        pGpuMgr->nvlinkBwMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE,
                                                GPU_NVLINK_BW_MODE_3QUARTER,
                                                pGpuMgr->nvlinkBwMode);
    }
    else if (portStringCompare(pStrChar, "LINKCOUNT", strLength) == 0)
    {
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_NVLINK_BW_LINK_COUNT, &linkCount) == NV_OK)
        {
            pGpuMgr->nvlinkBwMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE,
                                                    GPU_NVLINK_BW_MODE_LINK_COUNT,
                                                    pGpuMgr->nvlinkBwMode);
            pGpuMgr->nvlinkBwMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE_LINK_COUNT,
                                                    linkCount, pGpuMgr->nvlinkBwMode);
            pGpuMgr->bwModeScope = GPU_NVLINK_BW_MODE_SCOPE_PER_NODE;
        }
    }

out:
    NV_PRINTF(LEVEL_INFO, "nvlinkBwMode=%d\n", pGpuMgr->nvlinkBwMode);
}

static NvBool
_gpumgrIsP2PObjectPresent(void)
{
    NvU32 attachedGpuCount;
    NvU32 attachedGpuMask;
    KernelBus *pKernelBus;
    NV_STATUS rmStatus;
    NvU32 gpuIndex;
    OBJGPU *pGpu;

    rmStatus = gpumgrGetGpuAttachInfo(&attachedGpuCount, &attachedGpuMask);
    if (rmStatus != NV_OK)
    {
        return NV_FALSE;
    }

    gpuIndex = 0;
    for(pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex);
        pGpu != NULL;
        pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex))
    {
        pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
        if (pKernelBus->totalP2pObjectsAliveRefCount > 0)
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Set nvlink bandwidth mode for PER_GPU scope
 * TODO: Move RBM code into it's own source file
 *
 * @param[in]  mode        nvlink bandwidth mode
 *
 * @return NV_OK on success, appropriate error on failure.
 */
NV_STATUS
gpumgrSetGpuNvlinkBwModePerGpu_IMPL
(
    OBJGPU *pGpu,
    NvU8 mode
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (pGpuMgr->bwModeScope == GPU_NVLINK_BW_MODE_SCOPE_PER_NODE)
    {
        // Scope is set to PER_NODE.
        NV_PRINTF(LEVEL_ERROR, "Unable to override bw mode setting in current scope.\n");
        return NV_ERR_IN_USE;
    }

    if (mode == pKernelNvlink->nvlinkBwMode)
    {
        // Current link count matches requested link count
        NV_PRINTF(LEVEL_ERROR, "Requested RBM link count is already set.\n");
        return NV_OK;
    }

    if (_gpumgrIsP2PObjectPresent())
    {
        return NV_ERR_IN_USE;
    }

    // Set requested rbm link count for gpu
    NV_ASSERT_OK_OR_RETURN(gpuFabricProbeSetBwModePerGpu(pGpu, mode));

    //
    // TODO: Need to check if all GPU BW modes are _FULL when requested mode
    // is _FULL and if so change bwModeScope to _UNSET
    //
    pGpuMgr->bwModeScope = GPU_NVLINK_BW_MODE_SCOPE_PER_GPU;

    return NV_OK;
}

/*!
 * @brief Set nvlink bandwidth mode
 *
 * @param[in]  mode        nvlink bandwidth mode
 *
 * @return NV_OK on success, appropriate error on failure.
 */
NV_STATUS
gpumgrSetGpuNvlinkBwMode_IMPL
(
    NvU8 mode
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS  status;

    if (pGpuMgr->bwModeScope == GPU_NVLINK_BW_MODE_SCOPE_PER_GPU)
    {
        // Scope is set to PER_GPU.
        NV_PRINTF(LEVEL_ERROR, "Unable to override bw mode setting in current scope\n");
        return NV_ERR_IN_USE;
    }

    if (mode == pGpuMgr->nvlinkBwMode)
    {
        NV_PRINTF(LEVEL_INFO, "Requested BW mode is already set.\n");
        return NV_OK;
    }

    if (_gpumgrIsP2PObjectPresent())
    {
        return NV_ERR_IN_USE;
    }

    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) > GPU_NVLINK_BW_MODE_3QUARTER)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = gpuFabricProbeSetBwMode(mode);
    if (status != NV_OK)
    {
        return status;
    }

    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_FULL)
    {
        pGpuMgr->bwModeScope = GPU_NVLINK_BW_MODE_SCOPE_UNSET;
        goto done;
    }
    pGpuMgr->bwModeScope = GPU_NVLINK_BW_MODE_SCOPE_PER_NODE;

done:
    pGpuMgr->nvlinkBwMode = mode;
    return NV_OK;
}

/*!
 * @brief Adds an entry in the system partition topology save for the given GPU
 *        ID. Note that this does not create any saved partition topology.
 *
 * @param[in] DomainBusDevice: the PCI DomainBusDevice for the gpu to be registered
 */
void
gpumgrAddSystemMIGInstanceTopo_IMPL
(
    NvU64 domainBusDevice
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    if (gpumgrGetSystemMIGInstanceTopo(domainBusDevice, NULL))
    {
        // This gpu is already registered
        return;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->MIGTopologyInfo); i++)
    {
        // add new gpu entry into the first slot available
        if (!pGpuMgr->MIGTopologyInfo[i].bValid)
        {
            pGpuMgr->MIGTopologyInfo[i].bValid = NV_TRUE;
            pGpuMgr->MIGTopologyInfo[i].domainBusDevice = domainBusDevice;

            // Set MIG enablement to disabled by default
            pGpuMgr->MIGTopologyInfo[i].bMIGEnabled = NV_FALSE;
            break;
        }
    }

    // Shouldn't be possible to not find an open slot
    NV_ASSERT(i < NV_ARRAY_ELEMENTS(pGpuMgr->MIGTopologyInfo));
}

/*!
 * @brief Retrieves the entry in the system partition topology save for the given GPU
 *        ID. Note that retrieval of this entry does not constitute a guarantee
 *        that there is any valid data saved.
 *
 * @param[in]  DomainBusDevice: the PCI DomainBusDevice for the gpu to be registered
 * @param[out] ppTopology: Stores the saved MIG topology for the given GPU, if
 *                         found.
 *
 * @returns NV_TRUE if entry found
 *          NV_FALSE otherwise
 */
NvBool
gpumgrGetSystemMIGInstanceTopo_IMPL
(
    NvU64 domainBusDevice,
    GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY **ppTopology
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->MIGTopologyInfo); i++)
    {
        //
        // Choose the correct GPU by comparing PCI BusDomainDevice
        // This ensures we are using the same GPU across gpu load/unload
        //
        if (!pGpuMgr->MIGTopologyInfo[i].bValid ||
            (pGpuMgr->MIGTopologyInfo[i].domainBusDevice != domainBusDevice))
        {
            continue;
        }

        if (NULL != ppTopology)
            *ppTopology = &pGpuMgr->MIGTopologyInfo[i];

        return NV_TRUE;
    }

    return NV_FALSE;
}


/*!
 * @brief Retrieves the entry in the system partition topology save for the given GPU
 *        ID and returns value of bMIGEnabled.
 *
 * @param[in]  domainBusDevice: the PCI DomainBusDevice for the gpu to be registered
 *
 * @returns NV_TRUE if entry found
 *          NV_FALSE otherwise
 */
NvBool
gpumgrIsSystemMIGEnabled_IMPL
(
    NvU64 domainBusDevice
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->MIGTopologyInfo); i++)
    {
        //
        // Choose the correct GPU by comparing PCI BusDomainDevice
        // This ensures we are using the same GPU across gpu load/unload
        //
        if (pGpuMgr->MIGTopologyInfo[i].bValid &&
            (pGpuMgr->MIGTopologyInfo[i].domainBusDevice == domainBusDevice))
        {
            return pGpuMgr->MIGTopologyInfo[i].bMIGEnabled;
        }
    }

    return NV_FALSE;
}


/*!
 * @brief Retrieves the entry in the system partition topology save for the given GPU
 *        ID and sets value of bMIGEnabled.
 *
 * @param[in]  DomainBusDevice: the PCI DomainBusDevice for the gpu to be registered
 * @param[in]  bMIGEnabled:     The new MIG enablement state to be set
 *
 * @returns NV_TRUE if entry found
 *          NV_FALSE otherwise
 */
void
gpumgrSetSystemMIGEnabled_IMPL
(
    NvU64 domainBusDevice,
    NvBool bMIGEnabled
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpuMgr->MIGTopologyInfo); i++)
    {
        //
        // Choose the correct GPU by comparing PCI BusDomainDevice
        // This ensures we are using the same GPU across gpu load/unload
        //
        if (pGpuMgr->MIGTopologyInfo[i].bValid &&
            (pGpuMgr->MIGTopologyInfo[i].domainBusDevice == domainBusDevice))
        {
            pGpuMgr->MIGTopologyInfo[i].bMIGEnabled = bMIGEnabled;
            break;
        }
    }
}

static void
_gpumgrUnregisterRmCapsForMIGCI(GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave)
{
    GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSave;
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGPUInstanceSave->saveCI); i++)
    {
        pComputeInstanceSave = &pGPUInstanceSave->saveCI[i];

        if (pComputeInstanceSave->bValid)
        {
            osRmCapUnregister(&pComputeInstanceSave->pOsRmCaps);

            //
            // Mark invalid as the partition caps have been unregistered from RM
            // completely.
            //
            pComputeInstanceSave->bValid = NV_FALSE;
        }
    }
}

void
gpumgrUnregisterRmCapsForMIGGI_IMPL(NvU64 gpuDomainBusDevice)
{
    GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY *pTopologySave;
    GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave;
    NvU32 i;

    if (!gpumgrGetSystemMIGInstanceTopo(gpuDomainBusDevice, &pTopologySave))
    {
        return;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(pTopologySave->saveGI); i++)
    {
        pGPUInstanceSave = &pTopologySave->saveGI[i];

        if (pGPUInstanceSave->bValid)
        {
            _gpumgrUnregisterRmCapsForMIGCI(pGPUInstanceSave);

            osRmCapUnregister(&pGPUInstanceSave->pOsRmCaps);

            //
            // Mark invalid as the partition caps have been unregistered from RM
            // completely.
            //
            pGPUInstanceSave->bValid = NV_FALSE;
        }
    }
}

void
gpumgrCacheCreateGpuInstance_IMPL(OBJGPU *pGpu, NvU32 swizzId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    GPUMGR_CACHED_MIG_GPU_INSTANCE *pGpuInstances = NULL;
    NvU32 i;

    portSyncRwLockAcquireWrite(pGpuMgr->cachedMIGInfoLock);

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (pGpuMgr->cachedMIGInfo[i].gpuId == pGpu->gpuId)
        {
            pGpuInstances = pGpuMgr->cachedMIGInfo[i].gpuInstances;
            break;
        }
    }

    NV_ASSERT_OR_GOTO(pGpuInstances != NULL, done);

    for (i = 0; i < GPUMGR_MAX_GPU_INSTANCES; i++)
    {
        if (!pGpuInstances[i].bValid)
        {
            pGpuInstances[i].bValid = NV_TRUE;
            pGpuInstances[i].swizzId = swizzId;
            goto done;
        }
    }

    NV_ASSERT(!"Unreachable");

done:
    portSyncRwLockReleaseWrite(pGpuMgr->cachedMIGInfoLock);
}

void
gpumgrCacheDestroyGpuInstance_IMPL(OBJGPU *pGpu, NvU32 swizzId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    GPUMGR_CACHED_MIG_GPU_INSTANCE *pGpuInstances = NULL;
    NvU32 i;

    portSyncRwLockAcquireWrite(pGpuMgr->cachedMIGInfoLock);

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (pGpuMgr->cachedMIGInfo[i].gpuId == pGpu->gpuId)
        {
            pGpuInstances = pGpuMgr->cachedMIGInfo[i].gpuInstances;
            break;
        }
    }

    NV_ASSERT_OR_GOTO(pGpuInstances != NULL, done);

    for (i = 0; i < GPUMGR_MAX_GPU_INSTANCES; i++)
    {
        if (pGpuInstances[i].bValid &&
            pGpuInstances[i].swizzId == swizzId)
        {
            pGpuInstances[i].bValid = NV_FALSE;
            goto done;
        }
    }

    NV_ASSERT(!"Unreachable");

done:
    portSyncRwLockReleaseWrite(pGpuMgr->cachedMIGInfoLock);
}

void
gpumgrCacheCreateComputeInstance_IMPL(OBJGPU *pGpu, NvU32 swizzId, NvU32 ciId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    GPUMGR_CACHED_MIG_GPU_INSTANCE *pGpuInstances = NULL;
    NvU32 i;

    portSyncRwLockAcquireWrite(pGpuMgr->cachedMIGInfoLock);

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (pGpuMgr->cachedMIGInfo[i].gpuId == pGpu->gpuId)
        {
            pGpuInstances = pGpuMgr->cachedMIGInfo[i].gpuInstances;
            break;
        }
    }

    NV_ASSERT_OR_GOTO(pGpuInstances != NULL, done);

    for (i = 0; i < GPUMGR_MAX_GPU_INSTANCES; i++)
    {
        if (pGpuInstances[i].bValid &&
            pGpuInstances[i].swizzId == swizzId)
        {
            pGpuInstances[i].bValidComputeInstances[ciId] = NV_TRUE;
            goto done;
        }
    }

    NV_ASSERT(!"Unreachable");

done:
    portSyncRwLockReleaseWrite(pGpuMgr->cachedMIGInfoLock);
}

void
gpumgrCacheDestroyComputeInstance_IMPL(OBJGPU *pGpu, NvU32 swizzId, NvU32 ciId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    GPUMGR_CACHED_MIG_GPU_INSTANCE *pGpuInstances = NULL;
    NvU32 i;

    portSyncRwLockAcquireWrite(pGpuMgr->cachedMIGInfoLock);

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (pGpuMgr->cachedMIGInfo[i].gpuId == pGpu->gpuId)
        {
            pGpuInstances = pGpuMgr->cachedMIGInfo[i].gpuInstances;
            break;
        }
    }

    NV_ASSERT_OR_GOTO(pGpuInstances != NULL, done);

    for (i = 0; i < GPUMGR_MAX_GPU_INSTANCES; i++)
    {
        if (pGpuInstances[i].bValid &&
            pGpuInstances[i].swizzId == swizzId)
        {
            pGpuInstances[i].bValidComputeInstances[ciId] = NV_FALSE;
            goto done;
        }
    }

    NV_ASSERT(!"Unreachable");

done:
    portSyncRwLockReleaseWrite(pGpuMgr->cachedMIGInfoLock);
}

void
gpumgrCacheSetMIGEnabled_IMPL(OBJGPU *pGpu, NvBool bMIGEnabled)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    GPUMGR_CACHED_MIG_STATE *pMIGInfo = NULL;
    NvU32 gpuId = pGpu->gpuId;
    NvU32 i;

    portSyncRwLockAcquireWrite(pGpuMgr->cachedMIGInfoLock);

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (pGpuMgr->cachedMIGInfo[i].bValid &&
            (pGpuMgr->cachedMIGInfo[i].gpuId == gpuId))
        {
            pMIGInfo = &pGpuMgr->cachedMIGInfo[i];
            break;
        }
    }

    if (pMIGInfo == NULL)
    {
        // Get first invalid entry, for not yet seen gpuId.
        for (i = 0; i < NV_MAX_DEVICES; i++)
        {
            if (!pGpuMgr->cachedMIGInfo[i].bValid)
            {
                pMIGInfo = &pGpuMgr->cachedMIGInfo[i];
                pMIGInfo->bValid = NV_TRUE;
                pMIGInfo->gpuId = gpuId;
                break;
            }
        }
    }

    NV_ASSERT_OR_GOTO(pMIGInfo != NULL, done);

    pMIGInfo->bMIGEnabled = bMIGEnabled;
    if (!bMIGEnabled)
    {
        portMemSet(&pMIGInfo->gpuInstances, 0x0, sizeof(pMIGInfo->gpuInstances));
    }

done:
    portSyncRwLockReleaseWrite(pGpuMgr->cachedMIGInfoLock);
}

NV_STATUS
gpumgrCacheGetActiveDeviceIds_IMPL
(
    NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS *pActiveDeviceIdsParams
)
{
    NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS probedGpuIds;
    NV0000_CTRL_GPU_ACTIVE_DEVICE *pDevices = pActiveDeviceIdsParams->devices;
    NvU32 *pNumDevices = &pActiveDeviceIdsParams->numDevices;
    NvU32 total = 0;
    NvU32 i;

    NV_ASSERT_OK_OR_RETURN(gpumgrGetProbedGpuIds(&probedGpuIds));

    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS status = NV_OK;

    portSyncRwLockAcquireRead(pGpuMgr->cachedMIGInfoLock);

    // Walk probed gpus.
    for (i = 0;
         (i < NV0000_CTRL_GPU_MAX_PROBED_GPUS) &&
         (probedGpuIds.gpuIds[i] != NV0000_CTRL_GPU_INVALID_ID); i++)
    {
        NvU32 gpuId = probedGpuIds.gpuIds[i];
        NvBool bGpuHandled = NV_FALSE;
        NvU32 gpuIdx;

        for (gpuIdx = 0; gpuIdx < NV_MAX_DEVICES; gpuIdx++)
        {
            NvU32 giIdx;
            GPUMGR_CACHED_MIG_STATE *pMIGState = &pGpuMgr->cachedMIGInfo[gpuIdx];
            GPUMGR_CACHED_MIG_GPU_INSTANCE *pGpuInstances = pMIGState->gpuInstances;

            if (!pMIGState->bValid || (pMIGState->gpuId != gpuId))
            {
                continue;
            }

            // MIG not enabled, add device to list and carry on
            if (!pMIGState->bMIGEnabled)
            {
                break;
            }

            bGpuHandled = NV_TRUE;

            for (giIdx = 0; giIdx < GPUMGR_MAX_GPU_INSTANCES; giIdx++)
            {
                NvU32 ciIdx;
                NvU32 swizzId = pGpuInstances[giIdx].swizzId;
                NvBool *pbValidComputeInstances =
                    pGpuInstances[giIdx].bValidComputeInstances;

                if (!pGpuInstances[giIdx].bValid)
                    continue;

                for (ciIdx = 0; ciIdx < GPUMGR_MAX_COMPUTE_INSTANCES; ciIdx++)
                {
                    if (pbValidComputeInstances[ciIdx])
                    {
                        NV_ASSERT_OR_ELSE(total < NV0000_CTRL_GPU_MAX_ACTIVE_DEVICES,
                                          status = NV_ERR_INVALID_STATE; goto done; );

                        pDevices[total].gpuId = gpuId;
                        pDevices[total].gpuInstanceId = swizzId;
                        pDevices[total].computeInstanceId = ciIdx;
                        total++;
                    }
                }
            }
        }

        // Not in MIG mode, or we never had it in cache.
        if (!bGpuHandled)
        {
            NV_ASSERT_OR_ELSE(total < NV0000_CTRL_GPU_MAX_ACTIVE_DEVICES,
                              status = NV_ERR_INVALID_STATE; goto done; );

            pDevices[total].gpuId = gpuId;
            pDevices[total].gpuInstanceId = NV0000_CTRL_GPU_INVALID_ID;
            pDevices[total].computeInstanceId = NV0000_CTRL_GPU_INVALID_ID;
            total++;
        }
    }

    *pNumDevices = total;

done:
    portSyncRwLockReleaseRead(pGpuMgr->cachedMIGInfoLock);

    return status;
}

/**
 * @brief Saves a pointer to the current GPU instance in thread local storage,
 *        to be logged by NVLOG, until gpumgrSetGpuRelease is called.
 *        Returns a pointer to tls entry (to be passed to gpumgrSetGpuRelease)
 *
 * @param[in] pGpu
 */
NvBool
gpumgrSetGpuAcquire(OBJGPU *pGpu)
{
    NvU32 **ppGpuInstance;
    ppGpuInstance = (NvU32 **)tlsEntryAcquire
                    (TLS_ENTRY_ID_CURRENT_GPU_INSTANCE);
    if (ppGpuInstance)
    {
        *ppGpuInstance = &(pGpu->gpuInstance);
        return NV_TRUE;
    }
    return NV_FALSE;
}

/**
 * @brief Releases the thread local storage for GPU ID.
 */
void
gpumgrSetGpuRelease(void)
{
    tlsEntryRelease(TLS_ENTRY_ID_CURRENT_GPU_INSTANCE);
}

/**
* @brief Returns the type of bridge SLI_BT_*
*/
NvU8
gpumgrGetGpuBridgeType(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    return pGpuMgr->gpuBridgeType;
}

/**
* @brief Init the PCIE P2P info cache
*/
NV_STATUS
gpumgrInitPcieP2PCapsCache_IMPL(OBJGPUMGR* pGpuMgr)
{
    listInitIntrusive(&pGpuMgr->pcieP2PCapsInfoCache);
    pGpuMgr->pcieP2PCapsInfoLock = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
    if (pGpuMgr->pcieP2PCapsInfoLock == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    return NV_OK;
}

/**
* @brief Destroy the PCIE P2P info cache
*/
void
gpumgrDestroyPcieP2PCapsCache_IMPL(OBJGPUMGR* pGpuMgr)
{
    PCIEP2PCAPSINFO *pPcieCapsInfo, *pPcieCapsInfoNext;

    portSyncMutexAcquire(pGpuMgr->pcieP2PCapsInfoLock);

    // Remove and free all entries that have this GPU
    for (pPcieCapsInfo = listHead(&(pGpuMgr->pcieP2PCapsInfoCache));
         pPcieCapsInfo != NULL;
         pPcieCapsInfo = pPcieCapsInfoNext)
    {
        pPcieCapsInfoNext = listNext(&(pGpuMgr->pcieP2PCapsInfoCache), pPcieCapsInfo);
        portMemFree(pPcieCapsInfo);
    }

    listDestroy(&pGpuMgr->pcieP2PCapsInfoCache);
    portSyncMutexRelease(pGpuMgr->pcieP2PCapsInfoLock);

    portSyncMutexDestroy(pGpuMgr->pcieP2PCapsInfoLock);
}

/**
* @brief Add an entry in the PCIE P2P info cache
 * @param[in]   gpuMask             NvU32 value
 * @param[in]   p2pWriteCapsStatus  NvU8 value
 * @param[in]   pP2PReadCapsStatus  NvU8 value
 *
 * @return      NV_OK or NV_ERR_NO_MEMORY
 */
NV_STATUS
gpumgrStorePcieP2PCapsCache_IMPL
(
    NvU32  gpuMask,
    NvU8   p2pWriteCapsStatus,
    NvU8   p2pReadCapsStatus
)
{
    OBJSYS          *pSys     = SYS_GET_INSTANCE();
    OBJGPUMGR       *pGpuMgr  = SYS_GET_GPUMGR(pSys);
    PCIEP2PCAPSINFO *pPcieCapsInfo;
    NvU32            gpuInstance;
    OBJGPU          *pGpu;
    NvU32            gpuCount = 0;
    NvU32            status   = NV_OK;

    portSyncMutexAcquire(pGpuMgr->pcieP2PCapsInfoLock);
    if (_gpumgrGetPcieP2PCapsFromCache(gpuMask, NULL, NULL))
    {
        // Entry already present in cache
        goto exit;
    }

    pPcieCapsInfo = portMemAllocNonPaged(sizeof(PCIEP2PCAPSINFO));
    if (pPcieCapsInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }
    listAppendExisting(&(pGpuMgr->pcieP2PCapsInfoCache), pPcieCapsInfo);

    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        pPcieCapsInfo->gpuId[gpuCount] = pGpu->gpuId;
        gpuCount++;
    }

    pPcieCapsInfo->gpuCount = gpuCount;
    pPcieCapsInfo->p2pWriteCapsStatus = p2pWriteCapsStatus;
    pPcieCapsInfo->p2pReadCapsStatus = p2pReadCapsStatus;

exit:
    portSyncMutexRelease(pGpuMgr->pcieP2PCapsInfoLock);
    return status;
}

/**
 * @brief Get the PCIE P2P info from cache if present
 * - Helper function
 *
 * @param[in]   gpuMask             NvU32 value
 * @param[out]  pP2PWriteCapsStatus NvU8* pointer
 *                  Can be NULL
 * @param[out]  pP2PReadCapsStatus  NvU8* pointer
 *                  Can be NULL
 * Return       bFound              NvBool
 */
static NvBool
_gpumgrGetPcieP2PCapsFromCache
(
    NvU32   gpuMask,
    NvU8   *pP2PWriteCapsStatus,
    NvU8   *pP2PReadCapsStatus)
{
    OBJSYS          *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR       *pGpuMgr = SYS_GET_GPUMGR(pSys);
    PCIEP2PCAPSINFO *pPcieCapsInfo;
    pcieP2PCapsInfoListIter it;
    NvU32            gpuInstance;
    NvU32            gpuCount;
    NvU32            remainingGpuCount;
    OBJGPU          *pGpu;
    NvU32            gpuIdLoop;
    NvBool           bFound = NV_FALSE;

    gpuCount = gpumgrGetSubDeviceCount(gpuMask);

    it = listIterAll(&pGpuMgr->pcieP2PCapsInfoCache);
    while (listIterNext(&it))
    {
        pPcieCapsInfo = it.pValue;
        if (gpuCount != pPcieCapsInfo->gpuCount)
        {
            continue;
        }

        //
        // Same count  of GPUs in gpuId array  and GPU mask.
        // All GPU in the gpuMask must have a match in gpuId[]
        //
        gpuInstance = 0;
        remainingGpuCount = gpuCount;
        while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
        {
            gpuIdLoop = 0;
            while (gpuIdLoop < gpuCount)
            {
                if (pPcieCapsInfo->gpuId[gpuIdLoop] == pGpu->gpuId)
                {
                    remainingGpuCount--;
                    break;
                }
                gpuIdLoop++;
            }
            if (remainingGpuCount == 0)
            {
                break;
            }
        }

        if (remainingGpuCount == 0)
        {
            if (pP2PWriteCapsStatus != NULL)
                *pP2PWriteCapsStatus = pPcieCapsInfo->p2pWriteCapsStatus;
            if (pP2PReadCapsStatus != NULL)
                *pP2PReadCapsStatus = pPcieCapsInfo->p2pReadCapsStatus;
            bFound = NV_TRUE;
            break;
        }
    }
    return bFound;
}

/**
 * @brief Get the PCIE P2P info from cache if present
 * - Take cache locks
 *
 * @param[in]   gpuMask             NvU32 value
 * @param[out]  pP2PWriteCapsStatus NvU8* pointer
 *                  Can be NULL
 * @param[out]  pP2PReadCapsStatus  NvU8* pointer
 *                  Can be NULL
 *
 * return       bFound              NvBool
 */
NvBool
gpumgrGetPcieP2PCapsFromCache_IMPL
(
    NvU32   gpuMask,
    NvU8   *pP2PWriteCapsStatus,
    NvU8   *pP2PReadCapsStatus
)
{
    OBJSYS    *pSys    = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvBool     bFound;

    portSyncMutexAcquire(pGpuMgr->pcieP2PCapsInfoLock);

    bFound = _gpumgrGetPcieP2PCapsFromCache(gpuMask, pP2PWriteCapsStatus, pP2PReadCapsStatus);

    portSyncMutexRelease(pGpuMgr->pcieP2PCapsInfoLock);

    return bFound;
}


/**
 * @brief Remove the PCIE P2P info from cache if present
 *
 * @param[in]   gpuId             NvU32 value
 */
void
gpumgrRemovePcieP2PCapsFromCache_IMPL
(
   NvU32 gpuId
)
{
    OBJSYS          *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR       *pGpuMgr = SYS_GET_GPUMGR(pSys);
    PCIEP2PCAPSINFO *pPcieCapsInfo, *pPcieCapsInfoNext;
    NvU32            gpuIdLoop;

    portSyncMutexAcquire(pGpuMgr->pcieP2PCapsInfoLock);

    // Remove and free all entries that have this GPU
    for (pPcieCapsInfo = listHead(&(pGpuMgr->pcieP2PCapsInfoCache));
         pPcieCapsInfo != NULL;
         pPcieCapsInfo = pPcieCapsInfoNext)
    {
        // As we potentially remove an entry we need to save off the next one.
        pPcieCapsInfoNext = listNext(&(pGpuMgr->pcieP2PCapsInfoCache), pPcieCapsInfo);
        gpuIdLoop = 0;
        while (gpuIdLoop < pPcieCapsInfo->gpuCount)
        {
            if (pPcieCapsInfo->gpuId[gpuIdLoop] == gpuId)
            {
                listRemove(&pGpuMgr->pcieP2PCapsInfoCache, pPcieCapsInfo);
                portMemFree(pPcieCapsInfo);
                // Go to next entry (for loop)
                break;
            }
            gpuIdLoop++;
        }
    }
    portSyncMutexRelease(pGpuMgr->pcieP2PCapsInfoLock);
}

NvBool gpumgrAreAllGpusInOffloadMode(void)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    return pGpuMgr->gpuMonolithicRmMask == 0;
}

NvBool gpumgrIsSafeToReadGpuInfo(void)
{
    //
    // A thread that tears down the GPU must own both the API lock for WRITE
    // and all GPU locks.
    //
    // Conversely, if you hold the API lock (either READ or WRITE), or hold
    // any GPU locks, you know that no GPUs will be freed from under you.
    //

    //
    // NOTE: Currently rmapiLockIsOwner() returns TRUE if you own the lock in
    // either READ or WRITE modes
    //
    return rmapiLockIsOwner() || (rmGpuLocksGetOwnedMask() != 0);
}

//
// Workaround for Bug 3809777. This is a HW bug happening in Ampere and
// Ada GPU's. For these GPU's, after device reset, CRS (Configuration Request
// Retry Status) is being released without waiting for GFW boot completion.
// MSI-X capability in the config space may be inconsistent when GFW boot
// is in progress, so this function checks if MSI-X is allowed.
// For Hopper and above, the CRS will be released after
// GFW boot completion, so the WAR is not needed.
// The bug will be exposed only when GPU is running inside guest in
// pass-through mode.
//
NvBool gpumgrIsDeviceMsixAllowed
(
    RmPhysAddr  bar0BaseAddr,
    NvU32       pmcBoot1,
    NvU32       pmcBoot42
)
{
    OBJSYS          *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR   *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    NvU32            chipArch;

    if ((hypervisorGetHypervisorType(pHypervisor) == OS_HYPERVISOR_UNKNOWN) ||
        !FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _REAL, pmcBoot1))
    {
        return NV_TRUE;
    }

    chipArch = decodePmcBoot42Architecture(pmcBoot42);
    if ((chipArch != NV_PMC_BOOT_42_ARCHITECTURE_AD100) &&
        (chipArch != NV_PMC_BOOT_42_ARCHITECTURE_GA100))
    {
        return NV_TRUE;
    }

    return gpuIsMsixAllowed_TU102(bar0BaseAddr);
}

//
// Workaround for Bug 5041782
// There is a BAR firewall that will prevent any reads/writes to the BAR
// register space while a reset is still pending. We must wait for this
// to drop. Return true if the wait is successful, false otherwise
//
NvBool gpumgrWaitForBarFirewall
(
    NvU32 domain,
    NvU8  bus,
    NvU8  device,
    NvU8  function,
    NvU16 devId
)
{

    if (
        (devId >= 0x2900 && devId <= 0x29FF)
        || (devId >= 0x3180 && devId <= 0x327F)
        )
    {
        return gpuWaitForBarFirewall_GB100(domain, bus, device, function);
    }

    if (devId >= 0x2B80 && devId <= 0x2F7F)
    {
        return gpuWaitForBarFirewall_GB202(domain, bus, device, function);
    }

    return NV_TRUE;
}

