// SPDX-License-Identifier: GPL-2.0+
/*
 * fl512.c
 * Anders Gnistrup <ex18@kalman.iau.dtu.dk>
 *
 * COMEDI - Linux Control and Measurement Device Interface
 * Copyright (C) 2000 David A. Schleef <ds@schleef.org>
 */

/*
 * Driver: fl512
 * Description: unknown
 * Author: Anders Gnistrup <ex18@kalman.iau.dtu.dk>
 * Devices: [unknown] FL512 (fl512)
 * Status: unknown
 *
 * Digital I/O is not supported.
 *
 * Configuration options:
 *   [0] - I/O port base address
 */

#include <linux/module.h>
#include "../comedidev.h"

#include <linux/delay.h>

/*
 * Register I/O map
 */
#define FL512_AI_LSB_REG		0x02
#define FL512_AI_MSB_REG		0x03
#define FL512_AI_MUX_REG		0x02
#define FL512_AI_START_CONV_REG		0x03
#define FL512_AO_DATA_REG(x)		(0x04 + ((x) * 2))
#define FL512_AO_TRIG_REG(x)		(0x04 + ((x) * 2))

static const struct comedi_lrange range_fl512 = {
	4, {
		BIP_RANGE(0.5),
		BIP_RANGE(1),
		BIP_RANGE(5),
		BIP_RANGE(10),
		UNI_RANGE(1),
		UNI_RANGE(5),
		UNI_RANGE(10)
	}
};

static int fl512_ai_insn_read(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn,
			      unsigned int *data)
{
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int val;
	int i;

	outb(chan, dev->iobase + FL512_AI_MUX_REG);

	for (i = 0; i < insn->n; i++) {
		outb(0, dev->iobase + FL512_AI_START_CONV_REG);

		/* XXX should test "done" flag instead of delay */
		usleep_range(30, 100);

		val = inb(dev->iobase + FL512_AI_LSB_REG);
		val |= (inb(dev->iobase + FL512_AI_MSB_REG) << 8);
		val &= s->maxdata;

		data[i] = val;
	}

	return insn->n;
}

static int fl512_ao_insn_write(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn,
			       unsigned int *data)
{
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int val = s->readback[chan];
	int i;

	for (i = 0; i < insn->n; i++) {
		val = data[i];

		/* write LSB, MSB then trigger conversion */
		outb(val & 0x0ff, dev->iobase + FL512_AO_DATA_REG(chan));
		outb((val >> 8) & 0xf, dev->iobase + FL512_AO_DATA_REG(chan));
		inb(dev->iobase + FL512_AO_TRIG_REG(chan));
	}
	s->readback[chan] = val;

	return insn->n;
}

static int fl512_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_request_region(dev, it->options[0], 0x10);
	if (ret)
		return ret;

	ret = comedi_alloc_subdevices(dev, 2);
	if (ret)
		return ret;

	/* Analog Input subdevice */
	s = &dev->subdevices[0];
	s->type		= COMEDI_SUBD_AI;
	s->subdev_flags	= SDF_READABLE | SDF_GROUND;
	s->n_chan	= 16;
	s->maxdata	= 0x0fff;
	s->range_table	= &range_fl512;
	s->insn_read	= fl512_ai_insn_read;

	/* Analog Output subdevice */
	s = &dev->subdevices[1];
	s->type		= COMEDI_SUBD_AO;
	s->subdev_flags	= SDF_WRITABLE;
	s->n_chan	= 2;
	s->maxdata	= 0x0fff;
	s->range_table	= &range_fl512;
	s->insn_write	= fl512_ao_insn_write;

	return comedi_alloc_subdev_readback(s);
}

static struct comedi_driver fl512_driver = {
	.driver_name	= "fl512",
	.module		= THIS_MODULE,
	.attach		= fl512_attach,
	.detach		= comedi_legacy_detach,
};
module_comedi_driver(fl512_driver);

MODULE_AUTHOR("Comedi https://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
