// SPDX-License-Identifier: GPL-2.0
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/security.h>
#include <asm/cacheflush.h>
#include <asm/machdep.h>
#include <asm/mman.h>
#include <asm/tlb.h>

void radix__flush_hugetlb_page(struct vm_area_struct *vma, unsigned long vmaddr)
{
	int psize;
	struct hstate *hstate = hstate_file(vma->vm_file);

	psize = hstate_get_psize(hstate);
	radix__flush_tlb_page_psize(vma->vm_mm, vmaddr, psize);
}

void radix__local_flush_hugetlb_page(struct vm_area_struct *vma, unsigned long vmaddr)
{
	int psize;
	struct hstate *hstate = hstate_file(vma->vm_file);

	psize = hstate_get_psize(hstate);
	radix__local_flush_tlb_page_psize(vma->vm_mm, vmaddr, psize);
}

void radix__flush_hugetlb_tlb_range(struct vm_area_struct *vma, unsigned long start,
				   unsigned long end)
{
	int psize;
	struct hstate *hstate = hstate_file(vma->vm_file);

	psize = hstate_get_psize(hstate);
	radix__flush_tlb_range_psize(vma->vm_mm, start, end, psize);
}

/*
 * A vairant of hugetlb_get_unmapped_area doing topdown search
 * FIXME!! should we do as x86 does or non hugetlb area does ?
 * ie, use topdown or not based on mmap_is_legacy check ?
 */
unsigned long
radix__hugetlb_get_unmapped_area(struct file *file, unsigned long addr,
				unsigned long len, unsigned long pgoff,
				unsigned long flags)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	struct hstate *h = hstate_file(file);
	int fixed = (flags & MAP_FIXED);
	unsigned long high_limit;
	struct vm_unmapped_area_info info;

	high_limit = DEFAULT_MAP_WINDOW;
	if (addr >= high_limit || (fixed && (addr + len > high_limit)))
		high_limit = TASK_SIZE;

	if (len & ~huge_page_mask(h))
		return -EINVAL;
	if (len > high_limit)
		return -ENOMEM;

	if (fixed) {
		if (addr > high_limit - len)
			return -ENOMEM;
		if (prepare_hugepage_range(file, addr, len))
			return -EINVAL;
		return addr;
	}

	if (addr) {
		addr = ALIGN(addr, huge_page_size(h));
		vma = find_vma(mm, addr);
		if (high_limit - len >= addr && addr >= mmap_min_addr &&
		    (!vma || addr + len <= vm_start_gap(vma)))
			return addr;
	}
	/*
	 * We are always doing an topdown search here. Slice code
	 * does that too.
	 */
	info.flags = VM_UNMAPPED_AREA_TOPDOWN;
	info.length = len;
	info.low_limit = max(PAGE_SIZE, mmap_min_addr);
	info.high_limit = mm->mmap_base + (high_limit - DEFAULT_MAP_WINDOW);
	info.align_mask = PAGE_MASK & ~huge_page_mask(h);
	info.align_offset = 0;

	return vm_unmapped_area(&info);
}

void radix__huge_ptep_modify_prot_commit(struct vm_area_struct *vma,
					 unsigned long addr, pte_t *ptep,
					 pte_t old_pte, pte_t pte)
{
	struct mm_struct *mm = vma->vm_mm;

	/*
	 * To avoid NMMU hang while relaxing access we need to flush the tlb before
	 * we set the new value.
	 */
	if (is_pte_rw_upgrade(pte_val(old_pte), pte_val(pte)) &&
	    (atomic_read(&mm->context.copros) > 0))
		radix__flush_hugetlb_page(vma, addr);

	set_huge_pte_at(vma->vm_mm, addr, ptep, pte);
}
