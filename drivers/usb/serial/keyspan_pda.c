// SPDX-License-Identifier: GPL-2.0+
/*
 * USB Keyspan PDA / Xircom / Entrega Converter driver
 *
 * Copyright (C) 1999 - 2001 Greg Kroah-Hartman	<greg@kroah.com>
 * Copyright (C) 1999, 2000 Brian Warner	<warner@lothar.com>
 * Copyright (C) 2000 Al Borchers		<borchers@steinerpoint.com>
 * Copyright (C) 2020 Johan Hovold <johan@kernel.org>
 *
 * See Documentation/usb/usb-serial.rst for more information on using this
 * driver
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/usb/ezusb.h>

#define DRIVER_AUTHOR "Brian Warner <warner@lothar.com>, Johan Hovold <johan@kernel.org>"
#define DRIVER_DESC "USB Keyspan PDA Converter driver"

#define KEYSPAN_TX_THRESHOLD	128

struct keyspan_pda_private {
	int			tx_room;
	struct work_struct	unthrottle_work;
	struct usb_serial	*serial;
	struct usb_serial_port	*port;
};

static int keyspan_pda_write_start(struct usb_serial_port *port);

#define KEYSPAN_VENDOR_ID		0x06cd
#define KEYSPAN_PDA_FAKE_ID		0x0103
#define KEYSPAN_PDA_ID			0x0104 /* no clue */

/* For Xircom PGSDB9 and older Entrega version of the same device */
#define XIRCOM_VENDOR_ID		0x085a
#define XIRCOM_FAKE_ID			0x8027
#define XIRCOM_FAKE_ID_2		0x8025 /* "PGMFHUB" serial */
#define ENTREGA_VENDOR_ID		0x1645
#define ENTREGA_FAKE_ID			0x8093

static const struct usb_device_id id_table_combined[] = {
	{ USB_DEVICE(KEYSPAN_VENDOR_ID, KEYSPAN_PDA_FAKE_ID) },
	{ USB_DEVICE(XIRCOM_VENDOR_ID, XIRCOM_FAKE_ID) },
	{ USB_DEVICE(XIRCOM_VENDOR_ID, XIRCOM_FAKE_ID_2) },
	{ USB_DEVICE(ENTREGA_VENDOR_ID, ENTREGA_FAKE_ID) },
	{ USB_DEVICE(KEYSPAN_VENDOR_ID, KEYSPAN_PDA_ID) },
	{ }						/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, id_table_combined);

static const struct usb_device_id id_table_std[] = {
	{ USB_DEVICE(KEYSPAN_VENDOR_ID, KEYSPAN_PDA_ID) },
	{ }						/* Terminating entry */
};

static const struct usb_device_id id_table_fake[] = {
	{ USB_DEVICE(KEYSPAN_VENDOR_ID, KEYSPAN_PDA_FAKE_ID) },
	{ USB_DEVICE(XIRCOM_VENDOR_ID, XIRCOM_FAKE_ID) },
	{ USB_DEVICE(XIRCOM_VENDOR_ID, XIRCOM_FAKE_ID_2) },
	{ USB_DEVICE(ENTREGA_VENDOR_ID, ENTREGA_FAKE_ID) },
	{ }						/* Terminating entry */
};

static int keyspan_pda_get_write_room(struct keyspan_pda_private *priv)
{
	struct usb_serial_port *port = priv->port;
	struct usb_serial *serial = port->serial;
	u8 *room;
	int rc;

	room = kmalloc(1, GFP_KERNEL);
	if (!room)
		return -ENOMEM;

	rc = usb_control_msg(serial->dev,
			     usb_rcvctrlpipe(serial->dev, 0),
			     6, /* write_room */
			     USB_TYPE_VENDOR | USB_RECIP_INTERFACE
			     | USB_DIR_IN,
			     0, /* value: 0 means "remaining room" */
			     0, /* index */
			     room,
			     1,
			     2000);
	if (rc != 1) {
		if (rc >= 0)
			rc = -EIO;
		dev_dbg(&port->dev, "roomquery failed: %d\n", rc);
		goto out_free;
	}

	dev_dbg(&port->dev, "roomquery says %d\n", *room);
	rc = *room;
out_free:
	kfree(room);

	return rc;
}

