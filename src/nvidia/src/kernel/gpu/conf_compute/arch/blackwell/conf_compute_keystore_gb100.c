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

#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/conf_compute/conf_compute.h"
// #include "class/cl2080.h"
// #include "libraries/nvport/nvport.h"
#include "kernel/gpu/spdm/libspdm_includes.h"
#include "hal/library/cryptlib.h"

/*!
 * Return the key ID for a given LCE channel and rotation operation.
 * If rotateOperation is ROTATE_IV_ALL_VALID then it will return the least
 * key ID of the key pair; ie the one that corresponds to an even numbered slot.
 *
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[in]   pKernelChannel  : KernelChannel pointer
 * @param[in]   rotateOperation : The type of rotation operation
 * @param[out]  pKeyId          : pointer to a key Id
 */
NV_STATUS
confComputeGetLceKeyIdFromKChannel_GB100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    ROTATE_IV_TYPE       rotateOperation,
    NvU16               *pKeyId
)
{
    NV_ASSERT_OR_RETURN(pKeyId != NULL, NV_ERR_INVALID_PARAMETER);

    if ((rotateOperation == ROTATE_IV_ENCRYPT) ||
        (rotateOperation == ROTATE_IV_ALL_VALID))
    {
        *pKeyId = CC_LKEYID_LCE_H2D_KERN;
    }
    else
    {
        *pKeyId = CC_LKEYID_LCE_D2H_KERN;
    }

    return NV_OK;
}

