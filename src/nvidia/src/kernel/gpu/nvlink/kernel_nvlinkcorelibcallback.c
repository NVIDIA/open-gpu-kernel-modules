/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "core/hal.h"
#include "core/locks.h"
#include "core/thread_state.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"

#if defined(INCLUDE_NVLINK_LIB)

static void _knvlinkCorePassiveLinkChangeCallback(NvU32, void *);

/*!
 * Compile time assert to ensure NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_BUFFER_SIZE ==
 * NVLINK_MAX_SEED_BUFFER_SIZE
 */
ct_assert(NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_BUFFER_SIZE ==
          NVLINK_MAX_SEED_BUFFER_SIZE);

/*!
 * Compile time asserts to ensure NV2080_INTERNAL_NVLINK_CORE_LINK_STATE* ==
 * NVLINK_LINKSTATE*
 */
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_OFF ==
          NVLINK_LINKSTATE_OFF);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_HS ==
          NVLINK_LINKSTATE_HS);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_SAFE ==
          NVLINK_LINKSTATE_SAFE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_FAULT ==
          NVLINK_LINKSTATE_FAULT);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_RECOVERY ==
          NVLINK_LINKSTATE_RECOVERY);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_FAIL ==
          NVLINK_LINKSTATE_FAIL);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DETECT ==
          NVLINK_LINKSTATE_DETECT);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_RESET ==
          NVLINK_LINKSTATE_RESET);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_ENABLE_PM ==
          NVLINK_LINKSTATE_ENABLE_PM);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DISABLE_PM ==
          NVLINK_LINKSTATE_DISABLE_PM);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_SLEEP ==
          NVLINK_LINKSTATE_SLEEP);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_SAVE_STATE ==
          NVLINK_LINKSTATE_SAVE_STATE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_RESTORE_STATE ==
          NVLINK_LINKSTATE_RESTORE_STATE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_PRE_HS ==
          NVLINK_LINKSTATE_PRE_HS);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DISABLE_ERR_DETECT ==
          NVLINK_LINKSTATE_DISABLE_ERR_DETECT);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_LANE_DISABLE ==
          NVLINK_LINKSTATE_LANE_DISABLE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_LANE_SHUTDOWN ==
          NVLINK_LINKSTATE_LANE_SHUTDOWN);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_TRAFFIC_SETUP ==
          NVLINK_LINKSTATE_TRAFFIC_SETUP);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITPHASE1 ==
          NVLINK_LINKSTATE_INITPHASE1);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITNEGOTIATE ==
          NVLINK_LINKSTATE_INITNEGOTIATE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_POST_INITNEGOTIATE ==
          NVLINK_LINKSTATE_POST_INITNEGOTIATE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITOPTIMIZE ==
          NVLINK_LINKSTATE_INITOPTIMIZE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_POST_INITOPTIMIZE ==
          NVLINK_LINKSTATE_POST_INITOPTIMIZE);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DISABLE_HEARTBEAT ==
          NVLINK_LINKSTATE_DISABLE_HEARTBEAT);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_CONTAIN ==
          NVLINK_LINKSTATE_CONTAIN);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITTL ==
          NVLINK_LINKSTATE_INITTL);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITPHASE5 ==
          NVLINK_LINKSTATE_INITPHASE5);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_ALI ==
          NVLINK_LINKSTATE_ALI);
ct_assert(NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_ACTIVE_PENDING ==
          NVLINK_LINKSTATE_ACTIVE_PENDING);

/*!
 * Compile time asserts to ensure NV2080_NVLINK_CORE_SUBLINK_STATE_TX* ==
 * NVLINK_SUBLINK_STATE_TX*
 */
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_HS ==
          NVLINK_SUBLINK_STATE_TX_HS);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_SINGLE_LANE ==
          NVLINK_SUBLINK_STATE_TX_SINGLE_LANE);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_TRAIN ==
          NVLINK_SUBLINK_STATE_TX_TRAIN);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_SAFE ==
          NVLINK_SUBLINK_STATE_TX_SAFE);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_OFF ==
          NVLINK_SUBLINK_STATE_TX_OFF);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_COMMON_MODE ==
          NVLINK_SUBLINK_STATE_TX_COMMON_MODE);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_COMMON_MODE_DISABLE ==
          NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_DATA_READY ==
          NVLINK_SUBLINK_STATE_TX_DATA_READY);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_EQ ==
          NVLINK_SUBLINK_STATE_TX_EQ);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_PRBS_EN ==
          NVLINK_SUBLINK_STATE_TX_PRBS_EN);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_TX_POST_HS ==
          NVLINK_SUBLINK_STATE_TX_POST_HS);

