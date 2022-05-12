// SPDX-License-Identifier: GPL-2.0-only
/*
 *	fs/proc/vmcore.c Interface for accessing the crash
 * 				 dump from the system's previous life.
 * 	Heavily borrowed from fs/proc/kcore.c
 *	Created by: Hariprasad Nellitheertha (hari@in.ibm.com)
 *	Copyright (C) IBM Corporation, 2004. All rights reserved
 *
 */

#include <linux/mm.h>
#include <linux/kcore.h>
#include <linux/user.h>
#include <linux/elf.h>
#include <linux/elfcore.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/highmem.h>
#include <linux/printk.h>
#include <linux/memblock.h>
#include <linux/init.h>
#include <linux/crash_dump.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/vmalloc.h>
#include <linux/pagemap.h>
#include <linux/uaccess.h>
#include <linux/mem_encrypt.h>
#include <asm/io.h>
#include "internal.h"

/* List representing chunks of contiguous memory areas and their offsets in
 * vmcore file.
 */
static LIST_HEAD(vmcore_list);

/* Stores the pointer to the buffer containing kernel elf core headers. */
static char *elfcorebuf;
static size_t elfcorebuf_sz;
static size_t elfcorebuf_sz_orig;

static char *elfnotes_buf;
static size_t elfnotes_sz;
/* Size of all notes minus the device dump notes */
static size_t elfnotes_orig_sz;

/* Total size of vmcore file. */
static u64 vmcore_size;

static struct proc_dir_entry *proc_vmcore;

#ifdef CONFIG_PROC_VMCORE_DEVICE_DUMP
/* Device Dump list and mutex to synchronize access to list */
static LIST_HEAD(vmcoredd_list);
static DEFINE_MUTEX(vmcoredd_mutex);

static bool vmcoredd_disabled;
core_param(novmcoredd, vmcoredd_disabled, bool, 0);
#endif /* CONFIG_PROC_VMCORE_DEVICE_DUMP */

/* Device Dump Size */
static size_t vmcoredd_orig_sz;

/*
 * Returns > 0 for RAM pages, 0 for non-RAM pages, < 0 on error
 * The called function has to take care of module refcounting.
 */
static int (*oldmem_pfn_is_ram)(unsigned long pfn);

int register_oldmem_pfn_is_ram(int (*fn)(unsigned long pfn))
{
	if (oldmem_pfn_is_ram)
		return -EBUSY;
	oldmem_pfn_is_ram = fn;
	return 0;
}
EXPORT_SYMBOL_GPL(register_oldmem_pfn_is_ram);

void unregister_oldmem_pfn_is_ram(void)
{
	oldmem_pfn_is_ram = NULL;
	wmb();
}
EXPORT_SYMBOL_GPL(unregister_oldmem_pfn_is_ram);

static int pfn_is_ram(unsigned long pfn)
{
	int (*fn)(unsigned long pfn);
	/* pfn is ram unless fn() checks pagetype */
	int ret = 1;

	/*
	 * Ask hypervisor if the pfn is really ram.
	 * A ballooned page contains no data and reading from such a page
	 * will cause high load in the hypervisor.
	 */
	fn = oldmem_pfn_is_ram;
	if (fn)
		ret = fn(pfn);

	return ret;
}

/* Reads a page from the oldmem device from given offset. */
ssize_t read_from_oldmem(char *buf, size_t count,
			 u64 *ppos, int userbuf,
			 bool encrypted)
{
	unsigned long pfn, offset;
	size_t nr_bytes;
	ssize_t read = 0, tmp;

	if (!count)
		return 0;

	offset = (unsigned long)(*ppos % PAGE_SIZE);
	pfn = (unsigned long)(*ppos / PAGE_SIZE);

	do {
		if (count > (PAGE_SIZE - offset))
			nr_bytes = PAGE_SIZE - offset;
		else
			nr_bytes = count;

		/* If pfn is not ram, return zeros for sparse dump files */
		if (pfn_is_ram(pfn) == 0)
			memset(buf, 0, nr_bytes);
		else {
			if (encrypted)
				tmp = copy_oldmem_page_encrypted(pfn, buf,
								 nr_bytes,
								 offset,
								 userbuf);
			else
				tmp = copy_oldmem_page(pfn, buf, nr_bytes,
						       offset, userbuf);

			if (tmp < 0)
				return tmp;
		}
		*ppos += nr_bytes;
		count -= nr_bytes;
		buf += nr_bytes;
		read += nr_bytes;
		++pfn;
		offset = 0;
	} while (count);

	return read;
}

/*
 * Architectures may override this function to allocate ELF header in 2nd kernel
 */
int __weak elfcorehdr_alloc(unsigned long long *addr, unsigned long long *size)
{
	return 0;
}

/*
 * Architectures may override this function to free header
 */
void __weak elfcorehdr_free(unsigned long long addr)
{}

/*
 * Architectures may override this function to read from ELF header
 */
ssize_t __weak elfcorehdr_read(char *buf, size_t count, u64 *ppos)
{
	return read_from_oldmem(buf, count, ppos, 0, false);
}

/*
 * Architectures may override this function to read from notes sections
 */
ssize_t __weak elfcorehdr_read_notes(char *buf, size_t count, u64 *ppos)
{
	return read_from_oldmem(buf, count, ppos, 0, mem_encrypt_active());
}

/*
 * Architectures may override this function to map oldmem
 */
int __weak remap_oldmem_pfn_range(struct vm_area_struct *vma,
				  unsigned long from, unsigned long pfn,
				  unsigned long size, pgprot_t prot)
{
	prot = pgprot_encrypted(prot);
	return remap_pfn_range(vma, from, pfn, size, prot);
}

/*
 * Architectures which support memory encryption override this.
 */
ssize_t __weak
copy_oldmem_page_encrypted(unsigned long pfn, char *buf, size_t csize,
			   unsigned long offset, int userbuf)
{
	return copy_oldmem_page(pfn, buf, csize, offset, userbuf);
}

/*
 * Copy to either kernel or user space
 */
static int copy_to(void *target, void *src, size_t size, int userbuf)
{
	if (userbuf) {
		if (copy_to_user((char __user *) target, src, size))
			return -EFAULT;
	} else {
		memcpy(target, src, size);
	}
	return 0;
}

