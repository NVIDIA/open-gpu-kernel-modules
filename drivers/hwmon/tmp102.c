// SPDX-License-Identifier: GPL-2.0-or-later
/* Texas Instruments TMP102 SMBus temperature sensor driver
 *
 * Copyright (C) 2010 Steven King <sfking@fdwdc.com>
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/regmap.h>
#include <linux/of.h>

#define	DRIVER_NAME "tmp102"

#define	TMP102_TEMP_REG			0x00
#define	TMP102_CONF_REG			0x01
/* note: these bit definitions are byte swapped */
#define		TMP102_CONF_SD		0x0100
#define		TMP102_CONF_TM		0x0200
#define		TMP102_CONF_POL		0x0400
#define		TMP102_CONF_F0		0x0800
#define		TMP102_CONF_F1		0x1000
#define		TMP102_CONF_R0		0x2000
#define		TMP102_CONF_R1		0x4000
#define		TMP102_CONF_OS		0x8000
#define		TMP102_CONF_EM		0x0010
#define		TMP102_CONF_AL		0x0020
#define		TMP102_CONF_CR0		0x0040
#define		TMP102_CONF_CR1		0x0080
#define	TMP102_TLOW_REG			0x02
#define	TMP102_THIGH_REG		0x03

#define TMP102_CONFREG_MASK	(TMP102_CONF_SD | TMP102_CONF_TM | \
				 TMP102_CONF_POL | TMP102_CONF_F0 | \
				 TMP102_CONF_F1 | TMP102_CONF_OS | \
				 TMP102_CONF_EM | TMP102_CONF_AL | \
				 TMP102_CONF_CR0 | TMP102_CONF_CR1)

#define TMP102_CONFIG_CLEAR	(TMP102_CONF_SD | TMP102_CONF_OS | \
				 TMP102_CONF_CR0)
#define TMP102_CONFIG_SET	(TMP102_CONF_TM | TMP102_CONF_EM | \
				 TMP102_CONF_CR1)

#define CONVERSION_TIME_MS		35	/* in milli-seconds */

struct tmp102 {
	struct regmap *regmap;
	u16 config_orig;
	unsigned long ready_time;
};

/* convert left adjusted 13-bit TMP102 register value to milliCelsius */
static inline int tmp102_reg_to_mC(s16 val)
{
	return ((val & ~0x01) * 1000) / 128;
}

/* convert milliCelsius to left adjusted 13-bit TMP102 register value */
static inline u16 tmp102_mC_to_reg(int val)
{
	return (val * 128) / 1000;
}

static int tmp102_read(struct device *dev, enum hwmon_sensor_types type,
		       u32 attr, int channel, long *temp)
{
	struct tmp102 *tmp102 = dev_get_drvdata(dev);
	unsigned int regval;
	int err, reg;

	switch (attr) {
	case hwmon_temp_input:
		/* Is it too early to return a conversion ? */
		if (time_before(jiffies, tmp102->ready_time)) {
			dev_dbg(dev, "%s: Conversion not ready yet..\n", __func__);
			return -EAGAIN;
		}
		reg = TMP102_TEMP_REG;
		break;
	case hwmon_temp_max_hyst:
		reg = TMP102_TLOW_REG;
		break;
	case hwmon_temp_max:
		reg = TMP102_THIGH_REG;
		break;
	default:
		return -EOPNOTSUPP;
	}

	err = regmap_read(tmp102->regmap, reg, &regval);
	if (err < 0)
		return err;
	*temp = tmp102_reg_to_mC(regval);

	return 0;
}

static int tmp102_write(struct device *dev, enum hwmon_sensor_types type,
			u32 attr, int channel, long temp)
{
	struct tmp102 *tmp102 = dev_get_drvdata(dev);
	int reg;

	switch (attr) {
	case hwmon_temp_max_hyst:
		reg = TMP102_TLOW_REG;
		break;
	case hwmon_temp_max:
		reg = TMP102_THIGH_REG;
		break;
	default:
		return -EOPNOTSUPP;
	}

	temp = clamp_val(temp, -256000, 255000);
	return regmap_write(tmp102->regmap, reg, tmp102_mC_to_reg(temp));
}

static umode_t tmp102_is_visible(const void *data, enum hwmon_sensor_types type,
				 u32 attr, int channel)
{
	if (type != hwmon_temp)
		return 0;

	switch (attr) {
	case hwmon_temp_input:
		return 0444;
	case hwmon_temp_max_hyst:
	case hwmon_temp_max:
		return 0644;
	default:
		return 0;
	}
}

static const struct hwmon_channel_info *tmp102_info[] = {
	HWMON_CHANNEL_INFO(chip,
			   HWMON_C_REGISTER_TZ),
	HWMON_CHANNEL_INFO(temp,
			   HWMON_T_INPUT | HWMON_T_MAX | HWMON_T_MAX_HYST),
	NULL
};

