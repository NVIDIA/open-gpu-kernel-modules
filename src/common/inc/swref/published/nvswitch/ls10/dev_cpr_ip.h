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

#ifndef __ls10_dev_cpr_ip_h__
#define __ls10_dev_cpr_ip_h__
#define NV_CPR_SYS_NVLW_CG1                                   0x00000110      /* RW-4R */
#define NV_CPR_SYS_NVLW_CG1_SLCG                              0:0             /* RWEVF */
#define NV_CPR_SYS_NVLW_CG1_SLCG_ENABLED                      0x00000000      /* RW--V */
#define NV_CPR_SYS_NVLW_CG1_SLCG_DISABLED                     0x00000001      /* RWE-V */
#define NV_CPR_SYS_NVLW_CG1_SLCG__PROD                        0x00000000      /* RW--V */
#define NV_CPR_SYS_INTR_CTRL(i)                               (0x00000130+(i)*0x4) /* RW-4A */
#define NV_CPR_SYS_INTR_CTRL__SIZE_1                          3               /*       */
#define NV_CPR_SYS_INTR_CTRL_MESSAGE                          31:0            /* RWEVF */
#define NV_CPR_SYS_INTR_CTRL_MESSAGE_INIT                     0x00000000      /* RWE-V */
#define NV_CPR_SYS_INTR_RETRIGGER(i)                          (0x00000150+(i)*0x4) /* RW-4A */
#define NV_CPR_SYS_INTR_RETRIGGER__SIZE_1                     3               /*       */
#define NV_CPR_SYS_INTR_RETRIGGER_TRIGGER                     0:0             /* RWEVF */
#define NV_CPR_SYS_INTR_RETRIGGER_TRIGGER_INIT                0x00000000      /* RWE-V */
#define NV_CPR_SYS_INTR_RETRIGGER_TRIGGER_TRUE                0x00000001      /* RW--V */
#define NV_CPR_SYS_NVLW_INTR_0_STATUS                         0x000001a0      /* R--4R */
#define NV_CPR_SYS_NVLW_INTR_0_STATUS_CPR_INTR                0:0             /* R-EVF */
#define NV_CPR_SYS_NVLW_INTR_0_STATUS_CPR_INTR_INIT           0x00000000      /* R-E-V */
#define NV_CPR_SYS_NVLW_INTR_0_STATUS_INTR0                   31:31           /* R-EVF */
#define NV_CPR_SYS_NVLW_INTR_0_STATUS_INTR0_INIT              0x00000000      /* R-E-V */
#define NV_CPR_SYS_NVLW_INTR_1_STATUS                         0x000001a4      /* R--4R */
#define NV_CPR_SYS_NVLW_INTR_1_STATUS_CPR_INTR                0:0             /* R-EVF */
#define NV_CPR_SYS_NVLW_INTR_1_STATUS_CPR_INTR_INIT           0x00000000      /* R-E-V */
#define NV_CPR_SYS_NVLW_INTR_1_STATUS_INTR1                   31:31           /* R-EVF */
#define NV_CPR_SYS_NVLW_INTR_1_STATUS_INTR1_INIT              0x00000000      /* R-E-V */
#define NV_CPR_SYS_NVLW_INTR_2_STATUS                         0x000001a8      /* R--4R */
#define NV_CPR_SYS_NVLW_INTR_2_STATUS_CPR_INTR                0:0             /* R-EVF */
#define NV_CPR_SYS_NVLW_INTR_2_STATUS_CPR_INTR_INIT           0x00000000      /* R-E-V */
#define NV_CPR_SYS_NVLW_INTR_2_STATUS_INTR2                   31:31           /* R-EVF */
#define NV_CPR_SYS_NVLW_INTR_2_STATUS_INTR2_INIT              0x00000000      /* R-E-V */
#define NV_CPR_SYS_NVLW_INTR_0_MASK                           0x000001c0      /* RW-4R */
#define NV_CPR_SYS_NVLW_INTR_0_MASK_CPR_INTR                  0:0             /* RWEVF */
#define NV_CPR_SYS_NVLW_INTR_0_MASK_CPR_INTR_ENABLE           0x00000001      /* RW--V */
#define NV_CPR_SYS_NVLW_INTR_0_MASK_CPR_INTR_DISABLE          0x00000000      /* RWE-V */
#define NV_CPR_SYS_NVLW_INTR_0_MASK_INTR0                     1:1             /* RWEVF */
#define NV_CPR_SYS_NVLW_INTR_0_MASK_INTR0_ENABLE              0x00000001      /* RW--V */
#define NV_CPR_SYS_NVLW_INTR_0_MASK_INTR0_DISABLE             0x00000000      /* RWE-V */
#define NV_CPR_SYS_NVLW_INTR_1_MASK                           0x000001c4      /* RW-4R */
#define NV_CPR_SYS_NVLW_INTR_1_MASK_CPR_INTR                  0:0             /* RWEVF */
#define NV_CPR_SYS_NVLW_INTR_1_MASK_CPR_INTR_ENABLE           0x00000001      /* RW--V */
#define NV_CPR_SYS_NVLW_INTR_1_MASK_CPR_INTR_DISABLE          0x00000000      /* RWE-V */
#define NV_CPR_SYS_NVLW_INTR_1_MASK_INTR1                     1:1             /* RWEVF */
#define NV_CPR_SYS_NVLW_INTR_1_MASK_INTR1_ENABLE              0x00000001      /* RW--V */
#define NV_CPR_SYS_NVLW_INTR_1_MASK_INTR1_DISABLE             0x00000000      /* RWE-V */
#define NV_CPR_SYS_NVLW_INTR_2_MASK                           0x000001c8      /* RW-4R */
#define NV_CPR_SYS_NVLW_INTR_2_MASK_CPR_INTR                  0:0             /* RWEVF */
#define NV_CPR_SYS_NVLW_INTR_2_MASK_CPR_INTR_ENABLE           0x00000001      /* RW--V */
#define NV_CPR_SYS_NVLW_INTR_2_MASK_CPR_INTR_DISABLE          0x00000000      /* RWE-V */
#define NV_CPR_SYS_NVLW_INTR_2_MASK_INTR2                     1:1             /* RWEVF */
#define NV_CPR_SYS_NVLW_INTR_2_MASK_INTR2_ENABLE              0x00000001      /* RW--V */
#define NV_CPR_SYS_NVLW_INTR_2_MASK_INTR2_DISABLE             0x00000000      /* RWE-V */
#define NV_CPR_SYS_ERR_STATUS_0                               0x00000200      /* RW-4R */
#define NV_CPR_SYS_ERR_STATUS_0_ENGINE_RESET_ERR              0:0             /* RWIVF */
#define NV_CPR_SYS_ERR_STATUS_0_ENGINE_RESET_ERR_NONE         0x00000000      /* RWI-V */
#define NV_CPR_SYS_ERR_STATUS_0_ENGINE_RESET_ERR_CLEAR        0x00000001      /* RW--V */
#define NV_CPR_SYS_ERR_LOG_EN_0                               0x00000204      /* RW-4R */
#define NV_CPR_SYS_ERR_LOG_EN_0_ENGINE_RESET_ERR              0:0             /* RWEVF */
#define NV_CPR_SYS_ERR_LOG_EN_0_ENGINE_RESET_ERR__PROD        0x00000001      /* RW--V */
#define NV_CPR_SYS_ERR_LOG_EN_0_ENGINE_RESET_ERR_DISABLE      0x00000000      /* RWE-V */
#define NV_CPR_SYS_ERR_LOG_EN_0_ENGINE_RESET_ERR_ENABLE       0x00000001      /* RW--V */
#define NV_CPR_SYS_ERR_FATAL_REPORT_EN_0                      0x00000208      /* RW-4R */
#define NV_CPR_SYS_ERR_FATAL_REPORT_EN_0_ENGINE_RESET_ERR     0:0             /* RWEVF */
#define NV_CPR_SYS_ERR_FATAL_REPORT_EN_0_ENGINE_RESET_ERR_DISABLE 0x00000000  /* RWE-V */
#define NV_CPR_SYS_ERR_FATAL_REPORT_EN_0_ENGINE_RESET_ERR_ENABLE 0x00000001   /* RW--V */
#define NV_CPR_SYS_ERR_NON_FATAL_REPORT_EN_0                  0x0000020c      /* RW-4R */
#define NV_CPR_SYS_ERR_NON_FATAL_REPORT_EN_0_ENGINE_RESET_ERR 0:0             /* RWEVF */
#define NV_CPR_SYS_ERR_NON_FATAL_REPORT_EN_0_ENGINE_RESET_ERR_DISABLE 0x00000000 /* RWE-V */
#define NV_CPR_SYS_ERR_NON_FATAL_REPORT_EN_0_ENGINE_RESET_ERR_ENABLE 0x00000001 /* RW--V */
#define NV_CPR_SYS_ERR_CORRECTABLE_REPORT_EN_0                0x00000210      /* RW-4R */
#define NV_CPR_SYS_ERR_CORRECTABLE_REPORT_EN_0_ENGINE_RESET_ERR 0:0           /* RWEVF */
#define NV_CPR_SYS_ERR_CORRECTABLE_REPORT_EN_0_ENGINE_RESET_ERR_DISABLE 0x00000000 /* RWE-V */
#define NV_CPR_SYS_ERR_CORRECTABLE_REPORT_EN_0_ENGINE_RESET_ERR_ENABLE 0x00000001 /* RW--V */
#define NV_CPR_SYS_ERR_CONTAIN_EN_0                           0x00000214      /* RW-4R */
#define NV_CPR_SYS_ERR_CONTAIN_EN_0_ENGINE_RESET_ERR          0:0             /* RWEVF */
#define NV_CPR_SYS_ERR_CONTAIN_EN_0_ENGINE_RESET_ERR_DISABLE  0x00000000      /* RWE-V */
#define NV_CPR_SYS_ERR_CONTAIN_EN_0_ENGINE_RESET_ERR_ENABLE   0x00000001      /* RW--V */
#define NV_CPR_SYS_ERR_FIRST_0                                0x0000021c      /* RW-4R */
#define NV_CPR_SYS_ERR_FIRST_0_ENGINE_RESET_ERR               0:0             /* RWIVF */
#define NV_CPR_SYS_ERR_FIRST_0_ENGINE_RESET_ERR_NONE          0x00000000      /* RWI-V */
#define NV_CPR_SYS_ERR_FIRST_0_ENGINE_RESET_ERR_CLEAR         0x00000001      /* RW--V */
#endif // __ls10_dev_cpr_ip_h__
