// SPDX-License-Identifier: GPL-2.0
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/export.h>
#include <linux/mod_devicetable.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/irq.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <asm/spitfire.h>

#include "of_device_common.h"

void __iomem *of_ioremap(struct resource *res, unsigned long offset, unsigned long size, char *name)
{
	unsigned long ret = res->start + offset;
	struct resource *r;

	if (res->flags & IORESOURCE_MEM)
		r = request_mem_region(ret, size, name);
	else
		r = request_region(ret, size, name);
	if (!r)
		ret = 0;

	return (void __iomem *) ret;
}
EXPORT_SYMBOL(of_ioremap);

void of_iounmap(struct resource *res, void __iomem *base, unsigned long size)
{
	if (res->flags & IORESOURCE_MEM)
		release_mem_region((unsigned long) base, size);
	else
		release_region((unsigned long) base, size);
}
EXPORT_SYMBOL(of_iounmap);

/*
 * PCI bus specific translator
 */

static int of_bus_pci_match(struct device_node *np)
{
	if (of_node_name_eq(np, "pci")) {
		const char *model = of_get_property(np, "model", NULL);

		if (model && !strcmp(model, "SUNW,simba"))
			return 0;

		/* Do not do PCI specific frobbing if the
		 * PCI bridge lacks a ranges property.  We
		 * want to pass it through up to the next
		 * parent as-is, not with the PCI translate
		 * method which chops off the top address cell.
		 */
		if (!of_find_property(np, "ranges", NULL))
			return 0;

		return 1;
	}

	return 0;
}

static int of_bus_simba_match(struct device_node *np)
{
	const char *model = of_get_property(np, "model", NULL);

	if (model && !strcmp(model, "SUNW,simba"))
		return 1;

	/* Treat PCI busses lacking ranges property just like
	 * simba.
	 */
	if (of_node_name_eq(np, "pci")) {
		if (!of_find_property(np, "ranges", NULL))
			return 1;
	}

	return 0;
}

static int of_bus_simba_map(u32 *addr, const u32 *range,
			    int na, int ns, int pna)
{
	return 0;
}

static void of_bus_pci_count_cells(struct device_node *np,
				   int *addrc, int *sizec)
{
	if (addrc)
		*addrc = 3;
	if (sizec)
		*sizec = 2;
}

static int of_bus_pci_map(u32 *addr, const u32 *range,
			  int na, int ns, int pna)
{
	u32 result[OF_MAX_ADDR_CELLS];
	int i;

	/* Check address type match */
	if (!((addr[0] ^ range[0]) & 0x03000000))
		goto type_match;

	/* Special exception, we can map a 64-bit address into
	 * a 32-bit range.
	 */
	if ((addr[0] & 0x03000000) == 0x03000000 &&
	    (range[0] & 0x03000000) == 0x02000000)
		goto type_match;

	return -EINVAL;

type_match:
	if (of_out_of_range(addr + 1, range + 1, range + na + pna,
			    na - 1, ns))
		return -EINVAL;

	/* Start with the parent range base.  */
	memcpy(result, range + na, pna * 4);

	/* Add in the child address offset, skipping high cell.  */
	for (i = 0; i < na - 1; i++)
		result[pna - 1 - i] +=
			(addr[na - 1 - i] -
			 range[na - 1 - i]);

	memcpy(addr, result, pna * 4);

	return 0;
}

static unsigned long of_bus_pci_get_flags(const u32 *addr, unsigned long flags)
{
	u32 w = addr[0];

	/* For PCI, we override whatever child busses may have used.  */
	flags = 0;
	switch((w >> 24) & 0x03) {
	case 0x01:
		flags |= IORESOURCE_IO;
		break;

	case 0x02: /* 32 bits */
	case 0x03: /* 64 bits */
		flags |= IORESOURCE_MEM;
		break;
	}
	if (w & 0x40000000)
		flags |= IORESOURCE_PREFETCH;
	return flags;
}

