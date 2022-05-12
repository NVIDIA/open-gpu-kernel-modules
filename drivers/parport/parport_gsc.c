// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *      Low-level parallel-support for PC-style hardware integrated in the 
 *	LASI-Controller (on GSC-Bus) for HP-PARISC Workstations
 *
 *	(C) 1999-2001 by Helge Deller <deller@gmx.de>
 * 
 * based on parport_pc.c by 
 * 	    Grant Guenther <grant@torque.net>
 * 	    Phil Blundell <philb@gnu.org>
 *          Tim Waugh <tim@cyberelk.demon.co.uk>
 *	    Jose Renau <renau@acm.org>
 *          David Campbell
 *          Andrea Arcangeli
 */

#undef DEBUG	/* undef for production */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/sysctl.h>

#include <asm/io.h>
#include <asm/dma.h>
#include <linux/uaccess.h>
#include <asm/superio.h>

#include <linux/parport.h>
#include <asm/pdc.h>
#include <asm/parisc-device.h>
#include <asm/hardware.h>
#include "parport_gsc.h"


MODULE_AUTHOR("Helge Deller <deller@gmx.de>");
MODULE_DESCRIPTION("HP-PARISC PC-style parallel port driver");
MODULE_LICENSE("GPL");


/*
 * Clear TIMEOUT BIT in EPP MODE
 *
 * This is also used in SPP detection.
 */
static int clear_epp_timeout(struct parport *pb)
{
	unsigned char r;

	if (!(parport_gsc_read_status(pb) & 0x01))
		return 1;

	/* To clear timeout some chips require double read */
	parport_gsc_read_status(pb);
	r = parport_gsc_read_status(pb);
	parport_writeb (r | 0x01, STATUS (pb)); /* Some reset by writing 1 */
	parport_writeb (r & 0xfe, STATUS (pb)); /* Others by writing 0 */
	r = parport_gsc_read_status(pb);

	return !(r & 0x01);
}

/*
 * Access functions.
 *
 * Most of these aren't static because they may be used by the
 * parport_xxx_yyy macros.  extern __inline__ versions of several
 * of these are in parport_gsc.h.
 */

void parport_gsc_init_state(struct pardevice *dev, struct parport_state *s)
{
	s->u.pc.ctr = 0xc | (dev->irq_func ? 0x10 : 0x0);
}

void parport_gsc_save_state(struct parport *p, struct parport_state *s)
{
	s->u.pc.ctr = parport_readb (CONTROL (p));
}

void parport_gsc_restore_state(struct parport *p, struct parport_state *s)
{
	parport_writeb (s->u.pc.ctr, CONTROL (p));
}

struct parport_operations parport_gsc_ops = 
{
	.write_data	= parport_gsc_write_data,
	.read_data	= parport_gsc_read_data,

	.write_control	= parport_gsc_write_control,
	.read_control	= parport_gsc_read_control,
	.frob_control	= parport_gsc_frob_control,

	.read_status	= parport_gsc_read_status,

	.enable_irq	= parport_gsc_enable_irq,
	.disable_irq	= parport_gsc_disable_irq,

	.data_forward	= parport_gsc_data_forward,
	.data_reverse	= parport_gsc_data_reverse,

	.init_state	= parport_gsc_init_state,
	.save_state	= parport_gsc_save_state,
	.restore_state	= parport_gsc_restore_state,

	.epp_write_data	= parport_ieee1284_epp_write_data,
	.epp_read_data	= parport_ieee1284_epp_read_data,
	.epp_write_addr	= parport_ieee1284_epp_write_addr,
	.epp_read_addr	= parport_ieee1284_epp_read_addr,

	.ecp_write_data	= parport_ieee1284_ecp_write_data,
	.ecp_read_data	= parport_ieee1284_ecp_read_data,
	.ecp_write_addr	= parport_ieee1284_ecp_write_addr,

	.compat_write_data 	= parport_ieee1284_write_compat,
	.nibble_read_data	= parport_ieee1284_read_nibble,
	.byte_read_data		= parport_ieee1284_read_byte,

	.owner		= THIS_MODULE,
};

/* --- Mode detection ------------------------------------- */

/*
 * Checks for port existence, all ports support SPP MODE
 */
