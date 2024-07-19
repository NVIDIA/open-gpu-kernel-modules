/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl2080_notification_h_
#define _cl2080_notification_h_

#include "nvcfg_sdk.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nvcfg_sdk.h"

/* event values */
#define NV2080_NOTIFIERS_SW                                        (0)
#define NV2080_NOTIFIERS_HOTPLUG                                   (1)
#define NV2080_NOTIFIERS_POWER_CONNECTOR                           (2)
#define NV2080_NOTIFIERS_THERMAL_SW                                (3)
#define NV2080_NOTIFIERS_THERMAL_HW                                (4)
#define NV2080_NOTIFIERS_FULL_SCREEN_CHANGE                        (5)
#define NV2080_NOTIFIERS_EVENTBUFFER                               (6)
#define NV2080_NOTIFIERS_DP_IRQ                                    (7)
#define NV2080_NOTIFIERS_GR_DEBUG_INTR                             (8)
#define NV2080_NOTIFIERS_PMU_EVENT                                 (9)
#define NV2080_NOTIFIERS_PMU_COMMAND                               (10)
#define NV2080_NOTIFIERS_TIMER                                     (11)
#define NV2080_NOTIFIERS_GRAPHICS                                  (12)
#define NV2080_NOTIFIERS_PPP                                       (13)
#define NV2080_NOTIFIERS_VLD                                       (14) // also known as BSP
#define NV2080_NOTIFIERS_NVDEC0                                    NV2080_NOTIFIERS_VLD
#define NV2080_NOTIFIERS_NVDEC1                                    (15)
#define NV2080_NOTIFIERS_NVDEC2                                    (16)
#define NV2080_NOTIFIERS_NVDEC3                                    (17)
#define NV2080_NOTIFIERS_NVDEC4                                    (18)
#define NV2080_NOTIFIERS_NVDEC5                                    (19)
#define NV2080_NOTIFIERS_NVDEC6                                    (20)
#define NV2080_NOTIFIERS_NVDEC7                                    (21)
#define NV2080_NOTIFIERS_PDEC                                      (22) // also known as VP
#define NV2080_NOTIFIERS_CE0                                       (23)
#define NV2080_NOTIFIERS_CE1                                       (24)
#define NV2080_NOTIFIERS_CE2                                       (25)
#define NV2080_NOTIFIERS_CE3                                       (26)
#define NV2080_NOTIFIERS_CE4                                       (27)
#define NV2080_NOTIFIERS_CE5                                       (28)
#define NV2080_NOTIFIERS_CE6                                       (29)
#define NV2080_NOTIFIERS_CE7                                       (30)
#define NV2080_NOTIFIERS_CE8                                       (31)
#define NV2080_NOTIFIERS_CE9                                       (32)
#define NV2080_NOTIFIERS_PSTATE_CHANGE                             (33)
#define NV2080_NOTIFIERS_HDCP_STATUS_CHANGE                        (34)
#define NV2080_NOTIFIERS_FIFO_EVENT_MTHD                           (35)
#define NV2080_NOTIFIERS_PRIV_RING_HANG                            (36)
#define NV2080_NOTIFIERS_RC_ERROR                                  (37)
#define NV2080_NOTIFIERS_MSENC                                     (38)
#define NV2080_NOTIFIERS_NVENC0                                    NV2080_NOTIFIERS_MSENC
#define NV2080_NOTIFIERS_NVENC1                                    (39)
#define NV2080_NOTIFIERS_NVENC2                                    (40)
#define NV2080_NOTIFIERS_UNUSED_0                                  (41) // Unused
#define NV2080_NOTIFIERS_ACPI_NOTIFY                               (42)
#define NV2080_NOTIFIERS_COOLER_DIAG_ZONE                          (43)
#define NV2080_NOTIFIERS_THERMAL_DIAG_ZONE                         (44)
#define NV2080_NOTIFIERS_AUDIO_HDCP_REQUEST                        (45)
#define NV2080_NOTIFIERS_WORKLOAD_MODULATION_CHANGE                (46)
#define NV2080_NOTIFIERS_GPIO_0_RISING_INTERRUPT                   (47)
#define NV2080_NOTIFIERS_GPIO_1_RISING_INTERRUPT                   (48)
#define NV2080_NOTIFIERS_GPIO_2_RISING_INTERRUPT                   (49)
#define NV2080_NOTIFIERS_GPIO_3_RISING_INTERRUPT                   (50)
#define NV2080_NOTIFIERS_GPIO_4_RISING_INTERRUPT                   (51)
#define NV2080_NOTIFIERS_GPIO_5_RISING_INTERRUPT                   (52)
#define NV2080_NOTIFIERS_GPIO_6_RISING_INTERRUPT                   (53)
#define NV2080_NOTIFIERS_GPIO_7_RISING_INTERRUPT                   (54)
#define NV2080_NOTIFIERS_GPIO_8_RISING_INTERRUPT                   (55)
#define NV2080_NOTIFIERS_GPIO_9_RISING_INTERRUPT                   (56)
#define NV2080_NOTIFIERS_GPIO_10_RISING_INTERRUPT                  (57)
#define NV2080_NOTIFIERS_GPIO_11_RISING_INTERRUPT                  (58)
#define NV2080_NOTIFIERS_GPIO_12_RISING_INTERRUPT                  (59)
#define NV2080_NOTIFIERS_GPIO_13_RISING_INTERRUPT                  (60)
#define NV2080_NOTIFIERS_GPIO_14_RISING_INTERRUPT                  (61)
#define NV2080_NOTIFIERS_GPIO_15_RISING_INTERRUPT                  (62)
#define NV2080_NOTIFIERS_GPIO_16_RISING_INTERRUPT                  (63)
#define NV2080_NOTIFIERS_GPIO_17_RISING_INTERRUPT                  (64)
#define NV2080_NOTIFIERS_GPIO_18_RISING_INTERRUPT                  (65)
#define NV2080_NOTIFIERS_GPIO_19_RISING_INTERRUPT                  (66)
#define NV2080_NOTIFIERS_GPIO_20_RISING_INTERRUPT                  (67)
#define NV2080_NOTIFIERS_GPIO_21_RISING_INTERRUPT                  (68)
#define NV2080_NOTIFIERS_GPIO_22_RISING_INTERRUPT                  (69)
#define NV2080_NOTIFIERS_GPIO_23_RISING_INTERRUPT                  (70)
#define NV2080_NOTIFIERS_GPIO_24_RISING_INTERRUPT                  (71)
#define NV2080_NOTIFIERS_GPIO_25_RISING_INTERRUPT                  (72)
#define NV2080_NOTIFIERS_GPIO_26_RISING_INTERRUPT                  (73)
#define NV2080_NOTIFIERS_GPIO_27_RISING_INTERRUPT                  (74)
#define NV2080_NOTIFIERS_GPIO_28_RISING_INTERRUPT                  (75)
#define NV2080_NOTIFIERS_GPIO_29_RISING_INTERRUPT                  (76)
#define NV2080_NOTIFIERS_GPIO_30_RISING_INTERRUPT                  (77)
#define NV2080_NOTIFIERS_GPIO_31_RISING_INTERRUPT                  (78)
#define NV2080_NOTIFIERS_GPIO_0_FALLING_INTERRUPT                  (79)
#define NV2080_NOTIFIERS_GPIO_1_FALLING_INTERRUPT                  (80)
#define NV2080_NOTIFIERS_GPIO_2_FALLING_INTERRUPT                  (81)
#define NV2080_NOTIFIERS_GPIO_3_FALLING_INTERRUPT                  (82)
#define NV2080_NOTIFIERS_GPIO_4_FALLING_INTERRUPT                  (83)
#define NV2080_NOTIFIERS_GPIO_5_FALLING_INTERRUPT                  (84)
#define NV2080_NOTIFIERS_GPIO_6_FALLING_INTERRUPT                  (85)
#define NV2080_NOTIFIERS_GPIO_7_FALLING_INTERRUPT                  (86)
#define NV2080_NOTIFIERS_GPIO_8_FALLING_INTERRUPT                  (87)
#define NV2080_NOTIFIERS_GPIO_9_FALLING_INTERRUPT                  (88)
#define NV2080_NOTIFIERS_GPIO_10_FALLING_INTERRUPT                 (89)
#define NV2080_NOTIFIERS_GPIO_11_FALLING_INTERRUPT                 (90)
#define NV2080_NOTIFIERS_GPIO_12_FALLING_INTERRUPT                 (91)
#define NV2080_NOTIFIERS_GPIO_13_FALLING_INTERRUPT                 (92)
#define NV2080_NOTIFIERS_GPIO_14_FALLING_INTERRUPT                 (93)
#define NV2080_NOTIFIERS_GPIO_15_FALLING_INTERRUPT                 (94)
#define NV2080_NOTIFIERS_GPIO_16_FALLING_INTERRUPT                 (95)
#define NV2080_NOTIFIERS_GPIO_17_FALLING_INTERRUPT                 (96)
#define NV2080_NOTIFIERS_GPIO_18_FALLING_INTERRUPT                 (97)
#define NV2080_NOTIFIERS_GPIO_19_FALLING_INTERRUPT                 (98)
#define NV2080_NOTIFIERS_GPIO_20_FALLING_INTERRUPT                 (99)
#define NV2080_NOTIFIERS_GPIO_21_FALLING_INTERRUPT                 (100)
#define NV2080_NOTIFIERS_GPIO_22_FALLING_INTERRUPT                 (101)
#define NV2080_NOTIFIERS_GPIO_23_FALLING_INTERRUPT                 (102)
#define NV2080_NOTIFIERS_GPIO_24_FALLING_INTERRUPT                 (103)
#define NV2080_NOTIFIERS_GPIO_25_FALLING_INTERRUPT                 (104)
#define NV2080_NOTIFIERS_GPIO_26_FALLING_INTERRUPT                 (105)
#define NV2080_NOTIFIERS_GPIO_27_FALLING_INTERRUPT                 (106)
#define NV2080_NOTIFIERS_GPIO_28_FALLING_INTERRUPT                 (107)
#define NV2080_NOTIFIERS_GPIO_29_FALLING_INTERRUPT                 (108)
#define NV2080_NOTIFIERS_GPIO_30_FALLING_INTERRUPT                 (109)
#define NV2080_NOTIFIERS_GPIO_31_FALLING_INTERRUPT                 (110)
#define NV2080_NOTIFIERS_ECC_SBE                                   (111)
#define NV2080_NOTIFIERS_ECC_DBE                                   (112)
#define NV2080_NOTIFIERS_STEREO_EMITTER_DETECTION                  (113)
#define NV2080_NOTIFIERS_GC5_GPU_READY                             (114)
#define NV2080_NOTIFIERS_SEC2                                      (115)
#define NV2080_NOTIFIERS_GC6_REFCOUNT_INC                          (116)
#define NV2080_NOTIFIERS_GC6_REFCOUNT_DEC                          (117)
#define NV2080_NOTIFIERS_POWER_EVENT                               (118)
#define NV2080_NOTIFIERS_CLOCKS_CHANGE                             (119)
#define NV2080_NOTIFIERS_HOTPLUG_PROCESSING_COMPLETE               (120)
#define NV2080_NOTIFIERS_PHYSICAL_PAGE_FAULT                       (121)
#define NV2080_NOTIFIERS_RESERVED122                               (122)
#define NV2080_NOTIFIERS_NVLINK_ERROR_FATAL                        (123)
#define NV2080_NOTIFIERS_PRIV_REG_ACCESS_FAULT                     (124)
#define NV2080_NOTIFIERS_NVLINK_ERROR_RECOVERY_REQUIRED            (125)
#define NV2080_NOTIFIERS_NVJPG                                     (126)
#define NV2080_NOTIFIERS_NVJPEG0                                   NV2080_NOTIFIERS_NVJPG
#define NV2080_NOTIFIERS_NVJPEG1                                   (127)
#define NV2080_NOTIFIERS_NVJPEG2                                   (128)
#define NV2080_NOTIFIERS_NVJPEG3                                   (129)
#define NV2080_NOTIFIERS_NVJPEG4                                   (130)
#define NV2080_NOTIFIERS_NVJPEG5                                   (131)
#define NV2080_NOTIFIERS_NVJPEG6                                   (132)
#define NV2080_NOTIFIERS_NVJPEG7                                   (133)
#define NV2080_NOTIFIERS_RUNLIST_AND_ENG_IDLE                      (134)
#define NV2080_NOTIFIERS_RUNLIST_ACQUIRE                           (135)
#define NV2080_NOTIFIERS_RUNLIST_ACQUIRE_AND_ENG_IDLE              (136)
#define NV2080_NOTIFIERS_RUNLIST_IDLE                              (137)
#define NV2080_NOTIFIERS_TSG_PREEMPT_COMPLETE                      (138)
#define NV2080_NOTIFIERS_RUNLIST_PREEMPT_COMPLETE                  (139)
#define NV2080_NOTIFIERS_CTXSW_TIMEOUT                             (140)
#define NV2080_NOTIFIERS_INFOROM_ECC_OBJECT_UPDATED                (141)
#define NV2080_NOTIFIERS_NVTELEMETRY_REPORT_EVENT                  (142)
#define NV2080_NOTIFIERS_DSTATE_XUSB_PPC                           (143)
#define NV2080_NOTIFIERS_FECS_CTX_SWITCH                           (144)
#define NV2080_NOTIFIERS_XUSB_PPC_CONNECTED                        (145)
#define NV2080_NOTIFIERS_GR0                                       NV2080_NOTIFIERS_GRAPHICS
#define NV2080_NOTIFIERS_GR1                                       (146)
#define NV2080_NOTIFIERS_GR2                                       (147)
#define NV2080_NOTIFIERS_GR3                                       (148)
#define NV2080_NOTIFIERS_GR4                                       (149)
#define NV2080_NOTIFIERS_GR5                                       (150)
#define NV2080_NOTIFIERS_GR6                                       (151)
#define NV2080_NOTIFIERS_GR7                                       (152)
#define NV2080_NOTIFIERS_OFA                                       (153)
#define NV2080_NOTIFIERS_OFA0                                      NV2080_NOTIFIERS_OFA
#define NV2080_NOTIFIERS_DSTATE_HDA                                (154)
#define NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL                    (155)
#define NV2080_NOTIFIERS_POISON_ERROR_FATAL                        (156)
#define NV2080_NOTIFIERS_UCODE_RESET                               (157)
#define NV2080_NOTIFIERS_PLATFORM_POWER_MODE_CHANGE                (158)
#define NV2080_NOTIFIERS_SMC_CONFIG_UPDATE                         (159)
#define NV2080_NOTIFIERS_INFOROM_RRL_OBJECT_UPDATED                (160)
#define NV2080_NOTIFIERS_INFOROM_PBL_OBJECT_UPDATED                (161)
#define NV2080_NOTIFIERS_LPWR_DIFR_PREFETCH_REQUEST                (162)
#define NV2080_NOTIFIERS_SEC_FAULT_ERROR                           (163)
#define NV2080_NOTIFIERS_UNUSED_1                                  (164) // Unused
#define NV2080_NOTIFIERS_NVLINK_INFO_LINK_UP                       (165)
// removal tracking bug: 3748354
#define NV2080_NOTIFIERS_CE10                                      (166)
#define NV2080_NOTIFIERS_CE11                                      (167)
#define NV2080_NOTIFIERS_CE12                                      (168)
#define NV2080_NOTIFIERS_CE13                                      (169)
#define NV2080_NOTIFIERS_CE14                                      (170)
#define NV2080_NOTIFIERS_CE15                                      (171)
#define NV2080_NOTIFIERS_CE16                                      (172)
#define NV2080_NOTIFIERS_CE17                                      (173)
#define NV2080_NOTIFIERS_CE18                                      (174)
#define NV2080_NOTIFIERS_CE19                                      (175)
#define NV2080_NOTIFIERS_NVLINK_INFO_LINK_DOWN                     (176)
#define NV2080_NOTIFIERS_NVPCF_EVENTS                              (177)
#define NV2080_NOTIFIERS_HDMI_FRL_RETRAINING_REQUEST               (178)
#define NV2080_NOTIFIERS_VRR_SET_TIMEOUT                           (179)
// removal tracking bug: 3748354
#define NV2080_NOTIFIERS_OFA1                                      (180)
#define NV2080_NOTIFIERS_AUX_POWER_EVENT                           (181)
#define NV2080_NOTIFIERS_AUX_POWER_STATE_CHANGE                    (182)
#define NV2080_NOTIFIERS_RESERVED_183                              (183) // Unused
#define NV2080_NOTIFIERS_GSP_PERF_TRACE                            (184)
#define NV2080_NOTIFIERS_INBAND_RESPONSE                           (185)
#define NV2080_NOTIFIERS_RESERVED_186                              (186) // Unused
#define NV2080_NOTIFIERS_ECC_SBE_STORM                             (187)
#define NV2080_NOTIFIERS_DRAM_RETIREMENT_EVENT                     (188)
#define NV2080_NOTIFIERS_DRAM_RETIREMENT_FAILURE                   (189)
#define NV2080_NOTIFIERS_MAXCOUNT                                  (190)

