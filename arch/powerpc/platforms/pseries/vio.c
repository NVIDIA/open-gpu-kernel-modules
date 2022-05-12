// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * IBM PowerPC Virtual I/O Infrastructure Support.
 *
 *    Copyright (c) 2003,2008 IBM Corp.
 *     Dave Engebretsen engebret@us.ibm.com
 *     Santiago Leon santil@us.ibm.com
 *     Hollis Blanchard <hollisb@us.ibm.com>
 *     Stephen Rothwell
 *     Robert Jennings <rcjenn@us.ibm.com>
 */

#include <linux/cpu.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/stat.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/console.h>
#include <linux/export.h>
#include <linux/mm.h>
#include <linux/dma-map-ops.h>
#include <linux/kobject.h>
#include <linux/kexec.h>

#include <asm/iommu.h>
#include <asm/dma.h>
#include <asm/vio.h>
#include <asm/prom.h>
#include <asm/firmware.h>
#include <asm/tce.h>
#include <asm/page.h>
#include <asm/hvcall.h>
#include <asm/machdep.h>

static struct vio_dev vio_bus_device  = { /* fake "parent" device */
	.name = "vio",
	.type = "",
	.dev.init_name = "vio",
	.dev.bus = &vio_bus_type,
};

#ifdef CONFIG_PPC_SMLPAR
/**
 * vio_cmo_pool - A pool of IO memory for CMO use
 *
 * @size: The size of the pool in bytes
 * @free: The amount of free memory in the pool
 */
struct vio_cmo_pool {
	size_t size;
	size_t free;
};

/* How many ms to delay queued balance work */
#define VIO_CMO_BALANCE_DELAY 100

/* Portion out IO memory to CMO devices by this chunk size */
#define VIO_CMO_BALANCE_CHUNK 131072

/**
 * vio_cmo_dev_entry - A device that is CMO-enabled and requires entitlement
 *
 * @vio_dev: struct vio_dev pointer
 * @list: pointer to other devices on bus that are being tracked
 */
struct vio_cmo_dev_entry {
	struct vio_dev *viodev;
	struct list_head list;
};

/**
 * vio_cmo - VIO bus accounting structure for CMO entitlement
 *
 * @lock: spinlock for entire structure
 * @balance_q: work queue for balancing system entitlement
 * @device_list: list of CMO-enabled devices requiring entitlement
 * @entitled: total system entitlement in bytes
 * @reserve: pool of memory from which devices reserve entitlement, incl. spare
 * @excess: pool of excess entitlement not needed for device reserves or spare
 * @spare: IO memory for device hotplug functionality
 * @min: minimum necessary for system operation
 * @desired: desired memory for system operation
 * @curr: bytes currently allocated
 * @high: high water mark for IO data usage
 */
static struct vio_cmo {
	spinlock_t lock;
	struct delayed_work balance_q;
	struct list_head device_list;
	size_t entitled;
	struct vio_cmo_pool reserve;
	struct vio_cmo_pool excess;
	size_t spare;
	size_t min;
	size_t desired;
	size_t curr;
	size_t high;
} vio_cmo;

/**
 * vio_cmo_OF_devices - Count the number of OF devices that have DMA windows
 */
static int vio_cmo_num_OF_devs(void)
{
	struct device_node *node_vroot;
	int count = 0;

	/*
	 * Count the number of vdevice entries with an
	 * ibm,my-dma-window OF property
	 */
	node_vroot = of_find_node_by_name(NULL, "vdevice");
	if (node_vroot) {
		struct device_node *of_node;
		struct property *prop;

		for_each_child_of_node(node_vroot, of_node) {
			prop = of_find_property(of_node, "ibm,my-dma-window",
			                       NULL);
			if (prop)
				count++;
		}
	}
	of_node_put(node_vroot);
	return count;
}

/**
 * vio_cmo_alloc - allocate IO memory for CMO-enable devices
 *
 * @viodev: VIO device requesting IO memory
 * @size: size of allocation requested
 *
 * Allocations come from memory reserved for the devices and any excess
 * IO memory available to all devices.  The spare pool used to service
 * hotplug must be equal to %VIO_CMO_MIN_ENT for the excess pool to be
 * made available.
 *
 * Return codes:
 *  0 for successful allocation and -ENOMEM for a failure
 */
static inline int vio_cmo_alloc(struct vio_dev *viodev, size_t size)
{
	unsigned long flags;
	size_t reserve_free = 0;
	size_t excess_free = 0;
	int ret = -ENOMEM;

	spin_lock_irqsave(&vio_cmo.lock, flags);

	/* Determine the amount of free entitlement available in reserve */
	if (viodev->cmo.entitled > viodev->cmo.allocated)
		reserve_free = viodev->cmo.entitled - viodev->cmo.allocated;

	/* If spare is not fulfilled, the excess pool can not be used. */
	if (vio_cmo.spare >= VIO_CMO_MIN_ENT)
		excess_free = vio_cmo.excess.free;

	/* The request can be satisfied */
	if ((reserve_free + excess_free) >= size) {
		vio_cmo.curr += size;
		if (vio_cmo.curr > vio_cmo.high)
			vio_cmo.high = vio_cmo.curr;
		viodev->cmo.allocated += size;
		size -= min(reserve_free, size);
		vio_cmo.excess.free -= size;
		ret = 0;
	}

	spin_unlock_irqrestore(&vio_cmo.lock, flags);
	return ret;
}

/**
 * vio_cmo_dealloc - deallocate IO memory from CMO-enable devices
 * @viodev: VIO device freeing IO memory
 * @size: size of deallocation
 *
 * IO memory is freed by the device back to the correct memory pools.
 * The spare pool is replenished first from either memory pool, then
 * the reserve pool is used to reduce device entitlement, the excess
 * pool is used to increase the reserve pool toward the desired entitlement
 * target, and then the remaining memory is returned to the pools.
 *
 */
static inline void vio_cmo_dealloc(struct vio_dev *viodev, size_t size)
{
	unsigned long flags;
	size_t spare_needed = 0;
	size_t excess_freed = 0;
	size_t reserve_freed = size;
	size_t tmp;
	int balance = 0;

	spin_lock_irqsave(&vio_cmo.lock, flags);
	vio_cmo.curr -= size;

	/* Amount of memory freed from the excess pool */
	if (viodev->cmo.allocated > viodev->cmo.entitled) {
		excess_freed = min(reserve_freed, (viodev->cmo.allocated -
		                                   viodev->cmo.entitled));
		reserve_freed -= excess_freed;
	}

	/* Remove allocation from device */
	viodev->cmo.allocated -= (reserve_freed + excess_freed);

	/* Spare is a subset of the reserve pool, replenish it first. */
	spare_needed = VIO_CMO_MIN_ENT - vio_cmo.spare;

	/*
	 * Replenish the spare in the reserve pool from the excess pool.
	 * This moves entitlement into the reserve pool.
	 */
	if (spare_needed && excess_freed) {
		tmp = min(excess_freed, spare_needed);
		vio_cmo.excess.size -= tmp;
		vio_cmo.reserve.size += tmp;
		vio_cmo.spare += tmp;
		excess_freed -= tmp;
		spare_needed -= tmp;
		balance = 1;
	}

	/*
	 * Replenish the spare in the reserve pool from the reserve pool.
	 * This removes entitlement from the device down to VIO_CMO_MIN_ENT,
	 * if needed, and gives it to the spare pool. The amount of used
	 * memory in this pool does not change.
	 */
	if (spare_needed && reserve_freed) {
		tmp = min3(spare_needed, reserve_freed, (viodev->cmo.entitled - VIO_CMO_MIN_ENT));

		vio_cmo.spare += tmp;
		viodev->cmo.entitled -= tmp;
		reserve_freed -= tmp;
		spare_needed -= tmp;
		balance = 1;
	}

	/*
	 * Increase the reserve pool until the desired allocation is met.
	 * Move an allocation freed from the excess pool into the reserve
	 * pool and schedule a balance operation.
	 */
	if (excess_freed && (vio_cmo.desired > vio_cmo.reserve.size)) {
		tmp = min(excess_freed, (vio_cmo.desired - vio_cmo.reserve.size));

		vio_cmo.excess.size -= tmp;
		vio_cmo.reserve.size += tmp;
		excess_freed -= tmp;
		balance = 1;
	}

	/* Return memory from the excess pool to that pool */
	if (excess_freed)
		vio_cmo.excess.free += excess_freed;

	if (balance)
		schedule_delayed_work(&vio_cmo.balance_q, VIO_CMO_BALANCE_DELAY);
	spin_unlock_irqrestore(&vio_cmo.lock, flags);
}

