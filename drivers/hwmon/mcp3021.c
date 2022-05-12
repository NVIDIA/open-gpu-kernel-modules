// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * mcp3021.c - driver for Microchip MCP3021 and MCP3221
 *
 * Copyright (C) 2008-2009, 2012 Freescale Semiconductor, Inc.
 * Author: Mingkai Hu <Mingkai.hu@freescale.com>
 * Reworked by Sven Schuchmann <schuchmann@schleissheimer.de>
 * DT support added by Clemens Gruber <clemens.gruber@pqgruber.com>
 *
 * This driver export the value of analog input voltage to sysfs, the
 * voltage unit is mV. Through the sysfs interface, lm-sensors tool
 * can also display the input voltage.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hwmon.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>

/* Vdd / reference voltage in millivolt */
#define MCP3021_VDD_REF_MAX	5500
#define MCP3021_VDD_REF_MIN	2700
#define MCP3021_VDD_REF_DEFAULT	3300

/* output format */
#define MCP3021_SAR_SHIFT	2
#define MCP3021_SAR_MASK	0x3ff
#define MCP3021_OUTPUT_RES	10	/* 10-bit resolution */

#define MCP3221_SAR_SHIFT	0
#define MCP3221_SAR_MASK	0xfff
#define MCP3221_OUTPUT_RES	12	/* 12-bit resolution */

enum chips {
	mcp3021,
	mcp3221
};

/*
 * Client data (each client gets its own)
 */
struct mcp3021_data {
	struct device *hwmon_dev;
	u32 vdd;        /* supply and reference voltage in millivolt */
	u16 sar_shift;
	u16 sar_mask;
	u8 output_res;
};

static int mcp3021_read16(struct i2c_client *client)
{
	struct mcp3021_data *data = i2c_get_clientdata(client);
	int ret;
	u16 reg;
	__be16 buf;

	ret = i2c_master_recv(client, (char *)&buf, 2);
	if (ret < 0)
		return ret;
	if (ret != 2)
		return -EIO;

	/* The output code of the MCP3021 is transmitted with MSB first. */
	reg = be16_to_cpu(buf);

	/*
	 * The ten-bit output code is composed of the lower 4-bit of the
	 * first byte and the upper 6-bit of the second byte.
	 */
	reg = (reg >> data->sar_shift) & data->sar_mask;

	return reg;
}

static inline u16 volts_from_reg(struct mcp3021_data *data, u16 val)
{
	return DIV_ROUND_CLOSEST(data->vdd * val, 1 << data->output_res);
}

static ssize_t in0_input_show(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mcp3021_data *data = i2c_get_clientdata(client);
	int reg, in_input;

	reg = mcp3021_read16(client);
	if (reg < 0)
		return reg;

	in_input = volts_from_reg(data, reg);

	return sprintf(buf, "%d\n", in_input);
}

static DEVICE_ATTR_RO(in0_input);

static const struct i2c_device_id mcp3021_id[];

static int mcp3021_probe(struct i2c_client *client)
{
	int err;
	struct mcp3021_data *data = NULL;
	struct device_node *np = client->dev.of_node;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	data = devm_kzalloc(&client->dev, sizeof(struct mcp3021_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	i2c_set_clientdata(client, data);

	if (np) {
		if (!of_property_read_u32(np, "reference-voltage-microvolt",
					  &data->vdd))
			data->vdd /= 1000;
		else
			data->vdd = MCP3021_VDD_REF_DEFAULT;
	} else {
		u32 *pdata = dev_get_platdata(&client->dev);

		if (pdata)
			data->vdd = *pdata;
		else
			data->vdd = MCP3021_VDD_REF_DEFAULT;
	}

	switch (i2c_match_id(mcp3021_id, client)->driver_data) {
	case mcp3021:
		data->sar_shift = MCP3021_SAR_SHIFT;
		data->sar_mask = MCP3021_SAR_MASK;
		data->output_res = MCP3021_OUTPUT_RES;
		break;

	case mcp3221:
		data->sar_shift = MCP3221_SAR_SHIFT;
		data->sar_mask = MCP3221_SAR_MASK;
		data->output_res = MCP3221_OUTPUT_RES;
		break;
	}

	if (data->vdd > MCP3021_VDD_REF_MAX || data->vdd < MCP3021_VDD_REF_MIN)
		return -EINVAL;

	err = sysfs_create_file(&client->dev.kobj, &dev_attr_in0_input.attr);
	if (err)
		return err;

	data->hwmon_dev = hwmon_device_register(&client->dev);
	if (IS_ERR(data->hwmon_dev)) {
		err = PTR_ERR(data->hwmon_dev);
		goto exit_remove;
	}

	return 0;

exit_remove:
	sysfs_remove_file(&client->dev.kobj, &dev_attr_in0_input.attr);
	return err;
}

static int mcp3021_remove(struct i2c_client *client)
{
	struct mcp3021_data *data = i2c_get_clientdata(client);

	hwmon_device_unregister(data->hwmon_dev);
	sysfs_remove_file(&client->dev.kobj, &dev_attr_in0_input.attr);

	return 0;
}

static const struct i2c_device_id mcp3021_id[] = {
	{ "mcp3021", mcp3021 },
	{ "mcp3221", mcp3221 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mcp3021_id);

#ifdef CONFIG_OF
static const struct of_device_id of_mcp3021_match[] = {
	{ .compatible = "microchip,mcp3021", .data = (void *)mcp3021 },
	{ .compatible = "microchip,mcp3221", .data = (void *)mcp3221 },
	{ }
};
MODULE_DEVICE_TABLE(of, of_mcp3021_match);
#endif

static struct i2c_driver mcp3021_driver = {
	.driver = {
		.name = "mcp3021",
		.of_match_table = of_match_ptr(of_mcp3021_match),
	},
	.probe_new = mcp3021_probe,
	.remove = mcp3021_remove,
	.id_table = mcp3021_id,
};

module_i2c_driver(mcp3021_driver);

MODULE_AUTHOR("Mingkai Hu <Mingkai.hu@freescale.com>");
MODULE_DESCRIPTION("Microchip MCP3021/MCP3221 driver");
MODULE_LICENSE("GPL");
