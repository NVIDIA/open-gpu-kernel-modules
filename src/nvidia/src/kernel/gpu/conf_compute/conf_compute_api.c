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
 *      Confidential Compute API Object Module                              *
 *                                                                          *
 \**************************************************************************/

#include "core/locks.h"
#include "rmapi/rs_utils.h"
#include "core/system.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/conf_compute/conf_compute_api.h"
#include "gpu/subdevice/subdevice.h"
#include "class/clcb33.h" // NV_CONFIDENTIAL_COMPUTE
#include "nvrm_registry.h"

NV_STATUS
confComputeApiConstruct_IMPL
(
    ConfidentialComputeApi       *pConfComputeApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    pConfComputeApi->pCcCaps = &pGpuMgr->ccCaps;

    return NV_OK;
}

void
confComputeApiDestruct_IMPL
(
    ConfidentialComputeApi *pConfComputeApi
)
{
}

NV_STATUS
confComputeApiCtrlCmdSystemGetCapabilities_IMPL
(
    ConfidentialComputeApi                                  *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS *pParams
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    CONF_COMPUTE_CAPS *pCcCaps = pConfComputeApi->pCcCaps;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    pParams->cpuCapability = NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_NONE;
    if ((sysGetStaticConfig(pSys))->bOsCCEnabled)
    {
        pParams->cpuCapability = NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_AMD_SEV;
        if ((sysGetStaticConfig(pSys))->bOsCCSevSnpEnabled)
        {
            pParams->cpuCapability = NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_AMD_SEV_SNP;
        }
        else if ((sysGetStaticConfig(pSys))->bOsCCSnpVtomEnabled)
        {
            pParams->cpuCapability = NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_AMD_SNP_VTOM;
        }
        else if ((sysGetStaticConfig(pSys))->bOsCCTdxEnabled)
        {
            pParams->cpuCapability = NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_INTEL_TDX;
        }
    }

    pParams->gpusCapability = NV_CONF_COMPUTE_SYSTEM_GPUS_CAPABILITY_NONE;
    pParams->environment = NV_CONF_COMPUTE_SYSTEM_ENVIRONMENT_UNAVAILABLE;
    pParams->ccFeature = NV_CONF_COMPUTE_SYSTEM_FEATURE_DISABLED;
    pParams->devToolsMode = NV_CONF_COMPUTE_SYSTEM_DEVTOOLS_MODE_DISABLED;
    pParams->multiGpuMode = NV_CONF_COMPUTE_SYSTEM_MULTI_GPU_MODE_NONE;

    if (pCcCaps->bApmFeatureCapable)
    {
        pParams->gpusCapability = NV_CONF_COMPUTE_SYSTEM_GPUS_CAPABILITY_APM;
    }
    else if (pCcCaps->bHccFeatureCapable)
    {
        pParams->gpusCapability = NV_CONF_COMPUTE_SYSTEM_GPUS_CAPABILITY_HCC;
    }

    if (pCcCaps->bCCFeatureEnabled)
    {
        if (pCcCaps->bApmFeatureCapable)
        {
            pParams->ccFeature = NV_CONF_COMPUTE_SYSTEM_FEATURE_APM_ENABLED;
        }
        else if (pCcCaps->bHccFeatureCapable)
        {
            pParams->ccFeature = NV_CONF_COMPUTE_SYSTEM_FEATURE_HCC_ENABLED;
        }
    }

    if (pParams->ccFeature != NV_CONF_COMPUTE_SYSTEM_FEATURE_DISABLED)
    {
        pParams->environment = NV_CONF_COMPUTE_SYSTEM_ENVIRONMENT_PROD;
    }

    if (pCcCaps->bDevToolsModeEnabled)
    {
        pParams->devToolsMode = NV_CONF_COMPUTE_SYSTEM_DEVTOOLS_MODE_ENABLED;
        pParams->environment = NV_CONF_COMPUTE_SYSTEM_ENVIRONMENT_SIM;
    }

    if (pCcCaps->bMultiGpuProtectedPcieModeEnabled)
    {
        // Do not advertise HCC as ON to callers when PPCIe is ON
        pParams->ccFeature = NV_CONF_COMPUTE_SYSTEM_FEATURE_DISABLED;
        pParams->multiGpuMode = NV_CONF_COMPUTE_SYSTEM_MULTI_GPU_MODE_PROTECTED_PCIE;
    }
    else if (pCcCaps->bMultiGpuNvleModeEnabled)
    {
        pParams->multiGpuMode = NV_CONF_COMPUTE_SYSTEM_MULTI_GPU_MODE_NVLE;
    }

    return NV_OK;
}

