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

#include "core/system.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/bif/kernel_bif.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/gpu.h"
#include "platform/chipset/chipset.h"
#include "platform/p2p/p2p_caps.h"
#include "nvRmReg.h"
#include "nvlimits.h"

/**
 * @brief Determines if the GPUs are P2P compatible
 *
 * @param[in] pGpu0
 * @param[in] pGpu1
 *
 * @return NV_TRUE if the GPUs are P2P compatible
 */
static NvBool
areGpusP2PCompatible(OBJGPU *pGpu0, OBJGPU *pGpu1)
{
    // Mark GPUs of different arch or impl incapable of P2P over pcie
    if ((gpuGetChipArch(pGpu0) != gpuGetChipArch(pGpu1)) ||
        (gpuGetChipImpl(pGpu0) != gpuGetChipImpl(pGpu1)))
    {
        return NV_FALSE;
    }

    // Mark GPUs of different notebook implementation incapable of P2P over pcie
    if (IsMobile(pGpu0) != IsMobile(pGpu1))
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

NV_STATUS
p2pGetCaps
(
    NvU32 gpuMask,
    NvBool *pP2PWriteCapable,
    NvBool *pP2PReadCapable,
    P2P_CONNECTIVITY *pConnectivity
)
{
    NvU8 p2PWriteCapsStatus;
    NvU8 p2PReadCapsStatus;
    NV_STATUS status;
    P2P_CONNECTIVITY connectivity;

    if ((pP2PWriteCapable == NULL) || (pP2PReadCapable == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = p2pGetCapsStatus(gpuMask, &p2PWriteCapsStatus,
                              &p2PReadCapsStatus, &connectivity
                              );
    if (status != NV_OK)
    {
        return status;
    }

    //
    // The classes like NV50_P2P, NV50_THIRD_PARTY_P2P depends on direct P2P
    // connectivity, hence the check.
    //
    if (!((connectivity == P2P_CONNECTIVITY_PCIE) ||
          (connectivity == P2P_CONNECTIVITY_PCIE_BAR1) ||
          (connectivity == P2P_CONNECTIVITY_NVLINK) ||
          (connectivity == P2P_CONNECTIVITY_C2C)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    *pP2PWriteCapable = (p2PWriteCapsStatus == NV0000_P2P_CAPS_STATUS_OK);
    *pP2PReadCapable = (p2PReadCapsStatus == NV0000_P2P_CAPS_STATUS_OK);

    if (pConnectivity != NULL)
    {
        *pConnectivity = connectivity;
    }

    return status;
}

static NV_STATUS
_kp2pCapsGetStatusIndirectOverNvLink
(
    NvU32 gpuMask,
    NvU8 *pP2PWriteCapStatus,
    NvU8 *pP2PReadCapStatus
)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuInstance  = 0;
    OBJGPU *pFirstGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
    NvBool bIndirectPeers = NV_FALSE;
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pFirstGpu);

    if ((pKernelBif->forceP2PType != NV_REG_STR_RM_FORCE_P2P_TYPE_DEFAULT) &&
        (pKernelBif->forceP2PType != NV_REG_STR_RM_FORCE_P2P_TYPE_NVLINK))
    {
        *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        return NV_OK;
    }

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        bIndirectPeers = gpumgrCheckIndirectPeer(pFirstGpu, pGpu);
        if (!bIndirectPeers)
        {
            break;
        }
    }

    if (bIndirectPeers)
    {
        *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_OK;
        *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_OK;
    }
    else
    {
        *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
    }

    return NV_OK;
}

static NV_STATUS
_gpumgrGetP2PCapsStatusOverNvLink
(
    NvU32 gpuMask,
    NvU8 *pP2PWriteCapStatus,
    NvU8 *pP2PReadCapStatus
)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuInstance  = 0;
    NV_STATUS status;
    OBJGPU *pFirstGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
    RMTIMEOUT timeout;
    NvU32 linkTrainingTimeout = 10000000;
    KernelBif *pKernelBif = NULL;
    KernelNvlink *pKernelNvlink = NULL;

    NV_ASSERT_OR_RETURN(pFirstGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pFirstGpu);
    pKernelBif = GPU_GET_KERNEL_BIF(pFirstGpu);

    if ((pKernelBif->forceP2PType != NV_REG_STR_RM_FORCE_P2P_TYPE_DEFAULT) &&
        (pKernelBif->forceP2PType != NV_REG_STR_RM_FORCE_P2P_TYPE_NVLINK))
    {
        *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        return NV_OK;
    }

    //
    // Re-initialize to check loop back configuration if only single GPU in
    // requested mask.
    //
    gpuInstance = (gpumgrGetSubDeviceCount(gpuMask) > 1) ? gpuInstance : 0;

    // Check NvLink P2P connectivity
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        //
        // If ALI is enabled then poll to make sure that the links have
        // finished training on the two given gpus. If timeout occurs then
        // log an error, but continue on as there could be another gpu pair
        // that could have connectivity
        //

        if ((pKernelNvlink != NULL) &&
            knvlinkDiscoverPostRxDetLinks_HAL(pFirstGpu, pKernelNvlink, pGpu) == NV_OK)
        {
            // Check to make sure that the links are active

            gpuSetTimeout(pGpu, linkTrainingTimeout, &timeout, IS_SILICON(pGpu) ?
                (GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE | GPU_TIMEOUT_FLAGS_DEFAULT) : 0);
            do
            {
                status = gpuCheckTimeout(pGpu, &timeout);

                if (knvlinkCheckTrainingIsComplete(pFirstGpu, pGpu, pKernelNvlink) == NV_OK)
                {
                    break;
                }

                if (status == NV_ERR_TIMEOUT)
                {
                    NV_PRINTF(LEVEL_ERROR,
                            "Links failed to train for the given gpu pairs!\n");
                    return status;
                }
            }
            while(status != NV_ERR_TIMEOUT);
        }

        // Ensure that we can create a NvLink P2P object between the two object
        if ((pKernelNvlink != NULL) &&
            knvlinkIsNvlinkP2pSupported(pFirstGpu, pKernelNvlink, pGpu))
        {
            // Ensure training completes on legacy nvlink devices
            status = knvlinkTrainP2pLinksToActive(pFirstGpu, pGpu, pKernelNvlink);
            NV_ASSERT(status == NV_OK);

            if (status != NV_OK)
            {
                *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
                *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
                return NV_OK;
            }
        }
        else
        {
            *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
            *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
            return NV_OK;
        }
    }

    *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_OK;
    *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_OK;
    return NV_OK;
}

