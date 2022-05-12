// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * linux/arch/arm/mach-sa1100/pci-nanoengine.c
 *
 * PCI functions for BSE nanoEngine PCI
 *
 * Copyright (C) 2010 Marcelo Roberto Jimenez <mroberto@cpti.cetuc.puc-rio.br>
 */
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/pci.h>

#include <asm/mach/pci.h>
#include <asm/mach-types.h>

#include <mach/nanoengine.h>
#include <mach/hardware.h>

static void __iomem *nanoengine_pci_map_bus(struct pci_bus *bus,
					    unsigned int devfn, int where)
{
	if (bus->number != 0 || (devfn >> 3) != 0)
		return NULL;

	return (void __iomem *)NANO_PCI_CONFIG_SPACE_VIRT +
		((bus->number << 16) | (devfn << 8) | (where & ~3));
}

static struct pci_ops pci_nano_ops = {
	.map_bus = nanoengine_pci_map_bus,
	.read	= pci_generic_config_read32,
	.write	= pci_generic_config_write32,
};

static int __init pci_nanoengine_map_irq(const struct pci_dev *dev, u8 slot,
	u8 pin)
{
	return NANOENGINE_IRQ_GPIO_PCI;
}

static struct resource pci_io_ports =
	DEFINE_RES_IO_NAMED(0x400, 0x400, "PCI IO");

static struct resource pci_non_prefetchable_memory = {
	.name	= "PCI non-prefetchable",
	.start	= NANO_PCI_MEM_RW_PHYS,
	/* nanoEngine documentation says there is a 1 Megabyte window here,
	 * but PCI reports just 128 + 8 kbytes. */
	.end	= NANO_PCI_MEM_RW_PHYS + NANO_PCI_MEM_RW_SIZE - 1,
/*	.end	= NANO_PCI_MEM_RW_PHYS + SZ_128K + SZ_8K - 1,*/
	.flags	= IORESOURCE_MEM,
};

/*
 * nanoEngine PCI reports 1 Megabyte of prefetchable memory, but it
 * overlaps with previously defined memory.
 *
 * Here is what happens:
 *
# dmesg
...
pci 0000:00:00.0: [8086:1209] type 0 class 0x000200
pci 0000:00:00.0: reg 10: [mem 0x00021000-0x00021fff]
pci 0000:00:00.0: reg 14: [io  0x0000-0x003f]
pci 0000:00:00.0: reg 18: [mem 0x00000000-0x0001ffff]
pci 0000:00:00.0: reg 30: [mem 0x00000000-0x000fffff pref]
pci 0000:00:00.0: supports D1 D2
pci 0000:00:00.0: PME# supported from D0 D1 D2 D3hot
pci 0000:00:00.0: PME# disabled
PCI: bus0: Fast back to back transfers enabled
pci 0000:00:00.0: BAR 6: can't assign mem pref (size 0x100000)
pci 0000:00:00.0: BAR 2: assigned [mem 0x18600000-0x1861ffff]
pci 0000:00:00.0: BAR 2: set to [mem 0x18600000-0x1861ffff] (PCI address [0x0-0x1ffff])
pci 0000:00:00.0: BAR 0: assigned [mem 0x18620000-0x18620fff]
pci 0000:00:00.0: BAR 0: set to [mem 0x18620000-0x18620fff] (PCI address [0x20000-0x20fff])
pci 0000:00:00.0: BAR 1: assigned [io  0x0400-0x043f]
pci 0000:00:00.0: BAR 1: set to [io  0x0400-0x043f] (PCI address [0x0-0x3f])
 *
 * On the other hand, if we do not request the prefetchable memory resource,
 * linux will alloc it first and the two non-prefetchable memory areas that
 * are our real interest will not be mapped. So we choose to map it to an
 * unused area. It gets recognized as expansion ROM, but becomes disabled.
 *
 * Here is what happens then:
 *
# dmesg
...
pci 0000:00:00.0: [8086:1209] type 0 class 0x000200
pci 0000:00:00.0: reg 10: [mem 0x00021000-0x00021fff]
pci 0000:00:00.0: reg 14: [io  0x0000-0x003f]
pci 0000:00:00.0: reg 18: [mem 0x00000000-0x0001ffff]
pci 0000:00:00.0: reg 30: [mem 0x00000000-0x000fffff pref]
pci 0000:00:00.0: supports D1 D2
pci 0000:00:00.0: PME# supported from D0 D1 D2 D3hot
pci 0000:00:00.0: PME# disabled
PCI: bus0: Fast back to back transfers enabled
pci 0000:00:00.0: BAR 6: assigned [mem 0x78000000-0x780fffff pref]
pci 0000:00:00.0: BAR 2: assigned [mem 0x18600000-0x1861ffff]
pci 0000:00:00.0: BAR 2: set to [mem 0x18600000-0x1861ffff] (PCI address [0x0-0x1ffff])
pci 0000:00:00.0: BAR 0: assigned [mem 0x18620000-0x18620fff]
pci 0000:00:00.0: BAR 0: set to [mem 0x18620000-0x18620fff] (PCI address [0x20000-0x20fff])
pci 0000:00:00.0: BAR 1: assigned [io  0x0400-0x043f]
pci 0000:00:00.0: BAR 1: set to [io  0x0400-0x043f] (PCI address [0x0-0x3f])

# lspci -vv -s 0000:00:00.0
00:00.0 Class 0200: Device 8086:1209 (rev 09)
        Control: I/O+ Mem+ BusMaster+ SpecCycle- MemWINV- VGASnoop- ParErr+ Stepping- SERR+ FastB2B- DisINTx-
        Status: Cap+ 66MHz- UDF- FastB2B+ ParErr- DEVSEL=medium >TAbort- <TAbort- <MAbort- >SERR+ <PERR+ INTx-
        Latency: 0 (2000ns min, 14000ns max), Cache Line Size: 32 bytes
        Interrupt: pin A routed to IRQ 0
        Region 0: Memory at 18620000 (32-bit, non-prefetchable) [size=4K]
        Region 1: I/O ports at 0400 [size=64]
        Region 2: [virtual] Memory at 18600000 (32-bit, non-prefetchable) [size=128K]
        [virtual] Expansion ROM at 78000000 [disabled] [size=1M]
        Capabilities: [dc] Power Management version 2
                Flags: PMEClk- DSI+ D1+ D2+ AuxCurrent=0mA PME(D0+,D1+,D2+,D3hot+,D3cold-)
                Status: D0 NoSoftRst- PME-Enable- DSel=0 DScale=2 PME-
        Kernel driver in use: e100
        Kernel modules: e100
 *
 */
