// SPDX-License-Identifier: GPL-2.0-only
 /*
  * iio/adc/max1363.c
  * Copyright (C) 2008-2010 Jonathan Cameron
  *
  * based on linux/drivers/i2c/chips/max123x
  * Copyright (C) 2002-2004 Stefan Eletzhofer
  *
  * based on linux/drivers/acron/char/pcf8583.c
  * Copyright (C) 2000 Russell King
  *
  * Driver for max1363 and similar chips.
  */

#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/events.h>
#include <linux/iio/buffer.h>
#include <linux/iio/driver.h>
#include <linux/iio/kfifo_buf.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

#define MAX1363_SETUP_BYTE(a) ((a) | 0x80)

/* There is a fair bit more defined here than currently
 * used, but the intention is to support everything these
 * chips do in the long run */

/* see data sheets */
/* max1363 and max1236, max1237, max1238, max1239 */
#define MAX1363_SETUP_AIN3_IS_AIN3_REF_IS_VDD	0x00
#define MAX1363_SETUP_AIN3_IS_REF_EXT_TO_REF	0x20
#define MAX1363_SETUP_AIN3_IS_AIN3_REF_IS_INT	0x40
#define MAX1363_SETUP_AIN3_IS_REF_REF_IS_INT	0x60
#define MAX1363_SETUP_POWER_UP_INT_REF		0x10
#define MAX1363_SETUP_POWER_DOWN_INT_REF	0x00

/* think about including max11600 etc - more settings */
#define MAX1363_SETUP_EXT_CLOCK			0x08
#define MAX1363_SETUP_INT_CLOCK			0x00
#define MAX1363_SETUP_UNIPOLAR			0x00
#define MAX1363_SETUP_BIPOLAR			0x04
#define MAX1363_SETUP_RESET			0x00
#define MAX1363_SETUP_NORESET			0x02
/* max1363 only - though don't care on others.
 * For now monitor modes are not implemented as the relevant
 * line is not connected on my test board.
 * The definitions are here as I intend to add this soon.
 */
#define MAX1363_SETUP_MONITOR_SETUP		0x01

/* Specific to the max1363 */
#define MAX1363_MON_RESET_CHAN(a) (1 << ((a) + 4))
#define MAX1363_MON_INT_ENABLE			0x01

/* defined for readability reasons */
/* All chips */
#define MAX1363_CONFIG_BYTE(a) ((a))

#define MAX1363_CONFIG_SE			0x01
#define MAX1363_CONFIG_DE			0x00
#define MAX1363_CONFIG_SCAN_TO_CS		0x00
#define MAX1363_CONFIG_SCAN_SINGLE_8		0x20
#define MAX1363_CONFIG_SCAN_MONITOR_MODE	0x40
#define MAX1363_CONFIG_SCAN_SINGLE_1		0x60
/* max123{6-9} only */
#define MAX1236_SCAN_MID_TO_CHANNEL		0x40

/* max1363 only - merely part of channel selects or don't care for others */
#define MAX1363_CONFIG_EN_MON_MODE_READ 0x18

#define MAX1363_CHANNEL_SEL(a) ((a) << 1)

/* max1363 strictly 0x06 - but doesn't matter */
#define MAX1363_CHANNEL_SEL_MASK		0x1E
#define MAX1363_SCAN_MASK			0x60
#define MAX1363_SE_DE_MASK			0x01

#define MAX1363_MAX_CHANNELS 25
/**
 * struct max1363_mode - scan mode information
 * @conf:	The corresponding value of the configuration register
 * @modemask:	Bit mask corresponding to channels enabled in this mode
 */
struct max1363_mode {
	int8_t		conf;
	DECLARE_BITMAP(modemask, MAX1363_MAX_CHANNELS);
};

/* This must be maintained along side the max1363_mode_table in max1363_core */
enum max1363_modes {
	/* Single read of a single channel */
	_s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7, _s8, _s9, _s10, _s11,
	/* Differential single read */
	d0m1, d2m3, d4m5, d6m7, d8m9, d10m11,
	d1m0, d3m2, d5m4, d7m6, d9m8, d11m10,
	/* Scan to channel and mid to channel where overlapping */
	s0to1, s0to2, s2to3, s0to3, s0to4, s0to5, s0to6,
	s6to7, s0to7, s6to8, s0to8, s6to9,
	s0to9, s6to10, s0to10, s6to11, s0to11,
	/* Differential scan to channel and mid to channel where overlapping */
	d0m1to2m3, d0m1to4m5, d0m1to6m7, d6m7to8m9,
	d0m1to8m9, d6m7to10m11, d0m1to10m11, d1m0to3m2,
	d1m0to5m4, d1m0to7m6, d7m6to9m8, d1m0to9m8,
	d7m6to11m10, d1m0to11m10,
};

/**
 * struct max1363_chip_info - chip specifc information
 * @info:		iio core function callbacks structure
 * @channels:		channel specification
 * @num_channels:       number of channels
 * @mode_list:		array of available scan modes
 * @default_mode:	the scan mode in which the chip starts up
 * @int_vref_mv:	the internal reference voltage
 * @num_modes:		number of modes
 * @bits:		accuracy of the adc in bits
 */
struct max1363_chip_info {
	const struct iio_info		*info;
	const struct iio_chan_spec	*channels;
	int				num_channels;
	const enum max1363_modes	*mode_list;
	enum max1363_modes		default_mode;
	u16				int_vref_mv;
	u8				num_modes;
	u8				bits;
};

/**
 * struct max1363_state - driver instance specific data
 * @client:		i2c_client
 * @setupbyte:		cache of current device setup byte
 * @configbyte:		cache of current device config byte
 * @chip_info:		chip model specific constants, available modes, etc.
 * @current_mode:	the scan mode of this chip
 * @requestedmask:	a valid requested set of channels
 * @reg:		supply regulator
 * @lock:		lock to ensure state is consistent
 * @monitor_on:		whether monitor mode is enabled
 * @monitor_speed:	parameter corresponding to device monitor speed setting
 * @mask_high:		bitmask for enabled high thresholds
 * @mask_low:		bitmask for enabled low thresholds
 * @thresh_high:	high threshold values
 * @thresh_low:		low threshold values
 * @vref:		Reference voltage regulator
 * @vref_uv:		Actual (external or internal) reference voltage
 * @send:		function used to send data to the chip
 * @recv:		function used to receive data from the chip
 */
struct max1363_state {
	struct i2c_client		*client;
	u8				setupbyte;
	u8				configbyte;
	const struct max1363_chip_info	*chip_info;
	const struct max1363_mode	*current_mode;
	u32				requestedmask;
	struct regulator		*reg;
	struct mutex			lock;

	/* Using monitor modes and buffer at the same time is
	   currently not supported */
	bool				monitor_on;
	unsigned int			monitor_speed:3;
	u8				mask_high;
	u8				mask_low;
	/* 4x unipolar first then the fours bipolar ones */
	s16				thresh_high[8];
	s16				thresh_low[8];
	struct regulator		*vref;
	u32				vref_uv;
	int				(*send)(const struct i2c_client *client,
						const char *buf, int count);
	int				(*recv)(const struct i2c_client *client,
						char *buf, int count);
};

#define MAX1363_MODE_SINGLE(_num, _mask) {				\
		.conf = MAX1363_CHANNEL_SEL(_num)			\
			| MAX1363_CONFIG_SCAN_SINGLE_1			\
			| MAX1363_CONFIG_SE,				\
			.modemask[0] = _mask,				\
			}

#define MAX1363_MODE_SCAN_TO_CHANNEL(_num, _mask) {			\
		.conf = MAX1363_CHANNEL_SEL(_num)			\
			| MAX1363_CONFIG_SCAN_TO_CS			\
			| MAX1363_CONFIG_SE,				\
			.modemask[0] = _mask,				\
			}