/**
 * vio_cmo_entitlement_update - Manage system entitlement changes
 *
 * @new_entitlement: new system entitlement to attempt to accommodate
 *
 * Increases in entitlement will be used to fulfill the spare entitlement
 * and the rest is given to the excess pool.  Decreases, if they are
 * possible, come from the excess pool and from unused device entitlement
 *
 * Returns: 0 on success, -ENOMEM when change can not be made
 */
int vio_cmo_entitlement_update(size_t new_entitlement)
{
	struct vio_dev *viodev;
	struct vio_cmo_dev_entry *dev_ent;
	unsigned long flags;
	size_t avail, delta, tmp;

	spin_lock_irqsave(&vio_cmo.lock, flags);

	/* Entitlement increases */
	if (new_entitlement > vio_cmo.entitled) {
		delta = new_entitlement - vio_cmo.entitled;

		/* Fulfill spare allocation */
		if (vio_cmo.spare < VIO_CMO_MIN_ENT) {
			tmp = min(delta, (VIO_CMO_MIN_ENT - vio_cmo.spare));
			vio_cmo.spare += tmp;
			vio_cmo.reserve.size += tmp;
			delta -= tmp;
		}

		/* Remaining new allocation goes to the excess pool */
		vio_cmo.entitled += delta;
		vio_cmo.excess.size += delta;
		vio_cmo.excess.free += delta;

		goto out;
	}

	/* Entitlement decreases */
	delta = vio_cmo.entitled - new_entitlement;
	avail = vio_cmo.excess.free;

	/*
	 * Need to check how much unused entitlement each device can
	 * sacrifice to fulfill entitlement change.
	 */
	list_for_each_entry(dev_ent, &vio_cmo.device_list, list) {
		if (avail >= delta)
			break;

		viodev = dev_ent->viodev;
		if ((viodev->cmo.entitled > viodev->cmo.allocated) &&
		    (viodev->cmo.entitled > VIO_CMO_MIN_ENT))
				avail += viodev->cmo.entitled -
				         max_t(size_t, viodev->cmo.allocated,
				               VIO_CMO_MIN_ENT);
	}

	if (delta <= avail) {
		vio_cmo.entitled -= delta;

		/* Take entitlement from the excess pool first */
		tmp = min(vio_cmo.excess.free, delta);
		vio_cmo.excess.size -= tmp;
		vio_cmo.excess.free -= tmp;
		delta -= tmp;

		/*
		 * Remove all but VIO_CMO_MIN_ENT bytes from devices
		 * until entitlement change is served
		 */
		list_for_each_entry(dev_ent, &vio_cmo.device_list, list) {
			if (!delta)
				break;

			viodev = dev_ent->viodev;
			tmp = 0;
			if ((viodev->cmo.entitled > viodev->cmo.allocated) &&
			    (viodev->cmo.entitled > VIO_CMO_MIN_ENT))
				tmp = viodev->cmo.entitled -
				      max_t(size_t, viodev->cmo.allocated,
				            VIO_CMO_MIN_ENT);
			viodev->cmo.entitled -= min(tmp, delta);
			delta -= min(tmp, delta);
		}
	} else {
		spin_unlock_irqrestore(&vio_cmo.lock, flags);
		return -ENOMEM;
	}

out:
	schedule_delayed_work(&vio_cmo.balance_q, 0);
	spin_unlock_irqrestore(&vio_cmo.lock, flags);
	return 0;
}

/**
 * vio_cmo_balance - Balance entitlement among devices
 *
 * @work: work queue structure for this operation
 *
 * Any system entitlement above the minimum needed for devices, or
 * already allocated to devices, can be distributed to the devices.
 * The list of devices is iterated through to recalculate the desired
 * entitlement level and to determine how much entitlement above the
 * minimum entitlement is allocated to devices.
 *
 * Small chunks of the available entitlement are given to devices until
 * their requirements are fulfilled or there is no entitlement left to give.
 * Upon completion sizes of the reserve and excess pools are calculated.
 *
 * The system minimum entitlement level is also recalculated here.
 * Entitlement will be reserved for devices even after vio_bus_remove to
 * accommodate reloading the driver.  The OF tree is walked to count the
 * number of devices present and this will remove entitlement for devices
 * that have actually left the system after having vio_bus_remove called.
 */