// Returns true if overrides are enabled for PCI-E.
static NvBool
_kp2pCapsCheckStatusOverridesForPcie
(
    NvU32 gpuMask,
    NvU8 *pP2PWriteCapStatus,
    NvU8 *pP2PReadCapStatus
)
{
    KernelBif *pKernelBif  = NULL;
    NvU32      gpuInstance = 0;
    OBJGPU    *pGpu        = NULL;

    // Check overrides for all GPUs in the mask.
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
        if (pKernelBif->p2pOverride != BIF_P2P_NOT_OVERRIDEN)
        {
            switch(DRF_VAL(_REG_STR, _CL_FORCE_P2P, _READ, pKernelBif->p2pOverride))
            {
                case NV_REG_STR_CL_FORCE_P2P_READ_DISABLE:
                    *pP2PReadCapStatus = NV0000_P2P_CAPS_STATUS_DISABLED_BY_REGKEY;
                    break;
                case NV_REG_STR_CL_FORCE_P2P_READ_ENABLE:
                    *pP2PReadCapStatus = NV0000_P2P_CAPS_STATUS_OK;
                    break;
                default:
                    break;
            }

            switch(DRF_VAL(_REG_STR, _CL_FORCE_P2P, _WRITE, pKernelBif->p2pOverride))
            {
                case NV_REG_STR_CL_FORCE_P2P_WRITE_DISABLE:
                    *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_DISABLED_BY_REGKEY;
                    break;
                case NV_REG_STR_CL_FORCE_P2P_WRITE_ENABLE:
                    *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_OK;
                    break;
                default:
                    break;
            }

            return NV_TRUE;
        }

    }

    return NV_FALSE;
}

