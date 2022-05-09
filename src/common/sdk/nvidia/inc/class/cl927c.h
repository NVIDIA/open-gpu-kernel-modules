/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _cl927c_h_
#define _cl927c_h_


#ifdef __cplusplus
extern "C" {
#endif

#define NV927C_BASE_CHANNEL_DMA                                                 (0x0000927C)

#define NV_DISP_BASE_NOTIFIER_1                                                      0x00000000
#define NV_DISP_BASE_NOTIFIER_1_SIZEOF                                               0x00000004
#define NV_DISP_BASE_NOTIFIER_1__0                                                   0x00000000
#define NV_DISP_BASE_NOTIFIER_1__0_PRESENTATION_COUNT                                15:0
#define NV_DISP_BASE_NOTIFIER_1__0_TIMESTAMP                                         29:16
#define NV_DISP_BASE_NOTIFIER_1__0_STATUS                                            31:30
#define NV_DISP_BASE_NOTIFIER_1__0_STATUS_NOT_BEGUN                                  0x00000000
#define NV_DISP_BASE_NOTIFIER_1__0_STATUS_BEGUN                                      0x00000001
#define NV_DISP_BASE_NOTIFIER_1__0_STATUS_FINISHED                                   0x00000002


#define NV_DISP_NOTIFICATION_2                                                       0x00000000
#define NV_DISP_NOTIFICATION_2_SIZEOF                                                0x00000010
#define NV_DISP_NOTIFICATION_2_TIME_STAMP_0                                          0x00000000
#define NV_DISP_NOTIFICATION_2_TIME_STAMP_0_NANOSECONDS0                             31:0
#define NV_DISP_NOTIFICATION_2_TIME_STAMP_1                                          0x00000001
#define NV_DISP_NOTIFICATION_2_TIME_STAMP_1_NANOSECONDS1                             31:0
#define NV_DISP_NOTIFICATION_2_INFO32_2                                              0x00000002
#define NV_DISP_NOTIFICATION_2_INFO32_2_R0                                           31:0
#define NV_DISP_NOTIFICATION_2_INFO16_3                                              0x00000003
#define NV_DISP_NOTIFICATION_2_INFO16_3_PRESENT_COUNT                                7:0
#define NV_DISP_NOTIFICATION_2_INFO16_3_FIELD                                        8:8
#define NV_DISP_NOTIFICATION_2_INFO16_3_R1                                           15:9
#define NV_DISP_NOTIFICATION_2__3_STATUS                                             31:16
#define NV_DISP_NOTIFICATION_2__3_STATUS_NOT_BEGUN                                   0x00008000
#define NV_DISP_NOTIFICATION_2__3_STATUS_BEGUN                                       0x0000FFFF
#define NV_DISP_NOTIFICATION_2__3_STATUS_FINISHED                                    0x00000000


#define NV_DISP_NOTIFICATION_INFO16                                                  0x00000000
#define NV_DISP_NOTIFICATION_INFO16_SIZEOF                                           0x00000002
#define NV_DISP_NOTIFICATION_INFO16__0                                               0x00000000
#define NV_DISP_NOTIFICATION_INFO16__0_PRESENT_COUNT                                 7:0
#define NV_DISP_NOTIFICATION_INFO16__0_FIELD                                         8:8
#define NV_DISP_NOTIFICATION_INFO16__0_R1                                            15:9


#define NV_DISP_NOTIFICATION_STATUS                                                  0x00000000
#define NV_DISP_NOTIFICATION_STATUS_SIZEOF                                           0x00000002
#define NV_DISP_NOTIFICATION_STATUS__0                                               0x00000000
#define NV_DISP_NOTIFICATION_STATUS__0_STATUS                                        15:0
#define NV_DISP_NOTIFICATION_STATUS__0_STATUS_NOT_BEGUN                              0x00008000
#define NV_DISP_NOTIFICATION_STATUS__0_STATUS_BEGUN                                  0x0000FFFF
#define NV_DISP_NOTIFICATION_STATUS__0_STATUS_FINISHED                               0x00000000


// dma opcode instructions
#define NV927C_DMA                                                         0x00000000 
#define NV927C_DMA_OPCODE                                                       31:29 
#define NV927C_DMA_OPCODE_METHOD                                           0x00000000 
#define NV927C_DMA_OPCODE_JUMP                                             0x00000001 
#define NV927C_DMA_OPCODE_NONINC_METHOD                                    0x00000002 
#define NV927C_DMA_OPCODE_SET_SUBDEVICE_MASK                               0x00000003 
#define NV927C_DMA_OPCODE                                                       31:29 
#define NV927C_DMA_OPCODE_METHOD                                           0x00000000 
#define NV927C_DMA_OPCODE_NONINC_METHOD                                    0x00000002 
#define NV927C_DMA_METHOD_COUNT                                                 27:18 
#define NV927C_DMA_METHOD_OFFSET                                                 11:2 
#define NV927C_DMA_DATA                                                          31:0 
#define NV927C_DMA_DATA_NOP                                                0x00000000 
#define NV927C_DMA_OPCODE                                                       31:29 
#define NV927C_DMA_OPCODE_JUMP                                             0x00000001 
#define NV927C_DMA_JUMP_OFFSET                                                   11:2 
#define NV927C_DMA_OPCODE                                                       31:29 
#define NV927C_DMA_OPCODE_SET_SUBDEVICE_MASK                               0x00000003 
#define NV927C_DMA_SET_SUBDEVICE_MASK_VALUE                                      11:0 

// class methods
#define NV927C_PUT                                                              (0x00000000)
#define NV927C_PUT_PTR                                                          11:2
#define NV927C_GET                                                              (0x00000004)
#define NV927C_GET_PTR                                                          11:2
#define NV927C_GET_SCANLINE                                                     (0x00000010)
#define NV927C_GET_SCANLINE_LINE                                                15:0
#define NV927C_UPDATE                                                           (0x00000080)
#define NV927C_UPDATE_INTERLOCK_WITH_CORE                                       0:0
#define NV927C_UPDATE_INTERLOCK_WITH_CORE_DISABLE                               (0x00000000)
#define NV927C_UPDATE_INTERLOCK_WITH_CORE_ENABLE                                (0x00000001)
#define NV927C_UPDATE_SPECIAL_HANDLING                                          25:24
#define NV927C_UPDATE_SPECIAL_HANDLING_NONE                                     (0x00000000)
#define NV927C_UPDATE_SPECIAL_HANDLING_INTERRUPT_RM                             (0x00000001)
#define NV927C_UPDATE_SPECIAL_HANDLING_MODE_SWITCH                              (0x00000002)
#define NV927C_UPDATE_SPECIAL_HANDLING_REASON                                   23:16
#define NV927C_SET_PRESENT_CONTROL                                              (0x00000084)
#define NV927C_SET_PRESENT_CONTROL_BEGIN_MODE                                   9:8
#define NV927C_SET_PRESENT_CONTROL_BEGIN_MODE_NON_TEARING                       (0x00000000)
#define NV927C_SET_PRESENT_CONTROL_BEGIN_MODE_IMMEDIATE                         (0x00000001)
#define NV927C_SET_PRESENT_CONTROL_BEGIN_MODE_ON_LINE                           (0x00000002)
#define NV927C_SET_PRESENT_CONTROL_STEREO_FLIP_MODE                             3:3
#define NV927C_SET_PRESENT_CONTROL_STEREO_FLIP_MODE_PAIR_FLIP                   (0x00000000)
#define NV927C_SET_PRESENT_CONTROL_STEREO_FLIP_MODE_AT_ANY_FRAME                (0x00000001)
#define NV927C_SET_PRESENT_CONTROL_TIMESTAMP_MODE                               2:2
#define NV927C_SET_PRESENT_CONTROL_TIMESTAMP_MODE_DISABLE                       (0x00000000)
#define NV927C_SET_PRESENT_CONTROL_TIMESTAMP_MODE_ENABLE                        (0x00000001)
#define NV927C_SET_PRESENT_CONTROL_MIN_PRESENT_INTERVAL                         7:4
#define NV927C_SET_PRESENT_CONTROL_BEGIN_LINE                                   30:16
#define NV927C_SET_PRESENT_CONTROL_ON_LINE_MARGIN                               15:10
#define NV927C_SET_PRESENT_CONTROL_MODE                                         1:0
#define NV927C_SET_PRESENT_CONTROL_MODE_MONO                                    (0x00000000)
#define NV927C_SET_PRESENT_CONTROL_MODE_STEREO                                  (0x00000001)
#define NV927C_SET_PRESENT_CONTROL_MODE_SPEC_FLIP                               (0x00000002)
#define NV927C_SET_SEMAPHORE_CONTROL                                            (0x00000088)
#define NV927C_SET_SEMAPHORE_CONTROL_OFFSET                                     11:2
#define NV927C_SET_SEMAPHORE_CONTROL_DELAY                                      26:26
#define NV927C_SET_SEMAPHORE_CONTROL_DELAY_DISABLE                              (0x00000000)
#define NV927C_SET_SEMAPHORE_CONTROL_DELAY_ENABLE                               (0x00000001)
#define NV927C_SET_SEMAPHORE_CONTROL_FORMAT                                     28:28
#define NV927C_SET_SEMAPHORE_CONTROL_FORMAT_LEGACY                              (0x00000000)
#define NV927C_SET_SEMAPHORE_CONTROL_FORMAT_FOUR_WORD                           (0x00000001)
#define NV927C_SET_SEMAPHORE_ACQUIRE                                            (0x0000008C)
#define NV927C_SET_SEMAPHORE_ACQUIRE_VALUE                                      31:0
#define NV927C_SET_SEMAPHORE_RELEASE                                            (0x00000090)
#define NV927C_SET_SEMAPHORE_RELEASE_VALUE                                      31:0
#define NV927C_SET_CONTEXT_DMA_SEMAPHORE                                        (0x00000094)
#define NV927C_SET_CONTEXT_DMA_SEMAPHORE_HANDLE                                 31:0
#define NV927C_SET_NOTIFIER_CONTROL                                             (0x000000A0)
#define NV927C_SET_NOTIFIER_CONTROL_MODE                                        30:30
#define NV927C_SET_NOTIFIER_CONTROL_MODE_WRITE                                  (0x00000000)
#define NV927C_SET_NOTIFIER_CONTROL_MODE_WRITE_AWAKEN                           (0x00000001)
#define NV927C_SET_NOTIFIER_CONTROL_OFFSET                                      11:2
#define NV927C_SET_NOTIFIER_CONTROL_DELAY                                       26:26
#define NV927C_SET_NOTIFIER_CONTROL_DELAY_DISABLE                               (0x00000000)
#define NV927C_SET_NOTIFIER_CONTROL_DELAY_ENABLE                                (0x00000001)
#define NV927C_SET_NOTIFIER_CONTROL_FORMAT                                      28:28
#define NV927C_SET_NOTIFIER_CONTROL_FORMAT_LEGACY                               (0x00000000)
#define NV927C_SET_NOTIFIER_CONTROL_FORMAT_FOUR_WORD                            (0x00000001)
#define NV927C_SET_CONTEXT_DMA_NOTIFIER                                         (0x000000A4)
#define NV927C_SET_CONTEXT_DMA_NOTIFIER_HANDLE                                  31:0
#define NV927C_SET_CONTEXT_DMAS_ISO(b)                                          (0x000000C0 + (b)*0x00000004)
#define NV927C_SET_CONTEXT_DMAS_ISO_HANDLE                                      31:0
#define NV927C_SET_BASE_LUT_LO                                                  (0x000000E0)
#define NV927C_SET_BASE_LUT_LO_ENABLE                                           31:30
#define NV927C_SET_BASE_LUT_LO_ENABLE_DISABLE                                   (0x00000000)
#define NV927C_SET_BASE_LUT_LO_ENABLE_USE_CORE_LUT                              (0x00000001)
#define NV927C_SET_BASE_LUT_LO_ENABLE_ENABLE                                    (0x00000002)
#define NV927C_SET_BASE_LUT_LO_MODE                                             27:24
#define NV927C_SET_BASE_LUT_LO_MODE_LORES                                       (0x00000000)
#define NV927C_SET_BASE_LUT_LO_MODE_HIRES                                       (0x00000001)
#define NV927C_SET_BASE_LUT_LO_MODE_INDEX_1025_UNITY_RANGE                      (0x00000003)
#define NV927C_SET_BASE_LUT_LO_MODE_INTERPOLATE_1025_UNITY_RANGE                (0x00000004)
#define NV927C_SET_BASE_LUT_LO_MODE_INTERPOLATE_1025_XRBIAS_RANGE               (0x00000005)
#define NV927C_SET_BASE_LUT_LO_MODE_INTERPOLATE_1025_XVYCC_RANGE                (0x00000006)
#define NV927C_SET_BASE_LUT_LO_MODE_INTERPOLATE_257_UNITY_RANGE                 (0x00000007)
#define NV927C_SET_BASE_LUT_LO_MODE_INTERPOLATE_257_LEGACY_RANGE                (0x00000008)
#define NV927C_SET_BASE_LUT_HI                                                  (0x000000E4)
#define NV927C_SET_BASE_LUT_HI_ORIGIN                                           31:0
#define NV927C_SET_OUTPUT_LUT_LO                                                (0x000000E8)
#define NV927C_SET_OUTPUT_LUT_LO_ENABLE                                         31:30
#define NV927C_SET_OUTPUT_LUT_LO_ENABLE_DISABLE                                 (0x00000000)
#define NV927C_SET_OUTPUT_LUT_LO_ENABLE_USE_CORE_LUT                            (0x00000001)
#define NV927C_SET_OUTPUT_LUT_LO_ENABLE_ENABLE                                  (0x00000002)
#define NV927C_SET_OUTPUT_LUT_LO_MODE                                           27:24
#define NV927C_SET_OUTPUT_LUT_LO_MODE_LORES                                     (0x00000000)
#define NV927C_SET_OUTPUT_LUT_LO_MODE_HIRES                                     (0x00000001)
#define NV927C_SET_OUTPUT_LUT_LO_MODE_INDEX_1025_UNITY_RANGE                    (0x00000003)
#define NV927C_SET_OUTPUT_LUT_LO_MODE_INTERPOLATE_1025_UNITY_RANGE              (0x00000004)
#define NV927C_SET_OUTPUT_LUT_LO_MODE_INTERPOLATE_1025_XRBIAS_RANGE             (0x00000005)
#define NV927C_SET_OUTPUT_LUT_LO_MODE_INTERPOLATE_1025_XVYCC_RANGE              (0x00000006)
#define NV927C_SET_OUTPUT_LUT_LO_MODE_INTERPOLATE_257_UNITY_RANGE               (0x00000007)
#define NV927C_SET_OUTPUT_LUT_LO_MODE_INTERPOLATE_257_LEGACY_RANGE              (0x00000008)
#define NV927C_SET_OUTPUT_LUT_HI                                                (0x000000EC)
#define NV927C_SET_OUTPUT_LUT_HI_ORIGIN                                         31:0
#define NV927C_SET_CONTEXT_DMA_LUT                                              (0x000000FC)
#define NV927C_SET_CONTEXT_DMA_LUT_HANDLE                                       31:0
#define NV927C_SET_PROCESSING                                                   (0x00000110)
#define NV927C_SET_PROCESSING_USE_GAIN_OFS                                      0:0
#define NV927C_SET_PROCESSING_USE_GAIN_OFS_DISABLE                              (0x00000000)
#define NV927C_SET_PROCESSING_USE_GAIN_OFS_ENABLE                               (0x00000001)
#define NV927C_SET_CONVERSION_RED                                               (0x00000114)
#define NV927C_SET_CONVERSION_RED_GAIN                                          15:0
#define NV927C_SET_CONVERSION_RED_OFS                                           31:16
#define NV927C_SET_CONVERSION_GRN                                               (0x00000118)
#define NV927C_SET_CONVERSION_GRN_GAIN                                          15:0
#define NV927C_SET_CONVERSION_GRN_OFS                                           31:16
#define NV927C_SET_CONVERSION_BLU                                               (0x0000011C)
#define NV927C_SET_CONVERSION_BLU_GAIN                                          15:0
#define NV927C_SET_CONVERSION_BLU_OFS                                           31:16
#define NV927C_SET_TIMESTAMP_ORIGIN_LO                                          (0x00000130)
#define NV927C_SET_TIMESTAMP_ORIGIN_LO_TIMESTAMP_LO                             31:0
#define NV927C_SET_TIMESTAMP_ORIGIN_HI                                          (0x00000134)
#define NV927C_SET_TIMESTAMP_ORIGIN_HI_TIMESTAMP_HI                             31:0
#define NV927C_SET_UPDATE_TIMESTAMP_LO                                          (0x00000138)
#define NV927C_SET_UPDATE_TIMESTAMP_LO_TIMESTAMP_LO                             31:0
#define NV927C_SET_UPDATE_TIMESTAMP_HI                                          (0x0000013C)
#define NV927C_SET_UPDATE_TIMESTAMP_HI_TIMESTAMP_HI                             31:0
#define NV927C_SET_CSC_RED2RED                                                  (0x00000140)
#define NV927C_SET_CSC_RED2RED_OWNER                                            31:31
#define NV927C_SET_CSC_RED2RED_OWNER_CORE                                       (0x00000000)
#define NV927C_SET_CSC_RED2RED_OWNER_BASE                                       (0x00000001)
#define NV927C_SET_CSC_RED2RED_COEFF                                            18:0
#define NV927C_SET_CSC_GRN2RED                                                  (0x00000144)
#define NV927C_SET_CSC_GRN2RED_COEFF                                            18:0
#define NV927C_SET_CSC_BLU2RED                                                  (0x00000148)
#define NV927C_SET_CSC_BLU2RED_COEFF                                            18:0
#define NV927C_SET_CSC_CONSTANT2RED                                             (0x0000014C)
#define NV927C_SET_CSC_CONSTANT2RED_COEFF                                       18:0
#define NV927C_SET_CSC_RED2GRN                                                  (0x00000150)
#define NV927C_SET_CSC_RED2GRN_COEFF                                            18:0
#define NV927C_SET_CSC_GRN2GRN                                                  (0x00000154)
#define NV927C_SET_CSC_GRN2GRN_COEFF                                            18:0
#define NV927C_SET_CSC_BLU2GRN                                                  (0x00000158)
#define NV927C_SET_CSC_BLU2GRN_COEFF                                            18:0
#define NV927C_SET_CSC_CONSTANT2GRN                                             (0x0000015C)
#define NV927C_SET_CSC_CONSTANT2GRN_COEFF                                       18:0
#define NV927C_SET_CSC_RED2BLU                                                  (0x00000160)
#define NV927C_SET_CSC_RED2BLU_COEFF                                            18:0
#define NV927C_SET_CSC_GRN2BLU                                                  (0x00000164)
#define NV927C_SET_CSC_GRN2BLU_COEFF                                            18:0
#define NV927C_SET_CSC_BLU2BLU                                                  (0x00000168)
#define NV927C_SET_CSC_BLU2BLU_COEFF                                            18:0
#define NV927C_SET_CSC_CONSTANT2BLU                                             (0x0000016C)
#define NV927C_SET_CSC_CONSTANT2BLU_COEFF                                       18:0
#define NV927C_SET_SPARE                                                        (0x000003BC)
#define NV927C_SET_SPARE_UNUSED                                                 31:0
#define NV927C_SET_SPARE_NOOP(b)                                                (0x000003C0 + (b)*0x00000004)
#define NV927C_SET_SPARE_NOOP_UNUSED                                            31:0

#define NV927C_SURFACE_SET_OFFSET(a,b)                                          (0x00000400 + (a)*0x00000020 + (b)*0x00000004)
#define NV927C_SURFACE_SET_OFFSET_ORIGIN                                        31:0
#define NV927C_SURFACE_SET_SIZE(a)                                              (0x00000408 + (a)*0x00000020)
#define NV927C_SURFACE_SET_SIZE_WIDTH                                           15:0
#define NV927C_SURFACE_SET_SIZE_HEIGHT                                          31:16
#define NV927C_SURFACE_SET_STORAGE(a)                                           (0x0000040C + (a)*0x00000020)
#define NV927C_SURFACE_SET_STORAGE_BLOCK_HEIGHT                                 3:0
#define NV927C_SURFACE_SET_STORAGE_BLOCK_HEIGHT_ONE_GOB                         (0x00000000)
#define NV927C_SURFACE_SET_STORAGE_BLOCK_HEIGHT_TWO_GOBS                        (0x00000001)
#define NV927C_SURFACE_SET_STORAGE_BLOCK_HEIGHT_FOUR_GOBS                       (0x00000002)
#define NV927C_SURFACE_SET_STORAGE_BLOCK_HEIGHT_EIGHT_GOBS                      (0x00000003)
#define NV927C_SURFACE_SET_STORAGE_BLOCK_HEIGHT_SIXTEEN_GOBS                    (0x00000004)
#define NV927C_SURFACE_SET_STORAGE_BLOCK_HEIGHT_THIRTYTWO_GOBS                  (0x00000005)
#define NV927C_SURFACE_SET_STORAGE_PITCH                                        20:8
#define NV927C_SURFACE_SET_STORAGE_MEMORY_LAYOUT                                24:24
#define NV927C_SURFACE_SET_STORAGE_MEMORY_LAYOUT_BLOCKLINEAR                    (0x00000000)
#define NV927C_SURFACE_SET_STORAGE_MEMORY_LAYOUT_PITCH                          (0x00000001)
#define NV927C_SURFACE_SET_PARAMS(a)                                            (0x00000410 + (a)*0x00000020)
#define NV927C_SURFACE_SET_PARAMS_FORMAT                                        15:8
#define NV927C_SURFACE_SET_PARAMS_FORMAT_I8                                     (0x0000001E)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_VOID16                                 (0x0000001F)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_VOID32                                 (0x0000002E)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_RF16_GF16_BF16_AF16                    (0x000000CA)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_A8R8G8B8                               (0x000000CF)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_A2B10G10R10                            (0x000000D1)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_X2BL10GL10RL10_XRBIAS                  (0x00000022)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_X2BL10GL10RL10_XVYCC                   (0x00000024)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_A8B8G8R8                               (0x000000D5)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_R5G6B5                                 (0x000000E8)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_A1R5G5B5                               (0x000000E9)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_R16_G16_B16_A16                        (0x000000C6)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_R16_G16_B16_A16_NVBIAS                 (0x00000023)
#define NV927C_SURFACE_SET_PARAMS_FORMAT_A2R10G10B10                            (0x000000DF)
#define NV927C_SURFACE_SET_PARAMS_SUPER_SAMPLE                                  1:0
#define NV927C_SURFACE_SET_PARAMS_SUPER_SAMPLE_X1_AA                            (0x00000000)
#define NV927C_SURFACE_SET_PARAMS_SUPER_SAMPLE_X4_AA                            (0x00000002)
#define NV927C_SURFACE_SET_PARAMS_GAMMA                                         2:2
#define NV927C_SURFACE_SET_PARAMS_GAMMA_LINEAR                                  (0x00000000)
#define NV927C_SURFACE_SET_PARAMS_GAMMA_SRGB                                    (0x00000001)
#define NV927C_SURFACE_SET_PARAMS_LAYOUT                                        5:4
#define NV927C_SURFACE_SET_PARAMS_LAYOUT_FRM                                    (0x00000000)
#define NV927C_SURFACE_SET_PARAMS_LAYOUT_FLD1                                   (0x00000001)
#define NV927C_SURFACE_SET_PARAMS_LAYOUT_FLD2                                   (0x00000002)

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _cl927c_h