static void vio_cmo_balance(struct work_struct *work)
{
	struct vio_cmo *cmo;
	struct vio_dev *viodev;
	struct vio_cmo_dev_entry *dev_ent;
	unsigned long flags;
	size_t avail = 0, level, chunk, need;
	int devcount = 0, fulfilled;

	cmo = container_of(work, struct vio_cmo, balance_q.work);

	spin_lock_irqsave(&vio_cmo.lock, flags);

	/* Calculate minimum entitlement and fulfill spare */
	cmo->min = vio_cmo_num_OF_devs() * VIO_CMO_MIN_ENT;
	BUG_ON(cmo->min > cmo->entitled);
	cmo->spare = min_t(size_t, VIO_CMO_MIN_ENT, (cmo->entitled - cmo->min));
	cmo->min += cmo->spare;
	cmo->desired = cmo->min;

	/*
	 * Determine how much entitlement is available and reset device
	 * entitlements
	 */
	avail = cmo->entitled - cmo->spare;
	list_for_each_entry(dev_ent, &vio_cmo.device_list, list) {
		viodev = dev_ent->viodev;
		devcount++;
		viodev->cmo.entitled = VIO_CMO_MIN_ENT;
		cmo->desired += (viodev->cmo.desired - VIO_CMO_MIN_ENT);
		avail -= max_t(size_t, viodev->cmo.allocated, VIO_CMO_MIN_ENT);
	}

	/*
	 * Having provided each device with the minimum entitlement, loop
	 * over the devices portioning out the remaining entitlement
	 * until there is nothing left.
	 */
	level = VIO_CMO_MIN_ENT;
	while (avail) {
		fulfilled = 0;
		list_for_each_entry(dev_ent, &vio_cmo.device_list, list) {
			viodev = dev_ent->viodev;

			if (viodev->cmo.desired <= level) {
				fulfilled++;
				continue;
			}

			/*
			 * Give the device up to VIO_CMO_BALANCE_CHUNK
			 * bytes of entitlement, but do not exceed the
			 * desired level of entitlement for the device.
			 */
			chunk = min_t(size_t, avail, VIO_CMO_BALANCE_CHUNK);
			chunk = min(chunk, (viodev->cmo.desired -
			                    viodev->cmo.entitled));
			viodev->cmo.entitled += chunk;

			/*
			 * If the memory for this entitlement increase was
			 * already allocated to the device it does not come
			 * from the available pool being portioned out.
			 */
			need = max(viodev->cmo.allocated, viodev->cmo.entitled)-
			       max(viodev->cmo.allocated, level);
			avail -= need;

		}
		if (fulfilled == devcount)
			break;
		level += VIO_CMO_BALANCE_CHUNK;
	}

	/* Calculate new reserve and excess pool sizes */
	cmo->reserve.size = cmo->min;
	cmo->excess.free = 0;
	cmo->excess.size = 0;
	need = 0;
	list_for_each_entry(dev_ent, &vio_cmo.device_list, list) {
		viodev = dev_ent->viodev;
		/* Calculated reserve size above the minimum entitlement */
		if (viodev->cmo.entitled)
			cmo->reserve.size += (viodev->cmo.entitled -
			                      VIO_CMO_MIN_ENT);
		/* Calculated used excess entitlement */
		if (viodev->cmo.allocated > viodev->cmo.entitled)
			need += viodev->cmo.allocated - viodev->cmo.entitled;
	}
	cmo->excess.size = cmo->entitled - cmo->reserve.size;
	cmo->excess.free = cmo->excess.size - need;

	cancel_delayed_work(to_delayed_work(work));
	spin_unlock_irqrestore(&vio_cmo.lock, flags);
}

static void *vio_dma_iommu_alloc_coherent(struct device *dev, size_t size,
					  dma_addr_t *dma_handle, gfp_t flag,
					  unsigned long attrs)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	void *ret;

	if (vio_cmo_alloc(viodev, roundup(size, PAGE_SIZE))) {
		atomic_inc(&viodev->cmo.allocs_failed);
		return NULL;
	}

	ret = iommu_alloc_coherent(dev, get_iommu_table_base(dev), size,
				    dma_handle, dev->coherent_dma_mask, flag,
				    dev_to_node(dev));
	if (unlikely(ret == NULL)) {
		vio_cmo_dealloc(viodev, roundup(size, PAGE_SIZE));
		atomic_inc(&viodev->cmo.allocs_failed);
	}

	return ret;
}

static void vio_dma_iommu_free_coherent(struct device *dev, size_t size,
					void *vaddr, dma_addr_t dma_handle,
					unsigned long attrs)
{
	struct vio_dev *viodev = to_vio_dev(dev);

	iommu_free_coherent(get_iommu_table_base(dev), size, vaddr, dma_handle);
	vio_cmo_dealloc(viodev, roundup(size, PAGE_SIZE));
}

static dma_addr_t vio_dma_iommu_map_page(struct device *dev, struct page *page,
                                         unsigned long offset, size_t size,
                                         enum dma_data_direction direction,
                                         unsigned long attrs)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	struct iommu_table *tbl = get_iommu_table_base(dev);
	dma_addr_t ret = DMA_MAPPING_ERROR;

	if (vio_cmo_alloc(viodev, roundup(size, IOMMU_PAGE_SIZE(tbl))))
		goto out_fail;
	ret = iommu_map_page(dev, tbl, page, offset, size, dma_get_mask(dev),
			direction, attrs);
	if (unlikely(ret == DMA_MAPPING_ERROR))
		goto out_deallocate;
	return ret;

out_deallocate:
	vio_cmo_dealloc(viodev, roundup(size, IOMMU_PAGE_SIZE(tbl)));
out_fail:
	atomic_inc(&viodev->cmo.allocs_failed);
	return DMA_MAPPING_ERROR;
}

static void vio_dma_iommu_unmap_page(struct device *dev, dma_addr_t dma_handle,
				     size_t size,
				     enum dma_data_direction direction,
				     unsigned long attrs)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	struct iommu_table *tbl = get_iommu_table_base(dev);

	iommu_unmap_page(tbl, dma_handle, size, direction, attrs);
	vio_cmo_dealloc(viodev, roundup(size, IOMMU_PAGE_SIZE(tbl)));
}

static int vio_dma_iommu_map_sg(struct device *dev, struct scatterlist *sglist,
                                int nelems, enum dma_data_direction direction,
                                unsigned long attrs)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	struct iommu_table *tbl = get_iommu_table_base(dev);
	struct scatterlist *sgl;
	int ret, count;
	size_t alloc_size = 0;

	for_each_sg(sglist, sgl, nelems, count)
		alloc_size += roundup(sgl->length, IOMMU_PAGE_SIZE(tbl));

	if (vio_cmo_alloc(viodev, alloc_size))
		goto out_fail;
	ret = ppc_iommu_map_sg(dev, tbl, sglist, nelems, dma_get_mask(dev),
			direction, attrs);
	if (unlikely(!ret))
		goto out_deallocate;

	for_each_sg(sglist, sgl, ret, count)
		alloc_size -= roundup(sgl->dma_length, IOMMU_PAGE_SIZE(tbl));
	if (alloc_size)
		vio_cmo_dealloc(viodev, alloc_size);
	return ret;

out_deallocate:
	vio_cmo_dealloc(viodev, alloc_size);
out_fail:
	atomic_inc(&viodev->cmo.allocs_failed);
	return 0;
}

static void vio_dma_iommu_unmap_sg(struct device *dev,
		struct scatterlist *sglist, int nelems,
		enum dma_data_direction direction,
		unsigned long attrs)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	struct iommu_table *tbl = get_iommu_table_base(dev);
	struct scatterlist *sgl;
	size_t alloc_size = 0;
	int count;

	for_each_sg(sglist, sgl, nelems, count)
		alloc_size += roundup(sgl->dma_length, IOMMU_PAGE_SIZE(tbl));

	ppc_iommu_unmap_sg(tbl, sglist, nelems, direction, attrs);
	vio_cmo_dealloc(viodev, alloc_size);
}

static const struct dma_map_ops vio_dma_mapping_ops = {
	.alloc             = vio_dma_iommu_alloc_coherent,
	.free              = vio_dma_iommu_free_coherent,
	.map_sg            = vio_dma_iommu_map_sg,
	.unmap_sg          = vio_dma_iommu_unmap_sg,
	.map_page          = vio_dma_iommu_map_page,
	.unmap_page        = vio_dma_iommu_unmap_page,
	.dma_supported     = dma_iommu_dma_supported,
	.get_required_mask = dma_iommu_get_required_mask,
	.mmap		   = dma_common_mmap,
	.get_sgtable	   = dma_common_get_sgtable,
	.alloc_pages	   = dma_common_alloc_pages,
	.free_pages	   = dma_common_free_pages,
};

