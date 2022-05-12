/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linux/of.h>	/* linux/of.h gets to determine #include ordering */
#ifndef _SPARC_PROM_H
#define _SPARC_PROM_H
#ifdef __KERNEL__

/*
 * Definitions for talking to the Open Firmware PROM on
 * Power Macintosh computers.
 *
 * Copyright (C) 1996-2005 Paul Mackerras.
 *
 * Updates for PPC64 by Peter Bergner & David Engebretsen, IBM Corp.
 * Updates for SPARC by David S. Miller
 */
#include <linux/types.h>
#include <linux/of_pdt.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/irqdomain.h>

#define of_compat_cmp(s1, s2, l)	strncmp((s1), (s2), (l))
#define of_prop_cmp(s1, s2)		strcasecmp((s1), (s2))
#define of_node_cmp(s1, s2)		strcmp((s1), (s2))

struct of_irq_controller {
	unsigned int	(*irq_build)(struct device_node *, unsigned int, void *);
	void		*data;
};

struct device_node *of_find_node_by_cpuid(int cpuid);
int of_set_property(struct device_node *node, const char *name, void *val, int len);
extern struct mutex of_set_property_mutex;
int of_getintprop_default(struct device_node *np,
			  const char *name,
				 int def);
int of_find_in_proplist(const char *list, const char *match, int len);

void prom_build_devicetree(void);
void of_populate_present_mask(void);
void of_fill_in_cpu_data(void);

struct resource;
void __iomem *of_ioremap(struct resource *res, unsigned long offset, unsigned long size, char *name);
void of_iounmap(struct resource *res, void __iomem *base, unsigned long size);

extern struct device_node *of_console_device;
extern char *of_console_path;
extern char *of_console_options;

void irq_trans_init(struct device_node *dp);
char *build_path_component(struct device_node *dp);

#endif /* __KERNEL__ */
#endif /* _SPARC_PROM_H */
