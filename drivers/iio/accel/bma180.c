// SPDX-License-Identifier: GPL-2.0-only
/*
 * bma180.c - IIO driver for Bosch BMA180 triaxial acceleration sensor
 *
 * Copyright 2013 Oleksandr Kravchenko <x0199363@ti.com>
 *
 * Support for BMA250 (c) Peter Meerwald <pmeerw@pmeerw.net>
 *
 * SPI is not supported by driver
 * BMA023/BMA150/SMB380: 7-bit I2C slave address 0x38
 * BMA180: 7-bit I2C slave address 0x40 or 0x41
 * BMA250: 7-bit I2C slave address 0x18 or 0x19
 * BMA254: 7-bit I2C slave address 0x18 or 0x19
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/of_device.h>
#include <linux/of.h>
#include <linux/bitops.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/buffer.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

#define BMA180_DRV_NAME "bma180"
#define BMA180_IRQ_NAME "bma180_event"

enum chip_ids {
	BMA023,
	BMA150,
	BMA180,
	BMA250,
	BMA254,
};

struct bma180_data;

struct bma180_part_info {
	u8 chip_id;
	const struct iio_chan_spec *channels;
	unsigned int num_channels;
	const int *scale_table;
	unsigned int num_scales;
	const int *bw_table;
	unsigned int num_bw;
	int temp_offset;

	u8 int_reset_reg, int_reset_mask;
	u8 sleep_reg, sleep_mask;
	u8 bw_reg, bw_mask;
	u8 scale_reg, scale_mask;
	u8 power_reg, power_mask, lowpower_val;
	u8 int_enable_reg, int_enable_mask;
	u8 int_map_reg, int_enable_dataready_int1_mask;
	u8 softreset_reg, softreset_val;

	int (*chip_config)(struct bma180_data *data);
	void (*chip_disable)(struct bma180_data *data);
};

/* Register set */
#define BMA023_CTRL_REG0	0x0a
#define BMA023_CTRL_REG1	0x0b
#define BMA023_CTRL_REG2	0x14
#define BMA023_CTRL_REG3	0x15

#define BMA023_RANGE_MASK	GENMASK(4, 3) /* Range of accel values */
#define BMA023_BW_MASK		GENMASK(2, 0) /* Accel bandwidth */
#define BMA023_SLEEP		BIT(0)
#define BMA023_INT_RESET_MASK	BIT(6)
#define BMA023_NEW_DATA_INT	BIT(5) /* Intr every new accel data is ready */
#define BMA023_RESET_VAL	BIT(1)

#define BMA180_CHIP_ID		0x00 /* Need to distinguish BMA180 from other */
#define BMA180_ACC_X_LSB	0x02 /* First of 6 registers of accel data */
#define BMA180_TEMP		0x08
#define BMA180_CTRL_REG0	0x0d
#define BMA180_RESET		0x10
#define BMA180_BW_TCS		0x20
#define BMA180_CTRL_REG3	0x21
#define BMA180_TCO_Z		0x30
#define BMA180_OFFSET_LSB1	0x35

/* BMA180_CTRL_REG0 bits */
#define BMA180_DIS_WAKE_UP	BIT(0) /* Disable wake up mode */
#define BMA180_SLEEP		BIT(1) /* 1 - chip will sleep */
#define BMA180_EE_W		BIT(4) /* Unlock writing to addr from 0x20 */
#define BMA180_RESET_INT	BIT(6) /* Reset pending interrupts */

/* BMA180_CTRL_REG3 bits */
#define BMA180_NEW_DATA_INT	BIT(1) /* Intr every new accel data is ready */

/* BMA180_OFFSET_LSB1 skipping mode bit */
#define BMA180_SMP_SKIP		BIT(0)

/* Bit masks for registers bit fields */
#define BMA180_RANGE		0x0e /* Range of measured accel values */
#define BMA180_BW		0xf0 /* Accel bandwidth */
#define BMA180_MODE_CONFIG	0x03 /* Config operation modes */

/* We have to write this value in reset register to do soft reset */
#define BMA180_RESET_VAL	0xb6

#define BMA023_ID_REG_VAL	0x02
#define BMA180_ID_REG_VAL	0x03
#define BMA250_ID_REG_VAL	0x03
#define BMA254_ID_REG_VAL	0xfa /* 250 decimal */

/* Chip power modes */
#define BMA180_LOW_POWER	0x03

#define BMA250_RANGE_REG	0x0f
#define BMA250_BW_REG		0x10
#define BMA250_POWER_REG	0x11
#define BMA250_RESET_REG	0x14
#define BMA250_INT_ENABLE_REG	0x17
#define BMA250_INT_MAP_REG	0x1a
#define BMA250_INT_RESET_REG	0x21

#define BMA250_RANGE_MASK	GENMASK(3, 0) /* Range of accel values */
#define BMA250_BW_MASK		GENMASK(4, 0) /* Accel bandwidth */
#define BMA250_SUSPEND_MASK	BIT(7) /* chip will sleep */
#define BMA250_LOWPOWER_MASK	BIT(6)
#define BMA250_DATA_INTEN_MASK	BIT(4)
#define BMA250_INT1_DATA_MASK	BIT(0)
#define BMA250_INT_RESET_MASK	BIT(7) /* Reset pending interrupts */

