/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_PGPROT_H__

#define __NV_PGPROT_H__

#include "cpuopsys.h"

#include <linux/mm.h>

#if !defined(NV_VMWARE)
#if defined(NVCPU_X86_64)
/* mark memory UC-, rather than UC (don't use _PAGE_PWT) */
static inline pgprot_t pgprot_noncached_weak(pgprot_t old_prot)
    {
        pgprot_t new_prot = old_prot;
        if (boot_cpu_data.x86 > 3)
            new_prot = __pgprot(pgprot_val(old_prot) | _PAGE_PCD);
        return new_prot;
    }

#if !defined (pgprot_noncached)
static inline pgprot_t pgprot_noncached(pgprot_t old_prot)
    {
        pgprot_t new_prot = old_prot;
        if (boot_cpu_data.x86 > 3)
            new_prot = __pgprot(pgprot_val(old_prot) | _PAGE_PCD | _PAGE_PWT);
        return new_prot;
    }
#endif
static inline pgprot_t pgprot_modify_writecombine(pgprot_t old_prot)
    {
        pgprot_t new_prot = old_prot;
        pgprot_val(new_prot) &= ~(_PAGE_PSE | _PAGE_PCD | _PAGE_PWT);
        new_prot = __pgprot(pgprot_val(new_prot) | _PAGE_PWT);
        return new_prot;
    }
#endif /* defined(NVCPU_X86_64) */
#endif /* !defined(NV_VMWARE) */

#if defined(NVCPU_AARCH64)
extern NvBool nvos_is_chipset_io_coherent(void);
/*
 * Don't rely on the kernel's definition of pgprot_noncached(), as on 64-bit
 * ARM that's not for system memory, but device memory instead. For I/O cache
 * coherent systems, use cached mappings instead of uncached.
 */
#define NV_PGPROT_UNCACHED(old_prot)   \
    ((nvos_is_chipset_io_coherent()) ? \
     (old_prot) :                      \
     __pgprot_modify((old_prot), PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_NORMAL_NC)))
#elif defined(NVCPU_PPC64LE)
/* Don't attempt to mark sysmem pages as uncached on ppc64le */
#define NV_PGPROT_UNCACHED(old_prot)          old_prot
#else
#define NV_PGPROT_UNCACHED(old_prot)          pgprot_noncached(old_prot)
#endif

#define NV_PGPROT_UNCACHED_DEVICE(old_prot)     pgprot_noncached(old_prot)
#if defined(NVCPU_AARCH64)
#define NV_PROT_WRITE_COMBINED_DEVICE   (PROT_DEFAULT | PTE_PXN | PTE_UXN |   \
                                         PTE_ATTRINDX(MT_DEVICE_nGnRE))
#define NV_PGPROT_WRITE_COMBINED_DEVICE(old_prot)                             \
    __pgprot_modify(old_prot, PTE_ATTRINDX_MASK, NV_PROT_WRITE_COMBINED_DEVICE)
#define NV_PGPROT_WRITE_COMBINED(old_prot)      NV_PGPROT_UNCACHED(old_prot)
#define NV_PGPROT_READ_ONLY(old_prot)                                         \
            __pgprot_modify(old_prot, 0, PTE_RDONLY)
#elif defined(NVCPU_X86_64)
#define NV_PGPROT_UNCACHED_WEAK(old_prot)       pgprot_noncached_weak(old_prot)
#define NV_PGPROT_WRITE_COMBINED_DEVICE(old_prot)                             \
    pgprot_modify_writecombine(old_prot)
#define NV_PGPROT_WRITE_COMBINED(old_prot)                                    \
    NV_PGPROT_WRITE_COMBINED_DEVICE(old_prot)
#define NV_PGPROT_READ_ONLY(old_prot)                                         \
    __pgprot(pgprot_val((old_prot)) & ~_PAGE_RW)
#elif defined(NVCPU_PPC64LE)
/*
 * Some kernels use H_PAGE instead of _PAGE
 */
#if defined(_PAGE_RW)
#define NV_PAGE_RW _PAGE_RW
#elif defined(H_PAGE_RW)
#define NV_PAGE_RW H_PAGE_RW
#else
#warning "The kernel does not provide page protection defines!"
#endif

#if defined(_PAGE_4K_PFN)
#define NV_PAGE_4K_PFN _PAGE_4K_PFN
#elif defined(H_PAGE_4K_PFN)
#define NV_PAGE_4K_PFN H_PAGE_4K_PFN
#else
#undef NV_PAGE_4K_PFN
#endif

#define NV_PGPROT_WRITE_COMBINED_DEVICE(old_prot)                             \
    pgprot_writecombine(old_prot)
/* Don't attempt to mark sysmem pages as write combined on ppc64le */
#define NV_PGPROT_WRITE_COMBINED(old_prot)    old_prot
#define NV_PGPROT_READ_ONLY(old_prot)                                         \
    __pgprot(pgprot_val((old_prot)) & ~NV_PAGE_RW)
#elif defined(NVCPU_RISCV64)
#define NV_PGPROT_WRITE_COMBINED_DEVICE(old_prot)                             \
    pgprot_writecombine(old_prot)
/* Don't attempt to mark sysmem pages as write combined on riscv */
#define NV_PGPROT_WRITE_COMBINED(old_prot)     old_prot
#define NV_PGPROT_READ_ONLY(old_prot)                                         \
            __pgprot(pgprot_val((old_prot)) & ~_PAGE_WRITE)
#else
/* Writecombine is not supported */
#undef NV_PGPROT_WRITE_COMBINED_DEVICE(old_prot)
#undef NV_PGPROT_WRITE_COMBINED(old_prot)
#define NV_PGPROT_READ_ONLY(old_prot)
#endif

#endif /* __NV_PGPROT_H__ */
