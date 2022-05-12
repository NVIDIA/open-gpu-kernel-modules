// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/common/sa1111.c
 *
 * SA1111 support
 *
 * Original code by John Dorsey
 *
 * This file contains all generic SA1111 support.
 *
 * All initialization functions provided here are intended to be called
 * from machine specific code with proper arguments when required.
 */
#include <linux/module.h>
#include <linux/gpio/driver.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/dma-map-ops.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <asm/mach/irq.h>
#include <asm/mach-types.h>
#include <linux/sizes.h>

#include <asm/hardware/sa1111.h>

/* SA1111 IRQs */
#define IRQ_GPAIN0		(0)
#define IRQ_GPAIN1		(1)
#define IRQ_GPAIN2		(2)
#define IRQ_GPAIN3		(3)
#define IRQ_GPBIN0		(4)
#define IRQ_GPBIN1		(5)
#define IRQ_GPBIN2		(6)
#define IRQ_GPBIN3		(7)
#define IRQ_GPBIN4		(8)
#define IRQ_GPBIN5		(9)
#define IRQ_GPCIN0		(10)
#define IRQ_GPCIN1		(11)
#define IRQ_GPCIN2		(12)
#define IRQ_GPCIN3		(13)
#define IRQ_GPCIN4		(14)
#define IRQ_GPCIN5		(15)
#define IRQ_GPCIN6		(16)
#define IRQ_GPCIN7		(17)
#define IRQ_MSTXINT		(18)
#define IRQ_MSRXINT		(19)
#define IRQ_MSSTOPERRINT	(20)
#define IRQ_TPTXINT		(21)
#define IRQ_TPRXINT		(22)
#define IRQ_TPSTOPERRINT	(23)
#define SSPXMTINT		(24)
#define SSPRCVINT		(25)
#define SSPROR			(26)
#define AUDXMTDMADONEA		(32)
#define AUDRCVDMADONEA		(33)
#define AUDXMTDMADONEB		(34)
#define AUDRCVDMADONEB		(35)
#define AUDTFSR			(36)
#define AUDRFSR			(37)
#define AUDTUR			(38)
#define AUDROR			(39)
#define AUDDTS			(40)
#define AUDRDD			(41)
#define AUDSTO			(42)
#define IRQ_USBPWR		(43)
#define IRQ_HCIM		(44)
#define IRQ_HCIBUFFACC		(45)
#define IRQ_HCIRMTWKP		(46)
#define IRQ_NHCIMFCIR		(47)
#define IRQ_USB_PORT_RESUME	(48)
#define IRQ_S0_READY_NINT	(49)
#define IRQ_S1_READY_NINT	(50)
#define IRQ_S0_CD_VALID		(51)
#define IRQ_S1_CD_VALID		(52)
#define IRQ_S0_BVD1_STSCHG	(53)
#define IRQ_S1_BVD1_STSCHG	(54)
#define SA1111_IRQ_NR		(55)

extern void sa1110_mb_enable(void);
extern void sa1110_mb_disable(void);

/*
 * We keep the following data for the overall SA1111.  Note that the
 * struct device and struct resource are "fake"; they should be supplied
 * by the bus above us.  However, in the interests of getting all SA1111
 * drivers converted over to the device model, we provide this as an
 * anchor point for all the other drivers.
 */
struct sa1111 {
	struct device	*dev;
	struct clk	*clk;
	unsigned long	phys;
	int		irq;
	int		irq_base;	/* base for cascaded on-chip IRQs */
	spinlock_t	lock;
	void __iomem	*base;
	struct sa1111_platform_data *pdata;
	struct irq_domain *irqdomain;
	struct gpio_chip gc;
#ifdef CONFIG_PM
	void		*saved_state;
#endif
};

/*
 * We _really_ need to eliminate this.  Its only users
 * are the PWM and DMA checking code.
 */
static struct sa1111 *g_sa1111;

struct sa1111_dev_info {
	unsigned long	offset;
	unsigned long	skpcr_mask;
	bool		dma;
	unsigned int	devid;
	unsigned int	hwirq[6];
};

static struct sa1111_dev_info sa1111_devices[] = {
	{
		.offset		= SA1111_USB,
		.skpcr_mask	= SKPCR_UCLKEN,
		.dma		= true,
		.devid		= SA1111_DEVID_USB,
		.hwirq = {
			IRQ_USBPWR,
			IRQ_HCIM,
			IRQ_HCIBUFFACC,
			IRQ_HCIRMTWKP,
			IRQ_NHCIMFCIR,
			IRQ_USB_PORT_RESUME
		},
	},
	{
		.offset		= 0x0600,
		.skpcr_mask	= SKPCR_I2SCLKEN | SKPCR_L3CLKEN,
		.dma		= true,
		.devid		= SA1111_DEVID_SAC,
		.hwirq = {
			AUDXMTDMADONEA,
			AUDXMTDMADONEB,
			AUDRCVDMADONEA,
			AUDRCVDMADONEB
		},
	},
	{
		.offset		= 0x0800,
		.skpcr_mask	= SKPCR_SCLKEN,
		.devid		= SA1111_DEVID_SSP,
	},
	{
		.offset		= SA1111_KBD,
		.skpcr_mask	= SKPCR_PTCLKEN,
		.devid		= SA1111_DEVID_PS2_KBD,
		.hwirq = {
			IRQ_TPRXINT,
			IRQ_TPTXINT
		},
	},
	{
		.offset		= SA1111_MSE,
		.skpcr_mask	= SKPCR_PMCLKEN,
		.devid		= SA1111_DEVID_PS2_MSE,
		.hwirq = {
			IRQ_MSRXINT,
			IRQ_MSTXINT
		},
	},
	{
		.offset		= 0x1800,
		.skpcr_mask	= 0,
		.devid		= SA1111_DEVID_PCMCIA,
		.hwirq = {
			IRQ_S0_READY_NINT,
			IRQ_S0_CD_VALID,
			IRQ_S0_BVD1_STSCHG,
			IRQ_S1_READY_NINT,
			IRQ_S1_CD_VALID,
			IRQ_S1_BVD1_STSCHG,
		},
	},
};

static int sa1111_map_irq(struct sa1111 *sachip, irq_hw_number_t hwirq)
{
	return irq_create_mapping(sachip->irqdomain, hwirq);
}

static void sa1111_handle_irqdomain(struct irq_domain *irqdomain, int irq)
{
	struct irq_desc *d = irq_to_desc(irq_linear_revmap(irqdomain, irq));

	if (d)
		generic_handle_irq_desc(d);
}

