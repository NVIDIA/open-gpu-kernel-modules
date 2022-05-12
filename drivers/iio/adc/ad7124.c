// SPDX-License-Identifier: GPL-2.0+
/*
 * AD7124 SPI ADC driver
 *
 * Copyright 2018 Analog Devices Inc.
 */
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/kfifo.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>
#include <linux/spi/spi.h>

#include <linux/iio/iio.h>
#include <linux/iio/adc/ad_sigma_delta.h>
#include <linux/iio/sysfs.h>

/* AD7124 registers */
#define AD7124_COMMS			0x00
#define AD7124_STATUS			0x00
#define AD7124_ADC_CONTROL		0x01
#define AD7124_DATA			0x02
#define AD7124_IO_CONTROL_1		0x03
#define AD7124_IO_CONTROL_2		0x04
#define AD7124_ID			0x05
#define AD7124_ERROR			0x06
#define AD7124_ERROR_EN		0x07
#define AD7124_MCLK_COUNT		0x08
#define AD7124_CHANNEL(x)		(0x09 + (x))
#define AD7124_CONFIG(x)		(0x19 + (x))
#define AD7124_FILTER(x)		(0x21 + (x))
#define AD7124_OFFSET(x)		(0x29 + (x))
#define AD7124_GAIN(x)			(0x31 + (x))

/* AD7124_STATUS */
#define AD7124_STATUS_POR_FLAG_MSK	BIT(4)

/* AD7124_ADC_CONTROL */
#define AD7124_ADC_CTRL_REF_EN_MSK	BIT(8)
#define AD7124_ADC_CTRL_REF_EN(x)	FIELD_PREP(AD7124_ADC_CTRL_REF_EN_MSK, x)
#define AD7124_ADC_CTRL_PWR_MSK	GENMASK(7, 6)
#define AD7124_ADC_CTRL_PWR(x)		FIELD_PREP(AD7124_ADC_CTRL_PWR_MSK, x)
#define AD7124_ADC_CTRL_MODE_MSK	GENMASK(5, 2)
#define AD7124_ADC_CTRL_MODE(x)	FIELD_PREP(AD7124_ADC_CTRL_MODE_MSK, x)

/* AD7124 ID */
#define AD7124_DEVICE_ID_MSK		GENMASK(7, 4)
#define AD7124_DEVICE_ID_GET(x)		FIELD_GET(AD7124_DEVICE_ID_MSK, x)
#define AD7124_SILICON_REV_MSK		GENMASK(3, 0)
#define AD7124_SILICON_REV_GET(x)	FIELD_GET(AD7124_SILICON_REV_MSK, x)

#define CHIPID_AD7124_4			0x0
#define CHIPID_AD7124_8			0x1

/* AD7124_CHANNEL_X */
#define AD7124_CHANNEL_EN_MSK		BIT(15)
#define AD7124_CHANNEL_EN(x)		FIELD_PREP(AD7124_CHANNEL_EN_MSK, x)
#define AD7124_CHANNEL_SETUP_MSK	GENMASK(14, 12)
#define AD7124_CHANNEL_SETUP(x)	FIELD_PREP(AD7124_CHANNEL_SETUP_MSK, x)
#define AD7124_CHANNEL_AINP_MSK	GENMASK(9, 5)
#define AD7124_CHANNEL_AINP(x)		FIELD_PREP(AD7124_CHANNEL_AINP_MSK, x)
#define AD7124_CHANNEL_AINM_MSK	GENMASK(4, 0)
#define AD7124_CHANNEL_AINM(x)		FIELD_PREP(AD7124_CHANNEL_AINM_MSK, x)

/* AD7124_CONFIG_X */
#define AD7124_CONFIG_BIPOLAR_MSK	BIT(11)
#define AD7124_CONFIG_BIPOLAR(x)	FIELD_PREP(AD7124_CONFIG_BIPOLAR_MSK, x)
#define AD7124_CONFIG_REF_SEL_MSK	GENMASK(4, 3)
#define AD7124_CONFIG_REF_SEL(x)	FIELD_PREP(AD7124_CONFIG_REF_SEL_MSK, x)
#define AD7124_CONFIG_PGA_MSK		GENMASK(2, 0)
#define AD7124_CONFIG_PGA(x)		FIELD_PREP(AD7124_CONFIG_PGA_MSK, x)
#define AD7124_CONFIG_IN_BUFF_MSK	GENMASK(7, 6)
#define AD7124_CONFIG_IN_BUFF(x)	FIELD_PREP(AD7124_CONFIG_IN_BUFF_MSK, x)

