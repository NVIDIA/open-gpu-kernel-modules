/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/conf_compute/conf_compute.h"
#include "class/cl2080.h"

#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "class/clc86fsw.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "nvrm_registry.h"

static void initKeyRotationRegistryOverrides(OBJGPU *pGpu, ConfidentialCompute *pConfCompute);
static void getKeyPairForKeySpace(NvU32 keySpace, NvBool bKernel, NvU32 *pGlobalH2DKey, NvU32 *pGlobalD2HKey);
static NV_STATUS triggerKeyRotationByKeyPair(OBJGPU *pGpu, ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
static NV_STATUS calculateEncryptionStatsByKeyPair(OBJGPU *pGpu, ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
static NV_STATUS notifyKeyRotationByKeyPair(OBJGPU *pGpu, ConfidentialCompute *pConfCompute, NvU32 h2dKey);
static NvBool isLowerThresholdCrossed(ConfidentialCompute *pConfCompute, KEY_ROTATION_STATS_INFO *pH2DInfo,
                                      KEY_ROTATION_STATS_INFO *pD2HInfo);
static NvBool isUpperThresholdCrossed(ConfidentialCompute *pConfCompute, KEY_ROTATION_STATS_INFO *pH2DInfo,
                                      KEY_ROTATION_STATS_INFO *pD2HInfo);
static NV_STATUS keyRotationTimeoutCallback(OBJGPU *pGpu, OBJTMR *pTmr, TMR_EVENT *pTmrEvent);

/*!
 * Conditionally enables key rotation support
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 */
NV_STATUS
confComputeEnableKeyRotationSupport_GH100
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute
)
{

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED) &&
        pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
    {
        pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED, NV_TRUE);

        //
        // TODO: sbellock default values need to be defined and set separately
        // for prod flow based on attacker advantage table.
        //
        pConfCompute->lowerThreshold.totalBytesEncrypted = NV_U64_MAX;
        pConfCompute->lowerThreshold.totalEncryptOps = 500;
        pConfCompute->upperThreshold.totalBytesEncrypted = NV_U64_MAX;
        pConfCompute->upperThreshold.totalEncryptOps = 1000;
        initKeyRotationRegistryOverrides(pGpu, pConfCompute);
    }
    return NV_OK;
}

/*!
 * Enables/disables key rotation by setting up the 1 sec callback for key rotation
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 * @param[in]     bEnable                  : If key rotation should be enabled
 */
NV_STATUS
confComputeEnableKeyRotationCallback_GH100
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvBool bEnable
)
{
    if (bEnable)
    {
        // Hook into the 1 Hz OS timer
        osSchedule1HzCallback(pGpu,
                              confComputeKeyRotationCallback,
                              NULL /* pData */,
                              NV_OS_1HZ_REPEAT);
    }
    else
    {
        osRemove1HzCallback(pGpu,
                            confComputeKeyRotationCallback,
                            NULL /* pData */);
    }
    return NV_OK;
}

 /*!
  * Calculates encryption statistics and triggers key rotation if thresholds are crossed.
  *
  * @param[in]     pGpu                     : OBJGPU Pointer
  * @param[in]     pConfCompute             : ConfidentialCompute pointer
  */
