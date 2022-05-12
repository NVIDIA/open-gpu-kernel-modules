// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 Rafał Miłecki <rafal@milecki.pl>
 */

#include <linux/err.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#define FLAG_BCM4708		BIT(1)
#define FLAG_BCM4709		BIT(2)
#define FLAG_BCM53012		BIT(3)

struct ns_pinctrl {
	struct device *dev;
	unsigned int chipset_flag;
	struct pinctrl_dev *pctldev;
	struct regmap *regmap;
	u32 offset;

	struct pinctrl_desc pctldesc;
	struct ns_pinctrl_group *groups;
	unsigned int num_groups;
	struct ns_pinctrl_function *functions;
	unsigned int num_functions;
};

/*
 * Pins
 */

static const struct pinctrl_pin_desc ns_pinctrl_pins[] = {
	{ 0, "spi_clk", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 1, "spi_ss", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 2, "spi_mosi", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 3, "spi_miso", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 4, "i2c_scl", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 5, "i2c_sda", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 6, "mdc", (void *)(FLAG_BCM4709 | FLAG_BCM53012) },
	{ 7, "mdio", (void *)(FLAG_BCM4709 | FLAG_BCM53012) },
	{ 8, "pwm0", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 9, "pwm1", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 10, "pwm2", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 11, "pwm3", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 12, "uart1_rx", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 13, "uart1_tx", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 14, "uart1_cts", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 15, "uart1_rts", (void *)(FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012) },
	{ 16, "uart2_rx", (void *)(FLAG_BCM4709 | FLAG_BCM53012) },
	{ 17, "uart2_tx", (void *)(FLAG_BCM4709 | FLAG_BCM53012) },
/* TODO { ??, "xtal_out", (void *)(FLAG_BCM4709) }, */
	{ 22, "sdio_pwr", (void *)(FLAG_BCM4709 | FLAG_BCM53012) },
	{ 23, "sdio_en_1p8v", (void *)(FLAG_BCM4709 | FLAG_BCM53012) },
};

/*
 * Groups
 */

struct ns_pinctrl_group {
	const char *name;
	const unsigned int *pins;
	const unsigned int num_pins;
	unsigned int chipsets;
};

static const unsigned int spi_pins[] = { 0, 1, 2, 3 };
static const unsigned int i2c_pins[] = { 4, 5 };
static const unsigned int mdio_pins[] = { 6, 7 };
static const unsigned int pwm0_pins[] = { 8 };
static const unsigned int pwm1_pins[] = { 9 };
static const unsigned int pwm2_pins[] = { 10 };
static const unsigned int pwm3_pins[] = { 11 };
static const unsigned int uart1_pins[] = { 12, 13, 14, 15 };
static const unsigned int uart2_pins[] = { 16, 17 };
static const unsigned int sdio_pwr_pins[] = { 22 };
static const unsigned int sdio_1p8v_pins[] = { 23 };

#define NS_GROUP(_name, _pins, _chipsets)		\
{							\
	.name = _name,					\
	.pins = _pins,					\
	.num_pins = ARRAY_SIZE(_pins),			\
	.chipsets = _chipsets,				\
}