/**
 * vio_cmo_set_dev_desired - Set desired entitlement for a device
 *
 * @viodev: struct vio_dev for device to alter
 * @desired: new desired entitlement level in bytes
 *
 * For use by devices to request a change to their entitlement at runtime or
 * through sysfs.  The desired entitlement level is changed and a balancing
 * of system resources is scheduled to run in the future.
 */
void vio_cmo_set_dev_desired(struct vio_dev *viodev, size_t desired)
{
	unsigned long flags;
	struct vio_cmo_dev_entry *dev_ent;
	int found = 0;

	if (!firmware_has_feature(FW_FEATURE_CMO))
		return;

	spin_lock_irqsave(&vio_cmo.lock, flags);
	if (desired < VIO_CMO_MIN_ENT)
		desired = VIO_CMO_MIN_ENT;

	/*
	 * Changes will not be made for devices not in the device list.
	 * If it is not in the device list, then no driver is loaded
	 * for the device and it can not receive entitlement.
	 */
	list_for_each_entry(dev_ent, &vio_cmo.device_list, list)
		if (viodev == dev_ent->viodev) {
			found = 1;
			break;
		}
	if (!found) {
		spin_unlock_irqrestore(&vio_cmo.lock, flags);
		return;
	}

	/* Increase/decrease in desired device entitlement */
	if (desired >= viodev->cmo.desired) {
		/* Just bump the bus and device values prior to a balance*/
		vio_cmo.desired += desired - viodev->cmo.desired;
		viodev->cmo.desired = desired;
	} else {
		/* Decrease bus and device values for desired entitlement */
		vio_cmo.desired -= viodev->cmo.desired - desired;
		viodev->cmo.desired = desired;
		/*
		 * If less entitlement is desired than current entitlement, move
		 * any reserve memory in the change region to the excess pool.
		 */
		if (viodev->cmo.entitled > desired) {
			vio_cmo.reserve.size -= viodev->cmo.entitled - desired;
			vio_cmo.excess.size += viodev->cmo.entitled - desired;
			/*
			 * If entitlement moving from the reserve pool to the
			 * excess pool is currently unused, add to the excess
			 * free counter.
			 */
			if (viodev->cmo.allocated < viodev->cmo.entitled)
				vio_cmo.excess.free += viodev->cmo.entitled -
				                       max(viodev->cmo.allocated, desired);
			viodev->cmo.entitled = desired;
		}
	}
	schedule_delayed_work(&vio_cmo.balance_q, 0);
	spin_unlock_irqrestore(&vio_cmo.lock, flags);
}

/**
 * vio_cmo_bus_probe - Handle CMO specific bus probe activities
 *
 * @viodev - Pointer to struct vio_dev for device
 *
 * Determine the devices IO memory entitlement needs, attempting
 * to satisfy the system minimum entitlement at first and scheduling
 * a balance operation to take care of the rest at a later time.
 *
 * Returns: 0 on success, -EINVAL when device doesn't support CMO, and
 *          -ENOMEM when entitlement is not available for device or
 *          device entry.
 *
 */
static int vio_cmo_bus_probe(struct vio_dev *viodev)
{
	struct vio_cmo_dev_entry *dev_ent;
	struct device *dev = &viodev->dev;
	struct iommu_table *tbl;
	struct vio_driver *viodrv = to_vio_driver(dev->driver);
	unsigned long flags;
	size_t size;
	bool dma_capable = false;

	tbl = get_iommu_table_base(dev);

	/* A device requires entitlement if it has a DMA window property */
	switch (viodev->family) {
	case VDEVICE:
		if (of_get_property(viodev->dev.of_node,
					"ibm,my-dma-window", NULL))
			dma_capable = true;
		break;
	case PFO:
		dma_capable = false;
		break;
	default:
		dev_warn(dev, "unknown device family: %d\n", viodev->family);
		BUG();
		break;
	}

	/* Configure entitlement for the device. */
	if (dma_capable) {
		/* Check that the driver is CMO enabled and get desired DMA */
		if (!viodrv->get_desired_dma) {
			dev_err(dev, "%s: device driver does not support CMO\n",
			        __func__);
			return -EINVAL;
		}

		viodev->cmo.desired =
			IOMMU_PAGE_ALIGN(viodrv->get_desired_dma(viodev), tbl);
		if (viodev->cmo.desired < VIO_CMO_MIN_ENT)
			viodev->cmo.desired = VIO_CMO_MIN_ENT;
		size = VIO_CMO_MIN_ENT;

		dev_ent = kmalloc(sizeof(struct vio_cmo_dev_entry),
		                  GFP_KERNEL);
		if (!dev_ent)
			return -ENOMEM;

		dev_ent->viodev = viodev;
		spin_lock_irqsave(&vio_cmo.lock, flags);
		list_add(&dev_ent->list, &vio_cmo.device_list);
	} else {
		viodev->cmo.desired = 0;
		size = 0;
		spin_lock_irqsave(&vio_cmo.lock, flags);
	}

	/*
	 * If the needs for vio_cmo.min have not changed since they
	 * were last set, the number of devices in the OF tree has
	 * been constant and the IO memory for this is already in
	 * the reserve pool.
	 */
	if (vio_cmo.min == ((vio_cmo_num_OF_devs() + 1) *
	                    VIO_CMO_MIN_ENT)) {
		/* Updated desired entitlement if device requires it */
		if (size)
			vio_cmo.desired += (viodev->cmo.desired -
		                        VIO_CMO_MIN_ENT);
	} else {
		size_t tmp;

		tmp = vio_cmo.spare + vio_cmo.excess.free;
		if (tmp < size) {
			dev_err(dev, "%s: insufficient free "
			        "entitlement to add device. "
			        "Need %lu, have %lu\n", __func__,
				size, (vio_cmo.spare + tmp));
			spin_unlock_irqrestore(&vio_cmo.lock, flags);
			return -ENOMEM;
		}

		/* Use excess pool first to fulfill request */
		tmp = min(size, vio_cmo.excess.free);
		vio_cmo.excess.free -= tmp;
		vio_cmo.excess.size -= tmp;
		vio_cmo.reserve.size += tmp;

		/* Use spare if excess pool was insufficient */
		vio_cmo.spare -= size - tmp;

		/* Update bus accounting */
		vio_cmo.min += size;
		vio_cmo.desired += viodev->cmo.desired;
	}
	spin_unlock_irqrestore(&vio_cmo.lock, flags);
	return 0;
}

/**
 * vio_cmo_bus_remove - Handle CMO specific bus removal activities
 *
 * @viodev - Pointer to struct vio_dev for device
 *
 * Remove the device from the cmo device list.  The minimum entitlement
 * will be reserved for the device as long as it is in the system.  The
 * rest of the entitlement the device had been allocated will be returned
 * to the system.
 */
