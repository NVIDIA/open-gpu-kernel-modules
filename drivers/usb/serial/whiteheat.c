// SPDX-License-Identifier: GPL-2.0+
/*
 * USB ConnectTech WhiteHEAT driver
 *
 *	Copyright (C) 2002
 *	    Connect Tech Inc.
 *
 *	Copyright (C) 1999 - 2001
 *	    Greg Kroah-Hartman (greg@kroah.com)
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
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <asm/termbits.h>
#include <linux/usb.h>
#include <linux/serial_reg.h>
#include <linux/serial.h>
#include <linux/usb/serial.h>
#include <linux/usb/ezusb.h>
#include "whiteheat.h"			/* WhiteHEAT specific commands */

#ifndef CMSPAR
#define CMSPAR 0
#endif

/*
 * Version Information
 */
#define DRIVER_AUTHOR "Greg Kroah-Hartman <greg@kroah.com>, Stuart MacDonald <stuartm@connecttech.com>"
#define DRIVER_DESC "USB ConnectTech WhiteHEAT driver"

#define CONNECT_TECH_VENDOR_ID		0x0710
#define CONNECT_TECH_FAKE_WHITE_HEAT_ID	0x0001
#define CONNECT_TECH_WHITE_HEAT_ID	0x8001

/*
   ID tables for whiteheat are unusual, because we want to different
   things for different versions of the device.  Eventually, this
   will be doable from a single table.  But, for now, we define two
   separate ID tables, and then a third table that combines them
   just for the purpose of exporting the autoloading information.
*/
static const struct usb_device_id id_table_std[] = {
	{ USB_DEVICE(CONNECT_TECH_VENDOR_ID, CONNECT_TECH_WHITE_HEAT_ID) },
	{ }						/* Terminating entry */
};

static const struct usb_device_id id_table_prerenumeration[] = {
	{ USB_DEVICE(CONNECT_TECH_VENDOR_ID, CONNECT_TECH_FAKE_WHITE_HEAT_ID) },
	{ }						/* Terminating entry */
};

static const struct usb_device_id id_table_combined[] = {
	{ USB_DEVICE(CONNECT_TECH_VENDOR_ID, CONNECT_TECH_WHITE_HEAT_ID) },
	{ USB_DEVICE(CONNECT_TECH_VENDOR_ID, CONNECT_TECH_FAKE_WHITE_HEAT_ID) },
	{ }						/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, id_table_combined);


/* function prototypes for the Connect Tech WhiteHEAT prerenumeration device */
static int  whiteheat_firmware_download(struct usb_serial *serial,
					const struct usb_device_id *id);
static int  whiteheat_firmware_attach(struct usb_serial *serial);

/* function prototypes for the Connect Tech WhiteHEAT serial converter */
static int  whiteheat_attach(struct usb_serial *serial);
static void whiteheat_release(struct usb_serial *serial);
static int  whiteheat_port_probe(struct usb_serial_port *port);
static void whiteheat_port_remove(struct usb_serial_port *port);
static int  whiteheat_open(struct tty_struct *tty,
			struct usb_serial_port *port);
static void whiteheat_close(struct usb_serial_port *port);
static void whiteheat_get_serial(struct tty_struct *tty,
			struct serial_struct *ss);
static void whiteheat_set_termios(struct tty_struct *tty,
			struct usb_serial_port *port, struct ktermios *old);
static int  whiteheat_tiocmget(struct tty_struct *tty);
static int  whiteheat_tiocmset(struct tty_struct *tty,
			unsigned int set, unsigned int clear);
static void whiteheat_break_ctl(struct tty_struct *tty, int break_state);

static struct usb_serial_driver whiteheat_fake_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"whiteheatnofirm",
	},
	.description =		"Connect Tech - WhiteHEAT - (prerenumeration)",
	.id_table =		id_table_prerenumeration,
	.num_ports =		1,
	.probe =		whiteheat_firmware_download,
	.attach =		whiteheat_firmware_attach,
};

static struct usb_serial_driver whiteheat_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"whiteheat",
	},
	.description =		"Connect Tech - WhiteHEAT",
	.id_table =		id_table_std,
	.num_ports =		4,
	.num_bulk_in =		5,
	.num_bulk_out =		5,
	.attach =		whiteheat_attach,
	.release =		whiteheat_release,
	.port_probe =		whiteheat_port_probe,
	.port_remove =		whiteheat_port_remove,
	.open =			whiteheat_open,
	.close =		whiteheat_close,
	.get_serial =		whiteheat_get_serial,
	.set_termios =		whiteheat_set_termios,
	.break_ctl =		whiteheat_break_ctl,
	.tiocmget =		whiteheat_tiocmget,
	.tiocmset =		whiteheat_tiocmset,
	.throttle =		usb_serial_generic_throttle,
	.unthrottle =		usb_serial_generic_unthrottle,
};

