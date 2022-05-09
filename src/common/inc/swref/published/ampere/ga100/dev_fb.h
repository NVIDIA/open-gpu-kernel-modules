/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __ga100_dev_fb_h__
#define __ga100_dev_fb_h__
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR                    0x00100C10 /* RW-4R */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_SHIFT                       8 /*       */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_ADR_39_08                31:0 /* RWIVF */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_ADR_39_08_INIT     0x00000000 /* RWI-V */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_HI                 0x00100C40 /* RW-4R */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_HI_MASK                  0x7F /*       */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_HI_ADR_63_40             23:0 /* RWIVF */
#define NV_PFB_FBHUB_POISON_INTR_VECTOR                                                    0x00100A24 /* R--4R */
#define NV_PFB_FBHUB_POISON_INTR_VECTOR_HW                                                        7:0 /* R-IVF */
#define NV_PFB_FBHUB_POISON_INTR_VECTOR_HW_INIT                     135 /* R-I-V */
#define NV_PFB_PRI_MMU_LOCK_CFG_PRIV_LEVEL_MASK                                      0x001FA7C8 /* RW-4R */
#define NV_PFB_PRI_MMU_LOCK_CFG_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0                      0:0 /*       */
#define NV_PFB_PRI_MMU_LOCK_CFG_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_ENABLE        0x00000001 /*       */
#define NV_PFB_PRI_MMU_LOCK_CFG_PRIV_LEVEL_MASK_READ_PROTECTION_LEVEL0_DISABLE       0x00000000 /*       */
#define NV_PFB_PRI_MMU_LOCK_ADDR_LO                                            0x001FA82C /* RW-4R */
#define NV_PFB_PRI_MMU_LOCK_ADDR_LO__PRIV_LEVEL_MASK       0x001FA7C8 /*       */
#define NV_PFB_PRI_MMU_LOCK_ADDR_LO_VAL                                              31:4 /* RWEVF */
#define NV_PFB_PRI_MMU_LOCK_ADDR_LO_ALIGNMENT                                  0x0000000c /*       */
#define NV_PFB_PRI_MMU_LOCK_ADDR_HI                                            0x001FA830 /* RW-4R */
#define NV_PFB_PRI_MMU_LOCK_ADDR_HI_VAL                                              31:4 /* RWEVF */
#define NV_PFB_PRI_MMU_LOCK_ADDR_HI_ALIGNMENT                                  0x0000000c /*       */
#endif // __ga100_dev_fb_h__