static const struct ns_pinctrl_group ns_pinctrl_groups[] = {
	NS_GROUP("spi_grp", spi_pins, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("i2c_grp", i2c_pins, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("mdio_grp", mdio_pins, FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("pwm0_grp", pwm0_pins, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("pwm1_grp", pwm1_pins, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("pwm2_grp", pwm2_pins, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("pwm3_grp", pwm3_pins, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("uart1_grp", uart1_pins, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("uart2_grp", uart2_pins, FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("sdio_pwr_grp", sdio_pwr_pins, FLAG_BCM4709 | FLAG_BCM53012),
	NS_GROUP("sdio_1p8v_grp", sdio_1p8v_pins, FLAG_BCM4709 | FLAG_BCM53012),
};

/*
 * Functions
 */

struct ns_pinctrl_function {
	const char *name;
	const char * const *groups;
	const unsigned int num_groups;
	unsigned int chipsets;
};

static const char * const spi_groups[] = { "spi_grp" };
static const char * const i2c_groups[] = { "i2c_grp" };
static const char * const mdio_groups[] = { "mdio_grp" };
static const char * const pwm_groups[] = { "pwm0_grp", "pwm1_grp", "pwm2_grp",
					   "pwm3_grp" };
static const char * const uart1_groups[] = { "uart1_grp" };
static const char * const uart2_groups[] = { "uart2_grp" };
static const char * const sdio_groups[] = { "sdio_pwr_grp", "sdio_1p8v_grp" };

#define NS_FUNCTION(_name, _groups, _chipsets)		\
{							\
	.name = _name,					\
	.groups = _groups,				\
	.num_groups = ARRAY_SIZE(_groups),		\
	.chipsets = _chipsets,				\
}

static const struct ns_pinctrl_function ns_pinctrl_functions[] = {
	NS_FUNCTION("spi", spi_groups, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_FUNCTION("i2c", i2c_groups, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_FUNCTION("mdio", mdio_groups, FLAG_BCM4709 | FLAG_BCM53012),
	NS_FUNCTION("pwm", pwm_groups, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_FUNCTION("uart1", uart1_groups, FLAG_BCM4708 | FLAG_BCM4709 | FLAG_BCM53012),
	NS_FUNCTION("uart2", uart2_groups, FLAG_BCM4709 | FLAG_BCM53012),
	NS_FUNCTION("sdio", sdio_groups, FLAG_BCM4709 | FLAG_BCM53012),
};

/*
 * Groups code
 */

static int ns_pinctrl_get_groups_count(struct pinctrl_dev *pctrl_dev)
{
	struct ns_pinctrl *ns_pinctrl = pinctrl_dev_get_drvdata(pctrl_dev);

	return ns_pinctrl->num_groups;
}

static const char *ns_pinctrl_get_group_name(struct pinctrl_dev *pctrl_dev,
					     unsigned int selector)
{
	struct ns_pinctrl *ns_pinctrl = pinctrl_dev_get_drvdata(pctrl_dev);

	return ns_pinctrl->groups[selector].name;
}

static int ns_pinctrl_get_group_pins(struct pinctrl_dev *pctrl_dev,
				     unsigned int selector,
				     const unsigned int **pins,
				     unsigned int *num_pins)
{
	struct ns_pinctrl *ns_pinctrl = pinctrl_dev_get_drvdata(pctrl_dev);

	*pins = ns_pinctrl->groups[selector].pins;
	*num_pins = ns_pinctrl->groups[selector].num_pins;

	return 0;
}

static const struct pinctrl_ops ns_pinctrl_ops = {
	.get_groups_count = ns_pinctrl_get_groups_count,
	.get_group_name = ns_pinctrl_get_group_name,
	.get_group_pins = ns_pinctrl_get_group_pins,
	.dt_node_to_map = pinconf_generic_dt_node_to_map_group,
	.dt_free_map = pinconf_generic_dt_free_map,
};

/*
 * Functions code
 */

static int ns_pinctrl_get_functions_count(struct pinctrl_dev *pctrl_dev)
{
	struct ns_pinctrl *ns_pinctrl = pinctrl_dev_get_drvdata(pctrl_dev);

	return ns_pinctrl->num_functions;
}

static const char *ns_pinctrl_get_function_name(struct pinctrl_dev *pctrl_dev,
						unsigned int selector)
{
	struct ns_pinctrl *ns_pinctrl = pinctrl_dev_get_drvdata(pctrl_dev);

	return ns_pinctrl->functions[selector].name;
}

static int ns_pinctrl_get_function_groups(struct pinctrl_dev *pctrl_dev,
					  unsigned int selector,
					  const char * const **groups,
					  unsigned * const num_groups)
{
	struct ns_pinctrl *ns_pinctrl = pinctrl_dev_get_drvdata(pctrl_dev);

	*groups = ns_pinctrl->functions[selector].groups;
	*num_groups = ns_pinctrl->functions[selector].num_groups;

	return 0;
}

static int ns_pinctrl_set_mux(struct pinctrl_dev *pctrl_dev,
			      unsigned int func_select,
			      unsigned int grp_select)
{
	struct ns_pinctrl *ns_pinctrl = pinctrl_dev_get_drvdata(pctrl_dev);
	u32 unset = 0;
	u32 tmp;
	int i;

	for (i = 0; i < ns_pinctrl->groups[grp_select].num_pins; i++) {
		int pin_number = ns_pinctrl->groups[grp_select].pins[i];

		unset |= BIT(pin_number);
	}

	regmap_read(ns_pinctrl->regmap, ns_pinctrl->offset, &tmp);
	tmp &= ~unset;
	regmap_write(ns_pinctrl->regmap, ns_pinctrl->offset, tmp);

	return 0;
}

static const struct pinmux_ops ns_pinctrl_pmxops = {
	.get_functions_count = ns_pinctrl_get_functions_count,
	.get_function_name = ns_pinctrl_get_function_name,
	.get_function_groups = ns_pinctrl_get_function_groups,
	.set_mux = ns_pinctrl_set_mux,
};

/*
 * Controller code
 */

static struct pinctrl_desc ns_pinctrl_desc = {
	.name = "pinctrl-ns",
	.pctlops = &ns_pinctrl_ops,
	.pmxops = &ns_pinctrl_pmxops,
};

static const struct of_device_id ns_pinctrl_of_match_table[] = {
	{ .compatible = "brcm,bcm4708-pinmux", .data = (void *)FLAG_BCM4708, },
	{ .compatible = "brcm,bcm4709-pinmux", .data = (void *)FLAG_BCM4709, },
	{ .compatible = "brcm,bcm53012-pinmux", .data = (void *)FLAG_BCM53012, },
	{ }
};

static int ns_pinctrl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	const struct of_device_id *of_id;
	struct ns_pinctrl *ns_pinctrl;
	struct pinctrl_desc *pctldesc;
	struct pinctrl_pin_desc *pin;
	struct ns_pinctrl_group *group;
	struct ns_pinctrl_function *function;
	int i;

	ns_pinctrl = devm_kzalloc(dev, sizeof(*ns_pinctrl), GFP_KERNEL);
	if (!ns_pinctrl)
		return -ENOMEM;
	pctldesc = &ns_pinctrl->pctldesc;
	platform_set_drvdata(pdev, ns_pinctrl);

	/* Set basic properties */

	ns_pinctrl->dev = dev;

	of_id = of_match_device(ns_pinctrl_of_match_table, dev);
	if (!of_id)
		return -EINVAL;
	ns_pinctrl->chipset_flag = (uintptr_t)of_id->data;

	ns_pinctrl->regmap = syscon_node_to_regmap(of_get_parent(np));
	if (IS_ERR(ns_pinctrl->regmap)) {
		int err = PTR_ERR(ns_pinctrl->regmap);

		dev_err(dev, "Failed to map pinctrl regs: %d\n", err);

		return err;
	}

	if (of_property_read_u32(np, "offset", &ns_pinctrl->offset)) {
		dev_err(dev, "Failed to get register offset\n");
		return -ENOENT;
	}

	memcpy(pctldesc, &ns_pinctrl_desc, sizeof(*pctldesc));

	/* Set pinctrl properties */

	pctldesc->pins = devm_kcalloc(dev, ARRAY_SIZE(ns_pinctrl_pins),
				      sizeof(struct pinctrl_pin_desc),
				      GFP_KERNEL);
	if (!pctldesc->pins)
		return -ENOMEM;
	for (i = 0, pin = (struct pinctrl_pin_desc *)&pctldesc->pins[0];
	     i < ARRAY_SIZE(ns_pinctrl_pins); i++) {
		const struct pinctrl_pin_desc *src = &ns_pinctrl_pins[i];
		unsigned int chipsets = (uintptr_t)src->drv_data;

		if (chipsets & ns_pinctrl->chipset_flag) {
			memcpy(pin++, src, sizeof(*src));
			pctldesc->npins++;
		}
	}

	ns_pinctrl->groups = devm_kcalloc(dev, ARRAY_SIZE(ns_pinctrl_groups),
					  sizeof(struct ns_pinctrl_group),
					  GFP_KERNEL);
	if (!ns_pinctrl->groups)
		return -ENOMEM;
	for (i = 0, group = &ns_pinctrl->groups[0];
	     i < ARRAY_SIZE(ns_pinctrl_groups); i++) {
		const struct ns_pinctrl_group *src = &ns_pinctrl_groups[i];

		if (src->chipsets & ns_pinctrl->chipset_flag) {
			memcpy(group++, src, sizeof(*src));
			ns_pinctrl->num_groups++;
		}
	}

	ns_pinctrl->functions = devm_kcalloc(dev,
					     ARRAY_SIZE(ns_pinctrl_functions),
					     sizeof(struct ns_pinctrl_function),
					     GFP_KERNEL);
	if (!ns_pinctrl->functions)
		return -ENOMEM;
	for (i = 0, function = &ns_pinctrl->functions[0];
	     i < ARRAY_SIZE(ns_pinctrl_functions); i++) {
		const struct ns_pinctrl_function *src = &ns_pinctrl_functions[i];

		if (src->chipsets & ns_pinctrl->chipset_flag) {
			memcpy(function++, src, sizeof(*src));
			ns_pinctrl->num_functions++;
		}
	}

	/* Register */

	ns_pinctrl->pctldev = devm_pinctrl_register(dev, pctldesc, ns_pinctrl);
	if (IS_ERR(ns_pinctrl->pctldev)) {
		dev_err(dev, "Failed to register pinctrl\n");
		return PTR_ERR(ns_pinctrl->pctldev);
	}

	return 0;
}

static struct platform_driver ns_pinctrl_driver = {
	.probe = ns_pinctrl_probe,
	.driver = {
		.name = "ns-pinmux",
		.of_match_table = ns_pinctrl_of_match_table,
	},
};

module_platform_driver(ns_pinctrl_driver);

MODULE_AUTHOR("Rafał Miłecki");
MODULE_LICENSE("GPL v2");
MODULE_DEVICE_TABLE(of, ns_pinctrl_of_match_table);
