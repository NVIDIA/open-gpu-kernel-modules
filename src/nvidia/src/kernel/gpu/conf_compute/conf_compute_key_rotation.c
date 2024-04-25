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

#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/conf_compute/conf_compute.h"
#include "gpu/conf_compute/conf_compute_keystore.h"
#include "class/clc86fsw.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"

static NV_STATUS performKeyRotationByKeyPair(OBJGPU *pGpu, ConfidentialCompute *pConfCompute,
                                             NvU32 h2dKey, NvU32 d2hKey);

// Callback that will check stats and trigger key rotation
void
confComputeKeyRotationCallback
(
    OBJGPU *pGpu,
    void   *data
)
{
    NV_STATUS status;
    status = confComputeTriggerKeyRotation_HAL(pGpu, GPU_GET_CONF_COMPUTE(pGpu));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Key rotation callback failed with status 0x%x\n", status);
        NV_ASSERT(status == NV_OK);
    }
}

void
performKeyRotation_WORKITEM
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    KEY_ROTATION_WORKITEM_INFO *pWorkItemInfo = (KEY_ROTATION_WORKITEM_INFO *)pArgs;
    NvU32 h2dKey = pWorkItemInfo->h2dKey;
    NvU32 d2hKey = pWorkItemInfo->d2hKey;
    KernelChannel *pKernelChannel = NULL;
    NvU16 notifyStatus = 0x0;
    CHANNEL_ITERATOR iter = {0};
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS params = {0};
    NV_STATUS status = NV_OK;

    if (pWorkItemInfo->status == KEY_ROTATION_STATUS_PENDING)
    {
        // This means all channels reported idle and we can go ahead with KR
        status = performKeyRotationByKeyPair(pGpu, pConfCompute, h2dKey, d2hKey);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to perform key rotation with status = 0x%x for h2dKey = 0x%x\n", status, h2dKey);
            NV_ASSERT_OK(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_FAILED_ROTATION));
            goto done;
        }
    }
    else if ((pWorkItemInfo->status == KEY_ROTATION_STATUS_FAILED_THRESHOLD) ||
             (pWorkItemInfo->status == KEY_ROTATION_STATUS_FAILED_TIMEOUT))
    {
        // This means we need to notify and RC non-idle channels and go ahead with KR
        NV_ASSERT_OR_RETURN_VOID(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter) == NV_OK);
        while(confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) == NV_OK)
        {
            if (!kchannelIsDisabledForKeyRotation(pGpu, pKernelChannel))
            {
                // update notifier memory
                notifyStatus =
                    FLD_SET_DRF(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _IN_PROGRESS, _FALSE, notifyStatus);

                notifyStatus =
                    FLD_SET_DRF_NUM(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _VALUE, pWorkItemInfo->status, notifyStatus);

                NV_ASSERT_OK(kchannelUpdateNotifierMem(pKernelChannel, NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS,
                                                       0, 0, notifyStatus));

                NV_PRINTF(LEVEL_INFO, "chid 0x%x was NOT disabled for key rotation, writing notifier with val 0x%x\n", kchannelGetDebugTag(pKernelChannel), (NvU32)notifyStatus);
                // send events to clients if registered
                kchannelNotifyEvent(pKernelChannel, NVC86F_NOTIFIERS_KEY_ROTATION, 0, pWorkItemInfo->status, NULL, 0);
            }
        }

        // RC all non-idle channels
        params.globalH2DKey = h2dKey;
        status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION,
                                 &params, sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Control call to RC non-idle channels failed with status 0x%x, can't perform key rotation for h2dKey = 0x%x\n",
                      status, h2dKey);
            NV_ASSERT_OK(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_FAILED_ROTATION));
            goto done;
        }

        // perform key rotation
        status = performKeyRotationByKeyPair(pGpu, pConfCompute, h2dKey, d2hKey);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to perform key rotation with status = 0x%x for h2dKey = 0x%x\n", status, h2dKey);
            NV_ASSERT_OK(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_FAILED_ROTATION));
            goto done;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Unexpected key rotation status 0x%x\n", pWorkItemInfo->status);
        status = NV_ERR_INVALID_STATE;
    }

done:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "KR fialed with status 0x%x\n", status);
        // TODO CONFCOMP-984: Implement failure sequence from kpadwal
    }
}

