/*
 * Copyright (c) 1993-2022, NVIDIA CORPORATION. All rights reserved.
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

#define ROBUST_CHANNEL_GR_EXCEPTION                     (13)
#define ROBUST_CHANNEL_GR_ERROR_SW_NOTIFY               (13)
#define ROBUST_CHANNEL_FAKE_ERROR                       (14)
#define ROBUST_CHANNEL_DISP_MISSED_NOTIFIER             (19)
#define ROBUST_CHANNEL_MPEG_ERROR_SW_METHOD             (20)
#define ROBUST_CHANNEL_ME_ERROR_SW_METHOD               (21)
#define ROBUST_CHANNEL_VP_ERROR_SW_METHOD               (22)
#define ROBUST_CHANNEL_RC_LOGGING_ENABLED               (23)
#define ROBUST_CHANNEL_VP_ERROR                         (27)
#define ROBUST_CHANNEL_VP2_ERROR                        (28)
#define ROBUST_CHANNEL_BSP_ERROR                        (29)
#define ROBUST_CHANNEL_BAD_ADDR_ACCESS                  (30)
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
#define PMU_ERROR                                       (59)
#define ROBUST_CHANNEL_SEC2_ERROR                       (60)
#define PMU_BREAKPOINT                                  (61)
#define PMU_HALT_ERROR                                  (62)
#define INFOROM_PAGE_RETIREMENT_EVENT                   (63)
#define INFOROM_PAGE_RETIREMENT_FAILURE                 (64)
#define INFOROM_DRAM_RETIREMENT_EVENT                   (63)
#define INFOROM_DRAM_RETIREMENT_FAILURE                 (64)
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
#define SEC_FAULT_ERROR                                 (110)
#define GSP_RPC_TIMEOUT                                 (119)
#define GSP_ERROR                                       (120)
#define C2C_ERROR                                       (121)
#define SPI_PMU_RPC_READ_FAIL                           (122)
#define SPI_PMU_RPC_WRITE_FAIL                          (123)
#define SPI_PMU_RPC_ERASE_FAIL                          (124)
#define INFOROM_FS_ERROR                                (125)
#define ROBUST_CHANNEL_LAST_ERROR                       (INFOROM_FS_ERROR)


// Indexed CE reference
#define ROBUST_CHANNEL_CE_ERROR(x)                                        \
    (x < 3 ? ROBUST_CHANNEL_CE0_ERROR + (x) :                             \
             ((x < 6) ? (ROBUST_CHANNEL_CE3_ERROR + (x - 3)) :            \
                        ((x < 9) ? (ROBUST_CHANNEL_CE6_ERROR + (x - 6)) : \
                                   ROBUST_CHANNEL_CE9_ERROR)))

#define ROBUST_CHANNEL_IS_CE_ERROR(x)                                      \
    ((x == ROBUST_CHANNEL_CE0_ERROR) || (x == ROBUST_CHANNEL_CE1_ERROR) || \
     (x == ROBUST_CHANNEL_CE2_ERROR) || (x == ROBUST_CHANNEL_CE3_ERROR) || \
     (x == ROBUST_CHANNEL_CE4_ERROR) || (x == ROBUST_CHANNEL_CE5_ERROR) || \
     (x == ROBUST_CHANNEL_CE6_ERROR) || (x == ROBUST_CHANNEL_CE7_ERROR) || \
     (x == ROBUST_CHANNEL_CE8_ERROR) || (x == ROBUST_CHANNEL_CE9_ERROR))

#define ROBUST_CHANNEL_CE_ERROR_IDX(x)                                      \
    (((x >= ROBUST_CHANNEL_CE0_ERROR) && (x <= ROBUST_CHANNEL_CE2_ERROR)) ? \
         (x - ROBUST_CHANNEL_CE0_ERROR) :                                   \
         (((x >= ROBUST_CHANNEL_CE3_ERROR) &&                               \
           (x <= ROBUST_CHANNEL_CE5_ERROR)) ?                               \
              (x - ROBUST_CHANNEL_CE3_ERROR) :                              \
              (((x >= ROBUST_CHANNEL_CE6_ERROR) &&                          \
                (x <= ROBUST_CHANNEL_CE8_ERROR)) ?                          \
                   (x - ROBUST_CHANNEL_CE6_ERROR) :                         \
                   (x - ROBUST_CHANNEL_CE9_ERROR))))

// Indexed NVDEC reference
#define ROBUST_CHANNEL_NVDEC_ERROR(x)                                      \
    ((x == 0) ?                                                            \
         (ROBUST_CHANNEL_NVDEC0_ERROR) :                                   \
         (((x >= 1) && (x <= 2)) ? (ROBUST_CHANNEL_NVDEC1_ERROR + x - 1) : \
                                   (ROBUST_CHANNEL_NVDEC3_ERROR + x - 3)))

#define ROBUST_CHANNEL_IS_NVDEC_ERROR(x)   \
    ((x == ROBUST_CHANNEL_NVDEC0_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC1_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC2_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC3_ERROR) || \
     (x == ROBUST_CHANNEL_NVDEC4_ERROR))

#define ROBUST_CHANNEL_NVDEC_ERROR_IDX(x)             \
    (((x == ROBUST_CHANNEL_NVDEC0_ERROR)) ?           \
         (x - ROBUST_CHANNEL_NVDEC0_ERROR) :          \
         (((x >= ROBUST_CHANNEL_NVDEC1_ERROR) &&      \
           (x <= ROBUST_CHANNEL_NVDEC2_ERROR)) ?      \
              (x - ROBUST_CHANNEL_NVDEC1_ERROR + 1) : \
              (x - ROBUST_CHANNEL_NVDEC3_ERROR + 3)))

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

// Error Levels
#define ROBUST_CHANNEL_ERROR_RECOVERY_LEVEL_INFO      (0)
#define ROBUST_CHANNEL_ERROR_RECOVERY_LEVEL_NON_FATAL (1)
#define ROBUST_CHANNEL_ERROR_RECOVERY_LEVEL_FATAL     (2)

#define ROBUST_CHANNEL_ERROR_STR_PUBLIC_PUBLISHED  \
       {"Unknown Error",                         \
        "DMA Engine Error (FIFO Error 1)",       \
        "DMA Engine Error (FIFO Error 2)",       \
        "DMA Engine Error (FIFO Error 3)",       \
        "DMA Engine Error (FIFO Error 4)",       \
        "DMA Engine Error (FIFO Error 5)",       \
        "DMA Engine Error (FIFO Error 6)",       \
        "DMA Engine Error (FIFO Error 7)",       \
        "DMA Engine Error (FIFO Error 8)",       \
        "Graphics Engine Error (GR Error 1)",    \
        "Graphics Engine Error (GR Error 2)",    \
        "Graphics Engine Error (GR Error 3)",    \
        "Graphics Engine Error (GR Error 4)",    \
        "Graphics Engine Error (GR Exception Error)",\
        "Fake Error",                            \
        "Display Engine Error (CRTC Error 1)",   \
        "Display Engine Error (CRTC Error 2)",   \
        "Display Engine Error (CRTC Error 3)",   \
        "Bus Interface Error (BIF Error)",       \
        "Client Reported Error",                 \
        "Video Engine Error (MPEG Error)",       \
        "Video Engine Error (ME Error)",         \
        "Video Engine Error (VP Error 1)",       \
        "Error Reporting Enabled",               \
        "Graphics Engine Error (GR Error 6)",    \
        "Graphics Engine Error (GR Error 7)",    \
        "DMA Engine Error (FIFO Error 9)",       \
        "Video Engine Error (VP Error 2)",       \
        "Video Engine Error (VP2 Error)",        \
        "Video Engine Error (BSP Error)",        \
        "Access Violation Error (MMU Error 1)",  \
        "Access Violation Error (MMU Error 2)",  \
        "DMA Engine Error (PBDMA Error)",        \
        "Security Engine Error (SEC Error)",     \
        "Video Engine Error (MSVLD Error)",      \
        "Video Engine Error (MSPDEC Error)",     \
        "Video Engine Error (MSPPP Error)",      \
        "Graphics Engine Error (FECS Error 1)",  \
        "Graphics Engine Error (FECS Error 2)",  \
        "DMA Engine Error (CE Error 1)",         \
        "DMA Engine Error (CE Error 2)",         \
        "DMA Engine Error (CE Error 3)",         \
        "Video Engine Error (VIC Error)",        \
        "Verification Error",                    \
        "Access Violation Error (MMU Error 3)",  \
        "Operating System Error (OS Error 1)",   \
        "Operating System Error (OS Error 2)",   \
        "Video Engine Error (MSENC/NVENC0 Error)",\
        "ECC Error (DBE Error)",                 \
        "Power State Locked",                    \
        "Power State Event (RC Error)",          \
        "Power State Event (Stress Test Error)", \
        "Power State Event (Thermal Event 1)",   \
        "Power State Event (Thermal Event 2)",   \
        "Power State Event (Power Event)",       \
        "Power State Event (Thermal Event 3)",   \
        "Display Engine Error (EVO Error)",      \
        "FB Interface Error (FBPA Error 1)",     \
        "FB Interface Error (FBPA Error 2)",     \
        "PMU error",                             \
        "SEC2 error",                            \
        "PMU Breakpoint (non-fatal)",            \
        "PMU Halt Error",                        \
        "INFOROM Page Retirement Event",         \
        "INFOROM Page Retirement Failure",       \
        "Video Engine Error (NVENC1 Error)",     \
        "Graphics Engine Error (FECS Error 3)",  \
        "Graphics Engine Error (FECS Error 4)",  \
        "Video Engine Error (NVDEC0 Error)",     \
        "Graphics Engine Error (GR Class Error)",\
        "DMA Engine Error (CE Error 4)",         \
        "DMA Engine Error (CE Error 5)",         \
        "DMA Engine Error (CE Error 6)",         \
        "Video Engine Error (NVENC2 Error)",     \
        "NVLink Error",                          \
        "DMA Engine Error (CE Error 6)",         \
        "DMA Engine Error (CE Error 7)",         \
        "DMA Engine Error (CE Error 8)",         \
        "vGPU device cannot be started",         \
        "GPU has fallen off the bus",            \
        "DMA Engine Error (Pushbuffer CRC mismatch)",\
        "VGA Subsystem Error",                   \
        "Video JPEG Engine Error (NVJPG Error)", \
        "Video Engine Error (NVDEC1 Error)",     \
        "Video Engine Error (NVDEC2 Error)",     \
        "DMA Engine Error (CE Error 9)",         \
        "Video OFA Engine Error (OFA0 Error)",   \
        "NvTelemetry Driver Reoprt",             \
        "Video Engine Error (NVDEC3 Error)",     \
        "Video Engine Error (NVDEC4 Error)",     \
        "FB Interface Error (FBPA Error 3)",     \
        "Reserved Xid",                          \
        "Excessive SBE interrupts",              \
        "INFOROM Erase Limit Exceeded",          \
        "Contained error",                       \
        "Uncontained error"

#define ROBUST_CHANNEL_ERROR_STR_PUBLIC             \
        ROBUST_CHANNEL_ERROR_STR_PUBLIC_PUBLISHED}

#endif  // NVERROR_H
