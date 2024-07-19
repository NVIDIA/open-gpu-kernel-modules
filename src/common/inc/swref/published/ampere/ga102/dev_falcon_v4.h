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
#ifndef __ga102_dev_falcon_v4_h__
#define __ga102_dev_falcon_v4_h__

#define NV_PFALCON_FALCON_IRQSCLR                                                                      0x00000004     /* -W-4R */
#define NV_PFALCON_FALCON_IRQSCLR_HALT                                                                 4:4            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_HALT_SET                                                             0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSCLR_SWGEN0                                                               6:6            /* -WXVF */
#define NV_PFALCON_FALCON_IRQSCLR_SWGEN0_SET                                                           0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQSTAT                                                                      0x00000008     /* R--4R */
#define NV_PFALCON_FALCON_IRQSTAT_HALT                                                                 4:4            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_HALT_TRUE                                                            0x00000001     /* R---V */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN0                                                               6:6            /* R-IVF */
#define NV_PFALCON_FALCON_IRQSTAT_SWGEN0_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_INTR_RETRIGGER(i)                                                            (0x000003e8+(i)*4) /* -W-4A */
#define NV_PFALCON_FALCON_INTR_RETRIGGER__SIZE_1                                                       2              /*       */
#define NV_PFALCON_FALCON_INTR_RETRIGGER_TRIGGER                                                       0:0            /* -W-VF */
#define NV_PFALCON_FALCON_INTR_RETRIGGER_TRIGGER_TRUE                                                  0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_IRQMSET                                                                      0x00000010     /* -W-4R */
#define NV_PFALCON_FALCON_IRQMCLR                                                                      0x00000014     /* -W-4R */
#define NV_PFALCON_FALCON_IRQMASK                                                                      0x00000018     /* R--4R */
#define NV_PFALCON_FALCON_IRQDEST                                                                      0x0000001c     /* RW-4R */
#define NV_PFALCON_FALCON_MAILBOX0                                                                     0x00000040     /* RW-4R */
#define NV_PFALCON_FALCON_MAILBOX1                                                                     0x00000044     /* RW-4R */
#define NV_PFALCON_FALCON_DMACTL                                                                       0x0000010c     /* RW-4R */
#define NV_PFALCON_FALCON_DMACTL_REQUIRE_CTX                                                           0:0            /* RWIVF */
#define NV_PFALCON_FALCON_DMACTL_REQUIRE_CTX_FALSE                                                     0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMACTL_DMEM_SCRUBBING                                                        1:1            /* R--VF */
#define NV_PFALCON_FALCON_DMACTL_DMEM_SCRUBBING_DONE                                                   0x00000000     /* R---V */
#define NV_PFALCON_FALCON_DMACTL_IMEM_SCRUBBING                                                        2:2            /* R--VF */
#define NV_PFALCON_FALCON_DMACTL_IMEM_SCRUBBING_DONE                                                   0x00000000     /* R---V */
#define NV_PFALCON_FALCON_DMATRFBASE                                                                   0x00000110     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFBASE_BASE                                                              31:0           /* RWIVF */
#define NV_PFALCON_FALCON_DMATRFMOFFS                                                                  0x00000114     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFMOFFS_OFFS                                                             23:0           /* RWIVF */
#define NV_PFALCON_FALCON_DMATRFCMD                                                                    0x00000118     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFCMD_FULL                                                               0:0            /* R-XVF */
#define NV_PFALCON_FALCON_DMATRFCMD_FULL_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_DMATRFCMD_FULL_FALSE                                                         0x00000000     /* R---V */
#define NV_PFALCON_FALCON_DMATRFCMD_IDLE                                                               1:1            /* R-XVF */
#define NV_PFALCON_FALCON_DMATRFCMD_IDLE_TRUE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_DMATRFCMD_IDLE_FALSE                                                         0x00000000     /* R---V */
#define NV_PFALCON_FALCON_DMATRFCMD_SEC                                                                3:2            /* RWXVF */
#define NV_PFALCON_FALCON_DMATRFCMD_IMEM                                                               4:4            /* RWXVF */
#define NV_PFALCON_FALCON_DMATRFCMD_IMEM_TRUE                                                          0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMATRFCMD_IMEM_FALSE                                                         0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMATRFCMD_WRITE                                                              5:5            /* RWXVF */
#define NV_PFALCON_FALCON_DMATRFCMD_WRITE_TRUE                                                         0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMATRFCMD_WRITE_FALSE                                                        0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMATRFCMD_SIZE                                                               10:8           /* RWXVF */
#define NV_PFALCON_FALCON_DMATRFCMD_SIZE_256B                                                          0x00000006     /* RW--V */
#define NV_PFALCON_FALCON_DMATRFCMD_CTXDMA                                                             14:12          /* RWXVF */
#define NV_PFALCON_FALCON_DMATRFCMD_SET_DMTAG                                                          16:16          /* RWIVF */
#define NV_PFALCON_FALCON_DMATRFCMD_SET_DMTAG_TRUE                                                     0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMATRFFBOFFS                                                                 0x0000011c     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFFBOFFS_OFFS                                                            31:0           /* RWIVF */
#define NV_PFALCON_FALCON_DMATRFBASE1                                                                  0x00000128     /* RW-4R */
#define NV_PFALCON_FALCON_DMATRFBASE1_BASE                                                             8:0            /* RWIVF */
#define NV_PFALCON_FALCON_IMEMC(i)                                                                     (0x00000180+(i)*16) /* RW-4A */
#define NV_PFALCON_FALCON_IMEMC_OFFS                                                                   7:2            /* RWIVF */
#define NV_PFALCON_FALCON_IMEMC_BLK                                                                    23:8           /* RWIVF */
#define NV_PFALCON_FALCON_IMEMC_AINCW                                                                  24:24          /* RWIVF */
#define NV_PFALCON_FALCON_IMEMC_AINCW_TRUE                                                             0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_IMEMC_AINCW_FALSE                                                            0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_IMEMC_SECURE                                                                 28:28          /* RWIVF */
#define NV_PFALCON_FALCON_IMEMD(i)                                                                     (0x00000184+(i)*16) /* RW-4A */
#define NV_PFALCON_FALCON_IMEMD_DATA                                                                   31:0           /* RWXVF */
#define NV_PFALCON_FALCON_IMEMT(i)                                                                     (0x00000188+(i)*16) /* RW-4A */
#define NV_PFALCON_FALCON_IMEMT_TAG                                                                    15:0           /* RWXVF */
#define NV_PFALCON_FALCON_DMEMC(i)                                                                     (0x000001c0+(i)*8) /* RW-4A */
#define NV_PFALCON_FALCON_DMEMC_OFFS                                                                   7:2            /*       */
#define NV_PFALCON_FALCON_DMEMC_BLK                                                                    23:8           /*       */
#define NV_PFALCON_FALCON_DMEMC_AINCW                                                                  24:24          /* RWIVF */
#define NV_PFALCON_FALCON_DMEMC_AINCW_TRUE                                                             0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_DMEMC_AINCW_FALSE                                                            0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_DMEMD(i)                                                                     (0x000001c4+(i)*8) /* RW-4A */
#define NV_PFALCON_FALCON_DMEMD_DATA                                                                   31:0           /* RWXVF */
#define NV_PFALCON_FALCON_HWCFG                                                                        0x00000108     /* R--4R */
#define NV_PFALCON_FALCON_HWCFG_IMEM_SIZE                                                              8:0            /* R--VF */
#define NV_PFALCON_FALCON_HWCFG2                                                                       0x000000f4     /* R--4R */
#define NV_PFALCON_FALCON_HWCFG2_RISCV                                                                 10:10          /* R--VF */
#define NV_PFALCON_FALCON_HWCFG2_RISCV_ENABLE                                                          0x00000001     /* R---V */
#define NV_PFALCON_FALCON_HWCFG2_MEM_SCRUBBING                                                         12:12          /* R--VF */
#define NV_PFALCON_FALCON_HWCFG2_MEM_SCRUBBING_DONE                                                    0x00000000     /* R---V */
#define NV_PFALCON_FALCON_OS                                                                           0x00000080     /* RW-4R */
#define NV_PFALCON_FALCON_RM                                                                           0x00000084     /* RW-4R */
#define NV_PFALCON_FALCON_DEBUGINFO                                                                    0x00000094     /* RW-4R */
#define NV_PFALCON_FALCON_CPUCTL                                                                       0x00000100     /* RW-4R */
#define NV_PFALCON_FALCON_CPUCTL_STARTCPU                                                              1:1            /* -WXVF */
#define NV_PFALCON_FALCON_CPUCTL_STARTCPU_TRUE                                                         0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_STARTCPU_FALSE                                                        0x00000000     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_HALTED                                                                4:4            /* R-XVF */
#define NV_PFALCON_FALCON_CPUCTL_HALTED_TRUE                                                           0x00000001     /* R---V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_EN                                                              6:6            /* RWIVF */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_EN_TRUE                                                         0x00000001     /* RW--V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_EN_FALSE                                                        0x00000000     /* RW--V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS                                                                 0x00000130     /* -W-4R */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_STARTCPU                                                        1:1            /* -WXVF */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_STARTCPU_TRUE                                                   0x00000001     /* -W--V */
#define NV_PFALCON_FALCON_CPUCTL_ALIAS_STARTCPU_FALSE                                                  0x00000000     /* -W--V */
#define NV_PFALCON_FALCON_BOOTVEC                                                                      0x00000104     /* RW-4R */

#endif // __ga102_dev_falcon_v4_h__
