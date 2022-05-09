/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl_fermi_twod_a_h_
#define _cl_fermi_twod_a_h_

#define FERMI_TWOD_A    0x902D

typedef volatile struct fermi_twod_a_struct {
    NvU32 SetObject;
    NvU32 Reserved_0x04[0x3F];
    NvU32 NoOperation;
    NvU32 SetNotifyA;
    NvU32 SetNotifyB;
    NvU32 Notify;
    NvU32 WaitForIdle;
    NvU32 LoadMmeInstructionRamPointer;
    NvU32 LoadMmeInstructionRam;
    NvU32 LoadMmeStartAddressRamPointer;
    NvU32 LoadMmeStartAddressRam;
    NvU32 SetMmeShadowRamControl;
    NvU32 Reserved_0x128[0x2];
    NvU32 SetGlobalRenderEnableA;
    NvU32 SetGlobalRenderEnableB;
    NvU32 SetGlobalRenderEnableC;
    NvU32 SendGoIdle;
    NvU32 PmTrigger;
    NvU32 Reserved_0x144[0x3];
    NvU32 SetInstrumentationMethodHeader;
    NvU32 SetInstrumentationMethodData;
    NvU32 Reserved_0x158[0x25];
    NvU32 SetMmeSwitchState;
    NvU32 Reserved_0x1F0[0x4];
    NvU32 SetDstFormat;
    NvU32 SetDstMemoryLayout;
    NvU32 SetDstBlockSize;
    NvU32 SetDstDepth;
    NvU32 SetDstLayer;
    NvU32 SetDstPitch;
    NvU32 SetDstWidth;
    NvU32 SetDstHeight;
    NvU32 SetDstOffsetUpper;
    NvU32 SetDstOffsetLower;
    NvU32 FlushAndInvalidateRopMiniCache;
    NvU32 SetSpareNoop06;
    NvU32 SetSrcFormat;
    NvU32 SetSrcMemoryLayout;
    NvU32 SetSrcBlockSize;
    NvU32 SetSrcDepth;
    NvU32 TwodInvalidateTextureDataCache;
    NvU32 SetSrcPitch;
    NvU32 SetSrcWidth;
    NvU32 SetSrcHeight;
    NvU32 SetSrcOffsetUpper;
    NvU32 SetSrcOffsetLower;
    NvU32 SetPixelsFromMemorySectorPromotion;
    NvU32 SetSpareNoop12;
    NvU32 SetNumProcessingClusters;
    NvU32 SetRenderEnableA;
    NvU32 SetRenderEnableB;
    NvU32 SetRenderEnableC;
    NvU32 SetSpareNoop08;
    NvU32 SetSpareNoop01;
    NvU32 SetSpareNoop11;
    NvU32 SetSpareNoop07;
    NvU32 SetClipX0;
    NvU32 SetClipY0;
    NvU32 SetClipWidth;
    NvU32 SetClipHeight;
    NvU32 SetClipEnable;
    NvU32 SetColorKeyFormat;
    NvU32 SetColorKey;
    NvU32 SetColorKeyEnable;
    NvU32 SetRop;
    NvU32 SetBeta1;
    NvU32 SetBeta4;
    NvU32 SetOperation;
    NvU32 SetPatternOffset;
    NvU32 SetPatternSelect;
    NvU32 SetDstColorRenderToZetaSurface;
    NvU32 SetSpareNoop04;
    NvU32 SetSpareNoop15;
    NvU32 SetSpareNoop13;
    NvU32 SetSpareNoop03;
    NvU32 SetSpareNoop14;
    NvU32 SetSpareNoop02;
    NvU32 SetCompression;
    NvU32 SetSpareNoop09;
    NvU32 SetRenderEnableOverride;
    NvU32 SetPixelsFromMemoryDirection;
    NvU32 SetSpareNoop10;
    NvU32 SetMonochromePatternColorFormat;
    NvU32 SetMonochromePatternFormat;
    NvU32 SetMonochromePatternColor0;
    NvU32 SetMonochromePatternColor1;
    NvU32 SetMonochromePattern0;
    NvU32 SetMonochromePattern1;
    NvU32 ColorPatternX8R8G8B8[0x40];
    NvU32 ColorPatternR5G6B5[0x20];
    NvU32 ColorPatternX1R5G5B5[0x20];
    NvU32 ColorPatternY8[0x10];
    NvU32 SetRenderSolidPrimColor0;
    NvU32 SetRenderSolidPrimColor1;
    NvU32 SetRenderSolidPrimColor2;
    NvU32 SetRenderSolidPrimColor3;
    NvU32 SetMmeMemAddressA;
    NvU32 SetMmeMemAddressB;
    NvU32 SetMmeDataRamAddress;
    NvU32 MmeDmaRead;
    NvU32 MmeDmaReadFifoed;
    NvU32 MmeDmaWrite;
    NvU32 MmeDmaReduction;
    NvU32 MmeDmaSysmembar;
    NvU32 MmeDmaSync;
    NvU32 SetMmeDataFifoConfig;
    NvU32 Reserved_0x578[0x2];
    NvU32 RenderSolidPrimMode;
    NvU32 SetRenderSolidPrimColorFormat;
    NvU32 SetRenderSolidPrimColor;
    NvU32 SetRenderSolidLineTieBreakBits;
    NvU32 Reserved_0x590[0x14];
    NvU32 RenderSolidPrimPointXY;
    NvU32 Reserved_0x5E4[0x7];
    struct {
        NvU32 SetX;
        NvU32 Y;
    } RenderSolidPrimPoint[0x40];
    NvU32 SetPixelsFromCpuDataType;
    NvU32 SetPixelsFromCpuColorFormat;
    NvU32 SetPixelsFromCpuIndexFormat;
    NvU32 SetPixelsFromCpuMonoFormat;
    NvU32 SetPixelsFromCpuWrap;
    NvU32 SetPixelsFromCpuColor0;
    NvU32 SetPixelsFromCpuColor1;
    NvU32 SetPixelsFromCpuMonoOpacity;
    NvU32 Reserved_0x820[0x6];
    NvU32 SetPixelsFromCpuSrcWidth;
    NvU32 SetPixelsFromCpuSrcHeight;
    NvU32 SetPixelsFromCpuDxDuFrac;
    NvU32 SetPixelsFromCpuDxDuInt;
    NvU32 SetPixelsFromCpuDyDvFrac;
    NvU32 SetPixelsFromCpuDyDvInt;
    NvU32 SetPixelsFromCpuDstX0Frac;
    NvU32 SetPixelsFromCpuDstX0Int;
    NvU32 SetPixelsFromCpuDstY0Frac;
    NvU32 SetPixelsFromCpuDstY0Int;
    NvU32 PixelsFromCpuData;
    NvU32 Reserved_0x864[0x3];
    NvU32 SetBigEndianControl;
    NvU32 Reserved_0x874[0x3];
    NvU32 SetPixelsFromMemoryBlockShape;
    NvU32 SetPixelsFromMemoryCorralSize;
    NvU32 SetPixelsFromMemorySafeOverlap;
    NvU32 SetPixelsFromMemorySampleMode;
    NvU32 Reserved_0x890[0x8];
    NvU32 SetPixelsFromMemoryDstX0;
    NvU32 SetPixelsFromMemoryDstY0;
    NvU32 SetPixelsFromMemoryDstWidth;
    NvU32 SetPixelsFromMemoryDstHeight;
    NvU32 SetPixelsFromMemoryDuDxFrac;
    NvU32 SetPixelsFromMemoryDuDxInt;
    NvU32 SetPixelsFromMemoryDvDyFrac;
    NvU32 SetPixelsFromMemoryDvDyInt;
    NvU32 SetPixelsFromMemorySrcX0Frac;
    NvU32 SetPixelsFromMemorySrcX0Int;
    NvU32 SetPixelsFromMemorySrcY0Frac;
    NvU32 PixelsFromMemorySrcY0Int;
    NvU32 SetFalcon00;
    NvU32 SetFalcon01;
    NvU32 SetFalcon02;
    NvU32 SetFalcon03;
    NvU32 SetFalcon04;
    NvU32 SetFalcon05;
    NvU32 SetFalcon06;
    NvU32 SetFalcon07;
    NvU32 SetFalcon08;
    NvU32 SetFalcon09;
    NvU32 SetFalcon10;
    NvU32 SetFalcon11;
    NvU32 SetFalcon12;
    NvU32 SetFalcon13;
    NvU32 SetFalcon14;
    NvU32 SetFalcon15;
    NvU32 SetFalcon16;
    NvU32 SetFalcon17;
    NvU32 SetFalcon18;
    NvU32 SetFalcon19;
    NvU32 SetFalcon20;
    NvU32 SetFalcon21;
    NvU32 SetFalcon22;
    NvU32 SetFalcon23;
    NvU32 SetFalcon24;
    NvU32 SetFalcon25;
    NvU32 SetFalcon26;
    NvU32 SetFalcon27;
    NvU32 SetFalcon28;
    NvU32 SetFalcon29;
    NvU32 SetFalcon30;
    NvU32 SetFalcon31;
    NvU32 Reserved_0x960[0x123];
    NvU32 MmeDmaWriteMethodBarrier;
    NvU32 Reserved_0xDF0[0x984];
    NvU32 SetMmeShadowScratch[0x100];
    struct {
        NvU32 Macro;
        NvU32 Data;
    } CallMme[0xE0];
} fermi_twod_a_t;


