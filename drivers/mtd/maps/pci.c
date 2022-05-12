// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/drivers/mtd/maps/pci.c
 *
 *  Copyright (C) 2001 Russell King, All rights reserved.
 *
 * Generic PCI memory map driver.  We support the following boards:
 *  - Intel IQ80310 ATU.
 *  - Intel EBSA285 (blank rom programming mode). Tested working 27/09/2001
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

struct map_pci_info;

struct mtd_pci_info {
	int  (*init)(struct pci_dev *dev, struct map_pci_info *map);
	void (*exit)(struct pci_dev *dev, struct map_pci_info *map);
	unsigned long (*translate)(struct map_pci_info *map, unsigned long ofs);
	const char *map_name;
};

struct map_pci_info {
	struct map_info map;
	void __iomem *base;
	void (*exit)(struct pci_dev *dev, struct map_pci_info *map);
	unsigned long (*translate)(struct map_pci_info *map, unsigned long ofs);
	struct pci_dev *dev;
};

static map_word mtd_pci_read8(struct map_info *_map, unsigned long ofs)
{
	struct map_pci_info *map = (struct map_pci_info *)_map;
	map_word val;
	val.x[0]= readb(map->base + map->translate(map, ofs));
	return val;
}

static map_word mtd_pci_read32(struct map_info *_map, unsigned long ofs)
{
	struct map_pci_info *map = (struct map_pci_info *)_map;
	map_word val;
	val.x[0] = readl(map->base + map->translate(map, ofs));
	return val;
}

static void mtd_pci_copyfrom(struct map_info *_map, void *to, unsigned long from, ssize_t len)
{
	struct map_pci_info *map = (struct map_pci_info *)_map;
	memcpy_fromio(to, map->base + map->translate(map, from), len);
}

static void mtd_pci_write8(struct map_info *_map, map_word val, unsigned long ofs)
{
	struct map_pci_info *map = (struct map_pci_info *)_map;
	writeb(val.x[0], map->base + map->translate(map, ofs));
}

static void mtd_pci_write32(struct map_info *_map, map_word val, unsigned long ofs)
{
	struct map_pci_info *map = (struct map_pci_info *)_map;
	writel(val.x[0], map->base + map->translate(map, ofs));
}

static void mtd_pci_copyto(struct map_info *_map, unsigned long to, const void *from, ssize_t len)
{
	struct map_pci_info *map = (struct map_pci_info *)_map;
	memcpy_toio(map->base + map->translate(map, to), from, len);
}

static const struct map_info mtd_pci_map = {
	.phys =		NO_XIP,
	.copy_from =	mtd_pci_copyfrom,
	.copy_to =	mtd_pci_copyto,
};

/*
 * Intel IOP80310 Flash driver
 */

static int
intel_iq80310_init(struct pci_dev *dev, struct map_pci_info *map)
{
	u32 win_base;

	map->map.bankwidth = 1;
	map->map.read = mtd_pci_read8;
	map->map.write = mtd_pci_write8;

	map->map.size     = 0x00800000;
	map->base         = ioremap(pci_resource_start(dev, 0),
					    pci_resource_len(dev, 0));

	if (!map->base)
		return -ENOMEM;

	/*
	 * We want to base the memory window at Xscale
	 * bus address 0, not 0x1000.
	 */
	pci_read_config_dword(dev, 0x44, &win_base);
	pci_write_config_dword(dev, 0x44, 0);

	map->map.map_priv_2 = win_base;

	return 0;
}

static void
intel_iq80310_exit(struct pci_dev *dev, struct map_pci_info *map)
{
	if (map->base)
		iounmap(map->base);
	pci_write_config_dword(dev, 0x44, map->map.map_priv_2);
}

static unsigned long
intel_iq80310_translate(struct map_pci_info *map, unsigned long ofs)
{
	unsigned long page_addr = ofs & 0x00400000;

	/*
	 * This mundges the flash location so we avoid
	 * the first 80 bytes (they appear to read nonsense).
	 */
	if (page_addr) {
		writel(0x00000008, map->base + 0x1558);
		writel(0x00000000, map->base + 0x1550);
	} else {
		writel(0x00000007, map->base + 0x1558);
		writel(0x00800000, map->base + 0x1550);
		ofs += 0x00800000;
	}

	return ofs;
}

static struct mtd_pci_info intel_iq80310_info = {
	.init =		intel_iq80310_init,
	.exit =		intel_iq80310_exit,
	.translate =	intel_iq80310_translate,
	.map_name =	"cfi_probe",
};

/*
 * Intel DC21285 driver
 */

