/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_clc8b5_h__
#define __gh100_clc8b5_h__
#define HOPPER_DMA_COPY_A                                                            (0x0000C8B5)
#define NVC8B5_SET_SEMAPHORE_A                                                  (0x00000240)
#define NVC8B5_SET_SEMAPHORE_A_UPPER                                            24:0
#define NVC8B5_SET_SEMAPHORE_B                                                  (0x00000244)
#define NVC8B5_SET_SEMAPHORE_B_LOWER                                            31:0
#define NVC8B5_SET_SEMAPHORE_PAYLOAD                                            (0x00000248)
#define NVC8B5_SET_DST_PHYS_MODE                                                (0x00000264)
#define NVC8B5_SET_DST_PHYS_MODE_TARGET                                         1:0
#define NVC8B5_SET_DST_PHYS_MODE_TARGET_LOCAL_FB                                (0x00000000)
#define NVC8B5_LAUNCH_DMA                                                       (0x00000300)
#define NVC8B5_LAUNCH_DMA_DATA_TRANSFER_TYPE                                    1:0
#define NVC8B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_PIPELINED                          (0x00000001)
#define NVC8B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_NON_PIPELINED                      (0x00000002)
#define NVC8B5_LAUNCH_DMA_FLUSH_ENABLE                                          2:2
#define NVC8B5_LAUNCH_DMA_FLUSH_ENABLE_TRUE                                     (0x00000001)
#define NVC8B5_LAUNCH_DMA_SEMAPHORE_TYPE                                        4:3
#define NVC8B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_ONE_WORD_SEMAPHORE             (0x00000001)
#define NVC8B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT                                     7:7
#define NVC8B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVC8B5_LAUNCH_DMA_DST_MEMORY_LAYOUT                                     8:8
#define NVC8B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVC8B5_LAUNCH_DMA_MULTI_LINE_ENABLE                                     9:9
#define NVC8B5_LAUNCH_DMA_MULTI_LINE_ENABLE_FALSE                               (0x00000000)
#define NVC8B5_LAUNCH_DMA_REMAP_ENABLE                                          10:10
#define NVC8B5_LAUNCH_DMA_REMAP_ENABLE_FALSE                                    (0x00000000)
#define NVC8B5_LAUNCH_DMA_SRC_TYPE                                              12:12
#define NVC8B5_LAUNCH_DMA_SRC_TYPE_PHYSICAL                                     (0x00000001)
#define NVC8B5_LAUNCH_DMA_DST_TYPE                                              13:13
#define NVC8B5_LAUNCH_DMA_DST_TYPE_PHYSICAL                                     (0x00000001)
#define NVC8B5_LAUNCH_DMA_COPY_TYPE                                             21:20
#define NVC8B5_LAUNCH_DMA_COPY_TYPE_PROT2PROT                                   (0x00000000)
#define NVC8B5_LAUNCH_DMA_COPY_TYPE_DEFAULT                                     (0x00000000)
#define NVC8B5_LAUNCH_DMA_COPY_TYPE_SECURE                                      (0x00000001)
#define NVC8B5_LAUNCH_DMA_MEMORY_SCRUB_ENABLE                                   23:23
#define NVC8B5_LAUNCH_DMA_MEMORY_SCRUB_ENABLE_TRUE                              (0x00000001)
#define NVC8B5_LAUNCH_DMA_DISABLE_PLC                                           26:26
#define NVC8B5_LAUNCH_DMA_DISABLE_PLC_TRUE                                      (0x00000001)
#define NVC8B5_OFFSET_OUT_UPPER                                                 (0x00000408)
#define NVC8B5_OFFSET_OUT_UPPER_UPPER                                           24:0
#define NVC8B5_OFFSET_OUT_LOWER                                                 (0x0000040C)
#define NVC8B5_OFFSET_OUT_LOWER_VALUE                                           31:0
#define NVC8B5_LINE_LENGTH_IN                                                   (0x00000418)
#define NVC8B5_SET_SECURE_COPY_MODE                                             (0x00000500)
#define NVC8B5_SET_SECURE_COPY_MODE_MODE                                        0:0
#define NVC8B5_SET_SECURE_COPY_MODE_MODE_ENCRYPT                                (0x00000000)
#define NVC8B5_SET_SECURE_COPY_MODE_MODE_DECRYPT                                (0x00000001)
#define NVC8B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_UPPER                          (0x00000514)
#define NVC8B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_UPPER_UPPER                    24:0
#define NVC8B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_LOWER                          (0x00000518)
#define NVC8B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_LOWER_LOWER                    31:0
#define NVC8B5_SET_ENCRYPT_AUTH_TAG_ADDR_UPPER                                  (0x00000530)
#define NVC8B5_SET_ENCRYPT_AUTH_TAG_ADDR_UPPER_UPPER                            24:0
#define NVC8B5_SET_ENCRYPT_AUTH_TAG_ADDR_LOWER                                  (0x00000534)
#define NVC8B5_SET_ENCRYPT_AUTH_TAG_ADDR_LOWER_LOWER                            31:0
#define NVC8B5_SET_ENCRYPT_IV_ADDR_UPPER                                        (0x00000538)
#define NVC8B5_SET_ENCRYPT_IV_ADDR_UPPER_UPPER                                  24:0
#define NVC8B5_SET_ENCRYPT_IV_ADDR_LOWER                                        (0x0000053C)
#define NVC8B5_SET_ENCRYPT_IV_ADDR_LOWER_LOWER                                  31:0
#define NVC8B5_SET_MEMORY_SCRUB_PARAMETERS                                      (0x000006FC)
#define NVC8B5_SET_MEMORY_SCRUB_PARAMETERS_DISCARDABLE                          0:0
#define NVC8B5_SET_MEMORY_SCRUB_PARAMETERS_DISCARDABLE_FALSE                    (0x00000000)
#define NVC8B5_SET_REMAP_CONST_A                                                (0x00000700)
#define NVC8B5_SET_REMAP_CONST_B                                                (0x00000704)
#define NVC8B5_SET_REMAP_COMPONENTS                                             (0x00000708)
#define NVC8B5_SET_REMAP_COMPONENTS_DST_X                                       2:0
#define NVC8B5_SET_REMAP_COMPONENTS_DST_X_CONST_A                               (0x00000004)
#define NVC8B5_SET_REMAP_COMPONENTS_DST_X_CONST_B                               (0x00000005)
#define NVC8B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE                              17:16
#define NVC8B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_ONE                          (0x00000000)
#define NVC8B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS                          25:24
#define NVC8B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_ONE                      (0x00000000)
#endif // __gh100_clc8b5_h__
