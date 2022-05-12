/*
 * generic/default IDE host driver
 *
 * Copyright (C) 2004, 2008-2009 Bartlomiej Zolnierkiewicz
 * This code was split off from ide.c.  See it for original copyrights.
 *
 * May be copied or modified under the terms of the GNU General Public License.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ide.h>
#include <linux/pci_ids.h>

/* FIXME: convert arm to use ide_platform host driver */
#ifdef CONFIG_ARM
#include <asm/irq.h>
#endif

#define DRV_NAME	"ide_generic"

static int probe_mask;
module_param(probe_mask, int, 0);
MODULE_PARM_DESC(probe_mask, "probe mask for legacy ISA IDE ports");

static const struct ide_port_info ide_generic_port_info = {
	.host_flags		= IDE_HFLAG_NO_DMA,
	.chipset		= ide_generic,
};

#ifdef CONFIG_ARM
static const u16 legacy_bases[] = { 0x1f0 };
static const int legacy_irqs[]  = { IRQ_HARDDISK };
#elif defined(CONFIG_ALPHA)
static const u16 legacy_bases[] = { 0x1f0, 0x170, 0x1e8, 0x168 };
static const int legacy_irqs[]  = { 14, 15, 11, 10 };
#else
static const u16 legacy_bases[] = { 0x1f0, 0x170, 0x1e8, 0x168, 0x1e0, 0x160 };
static const int legacy_irqs[]  = { 14, 15, 11, 10, 8, 12 };
#endif

static void ide_generic_check_pci_legacy_iobases(int *primary, int *secondary)
{
#ifdef CONFIG_PCI
	struct pci_dev *p = NULL;
	u16 val;

	for_each_pci_dev(p) {
		if (pci_resource_start(p, 0) == 0x1f0)
			*primary = 1;
		if (pci_resource_start(p, 2) == 0x170)
			*secondary = 1;

		/* Cyrix CS55{1,2}0 pre SFF MWDMA ATA on the bridge */
		if (p->vendor == PCI_VENDOR_ID_CYRIX &&
		    (p->device == PCI_DEVICE_ID_CYRIX_5510 ||
		     p->device == PCI_DEVICE_ID_CYRIX_5520))
			*primary = *secondary = 1;

		/* Intel MPIIX - PIO ATA on non PCI side of bridge */
		if (p->vendor == PCI_VENDOR_ID_INTEL &&
		    p->device == PCI_DEVICE_ID_INTEL_82371MX) {
			pci_read_config_word(p, 0x6C, &val);
			if (val & 0x8000) {
				/* ATA port enabled */
				if (val & 0x4000)
					*secondary = 1;
				else
					*primary = 1;
			}
		}
	}
#endif
}

static int __init ide_generic_init(void)
{
	struct ide_hw hw, *hws[] = { &hw };
	unsigned long io_addr;
	int i, rc = 0, primary = 0, secondary = 0;

	ide_generic_check_pci_legacy_iobases(&primary, &secondary);

	if (!probe_mask) {
		printk(KERN_INFO DRV_NAME ": please use \"probe_mask=0x3f\" "
		     "module parameter for probing all legacy ISA IDE ports\n");

		if (primary == 0)
			probe_mask |= 0x1;

		if (secondary == 0)
			probe_mask |= 0x2;
	} else
		printk(KERN_INFO DRV_NAME ": enforcing probing of I/O ports "
			"upon user request\n");

	for (i = 0; i < ARRAY_SIZE(legacy_bases); i++) {
		io_addr = legacy_bases[i];

		if ((probe_mask & (1 << i)) && io_addr) {
			if (!request_region(io_addr, 8, DRV_NAME)) {
				printk(KERN_ERR "%s: I/O resource 0x%lX-0x%lX "
						"not free.\n",
						DRV_NAME, io_addr, io_addr + 7);
				rc = -EBUSY;
				continue;
			}

			if (!request_region(io_addr + 0x206, 1, DRV_NAME)) {
				printk(KERN_ERR "%s: I/O resource 0x%lX "
						"not free.\n",
						DRV_NAME, io_addr + 0x206);
				release_region(io_addr, 8);
				rc = -EBUSY;
				continue;
			}

			memset(&hw, 0, sizeof(hw));
			ide_std_init_ports(&hw, io_addr, io_addr + 0x206);
#ifdef CONFIG_IA64
			hw.irq = isa_irq_to_vector(legacy_irqs[i]);
#else
			hw.irq = legacy_irqs[i];
#endif
			rc = ide_host_add(&ide_generic_port_info, hws, 1, NULL);
			if (rc) {
				release_region(io_addr + 0x206, 1);
				release_region(io_addr, 8);
			}
		}
	}

	return rc;
}

module_init(ide_generic_init);

MODULE_LICENSE("GPL");