static void keyspan_pda_request_unthrottle(struct work_struct *work)
{
	struct keyspan_pda_private *priv =
		container_of(work, struct keyspan_pda_private, unthrottle_work);
	struct usb_serial_port *port = priv->port;
	struct usb_serial *serial = port->serial;
	unsigned long flags;
	int result;

	dev_dbg(&port->dev, "%s\n", __func__);

	/*
	 * Ask the device to tell us when the tx buffer becomes
	 * sufficiently empty.
	 */
	result = usb_control_msg(serial->dev,
				 usb_sndctrlpipe(serial->dev, 0),
				 7, /* request_unthrottle */
				 USB_TYPE_VENDOR | USB_RECIP_INTERFACE
				 | USB_DIR_OUT,
				 KEYSPAN_TX_THRESHOLD,
				 0, /* index */
				 NULL,
				 0,
				 2000);
	if (result < 0)
		dev_dbg(&serial->dev->dev, "%s - error %d from usb_control_msg\n",
			__func__, result);
	/*
	 * Need to check available space after requesting notification in case
	 * buffer is already empty so that no notification is sent.
	 */
	result = keyspan_pda_get_write_room(priv);
	if (result > KEYSPAN_TX_THRESHOLD) {
		spin_lock_irqsave(&port->lock, flags);
		priv->tx_room = max(priv->tx_room, result);
		spin_unlock_irqrestore(&port->lock, flags);

		usb_serial_port_softint(port);
	}
}

static void keyspan_pda_rx_interrupt(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	unsigned char *data = urb->transfer_buffer;
	unsigned int len = urb->actual_length;
	int retval;
	int status = urb->status;
	struct keyspan_pda_private *priv;
	unsigned long flags;

	priv = usb_get_serial_port_data(port);

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(&urb->dev->dev, "%s - urb shutting down with status: %d\n", __func__, status);
		return;
	default:
		dev_dbg(&urb->dev->dev, "%s - nonzero urb status received: %d\n", __func__, status);
		goto exit;
	}

	if (len < 1) {
		dev_warn(&port->dev, "short message received\n");
		goto exit;
	}

	/* see if the message is data or a status interrupt */
	switch (data[0]) {
	case 0:
		 /* rest of message is rx data */
		if (len < 2)
			break;
		tty_insert_flip_string(&port->port, data + 1, len - 1);
		tty_flip_buffer_push(&port->port);
		break;
	case 1:
		/* status interrupt */
		if (len < 2) {
			dev_warn(&port->dev, "short interrupt message received\n");
			break;
		}
		dev_dbg(&port->dev, "rx int, d1=%d\n", data[1]);
		switch (data[1]) {
		case 1: /* modemline change */
			break;
		case 2: /* tx unthrottle interrupt */
			spin_lock_irqsave(&port->lock, flags);
			priv->tx_room = max(priv->tx_room, KEYSPAN_TX_THRESHOLD);
			spin_unlock_irqrestore(&port->lock, flags);

			keyspan_pda_write_start(port);

			usb_serial_port_softint(port);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval)
		dev_err(&port->dev,
			"%s - usb_submit_urb failed with result %d\n",
			__func__, retval);
}

static void keyspan_pda_rx_throttle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;

	/*
	 * Stop receiving characters. We just turn off the URB request, and
	 * let chars pile up in the device. If we're doing hardware
	 * flowcontrol, the device will signal the other end when its buffer
	 * fills up. If we're doing XON/XOFF, this would be a good time to
	 * send an XOFF, although it might make sense to foist that off upon
	 * the device too.
	 */
	usb_kill_urb(port->interrupt_in_urb);
}

static void keyspan_pda_rx_unthrottle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;

	/* just restart the receive interrupt URB */
	if (usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL))
		dev_dbg(&port->dev, "usb_submit_urb(read urb) failed\n");
}

static speed_t keyspan_pda_setbaud(struct usb_serial *serial, speed_t baud)
{
	int rc;
	int bindex;

	switch (baud) {
	case 110:
		bindex = 0;
		break;
	case 300:
		bindex = 1;
		break;
	case 1200:
		bindex = 2;
		break;
	case 2400:
		bindex = 3;
		break;
	case 4800:
		bindex = 4;
		break;
	case 9600:
		bindex = 5;
		break;
	case 19200:
		bindex = 6;
		break;
	case 38400:
		bindex = 7;
		break;
	case 57600:
		bindex = 8;
		break;
	case 115200:
		bindex = 9;
		break;
	default:
		bindex = 5;	/* Default to 9600 */
		baud = 9600;
	}

	rc = usb_control_msg(serial->dev, usb_sndctrlpipe(serial->dev, 0),
			     0, /* set baud */
			     USB_TYPE_VENDOR
			     | USB_RECIP_INTERFACE
			     | USB_DIR_OUT, /* type */
			     bindex, /* value */
			     0, /* index */
			     NULL, /* &data */
			     0, /* size */
			     2000); /* timeout */
	if (rc < 0)
		return 0;

	return baud;
}