#ifdef CONFIG_PROC_VMCORE_DEVICE_DUMP
static int vmcoredd_copy_dumps(void *dst, u64 start, size_t size, int userbuf)
{
	struct vmcoredd_node *dump;
	u64 offset = 0;
	int ret = 0;
	size_t tsz;
	char *buf;

	mutex_lock(&vmcoredd_mutex);
	list_for_each_entry(dump, &vmcoredd_list, list) {
		if (start < offset + dump->size) {
			tsz = min(offset + (u64)dump->size - start, (u64)size);
			buf = dump->buf + start - offset;
			if (copy_to(dst, buf, tsz, userbuf)) {
				ret = -EFAULT;
				goto out_unlock;
			}

			size -= tsz;
			start += tsz;
			dst += tsz;

			/* Leave now if buffer filled already */
			if (!size)
				goto out_unlock;
		}
		offset += dump->size;
	}

out_unlock:
	mutex_unlock(&vmcoredd_mutex);
	return ret;
}

#ifdef CONFIG_MMU
static int vmcoredd_mmap_dumps(struct vm_area_struct *vma, unsigned long dst,
			       u64 start, size_t size)
{
	struct vmcoredd_node *dump;
	u64 offset = 0;
	int ret = 0;
	size_t tsz;
	char *buf;

	mutex_lock(&vmcoredd_mutex);
	list_for_each_entry(dump, &vmcoredd_list, list) {
		if (start < offset + dump->size) {
			tsz = min(offset + (u64)dump->size - start, (u64)size);
			buf = dump->buf + start - offset;
			if (remap_vmalloc_range_partial(vma, dst, buf, 0,
							tsz)) {
				ret = -EFAULT;
				goto out_unlock;
			}

			size -= tsz;
			start += tsz;
			dst += tsz;

			/* Leave now if buffer filled already */
			if (!size)
				goto out_unlock;
		}
		offset += dump->size;
	}

out_unlock:
	mutex_unlock(&vmcoredd_mutex);
	return ret;
}
#endif /* CONFIG_MMU */
#endif /* CONFIG_PROC_VMCORE_DEVICE_DUMP */

/* Read from the ELF header and then the crash dump. On error, negative value is
 * returned otherwise number of bytes read are returned.
 */
static ssize_t __read_vmcore(char *buffer, size_t buflen, loff_t *fpos,
			     int userbuf)
{
	ssize_t acc = 0, tmp;
	size_t tsz;
	u64 start;
	struct vmcore *m = NULL;

	if (buflen == 0 || *fpos >= vmcore_size)
		return 0;

	/* trim buflen to not go beyond EOF */
	if (buflen > vmcore_size - *fpos)
		buflen = vmcore_size - *fpos;

	/* Read ELF core header */
	if (*fpos < elfcorebuf_sz) {
		tsz = min(elfcorebuf_sz - (size_t)*fpos, buflen);
		if (copy_to(buffer, elfcorebuf + *fpos, tsz, userbuf))
			return -EFAULT;
		buflen -= tsz;
		*fpos += tsz;
		buffer += tsz;
		acc += tsz;

		/* leave now if filled buffer already */
		if (buflen == 0)
			return acc;
	}

	/* Read Elf note segment */
	if (*fpos < elfcorebuf_sz + elfnotes_sz) {
		void *kaddr;

		/* We add device dumps before other elf notes because the
		 * other elf notes may not fill the elf notes buffer
		 * completely and we will end up with zero-filled data
		 * between the elf notes and the device dumps. Tools will
		 * then try to decode this zero-filled data as valid notes
		 * and we don't want that. Hence, adding device dumps before
		 * the other elf notes ensure that zero-filled data can be
		 * avoided.
		 */
#ifdef CONFIG_PROC_VMCORE_DEVICE_DUMP
		/* Read device dumps */
		if (*fpos < elfcorebuf_sz + vmcoredd_orig_sz) {
			tsz = min(elfcorebuf_sz + vmcoredd_orig_sz -
				  (size_t)*fpos, buflen);
			start = *fpos - elfcorebuf_sz;
			if (vmcoredd_copy_dumps(buffer, start, tsz, userbuf))
				return -EFAULT;

			buflen -= tsz;
			*fpos += tsz;
			buffer += tsz;
			acc += tsz;

			/* leave now if filled buffer already */
			if (!buflen)
				return acc;
		}
#endif /* CONFIG_PROC_VMCORE_DEVICE_DUMP */

		/* Read remaining elf notes */
		tsz = min(elfcorebuf_sz + elfnotes_sz - (size_t)*fpos, buflen);
		kaddr = elfnotes_buf + *fpos - elfcorebuf_sz - vmcoredd_orig_sz;
		if (copy_to(buffer, kaddr, tsz, userbuf))
			return -EFAULT;

		buflen -= tsz;
		*fpos += tsz;
		buffer += tsz;
		acc += tsz;

		/* leave now if filled buffer already */
		if (buflen == 0)
			return acc;
	}

	list_for_each_entry(m, &vmcore_list, list) {
		if (*fpos < m->offset + m->size) {
			tsz = (size_t)min_t(unsigned long long,
					    m->offset + m->size - *fpos,
					    buflen);
			start = m->paddr + *fpos - m->offset;
			tmp = read_from_oldmem(buffer, tsz, &start,
					       userbuf, mem_encrypt_active());
			if (tmp < 0)
				return tmp;
			buflen -= tsz;
			*fpos += tsz;
			buffer += tsz;
			acc += tsz;

			/* leave now if filled buffer already */
			if (buflen == 0)
				return acc;
		}
	}

	return acc;
}

static ssize_t read_vmcore(struct file *file, char __user *buffer,
			   size_t buflen, loff_t *fpos)
{
	return __read_vmcore((__force char *) buffer, buflen, fpos, 1);
}

/*
 * The vmcore fault handler uses the page cache and fills data using the
 * standard __vmcore_read() function.
 *
 * On s390 the fault handler is used for memory regions that can't be mapped
 * directly with remap_pfn_range().
 */
static vm_fault_t mmap_vmcore_fault(struct vm_fault *vmf)
{
#ifdef CONFIG_S390
	struct address_space *mapping = vmf->vma->vm_file->f_mapping;
	pgoff_t index = vmf->pgoff;
	struct page *page;
	loff_t offset;
	char *buf;
	int rc;

	page = find_or_create_page(mapping, index, GFP_KERNEL);
	if (!page)
		return VM_FAULT_OOM;
	if (!PageUptodate(page)) {
		offset = (loff_t) index << PAGE_SHIFT;
		buf = __va((page_to_pfn(page) << PAGE_SHIFT));
		rc = __read_vmcore(buf, PAGE_SIZE, &offset, 0);
		if (rc < 0) {
			unlock_page(page);
			put_page(page);
			return vmf_error(rc);
		}
		SetPageUptodate(page);
	}
	unlock_page(page);
	vmf->page = page;
	return 0;
#else
	return VM_FAULT_SIGBUS;
#endif
}

static const struct vm_operations_struct vmcore_mmap_ops = {
	.fault = mmap_vmcore_fault,
};