static int
intel_dc21285_init(struct pci_dev *dev, struct map_pci_info *map)
{
	unsigned long base, len;

	base = pci_resource_start(dev, PCI_ROM_RESOURCE);
	len  = pci_resource_len(dev, PCI_ROM_RESOURCE);

	if (!len || !base) {
		/*
		 * No ROM resource
		 */
		base = pci_resource_start(dev, 2);
		len  = pci_resource_len(dev, 2);

		/*
		 * We need to re-allocate PCI BAR2 address range to the
		 * PCI ROM BAR, and disable PCI BAR2.
		 */
	} else {
		/*
		 * Hmm, if an address was allocated to the ROM resource, but
		 * not enabled, should we be allocating a new resource for it
		 * or simply enabling it?
		 */
		pci_enable_rom(dev);
		printk("%s: enabling expansion ROM\n", pci_name(dev));
	}

	if (!len || !base)
		return -ENXIO;

	map->map.bankwidth = 4;
	map->map.read = mtd_pci_read32;
	map->map.write = mtd_pci_write32;
	map->map.size     = len;
	map->base         = ioremap(base, len);

	if (!map->base)
		return -ENOMEM;

	return 0;
}

static void
intel_dc21285_exit(struct pci_dev *dev, struct map_pci_info *map)
{
	if (map->base)
		iounmap(map->base);

	/*
	 * We need to undo the PCI BAR2/PCI ROM BAR address alteration.
	 */
	pci_disable_rom(dev);
}

static unsigned long
intel_dc21285_translate(struct map_pci_info *map, unsigned long ofs)
{
	return ofs & 0x00ffffc0 ? ofs : (ofs ^ (1 << 5));
}

static struct mtd_pci_info intel_dc21285_info = {
	.init =		intel_dc21285_init,
	.exit =		intel_dc21285_exit,
	.translate =	intel_dc21285_translate,
	.map_name =	"jedec_probe",
};

/*
 * PCI device ID table
 */

static const struct pci_device_id mtd_pci_ids[] = {
	{
		.vendor =	PCI_VENDOR_ID_INTEL,
		.device =	0x530d,
		.subvendor =	PCI_ANY_ID,
		.subdevice =	PCI_ANY_ID,
		.class =	PCI_CLASS_MEMORY_OTHER << 8,
		.class_mask =	0xffff00,
		.driver_data =	(unsigned long)&intel_iq80310_info,
	},
	{
		.vendor =	PCI_VENDOR_ID_DEC,
		.device =	PCI_DEVICE_ID_DEC_21285,
		.subvendor =	0,	/* DC21285 defaults to 0 on reset */
		.subdevice =	0,	/* DC21285 defaults to 0 on reset */
		.driver_data =	(unsigned long)&intel_dc21285_info,
	},
	{ 0, }
};

/*
 * Generic code follows.
 */

static int mtd_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	struct mtd_pci_info *info = (struct mtd_pci_info *)id->driver_data;
	struct map_pci_info *map = NULL;
	struct mtd_info *mtd = NULL;
	int err;

	err = pci_enable_device(dev);
	if (err)
		goto out;

	err = pci_request_regions(dev, "pci mtd");
	if (err)
		goto out;

	map = kmalloc(sizeof(*map), GFP_KERNEL);
	err = -ENOMEM;
	if (!map)
		goto release;

	map->map       = mtd_pci_map;
	map->map.name  = pci_name(dev);
	map->dev       = dev;
	map->exit      = info->exit;
	map->translate = info->translate;

	err = info->init(dev, map);
	if (err)
		goto release;

	mtd = do_map_probe(info->map_name, &map->map);
	err = -ENODEV;
	if (!mtd)
		goto release;

	mtd->owner = THIS_MODULE;
	mtd_device_register(mtd, NULL, 0);

	pci_set_drvdata(dev, mtd);

	return 0;

release:
	if (map) {
		map->exit(dev, map);
		kfree(map);
	}

	pci_release_regions(dev);
out:
	return err;
}

static void mtd_pci_remove(struct pci_dev *dev)
{
	struct mtd_info *mtd = pci_get_drvdata(dev);
	struct map_pci_info *map = mtd->priv;

	mtd_device_unregister(mtd);
	map_destroy(mtd);
	map->exit(dev, map);
	kfree(map);

	pci_release_regions(dev);
}

static struct pci_driver mtd_pci_driver = {
	.name =		"MTD PCI",
	.probe =	mtd_pci_probe,
	.remove =	mtd_pci_remove,
	.id_table =	mtd_pci_ids,
};

module_pci_driver(mtd_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Russell King <rmk@arm.linux.org.uk>");
MODULE_DESCRIPTION("Generic PCI map driver");
MODULE_DEVICE_TABLE(pci, mtd_pci_ids);
