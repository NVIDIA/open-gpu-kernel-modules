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
#include "libraries/nvport/nvport.h"
#include "kernel/gpu/spdm/libspdm_includes.h"
#include "hal/library/cryptlib.h"

//#include "hopper/gh100/dev_se_seb.h"

//
// The keystore holds keys, IV masks, and IVs for the LCE, SEC2, and GSP channels. It owns the channel
// counter for each key and helps prevent IV reuse. The keystore is comprised of key slots. A key
// slot holds either AES (key, IV mask, IV) or HMAC (key, nonce) secrets. Key slots are paired
// so that the even-numbered key slot typically holds the host-to-device secrets and the subsequent
// odd-numbered key slot typically holds the device-to-host secrets. The flow for accessing
// the keystore is:
//
// 1. confComputeKeyStoreInit()
// 2. confComputeKeyStoreGetExportMasterKey()
//      a. Caller copies the Export Master Key into the keystore.
// 4. confComputeKeyStoreDeriveKey() as needed.
//      a. Caller retrieves IV masks from GSP and calls confComputeKeyStoreDepositIvMask() as needed.
// 5. confComputeKeyStoreClearExportMasterKey() once all keys have been derived.
// 6. confComputeKeyStoreRetrieveVia*() as needed.
// 7. confComputeKeyStoreUpdateKey() as needed.
// 8. confComputeKeyStoreDeinit() at the end of the confidential compute session.
//

#define CONCAT64(hi, low) ((((NvU64)(hi) << 32)) | ((NvU64)(low)))

typedef struct cryptoBundle_t
{
    union
    {
        CC_AES_CRYPTOBUNDLE  cryptBundle;
        CC_HMAC_CRYPTOBUNDLE hmacBundle;
    };
    enum {NO_CHAN_COUNTER, CRYPT_COUNTER, HMAC_COUNTER} type;
} cryptoBundle_t;

typedef cryptoBundle_t keySlot_t[CC_KEYSPACE_TOTAL_SIZE];

static NV_STATUS checkSlot(ConfidentialCompute *pConfCompute, NvU32 slotNumber);
static void incrementChannelCounter(ConfidentialCompute *pConfCompute, NvU32 slotNumber);
static NvU64 getChannelCounter(ConfidentialCompute *pConfCompute, NvU32 slotNumber);
static NV_STATUS getKeyIdSec2(KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation,
                              NvU16 *keyId);

NV_STATUS
confComputeKeyStoreInit_GH100(ConfidentialCompute *pConfCompute)
{
    NvU32          index;
    cryptoBundle_t (*pKeyStore)[];

    NV_PRINTF(LEVEL_INFO, "Initializing keystore.\n");

    pConfCompute->m_keySlot = portMemAllocNonPaged((NvLength) sizeof(keySlot_t));

    if (pConfCompute->m_keySlot == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pConfCompute->m_keySlot, 0, (NvLength) sizeof(keySlot_t));

    pKeyStore = pConfCompute->m_keySlot;

    // GSP key slots don't have a channel counter.
    for (index = 0; index < CC_KEYSPACE_GSP_SIZE; index++)
    {
        (*pKeyStore)[index].type = NO_CHAN_COUNTER;
    }

    // SEC2 key slots are a mix of encryption / decryption with channel counter and HMAC.
    ct_assert(CC_KEYSPACE_SEC2_SIZE == 6);

    (*pKeyStore)[index++].type = CRYPT_COUNTER;
    (*pKeyStore)[index++].type = HMAC_COUNTER;
    (*pKeyStore)[index++].type = CRYPT_COUNTER;
    (*pKeyStore)[index++].type = HMAC_COUNTER;
    (*pKeyStore)[index++].type = CRYPT_COUNTER;
    (*pKeyStore)[index++].type = HMAC_COUNTER;

    // The remaining LCE key slots are all encryption / decryption with channel counter.
    for (; index < CC_KEYSPACE_TOTAL_SIZE; index++)
    {
        (*pKeyStore)[index].type = CRYPT_COUNTER;
    }

    return NV_OK;
}