/*!
 * Compile time asserts to ensure NV2080_NVLINK_CORE_SUBLINK_STATE_RX* ==
 * NVLINK_SUBLINK_STATE_RX*
 */
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_RX_HS ==
          NVLINK_SUBLINK_STATE_RX_HS);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_RX_SINGLE_LANE ==
          NVLINK_SUBLINK_STATE_RX_SINGLE_LANE);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_RX_TRAIN ==
          NVLINK_SUBLINK_STATE_RX_TRAIN);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_RX_SAFE ==
          NVLINK_SUBLINK_STATE_RX_SAFE);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_RX_OFF ==
          NVLINK_SUBLINK_STATE_RX_OFF);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_RX_RXCAL ==
          NVLINK_SUBLINK_STATE_RX_RXCAL);
ct_assert(NV2080_NVLINK_CORE_SUBLINK_STATE_RX_INIT_TERM ==
          NVLINK_SUBLINK_STATE_RX_INIT_TERM);

/*!
 * @brief Callback function for adding link to nvlink core
 *
 * @param[in]  nvlink_link pointer
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreAddLinkCallback
(
    nvlink_link *link
)
{
    return 0;
}

/*!
 * @brief Callback function for removing link from nvlink core
 *
 * @param[in]  nvlink_link pointer
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreRemoveLinkCallback
(
    nvlink_link *link
)
{
    portMemFree((NvU8 *)link->linkName);
    link->linkName      = NULL;
    link->link_handlers = NULL;
    link->dev           = NULL;
    portMemFree(link);
    return 0;
}

/*!
 * @brief Callback function for locking a link so its state can be accessed
 *        and modified atomically.
 *
 * @param[in]  nvlink_link pointer
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreLockLinkCallback
(
    nvlink_link *link
)
{
    KNVLINK_RM_LINK *pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    OBJSYS          *pSys        = SYS_GET_INSTANCE();

    // Return if NVLink fabric is managed by FM
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        return NVL_SUCCESS;
    }

    //
    // We track the lock state of this API via the master/parent GPU of the
    // subdevice, since the locking APIs currently available to us operate at
    // the device level.
    //
    OBJGPU       *pGpu          = gpumgrGetParentGPU(pNvlinkLink->pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT_OR_RETURN(pKernelNvlink != NULL, NVL_ERR_INVALID_STATE);

    // First check if the lock is already held
    if (rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)))
    {
        //
        // If the lock is held with deviceLockRefcount == 0, it means the
        // lock was acquired before this function was first called for the
        // device, so we will not release the lock via the unlock callback.
        //
        if (pKernelNvlink->deviceLockRefcount > 0)
        {
            pKernelNvlink->deviceLockRefcount++;
            NV_PRINTF(LEVEL_INFO, "incremented device lock refcnt to %u\n",
                      pKernelNvlink->deviceLockRefcount);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "device lock acquired outside of the core library callbacks\n");
        }
    }
    else
    {
        if (rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_NVLINK) == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "acquired device GPU locks\n");

            pKernelNvlink->deviceLockRefcount++;
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "failed to acquire device GPU locks!\n");

            return NVL_ERR_GENERIC;
        }
    }

    return NVL_SUCCESS;
}

/*!
 * @brief Callback function for unlocking a link.
 *
 * This should only be called after nvlinkCoreLockLinkCallback().
 *
 * @param[in]  nvlink_link pointer
 */
void
knvlinkCoreUnlockLinkCallback
(
    nvlink_link *link
)
{
    KNVLINK_RM_LINK *pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    OBJSYS          *pSys        = SYS_GET_INSTANCE();

    // Return if NVLink fabric is managed by FM
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        return;
    }

    //
    // We track the lock state of this API via the master/parent GPU of the
    // subdevice, since the locking APIs currently available to us operate at
    // the device level.
    //
    OBJGPU       *pGpu          = gpumgrGetParentGPU(pNvlinkLink->pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT_OR_RETURN_VOID(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    if (pKernelNvlink == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pKernelNvlink is NULL, returning early\n");

        return;
    }

    if (pKernelNvlink->deviceLockRefcount > 0)
    {
        if (--pKernelNvlink->deviceLockRefcount == 0)
        {
            rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
            NV_PRINTF(LEVEL_INFO, "released device GPU locks\n");
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "decremented device lock refcnt to %u\n",
                      pKernelNvlink->deviceLockRefcount);
        }
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "device lock acquired outside of the core library callbacks\n");
    }
}

