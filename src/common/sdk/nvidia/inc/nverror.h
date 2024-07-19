/*
 * Copyright (c) 1993-2024, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef NVERROR_H
#define NVERROR_H
/******************************************************************************
*
*   File:  nverror.h
*
*   Description:
*       This file contains the error codes set when the error notifier
*   is signaled.
*
******************************************************************************/

#include "nvcfg_sdk.h"

#define ROBUST_CHANNEL_GR_EXCEPTION                     (13)
#define ROBUST_CHANNEL_FAKE_ERROR                       (14)
#define ROBUST_CHANNEL_VBLANK_CALLBACK_TIMEOUT          (16)
#define ROBUST_CHANNEL_DISP_MISSED_NOTIFIER             (19)
#define ROBUST_CHANNEL_MPEG_ERROR_SW_METHOD             (20)
#define ROBUST_CHANNEL_ME_ERROR_SW_METHOD               (21)
#define ROBUST_CHANNEL_VP_ERROR_SW_METHOD               (22)
#define ROBUST_CHANNEL_RC_LOGGING_ENABLED               (23)
#define ROBUST_CHANNEL_VP_ERROR                         (27)
#define ROBUST_CHANNEL_VP2_ERROR                        (28)
#define ROBUST_CHANNEL_BSP_ERROR                        (29)
#define ROBUST_CHANNEL_UNUSED_ERROR_30                  (30)
#define ROBUST_CHANNEL_FIFO_ERROR_MMU_ERR_FLT           (31)
#define ROBUST_CHANNEL_PBDMA_ERROR                      (32)
#define ROBUST_CHANNEL_SEC_ERROR                        (33)
#define ROBUST_CHANNEL_MSVLD_ERROR                      (34)
#define ROBUST_CHANNEL_MSPDEC_ERROR                     (35)
#define ROBUST_CHANNEL_MSPPP_ERROR                      (36)
#define ROBUST_CHANNEL_CE0_ERROR                        (39)
#define ROBUST_CHANNEL_CE1_ERROR                        (40)
#define ROBUST_CHANNEL_CE2_ERROR                        (41)
#define ROBUST_CHANNEL_VIC_ERROR                        (42)
#define ROBUST_CHANNEL_RESETCHANNEL_VERIF_ERROR         (43)
#define ROBUST_CHANNEL_GR_FAULT_DURING_CTXSW            (44)
#define ROBUST_CHANNEL_PREEMPTIVE_REMOVAL               (45)
#define ROBUST_CHANNEL_NVENC0_ERROR                     (47)
#define ROBUST_CHANNEL_GPU_ECC_DBE                      (48)
#define FB_MEMORY_ERROR                                 (58)
#define PMU_ERROR                                       (59)
#define ROBUST_CHANNEL_SEC2_ERROR                       (60)
#define PMU_BREAKPOINT                                  (61)
#define PMU_HALT_ERROR                                  (62)
#define INFOROM_PAGE_RETIREMENT_EVENT                   (63)
#define INFOROM_DRAM_RETIREMENT_EVENT                   INFOROM_PAGE_RETIREMENT_EVENT
#define INFOROM_PAGE_RETIREMENT_FAILURE                 (64)
#define INFOROM_DRAM_RETIREMENT_FAILURE                 INFOROM_PAGE_RETIREMENT_FAILURE
#define ROBUST_CHANNEL_NVENC1_ERROR                     (65)
#define ROBUST_CHANNEL_NVDEC0_ERROR                     (68)
#define ROBUST_CHANNEL_GR_CLASS_ERROR                   (69)
#define ROBUST_CHANNEL_CE3_ERROR                        (70)
#define ROBUST_CHANNEL_CE4_ERROR                        (71)
#define ROBUST_CHANNEL_CE5_ERROR                        (72)
#define ROBUST_CHANNEL_NVENC2_ERROR                     (73)
#define NVLINK_ERROR                                    (74)
#define ROBUST_CHANNEL_CE6_ERROR                        (75)
#define ROBUST_CHANNEL_CE7_ERROR                        (76)
#define ROBUST_CHANNEL_CE8_ERROR                        (77)
#define VGPU_START_ERROR                                (78)
#define ROBUST_CHANNEL_GPU_HAS_FALLEN_OFF_THE_BUS       (79)
#define PBDMA_PUSHBUFFER_CRC_MISMATCH                   (80)
#define ROBUST_CHANNEL_VGA_SUBSYSTEM_ERROR              (81)
#define ROBUST_CHANNEL_NVJPG0_ERROR                     (82)
#define ROBUST_CHANNEL_NVDEC1_ERROR                     (83)
#define ROBUST_CHANNEL_NVDEC2_ERROR                     (84)
#define ROBUST_CHANNEL_CE9_ERROR                        (85)
#define ROBUST_CHANNEL_OFA0_ERROR                       (86)
#define NVTELEMETRY_DRIVER_REPORT                       (87)
#define ROBUST_CHANNEL_NVDEC3_ERROR                     (88)
#define ROBUST_CHANNEL_NVDEC4_ERROR                     (89)
#define LTC_ERROR                                       (90)
#define RESERVED_XID                                    (91)
#define EXCESSIVE_SBE_INTERRUPTS                        (92)
#define INFOROM_ERASE_LIMIT_EXCEEDED                    (93)
#define ROBUST_CHANNEL_CONTAINED_ERROR                  (94)
#define ROBUST_CHANNEL_UNCONTAINED_ERROR                (95)
#define ROBUST_CHANNEL_NVDEC5_ERROR                     (96)
#define ROBUST_CHANNEL_NVDEC6_ERROR                     (97)
#define ROBUST_CHANNEL_NVDEC7_ERROR                     (98)
#define ROBUST_CHANNEL_NVJPG1_ERROR                     (99)
#define ROBUST_CHANNEL_NVJPG2_ERROR                     (100)
#define ROBUST_CHANNEL_NVJPG3_ERROR                     (101)
#define ROBUST_CHANNEL_NVJPG4_ERROR                     (102)
#define ROBUST_CHANNEL_NVJPG5_ERROR                     (103)
#define ROBUST_CHANNEL_NVJPG6_ERROR                     (104)
#define ROBUST_CHANNEL_NVJPG7_ERROR                     (105)
#define DESTINATION_FLA_TRANSLATION_ERROR               (108)
#define SEC_FAULT_ERROR                                 (110)
#define GSP_RPC_TIMEOUT                                 (119)
#define GSP_ERROR                                       (120)
#define C2C_ERROR                                       (121)
#define SPI_PMU_RPC_READ_FAIL                           (122)
#define SPI_PMU_RPC_WRITE_FAIL                          (123)
#define SPI_PMU_RPC_ERASE_FAIL                          (124)
#define INFOROM_FS_ERROR                                (125)
#define ROBUST_CHANNEL_CE10_ERROR                       (126)
#define ROBUST_CHANNEL_CE11_ERROR                       (127)
#define ROBUST_CHANNEL_CE12_ERROR                       (128)
#define ROBUST_CHANNEL_CE13_ERROR                       (129)
#define ROBUST_CHANNEL_CE14_ERROR                       (130)
#define ROBUST_CHANNEL_CE15_ERROR                       (131)
#define ROBUST_CHANNEL_CE16_ERROR                       (132)
#define ROBUST_CHANNEL_CE17_ERROR                       (133)
#define ROBUST_CHANNEL_CE18_ERROR                       (134)
#define ROBUST_CHANNEL_CE19_ERROR                       (135)
#define ALI_TRAINING_FAIL                               (136)
#define NVLINK_FLA_PRIV_ERR                             (137)
#define ROBUST_CHANNEL_DLA_ERROR                        (138)
#define ROBUST_CHANNEL_OFA1_ERROR                       (139)
#define UNRECOVERABLE_ECC_ERROR_ESCAPE                  (140)
#define ROBUST_CHANNEL_FAST_PATH_ERROR                  (141)
#define GPU_INIT_ERROR                                  (143)
#define NVLINK_SAW_ERROR                                (144)
#define NVLINK_RLW_ERROR                                (145)
#define NVLINK_TLW_ERROR                                (146)
#define NVLINK_TREX_ERROR                               (147)
#define NVLINK_NVLPW_CTRL_ERROR                         (148)
#define NVLINK_NETIR_ERROR                              (149)
#define NVLINK_MSE_ERROR                                (150)
#define ROBUST_CHANNEL_KEY_ROTATION_ERROR               (151)
#define RESERVED7_ERROR                                 (152)
#define RESERVED8_ERROR                                 (153)
#define ROBUST_CHANNEL_LAST_ERROR                       (153)


