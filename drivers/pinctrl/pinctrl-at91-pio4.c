// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for the Atmel PIO4 controller
 *
 * Copyright (C) 2015 Atmel,
 *               2015 Ludovic Desroches <ludovic.desroches@atmel.com>
 */

#include <dt-bindings/pinctrl/at91.h>
#include <linux/clk.h>
#include <linux/gpio/driver.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/slab.h>
#include "core.h"
#include "pinconf.h"
#include "pinctrl-utils.h"

/*
 * Warning:
 * In order to not introduce confusion between Atmel PIO groups and pinctrl
 * framework groups, Atmel PIO groups will be called banks, line is kept to
 * designed the pin id into this bank.
 */

#define ATMEL_PIO_MSKR		0x0000
#define ATMEL_PIO_CFGR		0x0004
#define		ATMEL_PIO_CFGR_FUNC_MASK	GENMASK(2, 0)
#define		ATMEL_PIO_DIR_MASK		BIT(8)
#define		ATMEL_PIO_PUEN_MASK		BIT(9)
#define		ATMEL_PIO_PDEN_MASK		BIT(10)
#define		ATMEL_PIO_SR_MASK		BIT(11)
#define		ATMEL_PIO_IFEN_MASK		BIT(12)
#define		ATMEL_PIO_IFSCEN_MASK		BIT(13)
#define		ATMEL_PIO_OPD_MASK		BIT(14)
#define		ATMEL_PIO_SCHMITT_MASK		BIT(15)
#define		ATMEL_PIO_DRVSTR_MASK		GENMASK(17, 16)
#define		ATMEL_PIO_DRVSTR_OFFSET		16
#define		ATMEL_PIO_CFGR_EVTSEL_MASK	GENMASK(26, 24)
#define		ATMEL_PIO_CFGR_EVTSEL_FALLING	(0 << 24)
#define		ATMEL_PIO_CFGR_EVTSEL_RISING	(1 << 24)
#define		ATMEL_PIO_CFGR_EVTSEL_BOTH	(2 << 24)
#define		ATMEL_PIO_CFGR_EVTSEL_LOW	(3 << 24)
#define		ATMEL_PIO_CFGR_EVTSEL_HIGH	(4 << 24)
#define ATMEL_PIO_PDSR		0x0008
#define ATMEL_PIO_LOCKSR	0x000C
#define ATMEL_PIO_SODR		0x0010
#define ATMEL_PIO_CODR		0x0014
#define ATMEL_PIO_ODSR		0x0018
#define ATMEL_PIO_IER		0x0020
#define ATMEL_PIO_IDR		0x0024
#define ATMEL_PIO_IMR		0x0028
#define ATMEL_PIO_ISR		0x002C
#define ATMEL_PIO_IOFR		0x003C

#define ATMEL_PIO_NPINS_PER_BANK	32
#define ATMEL_PIO_BANK(pin_id)		(pin_id / ATMEL_PIO_NPINS_PER_BANK)
#define ATMEL_PIO_LINE(pin_id)		(pin_id % ATMEL_PIO_NPINS_PER_BANK)
#define ATMEL_PIO_BANK_OFFSET		0x40

#define ATMEL_GET_PIN_NO(pinfunc)	((pinfunc) & 0xff)
#define ATMEL_GET_PIN_FUNC(pinfunc)	((pinfunc >> 16) & 0xf)
#define ATMEL_GET_PIN_IOSET(pinfunc)	((pinfunc >> 20) & 0xf)

/* Custom pinconf parameters */
#define ATMEL_PIN_CONFIG_DRIVE_STRENGTH	(PIN_CONFIG_END + 1)

/**
 * struct atmel_pioctrl_data - Atmel PIO controller (pinmux + gpio) data struct
 * @nbanks: number of PIO banks
 * @last_bank_count: number of lines in the last bank (can be less than
 *	the rest of the banks).
 * @slew_rate_support: slew rate support
 */
struct atmel_pioctrl_data {
	unsigned int nbanks;
	unsigned int last_bank_count;
	unsigned int slew_rate_support;
};

struct atmel_group {
	const char *name;
	u32 pin;
};

struct atmel_pin {
	unsigned int pin_id;
	unsigned int mux;
	unsigned int ioset;
	unsigned int bank;
	unsigned int line;
	const char *device;
};

/**
 * struct atmel_pioctrl - Atmel PIO controller (pinmux + gpio)
 * @reg_base: base address of the controller.
 * @clk: clock of the controller.
 * @nbanks: number of PIO groups, it can vary depending on the SoC.
 * @pinctrl_dev: pinctrl device registered.
 * @groups: groups table to provide group name and pin in the group to pinctrl.
 * @group_names: group names table to provide all the group/pin names to
 *     pinctrl or gpio.
 * @pins: pins table used for both pinctrl and gpio. pin_id, bank and line
 *     fields are set at probe time. Other ones are set when parsing dt
 *     pinctrl.
 * @npins: number of pins.
 * @gpio_chip: gpio chip registered.
 * @irq_domain: irq domain for the gpio controller.
 * @irqs: table containing the hw irq number of the bank. The index of the
 *     table is the bank id.
 * @pm_wakeup_sources: bitmap of wakeup sources (lines)
 * @pm_suspend_backup: backup/restore register values on suspend/resume
 * @dev: device entry for the Atmel PIO controller.
 * @node: node of the Atmel PIO controller.
 * @slew_rate_support: slew rate support
 */
struct atmel_pioctrl {
	void __iomem		*reg_base;
	struct clk		*clk;
	unsigned int		nbanks;
	struct pinctrl_dev	*pinctrl_dev;
	struct atmel_group	*groups;
	const char * const	*group_names;
	struct atmel_pin	**pins;
	unsigned int		npins;
	struct gpio_chip	*gpio_chip;
	struct irq_domain	*irq_domain;
	int			*irqs;
	unsigned int		*pm_wakeup_sources;
	struct {
		u32		imr;
		u32		odsr;
		u32		cfgr[ATMEL_PIO_NPINS_PER_BANK];
	} *pm_suspend_backup;
	struct device		*dev;
	struct device_node	*node;
	unsigned int		slew_rate_support;
};

