// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright(c) 2013-2015 Intel Corporation. All rights reserved.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/libnvdimm.h>
#include <linux/sched/mm.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/fcntl.h>
#include <linux/async.h>
#include <linux/genhd.h>
#include <linux/ndctl.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/nd.h>
#include "nd-core.h"
#include "nd.h"
#include "pfn.h"

int nvdimm_major;
static int nvdimm_bus_major;
struct class *nd_class;
static DEFINE_IDA(nd_ida);

static int to_nd_device_type(struct device *dev)
{
	if (is_nvdimm(dev))
		return ND_DEVICE_DIMM;
	else if (is_memory(dev))
		return ND_DEVICE_REGION_PMEM;
	else if (is_nd_blk(dev))
		return ND_DEVICE_REGION_BLK;
	else if (is_nd_dax(dev))
		return ND_DEVICE_DAX_PMEM;
	else if (is_nd_region(dev->parent))
		return nd_region_to_nstype(to_nd_region(dev->parent));

	return 0;
}

static int nvdimm_bus_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	return add_uevent_var(env, "MODALIAS=" ND_DEVICE_MODALIAS_FMT,
			to_nd_device_type(dev));
}

static struct module *to_bus_provider(struct device *dev)
{
	/* pin bus providers while regions are enabled */
	if (is_nd_region(dev)) {
		struct nvdimm_bus *nvdimm_bus = walk_to_nvdimm_bus(dev);

		return nvdimm_bus->nd_desc->module;
	}
	return NULL;
}

static void nvdimm_bus_probe_start(struct nvdimm_bus *nvdimm_bus)
{
	nvdimm_bus_lock(&nvdimm_bus->dev);
	nvdimm_bus->probe_active++;
	nvdimm_bus_unlock(&nvdimm_bus->dev);
}

static void nvdimm_bus_probe_end(struct nvdimm_bus *nvdimm_bus)
{
	nvdimm_bus_lock(&nvdimm_bus->dev);
	if (--nvdimm_bus->probe_active == 0)
		wake_up(&nvdimm_bus->wait);
	nvdimm_bus_unlock(&nvdimm_bus->dev);
}

static int nvdimm_bus_probe(struct device *dev)
{
	struct nd_device_driver *nd_drv = to_nd_device_driver(dev->driver);
	struct module *provider = to_bus_provider(dev);
	struct nvdimm_bus *nvdimm_bus = walk_to_nvdimm_bus(dev);
	int rc;

	if (!try_module_get(provider))
		return -ENXIO;

	dev_dbg(&nvdimm_bus->dev, "START: %s.probe(%s)\n",
			dev->driver->name, dev_name(dev));

	nvdimm_bus_probe_start(nvdimm_bus);
	debug_nvdimm_lock(dev);
	rc = nd_drv->probe(dev);
	debug_nvdimm_unlock(dev);

	if ((rc == 0 || rc == -EOPNOTSUPP) &&
			dev->parent && is_nd_region(dev->parent))
		nd_region_advance_seeds(to_nd_region(dev->parent), dev);
	nvdimm_bus_probe_end(nvdimm_bus);

	dev_dbg(&nvdimm_bus->dev, "END: %s.probe(%s) = %d\n", dev->driver->name,
			dev_name(dev), rc);

	if (rc != 0)
		module_put(provider);
	return rc;
}

static int nvdimm_bus_remove(struct device *dev)
{
	struct nd_device_driver *nd_drv = to_nd_device_driver(dev->driver);
	struct module *provider = to_bus_provider(dev);
	struct nvdimm_bus *nvdimm_bus = walk_to_nvdimm_bus(dev);

	if (nd_drv->remove) {
		debug_nvdimm_lock(dev);
		nd_drv->remove(dev);
		debug_nvdimm_unlock(dev);
	}

	dev_dbg(&nvdimm_bus->dev, "%s.remove(%s)\n", dev->driver->name,
			dev_name(dev));
	module_put(provider);
	return 0;
}

static void nvdimm_bus_shutdown(struct device *dev)
{
	struct nvdimm_bus *nvdimm_bus = walk_to_nvdimm_bus(dev);
	struct nd_device_driver *nd_drv = NULL;

	if (dev->driver)
		nd_drv = to_nd_device_driver(dev->driver);

	if (nd_drv && nd_drv->shutdown) {
		nd_drv->shutdown(dev);
		dev_dbg(&nvdimm_bus->dev, "%s.shutdown(%s)\n",
				dev->driver->name, dev_name(dev));
	}
}

void nd_device_notify(struct device *dev, enum nvdimm_event event)
{
	nd_device_lock(dev);
	if (dev->driver) {
		struct nd_device_driver *nd_drv;

		nd_drv = to_nd_device_driver(dev->driver);
		if (nd_drv->notify)
			nd_drv->notify(dev, event);
	}
	nd_device_unlock(dev);
}
EXPORT_SYMBOL(nd_device_notify);

void nvdimm_region_notify(struct nd_region *nd_region, enum nvdimm_event event)
{
	struct nvdimm_bus *nvdimm_bus = walk_to_nvdimm_bus(&nd_region->dev);

	if (!nvdimm_bus)
		return;

	/* caller is responsible for holding a reference on the device */
	nd_device_notify(&nd_region->dev, event);
}
EXPORT_SYMBOL_GPL(nvdimm_region_notify);

struct clear_badblocks_context {
	resource_size_t phys, cleared;
};

