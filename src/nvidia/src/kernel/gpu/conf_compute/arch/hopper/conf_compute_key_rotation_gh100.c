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
#include "kernel/gpu/conf_compute/ccsl.h"

static void initKeyRotationRegistryOverrides(OBJGPU *pGpu, ConfidentialCompute *pConfCompute);
static void initInternalKeyRotationRegistryOverrides(OBJGPU *pGpu, ConfidentialCompute *pConfCompute);
static void getKeyPairForKeySpace(NvU32 keySpace, NvBool bKernel, NvU32 *pGlobalH2DKey, NvU32 *pGlobalD2HKey);
static NV_STATUS triggerKeyRotationByKeyPair(OBJGPU *pGpu, ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
static NV_STATUS calculateEncryptionStatsByKeyPair(OBJGPU *pGpu, ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
static NvBool isLowerThresholdCrossed(ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
static NvBool isUpperThresholdCrossed(ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
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
        initKeyRotationRegistryOverrides(pGpu, pConfCompute);
    }

    return NV_OK;
}

/*!
 * Conditionally enables key rotation support for keys internal to RM
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 */
NV_STATUS
confComputeEnableInternalKeyRotationSupport_GH100
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    pConfCompute->keyRotationInternalThreshold = KEY_ROTATION_DEFAULT_INTERNAL_THRESHOLD;

    // Check if we can even support internal key rotation
    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED) &&
        pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
    {
        // TODO CONFCOMP-1740: Default to enabling key rotation when supported
        // pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED, NV_TRUE);
        initInternalKeyRotationRegistryOverrides(pGpu, pConfCompute);
    }

    // If key rotation is disabled, ensure we set values accordingly
    if (!pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED))
    {
        pConfCompute->keyRotationInternalThreshold = 0;
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
 * Returns H2D and D2H keys for given priv level.
 * For kernel keys, this will only return UVM keys and not the GSP or scrubber keys.
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 * @param[in]     keySpace                 : keyspace
 * @param[in]     bKernel                  : is it kernel priv level
 * @param[out]    pGlobalH2DKey            : h2d key pointer
 * @param[out]    pGlobalD2HKey            : d2h key pointer
 */
void
confComputeGetKeyPairForKeySpace_GH100
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 keySpace,
    NvBool bKernel,
    NvU32 *pGlobalH2DKey,
    NvU32 *pGlobalD2HKey
)
{
    NvU32 h2dKey, d2hKey;
    getKeyPairForKeySpace(keySpace, bKernel, &h2dKey, &d2hKey);
    if (pGlobalH2DKey != NULL)
        *pGlobalH2DKey = h2dKey;

    if (pGlobalD2HKey != NULL)
        *pGlobalD2HKey = d2hKey;
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

    for (keySpace = 0; keySpace < CC_KEYSPACE_SIZE; keySpace++)
    {
        if (keySpace == CC_KEYSPACE_GSP)
            continue;

        if (!(pConfCompute->keyRotationEnableMask & NVBIT(keySpace)))
        {
            NV_PRINTF(LEVEL_INFO, "Skipping keyspace = %d since mask = 0x%x\n", keySpace, pConfCompute->keyRotationEnableMask);
            continue;
        }

        // calculate kernel channels stats for keyspace. (this doesn't include scrubber keys)
        if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_KEY_ROTATION, _KERNEL_KEYS, _YES, pConfCompute->keyRotationEnableMask))
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
        if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_KEY_ROTATION, _USER_KEYS, _YES, pConfCompute->keyRotationEnableMask))
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
    NvU32 h2dIndex, d2hIndex;

    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));

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

    if (isUpperThresholdCrossed(pConfCompute, h2dKey, d2hKey))
    {
        NV_PRINTF(LEVEL_ERROR, "Crossed UPPER threshold for key = 0x%x\n", h2dKey);
        NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_FAILED_THRESHOLD));
        NV_ASSERT_OK_OR_RETURN(confComputePerformKeyRotation(pGpu, pConfCompute, h2dKey, d2hKey, NV_TRUE));
    }
    else if (isLowerThresholdCrossed(pConfCompute, h2dKey, d2hKey))
    {
        NV_PRINTF(LEVEL_INFO, "Crossed LOWER threshold for key = 0x%x\n", h2dKey);
        if (state == KEY_ROTATION_STATUS_IDLE)
        {
            NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_PENDING));

            // Schedule first callback. Timeouts are not enforced on kernel channels (UVM).
            if (!confComputeGlobalKeyIsUvmKey_HAL(pConfCompute, h2dKey))
            {
                //
                // Start the timeout timer once lower threshold is crossed.
                //
                // If timer is not already created then create it now. Else, just schedule a callback.
                // make sure callback is canceled if we schedule the KR task (after crossing lower or upper threshold)
                // make sure all these timer events are deleted as part of RM shutdown
                //
                OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
                if (pConfCompute->keyRotationTimeoutInfo[h2dIndex].pTimer == NULL)
                {
                    NvU32 *pH2DKey = portMemAllocNonPaged(sizeof(NvU32));
                    *pH2DKey = h2dKey;
                    NV_ASSERT_OK_OR_RETURN(tmrEventCreate(pTmr, &pConfCompute->keyRotationTimeoutInfo[h2dIndex].pTimer,
                                                          keyRotationTimeoutCallback, (void*)pH2DKey, TMR_FLAGS_NONE));
                }
                if (confComputeIsUvmKeyRotationPending(pGpu, pConfCompute))
                {
                    // If there is any UVM KR pending do not schedule the timer for timeout
                    NV_ASSERT_OK_OR_RETURN(confComputeStopKeyRotationTimer(pGpu, pConfCompute, h2dKey));
                }
                else
                {
                    // no kernel KR pending, start/schedule timer for timeout
                    NV_ASSERT_OK_OR_RETURN(tmrEventScheduleRelSec(pTmr,
                                                                  pConfCompute->keyRotationTimeoutInfo[h2dIndex].pTimer,
                                                                  pConfCompute->keyRotationTimeout));
                }
            }
            else
            {
                // if this is a kernel KR, check if there are any pending user KRs
                NvU32 userH2DKey;
                KEY_ROTATION_STATUS userKRStatus;
                NvU32 keySpace;
                for (keySpace = 0; keySpace < CC_KEYSPACE_SIZE; keySpace++)
                {
                    if (keySpace == CC_KEYSPACE_GSP)
                        continue;

                    confComputeGetKeyPairForKeySpace_HAL(pGpu, pConfCompute, keySpace, NV_FALSE, &userH2DKey, NULL);
                    NV_ASSERT_OK_OR_RETURN(confComputeGetKeyRotationStatus(pConfCompute, userH2DKey, &userKRStatus));
                    if (userKRStatus == KEY_ROTATION_STATUS_PENDING)
                    {
                        NV_PRINTF(LEVEL_INFO, "User key rotation pending on key 0x%x\n", userH2DKey);
                        NV_ASSERT_OK_OR_RETURN(confComputeStopKeyRotationTimer(pGpu, pConfCompute, userH2DKey));
                    }
                }
            }

            //
            // Notify clients of pending KR
            // We can't schedule a workitem for notifying since it may get scheduled too late and
            // we might have already crossed the upper threshold by then.
            //
            {
                CHANNEL_ITERATOR iter = {0};
                KernelChannel *pKernelChannel = NULL;

                // notify all channels
                NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter));
                while(confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) == NV_OK)
                {
                    NV_ASSERT_OK_OR_RETURN(kchannelUpdateNotifierMem(pKernelChannel, NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS,
                                                                     pConfCompute->keyRotationCount[h2dIndex], 0, (NvU32)KEY_ROTATION_STATUS_PENDING));
                    NV_PRINTF(LEVEL_INFO, "chid 0x%x has pending key rotation, writing notifier with val 0x%x\n",
                    kchannelGetDebugTag(pKernelChannel), (NvU32)KEY_ROTATION_STATUS_PENDING);

                    // send events to clients if registered
                    kchannelNotifyEvent(pKernelChannel, NVC86F_NOTIFIERS_KEY_ROTATION, pConfCompute->keyRotationCount[h2dIndex],
                                        KEY_ROTATION_STATUS_PENDING, NULL, 0);
                }
            }
        }

        //
        // There can be a scenario where last channel on key gets freed without lower threshold being
        // crossed but when encrypt stats are computed in the next callback lower threshold is crossed here.
        // In this case, schedule key rotation.
        //
        {
            CHANNEL_ITERATOR iter = {0};
            KernelChannel *pKernelChannel = NULL;
            NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iter));
            if (confComputeGetNextChannelForKey(pGpu, pConfCompute, &iter, h2dKey, &pKernelChannel) != NV_OK)
            {
                NV_ASSERT_OK_OR_RETURN(confComputePerformKeyRotation(pGpu, pConfCompute, h2dKey, d2hKey, NV_TRUE));
            }
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
    NV_ASSERT_OR_RETURN(pEvent->pUserData != NULL, NV_ERR_INVALID_STATE);
    NvU32 key = *(NvU32*)pEvent->pUserData;
    confComputeGetKeyPairByKey(pConfCompute, key, &h2dKey, &d2hKey);
    NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationStatus(pConfCompute, h2dKey, KEY_ROTATION_STATUS_FAILED_TIMEOUT));
    NV_PRINTF(LEVEL_INFO, "Hit timeout on key 0x%x, triggering KR\n", h2dKey);
    return confComputePerformKeyRotation(pGpu, pConfCompute, h2dKey, d2hKey, NV_TRUE);
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
        NV_PRINTF(LEVEL_INFO, "Total d2h bytes encrypted  = 0x%llx\n", pConfCompute->aggregateStats[d2hIndex].totalBytesEncrypted);
        NV_PRINTF(LEVEL_INFO, "Total h2d encrypt ops  = 0x%llx\n", pConfCompute->aggregateStats[h2dIndex].totalEncryptOps);
        NV_PRINTF(LEVEL_INFO, "Total d2h encrypt ops  = 0x%llx\n", pConfCompute->aggregateStats[d2hIndex].totalEncryptOps);
    }
    return NV_OK;
}

