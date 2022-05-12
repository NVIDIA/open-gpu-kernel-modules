// SPDX-License-Identifier: GPL-2.0+
/*
 *	Serial Device Initialisation for Lasi/Asp/Wax/Dino
 *
 *	(c) Copyright Matthew Wilcox <willy@debian.org> 2001-2002
 */

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/serial_core.h>
#include <linux/signal.h>
#include <linux/types.h>

#include <asm/hardware.h>
#include <asm/parisc-device.h>
#include <asm/io.h>

#include "8250.h"

static int __init serial_init_chip(struct parisc_device *dev)
{
	struct uart_8250_port uart;
	unsigned long address;
	int err;

#ifdef CONFIG_64BIT
	if (!dev->irq && (dev->id.sversion == 0xad))
		dev->irq = iosapic_serial_irq(dev);
#endif

	if (!dev->irq) {
		/* We find some unattached serial ports by walking native
		 * busses.  These should be silently ignored.  Otherwise,
		 * what we have here is a missing parent device, so tell
		 * the user what they're missing.
		 */
		if (parisc_parent(dev)->id.hw_type != HPHW_IOA)
			dev_info(&dev->dev,
				"Serial: device 0x%llx not configured.\n"
				"Enable support for Wax, Lasi, Asp or Dino.\n",
				(unsigned long long)dev->hpa.start);
		return -ENODEV;
	}

	address = dev->hpa.start;
	if (dev->id.sversion != 0x8d)
		address += 0x800;

	memset(&uart, 0, sizeof(uart));
	uart.port.iotype	= UPIO_MEM;
	/* 7.272727MHz on Lasi.  Assumed the same for Dino, Wax and Timi. */
	uart.port.uartclk	= (dev->id.sversion != 0xad) ?
					7272727 : 1843200;
	uart.port.mapbase	= address;
	uart.port.membase	= ioremap(address, 16);
	if (!uart.port.membase) {
		dev_warn(&dev->dev, "Failed to map memory\n");
		return -ENOMEM;
	}
	uart.port.irq	= dev->irq;
	uart.port.flags	= UPF_BOOT_AUTOCONF;
	uart.port.dev	= &dev->dev;

	err = serial8250_register_8250_port(&uart);
	if (err < 0) {
		dev_warn(&dev->dev,
			"serial8250_register_8250_port returned error %d\n",
			err);
		iounmap(uart.port.membase);
		return err;
	}

	return 0;
}

static const struct parisc_device_id serial_tbl[] __initconst = {
	{ HPHW_FIO, HVERSION_REV_ANY_ID, HVERSION_ANY_ID, 0x00075 },
	{ HPHW_FIO, HVERSION_REV_ANY_ID, HVERSION_ANY_ID, 0x0008c },
	{ HPHW_FIO, HVERSION_REV_ANY_ID, HVERSION_ANY_ID, 0x0008d },
	{ HPHW_FIO, HVERSION_REV_ANY_ID, HVERSION_ANY_ID, 0x000ad },
	{ 0 }
};

/* Hack.  Some machines have SERIAL_0 attached to Lasi and SERIAL_1
 * attached to Dino.  Unfortunately, Dino appears before Lasi in the device
 * tree.  To ensure that ttyS0 == SERIAL_0, we register two drivers; one
 * which only knows about Lasi and then a second which will find all the
 * other serial ports.  HPUX ignores this problem.
 */
static const struct parisc_device_id lasi_tbl[] __initconst = {
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x03B, 0x0008C }, /* C1xx/C1xxL */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x03C, 0x0008C }, /* B132L */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x03D, 0x0008C }, /* B160L */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x03E, 0x0008C }, /* B132L+ */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x03F, 0x0008C }, /* B180L+ */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x046, 0x0008C }, /* Rocky2 120 */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x047, 0x0008C }, /* Rocky2 150 */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x04E, 0x0008C }, /* Kiji L2 132 */
	{ HPHW_FIO, HVERSION_REV_ANY_ID, 0x056, 0x0008C }, /* Raven+ */
	{ 0 }
};


MODULE_DEVICE_TABLE(parisc, serial_tbl);

static struct parisc_driver lasi_driver __refdata = {
	.name		= "serial_1",
	.id_table	= lasi_tbl,
	.probe		= serial_init_chip,
};

static struct parisc_driver serial_driver __refdata = {
	.name		= "serial",
	.id_table	= serial_tbl,
	.probe		= serial_init_chip,
};

static int __init probe_serial_gsc(void)
{
	register_parisc_driver(&lasi_driver);
	register_parisc_driver(&serial_driver);
	return 0;
}

module_init(probe_serial_gsc);

MODULE_LICENSE("GPL");
