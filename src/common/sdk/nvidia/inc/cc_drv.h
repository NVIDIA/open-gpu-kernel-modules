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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      cc_drv.finn
//



#include "nvtypes.h"
#include "nvcfg_sdk.h"

// CLASS NV_CONF_COMPUTE
#define CC_AES_256_GCM_IV_SIZE_BYTES  (0xcU) /* finn: Evaluated from "(96 / 8)" */
#define CC_AES_256_GCM_IV_SIZE_DWORD  (0x3U) /* finn: Evaluated from "(CC_AES_256_GCM_IV_SIZE_BYTES / 4)" */
#define CC_AES_256_GCM_KEY_SIZE_BYTES (0x20U) /* finn: Evaluated from "(256 / 8)" */
#define CC_AES_256_GCM_KEY_SIZE_DWORD (0x8U) /* finn: Evaluated from "(CC_AES_256_GCM_KEY_SIZE_BYTES / 4)" */

#define CC_HMAC_NONCE_SIZE_BYTES      (0x20U) /* finn: Evaluated from "(256 / 8)" */
#define CC_HMAC_NONCE_SIZE_DWORD      (0x8U) /* finn: Evaluated from "(CC_HMAC_NONCE_SIZE_BYTES / 4)" */
#define CC_HMAC_KEY_SIZE_BYTES        (0x20U) /* finn: Evaluated from "(256 / 8)" */
#define CC_HMAC_KEY_SIZE_DWORD        (0x8U) /* finn: Evaluated from "(CC_HMAC_KEY_SIZE_BYTES / 4)" */


// Type is shared between CC control calls and RMKeyStore
typedef enum ROTATE_IV_TYPE {
    ROTATE_IV_ENCRYPT = 0,  // Rotate the IV for encryptBundle
    ROTATE_IV_DECRYPT = 1,  // Rotate the IV for decryptBundle
    ROTATE_IV_HMAC = 2,     // Rotate the IV for hmacBundle
    ROTATE_IV_ALL_VALID = 3, // Rotate the IV for all valid bundles in the KMB
} ROTATE_IV_TYPE;

// Status value written into NvNotification.Info16
typedef enum KEY_ROTATION_STATUS {
    KEY_ROTATION_STATUS_IDLE = 0,                    // Key rotation complete/not in progress
    KEY_ROTATION_STATUS_PENDING = 1,                 // RM is waiting for clients to report their channels are idle for key rotation
    KEY_ROTATION_STATUS_IN_PROGRESS = 2,             // Key rotation is in progress
    KEY_ROTATION_STATUS_FAILED_TIMEOUT = 3,          // Key rotation timeout failure, RM will RC non-idle channels
    KEY_ROTATION_STATUS_FAILED_THRESHOLD = 4,        // Key rotation failed because upper threshold was crossed, RM will RC non-idle channels
    KEY_ROTATION_STATUS_FAILED_ROTATION = 5,         // Internal RM failure while rotating keys for a certain channel, RM will RC the channel
    KEY_ROTATION_STATUS_PENDING_TIMER_SUSPENDED = 6, // Key rotation timer suspended waiting for kernel key rotation to complete
    KEY_ROTATION_STATUS_MAX_COUNT = 7,
} KEY_ROTATION_STATUS;

typedef struct CC_AES_CRYPTOBUNDLE {
    NvU32 iv[CC_AES_256_GCM_IV_SIZE_DWORD];
    NvU32 key[CC_AES_256_GCM_KEY_SIZE_DWORD];
    NvU32 ivMask[CC_AES_256_GCM_IV_SIZE_DWORD];
} CC_AES_CRYPTOBUNDLE;
typedef struct CC_AES_CRYPTOBUNDLE *PCC_AES_CRYPTOBUNDLE;

typedef struct CC_HMAC_CRYPTOBUNDLE {
    NvU32 nonce[CC_HMAC_NONCE_SIZE_DWORD];
    NvU32 key[CC_HMAC_KEY_SIZE_DWORD];
} CC_HMAC_CRYPTOBUNDLE;
typedef struct CC_HMAC_CRYPTOBUNDLE *PCC_HMAC_CRYPTOBUNDLE;

typedef struct CC_KMB {
    CC_AES_CRYPTOBUNDLE encryptBundle;           // Bundle of encyption material

    union {
        CC_HMAC_CRYPTOBUNDLE hmacBundle;  // HMAC bundle used for method stream authenticity
        CC_AES_CRYPTOBUNDLE  decryptBundle;   // Bundle of decryption material
    };
    NvBool bIsWorkLaunch;                        // False if decryption parameters are valid
} CC_KMB;
typedef struct CC_KMB *PCC_KMB;

typedef struct CC_CRYPTOBUNDLE_STATS {
    NV_DECLARE_ALIGNED(NvU64 numEncryptionsH2D, 8);
    NV_DECLARE_ALIGNED(NvU64 numEncryptionsD2H, 8);
    NV_DECLARE_ALIGNED(NvU64 bytesEncryptedH2D, 8);
    NV_DECLARE_ALIGNED(NvU64 bytesEncryptedD2H, 8);
} CC_CRYPTOBUNDLE_STATS;
typedef struct CC_CRYPTOBUNDLE_STATS *PCC_CRYPTOBUNDLE_STATS;

