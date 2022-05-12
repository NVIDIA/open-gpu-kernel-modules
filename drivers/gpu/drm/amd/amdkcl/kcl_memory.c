// SPDX-License-Identifier: GPL-2.0 OR MIT
/**************************************************************************
 *
 * Copyright (c) 2006-2009 VMware, Inc., Palo Alto, CA., USA
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/
/*
 * Authors: Thomas Hellstrom <thellstrom-at-vmware-dot-com>
 */
#include <kcl/kcl_mm.h>

/* Copied from drivers/gpu/drm/ttm/ttm_bo_vm.c and modified for KCL */
#ifndef HAVE_VMF_INSERT_MIXED_PROT
vm_fault_t _kcl_vmf_insert_mixed_prot(struct vm_area_struct *vma, unsigned long addr,
			pfn_t pfn, pgprot_t pgprot)
{
	struct vm_area_struct cvma = *vma;

	cvma.vm_page_prot = pgprot;

	return vmf_insert_mixed(&cvma, addr, pfn);
}
EXPORT_SYMBOL(_kcl_vmf_insert_mixed_prot);
#endif

#ifndef HAVE_VMF_INSERT_PFN_PROT
#ifndef HAVE_VM_INSERT_PFN_PROT
int vm_insert_pfn_prot(struct vm_area_struct *vma, unsigned long addr,
                        unsigned long pfn, pgprot_t pgprot)
{
	struct vm_area_struct cvma = *vma;

	cvma.vm_page_prot = pgprot;

	return vm_insert_pfn(&cvma, addr, pfn);
}
#endif

vm_fault_t _kcl_vmf_insert_pfn_prot(struct vm_area_struct *vma, unsigned long addr,
		unsigned long pfn, pgprot_t pgprot)
{
	int err = vm_insert_pfn_prot(vma, addr, pfn, pgprot);

	if (err == -ENOMEM)
		return VM_FAULT_OOM;
	if (err < 0 && err != -EBUSY)
		return VM_FAULT_SIGBUS;

	return VM_FAULT_NOPAGE;
}
EXPORT_SYMBOL(_kcl_vmf_insert_pfn_prot);
#endif

#ifndef HAVE_VMF_INSERT_PFN_PMD_PROT
vm_fault_t vmf_insert_pfn_pmd_prot(struct vm_fault *vmf, pfn_t pfn,
				   pgprot_t pgprot, bool write)
{
#if !defined(HAVE_VM_FAULT_ADDRESS_VMA)
	pr_warn_once("%s is not available\n", __func__);
	return (vm_fault_t)0;
#else
	struct vm_fault cvmf = *vmf;
	struct vm_area_struct cvma = *vmf->vma;

	cvmf.vma = &cvma;
	cvma.vm_page_prot = pgprot;
#if defined(HAVE_VMF_INSERT_PFN_PMD_3ARGS)
	return vmf_insert_pfn_pmd(&cvmf, pfn, write);
#elif defined(HAVE_VM_FAULT_ADDRESS_VMA)
	return vmf_insert_pfn_pmd(&cvma, cvmf.address & PMD_MASK, cvmf.pmd, pfn, write);
#endif /* HAVE_VMF_INSERT_PFN_PMD_3ARGS */
#endif /* HAVE_VM_FAULT_ADDRESS_VMA */
}
EXPORT_SYMBOL_GPL(vmf_insert_pfn_pmd_prot);

#ifdef CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD
vm_fault_t vmf_insert_pfn_pud_prot(struct vm_fault *vmf, pfn_t pfn,
				   pgprot_t pgprot, bool write)
{
#if defined(HAVE_VMF_INSERT_PFN_PMD_3ARGS)
	struct vm_fault cvmf = *vmf;
	struct vm_area_struct cvma = *vmf->vma;

	cvmf.vma = &cvma;
	cvma.vm_page_prot = pgprot;

	return vmf_insert_pfn_pud(&cvmf, pfn, write);
#elif defined(HAVE_VMF_INSERT_PFN_PUD)
	struct vm_fault cvmf = *vmf;
	struct vm_area_struct cvma = *vmf->vma;
#ifdef HAVE_VM_FAULT_ADDRESS_VMA
	unsigned long addr = vmf->address & PUD_MASK;
#else
	unsigned long addr = (unsigned long)vmf->virtual_address & PUD_MASK;
#endif

	cvmf.vma = &cvma;
	cvma.vm_page_prot = pgprot;

	return vmf_insert_pfn_pud(&cvma, addr, cvmf.pud, pfn, write);
#else
	pr_warn_once("% is not available\n", __func__);
	return (vm_fault_t)0;
#endif
}
EXPORT_SYMBOL_GPL(vmf_insert_pfn_pud_prot);
#endif /* CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD */

#endif /* HAVE_VMF_INSERT_PFN_PMD_PROT */