/*
 * SA1111 interrupt support.  Since clearing an IRQ while there are
 * active IRQs causes the interrupt output to pulse, the upper levels
 * will call us again if there are more interrupts to process.
 */
static void sa1111_irq_handler(struct irq_desc *desc)
{
	unsigned int stat0, stat1, i;
	struct sa1111 *sachip = irq_desc_get_handler_data(desc);
	struct irq_domain *irqdomain;
	void __iomem *mapbase = sachip->base + SA1111_INTC;

	stat0 = readl_relaxed(mapbase + SA1111_INTSTATCLR0);
	stat1 = readl_relaxed(mapbase + SA1111_INTSTATCLR1);

	writel_relaxed(stat0, mapbase + SA1111_INTSTATCLR0);

	desc->irq_data.chip->irq_ack(&desc->irq_data);

	writel_relaxed(stat1, mapbase + SA1111_INTSTATCLR1);

	if (stat0 == 0 && stat1 == 0) {
		do_bad_IRQ(desc);
		return;
	}

	irqdomain = sachip->irqdomain;

	for (i = 0; stat0; i++, stat0 >>= 1)
		if (stat0 & 1)
			sa1111_handle_irqdomain(irqdomain, i);

	for (i = 32; stat1; i++, stat1 >>= 1)
		if (stat1 & 1)
			sa1111_handle_irqdomain(irqdomain, i);

	/* For level-based interrupts */
	desc->irq_data.chip->irq_unmask(&desc->irq_data);
}

static u32 sa1111_irqmask(struct irq_data *d)
{
	return BIT(irqd_to_hwirq(d) & 31);
}

static int sa1111_irqbank(struct irq_data *d)
{
	return (irqd_to_hwirq(d) / 32) * 4;
}

static void sa1111_ack_irq(struct irq_data *d)
{
}

static void sa1111_mask_irq(struct irq_data *d)
{
	struct sa1111 *sachip = irq_data_get_irq_chip_data(d);
	void __iomem *mapbase = sachip->base + SA1111_INTC + sa1111_irqbank(d);
	u32 ie;

	ie = readl_relaxed(mapbase + SA1111_INTEN0);
	ie &= ~sa1111_irqmask(d);
	writel(ie, mapbase + SA1111_INTEN0);
}

static void sa1111_unmask_irq(struct irq_data *d)
{
	struct sa1111 *sachip = irq_data_get_irq_chip_data(d);
	void __iomem *mapbase = sachip->base + SA1111_INTC + sa1111_irqbank(d);
	u32 ie;

	ie = readl_relaxed(mapbase + SA1111_INTEN0);
	ie |= sa1111_irqmask(d);
	writel_relaxed(ie, mapbase + SA1111_INTEN0);
}

/*
 * Attempt to re-trigger the interrupt.  The SA1111 contains a register
 * (INTSET) which claims to do this.  However, in practice no amount of
 * manipulation of INTEN and INTSET guarantees that the interrupt will
 * be triggered.  In fact, its very difficult, if not impossible to get
 * INTSET to re-trigger the interrupt.
 */
static int sa1111_retrigger_irq(struct irq_data *d)
{
	struct sa1111 *sachip = irq_data_get_irq_chip_data(d);
	void __iomem *mapbase = sachip->base + SA1111_INTC + sa1111_irqbank(d);
	u32 ip, mask = sa1111_irqmask(d);
	int i;

	ip = readl_relaxed(mapbase + SA1111_INTPOL0);
	for (i = 0; i < 8; i++) {
		writel_relaxed(ip ^ mask, mapbase + SA1111_INTPOL0);
		writel_relaxed(ip, mapbase + SA1111_INTPOL0);
		if (readl_relaxed(mapbase + SA1111_INTSTATCLR0) & mask)
			break;
	}

	if (i == 8) {
		pr_err("Danger Will Robinson: failed to re-trigger IRQ%d\n",
		       d->irq);
		return 0;
	}

	return 1;
}

static int sa1111_type_irq(struct irq_data *d, unsigned int flags)
{
	struct sa1111 *sachip = irq_data_get_irq_chip_data(d);
	void __iomem *mapbase = sachip->base + SA1111_INTC + sa1111_irqbank(d);
	u32 ip, mask = sa1111_irqmask(d);

	if (flags == IRQ_TYPE_PROBE)
		return 0;

	if ((!(flags & IRQ_TYPE_EDGE_RISING) ^ !(flags & IRQ_TYPE_EDGE_FALLING)) == 0)
		return -EINVAL;

	ip = readl_relaxed(mapbase + SA1111_INTPOL0);
	if (flags & IRQ_TYPE_EDGE_RISING)
		ip &= ~mask;
	else
		ip |= mask;
	writel_relaxed(ip, mapbase + SA1111_INTPOL0);
	writel_relaxed(ip, mapbase + SA1111_WAKEPOL0);

	return 0;
}

static int sa1111_wake_irq(struct irq_data *d, unsigned int on)
{
	struct sa1111 *sachip = irq_data_get_irq_chip_data(d);
	void __iomem *mapbase = sachip->base + SA1111_INTC + sa1111_irqbank(d);
	u32 we, mask = sa1111_irqmask(d);

	we = readl_relaxed(mapbase + SA1111_WAKEEN0);
	if (on)
		we |= mask;
	else
		we &= ~mask;
	writel_relaxed(we, mapbase + SA1111_WAKEEN0);

	return 0;
}

static struct irq_chip sa1111_irq_chip = {
	.name		= "SA1111",
	.irq_ack	= sa1111_ack_irq,
	.irq_mask	= sa1111_mask_irq,
	.irq_unmask	= sa1111_unmask_irq,
	.irq_retrigger	= sa1111_retrigger_irq,
	.irq_set_type	= sa1111_type_irq,
	.irq_set_wake	= sa1111_wake_irq,
};

static int sa1111_irqdomain_map(struct irq_domain *d, unsigned int irq,
	irq_hw_number_t hwirq)
{
	struct sa1111 *sachip = d->host_data;

	/* Disallow unavailable interrupts */
	if (hwirq > SSPROR && hwirq < AUDXMTDMADONEA)
		return -EINVAL;

	irq_set_chip_data(irq, sachip);
	irq_set_chip_and_handler(irq, &sa1111_irq_chip, handle_edge_irq);
	irq_clear_status_flags(irq, IRQ_NOREQUEST | IRQ_NOPROBE);

	return 0;
}

