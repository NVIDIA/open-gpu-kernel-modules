// SPDX-License-Identifier: GPL-2.0+
/*
 * Linux GPIOlib driver for the VIA VX855 integrated southbridge GPIO
 *
 * Copyright (C) 2009 VIA Technologies, Inc.
 * Copyright (C) 2010 One Laptop per Child
 * Author: Harald Welte <HaraldWelte@viatech.com>
 * All rights reserved.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio/driver.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/pci.h>
#include <linux/io.h>

#define MODULE_NAME "vx855_gpio"

/* The VX855 south bridge has the following GPIO pins:
 *	GPI 0...13	General Purpose Input
 *	GPO 0...12	General Purpose Output
 *	GPIO 0...14	General Purpose I/O (Open-Drain)
 */

#define NR_VX855_GPI	14
#define NR_VX855_GPO	13
#define NR_VX855_GPIO	15

#define NR_VX855_GPInO	(NR_VX855_GPI + NR_VX855_GPO)
#define NR_VX855_GP	(NR_VX855_GPI + NR_VX855_GPO + NR_VX855_GPIO)

struct vx855_gpio {
	struct gpio_chip gpio;
	spinlock_t lock;
	u32 io_gpi;
	u32 io_gpo;
};

/* resolve a GPIx into the corresponding bit position */
static inline u_int32_t gpi_i_bit(int i)
{
	if (i < 10)
		return 1 << i;
	else
		return 1 << (i + 14);
}

static inline u_int32_t gpo_o_bit(int i)
{
	if (i < 11)
		return 1 << i;
	else
		return 1 << (i + 14);
}

static inline u_int32_t gpio_i_bit(int i)
{
	if (i < 14)
		return 1 << (i + 10);
	else
		return 1 << (i + 14);
}

static inline u_int32_t gpio_o_bit(int i)
{
	if (i < 14)
		return 1 << (i + 11);
	else
		return 1 << (i + 13);
}

/* Mapping between numeric GPIO ID and the actual GPIO hardware numbering:
 * 0..13	GPI 0..13
 * 14..26	GPO 0..12
 * 27..41	GPIO 0..14
 */

static int vx855gpio_direction_input(struct gpio_chip *gpio,
				     unsigned int nr)
{
	struct vx855_gpio *vg = gpiochip_get_data(gpio);
	unsigned long flags;
	u_int32_t reg_out;

	/* Real GPI bits are always in input direction */
	if (nr < NR_VX855_GPI)
		return 0;

	/* Real GPO bits cannot be put in output direction */
	if (nr < NR_VX855_GPInO)
		return -EINVAL;

	/* Open Drain GPIO have to be set to one */
	spin_lock_irqsave(&vg->lock, flags);
	reg_out = inl(vg->io_gpo);
	reg_out |= gpio_o_bit(nr - NR_VX855_GPInO);
	outl(reg_out, vg->io_gpo);
	spin_unlock_irqrestore(&vg->lock, flags);

	return 0;
}

static int vx855gpio_get(struct gpio_chip *gpio, unsigned int nr)
{
	struct vx855_gpio *vg = gpiochip_get_data(gpio);
	u_int32_t reg_in;
	int ret = 0;

	if (nr < NR_VX855_GPI) {
		reg_in = inl(vg->io_gpi);
		if (reg_in & gpi_i_bit(nr))
			ret = 1;
	} else if (nr < NR_VX855_GPInO) {
		/* GPO don't have an input bit, we need to read it
		 * back from the output register */
		reg_in = inl(vg->io_gpo);
		if (reg_in & gpo_o_bit(nr - NR_VX855_GPI))
			ret = 1;
	} else {
		reg_in = inl(vg->io_gpi);
		if (reg_in & gpio_i_bit(nr - NR_VX855_GPInO))
			ret = 1;
	}

	return ret;
}

static void vx855gpio_set(struct gpio_chip *gpio, unsigned int nr,
			  int val)
{
	struct vx855_gpio *vg = gpiochip_get_data(gpio);
	unsigned long flags;
	u_int32_t reg_out;

	/* True GPI cannot be switched to output mode */
	if (nr < NR_VX855_GPI)
		return;

	spin_lock_irqsave(&vg->lock, flags);
	reg_out = inl(vg->io_gpo);
	if (nr < NR_VX855_GPInO) {
		if (val)
			reg_out |= gpo_o_bit(nr - NR_VX855_GPI);
		else
			reg_out &= ~gpo_o_bit(nr - NR_VX855_GPI);
	} else {
		if (val)
			reg_out |= gpio_o_bit(nr - NR_VX855_GPInO);
		else
			reg_out &= ~gpio_o_bit(nr - NR_VX855_GPInO);
	}
	outl(reg_out, vg->io_gpo);
	spin_unlock_irqrestore(&vg->lock, flags);
}

