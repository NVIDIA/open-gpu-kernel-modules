// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * max8952.c - Voltage and current regulation for the Maxim 8952
 *
 * Copyright (C) 2010 Samsung Electronics
 * MyungJoo Ham <myungjoo.ham@samsung.com>
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/max8952.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/regulator/of_regulator.h>
#include <linux/slab.h>

/* Registers */
enum {
	MAX8952_REG_MODE0,
	MAX8952_REG_MODE1,
	MAX8952_REG_MODE2,
	MAX8952_REG_MODE3,
	MAX8952_REG_CONTROL,
	MAX8952_REG_SYNC,
	MAX8952_REG_RAMP,
	MAX8952_REG_CHIP_ID1,
	MAX8952_REG_CHIP_ID2,
};

struct max8952_data {
	struct i2c_client	*client;
	struct max8952_platform_data *pdata;
	struct gpio_desc *vid0_gpiod;
	struct gpio_desc *vid1_gpiod;
	bool vid0;
	bool vid1;
};

static int max8952_read_reg(struct max8952_data *max8952, u8 reg)
{
	int ret = i2c_smbus_read_byte_data(max8952->client, reg);

	if (ret > 0)
		ret &= 0xff;

	return ret;
}

static int max8952_write_reg(struct max8952_data *max8952,
		u8 reg, u8 value)
{
	return i2c_smbus_write_byte_data(max8952->client, reg, value);
}

static int max8952_list_voltage(struct regulator_dev *rdev,
		unsigned int selector)
{
	struct max8952_data *max8952 = rdev_get_drvdata(rdev);

	if (rdev_get_id(rdev) != 0)
		return -EINVAL;

	return (max8952->pdata->dvs_mode[selector] * 10 + 770) * 1000;
}

static int max8952_get_voltage_sel(struct regulator_dev *rdev)
{
	struct max8952_data *max8952 = rdev_get_drvdata(rdev);
	u8 vid = 0;

	if (max8952->vid0)
		vid += 1;
	if (max8952->vid1)
		vid += 2;

	return vid;
}

static int max8952_set_voltage_sel(struct regulator_dev *rdev,
				   unsigned selector)
{
	struct max8952_data *max8952 = rdev_get_drvdata(rdev);

	if (!max8952->vid0_gpiod || !max8952->vid1_gpiod) {
		/* DVS not supported */
		return -EPERM;
	}

	max8952->vid0 = selector & 0x1;
	max8952->vid1 = (selector >> 1) & 0x1;
	gpiod_set_value(max8952->vid0_gpiod, max8952->vid0);
	gpiod_set_value(max8952->vid1_gpiod, max8952->vid1);

	return 0;
}

static const struct regulator_ops max8952_ops = {
	.list_voltage		= max8952_list_voltage,
	.get_voltage_sel	= max8952_get_voltage_sel,
	.set_voltage_sel	= max8952_set_voltage_sel,
};

static const struct regulator_desc regulator = {
	.name		= "MAX8952_VOUT",
	.id		= 0,
	.n_voltages	= MAX8952_NUM_DVS_MODE,
	.ops		= &max8952_ops,
	.type		= REGULATOR_VOLTAGE,
	.owner		= THIS_MODULE,
};

#ifdef CONFIG_OF
static const struct of_device_id max8952_dt_match[] = {
	{ .compatible = "maxim,max8952" },
	{},
};
MODULE_DEVICE_TABLE(of, max8952_dt_match);

static struct max8952_platform_data *max8952_parse_dt(struct device *dev)
{
	struct max8952_platform_data *pd;
	struct device_node *np = dev->of_node;
	int ret;
	int i;

	pd = devm_kzalloc(dev, sizeof(*pd), GFP_KERNEL);
	if (!pd)
		return NULL;

	if (of_property_read_u32(np, "max8952,default-mode", &pd->default_mode))
		dev_warn(dev, "Default mode not specified, assuming 0\n");