#define BMA254_RANGE_REG	0x0f
#define BMA254_BW_REG		0x10
#define BMA254_POWER_REG	0x11
#define BMA254_RESET_REG	0x14
#define BMA254_INT_ENABLE_REG	0x17
#define BMA254_INT_MAP_REG	0x1a
#define BMA254_INT_RESET_REG	0x21

#define BMA254_RANGE_MASK	GENMASK(3, 0) /* Range of accel values */
#define BMA254_BW_MASK		GENMASK(4, 0) /* Accel bandwidth */
#define BMA254_SUSPEND_MASK	BIT(7) /* chip will sleep */
#define BMA254_LOWPOWER_MASK	BIT(6)
#define BMA254_DATA_INTEN_MASK	BIT(4)
#define BMA254_INT2_DATA_MASK	BIT(7)
#define BMA254_INT1_DATA_MASK	BIT(0)
#define BMA254_INT_RESET_MASK	BIT(7) /* Reset pending interrupts */

struct bma180_data {
	struct regulator *vdd_supply;
	struct regulator *vddio_supply;
	struct i2c_client *client;
	struct iio_trigger *trig;
	const struct bma180_part_info *part_info;
	struct iio_mount_matrix orientation;
	struct mutex mutex;
	bool sleep_state;
	int scale;
	int bw;
	bool pmode;
	u8 buff[16]; /* 3x 16-bit + 8-bit + padding + timestamp */
};

enum bma180_chan {
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	TEMP
};

static int bma023_bw_table[] = { 25, 50, 100, 190, 375, 750, 1500 }; /* Hz */
static int bma023_scale_table[] = { 2452, 4903, 9709, };

static int bma180_bw_table[] = { 10, 20, 40, 75, 150, 300 }; /* Hz */
static int bma180_scale_table[] = { 1275, 1863, 2452, 3727, 4903, 9709, 19417 };

static int bma25x_bw_table[] = { 8, 16, 31, 63, 125, 250 }; /* Hz */
static int bma25x_scale_table[] = { 0, 0, 0, 38344, 0, 76590, 0, 0, 153180, 0,
	0, 0, 306458 };

static int bma180_get_data_reg(struct bma180_data *data, enum bma180_chan chan)
{
	int ret;

	if (data->sleep_state)
		return -EBUSY;

	switch (chan) {
	case TEMP:
		ret = i2c_smbus_read_byte_data(data->client, BMA180_TEMP);
		if (ret < 0)
			dev_err(&data->client->dev, "failed to read temp register\n");
		break;
	default:
		ret = i2c_smbus_read_word_data(data->client,
			BMA180_ACC_X_LSB + chan * 2);
		if (ret < 0)
			dev_err(&data->client->dev,
				"failed to read accel_%c register\n",
				'x' + chan);
	}

	return ret;
}

static int bma180_set_bits(struct bma180_data *data, u8 reg, u8 mask, u8 val)
{
	int ret = i2c_smbus_read_byte_data(data->client, reg);
	u8 reg_val = (ret & ~mask) | (val << (ffs(mask) - 1));

	if (ret < 0)
		return ret;

	return i2c_smbus_write_byte_data(data->client, reg, reg_val);
}

static int bma180_reset_intr(struct bma180_data *data)
{
	int ret = bma180_set_bits(data, data->part_info->int_reset_reg,
		data->part_info->int_reset_mask, 1);

	if (ret)
		dev_err(&data->client->dev, "failed to reset interrupt\n");

	return ret;
}

static int bma180_set_new_data_intr_state(struct bma180_data *data, bool state)
{
	int ret = bma180_set_bits(data, data->part_info->int_enable_reg,
			data->part_info->int_enable_mask, state);
	if (ret)
		goto err;
	ret = bma180_reset_intr(data);
	if (ret)
		goto err;

	return 0;

err:
	dev_err(&data->client->dev,
		"failed to set new data interrupt state %d\n", state);
	return ret;
}

static int bma180_set_sleep_state(struct bma180_data *data, bool state)
{
	int ret = bma180_set_bits(data, data->part_info->sleep_reg,
		data->part_info->sleep_mask, state);

	if (ret) {
		dev_err(&data->client->dev,
			"failed to set sleep state %d\n", state);
		return ret;
	}
	data->sleep_state = state;

	return 0;
}

static int bma180_set_ee_writing_state(struct bma180_data *data, bool state)
{
	int ret = bma180_set_bits(data, BMA180_CTRL_REG0, BMA180_EE_W, state);

	if (ret)
		dev_err(&data->client->dev,
			"failed to set ee writing state %d\n", state);

	return ret;
}

static int bma180_set_bw(struct bma180_data *data, int val)
{
	int ret, i;

	if (data->sleep_state)
		return -EBUSY;

	for (i = 0; i < data->part_info->num_bw; ++i) {
		if (data->part_info->bw_table[i] == val) {
			ret = bma180_set_bits(data, data->part_info->bw_reg,
				data->part_info->bw_mask, i);
			if (ret) {
				dev_err(&data->client->dev,
					"failed to set bandwidth\n");
				return ret;
			}
			data->bw = val;
			return 0;
		}
	}

	return -EINVAL;
}

