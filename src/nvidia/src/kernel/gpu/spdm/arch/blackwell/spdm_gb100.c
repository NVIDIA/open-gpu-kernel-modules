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

/*!
 * Provides the implementation for GB100-specific SPDM HALs
 * interfaces.
 */

/* ------------------------ Includes --------------------------------------- */
#include "gpu/spdm/spdm.h"
#include "gpu/gpu.h"
#include "conf_compute/cc_keystore.h"
#include "gpu/conf_compute/conf_compute.h"
#include "sec2/sec2ifheartbeat.h"
#include "gpu/spdm/libspdm_includes.h"
#include "gpu/timer/objtmr.h"

ct_assert(SPDM_HEARTBEAT_KEY_SIZE_BYTES == CC_AES_256_GCM_KEY_SIZE_BYTES);
ct_assert(RM_SEC2_HEARTBEAT_TAG_SIZE_BYTES ==
          (RM_SEC2_HEARTBEAT_TAG_SIZE_WORDS * sizeof(NvU32)));

typedef struct
{
    union {
        NvU8   tagByte[RM_SEC2_HEARTBEAT_TAG_SIZE_BYTES];
        NvU32  tagWord[RM_SEC2_HEARTBEAT_TAG_SIZE_WORDS];
    };

    NvU32 encNonce;
} SPDM_HEARTBEAT_SNAPSHOT;

/*!
 * @brief Build the heartbeat AES-GCM IV from counter + domain.
 *
 * @param[out] pIv      Buffer that receives the derived IV bytes.
 */

static void
_spdmHeartbeatBuildIv
(
    NvU32   iv_3_0,
    NvU32   iv_7_4,
    NvU8   *pIv
)
{
    NvU32 iv_11_8;
    NvU32 offset = 0;
    NvU32 size = sizeof(NvU32);

    //
    // SEC2 builds 12-bytes with format :
    // Total 12 bytes. Set from low address to high address, || means concatenate
    // { 4-bytes nonce-A || 4-bytes nonce-B || 4-bytes nonce-C }
    // nonce-c = (nonce-A ^ nonce-B)
    //
    portMemCopy(&pIv[offset], size, &iv_3_0, size);
    offset += size;
    portMemCopy(&pIv[offset], size, &iv_7_4, size);
    offset += size;

    iv_11_8 = (iv_3_0 ^ iv_7_4);
    portMemCopy(&pIv[offset], size, &iv_11_8, size);
}

/*!
 * @brief Creating the snapshot of SEC2 heartbeat context from PSEC registers.
 *
 * @param[in]  pGpu       OBJGPU pointer.
 * @param[out] pSnapshot  Snapshot structure to fill on success.
 *
 */
static void
 _spdmHeartbeatCreateSnapshot
(
    OBJGPU                  *pGpu,
    SPDM_HEARTBEAT_SNAPSHOT *pSnapshot
)
{
    NvU32 val;

    // read tag
    pSnapshot->tagWord[0] = GPU_REG_RD32(pGpu,  RM_SEC2_HEARTBEAT_TAG_REG_0);
    pSnapshot->tagWord[1] = GPU_REG_RD32(pGpu,  RM_SEC2_HEARTBEAT_TAG_REG_1);
    pSnapshot->tagWord[2] = GPU_REG_RD32(pGpu,  RM_SEC2_HEARTBEAT_TAG_REG_2);
    pSnapshot->tagWord[3] = GPU_REG_RD32(pGpu,  RM_SEC2_HEARTBEAT_TAG_REG_3);

    val = GPU_REG_RD32(pGpu, RM_SEC2_HEARTBEAT_NONCE_EXCHANGE_REG);
    pSnapshot->encNonce = val;

    portAtomicMemoryFenceLoad();
}

/*!
 * @brief Read the latest SEC2 heartbeat snapshot from PSEC registers.
 *
 * @param[in]  pGpu       OBJGPU pointer.
 * @param[out] pSnapshot  Snapshot structure to fill on success.
 *
 * @return NV_TRUE if a committed snapshot was read, NV_FALSE otherwise.
 */