static struct usb_serial_driver * const serial_drivers[] = {
	&whiteheat_fake_device, &whiteheat_device, NULL
};

struct whiteheat_command_private {
	struct mutex		mutex;
	__u8			port_running;
	__u8			command_finished;
	wait_queue_head_t	wait_command; /* for handling sleeping whilst
						 waiting for a command to
						 finish */
	__u8			result_buffer[64];
};

struct whiteheat_private {
	__u8			mcr;		/* FIXME: no locking on mcr */
};


/* local function prototypes */
static int start_command_port(struct usb_serial *serial);
static void stop_command_port(struct usb_serial *serial);
static void command_port_write_callback(struct urb *urb);
static void command_port_read_callback(struct urb *urb);

static int firm_send_command(struct usb_serial_port *port, __u8 command,
						__u8 *data, __u8 datasize);
static int firm_open(struct usb_serial_port *port);
static int firm_close(struct usb_serial_port *port);
static void firm_setup_port(struct tty_struct *tty);
static int firm_set_rts(struct usb_serial_port *port, __u8 onoff);
static int firm_set_dtr(struct usb_serial_port *port, __u8 onoff);
static int firm_set_break(struct usb_serial_port *port, __u8 onoff);
static int firm_purge(struct usb_serial_port *port, __u8 rxtx);
static int firm_get_dtr_rts(struct usb_serial_port *port);
static int firm_report_tx_done(struct usb_serial_port *port);


#define COMMAND_PORT		4
#define COMMAND_TIMEOUT		(2*HZ)	/* 2 second timeout for a command */
#define	COMMAND_TIMEOUT_MS	2000


/*****************************************************************************
 * Connect Tech's White Heat prerenumeration driver functions
 *****************************************************************************/

/* steps to download the firmware to the WhiteHEAT device:
 - hold the reset (by writing to the reset bit of the CPUCS register)
 - download the VEND_AX.HEX file to the chip using VENDOR_REQUEST-ANCHOR_LOAD
 - release the reset (by writing to the CPUCS register)
 - download the WH.HEX file for all addresses greater than 0x1b3f using
   VENDOR_REQUEST-ANCHOR_EXTERNAL_RAM_LOAD
 - hold the reset
 - download the WH.HEX file for all addresses less than 0x1b40 using
   VENDOR_REQUEST_ANCHOR_LOAD
 - release the reset
 - device renumerated itself and comes up as new device id with all
   firmware download completed.
*/
static int whiteheat_firmware_download(struct usb_serial *serial,
					const struct usb_device_id *id)
{
	int response;

	response = ezusb_fx1_ihex_firmware_download(serial->dev, "whiteheat_loader.fw");
	if (response >= 0) {
		response = ezusb_fx1_ihex_firmware_download(serial->dev, "whiteheat.fw");
		if (response >= 0)
			return 0;
	}
	return -ENOENT;
}


static int whiteheat_firmware_attach(struct usb_serial *serial)
{
	/* We want this device to fail to have a driver assigned to it */
	return 1;
}


/*****************************************************************************
 * Connect Tech's White Heat serial driver functions
 *****************************************************************************/