static int parport_SPP_supported(struct parport *pb)
{
	unsigned char r, w;

	/*
	 * first clear an eventually pending EPP timeout 
	 * I (sailer@ife.ee.ethz.ch) have an SMSC chipset
	 * that does not even respond to SPP cycles if an EPP
	 * timeout is pending
	 */
	clear_epp_timeout(pb);

	/* Do a simple read-write test to make sure the port exists. */
	w = 0xc;
	parport_writeb (w, CONTROL (pb));

	/* Is there a control register that we can read from?  Some
	 * ports don't allow reads, so read_control just returns a
	 * software copy. Some ports _do_ allow reads, so bypass the
	 * software copy here.  In addition, some bits aren't
	 * writable. */
	r = parport_readb (CONTROL (pb));
	if ((r & 0xf) == w) {
		w = 0xe;
		parport_writeb (w, CONTROL (pb));
		r = parport_readb (CONTROL (pb));
		parport_writeb (0xc, CONTROL (pb));
		if ((r & 0xf) == w)
			return PARPORT_MODE_PCSPP;
	}

	/* Try the data register.  The data lines aren't tri-stated at
	 * this stage, so we expect back what we wrote. */
	w = 0xaa;
	parport_gsc_write_data (pb, w);
	r = parport_gsc_read_data (pb);
	if (r == w) {
		w = 0x55;
		parport_gsc_write_data (pb, w);
		r = parport_gsc_read_data (pb);
		if (r == w)
			return PARPORT_MODE_PCSPP;
	}

	return 0;
}

/* Detect PS/2 support.
 *
 * Bit 5 (0x20) sets the PS/2 data direction; setting this high
 * allows us to read data from the data lines.  In theory we would get back
 * 0xff but any peripheral attached to the port may drag some or all of the
 * lines down to zero.  So if we get back anything that isn't the contents
 * of the data register we deem PS/2 support to be present. 
 *
 * Some SPP ports have "half PS/2" ability - you can't turn off the line
 * drivers, but an external peripheral with sufficiently beefy drivers of
 * its own can overpower them and assert its own levels onto the bus, from
 * where they can then be read back as normal.  Ports with this property
 * and the right type of device attached are likely to fail the SPP test,
 * (as they will appear to have stuck bits) and so the fact that they might
 * be misdetected here is rather academic. 
 */

static int parport_PS2_supported(struct parport *pb)
{
	int ok = 0;
  
	clear_epp_timeout(pb);

	/* try to tri-state the buffer */
	parport_gsc_data_reverse (pb);
	
	parport_gsc_write_data(pb, 0x55);
	if (parport_gsc_read_data(pb) != 0x55) ok++;

	parport_gsc_write_data(pb, 0xaa);
	if (parport_gsc_read_data(pb) != 0xaa) ok++;

	/* cancel input mode */
	parport_gsc_data_forward (pb);

	if (ok) {
		pb->modes |= PARPORT_MODE_TRISTATE;
	} else {
		struct parport_gsc_private *priv = pb->private_data;
		priv->ctr_writable &= ~0x20;
	}

	return ok;
}


/* --- Initialisation code -------------------------------- */