static const char * const atmel_functions[] = {
	"GPIO", "A", "B", "C", "D", "E", "F", "G"
};

static const struct pinconf_generic_params atmel_custom_bindings[] = {
	{"atmel,drive-strength", ATMEL_PIN_CONFIG_DRIVE_STRENGTH, 0},
};

/* --- GPIO --- */
static unsigned int atmel_gpio_read(struct atmel_pioctrl *atmel_pioctrl,
				    unsigned int bank, unsigned int reg)
{
	return readl_relaxed(atmel_pioctrl->reg_base
			     + ATMEL_PIO_BANK_OFFSET * bank + reg);
}

static void atmel_gpio_write(struct atmel_pioctrl *atmel_pioctrl,
			     unsigned int bank, unsigned int reg,
			     unsigned int val)
{
	writel_relaxed(val, atmel_pioctrl->reg_base
		       + ATMEL_PIO_BANK_OFFSET * bank + reg);
}

static void atmel_gpio_irq_ack(struct irq_data *d)
{
	/*
	 * Nothing to do, interrupt is cleared when reading the status
	 * register.
	 */
}

static int atmel_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	struct atmel_pioctrl *atmel_pioctrl = irq_data_get_irq_chip_data(d);
	struct atmel_pin *pin = atmel_pioctrl->pins[d->hwirq];
	unsigned int reg;

	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_MSKR,
			 BIT(pin->line));
	reg = atmel_gpio_read(atmel_pioctrl, pin->bank, ATMEL_PIO_CFGR);
	reg &= (~ATMEL_PIO_CFGR_EVTSEL_MASK);

	switch (type) {
	case IRQ_TYPE_EDGE_RISING:
		irq_set_handler_locked(d, handle_edge_irq);
		reg |= ATMEL_PIO_CFGR_EVTSEL_RISING;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		irq_set_handler_locked(d, handle_edge_irq);
		reg |= ATMEL_PIO_CFGR_EVTSEL_FALLING;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		irq_set_handler_locked(d, handle_edge_irq);
		reg |= ATMEL_PIO_CFGR_EVTSEL_BOTH;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		irq_set_handler_locked(d, handle_level_irq);
		reg |= ATMEL_PIO_CFGR_EVTSEL_LOW;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		irq_set_handler_locked(d, handle_level_irq);
		reg |= ATMEL_PIO_CFGR_EVTSEL_HIGH;
		break;
	case IRQ_TYPE_NONE:
	default:
		return -EINVAL;
	}

	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_CFGR, reg);

	return 0;
}

static void atmel_gpio_irq_mask(struct irq_data *d)
{
	struct atmel_pioctrl *atmel_pioctrl = irq_data_get_irq_chip_data(d);
	struct atmel_pin *pin = atmel_pioctrl->pins[d->hwirq];

	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_IDR,
			 BIT(pin->line));
}

static void atmel_gpio_irq_unmask(struct irq_data *d)
{
	struct atmel_pioctrl *atmel_pioctrl = irq_data_get_irq_chip_data(d);
	struct atmel_pin *pin = atmel_pioctrl->pins[d->hwirq];

	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_IER,
			 BIT(pin->line));
}

#ifdef CONFIG_PM_SLEEP

static int atmel_gpio_irq_set_wake(struct irq_data *d, unsigned int on)
{
	struct atmel_pioctrl *atmel_pioctrl = irq_data_get_irq_chip_data(d);
	int bank = ATMEL_PIO_BANK(d->hwirq);
	int line = ATMEL_PIO_LINE(d->hwirq);

	/* The gpio controller has one interrupt line per bank. */
	irq_set_irq_wake(atmel_pioctrl->irqs[bank], on);

	if (on)
		atmel_pioctrl->pm_wakeup_sources[bank] |= BIT(line);
	else
		atmel_pioctrl->pm_wakeup_sources[bank] &= ~(BIT(line));

	return 0;
}
#else
#define atmel_gpio_irq_set_wake NULL
#endif /* CONFIG_PM_SLEEP */

static struct irq_chip atmel_gpio_irq_chip = {
	.name		= "GPIO",
	.irq_ack	= atmel_gpio_irq_ack,
	.irq_mask	= atmel_gpio_irq_mask,
	.irq_unmask	= atmel_gpio_irq_unmask,
	.irq_set_type	= atmel_gpio_irq_set_type,
	.irq_set_wake	= atmel_gpio_irq_set_wake,
};

static int atmel_gpio_to_irq(struct gpio_chip *chip, unsigned int offset)
{
	struct atmel_pioctrl *atmel_pioctrl = gpiochip_get_data(chip);

	return irq_find_mapping(atmel_pioctrl->irq_domain, offset);
}

static void atmel_gpio_irq_handler(struct irq_desc *desc)
{
	unsigned int irq = irq_desc_get_irq(desc);
	struct atmel_pioctrl *atmel_pioctrl = irq_desc_get_handler_data(desc);
	struct irq_chip *chip = irq_desc_get_chip(desc);
	unsigned long isr;
	int n, bank = -1;

	/* Find from which bank is the irq received. */
	for (n = 0; n < atmel_pioctrl->nbanks; n++) {
		if (atmel_pioctrl->irqs[n] == irq) {
			bank = n;
			break;
		}
	}

	if (bank < 0) {
		dev_err(atmel_pioctrl->dev,
			"no bank associated to irq %u\n", irq);
		return;
	}

	chained_irq_enter(chip, desc);

	for (;;) {
		isr = (unsigned long)atmel_gpio_read(atmel_pioctrl, bank,
						     ATMEL_PIO_ISR);
		isr &= (unsigned long)atmel_gpio_read(atmel_pioctrl, bank,
						      ATMEL_PIO_IMR);
		if (!isr)
			break;

		for_each_set_bit(n, &isr, BITS_PER_LONG)
			generic_handle_irq(atmel_gpio_to_irq(
					atmel_pioctrl->gpio_chip,
					bank * ATMEL_PIO_NPINS_PER_BANK + n));
	}

	chained_irq_exit(chip, desc);
}