#define NV902D_SET_OBJECT                                                                                  0x0000
#define NV902D_SET_OBJECT_CLASS_ID                                                                           15:0
#define NV902D_SET_OBJECT_ENGINE_ID                                                                         20:16

#define NV902D_NO_OPERATION                                                                                0x0100
#define NV902D_NO_OPERATION_V                                                                                31:0

#define NV902D_SET_NOTIFY_A                                                                                0x0104
#define NV902D_SET_NOTIFY_A_ADDRESS_UPPER                                                                    24:0

#define NV902D_SET_NOTIFY_B                                                                                0x0108
#define NV902D_SET_NOTIFY_B_ADDRESS_LOWER                                                                    31:0

#define NV902D_NOTIFY                                                                                      0x010c
#define NV902D_NOTIFY_TYPE                                                                                   31:0
#define NV902D_NOTIFY_TYPE_WRITE_ONLY                                                                  0x00000000
#define NV902D_NOTIFY_TYPE_WRITE_THEN_AWAKEN                                                           0x00000001

#define NV902D_WAIT_FOR_IDLE                                                                               0x0110
#define NV902D_WAIT_FOR_IDLE_V                                                                               31:0

#define NV902D_LOAD_MME_INSTRUCTION_RAM_POINTER                                                            0x0114
#define NV902D_LOAD_MME_INSTRUCTION_RAM_POINTER_V                                                            31:0

#define NV902D_LOAD_MME_INSTRUCTION_RAM                                                                    0x0118
#define NV902D_LOAD_MME_INSTRUCTION_RAM_V                                                                    31:0

#define NV902D_LOAD_MME_START_ADDRESS_RAM_POINTER                                                          0x011c
#define NV902D_LOAD_MME_START_ADDRESS_RAM_POINTER_V                                                          31:0

#define NV902D_LOAD_MME_START_ADDRESS_RAM                                                                  0x0120
#define NV902D_LOAD_MME_START_ADDRESS_RAM_V                                                                  31:0

#define NV902D_SET_MME_SHADOW_RAM_CONTROL                                                                  0x0124
#define NV902D_SET_MME_SHADOW_RAM_CONTROL_MODE                                                                1:0
#define NV902D_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_TRACK                                            0x00000000
#define NV902D_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_TRACK_WITH_FILTER                                0x00000001
#define NV902D_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_PASSTHROUGH                                      0x00000002
#define NV902D_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_REPLAY                                           0x00000003

#define NV902D_SET_GLOBAL_RENDER_ENABLE_A                                                                  0x0130
#define NV902D_SET_GLOBAL_RENDER_ENABLE_A_OFFSET_UPPER                                                        7:0

#define NV902D_SET_GLOBAL_RENDER_ENABLE_B                                                                  0x0134
#define NV902D_SET_GLOBAL_RENDER_ENABLE_B_OFFSET_LOWER                                                       31:0

#define NV902D_SET_GLOBAL_RENDER_ENABLE_C                                                                  0x0138
#define NV902D_SET_GLOBAL_RENDER_ENABLE_C_MODE                                                                2:0
#define NV902D_SET_GLOBAL_RENDER_ENABLE_C_MODE_FALSE                                                   0x00000000
#define NV902D_SET_GLOBAL_RENDER_ENABLE_C_MODE_TRUE                                                    0x00000001
#define NV902D_SET_GLOBAL_RENDER_ENABLE_C_MODE_CONDITIONAL                                             0x00000002
#define NV902D_SET_GLOBAL_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                                         0x00000003
#define NV902D_SET_GLOBAL_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                                     0x00000004

#define NV902D_SEND_GO_IDLE                                                                                0x013c
#define NV902D_SEND_GO_IDLE_V                                                                                31:0

#define NV902D_PM_TRIGGER                                                                                  0x0140
#define NV902D_PM_TRIGGER_V                                                                                  31:0

#define NV902D_SET_INSTRUMENTATION_METHOD_HEADER                                                           0x0150
#define NV902D_SET_INSTRUMENTATION_METHOD_HEADER_V                                                           31:0

#define NV902D_SET_INSTRUMENTATION_METHOD_DATA                                                             0x0154
#define NV902D_SET_INSTRUMENTATION_METHOD_DATA_V                                                             31:0

#define NV902D_SET_MME_SWITCH_STATE                                                                        0x01ec
#define NV902D_SET_MME_SWITCH_STATE_VALID                                                                     0:0
#define NV902D_SET_MME_SWITCH_STATE_VALID_FALSE                                                        0x00000000
#define NV902D_SET_MME_SWITCH_STATE_VALID_TRUE                                                         0x00000001
#define NV902D_SET_MME_SWITCH_STATE_SAVE_MACRO                                                               11:4
#define NV902D_SET_MME_SWITCH_STATE_RESTORE_MACRO                                                           19:12

#define NV902D_SET_DST_FORMAT                                                                              0x0200
#define NV902D_SET_DST_FORMAT_V                                                                               7:0
#define NV902D_SET_DST_FORMAT_V_A8R8G8B8                                                               0x000000CF
#define NV902D_SET_DST_FORMAT_V_A8RL8GL8BL8                                                            0x000000D0
#define NV902D_SET_DST_FORMAT_V_A2R10G10B10                                                            0x000000DF
#define NV902D_SET_DST_FORMAT_V_A8B8G8R8                                                               0x000000D5
#define NV902D_SET_DST_FORMAT_V_A8BL8GL8RL8                                                            0x000000D6
#define NV902D_SET_DST_FORMAT_V_A2B10G10R10                                                            0x000000D1
#define NV902D_SET_DST_FORMAT_V_X8R8G8B8                                                               0x000000E6
#define NV902D_SET_DST_FORMAT_V_X8RL8GL8BL8                                                            0x000000E7
#define NV902D_SET_DST_FORMAT_V_X8B8G8R8                                                               0x000000F9
#define NV902D_SET_DST_FORMAT_V_X8BL8GL8RL8                                                            0x000000FA
#define NV902D_SET_DST_FORMAT_V_R5G6B5                                                                 0x000000E8
#define NV902D_SET_DST_FORMAT_V_A1R5G5B5                                                               0x000000E9
#define NV902D_SET_DST_FORMAT_V_X1R5G5B5                                                               0x000000F8
#define NV902D_SET_DST_FORMAT_V_Y8                                                                     0x000000F3
#define NV902D_SET_DST_FORMAT_V_Y16                                                                    0x000000EE
#define NV902D_SET_DST_FORMAT_V_Y32                                                                    0x000000FF
#define NV902D_SET_DST_FORMAT_V_Z1R5G5B5                                                               0x000000FB
#define NV902D_SET_DST_FORMAT_V_O1R5G5B5                                                               0x000000FC
#define NV902D_SET_DST_FORMAT_V_Z8R8G8B8                                                               0x000000FD
#define NV902D_SET_DST_FORMAT_V_O8R8G8B8                                                               0x000000FE
#define NV902D_SET_DST_FORMAT_V_Y1_8X8                                                                 0x0000001C
#define NV902D_SET_DST_FORMAT_V_RF16                                                                   0x000000F2
#define NV902D_SET_DST_FORMAT_V_RF32                                                                   0x000000E5
#define NV902D_SET_DST_FORMAT_V_RF32_GF32                                                              0x000000CB
#define NV902D_SET_DST_FORMAT_V_RF16_GF16_BF16_AF16                                                    0x000000CA
#define NV902D_SET_DST_FORMAT_V_RF16_GF16_BF16_X16                                                     0x000000CE
#define NV902D_SET_DST_FORMAT_V_RF32_GF32_BF32_AF32                                                    0x000000C0
#define NV902D_SET_DST_FORMAT_V_RF32_GF32_BF32_X32                                                     0x000000C3
#define NV902D_SET_DST_FORMAT_V_R16_G16_B16_A16                                                        0x000000C6
#define NV902D_SET_DST_FORMAT_V_RN16_GN16_BN16_AN16                                                    0x000000C7
#define NV902D_SET_DST_FORMAT_V_BF10GF11RF11                                                           0x000000E0
#define NV902D_SET_DST_FORMAT_V_AN8BN8GN8RN8                                                           0x000000D7
#define NV902D_SET_DST_FORMAT_V_RF16_GF16                                                              0x000000DE
#define NV902D_SET_DST_FORMAT_V_R16_G16                                                                0x000000DA
#define NV902D_SET_DST_FORMAT_V_RN16_GN16                                                              0x000000DB
#define NV902D_SET_DST_FORMAT_V_G8R8                                                                   0x000000EA
#define NV902D_SET_DST_FORMAT_V_GN8RN8                                                                 0x000000EB
#define NV902D_SET_DST_FORMAT_V_RN16                                                                   0x000000EF
#define NV902D_SET_DST_FORMAT_V_RN8                                                                    0x000000F4
#define NV902D_SET_DST_FORMAT_V_A8                                                                     0x000000F7

