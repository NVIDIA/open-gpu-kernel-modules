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

/***************************************************************************\
 *                                                                          *
 *      Confidential Compute Object Module                                  *
 *                                                                          *
 \**************************************************************************/

#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

#include "nvrm_registry.h"
#include "gpu/conf_compute/conf_compute.h"
#include "gpu/conf_compute/conf_compute_keystore.h"
#include "spdm/rmspdmvendordef.h"
#include "gsp/gsp_proxy_reg.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "kernel/gpu/conf_compute/ccsl.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "gpu/conf_compute/conf_compute_api.h"
#include "class/clcb33.h"
#include "spdm/rmspdmvendordef.h"

/*!
 * Local object related functions
 */
static NV_STATUS _confComputeInitRegistryOverrides(OBJGPU *, ConfidentialCompute*);
static NvU32 _confComputeGetKeyspaceSize(NvU16 keyspace);

#define KEY_ROTATION_THRESHOLD_DELTA 20000000ull
#define KEY_ROTATION_TIMEOUT         10

NV_STATUS
confComputeConstructEngine_IMPL(OBJGPU                  *pGpu,
                                ConfidentialCompute     *pConfCompute,
                                ENGDESCRIPTOR           engDesc)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV_STATUS status = NV_OK;
    NvU32 data = 0;
    NvBool bForceEnableCC = 0;

    pConfCompute->pSpdm              = NULL;
    pConfCompute->pGspHeartbeatTimer = NULL;
    pConfCompute->heartbeatPeriodSec = 0;

    portMemSet(&pConfCompute->ccStaticInfo, 0, sizeof(pConfCompute->ccStaticInfo));
    pConfCompute->gspProxyRegkeys = 0;

    pConfCompute->pRpcCcslCtx                = NULL;
    pConfCompute->pDmaCcslCtx                = NULL;
    pConfCompute->pReplayableFaultCcslCtx    = NULL;
    pConfCompute->pNonReplayableFaultCcslCtx = NULL;
    pConfCompute->pGspSec2RpcCcslCtx         = NULL;

    if (gpuIsCCEnabledInHw_HAL(pGpu))
    {
        pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED, NV_TRUE);
    }

    if (gpuIsDevModeEnabledInHw_HAL(pGpu))
    {
        pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED, NV_TRUE);
    }

    if (gpuIsProtectedPcieEnabledInHw_HAL(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "Enabling protected PCIe in secure PRI\n");
        // Internally, RM must use CC code paths for protected pcie as well
        pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED, NV_TRUE);
        pConfCompute->setProperty(pConfCompute,
            PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED, NV_TRUE);
    }

    status = _confComputeInitRegistryOverrides(pGpu, pConfCompute);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unexpected failure in confComputeConstructEngine! Status:0x%x\n", status);
        return status;
    }

    if ((sysGetStaticConfig(pSys)->bOsCCEnabled) && !gpuIsCCEnabledInHw_HAL(pGpu) &&
        !gpuIsProtectedPcieEnabledInHw_HAL(pGpu))
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_CC_FEATURE_CAPABLE))
        {
            NV_PRINTF(LEVEL_ERROR, "GPU confidential compute capability is not enabled.\n");
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "GPU does not support confidential compute.\n");
        }

        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_REQUEST);
    }

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED))
    {
        bForceEnableCC = (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONFIDENTIAL_COMPUTE, &data) == NV_OK) &&
         FLD_TEST_DRF(_REG_STR, _RM_CONFIDENTIAL_COMPUTE, _ENABLED, _YES, data);

        if (!RMCFG_FEATURE_PLATFORM_GSP && !RMCFG_FEATURE_PLATFORM_MODS && !bForceEnableCC)
        {
            if (!(sysGetStaticConfig(pSys)->bOsCCEnabled))
            {
                NV_PRINTF(LEVEL_ERROR, "CPU does not support confidential compute.\n");
                NV_ASSERT(0);
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED, NV_FALSE);
                return NV_ERR_INVALID_OPERATION;
            }
        }

        NV_CHECK_OR_RETURN(LEVEL_ERROR, confComputeIsGpuCcCapable_HAL(pGpu, pConfCompute), NV_ERR_INVALID_OPERATION);

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_APM_FEATURE_CAPABLE))
        {
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED, NV_TRUE);

            // Forcing DEV_MODE to False for APM
            NV_PRINTF(LEVEL_INFO, "Confidential Compute devtools mode DISABLED in APM.\n");
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED, NV_FALSE);
            pConfCompute->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _CONF_COMPUTE_DEV_MODE, _DISABLE);
        }
        else if (pGpu->getProperty(pGpu, PDB_PROP_GPU_CC_FEATURE_CAPABLE))
        {
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED, NV_TRUE);
            pGpu->setProperty(pGpu, PDB_PROP_GPU_FASTPATH_SEQ_ENABLED, NV_TRUE);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "GPU does not support confidential compute.\n");
            NV_ASSERT(0);
            return NV_ERR_INVALID_OPERATION;
        }
    }
    // init key rotation state
    pConfCompute->attackerAdvantage = SECURITY_POLICY_ATTACKER_ADVANTAGE_DEFAULT;
    pConfCompute->keyRotationThresholdDelta = KEY_ROTATION_THRESHOLD_DELTA;
    pConfCompute->keyRotationTimeout = KEY_ROTATION_TIMEOUT;
    NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationThreshold(pConfCompute,
                                                              pConfCompute->attackerAdvantage));

    for (NvU32 i = 0; i < CC_KEYSPACE_TOTAL_SIZE; i++)
    {
        pConfCompute->keyRotationState[i] = KEY_ROTATION_STATUS_IDLE;
        pConfCompute->keyRotationTimeoutInfo[i].pTimer = NULL;
        pConfCompute->keyRotationCallbackCount[i] = 0;
        pConfCompute->keyRotationCount[i] = 0;
    }
    portMemSet(pConfCompute->aggregateStats, 0, sizeof(pConfCompute->aggregateStats));
    portMemSet(pConfCompute->freedChannelAggregateStats, 0, sizeof(pConfCompute->freedChannelAggregateStats));
    pConfCompute->keyRotationEnableMask = 0;
    NV_ASSERT_OK_OR_RETURN(confComputeEnableKeyRotationSupport_HAL(pGpu, pConfCompute));
    NV_ASSERT_OK_OR_RETURN(confComputeEnableInternalKeyRotationSupport_HAL(pGpu, pConfCompute));

    return NV_OK;
}

