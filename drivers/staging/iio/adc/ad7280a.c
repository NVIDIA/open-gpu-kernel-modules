// SPDX-License-Identifier: GPL-2.0
/*
 * AD7280A Lithium Ion Battery Monitoring System
 *
 * Copyright 2011 Analog Devices Inc.
 */

#include <linux/crc8.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/spi/spi.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/events.h>

#include "ad7280a.h"

/* Registers */
#define AD7280A_CELL_VOLTAGE_1		0x0  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_2		0x1  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_3		0x2  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_4		0x3  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_5		0x4  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_6		0x5  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_1		0x6  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_2		0x7  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_3		0x8  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_4		0x9  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_5		0xA  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_6		0xB  /* D11 to D0, Read only */
#define AD7280A_SELF_TEST		0xC  /* D11 to D0, Read only */
#define AD7280A_CONTROL_HB		0xD  /* D15 to D8, Read/write */
#define AD7280A_CONTROL_LB		0xE  /* D7 to D0, Read/write */
#define AD7280A_CELL_OVERVOLTAGE	0xF  /* D7 to D0, Read/write */
#define AD7280A_CELL_UNDERVOLTAGE	0x10 /* D7 to D0, Read/write */
#define AD7280A_AUX_ADC_OVERVOLTAGE	0x11 /* D7 to D0, Read/write */
#define AD7280A_AUX_ADC_UNDERVOLTAGE	0x12 /* D7 to D0, Read/write */
#define AD7280A_ALERT			0x13 /* D7 to D0, Read/write */
#define AD7280A_CELL_BALANCE		0x14 /* D7 to D0, Read/write */
#define AD7280A_CB1_TIMER		0x15 /* D7 to D0, Read/write */
#define AD7280A_CB2_TIMER		0x16 /* D7 to D0, Read/write */
#define AD7280A_CB3_TIMER		0x17 /* D7 to D0, Read/write */
#define AD7280A_CB4_TIMER		0x18 /* D7 to D0, Read/write */
#define AD7280A_CB5_TIMER		0x19 /* D7 to D0, Read/write */
#define AD7280A_CB6_TIMER		0x1A /* D7 to D0, Read/write */
#define AD7280A_PD_TIMER		0x1B /* D7 to D0, Read/write */
#define AD7280A_READ			0x1C /* D7 to D0, Read/write */
#define AD7280A_CNVST_CONTROL		0x1D /* D7 to D0, Read/write */

/* Bits and Masks */
#define AD7280A_CTRL_HB_CONV_INPUT_ALL			0
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL_AUX1_3_4	BIT(6)
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL		BIT(7)
#define AD7280A_CTRL_HB_CONV_INPUT_SELF_TEST		(BIT(7) | BIT(6))
#define AD7280A_CTRL_HB_CONV_RES_READ_ALL		0
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL_AUX1_3_4	BIT(4)
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL		BIT(5)
#define AD7280A_CTRL_HB_CONV_RES_READ_NO		(BIT(5) | BIT(4))
#define AD7280A_CTRL_HB_CONV_START_CNVST		0
#define AD7280A_CTRL_HB_CONV_START_CS			BIT(3)
#define AD7280A_CTRL_HB_CONV_AVG_DIS			0
#define AD7280A_CTRL_HB_CONV_AVG_2			BIT(1)
#define AD7280A_CTRL_HB_CONV_AVG_4			BIT(2)
#define AD7280A_CTRL_HB_CONV_AVG_8			(BIT(2) | BIT(1))
#define AD7280A_CTRL_HB_CONV_AVG(x)			((x) << 1)
#define AD7280A_CTRL_HB_PWRDN_SW			BIT(0)

#define AD7280A_CTRL_LB_SWRST				BIT(7)
#define AD7280A_CTRL_LB_ACQ_TIME_400ns			0
#define AD7280A_CTRL_LB_ACQ_TIME_800ns			BIT(5)
#define AD7280A_CTRL_LB_ACQ_TIME_1200ns			BIT(6)
#define AD7280A_CTRL_LB_ACQ_TIME_1600ns			(BIT(6) | BIT(5))
#define AD7280A_CTRL_LB_ACQ_TIME(x)			((x) << 5)
#define AD7280A_CTRL_LB_MUST_SET			BIT(4)
#define AD7280A_CTRL_LB_THERMISTOR_EN			BIT(3)
#define AD7280A_CTRL_LB_LOCK_DEV_ADDR			BIT(2)
#define AD7280A_CTRL_LB_INC_DEV_ADDR			BIT(1)
#define AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN		BIT(0)

