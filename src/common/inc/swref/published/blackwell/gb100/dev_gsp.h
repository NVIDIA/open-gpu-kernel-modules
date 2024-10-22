/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_gsp_h__
#define __gb100_dev_gsp_h__

#define NV_PGSP_FALCON_MAILBOX0                                                                          0x110040       /* RW-4R */
#define NV_PGSP_FALCON_MAILBOX0_DATA                                                                     31:0           /* RWIVF */
#define NV_PGSP_FALCON_MAILBOX0_DATA_INIT                                                                0x00000000     /* RWI-V */
#define NV_PGSP_FALCON_MAILBOX1                                                                          0x110044       /* RW-4R */
#define NV_PGSP_FALCON_MAILBOX1_DATA                                                                     31:0           /* RWIVF */
#define NV_PGSP_FALCON_MAILBOX1_DATA_INIT                                                                0x00000000     /* RWI-V */
#define NV_PGSP_FALCON_ENGINE                                                                            0x1103c0       /* RW-4R */
#define NV_PGSP_FALCON_ENGINE_RESET                                                                      0:0            /* RWEVF */
#define NV_PGSP_FALCON_ENGINE_RESET_DEASSERT                                                             0              /*       */
#define NV_PGSP_FALCON_ENGINE_RESET_ASSERT                                                               1              /*       */
#define NV_PGSP_FALCON_ENGINE_RESET_STATUS                                                               10:8           /* R-EVF */
#define NV_PGSP_FALCON_ENGINE_RESET_STATUS_ASSERTED                                                      0x00000000     /* R-E-V */
#define NV_PGSP_FALCON_ENGINE_RESET_STATUS_DEASSERTED                                                    0x00000002     /* R---V */
#define NV_PGSP_MAILBOX(i)                                                                               (0x110804+(i)*4) /* RW-4A */
#define NV_PGSP_EMEMC(i)                                                                                 (0x110ac0+(i)*8) /* RW-4A */
#define NV_PGSP_EMEMC__SIZE_1                                                                            8              /*       */
#define NV_PGSP_EMEMC_OFFS                                                                               7:2            /* RWIVF */
#define NV_PGSP_EMEMC_OFFS_INIT                                                                          0x00000000     /* RWI-V */
#define NV_PGSP_EMEMC_BLK                                                                                15:8           /* RWIVF */
#define NV_PGSP_EMEMC_BLK_INIT                                                                           0x00000000     /* RWI-V */
#define NV_PGSP_EMEMC_AINCW                                                                              24:24          /* RWIVF */
#define NV_PGSP_EMEMC_AINCW_INIT                                                                         0x00000000     /* RWI-V */
#define NV_PGSP_EMEMC_AINCW_TRUE                                                                         0x00000001     /* RW--V */
#define NV_PGSP_EMEMC_AINCW_FALSE                                                                        0x00000000     /* RW--V */
#define NV_PGSP_EMEMC_AINCR                                                                              25:25          /* RWIVF */
#define NV_PGSP_EMEMC_AINCR_INIT                                                                         0x00000000     /* RWI-V */
#define NV_PGSP_EMEMC_AINCR_TRUE                                                                         0x00000001     /* RW--V */
#define NV_PGSP_EMEMC_AINCR_FALSE                                                                        0x00000000     /* RW--V */
#define NV_PGSP_EMEMD(i)                                                                                 (0x110ac4+(i)*8) /* RW-4A */
#define NV_PGSP_EMEMD__SIZE_1                                                                            8              /*       */
#define NV_PGSP_EMEMD_DATA                                                                               31:0           /* RWXVF */

#define NV_PGSP_FALCON_RESET_PRIV_LEVEL_MASK                                                             0x1103c4       /* RW-4R */
#define NV_PGSP_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0                                      0:0            /*       */
#define NV_PGSP_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_ENABLE                               0x00000001     /*       */
#define NV_PGSP_FALCON_RESET_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_DISABLE                              0x00000000     /*       */
#define NV_PGSP_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0                                     4:4            /*       */
#define NV_PGSP_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_ENABLE                              0x00000001     /*       */
#define NV_PGSP_FALCON_RESET_PRIV_LEVEL_MASK_WRITE_PROTECTION_LEVEL0_DISABLE                             0x00000000     /*       */

#define NV_PGSP_FALCON_IRQSTAT                                                                           0x110008       /* R--4R */
#define NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR                                                               24:24          /*       */
#define NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR_TRUE                                                          1              /*       */
#define NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR_FALSE                                                         0              /*       */

#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT                                                          0x111700       /* R--4R */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_GLOBAL_MEM                                               0:0            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_GLOBAL_MEM_FAULTED                                       0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_GLOBAL_MEM_NO_FAULT                                      0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ROM                                                      1:1            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ROM_FAULTED                                              0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ROM_NO_FAULT                                             0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ITCM                                                     2:2            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ITCM_FAULTED                                             0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ITCM_NO_FAULT                                            0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_DTCM                                                     3:3            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_DTCM_FAULTED                                             0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_DTCM_NO_FAULT                                            0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ICACHE                                                   4:4            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ICACHE_FAULTED                                           0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_ICACHE_NO_FAULT                                          0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_DCACHE                                                   5:5            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_DCACHE_FAULTED                                           0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_DCACHE_NO_FAULT                                          0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_RVCORE                                                   6:6            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_RVCORE_FAULTED                                           0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_RVCORE_NO_FAULT                                          0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_REG                                                      7:7            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_REG_FAULTED                                              0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_REG_NO_FAULT                                             0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_SE_LOGIC                                                 8:8            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_SE_LOGIC_FAULTED                                         0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_SE_LOGIC_NO_FAULT                                        0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_SE_KSLT                                                  9:9            /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_SE_KSLT_FAULTED                                          0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_SE_KSLT_NO_FAULT                                         0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_TKE                                                      10:10          /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_TKE_FAULTED                                              0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_TKE_NO_FAULT                                             0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_FBIF                                                     11:11          /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_FBIF_FAULTED                                             0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_FBIF_NO_FAULT                                            0x00000000     /* R-I-V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_MPURAM                                                   12:12          /* R-IVF */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_MPURAM_FAULTED                                           0x00000001     /* R---V */
#define NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT_MPURAM_NO_FAULT                                          0x00000000     /* R-I-V */