/* AD7124_FILTER_X */
#define AD7124_FILTER_FS_MSK		GENMASK(10, 0)
#define AD7124_FILTER_FS(x)		FIELD_PREP(AD7124_FILTER_FS_MSK, x)
#define AD7124_FILTER_TYPE_MSK		GENMASK(23, 21)
#define AD7124_FILTER_TYPE_SEL(x)	FIELD_PREP(AD7124_FILTER_TYPE_MSK, x)

#define AD7124_SINC3_FILTER 2
#define AD7124_SINC4_FILTER 0

#define AD7124_CONF_ADDR_OFFSET	20
#define AD7124_MAX_CONFIGS	8
#define AD7124_MAX_CHANNELS	16

enum ad7124_ids {
	ID_AD7124_4,
	ID_AD7124_8,
};

enum ad7124_ref_sel {
	AD7124_REFIN1,
	AD7124_REFIN2,
	AD7124_INT_REF,
	AD7124_AVDD_REF,
};

enum ad7124_power_mode {
	AD7124_LOW_POWER,
	AD7124_MID_POWER,
	AD7124_FULL_POWER,
};

static const unsigned int ad7124_gain[8] = {
	1, 2, 4, 8, 16, 32, 64, 128
};

static const unsigned int ad7124_reg_size[] = {
	1, 2, 3, 3, 2, 1, 3, 3, 1, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3
};

static const int ad7124_master_clk_freq_hz[3] = {
	[AD7124_LOW_POWER] = 76800,
	[AD7124_MID_POWER] = 153600,
	[AD7124_FULL_POWER] = 614400,
};

static const char * const ad7124_ref_names[] = {
	[AD7124_REFIN1] = "refin1",
	[AD7124_REFIN2] = "refin2",
	[AD7124_INT_REF] = "int",
	[AD7124_AVDD_REF] = "avdd",
};

struct ad7124_chip_info {
	const char *name;
	unsigned int chip_id;
	unsigned int num_inputs;
};

struct ad7124_channel_config {
	bool live;
	unsigned int cfg_slot;
	enum ad7124_ref_sel refsel;
	bool bipolar;
	bool buf_positive;
	bool buf_negative;
	unsigned int vref_mv;
	unsigned int pga_bits;
	unsigned int odr;
	unsigned int odr_sel_bits;
	unsigned int filter_type;
};

struct ad7124_channel {
	unsigned int nr;
	struct ad7124_channel_config cfg;
	unsigned int ain;
	unsigned int slot;
};

struct ad7124_state {
	const struct ad7124_chip_info *chip_info;
	struct ad_sigma_delta sd;
	struct ad7124_channel *channels;
	struct regulator *vref[4];
	struct clk *mclk;
	unsigned int adc_control;
	unsigned int num_channels;
	struct mutex cfgs_lock; /* lock for configs access */
	unsigned long cfg_slots_status; /* bitmap with slot status (1 means it is used) */
	DECLARE_KFIFO(live_cfgs_fifo, struct ad7124_channel_config *, AD7124_MAX_CONFIGS);
};

static const struct iio_chan_spec ad7124_channel_template = {
	.type = IIO_VOLTAGE,
	.indexed = 1,
	.differential = 1,
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
		BIT(IIO_CHAN_INFO_SCALE) |
		BIT(IIO_CHAN_INFO_OFFSET) |
		BIT(IIO_CHAN_INFO_SAMP_FREQ) |
		BIT(IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY),
	.scan_type = {
		.sign = 'u',
		.realbits = 24,
		.storagebits = 32,
		.shift = 8,
		.endianness = IIO_BE,
	},
};

static struct ad7124_chip_info ad7124_chip_info_tbl[] = {
	[ID_AD7124_4] = {
		.name = "ad7124-4",
		.chip_id = CHIPID_AD7124_4,
		.num_inputs = 8,
	},
	[ID_AD7124_8] = {
		.name = "ad7124-8",
		.chip_id = CHIPID_AD7124_8,
		.num_inputs = 16,
	},
};