/* note not available for max1363 hence naming */
#define MAX1236_MODE_SCAN_MID_TO_CHANNEL(_mid, _num, _mask) {		\
		.conf = MAX1363_CHANNEL_SEL(_num)			\
			| MAX1236_SCAN_MID_TO_CHANNEL			\
			| MAX1363_CONFIG_SE,				\
			.modemask[0] = _mask				\
}

#define MAX1363_MODE_DIFF_SINGLE(_nump, _numm, _mask) {			\
		.conf = MAX1363_CHANNEL_SEL(_nump)			\
			| MAX1363_CONFIG_SCAN_SINGLE_1			\
			| MAX1363_CONFIG_DE,				\
			.modemask[0] = _mask				\
			}

/* Can't think how to automate naming so specify for now */
#define MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(_num, _numvals, _mask) {	\
		.conf = MAX1363_CHANNEL_SEL(_num)			\
			| MAX1363_CONFIG_SCAN_TO_CS			\
			| MAX1363_CONFIG_DE,				\
			.modemask[0] = _mask				\
			}

/* note only available for max1363 hence naming */
#define MAX1236_MODE_DIFF_SCAN_MID_TO_CHANNEL(_num, _numvals, _mask) {	\
		.conf = MAX1363_CHANNEL_SEL(_num)			\
			| MAX1236_SCAN_MID_TO_CHANNEL			\
			| MAX1363_CONFIG_SE,				\
			.modemask[0] = _mask				\
}

static const struct max1363_mode max1363_mode_table[] = {
	/* All of the single channel options first */
	MAX1363_MODE_SINGLE(0, 1 << 0),
	MAX1363_MODE_SINGLE(1, 1 << 1),
	MAX1363_MODE_SINGLE(2, 1 << 2),
	MAX1363_MODE_SINGLE(3, 1 << 3),
	MAX1363_MODE_SINGLE(4, 1 << 4),
	MAX1363_MODE_SINGLE(5, 1 << 5),
	MAX1363_MODE_SINGLE(6, 1 << 6),
	MAX1363_MODE_SINGLE(7, 1 << 7),
	MAX1363_MODE_SINGLE(8, 1 << 8),
	MAX1363_MODE_SINGLE(9, 1 << 9),
	MAX1363_MODE_SINGLE(10, 1 << 10),
	MAX1363_MODE_SINGLE(11, 1 << 11),

	MAX1363_MODE_DIFF_SINGLE(0, 1, 1 << 12),
	MAX1363_MODE_DIFF_SINGLE(2, 3, 1 << 13),
	MAX1363_MODE_DIFF_SINGLE(4, 5, 1 << 14),
	MAX1363_MODE_DIFF_SINGLE(6, 7, 1 << 15),
	MAX1363_MODE_DIFF_SINGLE(8, 9, 1 << 16),
	MAX1363_MODE_DIFF_SINGLE(10, 11, 1 << 17),
	MAX1363_MODE_DIFF_SINGLE(1, 0, 1 << 18),
	MAX1363_MODE_DIFF_SINGLE(3, 2, 1 << 19),
	MAX1363_MODE_DIFF_SINGLE(5, 4, 1 << 20),
	MAX1363_MODE_DIFF_SINGLE(7, 6, 1 << 21),
	MAX1363_MODE_DIFF_SINGLE(9, 8, 1 << 22),
	MAX1363_MODE_DIFF_SINGLE(11, 10, 1 << 23),

	/* The multichannel scans next */
	MAX1363_MODE_SCAN_TO_CHANNEL(1, 0x003),
	MAX1363_MODE_SCAN_TO_CHANNEL(2, 0x007),
	MAX1236_MODE_SCAN_MID_TO_CHANNEL(2, 3, 0x00C),
	MAX1363_MODE_SCAN_TO_CHANNEL(3, 0x00F),
	MAX1363_MODE_SCAN_TO_CHANNEL(4, 0x01F),
	MAX1363_MODE_SCAN_TO_CHANNEL(5, 0x03F),
	MAX1363_MODE_SCAN_TO_CHANNEL(6, 0x07F),
	MAX1236_MODE_SCAN_MID_TO_CHANNEL(6, 7, 0x0C0),
	MAX1363_MODE_SCAN_TO_CHANNEL(7, 0x0FF),
	MAX1236_MODE_SCAN_MID_TO_CHANNEL(6, 8, 0x1C0),
	MAX1363_MODE_SCAN_TO_CHANNEL(8, 0x1FF),
	MAX1236_MODE_SCAN_MID_TO_CHANNEL(6, 9, 0x3C0),
	MAX1363_MODE_SCAN_TO_CHANNEL(9, 0x3FF),
	MAX1236_MODE_SCAN_MID_TO_CHANNEL(6, 10, 0x7C0),
	MAX1363_MODE_SCAN_TO_CHANNEL(10, 0x7FF),
	MAX1236_MODE_SCAN_MID_TO_CHANNEL(6, 11, 0xFC0),
	MAX1363_MODE_SCAN_TO_CHANNEL(11, 0xFFF),

	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(2, 2, 0x003000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(4, 3, 0x007000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(6, 4, 0x00F000),
	MAX1236_MODE_DIFF_SCAN_MID_TO_CHANNEL(8, 2, 0x018000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(8, 5, 0x01F000),
	MAX1236_MODE_DIFF_SCAN_MID_TO_CHANNEL(10, 3, 0x038000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(10, 6, 0x3F000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(3, 2, 0x0C0000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(5, 3, 0x1C0000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(7, 4, 0x3C0000),
	MAX1236_MODE_DIFF_SCAN_MID_TO_CHANNEL(9, 2, 0x600000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(9, 5, 0x7C0000),
	MAX1236_MODE_DIFF_SCAN_MID_TO_CHANNEL(11, 3, 0xE00000),
	MAX1363_MODE_DIFF_SCAN_TO_CHANNEL(11, 6, 0xFC0000),
};

static const struct max1363_mode
*max1363_match_mode(const unsigned long *mask,
	const struct max1363_chip_info *ci)
{
	int i;
	if (mask)
		for (i = 0; i < ci->num_modes; i++)
			if (bitmap_subset(mask,
					  max1363_mode_table[ci->mode_list[i]].
					  modemask,
					  MAX1363_MAX_CHANNELS))
				return &max1363_mode_table[ci->mode_list[i]];
	return NULL;
}

static int max1363_smbus_send(const struct i2c_client *client, const char *buf,
		int count)
{
	int i, err;

	for (i = err = 0; err == 0 && i < count; ++i)
		err = i2c_smbus_write_byte(client, buf[i]);

	return err ? err : count;
}

static int max1363_smbus_recv(const struct i2c_client *client, char *buf,
		int count)
{
	int i, ret;

	for (i = 0; i < count; ++i) {
		ret = i2c_smbus_read_byte(client);
		if (ret < 0)
			return ret;
		buf[i] = ret;
	}

	return count;
}

static int max1363_write_basic_config(struct max1363_state *st)
{
	u8 tx_buf[2] = { st->setupbyte, st->configbyte };

	return st->send(st->client, tx_buf, 2);
}

static int max1363_set_scan_mode(struct max1363_state *st)
{
	st->configbyte &= ~(MAX1363_CHANNEL_SEL_MASK
			    | MAX1363_SCAN_MASK
			    | MAX1363_SE_DE_MASK);
	st->configbyte |= st->current_mode->conf;

	return max1363_write_basic_config(st);
}

static int max1363_read_single_chan(struct iio_dev *indio_dev,
				    struct iio_chan_spec const *chan,
				    int *val,
				    long m)
{
	int ret = 0;
	s32 data;
	u8 rxbuf[2];
	struct max1363_state *st = iio_priv(indio_dev);
	struct i2c_client *client = st->client;

	ret = iio_device_claim_direct_mode(indio_dev);
	if (ret)
		return ret;
	mutex_lock(&st->lock);

	/*
	 * If monitor mode is enabled, the method for reading a single
	 * channel will have to be rather different and has not yet
	 * been implemented.
	 *
	 * Also, cannot read directly if buffered capture enabled.
	 */
	if (st->monitor_on) {
		ret = -EBUSY;
		goto error_ret;
	}

	/* Check to see if current scan mode is correct */
	if (st->current_mode != &max1363_mode_table[chan->address]) {
		/* Update scan mode if needed */
		st->current_mode = &max1363_mode_table[chan->address];
		ret = max1363_set_scan_mode(st);
		if (ret < 0)
			goto error_ret;
	}
	if (st->chip_info->bits != 8) {
		/* Get reading */
		data = st->recv(client, rxbuf, 2);
		if (data < 0) {
			ret = data;
			goto error_ret;
		}
		data = (rxbuf[1] | rxbuf[0] << 8) &
		  ((1 << st->chip_info->bits) - 1);
	} else {
		/* Get reading */
		data = st->recv(client, rxbuf, 1);
		if (data < 0) {
			ret = data;
			goto error_ret;
		}
		data = rxbuf[0];
	}
	*val = data;

error_ret:
	mutex_unlock(&st->lock);
	iio_device_release_direct_mode(indio_dev);
	return ret;

}

static int max1363_read_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *chan,
			    int *val,
			    int *val2,
			    long m)
{
	struct max1363_state *st = iio_priv(indio_dev);
	int ret;

	switch (m) {
	case IIO_CHAN_INFO_RAW:
		ret = max1363_read_single_chan(indio_dev, chan, val, m);
		if (ret < 0)
			return ret;
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		*val = st->vref_uv / 1000;
		*val2 = st->chip_info->bits;
		return IIO_VAL_FRACTIONAL_LOG2;
	default:
		return -EINVAL;
	}
	return 0;
}

/* Applies to max1363 */
static const enum max1363_modes max1363_mode_list[] = {
	_s0, _s1, _s2, _s3,
	s0to1, s0to2, s0to3,
	d0m1, d2m3, d1m0, d3m2,
	d0m1to2m3, d1m0to3m2,
};

static const struct iio_event_spec max1363_events[] = {
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_RISING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE) |
			BIT(IIO_EV_INFO_ENABLE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_FALLING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE) |
			BIT(IIO_EV_INFO_ENABLE),
	},
};

#define MAX1363_CHAN_U(num, addr, si, bits, ev_spec, num_ev_spec)	\
	{								\
		.type = IIO_VOLTAGE,					\
		.indexed = 1,						\
		.channel = num,						\
		.address = addr,					\
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),		\
		.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),	\
		.datasheet_name = "AIN"#num,				\
		.scan_type = {						\
			.sign = 'u',					\
			.realbits = bits,				\
			.storagebits = (bits > 8) ? 16 : 8,		\
			.endianness = IIO_BE,				\
		},							\
		.scan_index = si,					\
		.event_spec = ev_spec,					\
		.num_event_specs = num_ev_spec,				\
	}

/* bipolar channel */
#define MAX1363_CHAN_B(num, num2, addr, si, bits, ev_spec, num_ev_spec)	\
	{								\
		.type = IIO_VOLTAGE,					\
		.differential = 1,					\
		.indexed = 1,						\
		.channel = num,						\
		.channel2 = num2,					\
		.address = addr,					\
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),		\
		.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),	\
		.datasheet_name = "AIN"#num"-AIN"#num2,			\
		.scan_type = {						\
			.sign = 's',					\
			.realbits = bits,				\
			.storagebits = (bits > 8) ? 16 : 8,		\
			.endianness = IIO_BE,				\
		},							\
		.scan_index = si,					\
		.event_spec = ev_spec,					\
		.num_event_specs = num_ev_spec,				\
	}

#define MAX1363_4X_CHANS(bits, ev_spec, num_ev_spec) {			\
	MAX1363_CHAN_U(0, _s0, 0, bits, ev_spec, num_ev_spec),		\
	MAX1363_CHAN_U(1, _s1, 1, bits, ev_spec, num_ev_spec),		\
	MAX1363_CHAN_U(2, _s2, 2, bits, ev_spec, num_ev_spec),		\
	MAX1363_CHAN_U(3, _s3, 3, bits, ev_spec, num_ev_spec),		\
	MAX1363_CHAN_B(0, 1, d0m1, 4, bits, ev_spec, num_ev_spec),	\
	MAX1363_CHAN_B(2, 3, d2m3, 5, bits, ev_spec, num_ev_spec),	\
	MAX1363_CHAN_B(1, 0, d1m0, 6, bits, ev_spec, num_ev_spec),	\
	MAX1363_CHAN_B(3, 2, d3m2, 7, bits, ev_spec, num_ev_spec),	\
	IIO_CHAN_SOFT_TIMESTAMP(8)					\
	}