#define AD7280A_ALERT_GEN_STATIC_HIGH			BIT(6)
#define AD7280A_ALERT_RELAY_SIG_CHAIN_DOWN		(BIT(7) | BIT(6))

#define AD7280A_ALL_CELLS				(0xAD << 16)

#define AD7280A_MAX_SPI_CLK_HZ		700000 /* < 1MHz */
#define AD7280A_MAX_CHAIN		8
#define AD7280A_CELLS_PER_DEV		6
#define AD7280A_BITS			12
#define AD7280A_NUM_CH			(AD7280A_AUX_ADC_6 - \
					AD7280A_CELL_VOLTAGE_1 + 1)

#define AD7280A_CALC_VOLTAGE_CHAN_NUM(d, c) (((d) * AD7280A_CELLS_PER_DEV) + \
					     (c))
#define AD7280A_CALC_TEMP_CHAN_NUM(d, c)    (((d) * AD7280A_CELLS_PER_DEV) + \
					     (c) - AD7280A_CELLS_PER_DEV)

#define AD7280A_DEVADDR_MASTER		0
#define AD7280A_DEVADDR_ALL		0x1F
/* 5-bit device address is sent LSB first */
static unsigned int ad7280a_devaddr(unsigned int addr)
{
	return ((addr & 0x1) << 4) |
	       ((addr & 0x2) << 3) |
	       (addr & 0x4) |
	       ((addr & 0x8) >> 3) |
	       ((addr & 0x10) >> 4);
}

/* During a read a valid write is mandatory.
 * So writing to the highest available address (Address 0x1F)
 * and setting the address all parts bit to 0 is recommended
 * So the TXVAL is AD7280A_DEVADDR_ALL + CRC
 */
#define AD7280A_READ_TXVAL	0xF800030A

/*
 * AD7280 CRC
 *
 * P(x) = x^8 + x^5 + x^3 + x^2 + x^1 + x^0 = 0b100101111 => 0x2F
 */
#define POLYNOM		0x2F

struct ad7280_state {
	struct spi_device		*spi;
	struct iio_chan_spec		*channels;
	struct iio_dev_attr		*iio_attr;
	int				slave_num;
	int				scan_cnt;
	int				readback_delay_us;
	unsigned char			crc_tab[CRC8_TABLE_SIZE];
	unsigned char			ctrl_hb;
	unsigned char			ctrl_lb;
	unsigned char			cell_threshhigh;
	unsigned char			cell_threshlow;
	unsigned char			aux_threshhigh;
	unsigned char			aux_threshlow;
	unsigned char			cb_mask[AD7280A_MAX_CHAIN];
	struct mutex			lock; /* protect sensor state */

	__be32				buf[2] ____cacheline_aligned;
};

static unsigned char ad7280_calc_crc8(unsigned char *crc_tab, unsigned int val)
{
	unsigned char crc;

	crc = crc_tab[val >> 16 & 0xFF];
	crc = crc_tab[crc ^ (val >> 8 & 0xFF)];

	return  crc ^ (val & 0xFF);
}

static int ad7280_check_crc(struct ad7280_state *st, unsigned int val)
{
	unsigned char crc = ad7280_calc_crc8(st->crc_tab, val >> 10);

	if (crc != ((val >> 2) & 0xFF))
		return -EIO;

	return 0;
}

/* After initiating a conversion sequence we need to wait until the
 * conversion is done. The delay is typically in the range of 15..30 us
 * however depending an the number of devices in the daisy chain and the
 * number of averages taken, conversion delays and acquisition time options
 * it may take up to 250us, in this case we better sleep instead of busy
 * wait.
 */

static void ad7280_delay(struct ad7280_state *st)
{
	if (st->readback_delay_us < 50)
		udelay(st->readback_delay_us);
	else
		usleep_range(250, 500);
}

static int __ad7280_read32(struct ad7280_state *st, unsigned int *val)
{
	int ret;
	struct spi_transfer t = {
		.tx_buf	= &st->buf[0],
		.rx_buf = &st->buf[1],
		.len = 4,
	};

	st->buf[0] = cpu_to_be32(AD7280A_READ_TXVAL);

	ret = spi_sync_transfer(st->spi, &t, 1);
	if (ret)
		return ret;

	*val = be32_to_cpu(st->buf[1]);

	return 0;
}

