/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_hubmmu_base_h__
#define __gb100_dev_hubmmu_base_h__


#define NV_HUBMMU_PRI_MMU_FAULT_STATUS                                           0x0000A560 /* RW-4R */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS                                0:0 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS_RESET                   0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS_CLEAR                   0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS_SET                     0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT                                1:1 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT_RESET                   0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT_CLEAR                   0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT_SET                     0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS                                2:2 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS_RESET                   0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS_CLEAR                   0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS_SET                     0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT                                3:3 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT_RESET                   0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT_CLEAR                   0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT_SET                     0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS                                 4:4 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS_RESET                    0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS_CLEAR                    0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS_SET                      0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT                                 5:5 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT_RESET                    0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT_CLEAR                    0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT_SET                      0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS                               6:6 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS_RESET                  0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS_CLEAR                  0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS_SET                    0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT                               7:7 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT_RESET                  0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT_CLEAR                  0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT_SET                    0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_REPLAYABLE                                       8:8 /* R-EVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_REPLAYABLE_RESET                          0x00000000 /* R-E-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_REPLAYABLE_SET                            0x00000001 /* R---V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_VALID                                          31:31 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_VALID_RESET                               0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_VALID_CLEAR                               0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_STATUS_VALID_SET                                 0x00000001 /* RW--V */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_HI_VIRT(i,j)             (0x2004+((i)*128)+((j)*20)) /* RW-4A */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_HI_VIRT__SIZE_1                                                          64 /*       */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_HI_VIRT__SIZE_2                                                           2 /*       */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_HI_VIRT__PRIV_LEVEL_MASK      0xaa70          /*       */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_HI_VIRT_ADDR                  31:0 /* RW-VF */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT(i,j)             (0x2000+((i)*128)+((j)*20)) /* RW-4A */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT__SIZE_1                                                          64 /*       */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT__SIZE_2                                                           2 /*       */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT__PRIV_LEVEL_MASK      0xaa70          /*       */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT_ADDR_MODE             0:0      /* R--VF */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT_PHYS_APERTURE         2:1  /* RW-VF */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT_PHYS_VOL              3:3       /* RW-VF */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_LO_VIRT_ADDR                  31:12           /* RW-VF */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE                      31:31 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE_FALSE           0x00000000 /* RWE-V */
#define NV_HUBMMU_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE_TRUE            0x00000001 /* RW--V */

#define NV_HUBMMU_PRI_MMU_WPR1_ADDR_LO                             0xa81c /* RW-4R */
#define NV_HUBMMU_PRI_MMU_WPR1_ADDR_LO_VAL                         31:4 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_WPR1_ADDR_LO_ALIGNMENT                   0x0000000c /*       */
#define NV_HUBMMU_PRI_MMU_WPR1_ADDR_HI                             0xa820 /* RW-4R */
#define NV_HUBMMU_PRI_MMU_WPR1_ADDR_HI_VAL                         31:4 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_WPR1_ADDR_HI_ALIGNMENT                   0x0000000c /*       */
#define NV_HUBMMU_PRI_MMU_WPR2_ADDR_LO                             0xa824 /* RW-4R */
#define NV_HUBMMU_PRI_MMU_WPR2_ADDR_LO_VAL                         31:4 /* RWEVF */
#define NV_HUBMMU_PRI_MMU_WPR2_ADDR_LO_ALIGNMENT                   0x0000000c /*       */
#define NV_HUBMMU_PRI_MMU_WPR2_ADDR_HI                             0xa828 /* RW-4R */
#define NV_HUBMMU_PRI_MMU_WPR2_ADDR_HI_VAL                         31:4   /* RWEVF */
#define NV_HUBMMU_PRI_MMU_WPR2_ADDR_HI_ALIGNMENT                   0x0000000c /*       */
#endif // __gb100_dev_hubmmu_base_h__
