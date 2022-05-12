// SPDX-License-Identifier: GPL-2.0-only
/*
 * AD5024, AD5025, AD5044, AD5045, AD5064, AD5064-1, AD5065, AD5625, AD5625R,
 * AD5627, AD5627R, AD5628, AD5629R, AD5645R, AD5647R, AD5648, AD5665, AD5665R,
 * AD5666, AD5667, AD5667R, AD5668, AD5669R, LTC2606, LTC2607, LTC2609, LTC2616,
 * LTC2617, LTC2619, LTC2626, LTC2627, LTC2629, LTC2631, LTC2633, LTC2635
 * Digital to analog converters driver
 *
 * Copyright 2011 Analog Devices Inc.
 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/regulator/consumer.h>
#include <asm/unaligned.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

#define AD5064_MAX_DAC_CHANNELS			8
#define AD5064_MAX_VREFS			4

#define AD5064_ADDR(x)				((x) << 20)
#define AD5064_CMD(x)				((x) << 24)

#define AD5064_ADDR_ALL_DAC			0xF

#define AD5064_CMD_WRITE_INPUT_N		0x0
#define AD5064_CMD_UPDATE_DAC_N			0x1
#define AD5064_CMD_WRITE_INPUT_N_UPDATE_ALL	0x2
#define AD5064_CMD_WRITE_INPUT_N_UPDATE_N	0x3
#define AD5064_CMD_POWERDOWN_DAC		0x4
#define AD5064_CMD_CLEAR			0x5
#define AD5064_CMD_LDAC_MASK			0x6
#define AD5064_CMD_RESET			0x7
#define AD5064_CMD_CONFIG			0x8

#define AD5064_CMD_RESET_V2			0x5
#define AD5064_CMD_CONFIG_V2			0x7

#define AD5064_CONFIG_DAISY_CHAIN_ENABLE	BIT(1)
#define AD5064_CONFIG_INT_VREF_ENABLE		BIT(0)

#define AD5064_LDAC_PWRDN_NONE			0x0
#define AD5064_LDAC_PWRDN_1K			0x1
#define AD5064_LDAC_PWRDN_100K			0x2
#define AD5064_LDAC_PWRDN_3STATE		0x3

/**
 * enum ad5064_regmap_type - Register layout variant
 * @AD5064_REGMAP_ADI: Old Analog Devices register map layout
 * @AD5064_REGMAP_ADI2: New Analog Devices register map layout
 * @AD5064_REGMAP_LTC: LTC register map layout
 */
enum ad5064_regmap_type {
	AD5064_REGMAP_ADI,
	AD5064_REGMAP_ADI2,
	AD5064_REGMAP_LTC,
};

/**
 * struct ad5064_chip_info - chip specific information
 * @shared_vref:	whether the vref supply is shared between channels
 * @internal_vref:	internal reference voltage. 0 if the chip has no
 *			internal vref.
 * @channels:		channel specification
 * @num_channels:	number of channels
 * @regmap_type:	register map layout variant
 */

struct ad5064_chip_info {
	bool shared_vref;
	unsigned long internal_vref;
	const struct iio_chan_spec *channels;
	unsigned int num_channels;
	enum ad5064_regmap_type regmap_type;
};

struct ad5064_state;

typedef int (*ad5064_write_func)(struct ad5064_state *st, unsigned int cmd,
		unsigned int addr, unsigned int val);

/**
 * struct ad5064_state - driver instance specific data
 * @dev:		the device for this driver instance
 * @chip_info:		chip model specific constants, available modes etc
 * @vref_reg:		vref supply regulators
 * @pwr_down:		whether channel is powered down
 * @pwr_down_mode:	channel's current power down mode
 * @dac_cache:		current DAC raw value (chip does not support readback)
 * @use_internal_vref:	set to true if the internal reference voltage should be
 *			used.
 * @write:		register write callback
 * @lock:		maintain consistency between cached and dev state
 * @data:		i2c/spi transfer buffers
 */

struct ad5064_state {
	struct device			*dev;
	const struct ad5064_chip_info	*chip_info;
	struct regulator_bulk_data	vref_reg[AD5064_MAX_VREFS];
	bool				pwr_down[AD5064_MAX_DAC_CHANNELS];
	u8				pwr_down_mode[AD5064_MAX_DAC_CHANNELS];
	unsigned int			dac_cache[AD5064_MAX_DAC_CHANNELS];
	bool				use_internal_vref;