struct parport *parport_gsc_probe_port(unsigned long base,
				       unsigned long base_hi, int irq,
				       int dma, struct parisc_device *padev)
{
	struct parport_gsc_private *priv;
	struct parport_operations *ops;
	struct parport tmp;
	struct parport *p = &tmp;

	priv = kzalloc (sizeof (struct parport_gsc_private), GFP_KERNEL);
	if (!priv) {
		printk(KERN_DEBUG "parport (0x%lx): no memory!\n", base);
		return NULL;
	}
	ops = kmemdup(&parport_gsc_ops, sizeof(struct parport_operations),
		      GFP_KERNEL);
	if (!ops) {
		printk(KERN_DEBUG "parport (0x%lx): no memory for ops!\n",
		       base);
		kfree (priv);
		return NULL;
	}
	priv->ctr = 0xc;
	priv->ctr_writable = 0xff;
	priv->dma_buf = NULL;
	priv->dma_handle = 0;
	p->base = base;
	p->base_hi = base_hi;
	p->irq = irq;
	p->dma = dma;
	p->modes = PARPORT_MODE_PCSPP | PARPORT_MODE_SAFEININT;
	p->ops = ops;
	p->private_data = priv;
	p->physport = p;
	if (!parport_SPP_supported (p)) {
		/* No port. */
		kfree (priv);
		kfree(ops);
		return NULL;
	}
	parport_PS2_supported (p);

	if (!(p = parport_register_port(base, PARPORT_IRQ_NONE,
					PARPORT_DMA_NONE, ops))) {
		kfree (priv);
		kfree (ops);
		return NULL;
	}

	p->dev = &padev->dev;
	p->base_hi = base_hi;
	p->modes = tmp.modes;
	p->size = (p->modes & PARPORT_MODE_EPP)?8:3;
	p->private_data = priv;

	pr_info("%s: PC-style at 0x%lx", p->name, p->base);
	p->irq = irq;
	if (p->irq == PARPORT_IRQ_AUTO) {
		p->irq = PARPORT_IRQ_NONE;
	}
	if (p->irq != PARPORT_IRQ_NONE) {
		pr_cont(", irq %d", p->irq);

		if (p->dma == PARPORT_DMA_AUTO) {
			p->dma = PARPORT_DMA_NONE;
		}
	}
	if (p->dma == PARPORT_DMA_AUTO) /* To use DMA, giving the irq
                                           is mandatory (see above) */
		p->dma = PARPORT_DMA_NONE;

	pr_cont(" [");
#define printmode(x)							\
do {									\
	if (p->modes & PARPORT_MODE_##x)				\
		pr_cont("%s%s", f++ ? "," : "", #x);			\
} while (0)
	{
		int f = 0;
		printmode(PCSPP);
		printmode(TRISTATE);
		printmode(COMPAT);
		printmode(EPP);
//		printmode(ECP);
//		printmode(DMA);
	}
#undef printmode
	pr_cont("]\n");

	if (p->irq != PARPORT_IRQ_NONE) {
		if (request_irq (p->irq, parport_irq_handler,
				 0, p->name, p)) {
			pr_warn("%s: irq %d in use, resorting to polled operation\n",
				p->name, p->irq);
			p->irq = PARPORT_IRQ_NONE;
			p->dma = PARPORT_DMA_NONE;
		}
	}

	/* Done probing.  Now put the port into a sensible start-up state. */

	parport_gsc_write_data(p, 0);
	parport_gsc_data_forward (p);

	/* Now that we've told the sharing engine about the port, and
	   found out its characteristics, let the high-level drivers
	   know about it. */
	parport_announce_port (p);

	return p;
}


#define PARPORT_GSC_OFFSET 0x800

static int parport_count;

static int __init parport_init_chip(struct parisc_device *dev)
{
	struct parport *p;
	unsigned long port;

	if (!dev->irq) {
		pr_warn("IRQ not found for parallel device at 0x%llx\n",
			(unsigned long long)dev->hpa.start);
		return -ENODEV;
	}

	port = dev->hpa.start + PARPORT_GSC_OFFSET;
	
	/* some older machines with ASP-chip don't support
	 * the enhanced parport modes.
	 */
	if (boot_cpu_data.cpu_type > pcxt && !pdc_add_valid(port+4)) {

		/* Initialize bidirectional-mode (0x10) & data-tranfer-mode #1 (0x20) */
		pr_info("%s: initialize bidirectional-mode\n", __func__);
		parport_writeb ( (0x10 + 0x20), port + 4);

	} else {
		pr_info("%s: enhanced parport-modes not supported\n", __func__);
	}
	
	p = parport_gsc_probe_port(port, 0, dev->irq,
			/* PARPORT_IRQ_NONE */ PARPORT_DMA_NONE, dev);
	if (p)
		parport_count++;
	dev_set_drvdata(&dev->dev, p);

	return 0;
}

static int __exit parport_remove_chip(struct parisc_device *dev)
{
	struct parport *p = dev_get_drvdata(&dev->dev);
	if (p) {
		struct parport_gsc_private *priv = p->private_data;
		struct parport_operations *ops = p->ops;
		parport_remove_port(p);
		if (p->dma != PARPORT_DMA_NONE)
			free_dma(p->dma);
		if (p->irq != PARPORT_IRQ_NONE)
			free_irq(p->irq, p);
		if (priv->dma_buf)
			pci_free_consistent(priv->dev, PAGE_SIZE,
					    priv->dma_buf,
					    priv->dma_handle);
		kfree (p->private_data);
		parport_put_port(p);
		kfree (ops); /* hope no-one cached it */
	}
	return 0;
}

static const struct parisc_device_id parport_tbl[] __initconst = {
	{ HPHW_FIO, HVERSION_REV_ANY_ID, HVERSION_ANY_ID, 0x74 },
	{ 0, }
};

MODULE_DEVICE_TABLE(parisc, parport_tbl);

static struct parisc_driver parport_driver __refdata = {
	.name		= "Parallel",
	.id_table	= parport_tbl,
	.probe		= parport_init_chip,
	.remove		= __exit_p(parport_remove_chip),
};

int parport_gsc_init(void)
{
	return register_parisc_driver(&parport_driver);
}

static void parport_gsc_exit(void)
{
	unregister_parisc_driver(&parport_driver);
}

module_init(parport_gsc_init);
module_exit(parport_gsc_exit);