static const struct irq_domain_ops sa1111_irqdomain_ops = {
	.map = sa1111_irqdomain_map,
	.xlate = irq_domain_xlate_twocell,
};

static int sa1111_setup_irq(struct sa1111 *sachip, unsigned irq_base)
{
	void __iomem *irqbase = sachip->base + SA1111_INTC;
	int ret;

	/*
	 * We're guaranteed that this region hasn't been taken.
	 */
	request_mem_region(sachip->phys + SA1111_INTC, 512, "irq");

	ret = irq_alloc_descs(-1, irq_base, SA1111_IRQ_NR, -1);
	if (ret <= 0) {
		dev_err(sachip->dev, "unable to allocate %u irqs: %d\n",
			SA1111_IRQ_NR, ret);
		if (ret == 0)
			ret = -EINVAL;
		return ret;
	}

	sachip->irq_base = ret;

	/* disable all IRQs */
	writel_relaxed(0, irqbase + SA1111_INTEN0);
	writel_relaxed(0, irqbase + SA1111_INTEN1);
	writel_relaxed(0, irqbase + SA1111_WAKEEN0);
	writel_relaxed(0, irqbase + SA1111_WAKEEN1);

	/*
	 * detect on rising edge.  Note: Feb 2001 Errata for SA1111
	 * specifies that S0ReadyInt and S1ReadyInt should be '1'.
	 */
	writel_relaxed(0, irqbase + SA1111_INTPOL0);
	writel_relaxed(BIT(IRQ_S0_READY_NINT & 31) |
		       BIT(IRQ_S1_READY_NINT & 31),
		       irqbase + SA1111_INTPOL1);

	/* clear all IRQs */
	writel_relaxed(~0, irqbase + SA1111_INTSTATCLR0);
	writel_relaxed(~0, irqbase + SA1111_INTSTATCLR1);

	sachip->irqdomain = irq_domain_add_linear(NULL, SA1111_IRQ_NR,
						  &sa1111_irqdomain_ops,
						  sachip);
	if (!sachip->irqdomain) {
		irq_free_descs(sachip->irq_base, SA1111_IRQ_NR);
		return -ENOMEM;
	}

	irq_domain_associate_many(sachip->irqdomain,
				  sachip->irq_base + IRQ_GPAIN0,
				  IRQ_GPAIN0, SSPROR + 1 - IRQ_GPAIN0);
	irq_domain_associate_many(sachip->irqdomain,
				  sachip->irq_base + AUDXMTDMADONEA,
				  AUDXMTDMADONEA,
				  IRQ_S1_BVD1_STSCHG + 1 - AUDXMTDMADONEA);

	/*
	 * Register SA1111 interrupt
	 */
	irq_set_irq_type(sachip->irq, IRQ_TYPE_EDGE_RISING);
	irq_set_chained_handler_and_data(sachip->irq, sa1111_irq_handler,
					 sachip);

	dev_info(sachip->dev, "Providing IRQ%u-%u\n",
		sachip->irq_base, sachip->irq_base + SA1111_IRQ_NR - 1);

	return 0;
}

static void sa1111_remove_irq(struct sa1111 *sachip)
{
	struct irq_domain *domain = sachip->irqdomain;
	void __iomem *irqbase = sachip->base + SA1111_INTC;
	int i;

	/* disable all IRQs */
	writel_relaxed(0, irqbase + SA1111_INTEN0);
	writel_relaxed(0, irqbase + SA1111_INTEN1);
	writel_relaxed(0, irqbase + SA1111_WAKEEN0);
	writel_relaxed(0, irqbase + SA1111_WAKEEN1);

	irq_set_chained_handler_and_data(sachip->irq, NULL, NULL);
	for (i = 0; i < SA1111_IRQ_NR; i++)
		irq_dispose_mapping(irq_find_mapping(domain, i));
	irq_domain_remove(domain);

	release_mem_region(sachip->phys + SA1111_INTC, 512);
}

enum {
	SA1111_GPIO_PXDDR = (SA1111_GPIO_PADDR - SA1111_GPIO_PADDR),
	SA1111_GPIO_PXDRR = (SA1111_GPIO_PADRR - SA1111_GPIO_PADDR),
	SA1111_GPIO_PXDWR = (SA1111_GPIO_PADWR - SA1111_GPIO_PADDR),
	SA1111_GPIO_PXSDR = (SA1111_GPIO_PASDR - SA1111_GPIO_PADDR),
	SA1111_GPIO_PXSSR = (SA1111_GPIO_PASSR - SA1111_GPIO_PADDR),
};

static struct sa1111 *gc_to_sa1111(struct gpio_chip *gc)
{
	return container_of(gc, struct sa1111, gc);
}

static void __iomem *sa1111_gpio_map_reg(struct sa1111 *sachip, unsigned offset)
{
	void __iomem *reg = sachip->base + SA1111_GPIO;

	if (offset < 4)
		return reg + SA1111_GPIO_PADDR;
	if (offset < 10)
		return reg + SA1111_GPIO_PBDDR;
	if (offset < 18)
		return reg + SA1111_GPIO_PCDDR;
	return NULL;
}

static u32 sa1111_gpio_map_bit(unsigned offset)
{
	if (offset < 4)
		return BIT(offset);
	if (offset < 10)
		return BIT(offset - 4);
	if (offset < 18)
		return BIT(offset - 10);
	return 0;
}

static void sa1111_gpio_modify(void __iomem *reg, u32 mask, u32 set)
{
	u32 val;

	val = readl_relaxed(reg);
	val &= ~mask;
	val |= mask & set;
	writel_relaxed(val, reg);
}

static int sa1111_gpio_get_direction(struct gpio_chip *gc, unsigned offset)
{
	struct sa1111 *sachip = gc_to_sa1111(gc);
	void __iomem *reg = sa1111_gpio_map_reg(sachip, offset);
	u32 mask = sa1111_gpio_map_bit(offset);

	return !!(readl_relaxed(reg + SA1111_GPIO_PXDDR) & mask);
}

static int sa1111_gpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	struct sa1111 *sachip = gc_to_sa1111(gc);
	unsigned long flags;
	void __iomem *reg = sa1111_gpio_map_reg(sachip, offset);
	u32 mask = sa1111_gpio_map_bit(offset);

	spin_lock_irqsave(&sachip->lock, flags);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXDDR, mask, mask);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXSDR, mask, mask);
	spin_unlock_irqrestore(&sachip->lock, flags);

	return 0;
}