NV_STATUS
confComputeTriggerKeyRotation_GH100
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    NV_STATUS tempStatus, status = NV_OK;
    NvU32 globalD2HKey, globalH2DKey, keySpace;

    if ((pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED)) &&
        (pConfCompute->keyRotationChannelRefCount > 0))
    {
        pConfCompute->keyRotationCallbackCount++;
        NV_PRINTF(LEVEL_ERROR, "DUMMY KR: COUNT = %d\n", pConfCompute->keyRotationCallbackCount);
    }
    for (keySpace = 0; keySpace < CC_KEYSPACE_SIZE; keySpace++)
    {
        if (keySpace == CC_KEYSPACE_GSP)
            continue;

        if ((pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED)) &&
            !(pConfCompute->keyRotationEnableMask & NVBIT(keySpace)))
        {
            NV_PRINTF(LEVEL_INFO, "Skipping keyspace = %d since mask = 0x%x\n", keySpace, pConfCompute->keyRotationEnableMask);
            continue;
        }

        // calculate kernel channels stats for keyspace
        if ((!pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED)) ||
            (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_DUMMY_KEY_ROTATION, _KERNEL_KEYS, _YES, pConfCompute->keyRotationEnableMask)))
        {
            getKeyPairForKeySpace(keySpace, NV_TRUE, &globalH2DKey, &globalD2HKey);
            tempStatus = triggerKeyRotationByKeyPair(pGpu, pConfCompute, globalH2DKey, globalD2HKey);
            if (tempStatus != NV_OK)
            {
                NV_ASSERT(tempStatus == NV_OK);
                NV_PRINTF(LEVEL_ERROR, "Failed to calculate encryption statistics for H2D key 0x%x with status 0x%x\n", globalH2DKey, tempStatus);
                status = tempStatus;
            }
        }

        // calculate user channels stats for keyspace
        if ((!pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED)) ||
            (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_DUMMY_KEY_ROTATION, _USER_KEYS, _YES, pConfCompute->keyRotationEnableMask)))
        {
            getKeyPairForKeySpace(keySpace, NV_FALSE, &globalH2DKey, &globalD2HKey);
            tempStatus = triggerKeyRotationByKeyPair(pGpu, pConfCompute, globalH2DKey, globalD2HKey);
            if (tempStatus != NV_OK)
            {
                NV_ASSERT(tempStatus == NV_OK);
                NV_PRINTF(LEVEL_ERROR, "Failed to calculate encryption statistics for H2D key 0x%x with status 0x%x\n", globalH2DKey, tempStatus);
                status = tempStatus;
            }
        }
    }
    return status;
}

static NV_STATUS
triggerKeyRotationByKeyPair
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey,
    NvU32 d2hKey
)
{
    KEY_ROTATION_STATUS state;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeyRotationStatus(pConfCompute, h2dKey, &state));
    CHANNEL_ITERATOR iter = {0};
    KernelChannel *pKernelChannel = NULL;
    NvU32 h2dIndex, d2hIndex;

    // we won't need this once we have encryption statistics since unused keys will have stats = 0
    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED))
    {
        NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter));
        if (confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) != NV_OK)
        {
            //
            // If this is the last key and we haven't done KR yet even after crossing upper threshold then
            // it means there are no channels alive and we need to manually reset our counter
            //
            if ((h2dKey == CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_H2D_USER)) &&
                (pConfCompute->keyRotationCallbackCount > pConfCompute->upperThreshold.totalEncryptOps))
            {
                pConfCompute->keyRotationCallbackCount = 1;
            }
            return NV_OK;
        }
    }

    //
    // If key rotation is alredy scheduled because we crossed upper threshold or hit timeout
    // then we dont need to update encryption statistics as they will be zeroed out soon.
    //
    if ((state == KEY_ROTATION_STATUS_FAILED_THRESHOLD) ||
        (state == KEY_ROTATION_STATUS_FAILED_TIMEOUT))
    {
        return NV_OK;
    }

    //
    // CC session doesn't exist if key rotation failed
    // TODO CONFCOMP-984: RC all channels and other cleanup (kpadwal is working on adding this call)
    //
    if (state == KEY_ROTATION_STATUS_FAILED_ROTATION)
        return NV_ERR_INVALID_STATE;

    NV_ASSERT_OK_OR_RETURN(calculateEncryptionStatsByKeyPair(pGpu, pConfCompute, h2dKey, d2hKey));

    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));

    if (isUpperThresholdCrossed(pConfCompute, &pConfCompute->aggregateStats[h2dIndex],
                                &pConfCompute->aggregateStats[d2hIndex]))
    {
        NV_PRINTF(LEVEL_ERROR, "Crossed UPPER threshold for key = 0x%x\n", h2dKey);
        NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_FAILED_THRESHOLD));
        NV_ASSERT_OK_OR_RETURN(confComputeScheduleKeyRotationWorkItem(pGpu, pConfCompute, h2dKey, d2hKey));
    }
    else if (isLowerThresholdCrossed(pConfCompute, &pConfCompute->aggregateStats[h2dIndex],
                                     &pConfCompute->aggregateStats[d2hIndex]))
    {
        NV_PRINTF(LEVEL_INFO, "Crossed LOWER threshold for key = 0x%x\n", h2dKey);
        if (state == KEY_ROTATION_STATUS_IDLE)
        {
            NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_PENDING));

            //
            // Start the timeout timer once lower threshold is crossed.
            //
            // If timer is not already created then create it now. Else, just schedule a callback.
            // make sure callback is canceled if we schedule the KR task (after crossing lower or upper threshold)
            // make sure all these timer events are deleted as part of RM shutdown
            //
            OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
            if (pConfCompute->ppKeyRotationTimer[h2dIndex] == NULL)
            {
                NvU32 *pH2DKey = portMemAllocNonPaged(sizeof(NvU32));
                *pH2DKey = h2dKey;
                NV_ASSERT_OK_OR_RETURN(tmrEventCreate(pTmr, &pConfCompute->ppKeyRotationTimer[h2dIndex],
                                                      keyRotationTimeoutCallback, (void*)pH2DKey, TMR_FLAGS_NONE));
            }

            //
            // Schedule first callback.
            // TODO sbellock: don't use hardcoded 12.5 sec vaule
            //
            NV_ASSERT_OK_OR_RETURN(tmrEventScheduleRelSec(pTmr, pConfCompute->ppKeyRotationTimer[h2dIndex], 12.5));

            //
            // Notify clients of pending KR
            // We can't schedule a workitem for this since it may get scheduled too late and
            // we might have already crossed the upper threshold by then.
            //
            NV_ASSERT_OK_OR_RETURN(notifyKeyRotationByKeyPair(pGpu, pConfCompute, h2dKey));
        }
    }
    return NV_OK;
}