static int vx855gpio_direction_output(struct gpio_chip *gpio,
				      unsigned int nr, int val)
{
	/* True GPI cannot be switched to output mode */
	if (nr < NR_VX855_GPI)
		return -EINVAL;

	/* True GPO don't need to be switched to output mode,
	 * and GPIO are open-drain, i.e. also need no switching,
	 * so all we do is set the level */
	vx855gpio_set(gpio, nr, val);

	return 0;
}

static int vx855gpio_set_config(struct gpio_chip *gpio, unsigned int nr,
				unsigned long config)
{
	enum pin_config_param param = pinconf_to_config_param(config);

	/* The GPI cannot be single-ended */
	if (nr < NR_VX855_GPI)
		return -EINVAL;

	/* The GPO's are push-pull */
	if (nr < NR_VX855_GPInO) {
		if (param != PIN_CONFIG_DRIVE_PUSH_PULL)
			return -ENOTSUPP;
		return 0;
	}

	/* The GPIO's are open drain */
	if (param != PIN_CONFIG_DRIVE_OPEN_DRAIN)
		return -ENOTSUPP;

	return 0;
}

static const char *vx855gpio_names[NR_VX855_GP] = {
	"VX855_GPI0", "VX855_GPI1", "VX855_GPI2", "VX855_GPI3", "VX855_GPI4",
	"VX855_GPI5", "VX855_GPI6", "VX855_GPI7", "VX855_GPI8", "VX855_GPI9",
	"VX855_GPI10", "VX855_GPI11", "VX855_GPI12", "VX855_GPI13",
	"VX855_GPO0", "VX855_GPO1", "VX855_GPO2", "VX855_GPO3", "VX855_GPO4",
	"VX855_GPO5", "VX855_GPO6", "VX855_GPO7", "VX855_GPO8", "VX855_GPO9",
	"VX855_GPO10", "VX855_GPO11", "VX855_GPO12",
	"VX855_GPIO0", "VX855_GPIO1", "VX855_GPIO2", "VX855_GPIO3",
	"VX855_GPIO4", "VX855_GPIO5", "VX855_GPIO6", "VX855_GPIO7",
	"VX855_GPIO8", "VX855_GPIO9", "VX855_GPIO10", "VX855_GPIO11",
	"VX855_GPIO12", "VX855_GPIO13", "VX855_GPIO14"
};

static void vx855gpio_gpio_setup(struct vx855_gpio *vg)
{
	struct gpio_chip *c = &vg->gpio;

	c->label = "VX855 South Bridge";
	c->owner = THIS_MODULE;
	c->direction_input = vx855gpio_direction_input;
	c->direction_output = vx855gpio_direction_output;
	c->get = vx855gpio_get;
	c->set = vx855gpio_set;
	c->set_config = vx855gpio_set_config;
	c->dbg_show = NULL;
	c->base = 0;
	c->ngpio = NR_VX855_GP;
	c->can_sleep = false;
	c->names = vx855gpio_names;
}

/* This platform device is ordinarily registered by the vx855 mfd driver */
static int vx855gpio_probe(struct platform_device *pdev)
{
	struct resource *res_gpi;
	struct resource *res_gpo;
	struct vx855_gpio *vg;

	res_gpi = platform_get_resource(pdev, IORESOURCE_IO, 0);
	res_gpo = platform_get_resource(pdev, IORESOURCE_IO, 1);
	if (!res_gpi || !res_gpo)
		return -EBUSY;

	vg = devm_kzalloc(&pdev->dev, sizeof(*vg), GFP_KERNEL);
	if (!vg)
		return -ENOMEM;

	platform_set_drvdata(pdev, vg);

	dev_info(&pdev->dev, "found VX855 GPIO controller\n");
	vg->io_gpi = res_gpi->start;
	vg->io_gpo = res_gpo->start;
	spin_lock_init(&vg->lock);

	/*
	 * A single byte is used to control various GPIO ports on the VX855,
	 * and in the case of the OLPC XO-1.5, some of those ports are used
	 * for switches that are interpreted and exposed through ACPI. ACPI
	 * will have reserved the region, so our own reservation will not
	 * succeed. Ignore and continue.
	 */

	if (!devm_request_region(&pdev->dev, res_gpi->start,
				 resource_size(res_gpi), MODULE_NAME "_gpi"))
		dev_warn(&pdev->dev,
			"GPI I/O resource busy, probably claimed by ACPI\n");

	if (!devm_request_region(&pdev->dev, res_gpo->start,
				 resource_size(res_gpo), MODULE_NAME "_gpo"))
		dev_warn(&pdev->dev,
			"GPO I/O resource busy, probably claimed by ACPI\n");

	vx855gpio_gpio_setup(vg);

	return devm_gpiochip_add_data(&pdev->dev, &vg->gpio, vg);
}

static struct platform_driver vx855gpio_driver = {
	.driver = {
		.name	= MODULE_NAME,
	},
	.probe		= vx855gpio_probe,
};

module_platform_driver(vx855gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harald Welte <HaraldWelte@viatech.com>");
MODULE_DESCRIPTION("GPIO driver for the VIA VX855 chipset");
MODULE_ALIAS("platform:vx855_gpio");