static int sa1111_gpio_direction_output(struct gpio_chip *gc, unsigned offset,
	int value)
{
	struct sa1111 *sachip = gc_to_sa1111(gc);
	unsigned long flags;
	void __iomem *reg = sa1111_gpio_map_reg(sachip, offset);
	u32 mask = sa1111_gpio_map_bit(offset);

	spin_lock_irqsave(&sachip->lock, flags);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXDWR, mask, value ? mask : 0);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXSSR, mask, value ? mask : 0);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXDDR, mask, 0);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXSDR, mask, 0);
	spin_unlock_irqrestore(&sachip->lock, flags);

	return 0;
}

static int sa1111_gpio_get(struct gpio_chip *gc, unsigned offset)
{
	struct sa1111 *sachip = gc_to_sa1111(gc);
	void __iomem *reg = sa1111_gpio_map_reg(sachip, offset);
	u32 mask = sa1111_gpio_map_bit(offset);

	return !!(readl_relaxed(reg + SA1111_GPIO_PXDRR) & mask);
}

static void sa1111_gpio_set(struct gpio_chip *gc, unsigned offset, int value)
{
	struct sa1111 *sachip = gc_to_sa1111(gc);
	unsigned long flags;
	void __iomem *reg = sa1111_gpio_map_reg(sachip, offset);
	u32 mask = sa1111_gpio_map_bit(offset);

	spin_lock_irqsave(&sachip->lock, flags);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXDWR, mask, value ? mask : 0);
	sa1111_gpio_modify(reg + SA1111_GPIO_PXSSR, mask, value ? mask : 0);
	spin_unlock_irqrestore(&sachip->lock, flags);
}

static void sa1111_gpio_set_multiple(struct gpio_chip *gc, unsigned long *mask,
	unsigned long *bits)
{
	struct sa1111 *sachip = gc_to_sa1111(gc);
	unsigned long flags;
	void __iomem *reg = sachip->base + SA1111_GPIO;
	u32 msk, val;

	msk = *mask;
	val = *bits;

	spin_lock_irqsave(&sachip->lock, flags);
	sa1111_gpio_modify(reg + SA1111_GPIO_PADWR, msk & 15, val);
	sa1111_gpio_modify(reg + SA1111_GPIO_PASSR, msk & 15, val);
	sa1111_gpio_modify(reg + SA1111_GPIO_PBDWR, (msk >> 4) & 255, val >> 4);
	sa1111_gpio_modify(reg + SA1111_GPIO_PBSSR, (msk >> 4) & 255, val >> 4);
	sa1111_gpio_modify(reg + SA1111_GPIO_PCDWR, (msk >> 12) & 255, val >> 12);
	sa1111_gpio_modify(reg + SA1111_GPIO_PCSSR, (msk >> 12) & 255, val >> 12);
	spin_unlock_irqrestore(&sachip->lock, flags);
}

static int sa1111_gpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	struct sa1111 *sachip = gc_to_sa1111(gc);

	return sa1111_map_irq(sachip, offset);
}

static int sa1111_setup_gpios(struct sa1111 *sachip)
{
	sachip->gc.label = "sa1111";
	sachip->gc.parent = sachip->dev;
	sachip->gc.owner = THIS_MODULE;
	sachip->gc.get_direction = sa1111_gpio_get_direction;
	sachip->gc.direction_input = sa1111_gpio_direction_input;
	sachip->gc.direction_output = sa1111_gpio_direction_output;
	sachip->gc.get = sa1111_gpio_get;
	sachip->gc.set = sa1111_gpio_set;
	sachip->gc.set_multiple = sa1111_gpio_set_multiple;
	sachip->gc.to_irq = sa1111_gpio_to_irq;
	sachip->gc.base = -1;
	sachip->gc.ngpio = 18;

	return devm_gpiochip_add_data(sachip->dev, &sachip->gc, sachip);
}

/*
 * Bring the SA1111 out of reset.  This requires a set procedure:
 *  1. nRESET asserted (by hardware)
 *  2. CLK turned on from SA1110
 *  3. nRESET deasserted
 *  4. VCO turned on, PLL_BYPASS turned off
 *  5. Wait lock time, then assert RCLKEn
 *  7. PCR set to allow clocking of individual functions
 *
 * Until we've done this, the only registers we can access are:
 *   SBI_SKCR
 *   SBI_SMCR
 *   SBI_SKID
 */
static void sa1111_wake(struct sa1111 *sachip)
{
	unsigned long flags, r;

	spin_lock_irqsave(&sachip->lock, flags);

	clk_enable(sachip->clk);

	/*
	 * Turn VCO on, and disable PLL Bypass.
	 */
	r = readl_relaxed(sachip->base + SA1111_SKCR);
	r &= ~SKCR_VCO_OFF;
	writel_relaxed(r, sachip->base + SA1111_SKCR);
	r |= SKCR_PLL_BYPASS | SKCR_OE_EN;
	writel_relaxed(r, sachip->base + SA1111_SKCR);

	/*
	 * Wait lock time.  SA1111 manual _doesn't_
	 * specify a figure for this!  We choose 100us.
	 */
	udelay(100);

	/*
	 * Enable RCLK.  We also ensure that RDYEN is set.
	 */
	r |= SKCR_RCLKEN | SKCR_RDYEN;
	writel_relaxed(r, sachip->base + SA1111_SKCR);

	/*
	 * Wait 14 RCLK cycles for the chip to finish coming out
	 * of reset. (RCLK=24MHz).  This is 590ns.
	 */
	udelay(1);

	/*
	 * Ensure all clocks are initially off.
	 */
	writel_relaxed(0, sachip->base + SA1111_SKPCR);

	spin_unlock_irqrestore(&sachip->lock, flags);
}

#ifdef CONFIG_ARCH_SA1100

static u32 sa1111_dma_mask[] = {
	~0,
	~(1 << 20),
	~(1 << 23),
	~(1 << 24),
	~(1 << 25),
	~(1 << 20),
	~(1 << 20),
	0,
};

/*
 * Configure the SA1111 shared memory controller.
 */
