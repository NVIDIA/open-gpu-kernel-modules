/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES
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
#ifndef __gh100_dev_riscv_pri_h__
#define __gh100_dev_riscv_pri_h__

#define NV_PRISCV_RISCV_BCR_DMACFG                                                                     0x0000066c     /* RW-4R */
#define NV_PRISCV_RISCV_BCR_DMACFG_TARGET                                                              1:0            /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMACFG_TARGET_LOCAL_FB                                                     0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMACFG_TARGET_COHERENT_SYSMEM                                              0x00000001     /* RW--V */
#define NV_PRISCV_RISCV_BCR_DMACFG_TARGET_NONCOHERENT_SYSMEM                                           0x00000002     /* RW--V */
#define NV_PRISCV_RISCV_BCR_DMACFG_TARGET_IO                                                           0x00000003     /* RW--V */
#define NV_PRISCV_RISCV_BCR_DMACFG_POINTER_WALKING                                                     28:28          /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMACFG_POINTER_WALKING_FALSE                                               0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMACFG_POINTER_WALKING_TRUE                                                0x00000001     /* RW--V */
#define NV_PRISCV_RISCV_BCR_DMACFG_LOCK                                                                31:31          /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMACFG_LOCK_UNLOCKED                                                       0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMACFG_LOCK_LOCKED                                                         0x00000001     /* RW--V */

#define NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_LO                                                        0x00000670     /* RW-4R */
#define NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_LO_VAL                                                    31:0           /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_LO_VAL_INIT                                               0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_HI                                                        0x00000674     /* RW-4R */
#define NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_HI_VAL                                                    11:0           /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_HI_VAL_INIT                                               0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_LO                                                         0x00000678     /* RW-4R */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_LO_VAL                                                     31:0           /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_LO_VAL_INIT                                                0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_HI                                                         0x0000067c     /* RW-4R */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_HI_VAL                                                     11:0           /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_HI_VAL_INIT                                                0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_LO                                                         0x00000680     /* RW-4R */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_LO_VAL                                                     31:0           /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_LO_VAL_INIT                                                0x00000000     /* RWI-V */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_HI                                                         0x00000684     /* RW-4R */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_HI_VAL                                                     11:0           /* RWIVF */
#define NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_HI_VAL_INIT                                                0x00000000     /* RWI-V */

#define NV_PRISCV_RISCV_CPUCTL                                                                         0x00000388     /* RW-4R */
#define NV_PRISCV_RISCV_CPUCTL_STARTCPU                                                                0:0            /* -WIVF */
#define NV_PRISCV_RISCV_CPUCTL_STARTCPU_FALSE                                                          0x00000000     /* -WI-V */
#define NV_PRISCV_RISCV_CPUCTL_STARTCPU_TRUE                                                           0x00000001     /* -W--V */
#define NV_PRISCV_RISCV_CPUCTL_HALTED                                                                  4:4            /* R-IVF */
#define NV_PRISCV_RISCV_CPUCTL_HALTED_INIT                                                             0x00000001     /* R-I-V */
#define NV_PRISCV_RISCV_CPUCTL_HALTED_TRUE                                                             0x00000001     /* R---V */
#define NV_PRISCV_RISCV_CPUCTL_HALTED_FALSE                                                            0x00000000     /* R---V */
#define NV_PRISCV_RISCV_CPUCTL_STOPPED                                                                 5:5            /* R-IVF */
#define NV_PRISCV_RISCV_CPUCTL_STOPPED_INIT                                                            0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_CPUCTL_ACTIVE_STAT                                                             7:7            /* R-IVF */
#define NV_PRISCV_RISCV_CPUCTL_ACTIVE_STAT_INACTIVE                                                    0x00000000     /* R-I-V */
#define NV_PRISCV_RISCV_CPUCTL_ACTIVE_STAT_ACTIVE                                                      0x00000001     /* R---V */

#endif // __gh100_dev_riscv_pri_h__