static int ad7280_write(struct ad7280_state *st, unsigned int devaddr,
			unsigned int addr, bool all, unsigned int val)
{
	unsigned int reg = devaddr << 27 | addr << 21 |
			(val & 0xFF) << 13 | all << 12;

	reg |= ad7280_calc_crc8(st->crc_tab, reg >> 11) << 3 | 0x2;
	st->buf[0] = cpu_to_be32(reg);

	return spi_write(st->spi, &st->buf[0], 4);
}

static int ad7280_read(struct ad7280_state *st, unsigned int devaddr,
		       unsigned int addr)
{
	int ret;
	unsigned int tmp;

	/* turns off the read operation on all parts */
	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,
			   AD7280A_CTRL_HB_CONV_INPUT_ALL |
			   AD7280A_CTRL_HB_CONV_RES_READ_NO |
			   st->ctrl_hb);
	if (ret)
		return ret;

	/* turns on the read operation on the addressed part */
	ret = ad7280_write(st, devaddr, AD7280A_CONTROL_HB, 0,
			   AD7280A_CTRL_HB_CONV_INPUT_ALL |
			   AD7280A_CTRL_HB_CONV_RES_READ_ALL |
			   st->ctrl_hb);
	if (ret)
		return ret;

	/* Set register address on the part to be read from */
	ret = ad7280_write(st, devaddr, AD7280A_READ, 0, addr << 2);
	if (ret)
		return ret;

	ret = __ad7280_read32(st, &tmp);
	if (ret)
		return ret;

	if (ad7280_check_crc(st, tmp))
		return -EIO;

	if (((tmp >> 27) != devaddr) || (((tmp >> 21) & 0x3F) != addr))
		return -EFAULT;

	return (tmp >> 13) & 0xFF;
}

static int ad7280_read_channel(struct ad7280_state *st, unsigned int devaddr,
			       unsigned int addr)
{
	int ret;
	unsigned int tmp;

	ret = ad7280_write(st, devaddr, AD7280A_READ, 0, addr << 2);
	if (ret)
		return ret;

	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,
			   AD7280A_CTRL_HB_CONV_INPUT_ALL |
			   AD7280A_CTRL_HB_CONV_RES_READ_NO |
			   st->ctrl_hb);
	if (ret)
		return ret;

	ret = ad7280_write(st, devaddr, AD7280A_CONTROL_HB, 0,
			   AD7280A_CTRL_HB_CONV_INPUT_ALL |
			   AD7280A_CTRL_HB_CONV_RES_READ_ALL |
			   AD7280A_CTRL_HB_CONV_START_CS |
			   st->ctrl_hb);
	if (ret)
		return ret;

	ad7280_delay(st);

	ret = __ad7280_read32(st, &tmp);
	if (ret)
		return ret;

	if (ad7280_check_crc(st, tmp))
		return -EIO;

	if (((tmp >> 27) != devaddr) || (((tmp >> 23) & 0xF) != addr))
		return -EFAULT;

	return (tmp >> 11) & 0xFFF;
}

static int ad7280_read_all_channels(struct ad7280_state *st, unsigned int cnt,
				    unsigned int *array)
{
	int i, ret;
	unsigned int tmp, sum = 0;

	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_READ, 1,
			   AD7280A_CELL_VOLTAGE_1 << 2);
	if (ret)
		return ret;

	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,
			   AD7280A_CTRL_HB_CONV_INPUT_ALL |
			   AD7280A_CTRL_HB_CONV_RES_READ_ALL |
			   AD7280A_CTRL_HB_CONV_START_CS |
			   st->ctrl_hb);
	if (ret)
		return ret;

	ad7280_delay(st);

	for (i = 0; i < cnt; i++) {
		ret = __ad7280_read32(st, &tmp);
		if (ret)
			return ret;

		if (ad7280_check_crc(st, tmp))
			return -EIO;

		if (array)
			array[i] = tmp;
		/* only sum cell voltages */
		if (((tmp >> 23) & 0xF) <= AD7280A_CELL_VOLTAGE_6)
			sum += ((tmp >> 11) & 0xFFF);
	}

	return sum;
}

static void ad7280_sw_power_down(void *data)
{
	struct ad7280_state *st = data;

	ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,
		     AD7280A_CTRL_HB_PWRDN_SW | st->ctrl_hb);
}