static int atmel_gpio_direction_input(struct gpio_chip *chip,
				      unsigned int offset)
{
	struct atmel_pioctrl *atmel_pioctrl = gpiochip_get_data(chip);
	struct atmel_pin *pin = atmel_pioctrl->pins[offset];
	unsigned int reg;

	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_MSKR,
			 BIT(pin->line));
	reg = atmel_gpio_read(atmel_pioctrl, pin->bank, ATMEL_PIO_CFGR);
	reg &= ~ATMEL_PIO_DIR_MASK;
	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_CFGR, reg);

	return 0;
}

static int atmel_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct atmel_pioctrl *atmel_pioctrl = gpiochip_get_data(chip);
	struct atmel_pin *pin = atmel_pioctrl->pins[offset];
	unsigned int reg;

	reg = atmel_gpio_read(atmel_pioctrl, pin->bank, ATMEL_PIO_PDSR);

	return !!(reg & BIT(pin->line));
}

static int atmel_gpio_get_multiple(struct gpio_chip *chip, unsigned long *mask,
				   unsigned long *bits)
{
	struct atmel_pioctrl *atmel_pioctrl = gpiochip_get_data(chip);
	unsigned int bank;

	bitmap_zero(bits, atmel_pioctrl->npins);

	for (bank = 0; bank < atmel_pioctrl->nbanks; bank++) {
		unsigned int word = bank;
		unsigned int offset = 0;
		unsigned int reg;

#if ATMEL_PIO_NPINS_PER_BANK != BITS_PER_LONG
		word = BIT_WORD(bank * ATMEL_PIO_NPINS_PER_BANK);
		offset = bank * ATMEL_PIO_NPINS_PER_BANK % BITS_PER_LONG;
#endif
		if (!mask[word])
			continue;

		reg = atmel_gpio_read(atmel_pioctrl, bank, ATMEL_PIO_PDSR);
		bits[word] |= mask[word] & (reg << offset);
	}

	return 0;
}

static int atmel_gpio_direction_output(struct gpio_chip *chip,
				       unsigned int offset,
				       int value)
{
	struct atmel_pioctrl *atmel_pioctrl = gpiochip_get_data(chip);
	struct atmel_pin *pin = atmel_pioctrl->pins[offset];
	unsigned int reg;

	atmel_gpio_write(atmel_pioctrl, pin->bank,
			 value ? ATMEL_PIO_SODR : ATMEL_PIO_CODR,
			 BIT(pin->line));

	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_MSKR,
			 BIT(pin->line));
	reg = atmel_gpio_read(atmel_pioctrl, pin->bank, ATMEL_PIO_CFGR);
	reg |= ATMEL_PIO_DIR_MASK;
	atmel_gpio_write(atmel_pioctrl, pin->bank, ATMEL_PIO_CFGR, reg);

	return 0;
}

static void atmel_gpio_set(struct gpio_chip *chip, unsigned int offset, int val)
{
	struct atmel_pioctrl *atmel_pioctrl = gpiochip_get_data(chip);
	struct atmel_pin *pin = atmel_pioctrl->pins[offset];

	atmel_gpio_write(atmel_pioctrl, pin->bank,
			 val ? ATMEL_PIO_SODR : ATMEL_PIO_CODR,
			 BIT(pin->line));
}

static void atmel_gpio_set_multiple(struct gpio_chip *chip, unsigned long *mask,
				    unsigned long *bits)
{
	struct atmel_pioctrl *atmel_pioctrl = gpiochip_get_data(chip);
	unsigned int bank;

	for (bank = 0; bank < atmel_pioctrl->nbanks; bank++) {
		unsigned int bitmask;
		unsigned int word = bank;

/*
 * On a 64-bit platform, BITS_PER_LONG is 64 so it is necessary to iterate over
 * two 32bit words to handle the whole  bitmask
 */
#if ATMEL_PIO_NPINS_PER_BANK != BITS_PER_LONG
		word = BIT_WORD(bank * ATMEL_PIO_NPINS_PER_BANK);
#endif
		if (!mask[word])
			continue;

		bitmask = mask[word] & bits[word];
		atmel_gpio_write(atmel_pioctrl, bank, ATMEL_PIO_SODR, bitmask);

		bitmask = mask[word] & ~bits[word];
		atmel_gpio_write(atmel_pioctrl, bank, ATMEL_PIO_CODR, bitmask);

#if ATMEL_PIO_NPINS_PER_BANK != BITS_PER_LONG
		mask[word] >>= ATMEL_PIO_NPINS_PER_BANK;
		bits[word] >>= ATMEL_PIO_NPINS_PER_BANK;
#endif
	}
}

static struct gpio_chip atmel_gpio_chip = {
	.direction_input        = atmel_gpio_direction_input,
	.get                    = atmel_gpio_get,
	.get_multiple           = atmel_gpio_get_multiple,
	.direction_output       = atmel_gpio_direction_output,
	.set                    = atmel_gpio_set,
	.set_multiple           = atmel_gpio_set_multiple,
	.to_irq                 = atmel_gpio_to_irq,
	.base                   = 0,
};

/* --- PINCTRL --- */
static unsigned int atmel_pin_config_read(struct pinctrl_dev *pctldev,
					  unsigned int pin_id)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int bank = atmel_pioctrl->pins[pin_id]->bank;
	unsigned int line = atmel_pioctrl->pins[pin_id]->line;
	void __iomem *addr = atmel_pioctrl->reg_base
			     + bank * ATMEL_PIO_BANK_OFFSET;

	writel_relaxed(BIT(line), addr + ATMEL_PIO_MSKR);
	/* Have to set MSKR first, to access the right pin CFGR. */
	wmb();

	return readl_relaxed(addr + ATMEL_PIO_CFGR);
}

