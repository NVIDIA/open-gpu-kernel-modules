// SPDX-License-Identifier: GPL-2.0
/*
 * comedi/drivers/dt2801.c
 * Device Driver for DataTranslation DT2801
 *
 */
/*
 * Driver: dt2801
 * Description: Data Translation DT2801 series and DT01-EZ
 * Author: ds
 * Status: works
 * Devices: [Data Translation] DT2801 (dt2801), DT2801-A, DT2801/5716A,
 * DT2805, DT2805/5716A, DT2808, DT2818, DT2809, DT01-EZ
 *
 * This driver can autoprobe the type of board.
 *
 * Configuration options:
 * [0] - I/O port base address
 * [1] - unused
 * [2] - A/D reference 0=differential, 1=single-ended
 * [3] - A/D range
 *	  0 = [-10, 10]
 *	  1 = [0,10]
 * [4] - D/A 0 range
 *	  0 = [-10, 10]
 *	  1 = [-5,5]
 *	  2 = [-2.5,2.5]
 *	  3 = [0,10]
 *	  4 = [0,5]
 * [5] - D/A 1 range (same choices)
 */

#include <linux/module.h>
#include "../comedidev.h"
#include <linux/delay.h>

#define DT2801_TIMEOUT 1000

/* Hardware Configuration */
/* ====================== */

#define DT2801_MAX_DMA_SIZE (64 * 1024)

/* define's */
/* ====================== */

/* Commands */
#define DT_C_RESET       0x0
#define DT_C_CLEAR_ERR   0x1
#define DT_C_READ_ERRREG 0x2
#define DT_C_SET_CLOCK   0x3

#define DT_C_TEST        0xb
#define DT_C_STOP        0xf

#define DT_C_SET_DIGIN   0x4
#define DT_C_SET_DIGOUT  0x5
#define DT_C_READ_DIG    0x6
#define DT_C_WRITE_DIG   0x7

#define DT_C_WRITE_DAIM  0x8
#define DT_C_SET_DA      0x9
#define DT_C_WRITE_DA    0xa

#define DT_C_READ_ADIM   0xc
#define DT_C_SET_AD      0xd
#define DT_C_READ_AD     0xe

/*
 * Command modifiers (only used with read/write), EXTTRIG can be
 * used with some other commands.
 */
#define DT_MOD_DMA     BIT(4)
#define DT_MOD_CONT    BIT(5)
#define DT_MOD_EXTCLK  BIT(6)
#define DT_MOD_EXTTRIG BIT(7)

/* Bits in status register */
#define DT_S_DATA_OUT_READY   BIT(0)
#define DT_S_DATA_IN_FULL     BIT(1)
#define DT_S_READY            BIT(2)
#define DT_S_COMMAND          BIT(3)
#define DT_S_COMPOSITE_ERROR  BIT(7)

/* registers */
#define DT2801_DATA		0
#define DT2801_STATUS		1
#define DT2801_CMD		1

#if 0
/* ignore 'defined but not used' warning */
static const struct comedi_lrange range_dt2801_ai_pgh_bipolar = {
	4, {
		BIP_RANGE(10),
		BIP_RANGE(5),
		BIP_RANGE(2.5),
		BIP_RANGE(1.25)
	}
};
#endif
static const struct comedi_lrange range_dt2801_ai_pgl_bipolar = {
	4, {
		BIP_RANGE(10),
		BIP_RANGE(1),
		BIP_RANGE(0.1),
		BIP_RANGE(0.02)
	}
};

#if 0
/* ignore 'defined but not used' warning */
static const struct comedi_lrange range_dt2801_ai_pgh_unipolar = {
	4, {
		UNI_RANGE(10),
		UNI_RANGE(5),
		UNI_RANGE(2.5),
		UNI_RANGE(1.25)
	}
};
#endif
static const struct comedi_lrange range_dt2801_ai_pgl_unipolar = {
	4, {
		UNI_RANGE(10),
		UNI_RANGE(1),
		UNI_RANGE(0.1),
		UNI_RANGE(0.02)
	}
};

struct dt2801_board {
	const char *name;
	int boardcode;
	int ad_diff;
	int ad_chan;
	int adbits;
	int adrangetype;
	int dabits;
};

/*
 * Typeid's for the different boards of the DT2801-series
 * (taken from the test-software, that comes with the board)
 */