static int ad7280_chain_setup(struct ad7280_state *st)
{
	unsigned int val, n;
	int ret;

	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_LB, 1,
			   AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN |
			   AD7280A_CTRL_LB_LOCK_DEV_ADDR |
			   AD7280A_CTRL_LB_MUST_SET |
			   AD7280A_CTRL_LB_SWRST |
			   st->ctrl_lb);
	if (ret)
		return ret;

	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_LB, 1,
			   AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN |
			   AD7280A_CTRL_LB_LOCK_DEV_ADDR |
			   AD7280A_CTRL_LB_MUST_SET |
			   st->ctrl_lb);
	if (ret)
		goto error_power_down;

	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_READ, 1,
			   AD7280A_CONTROL_LB << 2);
	if (ret)
		goto error_power_down;

	for (n = 0; n <= AD7280A_MAX_CHAIN; n++) {
		ret = __ad7280_read32(st, &val);
		if (ret)
			goto error_power_down;

		if (val == 0)
			return n - 1;

		if (ad7280_check_crc(st, val)) {
			ret = -EIO;
			goto error_power_down;
		}

		if (n != ad7280a_devaddr(val >> 27)) {
			ret = -EIO;
			goto error_power_down;
		}
	}
	ret = -EFAULT;

error_power_down:
	ad7280_write(st, AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,
		     AD7280A_CTRL_HB_PWRDN_SW | st->ctrl_hb);

	return ret;
}

static ssize_t ad7280_show_balance_sw(struct device *dev,
				      struct device_attribute *attr,
				      char *buf)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct ad7280_state *st = iio_priv(indio_dev);
	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);

	return sprintf(buf, "%d\n",
		       !!(st->cb_mask[this_attr->address >> 8] &
		       (1 << ((this_attr->address & 0xFF) + 2))));
}

static ssize_t ad7280_store_balance_sw(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf,
				       size_t len)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct ad7280_state *st = iio_priv(indio_dev);
	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);
	bool readin;
	int ret;
	unsigned int devaddr, ch;

	ret = strtobool(buf, &readin);
	if (ret)
		return ret;

	devaddr = this_attr->address >> 8;
	ch = this_attr->address & 0xFF;

	mutex_lock(&st->lock);
	if (readin)
		st->cb_mask[devaddr] |= 1 << (ch + 2);
	else
		st->cb_mask[devaddr] &= ~(1 << (ch + 2));

	ret = ad7280_write(st, devaddr, AD7280A_CELL_BALANCE,
			   0, st->cb_mask[devaddr]);
	mutex_unlock(&st->lock);

	return ret ? ret : len;
}

static ssize_t ad7280_show_balance_timer(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct ad7280_state *st = iio_priv(indio_dev);
	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);
	int ret;
	unsigned int msecs;

	mutex_lock(&st->lock);
	ret = ad7280_read(st, this_attr->address >> 8,
			  this_attr->address & 0xFF);
	mutex_unlock(&st->lock);

	if (ret < 0)
		return ret;

	msecs = (ret >> 3) * 71500;

	return sprintf(buf, "%u\n", msecs);
}

static ssize_t ad7280_store_balance_timer(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf,
					  size_t len)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct ad7280_state *st = iio_priv(indio_dev);
	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);
	unsigned long val;
	int ret;

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		return ret;

	val /= 71500;

	if (val > 31)
		return -EINVAL;

	mutex_lock(&st->lock);
	ret = ad7280_write(st, this_attr->address >> 8,
			   this_attr->address & 0xFF,
			   0, (val & 0x1F) << 3);
	mutex_unlock(&st->lock);

	return ret ? ret : len;
}

static struct attribute *ad7280_attributes[AD7280A_MAX_CHAIN *
					   AD7280A_CELLS_PER_DEV * 2 + 1];

static const struct attribute_group ad7280_attrs_group = {
	.attrs = ad7280_attributes,
};

static void ad7280_voltage_channel_init(struct iio_chan_spec *chan, int i)
{
	chan->type = IIO_VOLTAGE;
	chan->differential = 1;
	chan->channel = i;
	chan->channel2 = chan->channel + 1;
}

static void ad7280_temp_channel_init(struct iio_chan_spec *chan, int i)
{
	chan->type = IIO_TEMP;
	chan->channel = i;
}

static void ad7280_common_fields_init(struct iio_chan_spec *chan, int addr,
				      int cnt)
{
	chan->indexed = 1;
	chan->info_mask_separate = BIT(IIO_CHAN_INFO_RAW);
	chan->info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE);
	chan->address = addr;
	chan->scan_index = cnt;
	chan->scan_type.sign = 'u';
	chan->scan_type.realbits = 12;
	chan->scan_type.storagebits = 32;
}