static NV_STATUS
performKeyRotationByKeyPair
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey,
    NvU32 d2hKey
)
{
    KernelChannel *pKernelChannel = NULL;
    NvU16  notifyStatus = 0x0;
    CHANNEL_ITERATOR iter = {0};
    NvU32 h2dIndex, d2hIndex;

    NV_ASSERT_OK_OR_RETURN(confComputeUpdateSecrets_HAL(pConfCompute, h2dKey));

    // notify clients
    NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter));
	while(confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) == NV_OK)
    {
        if (kchannelIsDisabledForKeyRotation(pGpu, pKernelChannel))
        {
            // update notifier memory
            notifyStatus =
                FLD_SET_DRF(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _IN_PROGRESS, _FALSE, notifyStatus);

            notifyStatus =
                FLD_SET_DRF_NUM(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _VALUE, (NvU16)KEY_ROTATION_STATUS_IDLE, notifyStatus);

            NV_ASSERT_OK(kchannelUpdateNotifierMem(pKernelChannel, NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS,
                                                   0, 0, notifyStatus));

            // send events to clients if registered
            kchannelNotifyEvent(pKernelChannel, NVC86F_NOTIFIERS_KEY_ROTATION, 0, (NvU16)KEY_ROTATION_STATUS_IDLE, NULL, 0);
            NV_PRINTF(LEVEL_INFO, "chid 0x%x was disabled for key rotation, writing notifier with val 0x%x\n", kchannelGetDebugTag(pKernelChannel), (NvU32)notifyStatus);

            // also reset channel sw state
            kchannelDisableForKeyRotation(pGpu, pKernelChannel, NV_FALSE);
            kchannelEnableAfterKeyRotation(pGpu, pKernelChannel, NV_FALSE);
        }

        // clear encrypt stats irrespective of whether this channel was reported idle or not.
        if (pKernelChannel->pEncStatsBuf != NULL)
            portMemSet(pKernelChannel->pEncStatsBuf, 0, sizeof(CC_CRYPTOBUNDLE_STATS));
    }

    // reset KR state
    pConfCompute->keyRotationCallbackCount = 1;

    // clear aggregate and freed channel stats
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));

    pConfCompute->aggregateStats[h2dIndex].totalBytesEncrypted = 0;
    pConfCompute->aggregateStats[h2dIndex].totalEncryptOps     = 0;
    pConfCompute->aggregateStats[d2hIndex].totalBytesEncrypted = 0;
    pConfCompute->aggregateStats[d2hIndex].totalEncryptOps     = 0;

    pConfCompute->freedChannelAggregateStats[h2dIndex].totalBytesEncrypted = 0;
    pConfCompute->freedChannelAggregateStats[h2dIndex].totalEncryptOps     = 0;
    pConfCompute->freedChannelAggregateStats[d2hIndex].totalBytesEncrypted = 0;
    pConfCompute->freedChannelAggregateStats[d2hIndex].totalEncryptOps     = 0;

    NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_IDLE));
    return NV_OK;
}

/*!
 * Checks if all channels corresponding to key pair
 * are disabled and schedules key rotation.
 *
 * @param[in]   pGpu            : OBJGPU pointer
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[out]  h2dKey          : h2d key
 * @param[out]  d2hKey          : d2h key
 */
NV_STATUS
confComputeCheckAndScheduleKeyRotation_IMPL
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey,
    NvU32 d2hKey
)
{
    CHANNEL_ITERATOR iter = {0};
    KernelChannel *pKernelChannel = NULL;
    KEY_ROTATION_STATUS state;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeyRotationStatus(pConfCompute, h2dKey, &state));
    NV_ASSERT_OR_RETURN(state == KEY_ROTATION_STATUS_PENDING, NV_ERR_INVALID_STATE);
    NvBool bIdle = NV_TRUE;

    NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter));
    while(confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) == NV_OK)
    {
        // check if all channels are idle
        if (!kchannelIsDisabledForKeyRotation(pGpu, pKernelChannel))
        {
            NV_PRINTF(LEVEL_INFO, "chid 0x%x was NOT disabled for key rotation, can't start KR yet\n", kchannelGetDebugTag(pKernelChannel));
            bIdle = NV_FALSE;
            break;
        }
    }

    // if all channels are idle, trigger key rotation
    if (bIdle)
    {
        NV_PRINTF(LEVEL_INFO, "scheduling KR for h2d key = 0x%x\n", h2dKey);
        NV_ASSERT_OK_OR_RETURN(confComputeScheduleKeyRotationWorkItem(pGpu, pConfCompute, h2dKey, d2hKey));
    }
    return NV_OK;
}

/*!
 * schedules key rotation workitem
 *
 * @param[in]   pGpu            : OBJGPU pointer
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[out]  h2dKey          : h2d key
 * @param[out]  d2hKey          : d2h key
 */