static NvBool
isUpperThresholdCrossed
(
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey,
    NvU32 d2hKey
)
{
    NvU32 h2dIndex, d2hIndex;

    NV_ASSERT_OK(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));

    return (confComputeIsUpperThresholdCrossed(pConfCompute, &pConfCompute->aggregateStats[h2dIndex]) ||
            confComputeIsUpperThresholdCrossed(pConfCompute, &pConfCompute->aggregateStats[d2hIndex]));
}

static NvBool
isLowerThresholdCrossed
(
    ConfidentialCompute *pConfCompute,
    NvU32 h2dKey,
    NvU32 d2hKey
)
{
    NvU32 h2dIndex, d2hIndex;
    NV_ASSERT_OK(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, h2dKey, &h2dIndex));
    NV_ASSERT_OK(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, d2hKey, &d2hIndex));

    return (confComputeIsLowerThresholdCrossed(pConfCompute, &pConfCompute->aggregateStats[h2dIndex]) ||
            confComputeIsLowerThresholdCrossed(pConfCompute, &pConfCompute->aggregateStats[d2hIndex]));
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

static void
initKeyRotationRegistryOverrides
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED))
    {
        NvU32 data;

        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_THRESHOLD_DELTA, &data) == NV_OK)
        {
            if (data == 0)
            {
                NV_PRINTF(LEVEL_ERROR, "Illegal value for RmKeyRotationThresholdDelta.\n");
                NV_PRINTF(LEVEL_ERROR, "Cancelling override of threshold delta.\n");
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "Setting key rotation threshold delta to %u.\n", data);
                pConfCompute->keyRotationThresholdDelta = data;
                NV_ASSERT_OK(confComputeSetKeyRotationThreshold(pConfCompute, pConfCompute->attackerAdvantage));
            }
        }
        else if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LOWER_THRESHOLD, &data) == NV_OK)
        {
            const NvU32 lowerThreshold = data;

            if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_UPPER_THRESHOLD, &data) == NV_OK)
            {
                const NvU32 upperThreshold = data;

                if (upperThreshold > lowerThreshold)
                {
                    NV_PRINTF(LEVEL_INFO, "Setting key rotation lower threshold to %u and upper threshold to %u.\n",
                              lowerThreshold, upperThreshold);
                    pConfCompute->keyRotationUpperThreshold = upperThreshold;
                    pConfCompute->keyRotationLowerThreshold = lowerThreshold;
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR, "RmKeyRotationUpperThreshold must be greater than RmKeyRotationLowerThreshold.\n");
                    NV_PRINTF(LEVEL_ERROR, "Cancelling override of upper and lower key rotation thresholds.\n");
                }
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "RmKeyRotationUpperThreshold must be set if RmKeyRotationLowerThreshold is set.\n");
                NV_PRINTF(LEVEL_ERROR, "Cancelling override of upper and lower key rotation thresholds.\n");
            }
        }

        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_TIMEOUT_IN_SEC, &data) == NV_OK)
        {
            const NvU32 minTimeout = 2;

            if (data >= minTimeout)
            {
                NV_PRINTF(LEVEL_INFO, "Setting key rotation user-mode timeout to %u seconds.\n", data);
                pConfCompute->keyRotationTimeout = data;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Key rotation user-mode timeout must be greater than or equal to %u.\n", minTimeout);
                NV_PRINTF(LEVEL_ERROR, "Cancelling override of user-mode timeout.\n");
            }
        }

        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION, &data) == NV_OK)
        {
            if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_KEY_ROTATION, _ENABLED, _YES, data))
            {
                NV_PRINTF(LEVEL_INFO, "Confidential Compute key rotation enabled via regkey override.\n");
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED, NV_TRUE);
                pConfCompute->keyRotationEnableMask = data;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "Confidential Compute key rotation disabled via regkey override.\n");
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED, NV_FALSE);
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED, NV_FALSE);
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "Confidential Compute key rotation is disabled.\n");
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED, NV_FALSE);
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED, NV_FALSE);
        }
    }
}