static void ad7280_total_voltage_channel_init(struct iio_chan_spec *chan,
					      int cnt, int dev)
{
	chan->type = IIO_VOLTAGE;
	chan->differential = 1;
	chan->channel = 0;
	chan->channel2 = dev * AD7280A_CELLS_PER_DEV;
	chan->address = AD7280A_ALL_CELLS;
	chan->indexed = 1;
	chan->info_mask_separate = BIT(IIO_CHAN_INFO_RAW);
	chan->info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE);
	chan->scan_index = cnt;
	chan->scan_type.sign = 'u';
	chan->scan_type.realbits = 32;
	chan->scan_type.storagebits = 32;
}

static void ad7280_timestamp_channel_init(struct iio_chan_spec *chan, int cnt)
{
	chan->type = IIO_TIMESTAMP;
	chan->channel = -1;
	chan->scan_index = cnt;
	chan->scan_type.sign = 's';
	chan->scan_type.realbits = 64;
	chan->scan_type.storagebits = 64;
}

static void ad7280_init_dev_channels(struct ad7280_state *st, int dev, int *cnt)
{
	int addr, ch, i;
	struct iio_chan_spec *chan;

	for (ch = AD7280A_CELL_VOLTAGE_1; ch <= AD7280A_AUX_ADC_6; ch++) {
		chan = &st->channels[*cnt];

		if (ch < AD7280A_AUX_ADC_1) {
			i = AD7280A_CALC_VOLTAGE_CHAN_NUM(dev, ch);
			ad7280_voltage_channel_init(chan, i);
		} else {
			i = AD7280A_CALC_TEMP_CHAN_NUM(dev, ch);
			ad7280_temp_channel_init(chan, i);
		}

		addr = ad7280a_devaddr(dev) << 8 | ch;
		ad7280_common_fields_init(chan, addr, *cnt);

		(*cnt)++;
	}
}

static int ad7280_channel_init(struct ad7280_state *st)
{
	int dev, cnt = 0;

	st->channels = devm_kcalloc(&st->spi->dev, (st->slave_num + 1) * 12 + 2,
				    sizeof(*st->channels), GFP_KERNEL);
	if (!st->channels)
		return -ENOMEM;

	for (dev = 0; dev <= st->slave_num; dev++)
		ad7280_init_dev_channels(st, dev, &cnt);

	ad7280_total_voltage_channel_init(&st->channels[cnt], cnt, dev);
	cnt++;
	ad7280_timestamp_channel_init(&st->channels[cnt], cnt);

	return cnt + 1;
}

static int ad7280_balance_switch_attr_init(struct iio_dev_attr *attr,
					   struct device *dev, int addr, int i)
{
	attr->address = addr;
	attr->dev_attr.attr.mode = 0644;
	attr->dev_attr.show = ad7280_show_balance_sw;
	attr->dev_attr.store = ad7280_store_balance_sw;
	attr->dev_attr.attr.name = devm_kasprintf(dev, GFP_KERNEL,
						  "in%d-in%d_balance_switch_en",
						  i, i + 1);
	if (!attr->dev_attr.attr.name)
		return -ENOMEM;

	return 0;
}

static int ad7280_balance_timer_attr_init(struct iio_dev_attr *attr,
					  struct device *dev, int addr, int i)
{
	attr->address = addr;
	attr->dev_attr.attr.mode = 0644;
	attr->dev_attr.show = ad7280_show_balance_timer;
	attr->dev_attr.store = ad7280_store_balance_timer;
	attr->dev_attr.attr.name = devm_kasprintf(dev, GFP_KERNEL,
						  "in%d-in%d_balance_timer",
						  i, i + 1);
	if (!attr->dev_attr.attr.name)
		return -ENOMEM;

	return 0;
}

static int ad7280_init_dev_attrs(struct ad7280_state *st, int dev, int *cnt)
{
	int addr, ch, i, ret;
	struct iio_dev_attr *iio_attr;
	struct device *sdev = &st->spi->dev;

	for (ch = AD7280A_CELL_VOLTAGE_1; ch <= AD7280A_CELL_VOLTAGE_6; ch++) {
		iio_attr = &st->iio_attr[*cnt];
		addr = ad7280a_devaddr(dev) << 8 | ch;
		i = dev * AD7280A_CELLS_PER_DEV + ch;

		ret = ad7280_balance_switch_attr_init(iio_attr, sdev, addr, i);
		if (ret < 0)
			return ret;

		ad7280_attributes[*cnt] = &iio_attr->dev_attr.attr;

		(*cnt)++;
		iio_attr = &st->iio_attr[*cnt];
		addr = ad7280a_devaddr(dev) << 8 | (AD7280A_CB1_TIMER + ch);

		ret = ad7280_balance_timer_attr_init(iio_attr, sdev, addr, i);
		if (ret < 0)
			return ret;

		ad7280_attributes[*cnt] = &iio_attr->dev_attr.attr;
		(*cnt)++;
	}

	ad7280_attributes[*cnt] = NULL;

	return 0;
}

