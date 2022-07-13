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

#include "os/os.h"
#include "core/hal.h"
#include "core/info_block.h"
#include "core/locks.h"
#include "core/thread_state.h"
#include "gpu/gpu.h"

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"

#include "nverror.h"

#if defined(INCLUDE_NVLINK_LIB)
#include "nvlink_os.h"
#endif

static void  _knvlinkRetrainLinkPrologue(OBJGPU *, KernelNvlink *, NvU32);

#if defined(INCLUDE_NVLINK_LIB)

static NV_STATUS _knvlinkActivateDiscoveredConns(OBJGPU *, KernelNvlink *, NvBool);
static NV_STATUS _knvlinkActivateDiscoveredP2pConn(OBJGPU *, KernelNvlink *, NvU32);
static NV_STATUS _knvlinkActivateDiscoveredSwitchConn(OBJGPU *, KernelNvlink *, NvU32);
static NV_STATUS _knvlinkActivateDiscoveredSysmemConn(OBJGPU *, KernelNvlink *, NvU32);
static NV_STATUS _knvlinkEnterSleep(OBJGPU *, KernelNvlink *, NvU32);
static NV_STATUS _knvlinkExitSleep(OBJGPU *, KernelNvlink *, NvU32);
static NvBool    _knvlinkUpdateSwitchLinkMasks(OBJGPU *, KernelNvlink *, NvU32);
static NvBool    _knvlinkUpdateSwitchLinkMasksGpuDegraded(OBJGPU *, KernelNvlink *);
static void      _knvlinkUpdatePeerConfigs(OBJGPU *, KernelNvlink *);
static void      _knvlinkPrintTopologySummary(OBJGPU *, KernelNvlink *);

#endif

