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

#ifndef __gm107_dev_fb_h__
#define __gm107_dev_fb_h__
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR                    0x00100C10 /* RW-4R */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_SHIFT                       8 /*       */
#define NV_PFB_NISO_FLUSH_SYSMEM_ADDR_ADR_39_08                31:0 /* RWIVF */
#define NV_PFB_PRI_MMU_INVALIDATE_PDB                               0x00100CB8 /* RW-4R */
#define NV_PFB_PRI_MMU_INVALIDATE_PDB_APERTURE                             1:1 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_PDB_APERTURE_VID_MEM              0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_PDB_APERTURE_SYS_MEM              0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_PDB_ADDR                                31:4 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_PDB_ADDR_ALIGNMENT                0x0000000c /*       */
#define NV_PFB_PRI_MMU_INVALIDATE                                   0x00100CBC /* RW-4R */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_VA                                   0:0 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_VA_FALSE                      0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_VA_TRUE                       0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB                                  1:1 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB_FALSE                     0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB_TRUE                      0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_HUBTLB_ONLY                              2:2 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_HUBTLB_ONLY_FALSE                 0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_HUBTLB_ONLY_TRUE                  0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_TRIGGER                                31:31 /* -WEVF */
#define NV_PFB_PRI_MMU_INVALIDATE_TRIGGER_FALSE                     0x00000000 /* -WE-V */
#define NV_PFB_PRI_MMU_INVALIDATE_TRIGGER_TRUE                      0x00000001 /* -W--T */
#endif // __gm107_dev_fb_h__