static int ad7280_attr_init(struct ad7280_state *st)
{
	int dev, cnt = 0, ret;

	st->iio_attr = devm_kcalloc(&st->spi->dev, 2, sizeof(*st->iio_attr) *
				    (st->slave_num + 1) * AD7280A_CELLS_PER_DEV,
				    GFP_KERNEL);
	if (!st->iio_attr)
		return -ENOMEM;

	for (dev = 0; dev <= st->slave_num; dev++) {
		ret = ad7280_init_dev_attrs(st, dev, &cnt);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static ssize_t ad7280_read_channel_config(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct ad7280_state *st = iio_priv(indio_dev);
	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);
	unsigned int val;

	switch (this_attr->address) {
	case AD7280A_CELL_OVERVOLTAGE:
		val = 1000 + (st->cell_threshhigh * 1568) / 100;
		break;
	case AD7280A_CELL_UNDERVOLTAGE:
		val = 1000 + (st->cell_threshlow * 1568) / 100;
		break;
	case AD7280A_AUX_ADC_OVERVOLTAGE:
		val = (st->aux_threshhigh * 196) / 10;
		break;
	case AD7280A_AUX_ADC_UNDERVOLTAGE:
		val = (st->aux_threshlow * 196) / 10;
		break;
	default:
		return -EINVAL;
	}

	return sprintf(buf, "%u\n", val);
}

static ssize_t ad7280_write_channel_config(struct device *dev,
					   struct device_attribute *attr,
					   const char *buf,
					   size_t len)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct ad7280_state *st = iio_priv(indio_dev);
	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);

	long val;
	int ret;

	ret = kstrtol(buf, 10, &val);
	if (ret)
		return ret;

	switch (this_attr->address) {
	case AD7280A_CELL_OVERVOLTAGE:
	case AD7280A_CELL_UNDERVOLTAGE:
		val = ((val - 1000) * 100) / 1568; /* LSB 15.68mV */
		break;
	case AD7280A_AUX_ADC_OVERVOLTAGE:
	case AD7280A_AUX_ADC_UNDERVOLTAGE:
		val = (val * 10) / 196; /* LSB 19.6mV */
		break;
	default:
		return -EFAULT;
	}

	val = clamp(val, 0L, 0xFFL);

	mutex_lock(&st->lock);
	switch (this_attr->address) {
	case AD7280A_CELL_OVERVOLTAGE:
		st->cell_threshhigh = val;
		break;
	case AD7280A_CELL_UNDERVOLTAGE:
		st->cell_threshlow = val;
		break;
	case AD7280A_AUX_ADC_OVERVOLTAGE:
		st->aux_threshhigh = val;
		break;
	case AD7280A_AUX_ADC_UNDERVOLTAGE:
		st->aux_threshlow = val;
		break;
	}

	ret = ad7280_write(st, AD7280A_DEVADDR_MASTER,
			   this_attr->address, 1, val);

	mutex_unlock(&st->lock);

	return ret ? ret : len;
}

