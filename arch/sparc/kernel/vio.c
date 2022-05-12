// SPDX-License-Identifier: GPL-2.0
/* vio.c: Virtual I/O channel devices probing infrastructure.
 *
 *    Copyright (c) 2003-2005 IBM Corp.
 *     Dave Engebretsen engebret@us.ibm.com
 *     Santiago Leon santil@us.ibm.com
 *     Hollis Blanchard <hollisb@us.ibm.com>
 *     Stephen Rothwell
 *
 * Adapted to sparc64 by David S. Miller davem@davemloft.net
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/export.h>
#include <linux/init.h>

#include <asm/mdesc.h>
#include <asm/vio.h>

static const struct vio_device_id *vio_match_device(
	const struct vio_device_id *matches,
	const struct vio_dev *dev)
{
	const char *type, *compat;
	int len;

	type = dev->type;
	compat = dev->compat;
	len = dev->compat_len;

	while (matches->type[0] || matches->compat[0]) {
		int match = 1;
		if (matches->type[0])
			match &= !strcmp(matches->type, type);

		if (matches->compat[0]) {
			match &= len &&
				of_find_in_proplist(compat, matches->compat, len);
		}
		if (match)
			return matches;
		matches++;
	}
	return NULL;
}

static int vio_hotplug(struct device *dev, struct kobj_uevent_env *env)
{
	const struct vio_dev *vio_dev = to_vio_dev(dev);

	add_uevent_var(env, "MODALIAS=vio:T%sS%s", vio_dev->type, vio_dev->compat);
	return 0;
}

static int vio_bus_match(struct device *dev, struct device_driver *drv)
{
	struct vio_dev *vio_dev = to_vio_dev(dev);
	struct vio_driver *vio_drv = to_vio_driver(drv);
	const struct vio_device_id *matches = vio_drv->id_table;

	if (!matches)
		return 0;

	return vio_match_device(matches, vio_dev) != NULL;
}

static int vio_device_probe(struct device *dev)
{
	struct vio_dev *vdev = to_vio_dev(dev);
	struct vio_driver *drv = to_vio_driver(dev->driver);
	const struct vio_device_id *id;

	if (!drv->probe)
		return -ENODEV;

	id = vio_match_device(drv->id_table, vdev);
	if (!id)
		return -ENODEV;

	/* alloc irqs (unless the driver specified not to) */
	if (!drv->no_irq) {
		if (vdev->tx_irq == 0 && vdev->tx_ino != ~0UL)
			vdev->tx_irq = sun4v_build_virq(vdev->cdev_handle,
							vdev->tx_ino);

		if (vdev->rx_irq == 0 && vdev->rx_ino != ~0UL)
			vdev->rx_irq = sun4v_build_virq(vdev->cdev_handle,
							vdev->rx_ino);
	}

	return drv->probe(vdev, id);
}

static int vio_device_remove(struct device *dev)
{
	struct vio_dev *vdev = to_vio_dev(dev);
	struct vio_driver *drv = to_vio_driver(dev->driver);

	if (drv->remove) {
		/*
		 * Ideally, we would remove/deallocate tx/rx virqs
		 * here - however, there are currently no support
		 * routines to do so at the moment. TBD
		 */

		return drv->remove(vdev);
	}

	return 1;
}

static ssize_t devspec_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct vio_dev *vdev = to_vio_dev(dev);
	const char *str = "none";

	if (!strcmp(vdev->type, "vnet-port"))
		str = "vnet";
	else if (!strcmp(vdev->type, "vdc-port"))
		str = "vdisk";

	return sprintf(buf, "%s\n", str);
}
static DEVICE_ATTR_RO(devspec);

static ssize_t type_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct vio_dev *vdev = to_vio_dev(dev);
	return sprintf(buf, "%s\n", vdev->type);
}
static DEVICE_ATTR_RO(type);

static ssize_t modalias_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	const struct vio_dev *vdev = to_vio_dev(dev);

	return sprintf(buf, "vio:T%sS%s\n", vdev->type, vdev->compat);
}
static DEVICE_ATTR_RO(modalias);

static struct attribute *vio_dev_attrs[] = {
	&dev_attr_devspec.attr,
	&dev_attr_type.attr,
	&dev_attr_modalias.attr,
	NULL,
 };