static int bma180_set_scale(struct bma180_data *data, int val)
{
	int ret, i;

	if (data->sleep_state)
		return -EBUSY;

	for (i = 0; i < data->part_info->num_scales; ++i)
		if (data->part_info->scale_table[i] == val) {
			ret = bma180_set_bits(data, data->part_info->scale_reg,
				data->part_info->scale_mask, i);
			if (ret) {
				dev_err(&data->client->dev,
					"failed to set scale\n");
				return ret;
			}
			data->scale = val;
			return 0;
		}

	return -EINVAL;
}

static int bma180_set_pmode(struct bma180_data *data, bool mode)
{
	u8 reg_val = mode ? data->part_info->lowpower_val : 0;
	int ret = bma180_set_bits(data, data->part_info->power_reg,
		data->part_info->power_mask, reg_val);

	if (ret) {
		dev_err(&data->client->dev, "failed to set power mode\n");
		return ret;
	}
	data->pmode = mode;

	return 0;
}

static int bma180_soft_reset(struct bma180_data *data)
{
	int ret = i2c_smbus_write_byte_data(data->client,
		data->part_info->softreset_reg,
		data->part_info->softreset_val);

	if (ret)
		dev_err(&data->client->dev, "failed to reset the chip\n");

	return ret;
}

static int bma180_chip_init(struct bma180_data *data)
{
	/* Try to read chip_id register. It must return 0x03. */
	int ret = i2c_smbus_read_byte_data(data->client, BMA180_CHIP_ID);

	if (ret < 0)
		return ret;
	if (ret != data->part_info->chip_id) {
		dev_err(&data->client->dev, "wrong chip ID %d expected %d\n",
			ret, data->part_info->chip_id);
		return -ENODEV;
	}

	ret = bma180_soft_reset(data);
	if (ret)
		return ret;
	/*
	 * No serial transaction should occur within minimum 10 us
	 * after soft_reset command
	 */
	msleep(20);

	return bma180_set_new_data_intr_state(data, false);
}

static int bma023_chip_config(struct bma180_data *data)
{
	int ret = bma180_chip_init(data);

	if (ret)
		goto err;

	ret = bma180_set_bw(data, 50); /* 50 Hz */
	if (ret)
		goto err;
	ret = bma180_set_scale(data, 2452); /* 2 G */
	if (ret)
		goto err;

	return 0;

err:
	dev_err(&data->client->dev, "failed to config the chip\n");
	return ret;
}

static int bma180_chip_config(struct bma180_data *data)
{
	int ret = bma180_chip_init(data);

	if (ret)
		goto err;
	ret = bma180_set_pmode(data, false);
	if (ret)
		goto err;
	ret = bma180_set_bits(data, BMA180_CTRL_REG0, BMA180_DIS_WAKE_UP, 1);
	if (ret)
		goto err;
	ret = bma180_set_ee_writing_state(data, true);
	if (ret)
		goto err;
	ret = bma180_set_bits(data, BMA180_OFFSET_LSB1, BMA180_SMP_SKIP, 1);
	if (ret)
		goto err;
	ret = bma180_set_bw(data, 20); /* 20 Hz */
	if (ret)
		goto err;
	ret = bma180_set_scale(data, 2452); /* 2 G */
	if (ret)
		goto err;

	return 0;

err:
	dev_err(&data->client->dev, "failed to config the chip\n");
	return ret;
}

static int bma25x_chip_config(struct bma180_data *data)
{
	int ret = bma180_chip_init(data);

	if (ret)
		goto err;
	ret = bma180_set_pmode(data, false);
	if (ret)
		goto err;
	ret = bma180_set_bw(data, 16); /* 16 Hz */
	if (ret)
		goto err;
	ret = bma180_set_scale(data, 38344); /* 2 G */
	if (ret)
		goto err;
	/*
	 * This enables dataready interrupt on the INT1 pin
	 * FIXME: support using the INT2 pin
	 */
	ret = bma180_set_bits(data, data->part_info->int_map_reg,
		data->part_info->int_enable_dataready_int1_mask, 1);
	if (ret)
		goto err;

	return 0;

err:
	dev_err(&data->client->dev, "failed to config the chip\n");
	return ret;
}

static void bma023_chip_disable(struct bma180_data *data)
{
	if (bma180_set_sleep_state(data, true))
		goto err;

	return;

err:
	dev_err(&data->client->dev, "failed to disable the chip\n");
}

static void bma180_chip_disable(struct bma180_data *data)
{
	if (bma180_set_new_data_intr_state(data, false))
		goto err;
	if (bma180_set_ee_writing_state(data, false))
		goto err;
	if (bma180_set_sleep_state(data, true))
		goto err;

	return;

err:
	dev_err(&data->client->dev, "failed to disable the chip\n");
}