static NV_STATUS
_kp2pCapsGetStatusOverPcie
(
    NvU32   gpuMask,
    NvU8   *pP2PWriteCapStatus,
    NvU8   *pP2PReadCapStatus
)
{
    OBJGPU *pGpu      = NULL;
    OBJGPU *pFirstGpu = NULL;
    NvU32 gpuInstance = 0;
    KernelBif *pKernelBif  = NULL;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);
    NvU32 iohDomain_ref = 0xFFFFFFFF;
    NvU8 iohBus_ref = 0xFF;
    NvU8 pciSwitchBus = 0, pciSwitchBus_ref = 0xFF;
    NvBool bCommonPciSwitchFound = NV_TRUE;
    NvU16 deviceID;
    NvU8 gpuP2PReadCapsStatus = NV0000_P2P_CAPS_STATUS_OK;
    NvU8 gpuP2PWriteCapsStatus = NV0000_P2P_CAPS_STATUS_OK;
    NvU32 lockedGpuMask = 0;
    NV_STATUS status = NV_OK;

    // Check if any overrides are enabled.
    if (_kp2pCapsCheckStatusOverridesForPcie(gpuMask, pP2PWriteCapStatus,
                                            pP2PReadCapStatus))
    {
        return NV_OK;
    }

    pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
    if (IS_GSP_CLIENT(pGpu))
    {
        if (gpumgrGetPcieP2PCapsFromCache(gpuMask, pP2PWriteCapStatus, pP2PReadCapStatus))
        {
            return NV_OK;
        }
    }

    // PCI-E topology checks
    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        //
        // While PCI-E P2P transactions are forwarded between different
        // root ports implemented within a given Intel I/O hub, they
        // are not forwarded between any two I/O hubs. We must therefore
        // complement the table-driven chipset validation check below
        // with an IOH-specific topology check.
        //
        if (pGpu->gpuClData.rootPort.addr.valid &&
            (pGpu->gpuClData.rootPort.VendorID == PCI_VENDOR_ID_INTEL))
        {
            deviceID = pGpu->gpuClData.rootPort.DeviceID;

            if (((deviceID >= DEVICE_ID_INTEL_3408_ROOT_PORT) &&
                 (deviceID <= DEVICE_ID_INTEL_3411_ROOT_PORT)) ||
                ((deviceID >= DEVICE_ID_INTEL_3C02_ROOT_PORT) &&
                 (deviceID <= DEVICE_ID_INTEL_3C0B_ROOT_PORT)) ||
                ((deviceID >= DEVICE_ID_INTEL_0E02_ROOT_PORT) &&
                 (deviceID <= DEVICE_ID_INTEL_0E0B_ROOT_PORT)) ||
                ((deviceID >= DEVICE_ID_INTEL_2F01_ROOT_PORT) &&
                 (deviceID <= DEVICE_ID_INTEL_2F0B_ROOT_PORT)) ||
                ((deviceID >= DEVICE_ID_INTEL_6F01_ROOT_PORT) &&
                 (deviceID <= DEVICE_ID_INTEL_6F0B_ROOT_PORT)) ||
                (deviceID == DEVICE_ID_INTEL_3420_ROOT_PORT) ||
                 (deviceID == DEVICE_ID_INTEL_3421_ROOT_PORT))
            {
                if (iohDomain_ref == 0xFFFFFFFF)
                {
                    iohDomain_ref = pGpu->gpuClData.rootPort.addr.domain;
                    iohBus_ref = pGpu->gpuClData.rootPort.addr.bus;
                }
                else if ((iohDomain_ref != pGpu->gpuClData.rootPort.addr.domain) ||
                         (iohBus_ref != pGpu->gpuClData.rootPort.addr.bus))
                {
                    *pP2PReadCapStatus = NV0000_P2P_CAPS_STATUS_IOH_TOPOLOGY_NOT_SUPPORTED;
                    *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_IOH_TOPOLOGY_NOT_SUPPORTED;
                    goto done;
                }
            }
        }

        // Test common bridges. Skip first GPU
        if (pFirstGpu == NULL)
        {
            pFirstGpu = pGpu;
            continue;
        }

        if (!areGpusP2PCompatible(pFirstGpu, pGpu))
        {
            *pP2PReadCapStatus  = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
            *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
            goto done;
        }

        // This call returns the most upper bridge
        clFindCommonDownstreamBR(pFirstGpu, pGpu, pCl, &pciSwitchBus);

        if (pciSwitchBus_ref == 0xFF)
        {
            pciSwitchBus_ref = pciSwitchBus;
        }

        // If no bridge found or different to the one previously found
        if ((pciSwitchBus == 0xFF) || (pciSwitchBus_ref != pciSwitchBus))
        {
            bCommonPciSwitchFound = NV_FALSE;
        }
    }

    // Check if GPUs have the HW P2P implementation 

    // Only lock for GSP_CLIENT. Get one GPU.
    if (pFirstGpu == NULL)
    {
        gpuInstance = 0;
        pFirstGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
    }

    if (IS_GSP_CLIENT(pFirstGpu))
    {
        // Lock GPUs
        lockedGpuMask = gpuMask;
        status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
            GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_P2P, &lockedGpuMask);

        // If we get NOTHING_TO_DO, we already have the needed locks, so don't free them
        if (status == NV_WARN_NOTHING_TO_DO)
            lockedGpuMask = 0;
        else if (status != NV_OK)
        {
            lockedGpuMask = 0;
            goto done;
        }
    }

    // Reset P2P caps as statuses will be accumulated below.
    *pP2PReadCapStatus = *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_OK;

    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS p2pCapsParams = {0};

        p2pCapsParams.bCommonPciSwitchFound = bCommonPciSwitchFound;

        NV_ASSERT_OK_OR_GOTO(status, pRmApi->Control(pRmApi,
            pGpu->hInternalClient,
            pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_INTERNAL_GET_PCIE_P2P_CAPS,
           &p2pCapsParams,
            sizeof(NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS)),
            done);

        // GPU specific P2P caps
        pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
        if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED))
            gpuP2PWriteCapsStatus = NV0000_P2P_CAPS_STATUS_GPU_NOT_SUPPORTED;
        if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED))
            gpuP2PReadCapsStatus = NV0000_P2P_CAPS_STATUS_GPU_NOT_SUPPORTED;

        //
        // Reconcile the system and GPU specific P2P information
        // The system P2P status takes precedence
        // Do not override status from not OK to OK
        //
        if (*pP2PReadCapStatus == NV0000_P2P_CAPS_STATUS_OK)
            *pP2PReadCapStatus = (p2pCapsParams.p2pReadCapsStatus == NV0000_P2P_CAPS_STATUS_OK ? gpuP2PReadCapsStatus : p2pCapsParams.p2pReadCapsStatus);
         if (*pP2PWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK)
            *pP2PWriteCapStatus =  (p2pCapsParams.p2pWriteCapsStatus == NV0000_P2P_CAPS_STATUS_OK ? gpuP2PWriteCapsStatus : p2pCapsParams.p2pWriteCapsStatus);

        // No need to continue if P2P is not supported
        if ((*pP2PReadCapStatus != NV0000_P2P_CAPS_STATUS_OK) &&
            (*pP2PWriteCapStatus != NV0000_P2P_CAPS_STATUS_OK))
        {
            break;
        }
    }