static int ad7124_find_closest_match(const int *array,
				     unsigned int size, int val)
{
	int i, idx;
	unsigned int diff_new, diff_old;

	diff_old = U32_MAX;
	idx = 0;

	for (i = 0; i < size; i++) {
		diff_new = abs(val - array[i]);
		if (diff_new < diff_old) {
			diff_old = diff_new;
			idx = i;
		}
	}

	return idx;
}

static int ad7124_spi_write_mask(struct ad7124_state *st,
				 unsigned int addr,
				 unsigned long mask,
				 unsigned int val,
				 unsigned int bytes)
{
	unsigned int readval;
	int ret;

	ret = ad_sd_read_reg(&st->sd, addr, bytes, &readval);
	if (ret < 0)
		return ret;

	readval &= ~mask;
	readval |= val;

	return ad_sd_write_reg(&st->sd, addr, bytes, readval);
}

static int ad7124_set_mode(struct ad_sigma_delta *sd,
			   enum ad_sigma_delta_mode mode)
{
	struct ad7124_state *st = container_of(sd, struct ad7124_state, sd);

	st->adc_control &= ~AD7124_ADC_CTRL_MODE_MSK;
	st->adc_control |= AD7124_ADC_CTRL_MODE(mode);

	return ad_sd_write_reg(&st->sd, AD7124_ADC_CONTROL, 2, st->adc_control);
}

static void ad7124_set_channel_odr(struct ad7124_state *st, unsigned int channel, unsigned int odr)
{
	unsigned int fclk, odr_sel_bits;

	fclk = clk_get_rate(st->mclk);
	/*
	 * FS[10:0] = fCLK / (fADC x 32) where:
	 * fADC is the output data rate
	 * fCLK is the master clock frequency
	 * FS[10:0] are the bits in the filter register
	 * FS[10:0] can have a value from 1 to 2047
	 */
	odr_sel_bits = DIV_ROUND_CLOSEST(fclk, odr * 32);
	if (odr_sel_bits < 1)
		odr_sel_bits = 1;
	else if (odr_sel_bits > 2047)
		odr_sel_bits = 2047;

	if (odr_sel_bits != st->channels[channel].cfg.odr_sel_bits)
		st->channels[channel].cfg.live = false;

	/* fADC = fCLK / (FS[10:0] x 32) */
	st->channels[channel].cfg.odr = DIV_ROUND_CLOSEST(fclk, odr_sel_bits * 32);
	st->channels[channel].cfg.odr_sel_bits = odr_sel_bits;
}

static int ad7124_get_3db_filter_freq(struct ad7124_state *st,
				      unsigned int channel)
{
	unsigned int fadc;

	fadc = st->channels[channel].cfg.odr;

	switch (st->channels[channel].cfg.filter_type) {
	case AD7124_SINC3_FILTER:
		return DIV_ROUND_CLOSEST(fadc * 230, 1000);
	case AD7124_SINC4_FILTER:
		return DIV_ROUND_CLOSEST(fadc * 262, 1000);
	default:
		return -EINVAL;
	}
}

static void ad7124_set_3db_filter_freq(struct ad7124_state *st, unsigned int channel,
				       unsigned int freq)
{
	unsigned int sinc4_3db_odr;
	unsigned int sinc3_3db_odr;
	unsigned int new_filter;
	unsigned int new_odr;

	sinc4_3db_odr = DIV_ROUND_CLOSEST(freq * 1000, 230);
	sinc3_3db_odr = DIV_ROUND_CLOSEST(freq * 1000, 262);

	if (sinc4_3db_odr > sinc3_3db_odr) {
		new_filter = AD7124_SINC3_FILTER;
		new_odr = sinc4_3db_odr;
	} else {
		new_filter = AD7124_SINC4_FILTER;
		new_odr = sinc3_3db_odr;
	}

	if (new_odr != st->channels[channel].cfg.odr)
		st->channels[channel].cfg.live = false;

	st->channels[channel].cfg.filter_type = new_filter;
	st->channels[channel].cfg.odr = new_odr;
}

static struct ad7124_channel_config *ad7124_find_similar_live_cfg(struct ad7124_state *st,
								  struct ad7124_channel_config *cfg)
{
	struct ad7124_channel_config *cfg_aux;
	ptrdiff_t cmp_size;
	int i;

