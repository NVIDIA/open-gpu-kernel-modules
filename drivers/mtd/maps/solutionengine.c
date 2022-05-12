/*
 * Flash and EPROM on Hitachi Solution Engine and similar boards.
 *
 * (C) 2001 Red Hat, Inc.
 *
 * GPL'd
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/errno.h>

static struct mtd_info *flash_mtd;
static struct mtd_info *eprom_mtd;

struct map_info soleng_eprom_map = {
	.name = "Solution Engine EPROM",
	.size = 0x400000,
	.bankwidth = 4,
};

struct map_info soleng_flash_map = {
	.name = "Solution Engine FLASH",
	.size = 0x400000,
	.bankwidth = 4,
};

static const char * const probes[] = { "RedBoot", "cmdlinepart", NULL };

static int __init init_soleng_maps(void)
{
	/* First probe at offset 0 */
	soleng_flash_map.phys = 0;
	soleng_flash_map.virt = (void __iomem *)P2SEGADDR(0);
	soleng_eprom_map.phys = 0x01000000;
	soleng_eprom_map.virt = (void __iomem *)P1SEGADDR(0x01000000);
	simple_map_init(&soleng_eprom_map);
	simple_map_init(&soleng_flash_map);

	printk(KERN_NOTICE "Probing for flash chips at 0x00000000:\n");
	flash_mtd = do_map_probe("cfi_probe", &soleng_flash_map);
	if (!flash_mtd) {
		/* Not there. Try swapping */
		printk(KERN_NOTICE "Probing for flash chips at 0x01000000:\n");
		soleng_flash_map.phys = 0x01000000;
		soleng_flash_map.virt = P2SEGADDR(0x01000000);
		soleng_eprom_map.phys = 0;
		soleng_eprom_map.virt = P1SEGADDR(0);
		flash_mtd = do_map_probe("cfi_probe", &soleng_flash_map);
		if (!flash_mtd) {
			/* Eep. */
			printk(KERN_NOTICE "Flash chips not detected at either possible location.\n");
			return -ENXIO;
		}
	}
	printk(KERN_NOTICE "Solution Engine: Flash at 0x%pap, EPROM at 0x%pap\n",
	       &soleng_flash_map.phys,
	       &soleng_eprom_map.phys);
	flash_mtd->owner = THIS_MODULE;

	eprom_mtd = do_map_probe("map_rom", &soleng_eprom_map);
	if (eprom_mtd) {
		eprom_mtd->owner = THIS_MODULE;
		mtd_device_register(eprom_mtd, NULL, 0);
	}

	mtd_device_parse_register(flash_mtd, probes, NULL, NULL, 0);

	return 0;
}

static void __exit cleanup_soleng_maps(void)
{
	if (eprom_mtd) {
		mtd_device_unregister(eprom_mtd);
		map_destroy(eprom_mtd);
	}

	mtd_device_unregister(flash_mtd);
	map_destroy(flash_mtd);
}

module_init(init_soleng_maps);
module_exit(cleanup_soleng_maps);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_DESCRIPTION("MTD map driver for Hitachi SolutionEngine (and similar) boards");

