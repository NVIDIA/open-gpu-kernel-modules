// SPDX-License-Identifier: GPL-2.0-only
/*
 * AD7298 SPI ADC driver
 *
 * Copyright 2011 Analog Devices Inc.
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/spi/spi.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/buffer.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

#define AD7298_WRITE	BIT(15) /* write to the control register */
#define AD7298_REPEAT	BIT(14) /* repeated conversion enable */
#define AD7298_CH(x)	BIT(13 - (x)) /* channel select */
#define AD7298_TSENSE	BIT(5) /* temperature conversion enable */
#define AD7298_EXTREF	BIT(2) /* external reference enable */
#define AD7298_TAVG	BIT(1) /* temperature sensor averaging enable */
#define AD7298_PDD	BIT(0) /* partial power down enable */

#define AD7298_MAX_CHAN		8
#define AD7298_INTREF_mV	2500

#define AD7298_CH_TEMP		9

struct ad7298_state {
	struct spi_device		*spi;
	struct regulator		*reg;
	unsigned			ext_ref;
	struct spi_transfer		ring_xfer[10];
	struct spi_transfer		scan_single_xfer[3];
	struct spi_message		ring_msg;
	struct spi_message		scan_single_msg;
	/*
	 * DMA (thus cache coherency maintenance) requires the
	 * transfer buffers to live in their own cache lines.
	 */
	__be16				rx_buf[12] ____cacheline_aligned;
	__be16				tx_buf[2];
};

#define AD7298_V_CHAN(index)						\
	{								\
		.type = IIO_VOLTAGE,					\
		.indexed = 1,						\
		.channel = index,					\
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),		\
		.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),	\
		.address = index,					\
		.scan_index = index,					\
		.scan_type = {						\
			.sign = 'u',					\
			.realbits = 12,					\
			.storagebits = 16,				\
			.endianness = IIO_BE,				\
		},							\
	}

static const struct iio_chan_spec ad7298_channels[] = {
	{
		.type = IIO_TEMP,
		.indexed = 1,
		.channel = 0,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
			BIT(IIO_CHAN_INFO_SCALE) |
			BIT(IIO_CHAN_INFO_OFFSET),
		.address = AD7298_CH_TEMP,
		.scan_index = -1,
		.scan_type = {
			.sign = 's',
			.realbits = 32,
			.storagebits = 32,
		},
	},
	AD7298_V_CHAN(0),
	AD7298_V_CHAN(1),
	AD7298_V_CHAN(2),
	AD7298_V_CHAN(3),
	AD7298_V_CHAN(4),
	AD7298_V_CHAN(5),
	AD7298_V_CHAN(6),
	AD7298_V_CHAN(7),
	IIO_CHAN_SOFT_TIMESTAMP(8),
};

/*
 * ad7298_update_scan_mode() setup the spi transfer buffer for the new scan mask
 */
static int ad7298_update_scan_mode(struct iio_dev *indio_dev,
	const unsigned long *active_scan_mask)
{
	struct ad7298_state *st = iio_priv(indio_dev);
	int i, m;
	unsigned short command;
	int scan_count;

	/* Now compute overall size */
	scan_count = bitmap_weight(active_scan_mask, indio_dev->masklength);

	command = AD7298_WRITE | st->ext_ref;

	for (i = 0, m = AD7298_CH(0); i < AD7298_MAX_CHAN; i++, m >>= 1)
		if (test_bit(i, active_scan_mask))
			command |= m;

	st->tx_buf[0] = cpu_to_be16(command);

	/* build spi ring message */
	st->ring_xfer[0].tx_buf = &st->tx_buf[0];
	st->ring_xfer[0].len = 2;
	st->ring_xfer[0].cs_change = 1;
	st->ring_xfer[1].tx_buf = &st->tx_buf[1];
	st->ring_xfer[1].len = 2;
	st->ring_xfer[1].cs_change = 1;

	spi_message_init(&st->ring_msg);
	spi_message_add_tail(&st->ring_xfer[0], &st->ring_msg);
	spi_message_add_tail(&st->ring_xfer[1], &st->ring_msg);

	for (i = 0; i < scan_count; i++) {
		st->ring_xfer[i + 2].rx_buf = &st->rx_buf[i];
		st->ring_xfer[i + 2].len = 2;
		st->ring_xfer[i + 2].cs_change = 1;
		spi_message_add_tail(&st->ring_xfer[i + 2], &st->ring_msg);
	}
	/* make sure last transfer cs_change is not set */
	st->ring_xfer[i + 1].cs_change = 0;

	return 0;
}

static irqreturn_t ad7298_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct ad7298_state *st = iio_priv(indio_dev);
	int b_sent;

	b_sent = spi_sync(st->spi, &st->ring_msg);
	if (b_sent)
		goto done;

	iio_push_to_buffers_with_timestamp(indio_dev, st->rx_buf,
		iio_get_time_ns(indio_dev));

done:
	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}

static int ad7298_scan_direct(struct ad7298_state *st, unsigned ch)
{
	int ret;
	st->tx_buf[0] = cpu_to_be16(AD7298_WRITE | st->ext_ref |
				   (AD7298_CH(0) >> ch));

	ret = spi_sync(st->spi, &st->scan_single_msg);
	if (ret)
		return ret;

	return be16_to_cpu(st->rx_buf[0]);
}