	cmp_size = (u8 *)&cfg->live - (u8 *)cfg;
	for (i = 0; i < st->num_channels; i++) {
		cfg_aux = &st->channels[i].cfg;

		if (cfg_aux->live && !memcmp(cfg, cfg_aux, cmp_size))
			return cfg_aux;
	}

	return NULL;
}

static int ad7124_find_free_config_slot(struct ad7124_state *st)
{
	unsigned int free_cfg_slot;

	free_cfg_slot = find_next_zero_bit(&st->cfg_slots_status, AD7124_MAX_CONFIGS, 0);
	if (free_cfg_slot == AD7124_MAX_CONFIGS)
		return -1;

	return free_cfg_slot;
}

static int ad7124_init_config_vref(struct ad7124_state *st, struct ad7124_channel_config *cfg)
{
	unsigned int refsel = cfg->refsel;

	switch (refsel) {
	case AD7124_REFIN1:
	case AD7124_REFIN2:
	case AD7124_AVDD_REF:
		if (IS_ERR(st->vref[refsel])) {
			dev_err(&st->sd.spi->dev,
				"Error, trying to use external voltage reference without a %s regulator.\n",
				ad7124_ref_names[refsel]);
			return PTR_ERR(st->vref[refsel]);
		}
		cfg->vref_mv = regulator_get_voltage(st->vref[refsel]);
		/* Conversion from uV to mV */
		cfg->vref_mv /= 1000;
		return 0;
	case AD7124_INT_REF:
		cfg->vref_mv = 2500;
		st->adc_control &= ~AD7124_ADC_CTRL_REF_EN_MSK;
		st->adc_control |= AD7124_ADC_CTRL_REF_EN(1);
		return ad_sd_write_reg(&st->sd, AD7124_ADC_CONTROL,
				      2, st->adc_control);
	default:
		dev_err(&st->sd.spi->dev, "Invalid reference %d\n", refsel);
		return -EINVAL;
	}
}

static int ad7124_write_config(struct ad7124_state *st, struct ad7124_channel_config *cfg,
			       unsigned int cfg_slot)
{
	unsigned int tmp;
	unsigned int val;
	int ret;

	cfg->cfg_slot = cfg_slot;

	tmp = (cfg->buf_positive << 1) + cfg->buf_negative;
	val = AD7124_CONFIG_BIPOLAR(cfg->bipolar) | AD7124_CONFIG_REF_SEL(cfg->refsel) |
	      AD7124_CONFIG_IN_BUFF(tmp);
	ret = ad_sd_write_reg(&st->sd, AD7124_CONFIG(cfg->cfg_slot), 2, val);
	if (ret < 0)
		return ret;

	tmp = AD7124_FILTER_TYPE_SEL(cfg->filter_type);
	ret = ad7124_spi_write_mask(st, AD7124_FILTER(cfg->cfg_slot), AD7124_FILTER_TYPE_MSK,
				    tmp, 3);
	if (ret < 0)
		return ret;

	ret = ad7124_spi_write_mask(st, AD7124_FILTER(cfg->cfg_slot), AD7124_FILTER_FS_MSK,
				    AD7124_FILTER_FS(cfg->odr_sel_bits), 3);
	if (ret < 0)
		return ret;

	return ad7124_spi_write_mask(st, AD7124_CONFIG(cfg->cfg_slot), AD7124_CONFIG_PGA_MSK,
				     AD7124_CONFIG_PGA(cfg->pga_bits), 2);
}

static struct ad7124_channel_config *ad7124_pop_config(struct ad7124_state *st)
{
	struct ad7124_channel_config *lru_cfg;
	struct ad7124_channel_config *cfg;
	int ret;
	int i;

	/*
	 * Pop least recently used config from the fifo
	 * in order to make room for the new one
	 */
	ret = kfifo_get(&st->live_cfgs_fifo, &lru_cfg);
	if (ret <= 0)
		return NULL;

	lru_cfg->live = false;

	/* mark slot as free */
	assign_bit(lru_cfg->cfg_slot, &st->cfg_slots_status, 0);

	/* invalidate all other configs that pointed to this one */
	for (i = 0; i < st->num_channels; i++) {
		cfg = &st->channels[i].cfg;

		if (cfg->cfg_slot == lru_cfg->cfg_slot)
			cfg->live = false;
	}