static void
initInternalKeyRotationRegistryOverrides
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    NvU32 data = 0;

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED))
    {
        NV_PRINTF(LEVEL_INFO, "RM internal key rotation not supported for protected PCIe!\n");
        pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED, NV_FALSE);
    }
    else if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION, &data) == NV_OK)
    {
        if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_KEY_ROTATION, _INTERNAL_KEYS, _YES, data))
        {
            NV_PRINTF(LEVEL_ERROR, "Enabling RM internal keys for Key Rotation by regkey override!\n");
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED, NV_TRUE);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Disabling RM internal keys for Key Rotation by regkey override!\n");
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED, NV_FALSE);
        }
    }

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED) &&
        (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_INTERNAL_THRESHOLD, &data) == NV_OK))
    {
        const NvU32 internalThreshold = data;
        if (internalThreshold < KEY_ROTATION_MINIMUM_INTERNAL_THRESHOLD)
        {
            NV_PRINTF(LEVEL_ERROR, "RmKeyRotationInternalThreshold must be higher than minimum of %u!\n",
                        KEY_ROTATION_MINIMUM_INTERNAL_THRESHOLD);
        }
        // TODO CONFCOMP-1740: For production, we must disable the ability to set threshold lower than minimum.
        // else
        {
            NV_PRINTF(LEVEL_INFO, "Setting internal key rotation threshold to %u.\n",
                        internalThreshold);
            pConfCompute->keyRotationInternalThreshold = internalThreshold;
        }
    }
}
