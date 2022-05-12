/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __CMA_H__
#define __CMA_H__

#include <linux/init.h>
#include <linux/types.h>
#include <linux/numa.h>

/*
 * There is always at least global CMA area and a few optional
 * areas configured in kernel .config.
 */
#ifdef CONFIG_CMA_AREAS
#define MAX_CMA_AREAS	(1 + CONFIG_CMA_AREAS)

#else
#define MAX_CMA_AREAS	(0)

#endif

#define CMA_MAX_NAME 64

struct cma;

extern unsigned long totalcma_pages;
extern phys_addr_t cma_get_base(const struct cma *cma);
extern unsigned long cma_get_size(const struct cma *cma);
extern const char *cma_get_name(const struct cma *cma);

extern int __init cma_declare_contiguous_nid(phys_addr_t base,
			phys_addr_t size, phys_addr_t limit,
			phys_addr_t alignment, unsigned int order_per_bit,
			bool fixed, const char *name, struct cma **res_cma,
			int nid);
static inline int __init cma_declare_contiguous(phys_addr_t base,
			phys_addr_t size, phys_addr_t limit,
			phys_addr_t alignment, unsigned int order_per_bit,
			bool fixed, const char *name, struct cma **res_cma)
{
	return cma_declare_contiguous_nid(base, size, limit, alignment,
			order_per_bit, fixed, name, res_cma, NUMA_NO_NODE);
}
extern int cma_init_reserved_mem(phys_addr_t base, phys_addr_t size,
					unsigned int order_per_bit,
					const char *name,
					struct cma **res_cma);
extern struct page *cma_alloc(struct cma *cma, unsigned long count, unsigned int align,
			      bool no_warn);
extern bool cma_release(struct cma *cma, const struct page *pages, unsigned long count);

extern int cma_for_each_area(int (*it)(struct cma *cma, void *data), void *data);
#endif