static int whiteheat_attach(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;
	struct whiteheat_hw_info *hw_info;
	int pipe;
	int ret;
	int alen;
	__u8 *command;
	__u8 *result;

	command_port = serial->port[COMMAND_PORT];

	pipe = usb_sndbulkpipe(serial->dev,
			command_port->bulk_out_endpointAddress);
	command = kmalloc(2, GFP_KERNEL);
	if (!command)
		goto no_command_buffer;
	command[0] = WHITEHEAT_GET_HW_INFO;
	command[1] = 0;

	result = kmalloc(sizeof(*hw_info) + 1, GFP_KERNEL);
	if (!result)
		goto no_result_buffer;
	/*
	 * When the module is reloaded the firmware is still there and
	 * the endpoints are still in the usb core unchanged. This is the
	 * unlinking bug in disguise. Same for the call below.
	 */
	usb_clear_halt(serial->dev, pipe);
	ret = usb_bulk_msg(serial->dev, pipe, command, 2,
						&alen, COMMAND_TIMEOUT_MS);
	if (ret) {
		dev_err(&serial->dev->dev, "%s: Couldn't send command [%d]\n",
			serial->type->description, ret);
		goto no_firmware;
	} else if (alen != 2) {
		dev_err(&serial->dev->dev, "%s: Send command incomplete [%d]\n",
			serial->type->description, alen);
		goto no_firmware;
	}

	pipe = usb_rcvbulkpipe(serial->dev,
				command_port->bulk_in_endpointAddress);
	/* See the comment on the usb_clear_halt() above */
	usb_clear_halt(serial->dev, pipe);
	ret = usb_bulk_msg(serial->dev, pipe, result,
			sizeof(*hw_info) + 1, &alen, COMMAND_TIMEOUT_MS);
	if (ret) {
		dev_err(&serial->dev->dev, "%s: Couldn't get results [%d]\n",
			serial->type->description, ret);
		goto no_firmware;
	} else if (alen != sizeof(*hw_info) + 1) {
		dev_err(&serial->dev->dev, "%s: Get results incomplete [%d]\n",
			serial->type->description, alen);
		goto no_firmware;
	} else if (result[0] != command[0]) {
		dev_err(&serial->dev->dev, "%s: Command failed [%d]\n",
			serial->type->description, result[0]);
		goto no_firmware;
	}

	hw_info = (struct whiteheat_hw_info *)&result[1];

	dev_info(&serial->dev->dev, "%s: Firmware v%d.%02d\n",
		 serial->type->description,
		 hw_info->sw_major_rev, hw_info->sw_minor_rev);

	command_info = kmalloc(sizeof(struct whiteheat_command_private),
								GFP_KERNEL);
	if (!command_info)
		goto no_command_private;

	mutex_init(&command_info->mutex);
	command_info->port_running = 0;
	init_waitqueue_head(&command_info->wait_command);
	usb_set_serial_port_data(command_port, command_info);
	command_port->write_urb->complete = command_port_write_callback;
	command_port->read_urb->complete = command_port_read_callback;
	kfree(result);
	kfree(command);

	return 0;

no_firmware:
	/* Firmware likely not running */
	dev_err(&serial->dev->dev,
		"%s: Unable to retrieve firmware version, try replugging\n",
		serial->type->description);
	dev_err(&serial->dev->dev,
		"%s: If the firmware is not running (status led not blinking)\n",
		serial->type->description);
	dev_err(&serial->dev->dev,
		"%s: please contact support@connecttech.com\n",
		serial->type->description);
	kfree(result);
	kfree(command);
	return -ENODEV;

no_command_private:
	kfree(result);
no_result_buffer:
	kfree(command);
no_command_buffer:
	return -ENOMEM;
}

static void whiteheat_release(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;

	/* free up our private data for our command port */
	command_port = serial->port[COMMAND_PORT];
	kfree(usb_get_serial_port_data(command_port));
}

static int whiteheat_port_probe(struct usb_serial_port *port)
{
	struct whiteheat_private *info;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	usb_set_serial_port_data(port, info);

	return 0;
}

static void whiteheat_port_remove(struct usb_serial_port *port)
{
	struct whiteheat_private *info;

	info = usb_get_serial_port_data(port);
	kfree(info);
}

static int whiteheat_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	int retval;

	retval = start_command_port(port->serial);
	if (retval)
		goto exit;

	/* send an open port command */
	retval = firm_open(port);
	if (retval) {
		stop_command_port(port->serial);
		goto exit;
	}

	retval = firm_purge(port, WHITEHEAT_PURGE_RX | WHITEHEAT_PURGE_TX);
	if (retval) {
		firm_close(port);
		stop_command_port(port->serial);
		goto exit;
	}

	if (tty)
		firm_setup_port(tty);

	/* Work around HCD bugs */
	usb_clear_halt(port->serial->dev, port->read_urb->pipe);
	usb_clear_halt(port->serial->dev, port->write_urb->pipe);

	retval = usb_serial_generic_open(tty, port);
	if (retval) {
		firm_close(port);
		stop_command_port(port->serial);
		goto exit;
	}
exit:
	return retval;
}


static void whiteheat_close(struct usb_serial_port *port)
{
	firm_report_tx_done(port);
	firm_close(port);

	usb_serial_generic_close(port);

	stop_command_port(port->serial);
}