static int nvdimm_clear_badblocks_region(struct device *dev, void *data)
{
	struct clear_badblocks_context *ctx = data;
	struct nd_region *nd_region;
	resource_size_t ndr_end;
	sector_t sector;

	/* make sure device is a region */
	if (!is_memory(dev))
		return 0;

	nd_region = to_nd_region(dev);
	ndr_end = nd_region->ndr_start + nd_region->ndr_size - 1;

	/* make sure we are in the region */
	if (ctx->phys < nd_region->ndr_start
			|| (ctx->phys + ctx->cleared) > ndr_end)
		return 0;

	sector = (ctx->phys - nd_region->ndr_start) / 512;
	badblocks_clear(&nd_region->bb, sector, ctx->cleared / 512);

	if (nd_region->bb_state)
		sysfs_notify_dirent(nd_region->bb_state);

	return 0;
}

static void nvdimm_clear_badblocks_regions(struct nvdimm_bus *nvdimm_bus,
		phys_addr_t phys, u64 cleared)
{
	struct clear_badblocks_context ctx = {
		.phys = phys,
		.cleared = cleared,
	};

	device_for_each_child(&nvdimm_bus->dev, &ctx,
			nvdimm_clear_badblocks_region);
}

static void nvdimm_account_cleared_poison(struct nvdimm_bus *nvdimm_bus,
		phys_addr_t phys, u64 cleared)
{
	if (cleared > 0)
		badrange_forget(&nvdimm_bus->badrange, phys, cleared);

	if (cleared > 0 && cleared / 512)
		nvdimm_clear_badblocks_regions(nvdimm_bus, phys, cleared);
}

long nvdimm_clear_poison(struct device *dev, phys_addr_t phys,
		unsigned int len)
{
	struct nvdimm_bus *nvdimm_bus = walk_to_nvdimm_bus(dev);
	struct nvdimm_bus_descriptor *nd_desc;
	struct nd_cmd_clear_error clear_err;
	struct nd_cmd_ars_cap ars_cap;
	u32 clear_err_unit, mask;
	unsigned int noio_flag;
	int cmd_rc, rc;

	if (!nvdimm_bus)
		return -ENXIO;

	nd_desc = nvdimm_bus->nd_desc;
	/*
	 * if ndctl does not exist, it's PMEM_LEGACY and
	 * we want to just pretend everything is handled.
	 */
	if (!nd_desc->ndctl)
		return len;

	memset(&ars_cap, 0, sizeof(ars_cap));
	ars_cap.address = phys;
	ars_cap.length = len;
	noio_flag = memalloc_noio_save();
	rc = nd_desc->ndctl(nd_desc, NULL, ND_CMD_ARS_CAP, &ars_cap,
			sizeof(ars_cap), &cmd_rc);
	memalloc_noio_restore(noio_flag);
	if (rc < 0)
		return rc;
	if (cmd_rc < 0)
		return cmd_rc;
	clear_err_unit = ars_cap.clear_err_unit;
	if (!clear_err_unit || !is_power_of_2(clear_err_unit))
		return -ENXIO;

	mask = clear_err_unit - 1;
	if ((phys | len) & mask)
		return -ENXIO;
	memset(&clear_err, 0, sizeof(clear_err));
	clear_err.address = phys;
	clear_err.length = len;
	noio_flag = memalloc_noio_save();
	rc = nd_desc->ndctl(nd_desc, NULL, ND_CMD_CLEAR_ERROR, &clear_err,
			sizeof(clear_err), &cmd_rc);
	memalloc_noio_restore(noio_flag);
	if (rc < 0)
		return rc;
	if (cmd_rc < 0)
		return cmd_rc;

	nvdimm_account_cleared_poison(nvdimm_bus, phys, clear_err.cleared);

	return clear_err.cleared;
}
EXPORT_SYMBOL_GPL(nvdimm_clear_poison);

static int nvdimm_bus_match(struct device *dev, struct device_driver *drv);

static struct bus_type nvdimm_bus_type = {
	.name = "nd",
	.uevent = nvdimm_bus_uevent,
	.match = nvdimm_bus_match,
	.probe = nvdimm_bus_probe,
	.remove = nvdimm_bus_remove,
	.shutdown = nvdimm_bus_shutdown,
};

static void nvdimm_bus_release(struct device *dev)
{
	struct nvdimm_bus *nvdimm_bus;

	nvdimm_bus = container_of(dev, struct nvdimm_bus, dev);
	ida_simple_remove(&nd_ida, nvdimm_bus->id);
	kfree(nvdimm_bus);
}

static const struct device_type nvdimm_bus_dev_type = {
	.release = nvdimm_bus_release,
	.groups = nvdimm_bus_attribute_groups,
};

bool is_nvdimm_bus(struct device *dev)
{
	return dev->type == &nvdimm_bus_dev_type;
}

struct nvdimm_bus *walk_to_nvdimm_bus(struct device *nd_dev)
{
	struct device *dev;

	for (dev = nd_dev; dev; dev = dev->parent)
		if (is_nvdimm_bus(dev))
			break;
	dev_WARN_ONCE(nd_dev, !dev, "invalid dev, not on nd bus\n");
	if (dev)
		return to_nvdimm_bus(dev);
	return NULL;
}

struct nvdimm_bus *to_nvdimm_bus(struct device *dev)
{
	struct nvdimm_bus *nvdimm_bus;

	nvdimm_bus = container_of(dev, struct nvdimm_bus, dev);
	WARN_ON(!is_nvdimm_bus(dev));
	return nvdimm_bus;
}
EXPORT_SYMBOL_GPL(to_nvdimm_bus);

