/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/conf_compute/conf_compute_keystore.h"
#include "class/cl2080.h"
#include "libraries/nvport/nvport.h"

#define NUM_SLOTS (KEYSTORE_SLOT_LCE_MGPU(7) + 1)

#define CONCAT64(hi, low) ((((NvU64)(hi) << 32)) | ((NvU64)(low)))

// TODO: Remove once key slots are populated by SPDM module.
#define lceBundle \
{\
    .encryptBundle =\
    {\
        .key = {0x38782F41, 0x3F442847, 0x2B4B6250, 0x65536756, 0x6B597033, 0x73367639, 0x79244226, 0x45294840},\
        .iv = {0x00000000, 0x00000000, 0x00000000},\
        .ivMask = {0x00000000, 0x00000000, 0x00000000}\
    },\
    .decryptBundle =\
    {\
        .key = {0x566B5970, 0x33733676, 0x39792442, 0x26452948, 0x404D6251, 0x65546857, 0x6D5A7134, 0x74377721},\
        .iv = {0x00000000, 0x00000000, 0x00000000},\
        .ivMask = {0x00000000, 0x00000000, 0x00000000}\
    },\
    .bIsWorkLaunch = NV_FALSE\
}

static enum {HCC, APM} ccMode;

// TODO: Remove initialization once key slots are populated by SPDM module.
//       Currently the APM Requester will populate the SEC2 slots.
//       HCC still relies on hardcoded keys.
static CC_KMB keySlot[NUM_SLOTS] =
{
    lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle,
    lceBundle, lceBundle, lceBundle
};

static NV_STATUS checkSlot (NvU32 slotNumber, ROTATE_IV_TYPE rotateOperation);
static void incrementChannelCounter (NvU32 slotNumber, ROTATE_IV_TYPE rotateOperation);
static NvU64 getChannelCounter (NvU32 slotNumber, ROTATE_IV_TYPE rotateOperation);
static NV_STATUS checkAndIncrementSlot (NvU32 slotNumber, ROTATE_IV_TYPE rotateOperation);
static NV_STATUS getSlotNumberLce (KernelChannel *pKernelChannel, NvU32 *slotNumber);
static NvU64 getChannelMaxValue (void);