NV_STATUS
confComputeApiCtrlCmdSystemGetGpusState_IMPL
(
    ConfidentialComputeApi                                *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS *pParams
)
{
    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    pParams->bAcceptClientRequest = pConfComputeApi->pCcCaps->bAcceptClientRequest;

    return NV_OK;
}

NV_STATUS
confComputeApiCtrlCmdSystemSetGpusState_IMPL
(
    ConfidentialComputeApi                                *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE_PARAMS *pParams
)
{
    OBJGPU    *pGpu;
    NvU32      gpuMask;
    NvU32      gpuInstance = 0;
    RM_API    *pRmApi      = NULL;
    NV_STATUS  status = NV_OK;
    NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS params = {0};

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Make sure 'ready state' can't be set after being set to false once.
    if (pConfComputeApi->pCcCaps->bFatalFailure)
        return NV_ERR_INVALID_ARGUMENT;

    if (pConfComputeApi->pCcCaps->bAcceptClientRequest && !pParams->bAcceptClientRequest)
    {
        pConfComputeApi->pCcCaps->bFatalFailure = NV_TRUE;
        pConfComputeApi->pCcCaps->bAcceptClientRequest = NV_FALSE;
    }

    params.bAcceptClientRequest = pParams->bAcceptClientRequest;
    (void)gpumgrGetGpuAttachInfo(NULL, &gpuMask);

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (IS_VIRTUAL(pGpu))
            return NV_ERR_NOT_SUPPORTED;

        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        status = pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE,
                                &params,
                                sizeof(params));
        if (status != NV_OK)
            return status;
    }

    pConfComputeApi->pCcCaps->bAcceptClientRequest = pParams->bAcceptClientRequest;
    return status;
}

NV_STATUS
confComputeApiCtrlCmdGpuGetVidmemSize_IMPL
(
    ConfidentialComputeApi                              *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS *pParams
)
{
    Subdevice        *pSubdevice            = NULL;
    OBJGPU           *pGpu                  = NULL;
    Heap             *pHeap                 = NULL;
    Heap             *pMemoryPartitionHeap  = NULL;
    KernelMIGManager *pKernelMIGManager     = NULL;
    NvU64             totalProtectedBytes   = 0;
    NvU64             totalUnprotectedBytes = 0;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        subdeviceGetByHandle(RES_GET_CLIENT(pConfComputeApi),
                             pParams->hSubDevice, &pSubdevice));

    pGpu = GPU_RES_GET_GPU(pSubdevice);
    pHeap = GPU_GET_HEAP(pGpu);
    pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    //
    // If MIG-GPU-Instancing is enabled, we check for GPU instance subscription
    // and provide GPU instance local info
    //
    if (IS_MIG_IN_USE(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
            kmigmgrGetMemoryPartitionHeapFromDevice(pGpu, pKernelMIGManager,
                                                    GPU_RES_GET_DEVICE(pSubdevice),
                                                    &pMemoryPartitionHeap));
        //
        // If client is associated with a GPU instance then point pHeap
        // to client's memory partition heap
        //
        if (pMemoryPartitionHeap != NULL)
             pHeap = pMemoryPartitionHeap;
    }

    pmaGetTotalProtectedMemory(&pHeap->pmaObject, &totalProtectedBytes);
    pmaGetTotalUnprotectedMemory(&pHeap->pmaObject, &totalUnprotectedBytes);

    pParams->protectedMemSizeInKb = totalProtectedBytes >> 10;
    pParams->unprotectedMemSizeInKb = totalUnprotectedBytes >> 10;

    return NV_OK;
}