	ad5064_write_func		write;
	struct mutex lock;

	/*
	 * DMA (thus cache coherency maintenance) requires the
	 * transfer buffers to live in their own cache lines.
	 */
	union {
		u8 i2c[3];
		__be32 spi;
	} data ____cacheline_aligned;
};

enum ad5064_type {
	ID_AD5024,
	ID_AD5025,
	ID_AD5044,
	ID_AD5045,
	ID_AD5064,
	ID_AD5064_1,
	ID_AD5065,
	ID_AD5625,
	ID_AD5625R_1V25,
	ID_AD5625R_2V5,
	ID_AD5627,
	ID_AD5627R_1V25,
	ID_AD5627R_2V5,
	ID_AD5628_1,
	ID_AD5628_2,
	ID_AD5629_1,
	ID_AD5629_2,
	ID_AD5645R_1V25,
	ID_AD5645R_2V5,
	ID_AD5647R_1V25,
	ID_AD5647R_2V5,
	ID_AD5648_1,
	ID_AD5648_2,
	ID_AD5665,
	ID_AD5665R_1V25,
	ID_AD5665R_2V5,
	ID_AD5666_1,
	ID_AD5666_2,
	ID_AD5667,
	ID_AD5667R_1V25,
	ID_AD5667R_2V5,
	ID_AD5668_1,
	ID_AD5668_2,
	ID_AD5669_1,
	ID_AD5669_2,
	ID_LTC2606,
	ID_LTC2607,
	ID_LTC2609,
	ID_LTC2616,
	ID_LTC2617,
	ID_LTC2619,
	ID_LTC2626,
	ID_LTC2627,
	ID_LTC2629,
	ID_LTC2631_L12,
	ID_LTC2631_H12,
	ID_LTC2631_L10,
	ID_LTC2631_H10,
	ID_LTC2631_L8,
	ID_LTC2631_H8,
	ID_LTC2633_L12,
	ID_LTC2633_H12,
	ID_LTC2633_L10,
	ID_LTC2633_H10,
	ID_LTC2633_L8,
	ID_LTC2633_H8,
	ID_LTC2635_L12,
	ID_LTC2635_H12,
	ID_LTC2635_L10,
	ID_LTC2635_H10,
	ID_LTC2635_L8,
	ID_LTC2635_H8,
};

static int ad5064_write(struct ad5064_state *st, unsigned int cmd,
	unsigned int addr, unsigned int val, unsigned int shift)
{
	val <<= shift;

	return st->write(st, cmd, addr, val);
}

static int ad5064_sync_powerdown_mode(struct ad5064_state *st,
	const struct iio_chan_spec *chan)
{
	unsigned int val, address;
	unsigned int shift;
	int ret;

	if (st->chip_info->regmap_type == AD5064_REGMAP_LTC) {
		val = 0;
		address = chan->address;
	} else {
		if (st->chip_info->regmap_type == AD5064_REGMAP_ADI2)
			shift = 4;
		else
			shift = 8;

		val = (0x1 << chan->address);
		address = 0;

		if (st->pwr_down[chan->channel])
			val |= st->pwr_down_mode[chan->channel] << shift;
	}

	ret = ad5064_write(st, AD5064_CMD_POWERDOWN_DAC, address, val, 0);

	return ret;
}

static const char * const ad5064_powerdown_modes[] = {
	"1kohm_to_gnd",
	"100kohm_to_gnd",
	"three_state",
};

static const char * const ltc2617_powerdown_modes[] = {
	"90kohm_to_gnd",
};

static int ad5064_get_powerdown_mode(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan)
{
	struct ad5064_state *st = iio_priv(indio_dev);

	return st->pwr_down_mode[chan->channel] - 1;
}

static int ad5064_set_powerdown_mode(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, unsigned int mode)
{
	struct ad5064_state *st = iio_priv(indio_dev);
	int ret;

	mutex_lock(&st->lock);
	st->pwr_down_mode[chan->channel] = mode + 1;

	ret = ad5064_sync_powerdown_mode(st, chan);
	mutex_unlock(&st->lock);

	return ret;
}