static NV_STATUS
keyRotationTimeoutCallback
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    NvU32 h2dKey, d2hKey;
    NvU32 key = *(NvU32*)pEvent->pUserData;
    confComputeGetKeyPairByKey(pConfCompute, key, &h2dKey, &d2hKey);
    NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_FAILED_TIMEOUT));
    return confComputeScheduleKeyRotationWorkItem(pGpu, pConfCompute, h2dKey, d2hKey);
}

static NV_STATUS
calculateEncryptionStatsByKeyPair
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey,
    NvU32 d2hKey
)
{
    CHANNEL_ITERATOR iter = {0};
    NvU64 totalH2Dbytes = 0;
    NvU64 totalD2Hbytes = 0;
    NvU64 totalEncryptOpsH2D = 0;
    NvU64 totalEncryptOpsD2H = 0;
    NvU32 h2dIndex, d2hIndex;

    // Iterate through all channels using the key pair and compute totals
    KernelChannel *pKernelChannel = NULL;

    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));

    NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter));
    while(confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) == NV_OK)
    {
        // TODO: Make this fatal
        if (pKernelChannel->pEncStatsBufMemDesc == NULL)
            continue;

        CC_CRYPTOBUNDLE_STATS *pEncStats = pKernelChannel->pEncStatsBuf;
        if (pEncStats == NULL)
        {
            NV_ASSERT(pEncStats != NULL);
            NV_PRINTF(LEVEL_ERROR, "Failed to get stats for chid = 0x%x RM engineId = 0x%x\n",
                kchannelGetDebugTag(pKernelChannel), kchannelGetEngineType(pKernelChannel));
            return NV_ERR_INVALID_STATE;
        }
        totalH2Dbytes += pEncStats->bytesEncryptedH2D;
        totalD2Hbytes += pEncStats->bytesEncryptedD2H;
        totalEncryptOpsH2D += pEncStats->numEncryptionsH2D;
        totalEncryptOpsD2H += pEncStats->numEncryptionsD2H;
        NV_PRINTF(LEVEL_INFO, "Encryption stats for chid 0x%x with h2dKey 0x%x\n", kchannelGetDebugTag(pKernelChannel), h2dKey);
        NV_PRINTF(LEVEL_INFO, "Total h2d bytes encrypted  = 0x%llx\n", pEncStats->bytesEncryptedH2D);
        NV_PRINTF(LEVEL_INFO, "Total d2h bytes encrypted  = 0x%llx\n", pEncStats->bytesEncryptedD2H);
        NV_PRINTF(LEVEL_INFO, "Total h2d encrypt ops  = 0x%llx\n", pEncStats->numEncryptionsH2D);
        NV_PRINTF(LEVEL_INFO, "Total d2h encrypt ops  = 0x%llx\n", pEncStats->numEncryptionsD2H);
    }

    // Add stats for freed channels
    totalH2Dbytes      += pConfCompute->freedChannelAggregateStats[h2dIndex].totalBytesEncrypted;
    totalEncryptOpsH2D += pConfCompute->freedChannelAggregateStats[h2dIndex].totalEncryptOps;
    totalD2Hbytes      += pConfCompute->freedChannelAggregateStats[d2hIndex].totalBytesEncrypted;
    totalEncryptOpsD2H += pConfCompute->freedChannelAggregateStats[d2hIndex].totalEncryptOps;

    pConfCompute->aggregateStats[h2dIndex].totalBytesEncrypted = totalH2Dbytes;
    pConfCompute->aggregateStats[h2dIndex].totalEncryptOps     = totalEncryptOpsH2D;
    pConfCompute->aggregateStats[d2hIndex].totalBytesEncrypted = totalD2Hbytes;
    pConfCompute->aggregateStats[d2hIndex].totalEncryptOps     = totalEncryptOpsD2H;

    if ((pConfCompute->aggregateStats[h2dIndex].totalBytesEncrypted > 0) ||
        (pConfCompute->aggregateStats[d2hIndex].totalBytesEncrypted > 0))
    {
        NV_PRINTF(LEVEL_INFO, "Aggregate stats for h2dKey 0x%x and d2hKey 0x%x\n", h2dKey, d2hKey);
        NV_PRINTF(LEVEL_INFO, "Total h2d bytes encrypted  = 0x%llx\n", pConfCompute->aggregateStats[h2dIndex].totalBytesEncrypted);
        NV_PRINTF(LEVEL_INFO, "Total d2h bytes encrypted  = 0x%llx\n", pConfCompute->aggregateStats[h2dIndex].totalEncryptOps);
        NV_PRINTF(LEVEL_INFO, "Total h2d encrypt ops  = 0x%llx\n", pConfCompute->aggregateStats[d2hIndex].totalBytesEncrypted);
        NV_PRINTF(LEVEL_INFO, "Total d2h encrypt ops  = 0x%llx\n", pConfCompute->aggregateStats[d2hIndex].totalEncryptOps);
    }
    return NV_OK;
}

