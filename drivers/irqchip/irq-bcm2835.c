// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2010 Broadcom
 * Copyright 2012 Simon Arlott, Chris Boot, Stephen Warren
 *
 * Quirk 1: Shortcut interrupts don't set the bank 1/2 register pending bits
 *
 * If an interrupt fires on bank 1 that isn't in the shortcuts list, bit 8
 * on bank 0 is set to signify that an interrupt in bank 1 has fired, and
 * to look in the bank 1 status register for more information.
 *
 * If an interrupt fires on bank 1 that _is_ in the shortcuts list, its
 * shortcut bit in bank 0 is set as well as its interrupt bit in the bank 1
 * status register, but bank 0 bit 8 is _not_ set.
 *
 * Quirk 2: You can't mask the register 1/2 pending interrupts
 *
 * In a proper cascaded interrupt controller, the interrupt lines with
 * cascaded interrupt controllers on them are just normal interrupt lines.
 * You can mask the interrupts and get on with things. With this controller
 * you can't do that.
 *
 * Quirk 3: The shortcut interrupts can't be (un)masked in bank 0
 *
 * Those interrupts that have shortcuts can only be masked/unmasked in
 * their respective banks' enable/disable registers. Doing so in the bank 0
 * enable/disable registers has no effect.
 *
 * The FIQ control register:
 *  Bits 0-6: IRQ (index in order of interrupts from banks 1, 2, then 0)
 *  Bit    7: Enable FIQ generation
 *  Bits  8+: Unused
 *
 * An interrupt must be disabled before configuring it for FIQ generation
 * otherwise both handlers will fire at the same time!
 */

#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/irqchip.h>
#include <linux/irqdomain.h>

#include <asm/exception.h>

/* Put the bank and irq (32 bits) into the hwirq */
#define MAKE_HWIRQ(b, n)	((b << 5) | (n))
#define HWIRQ_BANK(i)		(i >> 5)
#define HWIRQ_BIT(i)		BIT(i & 0x1f)

#define NR_IRQS_BANK0		8
#define BANK0_HWIRQ_MASK	0xff
/* Shortcuts can't be disabled so any unknown new ones need to be masked */
#define SHORTCUT1_MASK		0x00007c00
#define SHORTCUT2_MASK		0x001f8000
#define SHORTCUT_SHIFT		10
#define BANK1_HWIRQ		BIT(8)
#define BANK2_HWIRQ		BIT(9)
#define BANK0_VALID_MASK	(BANK0_HWIRQ_MASK | BANK1_HWIRQ | BANK2_HWIRQ \
					| SHORTCUT1_MASK | SHORTCUT2_MASK)

#define REG_FIQ_CONTROL		0x0c
#define FIQ_CONTROL_ENABLE	BIT(7)

#define NR_BANKS		3
#define IRQS_PER_BANK		32

static const int reg_pending[] __initconst = { 0x00, 0x04, 0x08 };
static const int reg_enable[] __initconst = { 0x18, 0x10, 0x14 };
static const int reg_disable[] __initconst = { 0x24, 0x1c, 0x20 };
static const int bank_irqs[] __initconst = { 8, 32, 32 };

static const int shortcuts[] = {
	7, 9, 10, 18, 19,		/* Bank 1 */
	21, 22, 23, 24, 25, 30		/* Bank 2 */
};

struct armctrl_ic {
	void __iomem *base;
	void __iomem *pending[NR_BANKS];
	void __iomem *enable[NR_BANKS];
	void __iomem *disable[NR_BANKS];
	struct irq_domain *domain;
};

static struct armctrl_ic intc __read_mostly;
static void __exception_irq_entry bcm2835_handle_irq(
	struct pt_regs *regs);
static void bcm2836_chained_handle_irq(struct irq_desc *desc);

static void armctrl_mask_irq(struct irq_data *d)
{
	writel_relaxed(HWIRQ_BIT(d->hwirq), intc.disable[HWIRQ_BANK(d->hwirq)]);
}

static void armctrl_unmask_irq(struct irq_data *d)
{
	writel_relaxed(HWIRQ_BIT(d->hwirq), intc.enable[HWIRQ_BANK(d->hwirq)]);
}

static struct irq_chip armctrl_chip = {
	.name = "ARMCTRL-level",
	.irq_mask = armctrl_mask_irq,
	.irq_unmask = armctrl_unmask_irq
};

static int armctrl_xlate(struct irq_domain *d, struct device_node *ctrlr,
	const u32 *intspec, unsigned int intsize,
	unsigned long *out_hwirq, unsigned int *out_type)
{
	if (WARN_ON(intsize != 2))
		return -EINVAL;

	if (WARN_ON(intspec[0] >= NR_BANKS))
		return -EINVAL;

	if (WARN_ON(intspec[1] >= IRQS_PER_BANK))
		return -EINVAL;

	if (WARN_ON(intspec[0] == 0 && intspec[1] >= NR_IRQS_BANK0))
		return -EINVAL;

	*out_hwirq = MAKE_HWIRQ(intspec[0], intspec[1]);
	*out_type = IRQ_TYPE_NONE;
	return 0;
}

