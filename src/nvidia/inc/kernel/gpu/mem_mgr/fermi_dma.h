/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef FERMI_DMA_H
#define FERMI_DMA_H

#ifdef NV_MMU_PTE
#error "dev_mmu.h included before fermi_dma.h"
#endif
#ifdef NV_PLTCG
#error "dev_ltc.h included before fermi_dma.h"
#endif

#include "gpu/mem_mgr/mem_desc.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"

// TODO -- replace those FERMI_* define usages in RM code
#define FERMI_SMALL_PAGESIZE    RM_PAGE_SIZE
#define FERMI_BIG_PAGESIZE_64K  RM_PAGE_SIZE_64K
#define FERMI_BIG_PAGESIZE_128K RM_PAGE_SIZE_128K

typedef struct DMAHALINFO_FERMI
{
    NvU32              vasReverse;
    NvU32              compTagLineMultiplier;     // Comptaglines increment by this value for VERIF only, see Bug 501651
} DMAHALINFO_FERMI, *PDMAHALINFO_FERMI;

#define DMA_GET_FERMI_INFOBLK(p)    ((PDMAHALINFO_FERMI) getInfoPtr((p)->infoList, HAL_IMPL_GF100))

//
// From GF100 dev_mmu.ref:
//    Each PDE maps a 64MB region of virtual memory when using 64KB big pages, or 128MB
//    when using 128KB big pages.  To map all 40b of virtual address space, the page
//    directory consists of 16K entries when using 64KB big pages (64MB * 16K = 2^26 * 2^14 = 2^40),
//    or 8K entries when using 128KB big pages (128MB * 8K = 2^27 * 2^13 = 2^40).
//
#define VASPACE_SIZE_MB_FERMI                   (1 << 20)
#define VASPACE_SIZE_FERMI                      (((NvU64)VASPACE_SIZE_MB_FERMI) << 20)

// Alignment Defines for page tables
#define PDB_SHIFT_FERMI                         12   // FERMITODO::DMA Will be there in dev_ram very soon
#define PDB_ALIGNMENT_FERMI                     (1 << PDB_SHIFT_FERMI)

#endif // FERMI_DMA_H