struct nvdimm_bus *nvdimm_to_bus(struct nvdimm *nvdimm)
{
	return to_nvdimm_bus(nvdimm->dev.parent);
}
EXPORT_SYMBOL_GPL(nvdimm_to_bus);

struct nvdimm_bus *nvdimm_bus_register(struct device *parent,
		struct nvdimm_bus_descriptor *nd_desc)
{
	struct nvdimm_bus *nvdimm_bus;
	int rc;

	nvdimm_bus = kzalloc(sizeof(*nvdimm_bus), GFP_KERNEL);
	if (!nvdimm_bus)
		return NULL;
	INIT_LIST_HEAD(&nvdimm_bus->list);
	INIT_LIST_HEAD(&nvdimm_bus->mapping_list);
	init_waitqueue_head(&nvdimm_bus->wait);
	nvdimm_bus->id = ida_simple_get(&nd_ida, 0, 0, GFP_KERNEL);
	if (nvdimm_bus->id < 0) {
		kfree(nvdimm_bus);
		return NULL;
	}
	mutex_init(&nvdimm_bus->reconfig_mutex);
	badrange_init(&nvdimm_bus->badrange);
	nvdimm_bus->nd_desc = nd_desc;
	nvdimm_bus->dev.parent = parent;
	nvdimm_bus->dev.type = &nvdimm_bus_dev_type;
	nvdimm_bus->dev.groups = nd_desc->attr_groups;
	nvdimm_bus->dev.bus = &nvdimm_bus_type;
	nvdimm_bus->dev.of_node = nd_desc->of_node;
	dev_set_name(&nvdimm_bus->dev, "ndbus%d", nvdimm_bus->id);
	rc = device_register(&nvdimm_bus->dev);
	if (rc) {
		dev_dbg(&nvdimm_bus->dev, "registration failed: %d\n", rc);
		goto err;
	}

	return nvdimm_bus;
 err:
	put_device(&nvdimm_bus->dev);
	return NULL;
}
EXPORT_SYMBOL_GPL(nvdimm_bus_register);

void nvdimm_bus_unregister(struct nvdimm_bus *nvdimm_bus)
{
	if (!nvdimm_bus)
		return;
	device_unregister(&nvdimm_bus->dev);
}
EXPORT_SYMBOL_GPL(nvdimm_bus_unregister);

static int child_unregister(struct device *dev, void *data)
{
	/*
	 * the singular ndctl class device per bus needs to be
	 * "device_destroy"ed, so skip it here
	 *
	 * i.e. remove classless children
	 */
	if (dev->class)
		return 0;

	if (is_nvdimm(dev)) {
		struct nvdimm *nvdimm = to_nvdimm(dev);
		bool dev_put = false;

		/* We are shutting down. Make state frozen artificially. */
		nvdimm_bus_lock(dev);
		set_bit(NVDIMM_SECURITY_FROZEN, &nvdimm->sec.flags);
		if (test_and_clear_bit(NDD_WORK_PENDING, &nvdimm->flags))
			dev_put = true;
		nvdimm_bus_unlock(dev);
		cancel_delayed_work_sync(&nvdimm->dwork);
		if (dev_put)
			put_device(dev);
	}
	nd_device_unregister(dev, ND_SYNC);

	return 0;
}

static void free_badrange_list(struct list_head *badrange_list)
{
	struct badrange_entry *bre, *next;

	list_for_each_entry_safe(bre, next, badrange_list, list) {
		list_del(&bre->list);
		kfree(bre);
	}
	list_del_init(badrange_list);
}

static void nd_bus_remove(struct device *dev)
{
	struct nvdimm_bus *nvdimm_bus = to_nvdimm_bus(dev);

	mutex_lock(&nvdimm_bus_list_mutex);
	list_del_init(&nvdimm_bus->list);
	mutex_unlock(&nvdimm_bus_list_mutex);

	wait_event(nvdimm_bus->wait,
			atomic_read(&nvdimm_bus->ioctl_active) == 0);

	nd_synchronize();
	device_for_each_child(&nvdimm_bus->dev, NULL, child_unregister);

	spin_lock(&nvdimm_bus->badrange.lock);
	free_badrange_list(&nvdimm_bus->badrange.list);
	spin_unlock(&nvdimm_bus->badrange.lock);

	nvdimm_bus_destroy_ndctl(nvdimm_bus);
}

static int nd_bus_probe(struct device *dev)
{
	struct nvdimm_bus *nvdimm_bus = to_nvdimm_bus(dev);
	int rc;

	rc = nvdimm_bus_create_ndctl(nvdimm_bus);
	if (rc)
		return rc;

	mutex_lock(&nvdimm_bus_list_mutex);
	list_add_tail(&nvdimm_bus->list, &nvdimm_bus_list);
	mutex_unlock(&nvdimm_bus_list_mutex);

	/* enable bus provider attributes to look up their local context */
	dev_set_drvdata(dev, nvdimm_bus->nd_desc);

	return 0;
}

static struct nd_device_driver nd_bus_driver = {
	.probe = nd_bus_probe,
	.remove = nd_bus_remove,
	.drv = {
		.name = "nd_bus",
		.suppress_bind_attrs = true,
		.bus = &nvdimm_bus_type,
		.owner = THIS_MODULE,
		.mod_name = KBUILD_MODNAME,
	},
};