void
confComputeKeyStoreDeinit_GH100(ConfidentialCompute *pConfCompute)
{
    NV_PRINTF(LEVEL_INFO, "Deinitializing keystore.\n");

    if (pConfCompute->m_keySlot != NULL)
    {
        portMemSet(pConfCompute->m_keySlot, 0, (NvLength) sizeof(keySlot_t));
        confComputeKeyStoreClearExportMasterKey_HAL(pConfCompute);
        portMemFree(pConfCompute->m_keySlot);
        pConfCompute->m_keySlot = NULL;
    }
}

void
*confComputeKeyStoreGetExportMasterKey_GH100
(
    ConfidentialCompute *pConfCompute
)
{
    return pConfCompute->m_exportMasterKey;
}

NV_STATUS
confComputeKeyStoreDeriveKey_GH100(ConfidentialCompute *pConfCompute, NvU32 globalKeyId)
{
    NvU32 slotIndex;
    cryptoBundle_t (*pKeyStore)[];
    uint8_t *pKey = NULL;
    size_t keySize = 0;

    pKeyStore = pConfCompute->m_keySlot;
    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, globalKeyId, &slotIndex));
    NV_PRINTF(LEVEL_INFO, "Deriving key for global key ID %x.\n", globalKeyId);

    if ((globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_USER)) ||
        (globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_KERN)) ||
        (globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER)))
    {
        pKey = (uint8_t *)(*pKeyStore)[slotIndex].hmacBundle.key;
        keySize = sizeof((*pKeyStore)[slotIndex].hmacBundle.key);
    }
    else
    {
        pKey = (uint8_t *)(*pKeyStore)[slotIndex].cryptBundle.key;
        keySize = sizeof((*pKeyStore)[slotIndex].cryptBundle.key);
    }

    if (!libspdm_hkdf_sha256_expand(pConfCompute->m_exportMasterKey,
                                sizeof(pConfCompute->m_exportMasterKey),
                                (const uint8_t *)(CC_GKEYID_GET_STR(globalKeyId)),
                                (size_t)portStringLength(CC_GKEYID_GET_STR(globalKeyId)),
                                pKey,
                                keySize))
    {
        return NV_ERR_FATAL_ERROR;
    }

    // LCEs will return an error / interrupt if the key is all 0s.
    if ((CC_GKEYID_GET_KEYSPACE(globalKeyId) >= CC_KEYSPACE_LCE0) &&
        (CC_GKEYID_GET_KEYSPACE(globalKeyId) <= confComputeGetMaxCeKeySpaceIdx(pConfCompute)))
    {
        for (NvU32 index = 0; index < CC_AES_256_GCM_KEY_SIZE_DWORD; index++)
        {
            if ((*pKeyStore)[slotIndex].cryptBundle.key[index] != 0)
            {
                return NV_OK;
            }
        }

        return NV_ERR_FATAL_ERROR;
    }
    return NV_OK;
    return NV_ERR_NOT_SUPPORTED;
}

void
confComputeKeyStoreDepositIvMask_GH100
(
    ConfidentialCompute *pConfCompute,
    NvU32                globalKeyId,
    void                *ivMask
)
{
    NvU32 slotNumber;
    cryptoBundle_t (*pKeyStore)[];

    pKeyStore = pConfCompute->m_keySlot;
    NV_ASSERT_OR_RETURN_VOID(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, globalKeyId, &slotNumber) == NV_OK);
    NV_PRINTF(LEVEL_INFO, "Depositing IV mask for global key ID %x.\n", globalKeyId);

    portMemCopy((*pKeyStore)[slotNumber].cryptBundle.ivMask,
                sizeof((*pKeyStore)[slotNumber].cryptBundle.ivMask),
                ivMask, CC_AES_256_GCM_IV_SIZE_BYTES);
}

void
confComputeKeyStoreClearExportMasterKey_GH100(ConfidentialCompute *pConfCompute)
{
    NV_PRINTF(LEVEL_INFO, "Clearing the Export Master Key.\n");

    portMemSet(pConfCompute->m_exportMasterKey, 0, (NvLength) sizeof(pConfCompute->m_exportMasterKey));
}