NV_STATUS
confComputeKeyStoreDeriveViaChannel_GB100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    ROTATE_IV_TYPE       rotateOperation,
    NvBool               bIncludeIvOrNonce,
    CC_KMB              *keyMaterialBundle
)
{

    NvU16            keyId;
    NvLength         keySize = 0;
    NvU8            *pKey = NULL;
    const uint8_t   *pStr = NULL;
    NvU8            curKeySeed[CC_EXPORT_MASTER_KEY_SIZE_BYTES] = {0};


    //
    // SEC2 Per Channel key support has not been added yet. Hence SEC2 keys need to be restored from keystore.
    // For CE, keys would be derived per channel. Replace the return with logic for deriving SEC2 keys
    //
    if (kchannelGetEngineType(pKernelChannel) == RM_ENGINE_TYPE_SEC2) {
        return NV_OK;
    }
    else if (RM_ENGINE_TYPE_IS_COPY(kchannelGetEngineType(pKernelChannel)))
    {
        if (confComputeGetAndUpdateCurrentKeySeed_HAL(pConfCompute, curKeySeed) != NV_OK)
        {
            return NV_ERR_INVALID_PARAMETER;
        }
        for (keyId = CC_LKEYID_LCE_H2D_KERN; keyId <= CC_LKEYID_LCE_D2H_KERN; keyId++)
        {
            if (keyId == CC_LKEYID_LCE_H2D_KERN)
            {
                pKey    = (NvU8 *)(pKernelChannel->clientKmb.encryptBundle.key);
                keySize = sizeof(pKernelChannel->clientKmb.encryptBundle.key);
                pStr = (const uint8_t *)CC_KEYID_PERCHANNEL_CPU_GPU_UPDATE_KEY;
            }

            if (keyId == CC_LKEYID_LCE_D2H_KERN)
            {
                pKey    = (NvU8 *)(pKernelChannel->clientKmb.decryptBundle.key);
                keySize = sizeof(pKernelChannel->clientKmb.decryptBundle.key);
                pStr = (const uint8_t *)CC_KEYID_PERCHANNEL_GPU_CPU_UPDATE_KEY;
            }

            if (!libspdm_hkdf_sha256_expand(curKeySeed,
                                            sizeof(pConfCompute->channelKeySeed),
                                            (const NvU8 *)pStr,
                                            (NvLength)portStringLength((const char*)pStr),
                                            pKey,
                                            keySize))
            {
                portMemSet(pKey, 0, sizeof(pConfCompute->channelKeySeed));
                return NV_ERR_FATAL_ERROR;
            }

            //
            // IVs are moved to per channel from Blackwell onwards. Hence setting IV to 0.
            // In Blackwell, message counter and channel counter start at 0. Channel
            // counter is used to track the number of IV rotations done.
            //
            pKernelChannel->clientKmb.encryptBundle.iv[2] = 0;
            pKernelChannel->clientKmb.encryptBundle.iv[1] = 0;
            pKernelChannel->clientKmb.encryptBundle.iv[0] = 0;
            pKernelChannel->clientKmb.encryptBundle.ivMask[2] = 0;
            pKernelChannel->clientKmb.encryptBundle.ivMask[1] = 0;
            pKernelChannel->clientKmb.encryptBundle.ivMask[0] = 0;

            pKernelChannel->clientKmb.decryptBundle.iv[2] = 0;
            pKernelChannel->clientKmb.decryptBundle.iv[1] = 0;
            pKernelChannel->clientKmb.decryptBundle.iv[0] = 0;
            pKernelChannel->clientKmb.decryptBundle.ivMask[2] = 0;
            pKernelChannel->clientKmb.decryptBundle.ivMask[1] = 0;
            pKernelChannel->clientKmb.decryptBundle.ivMask[0] = 0;
        }

    }
    else
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    return NV_OK;

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
confComputeKeyStoreRetrieveViaChannel_GB100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    ROTATE_IV_TYPE       rotateOperation,
    NvBool               bIncludeIvOrNonce,
    CC_KMB              *keyMaterialBundle
)
{
    //
    // SEC2 Per Channel key support has not been added yet. Hence SEC2 keys need to be restored from keystore.
    // For CE, keys would be derived per channel.
    // TODO: Remove this check once per-channel key support has been added for SEC2
    //
    if (kchannelGetEngineType(pKernelChannel) == RM_ENGINE_TYPE_SEC2) {
        return confComputeKeyStoreRetrieveViaChannel_GH100(pConfCompute, pKernelChannel, rotateOperation,
                                                           bIncludeIvOrNonce, keyMaterialBundle);
    }
    else if (RM_ENGINE_TYPE_IS_COPY(kchannelGetEngineType(pKernelChannel)))
    {
        if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
        {
            if (bIncludeIvOrNonce)
            {
                keyMaterialBundle->encryptBundle = pKernelChannel->clientKmb.encryptBundle;
            }
            else
            {
                portMemCopy(keyMaterialBundle->encryptBundle.key,
                            sizeof(keyMaterialBundle->encryptBundle.key),
                            pKernelChannel->clientKmb.encryptBundle.key,
                            sizeof(pKernelChannel->clientKmb.encryptBundle.key));
                portMemCopy(keyMaterialBundle->encryptBundle.ivMask,
                            sizeof(keyMaterialBundle->encryptBundle.ivMask),
                            pKernelChannel->clientKmb.encryptBundle.ivMask,
                            sizeof(pKernelChannel->clientKmb.encryptBundle.ivMask));
            }
        }
        if ((rotateOperation == ROTATE_IV_DECRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
        {
            if (bIncludeIvOrNonce)
            {
                keyMaterialBundle->decryptBundle = pKernelChannel->clientKmb.decryptBundle;
            }
            else
            {
                portMemCopy(keyMaterialBundle->decryptBundle.key,
                            sizeof(keyMaterialBundle->decryptBundle.key),
                            pKernelChannel->clientKmb.decryptBundle.key,
                            sizeof(pKernelChannel->clientKmb.decryptBundle.key));
                portMemCopy(keyMaterialBundle->decryptBundle.ivMask,
                            sizeof(keyMaterialBundle->decryptBundle.ivMask),
                            pKernelChannel->clientKmb.decryptBundle.ivMask,
                            sizeof(pKernelChannel->clientKmb.decryptBundle.ivMask));
            }
            keyMaterialBundle->bIsWorkLaunch = NV_FALSE;
        }
    }
    else
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    return NV_OK;
    return NV_ERR_NOT_SUPPORTED;
}

void *confComputeGetCurrentKeySeed_GB100
(
    ConfidentialCompute *pConfCompute
)
{
    return pConfCompute->channelKeySeed;
}
/*!
 * @brief Derives initial keyseed.
 *
 * @return NV_ERR_INVALID_ARGUMENT if engine is not correct.
 *         NV_OK otherwise.
 */
NV_STATUS
confComputeDeriveInitialKeySeed_GB100
(
     ConfidentialCompute *pConfCompute
)
{
    if (!libspdm_hkdf_sha256_expand(pConfCompute->m_exportMasterKey,
                                sizeof(pConfCompute->m_exportMasterKey),
                                (const uint8_t *)(CC_KEYID_PERCHANNEL_INITIAL_KEYSEED),
                                (size_t)portStringLength(CC_KEYID_PERCHANNEL_INITIAL_KEYSEED),
                                (confComputeGetCurrentKeySeed(pConfCompute)),
                                sizeof(pConfCompute->channelKeySeed)))
    {
        return NV_ERR_FATAL_ERROR;
    }
    return NV_OK;
}
NV_STATUS
confComputeGetAndUpdateCurrentKeySeed_GB100
(
     ConfidentialCompute *pConfCompute,
     NvU8 pKey[]
)
{
    portMemCopy(pKey, sizeof(pConfCompute->channelKeySeed), pConfCompute->channelKeySeed, sizeof(pConfCompute->channelKeySeed));
    portMemSet(pConfCompute->channelKeySeed, 0, sizeof(pConfCompute->channelKeySeed));
    if (!libspdm_hkdf_sha256_expand(pKey,
                                sizeof(pConfCompute->channelKeySeed),
                                (const uint8_t *)(CC_KEYID_PERCHANNEL_UPDATE_KEYSEED),
                                (size_t)portStringLength(CC_KEYID_PERCHANNEL_UPDATE_KEYSEED),
                                (confComputeGetCurrentKeySeed_HAL(pConfCompute)),
                                sizeof(pConfCompute->channelKeySeed)))
    {
        portMemSet(pKey, 0, sizeof(pConfCompute->channelKeySeed));
        return NV_ERR_FATAL_ERROR;
    }
    return NV_OK;
}

