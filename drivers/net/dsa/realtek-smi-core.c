// SPDX-License-Identifier: GPL-2.0+
/* Realtek Simple Management Interface (SMI) driver
 * It can be discussed how "simple" this interface is.
 *
 * The SMI protocol piggy-backs the MDIO MDC and MDIO signals levels
 * but the protocol is not MDIO at all. Instead it is a Realtek
 * pecularity that need to bit-bang the lines in a special way to
 * communicate with the switch.
 *
 * ASICs we intend to support with this driver:
 *
 * RTL8366   - The original version, apparently
 * RTL8369   - Similar enough to have the same datsheet as RTL8366
 * RTL8366RB - Probably reads out "RTL8366 revision B", has a quite
 *             different register layout from the other two
 * RTL8366S  - Is this "RTL8366 super"?
 * RTL8367   - Has an OpenWRT driver as well
 * RTL8368S  - Seems to be an alternative name for RTL8366RB
 * RTL8370   - Also uses SMI
 *
 * Copyright (C) 2017 Linus Walleij <linus.walleij@linaro.org>
 * Copyright (C) 2010 Antti Seppälä <a.seppala@gmail.com>
 * Copyright (C) 2010 Roman Yeryomin <roman@advem.lv>
 * Copyright (C) 2011 Colin Leitner <colin.leitner@googlemail.com>
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_mdio.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/bitops.h>
#include <linux/if_bridge.h>

#include "realtek-smi-core.h"

#define REALTEK_SMI_ACK_RETRY_COUNT		5
#define REALTEK_SMI_HW_STOP_DELAY		25	/* msecs */
#define REALTEK_SMI_HW_START_DELAY		100	/* msecs */

static inline void realtek_smi_clk_delay(struct realtek_smi *smi)
{
	ndelay(smi->clk_delay);
}

static void realtek_smi_start(struct realtek_smi *smi)
{
	/* Set GPIO pins to output mode, with initial state:
	 * SCK = 0, SDA = 1
	 */
	gpiod_direction_output(smi->mdc, 0);
	gpiod_direction_output(smi->mdio, 1);
	realtek_smi_clk_delay(smi);

	/* CLK 1: 0 -> 1, 1 -> 0 */
	gpiod_set_value(smi->mdc, 1);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdc, 0);
	realtek_smi_clk_delay(smi);

	/* CLK 2: */
	gpiod_set_value(smi->mdc, 1);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdio, 0);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdc, 0);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdio, 1);
}

static void realtek_smi_stop(struct realtek_smi *smi)
{
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdio, 0);
	gpiod_set_value(smi->mdc, 1);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdio, 1);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdc, 1);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdc, 0);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdc, 1);

	/* Add a click */
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdc, 0);
	realtek_smi_clk_delay(smi);
	gpiod_set_value(smi->mdc, 1);

	/* Set GPIO pins to input mode */
	gpiod_direction_input(smi->mdio);
	gpiod_direction_input(smi->mdc);
}

static void realtek_smi_write_bits(struct realtek_smi *smi, u32 data, u32 len)
{
	for (; len > 0; len--) {
		realtek_smi_clk_delay(smi);

		/* Prepare data */
		gpiod_set_value(smi->mdio, !!(data & (1 << (len - 1))));
		realtek_smi_clk_delay(smi);

		/* Clocking */
		gpiod_set_value(smi->mdc, 1);
		realtek_smi_clk_delay(smi);
		gpiod_set_value(smi->mdc, 0);
	}
}

static void realtek_smi_read_bits(struct realtek_smi *smi, u32 len, u32 *data)
{
	gpiod_direction_input(smi->mdio);

	for (*data = 0; len > 0; len--) {
		u32 u;

		realtek_smi_clk_delay(smi);

		/* Clocking */
		gpiod_set_value(smi->mdc, 1);
		realtek_smi_clk_delay(smi);
		u = !!gpiod_get_value(smi->mdio);
		gpiod_set_value(smi->mdc, 0);

		*data |= (u << (len - 1));
	}

	gpiod_direction_output(smi->mdio, 0);
}

static int realtek_smi_wait_for_ack(struct realtek_smi *smi)
{
	int retry_cnt;

	retry_cnt = 0;
	do {
		u32 ack;

		realtek_smi_read_bits(smi, 1, &ack);
		if (ack == 0)
			break;

		if (++retry_cnt > REALTEK_SMI_ACK_RETRY_COUNT) {
			dev_err(smi->dev, "ACK timeout\n");
			return -ETIMEDOUT;
		}
	} while (1);

	return 0;
}

static int realtek_smi_write_byte(struct realtek_smi *smi, u8 data)
{
	realtek_smi_write_bits(smi, data, 8);
	return realtek_smi_wait_for_ack(smi);
}