static void vio_cmo_bus_remove(struct vio_dev *viodev)
{
	struct vio_cmo_dev_entry *dev_ent;
	unsigned long flags;
	size_t tmp;

	spin_lock_irqsave(&vio_cmo.lock, flags);
	if (viodev->cmo.allocated) {
		dev_err(&viodev->dev, "%s: device had %lu bytes of IO "
		        "allocated after remove operation.\n",
		        __func__, viodev->cmo.allocated);
		BUG();
	}

	/*
	 * Remove the device from the device list being maintained for
	 * CMO enabled devices.
	 */
	list_for_each_entry(dev_ent, &vio_cmo.device_list, list)
		if (viodev == dev_ent->viodev) {
			list_del(&dev_ent->list);
			kfree(dev_ent);
			break;
		}

	/*
	 * Devices may not require any entitlement and they do not need
	 * to be processed.  Otherwise, return the device's entitlement
	 * back to the pools.
	 */
	if (viodev->cmo.entitled) {
		/*
		 * This device has not yet left the OF tree, it's
		 * minimum entitlement remains in vio_cmo.min and
		 * vio_cmo.desired
		 */
		vio_cmo.desired -= (viodev->cmo.desired - VIO_CMO_MIN_ENT);

		/*
		 * Save min allocation for device in reserve as long
		 * as it exists in OF tree as determined by later
		 * balance operation
		 */
		viodev->cmo.entitled -= VIO_CMO_MIN_ENT;

		/* Replenish spare from freed reserve pool */
		if (viodev->cmo.entitled && (vio_cmo.spare < VIO_CMO_MIN_ENT)) {
			tmp = min(viodev->cmo.entitled, (VIO_CMO_MIN_ENT -
			                                 vio_cmo.spare));
			vio_cmo.spare += tmp;
			viodev->cmo.entitled -= tmp;
		}

		/* Remaining reserve goes to excess pool */
		vio_cmo.excess.size += viodev->cmo.entitled;
		vio_cmo.excess.free += viodev->cmo.entitled;
		vio_cmo.reserve.size -= viodev->cmo.entitled;

		/*
		 * Until the device is removed it will keep a
		 * minimum entitlement; this will guarantee that
		 * a module unload/load will result in a success.
		 */
		viodev->cmo.entitled = VIO_CMO_MIN_ENT;
		viodev->cmo.desired = VIO_CMO_MIN_ENT;
		atomic_set(&viodev->cmo.allocs_failed, 0);
	}

	spin_unlock_irqrestore(&vio_cmo.lock, flags);
}

static void vio_cmo_set_dma_ops(struct vio_dev *viodev)
{
	set_dma_ops(&viodev->dev, &vio_dma_mapping_ops);
}

/**
 * vio_cmo_bus_init - CMO entitlement initialization at bus init time
 *
 * Set up the reserve and excess entitlement pools based on available
 * system entitlement and the number of devices in the OF tree that
 * require entitlement in the reserve pool.
 */
static void vio_cmo_bus_init(void)
{
	struct hvcall_mpp_data mpp_data;
	int err;

	memset(&vio_cmo, 0, sizeof(struct vio_cmo));
	spin_lock_init(&vio_cmo.lock);
	INIT_LIST_HEAD(&vio_cmo.device_list);
	INIT_DELAYED_WORK(&vio_cmo.balance_q, vio_cmo_balance);

	/* Get current system entitlement */
	err = h_get_mpp(&mpp_data);

	/*
	 * On failure, continue with entitlement set to 0, will panic()
	 * later when spare is reserved.
	 */
	if (err != H_SUCCESS) {
		printk(KERN_ERR "%s: unable to determine system IO "\
		       "entitlement. (%d)\n", __func__, err);
		vio_cmo.entitled = 0;
	} else {
		vio_cmo.entitled = mpp_data.entitled_mem;
	}

	/* Set reservation and check against entitlement */
	vio_cmo.spare = VIO_CMO_MIN_ENT;
	vio_cmo.reserve.size = vio_cmo.spare;
	vio_cmo.reserve.size += (vio_cmo_num_OF_devs() *
	                         VIO_CMO_MIN_ENT);
	if (vio_cmo.reserve.size > vio_cmo.entitled) {
		printk(KERN_ERR "%s: insufficient system entitlement\n",
		       __func__);
		panic("%s: Insufficient system entitlement", __func__);
	}

	/* Set the remaining accounting variables */
	vio_cmo.excess.size = vio_cmo.entitled - vio_cmo.reserve.size;
	vio_cmo.excess.free = vio_cmo.excess.size;
	vio_cmo.min = vio_cmo.reserve.size;
	vio_cmo.desired = vio_cmo.reserve.size;
}

/* sysfs device functions and data structures for CMO */

#define viodev_cmo_rd_attr(name)                                        \
static ssize_t cmo_##name##_show(struct device *dev,                    \
                                        struct device_attribute *attr,  \
                                         char *buf)                     \
{                                                                       \
	return sprintf(buf, "%lu\n", to_vio_dev(dev)->cmo.name);        \
}

static ssize_t cmo_allocs_failed_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	return sprintf(buf, "%d\n", atomic_read(&viodev->cmo.allocs_failed));
}

static ssize_t cmo_allocs_failed_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	atomic_set(&viodev->cmo.allocs_failed, 0);
	return count;
}

static ssize_t cmo_desired_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	size_t new_desired;
	int ret;

	ret = kstrtoul(buf, 10, &new_desired);
	if (ret)
		return ret;

	vio_cmo_set_dev_desired(viodev, new_desired);
	return count;
}

viodev_cmo_rd_attr(desired);
viodev_cmo_rd_attr(entitled);
viodev_cmo_rd_attr(allocated);

static ssize_t name_show(struct device *, struct device_attribute *, char *);
static ssize_t devspec_show(struct device *, struct device_attribute *, char *);
static ssize_t modalias_show(struct device *dev, struct device_attribute *attr,
			     char *buf);

static struct device_attribute dev_attr_name;
static struct device_attribute dev_attr_devspec;
static struct device_attribute dev_attr_modalias;

static DEVICE_ATTR_RO(cmo_entitled);
static DEVICE_ATTR_RO(cmo_allocated);
static DEVICE_ATTR_RW(cmo_desired);
static DEVICE_ATTR_RW(cmo_allocs_failed);

static struct attribute *vio_cmo_dev_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_devspec.attr,
	&dev_attr_modalias.attr,
	&dev_attr_cmo_entitled.attr,
	&dev_attr_cmo_allocated.attr,
	&dev_attr_cmo_desired.attr,
	&dev_attr_cmo_allocs_failed.attr,
	NULL,
};
ATTRIBUTE_GROUPS(vio_cmo_dev);

/* sysfs bus functions and data structures for CMO */

