/* SPDX-License-Identifier: GPL-2.0 */
/*
 * /proc/kcore definitions
 */
#ifndef _LINUX_KCORE_H
#define _LINUX_KCORE_H

enum kcore_type {
	KCORE_TEXT,
	KCORE_VMALLOC,
	KCORE_RAM,
	KCORE_VMEMMAP,
	KCORE_USER,
	KCORE_OTHER,
	KCORE_REMAP,
};

struct kcore_list {
	struct list_head list;
	unsigned long addr;
	unsigned long vaddr;
	size_t size;
	int type;
};

struct vmcore {
	struct list_head list;
	unsigned long long paddr;
	unsigned long long size;
	loff_t offset;
};

struct vmcoredd_node {
	struct list_head list;	/* List of dumps */
	void *buf;		/* Buffer containing device's dump */
	unsigned int size;	/* Size of the buffer */
};

#ifdef CONFIG_PROC_KCORE
void __init kclist_add(struct kcore_list *, void *, size_t, int type);

extern int __init register_mem_pfn_is_ram(int (*fn)(unsigned long pfn));
#else
static inline
void kclist_add(struct kcore_list *new, void *addr, size_t size, int type)
{
}
#endif

#endif /* _LINUX_KCORE_H */