static const struct iio_chan_spec max1036_channels[] =
	MAX1363_4X_CHANS(8, NULL, 0);
static const struct iio_chan_spec max1136_channels[] =
	MAX1363_4X_CHANS(10, NULL, 0);
static const struct iio_chan_spec max1236_channels[] =
	MAX1363_4X_CHANS(12, NULL, 0);
static const struct iio_chan_spec max1361_channels[] =
	MAX1363_4X_CHANS(10, max1363_events, ARRAY_SIZE(max1363_events));
static const struct iio_chan_spec max1363_channels[] =
	MAX1363_4X_CHANS(12, max1363_events, ARRAY_SIZE(max1363_events));

/* Applies to max1236, max1237 */
static const enum max1363_modes max1236_mode_list[] = {
	_s0, _s1, _s2, _s3,
	s0to1, s0to2, s0to3,
	d0m1, d2m3, d1m0, d3m2,
	d0m1to2m3, d1m0to3m2,
	s2to3,
};

/* Applies to max1238, max1239 */
static const enum max1363_modes max1238_mode_list[] = {
	_s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7, _s8, _s9, _s10, _s11,
	s0to1, s0to2, s0to3, s0to4, s0to5, s0to6,
	s0to7, s0to8, s0to9, s0to10, s0to11,
	d0m1, d2m3, d4m5, d6m7, d8m9, d10m11,
	d1m0, d3m2, d5m4, d7m6, d9m8, d11m10,
	d0m1to2m3, d0m1to4m5, d0m1to6m7, d0m1to8m9, d0m1to10m11,
	d1m0to3m2, d1m0to5m4, d1m0to7m6, d1m0to9m8, d1m0to11m10,
	s6to7, s6to8, s6to9, s6to10, s6to11,
	d6m7to8m9, d6m7to10m11, d7m6to9m8, d7m6to11m10,
};