static const struct irq_domain_ops armctrl_ops = {
	.xlate = armctrl_xlate
};

static int __init armctrl_of_init(struct device_node *node,
				  struct device_node *parent,
				  bool is_2836)
{
	void __iomem *base;
	int irq, b, i;
	u32 reg;

	base = of_iomap(node, 0);
	if (!base)
		panic("%pOF: unable to map IC registers\n", node);

	intc.domain = irq_domain_add_linear(node, MAKE_HWIRQ(NR_BANKS, 0),
			&armctrl_ops, NULL);
	if (!intc.domain)
		panic("%pOF: unable to create IRQ domain\n", node);

	for (b = 0; b < NR_BANKS; b++) {
		intc.pending[b] = base + reg_pending[b];
		intc.enable[b] = base + reg_enable[b];
		intc.disable[b] = base + reg_disable[b];

		for (i = 0; i < bank_irqs[b]; i++) {
			irq = irq_create_mapping(intc.domain, MAKE_HWIRQ(b, i));
			BUG_ON(irq <= 0);
			irq_set_chip_and_handler(irq, &armctrl_chip,
				handle_level_irq);
			irq_set_probe(irq);
		}

		reg = readl_relaxed(intc.enable[b]);
		if (reg) {
			writel_relaxed(reg, intc.disable[b]);
			pr_err(FW_BUG "Bootloader left irq enabled: "
			       "bank %d irq %*pbl\n", b, IRQS_PER_BANK, &reg);
		}
	}

	reg = readl_relaxed(base + REG_FIQ_CONTROL);
	if (reg & FIQ_CONTROL_ENABLE) {
		writel_relaxed(0, base + REG_FIQ_CONTROL);
		pr_err(FW_BUG "Bootloader left fiq enabled\n");
	}

	if (is_2836) {
		int parent_irq = irq_of_parse_and_map(node, 0);

		if (!parent_irq) {
			panic("%pOF: unable to get parent interrupt.\n",
			      node);
		}
		irq_set_chained_handler(parent_irq, bcm2836_chained_handle_irq);
	} else {
		set_handle_irq(bcm2835_handle_irq);
	}

	return 0;
}

static int __init bcm2835_armctrl_of_init(struct device_node *node,
					  struct device_node *parent)
{
	return armctrl_of_init(node, parent, false);
}

static int __init bcm2836_armctrl_of_init(struct device_node *node,
					  struct device_node *parent)
{
	return armctrl_of_init(node, parent, true);
}


/*
 * Handle each interrupt across the entire interrupt controller.  This reads the
 * status register before handling each interrupt, which is necessary given that
 * handle_IRQ may briefly re-enable interrupts for soft IRQ handling.
 */

static u32 armctrl_translate_bank(int bank)
{
	u32 stat = readl_relaxed(intc.pending[bank]);

	return MAKE_HWIRQ(bank, ffs(stat) - 1);
}

static u32 armctrl_translate_shortcut(int bank, u32 stat)
{
	return MAKE_HWIRQ(bank, shortcuts[ffs(stat >> SHORTCUT_SHIFT) - 1]);
}

static u32 get_next_armctrl_hwirq(void)
{
	u32 stat = readl_relaxed(intc.pending[0]) & BANK0_VALID_MASK;

	if (stat == 0)
		return ~0;
	else if (stat & BANK0_HWIRQ_MASK)
		return MAKE_HWIRQ(0, ffs(stat & BANK0_HWIRQ_MASK) - 1);
	else if (stat & SHORTCUT1_MASK)
		return armctrl_translate_shortcut(1, stat & SHORTCUT1_MASK);
	else if (stat & SHORTCUT2_MASK)
		return armctrl_translate_shortcut(2, stat & SHORTCUT2_MASK);
	else if (stat & BANK1_HWIRQ)
		return armctrl_translate_bank(1);
	else if (stat & BANK2_HWIRQ)
		return armctrl_translate_bank(2);
	else
		BUG();
}

static void __exception_irq_entry bcm2835_handle_irq(
	struct pt_regs *regs)
{
	u32 hwirq;

	while ((hwirq = get_next_armctrl_hwirq()) != ~0)
		handle_domain_irq(intc.domain, hwirq, regs);
}

static void bcm2836_chained_handle_irq(struct irq_desc *desc)
{
	u32 hwirq;

	while ((hwirq = get_next_armctrl_hwirq()) != ~0)
		generic_handle_irq(irq_linear_revmap(intc.domain, hwirq));
}

IRQCHIP_DECLARE(bcm2835_armctrl_ic, "brcm,bcm2835-armctrl-ic",
		bcm2835_armctrl_of_init);
IRQCHIP_DECLARE(bcm2836_armctrl_ic, "brcm,bcm2836-armctrl-ic",
		bcm2836_armctrl_of_init);
