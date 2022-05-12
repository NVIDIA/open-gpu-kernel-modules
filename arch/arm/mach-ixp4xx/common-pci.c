// SPDX-License-Identifier: GPL-2.0-only
/*
 * arch/arm/mach-ixp4xx/common-pci.c 
 *
 * IXP4XX PCI routines for all platforms
 *
 * Maintainer: Deepak Saxena <dsaxena@plexity.net>
 *
 * Copyright (C) 2002 Intel Corporation.
 * Copyright (C) 2003 Greg Ungerer <gerg@snapgear.com>
 * Copyright (C) 2003-2004 MontaVista Software, Inc.
 */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/export.h>
#include <asm/dma-mapping.h>

#include <asm/cputype.h>
#include <asm/irq.h>
#include <linux/sizes.h>
#include <asm/mach/pci.h>
#include <mach/hardware.h>


/*
 * IXP4xx PCI read function is dependent on whether we are 
 * running A0 or B0 (AppleGate) silicon.
 */
int (*ixp4xx_pci_read)(u32 addr, u32 cmd, u32* data);

/*
 * Base address for PCI register region
 */
unsigned long ixp4xx_pci_reg_base = 0;

/*
 * PCI cfg an I/O routines are done by programming a 
 * command/byte enable register, and then read/writing
 * the data from a data register. We need to ensure
 * these transactions are atomic or we will end up
 * with corrupt data on the bus or in a driver.
 */
static DEFINE_RAW_SPINLOCK(ixp4xx_pci_lock);

/*
 * Read from PCI config space
 */
static void crp_read(u32 ad_cbe, u32 *data)
{
	unsigned long flags;
	raw_spin_lock_irqsave(&ixp4xx_pci_lock, flags);
	*PCI_CRP_AD_CBE = ad_cbe;
	*data = *PCI_CRP_RDATA;
	raw_spin_unlock_irqrestore(&ixp4xx_pci_lock, flags);
}

/*
 * Write to PCI config space
 */
static void crp_write(u32 ad_cbe, u32 data)
{ 
	unsigned long flags;
	raw_spin_lock_irqsave(&ixp4xx_pci_lock, flags);
	*PCI_CRP_AD_CBE = CRP_AD_CBE_WRITE | ad_cbe;
	*PCI_CRP_WDATA = data;
	raw_spin_unlock_irqrestore(&ixp4xx_pci_lock, flags);
}

static inline int check_master_abort(void)
{
	/* check Master Abort bit after access */
	unsigned long isr = *PCI_ISR;

	if (isr & PCI_ISR_PFE) {
		/* make sure the Master Abort bit is reset */    
		*PCI_ISR = PCI_ISR_PFE;
		pr_debug("%s failed\n", __func__);
		return 1;
	}

	return 0;
}

int ixp4xx_pci_read_errata(u32 addr, u32 cmd, u32* data)
{
	unsigned long flags;
	int retval = 0;
	int i;

	raw_spin_lock_irqsave(&ixp4xx_pci_lock, flags);

	*PCI_NP_AD = addr;

	/* 
	 * PCI workaround  - only works if NP PCI space reads have 
	 * no side effects!!! Read 8 times. last one will be good.
	 */
	for (i = 0; i < 8; i++) {
		*PCI_NP_CBE = cmd;
		*data = *PCI_NP_RDATA;
		*data = *PCI_NP_RDATA;
	}

	if(check_master_abort())
		retval = 1;

	raw_spin_unlock_irqrestore(&ixp4xx_pci_lock, flags);
	return retval;
}

int ixp4xx_pci_read_no_errata(u32 addr, u32 cmd, u32* data)
{
	unsigned long flags;
	int retval = 0;

	raw_spin_lock_irqsave(&ixp4xx_pci_lock, flags);

	*PCI_NP_AD = addr;

	/* set up and execute the read */    
	*PCI_NP_CBE = cmd;

	/* the result of the read is now in NP_RDATA */
	*data = *PCI_NP_RDATA; 

	if(check_master_abort())
		retval = 1;

	raw_spin_unlock_irqrestore(&ixp4xx_pci_lock, flags);
	return retval;
}