static const struct iio_enum ad5064_powerdown_mode_enum = {
	.items = ad5064_powerdown_modes,
	.num_items = ARRAY_SIZE(ad5064_powerdown_modes),
	.get = ad5064_get_powerdown_mode,
	.set = ad5064_set_powerdown_mode,
};

static const struct iio_enum ltc2617_powerdown_mode_enum = {
	.items = ltc2617_powerdown_modes,
	.num_items = ARRAY_SIZE(ltc2617_powerdown_modes),
	.get = ad5064_get_powerdown_mode,
	.set = ad5064_set_powerdown_mode,
};

static ssize_t ad5064_read_dac_powerdown(struct iio_dev *indio_dev,
	uintptr_t private, const struct iio_chan_spec *chan, char *buf)
{
	struct ad5064_state *st = iio_priv(indio_dev);

	return sysfs_emit(buf, "%d\n", st->pwr_down[chan->channel]);
}

static ssize_t ad5064_write_dac_powerdown(struct iio_dev *indio_dev,
	 uintptr_t private, const struct iio_chan_spec *chan, const char *buf,
	 size_t len)
{
	struct ad5064_state *st = iio_priv(indio_dev);
	bool pwr_down;
	int ret;

	ret = strtobool(buf, &pwr_down);
	if (ret)
		return ret;

	mutex_lock(&st->lock);
	st->pwr_down[chan->channel] = pwr_down;

	ret = ad5064_sync_powerdown_mode(st, chan);
	mutex_unlock(&st->lock);
	return ret ? ret : len;
}

static int ad5064_get_vref(struct ad5064_state *st,
	struct iio_chan_spec const *chan)
{
	unsigned int i;

	if (st->use_internal_vref)
		return st->chip_info->internal_vref;

	i = st->chip_info->shared_vref ? 0 : chan->channel;
	return regulator_get_voltage(st->vref_reg[i].consumer);
}

static int ad5064_read_raw(struct iio_dev *indio_dev,
			   struct iio_chan_spec const *chan,
			   int *val,
			   int *val2,
			   long m)
{
	struct ad5064_state *st = iio_priv(indio_dev);
	int scale_uv;

	switch (m) {
	case IIO_CHAN_INFO_RAW:
		*val = st->dac_cache[chan->channel];
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		scale_uv = ad5064_get_vref(st, chan);
		if (scale_uv < 0)
			return scale_uv;

		*val = scale_uv / 1000;
		*val2 = chan->scan_type.realbits;
		return IIO_VAL_FRACTIONAL_LOG2;
	default:
		break;
	}
	return -EINVAL;
}

static int ad5064_write_raw(struct iio_dev *indio_dev,
	struct iio_chan_spec const *chan, int val, int val2, long mask)
{
	struct ad5064_state *st = iio_priv(indio_dev);
	int ret;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		if (val >= (1 << chan->scan_type.realbits) || val < 0)
			return -EINVAL;

		mutex_lock(&st->lock);
		ret = ad5064_write(st, AD5064_CMD_WRITE_INPUT_N_UPDATE_N,
				chan->address, val, chan->scan_type.shift);
		if (ret == 0)
			st->dac_cache[chan->channel] = val;
		mutex_unlock(&st->lock);
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static const struct iio_info ad5064_info = {
	.read_raw = ad5064_read_raw,
	.write_raw = ad5064_write_raw,
};

static const struct iio_chan_spec_ext_info ad5064_ext_info[] = {
	{
		.name = "powerdown",
		.read = ad5064_read_dac_powerdown,
		.write = ad5064_write_dac_powerdown,
		.shared = IIO_SEPARATE,
	},
	IIO_ENUM("powerdown_mode", IIO_SEPARATE, &ad5064_powerdown_mode_enum),
	IIO_ENUM_AVAILABLE("powerdown_mode", &ad5064_powerdown_mode_enum),
	{ },
};

static const struct iio_chan_spec_ext_info ltc2617_ext_info[] = {
	{
		.name = "powerdown",
		.read = ad5064_read_dac_powerdown,
		.write = ad5064_write_dac_powerdown,
		.shared = IIO_SEPARATE,
	},
	IIO_ENUM("powerdown_mode", IIO_SEPARATE, &ltc2617_powerdown_mode_enum),
	IIO_ENUM_AVAILABLE("powerdown_mode", &ltc2617_powerdown_mode_enum),
	{ },
};

#define AD5064_CHANNEL(chan, addr, bits, _shift, _ext_info) {		\
	.type = IIO_VOLTAGE,					\
	.indexed = 1,						\
	.output = 1,						\
	.channel = (chan),					\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |		\
	BIT(IIO_CHAN_INFO_SCALE),					\
	.address = addr,					\
	.scan_type = {						\
		.sign = 'u',					\
		.realbits = (bits),				\
		.storagebits = 16,				\
		.shift = (_shift),				\
	},							\
	.ext_info = (_ext_info),				\
}