ATTRIBUTE_GROUPS(vio_dev);

static struct bus_type vio_bus_type = {
	.name		= "vio",
	.dev_groups	= vio_dev_groups,
	.uevent         = vio_hotplug,
	.match		= vio_bus_match,
	.probe		= vio_device_probe,
	.remove		= vio_device_remove,
};

int __vio_register_driver(struct vio_driver *viodrv, struct module *owner,
			const char *mod_name)
{
	viodrv->driver.bus = &vio_bus_type;
	viodrv->driver.name = viodrv->name;
	viodrv->driver.owner = owner;
	viodrv->driver.mod_name = mod_name;

	return driver_register(&viodrv->driver);
}
EXPORT_SYMBOL(__vio_register_driver);

void vio_unregister_driver(struct vio_driver *viodrv)
{
	driver_unregister(&viodrv->driver);
}
EXPORT_SYMBOL(vio_unregister_driver);

static void vio_dev_release(struct device *dev)
{
	kfree(to_vio_dev(dev));
}

static ssize_t
show_pciobppath_attr(struct device *dev, struct device_attribute *attr,
		     char *buf)
{
	struct vio_dev *vdev;
	struct device_node *dp;

	vdev = to_vio_dev(dev);
	dp = vdev->dp;

	return scnprintf(buf, PAGE_SIZE, "%pOF\n", dp);
}

static DEVICE_ATTR(obppath, S_IRUSR | S_IRGRP | S_IROTH,
		   show_pciobppath_attr, NULL);

static struct device_node *cdev_node;

static struct vio_dev *root_vdev;
static u64 cdev_cfg_handle;

static const u64 *vio_cfg_handle(struct mdesc_handle *hp, u64 node)
{
	const u64 *cfg_handle = NULL;
	u64 a;

	mdesc_for_each_arc(a, hp, node, MDESC_ARC_TYPE_BACK) {
		u64 target;

		target = mdesc_arc_target(hp, a);
		cfg_handle = mdesc_get_property(hp, target,
						"cfg-handle", NULL);
		if (cfg_handle)
			break;
	}

	return cfg_handle;
}

/**
 * vio_vdev_node() - Find VDEV node in MD
 * @hp:  Handle to the MD
 * @vdev:  Pointer to VDEV
 *
 * Find the node in the current MD which matches the given vio_dev. This
 * must be done dynamically since the node value can change if the MD
 * is updated.
 *
 * NOTE: the MD must be locked, using mdesc_grab(), when calling this routine
 *
 * Return: The VDEV node in MDESC
 */
u64 vio_vdev_node(struct mdesc_handle *hp, struct vio_dev *vdev)
{
	u64 node;

	if (vdev == NULL)
		return MDESC_NODE_NULL;

	node = mdesc_get_node(hp, (const char *)vdev->node_name,
			      &vdev->md_node_info);

	return node;
}
EXPORT_SYMBOL(vio_vdev_node);

static void vio_fill_channel_info(struct mdesc_handle *hp, u64 mp,
				  struct vio_dev *vdev)
{
	u64 a;

	vdev->tx_ino = ~0UL;
	vdev->rx_ino = ~0UL;
	vdev->channel_id = ~0UL;
	mdesc_for_each_arc(a, hp, mp, MDESC_ARC_TYPE_FWD) {
		const u64 *chan_id;
		const u64 *irq;
		u64 target;

		target = mdesc_arc_target(hp, a);

		irq = mdesc_get_property(hp, target, "tx-ino", NULL);
		if (irq)
			vdev->tx_ino = *irq;

		irq = mdesc_get_property(hp, target, "rx-ino", NULL);
		if (irq)
			vdev->rx_ino = *irq;

		chan_id = mdesc_get_property(hp, target, "id", NULL);
		if (chan_id)
			vdev->channel_id = *chan_id;
	}

	vdev->cdev_handle = cdev_cfg_handle;
}

int vio_set_intr(unsigned long dev_ino, int state)
{
	int err;

	err = sun4v_vintr_set_valid(cdev_cfg_handle, dev_ino, state);
	return err;
}
EXPORT_SYMBOL(vio_set_intr);

