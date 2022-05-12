/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2013 - Virtual Open Systems
 * Author: Antonios Motakis <a.motakis@virtualopensystems.com>
 */

#ifndef VFIO_PLATFORM_PRIVATE_H
#define VFIO_PLATFORM_PRIVATE_H

#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/vfio.h>

#define VFIO_PLATFORM_OFFSET_SHIFT   40
#define VFIO_PLATFORM_OFFSET_MASK (((u64)(1) << VFIO_PLATFORM_OFFSET_SHIFT) - 1)

#define VFIO_PLATFORM_OFFSET_TO_INDEX(off)	\
	(off >> VFIO_PLATFORM_OFFSET_SHIFT)

#define VFIO_PLATFORM_INDEX_TO_OFFSET(index)	\
	((u64)(index) << VFIO_PLATFORM_OFFSET_SHIFT)

struct vfio_platform_irq {
	u32			flags;
	u32			count;
	int			hwirq;
	char			*name;
	struct eventfd_ctx	*trigger;
	bool			masked;
	spinlock_t		lock;
	struct virqfd		*unmask;
	struct virqfd		*mask;
};

struct vfio_platform_region {
	u64			addr;
	resource_size_t		size;
	u32			flags;
	u32			type;
#define VFIO_PLATFORM_REGION_TYPE_MMIO	1
#define VFIO_PLATFORM_REGION_TYPE_PIO	2
	void __iomem		*ioaddr;
};

struct vfio_platform_device {
	struct vfio_device		vdev;
	struct vfio_platform_region	*regions;
	u32				num_regions;
	struct vfio_platform_irq	*irqs;
	u32				num_irqs;
	int				refcnt;
	struct mutex			igate;
	struct module			*parent_module;
	const char			*compat;
	const char			*acpihid;
	struct module			*reset_module;
	struct device			*device;

	/*
	 * These fields should be filled by the bus specific binder
	 */
	void		*opaque;
	const char	*name;
	uint32_t	flags;
	/* callbacks to discover device resources */
	struct resource*
		(*get_resource)(struct vfio_platform_device *vdev, int i);
	int	(*get_irq)(struct vfio_platform_device *vdev, int i);
	int	(*of_reset)(struct vfio_platform_device *vdev);

	bool				reset_required;
};

typedef int (*vfio_platform_reset_fn_t)(struct vfio_platform_device *vdev);

struct vfio_platform_reset_node {
	struct list_head link;
	char *compat;
	struct module *owner;
	vfio_platform_reset_fn_t of_reset;
};

extern int vfio_platform_probe_common(struct vfio_platform_device *vdev,
				      struct device *dev);
void vfio_platform_remove_common(struct vfio_platform_device *vdev);

extern int vfio_platform_irq_init(struct vfio_platform_device *vdev);
extern void vfio_platform_irq_cleanup(struct vfio_platform_device *vdev);

extern int vfio_platform_set_irqs_ioctl(struct vfio_platform_device *vdev,
					uint32_t flags, unsigned index,
					unsigned start, unsigned count,
					void *data);

extern void __vfio_platform_register_reset(struct vfio_platform_reset_node *n);
extern void vfio_platform_unregister_reset(const char *compat,
					   vfio_platform_reset_fn_t fn);
#define vfio_platform_register_reset(__compat, __reset)		\
static struct vfio_platform_reset_node __reset ## _node = {	\
	.owner = THIS_MODULE,					\
	.compat = __compat,					\
	.of_reset = __reset,					\
};								\
__vfio_platform_register_reset(&__reset ## _node)

#define module_vfio_reset_handler(compat, reset)		\
MODULE_ALIAS("vfio-reset:" compat);				\
static int __init reset ## _module_init(void)			\
{								\
	vfio_platform_register_reset(compat, reset);		\
	return 0;						\
};								\
static void __exit reset ## _module_exit(void)			\
{								\
	vfio_platform_unregister_reset(compat, reset);		\
};								\
module_init(reset ## _module_init);				\
module_exit(reset ## _module_exit)

#endif /* VFIO_PLATFORM_PRIVATE_H */