static irqreturn_t ad7280_event_handler(int irq, void *private)
{
	struct iio_dev *indio_dev = private;
	struct ad7280_state *st = iio_priv(indio_dev);
	unsigned int *channels;
	int i, ret;

	channels = kcalloc(st->scan_cnt, sizeof(*channels), GFP_KERNEL);
	if (!channels)
		return IRQ_HANDLED;

	ret = ad7280_read_all_channels(st, st->scan_cnt, channels);
	if (ret < 0)
		goto out;

	for (i = 0; i < st->scan_cnt; i++) {
		if (((channels[i] >> 23) & 0xF) <= AD7280A_CELL_VOLTAGE_6) {
			if (((channels[i] >> 11) & 0xFFF) >=
			    st->cell_threshhigh) {
				u64 tmp = IIO_EVENT_CODE(IIO_VOLTAGE, 1, 0,
							 IIO_EV_DIR_RISING,
							 IIO_EV_TYPE_THRESH,
							 0, 0, 0);
				iio_push_event(indio_dev, tmp,
					       iio_get_time_ns(indio_dev));
			} else if (((channels[i] >> 11) & 0xFFF) <=
				   st->cell_threshlow) {
				u64 tmp = IIO_EVENT_CODE(IIO_VOLTAGE, 1, 0,
							 IIO_EV_DIR_FALLING,
							 IIO_EV_TYPE_THRESH,
							 0, 0, 0);
				iio_push_event(indio_dev, tmp,
					       iio_get_time_ns(indio_dev));
			}
		} else {
			if (((channels[i] >> 11) & 0xFFF) >=
			    st->aux_threshhigh) {
				u64 tmp = IIO_UNMOD_EVENT_CODE(IIO_TEMP, 0,
							IIO_EV_TYPE_THRESH,
							IIO_EV_DIR_RISING);
				iio_push_event(indio_dev, tmp,
					       iio_get_time_ns(indio_dev));
			} else if (((channels[i] >> 11) & 0xFFF) <=
				st->aux_threshlow) {
				u64 tmp = IIO_UNMOD_EVENT_CODE(IIO_TEMP, 0,
							IIO_EV_TYPE_THRESH,
							IIO_EV_DIR_FALLING);
				iio_push_event(indio_dev, tmp,
					       iio_get_time_ns(indio_dev));
			}
		}
	}

out:
	kfree(channels);

	return IRQ_HANDLED;
}

/* Note: No need to fix checkpatch warning that reads:
 *	CHECK: spaces preferred around that '-' (ctx:VxV)
 * The function argument is stringified and doesn't need a fix
 */
static IIO_DEVICE_ATTR_NAMED(in_thresh_low_value,
			     in_voltage-voltage_thresh_low_value,
			     0644,
			     ad7280_read_channel_config,
			     ad7280_write_channel_config,
			     AD7280A_CELL_UNDERVOLTAGE);

static IIO_DEVICE_ATTR_NAMED(in_thresh_high_value,
			     in_voltage-voltage_thresh_high_value,
			     0644,
			     ad7280_read_channel_config,
			     ad7280_write_channel_config,
			     AD7280A_CELL_OVERVOLTAGE);

static IIO_DEVICE_ATTR(in_temp_thresh_low_value,
		       0644,
		       ad7280_read_channel_config,
		       ad7280_write_channel_config,
		       AD7280A_AUX_ADC_UNDERVOLTAGE);

static IIO_DEVICE_ATTR(in_temp_thresh_high_value,
		       0644,
		       ad7280_read_channel_config,
		       ad7280_write_channel_config,
		       AD7280A_AUX_ADC_OVERVOLTAGE);

static struct attribute *ad7280_event_attributes[] = {
	&iio_dev_attr_in_thresh_low_value.dev_attr.attr,
	&iio_dev_attr_in_thresh_high_value.dev_attr.attr,
	&iio_dev_attr_in_temp_thresh_low_value.dev_attr.attr,
	&iio_dev_attr_in_temp_thresh_high_value.dev_attr.attr,
	NULL,
};

static const struct attribute_group ad7280_event_attrs_group = {
	.attrs = ad7280_event_attributes,
};

static int ad7280_read_raw(struct iio_dev *indio_dev,
			   struct iio_chan_spec const *chan,
			   int *val,
			   int *val2,
			   long m)
{
	struct ad7280_state *st = iio_priv(indio_dev);
	int ret;

	switch (m) {
	case IIO_CHAN_INFO_RAW:
		mutex_lock(&st->lock);
		if (chan->address == AD7280A_ALL_CELLS)
			ret = ad7280_read_all_channels(st, st->scan_cnt, NULL);
		else
			ret = ad7280_read_channel(st, chan->address >> 8,
						  chan->address & 0xFF);
		mutex_unlock(&st->lock);

		if (ret < 0)
			return ret;

		*val = ret;

		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		if ((chan->address & 0xFF) <= AD7280A_CELL_VOLTAGE_6)
			*val = 4000;
		else
			*val = 5000;

		*val2 = AD7280A_BITS;
		return IIO_VAL_FRACTIONAL_LOG2;
	}
	return -EINVAL;
}

static const struct iio_info ad7280_info = {
	.read_raw = ad7280_read_raw,
	.event_attrs = &ad7280_event_attrs_group,
	.attrs = &ad7280_attrs_group,
};

static const struct ad7280_platform_data ad7793_default_pdata = {
	.acquisition_time = AD7280A_ACQ_TIME_400ns,
	.conversion_averaging = AD7280A_CONV_AVG_DIS,
	.thermistor_term_en = true,
};

