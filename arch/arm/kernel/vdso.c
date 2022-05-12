// SPDX-License-Identifier: GPL-2.0-only
/*
 * Adapted from arm64 version.
 *
 * Copyright (C) 2012 ARM Limited
 * Copyright (C) 2015 Mentor Graphics Corporation.
 */

#include <linux/cache.h>
#include <linux/elf.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/of.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/timekeeper_internal.h>
#include <linux/vmalloc.h>
#include <asm/arch_timer.h>
#include <asm/barrier.h>
#include <asm/cacheflush.h>
#include <asm/page.h>
#include <asm/vdso.h>
#include <asm/vdso_datapage.h>
#include <clocksource/arm_arch_timer.h>
#include <vdso/helpers.h>
#include <vdso/vsyscall.h>

#define MAX_SYMNAME	64

static struct page **vdso_text_pagelist;

extern char vdso_start[], vdso_end[];

/* Total number of pages needed for the data and text portions of the VDSO. */
unsigned int vdso_total_pages __ro_after_init;

/*
 * The VDSO data page.
 */
static union vdso_data_store vdso_data_store __page_aligned_data;
struct vdso_data *vdso_data = vdso_data_store.data;

static struct page *vdso_data_page __ro_after_init;
static const struct vm_special_mapping vdso_data_mapping = {
	.name = "[vvar]",
	.pages = &vdso_data_page,
};

static int vdso_mremap(const struct vm_special_mapping *sm,
		struct vm_area_struct *new_vma)
{
	current->mm->context.vdso = new_vma->vm_start;

	return 0;
}

static struct vm_special_mapping vdso_text_mapping __ro_after_init = {
	.name = "[vdso]",
	.mremap = vdso_mremap,
};

struct elfinfo {
	Elf32_Ehdr	*hdr;		/* ptr to ELF */
	Elf32_Sym	*dynsym;	/* ptr to .dynsym section */
	unsigned long	dynsymsize;	/* size of .dynsym section */
	char		*dynstr;	/* ptr to .dynstr section */
};

/* Cached result of boot-time check for whether the arch timer exists,
 * and if so, whether the virtual counter is useable.
 */
bool cntvct_ok __ro_after_init;

static bool __init cntvct_functional(void)
{
	struct device_node *np;
	bool ret = false;

	if (!IS_ENABLED(CONFIG_ARM_ARCH_TIMER))
		goto out;

	/* The arm_arch_timer core should export
	 * arch_timer_use_virtual or similar so we don't have to do
	 * this.
	 */
	np = of_find_compatible_node(NULL, NULL, "arm,armv7-timer");
	if (!np)
		np = of_find_compatible_node(NULL, NULL, "arm,armv8-timer");
	if (!np)
		goto out_put;

	if (of_property_read_bool(np, "arm,cpu-registers-not-fw-configured"))
		goto out_put;

	ret = true;

out_put:
	of_node_put(np);
out:
	return ret;
}

static void * __init find_section(Elf32_Ehdr *ehdr, const char *name,
				  unsigned long *size)
{
	Elf32_Shdr *sechdrs;
	unsigned int i;
	char *secnames;

	/* Grab section headers and strings so we can tell who is who */
	sechdrs = (void *)ehdr + ehdr->e_shoff;
	secnames = (void *)ehdr + sechdrs[ehdr->e_shstrndx].sh_offset;

	/* Find the section they want */
	for (i = 1; i < ehdr->e_shnum; i++) {
		if (strcmp(secnames + sechdrs[i].sh_name, name) == 0) {
			if (size)
				*size = sechdrs[i].sh_size;
			return (void *)ehdr + sechdrs[i].sh_offset;
		}
	}

	if (size)
		*size = 0;
	return NULL;
}

static Elf32_Sym * __init find_symbol(struct elfinfo *lib, const char *symname)
{
	unsigned int i;

	for (i = 0; i < (lib->dynsymsize / sizeof(Elf32_Sym)); i++) {
		char name[MAX_SYMNAME], *c;

		if (lib->dynsym[i].st_name == 0)
			continue;
		strlcpy(name, lib->dynstr + lib->dynsym[i].st_name,
			MAX_SYMNAME);
		c = strchr(name, '@');
		if (c)
			*c = 0;
		if (strcmp(symname, name) == 0)
			return &lib->dynsym[i];
	}
	return NULL;
}

static void __init vdso_nullpatch_one(struct elfinfo *lib, const char *symname)
{
	Elf32_Sym *sym;

	sym = find_symbol(lib, symname);
	if (!sym)
		return;

	sym->st_name = 0;
}

static void __init patch_vdso(void *ehdr)
{
	struct elfinfo einfo;

	einfo = (struct elfinfo) {
		.hdr = ehdr,
	};

	einfo.dynsym = find_section(einfo.hdr, ".dynsym", &einfo.dynsymsize);
	einfo.dynstr = find_section(einfo.hdr, ".dynstr", NULL);

	/* If the virtual counter is absent or non-functional we don't
	 * want programs to incur the slight additional overhead of
	 * dispatching through the VDSO only to fall back to syscalls.
	 */
	if (!cntvct_ok) {
		vdso_nullpatch_one(&einfo, "__vdso_gettimeofday");
		vdso_nullpatch_one(&einfo, "__vdso_clock_gettime");
		vdso_nullpatch_one(&einfo, "__vdso_clock_gettime64");
	}
}

static int __init vdso_init(void)
{
	unsigned int text_pages;
	int i;

	if (memcmp(vdso_start, "\177ELF", 4)) {
		pr_err("VDSO is not a valid ELF object!\n");
		return -ENOEXEC;
	}

	text_pages = (vdso_end - vdso_start) >> PAGE_SHIFT;

	/* Allocate the VDSO text pagelist */
	vdso_text_pagelist = kcalloc(text_pages, sizeof(struct page *),
				     GFP_KERNEL);
	if (vdso_text_pagelist == NULL)
		return -ENOMEM;

	/* Grab the VDSO data page. */
	vdso_data_page = virt_to_page(vdso_data);

	/* Grab the VDSO text pages. */
	for (i = 0; i < text_pages; i++) {
		struct page *page;

		page = virt_to_page(vdso_start + i * PAGE_SIZE);
		vdso_text_pagelist[i] = page;
	}

	vdso_text_mapping.pages = vdso_text_pagelist;

	vdso_total_pages = 1; /* for the data/vvar page */
	vdso_total_pages += text_pages;

	cntvct_ok = cntvct_functional();

	patch_vdso(vdso_start);

	return 0;
}
arch_initcall(vdso_init);

static int install_vvar(struct mm_struct *mm, unsigned long addr)
{
	struct vm_area_struct *vma;

	vma = _install_special_mapping(mm, addr, PAGE_SIZE,
				       VM_READ | VM_MAYREAD,
				       &vdso_data_mapping);

	return PTR_ERR_OR_ZERO(vma);
}

/* assumes mmap_lock is write-locked */
void arm_install_vdso(struct mm_struct *mm, unsigned long addr)
{
	struct vm_area_struct *vma;
	unsigned long len;

	mm->context.vdso = 0;

	if (vdso_text_pagelist == NULL)
		return;

	if (install_vvar(mm, addr))
		return;

	/* Account for vvar page. */
	addr += PAGE_SIZE;
	len = (vdso_total_pages - 1) << PAGE_SHIFT;

	vma = _install_special_mapping(mm, addr, len,
		VM_READ | VM_EXEC | VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC,
		&vdso_text_mapping);

	if (!IS_ERR(vma))
		mm->context.vdso = addr;
}