// Indexed GR notifier reference
#define NV2080_NOTIFIERS_GR(x)         ((x == 0) ? (NV2080_NOTIFIERS_GR0) : (NV2080_NOTIFIERS_GR1 + (x - 1)))
#define NV2080_NOTIFIERS_GR_IDX(x)     ((x) - NV2080_NOTIFIERS_GR0)
#define NV2080_NOTIFIER_TYPE_IS_GR(x)  (((x) == NV2080_NOTIFIERS_GR0) || (((x) >= NV2080_NOTIFIERS_GR1) && ((x) <= NV2080_NOTIFIERS_GR7)))

// Indexed CE notifier reference
// removal tracking bug: 3748354
#define NV2080_NOTIFIERS_CE(x)         (((x) < 10) ? (NV2080_NOTIFIERS_CE0 + (x)) : (NV2080_NOTIFIERS_CE10 + (x) - 10))
#define NV2080_NOTIFIERS_CE_IDX(x)     (((x) <= NV2080_NOTIFIERS_CE9) ? ((x) - NV2080_NOTIFIERS_CE0) : ((x) - NV2080_NOTIFIERS_CE10 + 10))
#define NV2080_NOTIFIER_TYPE_IS_CE(x)  ((((x) >= NV2080_NOTIFIERS_CE0) && ((x) <= NV2080_NOTIFIERS_CE9)) || \
                                       (((x) >= NV2080_NOTIFIERS_CE10) && ((x) <= NV2080_NOTIFIERS_CE19)))

