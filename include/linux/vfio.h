/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * VFIO API definition
 *
 * Copyright (C) 2012 Red Hat, Inc.  All rights reserved.
 *     Author: Alex Williamson <alex.williamson@redhat.com>
 */
#ifndef VFIO_H
#define VFIO_H


#include <linux/iommu.h>
#include <linux/mm.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <uapi/linux/vfio.h>

struct vfio_device {
	struct device *dev;
	const struct vfio_device_ops *ops;
	struct vfio_group *group;

	/* Members below here are private, not for driver use */
	refcount_t refcount;
	struct completion comp;
	struct list_head group_next;
};

/**
 * struct vfio_device_ops - VFIO bus driver device callbacks
 *
 * @open: Called when userspace creates new file descriptor for device
 * @release: Called when userspace releases file descriptor for device
 * @read: Perform read(2) on device file descriptor
 * @write: Perform write(2) on device file descriptor
 * @ioctl: Perform ioctl(2) on device file descriptor, supporting VFIO_DEVICE_*
 *         operations documented below
 * @mmap: Perform mmap(2) on a region of the device file descriptor
 * @request: Request for the bus driver to release the device
 * @match: Optional device name match callback (return: 0 for no-match, >0 for
 *         match, -errno for abort (ex. match with insufficient or incorrect
 *         additional args)
 */
struct vfio_device_ops {
	char	*name;
	int	(*open)(struct vfio_device *vdev);
	void	(*release)(struct vfio_device *vdev);
	ssize_t	(*read)(struct vfio_device *vdev, char __user *buf,
			size_t count, loff_t *ppos);
	ssize_t	(*write)(struct vfio_device *vdev, const char __user *buf,
			 size_t count, loff_t *size);
	long	(*ioctl)(struct vfio_device *vdev, unsigned int cmd,
			 unsigned long arg);
	int	(*mmap)(struct vfio_device *vdev, struct vm_area_struct *vma);
	void	(*request)(struct vfio_device *vdev, unsigned int count);
	int	(*match)(struct vfio_device *vdev, char *buf);
};

extern struct iommu_group *vfio_iommu_group_get(struct device *dev);
extern void vfio_iommu_group_put(struct iommu_group *group, struct device *dev);

void vfio_init_group_dev(struct vfio_device *device, struct device *dev,
			 const struct vfio_device_ops *ops);
int vfio_register_group_dev(struct vfio_device *device);
void vfio_unregister_group_dev(struct vfio_device *device);
extern struct vfio_device *vfio_device_get_from_dev(struct device *dev);
extern void vfio_device_put(struct vfio_device *device);

/* events for the backend driver notify callback */
enum vfio_iommu_notify_type {
	VFIO_IOMMU_CONTAINER_CLOSE = 0,
};

/**
 * struct vfio_iommu_driver_ops - VFIO IOMMU driver callbacks
 */
struct vfio_iommu_driver_ops {
	char		*name;
	struct module	*owner;
	void		*(*open)(unsigned long arg);
	void		(*release)(void *iommu_data);
	ssize_t		(*read)(void *iommu_data, char __user *buf,
				size_t count, loff_t *ppos);
	ssize_t		(*write)(void *iommu_data, const char __user *buf,
				 size_t count, loff_t *size);
	long		(*ioctl)(void *iommu_data, unsigned int cmd,
				 unsigned long arg);
	int		(*mmap)(void *iommu_data, struct vm_area_struct *vma);
	int		(*attach_group)(void *iommu_data,
					struct iommu_group *group);
	void		(*detach_group)(void *iommu_data,
					struct iommu_group *group);
	int		(*pin_pages)(void *iommu_data,
				     struct iommu_group *group,
				     unsigned long *user_pfn,
				     int npage, int prot,
				     unsigned long *phys_pfn);
	int		(*unpin_pages)(void *iommu_data,
				       unsigned long *user_pfn, int npage);
	int		(*register_notifier)(void *iommu_data,
					     unsigned long *events,
					     struct notifier_block *nb);
	int		(*unregister_notifier)(void *iommu_data,
					       struct notifier_block *nb);
	int		(*dma_rw)(void *iommu_data, dma_addr_t user_iova,
				  void *data, size_t count, bool write);
	struct iommu_domain *(*group_iommu_domain)(void *iommu_data,
						   struct iommu_group *group);
	void		(*notify)(void *iommu_data,
				  enum vfio_iommu_notify_type event);
};

extern int vfio_register_iommu_driver(const struct vfio_iommu_driver_ops *ops);

extern void vfio_unregister_iommu_driver(
				const struct vfio_iommu_driver_ops *ops);