static NvBool
isUpperThresholdCrossed
(
    ConfidentialCompute *pConfCompute,
    KEY_ROTATION_STATS_INFO *pH2DInfo,
    KEY_ROTATION_STATS_INFO *pD2HInfo
)
{
    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED))
    {
        return (pConfCompute->keyRotationCallbackCount > pConfCompute->upperThreshold.totalEncryptOps);
    }
    else
    {
        return (confComputeIsUpperThresholdCrossed(pConfCompute, pH2DInfo) ||
                confComputeIsUpperThresholdCrossed(pConfCompute, pD2HInfo));
    }
}

static NvBool
isLowerThresholdCrossed
(
    ConfidentialCompute *pConfCompute,
    KEY_ROTATION_STATS_INFO *pH2DInfo,
    KEY_ROTATION_STATS_INFO *pD2HInfo
)
{
    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED))
    {
        return (pConfCompute->keyRotationCallbackCount > pConfCompute->lowerThreshold.totalEncryptOps);
    }
    else
    {
        return (confComputeIsLowerThresholdCrossed(pConfCompute, pH2DInfo) ||
                confComputeIsLowerThresholdCrossed(pConfCompute, pD2HInfo));
    }
}

static void
getKeyPairForKeySpace(NvU32 keySpace, NvBool bKernel, NvU32 *pGlobalH2DKey, NvU32 *pGlobalD2HKey)
{
    NvU32 localH2DKey, localD2HKey;
    if (keySpace == CC_KEYSPACE_SEC2)
    {
        if (bKernel)
        {
            localH2DKey = CC_LKEYID_CPU_SEC2_DATA_KERN;
            localD2HKey = CC_LKEYID_CPU_SEC2_HMAC_KERN;
        }
        else
        {
            localH2DKey = CC_LKEYID_CPU_SEC2_DATA_USER;
            localD2HKey = CC_LKEYID_CPU_SEC2_HMAC_USER;
        }
    }
    else
    {
        if (bKernel)
        {
            localH2DKey = CC_LKEYID_LCE_H2D_KERN;
            localD2HKey = CC_LKEYID_LCE_D2H_KERN;
        }
        else
        {
            localH2DKey = CC_LKEYID_LCE_H2D_USER;
            localD2HKey = CC_LKEYID_LCE_D2H_USER;
        }
    }
    *pGlobalH2DKey = CC_GKEYID_GEN(keySpace, localH2DKey);
    *pGlobalD2HKey = CC_GKEYID_GEN(keySpace, localD2HKey);
}

