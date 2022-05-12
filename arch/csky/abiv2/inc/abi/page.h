/* SPDX-License-Identifier: GPL-2.0 */

static inline void clear_user_page(void *addr, unsigned long vaddr,
				   struct page *page)
{
	clear_page(addr);
}

static inline void copy_user_page(void *to, void *from, unsigned long vaddr,
				  struct page *page)
{
	copy_page(to, from);
}