// Indexed CE reference
#define ROBUST_CHANNEL_CE_ERROR(x)                                        \
    ((x < 3) ?                                                            \
        (ROBUST_CHANNEL_CE0_ERROR + (x)) :                                \
     ((x < 6) ?                                                           \
        (ROBUST_CHANNEL_CE3_ERROR + (x - 3)) :                            \
     ((x < 9) ?                                                           \
        (ROBUST_CHANNEL_CE6_ERROR + (x - 6)) :                            \
     ((x == 9) ?                                                          \
        (ROBUST_CHANNEL_CE9_ERROR) :                                      \
        (ROBUST_CHANNEL_CE10_ERROR + (x - 10))))))

#define ROBUST_CHANNEL_IS_CE_ERROR(x)                                        \
    ((x == ROBUST_CHANNEL_CE0_ERROR) || (x == ROBUST_CHANNEL_CE1_ERROR) ||   \
     (x == ROBUST_CHANNEL_CE2_ERROR) || (x == ROBUST_CHANNEL_CE3_ERROR) ||   \
     (x == ROBUST_CHANNEL_CE4_ERROR) || (x == ROBUST_CHANNEL_CE5_ERROR) ||   \
     (x == ROBUST_CHANNEL_CE6_ERROR) || (x == ROBUST_CHANNEL_CE7_ERROR) ||   \
     (x == ROBUST_CHANNEL_CE8_ERROR) || (x == ROBUST_CHANNEL_CE9_ERROR) ||   \
     (x == ROBUST_CHANNEL_CE10_ERROR) || (x == ROBUST_CHANNEL_CE11_ERROR) || \
     (x == ROBUST_CHANNEL_CE12_ERROR) || (x == ROBUST_CHANNEL_CE13_ERROR) || \
     (x == ROBUST_CHANNEL_CE14_ERROR) || (x == ROBUST_CHANNEL_CE15_ERROR) || \
     (x == ROBUST_CHANNEL_CE16_ERROR) || (x == ROBUST_CHANNEL_CE17_ERROR) || \
     (x == ROBUST_CHANNEL_CE18_ERROR) || (x == ROBUST_CHANNEL_CE19_ERROR))