static int ad7280_probe(struct spi_device *spi)
{
	const struct ad7280_platform_data *pdata = dev_get_platdata(&spi->dev);
	struct ad7280_state *st;
	int ret;
	const unsigned short t_acq_ns[4] = {465, 1010, 1460, 1890};
	const unsigned short n_avg[4] = {1, 2, 4, 8};
	struct iio_dev *indio_dev;

	indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*st));
	if (!indio_dev)
		return -ENOMEM;

	st = iio_priv(indio_dev);
	spi_set_drvdata(spi, indio_dev);
	st->spi = spi;
	mutex_init(&st->lock);

	if (!pdata)
		pdata = &ad7793_default_pdata;

	crc8_populate_msb(st->crc_tab, POLYNOM);

	st->spi->max_speed_hz = AD7280A_MAX_SPI_CLK_HZ;
	st->spi->mode = SPI_MODE_1;
	spi_setup(st->spi);

	st->ctrl_lb = AD7280A_CTRL_LB_ACQ_TIME(pdata->acquisition_time & 0x3);
	st->ctrl_hb = AD7280A_CTRL_HB_CONV_AVG(pdata->conversion_averaging
			& 0x3) | (pdata->thermistor_term_en ?
			AD7280A_CTRL_LB_THERMISTOR_EN : 0);

	ret = ad7280_chain_setup(st);
	if (ret < 0)
		return ret;

	st->slave_num = ret;
	st->scan_cnt = (st->slave_num + 1) * AD7280A_NUM_CH;
	st->cell_threshhigh = 0xFF;
	st->aux_threshhigh = 0xFF;

	ret = devm_add_action_or_reset(&spi->dev, ad7280_sw_power_down, st);
	if (ret)
		return ret;

	/*
	 * Total Conversion Time = ((tACQ + tCONV) *
	 *			   (Number of Conversions per Part)) −
	 *			   tACQ + ((N - 1) * tDELAY)
	 *
	 * Readback Delay = Total Conversion Time + tWAIT
	 */

	st->readback_delay_us =
		((t_acq_ns[pdata->acquisition_time & 0x3] + 695) *
		 (AD7280A_NUM_CH * n_avg[pdata->conversion_averaging & 0x3])) -
		t_acq_ns[pdata->acquisition_time & 0x3] + st->slave_num * 250;

	/* Convert to usecs */
	st->readback_delay_us = DIV_ROUND_UP(st->readback_delay_us, 1000);
	st->readback_delay_us += 5; /* Add tWAIT */

	indio_dev->name = spi_get_device_id(spi)->name;
	indio_dev->modes = INDIO_DIRECT_MODE;

	ret = ad7280_channel_init(st);
	if (ret < 0)
		return ret;

	indio_dev->num_channels = ret;
	indio_dev->channels = st->channels;
	indio_dev->info = &ad7280_info;

	ret = ad7280_attr_init(st);
	if (ret < 0)
		return ret;

	ret = devm_iio_device_register(&spi->dev, indio_dev);
	if (ret)
		return ret;

	if (spi->irq > 0) {
		ret = ad7280_write(st, AD7280A_DEVADDR_MASTER,
				   AD7280A_ALERT, 1,
				   AD7280A_ALERT_RELAY_SIG_CHAIN_DOWN);
		if (ret)
			return ret;

		ret = ad7280_write(st, ad7280a_devaddr(st->slave_num),
				   AD7280A_ALERT, 0,
				   AD7280A_ALERT_GEN_STATIC_HIGH |
				   (pdata->chain_last_alert_ignore & 0xF));
		if (ret)
			return ret;

		ret = devm_request_threaded_irq(&spi->dev, spi->irq,
						NULL,
						ad7280_event_handler,
						IRQF_TRIGGER_FALLING |
						IRQF_ONESHOT,
						indio_dev->name,
						indio_dev);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct spi_device_id ad7280_id[] = {
	{"ad7280a", 0},
	{}
};
MODULE_DEVICE_TABLE(spi, ad7280_id);

static struct spi_driver ad7280_driver = {
	.driver = {
		.name	= "ad7280",
	},
	.probe		= ad7280_probe,
	.id_table	= ad7280_id,
};
module_spi_driver(ad7280_driver);

MODULE_AUTHOR("Michael Hennerich <michael.hennerich@analog.com>");
MODULE_DESCRIPTION("Analog Devices AD7280A");
MODULE_LICENSE("GPL v2");