/*
 * FHC/Central bus specific translator.
 *
 * This is just needed to hard-code the address and size cell
 * counts.  'fhc' and 'central' nodes lack the #address-cells and
 * #size-cells properties, and if you walk to the root on such
 * Enterprise boxes all you'll get is a #size-cells of 2 which is
 * not what we want to use.
 */
static int of_bus_fhc_match(struct device_node *np)
{
	return of_node_name_eq(np, "fhc") ||
		of_node_name_eq(np, "central");
}

#define of_bus_fhc_count_cells of_bus_sbus_count_cells

/*
 * Array of bus specific translators
 */

static struct of_bus of_busses[] = {
	/* PCI */
	{
		.name = "pci",
		.addr_prop_name = "assigned-addresses",
		.match = of_bus_pci_match,
		.count_cells = of_bus_pci_count_cells,
		.map = of_bus_pci_map,
		.get_flags = of_bus_pci_get_flags,
	},
	/* SIMBA */
	{
		.name = "simba",
		.addr_prop_name = "assigned-addresses",
		.match = of_bus_simba_match,
		.count_cells = of_bus_pci_count_cells,
		.map = of_bus_simba_map,
		.get_flags = of_bus_pci_get_flags,
	},
	/* SBUS */
	{
		.name = "sbus",
		.addr_prop_name = "reg",
		.match = of_bus_sbus_match,
		.count_cells = of_bus_sbus_count_cells,
		.map = of_bus_default_map,
		.get_flags = of_bus_default_get_flags,
	},
	/* FHC */
	{
		.name = "fhc",
		.addr_prop_name = "reg",
		.match = of_bus_fhc_match,
		.count_cells = of_bus_fhc_count_cells,
		.map = of_bus_default_map,
		.get_flags = of_bus_default_get_flags,
	},
	/* Default */
	{
		.name = "default",
		.addr_prop_name = "reg",
		.match = NULL,
		.count_cells = of_bus_default_count_cells,
		.map = of_bus_default_map,
		.get_flags = of_bus_default_get_flags,
	},
};

static struct of_bus *of_match_bus(struct device_node *np)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(of_busses); i ++)
		if (!of_busses[i].match || of_busses[i].match(np))
			return &of_busses[i];
	BUG();
	return NULL;
}

static int __init build_one_resource(struct device_node *parent,
				     struct of_bus *bus,
				     struct of_bus *pbus,
				     u32 *addr,
				     int na, int ns, int pna)
{
	const u32 *ranges;
	int rone, rlen;

	ranges = of_get_property(parent, "ranges", &rlen);
	if (ranges == NULL || rlen == 0) {
		u32 result[OF_MAX_ADDR_CELLS];
		int i;

		memset(result, 0, pna * 4);
		for (i = 0; i < na; i++)
			result[pna - 1 - i] =
				addr[na - 1 - i];

		memcpy(addr, result, pna * 4);
		return 0;
	}

	/* Now walk through the ranges */
	rlen /= 4;
	rone = na + pna + ns;
	for (; rlen >= rone; rlen -= rone, ranges += rone) {
		if (!bus->map(addr, ranges, na, ns, pna))
			return 0;
	}

	/* When we miss an I/O space match on PCI, just pass it up
	 * to the next PCI bridge and/or controller.
	 */
	if (!strcmp(bus->name, "pci") &&
	    (addr[0] & 0x03000000) == 0x01000000)
		return 0;

	return 1;
}

static int __init use_1to1_mapping(struct device_node *pp)
{
	/* If we have a ranges property in the parent, use it.  */
	if (of_find_property(pp, "ranges", NULL) != NULL)
		return 0;

	/* If the parent is the dma node of an ISA bus, pass
	 * the translation up to the root.
	 *
	 * Some SBUS devices use intermediate nodes to express
	 * hierarchy within the device itself.  These aren't
	 * real bus nodes, and don't have a 'ranges' property.
	 * But, we should still pass the translation work up
	 * to the SBUS itself.
	 */
	if (of_node_name_eq(pp, "dma") ||
	    of_node_name_eq(pp, "espdma") ||
	    of_node_name_eq(pp, "ledma") ||
	    of_node_name_eq(pp, "lebuffer"))
		return 0;

	/* Similarly for all PCI bridges, if we get this far
	 * it lacks a ranges property, and this will include
	 * cases like Simba.
	 */
	if (of_node_name_eq(pp, "pci"))
		return 0;

	return 1;
}

