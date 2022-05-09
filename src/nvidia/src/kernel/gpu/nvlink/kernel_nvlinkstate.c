/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "os/os.h"

static NV_STATUS _knvlinkCreateIoctrl(OBJGPU *, KernelNvlink *, NvU32);
static NV_STATUS _knvlinkFilterDiscoveredLinks(OBJGPU *, KernelNvlink *);
static NV_STATUS _knvlinkFilterIoctrls(OBJGPU *, KernelNvlink *);
static NV_STATUS _knvlinkProcessSysmemLinks(OBJGPU *, KernelNvlink *, NvBool);
static NV_STATUS _knvlinkPurgeState(OBJGPU *, KernelNvlink *);

/*!
 * @brief Create an IOCTRL object
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelNvlink KernelNvlink object pointer
 * @param[in]   PublicId      The ID of the ioctrl to be created
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkCreateIoctrl
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         PublicId
)
{
    KernelIoctrl *pKernelIoctrl = NULL;
    NV_STATUS     status        = NV_OK;

    status = objCreate(&pKernelIoctrl, pKernelNvlink, KernelIoctrl);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    pKernelNvlink->pKernelIoctrl[PublicId] = pKernelIoctrl;

    status = kioctrlConstructEngine(pGpu, pKernelIoctrl, PublicId);

    return status;
}

/*!
 * @brief Filter the discovered links against disabled links
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelNvlink KernelNvlink object pointer
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkFilterDiscoveredLinks
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    // Ensure any vbios disabled links are removed from discovered
    if (pKernelNvlink->vbiosDisabledLinkMask)
    {
        // Update the link mask if overridden through vbios
        pKernelNvlink->discoveredLinks &= ~(pKernelNvlink->vbiosDisabledLinkMask);

        NV_PRINTF(LEVEL_INFO,
                  "Links discovered after VBIOS overrides = 0x%x\n",
                  pKernelNvlink->discoveredLinks);
    }

    // Filter links that are disabled through regkey overrides
    if (pKernelNvlink->regkeyDisabledLinksMask)
    {
        pKernelNvlink->discoveredLinks &= ~(pKernelNvlink->regkeyDisabledLinksMask);

        NV_PRINTF(LEVEL_INFO,
                  "Links after applying disable links regkey = 0x%x\n",
                  pKernelNvlink->discoveredLinks);
    }

    return NV_OK;
}

/*!
 * @brief Filter the IOCTRLs which have no discovered links
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelNvlink KernelNvlink object pointer
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkFilterIoctrls
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    KernelIoctrl *pKernelIoctrl;
    NvU32         i;

    // Update local IOCTRL discovered masks after vbios and regkey overrides
    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->ioctrlMask)
    {
        pKernelIoctrl = KNVLINK_GET_IOCTRL(pKernelNvlink, i);

        // If minion force boot enabled, don't remove IOCTRL from list
        if (pKernelIoctrl->getProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_FORCE_BOOT))
        {
            continue;
        }

        pKernelIoctrl->localDiscoveredLinks &=
                 KIOCTRL_LINK_GLOBAL_TO_LOCAL_MASK(pKernelNvlink->discoveredLinks);

        // No need to handle the IOCTRL if no links are being enabled
        if (pKernelIoctrl->localDiscoveredLinks == 0x0)
        {
            pKernelNvlink->ioctrlMask &= ~(NVBIT(pKernelIoctrl->PublicId));
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NV_OK;
}

/*!
 * @brief NVLINK ConstructEngine
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 * @param[in] engDesc       NVLink Engine descriptor
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink,
    ENGDESCRIPTOR  engDesc
)
{
    NV_STATUS status    = NV_OK;
    NvU32     ioctrlIdx = 0;

    // Initialize the nvlink core library
    knvlinkCoreDriverLoadWar(pGpu, pKernelNvlink);

    // Return early if nvlink core is not supported
    status = knvlinkCoreIsDriverSupported(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        return status;
    }

    //
    // Apply NVLink regkey overrides for monolithic/CPU-RM.
    // If NVLink is disabled, so is related functionality.
    //
    status = knvlinkApplyRegkeyOverrides(pGpu, pKernelNvlink);
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        return status;
    }

    pKernelNvlink->bVerifTrainingEnable = NV_FALSE;
    pKernelNvlink->bL2Entry             = NV_FALSE;

    status = knvlinkConstructHal_HAL(pGpu, pKernelNvlink);
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        return status;
    }

    //
    // Create MAX KernelIoctrl objects.
    // Later in knvlinkStatePreInit_IMPL, we will remove the objects for
    // IOCTRLs that do not exist in the HW.
    //
    // We must use this ordering because we should not touch GPU registers
    // during object creation
    //
    for (ioctrlIdx = 0; ioctrlIdx < NVLINK_MAX_IOCTRLS_SW; ioctrlIdx++)
    {
        if (!pKernelNvlink->pKernelIoctrl[ioctrlIdx])
        {
            _knvlinkCreateIoctrl(pGpu, pKernelNvlink, ioctrlIdx);
        }
    }

    return NV_OK;
}

/*!
 * @brief Determine if the NVLink IP is present and usable
 *        This includes:
 *        1. Detecting IOCTRL in PTOP
 *        2. Detecting IOCTRL Discovery integrity
 *        3. Detecting at least 1 link exposed in IOCTRL Discovery
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pKernelNvlink KernelNvlink pointer
 */