#define MAX1363_12X_CHANS(bits) {				\
	MAX1363_CHAN_U(0, _s0, 0, bits, NULL, 0),		\
	MAX1363_CHAN_U(1, _s1, 1, bits, NULL, 0),		\
	MAX1363_CHAN_U(2, _s2, 2, bits, NULL, 0),		\
	MAX1363_CHAN_U(3, _s3, 3, bits, NULL, 0),		\
	MAX1363_CHAN_U(4, _s4, 4, bits, NULL, 0),		\
	MAX1363_CHAN_U(5, _s5, 5, bits, NULL, 0),		\
	MAX1363_CHAN_U(6, _s6, 6, bits, NULL, 0),		\
	MAX1363_CHAN_U(7, _s7, 7, bits, NULL, 0),		\
	MAX1363_CHAN_U(8, _s8, 8, bits, NULL, 0),		\
	MAX1363_CHAN_U(9, _s9, 9, bits, NULL, 0),		\
	MAX1363_CHAN_U(10, _s10, 10, bits, NULL, 0),		\
	MAX1363_CHAN_U(11, _s11, 11, bits, NULL, 0),		\
	MAX1363_CHAN_B(0, 1, d0m1, 12, bits, NULL, 0),		\
	MAX1363_CHAN_B(2, 3, d2m3, 13, bits, NULL, 0),		\
	MAX1363_CHAN_B(4, 5, d4m5, 14, bits, NULL, 0),		\
	MAX1363_CHAN_B(6, 7, d6m7, 15, bits, NULL, 0),		\
	MAX1363_CHAN_B(8, 9, d8m9, 16, bits, NULL, 0),		\
	MAX1363_CHAN_B(10, 11, d10m11, 17, bits, NULL, 0),	\
	MAX1363_CHAN_B(1, 0, d1m0, 18, bits, NULL, 0),		\
	MAX1363_CHAN_B(3, 2, d3m2, 19, bits, NULL, 0),		\
	MAX1363_CHAN_B(5, 4, d5m4, 20, bits, NULL, 0),		\
	MAX1363_CHAN_B(7, 6, d7m6, 21, bits, NULL, 0),		\
	MAX1363_CHAN_B(9, 8, d9m8, 22, bits, NULL, 0),		\
	MAX1363_CHAN_B(11, 10, d11m10, 23, bits, NULL, 0),	\
	IIO_CHAN_SOFT_TIMESTAMP(24)				\
	}
static const struct iio_chan_spec max1038_channels[] = MAX1363_12X_CHANS(8);
static const struct iio_chan_spec max1138_channels[] = MAX1363_12X_CHANS(10);
static const struct iio_chan_spec max1238_channels[] = MAX1363_12X_CHANS(12);

static const enum max1363_modes max11607_mode_list[] = {
	_s0, _s1, _s2, _s3,
	s0to1, s0to2, s0to3,
	s2to3,
	d0m1, d2m3, d1m0, d3m2,
	d0m1to2m3, d1m0to3m2,
};

static const enum max1363_modes max11608_mode_list[] = {
	_s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7,
	s0to1, s0to2, s0to3, s0to4, s0to5, s0to6, s0to7,
	s6to7,
	d0m1, d2m3, d4m5, d6m7,
	d1m0, d3m2, d5m4, d7m6,
	d0m1to2m3, d0m1to4m5, d0m1to6m7,
	d1m0to3m2, d1m0to5m4, d1m0to7m6,
};

#define MAX1363_8X_CHANS(bits) {			\
	MAX1363_CHAN_U(0, _s0, 0, bits, NULL, 0),	\
	MAX1363_CHAN_U(1, _s1, 1, bits, NULL, 0),	\
	MAX1363_CHAN_U(2, _s2, 2, bits, NULL, 0),	\
	MAX1363_CHAN_U(3, _s3, 3, bits, NULL, 0),	\
	MAX1363_CHAN_U(4, _s4, 4, bits, NULL, 0),	\
	MAX1363_CHAN_U(5, _s5, 5, bits, NULL, 0),	\
	MAX1363_CHAN_U(6, _s6, 6, bits, NULL, 0),	\
	MAX1363_CHAN_U(7, _s7, 7, bits, NULL, 0),	\
	MAX1363_CHAN_B(0, 1, d0m1, 8, bits, NULL, 0),	\
	MAX1363_CHAN_B(2, 3, d2m3, 9, bits, NULL, 0),	\
	MAX1363_CHAN_B(4, 5, d4m5, 10, bits, NULL, 0),	\
	MAX1363_CHAN_B(6, 7, d6m7, 11, bits, NULL, 0),	\
	MAX1363_CHAN_B(1, 0, d1m0, 12, bits, NULL, 0),	\
	MAX1363_CHAN_B(3, 2, d3m2, 13, bits, NULL, 0),	\
	MAX1363_CHAN_B(5, 4, d5m4, 14, bits, NULL, 0),	\
	MAX1363_CHAN_B(7, 6, d7m6, 15, bits, NULL, 0),	\
	IIO_CHAN_SOFT_TIMESTAMP(16)			\
}
static const struct iio_chan_spec max11602_channels[] = MAX1363_8X_CHANS(8);
static const struct iio_chan_spec max11608_channels[] = MAX1363_8X_CHANS(10);
static const struct iio_chan_spec max11614_channels[] = MAX1363_8X_CHANS(12);

static const enum max1363_modes max11644_mode_list[] = {
	_s0, _s1, s0to1, d0m1, d1m0,
};

#define MAX1363_2X_CHANS(bits) {			\
	MAX1363_CHAN_U(0, _s0, 0, bits, NULL, 0),	\
	MAX1363_CHAN_U(1, _s1, 1, bits, NULL, 0),	\
	MAX1363_CHAN_B(0, 1, d0m1, 2, bits, NULL, 0),	\
	MAX1363_CHAN_B(1, 0, d1m0, 3, bits, NULL, 0),	\
	IIO_CHAN_SOFT_TIMESTAMP(4)			\
	}

static const struct iio_chan_spec max11646_channels[] = MAX1363_2X_CHANS(10);
static const struct iio_chan_spec max11644_channels[] = MAX1363_2X_CHANS(12);

enum { max1361,
       max1362,
       max1363,
       max1364,
       max1036,
       max1037,
       max1038,
       max1039,
       max1136,
       max1137,
       max1138,
       max1139,
       max1236,
       max1237,
       max1238,
       max1239,
       max11600,
       max11601,
       max11602,
       max11603,
       max11604,
       max11605,
       max11606,
       max11607,
       max11608,
       max11609,
       max11610,
       max11611,
       max11612,
       max11613,
       max11614,
       max11615,
       max11616,
       max11617,
       max11644,
       max11645,
       max11646,
       max11647
};

static const int max1363_monitor_speeds[] = { 133000, 665000, 33300, 16600,
					      8300, 4200, 2000, 1000 };

static ssize_t max1363_monitor_show_freq(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct max1363_state *st = iio_priv(dev_to_iio_dev(dev));
	return sprintf(buf, "%d\n", max1363_monitor_speeds[st->monitor_speed]);
}

static ssize_t max1363_monitor_store_freq(struct device *dev,
					struct device_attribute *attr,
					const char *buf,
					size_t len)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct max1363_state *st = iio_priv(indio_dev);
	int i, ret;
	unsigned long val;
	bool found = false;

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		return -EINVAL;
	for (i = 0; i < ARRAY_SIZE(max1363_monitor_speeds); i++)
		if (val == max1363_monitor_speeds[i]) {
			found = true;
			break;
		}
	if (!found)
		return -EINVAL;

	mutex_lock(&st->lock);
	st->monitor_speed = i;
	mutex_unlock(&st->lock);

	return 0;
}

static IIO_DEV_ATTR_SAMP_FREQ(S_IRUGO | S_IWUSR,
			max1363_monitor_show_freq,
			max1363_monitor_store_freq);

static IIO_CONST_ATTR(sampling_frequency_available,
		"133000 665000 33300 16600 8300 4200 2000 1000");

static int max1363_read_thresh(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir, enum iio_event_info info, int *val,
	int *val2)
{
	struct max1363_state *st = iio_priv(indio_dev);
	if (dir == IIO_EV_DIR_FALLING)
		*val = st->thresh_low[chan->channel];
	else
		*val = st->thresh_high[chan->channel];
	return IIO_VAL_INT;
}

