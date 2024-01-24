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

#ifndef __gv100_dev_fault_h__
#define __gv100_dev_fault_h__
#define NV_PFAULT_MMU_ENG_ID_GRAPHICS          64 /*       */
#define NV_PFAULT_MMU_ENG_ID_PHYSICAL          31 /*       */
#define NV_PFAULT_ACCESS_TYPE_VIRT_READ          0x00000000 /*       */
#define NV_PFAULT_ACCESS_TYPE_VIRT_WRITE         0x00000001 /*       */
#define NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC        0x00000002 /*       */
#define NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_STRONG 0x00000002 /*       */
#define NV_PFAULT_ACCESS_TYPE_VIRT_PREFETCH      0x00000003 /*       */
#define NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_WEAK   0x00000004 /*       */
#define NV_PFAULT_ACCESS_TYPE_PHYS_READ          0x00000008 /*       */
#define NV_PFAULT_ACCESS_TYPE_PHYS_WRITE         0x00000009 /*       */
#define NV_PFAULT_ACCESS_TYPE_PHYS_ATOMIC        0x0000000a /*       */
#define NV_PFAULT_ACCESS_TYPE_PHYS_PREFETCH      0x0000000b /*       */
#define NV_PFAULT_MMU_ENG_ID_CE0               15 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE1               16 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE2               17 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE3               18 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE4               19 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE5               20 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE6               21 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE7               22 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE8               23 /*       */
#define NV_PFAULT_FAULT_TYPE_PDE                  0x00000000 /*       */
#define NV_PFAULT_FAULT_TYPE_PDE_SIZE             0x00000001 /*       */
#define NV_PFAULT_FAULT_TYPE_PTE                  0x00000002 /*       */
#define NV_PFAULT_FAULT_TYPE_VA_LIMIT_VIOLATION   0x00000003 /*       */
#define NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK   0x00000004 /*       */
#define NV_PFAULT_FAULT_TYPE_PRIV_VIOLATION       0x00000005 /*       */
#define NV_PFAULT_FAULT_TYPE_RO_VIOLATION         0x00000006 /*       */
#define NV_PFAULT_FAULT_TYPE_WO_VIOLATION         0x00000007 /*       */
#define NV_PFAULT_FAULT_TYPE_PITCH_MASK_VIOLATION 0x00000008 /*       */
#define NV_PFAULT_FAULT_TYPE_WORK_CREATION        0x00000009 /*       */
#define NV_PFAULT_FAULT_TYPE_UNSUPPORTED_APERTURE 0x0000000a /*       */
#define NV_PFAULT_FAULT_TYPE_COMPRESSION_FAILURE  0x0000000b /*       */
#define NV_PFAULT_FAULT_TYPE_UNSUPPORTED_KIND     0x0000000c /*       */
#define NV_PFAULT_FAULT_TYPE_REGION_VIOLATION     0x0000000d /*       */
#define NV_PFAULT_FAULT_TYPE_POISONED             0x0000000e /*       */
#define NV_PFAULT_FAULT_TYPE_ATOMIC_VIOLATION     0x0000000f /*       */
#define NV_PFAULT_MMU_CLIENT_TYPE_GPC    0x00000000 /*       */
#define NV_PFAULT_MMU_CLIENT_TYPE_HUB    0x00000001 /*       */
#endif // __gv100_dev_fault_h__
