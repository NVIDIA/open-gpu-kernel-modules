// SPDX-License-Identifier: GPL-2.0
/*
 *	linux/arch/alpha/kernel/sys_nautilus.c
 *
 *	Copyright (C) 1995 David A Rusling
 *	Copyright (C) 1998 Richard Henderson
 *	Copyright (C) 1999 Alpha Processor, Inc.,
 *		(David Daniel, Stig Telfer, Soohoon Lee)
 *
 * Code supporting NAUTILUS systems.
 *
 *
 * NAUTILUS has the following I/O features:
 *
 * a) Driven by AMD 751 aka IRONGATE (northbridge):
 *     4 PCI slots
 *     1 AGP slot
 *
 * b) Driven by ALI M1543C (southbridge)
 *     2 ISA slots
 *     2 IDE connectors
 *     1 dual drive capable FDD controller
 *     2 serial ports
 *     1 ECP/EPP/SP parallel port
 *     2 USB ports
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/memblock.h>
#include <linux/bitops.h>

#include <asm/ptrace.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <asm/mmu_context.h>
#include <asm/io.h>
#include <asm/core_irongate.h>
#include <asm/hwrpb.h>
#include <asm/tlbflush.h>

#include "proto.h"
#include "err_impl.h"
#include "irq_impl.h"
#include "pci_impl.h"
#include "machvec_impl.h"


static void __init
nautilus_init_irq(void)
{
	if (alpha_using_srm) {
		alpha_mv.device_interrupt = srm_device_interrupt;
	}

	init_i8259a_irqs();
	common_init_isa_dma();
}

static int
nautilus_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	/* Preserve the IRQ set up by the console.  */

	u8 irq;
	/* UP1500: AGP INTA is actually routed to IRQ 5, not IRQ 10 as
	   console reports. Check the device id of AGP bridge to distinguish
	   UP1500 from UP1000/1100. Note: 'pin' is 2 due to bridge swizzle. */
	if (slot == 1 && pin == 2 &&
	    dev->bus->self && dev->bus->self->device == 0x700f)
		return 5;
	pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &irq);
	return irq;
}

void
nautilus_kill_arch(int mode)
{
	struct pci_bus *bus = pci_isa_hose->bus;
	u32 pmuport;
	int off;

	switch (mode) {
	case LINUX_REBOOT_CMD_RESTART:
		if (! alpha_using_srm) {
			u8 t8;
			pci_bus_read_config_byte(bus, 0x38, 0x43, &t8);
			pci_bus_write_config_byte(bus, 0x38, 0x43, t8 | 0x80);
			outb(1, 0x92);
			outb(0, 0x92);
			/* NOTREACHED */
		}
		break;

	case LINUX_REBOOT_CMD_POWER_OFF:
		/* Assume M1543C */
		off = 0x2000;		/* SLP_TYPE = 0, SLP_EN = 1 */
		pci_bus_read_config_dword(bus, 0x88, 0x10, &pmuport);
		if (!pmuport) {
			/* M1535D/D+ */
			off = 0x3400;	/* SLP_TYPE = 5, SLP_EN = 1 */
			pci_bus_read_config_dword(bus, 0x88, 0xe0, &pmuport);
		}
		pmuport &= 0xfffe;
		outw(0xffff, pmuport);	/* Clear pending events. */
		outw(off, pmuport + 4);
		/* NOTREACHED */
		break;
	}
}

/* Perform analysis of a machine check that arrived from the system (NMI) */

static void
naut_sys_machine_check(unsigned long vector, unsigned long la_ptr,
		       struct pt_regs *regs)
{
	printk("PC %lx RA %lx\n", regs->pc, regs->r26);
	irongate_pci_clr_err();
}

/* Machine checks can come from two sources - those on the CPU and those
   in the system.  They are analysed separately but all starts here.  */

void
nautilus_machine_check(unsigned long vector, unsigned long la_ptr)
{
	char *mchk_class;

	/* Now for some analysis.  Machine checks fall into two classes --
	   those picked up by the system, and those picked up by the CPU.
	   Add to that the two levels of severity - correctable or not.  */

	if (vector == SCB_Q_SYSMCHK
	    && ((IRONGATE0->dramms & 0x300) == 0x300)) {
		unsigned long nmi_ctl;

		/* Clear ALI NMI */
		nmi_ctl = inb(0x61);
		nmi_ctl |= 0x0c;
		outb(nmi_ctl, 0x61);
		nmi_ctl &= ~0x0c;
		outb(nmi_ctl, 0x61);

		/* Write again clears error bits.  */
		IRONGATE0->stat_cmd = IRONGATE0->stat_cmd & ~0x100;
		mb();
		IRONGATE0->stat_cmd;

		/* Write again clears error bits.  */
		IRONGATE0->dramms = IRONGATE0->dramms;
		mb();
		IRONGATE0->dramms;

		draina();
		wrmces(0x7);
		mb();
		return;
	}

	if (vector == SCB_Q_SYSERR)
		mchk_class = "Correctable";
	else if (vector == SCB_Q_SYSMCHK)
		mchk_class = "Fatal";
	else {
		ev6_machine_check(vector, la_ptr);
		return;
	}

	printk(KERN_CRIT "NAUTILUS Machine check 0x%lx "
			 "[%s System Machine Check (NMI)]\n",
	       vector, mchk_class);

	naut_sys_machine_check(vector, la_ptr, get_irq_regs());

	/* Tell the PALcode to clear the machine check */
	draina();
	wrmces(0x7);
	mb();
}

