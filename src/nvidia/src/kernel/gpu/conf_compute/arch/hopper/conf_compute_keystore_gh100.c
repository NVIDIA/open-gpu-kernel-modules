/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
static NV_STATUS getKeyIdLce(KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation,
                             NvU16 *keyId);
static NV_STATUS getKeyIdSec2(KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation,
                              NvU16 *keyId);
static NV_STATUS getKeyspaceLce(KernelChannel *pKernelChannel, NvU16 *keyspace);
static NvU32 getKeySlotFromGlobalKeyId (NvU32 globalKeyId);
static NvU32 getKeyspaceSize(NvU16 keyspace);

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
    ct_assert(CC_KEYSPACE_SEC2_SIZE == 4);

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
    const NvU32    slotIndex = getKeySlotFromGlobalKeyId(globalKeyId);
    cryptoBundle_t (*pKeyStore)[];

    pKeyStore = pConfCompute->m_keySlot;

    NV_PRINTF(LEVEL_INFO, "Deriving key for global key ID %x.\n", globalKeyId);

    // SEC2 HMAC keys are not generated from the EMK but from the encryption/decryption key.
    if ((globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_USER)) ||
        (globalKeyId == CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_KERN)))
    {
        NvU32 sourceSlotIndex = 0;

        switch (CC_GKEYID_GET_LKEYID(globalKeyId))
        {
            case CC_LKEYID_CPU_SEC2_HMAC_USER:
                sourceSlotIndex = getKeySlotFromGlobalKeyId(
                    CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_USER));
                break;
            case CC_LKEYID_CPU_SEC2_HMAC_KERN:
                sourceSlotIndex = getKeySlotFromGlobalKeyId(
                    CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_KERN));
                break;
        }

        if (!libspdm_sha256_hash_all((const void *)(*pKeyStore)[sourceSlotIndex].cryptBundle.key,
                                     sizeof((*pKeyStore)[sourceSlotIndex].cryptBundle.key),
                                     (uint8_t *)(*pKeyStore)[slotIndex].hmacBundle.key))
        {
            return NV_ERR_FATAL_ERROR;
        }
    }
    else
    {
        if (!libspdm_hkdf_sha256_expand(pConfCompute->m_exportMasterKey,
                                        sizeof(pConfCompute->m_exportMasterKey),
                                        (const uint8_t *)(CC_GKEYID_GET_STR(globalKeyId)),
                                        (size_t)portStringLength(CC_GKEYID_GET_STR(globalKeyId)),
                                        (uint8_t *)(*pKeyStore)[slotIndex].cryptBundle.key,
                                        sizeof((*pKeyStore)[slotIndex].cryptBundle.key)))
        {
            return NV_ERR_FATAL_ERROR;
        }
    }

    // LCEs will return an error / interrupt if the key is all 0s.
    if ((CC_GKEYID_GET_KEYSPACE(globalKeyId) >= CC_KEYSPACE_LCE0) &&
        (CC_GKEYID_GET_KEYSPACE(globalKeyId) <= CC_KEYSPACE_LCE7))
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
    NvU32 slotNumber = getKeySlotFromGlobalKeyId(globalKeyId);
    cryptoBundle_t (*pKeyStore)[];

    pKeyStore = pConfCompute->m_keySlot;

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
    NvBool               includeSecrets,
    CC_KMB              *keyMaterialBundle
)
{
    NvU32 globalKeyId;
    NvU16 keyId;

    if (RM_ENGINE_TYPE_IS_COPY(kchannelGetEngineType(pKernelChannel)))
    {
        NvU16 keyspace;

        if (getKeyspaceLce(pKernelChannel, &keyspace) != NV_OK)
        {
            return NV_ERR_INVALID_PARAMETER;
        }

        if (getKeyIdLce(pKernelChannel, rotateOperation, &keyId) != NV_OK)
        {
            return NV_ERR_INVALID_PARAMETER;
        }

        globalKeyId = CC_GKEYID_GEN(keyspace, keyId);
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
                                                     includeSecrets, keyMaterialBundle);
}

