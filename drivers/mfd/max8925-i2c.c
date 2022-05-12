// SPDX-License-Identifier: GPL-2.0-only
/*
 * I2C driver for Maxim MAX8925
 *
 * Copyright (C) 2009 Marvell International Ltd.
 *	Haojian Zhuang <haojian.zhuang@marvell.com>
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/mfd/max8925.h>
#include <linux/slab.h>

#define RTC_I2C_ADDR		0x68
#define ADC_I2C_ADDR		0x47

static inline int max8925_read_device(struct i2c_client *i2c,
				      int reg, int bytes, void *dest)
{
	int ret;

	if (bytes > 1)
		ret = i2c_smbus_read_i2c_block_data(i2c, reg, bytes, dest);
	else {
		ret = i2c_smbus_read_byte_data(i2c, reg);
		if (ret < 0)
			return ret;
		*(unsigned char *)dest = (unsigned char)ret;
	}
	return ret;
}

static inline int max8925_write_device(struct i2c_client *i2c,
				       int reg, int bytes, void *src)
{
	unsigned char buf[9];
	int ret;

	buf[0] = (unsigned char)reg;
	memcpy(&buf[1], src, bytes);

	ret = i2c_master_send(i2c, buf, bytes + 1);
	if (ret < 0)
		return ret;
	return 0;
}

int max8925_reg_read(struct i2c_client *i2c, int reg)
{
	struct max8925_chip *chip = i2c_get_clientdata(i2c);
	unsigned char data = 0;
	int ret;

	mutex_lock(&chip->io_lock);
	ret = max8925_read_device(i2c, reg, 1, &data);
	mutex_unlock(&chip->io_lock);

	if (ret < 0)
		return ret;
	else
		return (int)data;
}
EXPORT_SYMBOL(max8925_reg_read);

int max8925_reg_write(struct i2c_client *i2c, int reg,
		unsigned char data)
{
	struct max8925_chip *chip = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&chip->io_lock);
	ret = max8925_write_device(i2c, reg, 1, &data);
	mutex_unlock(&chip->io_lock);

	return ret;
}
EXPORT_SYMBOL(max8925_reg_write);

int max8925_bulk_read(struct i2c_client *i2c, int reg,
		int count, unsigned char *buf)
{
	struct max8925_chip *chip = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&chip->io_lock);
	ret = max8925_read_device(i2c, reg, count, buf);
	mutex_unlock(&chip->io_lock);

	return ret;
}
EXPORT_SYMBOL(max8925_bulk_read);

int max8925_bulk_write(struct i2c_client *i2c, int reg,
		int count, unsigned char *buf)
{
	struct max8925_chip *chip = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&chip->io_lock);
	ret = max8925_write_device(i2c, reg, count, buf);
	mutex_unlock(&chip->io_lock);

	return ret;
}
EXPORT_SYMBOL(max8925_bulk_write);

int max8925_set_bits(struct i2c_client *i2c, int reg,
		unsigned char mask, unsigned char data)
{
	struct max8925_chip *chip = i2c_get_clientdata(i2c);
	unsigned char value;
	int ret;

	mutex_lock(&chip->io_lock);
	ret = max8925_read_device(i2c, reg, 1, &value);
	if (ret < 0)
		goto out;
	value &= ~mask;
	value |= data;
	ret = max8925_write_device(i2c, reg, 1, &value);
out:
	mutex_unlock(&chip->io_lock);
	return ret;
}
EXPORT_SYMBOL(max8925_set_bits);


static const struct i2c_device_id max8925_id_table[] = {
	{ "max8925", 0 },
	{ },
};

static int max8925_dt_init(struct device_node *np, struct device *dev,
			   struct max8925_platform_data *pdata)
{
	int ret;

	ret = of_property_read_u32(np, "maxim,tsc-irq", &pdata->tsc_irq);
	if (ret) {
		dev_err(dev, "Not found maxim,tsc-irq property\n");
		return -EINVAL;
	}
	return 0;
}

static int max8925_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	struct max8925_platform_data *pdata = dev_get_platdata(&client->dev);
	struct max8925_chip *chip;
	struct device_node *node = client->dev.of_node;

	if (node && !pdata) {
		/* parse DT to get platform data */
		pdata = devm_kzalloc(&client->dev,
				     sizeof(struct max8925_platform_data),
				     GFP_KERNEL);
		if (!pdata)
			return -ENOMEM;

		if (max8925_dt_init(node, &client->dev, pdata))
			return -EINVAL;
	} else if (!pdata) {
		pr_info("%s: platform data is missing\n", __func__);
		return -EINVAL;
	}

	chip = devm_kzalloc(&client->dev,
			    sizeof(struct max8925_chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;
	chip->i2c = client;
	chip->dev = &client->dev;
	i2c_set_clientdata(client, chip);
	dev_set_drvdata(chip->dev, chip);
	mutex_init(&chip->io_lock);

	chip->rtc = i2c_new_dummy_device(chip->i2c->adapter, RTC_I2C_ADDR);
	if (IS_ERR(chip->rtc)) {
		dev_err(chip->dev, "Failed to allocate I2C device for RTC\n");
		return PTR_ERR(chip->rtc);
	}
	i2c_set_clientdata(chip->rtc, chip);

	chip->adc = i2c_new_dummy_device(chip->i2c->adapter, ADC_I2C_ADDR);
	if (IS_ERR(chip->adc)) {
		dev_err(chip->dev, "Failed to allocate I2C device for ADC\n");
		i2c_unregister_device(chip->rtc);
		return PTR_ERR(chip->adc);
	}
	i2c_set_clientdata(chip->adc, chip);

	device_init_wakeup(&client->dev, 1);

	max8925_device_init(chip, pdata);

	return 0;
}

static int max8925_remove(struct i2c_client *client)
{
	struct max8925_chip *chip = i2c_get_clientdata(client);

	max8925_device_exit(chip);
	i2c_unregister_device(chip->adc);
	i2c_unregister_device(chip->rtc);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int max8925_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max8925_chip *chip = i2c_get_clientdata(client);

	if (device_may_wakeup(dev) && chip->wakeup_flag)
		enable_irq_wake(chip->core_irq);
	return 0;
}

static int max8925_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max8925_chip *chip = i2c_get_clientdata(client);

	if (device_may_wakeup(dev) && chip->wakeup_flag)
		disable_irq_wake(chip->core_irq);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(max8925_pm_ops, max8925_suspend, max8925_resume);

static const struct of_device_id max8925_dt_ids[] = {
	{ .compatible = "maxim,max8925", },
	{},
};

static struct i2c_driver max8925_driver = {
	.driver	= {
		.name	= "max8925",
		.pm     = &max8925_pm_ops,
		.of_match_table = max8925_dt_ids,
	},
	.probe		= max8925_probe,
	.remove		= max8925_remove,
	.id_table	= max8925_id_table,
};

static int __init max8925_i2c_init(void)
{
	int ret;

	ret = i2c_add_driver(&max8925_driver);
	if (ret != 0)
		pr_err("Failed to register MAX8925 I2C driver: %d\n", ret);

	return ret;
}
subsys_initcall(max8925_i2c_init);