static void bma25x_chip_disable(struct bma180_data *data)
{
	if (bma180_set_new_data_intr_state(data, false))
		goto err;
	if (bma180_set_sleep_state(data, true))
		goto err;

	return;

err:
	dev_err(&data->client->dev, "failed to disable the chip\n");
}

static ssize_t bma180_show_avail(char *buf, const int *vals, unsigned int n,
				 bool micros)
{
	size_t len = 0;
	int i;

	for (i = 0; i < n; i++) {
		if (!vals[i])
			continue;
		len += scnprintf(buf + len, PAGE_SIZE - len,
			micros ? "0.%06d " : "%d ", vals[i]);
	}
	buf[len - 1] = '\n';

	return len;
}

static ssize_t bma180_show_filter_freq_avail(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct bma180_data *data = iio_priv(dev_to_iio_dev(dev));

	return bma180_show_avail(buf, data->part_info->bw_table,
		data->part_info->num_bw, false);
}

static ssize_t bma180_show_scale_avail(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct bma180_data *data = iio_priv(dev_to_iio_dev(dev));

	return bma180_show_avail(buf, data->part_info->scale_table,
		data->part_info->num_scales, true);
}

static IIO_DEVICE_ATTR(in_accel_filter_low_pass_3db_frequency_available,
	S_IRUGO, bma180_show_filter_freq_avail, NULL, 0);

static IIO_DEVICE_ATTR(in_accel_scale_available,
	S_IRUGO, bma180_show_scale_avail, NULL, 0);

static struct attribute *bma180_attributes[] = {
	&iio_dev_attr_in_accel_filter_low_pass_3db_frequency_available.
		dev_attr.attr,
	&iio_dev_attr_in_accel_scale_available.dev_attr.attr,
	NULL,
};

static const struct attribute_group bma180_attrs_group = {
	.attrs = bma180_attributes,
};

static int bma180_read_raw(struct iio_dev *indio_dev,
		struct iio_chan_spec const *chan, int *val, int *val2,
		long mask)
{
	struct bma180_data *data = iio_priv(indio_dev);
	int ret;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		ret = iio_device_claim_direct_mode(indio_dev);
		if (ret)
			return ret;

		mutex_lock(&data->mutex);
		ret = bma180_get_data_reg(data, chan->scan_index);
		mutex_unlock(&data->mutex);
		iio_device_release_direct_mode(indio_dev);
		if (ret < 0)
			return ret;
		if (chan->scan_type.sign == 's') {
			*val = sign_extend32(ret >> chan->scan_type.shift,
				chan->scan_type.realbits - 1);
		} else {
			*val = ret;
		}
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY:
		*val = data->bw;
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		switch (chan->type) {
		case IIO_ACCEL:
			*val = 0;
			*val2 = data->scale;
			return IIO_VAL_INT_PLUS_MICRO;
		case IIO_TEMP:
			*val = 500;
			return IIO_VAL_INT;
		default:
			return -EINVAL;
		}
	case IIO_CHAN_INFO_OFFSET:
		*val = data->part_info->temp_offset;
		return IIO_VAL_INT;
	default:
		return -EINVAL;
	}
}

static int bma180_write_raw(struct iio_dev *indio_dev,
		struct iio_chan_spec const *chan, int val, int val2, long mask)
{
	struct bma180_data *data = iio_priv(indio_dev);
	int ret;

	switch (mask) {
	case IIO_CHAN_INFO_SCALE:
		if (val)
			return -EINVAL;
		mutex_lock(&data->mutex);
		ret = bma180_set_scale(data, val2);
		mutex_unlock(&data->mutex);
		return ret;
	case IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY:
		if (val2)
			return -EINVAL;
		mutex_lock(&data->mutex);
		ret = bma180_set_bw(data, val);
		mutex_unlock(&data->mutex);
		return ret;
	default:
		return -EINVAL;
	}
}

static const struct iio_info bma180_info = {
	.attrs			= &bma180_attrs_group,
	.read_raw		= bma180_read_raw,
	.write_raw		= bma180_write_raw,
};

static const char * const bma180_power_modes[] = { "low_noise", "low_power" };

static int bma180_get_power_mode(struct iio_dev *indio_dev,
		const struct iio_chan_spec *chan)
{
	struct bma180_data *data = iio_priv(indio_dev);

	return data->pmode;
}

static int bma180_set_power_mode(struct iio_dev *indio_dev,
		const struct iio_chan_spec *chan, unsigned int mode)
{
	struct bma180_data *data = iio_priv(indio_dev);
	int ret;

	mutex_lock(&data->mutex);
	ret = bma180_set_pmode(data, mode);
	mutex_unlock(&data->mutex);

	return ret;
}

static const struct iio_mount_matrix *
bma180_accel_get_mount_matrix(const struct iio_dev *indio_dev,
				const struct iio_chan_spec *chan)
{
	struct bma180_data *data = iio_priv(indio_dev);

	return &data->orientation;
}

static const struct iio_enum bma180_power_mode_enum = {
	.items = bma180_power_modes,
	.num_items = ARRAY_SIZE(bma180_power_modes),
	.get = bma180_get_power_mode,
	.set = bma180_set_power_mode,
};