static struct resource pci_prefetchable_memory = {
	.name	= "PCI prefetchable",
	.start	= 0x78000000,
	.end	= 0x78000000 + NANO_PCI_MEM_RW_SIZE - 1,
	.flags	= IORESOURCE_MEM  | IORESOURCE_PREFETCH,
};

static int __init pci_nanoengine_setup_resources(struct pci_sys_data *sys)
{
	if (request_resource(&ioport_resource, &pci_io_ports)) {
		printk(KERN_ERR "PCI: unable to allocate io port region\n");
		return -EBUSY;
	}
	if (request_resource(&iomem_resource, &pci_non_prefetchable_memory)) {
		release_resource(&pci_io_ports);
		printk(KERN_ERR "PCI: unable to allocate non prefetchable\n");
		return -EBUSY;
	}
	if (request_resource(&iomem_resource, &pci_prefetchable_memory)) {
		release_resource(&pci_io_ports);
		release_resource(&pci_non_prefetchable_memory);
		printk(KERN_ERR "PCI: unable to allocate prefetchable\n");
		return -EBUSY;
	}
	pci_add_resource_offset(&sys->resources, &pci_io_ports, sys->io_offset);
	pci_add_resource_offset(&sys->resources,
				&pci_non_prefetchable_memory, sys->mem_offset);
	pci_add_resource_offset(&sys->resources,
				&pci_prefetchable_memory, sys->mem_offset);

	return 1;
}

int __init pci_nanoengine_setup(int nr, struct pci_sys_data *sys)
{
	int ret = 0;

	pcibios_min_io = 0;
	pcibios_min_mem = 0;

	if (nr == 0) {
		sys->mem_offset = NANO_PCI_MEM_RW_PHYS;
		sys->io_offset = 0x400;
		ret = pci_nanoengine_setup_resources(sys);
		/* Enable alternate memory bus master mode, see
		 * "Intel StrongARM SA1110 Developer's Manual",
		 * section 10.8, "Alternate Memory Bus Master Mode". */
		GPDR = (GPDR & ~GPIO_MBREQ) | GPIO_MBGNT;
		GAFR |= GPIO_MBGNT | GPIO_MBREQ;
		TUCR |= TUCR_MBGPIO;
	}

	return ret;
}

static struct hw_pci nanoengine_pci __initdata = {
	.map_irq		= pci_nanoengine_map_irq,
	.nr_controllers		= 1,
	.ops			= &pci_nano_ops,
	.setup			= pci_nanoengine_setup,
};

static int __init nanoengine_pci_init(void)
{
	if (machine_is_nanoengine())
		pci_common_init(&nanoengine_pci);
	return 0;
}

subsys_initcall(nanoengine_pci_init);
