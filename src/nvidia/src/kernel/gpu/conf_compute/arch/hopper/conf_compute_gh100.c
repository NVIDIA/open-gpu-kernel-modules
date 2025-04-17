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
 *      Confidential Compute HALs                                           *
 *                                                                          *
 \**************************************************************************/

#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/conf_compute/conf_compute.h"
#include "published/hopper/gh100/dev_fuse.h"
#include "rmapi/rmapi.h"
#include "conf_compute/cc_keystore.h"
//#include "hopper/gh100/dev_se_seb.h"

/*!
 * check if debug mode is enabled.
 *
 * @param[in]  pGpu              GPU object pointer
 * @param[in]  pConfCompute      ConfidentialCompute pointer
 */
NvBool
confComputeIsDebugModeEnabled_GH100
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute
)
{
   NvU32 fuseStat = GPU_REG_RD32(pGpu, NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS);

   return !FLD_TEST_DRF(_FUSE_OPT, _SECURE_GSP_DEBUG_DIS, _DATA, _YES, fuseStat);
}

/*!
 * @brief confComputeIsGpuCcCapable
 *        Checks if Gpu can handle ConfCompute workloads.
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 */
NvBool
confComputeIsGpuCcCapable_GH100
(
    OBJGPU               *pGpu,
    ConfidentialCompute  *pConfCompute
)
{
    NvU32 reg;

    if (confComputeIsDebugModeEnabled_HAL(pGpu, pConfCompute))
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot boot Confidential Compute as debug board is not supported.\n");
        return NV_FALSE;
    }

    reg = GPU_REG_RD32(pGpu, NV_FUSE_SPARE_BIT_0);
    if (FLD_TEST_DRF(_FUSE, _SPARE_BIT_0, _DATA, _ENABLE, reg))
    {
        if (FLD_TEST_DRF(_FUSE, _SPARE_BIT_1, _DATA, _ENABLE, GPU_REG_RD32(pGpu, NV_FUSE_SPARE_BIT_1))
            && FLD_TEST_DRF(_FUSE, _SPARE_BIT_2, _DATA, _DISABLE, GPU_REG_RD32(pGpu, NV_FUSE_SPARE_BIT_2)))
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

/*!
 * @brief Derives secrets for given CE key space.
 *
 * @param[in]  ceRmEngineTypeIdx    the RM engine type for LCE
 * @param[in]  ccKeyspaceLCEIndex   the key space index
 *
 * @return NV_ERR_INVALID_ARGUMENT if engine is not correct.
 *         NV_OK otherwise.
 */
NV_STATUS
confComputeDeriveSecretsForCEKeySpace_GH100
(
    ConfidentialCompute *pConfCompute,
    RM_ENGINE_TYPE       ceRmEngineTypeIdx,
    NvU32                ccKeyspaceLCEIndex
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pConfCompute);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

    //
    // ceRmEngineTypeIdx is not exactly used as a CE index.
    // For example, ceRmEngineTypeIdx is 0 for the first secure CE which is
    // actually the LCE 2.
    // It is used as a key space index.
    //
    // TODO: refactor the code to use exact the engine type number, bug 4594450.
    //
    params.engineId = gpuGetNv2080EngineType(ceRmEngineTypeIdx);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                           pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                           &params,
                                           sizeof(params)));

    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_H2D_USER)));
    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_D2H_USER)));
    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_H2D_KERN)));
    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_D2H_KERN)));

    confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_H2D_USER),
        (void*)&params.ivMaskSet[0].ivMask);

    confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_D2H_USER),
        (void*)&params.ivMaskSet[1].ivMask);

    confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_H2D_KERN),
        (void*)&params.ivMaskSet[2].ivMask);

    confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_D2H_KERN),
        (void*)&params.ivMaskSet[3].ivMask);

    return NV_OK;
}

