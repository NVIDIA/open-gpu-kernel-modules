/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __ls10_dev_falcon_v4_h__
#define __ls10_dev_falcon_v4_h__
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK                                                          0x0000028c     /* RW-4R */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION                                          3:0            /* RWIVF */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_DEFAULT_PRIV_LEVEL                       0x0000000f     /* RWI-V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_ALL_LEVELS_ENABLED                       0x0000000f     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_ALL_LEVELS_DISABLED                      0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_ONLY_LEVEL3_ENABLED                      0x00000008     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0                                   0:0            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_ENABLE                            0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_DISABLE                           0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL1                                   1:1            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL1_ENABLE                            0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL1_DISABLE                           0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL2                                   2:2            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL2_ENABLE                            0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL2_DISABLE                           0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL3                                   3:3            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL3_ENABLE                            0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL3_DISABLE                           0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION                                         7:4            /* RWIVF */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_ALL_LEVELS_ENABLED                      0x0000000f     /* RWI-V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_ALL_LEVELS_DISABLED                     0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_ONLY_LEVEL3_ENABLED                     0x00000008     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0                                  4:4            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_ENABLE                           0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_DISABLE                          0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL1                                  5:5            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL1_ENABLE                           0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL1_DISABLE                          0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL2                                  6:6            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL2_ENABLE                           0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL2_DISABLE                          0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL3                                  7:7            /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL3_ENABLE                           0x00000001     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL3_DISABLE                          0x00000000     /*       */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_VIOLATION                                           8:8            /* RWIVF */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_VIOLATION_REPORT_ERROR                              0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_READ_VIOLATION_SOLDIER_ON                                0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_VIOLATION                                          9:9            /* RWIVF */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_VIOLATION_REPORT_ERROR                             0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_WRITE_VIOLATION_SOLDIER_ON                               0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_READ_CONTROL                                      10:10          /* RWIVF */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_READ_CONTROL_BLOCKED                              0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_READ_CONTROL_LOWERED                              0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_WRITE_CONTROL                                     11:11          /* RWIVF */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_WRITE_CONTROL_BLOCKED                             0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_WRITE_CONTROL_LOWERED                             0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_ENABLE                                            31:12          /* RWIVF */
#define NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK_SOURCE_ENABLE_ALL_SOURCES_ENABLED                        0x000fffff     /* RWI-V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK                                                        0x000003c4     /* RW-4R */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION                                        3:0            /* RWIVF */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_DEFAULT_PRIV_LEVEL                     0x0000000f     /* RWI-V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_ALL_LEVELS_ENABLED                     0x0000000f     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_ALL_LEVELS_DISABLED                    0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_ONLY_LEVEL3_ENABLED                    0x00000008     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0                                 0:0            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_ENABLE                          0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_DISABLE                         0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL1                                 1:1            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL1_ENABLE                          0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL1_DISABLE                         0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL2                                 2:2            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL2_ENABLE                          0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL2_DISABLE                         0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL3                                 3:3            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL3_ENABLE                          0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL3_DISABLE                         0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION                                       7:4            /* RWIVF */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_ALL_LEVELS_ENABLED                    0x0000000f     /* RWI-V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_ALL_LEVELS_DISABLED                   0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_ONLY_LEVEL3_ENABLED                   0x00000008     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0                                4:4            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_ENABLE                         0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_DISABLE                        0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL1                                5:5            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL1_ENABLE                         0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL1_DISABLE                        0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL2                                6:6            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL2_ENABLE                         0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL2_DISABLE                        0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL3                                7:7            /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL3_ENABLE                         0x00000001     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL3_DISABLE                        0x00000000     /*       */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_VIOLATION                                         8:8            /* RWIVF */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_VIOLATION_REPORT_ERROR                            0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_READ_VIOLATION_SOLDIER_ON                              0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_VIOLATION                                        9:9            /* RWIVF */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_VIOLATION_REPORT_ERROR                           0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_VIOLATION_SOLDIER_ON                             0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_READ_CONTROL                                    10:10          /* RWIVF */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_READ_CONTROL_BLOCKED                            0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_READ_CONTROL_LOWERED                            0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_WRITE_CONTROL                                   11:11          /* RWIVF */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_WRITE_CONTROL_BLOCKED                           0x00000001     /* RWI-V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_WRITE_CONTROL_LOWERED                           0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_ENABLE                                          31:12          /* RWIVF */
#define NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK_SOURCE_ENABLE_ALL_SOURCES_ENABLED                      0x000fffff     /* RWI-V */
#define NV_PFALCON_FALCON_IRQSSET                                                                      0x00000000     /* -W-4R */
#define NV_PFALCON_FALCON_IRQSSET_GPTMR                                                                0:0            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_GPTMR_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_WDTMR                                                                1:1            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_WDTMR_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_MTHD                                                                 2:2            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_MTHD_SET                                                             0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_CTXSW                                                                3:3            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_CTXSW_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_HALT                                                                 4:4            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_HALT_SET                                                             0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_EXTERR                                                               5:5            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_EXTERR_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_SWGEN0                                                               6:6            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_SWGEN0_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_SWGEN1                                                               7:7            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_SWGEN1_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_EXT                                                                  15:8           /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_DMA                                                                  16:16          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_DMA_SET                                                              0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_SHA                                                                  17:17          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_SHA_SET                                                              0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_MEMERR                                                               18:18          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_MEMERR_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_CTXSW_ERROR                                                          19:19          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_CTXSW_ERROR_SET                                                      0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_ICD                                                                  22:22          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_ICD_SET                                                              0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_IOPMP                                                                23:23          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_IOPMP_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSSET_CORE_MISMATCH                                                        24:24          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSSET_CORE_MISMATCH_SET                                                    0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR                                                                      0x00000004     /* -W-4R */
#define NV_PFALCON_FALCON_IRQSCLR_GPTMR                                                                0:0            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_GPTMR_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_WDTMR                                                                1:1            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_WDTMR_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_MTHD                                                                 2:2            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_MTHD_SET                                                             0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_CTXSW                                                                3:3            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_CTXSW_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_HALT                                                                 4:4            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_HALT_SET                                                             0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_EXTERR                                                               5:5            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_EXTERR_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_SWGEN0                                                               6:6            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_SWGEN0_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_SWGEN1                                                               7:7            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_SWGEN1_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_EXT                                                                  15:8           /*       */
#define NV_PFALCON_FALCON_IRQSCLR_DMA                                                                  16:16          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_DMA_SET                                                              0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_SHA                                                                  17:17          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_SHA_SET                                                              0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_MEMERR                                                               18:18          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_MEMERR_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_CTXSW_ERROR                                                          19:19          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_CTXSW_ERROR_SET                                                      0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_ICD                                                                  22:22          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_ICD_SET                                                              0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_IOPMP                                                                23:23          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_IOPMP_SET                                                            0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_CORE_MISMATCH                                                        24:24          /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_CORE_MISMATCH_SET                                                    0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSTAT                                                                      0x00000008     /* R--4R */
#define NV_PFALCON_FALCON_IRQSTAT_GPTMR                                                                0:0            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_GPTMR_FALSE                                                          0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_GPTMR_TRUE                                                           0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_WDTMR                                                                1:1            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_WDTMR_FALSE                                                          0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_WDTMR_TRUE                                                           0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_MTHD                                                                 2:2            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_MTHD_FALSE                                                           0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_MTHD_TRUE                                                            0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_CTXSW                                                                3:3            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_CTXSW_FALSE                                                          0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_CTXSW_TRUE                                                           0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_HALT                                                                 4:4            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_HALT_FALSE                                                           0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_HALT_TRUE                                                            0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_EXTERR                                                               5:5            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_EXTERR_FALSE                                                         0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_EXTERR_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN0                                                               6:6            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN0_FALSE                                                         0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN0_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN1                                                               7:7            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN1_FALSE                                                         0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN1_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_EXT                                                                  15:8           /*       */
#define NV_PFALCON_FALCON_IRQSTAT_DMA                                                                  16:16          /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_DMA_TRUE                                                             0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_DMA_FALSE                                                            0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_SHA                                                                  17:17          /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_SHA_TRUE                                                             0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_SHA_FALSE                                                            0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_MEMERR                                                               18:18          /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_MEMERR_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_MEMERR_FALSE                                                         0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_IRQSTAT_CTXSW_ERROR                                                          19:19          /* R-XVF */
#define NV_PFALCON_FALCON_IRQSTAT_CTXSW_ERROR_TRUE                                                     0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_CTXSW_ERROR_FALSE                                                    0x00000000     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_ICD                                                                  22:22          /* R-XVF */
#define NV_PFALCON_FALCON_IRQSTAT_ICD_TRUE                                                             0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_ICD_FALSE                                                            0x00000000     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_IOPMP                                                                23:23          /* R-XVF */
#define NV_PFALCON_FALCON_IRQSTAT_IOPMP_TRUE                                                           0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_IOPMP_FALSE                                                          0x00000000     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_CORE_MISMATCH                                                        24:24          /* R-XVF */
#define NV_PFALCON_FALCON_IRQSTAT_CORE_MISMATCH_TRUE                                                   0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_CORE_MISMATCH_FALSE                                                  0x00000000     /* R---V */
#define NV_PFALCON_FALCON_IRQMODE                                                                      0x0000000c     /* RW-4R */
#define NV_PFALCON_FALCON_IRQMASK                                                                      0x00000018     /* R--4R */
#define NV_PFALCON_FALCON_MAILBOX0                                                                     0x00000040     /* RW-4R */
#define NV_PFALCON_FALCON_MAILBOX0_DATA                                                                31:0           /* RWIVF */
#define NV_PFALCON_FALCON_MAILBOX0_DATA_INIT                                                           0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_MAILBOX1                                                                     0x00000044     /* RW-4R */
#define NV_PFALCON_FALCON_MAILBOX1_DATA                                                                31:0           /* RWIVF */
#define NV_PFALCON_FALCON_MAILBOX1_DATA_INIT                                                           0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_IDLESTATE                                                                    0x0000004c     /* RW-4R */
#define NV_PFALCON_FALCON_DMACTL                                                                       0x0000010c     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFBASE                                                                   0x00000110     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFBASE_BASE                                                              31:0           /* RWIVF */
#define NV_PFALCON_FALCON_DMATRFBASE_BASE_INIT                                                         0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMATRFMOFFS                                                                  0x00000114     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFMOFFS_OFFS                                                             23:0           /* RWIVF */
#define NV_PFALCON_FALCON_DMATRFMOFFS_OFFS_INIT                                                        0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMATRFCMD                                                                    0x00000118     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFFBOFFS                                                                 0x0000011c     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFFBOFFS_OFFS                                                            31:0           /* RWIVF */
#define NV_PFALCON_FALCON_DMATRFFBOFFS_OFFS_INIT                                                       0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMEMC(i)                                                                     (0x000001c0+(i)*8) /* RW-4A */
#define NV_PFALCON_FALCON_DMEMC__SIZE_1                                                                4              /*       */
#define NV_PFALCON_FALCON_DMEMC__DEVICE_MAP                                                            0x00000006 /*       */
#define NV_PFALCON_FALCON_DMEMC__PRIV_LEVEL_MASK                                                       NV_PFALCON_FALCON_PMB_DMEM_PRIV_LEVEL_MASK /*       */
#define NV_PFALCON_FALCON_DMEMC_ADDRESS                                                                23:0           /* RWIVF */
#define NV_PFALCON_FALCON_DMEMC_ADDRESS_INIT                                                           0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMEMC_OFFS                                                                   7:2            /*       */
#define NV_PFALCON_FALCON_DMEMC_OFFS_INIT                                                              0              /*       */
#define NV_PFALCON_FALCON_DMEMC_BLK                                                                    23:8           /*       */
#define NV_PFALCON_FALCON_DMEMC_BLK_INIT                                                               0              /*       */
#define NV_PFALCON_FALCON_DMEMC_AINCW                                                                  24:24          /* RWIVF */
#define NV_PFALCON_FALCON_DMEMC_AINCW_INIT                                                             0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMEMC_AINCW_TRUE                                                             0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_AINCW_FALSE                                                            0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_AINCR                                                                  25:25          /* RWIVF */
#define NV_PFALCON_FALCON_DMEMC_AINCR_INIT                                                             0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMEMC_AINCR_TRUE                                                             0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_AINCR_FALSE                                                            0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_SETTAG                                                                 26:26          /* RWIVF */
#define NV_PFALCON_FALCON_DMEMC_SETTAG_INIT                                                            0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMEMC_SETTAG_TRUE                                                            0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_SETTAG_FALSE                                                           0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_SETLVL                                                                 27:27          /* RWIVF */
#define NV_PFALCON_FALCON_DMEMC_SETLVL_INIT                                                            0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMEMC_SETLVL_TRUE                                                            0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_SETLVL_FALSE                                                           0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_VA                                                                     28:28          /* RWIVF */
#define NV_PFALCON_FALCON_DMEMC_VA_INIT                                                                0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_DMEMC_VA_TRUE                                                                0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_VA_FALSE                                                               0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_MISS                                                                   29:29          /* R-IVF */
#define NV_PFALCON_FALCON_DMEMC_MISS_TRUE                                                              0x00000001     /* R---V */
#define NV_PFALCON_FALCON_DMEMC_MISS_FALSE                                                             0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_DMEMC_MULTIHIT                                                               30:30          /* R-IVF */
#define NV_PFALCON_FALCON_DMEMC_MULTIHIT_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_DMEMC_MULTIHIT_FALSE                                                         0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_DMEMC_LVLERR                                                                 31:31          /* R-IVF */
#define NV_PFALCON_FALCON_DMEMC_LVLERR_TRUE                                                            0x00000001     /* R---V */
#define NV_PFALCON_FALCON_DMEMC_LVLERR_FALSE                                                           0x00000000     /* R-I-V */
#define NV_PFALCON_FALCON_DMEMD(i)                                                                     (0x000001c4+(i)*8) /* RW-4A */
#define NV_PFALCON_FALCON_DMEMD__SIZE_1                                                                4              /*       */
#define NV_PFALCON_FALCON_DMEMD__DEVICE_MAP                                                            0x00000006 /*       */
#define NV_PFALCON_FALCON_DMEMD__PRIV_LEVEL_MASK                                                       NV_PFALCON_FALCON_PMB_DMEM_PRIV_LEVEL_MASK /*       */
#define NV_PFALCON_FALCON_DMEMD_DATA                                                                   31:0           /* RWXVF */
#define NV_PFALCON_FALCON_HWCFG3                                                                       0x00000278     /* R--4R */
#define NV_PFALCON_FALCON_HWCFG3_IMEM_TOTAL_SIZE                                                       11:0           /* R--VF */
#define NV_PFALCON_FALCON_HWCFG3_DMEM_TOTAL_SIZE                                                       27:16          /* R--VF */
#define NV_PFALCON_FALCON_OS                                                                           0x00000080     /* RW-4R */
#define NV_PFALCON_FALCON_OS_VERSION                                                                   31:0           /* RWIVF */
#define NV_PFALCON_FALCON_OS_VERSION_INIT                                                              0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_CPUCTL                                                                       0x00000100     /* RW-4R */
#define NV_PFALCON_FALCON_CPUCTL_IINVAL                                                                0:0            /* -WXVF */
#define NV_PFALCON_FALCON_CPUCTL_IINVAL_TRUE                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_IINVAL_FALSE                                                          0x00000000     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_STARTCPU                                                              1:1            /* -WXVF */
#define NV_PFALCON_FALCON_CPUCTL_STARTCPU_TRUE                                                         0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_STARTCPU_FALSE                                                        0x00000000     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_SRESET                                                                2:2            /* -WXVF */
#define NV_PFALCON_FALCON_CPUCTL_SRESET_TRUE                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_SRESET_FALSE                                                          0x00000000     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_HRESET                                                                3:3            /* -WXVF */
#define NV_PFALCON_FALCON_CPUCTL_HRESET_TRUE                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_HRESET_FALSE                                                          0x00000000     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_HALTED                                                                4:4            /* R-XVF */
#define NV_PFALCON_FALCON_CPUCTL_HALTED_TRUE                                                           0x00000001     /* R---V */
#define NV_PFALCON_FALCON_CPUCTL_HALTED_FALSE                                                          0x00000000     /* R---V */
#define NV_PFALCON_FALCON_CPUCTL_STOPPED                                                               5:5            /* R-XVF */
#define NV_PFALCON_FALCON_CPUCTL_STOPPED_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_CPUCTL_STOPPED_FALSE                                                         0x00000000     /* R---V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_EN                                                              6:6            /* RWIVF */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_EN_TRUE                                                         0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_EN_FALSE                                                        0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_EN_INIT                                                         0x00000000     /* RWI-V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS                                                                 0x00000130     /* -W-4R */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_STARTCPU                                                        1:1            /* -WXVF */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_STARTCPU_TRUE                                                   0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_STARTCPU_FALSE                                                  0x00000000     /* -W--V */
#endif // __ls10_dev_falcon_v4_h__