#define NV902D_SET_DST_MEMORY_LAYOUT                                                                       0x0204
#define NV902D_SET_DST_MEMORY_LAYOUT_V                                                                        0:0
#define NV902D_SET_DST_MEMORY_LAYOUT_V_BLOCKLINEAR                                                     0x00000000
#define NV902D_SET_DST_MEMORY_LAYOUT_V_PITCH                                                           0x00000001

#define NV902D_SET_DST_BLOCK_SIZE                                                                          0x0208
#define NV902D_SET_DST_BLOCK_SIZE_HEIGHT                                                                      6:4
#define NV902D_SET_DST_BLOCK_SIZE_HEIGHT_ONE_GOB                                                       0x00000000
#define NV902D_SET_DST_BLOCK_SIZE_HEIGHT_TWO_GOBS                                                      0x00000001
#define NV902D_SET_DST_BLOCK_SIZE_HEIGHT_FOUR_GOBS                                                     0x00000002
#define NV902D_SET_DST_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                                                    0x00000003
#define NV902D_SET_DST_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                                                  0x00000004
#define NV902D_SET_DST_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                                                0x00000005
#define NV902D_SET_DST_BLOCK_SIZE_DEPTH                                                                      10:8
#define NV902D_SET_DST_BLOCK_SIZE_DEPTH_ONE_GOB                                                        0x00000000
#define NV902D_SET_DST_BLOCK_SIZE_DEPTH_TWO_GOBS                                                       0x00000001
#define NV902D_SET_DST_BLOCK_SIZE_DEPTH_FOUR_GOBS                                                      0x00000002
#define NV902D_SET_DST_BLOCK_SIZE_DEPTH_EIGHT_GOBS                                                     0x00000003
#define NV902D_SET_DST_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                                                   0x00000004
#define NV902D_SET_DST_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                                                 0x00000005

#define NV902D_SET_DST_DEPTH                                                                               0x020c
#define NV902D_SET_DST_DEPTH_V                                                                               31:0

#define NV902D_SET_DST_LAYER                                                                               0x0210
#define NV902D_SET_DST_LAYER_V                                                                               31:0

#define NV902D_SET_DST_PITCH                                                                               0x0214
#define NV902D_SET_DST_PITCH_V                                                                               31:0

#define NV902D_SET_DST_WIDTH                                                                               0x0218
#define NV902D_SET_DST_WIDTH_V                                                                               31:0

#define NV902D_SET_DST_HEIGHT                                                                              0x021c
#define NV902D_SET_DST_HEIGHT_V                                                                              31:0

#define NV902D_SET_DST_OFFSET_UPPER                                                                        0x0220
#define NV902D_SET_DST_OFFSET_UPPER_V                                                                         7:0

#define NV902D_SET_DST_OFFSET_LOWER                                                                        0x0224
#define NV902D_SET_DST_OFFSET_LOWER_V                                                                        31:0

#define NV902D_FLUSH_AND_INVALIDATE_ROP_MINI_CACHE                                                         0x0228
#define NV902D_FLUSH_AND_INVALIDATE_ROP_MINI_CACHE_V                                                          0:0

#define NV902D_SET_SPARE_NOOP06                                                                            0x022c
#define NV902D_SET_SPARE_NOOP06_V                                                                            31:0

#define NV902D_SET_SRC_FORMAT                                                                              0x0230
#define NV902D_SET_SRC_FORMAT_V                                                                               7:0
#define NV902D_SET_SRC_FORMAT_V_A8R8G8B8                                                               0x000000CF
#define NV902D_SET_SRC_FORMAT_V_A8RL8GL8BL8                                                            0x000000D0
#define NV902D_SET_SRC_FORMAT_V_A2R10G10B10                                                            0x000000DF
#define NV902D_SET_SRC_FORMAT_V_A8B8G8R8                                                               0x000000D5
#define NV902D_SET_SRC_FORMAT_V_A8BL8GL8RL8                                                            0x000000D6
#define NV902D_SET_SRC_FORMAT_V_A2B10G10R10                                                            0x000000D1
#define NV902D_SET_SRC_FORMAT_V_X8R8G8B8                                                               0x000000E6
#define NV902D_SET_SRC_FORMAT_V_X8RL8GL8BL8                                                            0x000000E7
#define NV902D_SET_SRC_FORMAT_V_X8B8G8R8                                                               0x000000F9
#define NV902D_SET_SRC_FORMAT_V_X8BL8GL8RL8                                                            0x000000FA
#define NV902D_SET_SRC_FORMAT_V_R5G6B5                                                                 0x000000E8
#define NV902D_SET_SRC_FORMAT_V_A1R5G5B5                                                               0x000000E9
#define NV902D_SET_SRC_FORMAT_V_X1R5G5B5                                                               0x000000F8
#define NV902D_SET_SRC_FORMAT_V_Y8                                                                     0x000000F3
#define NV902D_SET_SRC_FORMAT_V_AY8                                                                    0x0000001D
#define NV902D_SET_SRC_FORMAT_V_Y16                                                                    0x000000EE
#define NV902D_SET_SRC_FORMAT_V_Y32                                                                    0x000000FF
#define NV902D_SET_SRC_FORMAT_V_Z1R5G5B5                                                               0x000000FB
#define NV902D_SET_SRC_FORMAT_V_O1R5G5B5                                                               0x000000FC
#define NV902D_SET_SRC_FORMAT_V_Z8R8G8B8                                                               0x000000FD
#define NV902D_SET_SRC_FORMAT_V_O8R8G8B8                                                               0x000000FE
#define NV902D_SET_SRC_FORMAT_V_Y1_8X8                                                                 0x0000001C
#define NV902D_SET_SRC_FORMAT_V_RF16                                                                   0x000000F2
#define NV902D_SET_SRC_FORMAT_V_RF32                                                                   0x000000E5
#define NV902D_SET_SRC_FORMAT_V_RF32_GF32                                                              0x000000CB
#define NV902D_SET_SRC_FORMAT_V_RF16_GF16_BF16_AF16                                                    0x000000CA
#define NV902D_SET_SRC_FORMAT_V_RF16_GF16_BF16_X16                                                     0x000000CE
#define NV902D_SET_SRC_FORMAT_V_RF32_GF32_BF32_AF32                                                    0x000000C0
#define NV902D_SET_SRC_FORMAT_V_RF32_GF32_BF32_X32                                                     0x000000C3
#define NV902D_SET_SRC_FORMAT_V_R16_G16_B16_A16                                                        0x000000C6
#define NV902D_SET_SRC_FORMAT_V_RN16_GN16_BN16_AN16                                                    0x000000C7
#define NV902D_SET_SRC_FORMAT_V_BF10GF11RF11                                                           0x000000E0
#define NV902D_SET_SRC_FORMAT_V_AN8BN8GN8RN8                                                           0x000000D7
#define NV902D_SET_SRC_FORMAT_V_RF16_GF16                                                              0x000000DE
#define NV902D_SET_SRC_FORMAT_V_R16_G16                                                                0x000000DA
#define NV902D_SET_SRC_FORMAT_V_RN16_GN16                                                              0x000000DB
#define NV902D_SET_SRC_FORMAT_V_G8R8                                                                   0x000000EA
#define NV902D_SET_SRC_FORMAT_V_GN8RN8                                                                 0x000000EB
#define NV902D_SET_SRC_FORMAT_V_RN16                                                                   0x000000EF
#define NV902D_SET_SRC_FORMAT_V_RN8                                                                    0x000000F4
#define NV902D_SET_SRC_FORMAT_V_A8                                                                     0x000000F7