static NV_STATUS
_spdmReadHeartbeatSnapshot
(
    OBJGPU                  *pGpu,
    Spdm                    *pSpdm,
    SPDM_HEARTBEAT_SNAPSHOT *pSnapshot
)
{
    NvU32     event = NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_UNKNOWN;
    NvU32     val;
    NvU32     gpuInst;
    NvU64     currentTime;
    NV_STATUS status;
    OBJTMR   *pTmr = NULL;

    if (pGpu == NULL || pSnapshot == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    gpuInst = gpuGetInstance(pGpu);

    pTmr = GPU_GET_TIMER(pGpu);


    val = GPU_REG_RD32(pGpu, RM_SEC2_HEARTBEAT_HANDSHAKE_REG);

    event = DRF_VAL(_RM_SEC2, _HEARTBEAT_HANDSHAKE, _EVENT ,val);

    if (event == NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_SEC2_ERROR)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU-%d : SEC2 heartbeat fatal error detected !!! \n", gpuInst);
        return NV_ERR_INVALID_DEVICE;
    }

    if (event != NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_SEC2_READY)
    {
        status = tmrGetCurrentTime(pTmr, &currentTime);
        if (status != NV_OK && pGpu != NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                      SPDM_HBEAT_LOG "GPU-%u: tmrGetCurrentTime failed 0x%x\n",
                      gpuInst, status);
            return status;
        }

        //
        // Time the deadline from the RM_READY write instead of counting polls.
        // The poll that writes RM_READY also reads back here, so a poll count
        // left SEC2 only one heartbeat period to answer.
        //
        if (pSpdm->bRmHeartbeatContextReady &&
            ((currentTime - pSpdm->heartbeatRmReadyTime) > SPDM_SEC2_HEARTBEAT_TIMEOUT_NSEC))
        {
            NV_PRINTF(LEVEL_ERROR, SPDM_HBEAT_LOG "SEC2 didn't get ready %llu ms after RM_READY, snapshot count %u\n",
                      (currentTime - pSpdm->heartbeatRmReadyTime) / 1000000ULL,
                      pSpdm->snapshotCount);
            return NV_ERR_TIMEOUT;
        }

        pSpdm->snapshotCount ++;

        //
        // Even no snapshot available, we still need to update heartbeatLastProgressTime to log callback log
        //
        pSpdm->heartbeatLastProgressTime = currentTime;

        return NV_ERR_MORE_PROCESSING_REQUIRED;
    }

    _spdmHeartbeatCreateSnapshot(pGpu, pSnapshot);

    // reset snapshot count
    pSpdm->snapshotCount = 0;

    return NV_OK;
}

/*!
 * @brief Authenticate and validate one heartbeat snapshot.
 *
 * @param[in] pGpu              OBJGPU pointer.
 * @param[in] pSpdm             SPDM object pointer.
 * @param[in] pSnapshot         Snapshot to authenticate.
 * @param[in] expectedCounter   Counter value expected by RM.
 *
 * @return NV_OK if authentication and counter validation pass.
 */