// Indexed MSENC notifier reference
#define NV2080_NOTIFIERS_NVENC(x)         (NV2080_NOTIFIERS_NVENC0 + (x))
#define NV2080_NOTIFIERS_NVENC_IDX(x)     ((x) - NV2080_NOTIFIERS_NVENC0)
#define NV2080_NOTIFIER_TYPE_IS_NVENC(x)  (((x) >= NV2080_NOTIFIERS_NVENC0) && ((x) <= NV2080_NOTIFIERS_NVENC2))
// Indexed NVDEC notifier reference
#define NV2080_NOTIFIERS_NVDEC(x)         (NV2080_NOTIFIERS_NVDEC0 + (x))
#define NV2080_NOTIFIERS_NVDEC_IDX(x)     ((x) - NV2080_NOTIFIERS_NVDEC0)
#define NV2080_NOTIFIER_TYPE_IS_NVDEC(x)  (((x) >= NV2080_NOTIFIERS_NVDEC0) && ((x) <= NV2080_NOTIFIERS_NVDEC7))
// Indexed NVJPEG notifier reference
#define NV2080_NOTIFIERS_NVJPEG(x)         (NV2080_NOTIFIERS_NVJPEG0 + (x))
#define NV2080_NOTIFIERS_NVJPEG_IDX(x)     ((x) - NV2080_NOTIFIERS_NVJPEG0)
#define NV2080_NOTIFIER_TYPE_IS_NVJPEG(x)  (((x) >= NV2080_NOTIFIERS_NVJPEG0) && ((x) <= NV2080_NOTIFIERS_NVJPEG7))