static int whiteheat_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct whiteheat_private *info = usb_get_serial_port_data(port);
	unsigned int modem_signals = 0;

	firm_get_dtr_rts(port);
	if (info->mcr & UART_MCR_DTR)
		modem_signals |= TIOCM_DTR;
	if (info->mcr & UART_MCR_RTS)
		modem_signals |= TIOCM_RTS;

	return modem_signals;
}

static int whiteheat_tiocmset(struct tty_struct *tty,
			       unsigned int set, unsigned int clear)
{
	struct usb_serial_port *port = tty->driver_data;
	struct whiteheat_private *info = usb_get_serial_port_data(port);

	if (set & TIOCM_RTS)
		info->mcr |= UART_MCR_RTS;
	if (set & TIOCM_DTR)
		info->mcr |= UART_MCR_DTR;

	if (clear & TIOCM_RTS)
		info->mcr &= ~UART_MCR_RTS;
	if (clear & TIOCM_DTR)
		info->mcr &= ~UART_MCR_DTR;

	firm_set_dtr(port, info->mcr & UART_MCR_DTR);
	firm_set_rts(port, info->mcr & UART_MCR_RTS);
	return 0;
}


static void whiteheat_get_serial(struct tty_struct *tty, struct serial_struct *ss)
{
	ss->baud_base = 460800;
}


static void whiteheat_set_termios(struct tty_struct *tty,
	struct usb_serial_port *port, struct ktermios *old_termios)
{
	firm_setup_port(tty);
}

static void whiteheat_break_ctl(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;
	firm_set_break(port, break_state);
}


/*****************************************************************************
 * Connect Tech's White Heat callback routines
 *****************************************************************************/
static void command_port_write_callback(struct urb *urb)
{
	int status = urb->status;

	if (status) {
		dev_dbg(&urb->dev->dev, "nonzero urb status: %d\n", status);
		return;
	}
}


static void command_port_read_callback(struct urb *urb)
{
	struct usb_serial_port *command_port = urb->context;
	struct whiteheat_command_private *command_info;
	int status = urb->status;
	unsigned char *data = urb->transfer_buffer;
	int result;

	command_info = usb_get_serial_port_data(command_port);
	if (!command_info) {
		dev_dbg(&urb->dev->dev, "%s - command_info is NULL, exiting.\n", __func__);
		return;
	}
	if (!urb->actual_length) {
		dev_dbg(&urb->dev->dev, "%s - empty response, exiting.\n", __func__);
		return;
	}
	if (status) {
		dev_dbg(&urb->dev->dev, "%s - nonzero urb status: %d\n", __func__, status);
		if (status != -ENOENT)
			command_info->command_finished = WHITEHEAT_CMD_FAILURE;
		wake_up(&command_info->wait_command);
		return;
	}

	usb_serial_debug_data(&command_port->dev, __func__, urb->actual_length, data);

	if (data[0] == WHITEHEAT_CMD_COMPLETE) {
		command_info->command_finished = WHITEHEAT_CMD_COMPLETE;
		wake_up(&command_info->wait_command);
	} else if (data[0] == WHITEHEAT_CMD_FAILURE) {
		command_info->command_finished = WHITEHEAT_CMD_FAILURE;
		wake_up(&command_info->wait_command);
	} else if (data[0] == WHITEHEAT_EVENT) {
		/* These are unsolicited reports from the firmware, hence no
		   waiting command to wakeup */
		dev_dbg(&urb->dev->dev, "%s - event received\n", __func__);
	} else if ((data[0] == WHITEHEAT_GET_DTR_RTS) &&
		(urb->actual_length - 1 <= sizeof(command_info->result_buffer))) {
		memcpy(command_info->result_buffer, &data[1],
						urb->actual_length - 1);
		command_info->command_finished = WHITEHEAT_CMD_COMPLETE;
		wake_up(&command_info->wait_command);
	} else
		dev_dbg(&urb->dev->dev, "%s - bad reply from firmware\n", __func__);

	/* Continue trying to always read */
	result = usb_submit_urb(command_port->read_urb, GFP_ATOMIC);
	if (result)
		dev_dbg(&urb->dev->dev, "%s - failed resubmitting read urb, error %d\n",
			__func__, result);
}


/*****************************************************************************
 * Connect Tech's White Heat firmware interface
 *****************************************************************************/
