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

#define NVOC_CCSL_H_PRIVATE_ACCESS_ALLOWED

#include "core/prelude.h"
#include "core/locks.h"
#include "rmconfig.h"
#include "kernel/gpu/conf_compute/ccsl.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "nvport/nvport.h"
#include "rmapi/rmapi.h"
#include "ctrl/ctrlc56f.h"
#include <stddef.h>
// This guard is here until we fix CONF_COMPUTE and SPDM guards across whole RM
#include "kernel/gpu/spdm/libspdm_includes.h"
#include <hal/library/cryptlib.h>
#include <nvspdm_cryptlib_extensions.h>

#include "cc_drv.h"

#define MAX_DECRYPT_BUNDLES 16

static void
ccslSplit32(NvU8 *dst, NvU32 num)
{
    dst[3] = (NvU8) (num >> 24);
    dst[2] = (NvU8) (num >> 16);
    dst[1] = (NvU8) (num >> 8);
    dst[0] = (NvU8) (num);
}

static void
ccslSplit64(NvU8 *dst, NvU64 num)
{
    dst[7] = (NvU8) (num >> 56);
    dst[6] = (NvU8) (num >> 48);
    dst[5] = (NvU8) (num >> 40);
    dst[4] = (NvU8) (num >> 32);
    dst[3] = (NvU8) (num >> 24);
    dst[2] = (NvU8) (num >> 16);
    dst[1] = (NvU8) (num >> 8);
    dst[0] = (NvU8) (num);
}

static NvBool
isChannel
(
    pCcslContext pCtx
)
{
    return (pCtx->msgCounterSize == CSL_MSG_CTR_32) ? NV_TRUE : NV_FALSE;
}

static void
writeKmbToContext
(
    pCcslContext    pCtx,
    CC_KMB         *kmb,
    ROTATE_IV_TYPE  rotateOperation
)
{
    if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
    {
        for (NvU32 index = 0; index < CC_AES_256_GCM_KEY_SIZE_DWORD; index++)
        {
            ccslSplit32(pCtx->keyOut + 4 * index, kmb->encryptBundle.key[index]);
        }

        for (NvU32 index = 0; index < CC_AES_256_GCM_IV_SIZE_DWORD; index++)
        {
            ccslSplit32(pCtx->ivOut + 4 * index, kmb->encryptBundle.iv[index]);
            ccslSplit32(pCtx->ivMaskOut + 4 * index, kmb->encryptBundle.ivMask[index]);
        }
    }

    if ((rotateOperation == ROTATE_IV_DECRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID) ||
        (rotateOperation == ROTATE_IV_HMAC))
    {
        if (kmb->bIsWorkLaunch)
        {
            for (NvU32 index = 0; index < CC_AES_256_GCM_KEY_SIZE_DWORD; index++)
            {
                ccslSplit32(pCtx->keyIn + 4 * index, kmb->hmacBundle.key[index]);
            }

            for (NvU32 index = 0; index < CC_HMAC_NONCE_SIZE_DWORD; index++)
            {
                ccslSplit32(pCtx->nonce + 4 * index, kmb->hmacBundle.nonce[index]);
            }
        }
        else
        {
            for (NvU32 index = 0; index < CC_AES_256_GCM_KEY_SIZE_DWORD; index++)
            {
                ccslSplit32(pCtx->keyIn + 4 * index, kmb->decryptBundle.key[index]);

                if (isChannel(pCtx))
                {
                    ccslSplit32(pCtx->pDecryptBundles[pCtx->currDecryptBundle].keyIn + 4 * index, kmb->decryptBundle.key[index]);
                }
            }

            for (NvU32 index = 0; index < CC_AES_256_GCM_IV_SIZE_DWORD; index++)
            {
                ccslSplit32(pCtx->ivMaskIn + 4 * index, kmb->decryptBundle.ivMask[index]);

                if (isChannel(pCtx))
                {
                    ccslSplit32(pCtx->pDecryptBundles[pCtx->currDecryptBundle].ivMaskIn + 4 * index, kmb->decryptBundle.ivMask[index]);
                }

                ccslSplit32(pCtx->ivIn + 4 * index, kmb->decryptBundle.iv[index]);
            }
        }
    }
}

