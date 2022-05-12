// SPDX-License-Identifier: GPL-2.0-only
/*
 * Page Attribute Table (PAT) support: handle memory caching attributes in page tables.
 *
 * Authors: Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>
 *          Suresh B Siddha <suresh.b.siddha@intel.com>
 *
 * Loosely based on earlier PAT patchset from Eric Biederman and Andi Kleen.
 *
 * Basic principles:
 *
 * PAT is a CPU feature supported by all modern x86 CPUs, to allow the firmware and
 * the kernel to set one of a handful of 'caching type' attributes for physical
 * memory ranges: uncached, write-combining, write-through, write-protected,
 * and the most commonly used and default attribute: write-back caching.
 *
 * PAT support supercedes and augments MTRR support in a compatible fashion: MTRR is
 * a hardware interface to enumerate a limited number of physical memory ranges
 * and set their caching attributes explicitly, programmed into the CPU via MSRs.
 * Even modern CPUs have MTRRs enabled - but these are typically not touched
 * by the kernel or by user-space (such as the X server), we rely on PAT for any
 * additional cache attribute logic.
 *
 * PAT doesn't work via explicit memory ranges, but uses page table entries to add
 * cache attribute information to the mapped memory range: there's 3 bits used,
 * (_PAGE_PWT, _PAGE_PCD, _PAGE_PAT), with the 8 possible values mapped by the
 * CPU to actual cache attributes via an MSR loaded into the CPU (MSR_IA32_CR_PAT).
 *
 * ( There's a metric ton of finer details, such as compatibility with CPU quirks
 *   that only support 4 types of PAT entries, and interaction with MTRRs, see
 *   below for details. )
 */
#include <linux/module.h>
#include <linux/io.h>

/* Copied from arch/x86/mm/pat.c and modified for KCL */
#if !defined(HAVE_ARCH_IO_RESERVE_FREE_MEMTYPE_WC) && \
	defined(CONFIG_X86)
#include <asm/pgtable_types.h>

static int (*_kcl_io_reserve_memtype)(resource_size_t start, resource_size_t end,
			enum page_cache_mode *type);
static void (*_kcl_io_free_memtype)(resource_size_t start, resource_size_t end);

int _kcl_arch_io_reserve_memtype_wc(resource_size_t start, resource_size_t size)
{
#ifdef _PAGE_CACHE_WC
	unsigned long type = _PAGE_CACHE_WC;
#else
	enum page_cache_mode type = _PAGE_CACHE_MODE_WC;
#endif

	return _kcl_io_reserve_memtype(start, start + size, &type);
}
EXPORT_SYMBOL(_kcl_arch_io_reserve_memtype_wc);

void _kcl_arch_io_free_memtype_wc(resource_size_t start, resource_size_t size)
{
	_kcl_io_free_memtype(start, start + size);
}
EXPORT_SYMBOL(_kcl_arch_io_free_memtype_wc);

void amdkcl_io_init(void)
{
	_kcl_io_reserve_memtype = amdkcl_fp_setup("io_reserve_memtype", NULL);
	_kcl_io_free_memtype = amdkcl_fp_setup("io_free_memtype", NULL);
}
#else
void amdkcl_io_init(void)
{

}
#endif /* HAVE_ARCH_IO_RESERVE_FREE_MEMTYPE_WC */
