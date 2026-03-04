/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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


#ifndef __gp100_dev_fb_h__
#define __gp100_dev_fb_h__

#define NV_PFB_PRI_MMU_INVALIDATE_ALL_VA                                   0:0 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_VA_FALSE                      0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_VA_TRUE                       0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB                                  1:1 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB_FALSE                     0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB_TRUE                      0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_HUBTLB_ONLY                              2:2 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_HUBTLB_ONLY_FALSE                 0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_HUBTLB_ONLY_TRUE                  0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_REPLAY                                   5:3 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_REPLAY_NONE                       0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_REPLAY_START                      0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_REPLAY_START_ACK_ALL              0x00000002 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_REPLAY_CANCEL_TARGETED            0x00000003 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_REPLAY_CANCEL_GLOBAL              0x00000004 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_REPLAY_CANCEL                     0x00000004 /*       */
#define NV_PFB_PRI_MMU_INVALIDATE_SYS_MEMBAR                              6:6 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_SYS_MEMBAR_FALSE                 0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_SYS_MEMBAR_TRUE                  0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ACK                                     8:7 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_ACK_NONE_REQUIRED                0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ACK_INTRANODE                    0x00000002 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_ACK_GLOBALLY                     0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CANCEL_CLIENT_ID                       14:9 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_CANCEL_GPC_ID                         19:15 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_CANCEL_CLIENT_TYPE                    20:20 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_CANCEL_CLIENT_TYPE_GPC           0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CANCEL_CLIENT_TYPE_HUB           0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL                           26:24 /* RWXVF */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_ALL                  0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_PTE_ONLY             0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE0           0x00000002 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE1           0x00000003 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE2           0x00000004 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE3           0x00000005 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE4           0x00000006 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE5           0x00000007 /* RW--V */
#define NV_PFB_PRI_MMU_INVALIDATE_TRIGGER                                31:31 /* -WEVF */
#define NV_PFB_PRI_MMU_INVALIDATE_TRIGGER_FALSE                     0x00000000 /* -WE-V */
#define NV_PFB_PRI_MMU_INVALIDATE_TRIGGER_TRUE                      0x00000001 /* -W--T */

#endif // __gv100_dev_fb_h__
