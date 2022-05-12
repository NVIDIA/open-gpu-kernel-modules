// SPDX-License-Identifier: GPL-2.0-only
/*
 * This file is part of wl1251
 *
 * Copyright (C) 2008 Nokia Corporation
 */

#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/swab.h>
#include <linux/crc7.h>
#include <linux/spi/spi.h>
#include <linux/wl12xx.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>

#include "wl1251.h"
#include "reg.h"
#include "spi.h"

static irqreturn_t wl1251_irq(int irq, void *cookie)
{
	struct wl1251 *wl;

	wl1251_debug(DEBUG_IRQ, "IRQ");

	wl = cookie;

	ieee80211_queue_work(wl->hw, &wl->irq_work);

	return IRQ_HANDLED;
}

static struct spi_device *wl_to_spi(struct wl1251 *wl)
{
	return wl->if_priv;
}

static void wl1251_spi_reset(struct wl1251 *wl)
{
	u8 *cmd;
	struct spi_transfer t;
	struct spi_message m;

	cmd = kzalloc(WSPI_INIT_CMD_LEN, GFP_KERNEL);
	if (!cmd) {
		wl1251_error("could not allocate cmd for spi reset");
		return;
	}

	memset(&t, 0, sizeof(t));
	spi_message_init(&m);

	memset(cmd, 0xff, WSPI_INIT_CMD_LEN);

	t.tx_buf = cmd;
	t.len = WSPI_INIT_CMD_LEN;
	spi_message_add_tail(&t, &m);

	spi_sync(wl_to_spi(wl), &m);

	wl1251_dump(DEBUG_SPI, "spi reset -> ", cmd, WSPI_INIT_CMD_LEN);

	kfree(cmd);
}

static void wl1251_spi_wake(struct wl1251 *wl)
{
	struct spi_transfer t;
	struct spi_message m;
	u8 *cmd = kzalloc(WSPI_INIT_CMD_LEN, GFP_KERNEL);

	if (!cmd) {
		wl1251_error("could not allocate cmd for spi init");
		return;
	}

	memset(&t, 0, sizeof(t));
	spi_message_init(&m);

	/* Set WSPI_INIT_COMMAND
	 * the data is being send from the MSB to LSB
	 */
	cmd[0] = 0xff;
	cmd[1] = 0xff;
	cmd[2] = WSPI_INIT_CMD_START | WSPI_INIT_CMD_TX;
	cmd[3] = 0;
	cmd[4] = 0;
	cmd[5] = HW_ACCESS_WSPI_INIT_CMD_MASK << 3;
	cmd[5] |= HW_ACCESS_WSPI_FIXED_BUSY_LEN & WSPI_INIT_CMD_FIXEDBUSY_LEN;

	cmd[6] = WSPI_INIT_CMD_IOD | WSPI_INIT_CMD_IP | WSPI_INIT_CMD_CS
		| WSPI_INIT_CMD_WSPI | WSPI_INIT_CMD_WS;

	if (HW_ACCESS_WSPI_FIXED_BUSY_LEN == 0)
		cmd[6] |= WSPI_INIT_CMD_DIS_FIXEDBUSY;
	else
		cmd[6] |= WSPI_INIT_CMD_EN_FIXEDBUSY;

	cmd[7] = crc7_be(0, cmd+2, WSPI_INIT_CMD_CRC_LEN) | WSPI_INIT_CMD_END;
	/*
	 * The above is the logical order; it must actually be stored
	 * in the buffer byte-swapped.
	 */
	__swab32s((u32 *)cmd);
	__swab32s((u32 *)cmd+1);

	t.tx_buf = cmd;
	t.len = WSPI_INIT_CMD_LEN;
	spi_message_add_tail(&t, &m);

	spi_sync(wl_to_spi(wl), &m);

	wl1251_dump(DEBUG_SPI, "spi init -> ", cmd, WSPI_INIT_CMD_LEN);

	kfree(cmd);
}

static void wl1251_spi_reset_wake(struct wl1251 *wl)
{
	wl1251_spi_reset(wl);
	wl1251_spi_wake(wl);
}

static void wl1251_spi_read(struct wl1251 *wl, int addr, void *buf,
			    size_t len)
{
	struct spi_transfer t[3];
	struct spi_message m;
	u8 *busy_buf;
	u32 *cmd;

	cmd = &wl->buffer_cmd;
	busy_buf = wl->buffer_busyword;

	*cmd = 0;
	*cmd |= WSPI_CMD_READ;
	*cmd |= (len << WSPI_CMD_BYTE_LENGTH_OFFSET) & WSPI_CMD_BYTE_LENGTH;
	*cmd |= addr & WSPI_CMD_BYTE_ADDR;

	spi_message_init(&m);
	memset(t, 0, sizeof(t));

	t[0].tx_buf = cmd;
	t[0].len = 4;
	spi_message_add_tail(&t[0], &m);

	/* Busy and non busy words read */
	t[1].rx_buf = busy_buf;
	t[1].len = WL1251_BUSY_WORD_LEN;
	spi_message_add_tail(&t[1], &m);

	t[2].rx_buf = buf;
	t[2].len = len;
	spi_message_add_tail(&t[2], &m);

	spi_sync(wl_to_spi(wl), &m);

	/* FIXME: check busy words */

	wl1251_dump(DEBUG_SPI, "spi_read cmd -> ", cmd, sizeof(*cmd));
	wl1251_dump(DEBUG_SPI, "spi_read buf <- ", buf, len);
}