static int nvdimm_bus_match(struct device *dev, struct device_driver *drv)
{
	struct nd_device_driver *nd_drv = to_nd_device_driver(drv);

	if (is_nvdimm_bus(dev) && nd_drv == &nd_bus_driver)
		return true;

	return !!test_bit(to_nd_device_type(dev), &nd_drv->type);
}

static ASYNC_DOMAIN_EXCLUSIVE(nd_async_domain);

void nd_synchronize(void)
{
	async_synchronize_full_domain(&nd_async_domain);
}
EXPORT_SYMBOL_GPL(nd_synchronize);

static void nd_async_device_register(void *d, async_cookie_t cookie)
{
	struct device *dev = d;

	if (device_add(dev) != 0) {
		dev_err(dev, "%s: failed\n", __func__);
		put_device(dev);
	}
	put_device(dev);
	if (dev->parent)
		put_device(dev->parent);
}

static void nd_async_device_unregister(void *d, async_cookie_t cookie)
{
	struct device *dev = d;

	/* flush bus operations before delete */
	nvdimm_bus_lock(dev);
	nvdimm_bus_unlock(dev);

	device_unregister(dev);
	put_device(dev);
}

void __nd_device_register(struct device *dev)
{
	if (!dev)
		return;

	/*
	 * Ensure that region devices always have their NUMA node set as
	 * early as possible. This way we are able to make certain that
	 * any memory associated with the creation and the creation
	 * itself of the region is associated with the correct node.
	 */
	if (is_nd_region(dev))
		set_dev_node(dev, to_nd_region(dev)->numa_node);

	dev->bus = &nvdimm_bus_type;
	if (dev->parent) {
		get_device(dev->parent);
		if (dev_to_node(dev) == NUMA_NO_NODE)
			set_dev_node(dev, dev_to_node(dev->parent));
	}
	get_device(dev);

	async_schedule_dev_domain(nd_async_device_register, dev,
				  &nd_async_domain);
}

void nd_device_register(struct device *dev)
{
	device_initialize(dev);
	__nd_device_register(dev);
}
EXPORT_SYMBOL(nd_device_register);

void nd_device_unregister(struct device *dev, enum nd_async_mode mode)
{
	bool killed;

	switch (mode) {
	case ND_ASYNC:
		/*
		 * In the async case this is being triggered with the
		 * device lock held and the unregistration work needs to
		 * be moved out of line iff this is thread has won the
		 * race to schedule the deletion.
		 */
		if (!kill_device(dev))
			return;

		get_device(dev);
		async_schedule_domain(nd_async_device_unregister, dev,
				&nd_async_domain);
		break;
	case ND_SYNC:
		/*
		 * In the sync case the device is being unregistered due
		 * to a state change of the parent. Claim the kill state
		 * to synchronize against other unregistration requests,
		 * or otherwise let the async path handle it if the
		 * unregistration was already queued.
		 */
		nd_device_lock(dev);
		killed = kill_device(dev);
		nd_device_unlock(dev);

		if (!killed)
			return;

		nd_synchronize();
		device_unregister(dev);
		break;
	}
}
EXPORT_SYMBOL(nd_device_unregister);

/**
 * __nd_driver_register() - register a region or a namespace driver
 * @nd_drv: driver to register
 * @owner: automatically set by nd_driver_register() macro
 * @mod_name: automatically set by nd_driver_register() macro
 */
int __nd_driver_register(struct nd_device_driver *nd_drv, struct module *owner,
		const char *mod_name)
{
	struct device_driver *drv = &nd_drv->drv;

	if (!nd_drv->type) {
		pr_debug("driver type bitmask not set (%ps)\n",
				__builtin_return_address(0));
		return -EINVAL;
	}

	if (!nd_drv->probe) {
		pr_debug("%s ->probe() must be specified\n", mod_name);
		return -EINVAL;
	}

	drv->bus = &nvdimm_bus_type;
	drv->owner = owner;
	drv->mod_name = mod_name;

	return driver_register(drv);
}
EXPORT_SYMBOL(__nd_driver_register);

void nvdimm_check_and_set_ro(struct gendisk *disk)
{
	struct device *dev = disk_to_dev(disk)->parent;
	struct nd_region *nd_region = to_nd_region(dev->parent);
	int disk_ro = get_disk_ro(disk);

	/* catch the disk up with the region ro state */
	if (disk_ro == nd_region->ro)
		return;

	dev_info(dev, "%s read-%s, marking %s read-%s\n",
		 dev_name(&nd_region->dev), nd_region->ro ? "only" : "write",
		 disk->disk_name, nd_region->ro ? "only" : "write");
	set_disk_ro(disk, nd_region->ro);
}
EXPORT_SYMBOL(nvdimm_check_and_set_ro);

static ssize_t modalias_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	return sprintf(buf, ND_DEVICE_MODALIAS_FMT "\n",
			to_nd_device_type(dev));
}
static DEVICE_ATTR_RO(modalias);

static ssize_t devtype_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%s\n", dev->type->name);
}
static DEVICE_ATTR_RO(devtype);

static struct attribute *nd_device_attributes[] = {
	&dev_attr_modalias.attr,
	&dev_attr_devtype.attr,
	NULL,
};

/*
 * nd_device_attribute_group - generic attributes for all devices on an nd bus
 */
const struct attribute_group nd_device_attribute_group = {
	.attrs = nd_device_attributes,
};

static ssize_t numa_node_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", dev_to_node(dev));
}
static DEVICE_ATTR_RO(numa_node);