/*!
 * Initialize all registry overrides for this object
 *
 * @param[in]  pGpu              GPU object pointer
 * @param[in]  pConfCompute      ConfidentialCompute pointer
 */
static NV_STATUS
_confComputeInitRegistryOverrides
(
    OBJGPU                *pGpu,
    ConfidentialCompute   *pConfCompute
)
{
    NvU32 data = 0;

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_CONFIDENTIAL_COMPUTE, &data) == NV_OK) &&
         FLD_TEST_DRF(_REG_STR, _RM_CONFIDENTIAL_COMPUTE, _ENABLED, _YES, data))
    {
        NV_PRINTF(LEVEL_INFO, "Confidential Compute enabled via regkey override.\n");
        pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED, NV_TRUE);
        pConfCompute->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _CONFIDENTIAL_COMPUTE, _ENABLE);

        if (FLD_TEST_DRF(_REG_STR, _RM_CONFIDENTIAL_COMPUTE, _GPUS_READY_CHECK, _DISABLED, data))
        {
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED, NV_FALSE);
        }

        if (FLD_TEST_DRF(_REG_STR, _RM_CONFIDENTIAL_COMPUTE, _DEV_MODE_ENABLED, _YES, data))
        {
            NV_PRINTF(LEVEL_INFO, "Confidential Compute dev mode enabled via regkey override.\n");
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED, NV_TRUE);
            pConfCompute->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _CONF_COMPUTE_DEV_MODE, _ENABLE);
        }
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_CC_MULTI_GPU_MODE, &data) == NV_OK) &&
        (data == NV_REG_STR_RM_CC_MULTI_GPU_MODE_PROTECTED_PCIE))
    {
        NV_PRINTF(LEVEL_INFO, "Enabling protected PCIe\n");
        // Internally, RM must use CC code paths for protected pcie as well
        pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED, NV_TRUE);
        pConfCompute->setProperty(pConfCompute,
             PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED, NV_TRUE);
        pConfCompute->gspProxyRegkeys |=
            DRF_DEF(GSP, _PROXY_REG, _CONF_COMPUTE_MULTI_GPU_MODE, _PROTECTED_PCIE);
    }

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED))
    {
        if (confComputeIsSpdmEnabled(pGpu, pConfCompute))
        {
            NV_PRINTF(LEVEL_INFO, "SPDM is enabled by default.\n");
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED, NV_TRUE);
        }

        // Allow override of whatever default settings are.
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_SPDM_POLICY, &data) == NV_OK)
        {
            if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_SPDM_POLICY, _ENABLED, _YES, data))
            {
                NV_PRINTF(LEVEL_INFO, "Confidential Compute SPDM enabled via regkey override.\n");
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED, NV_TRUE);
            }
            else if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_SPDM_POLICY, _ENABLED, _NO, data))
            {
                // OPENRM-TODO: Always enable SPDM for debug.
                NV_PRINTF(LEVEL_INFO, "Confidential Compute SPDM disabled via regkey override.\n");
                pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED, NV_FALSE);
            }
        }

        if (IS_FMODEL(pGpu))
        {
            // Skip SPDM support on fmodel due to bugs 3553627 and 3556621.
            NV_PRINTF(LEVEL_INFO, "Confidential Compute SPDM disabled on Fmodel.\n");
            pConfCompute->setProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED, NV_FALSE);
        }

        if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED))
        {
            NV_STATUS status = objCreate(&pConfCompute->pSpdm, pConfCompute, Spdm);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "SPDM child object creation failed! Status:0x%x\n", status);
                return status;
            }

            if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT, &data) == NV_OK)
            {
                pConfCompute->heartbeatPeriodSec =
                    DRF_VAL(_REG_STR, _RM_CONF_COMPUTE_HEARTBEAT_PERIOD, _SECONDS, data);
                if (FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_HEARTBEAT_PERIOD, _SECONDS, _DISABLE, data))
                {
                    NV_PRINTF(LEVEL_ERROR, "SPDM heartbeat disabled via regkey!\n");
                }
                else if (pConfCompute->heartbeatPeriodSec < NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT_PERIOD_SECONDS_MIN ||
                         pConfCompute->heartbeatPeriodSec > NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT_PERIOD_SECONDS_MAX)
                {
                    // Check for overflow or underflow.
                    NV_PRINTF(LEVEL_ERROR, "Suggested heartbeat period is invalid. Falling back to default 4 sec.\n");
                    pConfCompute->heartbeatPeriodSec = SPDM_DEFAULT_HEARTBEAT_PERIOD_IN_SEC;
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR, "Overriding default heartbeat period to 0x%x sec\n",
                            pConfCompute->heartbeatPeriodSec);
                }
            }
            else
            {
                // Heartbeat is disabled by default.
                pConfCompute->heartbeatPeriodSec = 0;
            }
        }
    }

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED))
    {
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSP_OWNED_FAULT_BUFFERS_ENABLE, &data) == NV_OK)
        {
            pGpu->bIsGspOwnedFaultBuffersEnabled = data;
        }
        else
        {
            if (IS_GSP_CLIENT(pGpu) || RMCFG_FEATURE_PLATFORM_GSP)
            {
                pGpu->bIsGspOwnedFaultBuffersEnabled = NV_REG_STR_RM_GSP_OWNED_FAULT_BUFFERS_ENABLE_YES;
            }
            else
            {
                pGpu->bIsGspOwnedFaultBuffersEnabled = NV_REG_STR_RM_GSP_OWNED_FAULT_BUFFERS_ENABLE_NO;
            }
        }
    }
    return NV_OK;
}