static int realtek_smi_write_byte_noack(struct realtek_smi *smi, u8 data)
{
	realtek_smi_write_bits(smi, data, 8);
	return 0;
}

static int realtek_smi_read_byte0(struct realtek_smi *smi, u8 *data)
{
	u32 t;

	/* Read data */
	realtek_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* Send an ACK */
	realtek_smi_write_bits(smi, 0x00, 1);

	return 0;
}

static int realtek_smi_read_byte1(struct realtek_smi *smi, u8 *data)
{
	u32 t;

	/* Read data */
	realtek_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* Send an ACK */
	realtek_smi_write_bits(smi, 0x01, 1);

	return 0;
}

static int realtek_smi_read_reg(struct realtek_smi *smi, u32 addr, u32 *data)
{
	unsigned long flags;
	u8 lo = 0;
	u8 hi = 0;
	int ret;

	spin_lock_irqsave(&smi->lock, flags);

	realtek_smi_start(smi);

	/* Send READ command */
	ret = realtek_smi_write_byte(smi, smi->cmd_read);
	if (ret)
		goto out;

	/* Set ADDR[7:0] */
	ret = realtek_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* Set ADDR[15:8] */
	ret = realtek_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* Read DATA[7:0] */
	realtek_smi_read_byte0(smi, &lo);
	/* Read DATA[15:8] */
	realtek_smi_read_byte1(smi, &hi);

	*data = ((u32)lo) | (((u32)hi) << 8);

	ret = 0;

 out:
	realtek_smi_stop(smi);
	spin_unlock_irqrestore(&smi->lock, flags);

	return ret;
}

static int realtek_smi_write_reg(struct realtek_smi *smi,
				 u32 addr, u32 data, bool ack)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&smi->lock, flags);

	realtek_smi_start(smi);

	/* Send WRITE command */
	ret = realtek_smi_write_byte(smi, smi->cmd_write);
	if (ret)
		goto out;

	/* Set ADDR[7:0] */
	ret = realtek_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* Set ADDR[15:8] */
	ret = realtek_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* Write DATA[7:0] */
	ret = realtek_smi_write_byte(smi, data & 0xff);
	if (ret)
		goto out;

	/* Write DATA[15:8] */
	if (ack)
		ret = realtek_smi_write_byte(smi, data >> 8);
	else
		ret = realtek_smi_write_byte_noack(smi, data >> 8);
	if (ret)
		goto out;

	ret = 0;

 out:
	realtek_smi_stop(smi);
	spin_unlock_irqrestore(&smi->lock, flags);

	return ret;
}

/* There is one single case when we need to use this accessor and that
 * is when issueing soft reset. Since the device reset as soon as we write
 * that bit, no ACK will come back for natural reasons.
 */
int realtek_smi_write_reg_noack(struct realtek_smi *smi, u32 addr,
				u32 data)
{
	return realtek_smi_write_reg(smi, addr, data, false);
}
EXPORT_SYMBOL_GPL(realtek_smi_write_reg_noack);

/* Regmap accessors */

static int realtek_smi_write(void *ctx, u32 reg, u32 val)
{
	struct realtek_smi *smi = ctx;

	return realtek_smi_write_reg(smi, reg, val, true);
}

static int realtek_smi_read(void *ctx, u32 reg, u32 *val)
{
	struct realtek_smi *smi = ctx;

	return realtek_smi_read_reg(smi, reg, val);
}

static const struct regmap_config realtek_smi_mdio_regmap_config = {
	.reg_bits = 10, /* A4..A0 R4..R0 */
	.val_bits = 16,
	.reg_stride = 1,
	/* PHY regs are at 0x8000 */
	.max_register = 0xffff,
	.reg_format_endian = REGMAP_ENDIAN_BIG,
	.reg_read = realtek_smi_read,
	.reg_write = realtek_smi_write,
	.cache_type = REGCACHE_NONE,
};

static int realtek_smi_mdio_read(struct mii_bus *bus, int addr, int regnum)
{
	struct realtek_smi *smi = bus->priv;

	return smi->ops->phy_read(smi, addr, regnum);
}

static int realtek_smi_mdio_write(struct mii_bus *bus, int addr, int regnum,
				  u16 val)
{
	struct realtek_smi *smi = bus->priv;

	return smi->ops->phy_write(smi, addr, regnum, val);
}