	ret = of_property_read_u32_array(np, "max8952,dvs-mode-microvolt",
					pd->dvs_mode, ARRAY_SIZE(pd->dvs_mode));
	if (ret) {
		dev_err(dev, "max8952,dvs-mode-microvolt property not specified");
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(pd->dvs_mode); ++i) {
		if (pd->dvs_mode[i] < 770000 || pd->dvs_mode[i] > 1400000) {
			dev_err(dev, "DVS voltage %d out of range\n", i);
			return NULL;
		}
		pd->dvs_mode[i] = (pd->dvs_mode[i] - 770000) / 10000;
	}

	if (of_property_read_u32(np, "max8952,sync-freq", &pd->sync_freq))
		dev_warn(dev, "max8952,sync-freq property not specified, defaulting to 26MHz\n");

	if (of_property_read_u32(np, "max8952,ramp-speed", &pd->ramp_speed))
		dev_warn(dev, "max8952,ramp-speed property not specified, defaulting to 32mV/us\n");

	pd->reg_data = of_get_regulator_init_data(dev, np, &regulator);
	if (!pd->reg_data) {
		dev_err(dev, "Failed to parse regulator init data\n");
		return NULL;
	}

	return pd;
}
#else
static struct max8952_platform_data *max8952_parse_dt(struct device *dev)
{
	return NULL;
}
#endif