/*!
 * @brief Function to be executed when the master end
 *        of a link triggers the retraining of the link
 *
 * @param[in]  gpuInstance    Master GPU instance
 * @param[in]  linkChangeData Contains information of both ends
 */
static void
_knvlinkCorePassiveLinkChangeCallback
(
    NvU32  gpuInstance,
    void  *linkChangeData
)
{
    OBJGPU       *pGpu          = NULL;
    OBJSYS       *pSys          = SYS_GET_INSTANCE();
    KernelNvlink *pKernelNvlink = NULL;

    KNVLINK_RM_LINK    *pNvlinkLink;
    nvlink_link_change *link_change;
    nvlink_link        *slave;
    nvlink_link        *master;

    link_change = *(nvlink_link_change **)linkChangeData;
    master      = link_change->master;
    slave       = link_change->slave;
    pNvlinkLink = (KNVLINK_RM_LINK *)master->link_info;

    pGpu = gpumgrGetGpu(gpuInstance);
    NV_ASSERT(pGpu == pNvlinkLink->pGpu);

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // If link training is disabled through regkey
    if (pKernelNvlink && pKernelNvlink->bSkipLinkTraining)
    {
        return;
    }

    // If fabric is externally managed through FM, return
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        return;
    }

    if (osAcquireRmSema(pSys->pSema) == NV_OK)
    {
        if (master->link_handlers->lock(master) == NVL_SUCCESS)
        {
            if (slave->link_handlers->lock(slave) == NVL_SUCCESS)
            {
                if (pKernelNvlink != NULL)
                {
                    NvU32 linkId = pNvlinkLink->linkId;

                    //
                    // TODO: we should probably be smarter about detecting if
                    //       the master has already taken its own action in
                    //       retraining the link that would remove the need to
                    //       process this one. But for now, just blindly process
                    //       the link change request from the slave.
                    //
                    knvlinkRetrainLink(pGpu, pKernelNvlink, linkId,
                        link_change->change_type == nvlink_retrain_from_off);
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "master GPU does not support NVLINK!\n");
                    DBG_BREAKPOINT();
                }
                slave->link_handlers->unlock(slave);
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "failed to acquire slave lock!\n");
            }
            master->link_handlers->unlock(master);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "failed to acquire the master lock!\n");
        }
        osReleaseRmSema(pSys->pSema, NULL);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "failed to acquire the RM semaphore!\n");
    }

    return;
}