/*!
 * Establish the SPDM session with the GPU Responder.
 * After establishing the session, initialize all keys required
 * for the Confidential Compute session.
 *
 * Note: This assumes that Confidential Compute is the first object
 * to be initialized in the GPU child order list, and therefore
 * SPDM session establishment is the first thing to happen. If another
 * object precedes Confidential Compute - it will be initialized before
 * SPDM session establishment.
 *
 * @param[in]  pGpu              GPU object pointer
 * @param[in]  pConfCompute      ConfidentialCompute pointer
 */
NV_STATUS
confComputeEstablishSpdmSessionAndKeys_KERNEL
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    NV_STATUS status = NV_OK;

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED))
    {
        // Initialize keystore first, as it is still needed even if SPDM disabled.
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                            confComputeKeyStoreInit_HAL(pConfCompute),
                            ErrorExit);

        if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED))
        {
            //
            // Initialize SPDM session between Guest RM and SPDM Responder on GPU.
            // The session lifetime will track Confidential Compute object state lifetime.
            //
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                spdmContextInit(pGpu, pConfCompute->pSpdm),
                                ErrorExit);
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                spdmStart(pGpu, pConfCompute->pSpdm),
                                ErrorExit);          

            // Store the export master secret in the keystore.
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                spdmRetrieveExportSecret(pGpu, pConfCompute->pSpdm,
                                                        CC_EXPORT_MASTER_KEY_SIZE_BYTES,
                                                        confComputeKeyStoreGetExportMasterKey(pConfCompute)),
                                ErrorExit);

            // Derive secrets for encrypted communication
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                confComputeDeriveSecrets_HAL(pConfCompute, MC_ENGINE_IDX_GSP),
                                ErrorExit);

            // Initialize encryption contexts for encrypted traffic between Kernel-RM and GSP.
            NV_ASSERT_OK_OR_RETURN(ccslContextInitViaKeyId(pConfCompute,
                &pConfCompute->pRpcCcslCtx, CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_LOCKED_RPC)));

            NV_ASSERT_OK_OR_RETURN(ccslContextInitViaKeyId(pConfCompute,
                &pConfCompute->pDmaCcslCtx, CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_DMA)));

            NV_ASSERT_OK_OR_RETURN(ccslContextInitViaKeyId(pConfCompute,
                &pConfCompute->pReplayableFaultCcslCtx,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT)));

            NV_ASSERT_OK_OR_RETURN(ccslContextInitViaKeyId(pConfCompute,
                &pConfCompute->pNonReplayableFaultCcslCtx,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT)));
        }
    }