static int max1363_write_thresh(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir, enum iio_event_info info, int val,
	int val2)
{
	struct max1363_state *st = iio_priv(indio_dev);
	/* make it handle signed correctly as well */
	switch (st->chip_info->bits) {
	case 10:
		if (val > 0x3FF)
			return -EINVAL;
		break;
	case 12:
		if (val > 0xFFF)
			return -EINVAL;
		break;
	}

	switch (dir) {
	case IIO_EV_DIR_FALLING:
		st->thresh_low[chan->channel] = val;
		break;
	case IIO_EV_DIR_RISING:
		st->thresh_high[chan->channel] = val;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const u64 max1363_event_codes[] = {
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 0,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_FALLING),
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 1,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_FALLING),
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 2,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_FALLING),
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 3,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_FALLING),
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 0,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_RISING),
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 1,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_RISING),
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 2,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_RISING),
	IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 3,
			     IIO_EV_TYPE_THRESH, IIO_EV_DIR_RISING),
};

static irqreturn_t max1363_event_handler(int irq, void *private)
{
	struct iio_dev *indio_dev = private;
	struct max1363_state *st = iio_priv(indio_dev);
	s64 timestamp = iio_get_time_ns(indio_dev);
	unsigned long mask, loc;
	u8 rx;
	u8 tx[2] = { st->setupbyte,
		     MAX1363_MON_INT_ENABLE | (st->monitor_speed << 1) | 0xF0 };

	st->recv(st->client, &rx, 1);
	mask = rx;
	for_each_set_bit(loc, &mask, 8)
		iio_push_event(indio_dev, max1363_event_codes[loc], timestamp);
	st->send(st->client, tx, 2);

	return IRQ_HANDLED;
}

static int max1363_read_event_config(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir)
{
	struct max1363_state *st = iio_priv(indio_dev);
	int val;
	int number = chan->channel;

	mutex_lock(&st->lock);
	if (dir == IIO_EV_DIR_FALLING)
		val = (1 << number) & st->mask_low;
	else
		val = (1 << number) & st->mask_high;
	mutex_unlock(&st->lock);

	return val;
}

static int max1363_monitor_mode_update(struct max1363_state *st, int enabled)
{
	u8 *tx_buf;
	int ret, i = 3, j;
	unsigned long numelements;
	int len;
	const long *modemask;

	if (!enabled) {
		/* transition to buffered capture is not currently supported */
		st->setupbyte &= ~MAX1363_SETUP_MONITOR_SETUP;
		st->configbyte &= ~MAX1363_SCAN_MASK;
		st->monitor_on = false;
		return max1363_write_basic_config(st);
	}

	/* Ensure we are in the relevant mode */
	st->setupbyte |= MAX1363_SETUP_MONITOR_SETUP;
	st->configbyte &= ~(MAX1363_CHANNEL_SEL_MASK
			    | MAX1363_SCAN_MASK
			| MAX1363_SE_DE_MASK);
	st->configbyte |= MAX1363_CONFIG_SCAN_MONITOR_MODE;
	if ((st->mask_low | st->mask_high) & 0x0F) {
		st->configbyte |= max1363_mode_table[s0to3].conf;
		modemask = max1363_mode_table[s0to3].modemask;
	} else if ((st->mask_low | st->mask_high) & 0x30) {
		st->configbyte |= max1363_mode_table[d0m1to2m3].conf;
		modemask = max1363_mode_table[d0m1to2m3].modemask;
	} else {
		st->configbyte |= max1363_mode_table[d1m0to3m2].conf;
		modemask = max1363_mode_table[d1m0to3m2].modemask;
	}
	numelements = bitmap_weight(modemask, MAX1363_MAX_CHANNELS);
	len = 3 * numelements + 3;
	tx_buf = kmalloc(len, GFP_KERNEL);
	if (!tx_buf) {
		ret = -ENOMEM;
		goto error_ret;
	}
	tx_buf[0] = st->configbyte;
	tx_buf[1] = st->setupbyte;
	tx_buf[2] = (st->monitor_speed << 1);

	/*
	 * So we need to do yet another bit of nefarious scan mode
	 * setup to match what we need.
	 */
	for (j = 0; j < 8; j++)
		if (test_bit(j, modemask)) {
			/* Establish the mode is in the scan */
			if (st->mask_low & (1 << j)) {
				tx_buf[i] = (st->thresh_low[j] >> 4) & 0xFF;
				tx_buf[i + 1] = (st->thresh_low[j] << 4) & 0xF0;
			} else if (j < 4) {
				tx_buf[i] = 0;
				tx_buf[i + 1] = 0;
			} else {
				tx_buf[i] = 0x80;
				tx_buf[i + 1] = 0;
			}
			if (st->mask_high & (1 << j)) {
				tx_buf[i + 1] |=
					(st->thresh_high[j] >> 8) & 0x0F;
				tx_buf[i + 2] = st->thresh_high[j] & 0xFF;
			} else if (j < 4) {
				tx_buf[i + 1] |= 0x0F;
				tx_buf[i + 2] = 0xFF;
			} else {
				tx_buf[i + 1] |= 0x07;
				tx_buf[i + 2] = 0xFF;
			}
			i += 3;
		}


	ret = st->send(st->client, tx_buf, len);
	if (ret < 0)
		goto error_ret;
	if (ret != len) {
		ret = -EIO;
		goto error_ret;
	}

	/*
	 * Now that we hopefully have sensible thresholds in place it is
	 * time to turn the interrupts on.
	 * It is unclear from the data sheet if this should be necessary
	 * (i.e. whether monitor mode setup is atomic) but it appears to
	 * be in practice.
	 */
	tx_buf[0] = st->setupbyte;
	tx_buf[1] = MAX1363_MON_INT_ENABLE | (st->monitor_speed << 1) | 0xF0;
	ret = st->send(st->client, tx_buf, 2);
	if (ret < 0)
		goto error_ret;
	if (ret != 2) {
		ret = -EIO;
		goto error_ret;
	}
	ret = 0;
	st->monitor_on = true;
error_ret:

	kfree(tx_buf);

	return ret;
}

/*
 * To keep this manageable we always use one of 3 scan modes.
 * Scan 0...3, 0-1,2-3 and 1-0,3-2
 */

static inline int __max1363_check_event_mask(int thismask, int checkmask)
{
	int ret = 0;
	/* Is it unipolar */
	if (thismask < 4) {
		if (checkmask & ~0x0F) {
			ret = -EBUSY;
			goto error_ret;
		}
	} else if (thismask < 6) {
		if (checkmask & ~0x30) {
			ret = -EBUSY;
			goto error_ret;
		}
	} else if (checkmask & ~0xC0)
		ret = -EBUSY;
error_ret:
	return ret;
}

static int max1363_write_event_config(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir, int state)
{
	int ret = 0;
	struct max1363_state *st = iio_priv(indio_dev);
	u16 unifiedmask;
	int number = chan->channel;

	ret = iio_device_claim_direct_mode(indio_dev);
	if (ret)
		return ret;
	mutex_lock(&st->lock);

	unifiedmask = st->mask_low | st->mask_high;
	if (dir == IIO_EV_DIR_FALLING) {

		if (state == 0)
			st->mask_low &= ~(1 << number);
		else {
			ret = __max1363_check_event_mask((1 << number),
							 unifiedmask);
			if (ret)
				goto error_ret;
			st->mask_low |= (1 << number);
		}
	} else {
		if (state == 0)
			st->mask_high &= ~(1 << number);
		else {
			ret = __max1363_check_event_mask((1 << number),
							 unifiedmask);
			if (ret)
				goto error_ret;
			st->mask_high |= (1 << number);
		}
	}

	max1363_monitor_mode_update(st, !!(st->mask_high | st->mask_low));
error_ret:
	mutex_unlock(&st->lock);
	iio_device_release_direct_mode(indio_dev);

	return ret;
}

/*
 * As with scan_elements, only certain sets of these can
 * be combined.
 */
static struct attribute *max1363_event_attributes[] = {
	&iio_dev_attr_sampling_frequency.dev_attr.attr,
	&iio_const_attr_sampling_frequency_available.dev_attr.attr,
	NULL,
};

static const struct attribute_group max1363_event_attribute_group = {
	.attrs = max1363_event_attributes,
};