/*!
 * @brief Callback function for queuing a link change request from the
 *        link slave.
 *
 * This function is only called for links which are the master of the
 * connection.
 *
 * The master link is NOT locked at the time this callback is called,
 * and this callback must not attempt to acquire the master link lock.
 *
 * @param[in]  nvlink_link_change pointer
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreQueueLinkChangeCallback
(
    nvlink_link_change *link_change
)
{

    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu   = NULL;
    NV_STATUS        status = NV_OK;
    void            *pWorkItemData;

    pNvlinkLink = (KNVLINK_RM_LINK *)link_change->master->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        return NVL_ERR_INVALID_STATE;
    }

    // The master should be marked as such
    NV_ASSERT_OR_RETURN(link_change->master->master, NV_ERR_INVALID_STATE);

    pWorkItemData = portMemAllocNonPaged(sizeof(nvlink_link_change *));
    NV_ASSERT_OR_RETURN(pWorkItemData != NULL, NVL_NO_MEM);

    *((nvlink_link_change **)pWorkItemData) = link_change;

    //
    // This function will free the argument if it succeeds, hence the need for
    // the work item data wrapper.
    //
    status = osQueueWorkItem(pGpu, _knvlinkCorePassiveLinkChangeCallback,
                             pWorkItemData);
    if (status != NV_OK)
    {
        portMemFree(pWorkItemData);
        return NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*!
 * @brief Callback function for setting a DL link mode
 *
 * @param[in]  nvlink_link pointer
 * @param[in]  Link mode to be set
 * @param[in]  Flags
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreSetDlLinkModeCallback
(
    nvlink_link *link,
    NvU64        mode,
    NvU32        flags
)
{
    KNVLINK_RM_LINK  *pNvlinkLink;
    OBJGPU           *pGpu          = NULL;
    KernelNvlink     *pKernelNvlink = NULL;
    KernelIoctrl     *pKernelIoctrl = NULL;
    NV_STATUS         status        = NV_OK;
    NvU8              linkIndex;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_DL_LINK_MODE_PARAMS
                    *pSetDlLinkModeParams;

    portMemSet(&params, 0, sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    linkIndex     = pNvlinkLink->linkId;
    pKernelIoctrl = KNVLINK_LINK_GET_IOCTRL(pKernelNvlink, linkIndex);

    if ((pKernelNvlink->ipVerNvlink < NVLINK_VERSION_50) && (pKernelIoctrl == NULL))
        return 0;

    // If link training is disabled through regkey
    if (pKernelNvlink->bSkipLinkTraining)
    {
        return 0;
    }

    params.linkId = linkIndex;
    params.callbackType.type = NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_DL_LINK_MODE;

    pSetDlLinkModeParams = &params.callbackType.callbackParams.setDlLinkMode;
    pSetDlLinkModeParams->mode = mode;
    pSetDlLinkModeParams->bSync = (flags == NVLINK_STATE_CHANGE_SYNC) ?
                                   NV_TRUE : NV_FALSE;

    switch (mode)
    {
        case NVLINK_LINKSTATE_OFF:
        {
            pSetDlLinkModeParams->linkMode =
                NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_OFF;

            break;
        }
        case NVLINK_LINKSTATE_PRE_HS:
        {
            pSetDlLinkModeParams->linkMode =
                NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_PRE_HS;

            pSetDlLinkModeParams->linkModeParams.linkModePreHsParams.remoteDeviceType =
                pKernelNvlink->nvlinkLinks[linkIndex].remoteEndInfo.deviceType;
            pSetDlLinkModeParams->linkModeParams.linkModePreHsParams.ipVerDlPl =
                pKernelNvlink->nvlinkLinks[linkIndex].remoteEndInfo.ipVerDlPl;

            break;
        }
        case NVLINK_LINKSTATE_INITPHASE1:
        {
            pSetDlLinkModeParams->linkMode =
                NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITPHASE1;

            if ((pKernelIoctrl != NULL) && pKernelIoctrl->getProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS))
            {
                NvU32 *seedDataSrc  = pKernelNvlink->nvlinkLinks[linkIndex].core_link->seedData;
                NvU32 *seedDataDest =
                    pSetDlLinkModeParams->linkModeParams.linkModeInitPhase1Params.seedData;

                portMemCopy(seedDataDest, sizeof(*seedDataDest) * NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_BUFFER_SIZE,
                            seedDataSrc, sizeof(*seedDataSrc) * NVLINK_MAX_SEED_BUFFER_SIZE);
            }

            break;
        }
        case NVLINK_LINKSTATE_POST_INITNEGOTIATE:
        {
            pSetDlLinkModeParams->linkMode =
                NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_POST_INITNEGOTIATE;

            break;
        }
        case NVLINK_LINKSTATE_POST_INITOPTIMIZE:
        {
            NvU32 initoptimizeTimeout;
            THREAD_STATE_NODE threadNode;
            THREAD_STATE_NODE *pThreadNode = NULL;
            NvBool bDoThreadStateFree = NV_FALSE;

            status = threadStateGetCurrent(&pThreadNode, pGpu);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_INFO,
                          "Thread state not initialized!\n");

                //
                // There is a possiblity that the entrypoint to the
                // callback does not setup threadstate (ie. MODs).
                // If there is no thread state, then initialize it.
                //
                threadStateInit(&threadNode, THREAD_STATE_FLAGS_NONE);
                bDoThreadStateFree = NV_TRUE;

                // Getting thread state a second time should not fail
                status = threadStateGetCurrent(&pThreadNode, pGpu);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "Error getting current thread!\n");
                    threadStateFree(&threadNode, THREAD_STATE_FLAGS_NONE);
                    return 1;
                }

                NV_ASSERT(pThreadNode == &threadNode);
            }

            initoptimizeTimeout = gpuScaleTimeout(pGpu,
                NVLINK_INITOPTIMIZE_POLL_TIMEOUT);

            //
            // Override the thread state timeout,
            // so GSP doesn't timeout after 4 seconds
            //
            threadStateSetTimeoutOverride(pThreadNode,
                                          (NvU64)initoptimizeTimeout / 1000);

            pSetDlLinkModeParams->linkMode =
                NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_POST_INITOPTIMIZE;

            // Poll for 10 seconds to avoid GSP timeout
            while(1)
            {
                status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                             NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                             (void *)&params, sizeof(params));
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Error calling and polling for Init Optimize status! link 0x%x\n",
                              linkIndex);
                    break;
                }

                // Check if polling is done
                if (pSetDlLinkModeParams->linkModeParams.linkModePostInitOptimizeParams.bPollDone)
                {
                    break;
                }
                else
                {
                    // Add a 1 second delay so GSP isn't spammed with commands
                    osDelay(NVLINK_INITOPTIMIZE_POLL_COUNT_DELAY_MS);
                    osSpinLoop();
                }
            }

            if (bDoThreadStateFree)
            {
                threadStateFree(&threadNode, THREAD_STATE_FLAGS_NONE);
            }

            // Nothing else to do, return early
            return (status == NV_OK) ? 0 : 1;
        }
        default:
        {
            // Do nothing
            break;
        }
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error setting current link state: 0x%llx!\n", mode);
        return 1;
    }

    // Post control call operations
    switch (mode)
    {
        case NVLINK_LINKSTATE_SAVE_STATE:
        {
            // If the request was to save the link state, update on success
            link->bStateSaved = NV_TRUE;
            break;
        }
        case NVLINK_LINKSTATE_RESTORE_STATE:
        {
            // If the request was to restore the link state, update on success
            link->bStateSaved = NV_FALSE;
            break;
        }
        case NVLINK_LINKSTATE_OFF:
        {
            if ((pKernelIoctrl != NULL) && pKernelIoctrl->getProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS))
            {
                NvU32 *seedDataSrc  = pSetDlLinkModeParams->linkModeParams.linkModeOffParams.seedData;
                NvU32 *seedDataDest = pKernelNvlink->nvlinkLinks[linkIndex].core_link->seedData;

                portMemCopy(seedDataDest, sizeof(*seedDataDest) * NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_BUFFER_SIZE,
                            seedDataSrc, sizeof(*seedDataSrc) * NVLINK_MAX_SEED_BUFFER_SIZE);
            }

            break;
        }
        case NVLINK_LINKSTATE_POST_INITNEGOTIATE:
        {
            NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_POST_INITNEGOTIATE_PARAMS
                *pPostInitNegotiateParams;

            pPostInitNegotiateParams =
                &pSetDlLinkModeParams->linkModeParams.linkModePostInitNegotiateParams;

            // Save Remote/Local link SID info into core lib
            pKernelNvlink->nvlinkLinks[linkIndex].core_link->bInitnegotiateConfigGood =
                pPostInitNegotiateParams->bInitnegotiateConfigGood;
            pKernelNvlink->nvlinkLinks[linkIndex].core_link->remoteSid =
                pPostInitNegotiateParams->remoteLocalSidInfo.remoteSid;
            pKernelNvlink->nvlinkLinks[linkIndex].core_link->remoteDeviceType =
                pPostInitNegotiateParams->remoteLocalSidInfo.remoteDeviceType;
            pKernelNvlink->nvlinkLinks[linkIndex].core_link->remoteLinkId =
                pPostInitNegotiateParams->remoteLocalSidInfo.remoteLinkId;
            pKernelNvlink->nvlinkLinks[linkIndex].core_link->localSid =
                pPostInitNegotiateParams->remoteLocalSidInfo.localSid;

            break;
        }
        default:
        {
            // Do nothing
            break;
        }
    }

    return 0;
}

/*!
 * @brief Callback function for getting a DL link mode
 *
 * @param[in]  nvlink_link pointer
 * @param[out] Current mode of the link
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreGetDlLinkModeCallback
(
    nvlink_link *link,
    NvU64       *mode
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type = NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_DL_LINK_MODE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error getting current link state!\n");
        return 1;
    }

    *mode = (NvU64) params.callbackType.callbackParams.getDlLinkMode.mode;
    return 0;
}

/*!
 * @brief Callback function for setting a TL link mode
 *
 * @param[in]  nvlink_link pointer
 * @param[in]  Link mode to be set
 * @param[in]  Flags
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreSetTlLinkModeCallback
(
    nvlink_link *link,
    NvU64        mode,
    NvU32        flags
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TL_LINK_MODE_PARAMS
                    *pSetTlLinkModeParams;

    portMemSet(&params, 0, sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // If link training is disabled through regkey
    if (pKernelNvlink->bSkipLinkTraining)
    {
        return 0;
    }

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type = NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_TL_LINK_MODE;

    pSetTlLinkModeParams        = &params.callbackType.callbackParams.setTlLinkMode;
    pSetTlLinkModeParams->mode  = mode;
    pSetTlLinkModeParams->bSync = (flags == NVLINK_STATE_CHANGE_SYNC) ?
                                   NV_TRUE : NV_FALSE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error setting current link state!\n");
        return 1;
    }

    return 0;
}

/*!
 * @brief Callback function for getting a TL link mode
 *
 * @param[in]  nvlink_link pointer
 * @param[out] Current mode of the link
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreGetTlLinkModeCallback
(
    nvlink_link *link,
    NvU64       *mode
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type = NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_TL_LINK_MODE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error getting current link state!\n");
        return 1;
    }

    *mode = (NvU64) params.callbackType.callbackParams.getTlLinkMode.mode;
    return 0;
}

/*!
 * @brief Callback function for setting Tx sublink mode
 *
 * @param[in]  nvlink_link pointer
 * @param[in]  TX Sublink mode to be set
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreSetTxSublinkModeCallback
(
    nvlink_link *link,
    NvU64        mode,
    NvU32        flags
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TX_SUBLINK_MODE_PARAMS
        *pSetTxSublinkModeParams;

    portMemSet(&params, 0, sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // If link training is disabled through regkey
    if (pKernelNvlink->bSkipLinkTraining)
    {
        return 0;
    }

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type =
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_TX_SUBLINK_MODE;

    pSetTxSublinkModeParams        = &params.callbackType.callbackParams.setTxSublinkMode;
    pSetTxSublinkModeParams->mode  = mode;
    pSetTxSublinkModeParams->bSync = (flags == NVLINK_STATE_CHANGE_SYNC) ?
                                      NV_TRUE : NV_FALSE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error setting TX sublink mode. mode = 0x%08llx\n", mode);
        return 1;
    }

    switch(mode)
    {
        case NVLINK_SUBLINK_STATE_TX_COMMON_MODE:
        case NVLINK_SUBLINK_STATE_TX_EQ:
        case NVLINK_SUBLINK_STATE_TX_DATA_READY:
            link->tx_sublink_state = (NvU32) mode;
            break;
        default:
            break;
    }

    return 0;
}

/*!
 * @brief Callback function for setting Rx sublink mode
 *
 * @param[in]  nvlink_link pointer
 * @param[in]  RX Sublink mode to be set
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreSetRxSublinkModeCallback
(
    nvlink_link *link,
    NvU64        mode,
    NvU32        flags
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_SUBLINK_MODE_PARAMS
                    *pSetRxSublinkModeParams;

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // If link training is disabled through regkey
    if (pKernelNvlink->bSkipLinkTraining)
    {
        return 0;
    }

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type =
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_RX_SUBLINK_MODE;

    pSetRxSublinkModeParams        =
        &params.callbackType.callbackParams.setRxSublinkMode;
    pSetRxSublinkModeParams->mode  = mode;
    pSetRxSublinkModeParams->bSync = (flags == NVLINK_STATE_CHANGE_SYNC) ?
                                      NV_TRUE : NV_FALSE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error setting RX sublink mode!\n");
        return 1;
    }

    switch(mode)
    {
        case NVLINK_SUBLINK_STATE_RX_RXCAL:
        case NVLINK_SUBLINK_STATE_RX_INIT_TERM:
            link->rx_sublink_state = (NvU32) mode;
            break;
        default:
            break;
    }

    return 0;
}

/*!
 * @brief Callback function for getting Tx sublink mode
 *
 * @param[in]  nvlink_link pointer
 * @param[out] Current mode of the TX sublink
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreGetTxSublinkModeCallback
(
    nvlink_link *link,
    NvU64       *mode,
    NvU32       *subMode
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    // Initialize to default values
    params.callbackType.callbackParams.getTxSublinkMode.sublinkMode =
        NVLINK_SUBLINK_STATE_TX_OFF;
    params.callbackType.callbackParams.getTxSublinkMode.sublinkSubMode =
        NVLINK_SUBLINK_SUBSTATE_TX_STABLE;

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type =
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_TX_SUBLINK_MODE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error getting current TX sublink state!\n");
        return 1;
    }

    *mode    = (NvU64) params.callbackType.callbackParams.getTxSublinkMode.sublinkMode;
    *subMode = params.callbackType.callbackParams.getTxSublinkMode.sublinkSubMode;
    return 0;
}

/*!
 * @brief Callback function for getting Rx sublink mode
 *
 * @param[in]  nvlink_link pointer
 * @param[out] Current mode of the RX sublink
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreGetRxSublinkModeCallback
(
    nvlink_link *link,
    NvU64       *mode,
    NvU32       *subMode
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    // Initialize to default values
    params.callbackType.callbackParams.getRxSublinkMode.sublinkMode =
        NVLINK_SUBLINK_STATE_RX_OFF;
    params.callbackType.callbackParams.getRxSublinkMode.sublinkSubMode =
        NVLINK_SUBLINK_SUBSTATE_RX_STABLE;

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type =
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_RX_SUBLINK_MODE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error getting current RX sublink state!\n");
        return 1;
    }

    *mode    = (NvU64) params.callbackType.callbackParams.getRxSublinkMode.sublinkMode;
    *subMode = params.callbackType.callbackParams.getRxSublinkMode.sublinkSubMode;;
    return 0;
}

/*!
 * @brief Callback function for performing receiver detect
 *
 * @param[in]  nvlink_link pointer
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreSetRxSublinkDetectCallback
(
    nvlink_link *link,
    NvU32        flags
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_DETECT_PARAMS
                    *pSetRxDetectParams;

    portMemSet(&params, 0, sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type =
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_RX_SUBLINK_DETECT;

    pSetRxDetectParams        =
        &params.callbackType.callbackParams.setRxSublinkDetect;
    pSetRxDetectParams->bSync = (flags == NVLINK_STATE_CHANGE_SYNC) ?
                                 NV_TRUE : NV_FALSE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error performing RXDET (Receiver Detect) on link!\n");
        return 1;
    }

    return 0;
}

/*!
 * @brief Callback function for getting status of receiver detect
 *
 * @param[in]  nvlink_link pointer
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreGetRxSublinkDetectCallback
(
    nvlink_link *link
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NvU32            linkId;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_RX_DETECT_PARAMS
                    *pGetRxDetectParams;

    portMemSet(&params, 0,  sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;
    pGpu        = pNvlinkLink->pGpu;
    linkId      = pNvlinkLink->linkId;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId            = linkId;
    params.callbackType.type =
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_RX_SUBLINK_DETECT;

    pGetRxDetectParams = &params.callbackType.callbackParams.getRxSublinkDetect;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));

    // Store RXDET status mask
    pKernelNvlink->nvlinkLinks[linkId].laneRxdetStatusMask =
        pGetRxDetectParams->laneRxdetStatusMask;

    // Update bRxDetected field based on RXDET status
    link->bRxDetected = (status == NV_OK ? NV_TRUE : NV_FALSE);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "RXDET (Receiver Detect) failed on link!\n");
        return 1;
    }

    return 0;
}

/*!
 * @brief Callback function for sending a discovery token over a link
 *
 * @param[in]  nvlink_link pointer
 * @param[in]  Token to be sent on the link
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreWriteDiscoveryTokenCallback
(
    nvlink_link *link,
    NvU64        token
)
{
    KNVLINK_RM_LINK *pNvlinkLink   = (KNVLINK_RM_LINK *)link->link_info;
    OBJGPU          *pGpu          = pNvlinkLink->pGpu;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    //
    // If Nvlink4.0+ get the "token" values via SIDs stored
    // by MINION
    //
    if (pNvlinkLink->ipVerDlPl >= NVLINK_VERSION_40)
    {
        NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS params;
        portMemSet(&params, 0, sizeof(params));
        params.linkId = pNvlinkLink->linkId;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID,
                                     (void *)&params, sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Error updating Local/Remote SID Info!\n");
            return status;
        }

        link->remoteSid =
            params.remoteLocalSidInfo.remoteSid;
        link->remoteDeviceType =
            params.remoteLocalSidInfo.remoteDeviceType;
        link->remoteLinkId =
            params.remoteLocalSidInfo.remoteLinkId;
        link->localSid =
            params.remoteLocalSidInfo.localSid;
    }
    else
    {

        NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_RD_WR_DISCOVERY_TOKEN_PARAMS
                *pWriteDiscoveryTokenParams;

        portMemSet(&params, 0, sizeof(params));
        params.linkId            = pNvlinkLink->linkId;
        params.callbackType.type =
                NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_WRITE_DISCOVERY_TOKEN;

        pWriteDiscoveryTokenParams            =
            &params.callbackType.callbackParams.writeDiscoveryToken;
        pWriteDiscoveryTokenParams->ipVerDlPl = pNvlinkLink->ipVerDlPl;
        pWriteDiscoveryTokenParams->token     = token;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                     (void *)&params, sizeof(params));
    }

    if (status != NV_OK)
    {
        if (status != NV_ERR_NOT_SUPPORTED)
        {
            NV_PRINTF(LEVEL_ERROR, "Error writing Discovery Token!\n");
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "R4 Tokens not supported on the chip!\n");
        }

        return 1;
    }

    return 0;
}

/*!
 * @brief Callback function for getting a discovery token on a link
 *
 * @param[in]   nvlink_link pointer
 * @param[out]  Token received on the link
 *
 * @returns NVL_SUCCESS on success
 */