NV_STATUS
confComputeScheduleKeyRotationWorkItem_IMPL
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey,
    NvU32 d2hKey
)
{
    KEY_ROTATION_STATUS status;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeyRotationStatus(pConfCompute, h2dKey, &status));
    if (status == KEY_ROTATION_STATUS_IN_PROGRESS)
    {
        NV_PRINTF(LEVEL_INFO, "Key rotation is already scheduled for key 0x%x\n", h2dKey);
        return NV_OK;
    }

    // pWorkItemInfo will be freed by RmExecuteWorkItem after work item is done execution
    KEY_ROTATION_WORKITEM_INFO *pWorkItemInfo = portMemAllocNonPaged(sizeof(KEY_ROTATION_WORKITEM_INFO));
    NV_ASSERT_OR_RETURN(pWorkItemInfo != NULL, NV_ERR_NO_MEMORY);
    pWorkItemInfo->h2dKey = h2dKey;
    pWorkItemInfo->d2hKey = d2hKey;
    pWorkItemInfo->status = status;
    NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_IN_PROGRESS));

    // cancel timeout event in case it was scheduled
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU32 h2dIndex;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    if (pConfCompute->ppKeyRotationTimer[h2dIndex] != NULL)
    {
        tmrEventCancel(pTmr, pConfCompute->ppKeyRotationTimer[h2dIndex]);
    }

    // Queue workitem to perform key rotation
    NV_ASSERT_OK_OR_RETURN(osQueueWorkItemWithFlags(pGpu, performKeyRotation_WORKITEM, (void*)pWorkItemInfo,
                                                    (OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA |
                                                     OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW |
                                                     OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS_RW)));
    return NV_OK;
}

/*!
 * Sets KEY_ROTATION_STATUS for key pair corresponding to given key
 *
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[in]   globalKey       : key for which to set the status
 * @param[in]   status          : KEY_ROTATION_STATUS* value
 */
NV_STATUS confComputeSetKeyRotationStatus_IMPL
(
    ConfidentialCompute *pConfCompute,
    NvU32 globalKey,
    KEY_ROTATION_STATUS status
)
{
    NvU32 h2dKey, d2hKey;
    confComputeGetKeyPairByKey(pConfCompute, globalKey, &h2dKey, &d2hKey);
    NvU32 h2dIndex, d2hIndex;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));
    pConfCompute->keyRotationState[h2dIndex] = status;
    pConfCompute->keyRotationState[d2hIndex] = status;
    return NV_OK;
}

/*!
 * Gets KEY_ROTATION_STATUS for given key
 *
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[in]   globalKey       : key for which to set the status
 * @param[out]  pStatus         : KEY_ROTATION_STATUS* value
 */
NV_STATUS confComputeGetKeyRotationStatus_IMPL
(
    ConfidentialCompute *pConfCompute,
    NvU32 globalKey,
    KEY_ROTATION_STATUS* pStatus
)
{
    NvU32 h2dKey, d2hKey;
    confComputeGetKeyPairByKey(pConfCompute, globalKey, &h2dKey, &d2hKey);
    NvU32 h2dIndex, d2hIndex;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));
    NV_ASSERT_OR_RETURN(pConfCompute->keyRotationState[h2dIndex] ==
                        pConfCompute->keyRotationState[d2hIndex], NV_ERR_INVALID_STATE);
    *pStatus = pConfCompute->keyRotationState[h2dIndex];
    return NV_OK;
}

/*!
 * Get key pair from globalKey
 * This function can return an invalid key pair if input
 * is invalid key. Its the callers resposibility to check this.
 *
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[in]   globalKey       : globalKey
 * @param[out]  pH2DKey         : pointer to h2d key
 * @param[out]  pD2HKey         : pointer to d2h key
 */
void confComputeGetKeyPairByKey_IMPL
(
    ConfidentialCompute *pConfCompute,
    NvU32                globalKey,
    NvU32               *pH2DKey,
    NvU32               *pD2HKey
)
{
    NvU32 h2dKey, d2hKey;

    // h2dkey is always the least (even numbered) of the keypair.
    if ((CC_GKEYID_GET_LKEYID(globalKey) % 2) == 1)
    {
       h2dKey = CC_GKEYID_DEC_LKEYID(globalKey);
       d2hKey = globalKey;
    }
    else
    {
       h2dKey = globalKey;
       d2hKey = CC_GKEYID_INC_LKEYID(globalKey);
    }

    if (pH2DKey != NULL)
        *pH2DKey = h2dKey;

    if (pD2HKey != NULL)
        *pD2HKey = d2hKey;
}