NvBool
knvlinkIsPresent_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

    // On GSP clients, retrieve all device discovery info from GSP through RPC
    status = knvlinkCopyNvlinkDeviceInfo(pGpu, pKernelNvlink);
    if (status != NV_OK)
        return NV_FALSE;

    status = knvlinkCopyIoctrlDeviceInfo(pGpu, pKernelNvlink);
    if (status != NV_OK)
        return NV_FALSE;

    return NV_TRUE;
}

/*!
 * @brief NVLINK State Pre-Init
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkStatePreInitLocked_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return knvlinkRemoveMissingIoctrlObjects(pGpu, pKernelNvlink);
}

/*!
 * @brief Remove IOCTRL objects that are not present in the system
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkRemoveMissingIoctrlObjects_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32         ioctrlIdx     = 0;
    KernelIoctrl *pKernelIoctrl = NULL;
    NV_STATUS     status        = NV_OK;

    // On GSP clients, retrieve all device discovery info from GSP
    status = knvlinkCopyNvlinkDeviceInfo(pGpu, pKernelNvlink);
    if (status != NV_OK)
        return status;

    // Delete IOCTRL objects that are not present
    for (ioctrlIdx = 0; ioctrlIdx < NVLINK_MAX_IOCTRLS_SW; ioctrlIdx++)
    {
        pKernelIoctrl = KNVLINK_GET_IOCTRL(pKernelNvlink, ioctrlIdx);
        if (!(pKernelNvlink->ioctrlMask & NVBIT(ioctrlIdx)))
        {
            objDelete(pKernelIoctrl);
            pKernelNvlink->pKernelIoctrl[ioctrlIdx] = NULL;
        }
    }

    return NV_OK;
}

/*!
 * @brief NVLINK StateLoad
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 * @param[in] flags         Flags
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkStateLoad_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         flags
)
{
    NV_STATUS         status = NV_OK;
    OBJSYS           *pSys   = SYS_GET_INSTANCE();
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool            bMIGNvLinkP2PDisabled = ((pKernelMIGManager != NULL) &&
                                               !kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));
    NvU32             preInitializedLinks;
    NvU32             i;

    //
    // If we are on the resume path, nvlinkIsPresent will not be called,
    // but we need to call it to get all the devices set up, call it now.
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH))
    {
        // The return is not important, the path is.
        knvlinkIsPresent(pGpu, pKernelNvlink);
    }

    // For GSP-CLIENTs, the link masks and vbios info need to synchronize with GSP
    status = knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    // Filter discovered links against disabled links (vbios or regkey)
    status = _knvlinkFilterDiscoveredLinks(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    // Filter IOCTRLs which have no discovered links (vbios or regkey)
    status = _knvlinkFilterIoctrls(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    // Update power features supported based on the NVLink IP
    knvlinkSetPowerFeatures(pGpu, pKernelNvlink);

    if (!IS_RTLSIM(pGpu) || pKernelNvlink->bForceEnableCoreLibRtlsims)
    {
        // NvLink should not be registered with core-lib when MIG is enabled
        if (!knvlinkPoweredUpForD3_HAL(pGpu, pKernelNvlink) &&
            !bMIGNvLinkP2PDisabled)
        {
            // Add the NVGPU device to the nvlink core
            status = knvlinkCoreAddDevice(pGpu, pKernelNvlink);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to add GPU device to nvlink core\n");
                goto knvlinkStateLoad_end;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "MIG Enabled or NVLink L2 is supported on chip. "
                      "Skip device registration in RTD3/FGC6 exit\n");
        }
    }

    //
    // Process the mask of init disabled links. Links can be init disabled
    // by the hypervisor in a virtualized system for links that connect to
    // GPUs that do not belong to the same guest
    //
    status = knvlinkProcessInitDisabledLinks(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    // Remove the init disabled links from the discovered links mask
    pKernelNvlink->discoveredLinks &= ~(pKernelNvlink->initDisabledLinksMask);

    // Track un-connected links, we assume all discovered links are connected.
    pKernelNvlink->connectedLinksMask = pKernelNvlink->discoveredLinks;

    // Initialize initializedLinks to 0 (assume no links initialized)
    pKernelNvlink->initializedLinks = 0;

    // For GSP-CLIENTs, the link masks and vbios info need to synchronize with GSP
    status = knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    //
    // Save off the links that were previously initialized to be able to
    // optimize away a heavy flush later. This is needed on IBM systems
    //
    preInitializedLinks = pKernelNvlink->initializedLinks;

    // Load link speed if forced from OS
    status = knvlinkProgramLinkSpeed_HAL(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    //
    // At this point, the discovered links mask is filtered. If there are no
    // discovered links, then we skip the rest of the steps.
    //
    if (pKernelNvlink->discoveredLinks == 0)
    {
        goto knvlinkStateLoad_end;
    }

    //
    // Override configuration for NVLink topology. This can be either
    //     a. Legacy forced configurations
    //     b. Chiplib forced configurations
    //
    status = knvlinkOverrideConfig_HAL(pGpu, pKernelNvlink, NVLINK_PHASE_STATE_LOAD);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    //
    // Finalize the enabledLinks mask. If the configuration is not forced
    // (legacy or chiplib), this is same as the discovered links mask
    //
    if (pKernelNvlink->bRegistryLinkOverride)
    {
        pKernelNvlink->enabledLinks = pKernelNvlink->discoveredLinks &
                                      pKernelNvlink->registryLinkMask;
    }
    else if (bMIGNvLinkP2PDisabled)
    {
        // NvLink is not supported with MIG
        pKernelNvlink->enabledLinks = 0;
    }
    else
    {
        pKernelNvlink->enabledLinks = pKernelNvlink->discoveredLinks;
    }

    // Sense NVLink bridge presence and remove links on missing bridges.
    knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink);

    // Disconnected links mask tracks links whose remote ends are not discovered
    pKernelNvlink->disconnectedLinkMask = pKernelNvlink->enabledLinks;

    if (!IS_RTLSIM(pGpu) || pKernelNvlink->bForceEnableCoreLibRtlsims)
    {
        if (!knvlinkPoweredUpForD3_HAL(pGpu, pKernelNvlink))
        {
            // Register links in the nvlink core library
            FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
            {
                status = knvlinkCoreAddLink(pGpu, pKernelNvlink, i);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to register Link%d in nvlink core\n", i);
                    goto knvlinkStateLoad_end;
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "NVLink L2 is supported on the chip. "
                      "Skip link registration in RTD3/FGC6 exit\n");
        }
    }

    // RPC to GSP-RM to perform pre-topology setup on mask of enabled links
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_ENABLE_LINKS,
                                 NULL, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to perform pre-topology setup on mask of enabled links\n");
        return status;
    }

    //
    // Check for NVSwitch proxy presence and update the RM state accordingly. If
    // this is service VM, then the NVSwitch fabric address should not have been
    // by now. If this is guest VM, then NVSwitch fabric address should already
    // have been programmed by service VM.
    //
    knvlinkDetectNvswitchProxy(pGpu, pKernelNvlink);

    //
    // Check for NVSwitch proxy to enable External Fabric Management and force
    // init fabric manager state.
    //
    if (knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink))
    {
        sysEnableExternalFabricMgmt(pSys);
        sysForceInitFabricManagerState(pSys);
    }

    //
    // WAR Bug# 3261027: Sync-up External Fabric Management status with GSP-RM.
    //     Long term once objsys state is made available to GSP, this WAR won't
    //     be needed.
    //
    status = sysSyncExternalFabricMgmtWAR(pSys, pGpu);
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    //
    // If we are running on CPU-RM or monolithic, process SYSMEM links, if present
    // on the system.
    //
    status = _knvlinkProcessSysmemLinks(pGpu, pKernelNvlink,
                        (preInitializedLinks != pKernelNvlink->initializedLinks));
    if (status != NV_OK)
    {
        NV_ASSERT(status == NV_OK);
        goto knvlinkStateLoad_end;
    }

    //
    // FLA is supported only on Nvlink enabled systems
    // Don't move this code path, since FLA relies on property
    // "PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED", which is set in this
    // function.
    //
    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
    {
        if ((status = kbusInitFla_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), 0, 0)) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Init FLA failed, status:0x%x\n", status);
            NV_ASSERT(status == NV_OK);
        }
    }

knvlinkStateLoad_end:

    if (status != NV_OK)
    {
        _knvlinkPurgeState(pGpu, pKernelNvlink);
    }

    return status;
}

/*!
 * @brief NVLINK StatePostLoad
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 * @param[in] flags         Flags
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkStatePostLoad_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         flags
)
{
    NV_STATUS  status              = NV_OK;
    OBJGPU    *pRemoteGpu          = NULL;
    NvU32      gpuInstance;
    NvU32      gpuMask;
    knvlinkCoreUpdateDeviceUUID(pGpu, pKernelNvlink);

    if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        //
        // If link initialization to SAFE during driver load is force enabled
        // through regkey, then trigger topology discovery now. This can't be
        // done for ALI since topology discovery can only happen after
        // verification training is complete
        //
        if (
            (pKernelNvlink->bEnableSafeModeAtLoad || pKernelNvlink->bEnableTrainingAtLoad ||
             pKernelNvlink->bVerifTrainingEnable))
        {
            knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
        }

        //
        // Bug# 1667991: Enabling link training to high speed
        // at driver load for loopback or P2P links only as of now.
        //
        // Also train links on verif env like Emulation and Fmodel
        //
        if (pKernelNvlink->bEnableTrainingAtLoad || pKernelNvlink->bVerifTrainingEnable)
        {
            {
                status = gpumgrGetGpuAttachInfo(NULL, &gpuMask);
                NV_ASSERT_OR_RETURN(status == NV_OK, status);
                gpuInstance = 0;
                while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
                {
                    knvlinkTrainP2pLinksToActive(pGpu, pRemoteGpu, pKernelNvlink);
                }
            }
        }
    }

    status = knvlinkStatePostLoadHal_HAL(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR," failed for GPU 0x%x\n", pGpu->gpuInstance);
        return status;
    }

    return NV_OK;
}

/*!
 * @brief NVLINK StateUnload
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 * @param[in] flags         Flags
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkStateUnload_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         flags
)
{
    //
    // Don't tear down FLA when undergoing suspend/resume
    // Enable this only for CPU-RM and monolithic RM
    //
    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
    {
        kbusDestroyFla_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu));
    }

    return NV_OK;
}

/*!
 * @brief NVLINK StatePostUnload
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkStatePostUnload_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         flags
)
{
    OBJSYS    *pSys   = SYS_GET_INSTANCE();
    NV_STATUS  status = NV_OK;

    if ((knvlinkGetNumLinksToSystem(pGpu, pKernelNvlink) != 0) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        //
        // On GPU reset the CPU<->GPU NVLinks are reset, and leaving any GPU
        // memory cached on the CPU leads to fatal errors when the CPU tries to
        // flush it after the link is down.
        //
        // Handle this by flushing all of the CPU caches as part of destroying
        // the mapping. Do it only if the GPU is being drained as that's an
        // indication the GPU is going to be reset. Otherwise, the links stay
        // up and it's unnecessary to flush the cache.
        //
        // Do the flush before the link is put into safe mode below as the
        // flush is much slower (takes minutes) once that's done.
        //
        NvBool bFlush = pKernelNvlink->getProperty(pKernelNvlink,
                                               PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD);
        kmemsysTeardownCoherentCpuLink(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu), bFlush);
    }

    //
    // If GPU is in the D3 entry path and if NVLink L2 is supported and links are
    // expected to be in L2 before D3 entry is triggered, skip lane shutdown
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH) &&
        pKernelNvlink->getProperty(pKernelNvlink,
                                   PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED) &&
        pKernelNvlink->getProperty(pKernelNvlink,
                                   PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE))
    {
        goto knvlinkStatePostUnload_end;
    }

    //
    // Set HSHUB to init values.
    //
    // It is good to reset HSHUB when GPUs are going down. For example, a GPU
    // can be torn down because it (or its peers) experienced an NVLink error.
    // In such cases resetting HSHUB is must. Otherwise, consecutive RmInitAdaper
    // on the GPU  could fail if membars are emitted on the broken NVLinks.
    //
    // We do not set sysmem masks to init values because P9 systems are crashing
    // for an unknown reason with an HMI exception during consecutive
    // RmInitAdapter.
    //
    // TODO: For now, we are enabling this change for NVSwitch systems in r400_00
    // to unblock DGX-2 release. In chips_a, the change will be enabled on all
    // platforms (in discussion with ARCH for non-NVSwitch platforms).
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
        knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink))
    {
        knvlinkRemoveMapping_HAL(pGpu, pKernelNvlink, NV_FALSE,
                                 ((1 << NVLINK_MAX_PEERS_SW) - 1),
                                 NV_FALSE /* bL2Entry */);
    }

    //
    // Check if lane disable and shutdown during driver unload has been forced
    // using regkey override, or as a part of the external reset sequence.
    //
    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD) &&
                        !pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED) &&
                        !API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        NV2080_CTRL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS params;

        portMemSet(&params, 0, sizeof(params));
        params.linkMask = pKernelNvlink->enabledLinks;

        // Disable all the DL interrupts
        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_NVLINK_DISABLE_DL_INTERRUPTS,
                                     (void *)&params, sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to disable DL interrupts for the links\n");
            return status;
        }

        // Shutdown all the links through pseudo-clean shutdown
        status = knvlinkPrepareForXVEReset(pGpu, pKernelNvlink);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to pseudo-clean shutdown the links for GPU%d\n",
                      pGpu->gpuInstance);
            return status;
        }
    }