ErrorExit:

    return status;
}

/*!
 * Deinitialize the SPDM context with the GPU Responder.
 *
 * Note: This assumes that Confidential Compute is the first object
 * to be initialized in the GPU child order list, and therefore
 * SPDM deinitialization is the last thing to happen. If another
 * object precedes Confidential Compute - it will be deinitialized
 * before SPDM.
 *
 * @param[in]  pGpu              GPU object pointer
 * @param[in]  pConfCompute      ConfidentialCompute pointer
 */
static NV_STATUS
_confComputeDeinitSpdmSessionAndKeys
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    NV_STATUS status = NV_OK;

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_ENABLED))
    {
        // Keystore will always be initialized, regardless of whether we use SPDM
        confComputeKeyStoreDeinit_HAL(pConfCompute);

        if (pConfCompute->pSpdm == NULL)
        {
            //
            // If SPDM object doesn't exist, alert in logs and move on.
            // This means we either never established the session, or have already torn down.
            //
            NV_PRINTF(LEVEL_INFO, "SPDM teardown did not occur, as SPDM object is null!\n");
            return NV_OK;
        }

        //
        // Tear down SPDM session between Guest RM and SPDM Responder on GPU.
        // We must do in pre-unload, before Responder is torn down entirely.
        //
        if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED))
        {
            status = spdmContextDeinit(pGpu, pConfCompute->pSpdm, NV_TRUE);

            NV_PRINTF(LEVEL_INFO, "SPDM teardown successful.\n");

            // Deinitialize CCSL contexts.
            ccslContextClear(pConfCompute->pRpcCcslCtx);
            ccslContextClear(pConfCompute->pDmaCcslCtx);
            ccslContextClear(pConfCompute->pReplayableFaultCcslCtx);
            ccslContextClear(pConfCompute->pNonReplayableFaultCcslCtx);

            pConfCompute->pRpcCcslCtx                = NULL;
            pConfCompute->pDmaCcslCtx                = NULL;
            pConfCompute->pReplayableFaultCcslCtx    = NULL;
            pConfCompute->pNonReplayableFaultCcslCtx = NULL;
            pConfCompute->pGspSec2RpcCcslCtx         = NULL;
        }

        objDelete(pConfCompute->pSpdm);
        pConfCompute->pSpdm = NULL;
    }

    return status;
}