/**
 * vmcore_alloc_buf - allocate buffer in vmalloc memory
 * @sizez: size of buffer
 *
 * If CONFIG_MMU is defined, use vmalloc_user() to allow users to mmap
 * the buffer to user-space by means of remap_vmalloc_range().
 *
 * If CONFIG_MMU is not defined, use vzalloc() since mmap_vmcore() is
 * disabled and there's no need to allow users to mmap the buffer.
 */
static inline char *vmcore_alloc_buf(size_t size)
{
#ifdef CONFIG_MMU
	return vmalloc_user(size);
#else
	return vzalloc(size);
#endif
}

/*
 * Disable mmap_vmcore() if CONFIG_MMU is not defined. MMU is
 * essential for mmap_vmcore() in order to map physically
 * non-contiguous objects (ELF header, ELF note segment and memory
 * regions in the 1st kernel pointed to by PT_LOAD entries) into
 * virtually contiguous user-space in ELF layout.
 */
#ifdef CONFIG_MMU
/*
 * remap_oldmem_pfn_checked - do remap_oldmem_pfn_range replacing all pages
 * reported as not being ram with the zero page.
 *
 * @vma: vm_area_struct describing requested mapping
 * @from: start remapping from
 * @pfn: page frame number to start remapping to
 * @size: remapping size
 * @prot: protection bits
 *
 * Returns zero on success, -EAGAIN on failure.
 */
static int remap_oldmem_pfn_checked(struct vm_area_struct *vma,
				    unsigned long from, unsigned long pfn,
				    unsigned long size, pgprot_t prot)
{
	unsigned long map_size;
	unsigned long pos_start, pos_end, pos;
	unsigned long zeropage_pfn = my_zero_pfn(0);
	size_t len = 0;

	pos_start = pfn;
	pos_end = pfn + (size >> PAGE_SHIFT);

	for (pos = pos_start; pos < pos_end; ++pos) {
		if (!pfn_is_ram(pos)) {
			/*
			 * We hit a page which is not ram. Remap the continuous
			 * region between pos_start and pos-1 and replace
			 * the non-ram page at pos with the zero page.
			 */
			if (pos > pos_start) {
				/* Remap continuous region */
				map_size = (pos - pos_start) << PAGE_SHIFT;
				if (remap_oldmem_pfn_range(vma, from + len,
							   pos_start, map_size,
							   prot))
					goto fail;
				len += map_size;
			}
			/* Remap the zero page */
			if (remap_oldmem_pfn_range(vma, from + len,
						   zeropage_pfn,
						   PAGE_SIZE, prot))
				goto fail;
			len += PAGE_SIZE;
			pos_start = pos + 1;
		}
	}
	if (pos > pos_start) {
		/* Remap the rest */
		map_size = (pos - pos_start) << PAGE_SHIFT;
		if (remap_oldmem_pfn_range(vma, from + len, pos_start,
					   map_size, prot))
			goto fail;
	}
	return 0;
fail:
	do_munmap(vma->vm_mm, from, len, NULL);
	return -EAGAIN;
}

static int vmcore_remap_oldmem_pfn(struct vm_area_struct *vma,
			    unsigned long from, unsigned long pfn,
			    unsigned long size, pgprot_t prot)
{
	/*
	 * Check if oldmem_pfn_is_ram was registered to avoid
	 * looping over all pages without a reason.
	 */
	if (oldmem_pfn_is_ram)
		return remap_oldmem_pfn_checked(vma, from, pfn, size, prot);
	else
		return remap_oldmem_pfn_range(vma, from, pfn, size, prot);
}

static int mmap_vmcore(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;
	u64 start, end, len, tsz;
	struct vmcore *m;

	start = (u64)vma->vm_pgoff << PAGE_SHIFT;
	end = start + size;

	if (size > vmcore_size || end > vmcore_size)
		return -EINVAL;

	if (vma->vm_flags & (VM_WRITE | VM_EXEC))
		return -EPERM;

	vma->vm_flags &= ~(VM_MAYWRITE | VM_MAYEXEC);
	vma->vm_flags |= VM_MIXEDMAP;
	vma->vm_ops = &vmcore_mmap_ops;

	len = 0;

	if (start < elfcorebuf_sz) {
		u64 pfn;

		tsz = min(elfcorebuf_sz - (size_t)start, size);
		pfn = __pa(elfcorebuf + start) >> PAGE_SHIFT;
		if (remap_pfn_range(vma, vma->vm_start, pfn, tsz,
				    vma->vm_page_prot))
			return -EAGAIN;
		size -= tsz;
		start += tsz;
		len += tsz;

		if (size == 0)
			return 0;
	}

	if (start < elfcorebuf_sz + elfnotes_sz) {
		void *kaddr;

		/* We add device dumps before other elf notes because the
		 * other elf notes may not fill the elf notes buffer
		 * completely and we will end up with zero-filled data
		 * between the elf notes and the device dumps. Tools will
		 * then try to decode this zero-filled data as valid notes
		 * and we don't want that. Hence, adding device dumps before
		 * the other elf notes ensure that zero-filled data can be
		 * avoided. This also ensures that the device dumps and
		 * other elf notes can be properly mmaped at page aligned
		 * address.
		 */
#ifdef CONFIG_PROC_VMCORE_DEVICE_DUMP
		/* Read device dumps */
		if (start < elfcorebuf_sz + vmcoredd_orig_sz) {
			u64 start_off;

			tsz = min(elfcorebuf_sz + vmcoredd_orig_sz -
				  (size_t)start, size);
			start_off = start - elfcorebuf_sz;
			if (vmcoredd_mmap_dumps(vma, vma->vm_start + len,
						start_off, tsz))
				goto fail;

			size -= tsz;
			start += tsz;
			len += tsz;

			/* leave now if filled buffer already */
			if (!size)
				return 0;
		}
#endif /* CONFIG_PROC_VMCORE_DEVICE_DUMP */

		/* Read remaining elf notes */
		tsz = min(elfcorebuf_sz + elfnotes_sz - (size_t)start, size);
		kaddr = elfnotes_buf + start - elfcorebuf_sz - vmcoredd_orig_sz;
		if (remap_vmalloc_range_partial(vma, vma->vm_start + len,
						kaddr, 0, tsz))
			goto fail;

		size -= tsz;
		start += tsz;
		len += tsz;

		if (size == 0)
			return 0;
	}

	list_for_each_entry(m, &vmcore_list, list) {
		if (start < m->offset + m->size) {
			u64 paddr = 0;

			tsz = (size_t)min_t(unsigned long long,
					    m->offset + m->size - start, size);
			paddr = m->paddr + start - m->offset;
			if (vmcore_remap_oldmem_pfn(vma, vma->vm_start + len,
						    paddr >> PAGE_SHIFT, tsz,
						    vma->vm_page_prot))
				goto fail;
			size -= tsz;
			start += tsz;
			len += tsz;

			if (size == 0)
				return 0;
		}
	}

	return 0;
fail:
	do_munmap(vma->vm_mm, vma->vm_start, len, NULL);
	return -EAGAIN;
}
#else
static int mmap_vmcore(struct file *file, struct vm_area_struct *vma)
{
	return -ENOSYS;
}
#endif