knvlinkStatePostUnload_end:

    _knvlinkPurgeState(pGpu, pKernelNvlink);

    return status;
}

/*!
 * @brief Purge SW state
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkPurgeState
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    KernelIoctrl *pKernelIoctrl = NULL;
    NvU32         ioctrlIdx;

#if defined(INCLUDE_NVLINK_LIB)

    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGNvLinkP2PDisabled = ((pKernelMIGManager != NULL) &&
                          !kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));

    // With MIG NvLink registration was skipped with core-lib
    if (knvlinkPoweredUpForD3_HAL(pGpu, pKernelNvlink) || bMIGNvLinkP2PDisabled)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping device/link un-registration in RTD3 GC6 entry path\n");
        goto _knvlinkPurgeState_end;
    }

    if (!IS_RTLSIM(pGpu) || pKernelNvlink->bForceEnableCoreLibRtlsims)
    {
        if (pKernelNvlink->pNvlinkDev)
        {
            NvU32 linkId;

            // Un-register the links from nvlink core library
            FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->enabledLinks)
            {
                knvlinkCoreRemoveLink(pGpu, pKernelNvlink, linkId);
            }
            FOR_EACH_INDEX_IN_MASK_END;

            // Un-register the nvgpu device from nvlink core library
            knvlinkCoreRemoveDevice(pGpu, pKernelNvlink);
        }
    }

_knvlinkPurgeState_end:

#endif

    //
    // This GPU is being shutdown, so need to clear the peerLinkMasks and sysmem link
    // mask.
    //
    portMemSet(pKernelNvlink->peerLinkMasks, 0, sizeof(pKernelNvlink->peerLinkMasks));
    pKernelNvlink->sysmemLinkMask = 0;

    // Unload each IOCTRL object
    for (ioctrlIdx = 0; ioctrlIdx < pKernelNvlink->ioctrlNumEntries; ioctrlIdx++)
    {
        pKernelIoctrl = KNVLINK_GET_IOCTRL(pKernelNvlink, ioctrlIdx);
        if (pKernelIoctrl)
        {
            kioctrlDestructEngine(pKernelIoctrl);
            pKernelNvlink->ioctrlMask &= ~NVBIT(ioctrlIdx);
        }
    }

    // Destroy the chiplib configuration memory
    if (pKernelNvlink->pLinkConnection)
    {
        portMemFree(pKernelNvlink->pLinkConnection);
        pKernelNvlink->pLinkConnection = NULL;
    }

    return NV_OK;
}

void
knvlinkDestruct_IMPL
(
    KernelNvlink *pKernelNvlink
)
{
    OBJGPU       *pGpu          = ENG_GET_GPU(pKernelNvlink);
    KernelIoctrl *pKernelIoctrl = NULL;
    NvU32         ioctrlIdx;

    // Destroy the RM NVLink state
    _knvlinkPurgeState(pGpu, pKernelNvlink);
  
    // Free Ioctrls
    for (ioctrlIdx = 0; ioctrlIdx < NVLINK_MAX_IOCTRLS_SW; ioctrlIdx++)
    {
        pKernelIoctrl = KNVLINK_GET_IOCTRL(pKernelNvlink, ioctrlIdx);
        if (pKernelIoctrl)
        {
            objDelete(pKernelIoctrl);
            pKernelNvlink->pKernelIoctrl[ioctrlIdx] = NULL;
        }
    }

    // Unload the nvlink core library
    knvlinkCoreDriverUnloadWar(pGpu, pKernelNvlink);
}

/**
 * @brief Handle sysmem NVLink connections and ATS functionality
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  Nvlink pointer
 * @param[in] bFlush         Whether the CPU cache of the GPU mapping
 *                           should be flushed
 *
 * @return  NV_OK on success
 */