#define NV902D_SET_SRC_MEMORY_LAYOUT                                                                       0x0234
#define NV902D_SET_SRC_MEMORY_LAYOUT_V                                                                        0:0
#define NV902D_SET_SRC_MEMORY_LAYOUT_V_BLOCKLINEAR                                                     0x00000000
#define NV902D_SET_SRC_MEMORY_LAYOUT_V_PITCH                                                           0x00000001

#define NV902D_SET_SRC_BLOCK_SIZE                                                                          0x0238
#define NV902D_SET_SRC_BLOCK_SIZE_HEIGHT                                                                      6:4
#define NV902D_SET_SRC_BLOCK_SIZE_HEIGHT_ONE_GOB                                                       0x00000000
#define NV902D_SET_SRC_BLOCK_SIZE_HEIGHT_TWO_GOBS                                                      0x00000001
#define NV902D_SET_SRC_BLOCK_SIZE_HEIGHT_FOUR_GOBS                                                     0x00000002
#define NV902D_SET_SRC_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                                                    0x00000003
#define NV902D_SET_SRC_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                                                  0x00000004
#define NV902D_SET_SRC_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                                                0x00000005
#define NV902D_SET_SRC_BLOCK_SIZE_DEPTH                                                                      10:8
#define NV902D_SET_SRC_BLOCK_SIZE_DEPTH_ONE_GOB                                                        0x00000000
#define NV902D_SET_SRC_BLOCK_SIZE_DEPTH_TWO_GOBS                                                       0x00000001
#define NV902D_SET_SRC_BLOCK_SIZE_DEPTH_FOUR_GOBS                                                      0x00000002
#define NV902D_SET_SRC_BLOCK_SIZE_DEPTH_EIGHT_GOBS                                                     0x00000003
#define NV902D_SET_SRC_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                                                   0x00000004
#define NV902D_SET_SRC_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                                                 0x00000005

#define NV902D_SET_SRC_DEPTH                                                                               0x023c
#define NV902D_SET_SRC_DEPTH_V                                                                               31:0

#define NV902D_TWOD_INVALIDATE_TEXTURE_DATA_CACHE                                                          0x0240
#define NV902D_TWOD_INVALIDATE_TEXTURE_DATA_CACHE_V                                                           1:0
#define NV902D_TWOD_INVALIDATE_TEXTURE_DATA_CACHE_V_L1_ONLY                                            0x00000000
#define NV902D_TWOD_INVALIDATE_TEXTURE_DATA_CACHE_V_L2_ONLY                                            0x00000001
#define NV902D_TWOD_INVALIDATE_TEXTURE_DATA_CACHE_V_L1_AND_L2                                          0x00000002

#define NV902D_SET_SRC_PITCH                                                                               0x0244
#define NV902D_SET_SRC_PITCH_V                                                                               31:0

#define NV902D_SET_SRC_WIDTH                                                                               0x0248
#define NV902D_SET_SRC_WIDTH_V                                                                               31:0

#define NV902D_SET_SRC_HEIGHT                                                                              0x024c
#define NV902D_SET_SRC_HEIGHT_V                                                                              31:0

#define NV902D_SET_SRC_OFFSET_UPPER                                                                        0x0250
#define NV902D_SET_SRC_OFFSET_UPPER_V                                                                         7:0

#define NV902D_SET_SRC_OFFSET_LOWER                                                                        0x0254
#define NV902D_SET_SRC_OFFSET_LOWER_V                                                                        31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_SECTOR_PROMOTION                                                     0x0258
#define NV902D_SET_PIXELS_FROM_MEMORY_SECTOR_PROMOTION_V                                                      1:0
#define NV902D_SET_PIXELS_FROM_MEMORY_SECTOR_PROMOTION_V_NO_PROMOTION                                  0x00000000
#define NV902D_SET_PIXELS_FROM_MEMORY_SECTOR_PROMOTION_V_PROMOTE_TO_2_V                                0x00000001
#define NV902D_SET_PIXELS_FROM_MEMORY_SECTOR_PROMOTION_V_PROMOTE_TO_2_H                                0x00000002
#define NV902D_SET_PIXELS_FROM_MEMORY_SECTOR_PROMOTION_V_PROMOTE_TO_4                                  0x00000003

#define NV902D_SET_SPARE_NOOP12                                                                            0x025c
#define NV902D_SET_SPARE_NOOP12_V                                                                            31:0

#define NV902D_SET_NUM_PROCESSING_CLUSTERS                                                                 0x0260
#define NV902D_SET_NUM_PROCESSING_CLUSTERS_V                                                                  0:0
#define NV902D_SET_NUM_PROCESSING_CLUSTERS_V_ALL                                                       0x00000000
#define NV902D_SET_NUM_PROCESSING_CLUSTERS_V_ONE                                                       0x00000001

#define NV902D_SET_RENDER_ENABLE_A                                                                         0x0264
#define NV902D_SET_RENDER_ENABLE_A_OFFSET_UPPER                                                               7:0

#define NV902D_SET_RENDER_ENABLE_B                                                                         0x0268
#define NV902D_SET_RENDER_ENABLE_B_OFFSET_LOWER                                                              31:0

#define NV902D_SET_RENDER_ENABLE_C                                                                         0x026c
#define NV902D_SET_RENDER_ENABLE_C_MODE                                                                       2:0
#define NV902D_SET_RENDER_ENABLE_C_MODE_FALSE                                                          0x00000000
#define NV902D_SET_RENDER_ENABLE_C_MODE_TRUE                                                           0x00000001
#define NV902D_SET_RENDER_ENABLE_C_MODE_CONDITIONAL                                                    0x00000002
#define NV902D_SET_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                                                0x00000003
#define NV902D_SET_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                                            0x00000004

#define NV902D_SET_SPARE_NOOP08                                                                            0x0270
#define NV902D_SET_SPARE_NOOP08_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP01                                                                            0x0274
#define NV902D_SET_SPARE_NOOP01_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP11                                                                            0x0278
#define NV902D_SET_SPARE_NOOP11_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP07                                                                            0x027c
#define NV902D_SET_SPARE_NOOP07_V                                                                            31:0

#define NV902D_SET_CLIP_X0                                                                                 0x0280
#define NV902D_SET_CLIP_X0_V                                                                                 31:0

#define NV902D_SET_CLIP_Y0                                                                                 0x0284
#define NV902D_SET_CLIP_Y0_V                                                                                 31:0

#define NV902D_SET_CLIP_WIDTH                                                                              0x0288
#define NV902D_SET_CLIP_WIDTH_V                                                                              31:0

#define NV902D_SET_CLIP_HEIGHT                                                                             0x028c
#define NV902D_SET_CLIP_HEIGHT_V                                                                             31:0

#define NV902D_SET_CLIP_ENABLE                                                                             0x0290
#define NV902D_SET_CLIP_ENABLE_V                                                                              0:0
#define NV902D_SET_CLIP_ENABLE_V_FALSE                                                                 0x00000000
#define NV902D_SET_CLIP_ENABLE_V_TRUE                                                                  0x00000001

#define NV902D_SET_COLOR_KEY_FORMAT                                                                        0x0294
#define NV902D_SET_COLOR_KEY_FORMAT_V                                                                         2:0
#define NV902D_SET_COLOR_KEY_FORMAT_V_A16R5G6B5                                                        0x00000000
#define NV902D_SET_COLOR_KEY_FORMAT_V_A1R5G5B5                                                         0x00000001
#define NV902D_SET_COLOR_KEY_FORMAT_V_A8R8G8B8                                                         0x00000002
#define NV902D_SET_COLOR_KEY_FORMAT_V_A2R10G10B10                                                      0x00000003
#define NV902D_SET_COLOR_KEY_FORMAT_V_Y8                                                               0x00000004
#define NV902D_SET_COLOR_KEY_FORMAT_V_Y16                                                              0x00000005
#define NV902D_SET_COLOR_KEY_FORMAT_V_Y32                                                              0x00000006

#define NV902D_SET_COLOR_KEY                                                                               0x0298
#define NV902D_SET_COLOR_KEY_V                                                                               31:0

#define NV902D_SET_COLOR_KEY_ENABLE                                                                        0x029c
#define NV902D_SET_COLOR_KEY_ENABLE_V                                                                         0:0
#define NV902D_SET_COLOR_KEY_ENABLE_V_FALSE                                                            0x00000000
#define NV902D_SET_COLOR_KEY_ENABLE_V_TRUE                                                             0x00000001

#define NV902D_SET_ROP                                                                                     0x02a0
#define NV902D_SET_ROP_V                                                                                      7:0

