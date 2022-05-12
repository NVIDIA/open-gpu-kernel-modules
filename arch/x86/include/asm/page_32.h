/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_PAGE_32_H
#define _ASM_X86_PAGE_32_H

#include <asm/page_32_types.h>

#ifndef __ASSEMBLY__

#define __phys_addr_nodebug(x)	((x) - PAGE_OFFSET)
#ifdef CONFIG_DEBUG_VIRTUAL
extern unsigned long __phys_addr(unsigned long);
#else
#define __phys_addr(x)		__phys_addr_nodebug(x)
#endif
#define __phys_addr_symbol(x)	__phys_addr(x)
#define __phys_reloc_hide(x)	RELOC_HIDE((x), 0)

#ifdef CONFIG_FLATMEM
#define pfn_valid(pfn)		((pfn) < max_mapnr)
#endif /* CONFIG_FLATMEM */

#ifdef CONFIG_X86_USE_3DNOW
#include <asm/mmx.h>

static inline void clear_page(void *page)
{
	mmx_clear_page(page);
}

static inline void copy_page(void *to, void *from)
{
	mmx_copy_page(to, from);
}
#else  /* !CONFIG_X86_USE_3DNOW */
#include <linux/string.h>

static inline void clear_page(void *page)
{
	memset(page, 0, PAGE_SIZE);
}

static inline void copy_page(void *to, void *from)
{
	memcpy(to, from, PAGE_SIZE);
}
#endif	/* CONFIG_X86_3DNOW */
#endif	/* !__ASSEMBLY__ */

#endif /* _ASM_X86_PAGE_32_H */