NV_STATUS
confComputeKeyStoreRetrieveViaChannel_GH100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    ROTATE_IV_TYPE       rotateOperation,
    NvBool               bIncludeIvOrNonce,
    CC_KMB              *keyMaterialBundle
)
{
    NvU32 globalKeyId;
    NvU16 keyId;

    if (RM_ENGINE_TYPE_IS_COPY(kchannelGetEngineType(pKernelChannel)))
    {
        NvU16 keySpace;

        if (confComputeGetKeySpaceFromKChannel_HAL(pConfCompute, pKernelChannel,
                                                   &keySpace) != NV_OK)
        {
            return NV_ERR_INVALID_PARAMETER;
        }

        if (confComputeGetLceKeyIdFromKChannel_HAL(pConfCompute, pKernelChannel,
                                                   rotateOperation, &keyId) != NV_OK)
        {
            return NV_ERR_INVALID_PARAMETER;
        }

        globalKeyId = CC_GKEYID_GEN(keySpace, keyId);
    }
    else if (kchannelGetEngineType(pKernelChannel) == RM_ENGINE_TYPE_SEC2)
    {
        if (getKeyIdSec2(pKernelChannel, rotateOperation, &keyId) != NV_OK)
        {
            return NV_ERR_INVALID_PARAMETER;
        }

        globalKeyId = CC_GKEYID_GEN(CC_KEYSPACE_SEC2, keyId);
    }
    else
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    return confComputeKeyStoreRetrieveViaKeyId_GH100(pConfCompute, globalKeyId, rotateOperation,
                                                     bIncludeIvOrNonce, keyMaterialBundle);
}