static const struct proc_ops vmcore_proc_ops = {
	.proc_read	= read_vmcore,
	.proc_lseek	= default_llseek,
	.proc_mmap	= mmap_vmcore,
};

static struct vmcore* __init get_new_element(void)
{
	return kzalloc(sizeof(struct vmcore), GFP_KERNEL);
}

static u64 get_vmcore_size(size_t elfsz, size_t elfnotesegsz,
			   struct list_head *vc_list)
{
	u64 size;
	struct vmcore *m;

	size = elfsz + elfnotesegsz;
	list_for_each_entry(m, vc_list, list) {
		size += m->size;
	}
	return size;
}

/**
 * update_note_header_size_elf64 - update p_memsz member of each PT_NOTE entry
 *
 * @ehdr_ptr: ELF header
 *
 * This function updates p_memsz member of each PT_NOTE entry in the
 * program header table pointed to by @ehdr_ptr to real size of ELF
 * note segment.
 */
static int __init update_note_header_size_elf64(const Elf64_Ehdr *ehdr_ptr)
{
	int i, rc=0;
	Elf64_Phdr *phdr_ptr;
	Elf64_Nhdr *nhdr_ptr;

	phdr_ptr = (Elf64_Phdr *)(ehdr_ptr + 1);
	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		void *notes_section;
		u64 offset, max_sz, sz, real_sz = 0;
		if (phdr_ptr->p_type != PT_NOTE)
			continue;
		max_sz = phdr_ptr->p_memsz;
		offset = phdr_ptr->p_offset;
		notes_section = kmalloc(max_sz, GFP_KERNEL);
		if (!notes_section)
			return -ENOMEM;
		rc = elfcorehdr_read_notes(notes_section, max_sz, &offset);
		if (rc < 0) {
			kfree(notes_section);
			return rc;
		}
		nhdr_ptr = notes_section;
		while (nhdr_ptr->n_namesz != 0) {
			sz = sizeof(Elf64_Nhdr) +
				(((u64)nhdr_ptr->n_namesz + 3) & ~3) +
				(((u64)nhdr_ptr->n_descsz + 3) & ~3);
			if ((real_sz + sz) > max_sz) {
				pr_warn("Warning: Exceeded p_memsz, dropping PT_NOTE entry n_namesz=0x%x, n_descsz=0x%x\n",
					nhdr_ptr->n_namesz, nhdr_ptr->n_descsz);
				break;
			}
			real_sz += sz;
			nhdr_ptr = (Elf64_Nhdr*)((char*)nhdr_ptr + sz);
		}
		kfree(notes_section);
		phdr_ptr->p_memsz = real_sz;
		if (real_sz == 0) {
			pr_warn("Warning: Zero PT_NOTE entries found\n");
		}
	}

	return 0;
}

/**
 * get_note_number_and_size_elf64 - get the number of PT_NOTE program
 * headers and sum of real size of their ELF note segment headers and
 * data.
 *
 * @ehdr_ptr: ELF header
 * @nr_ptnote: buffer for the number of PT_NOTE program headers
 * @sz_ptnote: buffer for size of unique PT_NOTE program header
 *
 * This function is used to merge multiple PT_NOTE program headers
 * into a unique single one. The resulting unique entry will have
 * @sz_ptnote in its phdr->p_mem.
 *
 * It is assumed that program headers with PT_NOTE type pointed to by
 * @ehdr_ptr has already been updated by update_note_header_size_elf64
 * and each of PT_NOTE program headers has actual ELF note segment
 * size in its p_memsz member.
 */
static int __init get_note_number_and_size_elf64(const Elf64_Ehdr *ehdr_ptr,
						 int *nr_ptnote, u64 *sz_ptnote)
{
	int i;
	Elf64_Phdr *phdr_ptr;

	*nr_ptnote = *sz_ptnote = 0;

	phdr_ptr = (Elf64_Phdr *)(ehdr_ptr + 1);
	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		if (phdr_ptr->p_type != PT_NOTE)
			continue;
		*nr_ptnote += 1;
		*sz_ptnote += phdr_ptr->p_memsz;
	}

	return 0;
}

/**
 * copy_notes_elf64 - copy ELF note segments in a given buffer
 *
 * @ehdr_ptr: ELF header
 * @notes_buf: buffer into which ELF note segments are copied
 *
 * This function is used to copy ELF note segment in the 1st kernel
 * into the buffer @notes_buf in the 2nd kernel. It is assumed that
 * size of the buffer @notes_buf is equal to or larger than sum of the
 * real ELF note segment headers and data.
 *
 * It is assumed that program headers with PT_NOTE type pointed to by
 * @ehdr_ptr has already been updated by update_note_header_size_elf64
 * and each of PT_NOTE program headers has actual ELF note segment
 * size in its p_memsz member.
 */
static int __init copy_notes_elf64(const Elf64_Ehdr *ehdr_ptr, char *notes_buf)
{
	int i, rc=0;
	Elf64_Phdr *phdr_ptr;

	phdr_ptr = (Elf64_Phdr*)(ehdr_ptr + 1);

	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		u64 offset;
		if (phdr_ptr->p_type != PT_NOTE)
			continue;
		offset = phdr_ptr->p_offset;
		rc = elfcorehdr_read_notes(notes_buf, phdr_ptr->p_memsz,
					   &offset);
		if (rc < 0)
			return rc;
		notes_buf += phdr_ptr->p_memsz;
	}

	return 0;
}