static int nvdimm_dev_to_target_node(struct device *dev)
{
	struct device *parent = dev->parent;
	struct nd_region *nd_region = NULL;

	if (is_nd_region(dev))
		nd_region = to_nd_region(dev);
	else if (parent && is_nd_region(parent))
		nd_region = to_nd_region(parent);

	if (!nd_region)
		return NUMA_NO_NODE;
	return nd_region->target_node;
}

static ssize_t target_node_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", nvdimm_dev_to_target_node(dev));
}
static DEVICE_ATTR_RO(target_node);

static struct attribute *nd_numa_attributes[] = {
	&dev_attr_numa_node.attr,
	&dev_attr_target_node.attr,
	NULL,
};

static umode_t nd_numa_attr_visible(struct kobject *kobj, struct attribute *a,
		int n)
{
	struct device *dev = container_of(kobj, typeof(*dev), kobj);

	if (!IS_ENABLED(CONFIG_NUMA))
		return 0;

	if (a == &dev_attr_target_node.attr &&
			nvdimm_dev_to_target_node(dev) == NUMA_NO_NODE)
		return 0;

	return a->mode;
}

/*
 * nd_numa_attribute_group - NUMA attributes for all devices on an nd bus
 */
const struct attribute_group nd_numa_attribute_group = {
	.attrs = nd_numa_attributes,
	.is_visible = nd_numa_attr_visible,
};

int nvdimm_bus_create_ndctl(struct nvdimm_bus *nvdimm_bus)
{
	dev_t devt = MKDEV(nvdimm_bus_major, nvdimm_bus->id);
	struct device *dev;

	dev = device_create(nd_class, &nvdimm_bus->dev, devt, nvdimm_bus,
			"ndctl%d", nvdimm_bus->id);

	if (IS_ERR(dev))
		dev_dbg(&nvdimm_bus->dev, "failed to register ndctl%d: %ld\n",
				nvdimm_bus->id, PTR_ERR(dev));
	return PTR_ERR_OR_ZERO(dev);
}

void nvdimm_bus_destroy_ndctl(struct nvdimm_bus *nvdimm_bus)
{
	device_destroy(nd_class, MKDEV(nvdimm_bus_major, nvdimm_bus->id));
}

static const struct nd_cmd_desc __nd_cmd_dimm_descs[] = {
	[ND_CMD_IMPLEMENTED] = { },
	[ND_CMD_SMART] = {
		.out_num = 2,
		.out_sizes = { 4, 128, },
	},
	[ND_CMD_SMART_THRESHOLD] = {
		.out_num = 2,
		.out_sizes = { 4, 8, },
	},
	[ND_CMD_DIMM_FLAGS] = {
		.out_num = 2,
		.out_sizes = { 4, 4 },
	},
	[ND_CMD_GET_CONFIG_SIZE] = {
		.out_num = 3,
		.out_sizes = { 4, 4, 4, },
	},
	[ND_CMD_GET_CONFIG_DATA] = {
		.in_num = 2,
		.in_sizes = { 4, 4, },
		.out_num = 2,
		.out_sizes = { 4, UINT_MAX, },
	},
	[ND_CMD_SET_CONFIG_DATA] = {
		.in_num = 3,
		.in_sizes = { 4, 4, UINT_MAX, },
		.out_num = 1,
		.out_sizes = { 4, },
	},
	[ND_CMD_VENDOR] = {
		.in_num = 3,
		.in_sizes = { 4, 4, UINT_MAX, },
		.out_num = 3,
		.out_sizes = { 4, 4, UINT_MAX, },
	},
	[ND_CMD_CALL] = {
		.in_num = 2,
		.in_sizes = { sizeof(struct nd_cmd_pkg), UINT_MAX, },
		.out_num = 1,
		.out_sizes = { UINT_MAX, },
	},
};

const struct nd_cmd_desc *nd_cmd_dimm_desc(int cmd)
{
	if (cmd < ARRAY_SIZE(__nd_cmd_dimm_descs))
		return &__nd_cmd_dimm_descs[cmd];
	return NULL;
}
EXPORT_SYMBOL_GPL(nd_cmd_dimm_desc);

static const struct nd_cmd_desc __nd_cmd_bus_descs[] = {
	[ND_CMD_IMPLEMENTED] = { },
	[ND_CMD_ARS_CAP] = {
		.in_num = 2,
		.in_sizes = { 8, 8, },
		.out_num = 4,
		.out_sizes = { 4, 4, 4, 4, },
	},
	[ND_CMD_ARS_START] = {
		.in_num = 5,
		.in_sizes = { 8, 8, 2, 1, 5, },
		.out_num = 2,
		.out_sizes = { 4, 4, },
	},
	[ND_CMD_ARS_STATUS] = {
		.out_num = 3,
		.out_sizes = { 4, 4, UINT_MAX, },
	},
	[ND_CMD_CLEAR_ERROR] = {
		.in_num = 2,
		.in_sizes = { 8, 8, },
		.out_num = 3,
		.out_sizes = { 4, 4, 8, },
	},
	[ND_CMD_CALL] = {
		.in_num = 2,
		.in_sizes = { sizeof(struct nd_cmd_pkg), UINT_MAX, },
		.out_num = 1,
		.out_sizes = { UINT_MAX, },
	},
};

const struct nd_cmd_desc *nd_cmd_bus_desc(int cmd)
{
	if (cmd < ARRAY_SIZE(__nd_cmd_bus_descs))
		return &__nd_cmd_bus_descs[cmd];
	return NULL;
}
EXPORT_SYMBOL_GPL(nd_cmd_bus_desc);

