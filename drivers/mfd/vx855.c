// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Linux multi-function-device driver (MFD) for the integrated peripherals
 * of the VIA VX855 chipset
 *
 * Copyright (C) 2009 VIA Technologies, Inc.
 * Copyright (C) 2010 One Laptop per Child
 * Author: Harald Welte <HaraldWelte@viatech.com>
 * All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/pci.h>
#include <linux/mfd/core.h>

/* offset into pci config space indicating the 16bit register containing
 * the power management IO space base */
#define VX855_CFG_PMIO_OFFSET	0x88

/* ACPI I/O Space registers */
#define VX855_PMIO_ACPI		0x00
#define VX855_PMIO_ACPI_LEN	0x0b

/* Processor Power Management */
#define VX855_PMIO_PPM		0x10
#define VX855_PMIO_PPM_LEN	0x08

/* General Purpose Power Management */
#define VX855_PMIO_GPPM		0x20
#define VX855_PMIO_R_GPI	0x48
#define VX855_PMIO_R_GPO	0x4c
#define VX855_PMIO_GPPM_LEN	0x33

#define VSPIC_MMIO_SIZE	0x1000

static struct resource vx855_gpio_resources[] = {
	{
		.flags = IORESOURCE_IO,
	},
	{
		.flags = IORESOURCE_IO,
	},
};

static const struct mfd_cell vx855_cells[] = {
	{
		.name = "vx855_gpio",
		.num_resources = ARRAY_SIZE(vx855_gpio_resources),
		.resources = vx855_gpio_resources,

		/* we must ignore resource conflicts, for reasons outlined in
		 * the vx855_gpio driver */
		.ignore_resource_conflicts = true,
	},
};

static int vx855_probe(struct pci_dev *pdev,
				 const struct pci_device_id *id)
{
	int ret;
	u16 gpio_io_offset;

	ret = pci_enable_device(pdev);
	if (ret)
		return -ENODEV;

	pci_read_config_word(pdev, VX855_CFG_PMIO_OFFSET, &gpio_io_offset);
	if (!gpio_io_offset) {
		dev_warn(&pdev->dev,
			"BIOS did not assign PMIO base offset?!?\n");
		ret = -ENODEV;
		goto out;
	}

	/* mask out the lowest seven bits, as they are always zero, but
	 * hardware returns them as 0x01 */
	gpio_io_offset &= 0xff80;

	/* As the region identified here includes many non-GPIO things, we
	 * only work with the specific registers that concern us. */
	vx855_gpio_resources[0].start = gpio_io_offset + VX855_PMIO_R_GPI;
	vx855_gpio_resources[0].end = vx855_gpio_resources[0].start + 3;
	vx855_gpio_resources[1].start = gpio_io_offset + VX855_PMIO_R_GPO;
	vx855_gpio_resources[1].end = vx855_gpio_resources[1].start + 3;

	ret = mfd_add_devices(&pdev->dev, -1, vx855_cells, ARRAY_SIZE(vx855_cells),
			NULL, 0, NULL);

	/* we always return -ENODEV here in order to enable other
	 * drivers like old, not-yet-platform_device ported i2c-viapro */
	return -ENODEV;
out:
	pci_disable_device(pdev);
	return ret;
}

static void vx855_remove(struct pci_dev *pdev)
{
	mfd_remove_devices(&pdev->dev);
	pci_disable_device(pdev);
}

static const struct pci_device_id vx855_pci_tbl[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX855) },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, vx855_pci_tbl);

static struct pci_driver vx855_pci_driver = {
	.name		= "vx855",
	.id_table	= vx855_pci_tbl,
	.probe		= vx855_probe,
	.remove		= vx855_remove,
};

module_pci_driver(vx855_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harald Welte <HaraldWelte@viatech.com>");
MODULE_DESCRIPTION("Driver for the VIA VX855 chipset");