static void keyspan_pda_break_ctl(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;
	struct usb_serial *serial = port->serial;
	int value;
	int result;

	if (break_state == -1)
		value = 1; /* start break */
	else
		value = 0; /* clear break */

	result = usb_control_msg(serial->dev, usb_sndctrlpipe(serial->dev, 0),
			4, /* set break */
			USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_OUT,
			value, 0, NULL, 0, 2000);
	if (result < 0)
		dev_dbg(&port->dev, "%s - error %d from usb_control_msg\n",
			__func__, result);
}

static void keyspan_pda_set_termios(struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios)
{
	struct usb_serial *serial = port->serial;
	speed_t speed;

	/*
	 * cflag specifies lots of stuff: number of stop bits, parity, number
	 * of data bits, baud. What can the device actually handle?:
	 * CSTOPB (1 stop bit or 2)
	 * PARENB (parity)
	 * CSIZE (5bit .. 8bit)
	 * There is minimal hw support for parity (a PSW bit seems to hold the
	 * parity of whatever is in the accumulator). The UART either deals
	 * with 10 bits (start, 8 data, stop) or 11 bits (start, 8 data,
	 * 1 special, stop). So, with firmware changes, we could do:
	 * 8N1: 10 bit
	 * 8N2: 11 bit, extra bit always (mark?)
	 * 8[EOMS]1: 11 bit, extra bit is parity
	 * 7[EOMS]1: 10 bit, b0/b7 is parity
	 * 7[EOMS]2: 11 bit, b0/b7 is parity, extra bit always (mark?)
	 *
	 * HW flow control is dictated by the tty->termios.c_cflags & CRTSCTS
	 * bit.
	 *
	 * For now, just do baud.
	 */
	speed = tty_get_baud_rate(tty);
	speed = keyspan_pda_setbaud(serial, speed);

	if (speed == 0) {
		dev_dbg(&port->dev, "can't handle requested baud rate\n");
		/* It hasn't changed so.. */
		speed = tty_termios_baud_rate(old_termios);
	}
	/*
	 * Only speed can change so copy the old h/w parameters then encode
	 * the new speed.
	 */
	tty_termios_copy_hw(&tty->termios, old_termios);
	tty_encode_baud_rate(tty, speed, speed);
}

/*
 * Modem control pins: DTR and RTS are outputs and can be controlled.
 * DCD, RI, DSR, CTS are inputs and can be read. All outputs can also be
 * read. The byte passed is: DTR(b7) DCD RI DSR CTS RTS(b2) unused unused.
 */
static int keyspan_pda_get_modem_info(struct usb_serial *serial,
				      unsigned char *value)
{
	int rc;
	u8 *data;

	data = kmalloc(1, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	rc = usb_control_msg(serial->dev, usb_rcvctrlpipe(serial->dev, 0),
			     3, /* get pins */
			     USB_TYPE_VENDOR|USB_RECIP_INTERFACE|USB_DIR_IN,
			     0, 0, data, 1, 2000);
	if (rc == 1)
		*value = *data;
	else if (rc >= 0)
		rc = -EIO;

	kfree(data);
	return rc;
}

static int keyspan_pda_set_modem_info(struct usb_serial *serial,
				      unsigned char value)
{
	int rc;
	rc = usb_control_msg(serial->dev, usb_sndctrlpipe(serial->dev, 0),
			     3, /* set pins */
			     USB_TYPE_VENDOR|USB_RECIP_INTERFACE|USB_DIR_OUT,
			     value, 0, NULL, 0, 2000);
	return rc;
}

static int keyspan_pda_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct usb_serial *serial = port->serial;
	int rc;
	unsigned char status;
	int value;

	rc = keyspan_pda_get_modem_info(serial, &status);
	if (rc < 0)
		return rc;

	value = ((status & BIT(7)) ? TIOCM_DTR : 0) |
		((status & BIT(6)) ? TIOCM_CAR : 0) |
		((status & BIT(5)) ? TIOCM_RNG : 0) |
		((status & BIT(4)) ? TIOCM_DSR : 0) |
		((status & BIT(3)) ? TIOCM_CTS : 0) |
		((status & BIT(2)) ? TIOCM_RTS : 0);

	return value;
}

