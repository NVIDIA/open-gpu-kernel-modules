// SPDX-License-Identifier: GPL-2.0
/* uio_pci_generic - generic UIO driver for PCI 2.3 devices
 *
 * Copyright (C) 2009 Red Hat, Inc.
 * Author: Michael S. Tsirkin <mst@redhat.com>
 *
 * Since the driver does not declare any device ids, you must allocate
 * id and bind the device to the driver yourself.  For example:
 *
 * # echo "8086 10f5" > /sys/bus/pci/drivers/uio_pci_generic/new_id
 * # echo -n 0000:00:19.0 > /sys/bus/pci/drivers/e1000e/unbind
 * # echo -n 0000:00:19.0 > /sys/bus/pci/drivers/uio_pci_generic/bind
 * # ls -l /sys/bus/pci/devices/0000:00:19.0/driver
 * .../0000:00:19.0/driver -> ../../../bus/pci/drivers/uio_pci_generic
 *
 * Driver won't bind to devices which do not support the Interrupt Disable Bit
 * in the command register. All devices compliant to PCI 2.3 (circa 2002) and
 * all compliant PCI Express devices should support this bit.
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/uio_driver.h>

#define DRIVER_VERSION	"0.01.0"
#define DRIVER_AUTHOR	"Michael S. Tsirkin <mst@redhat.com>"
#define DRIVER_DESC	"Generic UIO driver for PCI 2.3 devices"

struct uio_pci_generic_dev {
	struct uio_info info;
	struct pci_dev *pdev;
};

static inline struct uio_pci_generic_dev *
to_uio_pci_generic_dev(struct uio_info *info)
{
	return container_of(info, struct uio_pci_generic_dev, info);
}

static int release(struct uio_info *info, struct inode *inode)
{
	struct uio_pci_generic_dev *gdev = to_uio_pci_generic_dev(info);

	/*
	 * This driver is insecure when used with devices doing DMA, but some
	 * people (mis)use it with such devices.
	 * Let's at least make sure DMA isn't left enabled after the userspace
	 * driver closes the fd.
	 * Note that there's a non-zero chance doing this will wedge the device
	 * at least until reset.
	 */
	pci_clear_master(gdev->pdev);
	return 0;
}

/* Interrupt handler. Read/modify/write the command register to disable
 * the interrupt. */
static irqreturn_t irqhandler(int irq, struct uio_info *info)
{
	struct uio_pci_generic_dev *gdev = to_uio_pci_generic_dev(info);

	if (!pci_check_and_mask_intx(gdev->pdev))
		return IRQ_NONE;

	/* UIO core will signal the user process. */
	return IRQ_HANDLED;
}

static int probe(struct pci_dev *pdev,
			   const struct pci_device_id *id)
{
	struct uio_pci_generic_dev *gdev;
	int err;

	err = pcim_enable_device(pdev);
	if (err) {
		dev_err(&pdev->dev, "%s: pci_enable_device failed: %d\n",
			__func__, err);
		return err;
	}

	if (pdev->irq && !pci_intx_mask_supported(pdev))
		return -ENODEV;

	gdev = devm_kzalloc(&pdev->dev, sizeof(struct uio_pci_generic_dev), GFP_KERNEL);
	if (!gdev)
		return -ENOMEM;

	gdev->info.name = "uio_pci_generic";
	gdev->info.version = DRIVER_VERSION;
	gdev->info.release = release;
	gdev->pdev = pdev;
	if (pdev->irq && (pdev->irq != IRQ_NOTCONNECTED)) {
		gdev->info.irq = pdev->irq;
		gdev->info.irq_flags = IRQF_SHARED;
		gdev->info.handler = irqhandler;
	} else {
		dev_warn(&pdev->dev, "No IRQ assigned to device: "
			 "no support for interrupts?\n");
	}

	return devm_uio_register_device(&pdev->dev, &gdev->info);
}

static struct pci_driver uio_pci_driver = {
	.name = "uio_pci_generic",
	.id_table = NULL, /* only dynamic id's */
	.probe = probe,
};

module_pci_driver(uio_pci_driver);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