/*!
 * @brief Get the nvlink connections for the GPU.
 *        This function calls into the core library to trigger topology discovery
 *        on the set of links that have been registered with the core library.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreGetRemoteDeviceInfo_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)

    OBJSYS *pSys                  = SYS_GET_INSTANCE();
    NvU32   flags                 = NVLINK_STATE_CHANGE_SYNC;
    NvBool  bNvswitchProxyPresent = NV_FALSE;
    NvBool  bUpdateConnStatus     = NV_FALSE;
    NvBool  bCheckDegradedMode    = NV_FALSE;
    NvU32   linkId;

    //
    // Topology discovery should NOT be triggered in RTD3/FGC6 exit path if L2 is
    // supported. The remote information will be restored when RM state is restored
    //
    if (!knvlinkPoweredUpForD3_HAL(pGpu, pKernelNvlink))
    {
        //
        // Optimization: Check for nvlink proxy only when system fabric is externally
        // managed. This would avoid RPCs in non-nvswitch cases.
        //
        if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
        {
            bNvswitchProxyPresent = knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink);
        }

        // We only need to look at links that are still considered disconnected
        FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->disconnectedLinkMask)
        {
            nvlink_conn_info conn_info = {0};
            bUpdateConnStatus          = NV_FALSE;

            if (pKernelNvlink->nvlinkLinks[linkId].core_link)
            {
                // Call the core library to get the remote end information
                if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
                {
                    nvlink_lib_get_remote_conn_info(
                            pKernelNvlink->nvlinkLinks[linkId].core_link, &conn_info);

                    //
                    // nvlink_lib_get_remote_conn_info could fail to return connection info if
                    // it runs on a shared-NVSwitch virtualization model (HGX-2) where GPU nodes
                    // can't see NVSwitches. In that case, examine the NVLink scratch register
                    // for connectivity information.
                    //
                    if (!conn_info.bConnected && bNvswitchProxyPresent)
                    {
                        conn_info.bConnected  = NV_TRUE;
                        conn_info.deviceType  = NVLINK_DEVICE_TYPE_NVSWITCH;
                        conn_info.pciDeviceId = NV_U32_MAX;
                        conn_info.domain      = NV_U32_MAX;
                        conn_info.bus         = NV_U16_MAX;
                        conn_info.device      = NV_U16_MAX;
                        conn_info.function    = NV_U16_MAX;
                    }

                    //
                    // New connection is discovered from core library. But we don't know if this
                    // connection was shutdown or reset by fabric manager while enabling degraded
                    // mode. So, we have to check for degraded mode.
                    //
                    if (conn_info.bConnected)
                    {
                        bCheckDegradedMode = NV_TRUE;
                    }
                }
                else
                {
                    // Aynchronous link initialization for IP 2.2
                    if (pKernelNvlink->ipVerNvlink == NVLINK_VERSION_22)
                    {
                        flags = NVLINK_STATE_CHANGE_ASYNC;
                    }

                    nvlink_lib_discover_and_get_remote_conn_info(
                            pKernelNvlink->nvlinkLinks[linkId].core_link, &conn_info, flags);
                }

                 // RPC into GSP-RM to update the link connected status only if its required
                if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected != conn_info.bConnected)
                    bUpdateConnStatus = NV_TRUE;

                pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected = conn_info.bConnected;

                if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected)
                {
                    // Update the RM cache for the remote device information for the link
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain      = conn_info.domain;
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus         = conn_info.bus;
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device      = conn_info.device;
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.function    = conn_info.function;
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.pciDeviceId = conn_info.pciDeviceId;
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType  = conn_info.deviceType;
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.linkNumber  = conn_info.linkNumber;
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.chipSid     = conn_info.chipSid;

                    nvlink_memcpy(pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.devUuid,
                                  conn_info.devUuid,
                                  NV_UUID_LEN);
                }

                if (bUpdateConnStatus)
                {
                    // RPC into GSP-RM to update the link remote connection status for pGpu
                    status = knvlinkUpdateLinkConnectionStatus(pGpu, pKernelNvlink, linkId);
                    if (status != NV_OK)
                    {
                        return status;
                    }
                }
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "L2 supported. Skip topology discovery on GPU%d in RTD3/FGC6 exit\n",
                  pGpu->gpuInstance);
    }

    //
    // Update the RM cache for the discovered connections and then activate
    // those connections. This includes all the post-topology settings like
    // buffer-ready and interrupt enables
    //
    status = _knvlinkActivateDiscoveredConns(pGpu, pKernelNvlink, bCheckDegradedMode);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Failed to activate the discovered connections on GPU%d\n",
                  pGpu->gpuInstance);
    }

#endif

    return status;
}

/*!
 * @brief Train all the connected sysmem links associated with the device
 *        to active through the nvlink core library.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkTrainSysmemLinksToActive_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU32   i;

    // On Fmodel, sysmem link training is not supported
    if (IS_FMODEL(pGpu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping unsupported sysmem link training on GPU%d\n",
                  pGpu->gpuInstance);

        return NV_OK;
    }

    // Return if link training is force disabled through regkey
    if (pKernelNvlink->bSkipLinkTraining)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping link training due to regkey on GPU%d\n",
                  pGpu->gpuInstance);

        return NV_OK;
    }

    // If fabric is managed by FM, return
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Fabric is externally managed, skip link training\n");

        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO, "Training sysmem links for GPU%d\n",
              pGpu->gpuInstance);

    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
    {
        if (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bConnected &&
            ((pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_IBMNPU)    ||
             (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_TEGRASHIM) ||
             (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_EBRIDGE)))
        {
            if (nvlink_lib_train_links_from_swcfg_to_active(
                    &pKernelNvlink->nvlinkLinks[i].core_link, 1, NVLINK_STATE_CHANGE_SYNC)
                    != NVL_SUCCESS)
            {
                nvErrorLog_va((void *)pGpu, NVLINK_ERROR,
                    "NVLink: failed to train link %d to remote PCI:%04x:%02x:%02x",
                    i,
                    pKernelNvlink->nvlinkLinks[i].remoteEndInfo.domain,
                    pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bus,
                    pKernelNvlink->nvlinkLinks[i].remoteEndInfo.device);

                return NV_ERR_NOT_SUPPORTED;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    //
    // After training links, we may have used up most of the available 4s
    // timeout during GPU state load. As a WAR in lieu of improving the
    // performance of link training SW, reset the timeout for now.
    //
    NV_PRINTF(LEVEL_INFO, "resetting timeout after link training\n");

    threadStateResetTimeout(pGpu);

#endif

    return NV_OK;
}

/*!
 * @brief Ensure links are trained and put into active.
 *
 * @param[in]  pGpu0          OBJGPU pointer
 * @param[in]  pGpu1          OBJGPU pointer
 * @param[in]  pKernelNvlink0 KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCheckTrainingIsComplete_IMPL
(
    OBJGPU       *pGpu0,
    OBJGPU       *pGpu1,
    KernelNvlink *pKernelNvlink0
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)
    OBJSYS       *pSys           = SYS_GET_INSTANCE();

    KernelNvlink *pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);

    if (pKernelNvlink1 == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Input mask contains a GPU on which NVLink is disabled.\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    // Link training will be triggered from KMD in L2 exit path
    if (knvlinkPoweredUpForD3_HAL(pGpu0, pKernelNvlink0))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skip link training on GPU%d in RTD3/FGC6 exit. Links will train to "
                  "ACTIVE in L2 exit path\n", pGpu0->gpuInstance);
        return NV_OK;
    }

    // Minion and SW training is by default disabled on RTL
    if (IS_RTLSIM(pGpu0) && !pKernelNvlink0->bForceEnableCoreLibRtlsims)
    {
        return NV_OK;
    }

    // Return if link training is force disabled through regkey
    if (pKernelNvlink0->bSkipLinkTraining)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping link training due to regkey on GPU%d\n",
                  pGpu0->gpuInstance);
        return NV_OK;
    }

    // Return if forced config, since SW training is not supported
    if (knvlinkIsForcedConfig(pGpu0, pKernelNvlink0))
    {
        NV_PRINTF(LEVEL_INFO, "Skipping link due to forced configuration\n");
        return NV_OK;
    }

    // If fabric is managed by FM, return
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Fabric is externally managed, skip link training\n");
        return NV_OK;
    }

#endif

    return status;
}

/*!
 * @brief Train all the connected links between the two given devices
 *        to active through the nvlink core library.
 *
 * @param[in]  pGpu0          OBJGPU pointer
 * @param[in]  pGpu1          OBJGPU pointer
 * @param[in]  pKernelNvlink0 KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkTrainP2pLinksToActive_IMPL
(
    OBJGPU       *pGpu0,
    OBJGPU       *pGpu1,
    KernelNvlink *pKernelNvlink0
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)

    OBJSYS *pSys        = SYS_GET_INSTANCE();
    NvU32   version     = pKernelNvlink0->ipVerNvlink;
    NvBool  bTrainLinks = NV_FALSE;
    NvU32   count       = 0;
    NvU32   i;

    nvlink_link *pLinks[NVLINK_MAX_LINKS_SW] = { 0 };

    // Link training will be triggered from KMD in L2 exit path
    if (knvlinkPoweredUpForD3_HAL(pGpu0, pKernelNvlink0))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skip link training on GPU%d in RTD3/FGC6 exit. Links will train to "
                  "ACTIVE in L2 exit path\n", pGpu0->gpuInstance);

        return NV_OK;
    }

    // Minion and SW training is by default disabled on RTL
    if (IS_RTLSIM(pGpu0) && !pKernelNvlink0->bForceEnableCoreLibRtlsims)
    {
        return NV_OK;
    }

    // Return if link training is force disabled through regkey
    if (pKernelNvlink0->bSkipLinkTraining)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping link training due to regkey on GPU%d\n",
                  pGpu0->gpuInstance);

        return NV_OK;
    }

    // Return if forced config, since SW training is not supported
    if (knvlinkIsForcedConfig(pGpu0, pKernelNvlink0))
    {
        NV_PRINTF(LEVEL_INFO, "Skipping link due to forced configuration\n");

        return NV_OK;
    }

    // If fabric is managed by FM, return
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Fabric is externally managed, skip link training\n");

        return NV_OK;
    }

    //
    // Bug# 3601144: On Ampere+ systems, return if links are already initialized,
    // since that implies links are already trained.
    //
    if (IsAMPEREorBetter(pGpu0))
    {
        NvU32 linkMask = 0;

        FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink0->enabledLinks)
        {
            if (KNVLINK_IS_LINK_CONNECTED_TO_GPU(pKernelNvlink0, i, pGpu1))
            {
                linkMask |= BIT(i);
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;

        if ((linkMask & pKernelNvlink0->initializedLinks) == linkMask)
        {
            NV_PRINTF(LEVEL_INFO, "P2P links are all trained already, return\n");
            return NV_OK;
        }
    }

    // Get the link train status for the enabled link masks
    NV2080_CTRL_NVLINK_ARE_LINKS_TRAINED_PARAMS linkTrainedParams;

    portMemSet(&linkTrainedParams, 0, sizeof(linkTrainedParams));
    linkTrainedParams.linkMask    = pKernelNvlink0->enabledLinks;
    linkTrainedParams.bActiveOnly = NV_TRUE;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu0))
    {
        threadStateResetTimeout(pGpu0);
    }

    status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                 NV2080_CTRL_CMD_NVLINK_ARE_LINKS_TRAINED,
                                 (void *)&linkTrainedParams,
                                 sizeof(linkTrainedParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the link train status for links\n");
        return status;
    }

    //
    // Bug# 3424466: Optimization - Return if all enabled links for this GPU are
    // already trained. The core library makes several callbacks to check link
    // state which results in numerous RPCs on GSP-RM platforms resulting in low
    // perf on chips which have low link training latency and low links count.
    //
    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink0->enabledLinks)
    {
        if (!KNVLINK_IS_LINK_CONNECTED_TO_GPU(pKernelNvlink0, i, pGpu1))
        {
            continue;
        }

        if (!linkTrainedParams.bIsLinkActive[i])
        {
            bTrainLinks = NV_TRUE;
            break;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (!bTrainLinks)
    {
        NV_PRINTF(LEVEL_INFO, "Enabled links are all trained already, return\n");
        return NV_OK;
    }

    // Train the mask of enabled links to ACTIVE state
    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink0->enabledLinks)
    {
        if (!KNVLINK_IS_LINK_CONNECTED_TO_GPU(pKernelNvlink0, i, pGpu1))
        {
            continue;
        }

        if (version >= NVLINK_VERSION_22)
        {
            // Capture links for parallel link training
            pLinks[count] = pKernelNvlink0->nvlinkLinks[i].core_link;
            count++;
        }
        else
        {
            // Invoke link training for NVLINK <= 2.0
            (void)nvlink_lib_train_links_from_swcfg_to_active(
                &pKernelNvlink0->nvlinkLinks[i].core_link, 1, NVLINK_STATE_CHANGE_SYNC);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Invoke link training for NVLINK >= 2.2
    if (count > 0)
    {
        //
        // nvlink_lib_train_links_from_swcfg_to_active with
        // NVLINK_STATE_CHANGE_ASYNC flag invokes link training asynchronously,
        // but the call itself is synchronous i.e. it will poll for link
        // training to complete.
        //
        NV_ASSERT(version >= NVLINK_VERSION_22);
        (void)nvlink_lib_train_links_from_swcfg_to_active(
            pLinks, count, NVLINK_STATE_CHANGE_ASYNC);
    }

    // Get the link train status for the enabled link masks
    portMemSet(&linkTrainedParams, 0, sizeof(linkTrainedParams));
    linkTrainedParams.linkMask    = pKernelNvlink0->enabledLinks;
    linkTrainedParams.bActiveOnly = NV_TRUE;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu0))
    {
        threadStateResetTimeout(pGpu0);
    }

    status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                 NV2080_CTRL_CMD_NVLINK_ARE_LINKS_TRAINED,
                                 (void *)&linkTrainedParams,
                                 sizeof(linkTrainedParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the link train status for links\n");
        return status;
    }

    // Check if the links are trained to "active" state.
    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink0->enabledLinks)
    {
        if (!KNVLINK_IS_LINK_CONNECTED_TO_GPU(pKernelNvlink0, i, pGpu1))
        {
            continue;
        }

        if (linkTrainedParams.bIsLinkActive[i])
        {
            continue;
        }

        nvErrorLog_va((void *)pGpu0, NVLINK_ERROR,
            "NVLink: Failed to train link %d to remote PCI:%04x:%02x:%02x",
            i,
            pKernelNvlink0->nvlinkLinks[i].remoteEndInfo.domain,
            pKernelNvlink0->nvlinkLinks[i].remoteEndInfo.bus,
            pKernelNvlink0->nvlinkLinks[i].remoteEndInfo.device);

        status = NV_ERR_INVALID_STATE;
    }
    FOR_EACH_INDEX_IN_MASK_END;

#endif

    return status;
}

/*!
 * knvlinkTrainFabricLinksToActive_IMPL
 *     Setup NVLinks between 2 peers connected to switch. Train the links to
 *     High Speed.
 *
 * Note: Desired sequence to setup NvLink P2P is:
 *       1. A client queries P2P capability among GPUs.
 *       2. If the GPUs are P2P compatible, create NV50_P2P object which invokes
 *          link training.
 * However, existing GPU<->GPU link training happens during step 1 through
 * gpumgrGetP2PCaps - which gets called on RmInitAdapter and may lead to timeout
 * based upon the time consumed by costly link training operations.
 *
 * For now, we are fixing this for nvswitch systems by adding this helper
 * function which should just get invoked during NV50_P2P object creation.
 *
 * This issue needs to be fixed for non-nvswitch systems as well. Bug:200285708.
 * Once the bug is fixed, knvlinkTrainFabricLinksToActive can be called from
 * knvlinkTrainP2pLinksToActive.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkTrainFabricLinksToActive_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU32   i;

    // Minion and SW training is by default disabled on RTL
    if (IS_RTLSIM(pGpu) && !pKernelNvlink->bForceEnableCoreLibRtlsims)
    {
        return NV_OK;
    }

    // Return if link training is force disabled through regkey
    if (pKernelNvlink->bSkipLinkTraining)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping link training due to regkey on GPU%d\n",
                  pGpu->gpuInstance);

        return NV_OK;
    }

    // If fabric is managed by FM, return
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Fabric is externally managed, skip link training\n");

        return NV_OK;
    }

    if (knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Nvlink in Forced Config - skip link training.\n");

        return NV_OK;
    }

    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
    {
        if ( pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bConnected &&
            (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType ==
                NVLINK_DEVICE_TYPE_NVSWITCH))
        {
            if (nvlink_lib_train_links_from_swcfg_to_active(
                &pKernelNvlink->nvlinkLinks[i].core_link, 1, NVLINK_STATE_CHANGE_SYNC)
                != NVL_SUCCESS)
            {
                nvErrorLog_va((void *)pGpu, NVLINK_ERROR,
                    "NVLink: failed to train link %d to remote PCI:%04x:%02x:%02x",
                    i,
                    pKernelNvlink->nvlinkLinks[i].remoteEndInfo.domain,
                    pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bus,
                    pKernelNvlink->nvlinkLinks[i].remoteEndInfo.device);

                return NV_ERR_INVALID_STATE;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

#endif

    return NV_OK;
}

/*!
 * @brief Transition/Wakeup the links into/from sleep (L2) state
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 * @param[in]  linkMask       Mask of links
 * @param[in]  bEntry         Enter/Exit sleep (L2)
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkEnterExitSleep_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkMask,
    NvBool        bEntry
)
{
#if defined(INCLUDE_NVLINK_LIB)

    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU32   linkId;

    // NVLink L2 as a feature should be enabled
    if (!pKernelNvlink->getProperty(pKernelNvlink,
                                    PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED))
    {
        NV_PRINTF(LEVEL_ERROR, "NVLink L2 is not supported. Returning\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    // Return error if NVLink fabric is managed by FM
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Skipping L2 entry/exit since fabric is externally managed\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    // Check if all the links in the mask are connected
    FOR_EACH_INDEX_IN_MASK(32, linkId, linkMask)
    {
        if (!pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU%d: Link%d is not connected. Returning\n",
                      pGpu->gpuInstance, linkId);

            return NV_ERR_NOT_SUPPORTED;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Links that share a PLL must enter/exit L2 together
    FOR_EACH_INDEX_IN_MASK(32, linkId, linkMask)
    {
        // If the link is a PLL master, consider the slave link
        if (pKernelNvlink->nvlinkLinks[linkId].pllMasterLinkId == linkId)
        {
            // If the slave link exists and is not init-disabled, it should be included
            if ( (pKernelNvlink->nvlinkLinks[linkId].pllSlaveLinkId != NVLINK_MAX_LINKS_SW)               &&
                 (NVBIT(pKernelNvlink->nvlinkLinks[linkId].pllSlaveLinkId) & pKernelNvlink->enabledLinks) &&
                !(NVBIT(pKernelNvlink->nvlinkLinks[linkId].pllSlaveLinkId) & linkMask) )
            {
                NV_PRINTF(LEVEL_ERROR,
                          "GPU%d: Links sharing PLL should enter/exit L2 together. Returning\n",
                          pGpu->gpuInstance);

                return NV_ERR_NOT_SUPPORTED;
            }
        }
        else
        {
            // For a slave link, its PLL master should be included if not init-disabled
            if ( (NVBIT(pKernelNvlink->nvlinkLinks[linkId].pllMasterLinkId) & pKernelNvlink->enabledLinks) &&
                !(NVBIT(pKernelNvlink->nvlinkLinks[linkId].pllMasterLinkId) & linkMask) )
            {
                NV_PRINTF(LEVEL_ERROR,
                          "GPU%d: Links sharing PLL should enter/exit L2 together. Returning\n",
                          pGpu->gpuInstance);

                return NV_ERR_NOT_SUPPORTED;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Device must be registered in the nvlink core library
    if (!pKernelNvlink->pNvlinkDev)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU%d: not registered in core lib. Returning\n",
                  pGpu->gpuInstance);

        return NV_ERR_NOT_SUPPORTED;
    }

    if (bEntry)
    {
        // Remove the peer mapping in HSHUB and transition links to sleep (L2)
        return _knvlinkEnterSleep(pGpu, pKernelNvlink, linkMask);
    }
    else
    {
        // Wakeup the links from sleep (L2) and setup the peer mapping in HSHUB
        return _knvlinkExitSleep(pGpu, pKernelNvlink, linkMask);
    }
#endif

    return NV_OK;
}

/*!
 * @brief Shutdown all the connected links associated with the device
 *        through the nvlink core library.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreShutdownDeviceLinks_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    nvlink_link *pLinks[NVLINK_MAX_LINKS_SW] = {0};
    OBJSYS      *pSys  = SYS_GET_INSTANCE();
    NvU32        count = 0;
    NvU32        linkId;

    // Skip link shutdown where fabric manager is present
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED) ||
        (pKernelNvlink->pNvlinkDev == NULL))
    {
        NV_PRINTF(LEVEL_INFO,
                  "core lib device is either externally managed or not present, skipping\n");

        return NV_OK;
    }

    // return early if there are no enabled links
    if (pKernelNvlink->enabledLinks == 0)
    {
        NV_PRINTF(LEVEL_INFO, "No links to shutdown for the GPU%d\n",
                  pGpu->gpuInstance);

        return NV_OK;
    }

    FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->enabledLinks)
    {
        // Capture the links for lane shutdown through core lib if supported
        if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED))
        {
            // Skip GPU in reset
            if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType ==
                                                           NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU)
            {
                OBJGPU* pRemoteGpu = gpumgrGetGpuFromBusInfo(
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain,
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus, 
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device);
                if (API_GPU_IN_RESET_SANITY_CHECK(pRemoteGpu))
                {
                    continue;
                }
            }
            pLinks[count] = pKernelNvlink->nvlinkLinks[linkId].core_link;
            count++;
        }
        else
        {
            nvlink_lib_powerdown_links_from_active_to_swcfg(
                        &pKernelNvlink->nvlinkLinks[linkId].core_link,
                        1, NVLINK_STATE_CHANGE_SYNC);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Trigger laneshutdown through core lib if shutdown is supported
    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED) && (count > 0))
    {
        if (nvlink_lib_powerdown_links_from_active_to_off(
                        pLinks, count, NVLINK_STATE_CHANGE_SYNC))
        {
            NV_PRINTF(LEVEL_ERROR, "Unable to turn off links for the GPU%d\n",
                      pGpu->gpuInstance);

            return NV_ERR_INVALID_STATE;
        }
    }

#endif

    return NV_OK;
}

/*!
 * @brief Reset all the connected links associated with the device
 *        through the nvlink core library.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreResetDeviceLinks_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    nvlink_link *pLinks[NVLINK_MAX_LINKS_SW] = {0};
    OBJSYS      *pSys  = SYS_GET_INSTANCE();
    NvU32        count = 0;
    NvU32        linkId;

    // Skip link reset where fabric manager is present
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED) ||
        (pKernelNvlink->pNvlinkDev == NULL))
    {
        NV_PRINTF(LEVEL_INFO,
                  "core lib device is either externally managed or not present, skipping\n");

        return NV_OK;
    }

    // return early if there are no enabled links
    if (pKernelNvlink->enabledLinks == 0)
    {
        NV_PRINTF(LEVEL_INFO, "No links to reset for the GPU%d\n",
                  pGpu->gpuInstance);

        return NV_OK;
    }

    // We only perform the link reset if lane shutdown is enabled
    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED))
    {
        FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->enabledLinks)
        {
            // Skip GPU in reset
            if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType ==
                                             NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU)
            {
                OBJGPU* pRemoteGpu = gpumgrGetGpuFromBusInfo(
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain,
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus,
                    pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device);
                if (API_GPU_IN_RESET_SANITY_CHECK(pRemoteGpu))
                {
                    continue;
                }
            }
            pLinks[count] = pKernelNvlink->nvlinkLinks[linkId].core_link;
            count++;
        }
        FOR_EACH_INDEX_IN_MASK_END;

        if (nvlink_lib_reset_links(pLinks, count, NVLINK_STATE_CHANGE_SYNC) && (count > 0))
        {
            NV_PRINTF(LEVEL_ERROR, "Unable to reset link(s) for GPU%d\n",
                      pGpu->gpuInstance);

            return NV_ERR_INVALID_STATE;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "Lane shutdown not enabled, skipping link(s) reset for GPU%d\n",
                  pGpu->gpuInstance);

        return NV_ERR_INVALID_STATE;
    }

#endif

    return NV_OK;
}

/*!
 * @brief Retrain a link from either safe mode or off.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 * @param[in]  linkId        Link ID of the link in question
 * @param[in]  bFromOff      Whether link should be retrained from SAFE/OFF
 *
 * @returns NV_OK if link retraining was successful
 */