	return lru_cfg;
}

static int ad7124_push_config(struct ad7124_state *st, struct ad7124_channel_config *cfg)
{
	struct ad7124_channel_config *lru_cfg;
	int free_cfg_slot;

	free_cfg_slot = ad7124_find_free_config_slot(st);
	if (free_cfg_slot >= 0) {
		/* push the new config in configs queue */
		kfifo_put(&st->live_cfgs_fifo, cfg);
	} else {
		/* pop one config to make room for the new one */
		lru_cfg = ad7124_pop_config(st);
		if (!lru_cfg)
			return -EINVAL;

		/* push the new config in configs queue */
		free_cfg_slot = lru_cfg->cfg_slot;
		kfifo_put(&st->live_cfgs_fifo, cfg);
	}

	/* mark slot as used */
	assign_bit(free_cfg_slot, &st->cfg_slots_status, 1);

	return ad7124_write_config(st, cfg, free_cfg_slot);
}

static int ad7124_enable_channel(struct ad7124_state *st, struct ad7124_channel *ch)
{
	ch->cfg.live = true;
	return ad_sd_write_reg(&st->sd, AD7124_CHANNEL(ch->nr), 2, ch->ain |
			      AD7124_CHANNEL_SETUP(ch->cfg.cfg_slot) | AD7124_CHANNEL_EN(1));
}

static int ad7124_prepare_read(struct ad7124_state *st, int address)
{
	struct ad7124_channel_config *cfg = &st->channels[address].cfg;
	struct ad7124_channel_config *live_cfg;

	/*
	 * Before doing any reads assign the channel a configuration.
	 * Check if channel's config is on the device
	 */
	if (!cfg->live) {
		/* check if config matches another one */
		live_cfg = ad7124_find_similar_live_cfg(st, cfg);
		if (!live_cfg)
			ad7124_push_config(st, cfg);
		else
			cfg->cfg_slot = live_cfg->cfg_slot;
	}

	/* point channel to the config slot and enable */
	return ad7124_enable_channel(st, &st->channels[address]);
}

static int ad7124_set_channel(struct ad_sigma_delta *sd, unsigned int channel)
{
	struct ad7124_state *st = container_of(sd, struct ad7124_state, sd);
	int ret;

	mutex_lock(&st->cfgs_lock);
	ret = ad7124_prepare_read(st, channel);
	mutex_unlock(&st->cfgs_lock);

	return ret;
}

static const struct ad_sigma_delta_info ad7124_sigma_delta_info = {
	.set_channel = ad7124_set_channel,
	.set_mode = ad7124_set_mode,
	.has_registers = true,
	.addr_shift = 0,
	.read_mask = BIT(6),
	.data_reg = AD7124_DATA,
	.irq_flags = IRQF_TRIGGER_FALLING
};

static int ad7124_read_raw(struct iio_dev *indio_dev,
			   struct iio_chan_spec const *chan,
			   int *val, int *val2, long info)
{
	struct ad7124_state *st = iio_priv(indio_dev);
	int idx, ret;

	switch (info) {
	case IIO_CHAN_INFO_RAW:
		ret = ad_sigma_delta_single_conversion(indio_dev, chan, val);
		if (ret < 0)
			return ret;

		/* After the conversion is performed, disable the channel */
		ret = ad_sd_write_reg(&st->sd, AD7124_CHANNEL(chan->address), 2,
				      st->channels[chan->address].ain | AD7124_CHANNEL_EN(0));
		if (ret < 0)
			return ret;

		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		mutex_lock(&st->cfgs_lock);

		idx = st->channels[chan->address].cfg.pga_bits;
		*val = st->channels[chan->address].cfg.vref_mv;
		if (st->channels[chan->address].cfg.bipolar)
			*val2 = chan->scan_type.realbits - 1 + idx;
		else
			*val2 = chan->scan_type.realbits + idx;

		mutex_unlock(&st->cfgs_lock);
		return IIO_VAL_FRACTIONAL_LOG2;
	case IIO_CHAN_INFO_OFFSET:
		mutex_lock(&st->cfgs_lock);
		if (st->channels[chan->address].cfg.bipolar)
			*val = -(1 << (chan->scan_type.realbits - 1));
		else
			*val = 0;

		mutex_unlock(&st->cfgs_lock);
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SAMP_FREQ:
		mutex_lock(&st->cfgs_lock);
		*val = st->channels[chan->address].cfg.odr;
		mutex_unlock(&st->cfgs_lock);

		return IIO_VAL_INT;
	case IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY:
		mutex_lock(&st->cfgs_lock);
		*val = ad7124_get_3db_filter_freq(st, chan->scan_index);
		mutex_unlock(&st->cfgs_lock);

		return IIO_VAL_INT;
	default:
		return -EINVAL;
	}
}