static int max8952_pmic_probe(struct i2c_client *client,
		const struct i2c_device_id *i2c_id)
{
	struct i2c_adapter *adapter = client->adapter;
	struct max8952_platform_data *pdata = dev_get_platdata(&client->dev);
	struct regulator_config config = { };
	struct max8952_data *max8952;
	struct regulator_dev *rdev;
	struct gpio_desc *gpiod;
	enum gpiod_flags gflags;

	int ret = 0;

	if (client->dev.of_node)
		pdata = max8952_parse_dt(&client->dev);

	if (!pdata) {
		dev_err(&client->dev, "Require the platform data\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE))
		return -EIO;

	max8952 = devm_kzalloc(&client->dev, sizeof(struct max8952_data),
			       GFP_KERNEL);
	if (!max8952)
		return -ENOMEM;

	max8952->client = client;
	max8952->pdata = pdata;

	config.dev = &client->dev;
	config.init_data = pdata->reg_data;
	config.driver_data = max8952;
	config.of_node = client->dev.of_node;

	if (pdata->reg_data->constraints.boot_on)
		gflags = GPIOD_OUT_HIGH;
	else
		gflags = GPIOD_OUT_LOW;
	gflags |= GPIOD_FLAGS_BIT_NONEXCLUSIVE;
	/*
	 * Do not use devm* here: the regulator core takes over the
	 * lifecycle management of the GPIO descriptor.
	 */
	gpiod = gpiod_get_optional(&client->dev,
				   "max8952,en",
				   gflags);
	if (IS_ERR(gpiod))
		return PTR_ERR(gpiod);
	if (gpiod)
		config.ena_gpiod = gpiod;

	rdev = devm_regulator_register(&client->dev, &regulator, &config);
	if (IS_ERR(rdev)) {
		ret = PTR_ERR(rdev);
		dev_err(&client->dev, "regulator init failed (%d)\n", ret);
		return ret;
	}

	max8952->vid0 = pdata->default_mode & 0x1;
	max8952->vid1 = (pdata->default_mode >> 1) & 0x1;

	/* Fetch vid0 and vid1 GPIOs if available */
	gflags = max8952->vid0 ? GPIOD_OUT_HIGH : GPIOD_OUT_LOW;
	max8952->vid0_gpiod = devm_gpiod_get_index_optional(&client->dev,
							    "max8952,vid",
							    0, gflags);
	if (IS_ERR(max8952->vid0_gpiod))
		return PTR_ERR(max8952->vid0_gpiod);
	gflags = max8952->vid1 ? GPIOD_OUT_HIGH : GPIOD_OUT_LOW;
	max8952->vid1_gpiod = devm_gpiod_get_index_optional(&client->dev,
							    "max8952,vid",
							    1, gflags);
	if (IS_ERR(max8952->vid1_gpiod))
		return PTR_ERR(max8952->vid1_gpiod);

	/* If either VID GPIO is missing just disable this */
	if (!max8952->vid0_gpiod || !max8952->vid1_gpiod) {
		dev_warn(&client->dev, "VID0/1 gpio invalid: "
			 "DVS not available.\n");
		max8952->vid0 = 0;
		max8952->vid1 = 0;
		/* Make sure if we have any descriptors they get set to low */
		if (max8952->vid0_gpiod)
			gpiod_set_value(max8952->vid0_gpiod, 0);
		if (max8952->vid1_gpiod)
			gpiod_set_value(max8952->vid1_gpiod, 0);

		/* Disable Pulldown of EN only */
		max8952_write_reg(max8952, MAX8952_REG_CONTROL, 0x60);

		dev_err(&client->dev, "DVS modes disabled because VID0 and VID1"
				" do not have proper controls.\n");
	} else {
		/*
		 * Disable Pulldown on EN, VID0, VID1 to reduce
		 * leakage current of MAX8952 assuming that MAX8952
		 * is turned on (EN==1). Note that without having VID0/1
		 * properly connected, turning pulldown off can be
		 * problematic. Thus, turn this off only when they are
		 * controllable by GPIO.
		 */
		max8952_write_reg(max8952, MAX8952_REG_CONTROL, 0x0);
	}

	max8952_write_reg(max8952, MAX8952_REG_MODE0,
			(max8952_read_reg(max8952,
					  MAX8952_REG_MODE0) & 0xC0) |
			(pdata->dvs_mode[0] & 0x3F));
	max8952_write_reg(max8952, MAX8952_REG_MODE1,
			(max8952_read_reg(max8952,
					  MAX8952_REG_MODE1) & 0xC0) |
			(pdata->dvs_mode[1] & 0x3F));
	max8952_write_reg(max8952, MAX8952_REG_MODE2,
			(max8952_read_reg(max8952,
					  MAX8952_REG_MODE2) & 0xC0) |
			(pdata->dvs_mode[2] & 0x3F));
	max8952_write_reg(max8952, MAX8952_REG_MODE3,
			(max8952_read_reg(max8952,
					  MAX8952_REG_MODE3) & 0xC0) |
			(pdata->dvs_mode[3] & 0x3F));

	max8952_write_reg(max8952, MAX8952_REG_SYNC,
			(max8952_read_reg(max8952, MAX8952_REG_SYNC) & 0x3F) |
			((pdata->sync_freq & 0x3) << 6));
	max8952_write_reg(max8952, MAX8952_REG_RAMP,
			(max8952_read_reg(max8952, MAX8952_REG_RAMP) & 0x1F) |
			((pdata->ramp_speed & 0x7) << 5));

	i2c_set_clientdata(client, max8952);

	return 0;
}

static const struct i2c_device_id max8952_ids[] = {
	{ "max8952", 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, max8952_ids);

static struct i2c_driver max8952_pmic_driver = {
	.probe		= max8952_pmic_probe,
	.driver		= {
		.name	= "max8952",
		.of_match_table = of_match_ptr(max8952_dt_match),
	},
	.id_table	= max8952_ids,
};

static int __init max8952_pmic_init(void)
{
	return i2c_add_driver(&max8952_pmic_driver);
}
subsys_initcall(max8952_pmic_init);

static void __exit max8952_pmic_exit(void)
{
	i2c_del_driver(&max8952_pmic_driver);
}
module_exit(max8952_pmic_exit);

MODULE_DESCRIPTION("MAXIM 8952 voltage regulator driver");
MODULE_AUTHOR("MyungJoo Ham <myungjoo.ham@samsung.com>");
MODULE_LICENSE("GPL");