u32 nd_cmd_in_size(struct nvdimm *nvdimm, int cmd,
		const struct nd_cmd_desc *desc, int idx, void *buf)
{
	if (idx >= desc->in_num)
		return UINT_MAX;

	if (desc->in_sizes[idx] < UINT_MAX)
		return desc->in_sizes[idx];

	if (nvdimm && cmd == ND_CMD_SET_CONFIG_DATA && idx == 2) {
		struct nd_cmd_set_config_hdr *hdr = buf;

		return hdr->in_length;
	} else if (nvdimm && cmd == ND_CMD_VENDOR && idx == 2) {
		struct nd_cmd_vendor_hdr *hdr = buf;

		return hdr->in_length;
	} else if (cmd == ND_CMD_CALL) {
		struct nd_cmd_pkg *pkg = buf;

		return pkg->nd_size_in;
	}

	return UINT_MAX;
}
EXPORT_SYMBOL_GPL(nd_cmd_in_size);

u32 nd_cmd_out_size(struct nvdimm *nvdimm, int cmd,
		const struct nd_cmd_desc *desc, int idx, const u32 *in_field,
		const u32 *out_field, unsigned long remainder)
{
	if (idx >= desc->out_num)
		return UINT_MAX;

	if (desc->out_sizes[idx] < UINT_MAX)
		return desc->out_sizes[idx];

	if (nvdimm && cmd == ND_CMD_GET_CONFIG_DATA && idx == 1)
		return in_field[1];
	else if (nvdimm && cmd == ND_CMD_VENDOR && idx == 2)
		return out_field[1];
	else if (!nvdimm && cmd == ND_CMD_ARS_STATUS && idx == 2) {
		/*
		 * Per table 9-276 ARS Data in ACPI 6.1, out_field[1] is
		 * "Size of Output Buffer in bytes, including this
		 * field."
		 */
		if (out_field[1] < 4)
			return 0;
		/*
		 * ACPI 6.1 is ambiguous if 'status' is included in the
		 * output size. If we encounter an output size that
		 * overshoots the remainder by 4 bytes, assume it was
		 * including 'status'.
		 */
		if (out_field[1] - 4 == remainder)
			return remainder;
		return out_field[1] - 8;
	} else if (cmd == ND_CMD_CALL) {
		struct nd_cmd_pkg *pkg = (struct nd_cmd_pkg *) in_field;

		return pkg->nd_size_out;
	}


	return UINT_MAX;
}
EXPORT_SYMBOL_GPL(nd_cmd_out_size);

void wait_nvdimm_bus_probe_idle(struct device *dev)
{
	struct nvdimm_bus *nvdimm_bus = walk_to_nvdimm_bus(dev);

	do {
		if (nvdimm_bus->probe_active == 0)
			break;
		nvdimm_bus_unlock(dev);
		nd_device_unlock(dev);
		wait_event(nvdimm_bus->wait,
				nvdimm_bus->probe_active == 0);
		nd_device_lock(dev);
		nvdimm_bus_lock(dev);
	} while (true);
}

static int nd_pmem_forget_poison_check(struct device *dev, void *data)
{
	struct nd_cmd_clear_error *clear_err =
		(struct nd_cmd_clear_error *)data;
	struct nd_btt *nd_btt = is_nd_btt(dev) ? to_nd_btt(dev) : NULL;
	struct nd_pfn *nd_pfn = is_nd_pfn(dev) ? to_nd_pfn(dev) : NULL;
	struct nd_dax *nd_dax = is_nd_dax(dev) ? to_nd_dax(dev) : NULL;
	struct nd_namespace_common *ndns = NULL;
	struct nd_namespace_io *nsio;
	resource_size_t offset = 0, end_trunc = 0, start, end, pstart, pend;

	if (nd_dax || !dev->driver)
		return 0;

	start = clear_err->address;
	end = clear_err->address + clear_err->cleared - 1;

	if (nd_btt || nd_pfn || nd_dax) {
		if (nd_btt)
			ndns = nd_btt->ndns;
		else if (nd_pfn)
			ndns = nd_pfn->ndns;
		else if (nd_dax)
			ndns = nd_dax->nd_pfn.ndns;

		if (!ndns)
			return 0;
	} else
		ndns = to_ndns(dev);

	nsio = to_nd_namespace_io(&ndns->dev);
	pstart = nsio->res.start + offset;
	pend = nsio->res.end - end_trunc;

	if ((pstart >= start) && (pend <= end))
		return -EBUSY;

	return 0;

}

static int nd_ns_forget_poison_check(struct device *dev, void *data)
{
	return device_for_each_child(dev, data, nd_pmem_forget_poison_check);
}

/* set_config requires an idle interleave set */
static int nd_cmd_clear_to_send(struct nvdimm_bus *nvdimm_bus,
		struct nvdimm *nvdimm, unsigned int cmd, void *data)
{
	struct nvdimm_bus_descriptor *nd_desc = nvdimm_bus->nd_desc;

	/* ask the bus provider if it would like to block this request */
	if (nd_desc->clear_to_send) {
		int rc = nd_desc->clear_to_send(nd_desc, nvdimm, cmd, data);

		if (rc)
			return rc;
	}

	/* require clear error to go through the pmem driver */
	if (!nvdimm && cmd == ND_CMD_CLEAR_ERROR)
		return device_for_each_child(&nvdimm_bus->dev, data,
				nd_ns_forget_poison_check);

	if (!nvdimm || cmd != ND_CMD_SET_CONFIG_DATA)
		return 0;

