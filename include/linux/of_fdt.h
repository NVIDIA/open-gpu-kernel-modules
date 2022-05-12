/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Definitions for working with the Flattened Device Tree data format
 *
 * Copyright 2009 Benjamin Herrenschmidt, IBM Corp
 * benh@kernel.crashing.org
 */

#ifndef _LINUX_OF_FDT_H
#define _LINUX_OF_FDT_H

#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>

/* Definitions used by the flattened device tree */
#define OF_DT_HEADER		0xd00dfeed	/* marker */

#ifndef __ASSEMBLY__

#if defined(CONFIG_OF_FLATTREE)

struct device_node;

/* For scanning an arbitrary device-tree at any time */
extern void *of_fdt_unflatten_tree(const unsigned long *blob,
				   struct device_node *dad,
				   struct device_node **mynodes);

/* TBD: Temporary export of fdt globals - remove when code fully merged */
extern int __initdata dt_root_addr_cells;
extern int __initdata dt_root_size_cells;
extern void *initial_boot_params;

extern char __dtb_start[];
extern char __dtb_end[];

/* Other Prototypes */
extern u64 of_flat_dt_translate_address(unsigned long node);
extern void of_fdt_limit_memory(int limit);
#endif /* CONFIG_OF_FLATTREE */

#ifdef CONFIG_OF_EARLY_FLATTREE
/* For scanning the flat device-tree at boot time */
extern int of_scan_flat_dt(int (*it)(unsigned long node, const char *uname,
				     int depth, void *data),
			   void *data);
extern int of_scan_flat_dt_subnodes(unsigned long node,
				    int (*it)(unsigned long node,
					      const char *uname,
					      void *data),
				    void *data);
extern int of_get_flat_dt_subnode_by_name(unsigned long node,
					  const char *uname);
extern const void *of_get_flat_dt_prop(unsigned long node, const char *name,
				       int *size);
extern int of_flat_dt_is_compatible(unsigned long node, const char *name);
extern unsigned long of_get_flat_dt_root(void);
extern uint32_t of_get_flat_dt_phandle(unsigned long node);

extern int early_init_dt_scan_chosen(unsigned long node, const char *uname,
				     int depth, void *data);
extern int early_init_dt_scan_memory(unsigned long node, const char *uname,
				     int depth, void *data);
extern int early_init_dt_scan_chosen_stdout(void);
extern void early_init_fdt_scan_reserved_mem(void);
extern void early_init_fdt_reserve_self(void);
extern void __init early_init_dt_scan_chosen_arch(unsigned long node);
extern void early_init_dt_add_memory_arch(u64 base, u64 size);
extern int early_init_dt_mark_hotplug_memory_arch(u64 base, u64 size);
extern int early_init_dt_reserve_memory_arch(phys_addr_t base, phys_addr_t size,
					     bool no_map);
extern u64 dt_mem_next_cell(int s, const __be32 **cellp);

/* Early flat tree scan hooks */
extern int early_init_dt_scan_root(unsigned long node, const char *uname,
				   int depth, void *data);

extern bool early_init_dt_scan(void *params);
extern bool early_init_dt_verify(void *params);
extern void early_init_dt_scan_nodes(void);

extern const char *of_flat_dt_get_machine_name(void);
extern const void *of_flat_dt_match_machine(const void *default_match,
		const void * (*get_next_compat)(const char * const**));

/* Other Prototypes */
extern void unflatten_device_tree(void);
extern void unflatten_and_copy_device_tree(void);
extern void early_init_devtree(void *);
extern void early_get_first_memblock_info(void *, phys_addr_t *);
#else /* CONFIG_OF_EARLY_FLATTREE */
static inline int early_init_dt_scan_chosen_stdout(void) { return -ENODEV; }
static inline void early_init_fdt_scan_reserved_mem(void) {}
static inline void early_init_fdt_reserve_self(void) {}
static inline const char *of_flat_dt_get_machine_name(void) { return NULL; }
static inline void unflatten_device_tree(void) {}
static inline void unflatten_and_copy_device_tree(void) {}
#endif /* CONFIG_OF_EARLY_FLATTREE */

#endif /* __ASSEMBLY__ */
#endif /* _LINUX_OF_FDT_H */