/* Merges all the PT_NOTE headers into one. */
static int __init merge_note_headers_elf64(char *elfptr, size_t *elfsz,
					   char **notes_buf, size_t *notes_sz)
{
	int i, nr_ptnote=0, rc=0;
	char *tmp;
	Elf64_Ehdr *ehdr_ptr;
	Elf64_Phdr phdr;
	u64 phdr_sz = 0, note_off;

	ehdr_ptr = (Elf64_Ehdr *)elfptr;

	rc = update_note_header_size_elf64(ehdr_ptr);
	if (rc < 0)
		return rc;

	rc = get_note_number_and_size_elf64(ehdr_ptr, &nr_ptnote, &phdr_sz);
	if (rc < 0)
		return rc;

	*notes_sz = roundup(phdr_sz, PAGE_SIZE);
	*notes_buf = vmcore_alloc_buf(*notes_sz);
	if (!*notes_buf)
		return -ENOMEM;

	rc = copy_notes_elf64(ehdr_ptr, *notes_buf);
	if (rc < 0)
		return rc;

	/* Prepare merged PT_NOTE program header. */
	phdr.p_type    = PT_NOTE;
	phdr.p_flags   = 0;
	note_off = sizeof(Elf64_Ehdr) +
			(ehdr_ptr->e_phnum - nr_ptnote +1) * sizeof(Elf64_Phdr);
	phdr.p_offset  = roundup(note_off, PAGE_SIZE);
	phdr.p_vaddr   = phdr.p_paddr = 0;
	phdr.p_filesz  = phdr.p_memsz = phdr_sz;
	phdr.p_align   = 0;

	/* Add merged PT_NOTE program header*/
	tmp = elfptr + sizeof(Elf64_Ehdr);
	memcpy(tmp, &phdr, sizeof(phdr));
	tmp += sizeof(phdr);

	/* Remove unwanted PT_NOTE program headers. */
	i = (nr_ptnote - 1) * sizeof(Elf64_Phdr);
	*elfsz = *elfsz - i;
	memmove(tmp, tmp+i, ((*elfsz)-sizeof(Elf64_Ehdr)-sizeof(Elf64_Phdr)));
	memset(elfptr + *elfsz, 0, i);
	*elfsz = roundup(*elfsz, PAGE_SIZE);

	/* Modify e_phnum to reflect merged headers. */
	ehdr_ptr->e_phnum = ehdr_ptr->e_phnum - nr_ptnote + 1;

	/* Store the size of all notes.  We need this to update the note
	 * header when the device dumps will be added.
	 */
	elfnotes_orig_sz = phdr.p_memsz;

	return 0;
}

/**
 * update_note_header_size_elf32 - update p_memsz member of each PT_NOTE entry
 *
 * @ehdr_ptr: ELF header
 *
 * This function updates p_memsz member of each PT_NOTE entry in the
 * program header table pointed to by @ehdr_ptr to real size of ELF
 * note segment.
 */
static int __init update_note_header_size_elf32(const Elf32_Ehdr *ehdr_ptr)
{
	int i, rc=0;
	Elf32_Phdr *phdr_ptr;
	Elf32_Nhdr *nhdr_ptr;

	phdr_ptr = (Elf32_Phdr *)(ehdr_ptr + 1);
	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		void *notes_section;
		u64 offset, max_sz, sz, real_sz = 0;
		if (phdr_ptr->p_type != PT_NOTE)
			continue;
		max_sz = phdr_ptr->p_memsz;
		offset = phdr_ptr->p_offset;
		notes_section = kmalloc(max_sz, GFP_KERNEL);
		if (!notes_section)
			return -ENOMEM;
		rc = elfcorehdr_read_notes(notes_section, max_sz, &offset);
		if (rc < 0) {
			kfree(notes_section);
			return rc;
		}
		nhdr_ptr = notes_section;
		while (nhdr_ptr->n_namesz != 0) {
			sz = sizeof(Elf32_Nhdr) +
				(((u64)nhdr_ptr->n_namesz + 3) & ~3) +
				(((u64)nhdr_ptr->n_descsz + 3) & ~3);
			if ((real_sz + sz) > max_sz) {
				pr_warn("Warning: Exceeded p_memsz, dropping PT_NOTE entry n_namesz=0x%x, n_descsz=0x%x\n",
					nhdr_ptr->n_namesz, nhdr_ptr->n_descsz);
				break;
			}
			real_sz += sz;
			nhdr_ptr = (Elf32_Nhdr*)((char*)nhdr_ptr + sz);
		}
		kfree(notes_section);
		phdr_ptr->p_memsz = real_sz;
		if (real_sz == 0) {
			pr_warn("Warning: Zero PT_NOTE entries found\n");
		}
	}

	return 0;
}

/**
 * get_note_number_and_size_elf32 - get the number of PT_NOTE program
 * headers and sum of real size of their ELF note segment headers and
 * data.
 *
 * @ehdr_ptr: ELF header
 * @nr_ptnote: buffer for the number of PT_NOTE program headers
 * @sz_ptnote: buffer for size of unique PT_NOTE program header
 *
 * This function is used to merge multiple PT_NOTE program headers
 * into a unique single one. The resulting unique entry will have
 * @sz_ptnote in its phdr->p_mem.
 *
 * It is assumed that program headers with PT_NOTE type pointed to by
 * @ehdr_ptr has already been updated by update_note_header_size_elf32
 * and each of PT_NOTE program headers has actual ELF note segment
 * size in its p_memsz member.
 */
static int __init get_note_number_and_size_elf32(const Elf32_Ehdr *ehdr_ptr,
						 int *nr_ptnote, u64 *sz_ptnote)
{
	int i;
	Elf32_Phdr *phdr_ptr;

	*nr_ptnote = *sz_ptnote = 0;

	phdr_ptr = (Elf32_Phdr *)(ehdr_ptr + 1);
	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		if (phdr_ptr->p_type != PT_NOTE)
			continue;
		*nr_ptnote += 1;
		*sz_ptnote += phdr_ptr->p_memsz;
	}

	return 0;
}

/**
 * copy_notes_elf32 - copy ELF note segments in a given buffer
 *
 * @ehdr_ptr: ELF header
 * @notes_buf: buffer into which ELF note segments are copied
 *
 * This function is used to copy ELF note segment in the 1st kernel
 * into the buffer @notes_buf in the 2nd kernel. It is assumed that
 * size of the buffer @notes_buf is equal to or larger than sum of the
 * real ELF note segment headers and data.
 *
 * It is assumed that program headers with PT_NOTE type pointed to by
 * @ehdr_ptr has already been updated by update_note_header_size_elf32
 * and each of PT_NOTE program headers has actual ELF note segment
 * size in its p_memsz member.
 */
static int __init copy_notes_elf32(const Elf32_Ehdr *ehdr_ptr, char *notes_buf)
{
	int i, rc=0;
	Elf32_Phdr *phdr_ptr;

	phdr_ptr = (Elf32_Phdr*)(ehdr_ptr + 1);

	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		u64 offset;
		if (phdr_ptr->p_type != PT_NOTE)
			continue;
		offset = phdr_ptr->p_offset;
		rc = elfcorehdr_read_notes(notes_buf, phdr_ptr->p_memsz,
					   &offset);
		if (rc < 0)
			return rc;
		notes_buf += phdr_ptr->p_memsz;
	}

	return 0;
}