static void atmel_pin_config_write(struct pinctrl_dev *pctldev,
				   unsigned int pin_id, u32 conf)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int bank = atmel_pioctrl->pins[pin_id]->bank;
	unsigned int line = atmel_pioctrl->pins[pin_id]->line;
	void __iomem *addr = atmel_pioctrl->reg_base
			     + bank * ATMEL_PIO_BANK_OFFSET;

	writel_relaxed(BIT(line), addr + ATMEL_PIO_MSKR);
	/* Have to set MSKR first, to access the right pin CFGR. */
	wmb();
	writel_relaxed(conf, addr + ATMEL_PIO_CFGR);
}

static int atmel_pctl_get_groups_count(struct pinctrl_dev *pctldev)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);

	return atmel_pioctrl->npins;
}

static const char *atmel_pctl_get_group_name(struct pinctrl_dev *pctldev,
					     unsigned int selector)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);

	return atmel_pioctrl->groups[selector].name;
}

static int atmel_pctl_get_group_pins(struct pinctrl_dev *pctldev,
				     unsigned int selector,
				     const unsigned int **pins,
				     unsigned int *num_pins)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);

	*pins = (unsigned int *)&atmel_pioctrl->groups[selector].pin;
	*num_pins = 1;

	return 0;
}

static struct atmel_group *
atmel_pctl_find_group_by_pin(struct pinctrl_dev *pctldev, unsigned int pin)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	int i;

	for (i = 0; i < atmel_pioctrl->npins; i++) {
		struct atmel_group *grp = atmel_pioctrl->groups + i;

		if (grp->pin == pin)
			return grp;
	}

	return NULL;
}

static int atmel_pctl_xlate_pinfunc(struct pinctrl_dev *pctldev,
				    struct device_node *np,
				    u32 pinfunc, const char **grp_name,
				    const char **func_name)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int pin_id, func_id;
	struct atmel_group *grp;

	pin_id = ATMEL_GET_PIN_NO(pinfunc);
	func_id = ATMEL_GET_PIN_FUNC(pinfunc);

	if (func_id >= ARRAY_SIZE(atmel_functions))
		return -EINVAL;

	*func_name = atmel_functions[func_id];

	grp = atmel_pctl_find_group_by_pin(pctldev, pin_id);
	if (!grp)
		return -EINVAL;
	*grp_name = grp->name;

	atmel_pioctrl->pins[pin_id]->mux = func_id;
	atmel_pioctrl->pins[pin_id]->ioset = ATMEL_GET_PIN_IOSET(pinfunc);
	/* Want the device name not the group one. */
	if (np->parent == atmel_pioctrl->node)
		atmel_pioctrl->pins[pin_id]->device = np->name;
	else
		atmel_pioctrl->pins[pin_id]->device = np->parent->name;

	return 0;
}

static int atmel_pctl_dt_subnode_to_map(struct pinctrl_dev *pctldev,
					struct device_node *np,
					struct pinctrl_map **map,
					unsigned int *reserved_maps,
					unsigned int *num_maps)
{
	unsigned int num_pins, num_configs, reserve;
	unsigned long *configs;
	struct property	*pins;
	u32 pinfunc;
	int ret, i;

	pins = of_find_property(np, "pinmux", NULL);
	if (!pins)
		return -EINVAL;

	ret = pinconf_generic_parse_dt_config(np, pctldev, &configs,
					      &num_configs);
	if (ret < 0) {
		dev_err(pctldev->dev, "%pOF: could not parse node property\n",
			np);
		return ret;
	}

	num_pins = pins->length / sizeof(u32);
	if (!num_pins) {
		dev_err(pctldev->dev, "no pins found in node %pOF\n", np);
		ret = -EINVAL;
		goto exit;
	}

	/*
	 * Reserve maps, at least there is a mux map and an optional conf
	 * map for each pin.
	 */
	reserve = 1;
	if (num_configs)
		reserve++;
	reserve *= num_pins;
	ret = pinctrl_utils_reserve_map(pctldev, map, reserved_maps, num_maps,
					reserve);
	if (ret < 0)
		goto exit;

	for (i = 0; i < num_pins; i++) {
		const char *group, *func;

		ret = of_property_read_u32_index(np, "pinmux", i, &pinfunc);
		if (ret)
			goto exit;

		ret = atmel_pctl_xlate_pinfunc(pctldev, np, pinfunc, &group,
					       &func);
		if (ret)
			goto exit;

		pinctrl_utils_add_map_mux(pctldev, map, reserved_maps, num_maps,
					  group, func);

		if (num_configs) {
			ret = pinctrl_utils_add_map_configs(pctldev, map,
					reserved_maps, num_maps, group,
					configs, num_configs,
					PIN_MAP_TYPE_CONFIGS_GROUP);
			if (ret < 0)
				goto exit;
		}
	}

exit:
	kfree(configs);
	return ret;
}

static int atmel_pctl_dt_node_to_map(struct pinctrl_dev *pctldev,
				     struct device_node *np_config,
				     struct pinctrl_map **map,
				     unsigned int *num_maps)
{
	struct device_node *np;
	unsigned int reserved_maps;
	int ret;

	*map = NULL;
	*num_maps = 0;
	reserved_maps = 0;

	/*
	 * If all the pins of a device have the same configuration (or no one),
	 * it is useless to add a subnode, so directly parse node referenced by
	 * phandle.
	 */
	ret = atmel_pctl_dt_subnode_to_map(pctldev, np_config, map,
					   &reserved_maps, num_maps);
	if (ret) {
		for_each_child_of_node(np_config, np) {
			ret = atmel_pctl_dt_subnode_to_map(pctldev, np, map,
						    &reserved_maps, num_maps);
			if (ret < 0) {
				of_node_put(np);
				break;
			}
		}
	}

	if (ret < 0) {
		pinctrl_utils_free_map(pctldev, *map, *num_maps);
		dev_err(pctldev->dev, "can't create maps for node %pOF\n",
			np_config);
	}

	return ret;
}