static int firm_send_command(struct usb_serial_port *port, __u8 command,
						__u8 *data, __u8 datasize)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;
	struct whiteheat_private *info;
	struct device *dev = &port->dev;
	__u8 *transfer_buffer;
	int retval = 0;
	int t;

	dev_dbg(dev, "%s - command %d\n", __func__, command);

	command_port = port->serial->port[COMMAND_PORT];
	command_info = usb_get_serial_port_data(command_port);

	if (command_port->bulk_out_size < datasize + 1)
		return -EIO;

	mutex_lock(&command_info->mutex);
	command_info->command_finished = false;

	transfer_buffer = (__u8 *)command_port->write_urb->transfer_buffer;
	transfer_buffer[0] = command;
	memcpy(&transfer_buffer[1], data, datasize);
	command_port->write_urb->transfer_buffer_length = datasize + 1;
	retval = usb_submit_urb(command_port->write_urb, GFP_NOIO);
	if (retval) {
		dev_dbg(dev, "%s - submit urb failed\n", __func__);
		goto exit;
	}

	/* wait for the command to complete */
	t = wait_event_timeout(command_info->wait_command,
		(bool)command_info->command_finished, COMMAND_TIMEOUT);
	if (!t)
		usb_kill_urb(command_port->write_urb);

	if (command_info->command_finished == false) {
		dev_dbg(dev, "%s - command timed out.\n", __func__);
		retval = -ETIMEDOUT;
		goto exit;
	}

	if (command_info->command_finished == WHITEHEAT_CMD_FAILURE) {
		dev_dbg(dev, "%s - command failed.\n", __func__);
		retval = -EIO;
		goto exit;
	}

	if (command_info->command_finished == WHITEHEAT_CMD_COMPLETE) {
		dev_dbg(dev, "%s - command completed.\n", __func__);
		switch (command) {
		case WHITEHEAT_GET_DTR_RTS:
			info = usb_get_serial_port_data(port);
			memcpy(&info->mcr, command_info->result_buffer,
					sizeof(struct whiteheat_dr_info));
				break;
		}
	}
exit:
	mutex_unlock(&command_info->mutex);
	return retval;
}


static int firm_open(struct usb_serial_port *port)
{
	struct whiteheat_simple open_command;

	open_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_OPEN,
		(__u8 *)&open_command, sizeof(open_command));
}


static int firm_close(struct usb_serial_port *port)
{
	struct whiteheat_simple close_command;

	close_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_CLOSE,
			(__u8 *)&close_command, sizeof(close_command));
}


static void firm_setup_port(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct device *dev = &port->dev;
	struct whiteheat_port_settings port_settings;
	unsigned int cflag = tty->termios.c_cflag;
	speed_t baud;

	port_settings.port = port->port_number + 1;

	/* get the byte size */
	switch (cflag & CSIZE) {
	case CS5:	port_settings.bits = 5;   break;
	case CS6:	port_settings.bits = 6;   break;
	case CS7:	port_settings.bits = 7;   break;
	default:
	case CS8:	port_settings.bits = 8;   break;
	}
	dev_dbg(dev, "%s - data bits = %d\n", __func__, port_settings.bits);

	/* determine the parity */
	if (cflag & PARENB)
		if (cflag & CMSPAR)
			if (cflag & PARODD)
				port_settings.parity = WHITEHEAT_PAR_MARK;
			else
				port_settings.parity = WHITEHEAT_PAR_SPACE;
		else
			if (cflag & PARODD)
				port_settings.parity = WHITEHEAT_PAR_ODD;
			else
				port_settings.parity = WHITEHEAT_PAR_EVEN;
	else
		port_settings.parity = WHITEHEAT_PAR_NONE;
	dev_dbg(dev, "%s - parity = %c\n", __func__, port_settings.parity);

	/* figure out the stop bits requested */
	if (cflag & CSTOPB)
		port_settings.stop = 2;
	else
		port_settings.stop = 1;
	dev_dbg(dev, "%s - stop bits = %d\n", __func__, port_settings.stop);

	/* figure out the flow control settings */
	if (cflag & CRTSCTS)
		port_settings.hflow = (WHITEHEAT_HFLOW_CTS |
						WHITEHEAT_HFLOW_RTS);
	else
		port_settings.hflow = WHITEHEAT_HFLOW_NONE;
	dev_dbg(dev, "%s - hardware flow control = %s %s %s %s\n", __func__,
	    (port_settings.hflow & WHITEHEAT_HFLOW_CTS) ? "CTS" : "",
	    (port_settings.hflow & WHITEHEAT_HFLOW_RTS) ? "RTS" : "",
	    (port_settings.hflow & WHITEHEAT_HFLOW_DSR) ? "DSR" : "",
	    (port_settings.hflow & WHITEHEAT_HFLOW_DTR) ? "DTR" : "");

	/* determine software flow control */
	if (I_IXOFF(tty))
		port_settings.sflow = WHITEHEAT_SFLOW_RXTX;
	else
		port_settings.sflow = WHITEHEAT_SFLOW_NONE;
	dev_dbg(dev, "%s - software flow control = %c\n", __func__, port_settings.sflow);

	port_settings.xon = START_CHAR(tty);
	port_settings.xoff = STOP_CHAR(tty);
	dev_dbg(dev, "%s - XON = %2x, XOFF = %2x\n", __func__, port_settings.xon, port_settings.xoff);

	/* get the baud rate wanted */
	baud = tty_get_baud_rate(tty);
	port_settings.baud = cpu_to_le32(baud);
	dev_dbg(dev, "%s - baud rate = %u\n", __func__, baud);

	/* fixme: should set validated settings */
	tty_encode_baud_rate(tty, baud, baud);

	/* handle any settings that aren't specified in the tty structure */
	port_settings.lloop = 0;

	/* now send the message to the device */
	firm_send_command(port, WHITEHEAT_SETUP_PORT,
			(__u8 *)&port_settings, sizeof(port_settings));
}