NV_STATUS
confComputeKeyStoreRetrieveViaKeyId_GH100
(
    ConfidentialCompute *pConfCompute,
    NvU32                globalKeyId,
    ROTATE_IV_TYPE       rotateOperation,
    NvBool               includeSecrets,
    CC_KMB              *keyMaterialBundle
)
{
    NvU32          slotNumber = getKeySlotFromGlobalKeyId(globalKeyId);
    cryptoBundle_t (*pKeyStore)[];

    pKeyStore = pConfCompute->m_keySlot;

    NV_PRINTF(LEVEL_INFO, "Retrieving KMB from slot number = %d and type is %d.\n",
              slotNumber, (*pKeyStore)[slotNumber].type);

    if ((slotNumber % 2) == 1)
    {
        slotNumber--;
    }

    if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
    {
        if (checkSlot(pConfCompute, slotNumber) != NV_OK)
        {
            // Right now returns error to caller. Depending on how the re-keying flow is designed
            // this may initiate re-keying.
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    if ((rotateOperation == ROTATE_IV_DECRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID) ||
        (rotateOperation == ROTATE_IV_HMAC))
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
        incrementChannelCounter(pConfCompute, slotNumber);

        if (includeSecrets)
        {
            keyMaterialBundle->encryptBundle = (*pKeyStore)[slotNumber].cryptBundle;
        }
        else
        {
            portMemCopy(keyMaterialBundle->encryptBundle.iv, sizeof(keyMaterialBundle->encryptBundle.iv),
                        (*pKeyStore)[slotNumber].cryptBundle.iv, CC_AES_256_GCM_IV_SIZE_BYTES);
        }
    }

    if ((rotateOperation == ROTATE_IV_DECRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID) ||
        (rotateOperation == ROTATE_IV_HMAC))
    {
        incrementChannelCounter(pConfCompute, slotNumber + 1);

        switch ((*pKeyStore)[slotNumber + 1].type)
        {
            case NO_CHAN_COUNTER:
            case CRYPT_COUNTER:
                if (includeSecrets)
                {
                    keyMaterialBundle->decryptBundle = (*pKeyStore)[slotNumber + 1].cryptBundle;
                }
                else
                {
                    portMemCopy(keyMaterialBundle->decryptBundle.iv, sizeof(keyMaterialBundle->decryptBundle.iv),
                                (*pKeyStore)[slotNumber].cryptBundle.iv, CC_AES_256_GCM_IV_SIZE_BYTES);
                }
                keyMaterialBundle->bIsWorkLaunch = NV_FALSE;
                break;
            case HMAC_COUNTER:
                if (includeSecrets)
                {
                    keyMaterialBundle->hmacBundle = (*pKeyStore)[slotNumber + 1].hmacBundle;
                }
                else
                {
                    portMemCopy(keyMaterialBundle->hmacBundle.nonce, sizeof(keyMaterialBundle->hmacBundle.nonce),
                                (*pKeyStore)[slotNumber].hmacBundle.nonce, CC_HMAC_NONCE_SIZE_BYTES);
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
    return NV_ERR_NOT_SUPPORTED;
}

//
// Return the key ID for a given LCE channel and rotation operation.
// If rotateOperation is ROTATE_IV_ALL_VALID then it will return the least
// key ID of the key pair; ie the one that corresponds to an even numbered slot.
//
static NV_STATUS
getKeyIdLce
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
            *keyId = CC_LKEYID_LCE_H2D_USER;
        }
        else
        {
            *keyId = CC_LKEYID_LCE_D2H_USER;
        }

        return NV_OK;
    }
    else if (kchannelCheckIsKernel(pKernelChannel))
    {
        if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
        {
            *keyId = CC_LKEYID_LCE_H2D_KERN;
        }
        else
        {
            *keyId = CC_LKEYID_LCE_D2H_KERN;
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
            *keyId = CC_LKEYID_CPU_SEC2_DATA_KERN;
        }
        else
        {
            *keyId = CC_LKEYID_CPU_SEC2_HMAC_KERN;
        }

        return NV_OK;
    }

    return NV_ERR_GENERIC;
}

static NV_STATUS
getKeyspaceLce
(
    KernelChannel *pKernelChannel,
    NvU16         *keyspace
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

static NvU32
getKeySlotFromGlobalKeyId
(
    NvU32 globalKeyId
)
{
    NvU16 keyspace = CC_GKEYID_GET_KEYSPACE(globalKeyId);
    NvU32 keySlotIndex = 0;

    for (NvU16 index = 0; index < CC_KEYSPACE_SIZE; index++)
    {
        if (index == keyspace)
        {
            break;
        }
        else
        {
            keySlotIndex += getKeyspaceSize(index);
        }
    }

    return keySlotIndex + CC_GKEYID_GET_LKEYID(globalKeyId);
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

static NvU32
getKeyspaceSize
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