NV_STATUS
knvlinkRetrainLink_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId,
    NvBool        bFromOff
)
{
    NV_STATUS status = NV_OK;

    // If NVLINK_LIB isn't enabled, we just execute prologue and return.
    _knvlinkRetrainLinkPrologue(pGpu, pKernelNvlink, linkId);

    OBJSYS *pSys    = SYS_GET_INSTANCE();

    // If fabric is managed by FM
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
#if defined(INCLUDE_NVLINK_LIB)

        //
        // Notify FM for link re-training.
        //
        // Note, at this point all DL interrupts should be disabled. The interrupts
        // will be enabled through nvlinkCoreReenableLinkInterruptsCallback only if
        // links can be successfully re-trained.
        //
        // It is responsibility of FM to highlight link re-training failures to the
        // system admin. Hence, we shouldn't be logging Xid in this case.
        //
        // It is worth to note that, there is no race in between interrupt
        // enable/disable register update as we notify FM only after disabling
        // interrupts.
        //
        gpuNotifySubDeviceEvent(pGpu,
                                NV2080_NOTIFIERS_NVLINK_ERROR_RECOVERY_REQUIRED,
                                NULL, 0, 0, (NvV16)NV2080_CTRL_NVLINK_UNIT_DL);

        return NV_OK;
#endif
    }