int realtek_smi_setup_mdio(struct realtek_smi *smi)
{
	struct device_node *mdio_np;
	int ret;

	mdio_np = of_get_compatible_child(smi->dev->of_node, "realtek,smi-mdio");
	if (!mdio_np) {
		dev_err(smi->dev, "no MDIO bus node\n");
		return -ENODEV;
	}

	smi->slave_mii_bus = devm_mdiobus_alloc(smi->dev);
	if (!smi->slave_mii_bus) {
		ret = -ENOMEM;
		goto err_put_node;
	}
	smi->slave_mii_bus->priv = smi;
	smi->slave_mii_bus->name = "SMI slave MII";
	smi->slave_mii_bus->read = realtek_smi_mdio_read;
	smi->slave_mii_bus->write = realtek_smi_mdio_write;
	snprintf(smi->slave_mii_bus->id, MII_BUS_ID_SIZE, "SMI-%d",
		 smi->ds->index);
	smi->slave_mii_bus->dev.of_node = mdio_np;
	smi->slave_mii_bus->parent = smi->dev;
	smi->ds->slave_mii_bus = smi->slave_mii_bus;

	ret = of_mdiobus_register(smi->slave_mii_bus, mdio_np);
	if (ret) {
		dev_err(smi->dev, "unable to register MDIO bus %s\n",
			smi->slave_mii_bus->id);
		goto err_put_node;
	}

	return 0;

err_put_node:
	of_node_put(mdio_np);

	return ret;
}

static int realtek_smi_probe(struct platform_device *pdev)
{
	const struct realtek_smi_variant *var;
	struct device *dev = &pdev->dev;
	struct realtek_smi *smi;
	struct device_node *np;
	int ret;

	var = of_device_get_match_data(dev);
	np = dev->of_node;

	smi = devm_kzalloc(dev, sizeof(*smi) + var->chip_data_sz, GFP_KERNEL);
	if (!smi)
		return -ENOMEM;
	smi->chip_data = (void *)smi + sizeof(*smi);
	smi->map = devm_regmap_init(dev, NULL, smi,
				    &realtek_smi_mdio_regmap_config);
	if (IS_ERR(smi->map)) {
		ret = PTR_ERR(smi->map);
		dev_err(dev, "regmap init failed: %d\n", ret);
		return ret;
	}

	/* Link forward and backward */
	smi->dev = dev;
	smi->clk_delay = var->clk_delay;
	smi->cmd_read = var->cmd_read;
	smi->cmd_write = var->cmd_write;
	smi->ops = var->ops;

	dev_set_drvdata(dev, smi);
	spin_lock_init(&smi->lock);

	/* TODO: if power is software controlled, set up any regulators here */

	/* Assert then deassert RESET */
	smi->reset = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(smi->reset)) {
		dev_err(dev, "failed to get RESET GPIO\n");
		return PTR_ERR(smi->reset);
	}
	msleep(REALTEK_SMI_HW_STOP_DELAY);
	gpiod_set_value(smi->reset, 0);
	msleep(REALTEK_SMI_HW_START_DELAY);
	dev_info(dev, "deasserted RESET\n");

	/* Fetch MDIO pins */
	smi->mdc = devm_gpiod_get_optional(dev, "mdc", GPIOD_OUT_LOW);
	if (IS_ERR(smi->mdc))
		return PTR_ERR(smi->mdc);
	smi->mdio = devm_gpiod_get_optional(dev, "mdio", GPIOD_OUT_LOW);
	if (IS_ERR(smi->mdio))
		return PTR_ERR(smi->mdio);

	smi->leds_disabled = of_property_read_bool(np, "realtek,disable-leds");

	ret = smi->ops->detect(smi);
	if (ret) {
		dev_err(dev, "unable to detect switch\n");
		return ret;
	}

	smi->ds = devm_kzalloc(dev, sizeof(*smi->ds), GFP_KERNEL);
	if (!smi->ds)
		return -ENOMEM;

	smi->ds->dev = dev;
	smi->ds->num_ports = smi->num_ports;
	smi->ds->priv = smi;

	smi->ds->ops = var->ds_ops;
	ret = dsa_register_switch(smi->ds);
	if (ret) {
		dev_err(dev, "unable to register switch ret = %d\n", ret);
		return ret;
	}
	return 0;
}

static int realtek_smi_remove(struct platform_device *pdev)
{
	struct realtek_smi *smi = dev_get_drvdata(&pdev->dev);

	dsa_unregister_switch(smi->ds);
	if (smi->slave_mii_bus)
		of_node_put(smi->slave_mii_bus->dev.of_node);
	gpiod_set_value(smi->reset, 1);

	return 0;
}

static const struct of_device_id realtek_smi_of_match[] = {
	{
		.compatible = "realtek,rtl8366rb",
		.data = &rtl8366rb_variant,
	},
	{
		/* FIXME: add support for RTL8366S and more */
		.compatible = "realtek,rtl8366s",
		.data = NULL,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, realtek_smi_of_match);

static struct platform_driver realtek_smi_driver = {
	.driver = {
		.name = "realtek-smi",
		.of_match_table = of_match_ptr(realtek_smi_of_match),
	},
	.probe  = realtek_smi_probe,
	.remove = realtek_smi_remove,
};
module_platform_driver(realtek_smi_driver);

MODULE_LICENSE("GPL");