int ixp4xx_pci_write(u32 addr, u32 cmd, u32 data)
{    
	unsigned long flags;
	int retval = 0;

	raw_spin_lock_irqsave(&ixp4xx_pci_lock, flags);

	*PCI_NP_AD = addr;

	/* set up the write */
	*PCI_NP_CBE = cmd;

	/* execute the write by writing to NP_WDATA */
	*PCI_NP_WDATA = data;

	if(check_master_abort())
		retval = 1;

	raw_spin_unlock_irqrestore(&ixp4xx_pci_lock, flags);
	return retval;
}

static u32 ixp4xx_config_addr(u8 bus_num, u16 devfn, int where)
{
	u32 addr;
	if (!bus_num) {
		/* type 0 */
		addr = BIT(32-PCI_SLOT(devfn)) | ((PCI_FUNC(devfn)) << 8) | 
		    (where & ~3);	
	} else {
		/* type 1 */
		addr = (bus_num << 16) | ((PCI_SLOT(devfn)) << 11) | 
			((PCI_FUNC(devfn)) << 8) | (where & ~3) | 1;
	}
	return addr;
}

/*
 * Mask table, bits to mask for quantity of size 1, 2 or 4 bytes.
 * 0 and 3 are not valid indexes...
 */
static u32 bytemask[] = {
	/*0*/	0,
	/*1*/	0xff,
	/*2*/	0xffff,
	/*3*/	0,
	/*4*/	0xffffffff,
};

static u32 local_byte_lane_enable_bits(u32 n, int size)
{
	if (size == 1)
		return (0xf & ~BIT(n)) << CRP_AD_CBE_BESL;
	if (size == 2)
		return (0xf & ~(BIT(n) | BIT(n+1))) << CRP_AD_CBE_BESL;
	if (size == 4)
		return 0;
	return 0xffffffff;
}

static int local_read_config(int where, int size, u32 *value)
{ 
	u32 n, data;
	pr_debug("local_read_config from %d size %d\n", where, size);
	n = where % 4;
	crp_read(where & ~3, &data);
	*value = (data >> (8*n)) & bytemask[size];
	pr_debug("local_read_config read %#x\n", *value);
	return PCIBIOS_SUCCESSFUL;
}

static int local_write_config(int where, int size, u32 value)
{
	u32 n, byte_enables, data;
	pr_debug("local_write_config %#x to %d size %d\n", value, where, size);
	n = where % 4;
	byte_enables = local_byte_lane_enable_bits(n, size);
	if (byte_enables == 0xffffffff)
		return PCIBIOS_BAD_REGISTER_NUMBER;
	data = value << (8*n);
	crp_write((where & ~3) | byte_enables, data);
	return PCIBIOS_SUCCESSFUL;
}

static u32 byte_lane_enable_bits(u32 n, int size)
{
	if (size == 1)
		return (0xf & ~BIT(n)) << 4;
	if (size == 2)
		return (0xf & ~(BIT(n) | BIT(n+1))) << 4;
	if (size == 4)
		return 0;
	return 0xffffffff;
}

static int ixp4xx_pci_read_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *value)
{
	u32 n, byte_enables, addr, data;
	u8 bus_num = bus->number;

	pr_debug("read_config from %d size %d dev %d:%d:%d\n", where, size,
		bus_num, PCI_SLOT(devfn), PCI_FUNC(devfn));

	*value = 0xffffffff;
	n = where % 4;
	byte_enables = byte_lane_enable_bits(n, size);
	if (byte_enables == 0xffffffff)
		return PCIBIOS_BAD_REGISTER_NUMBER;

	addr = ixp4xx_config_addr(bus_num, devfn, where);
	if (ixp4xx_pci_read(addr, byte_enables | NP_CMD_CONFIGREAD, &data))
		return PCIBIOS_DEVICE_NOT_FOUND;

	*value = (data >> (8*n)) & bytemask[size];
	pr_debug("read_config_byte read %#x\n", *value);
	return PCIBIOS_SUCCESSFUL;
}