#if defined(INCLUDE_NVLINK_LIB)
    //
    // If this is a slave endpoint requesting the retrain, kick off a request
    // to the master instead. There is no need to (and indeed, we should not)
    // hold the master endpoint lock here.
    //
    if (!pKernelNvlink->nvlinkLinks[linkId].core_link->master)
    {
        nvlink_link_change *link_change;
        nvlink_link *slave, *master;

        slave = pKernelNvlink->nvlinkLinks[linkId].core_link;
        if (nvlink_lib_get_link_master(slave, &master) != NVL_SUCCESS)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "link master could not be found from GPU%u link %u\n",
                      gpuGetInstance(pGpu), linkId);

            return NV_ERR_INVALID_STATE;
        }

        NV_ASSERT_OR_RETURN(master != slave, NV_ERR_INVALID_STATE);

        link_change         = &slave->link_change;
        link_change->slave  = slave;
        link_change->master = master;
        link_change->change_type = bFromOff ? nvlink_retrain_from_off :
                                                  nvlink_retrain_from_safe;

        if (master->link_handlers->queue_link_change(link_change) != NVL_SUCCESS)
        {
            return NV_ERR_GENERIC;
        }

        //
        // Because the link retrain request to the master is asynchronous,
        // tell the caller they'll need to wait.
        //
        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }
