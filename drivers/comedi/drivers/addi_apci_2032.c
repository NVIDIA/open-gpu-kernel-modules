// SPDX-License-Identifier: GPL-2.0+
/*
 * addi_apci_2032.c
 * Copyright (C) 2004,2005  ADDI-DATA GmbH for the source code of this module.
 * Project manager: Eric Stolz
 *
 *	ADDI-DATA GmbH
 *	Dieselstrasse 3
 *	D-77833 Ottersweier
 *	Tel: +19(0)7223/9493-0
 *	Fax: +49(0)7223/9493-92
 *	http://www.addi-data.com
 *	info@addi-data.com
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#include "../comedi_pci.h"
#include "addi_watchdog.h"

/*
 * PCI bar 1 I/O Register map
 */
#define APCI2032_DO_REG			0x00
#define APCI2032_INT_CTRL_REG		0x04
#define APCI2032_INT_CTRL_VCC_ENA	BIT(0)
#define APCI2032_INT_CTRL_CC_ENA	BIT(1)
#define APCI2032_INT_STATUS_REG		0x08
#define APCI2032_INT_STATUS_VCC		BIT(0)
#define APCI2032_INT_STATUS_CC		BIT(1)
#define APCI2032_STATUS_REG		0x0c
#define APCI2032_STATUS_IRQ		BIT(0)
#define APCI2032_WDOG_REG		0x10

struct apci2032_int_private {
	spinlock_t spinlock;		/* protects the following members */
	bool active;			/* an async command is running */
	unsigned char enabled_isns;	/* mask of enabled interrupt channels */
};

static int apci2032_do_insn_bits(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 struct comedi_insn *insn,
				 unsigned int *data)
{
	s->state = inl(dev->iobase + APCI2032_DO_REG);

	if (comedi_dio_update_state(s, data))
		outl(s->state, dev->iobase + APCI2032_DO_REG);

	data[1] = s->state;

	return insn->n;
}

static int apci2032_int_insn_bits(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn,
				  unsigned int *data)
{
	data[1] = inl(dev->iobase + APCI2032_INT_STATUS_REG) & 3;
	return insn->n;
}

static void apci2032_int_stop(struct comedi_device *dev,
			      struct comedi_subdevice *s)
{
	struct apci2032_int_private *subpriv = s->private;

	subpriv->active = false;
	subpriv->enabled_isns = 0;
	outl(0x0, dev->iobase + APCI2032_INT_CTRL_REG);
}

static int apci2032_int_cmdtest(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_cmd *cmd)
{
	int err = 0;

	/* Step 1 : check if triggers are trivially valid */

	err |= comedi_check_trigger_src(&cmd->start_src, TRIG_NOW);
	err |= comedi_check_trigger_src(&cmd->scan_begin_src, TRIG_EXT);
	err |= comedi_check_trigger_src(&cmd->convert_src, TRIG_NOW);
	err |= comedi_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= comedi_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);

	if (err)
		return 1;

	/* Step 2a : make sure trigger sources are unique */
	err |= comedi_check_trigger_is_unique(cmd->stop_src);

	/* Step 2b : and mutually compatible */

	if (err)
		return 2;

	/* Step 3: check if arguments are trivially valid */

	err |= comedi_check_trigger_arg_is(&cmd->start_arg, 0);
	err |= comedi_check_trigger_arg_is(&cmd->scan_begin_arg, 0);
	err |= comedi_check_trigger_arg_is(&cmd->convert_arg, 0);
	err |= comedi_check_trigger_arg_is(&cmd->scan_end_arg,
					   cmd->chanlist_len);
	if (cmd->stop_src == TRIG_COUNT)
		err |= comedi_check_trigger_arg_min(&cmd->stop_arg, 1);
	else	/* TRIG_NONE */
		err |= comedi_check_trigger_arg_is(&cmd->stop_arg, 0);

	if (err)
		return 3;

	/* Step 4: fix up any arguments */

	/* Step 5: check channel list if it exists */

	return 0;
}

static int apci2032_int_cmd(struct comedi_device *dev,
			    struct comedi_subdevice *s)
{
	struct comedi_cmd *cmd = &s->async->cmd;
	struct apci2032_int_private *subpriv = s->private;
	unsigned char enabled_isns;
	unsigned int n;
	unsigned long flags;

	enabled_isns = 0;
	for (n = 0; n < cmd->chanlist_len; n++)
		enabled_isns |= 1 << CR_CHAN(cmd->chanlist[n]);

	spin_lock_irqsave(&subpriv->spinlock, flags);

	subpriv->enabled_isns = enabled_isns;
	subpriv->active = true;
	outl(enabled_isns, dev->iobase + APCI2032_INT_CTRL_REG);

	spin_unlock_irqrestore(&subpriv->spinlock, flags);

	return 0;
}

static int apci2032_int_cancel(struct comedi_device *dev,
			       struct comedi_subdevice *s)
{
	struct apci2032_int_private *subpriv = s->private;
	unsigned long flags;

	spin_lock_irqsave(&subpriv->spinlock, flags);
	if (subpriv->active)
		apci2032_int_stop(dev, s);
	spin_unlock_irqrestore(&subpriv->spinlock, flags);

	return 0;
}

