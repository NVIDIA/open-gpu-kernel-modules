// SPDX-License-Identifier: GPL-2.0
/*
 * FPGA Bridge Framework Driver
 *
 *  Copyright (C) 2013-2016 Altera Corporation, All Rights Reserved.
 *  Copyright (C) 2017 Intel Corporation
 */
#include <linux/fpga/fpga-bridge.h>
#include <linux/idr.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

static DEFINE_IDA(fpga_bridge_ida);
static struct class *fpga_bridge_class;

/* Lock for adding/removing bridges to linked lists*/
static DEFINE_SPINLOCK(bridge_list_lock);

/**
 * fpga_bridge_enable - Enable transactions on the bridge
 *
 * @bridge: FPGA bridge
 *
 * Return: 0 for success, error code otherwise.
 */
int fpga_bridge_enable(struct fpga_bridge *bridge)
{
	dev_dbg(&bridge->dev, "enable\n");

	if (bridge->br_ops && bridge->br_ops->enable_set)
		return bridge->br_ops->enable_set(bridge, 1);

	return 0;
}
EXPORT_SYMBOL_GPL(fpga_bridge_enable);

/**
 * fpga_bridge_disable - Disable transactions on the bridge
 *
 * @bridge: FPGA bridge
 *
 * Return: 0 for success, error code otherwise.
 */
int fpga_bridge_disable(struct fpga_bridge *bridge)
{
	dev_dbg(&bridge->dev, "disable\n");

	if (bridge->br_ops && bridge->br_ops->enable_set)
		return bridge->br_ops->enable_set(bridge, 0);

	return 0;
}
EXPORT_SYMBOL_GPL(fpga_bridge_disable);

static struct fpga_bridge *__fpga_bridge_get(struct device *dev,
					     struct fpga_image_info *info)
{
	struct fpga_bridge *bridge;
	int ret = -ENODEV;

	bridge = to_fpga_bridge(dev);

	bridge->info = info;

	if (!mutex_trylock(&bridge->mutex)) {
		ret = -EBUSY;
		goto err_dev;
	}

	if (!try_module_get(dev->parent->driver->owner))
		goto err_ll_mod;

	dev_dbg(&bridge->dev, "get\n");

	return bridge;

err_ll_mod:
	mutex_unlock(&bridge->mutex);
err_dev:
	put_device(dev);
	return ERR_PTR(ret);
}

/**
 * of_fpga_bridge_get - get an exclusive reference to a fpga bridge
 *
 * @np: node pointer of a FPGA bridge
 * @info: fpga image specific information
 *
 * Return fpga_bridge struct if successful.
 * Return -EBUSY if someone already has a reference to the bridge.
 * Return -ENODEV if @np is not a FPGA Bridge.
 */
struct fpga_bridge *of_fpga_bridge_get(struct device_node *np,
				       struct fpga_image_info *info)
{
	struct device *dev;

	dev = class_find_device_by_of_node(fpga_bridge_class, np);
	if (!dev)
		return ERR_PTR(-ENODEV);

	return __fpga_bridge_get(dev, info);
}
EXPORT_SYMBOL_GPL(of_fpga_bridge_get);

static int fpga_bridge_dev_match(struct device *dev, const void *data)
{
	return dev->parent == data;
}

/**
 * fpga_bridge_get - get an exclusive reference to a fpga bridge
 * @dev:	parent device that fpga bridge was registered with
 * @info:	fpga manager info
 *
 * Given a device, get an exclusive reference to a fpga bridge.
 *
 * Return: fpga bridge struct or IS_ERR() condition containing error code.
 */
struct fpga_bridge *fpga_bridge_get(struct device *dev,
				    struct fpga_image_info *info)
{
	struct device *bridge_dev;

	bridge_dev = class_find_device(fpga_bridge_class, NULL, dev,
				       fpga_bridge_dev_match);
	if (!bridge_dev)
		return ERR_PTR(-ENODEV);

	return __fpga_bridge_get(bridge_dev, info);
}
EXPORT_SYMBOL_GPL(fpga_bridge_get);

/**
 * fpga_bridge_put - release a reference to a bridge
 *
 * @bridge: FPGA bridge
 */
void fpga_bridge_put(struct fpga_bridge *bridge)
{
	dev_dbg(&bridge->dev, "put\n");

	bridge->info = NULL;
	module_put(bridge->dev.parent->driver->owner);
	mutex_unlock(&bridge->mutex);
	put_device(&bridge->dev);
}
EXPORT_SYMBOL_GPL(fpga_bridge_put);

/**
 * fpga_bridges_enable - enable bridges in a list
 * @bridge_list: list of FPGA bridges
 *
 * Enable each bridge in the list.  If list is empty, do nothing.
 *
 * Return 0 for success or empty bridge list; return error code otherwise.
 */