/*!
 * Perform any init that must occur after GPU state load.
 *
 * @param[in]  pGpu          GPU object pointer
 * @param[in]  pConfCompute  ConfidentialCompute pointer
 * @param[in]  flags         Optional flags describing state load conditions
 */
NV_STATUS
confComputeStatePostLoad_IMPL
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32                flags
)
{
    NV_STATUS  status = NV_OK;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                    pGpu->hInternalClient,
                                    pGpu->hInternalSubdevice,
                                    NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO,
                                    &pConfCompute->ccStaticInfo,
                                    sizeof(pConfCompute->ccStaticInfo)));

    if (!IS_GSP_CLIENT(pGpu) && !RMCFG_FEATURE_PLATFORM_GSP)
    {
        NV_PRINTF(LEVEL_INFO, "Performing late SPDM initialization!\n");
        status = confComputeEstablishSpdmSessionAndKeys_HAL(pGpu, pConfCompute);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "ConfCompute : Failed initializing SPDM!");
            return status;
        }
    }

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED))
    {
        status = spdmSendInitRmDataCommand_HAL(pGpu, pConfCompute->pSpdm);
        if (status != NV_OK)
        {
            return status;
        }

        if (IS_GSP_CLIENT(pGpu) && (pConfCompute->heartbeatPeriodSec != 0))
        {
            NV_PRINTF(LEVEL_INFO, "ConfCompute: Registering for SPDM heartbeats with period of 0x%x sec.\n",
                      pConfCompute->heartbeatPeriodSec);
            status = spdmRegisterForHeartbeats(pGpu, pConfCompute->pSpdm, pConfCompute->heartbeatPeriodSec);
            if (status != NV_OK)
            {
                return status;
            }
        }
    }

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED) &&
        pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED))
    {
        status = confComputeEnableKeyRotationCallback_HAL(pGpu, pConfCompute, NV_TRUE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "ConfCompute : Failed to enable key rotation callback!");
            return status;
        }
    }

    return status;
}

/*!
 * Perform any deinit that must occur before GPU state unload.
 *
 * @param[in]  pGpu          GPU object pointer
 * @param[in]  pConfCompute  ConfidentialCompute pointer
 * @param[in]  flags         Optional flags describing state unload conditions
 */
NV_STATUS
confComputeStatePreUnload_KERNEL
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32                flags
)
{
    NV_STATUS status = NV_OK;
    NV_STATUS tempStatus = NV_OK;
    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED) &&
        pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED))
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        for (NvU32 i = 0; i < CC_KEYSPACE_TOTAL_SIZE; i++)
        {
            if (pConfCompute->keyRotationTimeoutInfo[i].pTimer != NULL)
            {
                tmrEventCancel(pTmr, pConfCompute->keyRotationTimeoutInfo[i].pTimer);
                portMemFree(pConfCompute->keyRotationTimeoutInfo[i].pTimer->pUserData);
                tmrEventDestroy(pTmr, pConfCompute->keyRotationTimeoutInfo[i].pTimer);
                pConfCompute->keyRotationTimeoutInfo[i].pTimer = NULL;
            }
        }
        tempStatus = confComputeEnableKeyRotationCallback_HAL(pGpu, pConfCompute, NV_FALSE);
        if (tempStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to disable key rotation 0x%x\n", tempStatus);
            status = tempStatus;
        }
    }

    if (IS_GSP_CLIENT(pGpu) && (pConfCompute->heartbeatPeriodSec != 0))
    {
        tempStatus = spdmUnregisterFromHeartbeats(pGpu, pConfCompute->pSpdm);
    }
    else if (!IS_GSP_CLIENT(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "Performing SPDM deinitialization in Pre Unload!\n");
        tempStatus = _confComputeDeinitSpdmSessionAndKeys(pGpu, pConfCompute);
    }

    if (tempStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to deinit spdm 0x%x\n", tempStatus);
        status = tempStatus;
    }

    return status;
}