static int of_resource_verbose;

static void __init build_device_resources(struct platform_device *op,
					  struct device *parent)
{
	struct platform_device *p_op;
	struct of_bus *bus;
	int na, ns;
	int index, num_reg;
	const void *preg;

	if (!parent)
		return;

	p_op = to_platform_device(parent);
	bus = of_match_bus(p_op->dev.of_node);
	bus->count_cells(op->dev.of_node, &na, &ns);

	preg = of_get_property(op->dev.of_node, bus->addr_prop_name, &num_reg);
	if (!preg || num_reg == 0)
		return;

	/* Convert to num-cells.  */
	num_reg /= 4;

	/* Convert to num-entries.  */
	num_reg /= na + ns;

	/* Prevent overrunning the op->resources[] array.  */
	if (num_reg > PROMREG_MAX) {
		printk(KERN_WARNING "%pOF: Too many regs (%d), "
		       "limiting to %d.\n",
		       op->dev.of_node, num_reg, PROMREG_MAX);
		num_reg = PROMREG_MAX;
	}

	op->resource = op->archdata.resource;
	op->num_resources = num_reg;
	for (index = 0; index < num_reg; index++) {
		struct resource *r = &op->resource[index];
		u32 addr[OF_MAX_ADDR_CELLS];
		const u32 *reg = (preg + (index * ((na + ns) * 4)));
		struct device_node *dp = op->dev.of_node;
		struct device_node *pp = p_op->dev.of_node;
		struct of_bus *pbus, *dbus;
		u64 size, result = OF_BAD_ADDR;
		unsigned long flags;
		int dna, dns;
		int pna, pns;

		size = of_read_addr(reg + na, ns);
		memcpy(addr, reg, na * 4);

		flags = bus->get_flags(addr, 0);

		if (use_1to1_mapping(pp)) {
			result = of_read_addr(addr, na);
			goto build_res;
		}

		dna = na;
		dns = ns;
		dbus = bus;

		while (1) {
			dp = pp;
			pp = dp->parent;
			if (!pp) {
				result = of_read_addr(addr, dna);
				break;
			}

			pbus = of_match_bus(pp);
			pbus->count_cells(dp, &pna, &pns);

			if (build_one_resource(dp, dbus, pbus, addr,
					       dna, dns, pna))
				break;

			flags = pbus->get_flags(addr, flags);

			dna = pna;
			dns = pns;
			dbus = pbus;
		}

	build_res:
		memset(r, 0, sizeof(*r));

		if (of_resource_verbose)
			printk("%pOF reg[%d] -> %llx\n",
			       op->dev.of_node, index,
			       result);

		if (result != OF_BAD_ADDR) {
			if (tlb_type == hypervisor)
				result &= 0x0fffffffffffffffUL;

			r->start = result;
			r->end = result + size - 1;
			r->flags = flags;
		}
		r->name = op->dev.of_node->full_name;
	}
}