// Indexed OFA notifier reference
// removal tracking bug: 3748354
#define NV2080_NOTIFIERS_OFAn(x)         ((x == 0) ? (NV2080_NOTIFIERS_OFA0) : (NV2080_NOTIFIERS_OFA1))
#define NV2080_NOTIFIERS_OFA_IDX(x)     ((x == NV2080_NOTIFIERS_OFA0) ? ((x) - NV2080_NOTIFIERS_OFA0) : ((x) - NV2080_NOTIFIERS_OFA1 + 1))
#define NV2080_NOTIFIER_TYPE_IS_OFA(x)  (((x) == NV2080_NOTIFIERS_OFA0) || ((x) == NV2080_NOTIFIERS_OFA1))

#define NV2080_NOTIFIERS_GPIO_RISING_INTERRUPT(pin)                (NV2080_NOTIFIERS_GPIO_0_RISING_INTERRUPT + (pin))
#define NV2080_NOTIFIERS_GPIO_FALLING_INTERRUPT(pin)               (NV2080_NOTIFIERS_GPIO_0_FALLING_INTERRUPT + (pin))

#define NV2080_SUBDEVICE_NOTIFICATION_STATUS_IN_PROGRESS              (0x8000)
#define NV2080_SUBDEVICE_NOTIFICATION_STATUS_BAD_ARGUMENT             (0x4000)
#define NV2080_SUBDEVICE_NOTIFICATION_STATUS_ERROR_INVALID_STATE      (0x2000)
#define NV2080_SUBDEVICE_NOTIFICATION_STATUS_ERROR_STATE_IN_USE       (0x1000)
#define NV2080_SUBDEVICE_NOTIFICATION_STATUS_DONE_SUCCESS             (0x0000)