NV_STATUS
confComputeDeriveSecrets_GH100(ConfidentialCompute *pConfCompute,
                               NvU32                engine)
{
    OBJGPU *pGpu = ENG_GET_GPU(pConfCompute);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_OK;
    }

    switch (engine)
    {
        case MC_ENGINE_IDX_GSP:
            // GSP automatically derives these keys so no need for CPU-RM to trigger it.
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_LOCKED_RPC)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_LOCKED_RPC)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_DMA)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_DMA)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_NVLE_P2P_WRAPPING)));
            break;
        case MC_ENGINE_IDX_SEC2:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_SEC2;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS,
                                                   &params,
                                                   sizeof(params)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_USER),
                                                 (void*)&params.ivMaskSet[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_USER].ivMask[0]);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_KERN),
                                                 (void*)&params.ivMaskSet[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_KERNEL].ivMask[0]);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_SCRUBBER),
                                                 (void*)&params.ivMaskSet[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_SCRUBBER].ivMask[0]);

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_SCRUBBER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER)));
        }
        break;

        case MC_ENGINE_IDX_CE2:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY0, CC_KEYSPACE_LCE0);
            break;

        case MC_ENGINE_IDX_CE3:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY1, CC_KEYSPACE_LCE1);
            break;

        case MC_ENGINE_IDX_CE4:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY2, CC_KEYSPACE_LCE2);
            break;

        case MC_ENGINE_IDX_CE5:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY3, CC_KEYSPACE_LCE3);
            break;

        case MC_ENGINE_IDX_CE6:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY4, CC_KEYSPACE_LCE4);
            break;

        case MC_ENGINE_IDX_CE7:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY5, CC_KEYSPACE_LCE5);
            break;

        case MC_ENGINE_IDX_CE8:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY6, CC_KEYSPACE_LCE6);
            break;

        case MC_ENGINE_IDX_CE9:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY7, CC_KEYSPACE_LCE7);
            break;


        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * Returns RM engine Id corresponding to a key space
 *
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 * @param[in]     keySpace                 : value of keyspace from cc_keystore.h
 */
RM_ENGINE_TYPE
confComputeGetEngineIdFromKeySpace_GH100
(
    ConfidentialCompute *pConfCompute,
    NvU32 keySpace
)
{
    if (keySpace == CC_KEYSPACE_GSP)
    {
        return RM_ENGINE_TYPE_NULL;
    }

    if (keySpace == CC_KEYSPACE_SEC2)
    {
        return RM_ENGINE_TYPE_SEC2;
    }

    switch (keySpace)
    {
        case CC_KEYSPACE_LCE0:
            return RM_ENGINE_TYPE_COPY2;

        case CC_KEYSPACE_LCE1:
            return RM_ENGINE_TYPE_COPY3;

        case CC_KEYSPACE_LCE2:
            return RM_ENGINE_TYPE_COPY4;

        case CC_KEYSPACE_LCE3:
            return RM_ENGINE_TYPE_COPY5;

        case CC_KEYSPACE_LCE4:
            return RM_ENGINE_TYPE_COPY6;

        case CC_KEYSPACE_LCE5:
            return RM_ENGINE_TYPE_COPY7;

        case CC_KEYSPACE_LCE6:
            return RM_ENGINE_TYPE_COPY8;

        case CC_KEYSPACE_LCE7:
            return RM_ENGINE_TYPE_COPY9;
    }

    return RM_ENGINE_TYPE_NULL;
 }

/*!
 * Checks if key is kernel key or user key
 *
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 * @param[in]     keyId                    : global keyId
 */
NvBool
confComputeGlobalKeyIsKernelPriv_GH100
(
    ConfidentialCompute *pConfCompute,
    NvU32 globalKeyId
)
{
    NvU32 keySpace = CC_GKEYID_GET_KEYSPACE(globalKeyId);
    NvU32 localKeyId = CC_GKEYID_GET_LKEYID(globalKeyId);
    if (keySpace == CC_KEYSPACE_GSP)
    {
        return NV_TRUE;
    }
    else if (keySpace == CC_KEYSPACE_SEC2)
    {
        switch (localKeyId)
        {
            case CC_LKEYID_CPU_SEC2_DATA_KERN:
            case CC_LKEYID_CPU_SEC2_HMAC_KERN:
            case CC_LKEYID_CPU_SEC2_DATA_SCRUBBER:
            case CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER:
                return NV_TRUE;
        }
    }
    else
    {
        NV_ASSERT((keySpace >= CC_KEYSPACE_LCE0) && (keySpace <= CC_KEYSPACE_LCE7));
        switch (localKeyId)
        {
            case CC_LKEYID_LCE_H2D_KERN:
            case CC_LKEYID_LCE_D2H_KERN:
                return NV_TRUE;
        }
    }
    return NV_FALSE;
}

