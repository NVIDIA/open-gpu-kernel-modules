/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KCL_KCL_MEMORY_H
#define _KCL_KCL_MEMORY_H

#ifndef HAVE_VMF_INSERT
static inline vm_fault_t vmf_insert_mixed(struct vm_area_struct *vma,
				unsigned long addr,
				pfn_t pfn)
{
	int err;
#if !defined(HAVE_PFN_T_VM_INSERT_MIXED)
	err = vm_insert_mixed(vma, addr, pfn_t_to_pfn(pfn));
#else
	err = vm_insert_mixed(vma, addr, pfn);
#endif
	if (err == -ENOMEM)
		return VM_FAULT_OOM;
	if (err < 0 && err != -EBUSY)
		return VM_FAULT_SIGBUS;

	return VM_FAULT_NOPAGE;
}

static inline vm_fault_t vmf_insert_pfn(struct vm_area_struct *vma,
				unsigned long addr, unsigned long pfn)
{
	int err = vm_insert_pfn(vma, addr, pfn);

	if (err == -ENOMEM)
		return VM_FAULT_OOM;
	if (err < 0 && err != -EBUSY)
		return VM_FAULT_SIGBUS;

	return VM_FAULT_NOPAGE;
}

#endif /* HAVE_VMF_INSERT */

#ifndef HAVE_VMF_INSERT_MIXED_PROT
vm_fault_t _kcl_vmf_insert_mixed_prot(struct vm_area_struct *vma, unsigned long addr,
				      pfn_t pfn, pgprot_t pgprot);
static inline
vm_fault_t vmf_insert_mixed_prot(struct vm_area_struct *vma, unsigned long addr,
			pfn_t pfn, pgprot_t pgprot)
{
	return _kcl_vmf_insert_mixed_prot(vma, addr, pfn, pgprot);
}
#endif /* HAVE_VMF_INSERT_MIXED_PROT */

#ifndef HAVE_VMF_INSERT_PFN_PROT
vm_fault_t _kcl_vmf_insert_pfn_prot(struct vm_area_struct *vma, unsigned long addr,
					unsigned long pfn, pgprot_t pgprot);
static inline
vm_fault_t vmf_insert_pfn_prot(struct vm_area_struct *vma, unsigned long addr,
					unsigned long pfn, pgprot_t pgprot)
{
	return _kcl_vmf_insert_pfn_prot(vma, addr, pfn, pgprot);
}
#endif /* HAVE_VMF_INSERT_PFN_PROT */

#ifndef HAVE_VMF_INSERT_PFN_PMD_PROT
vm_fault_t vmf_insert_pfn_pmd_prot(struct vm_fault *vmf, pfn_t pfn,
				   pgprot_t pgprot, bool write);
vm_fault_t vmf_insert_pfn_pud_prot(struct vm_fault *vmf, pfn_t pfn,
				   pgprot_t pgprot, bool write);
#endif /* HAVE_VMF_INSERT_PFN_PMD_PROT */

#endif