static int ad7124_write_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *chan,
			    int val, int val2, long info)
{
	struct ad7124_state *st = iio_priv(indio_dev);
	unsigned int res, gain, full_scale, vref;
	int ret = 0;

	mutex_lock(&st->cfgs_lock);

	switch (info) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		if (val2 != 0) {
			ret = -EINVAL;
			break;
		}

		ad7124_set_channel_odr(st, chan->address, val);
		break;
	case IIO_CHAN_INFO_SCALE:
		if (val != 0) {
			ret = -EINVAL;
			break;
		}

		if (st->channels[chan->address].cfg.bipolar)
			full_scale = 1 << (chan->scan_type.realbits - 1);
		else
			full_scale = 1 << chan->scan_type.realbits;

		vref = st->channels[chan->address].cfg.vref_mv * 1000000LL;
		res = DIV_ROUND_CLOSEST(vref, full_scale);
		gain = DIV_ROUND_CLOSEST(res, val2);
		res = ad7124_find_closest_match(ad7124_gain, ARRAY_SIZE(ad7124_gain), gain);

		if (st->channels[chan->address].cfg.pga_bits != res)
			st->channels[chan->address].cfg.live = false;

		st->channels[chan->address].cfg.pga_bits = res;
		break;
	case IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY:
		if (val2 != 0) {
			ret = -EINVAL;
			break;
		}

		ad7124_set_3db_filter_freq(st, chan->address, val);
		break;
	default:
		ret =  -EINVAL;
	}

	mutex_unlock(&st->cfgs_lock);
	return ret;
}

static int ad7124_reg_access(struct iio_dev *indio_dev,
			     unsigned int reg,
			     unsigned int writeval,
			     unsigned int *readval)
{
	struct ad7124_state *st = iio_priv(indio_dev);
	int ret;

	if (reg >= ARRAY_SIZE(ad7124_reg_size))
		return -EINVAL;

	if (readval)
		ret = ad_sd_read_reg(&st->sd, reg, ad7124_reg_size[reg],
				     readval);
	else
		ret = ad_sd_write_reg(&st->sd, reg, ad7124_reg_size[reg],
				      writeval);

	return ret;
}

static IIO_CONST_ATTR(in_voltage_scale_available,
	"0.000001164 0.000002328 0.000004656 0.000009313 0.000018626 0.000037252 0.000074505 0.000149011 0.000298023");

static struct attribute *ad7124_attributes[] = {
	&iio_const_attr_in_voltage_scale_available.dev_attr.attr,
	NULL,
};

static const struct attribute_group ad7124_attrs_group = {
	.attrs = ad7124_attributes,
};

static const struct iio_info ad7124_info = {
	.read_raw = ad7124_read_raw,
	.write_raw = ad7124_write_raw,
	.debugfs_reg_access = &ad7124_reg_access,
	.validate_trigger = ad_sd_validate_trigger,
	.attrs = &ad7124_attrs_group,
};

static int ad7124_soft_reset(struct ad7124_state *st)
{
	unsigned int readval, timeout;
	int ret;

	ret = ad_sd_reset(&st->sd, 64);
	if (ret < 0)
		return ret;

	timeout = 100;
	do {
		ret = ad_sd_read_reg(&st->sd, AD7124_STATUS, 1, &readval);
		if (ret < 0)
			return ret;

		if (!(readval & AD7124_STATUS_POR_FLAG_MSK))
			return 0;

		/* The AD7124 requires typically 2ms to power up and settle */
		usleep_range(100, 2000);
	} while (--timeout);

	dev_err(&st->sd.spi->dev, "Soft reset failed\n");

	return -EIO;
}