/* exported engine defines */
#define NV2080_ENGINE_TYPE_NULL                       (0x00000000)
#define NV2080_ENGINE_TYPE_GRAPHICS                   (0x00000001)
#define NV2080_ENGINE_TYPE_GR0                        NV2080_ENGINE_TYPE_GRAPHICS
#define NV2080_ENGINE_TYPE_GR1                        (0x00000002)
#define NV2080_ENGINE_TYPE_GR2                        (0x00000003)
#define NV2080_ENGINE_TYPE_GR3                        (0x00000004)
#define NV2080_ENGINE_TYPE_GR4                        (0x00000005)
#define NV2080_ENGINE_TYPE_GR5                        (0x00000006)
#define NV2080_ENGINE_TYPE_GR6                        (0x00000007)
#define NV2080_ENGINE_TYPE_GR7                        (0x00000008)
#define NV2080_ENGINE_TYPE_COPY0                      (0x00000009)
#define NV2080_ENGINE_TYPE_COPY1                      (0x0000000a)
#define NV2080_ENGINE_TYPE_COPY2                      (0x0000000b)
#define NV2080_ENGINE_TYPE_COPY3                      (0x0000000c)
#define NV2080_ENGINE_TYPE_COPY4                      (0x0000000d)
#define NV2080_ENGINE_TYPE_COPY5                      (0x0000000e)
#define NV2080_ENGINE_TYPE_COPY6                      (0x0000000f)
#define NV2080_ENGINE_TYPE_COPY7                      (0x00000010)
#define NV2080_ENGINE_TYPE_COPY8                      (0x00000011)
#define NV2080_ENGINE_TYPE_COPY9                      (0x00000012)
#define NV2080_ENGINE_TYPE_BSP                        (0x00000013)
#define NV2080_ENGINE_TYPE_NVDEC0                     NV2080_ENGINE_TYPE_BSP
#define NV2080_ENGINE_TYPE_NVDEC1                     (0x00000014)
#define NV2080_ENGINE_TYPE_NVDEC2                     (0x00000015)
#define NV2080_ENGINE_TYPE_NVDEC3                     (0x00000016)
#define NV2080_ENGINE_TYPE_NVDEC4                     (0x00000017)
#define NV2080_ENGINE_TYPE_NVDEC5                     (0x00000018)
#define NV2080_ENGINE_TYPE_NVDEC6                     (0x00000019)
#define NV2080_ENGINE_TYPE_NVDEC7                     (0x0000001a)
#define NV2080_ENGINE_TYPE_MSENC                      (0x0000001b)
#define NV2080_ENGINE_TYPE_NVENC0                      NV2080_ENGINE_TYPE_MSENC  /* Mutually exclusive alias */
#define NV2080_ENGINE_TYPE_NVENC1                     (0x0000001c)
#define NV2080_ENGINE_TYPE_NVENC2                     (0x0000001d)
#define NV2080_ENGINE_TYPE_VP                         (0x0000001e)
#define NV2080_ENGINE_TYPE_ME                         (0x0000001f)
#define NV2080_ENGINE_TYPE_PPP                        (0x00000020)
#define NV2080_ENGINE_TYPE_MPEG                       (0x00000021)
#define NV2080_ENGINE_TYPE_SW                         (0x00000022)
#define NV2080_ENGINE_TYPE_CIPHER                     (0x00000023)
#define NV2080_ENGINE_TYPE_TSEC                       NV2080_ENGINE_TYPE_CIPHER
#define NV2080_ENGINE_TYPE_VIC                        (0x00000024)
#define NV2080_ENGINE_TYPE_MP                         (0x00000025)
#define NV2080_ENGINE_TYPE_SEC2                       (0x00000026)
#define NV2080_ENGINE_TYPE_HOST                       (0x00000027)
#define NV2080_ENGINE_TYPE_DPU                        (0x00000028)
#define NV2080_ENGINE_TYPE_PMU                        (0x00000029)
#define NV2080_ENGINE_TYPE_FBFLCN                     (0x0000002a)
#define NV2080_ENGINE_TYPE_NVJPG                      (0x0000002b)
#define NV2080_ENGINE_TYPE_NVJPEG0                     NV2080_ENGINE_TYPE_NVJPG
#define NV2080_ENGINE_TYPE_NVJPEG1                    (0x0000002c)
#define NV2080_ENGINE_TYPE_NVJPEG2                    (0x0000002d)
#define NV2080_ENGINE_TYPE_NVJPEG3                    (0x0000002e)
#define NV2080_ENGINE_TYPE_NVJPEG4                    (0x0000002f)
#define NV2080_ENGINE_TYPE_NVJPEG5                    (0x00000030)
#define NV2080_ENGINE_TYPE_NVJPEG6                    (0x00000031)
#define NV2080_ENGINE_TYPE_NVJPEG7                    (0x00000032)
#define NV2080_ENGINE_TYPE_OFA                        (0x00000033)
#define NV2080_ENGINE_TYPE_OFA0                       NV2080_ENGINE_TYPE_OFA
// removal tracking bug: 3748354
// Update the TYPE_COMP_DECOMP_COPYN defines as well when you update COPYN defines
#define NV2080_ENGINE_TYPE_COPY10                     (0x00000034)
#define NV2080_ENGINE_TYPE_COPY11                     (0x00000035)
#define NV2080_ENGINE_TYPE_COPY12                     (0x00000036)
#define NV2080_ENGINE_TYPE_COPY13                     (0x00000037)
#define NV2080_ENGINE_TYPE_COPY14                     (0x00000038)
#define NV2080_ENGINE_TYPE_COPY15                     (0x00000039)
#define NV2080_ENGINE_TYPE_COPY16                     (0x0000003a)
#define NV2080_ENGINE_TYPE_COPY17                     (0x0000003b)
#define NV2080_ENGINE_TYPE_COPY18                     (0x0000003c)
#define NV2080_ENGINE_TYPE_COPY19                     (0x0000003d)
// removal tracking bug: 3748354
#define NV2080_ENGINE_TYPE_OFA1                       (0x0000003e)
#define NV2080_ENGINE_TYPE_RESERVED3f                 (0x0000003f)
// See TBD documentation for how these defines work with existing ENGINE_TYPE_COPYN defines
// removal tracking bug: 3748354
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY0          (0x00000040)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY1          (0x00000041)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY2          (0x00000042)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY3          (0x00000043)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY4          (0x00000044)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY5          (0x00000045)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY6          (0x00000046)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY7          (0x00000047)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY8          (0x00000048)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY9          (0x00000049)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY10         (0x0000004a)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY11         (0x0000004b)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY12         (0x0000004c)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY13         (0x0000004d)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY14         (0x0000004e)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY15         (0x0000004f)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY16         (0x00000050)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY17         (0x00000051)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY18         (0x00000052)
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY19         (0x00000053)
#define NV2080_ENGINE_TYPE_LAST                       (0x00000054)
#define NV2080_ENGINE_TYPE_ALLENGINES                 (0xffffffff)

