/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *      Keystore Object Module                                              *
 *                                                                          *
 \**************************************************************************/

#define NVOC_KEYSTORE_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/keystore/keystore.h"
#include "kernel/gpu/spdm/libspdm_includes.h"
#include "hal/library/cryptlib.h"

/*!
 * @brief Allocates a new key in the keystore.
 *
 * @param[in] pKeystore The keystore to allocate the key in.
 * @param[in] keyId     The ID of the key to allocate.
 *
 * @return NV_OK if the key was allocated successfully, errors otherwise.
 */
NV_STATUS keystoreAllocKey_IMPL(Keystore *pKeystore, NvU32 keyId)
{
    if (pKeystore == NULL || keyId >= KEYSTORE_MAX_KEYS || pKeystore->m_keys[keyId] != NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pKeystore->m_keys[keyId] = (KEYSTORE_KEY *)portMemAllocNonPaged(sizeof(KEYSTORE_KEY));
    if (pKeystore->m_keys[keyId] == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pKeystore->m_keys[keyId], 0, sizeof(KEYSTORE_KEY));
    return NV_OK;
}

/*!
 * @brief Gets a key from the keystore or allocates a new one if it doesn't exist.
 *
 * @param[in]  pKeystore The keystore to get the key from.
 * @param[in]  keyId     The ID of the key to get.
 * @param[out] ppKey     The pointer to the key.
 * @param[out] pIsNewKey Whether the key was newly allocated.
 *
 * @return NV_OK if the key was found or allocated successfully, errors otherwise.
 */
NV_STATUS keystoreGetOrAllocKey_IMPL(Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey, NvBool *pIsNewKey)
{
    NV_STATUS status = NV_OK;
    if (pKeystore == NULL || keyId >= KEYSTORE_MAX_KEYS || pIsNewKey == NULL || ppKey == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pKeystore->m_keys[keyId] == NULL)
    {
        *pIsNewKey = NV_TRUE;
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, keystoreAllocKey(pKeystore, keyId), ErrorExit);
    }
    else
    {
        *pIsNewKey = NV_FALSE;
    }

    *ppKey = pKeystore->m_keys[keyId];
    return NV_OK;

ErrorExit:
    if (*pIsNewKey)
    {
        keystoreFreeKey(pKeystore, keyId);
    }
    *ppKey = NULL;
    return status;
}

/*!
 * @brief Gets a key from the keystore.
 *
 * @param[in]  pKeystore The keystore to get the key from.
 * @param[in]  keyId     The ID of the key to get.
 * @param[out] ppKey     The pointer to the key.
 *
 * @return NV_OK if the key was found successfully, errors otherwise.
 */
NV_STATUS keystoreGetKey_IMPL(Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey)
{
    if (pKeystore == NULL || keyId >= KEYSTORE_MAX_KEYS || ppKey == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pKeystore->m_keys[keyId] == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    *ppKey = pKeystore->m_keys[keyId];
    return NV_OK;
}

/*!
 * @brief Frees a key from the keystore.
 *
 * @param[in] pKeystore The keystore to free the key from.
 * @param[in] keyId     The ID of the key to free.
 *
 * @return void
 */
void keystoreFreeKey_IMPL(Keystore *pKeystore, NvU32 keyId)
{
    if (pKeystore == NULL || keyId >= KEYSTORE_MAX_KEYS)
    {
        return;
    }

    if (pKeystore->m_keys[keyId] != NULL)
    {
        portMemSet(pKeystore->m_keys[keyId], 0, sizeof(KEYSTORE_KEY));
        portMemFree(pKeystore->m_keys[keyId]);
        pKeystore->m_keys[keyId] = NULL;
    }
}

/*!
 * @brief The constructor for the keystore.
 */
NV_STATUS
keystoreConstruct_IMPL(Keystore *pKeystore)
{
    portMemSet(pKeystore->m_keys, 0, sizeof(pKeystore->m_keys));
    return NV_OK;
}

/*!
 * @brief The destructor for the keystore.
 */
void keystoreDestruct_IMPL(Keystore *pKeystore)
{
    for (NvU32 i = 0; i < KEYSTORE_MAX_KEYS; i++)
    {
        keystoreFreeKey(pKeystore, i);
    }
}

/*!
 * @brief Sets the master key for the keystore.
 *
 * @param[in] pKeystore    The keystore object.
 * @param[in] keyId        The ID of the key to set.
 * @param[in] pKey         The key to set.
 * @param[in] keySizeBytes The size of the key.
 * @param[in] usage        The usage of the master key.
 *
 * @return NV_OK if the master key was set successfully, errors otherwise.
 */
NV_STATUS keystoreSetMasterKey_IMPL(Keystore *pKeystore, NvU32 keyId, NvU8 *pKey, NvLength keySizeBytes, KEY_USAGE usage)
{
    NV_STATUS     status     = NV_OK;
    KEYSTORE_KEY *pMasterKey = NULL;
    NvBool        bNewKey    = NV_FALSE;

    if (pKeystore == NULL)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    if (pKeystore->m_bInitialized)
    {
        status = NV_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    // If the first key is not HKDF, we can't derive future keys from it.
    if (!KEY_USAGE_IS_HKDF(usage))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    if (pKey == NULL || keySizeBytes != KEYSTORE_USAGE_TO_KEY_SIZE_BYTES(usage))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, keystoreGetOrAllocKey(pKeystore, keyId, &pMasterKey, &bNewKey), ErrorExit);

    pMasterKey->usage = usage;

    switch (usage)
    {
        case KEYSTORE_USAGE_HKDF_SHA256:
            portMemCopy(pMasterKey->key, keySizeBytes, pKey, keySizeBytes);
            pMasterKey->keySizeBytes = keySizeBytes;
            break;
        default:
            status = NV_ERR_INVALID_ARGUMENT;
            goto ErrorExit;
    }

    pKeystore->m_bInitialized = NV_TRUE;

    return status;

ErrorExit:
    if (bNewKey)
    {
        keystoreFreeKey(pKeystore, keyId);
    }
    return status;
}

/*!
 * @brief Derives a key from an HKDF-SHA256 key.
 *
 * @param[in]  pSrcKey        The source key.
 * @param[out] pDstKey        The destination key.
 * @param[in]  pInfo          The info to derive the key from.
 * @param[in]  infoSizeBytes  The size of the info.
 *
 * @return NV_OK if the key was derived successfully, errors otherwise.
 */
static NV_STATUS _keystoreDeriveKeyHkdfSha256(KEYSTORE_KEY *pSrcKey, KEYSTORE_KEY *pDstKey,
                                              const void *pInfo, NvLength infoSizeBytes)
{
    if (pSrcKey == NULL || pDstKey == NULL || pInfo == NULL || infoSizeBytes == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pSrcKey->usage != KEYSTORE_USAGE_HKDF_SHA256)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!libspdm_hkdf_sha256_expand(pSrcKey->key, pSrcKey->keySizeBytes, pInfo, infoSizeBytes, pDstKey->key, pDstKey->keySizeBytes))
    {
        return NV_ERR_FATAL_ERROR;
    }

    return NV_OK;
}