NV_STATUS
confComputeApiCtrlCmdGpuSetVidmemSize_IMPL
(
    ConfidentialComputeApi                              *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE_PARAMS *pParams
)
{
    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    return NV_OK;
}

NV_STATUS
confComputeApiCtrlCmdGetGpuCertificate_IMPL
(
    ConfidentialComputeApi                              *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS *pParams
)
{
    Subdevice           *pSubdevice   = NULL;
    OBJGPU              *pGpu         = NULL;
    ConfidentialCompute *pConfCompute = NULL;
    Spdm                *pSpdm        = NULL;
    NV_STATUS            status       = NV_OK;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        subdeviceGetByHandle(RES_GET_CLIENT(pConfComputeApi),
                             pParams->hSubDevice, &pSubdevice));
    pGpu         = GPU_RES_GET_GPU(pSubdevice);
    pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    pSpdm        = GPU_GET_SPDM(pGpu);

    if (pConfCompute != NULL && confComputeIsSpdmEnabled(pGpu, pConfCompute))
    {
        if (pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED))
        {
            // Set max size of certificate buffers before calling SPDM.
            pParams->certChainSize            = NV_CONF_COMPUTE_CERT_CHAIN_MAX_SIZE;
            pParams->attestationCertChainSize = NV_CONF_COMPUTE_ATTESTATION_CERT_CHAIN_MAX_SIZE;

            status = spdmGetCertChains_HAL(pGpu,
                                           pSpdm,
                                           pParams->certChain,
                                           &pParams->certChainSize,
                                           pParams->attestationCertChain,
                                           &pParams->attestationCertChainSize);
            if (status != NV_OK)
            {
                // Attestation failure, tear down the CC system.
                confComputeSetErrorState(pGpu, pConfCompute);
            }

            return status;
        }
    }
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
confComputeApiCtrlCmdGetGpuAttestationReport_IMPL
(
    ConfidentialComputeApi                                     *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS *pParams
)
{
    Subdevice           *pSubdevice   = NULL;
    OBJGPU              *pGpu         = NULL;
    ConfidentialCompute *pConfCompute = NULL;
    Spdm                *pSpdm        = NULL;
    NV_STATUS            status       = NV_OK;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        subdeviceGetByHandle(RES_GET_CLIENT(pConfComputeApi),
                             pParams->hSubDevice, &pSubdevice));

    pGpu         = GPU_RES_GET_GPU(pSubdevice);
    pSpdm        = GPU_GET_SPDM(pGpu);
    pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);

    if (pConfCompute != NULL &&
        confComputeIsSpdmEnabled(pGpu, pConfCompute) &&
        pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED))
    {
        // Set max size of report buffers before calling SPDM.
        pParams->attestationReportSize    = NV_CONF_COMPUTE_GPU_ATTESTATION_REPORT_MAX_SIZE;
        pParams->cecAttestationReportSize = NV_CONF_COMPUTE_GPU_CEC_ATTESTATION_REPORT_MAX_SIZE;

        status = spdmGetAttestationReport(pGpu,
                                          pSpdm,
                                          pParams->nonce,
                                          pParams->attestationReport,
                                          &pParams->attestationReportSize,
                                          &pParams->isCecAttestationReportPresent,
                                          pParams->cecAttestationReport,
                                          &pParams->cecAttestationReportSize);
        if (status != NV_OK)
        {
            // Attestation failure, tear down the CC system.
            confComputeSetErrorState(pGpu, pConfCompute);
        }

        return status;
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
confComputeApiCtrlCmdGpuGetNumSecureChannels_IMPL
(
    ConfidentialComputeApi                                     *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS *pParams
)
{
    Subdevice  *pSubdevice;
    OBJGPU     *pGpu;
    KernelFifo *pKernelFifo;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        subdeviceGetByHandle(RES_GET_CLIENT(pConfComputeApi),
        pParams->hSubDevice, &pSubdevice));

    pGpu = GPU_RES_GET_GPU(pSubdevice);
    pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    pParams->maxSec2Channels = pKernelFifo->maxSec2SecureChannels;
    pParams->maxCeChannels = pKernelFifo->maxCeSecureChannels;

    return NV_OK;
}