#define viobus_cmo_rd_attr(name)                                        \
static ssize_t cmo_bus_##name##_show(struct bus_type *bt, char *buf)    \
{                                                                       \
	return sprintf(buf, "%lu\n", vio_cmo.name);                     \
}                                                                       \
static struct bus_attribute bus_attr_cmo_bus_##name =			\
	__ATTR(cmo_##name, S_IRUGO, cmo_bus_##name##_show, NULL)

#define viobus_cmo_pool_rd_attr(name, var)                              \
static ssize_t                                                          \
cmo_##name##_##var##_show(struct bus_type *bt, char *buf)               \
{                                                                       \
	return sprintf(buf, "%lu\n", vio_cmo.name.var);                 \
}                                                                       \
static BUS_ATTR_RO(cmo_##name##_##var)

viobus_cmo_rd_attr(entitled);
viobus_cmo_rd_attr(spare);
viobus_cmo_rd_attr(min);
viobus_cmo_rd_attr(desired);
viobus_cmo_rd_attr(curr);
viobus_cmo_pool_rd_attr(reserve, size);
viobus_cmo_pool_rd_attr(excess, size);
viobus_cmo_pool_rd_attr(excess, free);

static ssize_t cmo_high_show(struct bus_type *bt, char *buf)
{
	return sprintf(buf, "%lu\n", vio_cmo.high);
}

static ssize_t cmo_high_store(struct bus_type *bt, const char *buf,
			      size_t count)
{
	unsigned long flags;

	spin_lock_irqsave(&vio_cmo.lock, flags);
	vio_cmo.high = vio_cmo.curr;
	spin_unlock_irqrestore(&vio_cmo.lock, flags);

	return count;
}
static BUS_ATTR_RW(cmo_high);

static struct attribute *vio_bus_attrs[] = {
	&bus_attr_cmo_bus_entitled.attr,
	&bus_attr_cmo_bus_spare.attr,
	&bus_attr_cmo_bus_min.attr,
	&bus_attr_cmo_bus_desired.attr,
	&bus_attr_cmo_bus_curr.attr,
	&bus_attr_cmo_high.attr,
	&bus_attr_cmo_reserve_size.attr,
	&bus_attr_cmo_excess_size.attr,
	&bus_attr_cmo_excess_free.attr,
	NULL,
};
ATTRIBUTE_GROUPS(vio_bus);

static void vio_cmo_sysfs_init(void)
{
	vio_bus_type.dev_groups = vio_cmo_dev_groups;
	vio_bus_type.bus_groups = vio_bus_groups;
}
#else /* CONFIG_PPC_SMLPAR */
int vio_cmo_entitlement_update(size_t new_entitlement) { return 0; }
void vio_cmo_set_dev_desired(struct vio_dev *viodev, size_t desired) {}
static int vio_cmo_bus_probe(struct vio_dev *viodev) { return 0; }
static void vio_cmo_bus_remove(struct vio_dev *viodev) {}
static void vio_cmo_set_dma_ops(struct vio_dev *viodev) {}
static void vio_cmo_bus_init(void) {}
static void vio_cmo_sysfs_init(void) { }
#endif /* CONFIG_PPC_SMLPAR */
EXPORT_SYMBOL(vio_cmo_entitlement_update);
EXPORT_SYMBOL(vio_cmo_set_dev_desired);


/*
 * Platform Facilities Option (PFO) support
 */

/**
 * vio_h_cop_sync - Perform a synchronous PFO co-processor operation
 *
 * @vdev - Pointer to a struct vio_dev for device
 * @op - Pointer to a struct vio_pfo_op for the operation parameters
 *
 * Calls the hypervisor to synchronously perform the PFO operation
 * described in @op.  In the case of a busy response from the hypervisor,
 * the operation will be re-submitted indefinitely unless a non-zero timeout
 * is specified or an error occurs. The timeout places a limit on when to
 * stop re-submitting a operation, the total time can be exceeded if an
 * operation is in progress.
 *
 * If op->hcall_ret is not NULL, this will be set to the return from the
 * last h_cop_op call or it will be 0 if an error not involving the h_call
 * was encountered.
 *
 * Returns:
 *	0 on success,
 *	-EINVAL if the h_call fails due to an invalid parameter,
 *	-E2BIG if the h_call can not be performed synchronously,
 *	-EBUSY if a timeout is specified and has elapsed,
 *	-EACCES if the memory area for data/status has been rescinded, or
 *	-EPERM if a hardware fault has been indicated
 */
int vio_h_cop_sync(struct vio_dev *vdev, struct vio_pfo_op *op)
{
	struct device *dev = &vdev->dev;
	unsigned long deadline = 0;
	long hret = 0;
	int ret = 0;

	if (op->timeout)
		deadline = jiffies + msecs_to_jiffies(op->timeout);

	while (true) {
		hret = plpar_hcall_norets(H_COP, op->flags,
				vdev->resource_id,
				op->in, op->inlen, op->out,
				op->outlen, op->csbcpb);

		if (hret == H_SUCCESS ||
		    (hret != H_NOT_ENOUGH_RESOURCES &&
		     hret != H_BUSY && hret != H_RESOURCE) ||
		    (op->timeout && time_after(deadline, jiffies)))
			break;

		dev_dbg(dev, "%s: hcall ret(%ld), retrying.\n", __func__, hret);
	}

	switch (hret) {
	case H_SUCCESS:
		ret = 0;
		break;
	case H_OP_MODE:
	case H_TOO_BIG:
		ret = -E2BIG;
		break;
	case H_RESCINDED:
		ret = -EACCES;
		break;
	case H_HARDWARE:
		ret = -EPERM;
		break;
	case H_NOT_ENOUGH_RESOURCES:
	case H_RESOURCE:
	case H_BUSY:
		ret = -EBUSY;
		break;
	default:
		ret = -EINVAL;
		break;
	}

	if (ret)
		dev_dbg(dev, "%s: Sync h_cop_op failure (ret:%d) (hret:%ld)\n",
				__func__, ret, hret);

	op->hcall_err = hret;
	return ret;
}
EXPORT_SYMBOL(vio_h_cop_sync);

static struct iommu_table *vio_build_iommu_table(struct vio_dev *dev)
{
	const __be32 *dma_window;
	struct iommu_table *tbl;
	unsigned long offset, size;

	dma_window = of_get_property(dev->dev.of_node,
				  "ibm,my-dma-window", NULL);
	if (!dma_window)
		return NULL;

	tbl = kzalloc(sizeof(*tbl), GFP_KERNEL);
	if (tbl == NULL)
		return NULL;

	kref_init(&tbl->it_kref);

	of_parse_dma_window(dev->dev.of_node, dma_window,
			    &tbl->it_index, &offset, &size);

	/* TCE table size - measured in tce entries */
	tbl->it_page_shift = IOMMU_PAGE_SHIFT_4K;
	tbl->it_size = size >> tbl->it_page_shift;
	/* offset for VIO should always be 0 */
	tbl->it_offset = offset >> tbl->it_page_shift;
	tbl->it_busno = 0;
	tbl->it_type = TCE_VB;
	tbl->it_blocksize = 16;

	if (firmware_has_feature(FW_FEATURE_LPAR))
		tbl->it_ops = &iommu_table_lpar_multi_ops;
	else
		tbl->it_ops = &iommu_table_pseries_ops;

	return iommu_init_table(tbl, -1, 0, 0);
}

/**
 * vio_match_device: - Tell if a VIO device has a matching
 *			VIO device id structure.
 * @ids:	array of VIO device id structures to search in
 * @dev:	the VIO device structure to match against
 *
 * Used by a driver to check whether a VIO device present in the
 * system is in its list of supported devices. Returns the matching
 * vio_device_id structure or NULL if there is no match.
 */
static const struct vio_device_id *vio_match_device(
		const struct vio_device_id *ids, const struct vio_dev *dev)
{
	while (ids->type[0] != '\0') {
		if ((strncmp(dev->type, ids->type, strlen(ids->type)) == 0) &&
		    of_device_is_compatible(dev->dev.of_node,
					 ids->compat))
			return ids;
		ids++;
	}
	return NULL;
}

/*
 * Convert from struct device to struct vio_dev and pass to driver.
 * dev->driver has already been set by generic code because vio_bus_match
 * succeeded.
 */
static int vio_bus_probe(struct device *dev)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	struct vio_driver *viodrv = to_vio_driver(dev->driver);
	const struct vio_device_id *id;
	int error = -ENODEV;

	if (!viodrv->probe)
		return error;

	id = vio_match_device(viodrv->id_table, viodev);
	if (id) {
		memset(&viodev->cmo, 0, sizeof(viodev->cmo));
		if (firmware_has_feature(FW_FEATURE_CMO)) {
			error = vio_cmo_bus_probe(viodev);
			if (error)
				return error;
		}
		error = viodrv->probe(viodev, id);
		if (error && firmware_has_feature(FW_FEATURE_CMO))
			vio_cmo_bus_remove(viodev);
	}

	return error;
}

/* convert from struct device to struct vio_dev and pass to driver. */
static int vio_bus_remove(struct device *dev)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	struct vio_driver *viodrv = to_vio_driver(dev->driver);
	struct device *devptr;

	/*
	 * Hold a reference to the device after the remove function is called
	 * to allow for CMO accounting cleanup for the device.
	 */
	devptr = get_device(dev);

	if (viodrv->remove)
		viodrv->remove(viodev);

	if (firmware_has_feature(FW_FEATURE_CMO))
		vio_cmo_bus_remove(viodev);

	put_device(devptr);
	return 0;
}

