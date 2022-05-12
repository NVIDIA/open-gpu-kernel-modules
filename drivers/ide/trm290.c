/*
 *  Copyright (c) 1997-1998  Mark Lord
 *  Copyright (c) 2007       MontaVista Software, Inc. <source@mvista.com>
 *
 *  May be copied or modified under the terms of the GNU General Public License
 *
 *  June 22, 2004 - get rid of check_region
 *                   - Jesper Juhl
 *
 */

/*
 * This module provides support for the bus-master IDE DMA function
 * of the Tekram TRM290 chip, used on a variety of PCI IDE add-on boards,
 * including a "Precision Instruments" board.  The TRM290 pre-dates
 * the sff-8038 standard (ide-dma.c) by a few months, and differs
 * significantly enough to warrant separate routines for some functions,
 * while re-using others from ide-dma.c.
 *
 * EXPERIMENTAL!  It works for me (a sample of one).
 *
 * Works reliably for me in DMA mode (READs only),
 * DMA WRITEs are disabled by default (see #define below);
 *
 * DMA is not enabled automatically for this chipset,
 * but can be turned on manually (with "hdparm -d1") at run time.
 *
 * I need volunteers with "spare" drives for further testing
 * and development, and maybe to help figure out the peculiarities.
 * Even knowing the registers (below), some things behave strangely.
 */

#define TRM290_NO_DMA_WRITES	/* DMA writes seem unreliable sometimes */

/*
 * TRM-290 PCI-IDE2 Bus Master Chip
 * ================================
 * The configuration registers are addressed in normal I/O port space
 * and are used as follows:
 *
 * trm290_base depends on jumper settings, and is probed for by ide-dma.c
 *
 * trm290_base+2 when WRITTEN: chiptest register (byte, write-only)
 *	bit7 must always be written as "1"
 *	bits6-2 undefined
 *	bit1 1=legacy_compatible_mode, 0=native_pci_mode
 *	bit0 1=test_mode, 0=normal(default)
 *
 * trm290_base+2 when READ: status register (byte, read-only)
 *	bits7-2 undefined
 *	bit1 channel0 busmaster interrupt status 0=none, 1=asserted
 *	bit0 channel0 interrupt status 0=none, 1=asserted
 *
 * trm290_base+3 Interrupt mask register
 *	bits7-5 undefined
 *	bit4 legacy_header: 1=present, 0=absent
 *	bit3 channel1 busmaster interrupt status 0=none, 1=asserted (read only)
 *	bit2 channel1 interrupt status 0=none, 1=asserted (read only)
 *	bit1 channel1 interrupt mask: 1=masked, 0=unmasked(default)
 *	bit0 channel0 interrupt mask: 1=masked, 0=unmasked(default)
 *
 * trm290_base+1 "CPR" Config Pointer Register (byte)
 *	bit7 1=autoincrement CPR bits 2-0 after each access of CDR
 *	bit6 1=min. 1 wait-state posted write cycle (default), 0=0 wait-state
 *	bit5 0=enabled master burst access (default), 1=disable  (write only)
 *	bit4 PCI DEVSEL# timing select: 1=medium(default), 0=fast
 *	bit3 0=primary IDE channel, 1=secondary IDE channel
 *	bits2-0 register index for accesses through CDR port
 *
 * trm290_base+0 "CDR" Config Data Register (word)
 *	two sets of seven config registers,
 *	selected by CPR bit 3 (channel) and CPR bits 2-0 (index 0 to 6),
 *	each index defined below:
 *
 * Index-0 Base address register for command block (word)
 *	defaults: 0x1f0 for primary, 0x170 for secondary
 *
 * Index-1 general config register (byte)
 *	bit7 1=DMA enable, 0=DMA disable
 *	bit6 1=activate IDE_RESET, 0=no action (default)
 *	bit5 1=enable IORDY, 0=disable IORDY (default)
 *	bit4 0=16-bit data port(default), 1=8-bit (XT) data port
 *	bit3 interrupt polarity: 1=active_low, 0=active_high(default)
 *	bit2 power-saving-mode(?): 1=enable, 0=disable(default) (write only)
 *	bit1 bus_master_mode(?): 1=enable, 0=disable(default)
 *	bit0 enable_io_ports: 1=enable(default), 0=disable
 *
 * Index-2 read-ahead counter preload bits 0-7 (byte, write only)
 *	bits7-0 bits7-0 of readahead count
 *
 * Index-3 read-ahead config register (byte, write only)
 *	bit7 1=enable_readahead, 0=disable_readahead(default)
 *	bit6 1=clear_FIFO, 0=no_action
 *	bit5 undefined
 *	bit4 mode4 timing control: 1=enable, 0=disable(default)
 *	bit3 undefined
 *	bit2 undefined
 *	bits1-0 bits9-8 of read-ahead count
 *
 * Index-4 base address register for control block (word)
 *	defaults: 0x3f6 for primary, 0x376 for secondary
 *
 * Index-5 data port timings (shared by both drives) (byte)
 *	standard PCI "clk" (clock) counts, default value = 0xf5
 *
 *	bits7-6 setup time:  00=1clk, 01=2clk, 10=3clk, 11=4clk
 *	bits5-3 hold time:	000=1clk, 001=2clk, 010=3clk,
 *				011=4clk, 100=5clk, 101=6clk,
 *				110=8clk, 111=12clk
 *	bits2-0 active time:	000=2clk, 001=3clk, 010=4clk,
 *				011=5clk, 100=6clk, 101=8clk,
 *				110=12clk, 111=16clk
 *
 * Index-6 command/control port timings (shared by both drives) (byte)
 *	same layout as Index-5, default value = 0xde
 *
 * Suggested CDR programming for PIO mode0 (600ns):
 *	0x01f0,0x21,0xff,0x80,0x03f6,0xf5,0xde	; primary
 *	0x0170,0x21,0xff,0x80,0x0376,0xf5,0xde	; secondary
 *
 * Suggested CDR programming for PIO mode3 (180ns):
 *	0x01f0,0x21,0xff,0x80,0x03f6,0x09,0xde	; primary
 *	0x0170,0x21,0xff,0x80,0x0376,0x09,0xde	; secondary
 *
 * Suggested CDR programming for PIO mode4 (120ns):
 *	0x01f0,0x21,0xff,0x80,0x03f6,0x00,0xde	; primary
 *	0x0170,0x21,0xff,0x80,0x0376,0x00,0xde	; secondary
 *
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/blkdev.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/ide.h>

#include <asm/io.h>

#define DRV_NAME "trm290"

static void trm290_prepare_drive (ide_drive_t *drive, unsigned int use_dma)
{
	ide_hwif_t *hwif = drive->hwif;
	u16 reg = 0;
	unsigned long flags;

	/* select PIO or DMA */
	reg = use_dma ? (0x21 | 0x82) : (0x21 & ~0x82);

	local_irq_save(flags);

	if (reg != hwif->select_data) {
		hwif->select_data = reg;
		/* set PIO/DMA */
		outb(0x51 | (hwif->channel << 3), hwif->config_data + 1);
		outw(reg & 0xff, hwif->config_data);
	}

	/* enable IRQ if not probing */
	if (drive->dev_flags & IDE_DFLAG_PRESENT) {
		reg = inw(hwif->config_data + 3);
		reg &= 0x13;
		reg &= ~(1 << hwif->channel);
		outw(reg, hwif->config_data + 3);
	}

	local_irq_restore(flags);
}