#endif

    if (bFromOff)
    {
        status = knvlinkRetrainLinkFromOff(pGpu, pKernelNvlink, linkId);
    }
    else
    {
        status = knvlinkRetrainLinkFromSafe(pGpu, pKernelNvlink, linkId);
    }

    return status;
}

/*!
 * @brief Retrain the link from OFF state
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 * @param[in]  linkId         Link ID of the link in question
 *
 * @returns NV_OK if link retraining was successful
 */
NV_STATUS
knvlinkRetrainLinkFromOff
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId
)
{

    return NV_OK;
}

/*!
 * @brief Retrain the link from SAFE state
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 * @param[in]  linkId         Link ID of the link in question
 *
 * @returns NV_OK if link retraining was successful
 */
NV_STATUS
knvlinkRetrainLinkFromSafe
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId
)
{

    return NV_OK;
}

/*!
 * @brief _knvlinkRetrainLinkPrologue currently disables DL interrupts
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 * @param[in]  linkId         Link ID of the link in question
 */
static void
_knvlinkRetrainLinkPrologue
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId
)
{

    return;
}

#if defined(INCLUDE_NVLINK_LIB)

/*!
 * @brief Activate the connections discovered in topology discovery
 *
 * @param[in]  pGpu               OBJGPU pointer
 * @param[in]  pKernelNvlink      KernelNvlink pointer
 * @param[in]  bCheckDegradedMode Whether to check for degraded mode
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkActivateDiscoveredConns
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bCheckDegradedMode
)
{
    NvU32      initDisconnectedLinkMask = pKernelNvlink->disconnectedLinkMask;
    NvU32      switchLinkMasks          = 0;
    NvBool     bPeerUpdated             = NV_FALSE;
    NV_STATUS  status                   = NV_OK;
    NvU32      linkId;

    //
    // Degraded Mode on LR10+ systems. Check for degraded mode if this was not done before
    // and if new connections were discovered from the core library.
    //
    if (bCheckDegradedMode)
    {
        status = knvlinkApplyNvswitchDegradedModeSettings_HAL(pGpu, pKernelNvlink,
                                                              &switchLinkMasks);
    }

    // We only need to look at links that are considered disconnected
    FOR_EACH_INDEX_IN_MASK(32, linkId, initDisconnectedLinkMask)
    {
        if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected)
        {
            // This link is now marked connected
            pKernelNvlink->disconnectedLinkMask &= ~NVBIT(linkId);

            if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType
                    == NVLINK_DEVICE_TYPE_GPU)
            {
                bPeerUpdated = NV_TRUE;

                //
                // Activate the p2p link. This includes copying the remote device
                // information for the remote link and enabling the post topology
                // steps on both the ends of the link.
                //
                // NOTE: HSHUB will nott be setup for the discovered peer link here
                //       and will only be configured when a P2P object is created
                //
                status = _knvlinkActivateDiscoveredP2pConn(pGpu, pKernelNvlink, linkId);
            }
            else if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType
                    == NVLINK_DEVICE_TYPE_NVSWITCH)
            {
                status = _knvlinkActivateDiscoveredSwitchConn(pGpu, pKernelNvlink, linkId);

                //
                // There is no need to mark link as a master. On NVSwitch systems,
                // External Fabric Management should be enabled by default.
                //
                switchLinkMasks |= NVBIT(linkId);
            }
            else
            {
                //
                // Activate the sysmem link. This includes even training the link to
                // ACTIVE, since for sysmem link post-topology steps should be setup
                // only after ACTIVE
                //
                status = _knvlinkActivateDiscoveredSysmemConn(pGpu, pKernelNvlink, linkId);
            }

            // If any of the above failed, return failure
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to activate link%d on GPU%d!!!\n", linkId,
                          pGpu->gpuInstance);

                return status;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

#if defined(NVCPU_PPC64LE) || defined(NVCPU_AARCH64)
    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED))
    {
        // Credits should be released after Active for sysmem
        knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink, pKernelNvlink->enabledLinks);
        if (status != NV_OK)
        {
            return status;
        }

        // Enable SYSMEM links in HSHUB.  On P9 this must happen after Active
        knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);
    }
#endif

    // If any new connection was discovered in this call
    if (initDisconnectedLinkMask != pKernelNvlink->disconnectedLinkMask)
    {
        if (pKernelNvlink->disconnectedLinkMask == pKernelNvlink->enabledLinks) //GPU degraded case
        {
            bPeerUpdated |= _knvlinkUpdateSwitchLinkMasksGpuDegraded(pGpu, pKernelNvlink);
        }
        else // other cases
        {
            bPeerUpdated |= _knvlinkUpdateSwitchLinkMasks(pGpu, pKernelNvlink,
                                                          switchLinkMasks);
        }

        _knvlinkPrintTopologySummary(pGpu, pKernelNvlink);

        //
        // Make sure we update the CE mappings for this GPU, if the known set
        // of peers has changed.
        //
        knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);
        if (bPeerUpdated)
        {
            //
            // Request that any peers updated also update their CE mappings,
            // since they now have a new peer.
            //
            _knvlinkUpdatePeerConfigs(pGpu, pKernelNvlink);
        }
    }

    return status;
}

/*!
 * @brief Activate the given P2P connection
 *        This function updates the RM state for the discovered P2P connection
 *        and enables post-topology steps on both ends of the connection. But,
 *        it does not configure HSHUB on any end of the connection. HSHUB will
 *        be configured only when a P2P object is created
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 * @param[in]  linkId         Link ID
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkActivateDiscoveredP2pConn
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId
)
{
    OBJGPU       *pGpu0             = pGpu;
    OBJGPU       *pGpu1             = NULL;
    KernelNvlink *pKernelNvlink0    = GPU_GET_KERNEL_NVLINK(pGpu0);
    NV_STATUS     status            = NV_OK;
    NvBool        bUpdateConnStatus = NV_FALSE;
    NvU32         remoteLinkId;
    NvU32         gpuInst;

    // Get the remote OBJGPU and Nvlink
    for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
    {
        pGpu1 = gpumgrGetGpu(gpuInst);

        if (pGpu1 &&
            // Just rely on PCIe DBDF values for detecting the remote
            (pKernelNvlink0->nvlinkLinks[linkId].remoteEndInfo.domain   == gpuGetDomain(pGpu1)) &&
            (pKernelNvlink0->nvlinkLinks[linkId].remoteEndInfo.bus      == gpuGetBus(pGpu1))    &&
            (pKernelNvlink0->nvlinkLinks[linkId].remoteEndInfo.device   == gpuGetDevice(pGpu1)) &&
            (pKernelNvlink0->nvlinkLinks[linkId].remoteEndInfo.function == 0))
        {
            KernelNvlink *pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);

            // Map the remote GPU's instance number to the associated links on this GPU.
            status = knvlinkSetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1,
                                             (pKernelNvlink0->peerLinkMasks[gpuInst] | NVBIT(linkId)));
            if (status != NV_OK)
                return status;

            //
            // Post Topology enable on the local end of the link.
            // Needs to happen before HSHUB is setup for this link on any end.
            //
            status = knvlinkEnableLinksPostTopology_HAL(pGpu0, pKernelNvlink0, NVBIT(linkId));
            if (status != NV_OK)
            {
                return status;
            }

            // Set the remote device information for the remote device
            if (pKernelNvlink1)
            {
                remoteLinkId = pKernelNvlink0->nvlinkLinks[linkId].remoteEndInfo.linkNumber;

                // RPC into GSP-RM to update the link remote connection status only if its required
                if (pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.bConnected == NV_FALSE)
                    bUpdateConnStatus = NV_TRUE;

                // Set the PCI information for remote end
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.bConnected  = NV_TRUE;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.domain      = pKernelNvlink0->pNvlinkDev->pciInfo.domain;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.bus         = pKernelNvlink0->pNvlinkDev->pciInfo.bus;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.device      = pKernelNvlink0->pNvlinkDev->pciInfo.device;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.function    = pKernelNvlink0->pNvlinkDev->pciInfo.function;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.pciDeviceId = pKernelNvlink0->pNvlinkDev->pciInfo.pciDeviceId;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.deviceType  = pKernelNvlink0->pNvlinkDev->type;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.chipSid     = pKernelNvlink0->nvlinkLinks[linkId].core_link->localSid;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.linkNumber  = linkId;

                // Update the DLPL revision in the connection information
                pKernelNvlink0->nvlinkLinks[linkId].remoteEndInfo.ipVerDlPl = pKernelNvlink1->nvlinkLinks[remoteLinkId].ipVerDlPl;
                pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.ipVerDlPl = pKernelNvlink0->nvlinkLinks[linkId].ipVerDlPl;

                if (bUpdateConnStatus)
                {
                    // RPC into GSP-RM to update the link remote connection status for pGpu1 for the given link
                    status = knvlinkUpdateLinkConnectionStatus(pGpu1, pKernelNvlink1, remoteLinkId);
                    if (status != NV_OK)
                    {
                        return status;
                    }
                }

                pKernelNvlink1->disconnectedLinkMask &= ~NVBIT(remoteLinkId);

                // Map this GPU's instance number to the associated link on the remote end.
                status = knvlinkSetLinkMaskToPeer(pGpu1, pKernelNvlink1, pGpu0,
                                                  (pKernelNvlink1->peerLinkMasks[gpuGetInstance(pGpu0)] | NVBIT(remoteLinkId)));
                if (status != NV_OK)
                    return status;

                //
                // Post Topology enable on the remote end of the link.
                // Needs to happen before HSHUB is setup for this link on any end.
                //
                status = knvlinkEnableLinksPostTopology_HAL(pGpu1, pKernelNvlink1, NVBIT(remoteLinkId));
                if (status != NV_OK)
                {
                    return status;
                }

                // Set the deviceUUID
                portMemCopy(pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.devUuid,
                            NV_UUID_LEN,
                            pGpu0->gpuUuid.uuid,
                            NV_UUID_LEN);

                //
                // The master of a GPU <-> GPU link depends on instance number. This is so that when locking
                // (which requires the master to be locked before the slave), the lower GPU instance number
                // will always be locked first, which is how rmGpuLocksAcquire acquires them. For loopback,
                // fall back to link ID instead.
                //
                if ((gpuGetInstance(pGpu0) < gpuGetInstance(pGpu1)) ||
                    ((gpuGetInstance(pGpu0) == gpuGetInstance(pGpu1)) &&
                     (linkId < remoteLinkId)))
                {
                    NV_ASSERT(NVL_SUCCESS == nvlink_lib_set_link_master(
                            pKernelNvlink0->nvlinkLinks[linkId].core_link));
                }
                else if ((gpuGetInstance(pGpu1) < gpuGetInstance(pGpu0)) ||
                         ((gpuGetInstance(pGpu1) == gpuGetInstance(pGpu0)) &&
                          (remoteLinkId < linkId)))
                {
                    NV_ASSERT(NVL_SUCCESS == nvlink_lib_set_link_master(
                            pKernelNvlink1->nvlinkLinks[remoteLinkId].core_link));
                }

                break;
            }
        }
    }

    return status;
}

/*!
 * @brief Activate the given switch connection
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 * @param[in]  linkId        Link ID
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkActivateDiscoveredSwitchConn
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId
)
{
    NV_STATUS status = NV_OK;

    // Post Topology enablement for switch links
    status = knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink, NVBIT(linkId));
    if (status != NV_OK)
    {
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Activate the given P2P connection
 *        This function updates the RM state for the discovered sysmem
 *        connection and trains the connection to ACTIVE, because, for
 *        sysmem link post-topology steps can only be configured after
 *        ACTIVE. HSHUB is also configured for sysmem link here.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 * @param[in]  linkId        Link ID
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkActivateDiscoveredSysmemConn
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_NVLINK_UPDATE_HSHUB_MUX_PARAMS    updateHshubMuxParams;
    NV2080_CTRL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS nvlinkSysmemParams;

    pKernelNvlink->sysmemLinkMask |= NVBIT(linkId);

    portMemSet(&nvlinkSysmemParams, 0, sizeof(nvlinkSysmemParams));
    nvlinkSysmemParams.sysmemLinkMask = pKernelNvlink->sysmemLinkMask;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_SETUP_NVLINK_SYSMEM,
                                 (void *)&nvlinkSysmemParams,
                                 sizeof(nvlinkSysmemParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup HSHUB NVLink sysmem links state\n");
        return status;
    }

    // Always make the GPU side the master for NPU connections
    NV_ASSERT(NVL_SUCCESS == nvlink_lib_set_link_master(
        pKernelNvlink->nvlinkLinks[linkId].core_link));

    // Train SYSMEM links to Active, and only then enable traffic
    status = knvlinkTrainSysmemLinksToActive(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FAILED TO TRAIN CPU/SYSMEM LINKS TO ACTIVE on GPU%d!!!\n",
                  pGpu->gpuInstance);

        NV_ASSERT(0);
    }

    portMemSet(&updateHshubMuxParams, 0, sizeof(updateHshubMuxParams));
    updateHshubMuxParams.updateType = NV2080_CTRL_NVLINK_UPDATE_HSHUB_MUX_TYPE_PROGRAM;
    updateHshubMuxParams.bSysMem    = NV_TRUE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_UPDATE_HSHUB_MUX,
                                 (void *)&updateHshubMuxParams,
                                 sizeof(updateHshubMuxParams));
    return status;
}

/*!
 * @brief Transition the mask of links into sleep (L2) state
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 * @param[in]  linkMask      Mask of links
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkEnterSleep
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkMask
)
{
    NV_STATUS retStatus = NV_OK;
    NvlStatus status    = NVL_SUCCESS;

    NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS      programBufferRdyParams;
    NV2080_CTRL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS saveRestoreHshubStateParams;

    portMemSet(&programBufferRdyParams, 0, sizeof(programBufferRdyParams));
    programBufferRdyParams.flags        = NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_FLAGS_SAVE;
    programBufferRdyParams.bSysmem      = NV_FALSE;
    programBufferRdyParams.peerLinkMask = linkMask;

    // Save Bufferready state for the the mask of links entering L2
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_PROGRAM_BUFFERREADY,
                                 (void *)&programBufferRdyParams,
                                 sizeof(programBufferRdyParams));
    if (status != NV_OK)
        return status;

    portMemSet(&saveRestoreHshubStateParams, 0, sizeof(saveRestoreHshubStateParams));
    saveRestoreHshubStateParams.linkMask = linkMask;
    saveRestoreHshubStateParams.bSave    = NV_TRUE;

    // Save HSHUB SW state for the links which will need to be restored later
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_SAVE_RESTORE_HSHUB_STATE,
                                 (void *)&saveRestoreHshubStateParams,
                                 sizeof(saveRestoreHshubStateParams));
    if (status != NV_OK)
        return status;

    // In L2 Entry path
    pKernelNvlink->bL2Entry = NV_TRUE;

    // Put the mask of links of the device to sleep
    status = nvlink_lib_powerdown_links_from_active_to_L2(pKernelNvlink->pNvlinkDev,
                                                          linkMask,
                                                          NVLINK_STATE_CHANGE_ASYNC);
    if (status == NVL_MORE_PROCESSING_REQUIRED)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Transition to L2 for GPU%d: linkMask 0x%x in progress... Waiting for "
                  "remote endpoints to request L2 entry\n", pGpu->gpuInstance,
                  linkMask);

        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    if (status != NVL_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to put the linkmask 0x%x of GPU%d to SLEEP\n",
                  linkMask, pGpu->gpuInstance);

        return NV_ERR_GENERIC;
    }

    return retStatus;
}

/*!
 * @brief Wakeup the mask of links from sleep (L2) state
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 * @param[in]  linkMask      Mask of links
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkExitSleep
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkMask
)
{
    NvlStatus  status         = NVL_SUCCESS;
    NvU32      linkId;
    NvU32      remoteLinkId;
    NvU32      gpuInst;
    NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS      programBufferRdyParams;
    NV2080_CTRL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS saveRestoreHshubStateParams;
    pKernelNvlink->bL2Entry = NV_FALSE;
    {
        // Wakeup the mask of links of the device from sleep using legacy l2 exit
        status = nvlink_lib_train_links_from_L2_to_active(pKernelNvlink->pNvlinkDev,
                                                          linkMask,
                                                          NVLINK_STATE_CHANGE_ASYNC);
    }

    if (status == NVL_SUCCESS)
    {
        // Perform post-initialization setup for links that exited L2
        FOR_EACH_INDEX_IN_MASK(32, linkId, linkMask)
        {
            // Post topology link enable for pre-Ampere. This sets up buffer ready
            status = knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink, NVBIT(linkId));
            if (status != NV_OK)
            {
                return status;
            }

            // Update the current NVLink configuration
            knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);

            // Perform post-topology initialization steps on the remote endpoint
            if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_GPU)
            {
                OBJGPU       *pGpu1          = NULL;
                KernelNvlink *pKernelNvlink1 = NULL;

                // Get the remote OBJGPU and Nvlink
                for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
                {
                    pGpu1 = gpumgrGetGpu(gpuInst);

                    if (pGpu1 &&
                        (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain   == gpuGetDomain(pGpu1) &&
                         pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus      == gpuGetBus(pGpu1)    &&
                         pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device   == gpuGetDevice(pGpu1) &&
                         pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.function == 0))
                    {
                        pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
                        remoteLinkId   = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.linkNumber;

                        // Post topology link enable for pre-Ampere. This sets up buffer ready
                        status = knvlinkEnableLinksPostTopology_HAL(pGpu1, pKernelNvlink1, NVBIT(remoteLinkId));
                        if (status != NV_OK)
                        {
                            return status;
                        }

                        // Update the current NVLink configuration
                        knvlinkUpdateCurrentConfig(pGpu1, pKernelNvlink1);

                        break;
                    }
                }
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }

    //
    // Restore HSHUB *ONLY AFTER* links have been trained and post-topology setup is complete
    // on both ends of the link. Only then HSHUB can be configured for P2P on any side of link
    //
    if (status == NVL_SUCCESS)
    {
        portMemSet(&saveRestoreHshubStateParams, 0, sizeof(saveRestoreHshubStateParams));
        saveRestoreHshubStateParams.linkMask = linkMask;
        saveRestoreHshubStateParams.bSave    = NV_FALSE;

        // Restore HSHUB SW state for the links which exited L2 state
        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_NVLINK_SAVE_RESTORE_HSHUB_STATE,
                                     (void *)&saveRestoreHshubStateParams,
                                     sizeof(saveRestoreHshubStateParams));
        if (status != NV_OK)
            return status;

        knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);

        portMemSet(&programBufferRdyParams, 0, sizeof(programBufferRdyParams));
        programBufferRdyParams.flags        = NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_FLAGS_RESTORE;
        programBufferRdyParams.bSysmem      = NV_FALSE;
        programBufferRdyParams.peerLinkMask = linkMask;

        // Restore Bufferready state for the links which exited L2 state
        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_NVLINK_PROGRAM_BUFFERREADY,
                                     (void *)&programBufferRdyParams,
                                     sizeof(programBufferRdyParams));
        if (status != NV_OK)
            return status;

        FOR_EACH_INDEX_IN_MASK(32, linkId, linkMask)
        {
            if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_GPU)
            {
                OBJGPU       *pGpu1          = NULL;
                KernelNvlink *pKernelNvlink1 = NULL;

                // Get the remote OBJGPU and Nvlink
                for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
                {
                    pGpu1 = gpumgrGetGpu(gpuInst);

                    if (pGpu1 &&
                        (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain   == gpuGetDomain(pGpu1) &&
                         pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus      == gpuGetBus(pGpu1)    &&
                         pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device   == gpuGetDevice(pGpu1) &&
                         pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.function == 0))
                    {
                        pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
                        remoteLinkId   = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.linkNumber;

                        portMemSet(&saveRestoreHshubStateParams, 0, sizeof(saveRestoreHshubStateParams));
                        saveRestoreHshubStateParams.linkMask = NVBIT(remoteLinkId);
                        saveRestoreHshubStateParams.bSave    = NV_FALSE;

                        // Restore HSHUB SW state for the links which exited L2 state
                        status = knvlinkExecGspRmRpc(pGpu1, pKernelNvlink1,
                                                     NV2080_CTRL_CMD_NVLINK_SAVE_RESTORE_HSHUB_STATE,
                                                     (void *)&saveRestoreHshubStateParams,
                                                     sizeof(saveRestoreHshubStateParams));
                        if (status != NV_OK)
                            return status;

                        knvlinkUpdateCurrentConfig(pGpu1, pKernelNvlink1);

                        portMemSet(&programBufferRdyParams, 0, sizeof(programBufferRdyParams));
                        programBufferRdyParams.flags        = NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_FLAGS_RESTORE;
                        programBufferRdyParams.bSysmem      = NV_FALSE;
                        programBufferRdyParams.peerLinkMask = NVBIT(remoteLinkId);

                        //
                        // Restore Buffer Ready state for the links from cached SW state after HSHUB
                        // settings have been restored
                        //
                        status = knvlinkExecGspRmRpc(pGpu1, pKernelNvlink1,
                                                     NV2080_CTRL_CMD_NVLINK_PROGRAM_BUFFERREADY,
                                                     (void *)&programBufferRdyParams,
                                                     sizeof(programBufferRdyParams));
                        if (status != NV_OK)
                            return status;

                        break;
                    }
                }
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }

    if (status == NVL_MORE_PROCESSING_REQUIRED)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Transition to L0 for GPU%d: linkMask 0x%x in progress... Waiting for "
                  "remote endpoints to request L2 exit\n", pGpu->gpuInstance,
                  linkMask);

        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    if (status != NVL_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to wakeup the linkmask 0x%x of GPU%d from SLEEP\n",
                  linkMask, pGpu->gpuInstance);

        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

/*!
 * @brief Updates GPU peer info (peerMask) based on switchLinkMasks
 *
 * @param[in]  pGpu             OBJGPU pointer
 * @param[in]  pKernelNvlink    KernelNvlink pointer
 * @param[in]  switchLinkMasks  Mask of switch links
 *
 * @return  Returns NV_TRUE if peerMask is updated
 */