static void vio_bus_shutdown(struct device *dev)
{
	struct vio_dev *viodev = to_vio_dev(dev);
	struct vio_driver *viodrv;

	if (dev->driver) {
		viodrv = to_vio_driver(dev->driver);
		if (viodrv->shutdown)
			viodrv->shutdown(viodev);
		else if (kexec_in_progress)
			vio_bus_remove(dev);
	}
}

/**
 * vio_register_driver: - Register a new vio driver
 * @viodrv:	The vio_driver structure to be registered.
 */
int __vio_register_driver(struct vio_driver *viodrv, struct module *owner,
			  const char *mod_name)
{
	// vio_bus_type is only initialised for pseries
	if (!machine_is(pseries))
		return -ENODEV;

	pr_debug("%s: driver %s registering\n", __func__, viodrv->name);

	/* fill in 'struct driver' fields */
	viodrv->driver.name = viodrv->name;
	viodrv->driver.pm = viodrv->pm;
	viodrv->driver.bus = &vio_bus_type;
	viodrv->driver.owner = owner;
	viodrv->driver.mod_name = mod_name;

	return driver_register(&viodrv->driver);
}
EXPORT_SYMBOL(__vio_register_driver);

/**
 * vio_unregister_driver - Remove registration of vio driver.
 * @viodrv:	The vio_driver struct to be removed form registration
 */
void vio_unregister_driver(struct vio_driver *viodrv)
{
	driver_unregister(&viodrv->driver);
}
EXPORT_SYMBOL(vio_unregister_driver);

/* vio_dev refcount hit 0 */
static void vio_dev_release(struct device *dev)
{
	struct iommu_table *tbl = get_iommu_table_base(dev);

	if (tbl)
		iommu_tce_table_put(tbl);
	of_node_put(dev->of_node);
	kfree(to_vio_dev(dev));
}

/**
 * vio_register_device_node: - Register a new vio device.
 * @of_node:	The OF node for this device.
 *
 * Creates and initializes a vio_dev structure from the data in
 * of_node and adds it to the list of virtual devices.
 * Returns a pointer to the created vio_dev or NULL if node has
 * NULL device_type or compatible fields.
 */
struct vio_dev *vio_register_device_node(struct device_node *of_node)
{
	struct vio_dev *viodev;
	struct device_node *parent_node;
	const __be32 *prop;
	enum vio_dev_family family;

	/*
	 * Determine if this node is a under the /vdevice node or under the
	 * /ibm,platform-facilities node.  This decides the device's family.
	 */
	parent_node = of_get_parent(of_node);
	if (parent_node) {
		if (of_node_is_type(parent_node, "ibm,platform-facilities"))
			family = PFO;
		else if (of_node_is_type(parent_node, "vdevice"))
			family = VDEVICE;
		else {
			pr_warn("%s: parent(%pOF) of %pOFn not recognized.\n",
					__func__,
					parent_node,
					of_node);
			of_node_put(parent_node);
			return NULL;
		}
		of_node_put(parent_node);
	} else {
		pr_warn("%s: could not determine the parent of node %pOFn.\n",
				__func__, of_node);
		return NULL;
	}

	if (family == PFO) {
		if (of_get_property(of_node, "interrupt-controller", NULL)) {
			pr_debug("%s: Skipping the interrupt controller %pOFn.\n",
					__func__, of_node);
			return NULL;
		}
	}

	/* allocate a vio_dev for this node */
	viodev = kzalloc(sizeof(struct vio_dev), GFP_KERNEL);
	if (viodev == NULL) {
		pr_warn("%s: allocation failure for VIO device.\n", __func__);
		return NULL;
	}

	/* we need the 'device_type' property, in order to match with drivers */
	viodev->family = family;
	if (viodev->family == VDEVICE) {
		unsigned int unit_address;

		viodev->type = of_node_get_device_type(of_node);
		if (!viodev->type) {
			pr_warn("%s: node %pOFn is missing the 'device_type' "
					"property.\n", __func__, of_node);
			goto out;
		}

		prop = of_get_property(of_node, "reg", NULL);
		if (prop == NULL) {
			pr_warn("%s: node %pOFn missing 'reg'\n",
					__func__, of_node);
			goto out;
		}
		unit_address = of_read_number(prop, 1);
		dev_set_name(&viodev->dev, "%x", unit_address);
		viodev->irq = irq_of_parse_and_map(of_node, 0);
		viodev->unit_address = unit_address;
	} else {
		/* PFO devices need their resource_id for submitting COP_OPs
		 * This is an optional field for devices, but is required when
		 * performing synchronous ops */
		prop = of_get_property(of_node, "ibm,resource-id", NULL);
		if (prop != NULL)
			viodev->resource_id = of_read_number(prop, 1);

		dev_set_name(&viodev->dev, "%pOFn", of_node);
		viodev->type = dev_name(&viodev->dev);
		viodev->irq = 0;
	}

	viodev->name = of_node->name;
	viodev->dev.of_node = of_node_get(of_node);

	set_dev_node(&viodev->dev, of_node_to_nid(of_node));

	/* init generic 'struct device' fields: */
	viodev->dev.parent = &vio_bus_device.dev;
	viodev->dev.bus = &vio_bus_type;
	viodev->dev.release = vio_dev_release;

	if (of_get_property(viodev->dev.of_node, "ibm,my-dma-window", NULL)) {
		if (firmware_has_feature(FW_FEATURE_CMO))
			vio_cmo_set_dma_ops(viodev);
		else
			set_dma_ops(&viodev->dev, &dma_iommu_ops);

		set_iommu_table_base(&viodev->dev,
				     vio_build_iommu_table(viodev));

		/* needed to ensure proper operation of coherent allocations
		 * later, in case driver doesn't set it explicitly */
		viodev->dev.coherent_dma_mask = DMA_BIT_MASK(64);
		viodev->dev.dma_mask = &viodev->dev.coherent_dma_mask;
	}

	/* register with generic device framework */
	if (device_register(&viodev->dev)) {
		printk(KERN_ERR "%s: failed to register device %s\n",
				__func__, dev_name(&viodev->dev));
		put_device(&viodev->dev);
		return NULL;
	}

	return viodev;

out:	/* Use this exit point for any return prior to device_register */
	kfree(viodev);