static int ad7124_check_chip_id(struct ad7124_state *st)
{
	unsigned int readval, chip_id, silicon_rev;
	int ret;

	ret = ad_sd_read_reg(&st->sd, AD7124_ID, 1, &readval);
	if (ret < 0)
		return ret;

	chip_id = AD7124_DEVICE_ID_GET(readval);
	silicon_rev = AD7124_SILICON_REV_GET(readval);

	if (chip_id != st->chip_info->chip_id) {
		dev_err(&st->sd.spi->dev,
			"Chip ID mismatch: expected %u, got %u\n",
			st->chip_info->chip_id, chip_id);
		return -ENODEV;
	}

	if (silicon_rev == 0) {
		dev_err(&st->sd.spi->dev,
			"Silicon revision empty. Chip may not be present\n");
		return -ENODEV;
	}

	return 0;
}

static int ad7124_of_parse_channel_config(struct iio_dev *indio_dev,
					  struct device_node *np)
{
	struct ad7124_state *st = iio_priv(indio_dev);
	struct ad7124_channel_config *cfg;
	struct ad7124_channel *channels;
	struct device_node *child;
	struct iio_chan_spec *chan;
	unsigned int ain[2], channel = 0, tmp;
	int ret;

	st->num_channels = of_get_available_child_count(np);
	if (!st->num_channels) {
		dev_err(indio_dev->dev.parent, "no channel children\n");
		return -ENODEV;
	}

	chan = devm_kcalloc(indio_dev->dev.parent, st->num_channels,
			    sizeof(*chan), GFP_KERNEL);
	if (!chan)
		return -ENOMEM;

	channels = devm_kcalloc(indio_dev->dev.parent, st->num_channels, sizeof(*channels),
				GFP_KERNEL);
	if (!channels)
		return -ENOMEM;

	indio_dev->channels = chan;
	indio_dev->num_channels = st->num_channels;
	st->channels = channels;

	for_each_available_child_of_node(np, child) {
		cfg = &st->channels[channel].cfg;

		ret = of_property_read_u32(child, "reg", &channel);
		if (ret)
			goto err;

		if (channel >= indio_dev->num_channels) {
			dev_err(indio_dev->dev.parent,
				"Channel index >= number of channels\n");
			ret = -EINVAL;
			goto err;
		}

		ret = of_property_read_u32_array(child, "diff-channels",
						 ain, 2);
		if (ret)
			goto err;

		st->channels[channel].nr = channel;
		st->channels[channel].ain = AD7124_CHANNEL_AINP(ain[0]) |
						  AD7124_CHANNEL_AINM(ain[1]);

		cfg->bipolar = of_property_read_bool(child, "bipolar");

		ret = of_property_read_u32(child, "adi,reference-select", &tmp);
		if (ret)
			cfg->refsel = AD7124_INT_REF;
		else
			cfg->refsel = tmp;

		cfg->buf_positive = of_property_read_bool(child, "adi,buffered-positive");
		cfg->buf_negative = of_property_read_bool(child, "adi,buffered-negative");

		chan[channel] = ad7124_channel_template;
		chan[channel].address = channel;
		chan[channel].scan_index = channel;
		chan[channel].channel = ain[0];
		chan[channel].channel2 = ain[1];
	}

	return 0;
err:
	of_node_put(child);

	return ret;
}

static int ad7124_setup(struct ad7124_state *st)
{
	unsigned int fclk, power_mode;
	int i, ret;

	fclk = clk_get_rate(st->mclk);
	if (!fclk)
		return -EINVAL;

	/* The power mode changes the master clock frequency */
	power_mode = ad7124_find_closest_match(ad7124_master_clk_freq_hz,
					ARRAY_SIZE(ad7124_master_clk_freq_hz),
					fclk);
	if (fclk != ad7124_master_clk_freq_hz[power_mode]) {
		ret = clk_set_rate(st->mclk, fclk);
		if (ret)
			return ret;
	}

	/* Set the power mode */
	st->adc_control &= ~AD7124_ADC_CTRL_PWR_MSK;
	st->adc_control |= AD7124_ADC_CTRL_PWR(power_mode);
	ret = ad_sd_write_reg(&st->sd, AD7124_ADC_CONTROL, 2, st->adc_control);
	if (ret < 0)
		return ret;

	mutex_init(&st->cfgs_lock);
	INIT_KFIFO(st->live_cfgs_fifo);
	for (i = 0; i < st->num_channels; i++) {

		ret = ad7124_init_config_vref(st, &st->channels[i].cfg);
		if (ret < 0)
			return ret;

		/*
		 * 9.38 SPS is the minimum output data rate supported
		 * regardless of the selected power mode. Round it up to 10 and
		 * set all channels to this default value.
		 */
		ad7124_set_channel_odr(st, i, 10);
	}

	return ret;
}