static int ad7298_scan_temp(struct ad7298_state *st, int *val)
{
	int ret;
	__be16 buf;

	buf = cpu_to_be16(AD7298_WRITE | AD7298_TSENSE |
			  AD7298_TAVG | st->ext_ref);

	ret = spi_write(st->spi, (u8 *)&buf, 2);
	if (ret)
		return ret;

	buf = cpu_to_be16(0);

	ret = spi_write(st->spi, (u8 *)&buf, 2);
	if (ret)
		return ret;

	usleep_range(101, 1000); /* sleep > 100us */

	ret = spi_read(st->spi, (u8 *)&buf, 2);
	if (ret)
		return ret;

	*val = sign_extend32(be16_to_cpu(buf), 11);

	return 0;
}

static int ad7298_get_ref_voltage(struct ad7298_state *st)
{
	int vref;

	if (st->reg) {
		vref = regulator_get_voltage(st->reg);
		if (vref < 0)
			return vref;

		return vref / 1000;
	} else {
		return AD7298_INTREF_mV;
	}
}

static int ad7298_read_raw(struct iio_dev *indio_dev,
			   struct iio_chan_spec const *chan,
			   int *val,
			   int *val2,
			   long m)
{
	int ret;
	struct ad7298_state *st = iio_priv(indio_dev);

	switch (m) {
	case IIO_CHAN_INFO_RAW:
		ret = iio_device_claim_direct_mode(indio_dev);
		if (ret)
			return ret;

		if (chan->address == AD7298_CH_TEMP)
			ret = ad7298_scan_temp(st, val);
		else
			ret = ad7298_scan_direct(st, chan->address);

		iio_device_release_direct_mode(indio_dev);

		if (ret < 0)
			return ret;

		if (chan->address != AD7298_CH_TEMP)
			*val = ret & GENMASK(chan->scan_type.realbits - 1, 0);

		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		switch (chan->type) {
		case IIO_VOLTAGE:
			*val = ad7298_get_ref_voltage(st);
			*val2 = chan->scan_type.realbits;
			return IIO_VAL_FRACTIONAL_LOG2;
		case IIO_TEMP:
			*val = ad7298_get_ref_voltage(st);
			*val2 = 10;
			return IIO_VAL_FRACTIONAL;
		default:
			return -EINVAL;
		}
	case IIO_CHAN_INFO_OFFSET:
		*val = 1093 - 2732500 / ad7298_get_ref_voltage(st);
		return IIO_VAL_INT;
	}
	return -EINVAL;
}

static const struct iio_info ad7298_info = {
	.read_raw = &ad7298_read_raw,
	.update_scan_mode = ad7298_update_scan_mode,
};

static void ad7298_reg_disable(void *data)
{
	struct regulator *reg = data;

	regulator_disable(reg);
}

static int ad7298_probe(struct spi_device *spi)
{
	struct ad7298_state *st;
	struct iio_dev *indio_dev;
	int ret;

	indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*st));
	if (indio_dev == NULL)
		return -ENOMEM;

	st = iio_priv(indio_dev);

	st->reg = devm_regulator_get_optional(&spi->dev, "vref");
	if (!IS_ERR(st->reg)) {
		st->ext_ref = AD7298_EXTREF;
	} else {
		ret = PTR_ERR(st->reg);
		if (ret != -ENODEV)
			return ret;

		st->reg = NULL;
	}

	if (st->reg) {
		ret = regulator_enable(st->reg);
		if (ret)
			return ret;

		ret = devm_add_action_or_reset(&spi->dev, ad7298_reg_disable,
					       st->reg);
		if (ret)
			return ret;
	}

	st->spi = spi;

	indio_dev->name = spi_get_device_id(spi)->name;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = ad7298_channels;
	indio_dev->num_channels = ARRAY_SIZE(ad7298_channels);
	indio_dev->info = &ad7298_info;

	/* Setup default message */

	st->scan_single_xfer[0].tx_buf = &st->tx_buf[0];
	st->scan_single_xfer[0].len = 2;
	st->scan_single_xfer[0].cs_change = 1;
	st->scan_single_xfer[1].tx_buf = &st->tx_buf[1];
	st->scan_single_xfer[1].len = 2;
	st->scan_single_xfer[1].cs_change = 1;
	st->scan_single_xfer[2].rx_buf = &st->rx_buf[0];
	st->scan_single_xfer[2].len = 2;

	spi_message_init(&st->scan_single_msg);
	spi_message_add_tail(&st->scan_single_xfer[0], &st->scan_single_msg);
	spi_message_add_tail(&st->scan_single_xfer[1], &st->scan_single_msg);
	spi_message_add_tail(&st->scan_single_xfer[2], &st->scan_single_msg);

	ret = devm_iio_triggered_buffer_setup(&spi->dev, indio_dev, NULL,
			&ad7298_trigger_handler, NULL);
	if (ret)
		return ret;

	return devm_iio_device_register(&spi->dev, indio_dev);
}

static const struct spi_device_id ad7298_id[] = {
	{"ad7298", 0},
	{}
};
MODULE_DEVICE_TABLE(spi, ad7298_id);

static struct spi_driver ad7298_driver = {
	.driver = {
		.name	= "ad7298",
	},
	.probe		= ad7298_probe,
	.id_table	= ad7298_id,
};
module_spi_driver(ad7298_driver);

MODULE_AUTHOR("Michael Hennerich <michael.hennerich@analog.com>");
MODULE_DESCRIPTION("Analog Devices AD7298 ADC");
MODULE_LICENSE("GPL v2");