void
sa1111_configure_smc(struct sa1111 *sachip, int sdram, unsigned int drac,
		     unsigned int cas_latency)
{
	unsigned int smcr = SMCR_DTIM | SMCR_MBGE | FInsrt(drac, SMCR_DRAC);

	if (cas_latency == 3)
		smcr |= SMCR_CLAT;

	writel_relaxed(smcr, sachip->base + SA1111_SMCR);

	/*
	 * Now clear the bits in the DMA mask to work around the SA1111
	 * DMA erratum (Intel StrongARM SA-1111 Microprocessor Companion
	 * Chip Specification Update, June 2000, Erratum #7).
	 */
	if (sachip->dev->dma_mask)
		*sachip->dev->dma_mask &= sa1111_dma_mask[drac >> 2];

	sachip->dev->coherent_dma_mask &= sa1111_dma_mask[drac >> 2];
}
#endif

static void sa1111_dev_release(struct device *_dev)
{
	struct sa1111_dev *dev = to_sa1111_device(_dev);

	kfree(dev);
}

static int
sa1111_init_one_child(struct sa1111 *sachip, struct resource *parent,
		      struct sa1111_dev_info *info)
{
	struct sa1111_dev *dev;
	unsigned i;
	int ret;

	dev = kzalloc(sizeof(struct sa1111_dev), GFP_KERNEL);
	if (!dev) {
		ret = -ENOMEM;
		goto err_alloc;
	}

	device_initialize(&dev->dev);
	dev_set_name(&dev->dev, "%4.4lx", info->offset);
	dev->devid	 = info->devid;
	dev->dev.parent  = sachip->dev;
	dev->dev.bus     = &sa1111_bus_type;
	dev->dev.release = sa1111_dev_release;
	dev->res.start   = sachip->phys + info->offset;
	dev->res.end     = dev->res.start + 511;
	dev->res.name    = dev_name(&dev->dev);
	dev->res.flags   = IORESOURCE_MEM;
	dev->mapbase     = sachip->base + info->offset;
	dev->skpcr_mask  = info->skpcr_mask;

	for (i = 0; i < ARRAY_SIZE(info->hwirq); i++)
		dev->hwirq[i] = info->hwirq[i];

	/*
	 * If the parent device has a DMA mask associated with it, and
	 * this child supports DMA, propagate it down to the children.
	 */
	if (info->dma && sachip->dev->dma_mask) {
		dev->dma_mask = *sachip->dev->dma_mask;
		dev->dev.dma_mask = &dev->dma_mask;
		dev->dev.coherent_dma_mask = sachip->dev->coherent_dma_mask;
	}

	ret = request_resource(parent, &dev->res);
	if (ret) {
		dev_err(sachip->dev, "failed to allocate resource for %s\n",
			dev->res.name);
		goto err_resource;
	}

	ret = device_add(&dev->dev);
	if (ret)
		goto err_add;
	return 0;

 err_add:
	release_resource(&dev->res);
 err_resource:
	put_device(&dev->dev);
 err_alloc:
	return ret;
}

/**
 *	sa1111_probe - probe for a single SA1111 chip.
 *	@phys_addr: physical address of device.
 *
 *	Probe for a SA1111 chip.  This must be called
 *	before any other SA1111-specific code.
 *
 *	Returns:
 *	%-ENODEV	device not found.
 *	%-EBUSY		physical address already marked in-use.
 *	%-EINVAL	no platform data passed
 *	%0		successful.
 */
static int __sa1111_probe(struct device *me, struct resource *mem, int irq)
{
	struct sa1111_platform_data *pd = me->platform_data;
	struct sa1111 *sachip;
	unsigned long id;
	unsigned int has_devs;
	int i, ret = -ENODEV;

	if (!pd)
		return -EINVAL;

	sachip = devm_kzalloc(me, sizeof(struct sa1111), GFP_KERNEL);
	if (!sachip)
		return -ENOMEM;

	sachip->clk = devm_clk_get(me, "SA1111_CLK");
	if (IS_ERR(sachip->clk))
		return PTR_ERR(sachip->clk);

	ret = clk_prepare(sachip->clk);
	if (ret)
		return ret;

	spin_lock_init(&sachip->lock);

	sachip->dev = me;
	dev_set_drvdata(sachip->dev, sachip);

	sachip->pdata = pd;
	sachip->phys = mem->start;
	sachip->irq = irq;

	/*
	 * Map the whole region.  This also maps the
	 * registers for our children.
	 */
	sachip->base = ioremap(mem->start, PAGE_SIZE * 2);
	if (!sachip->base) {
		ret = -ENOMEM;
		goto err_clk_unprep;
	}

	/*
	 * Probe for the chip.  Only touch the SBI registers.
	 */
	id = readl_relaxed(sachip->base + SA1111_SKID);
	if ((id & SKID_ID_MASK) != SKID_SA1111_ID) {
		printk(KERN_DEBUG "SA1111 not detected: ID = %08lx\n", id);
		ret = -ENODEV;
		goto err_unmap;
	}

	pr_info("SA1111 Microprocessor Companion Chip: silicon revision %lx, metal revision %lx\n",
		(id & SKID_SIREV_MASK) >> 4, id & SKID_MTREV_MASK);

	/*
	 * We found it.  Wake the chip up, and initialise.
	 */
	sa1111_wake(sachip);

	/*
	 * The interrupt controller must be initialised before any
	 * other device to ensure that the interrupts are available.
	 */
	ret = sa1111_setup_irq(sachip, pd->irq_base);
	if (ret)
		goto err_clk;

	/* Setup the GPIOs - should really be done after the IRQ setup */
	ret = sa1111_setup_gpios(sachip);
	if (ret)
		goto err_irq;

#ifdef CONFIG_ARCH_SA1100
	{
	unsigned int val;

	/*
	 * The SDRAM configuration of the SA1110 and the SA1111 must
	 * match.  This is very important to ensure that SA1111 accesses
	 * don't corrupt the SDRAM.  Note that this ungates the SA1111's
	 * MBGNT signal, so we must have called sa1110_mb_disable()
	 * beforehand.
	 */
	sa1111_configure_smc(sachip, 1,
			     FExtr(MDCNFG, MDCNFG_SA1110_DRAC0),
			     FExtr(MDCNFG, MDCNFG_SA1110_TDL0));

	/*
	 * We only need to turn on DCLK whenever we want to use the
	 * DMA.  It can otherwise be held firmly in the off position.
	 * (currently, we always enable it.)
	 */
	val = readl_relaxed(sachip->base + SA1111_SKPCR);
	writel_relaxed(val | SKPCR_DCLKEN, sachip->base + SA1111_SKPCR);

	/*
	 * Enable the SA1110 memory bus request and grant signals.
	 */
	sa1110_mb_enable();
	}
#endif

	g_sa1111 = sachip;

	has_devs = ~0;
	if (pd)
		has_devs &= ~pd->disable_devs;

	for (i = 0; i < ARRAY_SIZE(sa1111_devices); i++)
		if (sa1111_devices[i].devid & has_devs)
			sa1111_init_one_child(sachip, mem, &sa1111_devices[i]);

	return 0;

 err_irq:
	sa1111_remove_irq(sachip);
 err_clk:
	clk_disable(sachip->clk);
 err_unmap:
	iounmap(sachip->base);
 err_clk_unprep:
	clk_unprepare(sachip->clk);
	return ret;
}