NV_STATUS
_knvlinkProcessSysmemLinks
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bFlush
)
{
    NV_STATUS status = NV_OK;

#if defined(NVCPU_PPC64LE) || defined(NVCPU_AARCH64)
    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED))
    {
        //
        // In case of IBM or Tegra, the sysmem links will already have
        // been registered in nvlink core library. In order to trigger
        // topology detection, call knvlinkCoreGetRemoteDeviceInfo
        //
        if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink) && !pKernelNvlink->pLinkConnection)
        {
            //
            // Establish the current link topology and enable IBM CPU/SYSMEM links.
            // If any of the discovered links are CPU/SYSMEM, they will be trained,
            // post-enabled, and then enabled in HSHUB when the call has completed.
            //
            status = knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed call to get remote device info during IBM CPU/SYSMEM links "
                          "setup, failing NVLink StateLoad on GPU%d!!!\n\n",
                          pGpu->gpuInstance);

                return status;
            }
        }
        else
        {
            // If the topology is forced, then just apply the settings
            knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);
        }
    }
#else
    if (knvlinkIsForcedConfig(pGpu, pKernelNvlink) || !IS_SILICON(pGpu))
    {
        // Set up the current Nvlink configuration
        knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);
    }
#endif
    // Set Buffer ready for the sysmem links
    NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS programBufferRdyParams;

    portMemSet(&programBufferRdyParams, 0, sizeof(programBufferRdyParams));
    programBufferRdyParams.flags        = NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_FLAGS_SET;
    programBufferRdyParams.bSysmem      = NV_TRUE;
    programBufferRdyParams.peerLinkMask = 0;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_PROGRAM_BUFFERREADY,
                                 (void *)&programBufferRdyParams,
                                 sizeof(programBufferRdyParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to program bufferready for the sysmem nvlinks!\n");
        return status;
    }

    //
    // Enable functionality related to NVLink SYSMEM:
    // + ATS functionality if hardware support is available
    // + Apply the Bug 200279966 WAR
    //
    if (knvlinkGetNumLinksToSystem(pGpu, pKernelNvlink) != 0)
    {
        //
        // Configure sysmem atomics config after sysmem link is up.
        // Sysmem atomics are programmed from memsysConfigureSysmemAtomics_HAL
        // but in case of PPC+GV100, nvlink setup is not done during call to
        // memsysConfigureSysmemAtomics_HAL that leads to sysmem atomics not getting
        // configured. Hence configure the sysmem atomics now for taking care of
        // PPC+GV100.
        //

        NV2080_CTRL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS sysmemNvlinkAtsParams;
        portMemSet(&sysmemNvlinkAtsParams, 0, sizeof(sysmemNvlinkAtsParams));

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_NVLINK_ENABLE_SYSMEM_NVLINK_ATS,
                                     (void *)&sysmemNvlinkAtsParams,
                                     sizeof(sysmemNvlinkAtsParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to snable ATS functionality for NVLink sysmem!\n");
            return status;
        }

        status = kmemsysSetupCoherentCpuLink(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu), bFlush);
        NV_ASSERT_OR_RETURN(status == NV_OK, status);

        osSetNVLinkSysmemLinkState(pGpu, NV_TRUE);
    }

    return status;
}