NV_STATUS
confComputeKeyStoreRetrieveViaKeyId_GH100
(
    ConfidentialCompute *pConfCompute,
    NvU32                globalKeyId,
    ROTATE_IV_TYPE       rotateOperation,
    NvBool               bIncludeIvOrNonce,
    CC_KMB              *keyMaterialBundle
)
{
    NvU32          slotNumber;
    cryptoBundle_t (*pKeyStore)[];

    pKeyStore = pConfCompute->m_keySlot;

    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, globalKeyId, &slotNumber));
    NV_PRINTF(LEVEL_INFO, "Retrieving KMB from slot number = %d and type is %d.\n",
              slotNumber, (*pKeyStore)[slotNumber].type);

    if ((slotNumber % 2) == 1)
    {
        slotNumber--;
    }

    if (bIncludeIvOrNonce &&
        ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID)))
    {
        if (checkSlot(pConfCompute, slotNumber) != NV_OK)
        {
            // Right now returns error to caller. Depending on how the re-keying flow is designed
            // this may initiate re-keying.
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    if (bIncludeIvOrNonce &&
        ((rotateOperation == ROTATE_IV_DECRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID) ||
         (rotateOperation == ROTATE_IV_HMAC)))
    {
        if (checkSlot(pConfCompute, slotNumber + 1) != NV_OK)
        {
            // Right now returns error to caller. Depending on how the re-keying flow is designed
            // this may initiate re-keying.
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
    {
        if (bIncludeIvOrNonce)
        {
            incrementChannelCounter(pConfCompute, slotNumber);
        }

        if (bIncludeIvOrNonce)
        {
            keyMaterialBundle->encryptBundle = (*pKeyStore)[slotNumber].cryptBundle;
        }
        else
        {
            portMemCopy(keyMaterialBundle->encryptBundle.key,
                        sizeof(keyMaterialBundle->encryptBundle.key),
                        (*pKeyStore)[slotNumber].cryptBundle.key,
                        sizeof((*pKeyStore)[slotNumber].cryptBundle.key));
            portMemCopy(keyMaterialBundle->encryptBundle.ivMask,
                        sizeof(keyMaterialBundle->encryptBundle.ivMask),
                        (*pKeyStore)[slotNumber].cryptBundle.ivMask,
                        sizeof((*pKeyStore)[slotNumber].cryptBundle.ivMask));
        }
    }

    if ((rotateOperation == ROTATE_IV_DECRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID) ||
        (rotateOperation == ROTATE_IV_HMAC))
    {
        if (bIncludeIvOrNonce)
        {
            incrementChannelCounter(pConfCompute, slotNumber + 1);
        }

        switch ((*pKeyStore)[slotNumber + 1].type)
        {
            case NO_CHAN_COUNTER:
            case CRYPT_COUNTER:
                if (bIncludeIvOrNonce)
                {
                    keyMaterialBundle->decryptBundle = (*pKeyStore)[slotNumber + 1].cryptBundle;
                }
                else
                {
                    portMemCopy(keyMaterialBundle->decryptBundle.key,
                                sizeof(keyMaterialBundle->decryptBundle.key),
                                (*pKeyStore)[slotNumber + 1].cryptBundle.key,
                                sizeof((*pKeyStore)[slotNumber + 1].cryptBundle.key));
                    portMemCopy(keyMaterialBundle->decryptBundle.ivMask,
                                sizeof(keyMaterialBundle->decryptBundle.ivMask),
                                (*pKeyStore)[slotNumber + 1].cryptBundle.ivMask,
                                sizeof((*pKeyStore)[slotNumber + 1].cryptBundle.ivMask));
                }
                keyMaterialBundle->bIsWorkLaunch = NV_FALSE;
                break;
            case HMAC_COUNTER:
                if (bIncludeIvOrNonce)
                {
                    keyMaterialBundle->hmacBundle = (*pKeyStore)[slotNumber + 1].hmacBundle;
                }
                else
                {
                    portMemCopy(keyMaterialBundle->hmacBundle.key,
                                sizeof(keyMaterialBundle->hmacBundle.key),
                                (*pKeyStore)[slotNumber + 1].hmacBundle.key,
                                sizeof((*pKeyStore)[slotNumber + 1].hmacBundle.key));
                }
                keyMaterialBundle->bIsWorkLaunch = NV_TRUE;
                break;
        }
    }

    return NV_OK;
}

NV_STATUS
confComputeKeyStoreUpdateKey_GH100(ConfidentialCompute *pConfCompute, NvU32 globalKeyId)
{
    NvU32           slotIndex;
    cryptoBundle_t (*pKeyStore)[];
    NvU8            tempMem[CC_AES_256_GCM_KEY_SIZE_BYTES];
    NvU8           *pKey;
    NvU32           keySize;

    NV_ASSERT_OK_OR_RETURN(confComputeGetKeySlotFromGlobalKeyId(pConfCompute, globalKeyId, &slotIndex));
    NV_PRINTF(LEVEL_INFO, "Updating key with global key ID %x.\n", globalKeyId);

    pKeyStore = pConfCompute->m_keySlot;

    if ((globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_USER)) ||
        (globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_KERN)) ||
        (globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER)))
    {
        pKey = (uint8_t *)(*pKeyStore)[slotIndex].hmacBundle.key;
        keySize = sizeof((*pKeyStore)[slotIndex].hmacBundle.key);
    }
    else
    {
        pKey = (uint8_t *)(*pKeyStore)[slotIndex].cryptBundle.key;
        keySize = sizeof((*pKeyStore)[slotIndex].cryptBundle.key);
    }

    if (!libspdm_sha256_hash_all((const void *)pKey, keySize, tempMem))
    {
        return NV_ERR_FATAL_ERROR;
    }

    if (!libspdm_hkdf_sha256_expand(tempMem,
                                    sizeof(tempMem),
                                    (const uint8_t *)(CC_GKEYID_GET_STR(globalKeyId)),
                                    (size_t)portStringLength(CC_GKEYID_GET_STR(globalKeyId)),
                                    pKey,
                                    keySize))
    {
        return NV_ERR_FATAL_ERROR;
    }

    portMemSet(tempMem, 0, (NvLength) sizeof(tempMem));

    // LCEs will return an error / interrupt if the key is all 0s.
    if ((CC_GKEYID_GET_KEYSPACE(globalKeyId) >= CC_KEYSPACE_LCE0) &&
        (CC_GKEYID_GET_KEYSPACE(globalKeyId) <= confComputeGetMaxCeKeySpaceIdx(pConfCompute)))
    {
        for (NvU32 index = 0; index < CC_AES_256_GCM_KEY_SIZE_DWORD; index++)
        {
            if ((*pKeyStore)[slotIndex].cryptBundle.key[index] != 0)
            {
                return NV_OK;
            }
        }

        return NV_ERR_FATAL_ERROR;
    }

    return NV_OK;
}

/*!
 * Get key pair from channel
 *
 * @param[in]   pGpu            : OBJGPU pointer
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[in]   pKernelChannel  : KernelChannel pointer
 * @param[out]  pH2DKey         : pointer to h2d key
 * @param[out]  pD2HKey         : pointer to d2h key
 */