#define DECLARE_AD5064_CHANNELS(name, bits, shift, ext_info) \
const struct iio_chan_spec name[] = { \
	AD5064_CHANNEL(0, 0, bits, shift, ext_info), \
	AD5064_CHANNEL(1, 1, bits, shift, ext_info), \
	AD5064_CHANNEL(2, 2, bits, shift, ext_info), \
	AD5064_CHANNEL(3, 3, bits, shift, ext_info), \
	AD5064_CHANNEL(4, 4, bits, shift, ext_info), \
	AD5064_CHANNEL(5, 5, bits, shift, ext_info), \
	AD5064_CHANNEL(6, 6, bits, shift, ext_info), \
	AD5064_CHANNEL(7, 7, bits, shift, ext_info), \
}

#define DECLARE_AD5065_CHANNELS(name, bits, shift, ext_info) \
const struct iio_chan_spec name[] = { \
	AD5064_CHANNEL(0, 0, bits, shift, ext_info), \
	AD5064_CHANNEL(1, 3, bits, shift, ext_info), \
}

static DECLARE_AD5064_CHANNELS(ad5024_channels, 12, 8, ad5064_ext_info);
static DECLARE_AD5064_CHANNELS(ad5044_channels, 14, 6, ad5064_ext_info);
static DECLARE_AD5064_CHANNELS(ad5064_channels, 16, 4, ad5064_ext_info);

static DECLARE_AD5065_CHANNELS(ad5025_channels, 12, 8, ad5064_ext_info);
static DECLARE_AD5065_CHANNELS(ad5045_channels, 14, 6, ad5064_ext_info);
static DECLARE_AD5065_CHANNELS(ad5065_channels, 16, 4, ad5064_ext_info);

static DECLARE_AD5064_CHANNELS(ad5629_channels, 12, 4, ad5064_ext_info);
static DECLARE_AD5064_CHANNELS(ad5645_channels, 14, 2, ad5064_ext_info);
static DECLARE_AD5064_CHANNELS(ad5669_channels, 16, 0, ad5064_ext_info);

static DECLARE_AD5064_CHANNELS(ltc2607_channels, 16, 0, ltc2617_ext_info);
static DECLARE_AD5064_CHANNELS(ltc2617_channels, 14, 2, ltc2617_ext_info);
static DECLARE_AD5064_CHANNELS(ltc2627_channels, 12, 4, ltc2617_ext_info);
#define ltc2631_12_channels ltc2627_channels
static DECLARE_AD5064_CHANNELS(ltc2631_10_channels, 10, 6, ltc2617_ext_info);
static DECLARE_AD5064_CHANNELS(ltc2631_8_channels, 8, 8, ltc2617_ext_info);

#define LTC2631_INFO(vref, pchannels, nchannels)	\
	{						\
		.shared_vref = true,			\
		.internal_vref = vref,			\
		.channels = pchannels,			\
		.num_channels = nchannels,		\
		.regmap_type = AD5064_REGMAP_LTC,	\
	}


