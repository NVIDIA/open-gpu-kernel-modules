// SPDX-License-Identifier: GPL-2.0+
/*
 *  Pvpanic PCI Device Support
 *
 *  Copyright (C) 2021 Oracle.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/slab.h>

#include <uapi/misc/pvpanic.h>

#include "pvpanic.h"

#define PCI_VENDOR_ID_REDHAT             0x1b36
#define PCI_DEVICE_ID_REDHAT_PVPANIC     0x0011

MODULE_AUTHOR("Mihai Carabas <mihai.carabas@oracle.com>");
MODULE_DESCRIPTION("pvpanic device driver ");
MODULE_LICENSE("GPL");

static const struct pci_device_id pvpanic_pci_id_tbl[]  = {
	{ PCI_DEVICE(PCI_VENDOR_ID_REDHAT, PCI_DEVICE_ID_REDHAT_PVPANIC)},
	{}
};

static ssize_t capability_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct pvpanic_instance *pi = dev_get_drvdata(dev);

	return sysfs_emit(buf, "%x\n", pi->capability);
}
static DEVICE_ATTR_RO(capability);

static ssize_t events_show(struct device *dev,  struct device_attribute *attr, char *buf)
{
	struct pvpanic_instance *pi = dev_get_drvdata(dev);

	return sysfs_emit(buf, "%x\n", pi->events);
}

static ssize_t events_store(struct device *dev,  struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct pvpanic_instance *pi = dev_get_drvdata(dev);
	unsigned int tmp;
	int err;

	err = kstrtouint(buf, 16, &tmp);
	if (err)
		return err;

	if ((tmp & pi->capability) != tmp)
		return -EINVAL;

	pi->events = tmp;

	return count;
}
static DEVICE_ATTR_RW(events);

static struct attribute *pvpanic_pci_dev_attrs[] = {
	&dev_attr_capability.attr,
	&dev_attr_events.attr,
	NULL
};
ATTRIBUTE_GROUPS(pvpanic_pci_dev);

static int pvpanic_pci_probe(struct pci_dev *pdev,
			     const struct pci_device_id *ent)
{
	struct device *dev = &pdev->dev;
	struct pvpanic_instance *pi;
	void __iomem *base;
	int ret;

	ret = pci_enable_device(pdev);
	if (ret < 0)
		return ret;

	base = pci_iomap(pdev, 0, 0);
	if (!base)
		return -ENOMEM;

	pi = kmalloc(sizeof(*pi), GFP_ATOMIC);
	if (!pi)
		return -ENOMEM;

	pi->base = base;
	pi->capability = PVPANIC_PANICKED | PVPANIC_CRASH_LOADED;

	/* initlize capability by RDPT */
	pi->capability &= ioread8(base);
	pi->events = pi->capability;

	dev_set_drvdata(dev, pi);

	return pvpanic_probe(pi);
}

static void pvpanic_pci_remove(struct pci_dev *pdev)
{
	struct pvpanic_instance *pi = dev_get_drvdata(&pdev->dev);

	pvpanic_remove(pi);
	iounmap(pi->base);
	kfree(pi);
	pci_disable_device(pdev);
}

static struct pci_driver pvpanic_pci_driver = {
	.name =         "pvpanic-pci",
	.id_table =     pvpanic_pci_id_tbl,
	.probe =        pvpanic_pci_probe,
	.remove =       pvpanic_pci_remove,
	.driver = {
		.dev_groups = pvpanic_pci_dev_groups,
	},
};

module_pci_driver(pvpanic_pci_driver);