static irqreturn_t apci2032_interrupt(int irq, void *d)
{
	struct comedi_device *dev = d;
	struct comedi_subdevice *s = dev->read_subdev;
	struct comedi_cmd *cmd = &s->async->cmd;
	struct apci2032_int_private *subpriv;
	unsigned int val;

	if (!dev->attached)
		return IRQ_NONE;

	/* Check if VCC OR CC interrupt has occurred */
	val = inl(dev->iobase + APCI2032_STATUS_REG) & APCI2032_STATUS_IRQ;
	if (!val)
		return IRQ_NONE;

	subpriv = s->private;
	spin_lock(&subpriv->spinlock);

	val = inl(dev->iobase + APCI2032_INT_STATUS_REG) & 3;
	/* Disable triggered interrupt sources. */
	outl(~val & 3, dev->iobase + APCI2032_INT_CTRL_REG);
	/*
	 * Note: We don't reenable the triggered interrupt sources because they
	 * are level-sensitive, hardware error status interrupt sources and
	 * they'd keep triggering interrupts repeatedly.
	 */

	if (subpriv->active && (val & subpriv->enabled_isns) != 0) {
		unsigned short bits = 0;
		int i;

		/* Bits in scan data correspond to indices in channel list. */
		for (i = 0; i < cmd->chanlist_len; i++) {
			unsigned int chan = CR_CHAN(cmd->chanlist[i]);

			if (val & (1 << chan))
				bits |= (1 << i);
		}

		comedi_buf_write_samples(s, &bits, 1);

		if (cmd->stop_src == TRIG_COUNT &&
		    s->async->scans_done >= cmd->stop_arg)
			s->async->events |= COMEDI_CB_EOA;
	}

	spin_unlock(&subpriv->spinlock);

	comedi_handle_events(dev, s);

	return IRQ_HANDLED;
}

static int apci2032_reset(struct comedi_device *dev)
{
	outl(0x0, dev->iobase + APCI2032_DO_REG);
	outl(0x0, dev->iobase + APCI2032_INT_CTRL_REG);

	addi_watchdog_reset(dev->iobase + APCI2032_WDOG_REG);

	return 0;
}

static int apci2032_auto_attach(struct comedi_device *dev,
				unsigned long context_unused)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;
	dev->iobase = pci_resource_start(pcidev, 1);
	apci2032_reset(dev);

	if (pcidev->irq > 0) {
		ret = request_irq(pcidev->irq, apci2032_interrupt,
				  IRQF_SHARED, dev->board_name, dev);
		if (ret == 0)
			dev->irq = pcidev->irq;
	}

	ret = comedi_alloc_subdevices(dev, 3);
	if (ret)
		return ret;

	/* Initialize the digital output subdevice */
	s = &dev->subdevices[0];
	s->type		= COMEDI_SUBD_DO;
	s->subdev_flags	= SDF_WRITABLE;
	s->n_chan	= 32;
	s->maxdata	= 1;
	s->range_table	= &range_digital;
	s->insn_bits	= apci2032_do_insn_bits;

	/* Initialize the watchdog subdevice */
	s = &dev->subdevices[1];
	ret = addi_watchdog_init(s, dev->iobase + APCI2032_WDOG_REG);
	if (ret)
		return ret;

	/* Initialize the interrupt subdevice */
	s = &dev->subdevices[2];
	s->type		= COMEDI_SUBD_DI;
	s->subdev_flags	= SDF_READABLE;
	s->n_chan	= 2;
	s->maxdata	= 1;
	s->range_table	= &range_digital;
	s->insn_bits	= apci2032_int_insn_bits;
	if (dev->irq) {
		struct apci2032_int_private *subpriv;

		dev->read_subdev = s;
		subpriv = kzalloc(sizeof(*subpriv), GFP_KERNEL);
		if (!subpriv)
			return -ENOMEM;
		spin_lock_init(&subpriv->spinlock);
		s->private	= subpriv;
		s->subdev_flags	= SDF_READABLE | SDF_CMD_READ | SDF_PACKED;
		s->len_chanlist = 2;
		s->do_cmdtest	= apci2032_int_cmdtest;
		s->do_cmd	= apci2032_int_cmd;
		s->cancel	= apci2032_int_cancel;
	}

	return 0;
}

static void apci2032_detach(struct comedi_device *dev)
{
	if (dev->iobase)
		apci2032_reset(dev);
	comedi_pci_detach(dev);
	if (dev->read_subdev)
		kfree(dev->read_subdev->private);
}

static struct comedi_driver apci2032_driver = {
	.driver_name	= "addi_apci_2032",
	.module		= THIS_MODULE,
	.auto_attach	= apci2032_auto_attach,
	.detach		= apci2032_detach,
};

static int apci2032_pci_probe(struct pci_dev *dev,
			      const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &apci2032_driver, id->driver_data);
}

static const struct pci_device_id apci2032_pci_table[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_ADDIDATA, 0x1004) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, apci2032_pci_table);

static struct pci_driver apci2032_pci_driver = {
	.name		= "addi_apci_2032",
	.id_table	= apci2032_pci_table,
	.probe		= apci2032_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(apci2032_driver, apci2032_pci_driver);

MODULE_AUTHOR("Comedi https://www.comedi.org");
MODULE_DESCRIPTION("ADDI-DATA APCI-2032, 32 channel DO boards");
MODULE_LICENSE("GPL");