static const struct ad5064_chip_info ad5064_chip_info_tbl[] = {
	[ID_AD5024] = {
		.shared_vref = false,
		.channels = ad5024_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5025] = {
		.shared_vref = false,
		.channels = ad5025_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5044] = {
		.shared_vref = false,
		.channels = ad5044_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5045] = {
		.shared_vref = false,
		.channels = ad5045_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5064] = {
		.shared_vref = false,
		.channels = ad5064_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5064_1] = {
		.shared_vref = true,
		.channels = ad5064_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5065] = {
		.shared_vref = false,
		.channels = ad5065_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5625] = {
		.shared_vref = true,
		.channels = ad5629_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5625R_1V25] = {
		.shared_vref = true,
		.internal_vref = 1250000,
		.channels = ad5629_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5625R_2V5] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5629_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5627] = {
		.shared_vref = true,
		.channels = ad5629_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5627R_1V25] = {
		.shared_vref = true,
		.internal_vref = 1250000,
		.channels = ad5629_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5627R_2V5] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5629_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5628_1] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5024_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5628_2] = {
		.shared_vref = true,
		.internal_vref = 5000000,
		.channels = ad5024_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5629_1] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5629_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5629_2] = {
		.shared_vref = true,
		.internal_vref = 5000000,
		.channels = ad5629_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5645R_1V25] = {
		.shared_vref = true,
		.internal_vref = 1250000,
		.channels = ad5645_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5645R_2V5] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5645_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5647R_1V25] = {
		.shared_vref = true,
		.internal_vref = 1250000,
		.channels = ad5645_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5647R_2V5] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5645_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5648_1] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5044_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5648_2] = {
		.shared_vref = true,
		.internal_vref = 5000000,
		.channels = ad5044_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5665] = {
		.shared_vref = true,
		.channels = ad5669_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5665R_1V25] = {
		.shared_vref = true,
		.internal_vref = 1250000,
		.channels = ad5669_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5665R_2V5] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5669_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5666_1] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5064_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5666_2] = {
		.shared_vref = true,
		.internal_vref = 5000000,
		.channels = ad5064_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5667] = {
		.shared_vref = true,
		.channels = ad5669_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5667R_1V25] = {
		.shared_vref = true,
		.internal_vref = 1250000,
		.channels = ad5669_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5667R_2V5] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5669_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_ADI2
	},
	[ID_AD5668_1] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5064_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5668_2] = {
		.shared_vref = true,
		.internal_vref = 5000000,
		.channels = ad5064_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5669_1] = {
		.shared_vref = true,
		.internal_vref = 2500000,
		.channels = ad5669_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_AD5669_2] = {
		.shared_vref = true,
		.internal_vref = 5000000,
		.channels = ad5669_channels,
		.num_channels = 8,
		.regmap_type = AD5064_REGMAP_ADI,
	},
	[ID_LTC2606] = {
		.shared_vref = true,
		.internal_vref = 0,
		.channels = ltc2607_channels,
		.num_channels = 1,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2607] = {
		.shared_vref = true,
		.internal_vref = 0,
		.channels = ltc2607_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2609] = {
		.shared_vref = false,
		.internal_vref = 0,
		.channels = ltc2607_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2616] = {
		.shared_vref = true,
		.internal_vref = 0,
		.channels = ltc2617_channels,
		.num_channels = 1,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2617] = {
		.shared_vref = true,
		.internal_vref = 0,
		.channels = ltc2617_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2619] = {
		.shared_vref = false,
		.internal_vref = 0,
		.channels = ltc2617_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2626] = {
		.shared_vref = true,
		.internal_vref = 0,
		.channels = ltc2627_channels,
		.num_channels = 1,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2627] = {
		.shared_vref = true,
		.internal_vref = 0,
		.channels = ltc2627_channels,
		.num_channels = 2,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2629] = {
		.shared_vref = false,
		.internal_vref = 0,
		.channels = ltc2627_channels,
		.num_channels = 4,
		.regmap_type = AD5064_REGMAP_LTC,
	},
	[ID_LTC2631_L12] = LTC2631_INFO(2500000, ltc2631_12_channels, 1),
	[ID_LTC2631_H12] = LTC2631_INFO(4096000, ltc2631_12_channels, 1),
	[ID_LTC2631_L10] = LTC2631_INFO(2500000, ltc2631_10_channels, 1),
	[ID_LTC2631_H10] = LTC2631_INFO(4096000, ltc2631_10_channels, 1),
	[ID_LTC2631_L8] = LTC2631_INFO(2500000, ltc2631_8_channels, 1),
	[ID_LTC2631_H8] = LTC2631_INFO(4096000, ltc2631_8_channels, 1),
	[ID_LTC2633_L12] = LTC2631_INFO(2500000, ltc2631_12_channels, 2),
	[ID_LTC2633_H12] = LTC2631_INFO(4096000, ltc2631_12_channels, 2),
	[ID_LTC2633_L10] = LTC2631_INFO(2500000, ltc2631_10_channels, 2),
	[ID_LTC2633_H10] = LTC2631_INFO(4096000, ltc2631_10_channels, 2),
	[ID_LTC2633_L8] = LTC2631_INFO(2500000, ltc2631_8_channels, 2),
	[ID_LTC2633_H8] = LTC2631_INFO(4096000, ltc2631_8_channels, 2),
	[ID_LTC2635_L12] = LTC2631_INFO(2500000, ltc2631_12_channels, 4),
	[ID_LTC2635_H12] = LTC2631_INFO(4096000, ltc2631_12_channels, 4),
	[ID_LTC2635_L10] = LTC2631_INFO(2500000, ltc2631_10_channels, 4),
	[ID_LTC2635_H10] = LTC2631_INFO(4096000, ltc2631_10_channels, 4),
	[ID_LTC2635_L8] = LTC2631_INFO(2500000, ltc2631_8_channels, 4),
	[ID_LTC2635_H8] = LTC2631_INFO(4096000, ltc2631_8_channels, 4),
};