static int ixp4xx_pci_write_config(struct pci_bus *bus,  unsigned int devfn, int where, int size, u32 value)
{
	u32 n, byte_enables, addr, data;
	u8 bus_num = bus->number;

	pr_debug("write_config_byte %#x to %d size %d dev %d:%d:%d\n", value, where,
		size, bus_num, PCI_SLOT(devfn), PCI_FUNC(devfn));

	n = where % 4;
	byte_enables = byte_lane_enable_bits(n, size);
	if (byte_enables == 0xffffffff)
		return PCIBIOS_BAD_REGISTER_NUMBER;

	addr = ixp4xx_config_addr(bus_num, devfn, where);
	data = value << (8*n);
	if (ixp4xx_pci_write(addr, byte_enables | NP_CMD_CONFIGWRITE, data))
		return PCIBIOS_DEVICE_NOT_FOUND;

	return PCIBIOS_SUCCESSFUL;
}

struct pci_ops ixp4xx_ops = {
	.read =  ixp4xx_pci_read_config,
	.write = ixp4xx_pci_write_config,
};

/*
 * PCI abort handler
 */
static int abort_handler(unsigned long addr, unsigned int fsr, struct pt_regs *regs)
{
	u32 isr, status;

	isr = *PCI_ISR;
	local_read_config(PCI_STATUS, 2, &status);
	pr_debug("PCI: abort_handler addr = %#lx, isr = %#x, "
		"status = %#x\n", addr, isr, status);

	/* make sure the Master Abort bit is reset */    
	*PCI_ISR = PCI_ISR_PFE;
	status |= PCI_STATUS_REC_MASTER_ABORT;
	local_write_config(PCI_STATUS, 2, status);

	/*
	 * If it was an imprecise abort, then we need to correct the
	 * return address to be _after_ the instruction.
	 */
	if (fsr & (1 << 10))
		regs->ARM_pc += 4;

	return 0;
}