NvlStatus
knvlinkCoreReadDiscoveryTokenCallback
(
    nvlink_link *link,
    NvU64       *token
)
{
    KNVLINK_RM_LINK *pNvlinkLink   = (KNVLINK_RM_LINK *)link->link_info;
    OBJGPU          *pGpu          = pNvlinkLink->pGpu;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status        = NV_OK;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_RD_WR_DISCOVERY_TOKEN_PARAMS *pReadDiscoveryTokenParams = NULL;

    portMemSet(&params, 0, sizeof(params));

    if (token == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Bad token address provided!\n");
        return 1;
    }

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // If Nvlink4.0+ then reading tokens is no longer supported
    if (pNvlinkLink->ipVerDlPl >= NVLINK_VERSION_40)
    {
        status = NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        params.linkId            = pNvlinkLink->linkId;
        params.callbackType.type =
            NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_READ_DISCOVERY_TOKEN;

        pReadDiscoveryTokenParams            =
            &params.callbackType.callbackParams.readDiscoveryToken;
        pReadDiscoveryTokenParams->ipVerDlPl = pNvlinkLink->ipVerDlPl;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                     (void *)&params, sizeof(params));
    }

    if (status != NV_OK)
    {
        if (status != NV_ERR_NOT_SUPPORTED)
        {
            NV_PRINTF(LEVEL_ERROR, "Error reading discovery token!\n");
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "R4 Tokens not supported on the chip!\n");
        }

        return 1;
    }

    *token = pReadDiscoveryTokenParams->token;

    return 0;
}