static NV_STATUS
ccslUpdateViaKeyId
(
    ConfidentialCompute *pConfCompute,
    pCcslContext         pCtx,
    NvU32                globalKeyId
)
{
    NV_STATUS       status;
    CC_KMB          kmb;
    OBJSYS         *pSys            = SYS_GET_INSTANCE();
    OBJGPUMGR      *pGpuMgr         = SYS_GET_GPUMGR(pSys);
    ROTATE_IV_TYPE  rotateOperation = ROTATE_IV_ALL_VALID;
    NvU32           d2hGkeyId       = 0;
    NvU32           h2dGkeyId       = 0;

    // This function must be redesigned for multi-gpu
    if (!pGpuMgr->ccCaps.bHccFeatureCapable)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pCtx == NULL || pConfCompute == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!confComputeKeyStoreIsValidGlobalKeyId(pConfCompute, globalKeyId))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Determine whether the global key ID passed in is an encrypt key or a decrypt key.
    confComputeGetKeyPairByKey(pConfCompute, globalKeyId, &h2dGkeyId, &d2hGkeyId);
    rotateOperation = (globalKeyId == h2dGkeyId) ? ROTATE_IV_ENCRYPT : ROTATE_IV_DECRYPT;

    status = confComputeKeyStoreRetrieveViaKeyId_HAL(pConfCompute,
                                                     globalKeyId,
                                                     rotateOperation,
                                                     NV_TRUE,
                                                     &kmb);
    if (status != NV_OK)
    {
        return status;
    }

    writeKmbToContext(pCtx, &kmb, rotateOperation);
    portMemSet(&kmb, 0, sizeof(kmb));

    return NV_OK;
}