static NV_STATUS
notifyKeyRotationByKeyPair
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey
)
{
    KEY_ROTATION_STATUS status;
    CHANNEL_ITERATOR iter = {0};
    KernelChannel *pKernelChannel = NULL;
    NvU32 notifyStatus = 0;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeyRotationStatus(pConfCompute, h2dKey, &status));
    //
    // We expect this work item to be called soon after RM detects lower threshold is
    // crossed and schedules this.
    //
    NV_ASSERT_OR_RETURN(status == KEY_ROTATION_STATUS_PENDING, NV_ERR_INVALID_STATE);

	// notify all channels
	NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter));
	while(confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) == NV_OK)
	{
		// update notifier memory
		notifyStatus =
			FLD_SET_DRF(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _IN_PROGRESS, _TRUE, notifyStatus);

		notifyStatus =
			FLD_SET_DRF_NUM(_CHANNELGPFIFO, _NOTIFICATION_STATUS, _VALUE, status, notifyStatus);

		NV_ASSERT_OK_OR_RETURN(kchannelUpdateNotifierMem(pKernelChannel, NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS,
											             0, 0, notifyStatus));
		NV_PRINTF(LEVEL_INFO, "chid 0x%x has pending key rotation, writing notifier with val 0x%x\n", kchannelGetDebugTag(pKernelChannel), (NvU32)notifyStatus);

		// send events to clients if registered
		kchannelNotifyEvent(pKernelChannel, NVC86F_NOTIFIERS_KEY_ROTATION, 0, status, NULL, 0);
	}
    return NV_OK;
}

static void
initKeyRotationRegistryOverrides
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    //
    // Temp CONFCOMP-984: This will be removed once all RM clients support
    // key rotation by default.
    //
    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED))
    {
        NvU32 data;
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_DUMMY_KEY_ROTATION, &data) == NV_OK)
        {
            if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_DUMMY_KEY_ROTATION, _ENABLED, _YES, data))
            {
                NV_PRINTF(LEVEL_INFO, "Confidential Compute dummy key rotation enabled via regkey override.\n");
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED, NV_TRUE);
            }
            else if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_DUMMY_KEY_ROTATION, _ENABLED, _NO, data))
            {
                NV_PRINTF(LEVEL_INFO, "Confidential Compute dummy key rotation disabled via regkey override.\n");
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED, NV_FALSE);
            }
            if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DUMMY_KEY_ROTATION_ENABLED))
            {
                pConfCompute->keyRotationEnableMask = data;

                //
                // Set lower and upper thresholds to default values
                // this will go away once we stop supporting dummy KR
                //
                pConfCompute->lowerThreshold.totalBytesEncrypted = NV_U64_MAX;
                pConfCompute->lowerThreshold.totalEncryptOps = KEY_ROTATION_LOWER_THRESHOLD;
                pConfCompute->upperThreshold.totalBytesEncrypted = NV_U64_MAX;
                pConfCompute->upperThreshold.totalEncryptOps = KEY_ROTATION_UPPER_THRESHOLD;

                if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_DUMMY_KEY_ROTATION_LOWER_THRESHOLD, &data) == NV_OK)
                {
                    pConfCompute->lowerThreshold.totalEncryptOps = data;
                }

                if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_DUMMY_KEY_ROTATION_UPPER_THRESHOLD, &data) == NV_OK)
                {
                    pConfCompute->upperThreshold.totalEncryptOps = data;
                }
            }
        }
    }
}