/*!
 * @brief Callback function for post link training tasks.
 *
 * @param[in]   nvlink_link pointer
 */
void
knvlinkCoreTrainingCompleteCallback
(
    nvlink_link *link
)
{
    KNVLINK_RM_LINK *pNvlinkLink;
    OBJGPU          *pGpu          = NULL;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    pNvlinkLink = (KNVLINK_RM_LINK *)link->link_info;

    pGpu          = pNvlinkLink->pGpu;
    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId = pNvlinkLink->linkId;
    params.callbackType.type = NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_TRAINING_COMPLETE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error issuing NvLink Training Complete callback!\n");
    }
}

NvlStatus
knvlinkCoreGetCciLinkModeCallback
(
    nvlink_link *link,
    NvU64       *mode
)
{
    return NVL_SUCCESS;
}

/*
 * @brief nvlinkCoreGetUphyLoadCallback send ALI training on the specified link
 *
 * @param[in]  link        nvlink_link pointer
 */
NvlStatus
knvlinkCoreAliTrainingCallback
(
    nvlink_link *link
)
{
    KNVLINK_RM_LINK *pNvlinkLink = (KNVLINK_RM_LINK *) link->link_info;
    OBJGPU          *pGpu        = pNvlinkLink->pGpu;
    KernelNvlink   * pKernelNvlink     = NULL;
    NV_STATUS        status;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return 1;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (pKernelNvlink->ipVerNvlink >= NVLINK_VERSION_50)
        return 0;

    status = knvlinkPreTrainLinksToActiveAli(pGpu, pKernelNvlink,
                                                 BIT(pNvlinkLink->linkId), NV_TRUE);
    if (status != NV_OK)
    {
        goto knvlinkCoreAliTrainingCallback_end;
    }

    status = knvlinkTrainLinksToActiveAli(pGpu, pKernelNvlink, NVBIT(pNvlinkLink->linkId), NV_FALSE);

knvlinkCoreAliTrainingCallback_end:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to request Link %d to transition to active\n", pNvlinkLink->linkId);
        return 1;
    }

    return 0;
}

/*!
 * @brief nvlinkCoreGetUphyLoadCallback checks if uphy is locked
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[in]  link        nvlink_link pointer
 * @param[out] bUnlocked   Uphy is locked or unlocked
 */
void
knvlinkCoreGetUphyLoadCallback
(
    nvlink_link *link,
    NvBool      *bUnlocked
)
{
    KNVLINK_RM_LINK *pNvlinkLink   = (KNVLINK_RM_LINK *)link->link_info;
    OBJGPU          *pGpu          = pNvlinkLink->pGpu;
    KernelNvlink    *pKernelNvlink = NULL;
    NV_STATUS        status;
    NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS params;
    NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_UPHY_LOAD_PARAMS
        *pGetUphyLoadParams;

    portMemSet(&params, 0, sizeof(params));

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Error processing link info!\n");
        return;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    params.linkId            = pNvlinkLink->linkId;
    params.callbackType.type =
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_UPHY_LOAD;

    pGetUphyLoadParams =
        &params.callbackType.callbackParams.getUphyLoad;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error issuing NvLink Get Uphy Load callback!\n");
    }

    *bUnlocked = pGetUphyLoadParams->bUnlocked;
}

#endif
