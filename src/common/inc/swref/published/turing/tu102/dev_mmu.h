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

#ifndef __tu102_dev_mmu_h__
#define __tu102_dev_mmu_h__
#define NV_MMU_PDE_APERTURE_BIG                       (0*32+1):(0*32+0) /* RWXVF */
#define NV_MMU_PDE_APERTURE_BIG_INVALID                      0x00000000 /* RW--V */
#define NV_MMU_PDE_APERTURE_BIG_VIDEO_MEMORY                 0x00000001 /* RW--V */
#define NV_MMU_PDE_APERTURE_BIG_SYSTEM_COHERENT_MEMORY       0x00000002 /* RW--V */
#define NV_MMU_PDE_APERTURE_BIG_SYSTEM_NON_COHERENT_MEMORY   0x00000003 /* RW--V */
#define NV_MMU_PDE_SIZE                               (0*32+3):(0*32+2) /* RWXVF */
#define NV_MMU_PDE_SIZE_FULL                                 0x00000000 /* RW--V */
#define NV_MMU_PDE_SIZE_HALF                                 0x00000001 /* RW--V */
#define NV_MMU_PDE_SIZE_QUARTER                              0x00000002 /* RW--V */
#define NV_MMU_PDE_SIZE_EIGHTH                               0x00000003 /* RW--V */
#define NV_MMU_PDE_ADDRESS_BIG_SYS                   (0*32+31):(0*32+4) /* RWXVF */
#define NV_MMU_PDE_ADDRESS_BIG_VID                   (0*32+31-3):(0*32+4) /* RWXVF */
#define NV_MMU_PDE_APERTURE_SMALL                     (1*32+1):(1*32+0) /* RWXVF */
#define NV_MMU_PDE_APERTURE_SMALL_INVALID                    0x00000000 /* RW--V */
#define NV_MMU_PDE_APERTURE_SMALL_VIDEO_MEMORY               0x00000001 /* RW--V */
#define NV_MMU_PDE_APERTURE_SMALL_SYSTEM_COHERENT_MEMORY     0x00000002 /* RW--V */
#define NV_MMU_PDE_APERTURE_SMALL_SYSTEM_NON_COHERENT_MEMORY 0x00000003 /* RW--V */
#define NV_MMU_PDE_VOL_SMALL                          (1*32+2):(1*32+2) /* RWXVF */
#define NV_MMU_PDE_VOL_SMALL_TRUE                            0x00000001 /* RW--V */
#define NV_MMU_PDE_VOL_SMALL_FALSE                           0x00000000 /* RW--V */
#define NV_MMU_PDE_VOL_BIG                            (1*32+3):(1*32+3) /* RWXVF */
#define NV_MMU_PDE_VOL_BIG_TRUE                              0x00000001 /* RW--V */
#define NV_MMU_PDE_VOL_BIG_FALSE                             0x00000000 /* RW--V */
#define NV_MMU_PDE_ADDRESS_SMALL_SYS                 (1*32+31):(1*32+4) /* RWXVF */
#define NV_MMU_PDE_ADDRESS_SMALL_VID                 (1*32+31-3):(1*32+4) /* RWXVF */
#define NV_MMU_PDE_ADDRESS_SHIFT                             0x0000000c /*       */
#define NV_MMU_PDE__SIZE                                              8
#define NV_MMU_PTE_VALID                              (0*32+0):(0*32+0) /* RWXVF */
#define NV_MMU_PTE_VALID_TRUE                                       0x1 /* RW--V */
#define NV_MMU_PTE_VALID_FALSE                                      0x0 /* RW--V */
#define NV_MMU_PTE_PRIVILEGE                          (0*32+1):(0*32+1) /* RWXVF */
#define NV_MMU_PTE_PRIVILEGE_TRUE                                   0x1 /* RW--V */
#define NV_MMU_PTE_PRIVILEGE_FALSE                                  0x0 /* RW--V */
#define NV_MMU_PTE_READ_ONLY                          (0*32+2):(0*32+2) /* RWXVF */
#define NV_MMU_PTE_READ_ONLY_TRUE                                  0x1  /* RW--V */
#define NV_MMU_PTE_READ_ONLY_FALSE                                 0x0  /* RW--V */
#define NV_MMU_PTE_ENCRYPTED                          (0*32+3):(0*32+3) /* RWXVF */
#define NV_MMU_PTE_ENCRYPTED_TRUE                            0x00000001 /* R---V */
#define NV_MMU_PTE_ENCRYPTED_FALSE                           0x00000000 /* R---V */
#define NV_MMU_PTE_ADDRESS_SYS                      (0*32+31):(0*32+4) /* RWXVF */
#define NV_MMU_PTE_ADDRESS_VID                      (0*32+31-3):(0*32+4) /* RWXVF */
#define NV_MMU_PTE_ADDRESS_VID_PEER                (0*32+31):(0*32+32-3) /* RWXVF */
#define NV_MMU_PTE_ADDRESS_VID_PEER_0                       0x00000000 /* RW--V */
#define NV_MMU_PTE_ADDRESS_VID_PEER_1                       0x00000001 /* RW--V */
#define NV_MMU_PTE_ADDRESS_VID_PEER_2                       0x00000002 /* RW--V */
#define NV_MMU_PTE_ADDRESS_VID_PEER_3                       0x00000003 /* RW--V */
#define NV_MMU_PTE_ADDRESS_VID_PEER_4                       0x00000004 /* RW--V */
#define NV_MMU_PTE_ADDRESS_VID_PEER_5                       0x00000005 /* RW--V */
#define NV_MMU_PTE_ADDRESS_VID_PEER_6                       0x00000006 /* RW--V */
#define NV_MMU_PTE_ADDRESS_VID_PEER_7                       0x00000007 /* RW--V */
#define NV_MMU_PTE_VOL                                (1*32+0):(1*32+0) /* RWXVF */
#define NV_MMU_PTE_VOL_TRUE                                  0x00000001 /* RW--V */
#define NV_MMU_PTE_VOL_FALSE                                 0x00000000 /* RW--V */
#define NV_MMU_PTE_APERTURE                           (1*32+2):(1*32+1) /* RWXVF */
#define NV_MMU_PTE_APERTURE_VIDEO_MEMORY                     0x00000000 /* RW--V */
#define NV_MMU_PTE_APERTURE_PEER_MEMORY                      0x00000001 /* RW--V */
#define NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY           0x00000002 /* RW--V */
#define NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY       0x00000003 /* RW--V */
#define NV_MMU_PTE_LOCK                               (1*32+3):(1*32+3) /* RWXVF */
#define NV_MMU_PTE_LOCK_TRUE                                        0x1 /* RW--V */
#define NV_MMU_PTE_LOCK_FALSE                                       0x0 /* RW--V */
#define NV_MMU_PTE_COMPTAGLINE                      (1*32+20+11):(1*32+12) /* RWXVF */
#define NV_MMU_PTE_READ_DISABLE                     (1*32+30):(1*32+30) /* RWXVF */
#define NV_MMU_PTE_READ_DISABLE_TRUE                               0x1  /* RW--V */
#define NV_MMU_PTE_READ_DISABLE_FALSE                              0x0  /* RW--V */
#define NV_MMU_PTE_WRITE_DISABLE                    (1*32+31):(1*32+31) /* RWXVF */
#define NV_MMU_PTE_WRITE_DISABLE_TRUE                              0x1  /* RW--V */
#define NV_MMU_PTE_WRITE_DISABLE_FALSE                             0x0  /* RW--V */
#define NV_MMU_PTE_ADDRESS_SHIFT                             0x0000000c /*       */
#define NV_MMU_PTE__SIZE                                             8
#define NV_MMU_PTE_KIND                              (1*32+11):(1*32+4) /* RWXVF */
#define NV_MMU_PTE_KIND_INVALID                       0x07 /* R---V */
#define NV_MMU_PTE_KIND_PITCH                         0x00 /* R---V */
#define NV_MMU_PTE_KIND_GENERIC_MEMORY                                                  0x06 /* R---V */
#define NV_MMU_PTE_KIND_Z16                                                             0x01 /* R---V */
#define NV_MMU_PTE_KIND_S8                                                              0x02 /* R---V */
#define NV_MMU_PTE_KIND_S8Z24                                                           0x03 /* R---V */
#define NV_MMU_PTE_KIND_ZF32_X24S8                                                      0x04 /* R---V */
#define NV_MMU_PTE_KIND_Z24S8                                                           0x05 /* R---V */
#define NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE                                     0x08 /* R---V */
#define NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC                         0x09 /* R---V */
#define NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC                                     0x0A /* R---V */
#define NV_MMU_PTE_KIND_Z16_COMPRESSIBLE_DISABLE_PLC                                    0x0B /* R---V */
#define NV_MMU_PTE_KIND_S8Z24_COMPRESSIBLE_DISABLE_PLC                                  0x0C /* R---V */
#define NV_MMU_PTE_KIND_ZF32_X24S8_COMPRESSIBLE_DISABLE_PLC                             0x0D /* R---V */
#define NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC                                  0x0E /* R---V */
#define NV_MMU_PTE_KIND_SMSKED_MESSAGE                                                  0x0F /* R---V */
#define NV_MMU_CLIENT_KIND_Z16                                0x1 /* R---V */
#define NV_MMU_CLIENT_KIND_Z24S8                              0x5 /* R---V */
#define NV_MMU_CLIENT_KIND_INVALID                            0x7 /* R---V */
#define NV_MMU_VER2_PTE_ADDRESS_VID                            (35-3):8 /* RWXVF */
#define NV_MMU_VER2_PTE_COMPTAGLINE                          (20+35):36 /* RWXVF */
#define NV_MMU_VER2_PTE_ADDRESS_SHIFT                        0x0000000c /*       */
#endif // __tu102_dev_mmu_h__