static const struct hwmon_ops tmp102_hwmon_ops = {
	.is_visible = tmp102_is_visible,
	.read = tmp102_read,
	.write = tmp102_write,
};

static const struct hwmon_chip_info tmp102_chip_info = {
	.ops = &tmp102_hwmon_ops,
	.info = tmp102_info,
};

static void tmp102_restore_config(void *data)
{
	struct tmp102 *tmp102 = data;

	regmap_write(tmp102->regmap, TMP102_CONF_REG, tmp102->config_orig);
}

static bool tmp102_is_writeable_reg(struct device *dev, unsigned int reg)
{
	return reg != TMP102_TEMP_REG;
}

static bool tmp102_is_volatile_reg(struct device *dev, unsigned int reg)
{
	return reg == TMP102_TEMP_REG;
}

static const struct regmap_config tmp102_regmap_config = {
	.reg_bits = 8,
	.val_bits = 16,
	.max_register = TMP102_THIGH_REG,
	.writeable_reg = tmp102_is_writeable_reg,
	.volatile_reg = tmp102_is_volatile_reg,
	.val_format_endian = REGMAP_ENDIAN_BIG,
	.cache_type = REGCACHE_RBTREE,
	.use_single_read = true,
	.use_single_write = true,
};

static int tmp102_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct device *hwmon_dev;
	struct tmp102 *tmp102;
	unsigned int regval;
	int err;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_WORD_DATA)) {
		dev_err(dev,
			"adapter doesn't support SMBus word transactions\n");
		return -ENODEV;
	}

	tmp102 = devm_kzalloc(dev, sizeof(*tmp102), GFP_KERNEL);
	if (!tmp102)
		return -ENOMEM;

	i2c_set_clientdata(client, tmp102);

	tmp102->regmap = devm_regmap_init_i2c(client, &tmp102_regmap_config);
	if (IS_ERR(tmp102->regmap))
		return PTR_ERR(tmp102->regmap);

	err = regmap_read(tmp102->regmap, TMP102_CONF_REG, &regval);
	if (err < 0) {
		dev_err(dev, "error reading config register\n");
		return err;
	}

	if ((regval & ~TMP102_CONFREG_MASK) !=
	    (TMP102_CONF_R0 | TMP102_CONF_R1)) {
		dev_err(dev, "unexpected config register value\n");
		return -ENODEV;
	}

	tmp102->config_orig = regval;

	err = devm_add_action_or_reset(dev, tmp102_restore_config, tmp102);
	if (err)
		return err;

	regval &= ~TMP102_CONFIG_CLEAR;
	regval |= TMP102_CONFIG_SET;

	err = regmap_write(tmp102->regmap, TMP102_CONF_REG, regval);
	if (err < 0) {
		dev_err(dev, "error writing config register\n");
		return err;
	}

	/*
	 * Mark that we are not ready with data until the first
	 * conversion is complete
	 */
	tmp102->ready_time = jiffies + msecs_to_jiffies(CONVERSION_TIME_MS);

	hwmon_dev = devm_hwmon_device_register_with_info(dev, client->name,
							 tmp102,
							 &tmp102_chip_info,
							 NULL);
	if (IS_ERR(hwmon_dev)) {
		dev_dbg(dev, "unable to register hwmon device\n");
		return PTR_ERR(hwmon_dev);
	}
	dev_info(dev, "initialized\n");

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int tmp102_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct tmp102 *tmp102 = i2c_get_clientdata(client);

	return regmap_update_bits(tmp102->regmap, TMP102_CONF_REG,
				  TMP102_CONF_SD, TMP102_CONF_SD);
}

static int tmp102_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct tmp102 *tmp102 = i2c_get_clientdata(client);
	int err;

	err = regmap_update_bits(tmp102->regmap, TMP102_CONF_REG,
				 TMP102_CONF_SD, 0);

	tmp102->ready_time = jiffies + msecs_to_jiffies(CONVERSION_TIME_MS);

	return err;
}
#endif /* CONFIG_PM */

static SIMPLE_DEV_PM_OPS(tmp102_dev_pm_ops, tmp102_suspend, tmp102_resume);

static const struct i2c_device_id tmp102_id[] = {
	{ "tmp102", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tmp102_id);

static const struct of_device_id __maybe_unused tmp102_of_match[] = {
	{ .compatible = "ti,tmp102" },
	{ },
};
MODULE_DEVICE_TABLE(of, tmp102_of_match);

static struct i2c_driver tmp102_driver = {
	.driver.name	= DRIVER_NAME,
	.driver.of_match_table = of_match_ptr(tmp102_of_match),
	.driver.pm	= &tmp102_dev_pm_ops,
	.probe_new	= tmp102_probe,
	.id_table	= tmp102_id,
};

module_i2c_driver(tmp102_driver);

MODULE_AUTHOR("Steven King <sfking@fdwdc.com>");
MODULE_DESCRIPTION("Texas Instruments TMP102 temperature sensor driver");
MODULE_LICENSE("GPL");
