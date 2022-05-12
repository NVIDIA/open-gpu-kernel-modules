// SPDX-License-Identifier: GPL-2.0
// Copyright(c) 2015-17 Intel Corporation.

#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/pm_domain.h>
#include <linux/soundwire/sdw.h>
#include <linux/soundwire/sdw_type.h>
#include "bus.h"
#include "sysfs_local.h"

/**
 * sdw_get_device_id - find the matching SoundWire device id
 * @slave: SoundWire Slave Device
 * @drv: SoundWire Slave Driver
 *
 * The match is done by comparing the mfg_id and part_id from the
 * struct sdw_device_id.
 */
static const struct sdw_device_id *
sdw_get_device_id(struct sdw_slave *slave, struct sdw_driver *drv)
{
	const struct sdw_device_id *id;

	for (id = drv->id_table; id && id->mfg_id; id++)
		if (slave->id.mfg_id == id->mfg_id &&
		    slave->id.part_id == id->part_id  &&
		    (!id->sdw_version ||
		     slave->id.sdw_version == id->sdw_version) &&
		    (!id->class_id ||
		     slave->id.class_id == id->class_id))
			return id;

	return NULL;
}

static int sdw_bus_match(struct device *dev, struct device_driver *ddrv)
{
	struct sdw_slave *slave;
	struct sdw_driver *drv;
	int ret = 0;

	if (is_sdw_slave(dev)) {
		slave = dev_to_sdw_dev(dev);
		drv = drv_to_sdw_driver(ddrv);

		ret = !!sdw_get_device_id(slave, drv);
	}
	return ret;
}

int sdw_slave_modalias(const struct sdw_slave *slave, char *buf, size_t size)
{
	/* modalias is sdw:m<mfg_id>p<part_id>v<version>c<class_id> */

	return snprintf(buf, size, "sdw:m%04Xp%04Xv%02Xc%02X\n",
			slave->id.mfg_id, slave->id.part_id,
			slave->id.sdw_version, slave->id.class_id);
}

int sdw_slave_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct sdw_slave *slave = dev_to_sdw_dev(dev);
	char modalias[32];

	sdw_slave_modalias(slave, modalias, sizeof(modalias));

	if (add_uevent_var(env, "MODALIAS=%s", modalias))
		return -ENOMEM;

	return 0;
}

struct bus_type sdw_bus_type = {
	.name = "soundwire",
	.match = sdw_bus_match,
};
EXPORT_SYMBOL_GPL(sdw_bus_type);

static int sdw_drv_probe(struct device *dev)
{
	struct sdw_slave *slave = dev_to_sdw_dev(dev);
	struct sdw_driver *drv = drv_to_sdw_driver(dev->driver);
	const struct sdw_device_id *id;
	const char *name;
	int ret;

	/*
	 * fw description is mandatory to bind
	 */
	if (!dev->fwnode)
		return -ENODEV;

	if (!IS_ENABLED(CONFIG_ACPI) && !dev->of_node)
		return -ENODEV;

	id = sdw_get_device_id(slave, drv);
	if (!id)
		return -ENODEV;

	slave->ops = drv->ops;

	/*
	 * attach to power domain but don't turn on (last arg)
	 */
	ret = dev_pm_domain_attach(dev, false);
	if (ret)
		return ret;

	ret = drv->probe(slave, id);
	if (ret) {
		name = drv->name;
		if (!name)
			name = drv->driver.name;
		dev_err(dev, "Probe of %s failed: %d\n", name, ret);
		dev_pm_domain_detach(dev, false);
		return ret;
	}

	/* device is probed so let's read the properties now */
	if (slave->ops && slave->ops->read_prop)
		slave->ops->read_prop(slave);

	/* init the sysfs as we have properties now */
	ret = sdw_slave_sysfs_init(slave);
	if (ret < 0)
		dev_warn(dev, "Slave sysfs init failed:%d\n", ret);

	/*
	 * Check for valid clk_stop_timeout, use DisCo worst case value of
	 * 300ms
	 *
	 * TODO: check the timeouts and driver removal case
	 */
	if (slave->prop.clk_stop_timeout == 0)
		slave->prop.clk_stop_timeout = 300;

	slave->bus->clk_stop_timeout = max_t(u32, slave->bus->clk_stop_timeout,
					     slave->prop.clk_stop_timeout);

	slave->probed = true;
	complete(&slave->probe_complete);

	dev_dbg(dev, "probe complete\n");

	return 0;
}

static int sdw_drv_remove(struct device *dev)
{
	struct sdw_slave *slave = dev_to_sdw_dev(dev);
	struct sdw_driver *drv = drv_to_sdw_driver(dev->driver);
	int ret = 0;

	if (drv->remove)
		ret = drv->remove(slave);

	dev_pm_domain_detach(dev, false);

	return ret;
}

static void sdw_drv_shutdown(struct device *dev)
{
	struct sdw_slave *slave = dev_to_sdw_dev(dev);
	struct sdw_driver *drv = drv_to_sdw_driver(dev->driver);

	if (drv->shutdown)
		drv->shutdown(slave);
}

/**
 * __sdw_register_driver() - register a SoundWire Slave driver
 * @drv: driver to register
 * @owner: owning module/driver
 *
 * Return: zero on success, else a negative error code.
 */
int __sdw_register_driver(struct sdw_driver *drv, struct module *owner)
{
	const char *name;

	drv->driver.bus = &sdw_bus_type;

	if (!drv->probe) {
		name = drv->name;
		if (!name)
			name = drv->driver.name;

		pr_err("driver %s didn't provide SDW probe routine\n", name);
		return -EINVAL;
	}

	drv->driver.owner = owner;
	drv->driver.probe = sdw_drv_probe;

	if (drv->remove)
		drv->driver.remove = sdw_drv_remove;

	if (drv->shutdown)
		drv->driver.shutdown = sdw_drv_shutdown;

	return driver_register(&drv->driver);
}
EXPORT_SYMBOL_GPL(__sdw_register_driver);

/**
 * sdw_unregister_driver() - unregisters the SoundWire Slave driver
 * @drv: driver to unregister
 */
void sdw_unregister_driver(struct sdw_driver *drv)
{
	driver_unregister(&drv->driver);
}
EXPORT_SYMBOL_GPL(sdw_unregister_driver);

static int __init sdw_bus_init(void)
{
	sdw_debugfs_init();
	return bus_register(&sdw_bus_type);
}

static void __exit sdw_bus_exit(void)
{
	sdw_debugfs_exit();
	bus_unregister(&sdw_bus_type);
}

postcore_initcall(sdw_bus_init);
module_exit(sdw_bus_exit);

MODULE_DESCRIPTION("SoundWire bus");
MODULE_LICENSE("GPL v2");