static NV_STATUS
ccslRotationChecksEncrypt
(
    ConfidentialCompute *pConfCompute,
    pCcslContext         pCtx,
    NvU32                bufferSize
)
{
    NV_STATUS status = NV_OK;

    if (pConfCompute == NULL || pCtx == NULL || pCtx->pEncStatsBuffer == NULL ||
        !confComputeKeyStoreIsValidGlobalKeyId(pConfCompute, pCtx->globalKeyIdOut))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Ensure additions won't overflow the counters
    if ((pCtx->pEncStatsBuffer->bytesEncryptedH2D > (pCtx->pEncStatsBuffer->bytesEncryptedH2D + bufferSize)) ||
        (pCtx->pEncStatsBuffer->numEncryptionsH2D > (pCtx->pEncStatsBuffer->numEncryptionsH2D + 1)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pCtx->pEncStatsBuffer->bytesEncryptedH2D += bufferSize;
    pCtx->pEncStatsBuffer->numEncryptionsH2D += 1;

    // Check if we must trigger key rotation
    if (confComputeIsGivenThresholdCrossed((void *)pCtx->pEncStatsBuffer,
                                           pConfCompute->keyRotationInternalThreshold,
                                           NV_TRUE))
    {
        NV_PRINTF(LEVEL_INFO, "Triggering key rotation for global key id 0x%x\n", pCtx->globalKeyIdOut);
        NV_PRINTF(LEVEL_INFO, "Total bytes encrypted 0x%llx total encrypt ops 0x%llx\n",
                  pCtx->pEncStatsBuffer->bytesEncryptedH2D - bufferSize,
                  pCtx->pEncStatsBuffer->numEncryptionsH2D - 1);

        status = confComputeKeyStoreUpdateKey(pConfCompute, pCtx->globalKeyIdOut);
        if (status != NV_OK)
        {
            return status;
        }

        // Now we need to update the CCSL context
        status = ccslUpdateViaKeyId(pConfCompute, pCtx, pCtx->globalKeyIdOut);
        if (status != NV_OK)
        {
            return status;
        }

        // The aggregate stats must be reset as the keys have been rotated
        pCtx->pEncStatsBuffer->bytesEncryptedH2D = bufferSize;
        pCtx->pEncStatsBuffer->numEncryptionsH2D = 1;
    }

    return status;
}

static NV_STATUS
ccslRotationChecksDecrypt
(
    ConfidentialCompute *pConfCompute,
    pCcslContext         pCtx,
    NvU32                bufferSize
)
{
    NV_STATUS status = NV_OK;

    if (pConfCompute == NULL || pCtx == NULL || pCtx->pEncStatsBuffer == NULL ||
        !confComputeKeyStoreIsValidGlobalKeyId(pConfCompute, pCtx->globalKeyIdIn))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Ensure additions won't overflow the counters
    if ((pCtx->pEncStatsBuffer->bytesEncryptedD2H > (pCtx->pEncStatsBuffer->bytesEncryptedD2H + bufferSize)) ||
        (pCtx->pEncStatsBuffer->numEncryptionsD2H > (pCtx->pEncStatsBuffer->numEncryptionsD2H + 1)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pCtx->pEncStatsBuffer->bytesEncryptedD2H += bufferSize;
    pCtx->pEncStatsBuffer->numEncryptionsD2H += 1;

    // Check if we must trigger key rotation
    if (confComputeIsGivenThresholdCrossed((void *)pCtx->pEncStatsBuffer,
                                           pConfCompute->keyRotationInternalThreshold,
                                           NV_FALSE))
    {
        NV_PRINTF(LEVEL_INFO, "Triggering key rotation for global key id 0x%x\n", pCtx->globalKeyIdIn);
        NV_PRINTF(LEVEL_INFO, "Total bytes encrypted 0x%llx total encrypt ops 0x%llx\n",
                  pCtx->pEncStatsBuffer->bytesEncryptedD2H - bufferSize,
                  pCtx->pEncStatsBuffer->numEncryptionsD2H - 1);

        status = confComputeKeyStoreUpdateKey(pConfCompute, pCtx->globalKeyIdIn);
        if (status != NV_OK)
        {
            return status;
        }

        // Now we need to update the CCSL context
        status = ccslUpdateViaKeyId(pConfCompute, pCtx, pCtx->globalKeyIdIn);
        if (status != NV_OK)
        {
            return status;
        }

        // The aggregate stats must be reset as the keys have been rotated
        pCtx->pEncStatsBuffer->bytesEncryptedD2H = bufferSize;
        pCtx->pEncStatsBuffer->numEncryptionsD2H = 1;
    }

    return status;
}

NV_STATUS
ccslIncrementCounter_IMPL
(
    pCcslContext  pCtx,
    NvU8         *ctr,
    NvU64         increment
)
{
    NvU32 msgCounterLo = NvU32_BUILD(ctr[3], ctr[2], ctr[1], ctr[0]);

    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (pCtx->msgCounterSize)
    {
        case CSL_MSG_CTR_32:
            if (increment > NV_U32_MAX)
            {
                return NV_ERR_INVALID_ARGUMENT;
            }

            if (msgCounterLo > (NV_U32_MAX - increment))
            {
                NV_PRINTF(LEVEL_ERROR, "CCSL Error! IV overflow detected!\n");
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }

            msgCounterLo += increment;
            ccslSplit32(ctr, msgCounterLo);
            break;
        case CSL_MSG_CTR_64:
        {
            NvU32 msgCounterHi = NvU32_BUILD(ctr[7], ctr[6], ctr[5], ctr[4]);
            NvU64 msgCounter = ((NvU64) msgCounterHi << 32) | msgCounterLo;

            if (msgCounterLo > (NV_U64_MAX - increment))
            {
                NV_PRINTF(LEVEL_ERROR, "CCSL Error! IV overflow detected!\n");
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }

            msgCounter += increment;
            ccslSplit64(ctr, msgCounter);
            break;
        }
    }

    return NV_OK;
}

static OBJGPU *getGpuViaChannelHandle(NvHandle hClient, NvHandle hChannel)
{
    RsClient      *pChannelClient;
    KernelChannel *pKernelChannel;

    if (serverGetClientUnderLock(&g_resServ, hClient, &pChannelClient) != NV_OK)
    {
        return NULL;
    }

    if (CliGetKernelChannel(pChannelClient, hChannel, &pKernelChannel) != NV_OK)
    {
        return NULL;
    }

    return GPU_RES_GET_GPU(pKernelChannel);
}

static KernelChannel *getKernelChannelViaChannelHandle(NvHandle hClient, NvHandle hChannel)
{
    RsClient      *pChannelClient;
    KernelChannel *pKernelChannel;

    if (serverGetClientUnderLock(&g_resServ, hClient, &pChannelClient) != NV_OK)
    {
        return NULL;
    }

    if (CliGetKernelChannel(pChannelClient, hChannel, &pKernelChannel) != NV_OK)
    {
        return NULL;
    }

    return pKernelChannel;
}

static void openrmCtxFree(void *openrmCtx)
{
    if (openrmCtx != NULL)
    {
        libspdm_aead_free(openrmCtx);
    }
}

NV_STATUS
ccslContextInitViaChannel_IMPL
(
    pCcslContext *ppCtx,
    NvHandle      hClient,
    NvHandle      hSubdevice,
    NvHandle      hChannel
)
{
    OBJSYS            *pSys     = SYS_GET_INSTANCE();
    OBJGPUMGR         *pGpuMgr  = SYS_GET_GPUMGR(pSys);
    OBJGPU            *pGpu;
    RM_API            *pRmApi   = NULL;
    KernelChannel     *pKernelChannel;
    NV_STATUS          status;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    NVC56F_CTRL_CMD_GET_KMB_PARAMS getKmbParams;

    NV_PRINTF(LEVEL_INFO, "Initializing CCSL context via channel.\n");

    if(!pGpuMgr->ccCaps.bHccFeatureCapable)
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    if (ppCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = getGpuViaChannelHandle(hClient, hChannel);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_CHANNEL;
    }

    pKernelChannel = getKernelChannelViaChannelHandle(hClient, hChannel);
    if (pKernelChannel == NULL)
    {
        return NV_ERR_INVALID_CHANNEL;
    }

    pCcslContext pCtx = portMemAllocNonPaged(sizeof(*pCtx));
    if (pCtx == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    *ppCtx = pCtx;

    pCtx->msgCounterSize = CSL_MSG_CTR_32;
    pCtx->openrmCtx = NULL;
    pCtx->pDecryptBundles = NULL;

    if (!libspdm_aead_gcm_prealloc(&pCtx->openrmCtx))
    {
        status = NV_ERR_NO_MEMORY;
        goto ccslContextInitViaChannelCleanup;
    }

    pCtx->pDecryptBundles = portMemAllocNonPaged(sizeof(*pCtx->pDecryptBundles) * MAX_DECRYPT_BUNDLES);
    if (pCtx->pDecryptBundles == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto ccslContextInitViaChannelCleanup;
    }

    pCtx->hClient = hClient;
    pCtx->hSubdevice = hSubdevice;
    pCtx->hChannel = hChannel;

    if (IS_GSP_CLIENT(pGpu))
    {
        if (rmGpuLockIsOwner())
        {
            pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        }
        else
        {
            pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
        }
    }
    else
    {
        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    }
    portMemSet(&getKmbParams, 0, sizeof(getKmbParams));

    status = pRmApi->Control(pRmApi, hClient, hChannel,
                             NVC56F_CTRL_CMD_GET_KMB, &getKmbParams,
                             sizeof(getKmbParams));
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status, ccslContextInitViaChannelCleanup);

    pCtx->currDecryptBundle = 0;

    writeKmbToContext(pCtx, &getKmbParams.kmb, ROTATE_IV_ALL_VALID);

    status = memdescCreate(&pMemDesc, pGpu, sizeof(CC_CRYPTOBUNDLE_STATS), sizeof(NvU64), NV_TRUE,
                           ADDR_SYSMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status, ccslContextInitViaChannelCleanup);

    status = memdescAlloc(pMemDesc);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status, ccslContextInitViaChannelmemdescDestroy);

    status = kchannelSetEncryptionStatsBuffer_HAL(pGpu, pKernelChannel, pMemDesc, NV_TRUE);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status, ccslContextInitViaChannelmemdescFree);

    pCtx->pEncStatsBuffer = pKernelChannel->pEncStatsBuf;
    pCtx->pMemDesc = pMemDesc;

    // Set values only used for GSP keys to invalid
    pCtx->globalKeyIdIn  = CC_GKEYID_GEN(CC_KEYSPACE_SIZE, 0);
    pCtx->globalKeyIdOut = CC_GKEYID_GEN(CC_KEYSPACE_SIZE, 0);
    pCtx->pConfCompute   = NULL;

    return NV_OK;

ccslContextInitViaChannelmemdescFree:
    memdescFree(pMemDesc);

ccslContextInitViaChannelmemdescDestroy:
    memdescDestroy(pMemDesc);

ccslContextInitViaChannelCleanup:
    portMemFree(pCtx->pDecryptBundles);
    openrmCtxFree(pCtx->openrmCtx);
    portMemFree(pCtx);

    return status;
}

NV_STATUS
ccslContextInitViaKeyId_KERNEL
(
    ConfidentialCompute *pConfCompute,
    pCcslContext        *ppCtx,
    NvU32                globalKeyId
)
{
    NV_STATUS  status;
    CC_KMB     kmb;
    OBJSYS    *pSys    = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

    NV_PRINTF(LEVEL_INFO, "Initializing CCSL context via global key ID.\n");

    // This function must be redesigned for multi-gpu
    if(!pGpuMgr->ccCaps.bHccFeatureCapable)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (ppCtx == NULL || pConfCompute == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Check to see whether key is valid, as well as explicitly an internal key.
    // While this API would work for other keys, we only expect and explicitly
    // support it for internal (GSP) keys.
    //
    if (!confComputeKeyStoreIsValidGlobalKeyId(pConfCompute, globalKeyId) ||
        CC_GKEYID_GET_KEYSPACE(globalKeyId) != CC_KEYSPACE_GSP)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pCcslContext pCtx = portMemAllocNonPaged(sizeof(*pCtx));
    if (pCtx == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    *ppCtx = pCtx;

    // Ensure all pointers are default set to NULL, in case we get out of sync with channel context values
    portMemSet(pCtx, 0, sizeof(*pCtx));

    if (!libspdm_aead_gcm_prealloc(&pCtx->openrmCtx))
    {
        portMemFree(pCtx);
        return NV_ERR_NO_MEMORY;
    }

    pCtx->pEncStatsBuffer = (CC_CRYPTOBUNDLE_STATS *)portMemAllocNonPaged(sizeof(CC_CRYPTOBUNDLE_STATS));
    if (pCtx->pEncStatsBuffer == NULL)
    {
        portMemFree(pCtx);
        return NV_ERR_NO_MEMORY;
    }

    portMemSet((void *)pCtx->pEncStatsBuffer, 0, sizeof(CC_CRYPTOBUNDLE_STATS));

    status = confComputeKeyStoreRetrieveViaKeyId_HAL(pConfCompute,
                                                     globalKeyId,
                                                     ROTATE_IV_ALL_VALID,
                                                     NV_TRUE,
                                                     &kmb);
    if (status != NV_OK)
    {
        portMemFree((void *)pCtx->pEncStatsBuffer);
        openrmCtxFree(pCtx->openrmCtx);
        portMemFree(pCtx);
        return status;
    }

    // For now assume any call to this function uses a 64-bit message counter.
    pCtx->msgCounterSize = CSL_MSG_CTR_64;
    writeKmbToContext(pCtx, &kmb, ROTATE_IV_ALL_VALID);

    confComputeGetKeyPairByKey(pConfCompute, globalKeyId, &pCtx->globalKeyIdOut, &pCtx->globalKeyIdIn);
    pCtx->pConfCompute = pConfCompute;

    return NV_OK;
}

void
ccslContextClear_IMPL
(
    pCcslContext pCtx
)
{
    NV_PRINTF(LEVEL_INFO, "Clearing the CCSL context.\n");

    if (pCtx == NULL)
    {
        return;
    }

    portMemFree(pCtx->pDecryptBundles);
    if (!isChannel(pCtx))
    {
        portMemFree((void *)pCtx->pEncStatsBuffer);
    }

    openrmCtxFree(pCtx->openrmCtx);
    memdescFree(pCtx->pMemDesc);
    memdescDestroy(pCtx->pMemDesc);
    portMemFree(pCtx);
}

NV_STATUS
ccslContextUpdate_KERNEL(pCcslContext pCtx)
{
    OBJGPU    *pGpu;
    RM_API    *pRmApi = NULL;
    NV_STATUS  status = NV_OK;

    NVC56F_CTRL_CMD_GET_KMB_PARAMS getKmbParams;

    NV_PRINTF(LEVEL_INFO, "Updating the CCSL context.\n");

    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!isChannel(pCtx))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = getGpuViaChannelHandle(pCtx->hClient, pCtx->hChannel);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_CHANNEL;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        if (rmGpuLockIsOwner())
        {
            pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        }
        else
        {
            pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
        }
    }
    else
    {
        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    }

    portMemSet(&getKmbParams, 0, sizeof(getKmbParams));
    status = pRmApi->Control(pRmApi, pCtx->hClient, pCtx->hChannel,
                             NVC56F_CTRL_CMD_GET_KMB, &getKmbParams,
                             sizeof(getKmbParams));
    if (status != NV_OK)
    {
        portMemSet(&getKmbParams, 0, sizeof(getKmbParams));

        return status;
    }

    pCtx->currDecryptBundle = (pCtx->currDecryptBundle + 1) % MAX_DECRYPT_BUNDLES;
    writeKmbToContext(pCtx, &getKmbParams.kmb, ROTATE_IV_ALL_VALID);
    portMemSet(&getKmbParams, 0, sizeof(getKmbParams));

    return NV_OK;
}

NV_STATUS
ccslRotateIv_IMPL
(
    pCcslContext pCtx,
    NvU8         direction
)
{
    OBJGPU   *pGpu;
    RM_API   *pRmApi = NULL;
    NV_STATUS status = NV_OK;
    NvU32     gpuMaskRelease;

    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS rotateIvParams;

    if ((direction != CCSL_DIR_HOST_TO_DEVICE) && (direction != CCSL_DIR_DEVICE_TO_HOST))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!isChannel(pCtx))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemSet(&rotateIvParams, 0, sizeof(rotateIvParams));

    if (direction == CCSL_DIR_HOST_TO_DEVICE)
    {
        rotateIvParams.rotateIvType = ROTATE_IV_ENCRYPT;
    }
    else
    {
        rotateIvParams.rotateIvType = ROTATE_IV_DECRYPT;
    }

    pGpu = getGpuViaChannelHandle(pCtx->hClient, pCtx->hChannel);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_CHANNEL;
    }

    gpuMaskRelease = 0;
    if (IS_GSP_CLIENT(pGpu))
    {
        //
        // Attempt to acquire GPU lock. If unsuccessful then return error to UVM and it can
        // try later. This is needed as UVM may need to rotate a channel's IV while
        // RM is already holding a GPU lock.
        //
        status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
            GPU_LOCK_FLAGS_COND_ACQUIRE, RM_LOCK_MODULES_RPC, &gpuMaskRelease);

        if (status == NV_OK)
        {
            pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        }
        else if (status == NV_ERR_STATE_IN_USE)
        {
            return status;
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "Converting %s to NV_ERR_GENERIC.\n",
                      nvstatusToString(status));
            return NV_ERR_GENERIC;
        }
    }
    else
    {
        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    }

    NV_ASSERT_OK_OR_GOTO(status,
                         pRmApi->Control(pRmApi,
                                         pCtx->hClient,
                                         pCtx->hChannel,
                                         NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV,
                                         &rotateIvParams,
                                         sizeof(rotateIvParams)),
                         failed);

    if (IS_GSP_CLIENT(pGpu))
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }

    switch (direction)
    {
        case CCSL_DIR_HOST_TO_DEVICE:
            portMemCopy(pCtx->ivOut, sizeof(pCtx->ivOut),
                        &rotateIvParams.updatedKmb.encryptBundle.iv,
                        sizeof(rotateIvParams.updatedKmb.encryptBundle.iv));
            break;

        case CCSL_DIR_DEVICE_TO_HOST:
            portMemCopy(pCtx->ivIn, sizeof(pCtx->ivIn),
                        &rotateIvParams.updatedKmb.decryptBundle.iv,
                        sizeof(rotateIvParams.updatedKmb.decryptBundle.iv));
            break;
    }

    return NV_OK;