#define NV902D_SET_BETA1                                                                                   0x02a4
#define NV902D_SET_BETA1_V                                                                                   31:0

#define NV902D_SET_BETA4                                                                                   0x02a8
#define NV902D_SET_BETA4_B                                                                                    7:0
#define NV902D_SET_BETA4_G                                                                                   15:8
#define NV902D_SET_BETA4_R                                                                                  23:16
#define NV902D_SET_BETA4_A                                                                                  31:24

#define NV902D_SET_OPERATION                                                                               0x02ac
#define NV902D_SET_OPERATION_V                                                                                2:0
#define NV902D_SET_OPERATION_V_SRCCOPY_AND                                                             0x00000000
#define NV902D_SET_OPERATION_V_ROP_AND                                                                 0x00000001
#define NV902D_SET_OPERATION_V_BLEND_AND                                                               0x00000002
#define NV902D_SET_OPERATION_V_SRCCOPY                                                                 0x00000003
#define NV902D_SET_OPERATION_V_ROP                                                                     0x00000004
#define NV902D_SET_OPERATION_V_SRCCOPY_PREMULT                                                         0x00000005
#define NV902D_SET_OPERATION_V_BLEND_PREMULT                                                           0x00000006

#define NV902D_SET_PATTERN_OFFSET                                                                          0x02b0
#define NV902D_SET_PATTERN_OFFSET_X                                                                           5:0
#define NV902D_SET_PATTERN_OFFSET_Y                                                                          13:8

#define NV902D_SET_PATTERN_SELECT                                                                          0x02b4
#define NV902D_SET_PATTERN_SELECT_V                                                                           1:0
#define NV902D_SET_PATTERN_SELECT_V_MONOCHROME_8x8                                                     0x00000000
#define NV902D_SET_PATTERN_SELECT_V_MONOCHROME_64x1                                                    0x00000001
#define NV902D_SET_PATTERN_SELECT_V_MONOCHROME_1x64                                                    0x00000002
#define NV902D_SET_PATTERN_SELECT_V_COLOR                                                              0x00000003

#define NV902D_SET_DST_COLOR_RENDER_TO_ZETA_SURFACE                                                        0x02b8
#define NV902D_SET_DST_COLOR_RENDER_TO_ZETA_SURFACE_V                                                         0:0
#define NV902D_SET_DST_COLOR_RENDER_TO_ZETA_SURFACE_V_FALSE                                            0x00000000
#define NV902D_SET_DST_COLOR_RENDER_TO_ZETA_SURFACE_V_TRUE                                             0x00000001

#define NV902D_SET_SPARE_NOOP04                                                                            0x02bc
#define NV902D_SET_SPARE_NOOP04_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP15                                                                            0x02c0
#define NV902D_SET_SPARE_NOOP15_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP13                                                                            0x02c4
#define NV902D_SET_SPARE_NOOP13_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP03                                                                            0x02c8
#define NV902D_SET_SPARE_NOOP03_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP14                                                                            0x02cc
#define NV902D_SET_SPARE_NOOP14_V                                                                            31:0

#define NV902D_SET_SPARE_NOOP02                                                                            0x02d0
#define NV902D_SET_SPARE_NOOP02_V                                                                            31:0

#define NV902D_SET_COMPRESSION                                                                             0x02d4
#define NV902D_SET_COMPRESSION_ENABLE                                                                         0:0
#define NV902D_SET_COMPRESSION_ENABLE_FALSE                                                            0x00000000
#define NV902D_SET_COMPRESSION_ENABLE_TRUE                                                             0x00000001

#define NV902D_SET_SPARE_NOOP09                                                                            0x02d8
#define NV902D_SET_SPARE_NOOP09_V                                                                            31:0

#define NV902D_SET_RENDER_ENABLE_OVERRIDE                                                                  0x02dc
#define NV902D_SET_RENDER_ENABLE_OVERRIDE_MODE                                                                1:0
#define NV902D_SET_RENDER_ENABLE_OVERRIDE_MODE_USE_RENDER_ENABLE                                       0x00000000
#define NV902D_SET_RENDER_ENABLE_OVERRIDE_MODE_ALWAYS_RENDER                                           0x00000001
#define NV902D_SET_RENDER_ENABLE_OVERRIDE_MODE_NEVER_RENDER                                            0x00000002

#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION                                                            0x02e0
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_HORIZONTAL                                                    1:0
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_HORIZONTAL_HW_DECIDES                                  0x00000000
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_HORIZONTAL_LEFT_TO_RIGHT                               0x00000001
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_HORIZONTAL_RIGHT_TO_LEFT                               0x00000002
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_VERTICAL                                                      5:4
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_VERTICAL_HW_DECIDES                                    0x00000000
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_VERTICAL_TOP_TO_BOTTOM                                 0x00000001
#define NV902D_SET_PIXELS_FROM_MEMORY_DIRECTION_VERTICAL_BOTTOM_TO_TOP                                 0x00000002

#define NV902D_SET_SPARE_NOOP10                                                                            0x02e4
#define NV902D_SET_SPARE_NOOP10_V                                                                            31:0

#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT                                                         0x02e8
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V                                                          2:0
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V_A8X8R5G6B5                                        0x00000000
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V_A1R5G5B5                                          0x00000001
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V_A8R8G8B8                                          0x00000002
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V_A8Y8                                              0x00000003
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V_A8X8Y16                                           0x00000004
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V_Y32                                               0x00000005
#define NV902D_SET_MONOCHROME_PATTERN_COLOR_FORMAT_V_BYTE_EXPAND                                       0x00000006

#define NV902D_SET_MONOCHROME_PATTERN_FORMAT                                                               0x02ec
#define NV902D_SET_MONOCHROME_PATTERN_FORMAT_V                                                                0:0
#define NV902D_SET_MONOCHROME_PATTERN_FORMAT_V_CGA6_M1                                                 0x00000000
#define NV902D_SET_MONOCHROME_PATTERN_FORMAT_V_LE_M1                                                   0x00000001

#define NV902D_SET_MONOCHROME_PATTERN_COLOR0                                                               0x02f0
#define NV902D_SET_MONOCHROME_PATTERN_COLOR0_V                                                               31:0

#define NV902D_SET_MONOCHROME_PATTERN_COLOR1                                                               0x02f4
#define NV902D_SET_MONOCHROME_PATTERN_COLOR1_V                                                               31:0

#define NV902D_SET_MONOCHROME_PATTERN0                                                                     0x02f8
#define NV902D_SET_MONOCHROME_PATTERN0_V                                                                     31:0

#define NV902D_SET_MONOCHROME_PATTERN1                                                                     0x02fc
#define NV902D_SET_MONOCHROME_PATTERN1_V                                                                     31:0

#define NV902D_COLOR_PATTERN_X8R8G8B8(i)                                                           (0x0300+(i)*4)
#define NV902D_COLOR_PATTERN_X8R8G8B8_B0                                                                      7:0
#define NV902D_COLOR_PATTERN_X8R8G8B8_G0                                                                     15:8
#define NV902D_COLOR_PATTERN_X8R8G8B8_R0                                                                    23:16
#define NV902D_COLOR_PATTERN_X8R8G8B8_IGNORE0                                                               31:24

#define NV902D_COLOR_PATTERN_R5G6B5(i)                                                             (0x0400+(i)*4)
#define NV902D_COLOR_PATTERN_R5G6B5_B0                                                                        4:0
#define NV902D_COLOR_PATTERN_R5G6B5_G0                                                                       10:5
#define NV902D_COLOR_PATTERN_R5G6B5_R0                                                                      15:11
#define NV902D_COLOR_PATTERN_R5G6B5_B1                                                                      20:16
#define NV902D_COLOR_PATTERN_R5G6B5_G1                                                                      26:21
#define NV902D_COLOR_PATTERN_R5G6B5_R1                                                                      31:27

#define NV902D_COLOR_PATTERN_X1R5G5B5(i)                                                           (0x0480+(i)*4)
#define NV902D_COLOR_PATTERN_X1R5G5B5_B0                                                                      4:0
#define NV902D_COLOR_PATTERN_X1R5G5B5_G0                                                                      9:5
#define NV902D_COLOR_PATTERN_X1R5G5B5_R0                                                                    14:10
#define NV902D_COLOR_PATTERN_X1R5G5B5_IGNORE0                                                               15:15
#define NV902D_COLOR_PATTERN_X1R5G5B5_B1                                                                    20:16
#define NV902D_COLOR_PATTERN_X1R5G5B5_G1                                                                    25:21
#define NV902D_COLOR_PATTERN_X1R5G5B5_R1                                                                    30:26
#define NV902D_COLOR_PATTERN_X1R5G5B5_IGNORE1                                                               31:31