static const struct dt2801_board boardtypes[] = {
	{
	 .name = "dt2801",
	 .boardcode = 0x09,
	 .ad_diff = 2,
	 .ad_chan = 16,
	 .adbits = 12,
	 .adrangetype = 0,
	 .dabits = 12},
	{
	 .name = "dt2801-a",
	 .boardcode = 0x52,
	 .ad_diff = 2,
	 .ad_chan = 16,
	 .adbits = 12,
	 .adrangetype = 0,
	 .dabits = 12},
	{
	 .name = "dt2801/5716a",
	 .boardcode = 0x82,
	 .ad_diff = 1,
	 .ad_chan = 16,
	 .adbits = 16,
	 .adrangetype = 1,
	 .dabits = 12},
	{
	 .name = "dt2805",
	 .boardcode = 0x12,
	 .ad_diff = 1,
	 .ad_chan = 16,
	 .adbits = 12,
	 .adrangetype = 0,
	 .dabits = 12},
	{
	 .name = "dt2805/5716a",
	 .boardcode = 0x92,
	 .ad_diff = 1,
	 .ad_chan = 16,
	 .adbits = 16,
	 .adrangetype = 1,
	 .dabits = 12},
	{
	 .name = "dt2808",
	 .boardcode = 0x20,
	 .ad_diff = 0,
	 .ad_chan = 16,
	 .adbits = 12,
	 .adrangetype = 2,
	 .dabits = 8},
	{
	 .name = "dt2818",
	 .boardcode = 0xa2,
	 .ad_diff = 0,
	 .ad_chan = 4,
	 .adbits = 12,
	 .adrangetype = 0,
	 .dabits = 12},
	{
	 .name = "dt2809",
	 .boardcode = 0xb0,
	 .ad_diff = 0,
	 .ad_chan = 8,
	 .adbits = 12,
	 .adrangetype = 1,
	 .dabits = 12},
};

struct dt2801_private {
	const struct comedi_lrange *dac_range_types[2];
};

/*
 * These are the low-level routines:
 * writecommand: write a command to the board
 * writedata: write data byte
 * readdata: read data byte
 */

/*
 * Only checks DataOutReady-flag, not the Ready-flag as it is done
 *  in the examples of the manual. I don't see why this should be
 *  necessary.
 */
static int dt2801_readdata(struct comedi_device *dev, int *data)
{
	int stat = 0;
	int timeout = DT2801_TIMEOUT;

	do {
		stat = inb_p(dev->iobase + DT2801_STATUS);
		if (stat & (DT_S_COMPOSITE_ERROR | DT_S_READY))
			return stat;
		if (stat & DT_S_DATA_OUT_READY) {
			*data = inb_p(dev->iobase + DT2801_DATA);
			return 0;
		}
	} while (--timeout > 0);

	return -ETIME;
}

static int dt2801_readdata2(struct comedi_device *dev, int *data)
{
	int lb = 0;
	int hb = 0;
	int ret;

	ret = dt2801_readdata(dev, &lb);
	if (ret)
		return ret;
	ret = dt2801_readdata(dev, &hb);
	if (ret)
		return ret;

	*data = (hb << 8) + lb;
	return 0;
}

static int dt2801_writedata(struct comedi_device *dev, unsigned int data)
{
	int stat = 0;
	int timeout = DT2801_TIMEOUT;

	do {
		stat = inb_p(dev->iobase + DT2801_STATUS);

		if (stat & DT_S_COMPOSITE_ERROR)
			return stat;
		if (!(stat & DT_S_DATA_IN_FULL)) {
			outb_p(data & 0xff, dev->iobase + DT2801_DATA);
			return 0;
		}
	} while (--timeout > 0);

	return -ETIME;
}

static int dt2801_writedata2(struct comedi_device *dev, unsigned int data)
{
	int ret;

	ret = dt2801_writedata(dev, data & 0xff);
	if (ret < 0)
		return ret;
	ret = dt2801_writedata(dev, data >> 8);
	if (ret < 0)
		return ret;

	return 0;
}

static int dt2801_wait_for_ready(struct comedi_device *dev)
{
	int timeout = DT2801_TIMEOUT;
	int stat;

	stat = inb_p(dev->iobase + DT2801_STATUS);
	if (stat & DT_S_READY)
		return 0;
	do {
		stat = inb_p(dev->iobase + DT2801_STATUS);

		if (stat & DT_S_COMPOSITE_ERROR)
			return stat;
		if (stat & DT_S_READY)
			return 0;
	} while (--timeout > 0);

	return -ETIME;
}

static void dt2801_writecmd(struct comedi_device *dev, int command)
{
	int stat;

	dt2801_wait_for_ready(dev);

	stat = inb_p(dev->iobase + DT2801_STATUS);
	if (stat & DT_S_COMPOSITE_ERROR) {
		dev_dbg(dev->class_dev,
			"composite-error in %s, ignoring\n", __func__);
	}
	if (!(stat & DT_S_READY))
		dev_dbg(dev->class_dev, "!ready in %s, ignoring\n", __func__);
	outb_p(command, dev->iobase + DT2801_CMD);
}