static NV_STATUS
_spdmAuthenticateHeartbeatSnapshot
(
    OBJGPU                  *pGpu,
    Spdm                    *pSpdm,
    SPDM_HEARTBEAT_SNAPSHOT *pSnapshot,
    NvU32                    inputNonce,
    NvU32                    partialIv,
    NvU32                    aad,
    NvU8                    *pNonceOut
)
{
    NvU8      plainBytes[RM_SEC2_HEARTBEAT_NONCE_SIZE_BYTES] = {0};
    NvU8      iv[RM_SEC2_HEARTBEAT_IV_SIZE_BYTES];
    NvU32     gpuInst = gpuGetInstance(pGpu);
    size_t    plainSize = sizeof(plainBytes);
    NvU64     timeStamp;
    NV_STATUS status;
    OBJTMR    *pTmr = GPU_GET_TIMER(pGpu);

    _spdmHeartbeatBuildIv(inputNonce, partialIv, iv);

    if (!libspdm_aead_aes_gcm_decrypt(
            pSpdm->heartbeatKey, sizeof(pSpdm->heartbeatKey),
            iv, sizeof(iv),
            (NvU8 *)&aad, sizeof(aad),
            (NvU8 *)&pSnapshot->encNonce, RM_SEC2_HEARTBEAT_NONCE_SIZE_BYTES,
            pSnapshot->tagByte, RM_SEC2_HEARTBEAT_TAG_SIZE_BYTES,
            plainBytes, &plainSize) ||
            (plainSize != sizeof(plainBytes)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  SPDM_HBEAT_LOG "Decrypt FAILED, need to check all AES-GCM parameters !!! \n");
        return NV_ERR_INVALID_DATA;
    }

    status = tmrGetCurrentTime(pTmr, &timeStamp);
    if (status != NV_OK && pGpu != NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  SPDM_HBEAT_LOG "GPU-%u: tmrGetCurrentTime failed 0x%x\n",
                  gpuInst, status);
        return status;
    }

    pSpdm->heartbeatLastProgressTime = timeStamp;

    portMemCopy(pNonceOut, RM_SEC2_HEARTBEAT_NONCE_SIZE_BYTES,
                plainBytes, RM_SEC2_HEARTBEAT_NONCE_SIZE_BYTES);

    return NV_OK;
}

static NV_STATUS
_spdmHeartbeatIni
(
   OBJGPU *pGpu,
   Spdm   *pSpdm,
   NvU32  *pPartialIv,
   NvU32  *pNonce
)
{
    NvU32 nonce;
    NvU32 partialIv;
    NvU32 gpuInst;
    NvU32 val = 0;

    if (pGpu == NULL   || pSpdm == NULL ||
        pNonce == NULL || pPartialIv == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    gpuInst = gpuGetInstance(pGpu);

    if (!libspdm_random_bytes((uint8_t *)&nonce, sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_ERROR, "GPU-%d : RNG nonce error !! \n", gpuInst);
        return NV_ERR_INVALID_COMMAND;
    }

    *pNonce = nonce;

    if (!libspdm_random_bytes((uint8_t *)&partialIv, sizeof(NvU32)))
    {
        NV_PRINTF(LEVEL_ERROR, "GPU-%d : RNG partialIv error !! \n", gpuInst);
        return NV_ERR_INVALID_COMMAND;
    }

    *pPartialIv = partialIv;

    // write nonce to register
    GPU_REG_WR32(pGpu, RM_SEC2_HEARTBEAT_NONCE_EXCHANGE_REG, nonce);

    // write partialIv to register
    GPU_REG_WR32(pGpu, RM_SEC2_HEARTBEAT_IV_REG, partialIv);

    // Set Rm ready event to handshake register
    val = FLD_SET_DRF(_RM_SEC2_HEARTBEAT, _HANDSHAKE, _EVENT, _RM_READY, val);
    GPU_REG_WR32(pGpu, RM_SEC2_HEARTBEAT_HANDSHAKE_REG, val);

    pSpdm->bRmHeartbeatContextReady = NV_TRUE;
    pSpdm->snapshotCount = 0;
    return NV_OK;
}

/*!
 * @brief Validate heartbeat liveness/authenticity and advance handshake.
 *
 * @param[in] pGpu          OBJGPU pointer.
 * @param[in] pSpdm         SPDM object pointer.
 * @param[in] callbackTime  Current callback timestamp in nanoseconds.
 *
 * @return NV_OK when heartbeat state is healthy for this callback cycle.
 */
NV_STATUS spdmCheckHeartbeat_GB100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU64   callbackTime
)
{
    SPDM_HEARTBEAT_SNAPSHOT snapshot = {0};
    NV_STATUS               status;
    NvU32                   sec2NonceIn;
    NvU32                   sec2NonceOut;
    NvU32                   partialIv;
    NvU32                   gpuInst;

    if ((pGpu == NULL) || (pSpdm == NULL) ||
        (callbackTime < pSpdm->heartbeatLastProgressTime))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    gpuInst = gpuGetInstance(pGpu);

    if (!pSpdm->bRmHeartbeatContextReady)
    {
        status = _spdmHeartbeatIni(pGpu, pSpdm, &partialIv, &sec2NonceIn);

        if (status != NV_OK)
        {
            return status;
        }
        pSpdm->nonceIn = sec2NonceIn;
        pSpdm->partialIv = partialIv;
        pSpdm->heartbeatRmReadyTime = callbackTime;
    }
    else
    {
        sec2NonceIn = pSpdm->nonceIn;
        partialIv   = pSpdm->partialIv;
    }

    //
    // TODO : Adding timeout check for gap between callbackTime and pSpdm->heartbeatLastProgressTime;
    // gap = callbackTime - pSpdm->heartbeatLastProgressTime;
    //

    status = _spdmReadHeartbeatSnapshot(pGpu, pSpdm, &snapshot);

    if (status == NV_ERR_MORE_PROCESSING_REQUIRED)
    {
        return status;
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  SPDM_HBEAT_LOG "GPU-%d PSEC snapshot not committed yet; "
                  "SEC2 is halted !!!! \n", gpuInst);
        return status;
    }

    // Input nonce also act as AAD
    status = _spdmAuthenticateHeartbeatSnapshot(pGpu, pSpdm, &snapshot, sec2NonceIn, partialIv, sec2NonceIn,  (NvU8 *)&sec2NonceOut);

    if (status != NV_OK)
    {
        return status;
    }

    if (sec2NonceOut != (sec2NonceIn + 1))
    {
        NV_PRINTF(LEVEL_ERROR, "GPU-%d SEC2 NONCE mismatch, IN = 0x%x, OUT = 0x%x \n", gpuInst, sec2NonceIn, sec2NonceOut);
        return NV_ERR_INVALID_DATA;
    }
    else
    {
        // heartbeat context is consumed successfully now.
        pSpdm->bRmHeartbeatContextReady = NV_FALSE;
    }

    return NV_OK;
}