/* Merges all the PT_NOTE headers into one. */
static int __init merge_note_headers_elf32(char *elfptr, size_t *elfsz,
					   char **notes_buf, size_t *notes_sz)
{
	int i, nr_ptnote=0, rc=0;
	char *tmp;
	Elf32_Ehdr *ehdr_ptr;
	Elf32_Phdr phdr;
	u64 phdr_sz = 0, note_off;

	ehdr_ptr = (Elf32_Ehdr *)elfptr;

	rc = update_note_header_size_elf32(ehdr_ptr);
	if (rc < 0)
		return rc;

	rc = get_note_number_and_size_elf32(ehdr_ptr, &nr_ptnote, &phdr_sz);
	if (rc < 0)
		return rc;

	*notes_sz = roundup(phdr_sz, PAGE_SIZE);
	*notes_buf = vmcore_alloc_buf(*notes_sz);
	if (!*notes_buf)
		return -ENOMEM;

	rc = copy_notes_elf32(ehdr_ptr, *notes_buf);
	if (rc < 0)
		return rc;

	/* Prepare merged PT_NOTE program header. */
	phdr.p_type    = PT_NOTE;
	phdr.p_flags   = 0;
	note_off = sizeof(Elf32_Ehdr) +
			(ehdr_ptr->e_phnum - nr_ptnote +1) * sizeof(Elf32_Phdr);
	phdr.p_offset  = roundup(note_off, PAGE_SIZE);
	phdr.p_vaddr   = phdr.p_paddr = 0;
	phdr.p_filesz  = phdr.p_memsz = phdr_sz;
	phdr.p_align   = 0;

	/* Add merged PT_NOTE program header*/
	tmp = elfptr + sizeof(Elf32_Ehdr);
	memcpy(tmp, &phdr, sizeof(phdr));
	tmp += sizeof(phdr);

	/* Remove unwanted PT_NOTE program headers. */
	i = (nr_ptnote - 1) * sizeof(Elf32_Phdr);
	*elfsz = *elfsz - i;
	memmove(tmp, tmp+i, ((*elfsz)-sizeof(Elf32_Ehdr)-sizeof(Elf32_Phdr)));
	memset(elfptr + *elfsz, 0, i);
	*elfsz = roundup(*elfsz, PAGE_SIZE);

	/* Modify e_phnum to reflect merged headers. */
	ehdr_ptr->e_phnum = ehdr_ptr->e_phnum - nr_ptnote + 1;

	/* Store the size of all notes.  We need this to update the note
	 * header when the device dumps will be added.
	 */
	elfnotes_orig_sz = phdr.p_memsz;

	return 0;
}

/* Add memory chunks represented by program headers to vmcore list. Also update
 * the new offset fields of exported program headers. */
static int __init process_ptload_program_headers_elf64(char *elfptr,
						size_t elfsz,
						size_t elfnotes_sz,
						struct list_head *vc_list)
{
	int i;
	Elf64_Ehdr *ehdr_ptr;
	Elf64_Phdr *phdr_ptr;
	loff_t vmcore_off;
	struct vmcore *new;

	ehdr_ptr = (Elf64_Ehdr *)elfptr;
	phdr_ptr = (Elf64_Phdr*)(elfptr + sizeof(Elf64_Ehdr)); /* PT_NOTE hdr */

	/* Skip Elf header, program headers and Elf note segment. */
	vmcore_off = elfsz + elfnotes_sz;

	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		u64 paddr, start, end, size;

		if (phdr_ptr->p_type != PT_LOAD)
			continue;

		paddr = phdr_ptr->p_offset;
		start = rounddown(paddr, PAGE_SIZE);
		end = roundup(paddr + phdr_ptr->p_memsz, PAGE_SIZE);
		size = end - start;

		/* Add this contiguous chunk of memory to vmcore list.*/
		new = get_new_element();
		if (!new)
			return -ENOMEM;
		new->paddr = start;
		new->size = size;
		list_add_tail(&new->list, vc_list);

		/* Update the program header offset. */
		phdr_ptr->p_offset = vmcore_off + (paddr - start);
		vmcore_off = vmcore_off + size;
	}
	return 0;
}

static int __init process_ptload_program_headers_elf32(char *elfptr,
						size_t elfsz,
						size_t elfnotes_sz,
						struct list_head *vc_list)
{
	int i;
	Elf32_Ehdr *ehdr_ptr;
	Elf32_Phdr *phdr_ptr;
	loff_t vmcore_off;
	struct vmcore *new;

	ehdr_ptr = (Elf32_Ehdr *)elfptr;
	phdr_ptr = (Elf32_Phdr*)(elfptr + sizeof(Elf32_Ehdr)); /* PT_NOTE hdr */

	/* Skip Elf header, program headers and Elf note segment. */
	vmcore_off = elfsz + elfnotes_sz;

	for (i = 0; i < ehdr_ptr->e_phnum; i++, phdr_ptr++) {
		u64 paddr, start, end, size;

		if (phdr_ptr->p_type != PT_LOAD)
			continue;

		paddr = phdr_ptr->p_offset;
		start = rounddown(paddr, PAGE_SIZE);
		end = roundup(paddr + phdr_ptr->p_memsz, PAGE_SIZE);
		size = end - start;

		/* Add this contiguous chunk of memory to vmcore list.*/
		new = get_new_element();
		if (!new)
			return -ENOMEM;
		new->paddr = start;
		new->size = size;
		list_add_tail(&new->list, vc_list);

		/* Update the program header offset */
		phdr_ptr->p_offset = vmcore_off + (paddr - start);
		vmcore_off = vmcore_off + size;
	}
	return 0;
}

/* Sets offset fields of vmcore elements. */
static void set_vmcore_list_offsets(size_t elfsz, size_t elfnotes_sz,
				    struct list_head *vc_list)
{
	loff_t vmcore_off;
	struct vmcore *m;

	/* Skip Elf header, program headers and Elf note segment. */
	vmcore_off = elfsz + elfnotes_sz;

	list_for_each_entry(m, vc_list, list) {
		m->offset = vmcore_off;
		vmcore_off += m->size;
	}
}

static void free_elfcorebuf(void)
{
	free_pages((unsigned long)elfcorebuf, get_order(elfcorebuf_sz_orig));
	elfcorebuf = NULL;
	vfree(elfnotes_buf);
	elfnotes_buf = NULL;
}