static int firm_set_rts(struct usb_serial_port *port, __u8 onoff)
{
	struct whiteheat_set_rdb rts_command;

	rts_command.port = port->port_number + 1;
	rts_command.state = onoff;
	return firm_send_command(port, WHITEHEAT_SET_RTS,
			(__u8 *)&rts_command, sizeof(rts_command));
}


static int firm_set_dtr(struct usb_serial_port *port, __u8 onoff)
{
	struct whiteheat_set_rdb dtr_command;

	dtr_command.port = port->port_number + 1;
	dtr_command.state = onoff;
	return firm_send_command(port, WHITEHEAT_SET_DTR,
			(__u8 *)&dtr_command, sizeof(dtr_command));
}


static int firm_set_break(struct usb_serial_port *port, __u8 onoff)
{
	struct whiteheat_set_rdb break_command;

	break_command.port = port->port_number + 1;
	break_command.state = onoff;
	return firm_send_command(port, WHITEHEAT_SET_BREAK,
			(__u8 *)&break_command, sizeof(break_command));
}


static int firm_purge(struct usb_serial_port *port, __u8 rxtx)
{
	struct whiteheat_purge purge_command;

	purge_command.port = port->port_number + 1;
	purge_command.what = rxtx;
	return firm_send_command(port, WHITEHEAT_PURGE,
			(__u8 *)&purge_command, sizeof(purge_command));
}


static int firm_get_dtr_rts(struct usb_serial_port *port)
{
	struct whiteheat_simple get_dr_command;

	get_dr_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_GET_DTR_RTS,
			(__u8 *)&get_dr_command, sizeof(get_dr_command));
}


static int firm_report_tx_done(struct usb_serial_port *port)
{
	struct whiteheat_simple close_command;

	close_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_REPORT_TX_DONE,
			(__u8 *)&close_command, sizeof(close_command));
}


/*****************************************************************************
 * Connect Tech's White Heat utility functions
 *****************************************************************************/
static int start_command_port(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;
	int retval = 0;

	command_port = serial->port[COMMAND_PORT];
	command_info = usb_get_serial_port_data(command_port);
	mutex_lock(&command_info->mutex);
	if (!command_info->port_running) {
		/* Work around HCD bugs */
		usb_clear_halt(serial->dev, command_port->read_urb->pipe);

		retval = usb_submit_urb(command_port->read_urb, GFP_KERNEL);
		if (retval) {
			dev_err(&serial->dev->dev,
				"%s - failed submitting read urb, error %d\n",
				__func__, retval);
			goto exit;
		}
	}
	command_info->port_running++;

exit:
	mutex_unlock(&command_info->mutex);
	return retval;
}


static void stop_command_port(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;

	command_port = serial->port[COMMAND_PORT];
	command_info = usb_get_serial_port_data(command_port);
	mutex_lock(&command_info->mutex);
	command_info->port_running--;
	if (!command_info->port_running)
		usb_kill_urb(command_port->read_urb);
	mutex_unlock(&command_info->mutex);
}

module_usb_serial_driver(serial_drivers, id_table_combined);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

MODULE_FIRMWARE("whiteheat.fw");
MODULE_FIRMWARE("whiteheat_loader.fw");