static int keyspan_pda_tiocmset(struct tty_struct *tty,
				unsigned int set, unsigned int clear)
{
	struct usb_serial_port *port = tty->driver_data;
	struct usb_serial *serial = port->serial;
	int rc;
	unsigned char status;

	rc = keyspan_pda_get_modem_info(serial, &status);
	if (rc < 0)
		return rc;

	if (set & TIOCM_RTS)
		status |= BIT(2);
	if (set & TIOCM_DTR)
		status |= BIT(7);

	if (clear & TIOCM_RTS)
		status &= ~BIT(2);
	if (clear & TIOCM_DTR)
		status &= ~BIT(7);
	rc = keyspan_pda_set_modem_info(serial, status);
	return rc;
}

static int keyspan_pda_write_start(struct usb_serial_port *port)
{
	struct keyspan_pda_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	struct urb *urb;
	int count;
	int room;
	int rc;

	/*
	 * Guess how much room is left in the device's ring buffer. If our
	 * write will result in no room left, ask the device to give us an
	 * interrupt when the room available rises above a threshold but also
	 * query how much room is currently available (in case our guess was
	 * too conservative and the buffer is already empty when the
	 * unthrottle work is scheduled).
	 */

	/*
	 * We might block because of:
	 * the TX urb is in-flight (wait until it completes)
	 * the device is full (wait until it says there is room)
	 */
	spin_lock_irqsave(&port->lock, flags);

	room = priv->tx_room;
	count = kfifo_len(&port->write_fifo);

	if (!test_bit(0, &port->write_urbs_free) || count == 0 || room == 0) {
		spin_unlock_irqrestore(&port->lock, flags);
		return 0;
	}
	__clear_bit(0, &port->write_urbs_free);

	if (count > room)
		count = room;
	if (count > port->bulk_out_size)
		count = port->bulk_out_size;

	urb = port->write_urb;
	count = kfifo_out(&port->write_fifo, urb->transfer_buffer, count);
	urb->transfer_buffer_length = count;

	port->tx_bytes += count;
	priv->tx_room -= count;

	spin_unlock_irqrestore(&port->lock, flags);

	dev_dbg(&port->dev, "%s - count = %d, txroom = %d\n", __func__, count, room);

	rc = usb_submit_urb(urb, GFP_ATOMIC);
	if (rc) {
		dev_dbg(&port->dev, "usb_submit_urb(write bulk) failed\n");

		spin_lock_irqsave(&port->lock, flags);
		port->tx_bytes -= count;
		priv->tx_room = max(priv->tx_room, room + count);
		__set_bit(0, &port->write_urbs_free);
		spin_unlock_irqrestore(&port->lock, flags);

		return rc;
	}

	if (count == room)
		schedule_work(&priv->unthrottle_work);

	return count;
}

static void keyspan_pda_write_bulk_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);
	port->tx_bytes -= urb->transfer_buffer_length;
	__set_bit(0, &port->write_urbs_free);
	spin_unlock_irqrestore(&port->lock, flags);

	keyspan_pda_write_start(port);

	usb_serial_port_softint(port);
}

static int keyspan_pda_write(struct tty_struct *tty, struct usb_serial_port *port,
		const unsigned char *buf, int count)
{
	int rc;

	dev_dbg(&port->dev, "%s - count = %d\n", __func__, count);

	if (!count)
		return 0;

	count = kfifo_in_locked(&port->write_fifo, buf, count, &port->lock);

	rc = keyspan_pda_write_start(port);
	if (rc)
		return rc;

	return count;
}

static void keyspan_pda_dtr_rts(struct usb_serial_port *port, int on)
{
	struct usb_serial *serial = port->serial;

	if (on)
		keyspan_pda_set_modem_info(serial, BIT(7) | BIT(2));
	else
		keyspan_pda_set_modem_info(serial, 0);
}


static int keyspan_pda_open(struct tty_struct *tty,
					struct usb_serial_port *port)
{
	struct keyspan_pda_private *priv = usb_get_serial_port_data(port);
	int rc;

	/* find out how much room is in the Tx ring */
	rc = keyspan_pda_get_write_room(priv);
	if (rc < 0)
		return rc;

