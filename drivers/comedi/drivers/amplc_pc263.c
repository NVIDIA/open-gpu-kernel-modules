// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for Amplicon PC263 relay board.
 *
 * Copyright (C) 2002 MEV Ltd. <https://www.mev.co.uk/>
 *
 * COMEDI - Linux Control and Measurement Device Interface
 * Copyright (C) 2000 David A. Schleef <ds@schleef.org>
 */

/*
 * Driver: amplc_pc263
 * Description: Amplicon PC263
 * Author: Ian Abbott <abbotti@mev.co.uk>
 * Devices: [Amplicon] PC263 (pc263)
 * Updated: Fri, 12 Apr 2013 15:19:36 +0100
 * Status: works
 *
 * Configuration options:
 *   [0] - I/O port base address
 *
 * The board appears as one subdevice, with 16 digital outputs, each
 * connected to a reed-relay. Relay contacts are closed when output is 1.
 * The state of the outputs can be read.
 */

#include <linux/module.h>
#include "../comedidev.h"

/* PC263 registers */
#define PC263_DO_0_7_REG	0x00
#define PC263_DO_8_15_REG	0x01

struct pc263_board {
	const char *name;
};

static const struct pc263_board pc263_boards[] = {
	{
		.name = "pc263",
	},
};

static int pc263_do_insn_bits(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn,
			      unsigned int *data)
{
	if (comedi_dio_update_state(s, data)) {
		outb(s->state & 0xff, dev->iobase + PC263_DO_0_7_REG);
		outb((s->state >> 8) & 0xff, dev->iobase + PC263_DO_8_15_REG);
	}

	data[1] = s->state;

	return insn->n;
}

static int pc263_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_request_region(dev, it->options[0], 0x2);
	if (ret)
		return ret;

	ret = comedi_alloc_subdevices(dev, 1);
	if (ret)
		return ret;

	/* Digital Output subdevice */
	s = &dev->subdevices[0];
	s->type		= COMEDI_SUBD_DO;
	s->subdev_flags	= SDF_WRITABLE;
	s->n_chan	= 16;
	s->maxdata	= 1;
	s->range_table	= &range_digital;
	s->insn_bits	= pc263_do_insn_bits;

	/* read initial relay state */
	s->state = inb(dev->iobase + PC263_DO_0_7_REG) |
		   (inb(dev->iobase + PC263_DO_8_15_REG) << 8);

	return 0;
}

static struct comedi_driver amplc_pc263_driver = {
	.driver_name	= "amplc_pc263",
	.module		= THIS_MODULE,
	.attach		= pc263_attach,
	.detach		= comedi_legacy_detach,
	.board_name	= &pc263_boards[0].name,
	.offset		= sizeof(struct pc263_board),
	.num_names	= ARRAY_SIZE(pc263_boards),
};

module_comedi_driver(amplc_pc263_driver);

MODULE_AUTHOR("Comedi https://www.comedi.org");
MODULE_DESCRIPTION("Comedi driver for Amplicon PC263 relay board");
MODULE_LICENSE("GPL");