static const struct iio_chan_spec_ext_info bma023_ext_info[] = {
	IIO_MOUNT_MATRIX(IIO_SHARED_BY_DIR, bma180_accel_get_mount_matrix),
	{ }
};

static const struct iio_chan_spec_ext_info bma180_ext_info[] = {
	IIO_ENUM("power_mode", IIO_SHARED_BY_TYPE, &bma180_power_mode_enum),
	IIO_ENUM_AVAILABLE("power_mode", &bma180_power_mode_enum),
	IIO_MOUNT_MATRIX(IIO_SHARED_BY_DIR, bma180_accel_get_mount_matrix),
	{ }
};

#define BMA023_ACC_CHANNEL(_axis, _bits) {				\
	.type = IIO_ACCEL,						\
	.modified = 1,							\
	.channel2 = IIO_MOD_##_axis,					\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),			\
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE) |		\
		BIT(IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY),	\
	.scan_index = AXIS_##_axis,					\
	.scan_type = {							\
		.sign = 's',						\
		.realbits = _bits,					\
		.storagebits = 16,					\
		.shift = 16 - _bits,					\
	},								\
	.ext_info = bma023_ext_info,					\
}

#define BMA150_TEMP_CHANNEL {						\
	.type = IIO_TEMP,						\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |			\
		BIT(IIO_CHAN_INFO_SCALE) | BIT(IIO_CHAN_INFO_OFFSET),	\
	.scan_index = TEMP,						\
	.scan_type = {							\
		.sign = 'u',						\
		.realbits = 8,						\
		.storagebits = 16,					\
	},								\
}

#define BMA180_ACC_CHANNEL(_axis, _bits) {				\
	.type = IIO_ACCEL,						\
	.modified = 1,							\
	.channel2 = IIO_MOD_##_axis,					\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),			\
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE) |		\
		BIT(IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY),	\
	.scan_index = AXIS_##_axis,					\
	.scan_type = {							\
		.sign = 's',						\
		.realbits = _bits,					\
		.storagebits = 16,					\
		.shift = 16 - _bits,					\
	},								\
	.ext_info = bma180_ext_info,					\
}

#define BMA180_TEMP_CHANNEL {						\
	.type = IIO_TEMP,						\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |			\
		BIT(IIO_CHAN_INFO_SCALE) | BIT(IIO_CHAN_INFO_OFFSET),	\
	.scan_index = TEMP,						\
	.scan_type = {							\
		.sign = 's',						\
		.realbits = 8,						\
		.storagebits = 16,					\
	},								\
}

static const struct iio_chan_spec bma023_channels[] = {
	BMA023_ACC_CHANNEL(X, 10),
	BMA023_ACC_CHANNEL(Y, 10),
	BMA023_ACC_CHANNEL(Z, 10),
	IIO_CHAN_SOFT_TIMESTAMP(4),
};

static const struct iio_chan_spec bma150_channels[] = {
	BMA023_ACC_CHANNEL(X, 10),
	BMA023_ACC_CHANNEL(Y, 10),
	BMA023_ACC_CHANNEL(Z, 10),
	BMA150_TEMP_CHANNEL,
	IIO_CHAN_SOFT_TIMESTAMP(4),
};

static const struct iio_chan_spec bma180_channels[] = {
	BMA180_ACC_CHANNEL(X, 14),
	BMA180_ACC_CHANNEL(Y, 14),
	BMA180_ACC_CHANNEL(Z, 14),
	BMA180_TEMP_CHANNEL,
	IIO_CHAN_SOFT_TIMESTAMP(4),
};

static const struct iio_chan_spec bma250_channels[] = {
	BMA180_ACC_CHANNEL(X, 10),
	BMA180_ACC_CHANNEL(Y, 10),
	BMA180_ACC_CHANNEL(Z, 10),
	BMA180_TEMP_CHANNEL,
	IIO_CHAN_SOFT_TIMESTAMP(4),
};

static const struct iio_chan_spec bma254_channels[] = {
	BMA180_ACC_CHANNEL(X, 12),
	BMA180_ACC_CHANNEL(Y, 12),
	BMA180_ACC_CHANNEL(Z, 12),
	BMA180_TEMP_CHANNEL,
	IIO_CHAN_SOFT_TIMESTAMP(4),
};