static inline unsigned int ad5064_num_vref(struct ad5064_state *st)
{
	return st->chip_info->shared_vref ? 1 : st->chip_info->num_channels;
}

static const char * const ad5064_vref_names[] = {
	"vrefA",
	"vrefB",
	"vrefC",
	"vrefD",
};

static const char *ad5064_vref_name(struct ad5064_state *st,
	unsigned int vref)
{
	return st->chip_info->shared_vref ? "vref" : ad5064_vref_names[vref];
}

static int ad5064_set_config(struct ad5064_state *st, unsigned int val)
{
	unsigned int cmd;

	switch (st->chip_info->regmap_type) {
	case AD5064_REGMAP_ADI2:
		cmd = AD5064_CMD_CONFIG_V2;
		break;
	default:
		cmd = AD5064_CMD_CONFIG;
		break;
	}

	return ad5064_write(st, cmd, 0, val, 0);
}

static int ad5064_request_vref(struct ad5064_state *st, struct device *dev)
{
	unsigned int i;
	int ret;

	for (i = 0; i < ad5064_num_vref(st); ++i)
		st->vref_reg[i].supply = ad5064_vref_name(st, i);

	if (!st->chip_info->internal_vref)
		return devm_regulator_bulk_get(dev, ad5064_num_vref(st),
					       st->vref_reg);

	/*
	 * This assumes that when the regulator has an internal VREF
	 * there is only one external VREF connection, which is
	 * currently the case for all supported devices.
	 */
	st->vref_reg[0].consumer = devm_regulator_get_optional(dev, "vref");
	if (!IS_ERR(st->vref_reg[0].consumer))
		return 0;

	ret = PTR_ERR(st->vref_reg[0].consumer);
	if (ret != -ENODEV)
		return ret;

	/* If no external regulator was supplied use the internal VREF */
	st->use_internal_vref = true;
	ret = ad5064_set_config(st, AD5064_CONFIG_INT_VREF_ENABLE);
	if (ret)
		dev_err(dev, "Failed to enable internal vref: %d\n", ret);

	return ret;
}

static int ad5064_probe(struct device *dev, enum ad5064_type type,
			const char *name, ad5064_write_func write)
{
	struct iio_dev *indio_dev;
	struct ad5064_state *st;
	unsigned int midscale;
	unsigned int i;
	int ret;

	indio_dev = devm_iio_device_alloc(dev, sizeof(*st));
	if (indio_dev == NULL)
		return  -ENOMEM;

	st = iio_priv(indio_dev);
	mutex_init(&st->lock);
	dev_set_drvdata(dev, indio_dev);

	st->chip_info = &ad5064_chip_info_tbl[type];
	st->dev = dev;
	st->write = write;

	ret = ad5064_request_vref(st, dev);
	if (ret)
		return ret;

	if (!st->use_internal_vref) {
		ret = regulator_bulk_enable(ad5064_num_vref(st), st->vref_reg);
		if (ret)
			return ret;
	}

	indio_dev->name = name;
	indio_dev->info = &ad5064_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = st->chip_info->channels;
	indio_dev->num_channels = st->chip_info->num_channels;

	midscale = (1 << indio_dev->channels[0].scan_type.realbits) /  2;

	for (i = 0; i < st->chip_info->num_channels; ++i) {
		st->pwr_down_mode[i] = AD5064_LDAC_PWRDN_1K;
		st->dac_cache[i] = midscale;
	}

	ret = iio_device_register(indio_dev);
	if (ret)
		goto error_disable_reg;

	return 0;

error_disable_reg:
	if (!st->use_internal_vref)
		regulator_bulk_disable(ad5064_num_vref(st), st->vref_reg);

	return ret;
}