	/* prevent label manipulation while the kernel owns label updates */
	wait_nvdimm_bus_probe_idle(&nvdimm_bus->dev);
	if (atomic_read(&nvdimm->busy))
		return -EBUSY;
	return 0;
}

static int __nd_ioctl(struct nvdimm_bus *nvdimm_bus, struct nvdimm *nvdimm,
		int read_only, unsigned int ioctl_cmd, unsigned long arg)
{
	struct nvdimm_bus_descriptor *nd_desc = nvdimm_bus->nd_desc;
	const struct nd_cmd_desc *desc = NULL;
	unsigned int cmd = _IOC_NR(ioctl_cmd);
	struct device *dev = &nvdimm_bus->dev;
	void __user *p = (void __user *) arg;
	char *out_env = NULL, *in_env = NULL;
	const char *cmd_name, *dimm_name;
	u32 in_len = 0, out_len = 0;
	unsigned int func = cmd;
	unsigned long cmd_mask;
	struct nd_cmd_pkg pkg;
	int rc, i, cmd_rc;
	void *buf = NULL;
	u64 buf_len = 0;

	if (nvdimm) {
		desc = nd_cmd_dimm_desc(cmd);
		cmd_name = nvdimm_cmd_name(cmd);
		cmd_mask = nvdimm->cmd_mask;
		dimm_name = dev_name(&nvdimm->dev);
	} else {
		desc = nd_cmd_bus_desc(cmd);
		cmd_name = nvdimm_bus_cmd_name(cmd);
		cmd_mask = nd_desc->cmd_mask;
		dimm_name = "bus";
	}

	/* Validate command family support against bus declared support */
	if (cmd == ND_CMD_CALL) {
		unsigned long *mask;

		if (copy_from_user(&pkg, p, sizeof(pkg)))
			return -EFAULT;

		if (nvdimm) {
			if (pkg.nd_family > NVDIMM_FAMILY_MAX)
				return -EINVAL;
			mask = &nd_desc->dimm_family_mask;
		} else {
			if (pkg.nd_family > NVDIMM_BUS_FAMILY_MAX)
				return -EINVAL;
			mask = &nd_desc->bus_family_mask;
		}

		if (!test_bit(pkg.nd_family, mask))
			return -EINVAL;
	}

	if (!desc ||
	    (desc->out_num + desc->in_num == 0) ||
	    cmd > ND_CMD_CALL ||
	    !test_bit(cmd, &cmd_mask))
		return -ENOTTY;

	/* fail write commands (when read-only) */
	if (read_only)
		switch (cmd) {
		case ND_CMD_VENDOR:
		case ND_CMD_SET_CONFIG_DATA:
		case ND_CMD_ARS_START:
		case ND_CMD_CLEAR_ERROR:
		case ND_CMD_CALL:
			dev_dbg(dev, "'%s' command while read-only.\n",
					nvdimm ? nvdimm_cmd_name(cmd)
					: nvdimm_bus_cmd_name(cmd));
			return -EPERM;
		default:
			break;
		}

	/* process an input envelope */
	in_env = kzalloc(ND_CMD_MAX_ENVELOPE, GFP_KERNEL);
	if (!in_env)
		return -ENOMEM;
	for (i = 0; i < desc->in_num; i++) {
		u32 in_size, copy;

		in_size = nd_cmd_in_size(nvdimm, cmd, desc, i, in_env);
		if (in_size == UINT_MAX) {
			dev_err(dev, "%s:%s unknown input size cmd: %s field: %d\n",
					__func__, dimm_name, cmd_name, i);
			rc = -ENXIO;
			goto out;
		}
		if (in_len < ND_CMD_MAX_ENVELOPE)
			copy = min_t(u32, ND_CMD_MAX_ENVELOPE - in_len, in_size);
		else
			copy = 0;
		if (copy && copy_from_user(&in_env[in_len], p + in_len, copy)) {
			rc = -EFAULT;
			goto out;
		}
		in_len += in_size;
	}

	if (cmd == ND_CMD_CALL) {
		func = pkg.nd_command;
		dev_dbg(dev, "%s, idx: %llu, in: %u, out: %u, len %llu\n",
				dimm_name, pkg.nd_command,
				in_len, out_len, buf_len);
	}

	/* process an output envelope */
	out_env = kzalloc(ND_CMD_MAX_ENVELOPE, GFP_KERNEL);
	if (!out_env) {
		rc = -ENOMEM;
		goto out;
	}

	for (i = 0; i < desc->out_num; i++) {
		u32 out_size = nd_cmd_out_size(nvdimm, cmd, desc, i,
				(u32 *) in_env, (u32 *) out_env, 0);
		u32 copy;

		if (out_size == UINT_MAX) {
			dev_dbg(dev, "%s unknown output size cmd: %s field: %d\n",
					dimm_name, cmd_name, i);
			rc = -EFAULT;
			goto out;
		}
		if (out_len < ND_CMD_MAX_ENVELOPE)
			copy = min_t(u32, ND_CMD_MAX_ENVELOPE - out_len, out_size);
		else
			copy = 0;
		if (copy && copy_from_user(&out_env[out_len],
					p + in_len + out_len, copy)) {
			rc = -EFAULT;
			goto out;
		}
		out_len += out_size;
	}

	buf_len = (u64) out_len + (u64) in_len;
	if (buf_len > ND_IOCTL_MAX_BUFLEN) {
		dev_dbg(dev, "%s cmd: %s buf_len: %llu > %d\n", dimm_name,
				cmd_name, buf_len, ND_IOCTL_MAX_BUFLEN);
		rc = -EINVAL;
		goto out;
	}

	buf = vmalloc(buf_len);
	if (!buf) {
		rc = -ENOMEM;
		goto out;
	}

	if (copy_from_user(buf, p, buf_len)) {
		rc = -EFAULT;
		goto out;
	}

	nd_device_lock(dev);
	nvdimm_bus_lock(dev);
	rc = nd_cmd_clear_to_send(nvdimm_bus, nvdimm, func, buf);
	if (rc)
		goto out_unlock;

	rc = nd_desc->ndctl(nd_desc, nvdimm, cmd, buf, buf_len, &cmd_rc);
	if (rc < 0)
		goto out_unlock;

	if (!nvdimm && cmd == ND_CMD_CLEAR_ERROR && cmd_rc >= 0) {
		struct nd_cmd_clear_error *clear_err = buf;

		nvdimm_account_cleared_poison(nvdimm_bus, clear_err->address,
				clear_err->cleared);
	}

	if (copy_to_user(p, buf, buf_len))
		rc = -EFAULT;

out_unlock:
	nvdimm_bus_unlock(dev);
	nd_device_unlock(dev);
out:
	kfree(in_env);
	kfree(out_env);
	vfree(buf);
	return rc;
}