NV_STATUS
confComputeGetKeyPairByChannel_GH100
(
    OBJGPU *pGpu,
    ConfidentialCompute *pConfCompute,
    KernelChannel *pKernelChannel,
    NvU32 *pH2DKey,
    NvU32 *pD2HKey
)
{
    NvU16 keySpace = 0;
    NvU16 lh2dKeyId = 0;
    NvU16 ld2hKeyId = 0;
    RM_ENGINE_TYPE engineType = kchannelGetEngineType(pKernelChannel);
    if (engineType == RM_ENGINE_TYPE_SEC2)
    {
        keySpace = CC_KEYSPACE_SEC2;
        NV_ASSERT_OK_OR_RETURN(getKeyIdSec2(pKernelChannel, ROTATE_IV_ENCRYPT, &lh2dKeyId));
        NV_ASSERT_OK_OR_RETURN(getKeyIdSec2(pKernelChannel, ROTATE_IV_HMAC, &ld2hKeyId));
    }
    else
    {
        NV_ASSERT_OK_OR_RETURN(confComputeGetKeySpaceFromKChannel_HAL(pConfCompute,
                                    pKernelChannel, &keySpace) != NV_OK);
        NV_ASSERT_OK_OR_RETURN(confComputeGetLceKeyIdFromKChannel_HAL(pConfCompute, pKernelChannel,
                                    ROTATE_IV_ENCRYPT, &lh2dKeyId));
        NV_ASSERT_OK_OR_RETURN(confComputeGetLceKeyIdFromKChannel_HAL(pConfCompute, pKernelChannel,
                                    ROTATE_IV_DECRYPT, &ld2hKeyId));
    }

    if (pH2DKey != NULL)
    {
        *pH2DKey = CC_GKEYID_GEN(keySpace, lh2dKeyId);
    }
    if (pD2HKey != NULL)
    {
        *pD2HKey = CC_GKEYID_GEN(keySpace, ld2hKeyId);
    }
    return NV_OK;
}

NvBool
confComputeKeyStoreIsValidGlobalKeyId_GH100
(
    ConfidentialCompute *pConfCompute,
    NvU32                globalKeyId
)
{
    const char *globalKeyIdString = CC_GKEYID_GET_STR(globalKeyId);

    return (globalKeyIdString != NULL);
}

/*!
 * Return the key ID for a given LCE channel and rotation operation.
 * If rotateOperation is ROTATE_IV_ALL_VALID then it will return the least
 * key ID of the key pair; ie the one that corresponds to an even numbered slot.
 *
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[in]   pKernelChannel  : KernelChannel pointer
 * @param[in]   rotateOperation : The type of rotation operation
 * @param[out]  pKeyId          : pointer to keyId
 */