/*!
 * Account the encryption statistics of channel being freed
 *
 * The encryption statistics of freed channels are also accounted
 * in the per key aggregate statistics. This function accumalates
 * the stats for a channel being freed.
 *
 * @param[in]  pGpu            : OBJGPU pointer
 * @param[in]  pConfCompute    : conf comp pointer
 * @param[in]  pKernelChannel  : channel pointer
 */
NV_STATUS
confComputeUpdateFreedChannelStats_IMPL
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    KernelChannel *pKernelChannel
)
{
    // TODO CONFCOMP-984: Make this fatal
    if ((pKernelChannel->pEncStatsBufMemDesc == NULL) ||
        (pKernelChannel->pEncStatsBuf == NULL))
    {
        return NV_OK;
    }

    CC_CRYPTOBUNDLE_STATS *pEncStats = pKernelChannel->pEncStatsBuf;
    NvU32 h2dKey, d2hKey, h2dIndex, d2hIndex;

    NV_ASSERT_OK_OR_RETURN(confComputeGetKeyPairByChannel_HAL(pGpu, pConfCompute, pKernelChannel, &h2dKey, &d2hKey));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));

    pConfCompute->freedChannelAggregateStats[h2dIndex].totalBytesEncrypted += pEncStats->bytesEncryptedH2D;
    pConfCompute->freedChannelAggregateStats[h2dIndex].totalEncryptOps     += pEncStats->numEncryptionsH2D;
    pConfCompute->freedChannelAggregateStats[d2hIndex].totalBytesEncrypted += pEncStats->bytesEncryptedD2H;
    pConfCompute->freedChannelAggregateStats[d2hIndex].totalEncryptOps     += pEncStats->numEncryptionsD2H;
    return NV_OK;
}

NV_STATUS
confComputeSetKeyRotationThreshold_IMPL(ConfidentialCompute *pConfCompute,
                                        NvU64                attackerAdvantage)
{
    //
    // Limit beyond which an encryption key cannot be used.
    // The index is the attacker advantage as described in
    // https://datatracker.ietf.org/doc/draft-irtf-cfrg-aead-limits/
    // The limit is expressed in units of total amount of data encrypted
    // (in units of 16 B) plus the number of encryption invocations.
    //
    const NvU32 offset = 50;

    static const NvU64 keyRotationUpperThreshold[] = {
        777472127993ull,
        549755813887ull,
        388736063996ull,
        274877906943ull,
        194368031997ull,
        137438953471ull,
        97184015998ull,
        68719476735ull,
        48592007999ull,
        34359738367ull,
        24296003999ull,
        17179869183ull,
        12148001999ull,
        8589934591ull,
        6074000999ull,
        4294967295ull,
        3037000499ull,
        2147483647ull,
        1518500249ull,
        1073741823ull,
        759250124ull,
        536870911ull,
        379625061ull,
        268435455ull,
        189812530ull,
        134217727ull};

    NV_ASSERT_OR_RETURN((attackerAdvantage >= offset) &&
        (attackerAdvantage <= (offset + NV_ARRAY_ELEMENTS(keyRotationUpperThreshold) - 1)),
        NV_ERR_INVALID_ARGUMENT);

    pConfCompute->keyRotationUpperLimit = keyRotationUpperThreshold[attackerAdvantage - offset];
    pConfCompute->keyRotationLowerLimit = pConfCompute->keyRotationUpperLimit -
                                          pConfCompute->keyRotationLimitDelta;

    NV_PRINTF(LEVEL_INFO, "Setting key rotation attacker advantage to %llu.\n", attackerAdvantage);
    NV_PRINTF(LEVEL_INFO, "Key rotation lower limit is %llu and upper limit is %llu.\n",
              pConfCompute->keyRotationLowerLimit, pConfCompute->keyRotationUpperLimit);

    return NV_OK;
}

NvBool confComputeIsUpperThresholdCrossed_IMPL(ConfidentialCompute           *pConfCompute,
                                               const KEY_ROTATION_STATS_INFO *pStatsInfo)
{
    const NvU64 totalEncryptWork = (pStatsInfo->totalBytesEncrypted / 16) + pStatsInfo->totalEncryptOps;

    return (totalEncryptWork > pConfCompute->keyRotationUpperLimit);
}

NvBool confComputeIsLowerThresholdCrossed_IMPL(ConfidentialCompute           *pConfCompute,
                                               const KEY_ROTATION_STATS_INFO *pStatsInfo)
{
    const NvU64 totalEncryptWork = (pStatsInfo->totalBytesEncrypted / 16) + pStatsInfo->totalEncryptOps;

    return (totalEncryptWork > pConfCompute->keyRotationLowerLimit);
}