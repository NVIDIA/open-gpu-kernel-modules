/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2006 PathScale, Inc.  All Rights Reserved.
 */
#ifndef AMDKCL_IO_H
#define AMDKCL_IO_H

#include <linux/types.h>
#include <linux/io.h>

/* Copied from arch/x86/include/asm/io.h
 * include/linux/io.h
 */
#if !defined(HAVE_ARCH_IO_RESERVE_FREE_MEMTYPE_WC)

#ifdef CONFIG_X86
extern int _kcl_arch_io_reserve_memtype_wc(resource_size_t start, resource_size_t size);
extern void _kcl_arch_io_free_memtype_wc(resource_size_t start, resource_size_t size);
#define arch_io_reserve_memtype_wc _kcl_arch_io_reserve_memtype_wc
#define arch_io_free_memtype_wc _kcl_arch_io_free_memtype_wc
#endif

#ifndef arch_io_reserve_memtype_wc
/*
 * On x86 PAT systems we have memory tracking that keeps track of
 * the allowed mappings on memory ranges. This tracking works for
 * all the in-kernel mapping APIs (ioremap*), but where the user
 * wishes to map a range from a physical device into user memory
 * the tracking won't be updated. This API is to be used by
 * drivers which remap physical device pages into userspace,
 * and wants to make sure they are mapped WC and not UC.
 */
static inline int arch_io_reserve_memtype_wc(resource_size_t base,
					     resource_size_t size)
{
	return 0;
}

static inline void arch_io_free_memtype_wc(resource_size_t base,
					   resource_size_t size)
{
}
#endif

#endif /* HAVE_ARCH_IO_RESERVE_FREE_MEMTYPE_WC */

#endif /* AMDKCL_IO_H */