static NvBool
_knvlinkUpdateSwitchLinkMasks
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         switchLinkMasks
)
{
    KernelNvlink *pKernelNvlink1 = NULL;
    OBJGPU       *pGpu1          = NULL;
    NvBool        bPeerUpdated   = NV_FALSE;
    NV_STATUS     status         = NV_OK;
    NvU32         gpuInst;

    //
    // On NvSwitch systems, all the enabled and connected GPU links should
    // go through NvSwitch. We don't support GPU<->GPU or GPU<->NPU direct
    // connections on NvSwitch systems.
    //
    if (!knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        return bPeerUpdated;
    }

    for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
    {
        pGpu1 = gpumgrGetGpu(gpuInst);
        if (!pGpu1)
        {
            continue;
        }

        // No support for SLI P2P on nvswitch systems.
        if (IsSLIEnabled(pGpu1))
        {
            continue;
        }

        pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);

        if (!pKernelNvlink1)
        {
            continue;
        }

        if (!pKernelNvlink1->discoveredLinks)
        {
            continue;
        }

        if (!knvlinkIsGpuConnectedToNvswitch(pGpu1, pKernelNvlink1))
        {
            continue;
        }

        // Update local peerLinkMasks.
        status = knvlinkSetLinkMaskToPeer(pGpu, pKernelNvlink, pGpu1, switchLinkMasks);
        if (status != NV_OK)
            return NV_FALSE;

        //
        // Update remote peerLinkMasks only if a remote endpoint is connected.
        //
        // We are deliberately picking up loopback peerLinkMask, because it
        // represents the actual nvswitch connection mask for that GPU and
        // guarantees that the end point is connected to nvswitch.
        //
        status = knvlinkSetLinkMaskToPeer(pGpu1, pKernelNvlink1, pGpu,
                                pKernelNvlink1->peerLinkMasks[gpuGetInstance(pGpu1)]);
        if (status != NV_OK)
            return NV_FALSE;

        bPeerUpdated = NV_TRUE;
    }

    return bPeerUpdated;
}