static struct device_node * __init
apply_interrupt_map(struct device_node *dp, struct device_node *pp,
		    const u32 *imap, int imlen, const u32 *imask,
		    unsigned int *irq_p)
{
	struct device_node *cp;
	unsigned int irq = *irq_p;
	struct of_bus *bus;
	phandle handle;
	const u32 *reg;
	int na, num_reg, i;

	bus = of_match_bus(pp);
	bus->count_cells(dp, &na, NULL);

	reg = of_get_property(dp, "reg", &num_reg);
	if (!reg || !num_reg)
		return NULL;

	imlen /= ((na + 3) * 4);
	handle = 0;
	for (i = 0; i < imlen; i++) {
		int j;

		for (j = 0; j < na; j++) {
			if ((reg[j] & imask[j]) != imap[j])
				goto next;
		}
		if (imap[na] == irq) {
			handle = imap[na + 1];
			irq = imap[na + 2];
			break;
		}

	next:
		imap += (na + 3);
	}
	if (i == imlen) {
		/* Psycho and Sabre PCI controllers can have 'interrupt-map'
		 * properties that do not include the on-board device
		 * interrupts.  Instead, the device's 'interrupts' property
		 * is already a fully specified INO value.
		 *
		 * Handle this by deciding that, if we didn't get a
		 * match in the parent's 'interrupt-map', and the
		 * parent is an IRQ translator, then use the parent as
		 * our IRQ controller.
		 */
		if (pp->irq_trans)
			return pp;

		return NULL;
	}

	*irq_p = irq;
	cp = of_find_node_by_phandle(handle);

	return cp;
}

static unsigned int __init pci_irq_swizzle(struct device_node *dp,
					   struct device_node *pp,
					   unsigned int irq)
{
	const struct linux_prom_pci_registers *regs;
	unsigned int bus, devfn, slot, ret;

	if (irq < 1 || irq > 4)
		return irq;

	regs = of_get_property(dp, "reg", NULL);
	if (!regs)
		return irq;

	bus = (regs->phys_hi >> 16) & 0xff;
	devfn = (regs->phys_hi >> 8) & 0xff;
	slot = (devfn >> 3) & 0x1f;

	if (pp->irq_trans) {
		/* Derived from Table 8-3, U2P User's Manual.  This branch
		 * is handling a PCI controller that lacks a proper set of
		 * interrupt-map and interrupt-map-mask properties.  The
		 * Ultra-E450 is one example.
		 *
		 * The bit layout is BSSLL, where:
		 * B: 0 on bus A, 1 on bus B
		 * D: 2-bit slot number, derived from PCI device number as
		 *    (dev - 1) for bus A, or (dev - 2) for bus B
		 * L: 2-bit line number
		 */
		if (bus & 0x80) {
			/* PBM-A */
			bus  = 0x00;
			slot = (slot - 1) << 2;
		} else {
			/* PBM-B */
			bus  = 0x10;
			slot = (slot - 2) << 2;
		}
		irq -= 1;

		ret = (bus | slot | irq);
	} else {
		/* Going through a PCI-PCI bridge that lacks a set of
		 * interrupt-map and interrupt-map-mask properties.
		 */
		ret = ((irq - 1 + (slot & 3)) & 3) + 1;
	}

	return ret;
}

static int of_irq_verbose;

static unsigned int __init build_one_device_irq(struct platform_device *op,
						struct device *parent,
						unsigned int irq)
{
	struct device_node *dp = op->dev.of_node;
	struct device_node *pp, *ip;
	unsigned int orig_irq = irq;
	int nid;

	if (irq == 0xffffffff)
		return irq;

	if (dp->irq_trans) {
		irq = dp->irq_trans->irq_build(dp, irq,
					       dp->irq_trans->data);

		if (of_irq_verbose)
			printk("%pOF: direct translate %x --> %x\n",
			       dp, orig_irq, irq);

		goto out;
	}

	/* Something more complicated.  Walk up to the root, applying
	 * interrupt-map or bus specific translations, until we hit
	 * an IRQ translator.
	 *
	 * If we hit a bus type or situation we cannot handle, we
	 * stop and assume that the original IRQ number was in a
	 * format which has special meaning to it's immediate parent.
	 */
	pp = dp->parent;
	ip = NULL;
	while (pp) {
		const void *imap, *imsk;
		int imlen;

		imap = of_get_property(pp, "interrupt-map", &imlen);
		imsk = of_get_property(pp, "interrupt-map-mask", NULL);
		if (imap && imsk) {
			struct device_node *iret;
			int this_orig_irq = irq;

			iret = apply_interrupt_map(dp, pp,
						   imap, imlen, imsk,
						   &irq);

			if (of_irq_verbose)
				printk("%pOF: Apply [%pOF:%x] imap --> [%pOF:%x]\n",
				       op->dev.of_node,
				       pp, this_orig_irq, iret, irq);

			if (!iret)
				break;

			if (iret->irq_trans) {
				ip = iret;
				break;
			}
		} else {
			if (of_node_name_eq(pp, "pci")) {
				unsigned int this_orig_irq = irq;

				irq = pci_irq_swizzle(dp, pp, irq);
				if (of_irq_verbose)
					printk("%pOF: PCI swizzle [%pOF] "
					       "%x --> %x\n",
					       op->dev.of_node,
					       pp, this_orig_irq,
					       irq);

			}

			if (pp->irq_trans) {
				ip = pp;
				break;
			}
		}
		dp = pp;
		pp = pp->parent;
	}
	if (!ip)
		return orig_irq;

	irq = ip->irq_trans->irq_build(op->dev.of_node, irq,
				       ip->irq_trans->data);
	if (of_irq_verbose)
		printk("%pOF: Apply IRQ trans [%pOF] %x --> %x\n",
		      op->dev.of_node, ip, orig_irq, irq);

out:
	nid = of_node_to_nid(dp);
	if (nid != -1) {
		cpumask_t numa_mask;

		cpumask_copy(&numa_mask, cpumask_of_node(nid));
		irq_set_affinity(irq, &numa_mask);
	}

	return irq;
}