static int __init parse_crash_elf64_headers(void)
{
	int rc=0;
	Elf64_Ehdr ehdr;
	u64 addr;

	addr = elfcorehdr_addr;

	/* Read Elf header */
	rc = elfcorehdr_read((char *)&ehdr, sizeof(Elf64_Ehdr), &addr);
	if (rc < 0)
		return rc;

	/* Do some basic Verification. */
	if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0 ||
		(ehdr.e_type != ET_CORE) ||
		!vmcore_elf64_check_arch(&ehdr) ||
		ehdr.e_ident[EI_CLASS] != ELFCLASS64 ||
		ehdr.e_ident[EI_VERSION] != EV_CURRENT ||
		ehdr.e_version != EV_CURRENT ||
		ehdr.e_ehsize != sizeof(Elf64_Ehdr) ||
		ehdr.e_phentsize != sizeof(Elf64_Phdr) ||
		ehdr.e_phnum == 0) {
		pr_warn("Warning: Core image elf header is not sane\n");
		return -EINVAL;
	}

	/* Read in all elf headers. */
	elfcorebuf_sz_orig = sizeof(Elf64_Ehdr) +
				ehdr.e_phnum * sizeof(Elf64_Phdr);
	elfcorebuf_sz = elfcorebuf_sz_orig;
	elfcorebuf = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO,
					      get_order(elfcorebuf_sz_orig));
	if (!elfcorebuf)
		return -ENOMEM;
	addr = elfcorehdr_addr;
	rc = elfcorehdr_read(elfcorebuf, elfcorebuf_sz_orig, &addr);
	if (rc < 0)
		goto fail;

	/* Merge all PT_NOTE headers into one. */
	rc = merge_note_headers_elf64(elfcorebuf, &elfcorebuf_sz,
				      &elfnotes_buf, &elfnotes_sz);
	if (rc)
		goto fail;
	rc = process_ptload_program_headers_elf64(elfcorebuf, elfcorebuf_sz,
						  elfnotes_sz, &vmcore_list);
	if (rc)
		goto fail;
	set_vmcore_list_offsets(elfcorebuf_sz, elfnotes_sz, &vmcore_list);
	return 0;
fail:
	free_elfcorebuf();
	return rc;
}

static int __init parse_crash_elf32_headers(void)
{
	int rc=0;
	Elf32_Ehdr ehdr;
	u64 addr;

	addr = elfcorehdr_addr;

	/* Read Elf header */
	rc = elfcorehdr_read((char *)&ehdr, sizeof(Elf32_Ehdr), &addr);
	if (rc < 0)
		return rc;

	/* Do some basic Verification. */
	if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0 ||
		(ehdr.e_type != ET_CORE) ||
		!vmcore_elf32_check_arch(&ehdr) ||
		ehdr.e_ident[EI_CLASS] != ELFCLASS32||
		ehdr.e_ident[EI_VERSION] != EV_CURRENT ||
		ehdr.e_version != EV_CURRENT ||
		ehdr.e_ehsize != sizeof(Elf32_Ehdr) ||
		ehdr.e_phentsize != sizeof(Elf32_Phdr) ||
		ehdr.e_phnum == 0) {
		pr_warn("Warning: Core image elf header is not sane\n");
		return -EINVAL;
	}

	/* Read in all elf headers. */
	elfcorebuf_sz_orig = sizeof(Elf32_Ehdr) + ehdr.e_phnum * sizeof(Elf32_Phdr);
	elfcorebuf_sz = elfcorebuf_sz_orig;
	elfcorebuf = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO,
					      get_order(elfcorebuf_sz_orig));
	if (!elfcorebuf)
		return -ENOMEM;
	addr = elfcorehdr_addr;
	rc = elfcorehdr_read(elfcorebuf, elfcorebuf_sz_orig, &addr);
	if (rc < 0)
		goto fail;

	/* Merge all PT_NOTE headers into one. */
	rc = merge_note_headers_elf32(elfcorebuf, &elfcorebuf_sz,
				      &elfnotes_buf, &elfnotes_sz);
	if (rc)
		goto fail;
	rc = process_ptload_program_headers_elf32(elfcorebuf, elfcorebuf_sz,
						  elfnotes_sz, &vmcore_list);
	if (rc)
		goto fail;
	set_vmcore_list_offsets(elfcorebuf_sz, elfnotes_sz, &vmcore_list);
	return 0;
fail:
	free_elfcorebuf();
	return rc;
}

static int __init parse_crash_elf_headers(void)
{
	unsigned char e_ident[EI_NIDENT];
	u64 addr;
	int rc=0;

	addr = elfcorehdr_addr;
	rc = elfcorehdr_read(e_ident, EI_NIDENT, &addr);
	if (rc < 0)
		return rc;
	if (memcmp(e_ident, ELFMAG, SELFMAG) != 0) {
		pr_warn("Warning: Core image elf header not found\n");
		return -EINVAL;
	}

	if (e_ident[EI_CLASS] == ELFCLASS64) {
		rc = parse_crash_elf64_headers();
		if (rc)
			return rc;
	} else if (e_ident[EI_CLASS] == ELFCLASS32) {
		rc = parse_crash_elf32_headers();
		if (rc)
			return rc;
	} else {
		pr_warn("Warning: Core image elf header is not sane\n");
		return -EINVAL;
	}

	/* Determine vmcore size. */
	vmcore_size = get_vmcore_size(elfcorebuf_sz, elfnotes_sz,
				      &vmcore_list);

	return 0;
}

#ifdef CONFIG_PROC_VMCORE_DEVICE_DUMP
/**
 * vmcoredd_write_header - Write vmcore device dump header at the
 * beginning of the dump's buffer.
 * @buf: Output buffer where the note is written
 * @data: Dump info
 * @size: Size of the dump
 *
 * Fills beginning of the dump's buffer with vmcore device dump header.
 */
static void vmcoredd_write_header(void *buf, struct vmcoredd_data *data,
				  u32 size)
{
	struct vmcoredd_header *vdd_hdr = (struct vmcoredd_header *)buf;

	vdd_hdr->n_namesz = sizeof(vdd_hdr->name);
	vdd_hdr->n_descsz = size + sizeof(vdd_hdr->dump_name);
	vdd_hdr->n_type = NT_VMCOREDD;

	strncpy((char *)vdd_hdr->name, VMCOREDD_NOTE_NAME,
		sizeof(vdd_hdr->name));
	memcpy(vdd_hdr->dump_name, data->dump_name, sizeof(vdd_hdr->dump_name));
}

/**
 * vmcoredd_update_program_headers - Update all Elf program headers
 * @elfptr: Pointer to elf header
 * @elfnotesz: Size of elf notes aligned to page size
 * @vmcoreddsz: Size of device dumps to be added to elf note header
 *
 * Determine type of Elf header (Elf64 or Elf32) and update the elf note size.
 * Also update the offsets of all the program headers after the elf note header.
 */