/*!
 * @brief Updates GPU peer info (peerMask) when a GPU is degraded
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 *
 */
static NvBool
_knvlinkUpdateSwitchLinkMasksGpuDegraded
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    KernelNvlink *pKernelNvlink1 = NULL;
    OBJGPU       *pGpu1          = NULL;
    NvBool        bPeerUpdated   = NV_FALSE;
    NV_STATUS     status         = NV_OK;
    NvU32         gpuInst;

    for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
    {
        pGpu1 = gpumgrGetGpu(gpuInst);
        if (!pGpu1)
        {
            continue;
        }

        // No support for SLI P2P on nvswitch systems.
        if (IsSLIEnabled(pGpu1))
        {
            continue;
        }

        pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);

        if (!pKernelNvlink1)
        {
            continue;
        }

        if (!pKernelNvlink1->discoveredLinks)
        {
            continue;
        }

        if (!knvlinkIsGpuConnectedToNvswitch(pGpu1, pKernelNvlink1))
        {
            continue;
        }

        // Update local peerLinkMasks.
        status = knvlinkSetLinkMaskToPeer(pGpu, pKernelNvlink, pGpu1, 0);
        if (status != NV_OK)
            return NV_FALSE;

        // Update remote peerLinkMasks
        status = knvlinkSetLinkMaskToPeer(pGpu1, pKernelNvlink1, pGpu, 0);
        if (status != NV_OK)
            return NV_FALSE;

        bPeerUpdated = NV_TRUE;
    }

    return bPeerUpdated;
}