#define ROBUST_CHANNEL_CE_ERROR_IDX(x)                                       \
    (((x >= ROBUST_CHANNEL_CE0_ERROR) && (x <= ROBUST_CHANNEL_CE2_ERROR)) ?  \
         (x - ROBUST_CHANNEL_CE0_ERROR) :                                    \
     (((x >= ROBUST_CHANNEL_CE3_ERROR) && (x <= ROBUST_CHANNEL_CE5_ERROR)) ? \
         (x - ROBUST_CHANNEL_CE3_ERROR + 3) :                                \
     (((x >= ROBUST_CHANNEL_CE6_ERROR) && (x <= ROBUST_CHANNEL_CE8_ERROR)) ? \
         (x - ROBUST_CHANNEL_CE6_ERROR + 6) :                                \
     ((x == ROBUST_CHANNEL_CE9_ERROR) ?                                      \
         (x - ROBUST_CHANNEL_CE9_ERROR + 9) :                                \
         (x - ROBUST_CHANNEL_CE10_ERROR + 10)))))

// Indexed NVDEC reference
#define ROBUST_CHANNEL_NVDEC_ERROR(x)                   \
    ((x < 1) ?                                          \
        (ROBUST_CHANNEL_NVDEC0_ERROR) :                 \
      ((x < 3) ?                                        \
        (ROBUST_CHANNEL_NVDEC1_ERROR + (x - 1)) :       \
      ((x < 5) ?                                        \
        (ROBUST_CHANNEL_NVDEC3_ERROR + (x - 3)):        \
        (ROBUST_CHANNEL_NVDEC5_ERROR + (x - 5)))))

#define ROBUST_CHANNEL_IS_NVDEC_ERROR(x)   \
    ((x == ROBUST_CHANNEL_NVDEC0_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC1_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC2_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC3_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC4_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC5_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC6_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC7_ERROR))