enum nd_ioctl_mode {
	BUS_IOCTL,
	DIMM_IOCTL,
};

static int match_dimm(struct device *dev, void *data)
{
	long id = (long) data;

	if (is_nvdimm(dev)) {
		struct nvdimm *nvdimm = to_nvdimm(dev);

		return nvdimm->id == id;
	}

	return 0;
}

static long nd_ioctl(struct file *file, unsigned int cmd, unsigned long arg,
		enum nd_ioctl_mode mode)

{
	struct nvdimm_bus *nvdimm_bus, *found = NULL;
	long id = (long) file->private_data;
	struct nvdimm *nvdimm = NULL;
	int rc, ro;

	ro = ((file->f_flags & O_ACCMODE) == O_RDONLY);
	mutex_lock(&nvdimm_bus_list_mutex);
	list_for_each_entry(nvdimm_bus, &nvdimm_bus_list, list) {
		if (mode == DIMM_IOCTL) {
			struct device *dev;

			dev = device_find_child(&nvdimm_bus->dev,
					file->private_data, match_dimm);
			if (!dev)
				continue;
			nvdimm = to_nvdimm(dev);
			found = nvdimm_bus;
		} else if (nvdimm_bus->id == id) {
			found = nvdimm_bus;
		}

		if (found) {
			atomic_inc(&nvdimm_bus->ioctl_active);
			break;
		}
	}
	mutex_unlock(&nvdimm_bus_list_mutex);

	if (!found)
		return -ENXIO;

	nvdimm_bus = found;
	rc = __nd_ioctl(nvdimm_bus, nvdimm, ro, cmd, arg);

	if (nvdimm)
		put_device(&nvdimm->dev);
	if (atomic_dec_and_test(&nvdimm_bus->ioctl_active))
		wake_up(&nvdimm_bus->wait);

	return rc;
}

static long bus_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return nd_ioctl(file, cmd, arg, BUS_IOCTL);
}

static long dimm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return nd_ioctl(file, cmd, arg, DIMM_IOCTL);
}

static int nd_open(struct inode *inode, struct file *file)
{
	long minor = iminor(inode);

	file->private_data = (void *) minor;
	return 0;
}

static const struct file_operations nvdimm_bus_fops = {
	.owner = THIS_MODULE,
	.open = nd_open,
	.unlocked_ioctl = bus_ioctl,
	.compat_ioctl = compat_ptr_ioctl,
	.llseek = noop_llseek,
};

static const struct file_operations nvdimm_fops = {
	.owner = THIS_MODULE,
	.open = nd_open,
	.unlocked_ioctl = dimm_ioctl,
	.compat_ioctl = compat_ptr_ioctl,
	.llseek = noop_llseek,
};

int __init nvdimm_bus_init(void)
{
	int rc;

	rc = bus_register(&nvdimm_bus_type);
	if (rc)
		return rc;

	rc = register_chrdev(0, "ndctl", &nvdimm_bus_fops);
	if (rc < 0)
		goto err_bus_chrdev;
	nvdimm_bus_major = rc;

	rc = register_chrdev(0, "dimmctl", &nvdimm_fops);
	if (rc < 0)
		goto err_dimm_chrdev;
	nvdimm_major = rc;

	nd_class = class_create(THIS_MODULE, "nd");
	if (IS_ERR(nd_class)) {
		rc = PTR_ERR(nd_class);
		goto err_class;
	}

	rc = driver_register(&nd_bus_driver.drv);
	if (rc)
		goto err_nd_bus;

	return 0;

 err_nd_bus:
	class_destroy(nd_class);
 err_class:
	unregister_chrdev(nvdimm_major, "dimmctl");
 err_dimm_chrdev:
	unregister_chrdev(nvdimm_bus_major, "ndctl");
 err_bus_chrdev:
	bus_unregister(&nvdimm_bus_type);

	return rc;
}

void nvdimm_bus_exit(void)
{
	driver_unregister(&nd_bus_driver.drv);
	class_destroy(nd_class);
	unregister_chrdev(nvdimm_bus_major, "ndctl");
	unregister_chrdev(nvdimm_major, "dimmctl");
	bus_unregister(&nvdimm_bus_type);
	ida_destroy(&nd_ida);
}