/*
 * External user API
 */
extern struct vfio_group *vfio_group_get_external_user(struct file *filep);
extern void vfio_group_put_external_user(struct vfio_group *group);
extern struct vfio_group *vfio_group_get_external_user_from_dev(struct device
								*dev);
extern bool vfio_external_group_match_file(struct vfio_group *group,
					   struct file *filep);
extern int vfio_external_user_iommu_id(struct vfio_group *group);
extern long vfio_external_check_extension(struct vfio_group *group,
					  unsigned long arg);

#define VFIO_PIN_PAGES_MAX_ENTRIES	(PAGE_SIZE/sizeof(unsigned long))

extern int vfio_pin_pages(struct device *dev, unsigned long *user_pfn,
			  int npage, int prot, unsigned long *phys_pfn);
extern int vfio_unpin_pages(struct device *dev, unsigned long *user_pfn,
			    int npage);

extern int vfio_group_pin_pages(struct vfio_group *group,
				unsigned long *user_iova_pfn, int npage,
				int prot, unsigned long *phys_pfn);
extern int vfio_group_unpin_pages(struct vfio_group *group,
				  unsigned long *user_iova_pfn, int npage);

extern int vfio_dma_rw(struct vfio_group *group, dma_addr_t user_iova,
		       void *data, size_t len, bool write);

extern struct iommu_domain *vfio_group_iommu_domain(struct vfio_group *group);

/* each type has independent events */
enum vfio_notify_type {
	VFIO_IOMMU_NOTIFY = 0,
	VFIO_GROUP_NOTIFY = 1,
};

/* events for VFIO_IOMMU_NOTIFY */
#define VFIO_IOMMU_NOTIFY_DMA_UNMAP	BIT(0)

/* events for VFIO_GROUP_NOTIFY */
#define VFIO_GROUP_NOTIFY_SET_KVM	BIT(0)

extern int vfio_register_notifier(struct device *dev,
				  enum vfio_notify_type type,
				  unsigned long *required_events,
				  struct notifier_block *nb);
extern int vfio_unregister_notifier(struct device *dev,
				    enum vfio_notify_type type,
				    struct notifier_block *nb);

struct kvm;
extern void vfio_group_set_kvm(struct vfio_group *group, struct kvm *kvm);

/*
 * Sub-module helpers
 */
struct vfio_info_cap {
	struct vfio_info_cap_header *buf;
	size_t size;
};
extern struct vfio_info_cap_header *vfio_info_cap_add(
		struct vfio_info_cap *caps, size_t size, u16 id, u16 version);
extern void vfio_info_cap_shift(struct vfio_info_cap *caps, size_t offset);

extern int vfio_info_add_capability(struct vfio_info_cap *caps,
				    struct vfio_info_cap_header *cap,
				    size_t size);

extern int vfio_set_irqs_validate_and_prepare(struct vfio_irq_set *hdr,
					      int num_irqs, int max_irq_type,
					      size_t *data_size);

struct pci_dev;
#if IS_ENABLED(CONFIG_VFIO_SPAPR_EEH)
extern void vfio_spapr_pci_eeh_open(struct pci_dev *pdev);
extern void vfio_spapr_pci_eeh_release(struct pci_dev *pdev);
extern long vfio_spapr_iommu_eeh_ioctl(struct iommu_group *group,
				       unsigned int cmd,
				       unsigned long arg);
#else
static inline void vfio_spapr_pci_eeh_open(struct pci_dev *pdev)
{
}

static inline void vfio_spapr_pci_eeh_release(struct pci_dev *pdev)
{
}

static inline long vfio_spapr_iommu_eeh_ioctl(struct iommu_group *group,
					      unsigned int cmd,
					      unsigned long arg)
{
	return -ENOTTY;
}
#endif /* CONFIG_VFIO_SPAPR_EEH */

/*
 * IRQfd - generic
 */
struct virqfd {
	void			*opaque;
	struct eventfd_ctx	*eventfd;
	int			(*handler)(void *, void *);
	void			(*thread)(void *, void *);
	void			*data;
	struct work_struct	inject;
	wait_queue_entry_t		wait;
	poll_table		pt;
	struct work_struct	shutdown;
	struct virqfd		**pvirqfd;
};

extern int vfio_virqfd_enable(void *opaque,
			      int (*handler)(void *, void *),
			      void (*thread)(void *, void *),
			      void *data, struct virqfd **pvirqfd, int fd);
extern void vfio_virqfd_disable(struct virqfd **pvirqfd);

#endif /* VFIO_H */