static const struct bma180_part_info bma180_part_info[] = {
	[BMA023] = {
		.chip_id = BMA023_ID_REG_VAL,
		.channels = bma023_channels,
		.num_channels = ARRAY_SIZE(bma023_channels),
		.scale_table = bma023_scale_table,
		.num_scales = ARRAY_SIZE(bma023_scale_table),
		.bw_table = bma023_bw_table,
		.num_bw = ARRAY_SIZE(bma023_bw_table),
		/* No temperature channel */
		.temp_offset = 0,
		.int_reset_reg = BMA023_CTRL_REG0,
		.int_reset_mask = BMA023_INT_RESET_MASK,
		.sleep_reg = BMA023_CTRL_REG0,
		.sleep_mask = BMA023_SLEEP,
		.bw_reg = BMA023_CTRL_REG2,
		.bw_mask = BMA023_BW_MASK,
		.scale_reg = BMA023_CTRL_REG2,
		.scale_mask = BMA023_RANGE_MASK,
		/* No power mode on bma023 */
		.power_reg = 0,
		.power_mask = 0,
		.lowpower_val = 0,
		.int_enable_reg = BMA023_CTRL_REG3,
		.int_enable_mask = BMA023_NEW_DATA_INT,
		.softreset_reg = BMA023_CTRL_REG0,
		.softreset_val = BMA023_RESET_VAL,
		.chip_config = bma023_chip_config,
		.chip_disable = bma023_chip_disable,
	},
	[BMA150] = {
		.chip_id = BMA023_ID_REG_VAL,
		.channels = bma150_channels,
		.num_channels = ARRAY_SIZE(bma150_channels),
		.scale_table = bma023_scale_table,
		.num_scales = ARRAY_SIZE(bma023_scale_table),
		.bw_table = bma023_bw_table,
		.num_bw = ARRAY_SIZE(bma023_bw_table),
		.temp_offset = -60, /* 0 LSB @ -30 degree C */
		.int_reset_reg = BMA023_CTRL_REG0,
		.int_reset_mask = BMA023_INT_RESET_MASK,
		.sleep_reg = BMA023_CTRL_REG0,
		.sleep_mask = BMA023_SLEEP,
		.bw_reg = BMA023_CTRL_REG2,
		.bw_mask = BMA023_BW_MASK,
		.scale_reg = BMA023_CTRL_REG2,
		.scale_mask = BMA023_RANGE_MASK,
		/* No power mode on bma150 */
		.power_reg = 0,
		.power_mask = 0,
		.lowpower_val = 0,
		.int_enable_reg = BMA023_CTRL_REG3,
		.int_enable_mask = BMA023_NEW_DATA_INT,
		.softreset_reg = BMA023_CTRL_REG0,
		.softreset_val = BMA023_RESET_VAL,
		.chip_config = bma023_chip_config,
		.chip_disable = bma023_chip_disable,
	},
	[BMA180] = {
		.chip_id = BMA180_ID_REG_VAL,
		.channels = bma180_channels,
		.num_channels = ARRAY_SIZE(bma180_channels),
		.scale_table = bma180_scale_table,
		.num_scales = ARRAY_SIZE(bma180_scale_table),
		.bw_table = bma180_bw_table,
		.num_bw = ARRAY_SIZE(bma180_bw_table),
		.temp_offset = 48, /* 0 LSB @ 24 degree C */
		.int_reset_reg = BMA180_CTRL_REG0,
		.int_reset_mask = BMA180_RESET_INT,
		.sleep_reg = BMA180_CTRL_REG0,
		.sleep_mask = BMA180_SLEEP,
		.bw_reg = BMA180_BW_TCS,
		.bw_mask = BMA180_BW,
		.scale_reg = BMA180_OFFSET_LSB1,
		.scale_mask = BMA180_RANGE,
		.power_reg = BMA180_TCO_Z,
		.power_mask = BMA180_MODE_CONFIG,
		.lowpower_val = BMA180_LOW_POWER,
		.int_enable_reg = BMA180_CTRL_REG3,
		.int_enable_mask = BMA180_NEW_DATA_INT,
		.softreset_reg = BMA180_RESET,
		.softreset_val = BMA180_RESET_VAL,
		.chip_config = bma180_chip_config,
		.chip_disable = bma180_chip_disable,
	},
	[BMA250] = {
		.chip_id = BMA250_ID_REG_VAL,
		.channels = bma250_channels,
		.num_channels = ARRAY_SIZE(bma250_channels),
		.scale_table = bma25x_scale_table,
		.num_scales = ARRAY_SIZE(bma25x_scale_table),
		.bw_table = bma25x_bw_table,
		.num_bw = ARRAY_SIZE(bma25x_bw_table),
		.temp_offset = 48, /* 0 LSB @ 24 degree C */
		.int_reset_reg = BMA250_INT_RESET_REG,
		.int_reset_mask = BMA250_INT_RESET_MASK,
		.sleep_reg = BMA250_POWER_REG,
		.sleep_mask = BMA250_SUSPEND_MASK,
		.bw_reg = BMA250_BW_REG,
		.bw_mask = BMA250_BW_MASK,
		.scale_reg = BMA250_RANGE_REG,
		.scale_mask = BMA250_RANGE_MASK,
		.power_reg = BMA250_POWER_REG,
		.power_mask = BMA250_LOWPOWER_MASK,
		.lowpower_val = 1,
		.int_enable_reg = BMA250_INT_ENABLE_REG,
		.int_enable_mask = BMA250_DATA_INTEN_MASK,
		.int_map_reg = BMA250_INT_MAP_REG,
		.int_enable_dataready_int1_mask = BMA250_INT1_DATA_MASK,
		.softreset_reg = BMA250_RESET_REG,
		.softreset_val = BMA180_RESET_VAL,
		.chip_config = bma25x_chip_config,
		.chip_disable = bma25x_chip_disable,
	},
	[BMA254] = {
		.chip_id = BMA254_ID_REG_VAL,
		.channels = bma254_channels,
		.num_channels = ARRAY_SIZE(bma254_channels),
		.scale_table = bma25x_scale_table,
		.num_scales = ARRAY_SIZE(bma25x_scale_table),
		.bw_table = bma25x_bw_table,
		.num_bw = ARRAY_SIZE(bma25x_bw_table),
		.temp_offset = 46, /* 0 LSB @ 23 degree C */
		.int_reset_reg = BMA254_INT_RESET_REG,
		.int_reset_mask = BMA254_INT_RESET_MASK,
		.sleep_reg = BMA254_POWER_REG,
		.sleep_mask = BMA254_SUSPEND_MASK,
		.bw_reg = BMA254_BW_REG,
		.bw_mask = BMA254_BW_MASK,
		.scale_reg = BMA254_RANGE_REG,
		.scale_mask = BMA254_RANGE_MASK,
		.power_reg = BMA254_POWER_REG,
		.power_mask = BMA254_LOWPOWER_MASK,
		.lowpower_val = 1,
		.int_enable_reg = BMA254_INT_ENABLE_REG,
		.int_enable_mask = BMA254_DATA_INTEN_MASK,
		.int_map_reg = BMA254_INT_MAP_REG,
		.int_enable_dataready_int1_mask = BMA254_INT1_DATA_MASK,
		.softreset_reg = BMA254_RESET_REG,
		.softreset_val = BMA180_RESET_VAL,
		.chip_config = bma25x_chip_config,
		.chip_disable = bma25x_chip_disable,
	},
};