done:
    if (lockedGpuMask != 0)
    {
        rmGpuGroupLockRelease(lockedGpuMask, GPUS_LOCK_FLAGS_NONE);
    }

    if (status != NV_OK)
    {
        if (*pP2PReadCapStatus == NV0000_P2P_CAPS_STATUS_OK)
        {
            *pP2PReadCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        }
        if (*pP2PWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK)
        {
            *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
        }
    }

    //
    // Not fatal if failing, effect would be perf degradation as we would not hit the cache.
    // So just assert.
    //
    gpuInstance = 0;
    pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance);
    if (IS_GSP_CLIENT(pGpu))
    {
       NV_ASSERT_OK(gpumgrStorePcieP2PCapsCache(gpuMask, *pP2PWriteCapStatus, *pP2PReadCapStatus));
    }
    return status;
}

static NV_STATUS
_kp2pCapsGetStatusOverPcieBar1
(
    NvU32   gpuMask
)
{
    OBJGPU    *pGpuPeer    = NULL;
    NvU32      gpuInstance = 0;
    OBJGPU    *pFirstGpu   = gpumgrGetNextGpu(gpuMask, &gpuInstance);
    KernelBif *pKernelBif  = GPU_GET_KERNEL_BIF(pFirstGpu);
    NV_STATUS status       = NV_OK;

    if ((pKernelBif->forceP2PType != NV_REG_STR_RM_FORCE_P2P_TYPE_DEFAULT) &&
        (pKernelBif->forceP2PType != NV_REG_STR_RM_FORCE_P2P_TYPE_BAR1P2P))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Re-initialize to check loop back configuration if only single GPU in
    // requested mask.
    //
    gpuInstance = (gpumgrGetSubDeviceCount(gpuMask) > 1) ? gpuInstance : 0;

    while ((pGpuPeer = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (!kbusIsPcieBar1P2PCapable_HAL(pFirstGpu, GPU_GET_KERNEL_BUS(pFirstGpu),
                                          pGpuPeer, GPU_GET_KERNEL_BUS(pGpuPeer)))
        {
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }
    }

    return status;
}

NV_STATUS
p2pGetCapsStatus
(
    NvU32 gpuMask,
    NvU8 *pP2PWriteCapStatus,
    NvU8 *pP2PReadCapStatus,
    P2P_CONNECTIVITY *pConnectivity
)
{
    OBJSYS       *pSys          = SYS_GET_INSTANCE();
    KernelNvlink *pKernelNvlink = NULL;
    OBJGPU       *pGpu          = NULL;
    NvU32         gpuInstance   = 0;

    if ((pP2PWriteCapStatus == NULL) ||
        (pP2PReadCapStatus == NULL)  ||
        (pConnectivity == NULL)
        )
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Default values
    *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
    *pP2PReadCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
    *pConnectivity = P2P_CONNECTIVITY_UNKNOWN;

    // MIG-Nvlink-P2P can be incompatible, so check compatibility for all GPUs
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        NvBool bSmcNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                         kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));

        // If any of the GPU has MIG enabled, return with no P2P support
        if (!bSmcNvLinkP2PSupported)
        {
            NV_PRINTF(LEVEL_ERROR,
                  "P2P is marked unsupported with MIG for GPU instance = 0x%x\n",
                  gpuInstance);
            return NV_OK;
        }
    }

    gpuInstance = 0;

    // Check NvLink P2P connectivity.
    if (_gpumgrGetP2PCapsStatusOverNvLink(gpuMask, pP2PWriteCapStatus,
                                          pP2PReadCapStatus) == NV_OK)
    {
        if (*pP2PWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK &&
            *pP2PReadCapStatus == NV0000_P2P_CAPS_STATUS_OK)
        {
            *pConnectivity = P2P_CONNECTIVITY_NVLINK;
            return NV_OK;
        }
    }

    //
    // On NVSwitch systems, if the NVLink P2P path fails, don't fall back to
    // other P2P paths. To ensure that, check if any GPU in the mask has NVLink
    // support. If supported, enforce NVSwitch/NVLink connectivity by returning
    // NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED.
    //
    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
        if (pKernelNvlink != NULL && pKernelNvlink->discoveredLinks != 0 &&
            (pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
             knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink)))
        {
            *pP2PReadCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
            *pP2PWriteCapStatus = NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED;
            return NV_OK;
        }
    }

    // We didn't find direct P2P, so check for indirect P2P.
    if (_kp2pCapsGetStatusIndirectOverNvLink(gpuMask, pP2PWriteCapStatus,
                                            pP2PReadCapStatus) == NV_OK)
    {
        if ((*pP2PWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK) &&
            (*pP2PReadCapStatus == NV0000_P2P_CAPS_STATUS_OK))
        {
            *pConnectivity = P2P_CONNECTIVITY_NVLINK_INDIRECT;
            return NV_OK;
        }
    }

    //
    // Check PCIE P2P connectivity.
    //
    // We can control P2P connectivity for PCI-E peers using regkeys, hence
    // if either read or write is supported, return success. See
    // _p2pCapsCheckStatusOverridesForPcie for details.
    //
    if (_kp2pCapsGetStatusOverPcie(gpuMask, pP2PWriteCapStatus,
                                  pP2PReadCapStatus) == NV_OK)
    {
        if ((*pP2PWriteCapStatus == NV0000_P2P_CAPS_STATUS_OK) ||
            (*pP2PReadCapStatus == NV0000_P2P_CAPS_STATUS_OK))
        {
            if (_kp2pCapsGetStatusOverPcieBar1(gpuMask) == NV_OK)
                *pConnectivity = P2P_CONNECTIVITY_PCIE_BAR1;
            else
                *pConnectivity = P2P_CONNECTIVITY_PCIE;

            return NV_OK;
        }
    }

    return NV_OK;
}