#define NV902D_COLOR_PATTERN_Y8(i)                                                                 (0x0500+(i)*4)
#define NV902D_COLOR_PATTERN_Y8_Y0                                                                            7:0
#define NV902D_COLOR_PATTERN_Y8_Y1                                                                           15:8
#define NV902D_COLOR_PATTERN_Y8_Y2                                                                          23:16
#define NV902D_COLOR_PATTERN_Y8_Y3                                                                          31:24

#define NV902D_SET_RENDER_SOLID_PRIM_COLOR0                                                                0x0540
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR0_V                                                                31:0

#define NV902D_SET_RENDER_SOLID_PRIM_COLOR1                                                                0x0544
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR1_V                                                                31:0

#define NV902D_SET_RENDER_SOLID_PRIM_COLOR2                                                                0x0548
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR2_V                                                                31:0

#define NV902D_SET_RENDER_SOLID_PRIM_COLOR3                                                                0x054c
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR3_V                                                                31:0

#define NV902D_SET_MME_MEM_ADDRESS_A                                                                       0x0550
#define NV902D_SET_MME_MEM_ADDRESS_A_UPPER                                                                   24:0

#define NV902D_SET_MME_MEM_ADDRESS_B                                                                       0x0554
#define NV902D_SET_MME_MEM_ADDRESS_B_LOWER                                                                   31:0

#define NV902D_SET_MME_DATA_RAM_ADDRESS                                                                    0x0558
#define NV902D_SET_MME_DATA_RAM_ADDRESS_WORD                                                                 31:0

#define NV902D_MME_DMA_READ                                                                                0x055c
#define NV902D_MME_DMA_READ_LENGTH                                                                           31:0

#define NV902D_MME_DMA_READ_FIFOED                                                                         0x0560
#define NV902D_MME_DMA_READ_FIFOED_LENGTH                                                                    31:0

#define NV902D_MME_DMA_WRITE                                                                               0x0564
#define NV902D_MME_DMA_WRITE_LENGTH                                                                          31:0

#define NV902D_MME_DMA_REDUCTION                                                                           0x0568
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP                                                                 2:0
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_ADD                                                  0x00000000
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_MIN                                                  0x00000001
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_MAX                                                  0x00000002
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_INC                                                  0x00000003
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_DEC                                                  0x00000004
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_AND                                                  0x00000005
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_OR                                                   0x00000006
#define NV902D_MME_DMA_REDUCTION_REDUCTION_OP_RED_XOR                                                  0x00000007
#define NV902D_MME_DMA_REDUCTION_REDUCTION_FORMAT                                                             5:4
#define NV902D_MME_DMA_REDUCTION_REDUCTION_FORMAT_UNSIGNED                                             0x00000000
#define NV902D_MME_DMA_REDUCTION_REDUCTION_FORMAT_SIGNED                                               0x00000001
#define NV902D_MME_DMA_REDUCTION_REDUCTION_SIZE                                                               8:8
#define NV902D_MME_DMA_REDUCTION_REDUCTION_SIZE_FOUR_BYTES                                             0x00000000
#define NV902D_MME_DMA_REDUCTION_REDUCTION_SIZE_EIGHT_BYTES                                            0x00000001

#define NV902D_MME_DMA_SYSMEMBAR                                                                           0x056c
#define NV902D_MME_DMA_SYSMEMBAR_V                                                                            0:0

#define NV902D_MME_DMA_SYNC                                                                                0x0570
#define NV902D_MME_DMA_SYNC_VALUE                                                                            31:0

#define NV902D_SET_MME_DATA_FIFO_CONFIG                                                                    0x0574
#define NV902D_SET_MME_DATA_FIFO_CONFIG_FIFO_SIZE                                                             2:0
#define NV902D_SET_MME_DATA_FIFO_CONFIG_FIFO_SIZE_SIZE_0KB                                             0x00000000
#define NV902D_SET_MME_DATA_FIFO_CONFIG_FIFO_SIZE_SIZE_4KB                                             0x00000001
#define NV902D_SET_MME_DATA_FIFO_CONFIG_FIFO_SIZE_SIZE_8KB                                             0x00000002
#define NV902D_SET_MME_DATA_FIFO_CONFIG_FIFO_SIZE_SIZE_12KB                                            0x00000003
#define NV902D_SET_MME_DATA_FIFO_CONFIG_FIFO_SIZE_SIZE_16KB                                            0x00000004

#define NV902D_RENDER_SOLID_PRIM_MODE                                                                      0x0580
#define NV902D_RENDER_SOLID_PRIM_MODE_V                                                                       2:0
#define NV902D_RENDER_SOLID_PRIM_MODE_V_POINTS                                                         0x00000000
#define NV902D_RENDER_SOLID_PRIM_MODE_V_LINES                                                          0x00000001
#define NV902D_RENDER_SOLID_PRIM_MODE_V_POLYLINE                                                       0x00000002
#define NV902D_RENDER_SOLID_PRIM_MODE_V_TRIANGLES                                                      0x00000003
#define NV902D_RENDER_SOLID_PRIM_MODE_V_RECTS                                                          0x00000004

#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT                                                          0x0584
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V                                                           7:0
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_RF32_GF32_BF32_AF32                                0x000000C0
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_RF16_GF16_BF16_AF16                                0x000000CA
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_RF32_GF32                                          0x000000CB
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_A8R8G8B8                                           0x000000CF
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_A2R10G10B10                                        0x000000DF
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_A8B8G8R8                                           0x000000D5
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_A2B10G10R10                                        0x000000D1
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_X8R8G8B8                                           0x000000E6
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_X8B8G8R8                                           0x000000F9
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_R5G6B5                                             0x000000E8
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_A1R5G5B5                                           0x000000E9
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_X1R5G5B5                                           0x000000F8
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_Y8                                                 0x000000F3
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_Y16                                                0x000000EE
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_Y32                                                0x000000FF
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_Z1R5G5B5                                           0x000000FB
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_O1R5G5B5                                           0x000000FC
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_Z8R8G8B8                                           0x000000FD
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT_V_O8R8G8B8                                           0x000000FE

#define NV902D_SET_RENDER_SOLID_PRIM_COLOR                                                                 0x0588
#define NV902D_SET_RENDER_SOLID_PRIM_COLOR_V                                                                 31:0

#define NV902D_SET_RENDER_SOLID_LINE_TIE_BREAK_BITS                                                        0x058c
#define NV902D_SET_RENDER_SOLID_LINE_TIE_BREAK_BITS_XMAJ__XINC__YINC                                          0:0
#define NV902D_SET_RENDER_SOLID_LINE_TIE_BREAK_BITS_XMAJ__XDEC__YINC                                          4:4
#define NV902D_SET_RENDER_SOLID_LINE_TIE_BREAK_BITS_YMAJ__XINC__YINC                                          8:8
#define NV902D_SET_RENDER_SOLID_LINE_TIE_BREAK_BITS_YMAJ__XDEC__YINC                                        12:12

#define NV902D_RENDER_SOLID_PRIM_POINT_X_Y                                                                 0x05e0
#define NV902D_RENDER_SOLID_PRIM_POINT_X_Y_X                                                                 15:0
#define NV902D_RENDER_SOLID_PRIM_POINT_X_Y_Y                                                                31:16

#define NV902D_RENDER_SOLID_PRIM_POINT_SET_X(j)                                                    (0x0600+(j)*8)
#define NV902D_RENDER_SOLID_PRIM_POINT_SET_X_V                                                               31:0

#define NV902D_RENDER_SOLID_PRIM_POINT_Y(j)                                                        (0x0604+(j)*8)
#define NV902D_RENDER_SOLID_PRIM_POINT_Y_V                                                                   31:0

#define NV902D_SET_PIXELS_FROM_CPU_DATA_TYPE                                                               0x0800
#define NV902D_SET_PIXELS_FROM_CPU_DATA_TYPE_V                                                                0:0
#define NV902D_SET_PIXELS_FROM_CPU_DATA_TYPE_V_COLOR                                                   0x00000000
#define NV902D_SET_PIXELS_FROM_CPU_DATA_TYPE_V_INDEX                                                   0x00000001

#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT                                                            0x0804
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V                                                             7:0
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_A8R8G8B8                                             0x000000CF
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_A2R10G10B10                                          0x000000DF
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_A8B8G8R8                                             0x000000D5
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_A2B10G10R10                                          0x000000D1
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_X8R8G8B8                                             0x000000E6
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_X8B8G8R8                                             0x000000F9
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_R5G6B5                                               0x000000E8
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_A1R5G5B5                                             0x000000E9
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_X1R5G5B5                                             0x000000F8
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_Y8                                                   0x000000F3
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_Y16                                                  0x000000EE
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_Y32                                                  0x000000FF
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_Z1R5G5B5                                             0x000000FB
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_O1R5G5B5                                             0x000000FC
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_Z8R8G8B8                                             0x000000FD
#define NV902D_SET_PIXELS_FROM_CPU_COLOR_FORMAT_V_O8R8G8B8                                             0x000000FE