NvBool
confComputeAcceptClientRequest_IMPL
(
    OBJGPU                *pGpu,
    ConfidentialCompute   *pConfCompute
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    if (pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED))
    {
        return pGpuMgr->ccCaps.bAcceptClientRequest;
    }
    else
    {
        return NV_TRUE;
    }
}

NV_STATUS
confComputeStateInitLocked_IMPL
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                           pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO,
                                           &pConfCompute->ccStaticInfo,
                                           sizeof(pConfCompute->ccStaticInfo)));

    NV_PRINTF(LEVEL_INFO, "BAR1 Trusted: 0x%x PCIE Trusted: 0x%x\n",
              pConfCompute->ccStaticInfo.bIsBar1Trusted, pConfCompute->ccStaticInfo.bIsPcieTrusted);

    return NV_OK;
}

/*!
 * Init channel iterator for a given global key
 *
 * @param[in] pGpu                     : OBJGPU Pointer
 * @param[in] pConfCompute             : ConfidentialCompute pointer
 * @param[in] globalKey                : Key used by channels
 * @param[in/out] pIter                : kernelchannel iterator
 */
NV_STATUS
confComputeInitChannelIterForKey_IMPL
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    NvU32 globalKey,
    CHANNEL_ITERATOR *pIter
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 keySpace = CC_GKEYID_GET_KEYSPACE(globalKey);
    NvU32 engineId = confComputeGetEngineIdFromKeySpace_HAL(pConfCompute, keySpace);
    NV_ASSERT_OR_RETURN(engineId != RM_ENGINE_TYPE_NULL, NV_ERR_INVALID_ARGUMENT);

    NvU32 runlistId;
    NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate(pGpu, pKernelFifo, ENGINE_INFO_TYPE_RM_ENGINE_TYPE, engineId,
                                                ENGINE_INFO_TYPE_RUNLIST, &runlistId));
    kfifoGetChannelIterator(pGpu, pKernelFifo, pIter, runlistId);
    return NV_OK;
}

/*!
 * Gets next channel for a given global key
 *
 * @param[in] pGpu                     : OBJGPU Pointer
 * @param[in] pConfCompute             : ConfidentialCompute pointer
 * @param[in] pIt                      : channel iterator for a runlist
 * @param[in] globalKey                : Key used by channels
 * @param[out] ppKernelChannel         : kernelchannel
 */
NV_STATUS
confComputeGetNextChannelForKey_IMPL
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    CHANNEL_ITERATOR *pIt,
    NvU32 globalKey,
    KernelChannel **ppKernelChannel
)
{
    NV_ASSERT_OR_RETURN(ppKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);
    *ppKernelChannel = NULL;

    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvBool bKernelPriv = confComputeGlobalKeyIsKernelPriv_HAL(pConfCompute, globalKey);
    NvBool bUvmKey = confComputeGlobalKeyIsUvmKey_HAL(pConfCompute, globalKey);
    KernelChannel *pKernelChannel = NULL;
    while(kfifoGetNextKernelChannel(pGpu, pKernelFifo, pIt, &pKernelChannel) == NV_OK)
    {
        if (kchannelGetRunlistId(pKernelChannel) != pIt->runlistId)
            continue;

        if (!pKernelChannel->bCCSecureChannel)
            continue;

        if (!(bKernelPriv ^ kchannelCheckIsKernel(pKernelChannel)))
        {
            if (bKernelPriv)
            {
                if (bUvmKey && !pKernelChannel->bUseScrubKey)
                {
                    // return all kern channels that don't use scrub key
                   *ppKernelChannel = pKernelChannel;
                }
                else if (!bUvmKey && pKernelChannel->bUseScrubKey)
                {
                    // return all kern channels that use scrub key
                   *ppKernelChannel = pKernelChannel;
                }
            }
            else
            {
                // return all user channels
               *ppKernelChannel = pKernelChannel;
            }

            // if we found a channel then return early
            if (*ppKernelChannel != NULL)
                return NV_OK;
        }
    }
    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * Sets fatal error state in ConfCompute session by setting GPU ready
 * state to false and invalidating the SPDM session.
 *
 * @param[in] pGpu                     : OBJGPU Pointer
 * @param[in] pConfCompute             : ConfidentialCompute pointer
 */
void
confComputeSetErrorState_KERNEL
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute
)
{
    OBJSYS                                                     *pSys    = SYS_GET_INSTANCE();
    OBJGPUMGR                                                  *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS                                                   status  = NV_OK;
    RM_API                                                     *pRmApi  = NULL;
    NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS  params  = {0};

    NV_PRINTF(LEVEL_ERROR, "ConfCompute: Fatal error hit!\n");

    // Set ready state to false
    pConfCompute->bAcceptClientRequest   = NV_FALSE;
    pGpuMgr->ccCaps.bAcceptClientRequest = NV_FALSE;
    pGpuMgr->ccCaps.bFatalFailure        = NV_TRUE;
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE,
                             &params,
                             sizeof(params));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "ConfCompute: Failed setting GPU state to not ready!\n");
    }

    // Invalidate SPDM session and all keys
    status = _confComputeDeinitSpdmSessionAndKeys(pGpu, pConfCompute);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "ConfCompute: Failed tearing down SPDM!: 0x%x!\n", status);
    }
}