static void ad7124_reg_disable(void *r)
{
	regulator_disable(r);
}

static int ad7124_probe(struct spi_device *spi)
{
	const struct ad7124_chip_info *info;
	struct ad7124_state *st;
	struct iio_dev *indio_dev;
	int i, ret;

	info = of_device_get_match_data(&spi->dev);
	if (!info)
		return -ENODEV;

	indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*st));
	if (!indio_dev)
		return -ENOMEM;

	st = iio_priv(indio_dev);

	st->chip_info = info;

	ad_sd_init(&st->sd, indio_dev, spi, &ad7124_sigma_delta_info);

	spi_set_drvdata(spi, indio_dev);

	indio_dev->name = st->chip_info->name;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->info = &ad7124_info;

	ret = ad7124_of_parse_channel_config(indio_dev, spi->dev.of_node);
	if (ret < 0)
		return ret;

	for (i = 0; i < ARRAY_SIZE(st->vref); i++) {
		if (i == AD7124_INT_REF)
			continue;

		st->vref[i] = devm_regulator_get_optional(&spi->dev,
						ad7124_ref_names[i]);
		if (PTR_ERR(st->vref[i]) == -ENODEV)
			continue;
		else if (IS_ERR(st->vref[i]))
			return PTR_ERR(st->vref[i]);

		ret = regulator_enable(st->vref[i]);
		if (ret)
			return ret;

		ret = devm_add_action_or_reset(&spi->dev, ad7124_reg_disable,
					       st->vref[i]);
		if (ret)
			return ret;
	}

	st->mclk = devm_clk_get(&spi->dev, "mclk");
	if (IS_ERR(st->mclk))
		return PTR_ERR(st->mclk);

	ret = clk_prepare_enable(st->mclk);
	if (ret < 0)
		return ret;

	ret = ad7124_soft_reset(st);
	if (ret < 0)
		goto error_clk_disable_unprepare;

	ret = ad7124_check_chip_id(st);
	if (ret)
		goto error_clk_disable_unprepare;

	ret = ad7124_setup(st);
	if (ret < 0)
		goto error_clk_disable_unprepare;

	ret = ad_sd_setup_buffer_and_trigger(indio_dev);
	if (ret < 0)
		goto error_clk_disable_unprepare;

	ret = iio_device_register(indio_dev);
	if (ret < 0) {
		dev_err(&spi->dev, "Failed to register iio device\n");
		goto error_remove_trigger;
	}

	return 0;

error_remove_trigger:
	ad_sd_cleanup_buffer_and_trigger(indio_dev);
error_clk_disable_unprepare:
	clk_disable_unprepare(st->mclk);

	return ret;
}

static int ad7124_remove(struct spi_device *spi)
{
	struct iio_dev *indio_dev = spi_get_drvdata(spi);
	struct ad7124_state *st = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	ad_sd_cleanup_buffer_and_trigger(indio_dev);
	clk_disable_unprepare(st->mclk);

	return 0;
}

static const struct of_device_id ad7124_of_match[] = {
	{ .compatible = "adi,ad7124-4",
		.data = &ad7124_chip_info_tbl[ID_AD7124_4], },
	{ .compatible = "adi,ad7124-8",
		.data = &ad7124_chip_info_tbl[ID_AD7124_8], },
	{ },
};
MODULE_DEVICE_TABLE(of, ad7124_of_match);

static struct spi_driver ad71124_driver = {
	.driver = {
		.name = "ad7124",
		.of_match_table = ad7124_of_match,
	},
	.probe = ad7124_probe,
	.remove	= ad7124_remove,
};
module_spi_driver(ad71124_driver);

MODULE_AUTHOR("Stefan Popa <stefan.popa@analog.com>");
MODULE_DESCRIPTION("Analog Devices AD7124 SPI driver");
MODULE_LICENSE("GPL");