#define NV902D_SET_PIXELS_FROM_CPU_INDEX_FORMAT                                                            0x0808
#define NV902D_SET_PIXELS_FROM_CPU_INDEX_FORMAT_V                                                             1:0
#define NV902D_SET_PIXELS_FROM_CPU_INDEX_FORMAT_V_I1                                                   0x00000000
#define NV902D_SET_PIXELS_FROM_CPU_INDEX_FORMAT_V_I4                                                   0x00000001
#define NV902D_SET_PIXELS_FROM_CPU_INDEX_FORMAT_V_I8                                                   0x00000002

#define NV902D_SET_PIXELS_FROM_CPU_MONO_FORMAT                                                             0x080c
#define NV902D_SET_PIXELS_FROM_CPU_MONO_FORMAT_V                                                              0:0
#define NV902D_SET_PIXELS_FROM_CPU_MONO_FORMAT_V_CGA6_M1                                               0x00000000
#define NV902D_SET_PIXELS_FROM_CPU_MONO_FORMAT_V_LE_M1                                                 0x00000001

#define NV902D_SET_PIXELS_FROM_CPU_WRAP                                                                    0x0810
#define NV902D_SET_PIXELS_FROM_CPU_WRAP_V                                                                     1:0
#define NV902D_SET_PIXELS_FROM_CPU_WRAP_V_WRAP_PIXEL                                                   0x00000000
#define NV902D_SET_PIXELS_FROM_CPU_WRAP_V_WRAP_BYTE                                                    0x00000001
#define NV902D_SET_PIXELS_FROM_CPU_WRAP_V_WRAP_DWORD                                                   0x00000002

#define NV902D_SET_PIXELS_FROM_CPU_COLOR0                                                                  0x0814
#define NV902D_SET_PIXELS_FROM_CPU_COLOR0_V                                                                  31:0

#define NV902D_SET_PIXELS_FROM_CPU_COLOR1                                                                  0x0818
#define NV902D_SET_PIXELS_FROM_CPU_COLOR1_V                                                                  31:0

#define NV902D_SET_PIXELS_FROM_CPU_MONO_OPACITY                                                            0x081c
#define NV902D_SET_PIXELS_FROM_CPU_MONO_OPACITY_V                                                             0:0
#define NV902D_SET_PIXELS_FROM_CPU_MONO_OPACITY_V_TRANSPARENT                                          0x00000000
#define NV902D_SET_PIXELS_FROM_CPU_MONO_OPACITY_V_OPAQUE                                               0x00000001

#define NV902D_SET_PIXELS_FROM_CPU_SRC_WIDTH                                                               0x0838
#define NV902D_SET_PIXELS_FROM_CPU_SRC_WIDTH_V                                                               31:0

#define NV902D_SET_PIXELS_FROM_CPU_SRC_HEIGHT                                                              0x083c
#define NV902D_SET_PIXELS_FROM_CPU_SRC_HEIGHT_V                                                              31:0

#define NV902D_SET_PIXELS_FROM_CPU_DX_DU_FRAC                                                              0x0840
#define NV902D_SET_PIXELS_FROM_CPU_DX_DU_FRAC_V                                                              31:0

#define NV902D_SET_PIXELS_FROM_CPU_DX_DU_INT                                                               0x0844
#define NV902D_SET_PIXELS_FROM_CPU_DX_DU_INT_V                                                               31:0

#define NV902D_SET_PIXELS_FROM_CPU_DY_DV_FRAC                                                              0x0848
#define NV902D_SET_PIXELS_FROM_CPU_DY_DV_FRAC_V                                                              31:0

#define NV902D_SET_PIXELS_FROM_CPU_DY_DV_INT                                                               0x084c
#define NV902D_SET_PIXELS_FROM_CPU_DY_DV_INT_V                                                               31:0

#define NV902D_SET_PIXELS_FROM_CPU_DST_X0_FRAC                                                             0x0850
#define NV902D_SET_PIXELS_FROM_CPU_DST_X0_FRAC_V                                                             31:0

#define NV902D_SET_PIXELS_FROM_CPU_DST_X0_INT                                                              0x0854
#define NV902D_SET_PIXELS_FROM_CPU_DST_X0_INT_V                                                              31:0

#define NV902D_SET_PIXELS_FROM_CPU_DST_Y0_FRAC                                                             0x0858
#define NV902D_SET_PIXELS_FROM_CPU_DST_Y0_FRAC_V                                                             31:0

#define NV902D_SET_PIXELS_FROM_CPU_DST_Y0_INT                                                              0x085c
#define NV902D_SET_PIXELS_FROM_CPU_DST_Y0_INT_V                                                              31:0

#define NV902D_PIXELS_FROM_CPU_DATA                                                                        0x0860
#define NV902D_PIXELS_FROM_CPU_DATA_V                                                                        31:0

#define NV902D_SET_BIG_ENDIAN_CONTROL                                                                      0x0870
#define NV902D_SET_BIG_ENDIAN_CONTROL_X32_SWAP_1                                                              0:0
#define NV902D_SET_BIG_ENDIAN_CONTROL_X32_SWAP_4                                                              1:1
#define NV902D_SET_BIG_ENDIAN_CONTROL_X32_SWAP_8                                                              2:2
#define NV902D_SET_BIG_ENDIAN_CONTROL_X32_SWAP_16                                                             3:3
#define NV902D_SET_BIG_ENDIAN_CONTROL_X16_SWAP_1                                                              4:4
#define NV902D_SET_BIG_ENDIAN_CONTROL_X16_SWAP_4                                                              5:5
#define NV902D_SET_BIG_ENDIAN_CONTROL_X16_SWAP_8                                                              6:6
#define NV902D_SET_BIG_ENDIAN_CONTROL_X16_SWAP_16                                                             7:7
#define NV902D_SET_BIG_ENDIAN_CONTROL_X8_SWAP_1                                                               8:8
#define NV902D_SET_BIG_ENDIAN_CONTROL_X8_SWAP_4                                                               9:9
#define NV902D_SET_BIG_ENDIAN_CONTROL_X8_SWAP_8                                                             10:10
#define NV902D_SET_BIG_ENDIAN_CONTROL_X8_SWAP_16                                                            11:11
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_CGA6_SWAP_1                                                     12:12
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_CGA6_SWAP_4                                                     13:13
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_CGA6_SWAP_8                                                     14:14
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_CGA6_SWAP_16                                                    15:15
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_LE_SWAP_1                                                       16:16
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_LE_SWAP_4                                                       17:17
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_LE_SWAP_8                                                       18:18
#define NV902D_SET_BIG_ENDIAN_CONTROL_I1_X8_LE_SWAP_16                                                      19:19
#define NV902D_SET_BIG_ENDIAN_CONTROL_I4_SWAP_1                                                             20:20
#define NV902D_SET_BIG_ENDIAN_CONTROL_I4_SWAP_4                                                             21:21
#define NV902D_SET_BIG_ENDIAN_CONTROL_I4_SWAP_8                                                             22:22
#define NV902D_SET_BIG_ENDIAN_CONTROL_I4_SWAP_16                                                            23:23
#define NV902D_SET_BIG_ENDIAN_CONTROL_I8_SWAP_1                                                             24:24
#define NV902D_SET_BIG_ENDIAN_CONTROL_I8_SWAP_4                                                             25:25
#define NV902D_SET_BIG_ENDIAN_CONTROL_I8_SWAP_8                                                             26:26
#define NV902D_SET_BIG_ENDIAN_CONTROL_I8_SWAP_16                                                            27:27
#define NV902D_SET_BIG_ENDIAN_CONTROL_OVERRIDE                                                              28:28

#define NV902D_SET_PIXELS_FROM_MEMORY_BLOCK_SHAPE                                                          0x0880
#define NV902D_SET_PIXELS_FROM_MEMORY_BLOCK_SHAPE_V                                                           2:0
#define NV902D_SET_PIXELS_FROM_MEMORY_BLOCK_SHAPE_V_AUTO                                               0x00000000
#define NV902D_SET_PIXELS_FROM_MEMORY_BLOCK_SHAPE_V_SHAPE_8X8                                          0x00000001
#define NV902D_SET_PIXELS_FROM_MEMORY_BLOCK_SHAPE_V_SHAPE_16X4                                         0x00000002