	return NULL;
}
EXPORT_SYMBOL(vio_register_device_node);

/*
 * vio_bus_scan_for_devices - Scan OF and register each child device
 * @root_name - OF node name for the root of the subtree to search.
 *		This must be non-NULL
 *
 * Starting from the root node provide, register the device node for
 * each child beneath the root.
 */
static void vio_bus_scan_register_devices(char *root_name)
{
	struct device_node *node_root, *node_child;

	if (!root_name)
		return;

	node_root = of_find_node_by_name(NULL, root_name);
	if (node_root) {

		/*
		 * Create struct vio_devices for each virtual device in
		 * the device tree. Drivers will associate with them later.
		 */
		node_child = of_get_next_child(node_root, NULL);
		while (node_child) {
			vio_register_device_node(node_child);
			node_child = of_get_next_child(node_root, node_child);
		}
		of_node_put(node_root);
	}
}

/**
 * vio_bus_init: - Initialize the virtual IO bus
 */
static int __init vio_bus_init(void)
{
	int err;

	if (firmware_has_feature(FW_FEATURE_CMO))
		vio_cmo_sysfs_init();

	err = bus_register(&vio_bus_type);
	if (err) {
		printk(KERN_ERR "failed to register VIO bus\n");
		return err;
	}

	/*
	 * The fake parent of all vio devices, just to give us
	 * a nice directory
	 */
	err = device_register(&vio_bus_device.dev);
	if (err) {
		printk(KERN_WARNING "%s: device_register returned %i\n",
				__func__, err);
		return err;
	}

	if (firmware_has_feature(FW_FEATURE_CMO))
		vio_cmo_bus_init();

	return 0;
}
machine_postcore_initcall(pseries, vio_bus_init);

static int __init vio_device_init(void)
{
	vio_bus_scan_register_devices("vdevice");
	vio_bus_scan_register_devices("ibm,platform-facilities");

	return 0;
}
machine_device_initcall(pseries, vio_device_init);

static ssize_t name_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", to_vio_dev(dev)->name);
}
static DEVICE_ATTR_RO(name);

static ssize_t devspec_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct device_node *of_node = dev->of_node;

	return sprintf(buf, "%pOF\n", of_node);
}
static DEVICE_ATTR_RO(devspec);

static ssize_t modalias_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	const struct vio_dev *vio_dev = to_vio_dev(dev);
	struct device_node *dn;
	const char *cp;

	dn = dev->of_node;
	if (!dn) {
		strcpy(buf, "\n");
		return strlen(buf);
	}
	cp = of_get_property(dn, "compatible", NULL);
	if (!cp) {
		strcpy(buf, "\n");
		return strlen(buf);
	}

	return sprintf(buf, "vio:T%sS%s\n", vio_dev->type, cp);
}
static DEVICE_ATTR_RO(modalias);

static struct attribute *vio_dev_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_devspec.attr,
	&dev_attr_modalias.attr,
	NULL,
};
ATTRIBUTE_GROUPS(vio_dev);

void vio_unregister_device(struct vio_dev *viodev)
{
	device_unregister(&viodev->dev);
	if (viodev->family == VDEVICE)
		irq_dispose_mapping(viodev->irq);
}
EXPORT_SYMBOL(vio_unregister_device);

static int vio_bus_match(struct device *dev, struct device_driver *drv)
{
	const struct vio_dev *vio_dev = to_vio_dev(dev);
	struct vio_driver *vio_drv = to_vio_driver(drv);
	const struct vio_device_id *ids = vio_drv->id_table;

	return (ids != NULL) && (vio_match_device(ids, vio_dev) != NULL);
}

static int vio_hotplug(struct device *dev, struct kobj_uevent_env *env)
{
	const struct vio_dev *vio_dev = to_vio_dev(dev);
	struct device_node *dn;
	const char *cp;

	dn = dev->of_node;
	if (!dn)
		return -ENODEV;
	cp = of_get_property(dn, "compatible", NULL);
	if (!cp)
		return -ENODEV;

	add_uevent_var(env, "MODALIAS=vio:T%sS%s", vio_dev->type, cp);
	return 0;
}

struct bus_type vio_bus_type = {
	.name = "vio",
	.dev_groups = vio_dev_groups,
	.uevent = vio_hotplug,
	.match = vio_bus_match,
	.probe = vio_bus_probe,
	.remove = vio_bus_remove,
	.shutdown = vio_bus_shutdown,
};

/**
 * vio_get_attribute: - get attribute for virtual device
 * @vdev:	The vio device to get property.
 * @which:	The property/attribute to be extracted.
 * @length:	Pointer to length of returned data size (unused if NULL).
 *
 * Calls prom.c's of_get_property() to return the value of the
 * attribute specified by @which
*/
const void *vio_get_attribute(struct vio_dev *vdev, char *which, int *length)
{
	return of_get_property(vdev->dev.of_node, which, length);
}
EXPORT_SYMBOL(vio_get_attribute);

/* vio_find_name() - internal because only vio.c knows how we formatted the
 * kobject name
 */
static struct vio_dev *vio_find_name(const char *name)
{
	struct device *found;

	found = bus_find_device_by_name(&vio_bus_type, NULL, name);
	if (!found)
		return NULL;

	return to_vio_dev(found);
}

/**
 * vio_find_node - find an already-registered vio_dev
 * @vnode: device_node of the virtual device we're looking for
 *
 * Takes a reference to the embedded struct device which needs to be dropped
 * after use.
 */
struct vio_dev *vio_find_node(struct device_node *vnode)
{
	char kobj_name[20];
	struct device_node *vnode_parent;

	vnode_parent = of_get_parent(vnode);
	if (!vnode_parent)
		return NULL;

	/* construct the kobject name from the device node */
	if (of_node_is_type(vnode_parent, "vdevice")) {
		const __be32 *prop;
		
		prop = of_get_property(vnode, "reg", NULL);
		if (!prop)
			goto out;
		snprintf(kobj_name, sizeof(kobj_name), "%x",
			 (uint32_t)of_read_number(prop, 1));
	} else if (of_node_is_type(vnode_parent, "ibm,platform-facilities"))
		snprintf(kobj_name, sizeof(kobj_name), "%pOFn", vnode);
	else
		goto out;

	of_node_put(vnode_parent);
	return vio_find_name(kobj_name);
out:
	of_node_put(vnode_parent);
	return NULL;
}
EXPORT_SYMBOL(vio_find_node);

int vio_enable_interrupts(struct vio_dev *dev)
{
	int rc = h_vio_signal(dev->unit_address, VIO_IRQ_ENABLE);
	if (rc != H_SUCCESS)
		printk(KERN_ERR "vio: Error 0x%x enabling interrupts\n", rc);
	return rc;
}
EXPORT_SYMBOL(vio_enable_interrupts);

int vio_disable_interrupts(struct vio_dev *dev)
{
	int rc = h_vio_signal(dev->unit_address, VIO_IRQ_DISABLE);
	if (rc != H_SUCCESS)
		printk(KERN_ERR "vio: Error 0x%x disabling interrupts\n", rc);
	return rc;
}
EXPORT_SYMBOL(vio_disable_interrupts);

static int __init vio_init(void)
{
	dma_debug_add_bus(&vio_bus_type);
	return 0;
}
machine_fs_initcall(pseries, vio_init);