static void vmcoredd_update_program_headers(char *elfptr, size_t elfnotesz,
					    size_t vmcoreddsz)
{
	unsigned char *e_ident = (unsigned char *)elfptr;
	u64 start, end, size;
	loff_t vmcore_off;
	u32 i;

	vmcore_off = elfcorebuf_sz + elfnotesz;

	if (e_ident[EI_CLASS] == ELFCLASS64) {
		Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elfptr;
		Elf64_Phdr *phdr = (Elf64_Phdr *)(elfptr + sizeof(Elf64_Ehdr));

		/* Update all program headers */
		for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
			if (phdr->p_type == PT_NOTE) {
				/* Update note size */
				phdr->p_memsz = elfnotes_orig_sz + vmcoreddsz;
				phdr->p_filesz = phdr->p_memsz;
				continue;
			}

			start = rounddown(phdr->p_offset, PAGE_SIZE);
			end = roundup(phdr->p_offset + phdr->p_memsz,
				      PAGE_SIZE);
			size = end - start;
			phdr->p_offset = vmcore_off + (phdr->p_offset - start);
			vmcore_off += size;
		}
	} else {
		Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elfptr;
		Elf32_Phdr *phdr = (Elf32_Phdr *)(elfptr + sizeof(Elf32_Ehdr));

		/* Update all program headers */
		for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
			if (phdr->p_type == PT_NOTE) {
				/* Update note size */
				phdr->p_memsz = elfnotes_orig_sz + vmcoreddsz;
				phdr->p_filesz = phdr->p_memsz;
				continue;
			}

			start = rounddown(phdr->p_offset, PAGE_SIZE);
			end = roundup(phdr->p_offset + phdr->p_memsz,
				      PAGE_SIZE);
			size = end - start;
			phdr->p_offset = vmcore_off + (phdr->p_offset - start);
			vmcore_off += size;
		}
	}
}

/**
 * vmcoredd_update_size - Update the total size of the device dumps and update
 * Elf header
 * @dump_size: Size of the current device dump to be added to total size
 *
 * Update the total size of all the device dumps and update the Elf program
 * headers. Calculate the new offsets for the vmcore list and update the
 * total vmcore size.
 */
static void vmcoredd_update_size(size_t dump_size)
{
	vmcoredd_orig_sz += dump_size;
	elfnotes_sz = roundup(elfnotes_orig_sz, PAGE_SIZE) + vmcoredd_orig_sz;
	vmcoredd_update_program_headers(elfcorebuf, elfnotes_sz,
					vmcoredd_orig_sz);

	/* Update vmcore list offsets */
	set_vmcore_list_offsets(elfcorebuf_sz, elfnotes_sz, &vmcore_list);

	vmcore_size = get_vmcore_size(elfcorebuf_sz, elfnotes_sz,
				      &vmcore_list);
	proc_vmcore->size = vmcore_size;
}

/**
 * vmcore_add_device_dump - Add a buffer containing device dump to vmcore
 * @data: dump info.
 *
 * Allocate a buffer and invoke the calling driver's dump collect routine.
 * Write Elf note at the beginning of the buffer to indicate vmcore device
 * dump and add the dump to global list.
 */
int vmcore_add_device_dump(struct vmcoredd_data *data)
{
	struct vmcoredd_node *dump;
	void *buf = NULL;
	size_t data_size;
	int ret;

	if (vmcoredd_disabled) {
		pr_err_once("Device dump is disabled\n");
		return -EINVAL;
	}

	if (!data || !strlen(data->dump_name) ||
	    !data->vmcoredd_callback || !data->size)
		return -EINVAL;

	dump = vzalloc(sizeof(*dump));
	if (!dump) {
		ret = -ENOMEM;
		goto out_err;
	}

	/* Keep size of the buffer page aligned so that it can be mmaped */
	data_size = roundup(sizeof(struct vmcoredd_header) + data->size,
			    PAGE_SIZE);

	/* Allocate buffer for driver's to write their dumps */
	buf = vmcore_alloc_buf(data_size);
	if (!buf) {
		ret = -ENOMEM;
		goto out_err;
	}

	vmcoredd_write_header(buf, data, data_size -
			      sizeof(struct vmcoredd_header));

	/* Invoke the driver's dump collection routing */
	ret = data->vmcoredd_callback(data, buf +
				      sizeof(struct vmcoredd_header));
	if (ret)
		goto out_err;

	dump->buf = buf;
	dump->size = data_size;

	/* Add the dump to driver sysfs list */
	mutex_lock(&vmcoredd_mutex);
	list_add_tail(&dump->list, &vmcoredd_list);
	mutex_unlock(&vmcoredd_mutex);

	vmcoredd_update_size(data_size);
	return 0;

out_err:
	vfree(buf);
	vfree(dump);

	return ret;
}
EXPORT_SYMBOL(vmcore_add_device_dump);
#endif /* CONFIG_PROC_VMCORE_DEVICE_DUMP */

/* Free all dumps in vmcore device dump list */
static void vmcore_free_device_dumps(void)
{
#ifdef CONFIG_PROC_VMCORE_DEVICE_DUMP
	mutex_lock(&vmcoredd_mutex);
	while (!list_empty(&vmcoredd_list)) {
		struct vmcoredd_node *dump;

		dump = list_first_entry(&vmcoredd_list, struct vmcoredd_node,
					list);
		list_del(&dump->list);
		vfree(dump->buf);
		vfree(dump);
	}
	mutex_unlock(&vmcoredd_mutex);
#endif /* CONFIG_PROC_VMCORE_DEVICE_DUMP */
}

/* Init function for vmcore module. */
static int __init vmcore_init(void)
{
	int rc = 0;

	/* Allow architectures to allocate ELF header in 2nd kernel */
	rc = elfcorehdr_alloc(&elfcorehdr_addr, &elfcorehdr_size);
	if (rc)
		return rc;
	/*
	 * If elfcorehdr= has been passed in cmdline or created in 2nd kernel,
	 * then capture the dump.
	 */
	if (!(is_vmcore_usable()))
		return rc;
	rc = parse_crash_elf_headers();
	if (rc) {
		pr_warn("Kdump: vmcore not initialized\n");
		return rc;
	}
	elfcorehdr_free(elfcorehdr_addr);
	elfcorehdr_addr = ELFCORE_ADDR_ERR;

	proc_vmcore = proc_create("vmcore", S_IRUSR, NULL, &vmcore_proc_ops);
	if (proc_vmcore)
		proc_vmcore->size = vmcore_size;
	return 0;
}
fs_initcall(vmcore_init);

/* Cleanup function for vmcore module. */
void vmcore_cleanup(void)
{
	if (proc_vmcore) {
		proc_remove(proc_vmcore);
		proc_vmcore = NULL;
	}

	/* clear the vmcore list. */
	while (!list_empty(&vmcore_list)) {
		struct vmcore *m;

		m = list_first_entry(&vmcore_list, struct vmcore, list);
		list_del(&m->list);
		kfree(m);
	}
	free_elfcorebuf();

	/* clear vmcore device dump list */
	vmcore_free_device_dumps();
}