/*!
 * @brief Derives a key from an existing key within the store.
 *
 * @param[in] pKeystore     The keystore object.
 * @param[in] srcKeyId      The ID of the source key.
 * @param[in] dstKeyId      The ID of the destination key.
 * @param[in] pInfo         The info to derive the key from.
 * @param[in] infoSizeBytes The size of the info.
 * @param[in] usage         The usage of the derived key.
 *
 * @return NV_OK if the key was derived successfully, errors otherwise.
 */
NV_STATUS keystoreDeriveKey_IMPL(Keystore *pKeystore, NvU32 srcKeyId, NvU32 dstKeyId, const void *pInfo, NvLength infoSizeBytes, KEY_USAGE usage)
{
    NV_STATUS     status  = NV_OK;
    NvBool        bNewKey = NV_FALSE;

    KEYSTORE_KEY *pSrcKey = NULL;
    KEYSTORE_KEY *pDstKey = NULL;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, keystoreGetKey(pKeystore, srcKeyId, &pSrcKey), ErrorExit);

    if (KEYSTORE_USAGE_TO_KEY_SIZE_BYTES(usage) != KEYSTORE_USAGE_TO_KEY_SIZE_BYTES(pSrcKey->usage))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, keystoreGetOrAllocKey(pKeystore, dstKeyId, &pDstKey, &bNewKey), ErrorExit);
    pDstKey->usage = usage;
    pDstKey->keySizeBytes = KEYSTORE_USAGE_TO_KEY_SIZE_BYTES(usage);

    switch (pSrcKey->usage)
    {
        case KEYSTORE_USAGE_HKDF_SHA256:
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _keystoreDeriveKeyHkdfSha256(pSrcKey, pDstKey, pInfo, infoSizeBytes), ErrorExit);
            break;
        default:
            status = NV_ERR_INVALID_ARGUMENT;
            goto ErrorExit;
    }

    return status;

ErrorExit:
    if (bNewKey)
    {
        keystoreFreeKey(pKeystore, dstKeyId);
    }
    return status;
}

/*!
 * @brief Sets data for a key within the store.
 *
 * @param[in] pKeystore     The keystore object.
 * @param[in] keyId         The ID of the key to set the data for.
 * @param[in] pData         The data to set.
 * @param[in] dataSizeBytes The size of the data.
 *
 * @return NV_OK if the data was set successfully, errors otherwise.
 */
NV_STATUS keystoreSetData_IMPL(Keystore *pKeystore, NvU32 keyId, const void *pData, NvLength dataSizeBytes)
{
    NV_STATUS     status                = NV_OK;
    KEYSTORE_KEY *pKey                  = NULL;
    NvLength      expectedDataSizeBytes = 0;
    void         *pDstData              = NULL;

    if (pData == NULL || dataSizeBytes == 0)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, keystoreGetKey(pKeystore, keyId, &pKey), ErrorExit);

    switch (pKey->usage)
    {
        case KEYSTORE_USAGE_AES_256_GCM:
            expectedDataSizeBytes = sizeof(pKey->data.aes256gcm);
            pDstData = &pKey->data.aes256gcm;
            break;
        case KEYSTORE_USAGE_HMAC_SHA256:
            expectedDataSizeBytes = sizeof(pKey->data.hmacsha256.nonce);
            pDstData = &pKey->data.hmacsha256.nonce;
            break;
        default:
            status = NV_ERR_INVALID_ARGUMENT;
            goto ErrorExit;
    }

    if (dataSizeBytes != expectedDataSizeBytes)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    portMemCopy(pDstData, expectedDataSizeBytes, pData, dataSizeBytes);

ErrorExit:
    return status;
}