//
// NV2080_ENGINE_TYPE_COPY_SIZE is now defined as the maximum possible CE size.
// It does not reflect the max supported NV2080_ENGINE_TYPE_COPY counts. Bug 3713687 #90.
// It needs to use NV2080_ENGINE_TYPE_IS_COPY() to check if a CE is valid when
// the clients try to enumerate all NV2080_ENGINE_TYPE_COPYs.
//
#define NV2080_ENGINE_TYPE_COPY_SIZE 64

#define NV2080_ENGINE_TYPE_NVENC_SIZE 3
#define NV2080_ENGINE_TYPE_NVJPEG_SIZE 8
#define NV2080_ENGINE_TYPE_NVDEC_SIZE 8
#define NV2080_ENGINE_TYPE_GR_SIZE 8
#define NV2080_ENGINE_TYPE_OFA_SIZE 2

// Indexed engines
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY(i)      (NV2080_ENGINE_TYPE_COMP_DECOMP_COPY0 + (i))
#define NV2080_ENGINE_TYPE_IS_COMP_DECOMP_COPY(i)   (((i) >= NV2080_ENGINE_TYPE_COMP_DECOMP_COPY0) && ((i) <= NV2080_ENGINE_TYPE_COMP_DECOMP_COPY19))
#define NV2080_ENGINE_TYPE_COMP_DECOMP_COPY_IDX(i)  ((i) - NV2080_ENGINE_TYPE_COMP_DECOMP_COPY0)

// removal tracking bug: 3748354
#define NV2080_ENGINE_TYPE_COPY(i)     (((i) < 10) ? (NV2080_ENGINE_TYPE_COPY0 + (i)) : (NV2080_ENGINE_TYPE_COPY10 + (i) - 10))
#define NV2080_ENGINE_TYPE_IS_COPY(i)  ((((i) >= NV2080_ENGINE_TYPE_COPY0) && ((i) <= NV2080_ENGINE_TYPE_COPY9)) || \
                                        (((i) >= NV2080_ENGINE_TYPE_COPY10) && ((i) <= NV2080_ENGINE_TYPE_COPY19)) || \
                                        (NV2080_ENGINE_TYPE_IS_COMP_DECOMP_COPY(i)))
