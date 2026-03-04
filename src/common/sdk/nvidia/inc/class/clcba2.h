/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvtypes.h"

#ifndef _clcba2_h_
#define _clcba2_h_

#ifdef __cplusplus
extern "C" {
#endif

#define HOPPER_SEC2_WORK_LAUNCH_A                                               (0x0000CBA2)

typedef volatile struct _clcba2_tag0 {
    NvV32 Reserved00[0x100];
    NvV32 DecryptCopySrcAddrHi;                                                 // 0x00000400 - 0x00000403
    NvV32 DecryptCopySrcAddrLo;                                                 // 0x00000404 - 0x00000407
    NvV32 DecryptCopyDstAddrHi;                                                 // 0x00000408 - 0x0000040B
    NvV32 DecryptCopyDstAddrLo;                                                 // 0x0000040c - 0x0000040F
    NvU32 DecryptCopySize;                                                      // 0x00000410 - 0x00000413
    NvU32 DecryptCopyAuthTagAddrHi;                                             // 0x00000414 - 0x00000417
    NvU32 DecryptCopyAuthTagAddrLo;                                             // 0x00000418 - 0x0000041B
    NvV32 DigestAddrHi;                                                         // 0x0000041C - 0x0000041F
    NvV32 DigestAddrLo;                                                         // 0x00000420 - 0x00000423
    NvV32 Reserved01[0x7];
    NvV32 SemaphoreA;                                                           // 0x00000440 - 0x00000443
    NvV32 SemaphoreB;                                                           // 0x00000444 - 0x00000447
    NvV32 SemaphoreSetPayloadLower;                                             // 0x00000448 - 0x0000044B
    NvV32 SemaphoreSetPayloadUppper;                                            // 0x0000044C - 0x0000044F
    NvV32 SemaphoreD;                                                           // 0x00000450 - 0x00000453
    NvU32 Reserved02[0x7];
    NvV32 Execute;                                                              // 0x00000470 - 0x00000473
    NvV32 Reserved03[0x23];
} NVCBA2_HOPPER_SEC2_WORK_LAUNCH_AControlPio;

#define NVCBA2_DECRYPT_COPY_SRC_ADDR_HI                                         (0x00000400)
#define NVCBA2_DECRYPT_COPY_SRC_ADDR_HI_DATA                                    24:0
#define NVCBA2_DECRYPT_COPY_SRC_ADDR_LO                                         (0x00000404)
#define NVCBA2_DECRYPT_COPY_SRC_ADDR_LO_DATA                                    31:4
#define NVCBA2_DECRYPT_COPY_DST_ADDR_HI                                         (0x00000408)
#define NVCBA2_DECRYPT_COPY_DST_ADDR_HI_DATA                                    24:0
#define NVCBA2_DECRYPT_COPY_DST_ADDR_LO                                         (0x0000040c)
#define NVCBA2_DECRYPT_COPY_DST_ADDR_LO_DATA                                    31:4
#define NVCBA2_DECRYPT_COPY_SIZE                                                (0x00000410)
#define NVCBA2_DECRYPT_COPY_SIZE_DATA                                           31:2
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_HI                                    (0x00000414)
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_HI_DATA                               24:0
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_LO                                    (0x00000418)
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_LO_DATA                               31:4
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_HI                                   (0x0000041C)
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_HI_DATA                              24:0
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_LO                                   (0x00000420)
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_LO_DATA                              31:4
#define NVCBA2_SEMAPHORE_A                                                      (0x00000440)
#define NVCBA2_SEMAPHORE_A_UPPER                                                24:0
#define NVCBA2_SEMAPHORE_B                                                      (0x00000444)
#define NVCBA2_SEMAPHORE_B_LOWER                                                31:2
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_LOWER                                      (0x00000448)
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_LOWER_DATA                                 31:0
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_UPPER                                      (0x0000044C)
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_UPPER_DATA                                 31:0
#define NVCBA2_SEMAPHORE_D                                                      (0x00000450)
#define NVCBA2_SEMAPHORE_D_NOTIFY_INTR                                          0:0
#define NVCBA2_SEMAPHORE_D_NOTIFY_INTR_DISABLE                                  (0x00000000)
#define NVCBA2_SEMAPHORE_D_NOTIFY_INTR_ENABLE                                   (0x00000001)
#define NVCBA2_SEMAPHORE_D_PAYLOAD_SIZE                                         1:1
#define NVCBA2_SEMAPHORE_D_PAYLOAD_SIZE_32_BIT                                  (0x00000000)
#define NVCBA2_SEMAPHORE_D_PAYLOAD_SIZE_64_BIT                                  (0x00000001)
#define NVCBA2_SEMAPHORE_D_TIMESTAMP                                            2:2
#define NVCBA2_SEMAPHORE_D_TIMESTAMP_DISABLE                                    (0x00000000)
#define NVCBA2_SEMAPHORE_D_TIMESTAMP_ENABLE                                     (0x00000001)
#define NVCBA2_SEMAPHORE_D_FLUSH_DISABLE                                        3:3
#define NVCBA2_SEMAPHORE_D_FLUSH_DISABLE_FALSE                                  (0x00000000)
#define NVCBA2_SEMAPHORE_D_FLUSH_DISABLE_TRUE                                   (0x00000001)
#define NVCBA2_EXECUTE                                                          (0x00000470)
#define NVCBA2_EXECUTE_NOTIFY                                                   0:0
#define NVCBA2_EXECUTE_NOTIFY_DISABLE                                           (0x00000000)
#define NVCBA2_EXECUTE_NOTIFY_ENABLE                                            (0x00000001)
#define NVCBA2_EXECUTE_NOTIFY_ON                                                1:1
#define NVCBA2_EXECUTE_NOTIFY_ON_END                                            (0x00000000)
#define NVCBA2_EXECUTE_NOTIFY_ON_BEGIN                                          (0x00000001)
#define NVCBA2_EXECUTE_FLUSH_DISABLE                                            2:2
#define NVCBA2_EXECUTE_FLUSH_DISABLE_FALSE                                      (0x00000000)
#define NVCBA2_EXECUTE_FLUSH_DISABLE_TRUE                                       (0x00000001)
#define NVCBA2_EXECUTE_NOTIFY_INTR                                              3:3
#define NVCBA2_EXECUTE_NOTIFY_INTR_DISABLE                                      (0x00000000)
#define NVCBA2_EXECUTE_NOTIFY_INTR_ENABLE                                       (0x00000001)
#define NVCBA2_EXECUTE_PAYLOAD_SIZE                                             4:4
#define NVCBA2_EXECUTE_PAYLOAD_SIZE_32_BIT                                      (0x00000000)
#define NVCBA2_EXECUTE_PAYLOAD_SIZE_64_BIT                                      (0x00000001)
#define NVCBA2_EXECUTE_TIMESTAMP                                                5:5
#define NVCBA2_EXECUTE_TIMESTAMP_DISABLE                                        (0x00000000)
#define NVCBA2_EXECUTE_TIMESTAMP_ENABLE                                         (0x00000001)
#define NVCBA2_EXECUTE_PHYSICAL_SCRUBBER                                        6:6
#define NVCBA2_EXECUTE_PHYSICAL_SCRUBBER_DISABLE                                (0x00000000)
#define NVCBA2_EXECUTE_PHYSICAL_SCRUBBER_ENABLE                                 (0x00000001)

// Class definitions
#define NVCBA2_DECRYPT_COPY_SIZE_MAX_BYTES                                      (2*1024*1024)
#define NVCBA2_DECRYPT_SCRUB_SIZE_MAX_BYTES                                     (1024*1024*1024)

// Errors
#define NVCBA2_ERROR_NONE                                                       (0x00000000)
#define NVCBA2_ERROR_DECRYPT_COPY_SRC_ADDR_MISALIGNED_POINTER                   (0x00000001)
#define NVCBA2_ERROR_DECRYPT_COPY_DEST_ADDR_MISALIGNED_POINTER                  (0x00000002)
#define NVCBA2_ERROR_DECRYPT_COPY_AUTH_TAG_ADDR_MISALIGNED_POINTER              (0x00000003)
#define NVCBA2_ERROR_DECRYPT_COPY_DMA_NACK                                      (0x00000004)
#define NVCBA2_ERROR_DECRYPT_COPY_AUTH_TAG_MISMATCH                             (0x00000005)
#define NVCBA2_ERROR_METHOD_STREAM_AUTH_TAG_ADDR_MISALIGNED_POINTER             (0x00000006)
#define NVCBA2_ERROR_METHOD_STREAM_AUTH_TAG_ADDR_DMA_NACK                       (0x00000007)
#define NVCBA2_ERROR_METHOD_STREAM_AUTH_TAG_CHECK_FAILURE                       (0x00000008)
#define NVCBA2_ERROR_MISALIGNED_SIZE                                            (0x00000009)
#define NVCBA2_ERROR_MISSING_METHODS                                            (0x0000000A)
#define NVCBA2_ERROR_SEMAPHORE_RELEASE_DMA_NACK                                 (0x0000000B)
#define NVCBA2_ERROR_DECRYPT_SIZE_MAX_EXCEEDED                                  (0x0000000C)
#define NVCBA2_ERROR_OS_APPLICATION                                             (0x0000000D)
#define NVCBA2_ERROR_INVALID_CTXSW_REQUEST                                      (0x0000000E)
#define NVCBA2_ERROR_BUFFER_OVERFLOW                                            (0x0000000F)
#define NVCBA2_ERROR_IV_OVERFLOW                                                (0x00000010)
#define NVCBA2_ERROR_INTERNAL_SETUP_FAILURE                                     (0x00000011)
#define NVCBA2_ERROR_DECRYPT_COPY_INTERNAL_DMA_FAILURE                          (0x00000012)
#define NVCBA2_ERROR_METHOD_STREAM_AUTH_TAG_ADDR_INTERNAL_DMA_FAILURE           (0x00000013)
#define NVCBA2_ERROR_METHOD_STREAM_AUTH_TAG_HMAC_CALC_FAILURE                   (0x00000014)
#define NVCBA2_ERROR_NONCE_OVERFLOW                                             (0x00000015)
#define NVCBA2_ERROR_AES_GCM_DECRYPTION_FAILURE                                 (0x00000016)
#define NVCBA2_ERROR_SEMAPHORE_RELEASE_INTERNAL_DMA_FAILURE                     (0x00000017)
#define NVCBA2_ERROR_KEY_DERIVATION_FAILURE                                     (0x00000018)
#define NVCBA2_ERROR_SCRUBBER_FAILURE                                           (0x00000019)
#define NVCBA2_ERROR_SCRUBBER_INVALD_ADDRESS                                    (0x0000001a)
#define NVCBA2_ERROR_SCRUBBER_INSUFFICIENT_PERMISSIONS                          (0x0000001b)
#define NVCBA2_ERROR_SCRUBBER_MUTEX_ACQUIRE_FAILURE                             (0x0000001c)
#define NVCBA2_ERROR_SCRUB_SIZE_MAX_EXCEEDED                                    (0x0000001d)
#define NVCBA2_ERROR_SIZE_ZERO                                                  (0x0000001e)

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _clcba2_h