static irqreturn_t bma180_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct bma180_data *data = iio_priv(indio_dev);
	s64 time_ns = iio_get_time_ns(indio_dev);
	int bit, ret, i = 0;

	mutex_lock(&data->mutex);

	for_each_set_bit(bit, indio_dev->active_scan_mask,
			 indio_dev->masklength) {
		ret = bma180_get_data_reg(data, bit);
		if (ret < 0) {
			mutex_unlock(&data->mutex);
			goto err;
		}
		((s16 *)data->buff)[i++] = ret;
	}

	mutex_unlock(&data->mutex);

	iio_push_to_buffers_with_timestamp(indio_dev, data->buff, time_ns);
err:
	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}

static int bma180_data_rdy_trigger_set_state(struct iio_trigger *trig,
		bool state)
{
	struct iio_dev *indio_dev = iio_trigger_get_drvdata(trig);
	struct bma180_data *data = iio_priv(indio_dev);

	return bma180_set_new_data_intr_state(data, state);
}

static void bma180_trig_reen(struct iio_trigger *trig)
{
	struct iio_dev *indio_dev = iio_trigger_get_drvdata(trig);
	struct bma180_data *data = iio_priv(indio_dev);
	int ret;

	ret = bma180_reset_intr(data);
	if (ret)
		dev_err(&data->client->dev, "failed to reset interrupt\n");
}

static const struct iio_trigger_ops bma180_trigger_ops = {
	.set_trigger_state = bma180_data_rdy_trigger_set_state,
	.reenable = bma180_trig_reen,
};

static int bma180_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct bma180_data *data;
	struct iio_dev *indio_dev;
	enum chip_ids chip;
	int ret;

	indio_dev = devm_iio_device_alloc(dev, sizeof(*data));
	if (!indio_dev)
		return -ENOMEM;

	data = iio_priv(indio_dev);
	i2c_set_clientdata(client, indio_dev);
	data->client = client;
	if (client->dev.of_node)
		chip = (enum chip_ids)of_device_get_match_data(dev);
	else
		chip = id->driver_data;
	data->part_info = &bma180_part_info[chip];

	ret = iio_read_mount_matrix(dev, "mount-matrix",
				&data->orientation);
	if (ret)
		return ret;

	data->vdd_supply = devm_regulator_get(dev, "vdd");
	if (IS_ERR(data->vdd_supply))
		return dev_err_probe(dev, PTR_ERR(data->vdd_supply),
				     "Failed to get vdd regulator\n");

	data->vddio_supply = devm_regulator_get(dev, "vddio");
	if (IS_ERR(data->vddio_supply))
		return dev_err_probe(dev, PTR_ERR(data->vddio_supply),
				     "Failed to get vddio regulator\n");

	/* Typical voltage 2.4V these are min and max */
	ret = regulator_set_voltage(data->vdd_supply, 1620000, 3600000);
	if (ret)
		return ret;
	ret = regulator_set_voltage(data->vddio_supply, 1200000, 3600000);
	if (ret)
		return ret;
	ret = regulator_enable(data->vdd_supply);
	if (ret) {
		dev_err(dev, "Failed to enable vdd regulator: %d\n", ret);
		return ret;
	}
	ret = regulator_enable(data->vddio_supply);
	if (ret) {
		dev_err(dev, "Failed to enable vddio regulator: %d\n", ret);
		goto err_disable_vdd;
	}
	/* Wait to make sure we started up properly (3 ms at least) */
	usleep_range(3000, 5000);

	ret = data->part_info->chip_config(data);
	if (ret < 0)
		goto err_chip_disable;

	mutex_init(&data->mutex);
	indio_dev->channels = data->part_info->channels;
	indio_dev->num_channels = data->part_info->num_channels;
	indio_dev->name = id->name;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->info = &bma180_info;

	if (client->irq > 0) {
		data->trig = iio_trigger_alloc(dev, "%s-dev%d", indio_dev->name,
			indio_dev->id);
		if (!data->trig) {
			ret = -ENOMEM;
			goto err_chip_disable;
		}

		ret = devm_request_irq(dev, client->irq,
			iio_trigger_generic_data_rdy_poll, IRQF_TRIGGER_RISING,
			"bma180_event", data->trig);
		if (ret) {
			dev_err(dev, "unable to request IRQ\n");
			goto err_trigger_free;
		}

		data->trig->ops = &bma180_trigger_ops;
		iio_trigger_set_drvdata(data->trig, indio_dev);
		indio_dev->trig = iio_trigger_get(data->trig);

		ret = iio_trigger_register(data->trig);
		if (ret)
			goto err_trigger_free;
	}

	ret = iio_triggered_buffer_setup(indio_dev, NULL,
			bma180_trigger_handler, NULL);
	if (ret < 0) {
		dev_err(dev, "unable to setup iio triggered buffer\n");
		goto err_trigger_unregister;
	}

	ret = iio_device_register(indio_dev);
	if (ret < 0) {
		dev_err(dev, "unable to register iio device\n");
		goto err_buffer_cleanup;
	}

	return 0;