NV_STATUS
confComputeApiCtrlCmdSystemGetSecurityPolicy_IMPL
(
    ConfidentialComputeApi                          *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY_PARAMS *pParams
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    pParams->attackerAdvantage = pGpuMgr->ccAttackerAdvantage;

    return NV_OK;
}

NV_STATUS
confComputeApiCtrlCmdSystemSetSecurityPolicy_IMPL
(
    ConfidentialComputeApi                          *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY_PARAMS *pParams
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    OBJGPU    *pGpu;
    NvU32      gpuMask;
    NvU32      gpuInstance = 0;
    RM_API    *pRmApi      = NULL;
    NV_STATUS  status = NV_OK;
    NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS params = {0};

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // CC security policy can only be set before GpuReadyState is set.
    NV_CHECK_OR_RETURN(LEVEL_INFO, pConfComputeApi->pCcCaps->bAcceptClientRequest == NV_FALSE,
                       NV_ERR_INVALID_STATE);

    if ((pParams->attackerAdvantage < SET_SECURITY_POLICY_ATTACKER_ADVANTAGE_MIN) ||
        (pParams->attackerAdvantage > SET_SECURITY_POLICY_ATTACKER_ADVANTAGE_MAX))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    params.attackerAdvantage = pParams->attackerAdvantage;
    (void)gpumgrGetGpuAttachInfo(NULL, &gpuMask);

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        ConfidentialCompute* pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);

        status = pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY,
                                &params,
                                sizeof(params));
        if (status != NV_OK)
            return status;

        NV_ASSERT_OK_OR_RETURN(confComputeSetKeyRotationThreshold(pConfCompute,
                                                                  pParams->attackerAdvantage));
    }

    pGpuMgr->ccAttackerAdvantage = pParams->attackerAdvantage;

    return status;
}

NV_STATUS
confComputeApiCtrlCmdGpuGetKeyRotationState_IMPL
(
    ConfidentialComputeApi                                     *pConfComputeApi,
    NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_KEY_ROTATION_STATE_PARAMS *pParams
)
{
    Subdevice           *pSubdevice;
    OBJGPU              *pGpu;
    ConfidentialCompute *pConfCompute;
    NvBool               bKernelKeyRotation = NV_FALSE;
    NvBool               bUserKeyRotation = NV_FALSE;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        subdeviceGetByHandle(RES_GET_CLIENT(pConfComputeApi),
        pParams->hSubDevice, &pSubdevice));

    pGpu = GPU_RES_GET_GPU(pSubdevice);
    pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);

    if ((pConfCompute == NULL) ||
        !pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED) ||
        !pConfCompute->getProperty(pConfCompute, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED))
    {
        pParams->keyRotationState = NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_DISABLED;
        return NV_OK;
    }

    bKernelKeyRotation = FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_KEY_ROTATION, _KERNEL_KEYS, _YES,
                                      pConfCompute->keyRotationEnableMask);

    bUserKeyRotation = FLD_TEST_DRF(_REG_STR, _RM_CONF_COMPUTE_KEY_ROTATION, _USER_KEYS, _YES,
                                    pConfCompute->keyRotationEnableMask);
    if (bKernelKeyRotation && bUserKeyRotation)
    {
        pParams->keyRotationState = NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_BOTH_ENABLED;
    }
    else if (bKernelKeyRotation && !bUserKeyRotation)
    {
        pParams->keyRotationState = NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_KERN_ENABLED;
    }
    else if (!bKernelKeyRotation && bUserKeyRotation)
    {
        pParams->keyRotationState = NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_USER_ENABLED;
    }
    else
    {
        pParams->keyRotationState = NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_DISABLED;
    }
    return NV_OK;
}