static int sa1111_remove_one(struct device *dev, void *data)
{
	struct sa1111_dev *sadev = to_sa1111_device(dev);
	if (dev->bus != &sa1111_bus_type)
		return 0;
	device_del(&sadev->dev);
	release_resource(&sadev->res);
	put_device(&sadev->dev);
	return 0;
}

static void __sa1111_remove(struct sa1111 *sachip)
{
	device_for_each_child(sachip->dev, NULL, sa1111_remove_one);

	sa1111_remove_irq(sachip);

	clk_disable(sachip->clk);
	clk_unprepare(sachip->clk);

	iounmap(sachip->base);
}

struct sa1111_save_data {
	unsigned int	skcr;
	unsigned int	skpcr;
	unsigned int	skcdr;
	unsigned char	skaud;
	unsigned char	skpwm0;
	unsigned char	skpwm1;

	/*
	 * Interrupt controller
	 */
	unsigned int	intpol0;
	unsigned int	intpol1;
	unsigned int	inten0;
	unsigned int	inten1;
	unsigned int	wakepol0;
	unsigned int	wakepol1;
	unsigned int	wakeen0;
	unsigned int	wakeen1;
};

#ifdef CONFIG_PM

static int sa1111_suspend_noirq(struct device *dev)
{
	struct sa1111 *sachip = dev_get_drvdata(dev);
	struct sa1111_save_data *save;
	unsigned long flags;
	unsigned int val;
	void __iomem *base;

	save = kmalloc(sizeof(struct sa1111_save_data), GFP_KERNEL);
	if (!save)
		return -ENOMEM;
	sachip->saved_state = save;

	spin_lock_irqsave(&sachip->lock, flags);

	/*
	 * Save state.
	 */
	base = sachip->base;
	save->skcr     = readl_relaxed(base + SA1111_SKCR);
	save->skpcr    = readl_relaxed(base + SA1111_SKPCR);
	save->skcdr    = readl_relaxed(base + SA1111_SKCDR);
	save->skaud    = readl_relaxed(base + SA1111_SKAUD);
	save->skpwm0   = readl_relaxed(base + SA1111_SKPWM0);
	save->skpwm1   = readl_relaxed(base + SA1111_SKPWM1);

	writel_relaxed(0, sachip->base + SA1111_SKPWM0);
	writel_relaxed(0, sachip->base + SA1111_SKPWM1);

	base = sachip->base + SA1111_INTC;
	save->intpol0  = readl_relaxed(base + SA1111_INTPOL0);
	save->intpol1  = readl_relaxed(base + SA1111_INTPOL1);
	save->inten0   = readl_relaxed(base + SA1111_INTEN0);
	save->inten1   = readl_relaxed(base + SA1111_INTEN1);
	save->wakepol0 = readl_relaxed(base + SA1111_WAKEPOL0);
	save->wakepol1 = readl_relaxed(base + SA1111_WAKEPOL1);
	save->wakeen0  = readl_relaxed(base + SA1111_WAKEEN0);
	save->wakeen1  = readl_relaxed(base + SA1111_WAKEEN1);

	/*
	 * Disable.
	 */
	val = readl_relaxed(sachip->base + SA1111_SKCR);
	writel_relaxed(val | SKCR_SLEEP, sachip->base + SA1111_SKCR);

	clk_disable(sachip->clk);

	spin_unlock_irqrestore(&sachip->lock, flags);

#ifdef CONFIG_ARCH_SA1100
	sa1110_mb_disable();
#endif

	return 0;
}

/*
 *	sa1111_resume - Restore the SA1111 device state.
 *	@dev: device to restore
 *
 *	Restore the general state of the SA1111; clock control and
 *	interrupt controller.  Other parts of the SA1111 must be
 *	restored by their respective drivers, and must be called
 *	via LDM after this function.
 */
static int sa1111_resume_noirq(struct device *dev)
{
	struct sa1111 *sachip = dev_get_drvdata(dev);
	struct sa1111_save_data *save;
	unsigned long flags, id;
	void __iomem *base;

	save = sachip->saved_state;
	if (!save)
		return 0;

	/*
	 * Ensure that the SA1111 is still here.
	 * FIXME: shouldn't do this here.
	 */
	id = readl_relaxed(sachip->base + SA1111_SKID);
	if ((id & SKID_ID_MASK) != SKID_SA1111_ID) {
		__sa1111_remove(sachip);
		dev_set_drvdata(dev, NULL);
		kfree(save);
		return 0;
	}

	/*
	 * First of all, wake up the chip.
	 */
	sa1111_wake(sachip);

#ifdef CONFIG_ARCH_SA1100
	/* Enable the memory bus request/grant signals */
	sa1110_mb_enable();
#endif

	/*
	 * Only lock for write ops. Also, sa1111_wake must be called with
	 * released spinlock!
	 */
	spin_lock_irqsave(&sachip->lock, flags);

	writel_relaxed(0, sachip->base + SA1111_INTC + SA1111_INTEN0);
	writel_relaxed(0, sachip->base + SA1111_INTC + SA1111_INTEN1);

	base = sachip->base;
	writel_relaxed(save->skcr,     base + SA1111_SKCR);
	writel_relaxed(save->skpcr,    base + SA1111_SKPCR);
	writel_relaxed(save->skcdr,    base + SA1111_SKCDR);
	writel_relaxed(save->skaud,    base + SA1111_SKAUD);
	writel_relaxed(save->skpwm0,   base + SA1111_SKPWM0);
	writel_relaxed(save->skpwm1,   base + SA1111_SKPWM1);

	base = sachip->base + SA1111_INTC;
	writel_relaxed(save->intpol0,  base + SA1111_INTPOL0);
	writel_relaxed(save->intpol1,  base + SA1111_INTPOL1);
	writel_relaxed(save->inten0,   base + SA1111_INTEN0);
	writel_relaxed(save->inten1,   base + SA1111_INTEN1);
	writel_relaxed(save->wakepol0, base + SA1111_WAKEPOL0);
	writel_relaxed(save->wakepol1, base + SA1111_WAKEPOL1);
	writel_relaxed(save->wakeen0,  base + SA1111_WAKEEN0);
	writel_relaxed(save->wakeen1,  base + SA1111_WAKEEN1);

	spin_unlock_irqrestore(&sachip->lock, flags);

	sachip->saved_state = NULL;
	kfree(save);

	return 0;
}