err_buffer_cleanup:
	iio_triggered_buffer_cleanup(indio_dev);
err_trigger_unregister:
	if (data->trig)
		iio_trigger_unregister(data->trig);
err_trigger_free:
	iio_trigger_free(data->trig);
err_chip_disable:
	data->part_info->chip_disable(data);
	regulator_disable(data->vddio_supply);
err_disable_vdd:
	regulator_disable(data->vdd_supply);

	return ret;
}

static int bma180_remove(struct i2c_client *client)
{
	struct iio_dev *indio_dev = i2c_get_clientdata(client);
	struct bma180_data *data = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	iio_triggered_buffer_cleanup(indio_dev);
	if (data->trig) {
		iio_trigger_unregister(data->trig);
		iio_trigger_free(data->trig);
	}

	mutex_lock(&data->mutex);
	data->part_info->chip_disable(data);
	mutex_unlock(&data->mutex);
	regulator_disable(data->vddio_supply);
	regulator_disable(data->vdd_supply);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int bma180_suspend(struct device *dev)
{
	struct iio_dev *indio_dev = i2c_get_clientdata(to_i2c_client(dev));
	struct bma180_data *data = iio_priv(indio_dev);
	int ret;

	mutex_lock(&data->mutex);
	ret = bma180_set_sleep_state(data, true);
	mutex_unlock(&data->mutex);

	return ret;
}

static int bma180_resume(struct device *dev)
{
	struct iio_dev *indio_dev = i2c_get_clientdata(to_i2c_client(dev));
	struct bma180_data *data = iio_priv(indio_dev);
	int ret;

	mutex_lock(&data->mutex);
	ret = bma180_set_sleep_state(data, false);
	mutex_unlock(&data->mutex);

	return ret;
}

static SIMPLE_DEV_PM_OPS(bma180_pm_ops, bma180_suspend, bma180_resume);
#define BMA180_PM_OPS (&bma180_pm_ops)
#else
#define BMA180_PM_OPS NULL
#endif

static const struct i2c_device_id bma180_ids[] = {
	{ "bma023", BMA023 },
	{ "bma150", BMA150 },
	{ "bma180", BMA180 },
	{ "bma250", BMA250 },
	{ "bma254", BMA254 },
	{ "smb380", BMA150 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, bma180_ids);

static const struct of_device_id bma180_of_match[] = {
	{
		.compatible = "bosch,bma023",
		.data = (void *)BMA023
	},
	{
		.compatible = "bosch,bma150",
		.data = (void *)BMA150
	},
	{
		.compatible = "bosch,bma180",
		.data = (void *)BMA180
	},
	{
		.compatible = "bosch,bma250",
		.data = (void *)BMA250
	},
	{
		.compatible = "bosch,bma254",
		.data = (void *)BMA254
	},
	{
		.compatible = "bosch,smb380",
		.data = (void *)BMA150
	},
	{ }
};
MODULE_DEVICE_TABLE(of, bma180_of_match);

static struct i2c_driver bma180_driver = {
	.driver = {
		.name	= "bma180",
		.pm	= BMA180_PM_OPS,
		.of_match_table = bma180_of_match,
	},
	.probe		= bma180_probe,
	.remove		= bma180_remove,
	.id_table	= bma180_ids,
};

module_i2c_driver(bma180_driver);

MODULE_AUTHOR("Kravchenko Oleksandr <x0199363@ti.com>");
MODULE_AUTHOR("Texas Instruments, Inc.");
MODULE_DESCRIPTION("Bosch BMA023/BMA1x0/BMA25x triaxial acceleration sensor");
MODULE_LICENSE("GPL");