static void wl1251_spi_write(struct wl1251 *wl, int addr, void *buf,
			     size_t len)
{
	struct spi_transfer t[2];
	struct spi_message m;
	u32 *cmd;

	cmd = &wl->buffer_cmd;

	*cmd = 0;
	*cmd |= WSPI_CMD_WRITE;
	*cmd |= (len << WSPI_CMD_BYTE_LENGTH_OFFSET) & WSPI_CMD_BYTE_LENGTH;
	*cmd |= addr & WSPI_CMD_BYTE_ADDR;

	spi_message_init(&m);
	memset(t, 0, sizeof(t));

	t[0].tx_buf = cmd;
	t[0].len = sizeof(*cmd);
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = buf;
	t[1].len = len;
	spi_message_add_tail(&t[1], &m);

	spi_sync(wl_to_spi(wl), &m);

	wl1251_dump(DEBUG_SPI, "spi_write cmd -> ", cmd, sizeof(*cmd));
	wl1251_dump(DEBUG_SPI, "spi_write buf -> ", buf, len);
}

static void wl1251_spi_enable_irq(struct wl1251 *wl)
{
	return enable_irq(wl->irq);
}

static void wl1251_spi_disable_irq(struct wl1251 *wl)
{
	return disable_irq(wl->irq);
}

static int wl1251_spi_set_power(struct wl1251 *wl, bool enable)
{
	if (gpio_is_valid(wl->power_gpio))
		gpio_set_value(wl->power_gpio, enable);

	return 0;
}

static const struct wl1251_if_operations wl1251_spi_ops = {
	.read = wl1251_spi_read,
	.write = wl1251_spi_write,
	.reset = wl1251_spi_reset_wake,
	.enable_irq = wl1251_spi_enable_irq,
	.disable_irq = wl1251_spi_disable_irq,
	.power = wl1251_spi_set_power,
};

static int wl1251_spi_probe(struct spi_device *spi)
{
	struct wl1251_platform_data *pdata = dev_get_platdata(&spi->dev);
	struct device_node *np = spi->dev.of_node;
	struct ieee80211_hw *hw;
	struct wl1251 *wl;
	int ret;

	if (!np && !pdata) {
		wl1251_error("no platform data");
		return -ENODEV;
	}

	hw = wl1251_alloc_hw();
	if (IS_ERR(hw))
		return PTR_ERR(hw);

	wl = hw->priv;

	SET_IEEE80211_DEV(hw, &spi->dev);
	spi_set_drvdata(spi, wl);
	wl->if_priv = spi;
	wl->if_ops = &wl1251_spi_ops;

	/* This is the only SPI value that we need to set here, the rest
	 * comes from the board-peripherals file
	 */
	spi->bits_per_word = 32;

	ret = spi_setup(spi);
	if (ret < 0) {
		wl1251_error("spi_setup failed");
		goto out_free;
	}

	if (np) {
		wl->use_eeprom = of_property_read_bool(np, "ti,wl1251-has-eeprom");
		wl->power_gpio = of_get_named_gpio(np, "ti,power-gpio", 0);
	} else if (pdata) {
		wl->power_gpio = pdata->power_gpio;
		wl->use_eeprom = pdata->use_eeprom;
	}

	if (wl->power_gpio == -EPROBE_DEFER) {
		ret = -EPROBE_DEFER;
		goto out_free;
	}

	if (gpio_is_valid(wl->power_gpio)) {
		ret = devm_gpio_request_one(&spi->dev, wl->power_gpio,
					GPIOF_OUT_INIT_LOW, "wl1251 power");
		if (ret) {
			wl1251_error("Failed to request gpio: %d\n", ret);
			goto out_free;
		}
	} else {
		wl1251_error("set power gpio missing in platform data");
		ret = -ENODEV;
		goto out_free;
	}

	wl->irq = spi->irq;
	if (wl->irq < 0) {
		wl1251_error("irq missing in platform data");
		ret = -ENODEV;
		goto out_free;
	}

	irq_set_status_flags(wl->irq, IRQ_NOAUTOEN);
	ret = devm_request_irq(&spi->dev, wl->irq, wl1251_irq, 0,
							DRIVER_NAME, wl);
	if (ret < 0) {
		wl1251_error("request_irq() failed: %d", ret);
		goto out_free;
	}

	irq_set_irq_type(wl->irq, IRQ_TYPE_EDGE_RISING);

	wl->vio = devm_regulator_get(&spi->dev, "vio");
	if (IS_ERR(wl->vio)) {
		ret = PTR_ERR(wl->vio);
		wl1251_error("vio regulator missing: %d", ret);
		goto out_free;
	}

	ret = regulator_enable(wl->vio);
	if (ret)
		goto out_free;

	ret = wl1251_init_ieee80211(wl);
	if (ret)
		goto disable_regulator;

	return 0;

disable_regulator:
	regulator_disable(wl->vio);
out_free:
	ieee80211_free_hw(hw);

	return ret;
}

static int wl1251_spi_remove(struct spi_device *spi)
{
	struct wl1251 *wl = spi_get_drvdata(spi);

	wl1251_free_hw(wl);
	regulator_disable(wl->vio);

	return 0;
}

static struct spi_driver wl1251_spi_driver = {
	.driver = {
		.name		= DRIVER_NAME,
	},

	.probe		= wl1251_spi_probe,
	.remove		= wl1251_spi_remove,
};

module_spi_driver(wl1251_spi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kalle Valo <kvalo@adurom.com>");
MODULE_ALIAS("spi:wl1251");