static int dt2801_reset(struct comedi_device *dev)
{
	int board_code = 0;
	unsigned int stat;
	int timeout;

	/* pull random data from data port */
	inb_p(dev->iobase + DT2801_DATA);
	inb_p(dev->iobase + DT2801_DATA);
	inb_p(dev->iobase + DT2801_DATA);
	inb_p(dev->iobase + DT2801_DATA);

	/* dt2801_writecmd(dev,DT_C_STOP); */
	outb_p(DT_C_STOP, dev->iobase + DT2801_CMD);

	/* dt2801_wait_for_ready(dev); */
	usleep_range(100, 200);
	timeout = 10000;
	do {
		stat = inb_p(dev->iobase + DT2801_STATUS);
		if (stat & DT_S_READY)
			break;
	} while (timeout--);
	if (!timeout)
		dev_dbg(dev->class_dev, "timeout 1 status=0x%02x\n", stat);

	/* dt2801_readdata(dev,&board_code); */

	outb_p(DT_C_RESET, dev->iobase + DT2801_CMD);
	/* dt2801_writecmd(dev,DT_C_RESET); */

	usleep_range(100, 200);
	timeout = 10000;
	do {
		stat = inb_p(dev->iobase + DT2801_STATUS);
		if (stat & DT_S_READY)
			break;
	} while (timeout--);
	if (!timeout)
		dev_dbg(dev->class_dev, "timeout 2 status=0x%02x\n", stat);

	dt2801_readdata(dev, &board_code);

	return board_code;
}

static int probe_number_of_ai_chans(struct comedi_device *dev)
{
	int n_chans;
	int stat;
	int data;

	for (n_chans = 0; n_chans < 16; n_chans++) {
		dt2801_writecmd(dev, DT_C_READ_ADIM);
		dt2801_writedata(dev, 0);
		dt2801_writedata(dev, n_chans);
		stat = dt2801_readdata2(dev, &data);

		if (stat)
			break;
	}

	dt2801_reset(dev);
	dt2801_reset(dev);

	return n_chans;
}

static const struct comedi_lrange *dac_range_table[] = {
	&range_bipolar10,
	&range_bipolar5,
	&range_bipolar2_5,
	&range_unipolar10,
	&range_unipolar5
};

static const struct comedi_lrange *dac_range_lkup(int opt)
{
	if (opt < 0 || opt >= 5)
		return &range_unknown;
	return dac_range_table[opt];
}

static const struct comedi_lrange *ai_range_lkup(int type, int opt)
{
	switch (type) {
	case 0:
		return (opt) ?
		    &range_dt2801_ai_pgl_unipolar :
		    &range_dt2801_ai_pgl_bipolar;
	case 1:
		return (opt) ? &range_unipolar10 : &range_bipolar10;
	case 2:
		return &range_unipolar5;
	}
	return &range_unknown;
}

static int dt2801_error(struct comedi_device *dev, int stat)
{
	if (stat < 0) {
		if (stat == -ETIME)
			dev_dbg(dev->class_dev, "timeout\n");
		else
			dev_dbg(dev->class_dev, "error %d\n", stat);
		return stat;
	}
	dev_dbg(dev->class_dev, "error status 0x%02x, resetting...\n", stat);

	dt2801_reset(dev);
	dt2801_reset(dev);

	return -EIO;
}

static int dt2801_ai_insn_read(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	int d;
	int stat;
	int i;

	for (i = 0; i < insn->n; i++) {
		dt2801_writecmd(dev, DT_C_READ_ADIM);
		dt2801_writedata(dev, CR_RANGE(insn->chanspec));
		dt2801_writedata(dev, CR_CHAN(insn->chanspec));
		stat = dt2801_readdata2(dev, &d);

		if (stat != 0)
			return dt2801_error(dev, stat);

		data[i] = d;
	}

	return i;
}

static int dt2801_ao_insn_write(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn,
				unsigned int *data)
{
	unsigned int chan = CR_CHAN(insn->chanspec);

	dt2801_writecmd(dev, DT_C_WRITE_DAIM);
	dt2801_writedata(dev, chan);
	dt2801_writedata2(dev, data[0]);

	s->readback[chan] = data[0];

	return 1;
}