static const struct pinctrl_ops atmel_pctlops = {
	.get_groups_count	= atmel_pctl_get_groups_count,
	.get_group_name		= atmel_pctl_get_group_name,
	.get_group_pins		= atmel_pctl_get_group_pins,
	.dt_node_to_map		= atmel_pctl_dt_node_to_map,
	.dt_free_map		= pinctrl_utils_free_map,
};

static int atmel_pmx_get_functions_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(atmel_functions);
}

static const char *atmel_pmx_get_function_name(struct pinctrl_dev *pctldev,
					       unsigned int selector)
{
	return atmel_functions[selector];
}

static int atmel_pmx_get_function_groups(struct pinctrl_dev *pctldev,
					 unsigned int selector,
					 const char * const **groups,
					 unsigned * const num_groups)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);

	*groups = atmel_pioctrl->group_names;
	*num_groups = atmel_pioctrl->npins;

	return 0;
}

static int atmel_pmx_set_mux(struct pinctrl_dev *pctldev,
			     unsigned int function,
			     unsigned int group)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int pin;
	u32 conf;

	dev_dbg(pctldev->dev, "enable function %s group %s\n",
		atmel_functions[function], atmel_pioctrl->groups[group].name);

	pin = atmel_pioctrl->groups[group].pin;
	conf = atmel_pin_config_read(pctldev, pin);
	conf &= (~ATMEL_PIO_CFGR_FUNC_MASK);
	conf |= (function & ATMEL_PIO_CFGR_FUNC_MASK);
	dev_dbg(pctldev->dev, "pin: %u, conf: 0x%08x\n", pin, conf);
	atmel_pin_config_write(pctldev, pin, conf);

	return 0;
}

static const struct pinmux_ops atmel_pmxops = {
	.get_functions_count	= atmel_pmx_get_functions_count,
	.get_function_name	= atmel_pmx_get_function_name,
	.get_function_groups	= atmel_pmx_get_function_groups,
	.set_mux		= atmel_pmx_set_mux,
};

static int atmel_conf_pin_config_group_get(struct pinctrl_dev *pctldev,
					   unsigned int group,
					   unsigned long *config)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int param = pinconf_to_config_param(*config), arg = 0;
	struct atmel_group *grp = atmel_pioctrl->groups + group;
	unsigned int pin_id = grp->pin;
	u32 res;

	res = atmel_pin_config_read(pctldev, pin_id);

	switch (param) {
	case PIN_CONFIG_BIAS_PULL_UP:
		if (!(res & ATMEL_PIO_PUEN_MASK))
			return -EINVAL;
		arg = 1;
		break;
	case PIN_CONFIG_BIAS_PULL_DOWN:
		if ((res & ATMEL_PIO_PUEN_MASK) ||
		    (!(res & ATMEL_PIO_PDEN_MASK)))
			return -EINVAL;
		arg = 1;
		break;
	case PIN_CONFIG_BIAS_DISABLE:
		if ((res & ATMEL_PIO_PUEN_MASK) ||
		    ((res & ATMEL_PIO_PDEN_MASK)))
			return -EINVAL;
		arg = 1;
		break;
	case PIN_CONFIG_DRIVE_OPEN_DRAIN:
		if (!(res & ATMEL_PIO_OPD_MASK))
			return -EINVAL;
		arg = 1;
		break;
	case PIN_CONFIG_INPUT_SCHMITT_ENABLE:
		if (!(res & ATMEL_PIO_SCHMITT_MASK))
			return -EINVAL;
		arg = 1;
		break;
	case PIN_CONFIG_SLEW_RATE:
		if (!atmel_pioctrl->slew_rate_support)
			return -EOPNOTSUPP;
		if (!(res & ATMEL_PIO_SR_MASK))
			return -EINVAL;
		arg = 1;
		break;
	case ATMEL_PIN_CONFIG_DRIVE_STRENGTH:
		if (!(res & ATMEL_PIO_DRVSTR_MASK))
			return -EINVAL;
		arg = (res & ATMEL_PIO_DRVSTR_MASK) >> ATMEL_PIO_DRVSTR_OFFSET;
		break;
	default:
		return -ENOTSUPP;
	}

	*config = pinconf_to_config_packed(param, arg);
	return 0;
}

