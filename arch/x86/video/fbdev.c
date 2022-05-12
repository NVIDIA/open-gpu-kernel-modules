/*
 * Copyright (C) 2007 Antonino Daplas <adaplas@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 */
#include <linux/fb.h>
#include <linux/pci.h>
#include <linux/module.h>
#include <linux/vgaarb.h>

int fb_is_primary_device(struct fb_info *info)
{
	struct device *device = info->device;
	struct pci_dev *default_device = vga_default_device();
	struct pci_dev *pci_dev;
	struct resource *res;

	if (!device || !dev_is_pci(device))
		return 0;

	pci_dev = to_pci_dev(device);

	if (default_device) {
		if (pci_dev == default_device)
			return 1;
		return 0;
	}

	res = pci_dev->resource + PCI_ROM_RESOURCE;

	if (res->flags & IORESOURCE_ROM_SHADOW)
		return 1;

	return 0;
}
EXPORT_SYMBOL(fb_is_primary_device);
MODULE_LICENSE("GPL");