static int ad5064_remove(struct device *dev)
{
	struct iio_dev *indio_dev = dev_get_drvdata(dev);
	struct ad5064_state *st = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);

	if (!st->use_internal_vref)
		regulator_bulk_disable(ad5064_num_vref(st), st->vref_reg);

	return 0;
}

#if IS_ENABLED(CONFIG_SPI_MASTER)

static int ad5064_spi_write(struct ad5064_state *st, unsigned int cmd,
	unsigned int addr, unsigned int val)
{
	struct spi_device *spi = to_spi_device(st->dev);

	st->data.spi = cpu_to_be32(AD5064_CMD(cmd) | AD5064_ADDR(addr) | val);
	return spi_write(spi, &st->data.spi, sizeof(st->data.spi));
}

static int ad5064_spi_probe(struct spi_device *spi)
{
	const struct spi_device_id *id = spi_get_device_id(spi);

	return ad5064_probe(&spi->dev, id->driver_data, id->name,
				ad5064_spi_write);
}

static int ad5064_spi_remove(struct spi_device *spi)
{
	return ad5064_remove(&spi->dev);
}

static const struct spi_device_id ad5064_spi_ids[] = {
	{"ad5024", ID_AD5024},
	{"ad5025", ID_AD5025},
	{"ad5044", ID_AD5044},
	{"ad5045", ID_AD5045},
	{"ad5064", ID_AD5064},
	{"ad5064-1", ID_AD5064_1},
	{"ad5065", ID_AD5065},
	{"ad5628-1", ID_AD5628_1},
	{"ad5628-2", ID_AD5628_2},
	{"ad5648-1", ID_AD5648_1},
	{"ad5648-2", ID_AD5648_2},
	{"ad5666-1", ID_AD5666_1},
	{"ad5666-2", ID_AD5666_2},
	{"ad5668-1", ID_AD5668_1},
	{"ad5668-2", ID_AD5668_2},
	{"ad5668-3", ID_AD5668_2}, /* similar enough to ad5668-2 */
	{}
};
MODULE_DEVICE_TABLE(spi, ad5064_spi_ids);

static struct spi_driver ad5064_spi_driver = {
	.driver = {
		   .name = "ad5064",
	},
	.probe = ad5064_spi_probe,
	.remove = ad5064_spi_remove,
	.id_table = ad5064_spi_ids,
};

static int __init ad5064_spi_register_driver(void)
{
	return spi_register_driver(&ad5064_spi_driver);
}

static void ad5064_spi_unregister_driver(void)
{
	spi_unregister_driver(&ad5064_spi_driver);
}

#else

static inline int ad5064_spi_register_driver(void) { return 0; }
static inline void ad5064_spi_unregister_driver(void) { }

#endif

#if IS_ENABLED(CONFIG_I2C)

static int ad5064_i2c_write(struct ad5064_state *st, unsigned int cmd,
	unsigned int addr, unsigned int val)
{
	struct i2c_client *i2c = to_i2c_client(st->dev);
	unsigned int cmd_shift;
	int ret;

	switch (st->chip_info->regmap_type) {
	case AD5064_REGMAP_ADI2:
		cmd_shift = 3;
		break;
	default:
		cmd_shift = 4;
		break;
	}

	st->data.i2c[0] = (cmd << cmd_shift) | addr;
	put_unaligned_be16(val, &st->data.i2c[1]);

	ret = i2c_master_send(i2c, st->data.i2c, 3);
	if (ret < 0)
		return ret;

	return 0;
}

static int ad5064_i2c_probe(struct i2c_client *i2c,
	const struct i2c_device_id *id)
{
	return ad5064_probe(&i2c->dev, id->driver_data, id->name,
						ad5064_i2c_write);
}

static int ad5064_i2c_remove(struct i2c_client *i2c)
{
	return ad5064_remove(&i2c->dev);
}