static int atmel_conf_pin_config_group_set(struct pinctrl_dev *pctldev,
					   unsigned int group,
					   unsigned long *configs,
					   unsigned int num_configs)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	struct atmel_group *grp = atmel_pioctrl->groups + group;
	unsigned int bank, pin, pin_id = grp->pin;
	u32 mask, conf = 0;
	int i;

	conf = atmel_pin_config_read(pctldev, pin_id);

	/* Keep slew rate enabled by default. */
	if (atmel_pioctrl->slew_rate_support)
		conf |= ATMEL_PIO_SR_MASK;

	for (i = 0; i < num_configs; i++) {
		unsigned int param = pinconf_to_config_param(configs[i]);
		unsigned int arg = pinconf_to_config_argument(configs[i]);

		dev_dbg(pctldev->dev, "%s: pin=%u, config=0x%lx\n",
			__func__, pin_id, configs[i]);

		switch (param) {
		case PIN_CONFIG_BIAS_DISABLE:
			conf &= (~ATMEL_PIO_PUEN_MASK);
			conf &= (~ATMEL_PIO_PDEN_MASK);
			break;
		case PIN_CONFIG_BIAS_PULL_UP:
			conf |= ATMEL_PIO_PUEN_MASK;
			conf &= (~ATMEL_PIO_PDEN_MASK);
			break;
		case PIN_CONFIG_BIAS_PULL_DOWN:
			conf |= ATMEL_PIO_PDEN_MASK;
			conf &= (~ATMEL_PIO_PUEN_MASK);
			break;
		case PIN_CONFIG_DRIVE_OPEN_DRAIN:
			if (arg == 0)
				conf &= (~ATMEL_PIO_OPD_MASK);
			else
				conf |= ATMEL_PIO_OPD_MASK;
			break;
		case PIN_CONFIG_INPUT_SCHMITT_ENABLE:
			if (arg == 0)
				conf |= ATMEL_PIO_SCHMITT_MASK;
			else
				conf &= (~ATMEL_PIO_SCHMITT_MASK);
			break;
		case PIN_CONFIG_INPUT_DEBOUNCE:
			if (arg == 0) {
				conf &= (~ATMEL_PIO_IFEN_MASK);
				conf &= (~ATMEL_PIO_IFSCEN_MASK);
			} else {
				/*
				 * We don't care about the debounce value for several reasons:
				 * - can't have different debounce periods inside a same group,
				 * - the register to configure this period is a secure register.
				 * The debouncing filter can filter a pulse with a duration of less
				 * than 1/2 slow clock period.
				 */
				conf |= ATMEL_PIO_IFEN_MASK;
				conf |= ATMEL_PIO_IFSCEN_MASK;
			}
			break;
		case PIN_CONFIG_OUTPUT:
			conf |= ATMEL_PIO_DIR_MASK;
			bank = ATMEL_PIO_BANK(pin_id);
			pin = ATMEL_PIO_LINE(pin_id);
			mask = 1 << pin;

			if (arg == 0) {
				writel_relaxed(mask, atmel_pioctrl->reg_base +
					bank * ATMEL_PIO_BANK_OFFSET +
					ATMEL_PIO_CODR);
			} else {
				writel_relaxed(mask, atmel_pioctrl->reg_base +
					bank * ATMEL_PIO_BANK_OFFSET +
					ATMEL_PIO_SODR);
			}
			break;
		case PIN_CONFIG_SLEW_RATE:
			if (!atmel_pioctrl->slew_rate_support)
				break;
			/* And remove it if explicitly requested. */
			if (arg == 0)
				conf &= ~ATMEL_PIO_SR_MASK;
			break;
		case ATMEL_PIN_CONFIG_DRIVE_STRENGTH:
			switch (arg) {
			case ATMEL_PIO_DRVSTR_LO:
			case ATMEL_PIO_DRVSTR_ME:
			case ATMEL_PIO_DRVSTR_HI:
				conf &= (~ATMEL_PIO_DRVSTR_MASK);
				conf |= arg << ATMEL_PIO_DRVSTR_OFFSET;
				break;
			default:
				dev_warn(pctldev->dev, "drive strength not updated (incorrect value)\n");
			}
			break;
		default:
			dev_warn(pctldev->dev,
				 "unsupported configuration parameter: %u\n",
				 param);
			continue;
		}
	}

	dev_dbg(pctldev->dev, "%s: reg=0x%08x\n", __func__, conf);
	atmel_pin_config_write(pctldev, pin_id, conf);

	return 0;
}

static void atmel_conf_pin_config_dbg_show(struct pinctrl_dev *pctldev,
					   struct seq_file *s,
					   unsigned int pin_id)
{
	struct atmel_pioctrl *atmel_pioctrl = pinctrl_dev_get_drvdata(pctldev);
	u32 conf;

	if (!atmel_pioctrl->pins[pin_id]->device)
		return;

	if (atmel_pioctrl->pins[pin_id])
		seq_printf(s, " (%s, ioset %u) ",
			   atmel_pioctrl->pins[pin_id]->device,
			   atmel_pioctrl->pins[pin_id]->ioset);

	conf = atmel_pin_config_read(pctldev, pin_id);
	if (conf & ATMEL_PIO_PUEN_MASK)
		seq_printf(s, "%s ", "pull-up");
	if (conf & ATMEL_PIO_PDEN_MASK)
		seq_printf(s, "%s ", "pull-down");
	if (conf & ATMEL_PIO_IFEN_MASK)
		seq_printf(s, "%s ", "debounce");
	if (conf & ATMEL_PIO_OPD_MASK)
		seq_printf(s, "%s ", "open-drain");
	if (conf & ATMEL_PIO_SCHMITT_MASK)
		seq_printf(s, "%s ", "schmitt");
	if (atmel_pioctrl->slew_rate_support && (conf & ATMEL_PIO_SR_MASK))
		seq_printf(s, "%s ", "slew-rate");
	if (conf & ATMEL_PIO_DRVSTR_MASK) {
		switch ((conf & ATMEL_PIO_DRVSTR_MASK) >> ATMEL_PIO_DRVSTR_OFFSET) {
		case ATMEL_PIO_DRVSTR_ME:
			seq_printf(s, "%s ", "medium-drive");
			break;
		case ATMEL_PIO_DRVSTR_HI:
			seq_printf(s, "%s ", "high-drive");
			break;
		/* ATMEL_PIO_DRVSTR_LO and 0 which is the default value at reset */
		default:
			seq_printf(s, "%s ", "low-drive");
		}
	}
}

static const struct pinconf_ops atmel_confops = {
	.pin_config_group_get	= atmel_conf_pin_config_group_get,
	.pin_config_group_set	= atmel_conf_pin_config_group_set,
	.pin_config_dbg_show	= atmel_conf_pin_config_dbg_show,
};

static struct pinctrl_desc atmel_pinctrl_desc = {
	.name		= "atmel_pinctrl",
	.confops	= &atmel_confops,
	.pctlops	= &atmel_pctlops,
	.pmxops		= &atmel_pmxops,
};