NV_STATUS
confComputeGetLceKeyIdFromKChannel_GH100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    ROTATE_IV_TYPE       rotateOperation,
    NvU16               *pKeyId
)
{
    if (kchannelCheckIsUserMode(pKernelChannel))
    {
        if ((rotateOperation == ROTATE_IV_ENCRYPT) ||
            (rotateOperation == ROTATE_IV_ALL_VALID))
        {
            *pKeyId = CC_LKEYID_LCE_H2D_USER;
        }
        else
        {
            *pKeyId = CC_LKEYID_LCE_D2H_USER;
        }

        return NV_OK;
    }
    else if (kchannelCheckIsKernel(pKernelChannel))
    {
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

    return NV_ERR_GENERIC;
}

//
// Return the key ID for a given SEC2 channel and rotation operation.
// If rotateOperation is ROTATE_IV_ALL_VALID then it will return the least
// key ID of the key pair; ie the one that corresponds to an even numbered slot.
//
static NV_STATUS
getKeyIdSec2
(
    KernelChannel  *pKernelChannel,
    ROTATE_IV_TYPE  rotateOperation,
    NvU16          *keyId
)
{
    if (kchannelCheckIsUserMode(pKernelChannel))
    {
        if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
        {
            *keyId = CC_LKEYID_CPU_SEC2_DATA_USER;
        }
        else
        {
            *keyId = CC_LKEYID_CPU_SEC2_HMAC_USER;
        }

        return NV_OK;
    }
    else if (kchannelCheckIsKernel(pKernelChannel))
    {
        if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
        {
            if (pKernelChannel->bUseScrubKey)
                *keyId = CC_LKEYID_CPU_SEC2_DATA_SCRUBBER;
            else
                *keyId = CC_LKEYID_CPU_SEC2_DATA_KERN;
        }
        else
        {
            if (pKernelChannel->bUseScrubKey)
                *keyId = CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER;
            else
                *keyId = CC_LKEYID_CPU_SEC2_HMAC_KERN;
        }
        return NV_OK;
    }

    return NV_ERR_GENERIC;
}

/*!
 * Returns a key space corresponding to a channel
 *
 * @param[in]   pConfCompute               : ConfidentialCompute pointer
 * @param[in]   pKernelChannel             : KernelChannel pointer
 * @param[out]  keySpace                   : value of keyspace from cc_keystore.h
 *
 */
NV_STATUS
confComputeGetKeySpaceFromKChannel_GH100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    NvU16               *keyspace
)
{
    // The actual copy engine (2 through 9) is normalized to start at 0.
    switch (kchannelGetEngineType(pKernelChannel))
    {
        case RM_ENGINE_TYPE_COPY2:
            *keyspace = CC_KEYSPACE_LCE0;
            break;
        case RM_ENGINE_TYPE_COPY3:
            *keyspace = CC_KEYSPACE_LCE1;
            break;
        case RM_ENGINE_TYPE_COPY4:
            *keyspace = CC_KEYSPACE_LCE2;
            break;
        case RM_ENGINE_TYPE_COPY5:
            *keyspace = CC_KEYSPACE_LCE3;
            break;
        case RM_ENGINE_TYPE_COPY6:
            *keyspace = CC_KEYSPACE_LCE4;
            break;
        case RM_ENGINE_TYPE_COPY7:
            *keyspace = CC_KEYSPACE_LCE5;
            break;
        case RM_ENGINE_TYPE_COPY8:
            *keyspace = CC_KEYSPACE_LCE6;
            break;
        case RM_ENGINE_TYPE_COPY9:
            *keyspace = CC_KEYSPACE_LCE7;
            break;
        default:
            return NV_ERR_GENERIC;
    }

    return NV_OK;
}

static NV_STATUS
checkSlot
(
    ConfidentialCompute *pConfCompute,
    NvU32                slotNumber
)
{
    if (getChannelCounter(pConfCompute, slotNumber) == NV_U64_MAX)
    {
        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

static void
incrementChannelCounter
(
    ConfidentialCompute *pConfCompute,
    NvU32                slotNumber
)
{
    NvU64          channelCounter = getChannelCounter(pConfCompute, slotNumber);
    cryptoBundle_t (*pKeyStore)[];

    pKeyStore = pConfCompute->m_keySlot;

    channelCounter++;

    switch ((*pKeyStore)[slotNumber].type)
    {
        case NO_CHAN_COUNTER:
            break;
        case CRYPT_COUNTER:
            (*pKeyStore)[slotNumber].cryptBundle.iv[2] = NvU64_HI32(channelCounter);
            (*pKeyStore)[slotNumber].cryptBundle.iv[1] = NvU64_LO32(channelCounter);
            break;
        case HMAC_COUNTER:
            (*pKeyStore)[slotNumber].hmacBundle.nonce[7] = NvU64_HI32(channelCounter);
            (*pKeyStore)[slotNumber].hmacBundle.nonce[6] = NvU64_LO32(channelCounter);
            break;
    }
}

static NvU64
getChannelCounter
(
    ConfidentialCompute *pConfCompute,
    NvU32                slotNumber
)
{
    cryptoBundle_t (*pKeyStore)[];

    pKeyStore = pConfCompute->m_keySlot;

    switch ((*pKeyStore)[slotNumber].type)
    {
        case NO_CHAN_COUNTER:
            return 0;
        case CRYPT_COUNTER:
            return CONCAT64((*pKeyStore)[slotNumber].cryptBundle.iv[2],
                            (*pKeyStore)[slotNumber].cryptBundle.iv[1]);
        case HMAC_COUNTER:
            return CONCAT64((*pKeyStore)[slotNumber].hmacBundle.nonce[7],
                            (*pKeyStore)[slotNumber].hmacBundle.nonce[6]);
        default:
            NV_ASSERT_OR_RETURN(NV_FALSE, 0);
    }
}