#define NV_PGSP_ECC_INTR_STATUS                                                                          0x00110888     /* R--4R */
#define NV_PGSP_ECC_INTR_STATUS_CORRECTED                                                                0:0            /* R--VF */
#define NV_PGSP_ECC_INTR_STATUS_CORRECTED_NOT_PENDING                                                    0x0            /* R---V */
#define NV_PGSP_ECC_INTR_STATUS_CORRECTED_PENDING                                                        0x1            /* R---V */
#define NV_PGSP_ECC_INTR_STATUS_UNCORRECTED                                                              1:1            /* R--VF */
#define NV_PGSP_ECC_INTR_STATUS_UNCORRECTED_NOT_PENDING                                                  0x0            /* R---V */
#define NV_PGSP_ECC_INTR_STATUS_UNCORRECTED_PENDING                                                      0x1            /* R---V */

#define NV_PGSP_FALCON_ECC_STATUS                                                                        0x00110878     /* RW-4R */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_IMEM                                                   8:8            /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_IMEM_NOT_PENDING                                       0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_IMEM_PENDING                                           0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_IMEM_CLEAR                                             0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DMEM                                                   9:9            /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DMEM_NOT_PENDING                                       0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DMEM_PENDING                                           0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DMEM_CLEAR                                             0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_MPU_RAM                                                10:10          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_MPU_RAM_NOT_PENDING                                    0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_MPU_RAM_PENDING                                        0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_MPU_RAM_CLEAR                                          0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCLS                                                   11:11          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCLS_NOT_PENDING                                       0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCLS_PENDING                                           0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCLS_CLEAR                                             0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_REG                                                    12:12          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_REG_NOT_PENDING                                        0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_REG_PENDING                                            0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_REG_CLEAR                                              0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_EMEM                                                   13:13          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_EMEM_NOT_PENDING                                       0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_EMEM_PENDING                                           0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_EMEM_CLEAR                                             0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_ICACHE                                                 14:14          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_ICACHE_NOT_PENDING                                     0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_ICACHE_PENDING                                         0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_ICACHE_CLEAR                                           0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCACHE                                                 15:15          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCACHE_NOT_PENDING                                     0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCACHE_PENDING                                         0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_DCACHE_CLEAR                                           0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_TOTAL_COUNTER_OVERFLOW                                   16:16          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_TOTAL_COUNTER_OVERFLOW_NOT_PENDING                       0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_TOTAL_COUNTER_OVERFLOW_PENDING                           0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_TOTAL_COUNTER_OVERFLOW_CLEAR                             0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW                                  17:17          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW_NOT_PENDING                      0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW_PENDING                          0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_CORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW_CLEAR                            0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_TOTAL_COUNTER_OVERFLOW                                 18:18          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_TOTAL_COUNTER_OVERFLOW_NOT_PENDING                     0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_TOTAL_COUNTER_OVERFLOW_PENDING                         0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_TOTAL_COUNTER_OVERFLOW_CLEAR                           0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW                                19:19          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW_NOT_PENDING                    0x0            /* R-I-V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW_PENDING                        0x1            /* R---V */
#define NV_PGSP_FALCON_ECC_STATUS_UNCORRECTED_ERR_UNIQUE_COUNTER_OVERFLOW_CLEAR                          0x1            /* -W--C */
#define NV_PGSP_FALCON_ECC_STATUS_RESET                                                                  31:31          /* RWIVF */
#define NV_PGSP_FALCON_ECC_STATUS_RESET_TASK                                                             0x1            /* -W--T */
#define NV_PGSP_FALCON_ECC_STATUS_RESET_INIT                                                             0x0            /* R-I-V */

#define NV_PGSP_FALCON_ECC_CORRECTED_ERR_COUNT                                                           0x00110880     /* RW-4R */
#define NV_PGSP_FALCON_ECC_CORRECTED_ERR_COUNT_TOTAL                                                     15:0           /* RWIVF */
#define NV_PGSP_FALCON_ECC_CORRECTED_ERR_COUNT_TOTAL_INIT                                                0x0            /* RWI-V */
#define NV_PGSP_FALCON_ECC_CORRECTED_ERR_COUNT_UNIQUE                                                    31:16          /* RWIVF */
#define NV_PGSP_FALCON_ECC_CORRECTED_ERR_COUNT_UNIQUE_INIT                                               0x0            /* RWI-V */

#define NV_PGSP_FALCON_ECC_UNCORRECTED_ERR_COUNT                                                         0x00110884     /* RW-4R */
#define NV_PGSP_FALCON_ECC_UNCORRECTED_ERR_COUNT_TOTAL                                                   15:0           /* RWIVF */
#define NV_PGSP_FALCON_ECC_UNCORRECTED_ERR_COUNT_TOTAL_INIT                                              0x0            /* RWI-V */
#define NV_PGSP_FALCON_ECC_UNCORRECTED_ERR_COUNT_UNIQUE                                                  31:16          /* RWIVF */
#define NV_PGSP_FALCON_ECC_UNCORRECTED_ERR_COUNT_UNIQUE_INIT                                             0x0            /* RWI-V */

#endif // __gb100_dev_gsp_h__