	spin_lock_irq(&port->lock);
	priv->tx_room = rc;
	spin_unlock_irq(&port->lock);

	rc = usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);
	if (rc) {
		dev_dbg(&port->dev, "%s - usb_submit_urb(read int) failed\n", __func__);
		return rc;
	}

	return 0;
}

static void keyspan_pda_close(struct usb_serial_port *port)
{
	struct keyspan_pda_private *priv = usb_get_serial_port_data(port);

	/*
	 * Stop the interrupt URB first as its completion handler may submit
	 * the write URB.
	 */
	usb_kill_urb(port->interrupt_in_urb);
	usb_kill_urb(port->write_urb);

	cancel_work_sync(&priv->unthrottle_work);

	spin_lock_irq(&port->lock);
	kfifo_reset(&port->write_fifo);
	spin_unlock_irq(&port->lock);
}

/* download the firmware to a "fake" device (pre-renumeration) */
static int keyspan_pda_fake_startup(struct usb_serial *serial)
{
	unsigned int vid = le16_to_cpu(serial->dev->descriptor.idVendor);
	const char *fw_name;

	/* download the firmware here ... */
	ezusb_fx1_set_reset(serial->dev, 1);

	switch (vid) {
	case KEYSPAN_VENDOR_ID:
		fw_name = "keyspan_pda/keyspan_pda.fw";
		break;
	case XIRCOM_VENDOR_ID:
	case ENTREGA_VENDOR_ID:
		fw_name = "keyspan_pda/xircom_pgs.fw";
		break;
	default:
		dev_err(&serial->dev->dev, "%s: unknown vendor, aborting.\n",
			__func__);
		return -ENODEV;
	}

	if (ezusb_fx1_ihex_firmware_download(serial->dev, fw_name) < 0) {
		dev_err(&serial->dev->dev, "failed to load firmware \"%s\"\n",
			fw_name);
		return -ENOENT;
	}

	/*
	 * After downloading firmware renumeration will occur in a moment and
	 * the new device will bind to the real driver.
	 */

	/* We want this device to fail to have a driver assigned to it. */
	return 1;
}

MODULE_FIRMWARE("keyspan_pda/keyspan_pda.fw");
MODULE_FIRMWARE("keyspan_pda/xircom_pgs.fw");

static int keyspan_pda_port_probe(struct usb_serial_port *port)
{

	struct keyspan_pda_private *priv;

	priv = kmalloc(sizeof(struct keyspan_pda_private), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	INIT_WORK(&priv->unthrottle_work, keyspan_pda_request_unthrottle);
	priv->port = port;

	usb_set_serial_port_data(port, priv);

	return 0;
}

static void keyspan_pda_port_remove(struct usb_serial_port *port)
{
	struct keyspan_pda_private *priv;

	priv = usb_get_serial_port_data(port);
	kfree(priv);
}

static struct usb_serial_driver keyspan_pda_fake_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"keyspan_pda_pre",
	},
	.description =		"Keyspan PDA - (prerenumeration)",
	.id_table =		id_table_fake,
	.num_ports =		1,
	.attach =		keyspan_pda_fake_startup,
};

static struct usb_serial_driver keyspan_pda_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"keyspan_pda",
	},
	.description =		"Keyspan PDA",
	.id_table =		id_table_std,
	.num_ports =		1,
	.num_bulk_out =		1,
	.num_interrupt_in =	1,
	.dtr_rts =		keyspan_pda_dtr_rts,
	.open =			keyspan_pda_open,
	.close =		keyspan_pda_close,
	.write =		keyspan_pda_write,
	.write_bulk_callback =	keyspan_pda_write_bulk_callback,
	.read_int_callback =	keyspan_pda_rx_interrupt,
	.throttle =		keyspan_pda_rx_throttle,
	.unthrottle =		keyspan_pda_rx_unthrottle,
	.set_termios =		keyspan_pda_set_termios,
	.break_ctl =		keyspan_pda_break_ctl,
	.tiocmget =		keyspan_pda_tiocmget,
	.tiocmset =		keyspan_pda_tiocmset,
	.port_probe =		keyspan_pda_port_probe,
	.port_remove =		keyspan_pda_port_remove,
};

static struct usb_serial_driver * const serial_drivers[] = {
	&keyspan_pda_device,
	&keyspan_pda_fake_device,
	NULL
};

module_usb_serial_driver(serial_drivers, id_table_combined);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