static int dt2801_dio_insn_bits(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn,
				unsigned int *data)
{
	int which = (s == &dev->subdevices[3]) ? 1 : 0;
	unsigned int val = 0;

	if (comedi_dio_update_state(s, data)) {
		dt2801_writecmd(dev, DT_C_WRITE_DIG);
		dt2801_writedata(dev, which);
		dt2801_writedata(dev, s->state);
	}

	dt2801_writecmd(dev, DT_C_READ_DIG);
	dt2801_writedata(dev, which);
	dt2801_readdata(dev, &val);

	data[1] = val;

	return insn->n;
}

static int dt2801_dio_insn_config(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn,
				  unsigned int *data)
{
	int ret;

	ret = comedi_dio_insn_config(dev, s, insn, data, 0xff);
	if (ret)
		return ret;

	dt2801_writecmd(dev, s->io_bits ? DT_C_SET_DIGOUT : DT_C_SET_DIGIN);
	dt2801_writedata(dev, (s == &dev->subdevices[3]) ? 1 : 0);

	return insn->n;
}

/*
 * options:
 *	[0] - i/o base
 *	[1] - unused
 *	[2] - a/d 0=differential, 1=single-ended
 *	[3] - a/d range 0=[-10,10], 1=[0,10]
 *	[4] - dac0 range 0=[-10,10], 1=[-5,5], 2=[-2.5,2.5] 3=[0,10], 4=[0,5]
 *	[5] - dac1 range 0=[-10,10], 1=[-5,5], 2=[-2.5,2.5] 3=[0,10], 4=[0,5]
 */
static int dt2801_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	const struct dt2801_board *board;
	struct dt2801_private *devpriv;
	struct comedi_subdevice *s;
	int board_code, type;
	int ret = 0;
	int n_ai_chans;

	ret = comedi_request_region(dev, it->options[0], 0x2);
	if (ret)
		return ret;

	/* do some checking */

	board_code = dt2801_reset(dev);

	/* heh.  if it didn't work, try it again. */
	if (!board_code)
		board_code = dt2801_reset(dev);

	for (type = 0; type < ARRAY_SIZE(boardtypes); type++) {
		if (boardtypes[type].boardcode == board_code)
			goto havetype;
	}
	dev_dbg(dev->class_dev,
		"unrecognized board code=0x%02x, contact author\n", board_code);
	type = 0;

havetype:
	dev->board_ptr = boardtypes + type;
	board = dev->board_ptr;

	n_ai_chans = probe_number_of_ai_chans(dev);

	ret = comedi_alloc_subdevices(dev, 4);
	if (ret)
		goto out;

	devpriv = comedi_alloc_devpriv(dev, sizeof(*devpriv));
	if (!devpriv)
		return -ENOMEM;

	dev->board_name = board->name;

	s = &dev->subdevices[0];
	/* ai subdevice */
	s->type = COMEDI_SUBD_AI;
	s->subdev_flags = SDF_READABLE | SDF_GROUND;
#if 1
	s->n_chan = n_ai_chans;
#else
	if (it->options[2])
		s->n_chan = board->ad_chan;
	else
		s->n_chan = board->ad_chan / 2;
#endif
	s->maxdata = (1 << board->adbits) - 1;
	s->range_table = ai_range_lkup(board->adrangetype, it->options[3]);
	s->insn_read = dt2801_ai_insn_read;

	s = &dev->subdevices[1];
	/* ao subdevice */
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = 2;
	s->maxdata = (1 << board->dabits) - 1;
	s->range_table_list = devpriv->dac_range_types;
	devpriv->dac_range_types[0] = dac_range_lkup(it->options[4]);
	devpriv->dac_range_types[1] = dac_range_lkup(it->options[5]);
	s->insn_write = dt2801_ao_insn_write;

	ret = comedi_alloc_subdev_readback(s);
	if (ret)
		return ret;

	s = &dev->subdevices[2];
	/* 1st digital subdevice */
	s->type = COMEDI_SUBD_DIO;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
	s->n_chan = 8;
	s->maxdata = 1;
	s->range_table = &range_digital;
	s->insn_bits = dt2801_dio_insn_bits;
	s->insn_config = dt2801_dio_insn_config;

	s = &dev->subdevices[3];
	/* 2nd digital subdevice */
	s->type = COMEDI_SUBD_DIO;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
	s->n_chan = 8;
	s->maxdata = 1;
	s->range_table = &range_digital;
	s->insn_bits = dt2801_dio_insn_bits;
	s->insn_config = dt2801_dio_insn_config;

	ret = 0;
out:
	return ret;
}

static struct comedi_driver dt2801_driver = {
	.driver_name	= "dt2801",
	.module		= THIS_MODULE,
	.attach		= dt2801_attach,
	.detach		= comedi_legacy_detach,
};
module_comedi_driver(dt2801_driver);

MODULE_AUTHOR("Comedi https://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