static int __maybe_unused atmel_pctrl_suspend(struct device *dev)
{
	struct atmel_pioctrl *atmel_pioctrl = dev_get_drvdata(dev);
	int i, j;

	/*
	 * For each bank, save IMR to restore it later and disable all GPIO
	 * interrupts excepting the ones marked as wakeup sources.
	 */
	for (i = 0; i < atmel_pioctrl->nbanks; i++) {
		atmel_pioctrl->pm_suspend_backup[i].imr =
			atmel_gpio_read(atmel_pioctrl, i, ATMEL_PIO_IMR);
		atmel_gpio_write(atmel_pioctrl, i, ATMEL_PIO_IDR,
				 ~atmel_pioctrl->pm_wakeup_sources[i]);
		atmel_pioctrl->pm_suspend_backup[i].odsr =
			atmel_gpio_read(atmel_pioctrl, i, ATMEL_PIO_ODSR);
		for (j = 0; j < ATMEL_PIO_NPINS_PER_BANK; j++) {
			atmel_gpio_write(atmel_pioctrl, i,
					 ATMEL_PIO_MSKR, BIT(j));
			atmel_pioctrl->pm_suspend_backup[i].cfgr[j] =
				atmel_gpio_read(atmel_pioctrl, i,
						ATMEL_PIO_CFGR);
		}
	}

	return 0;
}

static int __maybe_unused atmel_pctrl_resume(struct device *dev)
{
	struct atmel_pioctrl *atmel_pioctrl = dev_get_drvdata(dev);
	int i, j;

	for (i = 0; i < atmel_pioctrl->nbanks; i++) {
		atmel_gpio_write(atmel_pioctrl, i, ATMEL_PIO_IER,
				 atmel_pioctrl->pm_suspend_backup[i].imr);
		atmel_gpio_write(atmel_pioctrl, i, ATMEL_PIO_SODR,
				 atmel_pioctrl->pm_suspend_backup[i].odsr);
		for (j = 0; j < ATMEL_PIO_NPINS_PER_BANK; j++) {
			atmel_gpio_write(atmel_pioctrl, i,
					 ATMEL_PIO_MSKR, BIT(j));
			atmel_gpio_write(atmel_pioctrl, i, ATMEL_PIO_CFGR,
					 atmel_pioctrl->pm_suspend_backup[i].cfgr[j]);
		}
	}

	return 0;
}

static const struct dev_pm_ops atmel_pctrl_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(atmel_pctrl_suspend, atmel_pctrl_resume)
};

/*
 * The number of banks can be different from a SoC to another one.
 * We can have up to 16 banks.
 */
static const struct atmel_pioctrl_data atmel_sama5d2_pioctrl_data = {
	.nbanks			= 4,
	.last_bank_count	= ATMEL_PIO_NPINS_PER_BANK,
};

static const struct atmel_pioctrl_data microchip_sama7g5_pioctrl_data = {
	.nbanks			= 5,
	.last_bank_count	= 8, /* sama7g5 has only PE0 to PE7 */
	.slew_rate_support	= 1,
};

static const struct of_device_id atmel_pctrl_of_match[] = {
	{
		.compatible = "atmel,sama5d2-pinctrl",
		.data = &atmel_sama5d2_pioctrl_data,
	}, {
		.compatible = "microchip,sama7g5-pinctrl",
		.data = &microchip_sama7g5_pioctrl_data,
	}, {
		/* sentinel */
	}
};