failed:
    if (IS_GSP_CLIENT(pGpu))
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }

    return status;
}

NV_STATUS
ccslEncryptWithIv_IMPL
(
    pCcslContext  pCtx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8         *encryptIv,
    NvU8 const   *aadBuffer,
    NvU32         aadSize,
    NvU8         *outputBuffer,
    NvU8         *authTagBuffer
)
{
    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NvU8   iv[CC_AES_256_GCM_IV_SIZE_BYTES] = {0};
    size_t outputBufferSize                 = bufferSize;

    // The "freshness" bit is right after the IV
    if (encryptIv[CC_AES_256_GCM_IV_SIZE_BYTES] != 1)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    encryptIv[CC_AES_256_GCM_IV_SIZE_BYTES] = 0;

    for (NvU64 i = 0; i < CC_AES_256_GCM_IV_SIZE_BYTES; i++)
    {
        iv[i] = encryptIv[i] ^ pCtx->ivMaskOut[i];
    }

    if(!libspdm_aead_aes_gcm_encrypt_prealloc(pCtx->openrmCtx,
        (NvU8 *)pCtx->keyOut, CC_AES_256_GCM_KEY_SIZE_BYTES,
        iv, CC_AES_256_GCM_IV_SIZE_BYTES, aadBuffer, aadSize,
        inputBuffer, bufferSize, authTagBuffer, 16,
        outputBuffer, &outputBufferSize))
    {
        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

NV_STATUS
ccslEncrypt_KERNEL
(
    pCcslContext  pCtx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8 const   *aadBuffer,
    NvU32         aadSize,
    NvU8         *outputBuffer,
    NvU8         *authTagBuffer
)
{
    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NvU8   iv[CC_AES_256_GCM_IV_SIZE_BYTES] = {0};
    size_t outputBufferSize                 = bufferSize;

    if (bufferSize == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (ccslIncrementCounter(pCtx, pCtx->ivOut, 1) != NV_OK)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    for (NvU64 i = 0; i < CC_AES_256_GCM_IV_SIZE_BYTES; i++)
    {
        iv[i] = pCtx->ivOut[i] ^ pCtx->ivMaskOut[i];
    }

    if(!libspdm_aead_aes_gcm_encrypt_prealloc(pCtx->openrmCtx,
        (NvU8 *)pCtx->keyOut, CC_AES_256_GCM_KEY_SIZE_BYTES,
        iv, CC_AES_256_GCM_IV_SIZE_BYTES, aadBuffer, aadSize,
        inputBuffer, bufferSize, authTagBuffer, 16,
        outputBuffer, &outputBufferSize))
    {
        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

NV_STATUS
ccslEncryptWithRotationChecks_KERNEL
(
    pCcslContext         pCtx,
    NvU32                bufferSize,
    NvU8 const          *inputBuffer,
    NvU8 const          *aadBuffer,
    NvU32                aadSize,
    NvU8                *outputBuffer,
    NvU8                *authTagBuffer
)
{
    NV_STATUS status = NV_OK;

    if (pCtx == NULL || pCtx->pConfCompute == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = ccslRotationChecksEncrypt(pCtx->pConfCompute, pCtx, bufferSize);
    if (status != NV_OK)
    {
        return status;
    }

    // If we've rolled the keys but failed to encrypt, we cannot roll back the key update.
    return ccslEncrypt(pCtx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer);
}

NV_STATUS
ccslDecrypt_KERNEL
(
    pCcslContext  pCtx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8 const   *decryptIv,
    NvU32         keyRotationId,
    NvU8 const   *aadBuffer,
    NvU32         aadSize,
    NvU8         *outputBuffer,
    NvU8 const   *authTagBuffer
)
{
    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NvU8   iv[CC_AES_256_GCM_IV_SIZE_BYTES] = {0};
    size_t outputBufferSize = bufferSize;
    NvU8 *keyIn = pCtx->keyIn;
    NvU8 *ivMaskIn = pCtx->ivMaskIn;

    if ((bufferSize == 0) || ((aadBuffer != NULL) && (aadSize == 0)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (isChannel(pCtx) && (keyRotationId != NV_U32_MAX))
    {
        pDecryptBundle pDecryptBundle = pCtx->pDecryptBundles + (keyRotationId % MAX_DECRYPT_BUNDLES);

        keyIn = pDecryptBundle->keyIn;
        ivMaskIn = pDecryptBundle->ivMaskIn;
    }

    if (decryptIv == NULL)
    {
        if (ccslIncrementCounter(pCtx, pCtx->ivIn, 1) != NV_OK)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        for (NvU64 i = 0; i < CC_AES_256_GCM_IV_SIZE_BYTES; i++)
        {
            iv[i] = pCtx->ivIn[i] ^ ivMaskIn[i];
        }
    }
    else
    {
        for (NvU64 i = 0; i < CC_AES_256_GCM_IV_SIZE_BYTES; i++)
        {
            iv[i] = decryptIv[i] ^ ivMaskIn[i];
        }
    }

    if(!libspdm_aead_aes_gcm_decrypt_prealloc(pCtx->openrmCtx,
        keyIn, CC_AES_256_GCM_KEY_SIZE_BYTES,
        iv, CC_AES_256_GCM_IV_SIZE_BYTES, aadBuffer, aadSize,
        inputBuffer, bufferSize, (NvU8 *) authTagBuffer, 16,
        outputBuffer, &outputBufferSize))
    {
        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

NV_STATUS
ccslDecryptWithRotationChecks_KERNEL
(
    pCcslContext         pCtx,
    NvU32                bufferSize,
    NvU8 const          *inputBuffer,
    NvU8 const          *decryptIv,
    NvU8 const          *aadBuffer,
    NvU32                aadSize,
    NvU8                *outputBuffer,
    NvU8 const          *authTagBuffer
)
{
    NV_STATUS status = NV_OK;

    if (pCtx == NULL || pCtx->pConfCompute == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = ccslRotationChecksDecrypt(pCtx->pConfCompute, pCtx, bufferSize);
    if (status != NV_OK)
    {
        return status;
    }

    // If we've rolled the keys but failed to decrypt, we cannot roll back the key update.
    return ccslDecrypt(pCtx, bufferSize, inputBuffer, decryptIv, NV_U32_MAX, aadBuffer, aadSize, outputBuffer, authTagBuffer);
}

static NV_STATUS ccslIncrementCounter192(NvU8 *ctr)
{
    NvU8 carry = 1;
    NvU64 i;
    NvBool overflow = NV_TRUE;

    for (i = 0; i < 192 / 8; ++i)
    {
        if (ctr[i] != 0xFF)
        {
            overflow = NV_FALSE;
            break;
        }
    }
    if (overflow)
    {
        NV_PRINTF(LEVEL_ERROR, "CCSL Error! IV overflow detected!\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    for (i = 0; i < 192 / 8; ++i)
    {
        ctr[i] += carry;
        carry = (ctr[i] == 0) && (carry == 1);
    }

    return NV_OK;
}

NV_STATUS
ccslSign_IMPL
(
    pCcslContext  pCtx,
    NvU32         bufferSize,
    NvU8 const   *inputBuffer,
    NvU8         *authTagBuffer
)
{
    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    void *hmac_ctx;

    if ((bufferSize == 0) || !isChannel(pCtx))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (ccslIncrementCounter192(pCtx->nonce) != NV_OK)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    hmac_ctx = libspdm_hmac_sha256_new();
    if (hmac_ctx == NULL)
    {
        return NV_ERR_GENERIC;
    }

    if (!libspdm_hmac_sha256_set_key(hmac_ctx, pCtx->keyIn, CC_AES_256_GCM_KEY_SIZE_BYTES))
    {
        libspdm_hmac_sha256_free(hmac_ctx);
        return NV_ERR_GENERIC;
    }

    if (!libspdm_hmac_sha256_update(hmac_ctx, inputBuffer, bufferSize))
    {
        libspdm_hmac_sha256_free(hmac_ctx);
        return NV_ERR_GENERIC;
    }

    if (!libspdm_hmac_sha256_update(hmac_ctx, pCtx->nonce, CC_HMAC_NONCE_SIZE_BYTES))
    {
        libspdm_hmac_sha256_free(hmac_ctx);
        return NV_ERR_GENERIC;
    }

    if (!libspdm_hmac_sha256_final(hmac_ctx, authTagBuffer))
    {
        libspdm_hmac_sha256_free(hmac_ctx);
        return NV_ERR_GENERIC;
    }

    libspdm_hmac_sha256_free(hmac_ctx);

    return NV_OK;
}

static NvU64
getMessageCounterAndLimit
(
    pCcslContext  pCtx,
    NvU8         *iv,
    NvU64        *limit
)
{
    NvU32 msgCounterLo = NvU32_BUILD(iv[3], iv[2], iv[1], iv[0]);
    NvU32 msgCounterHi = NvU32_BUILD(iv[7], iv[6], iv[5], iv[4]);

    switch (pCtx->msgCounterSize)
    {
        case CSL_MSG_CTR_32:
            *limit = NV_U32_MAX;
            return msgCounterLo;
        case CSL_MSG_CTR_64:
            *limit = NV_U64_MAX;
            return ((((NvU64) msgCounterHi) << 32) | msgCounterLo);
        default:
            return 0;
    }
}

NV_STATUS
ccslQueryMessagePool_IMPL
(
    pCcslContext  pCtx,
    NvU8          direction,
    NvU64        *messageNum
)
{
    NvU64 limit;
    NvU64 messageCounter;

    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (direction)
    {
        case CCSL_DIR_HOST_TO_DEVICE:
            messageCounter = getMessageCounterAndLimit(pCtx, pCtx->ivOut, &limit);
            break;
        case CCSL_DIR_DEVICE_TO_HOST:
            messageCounter = getMessageCounterAndLimit(pCtx, pCtx->ivIn, &limit);
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    *messageNum = limit - messageCounter;

    return NV_OK;
}

NV_STATUS
ccslIncrementIv_IMPL
(
    pCcslContext  pCtx,
    NvU8          direction,
    NvU64         increment,
    NvU8         *iv
)
{
    NV_STATUS status;
    void *ivPtr;

    if (pCtx == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (direction)
    {
        case CCSL_DIR_HOST_TO_DEVICE:
            ivPtr = pCtx->ivOut;
            break;
        case CCSL_DIR_DEVICE_TO_HOST:
            ivPtr = pCtx->ivIn;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    status = ccslIncrementCounter(pCtx, ivPtr, increment);

    if (status != NV_OK)
    {
        return status;
    }

    if (iv != NULL) {
        portMemCopy(iv, CC_AES_256_GCM_IV_SIZE_BYTES, ivPtr, CC_AES_256_GCM_IV_SIZE_BYTES);

        if (direction == CCSL_DIR_HOST_TO_DEVICE)
        {
            // The "freshness" bit is right after the IV.
            iv[CC_AES_256_GCM_IV_SIZE_BYTES] = 1;
        }
        else
        {
            // Decrypt IV cannot be used for encryption.
            iv[CC_AES_256_GCM_IV_SIZE_BYTES] = 0;
        }
    }

    return NV_OK;
}

NV_STATUS
ccslLogEncryption_IMPL
(
    pCcslContext pCtx,
    NvU8         direction,
    NvU32        bufferSize
)
{
    NV_STATUS status;
    NvU64 messageNum;

    status = ccslQueryMessagePool(pCtx, direction, &messageNum);
    if (status != NV_OK)
    {
        return status;
    }
    else if (messageNum == 0)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    if (isChannel(pCtx))
    {
        if (direction == CCSL_DIR_DEVICE_TO_HOST)
        {
            pCtx->pEncStatsBuffer->numEncryptionsD2H++;
            pCtx->pEncStatsBuffer->bytesEncryptedD2H += bufferSize;
        }
        else
        {
            pCtx->pEncStatsBuffer->numEncryptionsH2D++;
            pCtx->pEncStatsBuffer->bytesEncryptedH2D += bufferSize;
        }
    }
    else if (pCtx->globalKeyIdIn == CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT))
    {
        status = ccslRotationChecksDecrypt(pCtx->pConfCompute, pCtx, bufferSize);
    }

    return status;
}
