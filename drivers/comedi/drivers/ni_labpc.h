/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Header for ni_labpc ISA/PCMCIA/PCI drivers
 *
 * Copyright (C) 2003 Frank Mori Hess <fmhess@users.sourceforge.net>
 */

#ifndef _NI_LABPC_H
#define _NI_LABPC_H

enum transfer_type { fifo_not_empty_transfer, fifo_half_full_transfer,
	isa_dma_transfer
};

struct labpc_boardinfo {
	const char *name;
	int ai_speed;			/* maximum input speed in ns */
	unsigned ai_scan_up:1;		/* can auto scan up in ai channels */
	unsigned has_ao:1;		/* has analog outputs */
	unsigned is_labpc1200:1;	/* has extra regs compared to pc+ */
};

struct labpc_private {
	struct comedi_isadma *dma;
	struct comedi_8254 *counter;

	/*  number of data points left to be taken */
	unsigned long long count;
	/*  software copys of bits written to command registers */
	unsigned int cmd1;
	unsigned int cmd2;
	unsigned int cmd3;
	unsigned int cmd4;
	unsigned int cmd5;
	unsigned int cmd6;
	/*  store last read of board status registers */
	unsigned int stat1;
	unsigned int stat2;

	/* we are using dma/fifo-half-full/etc. */
	enum transfer_type current_transfer;
	/*
	 * function pointers so we can use inb/outb or readb/writeb as
	 * appropriate
	 */
	unsigned int (*read_byte)(struct comedi_device *dev, unsigned long reg);
	void (*write_byte)(struct comedi_device *dev,
			   unsigned int byte, unsigned long reg);
};

int labpc_common_attach(struct comedi_device *dev,
			unsigned int irq, unsigned long isr_flags);
void labpc_common_detach(struct comedi_device *dev);

#endif /* _NI_LABPC_H */
