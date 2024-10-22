/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/rmapi.h"
#include "conf_compute/cc_keystore.h"

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
confComputeDeriveSecretsForCEKeySpace_GB100
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
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_H2D_KERN)));

    NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_D2H_KERN)));

    confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_H2D_KERN),
        (void*)&params.ivMaskSet[CC_LKEYID_LCE_H2D_KERN].ivMask);

    confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
        CC_GKEYID_GEN(ccKeyspaceLCEIndex, CC_LKEYID_LCE_D2H_KERN),
        (void*)&params.ivMaskSet[CC_LKEYID_LCE_D2H_KERN].ivMask);

    return NV_OK;
}
NV_STATUS
confComputeDeriveSecrets_GB100
(
    ConfidentialCompute *pConfCompute,
    NvU32                engine
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pConfCompute);

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_OK;
    }

    switch (engine)
    {
        case MC_ENGINE_IDX_CE12:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY10, CC_KEYSPACE_LCE10);
            break;

        case MC_ENGINE_IDX_CE13:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY11, CC_KEYSPACE_LCE11);
            break;

        case MC_ENGINE_IDX_CE14:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY12, CC_KEYSPACE_LCE12);
            break;

        case MC_ENGINE_IDX_CE15:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY13, CC_KEYSPACE_LCE13);
            break;

        case MC_ENGINE_IDX_CE16:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY14, CC_KEYSPACE_LCE14);
            break;

        case MC_ENGINE_IDX_CE17:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY15, CC_KEYSPACE_LCE15);
            break;

        case MC_ENGINE_IDX_CE18:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY16, CC_KEYSPACE_LCE16);
            break;

        case MC_ENGINE_IDX_CE19:
            confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute,
                RM_ENGINE_TYPE_COPY17, CC_KEYSPACE_LCE17);
            break;

        default:
            return confComputeDeriveSecrets_GH100(pConfCompute, engine);
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
confComputeGetEngineIdFromKeySpace_GB100
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
        case CC_KEYSPACE_LCE10:
            return RM_ENGINE_TYPE_COPY12;

        case CC_KEYSPACE_LCE11:
            return RM_ENGINE_TYPE_COPY13;

        case CC_KEYSPACE_LCE12:
            return RM_ENGINE_TYPE_COPY14;

        case CC_KEYSPACE_LCE13:
            return RM_ENGINE_TYPE_COPY15;

        case CC_KEYSPACE_LCE14:
            return RM_ENGINE_TYPE_COPY16;

        case CC_KEYSPACE_LCE15:
            return RM_ENGINE_TYPE_COPY17;

        case CC_KEYSPACE_LCE16:
            return RM_ENGINE_TYPE_COPY18;

        case CC_KEYSPACE_LCE17:
            return RM_ENGINE_TYPE_COPY19;

        default:
            return confComputeGetEngineIdFromKeySpace_GH100(pConfCompute, keySpace);
    }

    return RM_ENGINE_TYPE_NULL;
}

/*!
 * Returns a key space corresponding to a channel
 *
 * @param[in]   pConfCompute               : ConfidentialCompute pointer
 * @param[in]   pKernelChannel             : KernelChannel pointer
 * @param[out]  keySpace                   : value of keyspace from cc_keystore.h
 */
NV_STATUS
confComputeGetKeySpaceFromKChannel_GB100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    NvU16               *keyspace
)
{
    switch (kchannelGetEngineType(pKernelChannel))
    {
        case RM_ENGINE_TYPE_COPY12:
            *keyspace = CC_KEYSPACE_LCE10;
            break;
        case RM_ENGINE_TYPE_COPY13:
            *keyspace = CC_KEYSPACE_LCE11;
            break;
        case RM_ENGINE_TYPE_COPY14:
            *keyspace = CC_KEYSPACE_LCE12;
            break;
        case RM_ENGINE_TYPE_COPY15:
            *keyspace = CC_KEYSPACE_LCE13;
            break;
        case RM_ENGINE_TYPE_COPY16:
            *keyspace = CC_KEYSPACE_LCE14;
            break;
        case RM_ENGINE_TYPE_COPY17:
            *keyspace = CC_KEYSPACE_LCE15;
            break;
        case RM_ENGINE_TYPE_COPY18:
            *keyspace = CC_KEYSPACE_LCE16;
            break;
        case RM_ENGINE_TYPE_COPY19:
            *keyspace = CC_KEYSPACE_LCE17;
            break;
        default:
            return confComputeGetKeySpaceFromKChannel_GH100(pConfCompute,
                    pKernelChannel, keyspace);
    }

    return NV_OK;
}