/*!
 * Deinitialize all keys required for the Confidential Compute session.
 *
 * Note: Must occur in destructor, rather than confComputeStateDestroy
 * as engine state is set to destroy before GSP-RM teardown. Since we
 * still need encryption until after GSP-RM teardown, we wait until
 * object destruction.
 *
 * @param[in]  pConfCompute  ConfidentialCompute pointer
 */
void
confComputeDestruct_KERNEL
(
    ConfidentialCompute *pConfCompute
)
{
    NV_STATUS  status = NV_OK;
    OBJGPU    *pGpu   = ENG_GET_GPU(pConfCompute);

    status = _confComputeDeinitSpdmSessionAndKeys(pGpu, pConfCompute);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "ConfCompute: Failed deinitializing SPDM: 0x%x!\n", status);
    }

    return;
}

/*!
 * Get key slot from global key
 *
 * @param[in]  pConfCompute             : ConfidentialCompute pointer
 * @param[in]  globalKeyId              : globalKeyId
 * @param[out] pSlot                    : key slot
 */
NV_STATUS
confComputeGetKeySlotFromGlobalKeyId_IMPL
(
    ConfidentialCompute *pConfCompute,
    NvU32  globalKeyId,
    NvU32 *pSlot
)
{
    NvU32 slot;
    NvU16 keyspace = CC_GKEYID_GET_KEYSPACE(globalKeyId);
    NvU32 keySlotIndex = 0;

    NV_ASSERT_OR_RETURN(pSlot != NULL, NV_ERR_INVALID_ARGUMENT);
    for (NvU16 index = 0; index < CC_KEYSPACE_SIZE; index++)
    {
        if (index == keyspace)
        {
            break;
        }
        else
        {
            keySlotIndex += _confComputeGetKeyspaceSize(index);
        }
    }

    slot = keySlotIndex + CC_GKEYID_GET_LKEYID(globalKeyId);
    if (slot >= CC_KEYSPACE_TOTAL_SIZE)
        return NV_ERR_INVALID_ARGUMENT;

    *pSlot = slot;
    return NV_OK;
}

static NvU32
_confComputeGetKeyspaceSize
(
    NvU16 keyspace
)
{
    switch (keyspace)
    {
        case CC_KEYSPACE_GSP:
            return CC_KEYSPACE_GSP_SIZE;
        case CC_KEYSPACE_SEC2:
            return CC_KEYSPACE_SEC2_SIZE;
        case CC_KEYSPACE_LCE0:
        case CC_KEYSPACE_LCE1:
        case CC_KEYSPACE_LCE2:
        case CC_KEYSPACE_LCE3:
        case CC_KEYSPACE_LCE4:
        case CC_KEYSPACE_LCE5:
        case CC_KEYSPACE_LCE6:
        case CC_KEYSPACE_LCE7:
            return CC_KEYSPACE_LCE_SIZE;
        default:
            NV_ASSERT_OR_RETURN(NV_FALSE, 0);
    }
}