static struct vio_dev *vio_create_one(struct mdesc_handle *hp, u64 mp,
				      const char *node_name,
				      struct device *parent)
{
	const char *type, *compat;
	struct device_node *dp;
	struct vio_dev *vdev;
	int err, tlen, clen;
	const u64 *id, *cfg_handle;

	type = mdesc_get_property(hp, mp, "device-type", &tlen);
	if (!type) {
		type = mdesc_get_property(hp, mp, "name", &tlen);
		if (!type) {
			type = mdesc_node_name(hp, mp);
			tlen = strlen(type) + 1;
		}
	}
	if (tlen > VIO_MAX_TYPE_LEN || strlen(type) >= VIO_MAX_TYPE_LEN) {
		printk(KERN_ERR "VIO: Type string [%s] is too long.\n",
		       type);
		return NULL;
	}

	id = mdesc_get_property(hp, mp, "id", NULL);

	cfg_handle = vio_cfg_handle(hp, mp);

	compat = mdesc_get_property(hp, mp, "device-type", &clen);
	if (!compat) {
		clen = 0;
	} else if (clen > VIO_MAX_COMPAT_LEN) {
		printk(KERN_ERR "VIO: Compat len %d for [%s] is too long.\n",
		       clen, type);
		return NULL;
	}

	vdev = kzalloc(sizeof(*vdev), GFP_KERNEL);
	if (!vdev) {
		printk(KERN_ERR "VIO: Could not allocate vio_dev\n");
		return NULL;
	}

	vdev->mp = mp;
	memcpy(vdev->type, type, tlen);
	if (compat)
		memcpy(vdev->compat, compat, clen);
	else
		memset(vdev->compat, 0, sizeof(vdev->compat));
	vdev->compat_len = clen;

	vdev->port_id = ~0UL;
	vdev->tx_irq = 0;
	vdev->rx_irq = 0;

	vio_fill_channel_info(hp, mp, vdev);

	if (!id) {
		dev_set_name(&vdev->dev, "%s", type);
		vdev->dev_no = ~(u64)0;
	} else if (!cfg_handle) {
		dev_set_name(&vdev->dev, "%s-%llu", type, *id);
		vdev->dev_no = *id;
	} else {
		dev_set_name(&vdev->dev, "%s-%llu-%llu", type,
			     *cfg_handle, *id);
		vdev->dev_no = *cfg_handle;
		vdev->port_id = *id;
	}

	vdev->dev.parent = parent;
	vdev->dev.bus = &vio_bus_type;
	vdev->dev.release = vio_dev_release;

	if (parent == NULL) {
		dp = cdev_node;
	} else if (to_vio_dev(parent) == root_vdev) {
		for_each_child_of_node(cdev_node, dp) {
			if (of_node_is_type(dp, type))
				break;
		}
	} else {
		dp = to_vio_dev(parent)->dp;
	}
	vdev->dp = dp;

	/*
	 * node_name is NULL for the parent/channel-devices node and
	 * the parent doesn't require the MD node info.
	 */
	if (node_name != NULL) {
		(void) snprintf(vdev->node_name, VIO_MAX_NAME_LEN, "%s",
				node_name);

		err = mdesc_get_node_info(hp, mp, node_name,
					  &vdev->md_node_info);
		if (err) {
			pr_err("VIO: Could not get MD node info %s, err=%d\n",
			       dev_name(&vdev->dev), err);
			kfree(vdev);
			return NULL;
		}
	}

	pr_info("VIO: Adding device %s (tx_ino = %llx, rx_ino = %llx)\n",
		dev_name(&vdev->dev), vdev->tx_ino, vdev->rx_ino);

	err = device_register(&vdev->dev);
	if (err) {
		printk(KERN_ERR "VIO: Could not register device %s, err=%d\n",
		       dev_name(&vdev->dev), err);
		put_device(&vdev->dev);
		return NULL;
	}
	if (vdev->dp)
		err = sysfs_create_file(&vdev->dev.kobj,
					&dev_attr_obppath.attr);

	return vdev;
}

static void vio_add(struct mdesc_handle *hp, u64 node,
		    const char *node_name)
{
	(void) vio_create_one(hp, node, node_name, &root_vdev->dev);
}

struct vio_remove_node_data {
	struct mdesc_handle *hp;
	u64 node;
};