/*!
 * Checks if key is UVM key
 *
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 * @param[in]     keyId                    : global keyId
 */
NvBool
confComputeGlobalKeyIsUvmKey_GH100
(
    ConfidentialCompute *pConfCompute,
    NvU32 globalKeyId
)
{
    NvU32 keySpace = CC_GKEYID_GET_KEYSPACE(globalKeyId);
    NvU32 localKeyId = CC_GKEYID_GET_LKEYID(globalKeyId);
    if (keySpace == CC_KEYSPACE_GSP)
    {
        return NV_FALSE;
    }
    else if (keySpace == CC_KEYSPACE_SEC2)
    {
        switch (localKeyId)
        {
            case CC_LKEYID_CPU_SEC2_DATA_SCRUBBER:
            case CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER:
                return NV_FALSE;
        }
    }
    return confComputeGlobalKeyIsKernelPriv_HAL(pConfCompute, globalKeyId);
}

NV_STATUS confComputeUpdateSecrets_GH100(ConfidentialCompute *pConfCompute,
                                         NvU32                globalKeyId)
{
    OBJGPU *pGpu   = ENG_GET_GPU(pConfCompute);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32   h2dKey, d2hKey;
    NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS params = {0};

    // GSP keys are not supported. They are updated without an RPC to GSP-RM.
    NV_ASSERT(CC_GKEYID_GET_KEYSPACE(globalKeyId) != CC_KEYSPACE_GSP);

    confComputeGetKeyPairByKey(pConfCompute, globalKeyId, &h2dKey, &d2hKey);
    params.globalH2DKey = h2dKey;

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(
                           pRmApi,
                           pGpu->hInternalClient,
                           pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_ROTATE_KEYS,
                           &params,
                           sizeof(NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS)));

    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreUpdateKey_HAL(pConfCompute, h2dKey));
    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreUpdateKey_HAL(pConfCompute, d2hKey));

    // Both SEC2 and LCEs have an encrypt IV mask.
    confComputeKeyStoreDepositIvMask_HAL(pConfCompute, h2dKey, &params.updatedEncryptIVMask);

    // Only LCEs have a decrypt IV mask.
    if ((CC_GKEYID_GET_KEYSPACE(d2hKey) >= CC_KEYSPACE_LCE0) &&
        (CC_GKEYID_GET_KEYSPACE(d2hKey) <= confComputeGetMaxCeKeySpaceIdx(pConfCompute)))
    {
        confComputeKeyStoreDepositIvMask_HAL(pConfCompute, d2hKey, &params.updatedDecryptIVMask);
    }

    CHANNEL_ITERATOR iterator;
    KernelChannel *pKernelChannel;

    NV_ASSERT_OK_OR_RETURN(confComputeInitChannelIterForKey(pGpu, pConfCompute, h2dKey, &iterator));

    while (confComputeGetNextChannelForKey(pGpu, pConfCompute, &iterator, h2dKey, &pKernelChannel) == NV_OK)
    {
        NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreRetrieveViaChannel(
            pConfCompute, pKernelChannel, ROTATE_IV_ALL_VALID, NV_FALSE, &pKernelChannel->clientKmb));

        // After key rotation channel counter stays the same but message counter is cleared.
        pKernelChannel->clientKmb.encryptBundle.iv[0] = 0x00000000;

        if ((CC_GKEYID_GET_KEYSPACE(d2hKey) >= CC_KEYSPACE_LCE0) &&
            (CC_GKEYID_GET_KEYSPACE(d2hKey) <= confComputeGetMaxCeKeySpaceIdx(pConfCompute)))
        {
            pKernelChannel->clientKmb.decryptBundle.iv[0] = 0x00000000;
        }
        else
        {
            pKernelChannel->clientKmb.hmacBundle.nonce[0] = 0x00000000;
            pKernelChannel->clientKmb.hmacBundle.nonce[1] = 0x00000000;
            pKernelChannel->clientKmb.hmacBundle.nonce[2] = 0x00000000;
            pKernelChannel->clientKmb.hmacBundle.nonce[3] = 0x00000000;
            pKernelChannel->clientKmb.hmacBundle.nonce[4] = 0x00000000;
            pKernelChannel->clientKmb.hmacBundle.nonce[5] = 0x00000000;
        }
	}
    return NV_OK;
}