extern void pcibios_claim_one_bus(struct pci_bus *);

static struct resource irongate_mem = {
	.name	= "Irongate PCI MEM",
	.flags	= IORESOURCE_MEM,
};
static struct resource busn_resource = {
	.name	= "PCI busn",
	.start	= 0,
	.end	= 255,
	.flags	= IORESOURCE_BUS,
};

void __init
nautilus_init_pci(void)
{
	struct pci_controller *hose = hose_head;
	struct pci_host_bridge *bridge;
	struct pci_bus *bus;
	unsigned long bus_align, bus_size, pci_mem;
	unsigned long memtop = max_low_pfn << PAGE_SHIFT;

	bridge = pci_alloc_host_bridge(0);
	if (!bridge)
		return;

	/* Use default IO. */
	pci_add_resource(&bridge->windows, &ioport_resource);
	/* Irongate PCI memory aperture, calculate requred size before
	   setting it up. */
	pci_add_resource(&bridge->windows, &irongate_mem);

	pci_add_resource(&bridge->windows, &busn_resource);
	bridge->dev.parent = NULL;
	bridge->sysdata = hose;
	bridge->busnr = 0;
	bridge->ops = alpha_mv.pci_ops;
	bridge->swizzle_irq = alpha_mv.pci_swizzle;
	bridge->map_irq = alpha_mv.pci_map_irq;
	bridge->size_windows = 1;

	/* Scan our single hose.  */
	if (pci_scan_root_bus_bridge(bridge)) {
		pci_free_host_bridge(bridge);
		return;
	}
	bus = hose->bus = bridge->bus;
	pcibios_claim_one_bus(bus);

	pci_bus_size_bridges(bus);

	/* Now we've got the size and alignment of PCI memory resources
	   stored in irongate_mem. Set up the PCI memory range: limit is
	   hardwired to 0xffffffff, base must be aligned to 16Mb. */
	bus_align = irongate_mem.start;
	bus_size = irongate_mem.end + 1 - bus_align;
	if (bus_align < 0x1000000UL)
		bus_align = 0x1000000UL;

	pci_mem = (0x100000000UL - bus_size) & -bus_align;
	irongate_mem.start = pci_mem;
	irongate_mem.end = 0xffffffffUL;

	/* Register our newly calculated PCI memory window in the resource
	   tree. */
	if (request_resource(&iomem_resource, &irongate_mem) < 0)
		printk(KERN_ERR "Failed to request MEM on hose 0\n");

	printk(KERN_INFO "Irongate pci_mem %pR\n", &irongate_mem);

	if (pci_mem < memtop)
		memtop = pci_mem;
	if (memtop > alpha_mv.min_mem_address) {
		free_reserved_area(__va(alpha_mv.min_mem_address),
				   __va(memtop), -1, NULL);
		printk(KERN_INFO "nautilus_init_pci: %ldk freed\n",
			(memtop - alpha_mv.min_mem_address) >> 10);
	}
	if ((IRONGATE0->dev_vendor >> 16) > 0x7006)	/* Albacore? */
		IRONGATE0->pci_mem = pci_mem;

	pci_bus_assign_resources(bus);
	pci_bus_add_devices(bus);
}

/*
 * The System Vectors
 */

struct alpha_machine_vector nautilus_mv __initmv = {
	.vector_name		= "Nautilus",
	DO_EV6_MMU,
	DO_DEFAULT_RTC,
	DO_IRONGATE_IO,
	.machine_check		= nautilus_machine_check,
	.max_isa_dma_address	= ALPHA_MAX_ISA_DMA_ADDRESS,
	.min_io_address		= DEFAULT_IO_BASE,
	.min_mem_address	= IRONGATE_DEFAULT_MEM_BASE,

	.nr_irqs		= 16,
	.device_interrupt	= isa_device_interrupt,

	.init_arch		= irongate_init_arch,
	.init_irq		= nautilus_init_irq,
	.init_rtc		= common_init_rtc,
	.init_pci		= nautilus_init_pci,
	.kill_arch		= nautilus_kill_arch,
	.pci_map_irq		= nautilus_map_irq,
	.pci_swizzle		= common_swizzle,
};
ALIAS_MV(nautilus)