NV_STATUS
ccKeyStoreInit (OBJGPU *pGpu)
{
    if (gpuIsApmFeatureEnabled(pGpu))
    {
        ccMode = APM;
    }
    else if (gpuIsCCFeatureEnabled(pGpu))
    {
        ccMode = HCC;
    }
    else
    {
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

NV_STATUS
ccKeyStoreDeposit
(
    NvU32  slotNumber,
    CC_KMB keyMaterialBundle
)
{
    if (slotNumber >= NUM_SLOTS)
    {
        return NV_ERR_INVALID_INDEX;
    }

    keySlot[slotNumber] = keyMaterialBundle;

    for (NvU32 index = 0; index < CC_AES_256_GCM_IV_SIZE_DWORD; index++)
    {
        keySlot[slotNumber].encryptBundle.iv[index] = 0;
        keySlot[slotNumber].decryptBundle.iv[index] = 0;
    }

    return NV_OK;
}

NV_STATUS
ccKeyStoreRetrieveViaChannel
(
    KernelChannel *pKernelChannel,
    ROTATE_IV_TYPE rotateOperation,
    PCC_KMB        keyMaterialBundle
)
{
    NvU32 slotNumber;

    if (RM_ENGINE_TYPE_IS_COPY(kchannelGetEngineType(pKernelChannel)))
    {
        if (getSlotNumberLce (pKernelChannel, &slotNumber) != NV_OK)
        {
            return NV_ERR_INVALID_PARAMETER;
        }
    }
    else if (kchannelGetEngineType(pKernelChannel) == RM_ENGINE_TYPE_SEC2)
    {
        slotNumber = KEYSTORE_SLOT_SEC2;
    }
    else
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    if ((rotateOperation == ROTATE_IV_ENCRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
    {
        if (checkAndIncrementSlot (slotNumber, ROTATE_IV_ENCRYPT) != NV_OK)
        {
            // Right now returns error to caller. Depending on how the re-keying flow is designed
            // this may initiate re-keying.

            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    if ((rotateOperation == ROTATE_IV_DECRYPT) || (rotateOperation == ROTATE_IV_ALL_VALID))
    {
        if (checkAndIncrementSlot (slotNumber, ROTATE_IV_DECRYPT) != NV_OK)
        {
            // Right now returns error to caller. Depending on how the re-keying flow is designed
            // this may initiate re-keying.

            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    *keyMaterialBundle = keySlot[slotNumber];

    return NV_OK;
}

void
cckeyStoreClear (void)
{
    portMemSet (keySlot, 0, sizeof(keySlot));
}

static NV_STATUS
getSlotNumberLce
(
    KernelChannel *pKernelChannel,
    NvU32         *slotNumber
)
{
    NvU8 index;

    // TODO: Get actual privilege level of channel. For now assume
    // all channels have user mode privilege.
    switch (kchannelGetEngineType(pKernelChannel))
    {
        case RM_ENGINE_TYPE_COPY0:
            index = 0;
            break;
        case RM_ENGINE_TYPE_COPY1:
            index = 1;
            break;
        case RM_ENGINE_TYPE_COPY2:
            index = 2;
            break;
        case RM_ENGINE_TYPE_COPY3:
            index = 3;
            break;
        case RM_ENGINE_TYPE_COPY4:
            index = 4;
            break;
        case RM_ENGINE_TYPE_COPY5:
            index = 5;
            break;
        case RM_ENGINE_TYPE_COPY6:
            index = 6;
            break;
        case RM_ENGINE_TYPE_COPY7:
            index = 7;
            break;
        default:
            return NV_ERR_GENERIC;
    }

    *slotNumber = KEYSTORE_SLOT_LCE_UMD(index);

    return NV_OK;
}

static NV_STATUS
checkAndIncrementSlot
(
    NvU32          slotNumber,
    ROTATE_IV_TYPE rotateOperation  // Will only be ROTATE_IV_ENCRYPT or ROTATE_IV_DECRYPT.
)
{
    if (checkSlot (slotNumber, rotateOperation) != NV_OK)
    {
        return NV_ERR_GENERIC;
    }

    // TODO: Unconditionally increment channel counter once HCC supports it.
    if (ccMode == APM)
    {
        incrementChannelCounter (slotNumber, rotateOperation);
    }

    return NV_OK;
}

static NV_STATUS
checkSlot
(
    NvU32 slotNumber,
    ROTATE_IV_TYPE rotateOperation  // Will only be ROTATE_IV_ENCRYPT or ROTATE_IV_DECRYPT.
)
{
    if (getChannelCounter (slotNumber, rotateOperation) == getChannelMaxValue())
    {
        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

static void
incrementChannelCounter
(
    NvU32          slotNumber,
    ROTATE_IV_TYPE rotateOperation  // Will only be ROTATE_IV_ENCRYPT or ROTATE_IV_DECRYPT.
)
{
    NvU64 channelCounter = getChannelCounter (slotNumber, rotateOperation);

    channelCounter++;

    switch (rotateOperation)
    {
        case ROTATE_IV_ENCRYPT:
            if (ccMode == HCC)
            {
                keySlot[slotNumber].encryptBundle.iv[2] = (NvU32) (channelCounter >> 32);
                keySlot[slotNumber].encryptBundle.iv[1] = (NvU32) (channelCounter);
            }
            else if (ccMode == APM)
            {
                keySlot[slotNumber].encryptBundle.iv[2] = (NvU32) (channelCounter);
            }
            break;
        case ROTATE_IV_DECRYPT:
            if (ccMode == HCC)
            {
                keySlot[slotNumber].decryptBundle.iv[2] = (NvU32) (channelCounter >> 32);
                keySlot[slotNumber].decryptBundle.iv[1] = (NvU32) (channelCounter);
            }
            else if (ccMode == APM)
            {
                keySlot[slotNumber].decryptBundle.iv[2] = (NvU32) (channelCounter);
            }
            break;
        default:
            break;
    }
}

static NvU64
getChannelCounter
(
    NvU32          slotNumber,
    ROTATE_IV_TYPE rotateOperation  // Will only be ROTATE_IV_ENCRYPT or ROTATE_IV_DECRYPT.
)
{
    NvU64 channelCounter = 0;

    switch (rotateOperation)
    {
        case ROTATE_IV_ENCRYPT:
            if (ccMode == HCC)
            {
                channelCounter = CONCAT64(keySlot[slotNumber].encryptBundle.iv[2],
                    keySlot[slotNumber].encryptBundle.iv[1]);
            }
            else if (ccMode == APM)
            {
                channelCounter = keySlot[slotNumber].encryptBundle.iv[2];
            }
            break;
        case ROTATE_IV_DECRYPT:
            if (ccMode == HCC)
            {
                channelCounter = CONCAT64(keySlot[slotNumber].decryptBundle.iv[2],
                    keySlot[slotNumber].decryptBundle.iv[1]);
            }
            else if (ccMode == APM)
            {
                channelCounter = keySlot[slotNumber].decryptBundle.iv[2];
            }
            break;
        default:
            break;
    }

    return channelCounter;
}

static NvU64
getChannelMaxValue (void)
{
    switch (ccMode)
    {
        case HCC:
            return NV_U64_MAX;
        case APM:
            return NV_U32_MAX;
        default:
            return 0;
    }
}