/*!
 * @brief Load Kernel-RM's local SEC2 heartbeat key material from CC keystore.
 *
 * Kernel-RM and GSP-FMC independently derive a dedicated heartbeat key from
 * the SPDM Export Master Secret using the same derivation string. GSP-FMC
 * wraps its copy for SEC2, where KeyMgr installs it into the RTOS-owned
 * heartbeat keyslot. Plaintext key material never crosses an RPC boundary.
 * Confidential Compute SEC2 bring-up completes both copies before SPDM
 * StatePostLoad arms the heartbeat timer.
 *
 * @param[in] pGpu   OBJGPU pointer.
 * @param[in] pSpdm  SPDM object pointer.
 *
 * @return NV_OK if key retrieval and local arm succeed.
 */
NV_STATUS spdmLoadHeartbeatKey_GB100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    ConfidentialCompute *pConfCompute;
    CC_KMB               kmb = {0};
    NV_STATUS            status;
    NvU32                gpuInst;

    if ((pGpu == NULL) || (pSpdm == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    if (pConfCompute == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    gpuInst = gpuGetInstance(pGpu);

    status = confComputeKeyStoreRetrieveViaKeyId_HAL(pConfCompute,
                 CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_HEARTBEAT),
                 ROTATE_IV_ENCRYPT,
                 NV_FALSE, // key + ivMask only — no IV increment side effects
                 &kmb);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, SPDM_HBEAT_LOG " GPU-%d LoadKey: keystore retrieve of CPU_SEC2_HEARTBEAT "
                  "failed, status 0x%x\n", gpuInst, status);
        portMemSet(&kmb, 0, sizeof(kmb));
        return status;
    }

    portMemCopy(pSpdm->heartbeatKey, sizeof(pSpdm->heartbeatKey),
                kmb.encryptBundle.key, sizeof(kmb.encryptBundle.key));
    portMemSet(&kmb, 0, sizeof(kmb));

    NV_PRINTF(LEVEL_INFO, SPDM_HBEAT_LOG " GPU-%d LoadKey: armed from dedicated CPU_SEC2_HEARTBEAT "
              "CC keystore key; key stays local, never sent\n", gpuInst);

    return NV_OK;
}

