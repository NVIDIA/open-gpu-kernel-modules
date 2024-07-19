/*******************************************************************************
    Copyright (c) 1993-2004 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/



#include "nvtypes.h"

#ifndef _clc9b5_h_
#define _clc9b5_h_

#ifdef __cplusplus
extern "C" {
#endif

#define BLACKWELL_DMA_COPY_A                                                            (0x0000C9B5)

typedef volatile struct _clc9b5_tag0 {
    NvV32 Reserved00[0x40];
    NvV32 Nop;                                                                  // 0x00000100 - 0x00000103
    NvV32 Reserved01[0xF];
    NvV32 PmTrigger;                                                            // 0x00000140 - 0x00000143
    NvV32 Reserved02[0x36];
    NvV32 SetMonitoredFenceType;                                                // 0x0000021C - 0x0000021F
    NvV32 SetMonitoredFenceSignalAddrBaseUpper;                                 // 0x00000220 - 0x00000223
    NvV32 SetMonitoredFenceSignalAddrBaseLower;                                 // 0x00000224 - 0x00000227
    NvV32 Reserved03[0x6];
    NvV32 SetSemaphoreA;                                                        // 0x00000240 - 0x00000243
    NvV32 SetSemaphoreB;                                                        // 0x00000244 - 0x00000247
    NvV32 SetSemaphorePayload;                                                  // 0x00000248 - 0x0000024B
    NvV32 SetSemaphorePayloadUpper;                                             // 0x0000024C - 0x0000024F
    NvV32 Reserved04[0x1];
    NvV32 SetRenderEnableA;                                                     // 0x00000254 - 0x00000257
    NvV32 SetRenderEnableB;                                                     // 0x00000258 - 0x0000025B
    NvV32 SetRenderEnableC;                                                     // 0x0000025C - 0x0000025F
    NvV32 SetSrcPhysMode;                                                       // 0x00000260 - 0x00000263
    NvV32 SetDstPhysMode;                                                       // 0x00000264 - 0x00000267
    NvV32 Reserved05[0x26];
    NvV32 LaunchDma;                                                            // 0x00000300 - 0x00000303
    NvV32 Reserved06[0x3F];
    NvV32 OffsetInUpper;                                                        // 0x00000400 - 0x00000403
    NvV32 OffsetInLower;                                                        // 0x00000404 - 0x00000407
    NvV32 OffsetOutUpper;                                                       // 0x00000408 - 0x0000040B
    NvV32 OffsetOutLower;                                                       // 0x0000040C - 0x0000040F
    NvV32 PitchIn;                                                              // 0x00000410 - 0x00000413
    NvV32 PitchOut;                                                             // 0x00000414 - 0x00000417
    NvV32 LineLengthIn;                                                         // 0x00000418 - 0x0000041B
    NvV32 LineCount;                                                            // 0x0000041C - 0x0000041F
    NvV32 Reserved07[0x38];
    NvV32 SetSecureCopyMode;                                                    // 0x00000500 - 0x00000503
    NvV32 SetDecryptIv0;                                                        // 0x00000504 - 0x00000507
    NvV32 SetDecryptIv1;                                                        // 0x00000508 - 0x0000050B
    NvV32 SetDecryptIv2;                                                        // 0x0000050C - 0x0000050F
    NvV32 Reserved_SetAESCounter;                                               // 0x00000510 - 0x00000513
    NvV32 SetDecryptAuthTagCompareAddrUpper;                                    // 0x00000514 - 0x00000517
    NvV32 SetDecryptAuthTagCompareAddrLower;                                    // 0x00000518 - 0x0000051B
    NvV32 Reserved08[0x5];
    NvV32 SetEncryptAuthTagAddrUpper;                                           // 0x00000530 - 0x00000533
    NvV32 SetEncryptAuthTagAddrLower;                                           // 0x00000534 - 0x00000537
    NvV32 SetEncryptIvAddrUpper;                                                // 0x00000538 - 0x0000053B
    NvV32 SetEncryptIvAddrLower;                                                // 0x0000053C - 0x0000053F
    NvV32 Reserved09[0x10];
    NvV32 SetCompressionParameters;                                             // 0x00000580 - 0x00000583
    NvV32 SetDecompressOutLength;                                               // 0x00000584 - 0x00000587
    NvV32 SetDecompressOutLengthAddrUpper;                                      // 0x00000588 - 0x0000058B
    NvV32 SetDecompressOutLengthAddrLower;                                      // 0x0000058C - 0x0000058F
    NvV32 SetDecompressChecksum;                                                // 0x00000590 - 0x00000593
    NvV32 Reserved10[0x5A];
    NvV32 SetMemoryScrubParameters;                                             // 0x000006FC - 0x000006FF
    NvV32 SetRemapConstA;                                                       // 0x00000700 - 0x00000703
    NvV32 SetRemapConstB;                                                       // 0x00000704 - 0x00000707
    NvV32 SetRemapComponents;                                                   // 0x00000708 - 0x0000070B
    NvV32 SetDstBlockSize;                                                      // 0x0000070C - 0x0000070F
    NvV32 SetDstWidth;                                                          // 0x00000710 - 0x00000713
    NvV32 SetDstHeight;                                                         // 0x00000714 - 0x00000717
    NvV32 SetDstDepth;                                                          // 0x00000718 - 0x0000071B
    NvV32 SetDstLayer;                                                          // 0x0000071C - 0x0000071F
    NvV32 SetDstOrigin;                                                         // 0x00000720 - 0x00000723
    NvV32 Reserved11[0x1];
    NvV32 SetSrcBlockSize;                                                      // 0x00000728 - 0x0000072B
    NvV32 SetSrcWidth;                                                          // 0x0000072C - 0x0000072F
    NvV32 SetSrcHeight;                                                         // 0x00000730 - 0x00000733
    NvV32 SetSrcDepth;                                                          // 0x00000734 - 0x00000737
    NvV32 SetSrcLayer;                                                          // 0x00000738 - 0x0000073B
    NvV32 SetSrcOrigin;                                                         // 0x0000073C - 0x0000073F
    NvV32 Reserved12[0x1];
    NvV32 SrcOriginX;                                                           // 0x00000744 - 0x00000747
    NvV32 SrcOriginY;                                                           // 0x00000748 - 0x0000074B
    NvV32 DstOriginX;                                                           // 0x0000074C - 0x0000074F
    NvV32 DstOriginY;                                                           // 0x00000750 - 0x00000753
    NvV32 Reserved13[0x270];
    NvV32 PmTriggerEnd;                                                         // 0x00001114 - 0x00001117
    NvV32 Reserved14[0x3BA];
} blackwell_dma_copy_aControlPio;

#define NVC9B5_NOP                                                              (0x00000100)
#define NVC9B5_NOP_PARAMETER                                                    31:0
#define NVC9B5_PM_TRIGGER                                                       (0x00000140)
#define NVC9B5_PM_TRIGGER_V                                                     31:0
#define NVC9B5_SET_MONITORED_FENCE_TYPE                                         (0x0000021C)
#define NVC9B5_SET_MONITORED_FENCE_TYPE_TYPE                                    0:0
#define NVC9B5_SET_MONITORED_FENCE_TYPE_TYPE_MONITORED_FENCE                    (0x00000000)
#define NVC9B5_SET_MONITORED_FENCE_TYPE_TYPE_MONITORED_FENCE_EXT                (0x00000001)
#define NVC9B5_SET_MONITORED_FENCE_SIGNAL_ADDR_BASE_UPPER                       (0x00000220)
#define NVC9B5_SET_MONITORED_FENCE_SIGNAL_ADDR_BASE_UPPER_UPPER                 24:0
#define NVC9B5_SET_MONITORED_FENCE_SIGNAL_ADDR_BASE_LOWER                       (0x00000224)
#define NVC9B5_SET_MONITORED_FENCE_SIGNAL_ADDR_BASE_LOWER_LOWER                 31:0
#define NVC9B5_SET_SEMAPHORE_A                                                  (0x00000240)
#define NVC9B5_SET_SEMAPHORE_A_UPPER                                            24:0
#define NVC9B5_SET_SEMAPHORE_B                                                  (0x00000244)
#define NVC9B5_SET_SEMAPHORE_B_LOWER                                            31:0
#define NVC9B5_SET_SEMAPHORE_PAYLOAD                                            (0x00000248)
#define NVC9B5_SET_SEMAPHORE_PAYLOAD_PAYLOAD                                    31:0
#define NVC9B5_SET_SEMAPHORE_PAYLOAD_UPPER                                      (0x0000024C)
#define NVC9B5_SET_SEMAPHORE_PAYLOAD_UPPER_PAYLOAD                              31:0
#define NVC9B5_SET_RENDER_ENABLE_A                                              (0x00000254)
#define NVC9B5_SET_RENDER_ENABLE_A_UPPER                                        24:0
#define NVC9B5_SET_RENDER_ENABLE_B                                              (0x00000258)
#define NVC9B5_SET_RENDER_ENABLE_B_LOWER                                        31:0
#define NVC9B5_SET_RENDER_ENABLE_C                                              (0x0000025C)
#define NVC9B5_SET_RENDER_ENABLE_C_MODE                                         2:0
#define NVC9B5_SET_RENDER_ENABLE_C_MODE_FALSE                                   (0x00000000)
#define NVC9B5_SET_RENDER_ENABLE_C_MODE_TRUE                                    (0x00000001)
#define NVC9B5_SET_RENDER_ENABLE_C_MODE_CONDITIONAL                             (0x00000002)
#define NVC9B5_SET_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                         (0x00000003)
#define NVC9B5_SET_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                     (0x00000004)
#define NVC9B5_SET_SRC_PHYS_MODE                                                (0x00000260)
#define NVC9B5_SET_SRC_PHYS_MODE_TARGET                                         1:0
#define NVC9B5_SET_SRC_PHYS_MODE_TARGET_LOCAL_FB                                (0x00000000)
#define NVC9B5_SET_SRC_PHYS_MODE_TARGET_COHERENT_SYSMEM                         (0x00000001)
#define NVC9B5_SET_SRC_PHYS_MODE_TARGET_NONCOHERENT_SYSMEM                      (0x00000002)
#define NVC9B5_SET_SRC_PHYS_MODE_TARGET_PEERMEM                                 (0x00000003)
#define NVC9B5_SET_SRC_PHYS_MODE_BASIC_KIND                                     5:2
#define NVC9B5_SET_SRC_PHYS_MODE_PEER_ID                                        8:6
#define NVC9B5_SET_SRC_PHYS_MODE_FLA                                            9:9
#define NVC9B5_SET_DST_PHYS_MODE                                                (0x00000264)
#define NVC9B5_SET_DST_PHYS_MODE_TARGET                                         1:0
#define NVC9B5_SET_DST_PHYS_MODE_TARGET_LOCAL_FB                                (0x00000000)
#define NVC9B5_SET_DST_PHYS_MODE_TARGET_COHERENT_SYSMEM                         (0x00000001)
#define NVC9B5_SET_DST_PHYS_MODE_TARGET_NONCOHERENT_SYSMEM                      (0x00000002)
#define NVC9B5_SET_DST_PHYS_MODE_TARGET_PEERMEM                                 (0x00000003)
#define NVC9B5_SET_DST_PHYS_MODE_BASIC_KIND                                     5:2
#define NVC9B5_SET_DST_PHYS_MODE_PEER_ID                                        8:6
#define NVC9B5_SET_DST_PHYS_MODE_FLA                                            9:9
#define NVC9B5_LAUNCH_DMA                                                       (0x00000300)
#define NVC9B5_LAUNCH_DMA_DATA_TRANSFER_TYPE                                    1:0
#define NVC9B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_NONE                               (0x00000000)
#define NVC9B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_PIPELINED                          (0x00000001)
#define NVC9B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_NON_PIPELINED                      (0x00000002)
#define NVC9B5_LAUNCH_DMA_FLUSH_ENABLE                                          2:2
#define NVC9B5_LAUNCH_DMA_FLUSH_ENABLE_FALSE                                    (0x00000000)
#define NVC9B5_LAUNCH_DMA_FLUSH_ENABLE_TRUE                                     (0x00000001)
#define NVC9B5_LAUNCH_DMA_FLUSH_TYPE                                            25:25
#define NVC9B5_LAUNCH_DMA_FLUSH_TYPE_SYS                                        (0x00000000)
#define NVC9B5_LAUNCH_DMA_FLUSH_TYPE_GL                                         (0x00000001)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_TYPE                                        4:3
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_TYPE_NONE                                   (0x00000000)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_SEMAPHORE_NO_TIMESTAMP         (0x00000001)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_SEMAPHORE_WITH_TIMESTAMP       (0x00000002)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_ONE_WORD_SEMAPHORE             (0x00000001)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_FOUR_WORD_SEMAPHORE            (0x00000002)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_CONDITIONAL_INTR_SEMAPHORE     (0x00000003)
#define NVC9B5_LAUNCH_DMA_INTERRUPT_TYPE                                        6:5
#define NVC9B5_LAUNCH_DMA_INTERRUPT_TYPE_NONE                                   (0x00000000)
#define NVC9B5_LAUNCH_DMA_INTERRUPT_TYPE_BLOCKING                               (0x00000001)
#define NVC9B5_LAUNCH_DMA_INTERRUPT_TYPE_NON_BLOCKING                           (0x00000002)
#define NVC9B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT                                     7:7
#define NVC9B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_BLOCKLINEAR                         (0x00000000)
#define NVC9B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVC9B5_LAUNCH_DMA_DST_MEMORY_LAYOUT                                     8:8
#define NVC9B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_BLOCKLINEAR                         (0x00000000)
#define NVC9B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVC9B5_LAUNCH_DMA_MULTI_LINE_ENABLE                                     9:9
#define NVC9B5_LAUNCH_DMA_MULTI_LINE_ENABLE_FALSE                               (0x00000000)
#define NVC9B5_LAUNCH_DMA_MULTI_LINE_ENABLE_TRUE                                (0x00000001)
#define NVC9B5_LAUNCH_DMA_REMAP_ENABLE                                          10:10
#define NVC9B5_LAUNCH_DMA_REMAP_ENABLE_FALSE                                    (0x00000000)
#define NVC9B5_LAUNCH_DMA_REMAP_ENABLE_TRUE                                     (0x00000001)
#define NVC9B5_LAUNCH_DMA_COMPRESSION_ENABLE                                    11:11
#define NVC9B5_LAUNCH_DMA_COMPRESSION_ENABLE_FALSE                              (0x00000000)
#define NVC9B5_LAUNCH_DMA_COMPRESSION_ENABLE_TRUE                               (0x00000001)
#define NVC9B5_LAUNCH_DMA_SRC_TYPE                                              12:12
#define NVC9B5_LAUNCH_DMA_SRC_TYPE_VIRTUAL                                      (0x00000000)
#define NVC9B5_LAUNCH_DMA_SRC_TYPE_PHYSICAL                                     (0x00000001)
#define NVC9B5_LAUNCH_DMA_DST_TYPE                                              13:13
#define NVC9B5_LAUNCH_DMA_DST_TYPE_VIRTUAL                                      (0x00000000)
#define NVC9B5_LAUNCH_DMA_DST_TYPE_PHYSICAL                                     (0x00000001)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION                                   17:14
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IMIN                              (0x00000000)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IMAX                              (0x00000001)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IXOR                              (0x00000002)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IAND                              (0x00000003)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IOR                               (0x00000004)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IADD                              (0x00000005)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_INC                               (0x00000006)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_DEC                               (0x00000007)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_INVALIDA                          (0x00000008)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_INVALIDB                          (0x00000009)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_FADD                              (0x0000000A)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_FMIN                              (0x0000000B)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_FMAX                              (0x0000000C)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_INVALIDC                          (0x0000000D)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_INVALIDD                          (0x0000000E)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_INVALIDE                          (0x0000000F)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_SIGN                              18:18
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_SIGN_SIGNED                       (0x00000000)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_SIGN_UNSIGNED                     (0x00000001)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_ENABLE                            19:19
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_ENABLE_FALSE                      (0x00000000)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_ENABLE_TRUE                       (0x00000001)
#define NVC9B5_LAUNCH_DMA_COPY_TYPE                                             21:20
#define NVC9B5_LAUNCH_DMA_COPY_TYPE_PROT2PROT                                   (0x00000000)
#define NVC9B5_LAUNCH_DMA_COPY_TYPE_DEFAULT                                     (0x00000000)
#define NVC9B5_LAUNCH_DMA_COPY_TYPE_SECURE                                      (0x00000001)
#define NVC9B5_LAUNCH_DMA_COPY_TYPE_NONPROT2NONPROT                             (0x00000002)
#define NVC9B5_LAUNCH_DMA_COPY_TYPE_RESERVED                                    (0x00000003)
#define NVC9B5_LAUNCH_DMA_VPRMODE                                               22:22
#define NVC9B5_LAUNCH_DMA_VPRMODE_VPR_NONE                                      (0x00000000)
#define NVC9B5_LAUNCH_DMA_VPRMODE_VPR_VID2VID                                   (0x00000001)
#define NVC9B5_LAUNCH_DMA_MEMORY_SCRUB_ENABLE                                   23:23
#define NVC9B5_LAUNCH_DMA_MEMORY_SCRUB_ENABLE_FALSE                             (0x00000000)
#define NVC9B5_LAUNCH_DMA_MEMORY_SCRUB_ENABLE_TRUE                              (0x00000001)
#define NVC9B5_LAUNCH_DMA_RESERVED_START_OF_COPY                                24:24
#define NVC9B5_LAUNCH_DMA_DISABLE_PLC                                           26:26
#define NVC9B5_LAUNCH_DMA_DISABLE_PLC_FALSE                                     (0x00000000)
#define NVC9B5_LAUNCH_DMA_DISABLE_PLC_TRUE                                      (0x00000001)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_PAYLOAD_SIZE                                27:27
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_PAYLOAD_SIZE_ONE_WORD                       (0x00000000)
#define NVC9B5_LAUNCH_DMA_SEMAPHORE_PAYLOAD_SIZE_TWO_WORD                       (0x00000001)
#define NVC9B5_LAUNCH_DMA_RESERVED_ERR_CODE                                     31:28
#define NVC9B5_OFFSET_IN_UPPER                                                  (0x00000400)
#define NVC9B5_OFFSET_IN_UPPER_UPPER                                            24:0
#define NVC9B5_OFFSET_IN_LOWER                                                  (0x00000404)
#define NVC9B5_OFFSET_IN_LOWER_VALUE                                            31:0
#define NVC9B5_OFFSET_OUT_UPPER                                                 (0x00000408)
#define NVC9B5_OFFSET_OUT_UPPER_UPPER                                           24:0
#define NVC9B5_OFFSET_OUT_LOWER                                                 (0x0000040C)
#define NVC9B5_OFFSET_OUT_LOWER_VALUE                                           31:0
#define NVC9B5_PITCH_IN                                                         (0x00000410)
#define NVC9B5_PITCH_IN_VALUE                                                   31:0
#define NVC9B5_PITCH_OUT                                                        (0x00000414)
#define NVC9B5_PITCH_OUT_VALUE                                                  31:0
#define NVC9B5_LINE_LENGTH_IN                                                   (0x00000418)
#define NVC9B5_LINE_LENGTH_IN_VALUE                                             31:0
#define NVC9B5_LINE_COUNT                                                       (0x0000041C)
#define NVC9B5_LINE_COUNT_VALUE                                                 31:0
#define NVC9B5_SET_SECURE_COPY_MODE                                             (0x00000500)
#define NVC9B5_SET_SECURE_COPY_MODE_MODE                                        0:0
#define NVC9B5_SET_SECURE_COPY_MODE_MODE_ENCRYPT                                (0x00000000)
#define NVC9B5_SET_SECURE_COPY_MODE_MODE_DECRYPT                                (0x00000001)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_SRC_TARGET                         20:19
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_SRC_TARGET_LOCAL_FB                (0x00000000)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_SRC_TARGET_COHERENT_SYSMEM         (0x00000001)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_SRC_TARGET_NONCOHERENT_SYSMEM      (0x00000002)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_SRC_TARGET_PEERMEM                 (0x00000003)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_SRC_PEER_ID                        23:21
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_SRC_FLA                            24:24
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_DST_TARGET                         26:25
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_DST_TARGET_LOCAL_FB                (0x00000000)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_DST_TARGET_COHERENT_SYSMEM         (0x00000001)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_DST_TARGET_NONCOHERENT_SYSMEM      (0x00000002)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_DST_TARGET_PEERMEM                 (0x00000003)
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_DST_PEER_ID                        29:27
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_DST_FLA                            30:30
#define NVC9B5_SET_SECURE_COPY_MODE_RESERVED_END_OF_COPY                        31:31
#define NVC9B5_SET_DECRYPT_IV0                                                  (0x00000504)
#define NVC9B5_SET_DECRYPT_IV0_VALUE                                            31:0
#define NVC9B5_SET_DECRYPT_IV1                                                  (0x00000508)
#define NVC9B5_SET_DECRYPT_IV1_VALUE                                            31:0
#define NVC9B5_SET_DECRYPT_IV2                                                  (0x0000050C)
#define NVC9B5_SET_DECRYPT_IV2_VALUE                                            31:0
#define NVC9B5_RESERVED_SET_AESCOUNTER                                          (0x00000510)
#define NVC9B5_RESERVED_SET_AESCOUNTER_VALUE                                    31:0
#define NVC9B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_UPPER                          (0x00000514)
#define NVC9B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_UPPER_UPPER                    24:0
#define NVC9B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_LOWER                          (0x00000518)
#define NVC9B5_SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_LOWER_LOWER                    31:0
#define NVC9B5_SET_ENCRYPT_AUTH_TAG_ADDR_UPPER                                  (0x00000530)
#define NVC9B5_SET_ENCRYPT_AUTH_TAG_ADDR_UPPER_UPPER                            24:0
#define NVC9B5_SET_ENCRYPT_AUTH_TAG_ADDR_LOWER                                  (0x00000534)
#define NVC9B5_SET_ENCRYPT_AUTH_TAG_ADDR_LOWER_LOWER                            31:0
#define NVC9B5_SET_ENCRYPT_IV_ADDR_UPPER                                        (0x00000538)
#define NVC9B5_SET_ENCRYPT_IV_ADDR_UPPER_UPPER                                  24:0
#define NVC9B5_SET_ENCRYPT_IV_ADDR_LOWER                                        (0x0000053C)
#define NVC9B5_SET_ENCRYPT_IV_ADDR_LOWER_LOWER                                  31:0
#define NVC9B5_SET_COMPRESSION_PARAMETERS                                       (0x00000580)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_OPERATION                             0:0
#define NVC9B5_SET_COMPRESSION_PARAMETERS_OPERATION_DECOMPRESS                  (0x00000000)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_OPERATION_COMPRESS                    (0x00000001)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_ALGO                                  3:1
#define NVC9B5_SET_COMPRESSION_PARAMETERS_ALGO_SNAPPY                           (0x00000000)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_ALGO_LZ4_DATA_ONLY                    (0x00000001)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_ALGO_LZ4_BLOCK                        (0x00000002)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_ALGO_LZ4_BLOCK_CHECKSUM               (0x00000003)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_ALGO_DEFLATE                          (0x00000004)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_ALGO_SNAPPY_WITH_LONG_FETCH           (0x00000005)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_CHECK_SUM                             29:28
#define NVC9B5_SET_COMPRESSION_PARAMETERS_CHECK_SUM_NONE                        (0x00000000)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_CHECK_SUM_ADLER32                     (0x00000001)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_CHECK_SUM_CRC32                       (0x00000002)
#define NVC9B5_SET_COMPRESSION_PARAMETERS_CHECK_SUM_SNAPPY_CRC                  (0x00000003)
#define NVC9B5_SET_DECOMPRESS_OUT_LENGTH                                        (0x00000584)
#define NVC9B5_SET_DECOMPRESS_OUT_LENGTH_V                                      31:0
#define NVC9B5_SET_DECOMPRESS_OUT_LENGTH_ADDR_UPPER                             (0x00000588)
#define NVC9B5_SET_DECOMPRESS_OUT_LENGTH_ADDR_UPPER_UPPER                       24:0
#define NVC9B5_SET_DECOMPRESS_OUT_LENGTH_ADDR_LOWER                             (0x0000058C)
#define NVC9B5_SET_DECOMPRESS_OUT_LENGTH_ADDR_LOWER_LOWER                       31:0
#define NVC9B5_SET_DECOMPRESS_CHECKSUM                                          (0x00000590)
#define NVC9B5_SET_DECOMPRESS_CHECKSUM_V                                        31:0
#define NVC9B5_SET_MEMORY_SCRUB_PARAMETERS                                      (0x000006FC)
#define NVC9B5_SET_MEMORY_SCRUB_PARAMETERS_DISCARDABLE                          0:0
#define NVC9B5_SET_MEMORY_SCRUB_PARAMETERS_DISCARDABLE_FALSE                    (0x00000000)
#define NVC9B5_SET_MEMORY_SCRUB_PARAMETERS_DISCARDABLE_TRUE                     (0x00000001)
#define NVC9B5_SET_REMAP_CONST_A                                                (0x00000700)
#define NVC9B5_SET_REMAP_CONST_A_V                                              31:0
#define NVC9B5_SET_REMAP_CONST_B                                                (0x00000704)
#define NVC9B5_SET_REMAP_CONST_B_V                                              31:0
#define NVC9B5_SET_REMAP_COMPONENTS                                             (0x00000708)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X                                       2:0
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X_SRC_X                                 (0x00000000)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X_SRC_Y                                 (0x00000001)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X_SRC_Z                                 (0x00000002)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X_SRC_W                                 (0x00000003)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X_CONST_A                               (0x00000004)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X_CONST_B                               (0x00000005)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_X_NO_WRITE                              (0x00000006)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y                                       6:4
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y_SRC_X                                 (0x00000000)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Y                                 (0x00000001)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Z                                 (0x00000002)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y_SRC_W                                 (0x00000003)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y_CONST_A                               (0x00000004)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y_CONST_B                               (0x00000005)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Y_NO_WRITE                              (0x00000006)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z                                       10:8
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z_SRC_X                                 (0x00000000)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Y                                 (0x00000001)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Z                                 (0x00000002)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z_SRC_W                                 (0x00000003)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z_CONST_A                               (0x00000004)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z_CONST_B                               (0x00000005)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_Z_NO_WRITE                              (0x00000006)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W                                       14:12
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W_SRC_X                                 (0x00000000)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W_SRC_Y                                 (0x00000001)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W_SRC_Z                                 (0x00000002)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W_SRC_W                                 (0x00000003)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W_CONST_A                               (0x00000004)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W_CONST_B                               (0x00000005)
#define NVC9B5_SET_REMAP_COMPONENTS_DST_W_NO_WRITE                              (0x00000006)
#define NVC9B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE                              17:16
#define NVC9B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_ONE                          (0x00000000)
#define NVC9B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_TWO                          (0x00000001)
#define NVC9B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_THREE                        (0x00000002)
#define NVC9B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_FOUR                         (0x00000003)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS                          21:20
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_ONE                      (0x00000000)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_TWO                      (0x00000001)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_THREE                    (0x00000002)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_FOUR                     (0x00000003)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS                          25:24
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_ONE                      (0x00000000)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_TWO                      (0x00000001)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_THREE                    (0x00000002)
#define NVC9B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_FOUR                     (0x00000003)
#define NVC9B5_SET_DST_BLOCK_SIZE                                               (0x0000070C)
#define NVC9B5_SET_DST_BLOCK_SIZE_WIDTH                                         3:0
#define NVC9B5_SET_DST_BLOCK_SIZE_WIDTH_ONE_GOB                                 (0x00000000)
#define NVC9B5_SET_DST_BLOCK_SIZE_HEIGHT                                        7:4
#define NVC9B5_SET_DST_BLOCK_SIZE_HEIGHT_ONE_GOB                                (0x00000000)
#define NVC9B5_SET_DST_BLOCK_SIZE_HEIGHT_TWO_GOBS                               (0x00000001)
#define NVC9B5_SET_DST_BLOCK_SIZE_HEIGHT_FOUR_GOBS                              (0x00000002)
#define NVC9B5_SET_DST_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                             (0x00000003)
#define NVC9B5_SET_DST_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                           (0x00000004)
#define NVC9B5_SET_DST_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                         (0x00000005)
#define NVC9B5_SET_DST_BLOCK_SIZE_DEPTH                                         11:8
#define NVC9B5_SET_DST_BLOCK_SIZE_DEPTH_ONE_GOB                                 (0x00000000)
#define NVC9B5_SET_DST_BLOCK_SIZE_DEPTH_TWO_GOBS                                (0x00000001)
#define NVC9B5_SET_DST_BLOCK_SIZE_DEPTH_FOUR_GOBS                               (0x00000002)
#define NVC9B5_SET_DST_BLOCK_SIZE_DEPTH_EIGHT_GOBS                              (0x00000003)
#define NVC9B5_SET_DST_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                            (0x00000004)
#define NVC9B5_SET_DST_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                          (0x00000005)
#define NVC9B5_SET_DST_BLOCK_SIZE_GOB_HEIGHT                                    15:12
#define NVC9B5_SET_DST_BLOCK_SIZE_GOB_HEIGHT_GOB_HEIGHT_FERMI_8                 (0x00000001)
#define NVC9B5_SET_DST_WIDTH                                                    (0x00000710)
#define NVC9B5_SET_DST_WIDTH_V                                                  31:0
#define NVC9B5_SET_DST_HEIGHT                                                   (0x00000714)
#define NVC9B5_SET_DST_HEIGHT_V                                                 31:0
#define NVC9B5_SET_DST_DEPTH                                                    (0x00000718)
#define NVC9B5_SET_DST_DEPTH_V                                                  31:0
#define NVC9B5_SET_DST_LAYER                                                    (0x0000071C)
#define NVC9B5_SET_DST_LAYER_V                                                  31:0
#define NVC9B5_SET_DST_ORIGIN                                                   (0x00000720)
#define NVC9B5_SET_DST_ORIGIN_X                                                 15:0
#define NVC9B5_SET_DST_ORIGIN_Y                                                 31:16
#define NVC9B5_SET_SRC_BLOCK_SIZE                                               (0x00000728)
#define NVC9B5_SET_SRC_BLOCK_SIZE_WIDTH                                         3:0
#define NVC9B5_SET_SRC_BLOCK_SIZE_WIDTH_ONE_GOB                                 (0x00000000)
#define NVC9B5_SET_SRC_BLOCK_SIZE_HEIGHT                                        7:4
#define NVC9B5_SET_SRC_BLOCK_SIZE_HEIGHT_ONE_GOB                                (0x00000000)
#define NVC9B5_SET_SRC_BLOCK_SIZE_HEIGHT_TWO_GOBS                               (0x00000001)
#define NVC9B5_SET_SRC_BLOCK_SIZE_HEIGHT_FOUR_GOBS                              (0x00000002)
#define NVC9B5_SET_SRC_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                             (0x00000003)
#define NVC9B5_SET_SRC_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                           (0x00000004)
#define NVC9B5_SET_SRC_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                         (0x00000005)
#define NVC9B5_SET_SRC_BLOCK_SIZE_DEPTH                                         11:8
#define NVC9B5_SET_SRC_BLOCK_SIZE_DEPTH_ONE_GOB                                 (0x00000000)
#define NVC9B5_SET_SRC_BLOCK_SIZE_DEPTH_TWO_GOBS                                (0x00000001)
#define NVC9B5_SET_SRC_BLOCK_SIZE_DEPTH_FOUR_GOBS                               (0x00000002)
#define NVC9B5_SET_SRC_BLOCK_SIZE_DEPTH_EIGHT_GOBS                              (0x00000003)
#define NVC9B5_SET_SRC_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                            (0x00000004)
#define NVC9B5_SET_SRC_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                          (0x00000005)
#define NVC9B5_SET_SRC_BLOCK_SIZE_GOB_HEIGHT                                    15:12
#define NVC9B5_SET_SRC_BLOCK_SIZE_GOB_HEIGHT_GOB_HEIGHT_FERMI_8                 (0x00000001)
#define NVC9B5_SET_SRC_WIDTH                                                    (0x0000072C)
#define NVC9B5_SET_SRC_WIDTH_V                                                  31:0
#define NVC9B5_SET_SRC_HEIGHT                                                   (0x00000730)
#define NVC9B5_SET_SRC_HEIGHT_V                                                 31:0
#define NVC9B5_SET_SRC_DEPTH                                                    (0x00000734)
#define NVC9B5_SET_SRC_DEPTH_V                                                  31:0
#define NVC9B5_SET_SRC_LAYER                                                    (0x00000738)
#define NVC9B5_SET_SRC_LAYER_V                                                  31:0
#define NVC9B5_SET_SRC_ORIGIN                                                   (0x0000073C)
#define NVC9B5_SET_SRC_ORIGIN_X                                                 15:0
#define NVC9B5_SET_SRC_ORIGIN_Y                                                 31:16
#define NVC9B5_SRC_ORIGIN_X                                                     (0x00000744)
#define NVC9B5_SRC_ORIGIN_X_VALUE                                               31:0
#define NVC9B5_SRC_ORIGIN_Y                                                     (0x00000748)
#define NVC9B5_SRC_ORIGIN_Y_VALUE                                               31:0
#define NVC9B5_DST_ORIGIN_X                                                     (0x0000074C)
#define NVC9B5_DST_ORIGIN_X_VALUE                                               31:0
#define NVC9B5_DST_ORIGIN_Y                                                     (0x00000750)
#define NVC9B5_DST_ORIGIN_Y_VALUE                                               31:0
#define NVC9B5_PM_TRIGGER_END                                                   (0x00001114)
#define NVC9B5_PM_TRIGGER_END_V                                                 31:0

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _clc9b5_h