static void trm290_dev_select(ide_drive_t *drive)
{
	trm290_prepare_drive(drive, !!(drive->dev_flags & IDE_DFLAG_USING_DMA));

	outb(drive->select | ATA_DEVICE_OBS, drive->hwif->io_ports.device_addr);
}

static int trm290_dma_check(ide_drive_t *drive, struct ide_cmd *cmd)
{
	if (cmd->tf_flags & IDE_TFLAG_WRITE) {
#ifdef TRM290_NO_DMA_WRITES
		/* always use PIO for writes */
		return 1;
#endif
	}
	return 0;
}

static int trm290_dma_setup(ide_drive_t *drive, struct ide_cmd *cmd)
{
	ide_hwif_t *hwif = drive->hwif;
	unsigned int count, rw = (cmd->tf_flags & IDE_TFLAG_WRITE) ? 1 : 2;

	count = ide_build_dmatable(drive, cmd);
	if (count == 0)
		/* try PIO instead of DMA */
		return 1;

	outl(hwif->dmatable_dma | rw, hwif->dma_base);
	/* start DMA */
	outw(count * 2 - 1, hwif->dma_base + 2);

	return 0;
}

static void trm290_dma_start(ide_drive_t *drive)
{
	trm290_prepare_drive(drive, 1);
}

static int trm290_dma_end(ide_drive_t *drive)
{
	u16 status = inw(drive->hwif->dma_base + 2);

	trm290_prepare_drive(drive, 0);

	return status != 0x00ff;
}

static int trm290_dma_test_irq(ide_drive_t *drive)
{
	u16 status = inw(drive->hwif->dma_base + 2);

	return status == 0x00ff;
}

static void trm290_dma_host_set(ide_drive_t *drive, int on)
{
}