static int vio_md_node_match(struct device *dev, void *arg)
{
	struct vio_dev *vdev = to_vio_dev(dev);
	struct vio_remove_node_data *node_data;
	u64 node;

	node_data = (struct vio_remove_node_data *)arg;

	node = vio_vdev_node(node_data->hp, vdev);

	if (node == node_data->node)
		return 1;
	else
		return 0;
}

static void vio_remove(struct mdesc_handle *hp, u64 node, const char *node_name)
{
	struct vio_remove_node_data node_data;
	struct device *dev;

	node_data.hp = hp;
	node_data.node = node;

	dev = device_find_child(&root_vdev->dev, (void *)&node_data,
				vio_md_node_match);
	if (dev) {
		printk(KERN_INFO "VIO: Removing device %s\n", dev_name(dev));

		device_unregister(dev);
		put_device(dev);
	} else {
		pr_err("VIO: %s node not found in MDESC\n", node_name);
	}
}

static struct mdesc_notifier_client vio_device_notifier = {
	.add		= vio_add,
	.remove		= vio_remove,
	.node_name	= "virtual-device-port",
};

/* We are only interested in domain service ports under the
 * "domain-services" node.  On control nodes there is another port
 * under "openboot" that we should not mess with as aparently that is
 * reserved exclusively for OBP use.
 */
static void vio_add_ds(struct mdesc_handle *hp, u64 node,
		       const char *node_name)
{
	int found;
	u64 a;

	found = 0;
	mdesc_for_each_arc(a, hp, node, MDESC_ARC_TYPE_BACK) {
		u64 target = mdesc_arc_target(hp, a);
		const char *name = mdesc_node_name(hp, target);

		if (!strcmp(name, "domain-services")) {
			found = 1;
			break;
		}
	}

	if (found)
		(void) vio_create_one(hp, node, node_name, &root_vdev->dev);
}

static struct mdesc_notifier_client vio_ds_notifier = {
	.add		= vio_add_ds,
	.remove		= vio_remove,
	.node_name	= "domain-services-port",
};

static const char *channel_devices_node = "channel-devices";
static const char *channel_devices_compat = "SUNW,sun4v-channel-devices";
static const char *cfg_handle_prop = "cfg-handle";

static int __init vio_init(void)
{
	struct mdesc_handle *hp;
	const char *compat;
	const u64 *cfg_handle;
	int err, len;
	u64 root;

	err = bus_register(&vio_bus_type);
	if (err) {
		printk(KERN_ERR "VIO: Could not register bus type err=%d\n",
		       err);
		return err;
	}

	hp = mdesc_grab();
	if (!hp)
		return 0;

	root = mdesc_node_by_name(hp, MDESC_NODE_NULL, channel_devices_node);
	if (root == MDESC_NODE_NULL) {
		printk(KERN_INFO "VIO: No channel-devices MDESC node.\n");
		mdesc_release(hp);
		return 0;
	}

	cdev_node = of_find_node_by_name(NULL, "channel-devices");
	err = -ENODEV;
	if (!cdev_node) {
		printk(KERN_INFO "VIO: No channel-devices OBP node.\n");
		goto out_release;
	}

	compat = mdesc_get_property(hp, root, "compatible", &len);
	if (!compat) {
		printk(KERN_ERR "VIO: Channel devices lacks compatible "
		       "property\n");
		goto out_release;
	}
	if (!of_find_in_proplist(compat, channel_devices_compat, len)) {
		printk(KERN_ERR "VIO: Channel devices node lacks (%s) "
		       "compat entry.\n", channel_devices_compat);
		goto out_release;
	}

	cfg_handle = mdesc_get_property(hp, root, cfg_handle_prop, NULL);
	if (!cfg_handle) {
		printk(KERN_ERR "VIO: Channel devices lacks %s property\n",
		       cfg_handle_prop);
		goto out_release;
	}

	cdev_cfg_handle = *cfg_handle;

	root_vdev = vio_create_one(hp, root, NULL, NULL);
	err = -ENODEV;
	if (!root_vdev) {
		printk(KERN_ERR "VIO: Could not create root device.\n");
		goto out_release;
	}

	mdesc_register_notifier(&vio_device_notifier);
	mdesc_register_notifier(&vio_ds_notifier);

	mdesc_release(hp);

	return err;

out_release:
	mdesc_release(hp);
	return err;
}

postcore_initcall(vio_init);