#else
#define sa1111_suspend_noirq NULL
#define sa1111_resume_noirq  NULL
#endif

static int sa1111_probe(struct platform_device *pdev)
{
	struct resource *mem;
	int irq;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem)
		return -EINVAL;
	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	return __sa1111_probe(&pdev->dev, mem, irq);
}

static int sa1111_remove(struct platform_device *pdev)
{
	struct sa1111 *sachip = platform_get_drvdata(pdev);

	if (sachip) {
#ifdef CONFIG_PM
		kfree(sachip->saved_state);
		sachip->saved_state = NULL;
#endif
		__sa1111_remove(sachip);
		platform_set_drvdata(pdev, NULL);
	}

	return 0;
}

static struct dev_pm_ops sa1111_pm_ops = {
	.suspend_noirq = sa1111_suspend_noirq,
	.resume_noirq = sa1111_resume_noirq,
};

/*
 *	Not sure if this should be on the system bus or not yet.
 *	We really want some way to register a system device at
 *	the per-machine level, and then have this driver pick
 *	up the registered devices.
 *
 *	We also need to handle the SDRAM configuration for
 *	PXA250/SA1110 machine classes.
 */
static struct platform_driver sa1111_device_driver = {
	.probe		= sa1111_probe,
	.remove		= sa1111_remove,
	.driver		= {
		.name	= "sa1111",
		.pm	= &sa1111_pm_ops,
	},
};

/*
 *	Get the parent device driver (us) structure
 *	from a child function device
 */
static inline struct sa1111 *sa1111_chip_driver(struct sa1111_dev *sadev)
{
	return (struct sa1111 *)dev_get_drvdata(sadev->dev.parent);
}

/*
 * The bits in the opdiv field are non-linear.
 */
static unsigned char opdiv_table[] = { 1, 4, 2, 8 };

static unsigned int __sa1111_pll_clock(struct sa1111 *sachip)
{
	unsigned int skcdr, fbdiv, ipdiv, opdiv;

	skcdr = readl_relaxed(sachip->base + SA1111_SKCDR);

	fbdiv = (skcdr & 0x007f) + 2;
	ipdiv = ((skcdr & 0x0f80) >> 7) + 2;
	opdiv = opdiv_table[(skcdr & 0x3000) >> 12];

	return 3686400 * fbdiv / (ipdiv * opdiv);
}

/**
 *	sa1111_pll_clock - return the current PLL clock frequency.
 *	@sadev: SA1111 function block
 *
 *	BUG: we should look at SKCR.  We also blindly believe that
 *	the chip is being fed with the 3.6864MHz clock.
 *
 *	Returns the PLL clock in Hz.
 */
unsigned int sa1111_pll_clock(struct sa1111_dev *sadev)
{
	struct sa1111 *sachip = sa1111_chip_driver(sadev);

	return __sa1111_pll_clock(sachip);
}
EXPORT_SYMBOL(sa1111_pll_clock);

/**
 *	sa1111_select_audio_mode - select I2S or AC link mode
 *	@sadev: SA1111 function block
 *	@mode: One of %SA1111_AUDIO_ACLINK or %SA1111_AUDIO_I2S
 *
 *	Frob the SKCR to select AC Link mode or I2S mode for
 *	the audio block.
 */
void sa1111_select_audio_mode(struct sa1111_dev *sadev, int mode)
{
	struct sa1111 *sachip = sa1111_chip_driver(sadev);
	unsigned long flags;
	unsigned int val;

	spin_lock_irqsave(&sachip->lock, flags);

	val = readl_relaxed(sachip->base + SA1111_SKCR);
	if (mode == SA1111_AUDIO_I2S) {
		val &= ~SKCR_SELAC;
	} else {
		val |= SKCR_SELAC;
	}
	writel_relaxed(val, sachip->base + SA1111_SKCR);

	spin_unlock_irqrestore(&sachip->lock, flags);
}
EXPORT_SYMBOL(sa1111_select_audio_mode);

/**
 *	sa1111_set_audio_rate - set the audio sample rate
 *	@sadev: SA1111 SAC function block
 *	@rate: sample rate to select
 */
int sa1111_set_audio_rate(struct sa1111_dev *sadev, int rate)
{
	struct sa1111 *sachip = sa1111_chip_driver(sadev);
	unsigned int div;

	if (sadev->devid != SA1111_DEVID_SAC)
		return -EINVAL;

	div = (__sa1111_pll_clock(sachip) / 256 + rate / 2) / rate;
	if (div == 0)
		div = 1;
	if (div > 128)
		div = 128;

	writel_relaxed(div - 1, sachip->base + SA1111_SKAUD);

	return 0;
}
EXPORT_SYMBOL(sa1111_set_audio_rate);

/**
 *	sa1111_get_audio_rate - get the audio sample rate
 *	@sadev: SA1111 SAC function block device
 */
int sa1111_get_audio_rate(struct sa1111_dev *sadev)
{
	struct sa1111 *sachip = sa1111_chip_driver(sadev);
	unsigned long div;

	if (sadev->devid != SA1111_DEVID_SAC)
		return -EINVAL;

	div = readl_relaxed(sachip->base + SA1111_SKAUD) + 1;

	return __sa1111_pll_clock(sachip) / (256 * div);
}
EXPORT_SYMBOL(sa1111_get_audio_rate);

/*
 * Individual device operations.
 */

/**
 *	sa1111_enable_device - enable an on-chip SA1111 function block
 *	@sadev: SA1111 function block device to enable
 */