static int max1363_update_scan_mode(struct iio_dev *indio_dev,
				    const unsigned long *scan_mask)
{
	struct max1363_state *st = iio_priv(indio_dev);

	/*
	 * Need to figure out the current mode based upon the requested
	 * scan mask in iio_dev
	 */
	st->current_mode = max1363_match_mode(scan_mask, st->chip_info);
	if (!st->current_mode)
		return -EINVAL;
	max1363_set_scan_mode(st);
	return 0;
}

static const struct iio_info max1238_info = {
	.read_raw = &max1363_read_raw,
	.update_scan_mode = &max1363_update_scan_mode,
};

static const struct iio_info max1363_info = {
	.read_event_value = &max1363_read_thresh,
	.write_event_value = &max1363_write_thresh,
	.read_event_config = &max1363_read_event_config,
	.write_event_config = &max1363_write_event_config,
	.read_raw = &max1363_read_raw,
	.update_scan_mode = &max1363_update_scan_mode,
	.event_attrs = &max1363_event_attribute_group,
};

/* max1363 and max1368 tested - rest from data sheet */
static const struct max1363_chip_info max1363_chip_info_tbl[] = {
	[max1361] = {
		.bits = 10,
		.int_vref_mv = 2048,
		.mode_list = max1363_mode_list,
		.num_modes = ARRAY_SIZE(max1363_mode_list),
		.default_mode = s0to3,
		.channels = max1361_channels,
		.num_channels = ARRAY_SIZE(max1361_channels),
		.info = &max1363_info,
	},
	[max1362] = {
		.bits = 10,
		.int_vref_mv = 4096,
		.mode_list = max1363_mode_list,
		.num_modes = ARRAY_SIZE(max1363_mode_list),
		.default_mode = s0to3,
		.channels = max1361_channels,
		.num_channels = ARRAY_SIZE(max1361_channels),
		.info = &max1363_info,
	},
	[max1363] = {
		.bits = 12,
		.int_vref_mv = 2048,
		.mode_list = max1363_mode_list,
		.num_modes = ARRAY_SIZE(max1363_mode_list),
		.default_mode = s0to3,
		.channels = max1363_channels,
		.num_channels = ARRAY_SIZE(max1363_channels),
		.info = &max1363_info,
	},
	[max1364] = {
		.bits = 12,
		.int_vref_mv = 4096,
		.mode_list = max1363_mode_list,
		.num_modes = ARRAY_SIZE(max1363_mode_list),
		.default_mode = s0to3,
		.channels = max1363_channels,
		.num_channels = ARRAY_SIZE(max1363_channels),
		.info = &max1363_info,
	},
	[max1036] = {
		.bits = 8,
		.int_vref_mv = 4096,
		.mode_list = max1236_mode_list,
		.num_modes = ARRAY_SIZE(max1236_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1036_channels,
		.num_channels = ARRAY_SIZE(max1036_channels),
	},
	[max1037] = {
		.bits = 8,
		.int_vref_mv = 2048,
		.mode_list = max1236_mode_list,
		.num_modes = ARRAY_SIZE(max1236_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1036_channels,
		.num_channels = ARRAY_SIZE(max1036_channels),
	},
	[max1038] = {
		.bits = 8,
		.int_vref_mv = 4096,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1038_channels,
		.num_channels = ARRAY_SIZE(max1038_channels),
	},
	[max1039] = {
		.bits = 8,
		.int_vref_mv = 2048,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1038_channels,
		.num_channels = ARRAY_SIZE(max1038_channels),
	},
	[max1136] = {
		.bits = 10,
		.int_vref_mv = 4096,
		.mode_list = max1236_mode_list,
		.num_modes = ARRAY_SIZE(max1236_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1136_channels,
		.num_channels = ARRAY_SIZE(max1136_channels),
	},
	[max1137] = {
		.bits = 10,
		.int_vref_mv = 2048,
		.mode_list = max1236_mode_list,
		.num_modes = ARRAY_SIZE(max1236_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1136_channels,
		.num_channels = ARRAY_SIZE(max1136_channels),
	},
	[max1138] = {
		.bits = 10,
		.int_vref_mv = 4096,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1138_channels,
		.num_channels = ARRAY_SIZE(max1138_channels),
	},
	[max1139] = {
		.bits = 10,
		.int_vref_mv = 2048,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1138_channels,
		.num_channels = ARRAY_SIZE(max1138_channels),
	},
	[max1236] = {
		.bits = 12,
		.int_vref_mv = 4096,
		.mode_list = max1236_mode_list,
		.num_modes = ARRAY_SIZE(max1236_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1236_channels,
		.num_channels = ARRAY_SIZE(max1236_channels),
	},
	[max1237] = {
		.bits = 12,
		.int_vref_mv = 2048,
		.mode_list = max1236_mode_list,
		.num_modes = ARRAY_SIZE(max1236_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1236_channels,
		.num_channels = ARRAY_SIZE(max1236_channels),
	},
	[max1238] = {
		.bits = 12,
		.int_vref_mv = 4096,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1238_channels,
		.num_channels = ARRAY_SIZE(max1238_channels),
	},
	[max1239] = {
		.bits = 12,
		.int_vref_mv = 2048,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1238_channels,
		.num_channels = ARRAY_SIZE(max1238_channels),
	},
	[max11600] = {
		.bits = 8,
		.int_vref_mv = 4096,
		.mode_list = max11607_mode_list,
		.num_modes = ARRAY_SIZE(max11607_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1036_channels,
		.num_channels = ARRAY_SIZE(max1036_channels),
	},
	[max11601] = {
		.bits = 8,
		.int_vref_mv = 2048,
		.mode_list = max11607_mode_list,
		.num_modes = ARRAY_SIZE(max11607_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1036_channels,
		.num_channels = ARRAY_SIZE(max1036_channels),
	},
	[max11602] = {
		.bits = 8,
		.int_vref_mv = 4096,
		.mode_list = max11608_mode_list,
		.num_modes = ARRAY_SIZE(max11608_mode_list),
		.default_mode = s0to7,
		.info = &max1238_info,
		.channels = max11602_channels,
		.num_channels = ARRAY_SIZE(max11602_channels),
	},
	[max11603] = {
		.bits = 8,
		.int_vref_mv = 2048,
		.mode_list = max11608_mode_list,
		.num_modes = ARRAY_SIZE(max11608_mode_list),
		.default_mode = s0to7,
		.info = &max1238_info,
		.channels = max11602_channels,
		.num_channels = ARRAY_SIZE(max11602_channels),
	},
	[max11604] = {
		.bits = 8,
		.int_vref_mv = 4096,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1038_channels,
		.num_channels = ARRAY_SIZE(max1038_channels),
	},
	[max11605] = {
		.bits = 8,
		.int_vref_mv = 2048,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1038_channels,
		.num_channels = ARRAY_SIZE(max1038_channels),
	},
	[max11606] = {
		.bits = 10,
		.int_vref_mv = 4096,
		.mode_list = max11607_mode_list,
		.num_modes = ARRAY_SIZE(max11607_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1136_channels,
		.num_channels = ARRAY_SIZE(max1136_channels),
	},
	[max11607] = {
		.bits = 10,
		.int_vref_mv = 2048,
		.mode_list = max11607_mode_list,
		.num_modes = ARRAY_SIZE(max11607_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1136_channels,
		.num_channels = ARRAY_SIZE(max1136_channels),
	},
	[max11608] = {
		.bits = 10,
		.int_vref_mv = 4096,
		.mode_list = max11608_mode_list,
		.num_modes = ARRAY_SIZE(max11608_mode_list),
		.default_mode = s0to7,
		.info = &max1238_info,
		.channels = max11608_channels,
		.num_channels = ARRAY_SIZE(max11608_channels),
	},
	[max11609] = {
		.bits = 10,
		.int_vref_mv = 2048,
		.mode_list = max11608_mode_list,
		.num_modes = ARRAY_SIZE(max11608_mode_list),
		.default_mode = s0to7,
		.info = &max1238_info,
		.channels = max11608_channels,
		.num_channels = ARRAY_SIZE(max11608_channels),
	},
	[max11610] = {
		.bits = 10,
		.int_vref_mv = 4096,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1138_channels,
		.num_channels = ARRAY_SIZE(max1138_channels),
	},
	[max11611] = {
		.bits = 10,
		.int_vref_mv = 2048,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1138_channels,
		.num_channels = ARRAY_SIZE(max1138_channels),
	},
	[max11612] = {
		.bits = 12,
		.int_vref_mv = 4096,
		.mode_list = max11607_mode_list,
		.num_modes = ARRAY_SIZE(max11607_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1363_channels,
		.num_channels = ARRAY_SIZE(max1363_channels),
	},
	[max11613] = {
		.bits = 12,
		.int_vref_mv = 2048,
		.mode_list = max11607_mode_list,
		.num_modes = ARRAY_SIZE(max11607_mode_list),
		.default_mode = s0to3,
		.info = &max1238_info,
		.channels = max1363_channels,
		.num_channels = ARRAY_SIZE(max1363_channels),
	},
	[max11614] = {
		.bits = 12,
		.int_vref_mv = 4096,
		.mode_list = max11608_mode_list,
		.num_modes = ARRAY_SIZE(max11608_mode_list),
		.default_mode = s0to7,
		.info = &max1238_info,
		.channels = max11614_channels,
		.num_channels = ARRAY_SIZE(max11614_channels),
	},
	[max11615] = {
		.bits = 12,
		.int_vref_mv = 2048,
		.mode_list = max11608_mode_list,
		.num_modes = ARRAY_SIZE(max11608_mode_list),
		.default_mode = s0to7,
		.info = &max1238_info,
		.channels = max11614_channels,
		.num_channels = ARRAY_SIZE(max11614_channels),
	},
	[max11616] = {
		.bits = 12,
		.int_vref_mv = 4096,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1238_channels,
		.num_channels = ARRAY_SIZE(max1238_channels),
	},
	[max11617] = {
		.bits = 12,
		.int_vref_mv = 2048,
		.mode_list = max1238_mode_list,
		.num_modes = ARRAY_SIZE(max1238_mode_list),
		.default_mode = s0to11,
		.info = &max1238_info,
		.channels = max1238_channels,
		.num_channels = ARRAY_SIZE(max1238_channels),
	},
	[max11644] = {
		.bits = 12,
		.int_vref_mv = 4096,
		.mode_list = max11644_mode_list,
		.num_modes = ARRAY_SIZE(max11644_mode_list),
		.default_mode = s0to1,
		.info = &max1238_info,
		.channels = max11644_channels,
		.num_channels = ARRAY_SIZE(max11644_channels),
	},
	[max11645] = {
		.bits = 12,
		.int_vref_mv = 2048,
		.mode_list = max11644_mode_list,
		.num_modes = ARRAY_SIZE(max11644_mode_list),
		.default_mode = s0to1,
		.info = &max1238_info,
		.channels = max11644_channels,
		.num_channels = ARRAY_SIZE(max11644_channels),
	},
	[max11646] = {
		.bits = 10,
		.int_vref_mv = 4096,
		.mode_list = max11644_mode_list,
		.num_modes = ARRAY_SIZE(max11644_mode_list),
		.default_mode = s0to1,
		.info = &max1238_info,
		.channels = max11646_channels,
		.num_channels = ARRAY_SIZE(max11646_channels),
	},
	[max11647] = {
		.bits = 10,
		.int_vref_mv = 2048,
		.mode_list = max11644_mode_list,
		.num_modes = ARRAY_SIZE(max11644_mode_list),
		.default_mode = s0to1,
		.info = &max1238_info,
		.channels = max11646_channels,
		.num_channels = ARRAY_SIZE(max11646_channels),
	},
};

static int max1363_initial_setup(struct max1363_state *st)
{
	st->setupbyte = MAX1363_SETUP_INT_CLOCK
		| MAX1363_SETUP_UNIPOLAR
		| MAX1363_SETUP_NORESET;

	if (st->vref)
		st->setupbyte |= MAX1363_SETUP_AIN3_IS_REF_EXT_TO_REF;
	else
		st->setupbyte |= MAX1363_SETUP_POWER_UP_INT_REF
		  | MAX1363_SETUP_AIN3_IS_AIN3_REF_IS_INT;

	/* Set scan mode writes the config anyway so wait until then */
	st->setupbyte = MAX1363_SETUP_BYTE(st->setupbyte);
	st->current_mode = &max1363_mode_table[st->chip_info->default_mode];
	st->configbyte = MAX1363_CONFIG_BYTE(st->configbyte);

	return max1363_set_scan_mode(st);
}

static int max1363_alloc_scan_masks(struct iio_dev *indio_dev)
{
	struct max1363_state *st = iio_priv(indio_dev);
	unsigned long *masks;
	int i;

	masks = devm_kzalloc(&indio_dev->dev,
			array3_size(BITS_TO_LONGS(MAX1363_MAX_CHANNELS),
				    sizeof(long),
				    st->chip_info->num_modes + 1),
			GFP_KERNEL);
	if (!masks)
		return -ENOMEM;

	for (i = 0; i < st->chip_info->num_modes; i++)
		bitmap_copy(masks + BITS_TO_LONGS(MAX1363_MAX_CHANNELS)*i,
			    max1363_mode_table[st->chip_info->mode_list[i]]
			    .modemask, MAX1363_MAX_CHANNELS);

	indio_dev->available_scan_masks = masks;

	return 0;
}

static irqreturn_t max1363_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct max1363_state *st = iio_priv(indio_dev);
	__u8 *rxbuf;
	int b_sent;
	size_t d_size;
	unsigned long numvals = bitmap_weight(st->current_mode->modemask,
					      MAX1363_MAX_CHANNELS);

	/* Ensure the timestamp is 8 byte aligned */
	if (st->chip_info->bits != 8)
		d_size = numvals*2;
	else
		d_size = numvals;
	if (indio_dev->scan_timestamp) {
		d_size += sizeof(s64);
		if (d_size % sizeof(s64))
			d_size += sizeof(s64) - (d_size % sizeof(s64));
	}
	/* Monitor mode prevents reading. Whilst not currently implemented
	 * might as well have this test in here in the meantime as it does
	 * no harm.
	 */
	if (numvals == 0)
		goto done;

	rxbuf = kmalloc(d_size,	GFP_KERNEL);
	if (rxbuf == NULL)
		goto done;
	if (st->chip_info->bits != 8)
		b_sent = st->recv(st->client, rxbuf, numvals * 2);
	else
		b_sent = st->recv(st->client, rxbuf, numvals);
	if (b_sent < 0)
		goto done_free;

	iio_push_to_buffers_with_timestamp(indio_dev, rxbuf,
					   iio_get_time_ns(indio_dev));

done_free:
	kfree(rxbuf);
done:
	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}

#define MAX1363_COMPATIBLE(of_compatible, cfg) {		\
			.compatible = of_compatible,		\
			.data = &max1363_chip_info_tbl[cfg],	\
}

static const struct of_device_id max1363_of_match[] = {
	MAX1363_COMPATIBLE("maxim,max1361", max1361),
	MAX1363_COMPATIBLE("maxim,max1362", max1362),
	MAX1363_COMPATIBLE("maxim,max1363", max1363),
	MAX1363_COMPATIBLE("maxim,max1364", max1364),
	MAX1363_COMPATIBLE("maxim,max1036", max1036),
	MAX1363_COMPATIBLE("maxim,max1037", max1037),
	MAX1363_COMPATIBLE("maxim,max1038", max1038),
	MAX1363_COMPATIBLE("maxim,max1039", max1039),
	MAX1363_COMPATIBLE("maxim,max1136", max1136),
	MAX1363_COMPATIBLE("maxim,max1137", max1137),
	MAX1363_COMPATIBLE("maxim,max1138", max1138),
	MAX1363_COMPATIBLE("maxim,max1139", max1139),
	MAX1363_COMPATIBLE("maxim,max1236", max1236),
	MAX1363_COMPATIBLE("maxim,max1237", max1237),
	MAX1363_COMPATIBLE("maxim,max1238", max1238),
	MAX1363_COMPATIBLE("maxim,max1239", max1239),
	MAX1363_COMPATIBLE("maxim,max11600", max11600),
	MAX1363_COMPATIBLE("maxim,max11601", max11601),
	MAX1363_COMPATIBLE("maxim,max11602", max11602),
	MAX1363_COMPATIBLE("maxim,max11603", max11603),
	MAX1363_COMPATIBLE("maxim,max11604", max11604),
	MAX1363_COMPATIBLE("maxim,max11605", max11605),
	MAX1363_COMPATIBLE("maxim,max11606", max11606),
	MAX1363_COMPATIBLE("maxim,max11607", max11607),
	MAX1363_COMPATIBLE("maxim,max11608", max11608),
	MAX1363_COMPATIBLE("maxim,max11609", max11609),
	MAX1363_COMPATIBLE("maxim,max11610", max11610),
	MAX1363_COMPATIBLE("maxim,max11611", max11611),
	MAX1363_COMPATIBLE("maxim,max11612", max11612),
	MAX1363_COMPATIBLE("maxim,max11613", max11613),
	MAX1363_COMPATIBLE("maxim,max11614", max11614),
	MAX1363_COMPATIBLE("maxim,max11615", max11615),
	MAX1363_COMPATIBLE("maxim,max11616", max11616),
	MAX1363_COMPATIBLE("maxim,max11617", max11617),
	MAX1363_COMPATIBLE("maxim,max11644", max11644),
	MAX1363_COMPATIBLE("maxim,max11645", max11645),
	MAX1363_COMPATIBLE("maxim,max11646", max11646),
	MAX1363_COMPATIBLE("maxim,max11647", max11647),
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, max1363_of_match);

static int max1363_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	int ret;
	struct max1363_state *st;
	struct iio_dev *indio_dev;
	struct regulator *vref;

	indio_dev = devm_iio_device_alloc(&client->dev,
					  sizeof(struct max1363_state));
	if (!indio_dev)
		return -ENOMEM;

	ret = iio_map_array_register(indio_dev, client->dev.platform_data);
	if (ret < 0)
		return ret;

	st = iio_priv(indio_dev);

	mutex_init(&st->lock);
	st->reg = devm_regulator_get(&client->dev, "vcc");
	if (IS_ERR(st->reg)) {
		ret = PTR_ERR(st->reg);
		goto error_unregister_map;
	}

	ret = regulator_enable(st->reg);
	if (ret)
		goto error_unregister_map;

	/* this is only used for device removal purposes */
	i2c_set_clientdata(client, indio_dev);

	st->chip_info = device_get_match_data(&client->dev);
	if (!st->chip_info)
		st->chip_info = &max1363_chip_info_tbl[id->driver_data];
	st->client = client;

	st->vref_uv = st->chip_info->int_vref_mv * 1000;
	vref = devm_regulator_get_optional(&client->dev, "vref");
	if (!IS_ERR(vref)) {
		int vref_uv;

		ret = regulator_enable(vref);
		if (ret)
			goto error_disable_reg;
		st->vref = vref;
		vref_uv = regulator_get_voltage(vref);
		if (vref_uv <= 0) {
			ret = -EINVAL;
			goto error_disable_reg;
		}
		st->vref_uv = vref_uv;
	}

	if (i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		st->send = i2c_master_send;
		st->recv = i2c_master_recv;
	} else if (i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)
			&& st->chip_info->bits == 8) {
		st->send = max1363_smbus_send;
		st->recv = max1363_smbus_recv;
	} else {
		ret = -EOPNOTSUPP;
		goto error_disable_reg;
	}

	ret = max1363_alloc_scan_masks(indio_dev);
	if (ret)
		goto error_disable_reg;

	indio_dev->name = id->name;
	indio_dev->channels = st->chip_info->channels;
	indio_dev->num_channels = st->chip_info->num_channels;
	indio_dev->info = st->chip_info->info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	ret = max1363_initial_setup(st);
	if (ret < 0)
		goto error_disable_reg;

	ret = iio_triggered_buffer_setup(indio_dev, NULL,
		&max1363_trigger_handler, NULL);
	if (ret)
		goto error_disable_reg;

	if (client->irq) {
		ret = devm_request_threaded_irq(&client->dev, st->client->irq,
					   NULL,
					   &max1363_event_handler,
					   IRQF_TRIGGER_RISING | IRQF_ONESHOT,
					   "max1363_event",
					   indio_dev);

		if (ret)
			goto error_uninit_buffer;
	}

	ret = iio_device_register(indio_dev);
	if (ret < 0)
		goto error_uninit_buffer;

	return 0;

error_uninit_buffer:
	iio_triggered_buffer_cleanup(indio_dev);
error_disable_reg:
	if (st->vref)
		regulator_disable(st->vref);
	regulator_disable(st->reg);
error_unregister_map:
	iio_map_array_unregister(indio_dev);
	return ret;
}

static int max1363_remove(struct i2c_client *client)
{
	struct iio_dev *indio_dev = i2c_get_clientdata(client);
	struct max1363_state *st = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	iio_triggered_buffer_cleanup(indio_dev);
	if (st->vref)
		regulator_disable(st->vref);
	regulator_disable(st->reg);
	iio_map_array_unregister(indio_dev);

	return 0;
}

static const struct i2c_device_id max1363_id[] = {
	{ "max1361", max1361 },
	{ "max1362", max1362 },
	{ "max1363", max1363 },
	{ "max1364", max1364 },
	{ "max1036", max1036 },
	{ "max1037", max1037 },
	{ "max1038", max1038 },
	{ "max1039", max1039 },
	{ "max1136", max1136 },
	{ "max1137", max1137 },
	{ "max1138", max1138 },
	{ "max1139", max1139 },
	{ "max1236", max1236 },
	{ "max1237", max1237 },
	{ "max1238", max1238 },
	{ "max1239", max1239 },
	{ "max11600", max11600 },
	{ "max11601", max11601 },
	{ "max11602", max11602 },
	{ "max11603", max11603 },
	{ "max11604", max11604 },
	{ "max11605", max11605 },
	{ "max11606", max11606 },
	{ "max11607", max11607 },
	{ "max11608", max11608 },
	{ "max11609", max11609 },
	{ "max11610", max11610 },
	{ "max11611", max11611 },
	{ "max11612", max11612 },
	{ "max11613", max11613 },
	{ "max11614", max11614 },
	{ "max11615", max11615 },
	{ "max11616", max11616 },
	{ "max11617", max11617 },
	{ "max11644", max11644 },
	{ "max11645", max11645 },
	{ "max11646", max11646 },
	{ "max11647", max11647 },
	{}
};

MODULE_DEVICE_TABLE(i2c, max1363_id);

static struct i2c_driver max1363_driver = {
	.driver = {
		.name = "max1363",
		.of_match_table = max1363_of_match,
	},
	.probe = max1363_probe,
	.remove = max1363_remove,
	.id_table = max1363_id,
};
module_i2c_driver(max1363_driver);

MODULE_AUTHOR("Jonathan Cameron <jic23@kernel.org>");
MODULE_DESCRIPTION("Maxim 1363 ADC");
MODULE_LICENSE("GPL v2");