static void init_hwif_trm290(ide_hwif_t *hwif)
{
	struct pci_dev *dev	= to_pci_dev(hwif->dev);
	unsigned int  cfg_base	= pci_resource_start(dev, 4);
	unsigned long flags;
	u8 reg = 0;

	if ((dev->class & 5) && cfg_base)
		printk(KERN_INFO DRV_NAME " %s: chip", pci_name(dev));
	else {
		cfg_base = 0x3df0;
		printk(KERN_INFO DRV_NAME " %s: using default", pci_name(dev));
	}
	printk(KERN_CONT " config base at 0x%04x\n", cfg_base);
	hwif->config_data = cfg_base;
	hwif->dma_base = (cfg_base + 4) ^ (hwif->channel ? 0x80 : 0);

	printk(KERN_INFO "    %s: BM-DMA at 0x%04lx-0x%04lx\n",
	       hwif->name, hwif->dma_base, hwif->dma_base + 3);

	if (ide_allocate_dma_engine(hwif))
		return;

	local_irq_save(flags);
	/* put config reg into first byte of hwif->select_data */
	outb(0x51 | (hwif->channel << 3), hwif->config_data + 1);
	/* select PIO as default */
	hwif->select_data = 0x21;
	outb(hwif->select_data, hwif->config_data);
	/* get IRQ info */
	reg = inb(hwif->config_data + 3);
	/* mask IRQs for both ports */
	reg = (reg & 0x10) | 0x03;
	outb(reg, hwif->config_data + 3);
	local_irq_restore(flags);

	if (reg & 0x10)
		/* legacy mode */
		hwif->irq = hwif->channel ? 15 : 14;

#if 1
	{
	/*
	 * My trm290-based card doesn't seem to work with all possible values
	 * for the control basereg, so this kludge ensures that we use only
	 * values that are known to work.  Ugh.		-ml
	 */
		u16 new, old, compat = hwif->channel ? 0x374 : 0x3f4;
		static u16 next_offset = 0;
		u8 old_mask;

		outb(0x54 | (hwif->channel << 3), hwif->config_data + 1);
		old = inw(hwif->config_data);
		old &= ~1;
		old_mask = inb(old + 2);
		if (old != compat && old_mask == 0xff) {
			/* leave lower 10 bits untouched */
			compat += (next_offset += 0x400);
			hwif->io_ports.ctl_addr = compat + 2;
			outw(compat | 1, hwif->config_data);
			new = inw(hwif->config_data);
			printk(KERN_INFO "%s: control basereg workaround: "
				"old=0x%04x, new=0x%04x\n",
				hwif->name, old, new & ~1);
		}
	}
#endif
}

static const struct ide_tp_ops trm290_tp_ops = {
	.exec_command		= ide_exec_command,
	.read_status		= ide_read_status,
	.read_altstatus		= ide_read_altstatus,
	.write_devctl		= ide_write_devctl,

	.dev_select		= trm290_dev_select,
	.tf_load		= ide_tf_load,
	.tf_read		= ide_tf_read,

	.input_data		= ide_input_data,
	.output_data		= ide_output_data,
};

static const struct ide_dma_ops trm290_dma_ops = {
	.dma_host_set		= trm290_dma_host_set,
	.dma_setup 		= trm290_dma_setup,
	.dma_start 		= trm290_dma_start,
	.dma_end		= trm290_dma_end,
	.dma_test_irq		= trm290_dma_test_irq,
	.dma_lost_irq		= ide_dma_lost_irq,
	.dma_check		= trm290_dma_check,
};

static const struct ide_port_info trm290_chipset = {
	.name		= DRV_NAME,
	.init_hwif	= init_hwif_trm290,
	.tp_ops 	= &trm290_tp_ops,
	.dma_ops	= &trm290_dma_ops,
	.host_flags	= IDE_HFLAG_TRM290 |
			  IDE_HFLAG_NO_ATAPI_DMA |
#if 0 /* play it safe for now */
			  IDE_HFLAG_TRUST_BIOS_FOR_DMA |
#endif
			  IDE_HFLAG_NO_AUTODMA |
			  IDE_HFLAG_NO_LBA48,
};

static int trm290_init_one(struct pci_dev *dev, const struct pci_device_id *id)
{
	return ide_pci_init_one(dev, &trm290_chipset, NULL);
}

static const struct pci_device_id trm290_pci_tbl[] = {
	{ PCI_VDEVICE(TEKRAM, PCI_DEVICE_ID_TEKRAM_DC290), 0 },
	{ 0, },
};
MODULE_DEVICE_TABLE(pci, trm290_pci_tbl);

static struct pci_driver trm290_pci_driver = {
	.name		= "TRM290_IDE",
	.id_table	= trm290_pci_tbl,
	.probe		= trm290_init_one,
	.remove		= ide_pci_remove,
};

static int __init trm290_ide_init(void)
{
	return ide_pci_register_driver(&trm290_pci_driver);
}

static void __exit trm290_ide_exit(void)
{
	pci_unregister_driver(&trm290_pci_driver);
}

module_init(trm290_ide_init);
module_exit(trm290_ide_exit);

MODULE_AUTHOR("Mark Lord");
MODULE_DESCRIPTION("PCI driver module for Tekram TRM290 IDE");
MODULE_LICENSE("GPL");