#define NV902D_SET_PIXELS_FROM_MEMORY_CORRAL_SIZE                                                          0x0884
#define NV902D_SET_PIXELS_FROM_MEMORY_CORRAL_SIZE_V                                                           9:0

#define NV902D_SET_PIXELS_FROM_MEMORY_SAFE_OVERLAP                                                         0x0888
#define NV902D_SET_PIXELS_FROM_MEMORY_SAFE_OVERLAP_V                                                          0:0
#define NV902D_SET_PIXELS_FROM_MEMORY_SAFE_OVERLAP_V_FALSE                                             0x00000000
#define NV902D_SET_PIXELS_FROM_MEMORY_SAFE_OVERLAP_V_TRUE                                              0x00000001

#define NV902D_SET_PIXELS_FROM_MEMORY_SAMPLE_MODE                                                          0x088c
#define NV902D_SET_PIXELS_FROM_MEMORY_SAMPLE_MODE_ORIGIN                                                      0:0
#define NV902D_SET_PIXELS_FROM_MEMORY_SAMPLE_MODE_ORIGIN_CENTER                                        0x00000000
#define NV902D_SET_PIXELS_FROM_MEMORY_SAMPLE_MODE_ORIGIN_CORNER                                        0x00000001
#define NV902D_SET_PIXELS_FROM_MEMORY_SAMPLE_MODE_FILTER                                                      4:4
#define NV902D_SET_PIXELS_FROM_MEMORY_SAMPLE_MODE_FILTER_POINT                                         0x00000000
#define NV902D_SET_PIXELS_FROM_MEMORY_SAMPLE_MODE_FILTER_BILINEAR                                      0x00000001

#define NV902D_SET_PIXELS_FROM_MEMORY_DST_X0                                                               0x08b0
#define NV902D_SET_PIXELS_FROM_MEMORY_DST_X0_V                                                               31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_DST_Y0                                                               0x08b4
#define NV902D_SET_PIXELS_FROM_MEMORY_DST_Y0_V                                                               31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_DST_WIDTH                                                            0x08b8
#define NV902D_SET_PIXELS_FROM_MEMORY_DST_WIDTH_V                                                            31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_DST_HEIGHT                                                           0x08bc
#define NV902D_SET_PIXELS_FROM_MEMORY_DST_HEIGHT_V                                                           31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_DU_DX_FRAC                                                           0x08c0
#define NV902D_SET_PIXELS_FROM_MEMORY_DU_DX_FRAC_V                                                           31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_DU_DX_INT                                                            0x08c4
#define NV902D_SET_PIXELS_FROM_MEMORY_DU_DX_INT_V                                                            31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_DV_DY_FRAC                                                           0x08c8
#define NV902D_SET_PIXELS_FROM_MEMORY_DV_DY_FRAC_V                                                           31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_DV_DY_INT                                                            0x08cc
#define NV902D_SET_PIXELS_FROM_MEMORY_DV_DY_INT_V                                                            31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_SRC_X0_FRAC                                                          0x08d0
#define NV902D_SET_PIXELS_FROM_MEMORY_SRC_X0_FRAC_V                                                          31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_SRC_X0_INT                                                           0x08d4
#define NV902D_SET_PIXELS_FROM_MEMORY_SRC_X0_INT_V                                                           31:0

#define NV902D_SET_PIXELS_FROM_MEMORY_SRC_Y0_FRAC                                                          0x08d8
#define NV902D_SET_PIXELS_FROM_MEMORY_SRC_Y0_FRAC_V                                                          31:0

#define NV902D_PIXELS_FROM_MEMORY_SRC_Y0_INT                                                               0x08dc
#define NV902D_PIXELS_FROM_MEMORY_SRC_Y0_INT_V                                                               31:0

#define NV902D_SET_FALCON00                                                                                0x08e0
#define NV902D_SET_FALCON00_V                                                                                31:0

#define NV902D_SET_FALCON01                                                                                0x08e4
#define NV902D_SET_FALCON01_V                                                                                31:0

#define NV902D_SET_FALCON02                                                                                0x08e8
#define NV902D_SET_FALCON02_V                                                                                31:0

#define NV902D_SET_FALCON03                                                                                0x08ec
#define NV902D_SET_FALCON03_V                                                                                31:0

#define NV902D_SET_FALCON04                                                                                0x08f0
#define NV902D_SET_FALCON04_V                                                                                31:0

#define NV902D_SET_FALCON05                                                                                0x08f4
#define NV902D_SET_FALCON05_V                                                                                31:0

#define NV902D_SET_FALCON06                                                                                0x08f8
#define NV902D_SET_FALCON06_V                                                                                31:0

#define NV902D_SET_FALCON07                                                                                0x08fc
#define NV902D_SET_FALCON07_V                                                                                31:0

#define NV902D_SET_FALCON08                                                                                0x0900
#define NV902D_SET_FALCON08_V                                                                                31:0

#define NV902D_SET_FALCON09                                                                                0x0904
#define NV902D_SET_FALCON09_V                                                                                31:0

#define NV902D_SET_FALCON10                                                                                0x0908
#define NV902D_SET_FALCON10_V                                                                                31:0

#define NV902D_SET_FALCON11                                                                                0x090c
#define NV902D_SET_FALCON11_V                                                                                31:0

#define NV902D_SET_FALCON12                                                                                0x0910
#define NV902D_SET_FALCON12_V                                                                                31:0

#define NV902D_SET_FALCON13                                                                                0x0914
#define NV902D_SET_FALCON13_V                                                                                31:0

#define NV902D_SET_FALCON14                                                                                0x0918
#define NV902D_SET_FALCON14_V                                                                                31:0

#define NV902D_SET_FALCON15                                                                                0x091c
#define NV902D_SET_FALCON15_V                                                                                31:0

#define NV902D_SET_FALCON16                                                                                0x0920
#define NV902D_SET_FALCON16_V                                                                                31:0

#define NV902D_SET_FALCON17                                                                                0x0924
#define NV902D_SET_FALCON17_V                                                                                31:0

#define NV902D_SET_FALCON18                                                                                0x0928
#define NV902D_SET_FALCON18_V                                                                                31:0

#define NV902D_SET_FALCON19                                                                                0x092c
#define NV902D_SET_FALCON19_V                                                                                31:0

#define NV902D_SET_FALCON20                                                                                0x0930
#define NV902D_SET_FALCON20_V                                                                                31:0

#define NV902D_SET_FALCON21                                                                                0x0934
#define NV902D_SET_FALCON21_V                                                                                31:0

#define NV902D_SET_FALCON22                                                                                0x0938
#define NV902D_SET_FALCON22_V                                                                                31:0

#define NV902D_SET_FALCON23                                                                                0x093c
#define NV902D_SET_FALCON23_V                                                                                31:0

#define NV902D_SET_FALCON24                                                                                0x0940
#define NV902D_SET_FALCON24_V                                                                                31:0

#define NV902D_SET_FALCON25                                                                                0x0944
#define NV902D_SET_FALCON25_V                                                                                31:0

#define NV902D_SET_FALCON26                                                                                0x0948
#define NV902D_SET_FALCON26_V                                                                                31:0

#define NV902D_SET_FALCON27                                                                                0x094c
#define NV902D_SET_FALCON27_V                                                                                31:0

#define NV902D_SET_FALCON28                                                                                0x0950
#define NV902D_SET_FALCON28_V                                                                                31:0

#define NV902D_SET_FALCON29                                                                                0x0954
#define NV902D_SET_FALCON29_V                                                                                31:0

#define NV902D_SET_FALCON30                                                                                0x0958
#define NV902D_SET_FALCON30_V                                                                                31:0

#define NV902D_SET_FALCON31                                                                                0x095c
#define NV902D_SET_FALCON31_V                                                                                31:0

#define NV902D_MME_DMA_WRITE_METHOD_BARRIER                                                                0x0dec
#define NV902D_MME_DMA_WRITE_METHOD_BARRIER_V                                                                 0:0

#define NV902D_SET_MME_SHADOW_SCRATCH(i)                                                           (0x3400+(i)*4)
#define NV902D_SET_MME_SHADOW_SCRATCH_V                                                                      31:0

#define NV902D_CALL_MME_MACRO(j)                                                                   (0x3800+(j)*8)
#define NV902D_CALL_MME_MACRO_V                                                                              31:0

#define NV902D_CALL_MME_DATA(j)                                                                    (0x3804+(j)*8)
#define NV902D_CALL_MME_DATA_V                                                                               31:0

#endif /* _cl_fermi_twod_a_h_ */