static struct platform_device * __init scan_one_device(struct device_node *dp,
						 struct device *parent)
{
	struct platform_device *op = kzalloc(sizeof(*op), GFP_KERNEL);
	const unsigned int *irq;
	struct dev_archdata *sd;
	int len, i;

	if (!op)
		return NULL;

	sd = &op->dev.archdata;
	sd->op = op;

	op->dev.of_node = dp;

	irq = of_get_property(dp, "interrupts", &len);
	if (irq) {
		op->archdata.num_irqs = len / 4;

		/* Prevent overrunning the op->irqs[] array.  */
		if (op->archdata.num_irqs > PROMINTR_MAX) {
			printk(KERN_WARNING "%pOF: Too many irqs (%d), "
			       "limiting to %d.\n",
			       dp, op->archdata.num_irqs, PROMINTR_MAX);
			op->archdata.num_irqs = PROMINTR_MAX;
		}
		memcpy(op->archdata.irqs, irq, op->archdata.num_irqs * 4);
	} else {
		op->archdata.num_irqs = 0;
	}

	build_device_resources(op, parent);
	for (i = 0; i < op->archdata.num_irqs; i++)
		op->archdata.irqs[i] = build_one_device_irq(op, parent, op->archdata.irqs[i]);

	op->dev.parent = parent;
	op->dev.bus = &platform_bus_type;
	if (!parent)
		dev_set_name(&op->dev, "root");
	else
		dev_set_name(&op->dev, "%08x", dp->phandle);
	op->dev.coherent_dma_mask = DMA_BIT_MASK(32);
	op->dev.dma_mask = &op->dev.coherent_dma_mask;

	if (of_device_register(op)) {
		printk("%pOF: Could not register of device.\n", dp);
		kfree(op);
		op = NULL;
	}

	return op;
}

static void __init scan_tree(struct device_node *dp, struct device *parent)
{
	while (dp) {
		struct platform_device *op = scan_one_device(dp, parent);

		if (op)
			scan_tree(dp->child, &op->dev);

		dp = dp->sibling;
	}
}

static int __init scan_of_devices(void)
{
	struct device_node *root = of_find_node_by_path("/");
	struct platform_device *parent;

	parent = scan_one_device(root, NULL);
	if (!parent)
		return 0;

	scan_tree(root->child, &parent->dev);
	return 0;
}
postcore_initcall(scan_of_devices);

static int __init of_debug(char *str)
{
	int val = 0;

	get_option(&str, &val);
	if (val & 1)
		of_resource_verbose = 1;
	if (val & 2)
		of_irq_verbose = 1;
	return 1;
}

__setup("of_debug=", of_debug);