void __init ixp4xx_pci_preinit(void)
{
	unsigned long cpuid = read_cpuid_id();

#ifdef CONFIG_IXP4XX_INDIRECT_PCI
	pcibios_min_mem = 0x10000000; /* 1 GB of indirect PCI MMIO space */
#else
	pcibios_min_mem = 0x48000000; /* 64 MB of PCI MMIO space */
#endif
	/*
	 * Determine which PCI read method to use.
	 * Rev 0 IXP425 requires workaround.
	 */
	if (!(cpuid & 0xf) && cpu_is_ixp42x()) {
		printk("PCI: IXP42x A0 silicon detected - "
			"PCI Non-Prefetch Workaround Enabled\n");
		ixp4xx_pci_read = ixp4xx_pci_read_errata;
	} else
		ixp4xx_pci_read = ixp4xx_pci_read_no_errata;


	/* hook in our fault handler for PCI errors */
	hook_fault_code(16+6, abort_handler, SIGBUS, 0,
			"imprecise external abort");

	pr_debug("setup PCI-AHB(inbound) and AHB-PCI(outbound) address mappings\n");

	/*
	 * We use identity AHB->PCI address translation
	 * in the 0x48000000 to 0x4bffffff address space
	 */
	*PCI_PCIMEMBASE = 0x48494A4B;

	/*
	 * We also use identity PCI->AHB address translation
	 * in 4 16MB BARs that begin at the physical memory start
	 */
	*PCI_AHBMEMBASE = (PHYS_OFFSET & 0xFF000000) +
		((PHYS_OFFSET & 0xFF000000) >> 8) +
		((PHYS_OFFSET & 0xFF000000) >> 16) +
		((PHYS_OFFSET & 0xFF000000) >> 24) +
		0x00010203;

	if (*PCI_CSR & PCI_CSR_HOST) {
		printk("PCI: IXP4xx is host\n");

		pr_debug("setup BARs in controller\n");

		/*
		 * We configure the PCI inbound memory windows to be
		 * 1:1 mapped to SDRAM
		 */
		local_write_config(PCI_BASE_ADDRESS_0, 4, PHYS_OFFSET);
		local_write_config(PCI_BASE_ADDRESS_1, 4, PHYS_OFFSET + SZ_16M);
		local_write_config(PCI_BASE_ADDRESS_2, 4, PHYS_OFFSET + SZ_32M);
		local_write_config(PCI_BASE_ADDRESS_3, 4,
					PHYS_OFFSET + SZ_32M + SZ_16M);

		/*
		 * Enable CSR window at 64 MiB to allow PCI masters
		 * to continue prefetching past 64 MiB boundary.
		 */
		local_write_config(PCI_BASE_ADDRESS_4, 4, PHYS_OFFSET + SZ_64M);

		/*
		 * Enable the IO window to be way up high, at 0xfffffc00
		 */
		local_write_config(PCI_BASE_ADDRESS_5, 4, 0xfffffc01);
		local_write_config(0x40, 4, 0x000080FF); /* No TRDY time limit */
	} else {
		printk("PCI: IXP4xx is target - No bus scan performed\n");
	}

	printk("PCI: IXP4xx Using %s access for memory space\n",
#ifndef CONFIG_IXP4XX_INDIRECT_PCI
			"direct"
#else
			"indirect"
#endif
		);

	pr_debug("clear error bits in ISR\n");
	*PCI_ISR = PCI_ISR_PSE | PCI_ISR_PFE | PCI_ISR_PPE | PCI_ISR_AHBE;

	/*
	 * Set Initialize Complete in PCI Control Register: allow IXP4XX to
	 * respond to PCI configuration cycles. Specify that the AHB bus is
	 * operating in big endian mode. Set up byte lane swapping between 
	 * little-endian PCI and the big-endian AHB bus 
	 */
#ifdef __ARMEB__
	*PCI_CSR = PCI_CSR_IC | PCI_CSR_ABE | PCI_CSR_PDS | PCI_CSR_ADS;
#else
	*PCI_CSR = PCI_CSR_IC | PCI_CSR_ABE;
#endif

	pr_debug("DONE\n");
}

int ixp4xx_setup(int nr, struct pci_sys_data *sys)
{
	struct resource *res;

	if (nr >= 1)
		return 0;

	res = kcalloc(2, sizeof(*res), GFP_KERNEL);
	if (res == NULL) {
		/* 
		 * If we're out of memory this early, something is wrong,
		 * so we might as well catch it here.
		 */
		panic("PCI: unable to allocate resources?\n");
	}

	local_write_config(PCI_COMMAND, 2, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	res[0].name = "PCI I/O Space";
	res[0].start = 0x00000000;
	res[0].end = 0x0000ffff;
	res[0].flags = IORESOURCE_IO;

	res[1].name = "PCI Memory Space";
	res[1].start = PCIBIOS_MIN_MEM;
	res[1].end = PCIBIOS_MAX_MEM;
	res[1].flags = IORESOURCE_MEM;

	request_resource(&ioport_resource, &res[0]);
	request_resource(&iomem_resource, &res[1]);

	pci_add_resource_offset(&sys->resources, &res[0], sys->io_offset);
	pci_add_resource_offset(&sys->resources, &res[1], sys->mem_offset);

	return 1;
}

EXPORT_SYMBOL(ixp4xx_pci_read);
EXPORT_SYMBOL(ixp4xx_pci_write);