int sa1111_enable_device(struct sa1111_dev *sadev)
{
	struct sa1111 *sachip = sa1111_chip_driver(sadev);
	unsigned long flags;
	unsigned int val;
	int ret = 0;

	if (sachip->pdata && sachip->pdata->enable)
		ret = sachip->pdata->enable(sachip->pdata->data, sadev->devid);

	if (ret == 0) {
		spin_lock_irqsave(&sachip->lock, flags);
		val = readl_relaxed(sachip->base + SA1111_SKPCR);
		writel_relaxed(val | sadev->skpcr_mask, sachip->base + SA1111_SKPCR);
		spin_unlock_irqrestore(&sachip->lock, flags);
	}
	return ret;
}
EXPORT_SYMBOL(sa1111_enable_device);

/**
 *	sa1111_disable_device - disable an on-chip SA1111 function block
 *	@sadev: SA1111 function block device to disable
 */
void sa1111_disable_device(struct sa1111_dev *sadev)
{
	struct sa1111 *sachip = sa1111_chip_driver(sadev);
	unsigned long flags;
	unsigned int val;

	spin_lock_irqsave(&sachip->lock, flags);
	val = readl_relaxed(sachip->base + SA1111_SKPCR);
	writel_relaxed(val & ~sadev->skpcr_mask, sachip->base + SA1111_SKPCR);
	spin_unlock_irqrestore(&sachip->lock, flags);

	if (sachip->pdata && sachip->pdata->disable)
		sachip->pdata->disable(sachip->pdata->data, sadev->devid);
}
EXPORT_SYMBOL(sa1111_disable_device);

int sa1111_get_irq(struct sa1111_dev *sadev, unsigned num)
{
	struct sa1111 *sachip = sa1111_chip_driver(sadev);
	if (num >= ARRAY_SIZE(sadev->hwirq))
		return -EINVAL;
	return sa1111_map_irq(sachip, sadev->hwirq[num]);
}
EXPORT_SYMBOL_GPL(sa1111_get_irq);

/*
 *	SA1111 "Register Access Bus."
 *
 *	We model this as a regular bus type, and hang devices directly
 *	off this.
 */
static int sa1111_match(struct device *_dev, struct device_driver *_drv)
{
	struct sa1111_dev *dev = to_sa1111_device(_dev);
	struct sa1111_driver *drv = SA1111_DRV(_drv);

	return !!(dev->devid & drv->devid);
}

static int sa1111_bus_probe(struct device *dev)
{
	struct sa1111_dev *sadev = to_sa1111_device(dev);
	struct sa1111_driver *drv = SA1111_DRV(dev->driver);
	int ret = -ENODEV;

	if (drv->probe)
		ret = drv->probe(sadev);
	return ret;
}

static int sa1111_bus_remove(struct device *dev)
{
	struct sa1111_dev *sadev = to_sa1111_device(dev);
	struct sa1111_driver *drv = SA1111_DRV(dev->driver);

	if (drv->remove)
		drv->remove(sadev);

	return 0;
}

struct bus_type sa1111_bus_type = {
	.name		= "sa1111-rab",
	.match		= sa1111_match,
	.probe		= sa1111_bus_probe,
	.remove		= sa1111_bus_remove,
};
EXPORT_SYMBOL(sa1111_bus_type);

int sa1111_driver_register(struct sa1111_driver *driver)
{
	driver->drv.bus = &sa1111_bus_type;
	return driver_register(&driver->drv);
}
EXPORT_SYMBOL(sa1111_driver_register);

void sa1111_driver_unregister(struct sa1111_driver *driver)
{
	driver_unregister(&driver->drv);
}
EXPORT_SYMBOL(sa1111_driver_unregister);

#ifdef CONFIG_DMABOUNCE
/*
 * According to the "Intel StrongARM SA-1111 Microprocessor Companion
 * Chip Specification Update" (June 2000), erratum #7, there is a
 * significant bug in the SA1111 SDRAM shared memory controller.  If
 * an access to a region of memory above 1MB relative to the bank base,
 * it is important that address bit 10 _NOT_ be asserted. Depending
 * on the configuration of the RAM, bit 10 may correspond to one
 * of several different (processor-relative) address bits.
 *
 * This routine only identifies whether or not a given DMA address
 * is susceptible to the bug.
 *
 * This should only get called for sa1111_device types due to the
 * way we configure our device dma_masks.
 */
static int sa1111_needs_bounce(struct device *dev, dma_addr_t addr, size_t size)
{
	/*
	 * Section 4.6 of the "Intel StrongARM SA-1111 Development Module
	 * User's Guide" mentions that jumpers R51 and R52 control the
	 * target of SA-1111 DMA (either SDRAM bank 0 on Assabet, or
	 * SDRAM bank 1 on Neponset). The default configuration selects
	 * Assabet, so any address in bank 1 is necessarily invalid.
	 */
	return (machine_is_assabet() || machine_is_pfs168()) &&
		(addr >= 0xc8000000 || (addr + size) >= 0xc8000000);
}

static int sa1111_notifier_call(struct notifier_block *n, unsigned long action,
	void *data)
{
	struct sa1111_dev *dev = to_sa1111_device(data);

	switch (action) {
	case BUS_NOTIFY_ADD_DEVICE:
		if (dev->dev.dma_mask && dev->dma_mask < 0xffffffffUL) {
			int ret = dmabounce_register_dev(&dev->dev, 1024, 4096,
					sa1111_needs_bounce);
			if (ret)
				dev_err(&dev->dev, "failed to register with dmabounce: %d\n", ret);
		}
		break;

	case BUS_NOTIFY_DEL_DEVICE:
		if (dev->dev.dma_mask && dev->dma_mask < 0xffffffffUL)
			dmabounce_unregister_dev(&dev->dev);
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block sa1111_bus_notifier = {
	.notifier_call = sa1111_notifier_call,
};
#endif

static int __init sa1111_init(void)
{
	int ret = bus_register(&sa1111_bus_type);
#ifdef CONFIG_DMABOUNCE
	if (ret == 0)
		bus_register_notifier(&sa1111_bus_type, &sa1111_bus_notifier);
#endif
	if (ret == 0)
		platform_driver_register(&sa1111_device_driver);
	return ret;
}

static void __exit sa1111_exit(void)
{
	platform_driver_unregister(&sa1111_device_driver);
#ifdef CONFIG_DMABOUNCE
	bus_unregister_notifier(&sa1111_bus_type, &sa1111_bus_notifier);
#endif
	bus_unregister(&sa1111_bus_type);
}

subsys_initcall(sa1111_init);
module_exit(sa1111_exit);

MODULE_DESCRIPTION("Intel Corporation SA1111 core driver");
MODULE_LICENSE("GPL");