#define NV2080_ENGINE_TYPE_COPY_IDX(i) (((i) <= NV2080_ENGINE_TYPE_COPY9) ? \
                                        ((i) - NV2080_ENGINE_TYPE_COPY0) : ((i) - NV2080_ENGINE_TYPE_COPY10 + 10))

#define NV2080_ENGINE_TYPE_NVENC(i)    (NV2080_ENGINE_TYPE_NVENC0+(i))
#define NV2080_ENGINE_TYPE_IS_NVENC(i)  (((i) >= NV2080_ENGINE_TYPE_NVENC0) && ((i) < NV2080_ENGINE_TYPE_NVENC(NV2080_ENGINE_TYPE_NVENC_SIZE)))
#define NV2080_ENGINE_TYPE_NVENC_IDX(i) ((i) - NV2080_ENGINE_TYPE_NVENC0)

#define NV2080_ENGINE_TYPE_NVDEC(i)    (NV2080_ENGINE_TYPE_NVDEC0+(i))
#define NV2080_ENGINE_TYPE_IS_NVDEC(i)  (((i) >= NV2080_ENGINE_TYPE_NVDEC0) && ((i) < NV2080_ENGINE_TYPE_NVDEC(NV2080_ENGINE_TYPE_NVDEC_SIZE)))
#define NV2080_ENGINE_TYPE_NVDEC_IDX(i) ((i) - NV2080_ENGINE_TYPE_NVDEC0)

#define NV2080_ENGINE_TYPE_NVJPEG(i)    (NV2080_ENGINE_TYPE_NVJPEG0+(i))
#define NV2080_ENGINE_TYPE_IS_NVJPEG(i)  (((i) >= NV2080_ENGINE_TYPE_NVJPEG0) && ((i) < NV2080_ENGINE_TYPE_NVJPEG(NV2080_ENGINE_TYPE_NVJPEG_SIZE)))
#define NV2080_ENGINE_TYPE_NVJPEG_IDX(i) ((i) - NV2080_ENGINE_TYPE_NVJPEG0)

#define NV2080_ENGINE_TYPE_GR(i)       (NV2080_ENGINE_TYPE_GR0 + (i))
#define NV2080_ENGINE_TYPE_IS_GR(i)    (((i) >= NV2080_ENGINE_TYPE_GR0) && ((i) < NV2080_ENGINE_TYPE_GR(NV2080_ENGINE_TYPE_GR_SIZE)))
#define NV2080_ENGINE_TYPE_GR_IDX(i)   ((i) - NV2080_ENGINE_TYPE_GR0)

// removal tracking bug: 3748354
#define NV2080_ENGINE_TYPE_OFAn(i)       ((i == 0) ? (NV2080_ENGINE_TYPE_OFA0) : (NV2080_ENGINE_TYPE_OFA1))
#define NV2080_ENGINE_TYPE_IS_OFA(i)    (((i) == NV2080_ENGINE_TYPE_OFA0) || ((i) == NV2080_ENGINE_TYPE_OFA1))
#define NV2080_ENGINE_TYPE_OFA_IDX(i)   ((i == NV2080_ENGINE_TYPE_OFA0) ? ((i) - NV2080_ENGINE_TYPE_OFA0) : ((i) - NV2080_ENGINE_TYPE_OFA1 + 1))

#define NV2080_ENGINE_TYPE_IS_VALID(i) (((i) > (NV2080_ENGINE_TYPE_NULL)) && ((i) < (NV2080_ENGINE_TYPE_LAST)))

/* exported client defines */
#define NV2080_CLIENT_TYPE_TEX                        (0x00000001)
#define NV2080_CLIENT_TYPE_COLOR                      (0x00000002)
#define NV2080_CLIENT_TYPE_DEPTH                      (0x00000003)
#define NV2080_CLIENT_TYPE_DA                         (0x00000004)
#define NV2080_CLIENT_TYPE_FE                         (0x00000005)
#define NV2080_CLIENT_TYPE_SCC                        (0x00000006)
#define NV2080_CLIENT_TYPE_WID                        (0x00000007)
#define NV2080_CLIENT_TYPE_MSVLD                      (0x00000008)
#define NV2080_CLIENT_TYPE_MSPDEC                     (0x00000009)
#define NV2080_CLIENT_TYPE_MSPPP                      (0x0000000a)
#define NV2080_CLIENT_TYPE_VIC                        (0x0000000b)
#define NV2080_CLIENT_TYPE_ALLCLIENTS                 (0xffffffff)

/* GC5 Gpu Ready event defines */
#define NV2080_GC5_EXIT_COMPLETE                      (0x00000001)
#define NV2080_GC5_ENTRY_ABORTED                      (0x00000002)

/* Platform Power Mode event defines */
#define NV2080_PLATFORM_POWER_MODE_CHANGE_COMPLETION        (0x00000000)
#define NV2080_PLATFORM_POWER_MODE_CHANGE_ACPI_NOTIFICATION (0x00000001)

/* NvNotification[] fields and values */
#define NV2080_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT          (0x4000)
/* pio method data structure */
typedef volatile struct _cl2080_tag0 {
    NvV32 Reserved00[0x7c0];
} Nv2080Typedef, Nv20Subdevice0;
#define  NV2080_TYPEDEF                                          Nv20Subdevice0