static const struct i2c_device_id ad5064_i2c_ids[] = {
	{"ad5625", ID_AD5625 },
	{"ad5625r-1v25", ID_AD5625R_1V25 },
	{"ad5625r-2v5", ID_AD5625R_2V5 },
	{"ad5627", ID_AD5627 },
	{"ad5627r-1v25", ID_AD5627R_1V25 },
	{"ad5627r-2v5", ID_AD5627R_2V5 },
	{"ad5629-1", ID_AD5629_1},
	{"ad5629-2", ID_AD5629_2},
	{"ad5629-3", ID_AD5629_2}, /* similar enough to ad5629-2 */
	{"ad5645r-1v25", ID_AD5645R_1V25 },
	{"ad5645r-2v5", ID_AD5645R_2V5 },
	{"ad5665", ID_AD5665 },
	{"ad5665r-1v25", ID_AD5665R_1V25 },
	{"ad5665r-2v5", ID_AD5665R_2V5 },
	{"ad5667", ID_AD5667 },
	{"ad5667r-1v25", ID_AD5667R_1V25 },
	{"ad5667r-2v5", ID_AD5667R_2V5 },
	{"ad5669-1", ID_AD5669_1},
	{"ad5669-2", ID_AD5669_2},
	{"ad5669-3", ID_AD5669_2}, /* similar enough to ad5669-2 */
	{"ltc2606", ID_LTC2606},
	{"ltc2607", ID_LTC2607},
	{"ltc2609", ID_LTC2609},
	{"ltc2616", ID_LTC2616},
	{"ltc2617", ID_LTC2617},
	{"ltc2619", ID_LTC2619},
	{"ltc2626", ID_LTC2626},
	{"ltc2627", ID_LTC2627},
	{"ltc2629", ID_LTC2629},
	{"ltc2631-l12", ID_LTC2631_L12},
	{"ltc2631-h12", ID_LTC2631_H12},
	{"ltc2631-l10", ID_LTC2631_L10},
	{"ltc2631-h10", ID_LTC2631_H10},
	{"ltc2631-l8", ID_LTC2631_L8},
	{"ltc2631-h8", ID_LTC2631_H8},
	{"ltc2633-l12", ID_LTC2633_L12},
	{"ltc2633-h12", ID_LTC2633_H12},
	{"ltc2633-l10", ID_LTC2633_L10},
	{"ltc2633-h10", ID_LTC2633_H10},
	{"ltc2633-l8", ID_LTC2633_L8},
	{"ltc2633-h8", ID_LTC2633_H8},
	{"ltc2635-l12", ID_LTC2635_L12},
	{"ltc2635-h12", ID_LTC2635_H12},
	{"ltc2635-l10", ID_LTC2635_L10},
	{"ltc2635-h10", ID_LTC2635_H10},
	{"ltc2635-l8", ID_LTC2635_L8},
	{"ltc2635-h8", ID_LTC2635_H8},
	{}
};
MODULE_DEVICE_TABLE(i2c, ad5064_i2c_ids);

static struct i2c_driver ad5064_i2c_driver = {
	.driver = {
		   .name = "ad5064",
	},
	.probe = ad5064_i2c_probe,
	.remove = ad5064_i2c_remove,
	.id_table = ad5064_i2c_ids,
};

static int __init ad5064_i2c_register_driver(void)
{
	return i2c_add_driver(&ad5064_i2c_driver);
}

static void __exit ad5064_i2c_unregister_driver(void)
{
	i2c_del_driver(&ad5064_i2c_driver);
}

#else

static inline int ad5064_i2c_register_driver(void) { return 0; }
static inline void ad5064_i2c_unregister_driver(void) { }

#endif

static int __init ad5064_init(void)
{
	int ret;

	ret = ad5064_spi_register_driver();
	if (ret)
		return ret;

	ret = ad5064_i2c_register_driver();
	if (ret) {
		ad5064_spi_unregister_driver();
		return ret;
	}

	return 0;
}
module_init(ad5064_init);

static void __exit ad5064_exit(void)
{
	ad5064_i2c_unregister_driver();
	ad5064_spi_unregister_driver();
}
module_exit(ad5064_exit);

MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("Analog Devices AD5024 and similar multi-channel DACs");
MODULE_LICENSE("GPL v2");