/*!
 * For each known peer, update their configurations, now that another
 * one of their peers (this GPU) has been initialized.
 *
 * This will update the PCE-LCE mappings, but it will not trigger any
 * HSHUB updates since peer IDs shouldn't have been allocated at this
 * point.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 */
static void
_knvlinkUpdatePeerConfigs
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32 gpuInst;

    for (gpuInst = 0; gpuInst < NV_ARRAY_ELEMENTS(pKernelNvlink->peerLinkMasks); gpuInst++)
    {
        if (pKernelNvlink->peerLinkMasks[gpuInst] != 0)
        {
            OBJGPU *pRemoteGpu = gpumgrGetGpu(gpuInst);

            if (pRemoteGpu != NULL)
            {
                KernelNvlink *pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);

                if (pRemoteKernelNvlink != NULL)
                {
                    NV_PRINTF(LEVEL_INFO,
                              "GPU%u requesting GPU%u NVLINK config update\n",
                              gpuGetInstance(pGpu),
                              gpuGetInstance(pRemoteGpu));

                    _knvlinkPrintTopologySummary(pRemoteGpu, pRemoteKernelNvlink);

                    // Update CE mappings on remote GPUs since we have new connections
                    knvlinkUpdateCurrentConfig(pRemoteGpu, pRemoteKernelNvlink);
                }
            }
        }
    }
}

/*!
 * Print the nvlink topology for this GPU
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 */
static void
_knvlinkPrintTopologySummary
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
#if NV_PRINTF_ENABLED

    NvU32     i;
    NV_STATUS status;

    if (DBG_RMMSG_CHECK(LEVEL_INFO) == 0)
    {
        return;
    }

    NV_PRINTF(LEVEL_INFO, "GPU%02u cached topology:\n", gpuGetInstance(pGpu));
 
    NV2080_CTRL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS params;
    portMemSet(&params, 0, sizeof(params));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to determine sysmem link mask\n");
        return;
    }

    // Print the discovered sysmem links
    if (params.sysmemLinkMask != 0)
    {
        NV_PRINTF(LEVEL_INFO, "    sysmem link mask : 0x%x\n", params.sysmemLinkMask);
    }

    // Print the discovered p2p links
    for (i = 0; i < NV_ARRAY_ELEMENTS(pKernelNvlink->peerLinkMasks); i++)
    {
        if (pKernelNvlink->peerLinkMasks[i] != 0)
        {
            NV_PRINTF(LEVEL_INFO, "    GPU%02u link mask  : 0x%x\n", i,
                      pKernelNvlink->peerLinkMasks[i]);
        }
    }

    // Print the links which do not have a connection yet
    if (pKernelNvlink->disconnectedLinkMask != 0)
    {
        NV_PRINTF(LEVEL_INFO, "    unknown link mask: 0x%x\n",
                  pKernelNvlink->disconnectedLinkMask);
    }

#endif
}

#endif
