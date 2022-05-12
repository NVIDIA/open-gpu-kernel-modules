// SPDX-License-Identifier: GPL-2.0+
/*
 * comedi/drivers/dyna_pci10xx.c
 * Copyright (C) 2011 Prashant Shah, pshah.mumbai@gmail.com
 */

/*
 * Driver: dyna_pci10xx
 * Description: Dynalog India PCI DAQ Cards, http://www.dynalogindia.com/
 * Devices: [Dynalog] PCI-1050 (dyna_pci1050)
 * Author: Prashant Shah <pshah.mumbai@gmail.com>
 * Status: Stable
 *
 * Developed at Automation Labs, Chemical Dept., IIT Bombay, India.
 * Prof. Kannan Moudgalya <kannan@iitb.ac.in>
 * http://www.iitb.ac.in
 *
 * Notes :
 * - Dynalog India Pvt. Ltd. does not have a registered PCI Vendor ID and
 *   they are using the PLX Technlogies Vendor ID since that is the PCI Chip
 *   used in the card.
 * - Dynalog India Pvt. Ltd. has provided the internal register specification
 *   for their cards in their manuals.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/mutex.h>

#include "../comedi_pci.h"

#define READ_TIMEOUT 50

static const struct comedi_lrange range_pci1050_ai = {
	3, {
		BIP_RANGE(10),
		BIP_RANGE(5),
		UNI_RANGE(10)
	}
};

static const char range_codes_pci1050_ai[] = { 0x00, 0x10, 0x30 };

struct dyna_pci10xx_private {
	struct mutex mutex;
	unsigned long BADR3;
};

static int dyna_pci10xx_ai_eoc(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn,
			       unsigned long context)
{
	unsigned int status;

	status = inw_p(dev->iobase);
	if (status & BIT(15))
		return 0;
	return -EBUSY;
}

static int dyna_pci10xx_insn_read_ai(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
{
	struct dyna_pci10xx_private *devpriv = dev->private;
	int n;
	u16 d = 0;
	int ret = 0;
	unsigned int chan, range;

	/* get the channel number and range */
	chan = CR_CHAN(insn->chanspec);
	range = range_codes_pci1050_ai[CR_RANGE((insn->chanspec))];

	mutex_lock(&devpriv->mutex);
	/* convert n samples */
	for (n = 0; n < insn->n; n++) {
		/* trigger conversion */
		smp_mb();
		outw_p(0x0000 + range + chan, dev->iobase + 2);
		usleep_range(10, 20);

		ret = comedi_timeout(dev, s, insn, dyna_pci10xx_ai_eoc, 0);
		if (ret)
			break;

		/* read data */
		d = inw_p(dev->iobase);
		/* mask the first 4 bits - EOC bits */
		d &= 0x0FFF;
		data[n] = d;
	}
	mutex_unlock(&devpriv->mutex);

	/* return the number of samples read/written */
	return ret ? ret : n;
}

/* analog output callback */
static int dyna_pci10xx_insn_write_ao(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      struct comedi_insn *insn,
				      unsigned int *data)
{
	struct dyna_pci10xx_private *devpriv = dev->private;
	int n;

	mutex_lock(&devpriv->mutex);
	for (n = 0; n < insn->n; n++) {
		smp_mb();
		/* trigger conversion and write data */
		outw_p(data[n], dev->iobase);
		usleep_range(10, 20);
	}
	mutex_unlock(&devpriv->mutex);
	return n;
}

/* digital input bit interface */
static int dyna_pci10xx_di_insn_bits(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
{
	struct dyna_pci10xx_private *devpriv = dev->private;
	u16 d = 0;

	mutex_lock(&devpriv->mutex);
	smp_mb();
	d = inw_p(devpriv->BADR3);
	usleep_range(10, 100);

	/* on return the data[0] contains output and data[1] contains input */
	data[1] = d;
	data[0] = s->state;
	mutex_unlock(&devpriv->mutex);
	return insn->n;
}

static int dyna_pci10xx_do_insn_bits(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
{
	struct dyna_pci10xx_private *devpriv = dev->private;

	mutex_lock(&devpriv->mutex);
	if (comedi_dio_update_state(s, data)) {
		smp_mb();
		outw_p(s->state, devpriv->BADR3);
		usleep_range(10, 100);
	}

	data[1] = s->state;
	mutex_unlock(&devpriv->mutex);

	return insn->n;
}

static int dyna_pci10xx_auto_attach(struct comedi_device *dev,
				    unsigned long context_unused)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	struct dyna_pci10xx_private *devpriv;
	struct comedi_subdevice *s;
	int ret;

	devpriv = comedi_alloc_devpriv(dev, sizeof(*devpriv));
	if (!devpriv)
		return -ENOMEM;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;
	dev->iobase = pci_resource_start(pcidev, 2);
	devpriv->BADR3 = pci_resource_start(pcidev, 3);

	mutex_init(&devpriv->mutex);

	ret = comedi_alloc_subdevices(dev, 4);
	if (ret)
		return ret;

	/* analog input */
	s = &dev->subdevices[0];
	s->type = COMEDI_SUBD_AI;
	s->subdev_flags = SDF_READABLE | SDF_GROUND | SDF_DIFF;
	s->n_chan = 16;
	s->maxdata = 0x0FFF;
	s->range_table = &range_pci1050_ai;
	s->insn_read = dyna_pci10xx_insn_read_ai;

	/* analog output */
	s = &dev->subdevices[1];
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = 1;
	s->maxdata = 0x0FFF;
	s->range_table = &range_unipolar10;
	s->insn_write = dyna_pci10xx_insn_write_ao;

	/* digital input */
	s = &dev->subdevices[2];
	s->type = COMEDI_SUBD_DI;
	s->subdev_flags = SDF_READABLE;
	s->n_chan = 16;
	s->maxdata = 1;
	s->range_table = &range_digital;
	s->insn_bits = dyna_pci10xx_di_insn_bits;

	/* digital output */
	s = &dev->subdevices[3];
	s->type = COMEDI_SUBD_DO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = 16;
	s->maxdata = 1;
	s->range_table = &range_digital;
	s->state = 0;
	s->insn_bits = dyna_pci10xx_do_insn_bits;

	return 0;
}

static void dyna_pci10xx_detach(struct comedi_device *dev)
{
	struct dyna_pci10xx_private *devpriv = dev->private;

	comedi_pci_detach(dev);
	if (devpriv)
		mutex_destroy(&devpriv->mutex);
}

static struct comedi_driver dyna_pci10xx_driver = {
	.driver_name	= "dyna_pci10xx",
	.module		= THIS_MODULE,
	.auto_attach	= dyna_pci10xx_auto_attach,
	.detach		= dyna_pci10xx_detach,
};

static int dyna_pci10xx_pci_probe(struct pci_dev *dev,
				  const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &dyna_pci10xx_driver,
				      id->driver_data);
}

static const struct pci_device_id dyna_pci10xx_pci_table[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_PLX, 0x1050) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, dyna_pci10xx_pci_table);

static struct pci_driver dyna_pci10xx_pci_driver = {
	.name		= "dyna_pci10xx",
	.id_table	= dyna_pci10xx_pci_table,
	.probe		= dyna_pci10xx_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(dyna_pci10xx_driver, dyna_pci10xx_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant Shah <pshah.mumbai@gmail.com>");
MODULE_DESCRIPTION("Comedi based drivers for Dynalog PCI DAQ cards");