int fpga_bridges_enable(struct list_head *bridge_list)
{
	struct fpga_bridge *bridge;
	int ret;

	list_for_each_entry(bridge, bridge_list, node) {
		ret = fpga_bridge_enable(bridge);
		if (ret)
			return ret;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(fpga_bridges_enable);

/**
 * fpga_bridges_disable - disable bridges in a list
 *
 * @bridge_list: list of FPGA bridges
 *
 * Disable each bridge in the list.  If list is empty, do nothing.
 *
 * Return 0 for success or empty bridge list; return error code otherwise.
 */
int fpga_bridges_disable(struct list_head *bridge_list)
{
	struct fpga_bridge *bridge;
	int ret;

	list_for_each_entry(bridge, bridge_list, node) {
		ret = fpga_bridge_disable(bridge);
		if (ret)
			return ret;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(fpga_bridges_disable);

/**
 * fpga_bridges_put - put bridges
 *
 * @bridge_list: list of FPGA bridges
 *
 * For each bridge in the list, put the bridge and remove it from the list.
 * If list is empty, do nothing.
 */
void fpga_bridges_put(struct list_head *bridge_list)
{
	struct fpga_bridge *bridge, *next;
	unsigned long flags;

	list_for_each_entry_safe(bridge, next, bridge_list, node) {
		fpga_bridge_put(bridge);

		spin_lock_irqsave(&bridge_list_lock, flags);
		list_del(&bridge->node);
		spin_unlock_irqrestore(&bridge_list_lock, flags);
	}
}
EXPORT_SYMBOL_GPL(fpga_bridges_put);

/**
 * of_fpga_bridge_get_to_list - get a bridge, add it to a list
 *
 * @np: node pointer of a FPGA bridge
 * @info: fpga image specific information
 * @bridge_list: list of FPGA bridges
 *
 * Get an exclusive reference to the bridge and and it to the list.
 *
 * Return 0 for success, error code from of_fpga_bridge_get() othewise.
 */
int of_fpga_bridge_get_to_list(struct device_node *np,
			       struct fpga_image_info *info,
			       struct list_head *bridge_list)
{
	struct fpga_bridge *bridge;
	unsigned long flags;

	bridge = of_fpga_bridge_get(np, info);
	if (IS_ERR(bridge))
		return PTR_ERR(bridge);

	spin_lock_irqsave(&bridge_list_lock, flags);
	list_add(&bridge->node, bridge_list);
	spin_unlock_irqrestore(&bridge_list_lock, flags);

	return 0;
}
EXPORT_SYMBOL_GPL(of_fpga_bridge_get_to_list);

/**
 * fpga_bridge_get_to_list - given device, get a bridge, add it to a list
 *
 * @dev: FPGA bridge device
 * @info: fpga image specific information
 * @bridge_list: list of FPGA bridges
 *
 * Get an exclusive reference to the bridge and and it to the list.
 *
 * Return 0 for success, error code from fpga_bridge_get() othewise.
 */
int fpga_bridge_get_to_list(struct device *dev,
			    struct fpga_image_info *info,
			    struct list_head *bridge_list)
{
	struct fpga_bridge *bridge;
	unsigned long flags;

	bridge = fpga_bridge_get(dev, info);
	if (IS_ERR(bridge))
		return PTR_ERR(bridge);

	spin_lock_irqsave(&bridge_list_lock, flags);
	list_add(&bridge->node, bridge_list);
	spin_unlock_irqrestore(&bridge_list_lock, flags);

	return 0;
}
EXPORT_SYMBOL_GPL(fpga_bridge_get_to_list);

static ssize_t name_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	struct fpga_bridge *bridge = to_fpga_bridge(dev);

	return sprintf(buf, "%s\n", bridge->name);
}

static ssize_t state_show(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct fpga_bridge *bridge = to_fpga_bridge(dev);
	int enable = 1;

	if (bridge->br_ops && bridge->br_ops->enable_show)
		enable = bridge->br_ops->enable_show(bridge);

	return sprintf(buf, "%s\n", enable ? "enabled" : "disabled");
}

static DEVICE_ATTR_RO(name);
static DEVICE_ATTR_RO(state);

static struct attribute *fpga_bridge_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_state.attr,
	NULL,
};
ATTRIBUTE_GROUPS(fpga_bridge);

/**
 * fpga_bridge_create - create and initialize a struct fpga_bridge
 * @dev:	FPGA bridge device from pdev
 * @name:	FPGA bridge name
 * @br_ops:	pointer to structure of fpga bridge ops
 * @priv:	FPGA bridge private data
 *
 * The caller of this function is responsible for freeing the bridge with
 * fpga_bridge_free().  Using devm_fpga_bridge_create() instead is recommended.
 *
 * Return: struct fpga_bridge or NULL
 */
struct fpga_bridge *fpga_bridge_create(struct device *dev, const char *name,
				       const struct fpga_bridge_ops *br_ops,
				       void *priv)
{
	struct fpga_bridge *bridge;
	int id, ret;

	if (!name || !strlen(name)) {
		dev_err(dev, "Attempt to register with no name!\n");
		return NULL;
	}

	bridge = kzalloc(sizeof(*bridge), GFP_KERNEL);
	if (!bridge)
		return NULL;

	id = ida_simple_get(&fpga_bridge_ida, 0, 0, GFP_KERNEL);
	if (id < 0)
		goto error_kfree;

	mutex_init(&bridge->mutex);
	INIT_LIST_HEAD(&bridge->node);

	bridge->name = name;
	bridge->br_ops = br_ops;
	bridge->priv = priv;

	device_initialize(&bridge->dev);
	bridge->dev.groups = br_ops->groups;
	bridge->dev.class = fpga_bridge_class;
	bridge->dev.parent = dev;
	bridge->dev.of_node = dev->of_node;
	bridge->dev.id = id;

	ret = dev_set_name(&bridge->dev, "br%d", id);
	if (ret)
		goto error_device;

	return bridge;

error_device:
	ida_simple_remove(&fpga_bridge_ida, id);
error_kfree:
	kfree(bridge);

	return NULL;
}
EXPORT_SYMBOL_GPL(fpga_bridge_create);

/**
 * fpga_bridge_free - free a fpga bridge created by fpga_bridge_create()
 * @bridge:	FPGA bridge struct
 */
void fpga_bridge_free(struct fpga_bridge *bridge)
{
	ida_simple_remove(&fpga_bridge_ida, bridge->dev.id);
	kfree(bridge);
}
EXPORT_SYMBOL_GPL(fpga_bridge_free);

static void devm_fpga_bridge_release(struct device *dev, void *res)
{
	struct fpga_bridge *bridge = *(struct fpga_bridge **)res;

	fpga_bridge_free(bridge);
}

/**
 * devm_fpga_bridge_create - create and init a managed struct fpga_bridge
 * @dev:	FPGA bridge device from pdev
 * @name:	FPGA bridge name
 * @br_ops:	pointer to structure of fpga bridge ops
 * @priv:	FPGA bridge private data
 *
 * This function is intended for use in a FPGA bridge driver's probe function.
 * After the bridge driver creates the struct with devm_fpga_bridge_create(), it
 * should register the bridge with fpga_bridge_register().  The bridge driver's
 * remove function should call fpga_bridge_unregister().  The bridge struct
 * allocated with this function will be freed automatically on driver detach.
 * This includes the case of a probe function returning error before calling
 * fpga_bridge_register(), the struct will still get cleaned up.
 *
 *  Return: struct fpga_bridge or NULL
 */
struct fpga_bridge
*devm_fpga_bridge_create(struct device *dev, const char *name,
			 const struct fpga_bridge_ops *br_ops, void *priv)
{
	struct fpga_bridge **ptr, *bridge;

	ptr = devres_alloc(devm_fpga_bridge_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return NULL;

	bridge = fpga_bridge_create(dev, name, br_ops, priv);
	if (!bridge) {
		devres_free(ptr);
	} else {
		*ptr = bridge;
		devres_add(dev, ptr);
	}

	return bridge;
}
EXPORT_SYMBOL_GPL(devm_fpga_bridge_create);

/**
 * fpga_bridge_register - register a FPGA bridge
 *
 * @bridge: FPGA bridge struct
 *
 * Return: 0 for success, error code otherwise.
 */
int fpga_bridge_register(struct fpga_bridge *bridge)
{
	struct device *dev = &bridge->dev;
	int ret;

	ret = device_add(dev);
	if (ret)
		return ret;

	of_platform_populate(dev->of_node, NULL, NULL, dev);

	dev_info(dev->parent, "fpga bridge [%s] registered\n", bridge->name);

	return 0;
}
EXPORT_SYMBOL_GPL(fpga_bridge_register);

/**
 * fpga_bridge_unregister - unregister a FPGA bridge
 *
 * @bridge: FPGA bridge struct
 *
 * This function is intended for use in a FPGA bridge driver's remove function.
 */
void fpga_bridge_unregister(struct fpga_bridge *bridge)
{
	/*
	 * If the low level driver provides a method for putting bridge into
	 * a desired state upon unregister, do it.
	 */
	if (bridge->br_ops && bridge->br_ops->fpga_bridge_remove)
		bridge->br_ops->fpga_bridge_remove(bridge);

	device_unregister(&bridge->dev);
}
EXPORT_SYMBOL_GPL(fpga_bridge_unregister);

static void fpga_bridge_dev_release(struct device *dev)
{
}

static int __init fpga_bridge_dev_init(void)
{
	fpga_bridge_class = class_create(THIS_MODULE, "fpga_bridge");
	if (IS_ERR(fpga_bridge_class))
		return PTR_ERR(fpga_bridge_class);

	fpga_bridge_class->dev_groups = fpga_bridge_groups;
	fpga_bridge_class->dev_release = fpga_bridge_dev_release;

	return 0;
}

static void __exit fpga_bridge_dev_exit(void)
{
	class_destroy(fpga_bridge_class);
	ida_destroy(&fpga_bridge_ida);
}

MODULE_DESCRIPTION("FPGA Bridge Driver");
MODULE_AUTHOR("Alan Tull <atull@kernel.org>");
MODULE_LICENSE("GPL v2");

subsys_initcall(fpga_bridge_dev_init);
module_exit(fpga_bridge_dev_exit);