#define ROBUST_CHANNEL_NVDEC_ERROR_IDX(x)                                           \
    ((x == ROBUST_CHANNEL_NVDEC0_ERROR) ?                                           \
         (x - ROBUST_CHANNEL_NVDEC0_ERROR) :                                        \
     (((x >= ROBUST_CHANNEL_NVDEC1_ERROR) && (x <= ROBUST_CHANNEL_NVDEC2_ERROR)) ?  \
         (x - ROBUST_CHANNEL_NVDEC1_ERROR + 1) :                                    \
     (((x >= ROBUST_CHANNEL_NVDEC3_ERROR) && (x <= ROBUST_CHANNEL_NVDEC4_ERROR)) ?  \
         (x - ROBUST_CHANNEL_NVDEC3_ERROR + 3) :                                    \
         (x - ROBUST_CHANNEL_NVDEC5_ERROR + 5))))

// Indexed NVENC reference
#define ROBUST_CHANNEL_NVENC_ERROR(x)                       \
    ((x == 0) ? (ROBUST_CHANNEL_NVENC0_ERROR) :             \
                ((x == 1) ? (ROBUST_CHANNEL_NVENC1_ERROR) : \
                            (ROBUST_CHANNEL_NVENC2_ERROR)))

#define ROBUST_CHANNEL_IS_NVENC_ERROR(x)   \
    ((x == ROBUST_CHANNEL_NVENC0_ERROR) || \
     (x == ROBUST_CHANNEL_NVENC1_ERROR) || \
     (x == ROBUST_CHANNEL_NVENC2_ERROR))

#define ROBUST_CHANNEL_NVENC_ERROR_IDX(x)             \
    (((x == ROBUST_CHANNEL_NVENC0_ERROR)) ?           \
         (x - ROBUST_CHANNEL_NVENC0_ERROR) :          \
         (((x == ROBUST_CHANNEL_NVENC1_ERROR)) ?      \
              (x - ROBUST_CHANNEL_NVENC1_ERROR + 1) : \
              (x - ROBUST_CHANNEL_NVENC2_ERROR + 2)))

// Indexed NVJPG reference
#define ROBUST_CHANNEL_NVJPG_ERROR(x)                   \
    ((x < 1) ?                                          \
        (ROBUST_CHANNEL_NVJPG0_ERROR) :                 \
        (ROBUST_CHANNEL_NVJPG1_ERROR + (x - 1)))

#define ROBUST_CHANNEL_IS_NVJPG_ERROR(x)                \
    ((x == ROBUST_CHANNEL_NVJPG0_ERROR) ||              \
     (x == ROBUST_CHANNEL_NVJPG1_ERROR) ||              \
     (x == ROBUST_CHANNEL_NVJPG2_ERROR) ||              \
     (x == ROBUST_CHANNEL_NVJPG3_ERROR) ||              \
     (x == ROBUST_CHANNEL_NVJPG4_ERROR) ||              \
     (x == ROBUST_CHANNEL_NVJPG5_ERROR) ||              \
     (x == ROBUST_CHANNEL_NVJPG6_ERROR) ||              \
     (x == ROBUST_CHANNEL_NVJPG7_ERROR))

#define ROBUST_CHANNEL_NVJPG_ERROR_IDX(x)               \
    ((x == ROBUST_CHANNEL_NVJPG0_ERROR) ?               \
         (x - ROBUST_CHANNEL_NVJPG0_ERROR) :            \
         (x - ROBUST_CHANNEL_NVJPG1_ERROR + 1))

// Indexed OFA reference
#define ROBUST_CHANNEL_OFA_ERROR(x)                    \
    ((x == 0) ?                                        \
        (ROBUST_CHANNEL_OFA0_ERROR) :                  \
        (ROBUST_CHANNEL_OFA1_ERROR))

#define ROBUST_CHANNEL_IS_OFA_ERROR(x)                 \
    ((x == ROBUST_CHANNEL_OFA0_ERROR) ||               \
     (x == ROBUST_CHANNEL_OFA1_ERROR))

#define ROBUST_CHANNEL_OFA_ERROR_IDX(x)                 \
    ((x == ROBUST_CHANNEL_OFA0_ERROR) ?                 \
        (x - ROBUST_CHANNEL_OFA0_ERROR) :               \
        (x - ROBUST_CHANNEL_OFA1_ERROR + 1))

// Error Levels
#define ROBUST_CHANNEL_ERROR_RECOVERY_LEVEL_INFO      (0)
#define ROBUST_CHANNEL_ERROR_RECOVERY_LEVEL_NON_FATAL (1)
#define ROBUST_CHANNEL_ERROR_RECOVERY_LEVEL_FATAL     (2)

#endif  // NVERROR_H