/* HDCP Status change notification information */
typedef struct Nv2080HdcpStatusChangeNotificationRec {
    NvU32 displayId;
    NvU32 hdcpStatusChangeNotif;
} Nv2080HdcpStatusChangeNotification;

/* Pstate change notification information */
typedef struct Nv2080PStateChangeNotificationRec {
    struct {
        NvU32 nanoseconds[2];  /* nanoseconds since Jan. 1, 1970       0-   7*/
    } timeStamp;               /*                                       -0007*/
    NvU32 NewPstate;
} Nv2080PStateChangeNotification;

/* Clocks change notification information */
typedef struct Nv2080ClocksChangeNotificationRec {
    struct {
        NvU32 nanoseconds[2];  /* nanoseconds since Jan. 1, 1970       0-   7*/
    } timeStamp;               /*                                       -0007*/
} Nv2080ClocksChangeNotification;

/* WorkLoad Modulation state change notification information*/
typedef struct Nv2080WorkloadModulationChangeNotificationRec {
    struct {
        NvU32 nanoseconds[2];  /* nanoseconds since Jan. 1, 1970       0-   7*/
    } timeStamp;               /*                                       -0007*/
    NvBool WorkloadModulationEnabled;
} Nv2080WorkloadModulationChangeNotification;

/* Hotplug notification information */
typedef struct {
    NvU32 plugDisplayMask;
    NvU32 unplugDisplayMask;
} Nv2080HotplugNotification;

/* Power state changing notification information */
typedef struct {
    NvBool bSwitchToAC;
    NvBool bGPUCapabilityChanged;
    NvU32  displayMaskAffected;
} Nv2080PowerEventNotification;

/* DP IRQ notification information */
typedef struct Nv2080DpIrqNotificationRec {
    NvU32 displayId;
} Nv2080DpIrqNotification;

/* XUSB/PPC D-State change notification information */
typedef struct Nv2080DstateXusbPpcNotificationRec {
    NvU32 dstateXusb;
    NvU32 dstatePpc;
} Nv2080DstateXusbPpcNotification;

/* XUSB/PPC Connection status notification information */
typedef struct Nv2080XusbPpcConnectStateNotificationRec {
    NvBool bConnected;
} Nv2080XusbPpcConnectStateNotification;

/* ACPI event notification information */
typedef struct Nv2080ACPIEvent {
    NvU32 event;
} Nv2080ACPIEvent;

/* Cooler Zone notification information */
typedef struct _NV2080_COOLER_DIAG_ZONE_NOTIFICATION_REC {
    NvU32 currentZone;
} NV2080_COOLER_DIAG_ZONE_NOTIFICATION_REC;

/* Thermal Zone notification information */
typedef struct _NV2080_THERM_DIAG_ZONE_NOTIFICATION_REC {
    NvU32 currentZone;
} NV2080_THERM_DIAG_ZONE_NOTIFICATION_REC;

/* HDCP ref count change notification information */
typedef struct Nv2080AudioHdcpRequestRec {
    NvU32 displayId;
    NvU32 requestedState;
} Nv2080AudioHdcpRequest;

/* Gpu ready event information */
typedef struct Nv2080GC5GpuReadyParams {
    NvU32 event;
    NvU32 sciIntr0;
    NvU32 sciIntr1;
} Nv2080GC5GpuReadyParams;

/* Priv reg access fault notification information */
typedef struct {
    NvU32 errAddr;
} Nv2080PrivRegAccessFaultNotification;

/* HDA D-State change notification information
 * See @HDACODEC_DSTATE for definitions
 */
typedef struct Nv2080DstateHdaCodecNotificationRec {
    NvU32 dstateHdaCodec;
} Nv2080DstateHdaCodecNotification;

/* HDMI FRL retraining request notification information */
typedef struct Nv2080HdmiFrlRequestNotificationRec {
    NvU32 displayId;
} Nv2080HdmiFrlRequestNotification;

/*
 * Platform Power Mode event information
 */
typedef struct _NV2080_PLATFORM_POWER_MODE_CHANGE_STATUS {
    NvU8 platformPowerModeIndex;
    NvU8 platformPowerModeMask;
    NvU8 eventReason;
} NV2080_PLATFORM_POWER_MODE_CHANGE_STATUS;

#define NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_INDEX                         7:0
#define NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_MASK                          15:8
#define NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_REASON                        23:16

/*
 * ENGINE_INFO_TYPE_NV2080 of the engine for which the QOS interrupt has been raised
 */
typedef struct {
    NvU32 engineType;
} Nv2080QosIntrNotification;

typedef struct {
    NvU64 physAddress  NV_ALIGN_BYTES(8);
} Nv2080EccDbeNotification;

/*
 * LPWR DIFR Prefetch Request - Size of L2 Cache
 */
typedef struct {
    NvU32 l2CacheSize;
} Nv2080LpwrDifrPrefetchNotification;

/*
 * Nvlink Link status change Notification
 */
typedef struct {
    NvU32 GpuId;
    NvU32 linkId;
} Nv2080NvlinkLnkChangeNotification;

typedef struct {
    NvU32 head;
} Nv2080VrrSetTimeoutNotification;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl2080_notification_h_ */
