/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_PAGEMAP_H
#define AMDKCL_PAGEMAP_H

#include <linux/pagemap.h>

#ifndef HAVE_MM_RELEASE_PAGES_2ARGS
static inline void _kcl_release_pages(struct page **pages, int nr)
{
	release_pages(pages, nr, 0);
}
#define release_pages _kcl_release_pages
#endif
#endif