static int atmel_pinctrl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct pinctrl_pin_desc	*pin_desc;
	const char **group_names;
	const struct of_device_id *match;
	int i, ret;
	struct resource	*res;
	struct atmel_pioctrl *atmel_pioctrl;
	const struct atmel_pioctrl_data *atmel_pioctrl_data;

	atmel_pioctrl = devm_kzalloc(dev, sizeof(*atmel_pioctrl), GFP_KERNEL);
	if (!atmel_pioctrl)
		return -ENOMEM;
	atmel_pioctrl->dev = dev;
	atmel_pioctrl->node = dev->of_node;
	platform_set_drvdata(pdev, atmel_pioctrl);

	match = of_match_node(atmel_pctrl_of_match, dev->of_node);
	if (!match) {
		dev_err(dev, "unknown compatible string\n");
		return -ENODEV;
	}
	atmel_pioctrl_data = match->data;
	atmel_pioctrl->nbanks = atmel_pioctrl_data->nbanks;
	atmel_pioctrl->npins = atmel_pioctrl->nbanks * ATMEL_PIO_NPINS_PER_BANK;
	/* if last bank has limited number of pins, adjust accordingly */
	if (atmel_pioctrl_data->last_bank_count != ATMEL_PIO_NPINS_PER_BANK) {
		atmel_pioctrl->npins -= ATMEL_PIO_NPINS_PER_BANK;
		atmel_pioctrl->npins += atmel_pioctrl_data->last_bank_count;
	}
	atmel_pioctrl->slew_rate_support = atmel_pioctrl_data->slew_rate_support;

	atmel_pioctrl->reg_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(atmel_pioctrl->reg_base))
		return PTR_ERR(atmel_pioctrl->reg_base);

	atmel_pioctrl->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(atmel_pioctrl->clk)) {
		dev_err(dev, "failed to get clock\n");
		return PTR_ERR(atmel_pioctrl->clk);
	}

	atmel_pioctrl->pins = devm_kcalloc(dev,
					   atmel_pioctrl->npins,
					   sizeof(*atmel_pioctrl->pins),
					   GFP_KERNEL);
	if (!atmel_pioctrl->pins)
		return -ENOMEM;

	pin_desc = devm_kcalloc(dev, atmel_pioctrl->npins, sizeof(*pin_desc),
				GFP_KERNEL);
	if (!pin_desc)
		return -ENOMEM;
	atmel_pinctrl_desc.pins = pin_desc;
	atmel_pinctrl_desc.npins = atmel_pioctrl->npins;
	atmel_pinctrl_desc.num_custom_params = ARRAY_SIZE(atmel_custom_bindings);
	atmel_pinctrl_desc.custom_params = atmel_custom_bindings;

	/* One pin is one group since a pin can achieve all functions. */
	group_names = devm_kcalloc(dev,
				   atmel_pioctrl->npins, sizeof(*group_names),
				   GFP_KERNEL);
	if (!group_names)
		return -ENOMEM;
	atmel_pioctrl->group_names = group_names;

	atmel_pioctrl->groups = devm_kcalloc(&pdev->dev,
			atmel_pioctrl->npins, sizeof(*atmel_pioctrl->groups),
			GFP_KERNEL);
	if (!atmel_pioctrl->groups)
		return -ENOMEM;
	for (i = 0 ; i < atmel_pioctrl->npins; i++) {
		struct atmel_group *group = atmel_pioctrl->groups + i;
		unsigned int bank = ATMEL_PIO_BANK(i);
		unsigned int line = ATMEL_PIO_LINE(i);

		atmel_pioctrl->pins[i] = devm_kzalloc(dev,
				sizeof(**atmel_pioctrl->pins), GFP_KERNEL);
		if (!atmel_pioctrl->pins[i])
			return -ENOMEM;

		atmel_pioctrl->pins[i]->pin_id = i;
		atmel_pioctrl->pins[i]->bank = bank;
		atmel_pioctrl->pins[i]->line = line;

		pin_desc[i].number = i;
		/* Pin naming convention: P(bank_name)(bank_pin_number). */
		pin_desc[i].name = kasprintf(GFP_KERNEL, "P%c%d",
					     bank + 'A', line);

		group->name = group_names[i] = pin_desc[i].name;
		group->pin = pin_desc[i].number;

		dev_dbg(dev, "pin_id=%u, bank=%u, line=%u", i, bank, line);
	}

	atmel_pioctrl->gpio_chip = &atmel_gpio_chip;
	atmel_pioctrl->gpio_chip->of_node = dev->of_node;
	atmel_pioctrl->gpio_chip->ngpio = atmel_pioctrl->npins;
	atmel_pioctrl->gpio_chip->label = dev_name(dev);
	atmel_pioctrl->gpio_chip->parent = dev;
	atmel_pioctrl->gpio_chip->names = atmel_pioctrl->group_names;

	atmel_pioctrl->pm_wakeup_sources = devm_kcalloc(dev,
			atmel_pioctrl->nbanks,
			sizeof(*atmel_pioctrl->pm_wakeup_sources),
			GFP_KERNEL);
	if (!atmel_pioctrl->pm_wakeup_sources)
		return -ENOMEM;

	atmel_pioctrl->pm_suspend_backup = devm_kcalloc(dev,
			atmel_pioctrl->nbanks,
			sizeof(*atmel_pioctrl->pm_suspend_backup),
			GFP_KERNEL);
	if (!atmel_pioctrl->pm_suspend_backup)
		return -ENOMEM;

	atmel_pioctrl->irqs = devm_kcalloc(dev,
					   atmel_pioctrl->nbanks,
					   sizeof(*atmel_pioctrl->irqs),
					   GFP_KERNEL);
	if (!atmel_pioctrl->irqs)
		return -ENOMEM;

	/* There is one controller but each bank has its own irq line. */
	for (i = 0; i < atmel_pioctrl->nbanks; i++) {
		res = platform_get_resource(pdev, IORESOURCE_IRQ, i);
		if (!res) {
			dev_err(dev, "missing irq resource for group %c\n",
				'A' + i);
			return -EINVAL;
		}
		atmel_pioctrl->irqs[i] = res->start;
		irq_set_chained_handler_and_data(res->start,
			atmel_gpio_irq_handler, atmel_pioctrl);
		dev_dbg(dev, "bank %i: irq=%pr\n", i, res);
	}

	atmel_pioctrl->irq_domain = irq_domain_add_linear(dev->of_node,
			atmel_pioctrl->gpio_chip->ngpio,
			&irq_domain_simple_ops, NULL);
	if (!atmel_pioctrl->irq_domain) {
		dev_err(dev, "can't add the irq domain\n");
		return -ENODEV;
	}
	atmel_pioctrl->irq_domain->name = "atmel gpio";

	for (i = 0; i < atmel_pioctrl->npins; i++) {
		int irq = irq_create_mapping(atmel_pioctrl->irq_domain, i);

		irq_set_chip_and_handler(irq, &atmel_gpio_irq_chip,
					 handle_simple_irq);
		irq_set_chip_data(irq, atmel_pioctrl);
		dev_dbg(dev,
			"atmel gpio irq domain: hwirq: %d, linux irq: %d\n",
			i, irq);
	}

	ret = clk_prepare_enable(atmel_pioctrl->clk);
	if (ret) {
		dev_err(dev, "failed to prepare and enable clock\n");
		goto clk_prepare_enable_error;
	}

	atmel_pioctrl->pinctrl_dev = devm_pinctrl_register(&pdev->dev,
							   &atmel_pinctrl_desc,
							   atmel_pioctrl);
	if (IS_ERR(atmel_pioctrl->pinctrl_dev)) {
		ret = PTR_ERR(atmel_pioctrl->pinctrl_dev);
		dev_err(dev, "pinctrl registration failed\n");
		goto clk_unprep;
	}

	ret = gpiochip_add_data(atmel_pioctrl->gpio_chip, atmel_pioctrl);
	if (ret) {
		dev_err(dev, "failed to add gpiochip\n");
		goto clk_unprep;
	}

	ret = gpiochip_add_pin_range(atmel_pioctrl->gpio_chip, dev_name(dev),
				     0, 0, atmel_pioctrl->gpio_chip->ngpio);
	if (ret) {
		dev_err(dev, "failed to add gpio pin range\n");
		goto gpiochip_add_pin_range_error;
	}

	dev_info(&pdev->dev, "atmel pinctrl initialized\n");

	return 0;

gpiochip_add_pin_range_error:
	gpiochip_remove(atmel_pioctrl->gpio_chip);

clk_unprep:
	clk_disable_unprepare(atmel_pioctrl->clk);

clk_prepare_enable_error:
	irq_domain_remove(atmel_pioctrl->irq_domain);

	return ret;
}

static struct platform_driver atmel_pinctrl_driver = {
	.driver = {
		.name = "pinctrl-at91-pio4",
		.of_match_table = atmel_pctrl_of_match,
		.pm = &atmel_pctrl_pm_ops,
		.suppress_bind_attrs = true,
	},
	.probe = atmel_pinctrl_probe,
};
builtin_platform_driver(atmel_pinctrl_driver);
