// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (c) 2000-2001 Vojtech Pavlik
 *  Copyright (c) 2002 Russell King
 */

/*
 * Acorn RiscPC PS/2 keyboard controller driver for Linux/ARM
 */

/*
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/serio.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>

#include <mach/hardware.h>
#include <asm/hardware/iomd.h>

MODULE_AUTHOR("Vojtech Pavlik, Russell King");
MODULE_DESCRIPTION("Acorn RiscPC PS/2 keyboard controller driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:kart");

struct rpckbd_data {
	int tx_irq;
	int rx_irq;
};

static int rpckbd_write(struct serio *port, unsigned char val)
{
	while (!(iomd_readb(IOMD_KCTRL) & (1 << 7)))
		cpu_relax();

	iomd_writeb(val, IOMD_KARTTX);

	return 0;
}

static irqreturn_t rpckbd_rx(int irq, void *dev_id)
{
	struct serio *port = dev_id;
	unsigned int byte;
	int handled = IRQ_NONE;

	while (iomd_readb(IOMD_KCTRL) & (1 << 5)) {
		byte = iomd_readb(IOMD_KARTRX);

		serio_interrupt(port, byte, 0);
		handled = IRQ_HANDLED;
	}
	return handled;
}

static irqreturn_t rpckbd_tx(int irq, void *dev_id)
{
	return IRQ_HANDLED;
}

static int rpckbd_open(struct serio *port)
{
	struct rpckbd_data *rpckbd = port->port_data;

	/* Reset the keyboard state machine. */
	iomd_writeb(0, IOMD_KCTRL);
	iomd_writeb(8, IOMD_KCTRL);
	iomd_readb(IOMD_KARTRX);

	if (request_irq(rpckbd->rx_irq, rpckbd_rx, 0, "rpckbd", port) != 0) {
		printk(KERN_ERR "rpckbd.c: Could not allocate keyboard receive IRQ\n");
		return -EBUSY;
	}

	if (request_irq(rpckbd->tx_irq, rpckbd_tx, 0, "rpckbd", port) != 0) {
		printk(KERN_ERR "rpckbd.c: Could not allocate keyboard transmit IRQ\n");
		free_irq(rpckbd->rx_irq, port);
		return -EBUSY;
	}

	return 0;
}

static void rpckbd_close(struct serio *port)
{
	struct rpckbd_data *rpckbd = port->port_data;

	free_irq(rpckbd->rx_irq, port);
	free_irq(rpckbd->tx_irq, port);
}

/*
 * Allocate and initialize serio structure for subsequent registration
 * with serio core.
 */
static int rpckbd_probe(struct platform_device *dev)
{
	struct rpckbd_data *rpckbd;
	struct serio *serio;
	int tx_irq, rx_irq;

	rx_irq = platform_get_irq(dev, 0);
	if (rx_irq <= 0)
		return rx_irq < 0 ? rx_irq : -ENXIO;

	tx_irq = platform_get_irq(dev, 1);
	if (tx_irq <= 0)
		return tx_irq < 0 ? tx_irq : -ENXIO;

	serio = kzalloc(sizeof(struct serio), GFP_KERNEL);
	rpckbd = kzalloc(sizeof(*rpckbd), GFP_KERNEL);
	if (!serio || !rpckbd) {
		kfree(rpckbd);
		kfree(serio);
		return -ENOMEM;
	}

	rpckbd->rx_irq = rx_irq;
	rpckbd->tx_irq = tx_irq;

	serio->id.type		= SERIO_8042;
	serio->write		= rpckbd_write;
	serio->open		= rpckbd_open;
	serio->close		= rpckbd_close;
	serio->dev.parent	= &dev->dev;
	serio->port_data	= rpckbd;
	strlcpy(serio->name, "RiscPC PS/2 kbd port", sizeof(serio->name));
	strlcpy(serio->phys, "rpckbd/serio0", sizeof(serio->phys));

	platform_set_drvdata(dev, serio);
	serio_register_port(serio);
	return 0;
}

static int rpckbd_remove(struct platform_device *dev)
{
	struct serio *serio = platform_get_drvdata(dev);
	struct rpckbd_data *rpckbd = serio->port_data;

	serio_unregister_port(serio);
	kfree(rpckbd);

	return 0;
}

static struct platform_driver rpckbd_driver = {
	.probe		= rpckbd_probe,
	.remove		= rpckbd_remove,
	.driver		= {
		.name	= "kart",
	},
};
module_platform_driver(rpckbd_driver);
