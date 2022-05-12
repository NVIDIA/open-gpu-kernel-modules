// SPDX-License-Identifier: GPL-2.0+
/*
 * Edgeport USB Serial Converter driver
 *
 * Copyright (C) 2000-2002 Inside Out Networks, All rights reserved.
 * Copyright (C) 2001-2002 Greg Kroah-Hartman <greg@kroah.com>
 *
 * Supports the following devices:
 *	EP/1 EP/2 EP/4 EP/21 EP/22 EP/221 EP/42 EP/421 WATCHPORT
 *
 * For questions or problems with this driver, contact Inside Out
 * Networks technical support, or Peter Berger <pberger@brimson.com>,
 * or Al Borchers <alborchers@steinerpoint.com>.
 */

#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/serial.h>
#include <linux/swab.h>
#include <linux/kfifo.h>
#include <linux/ioctl.h>
#include <linux/firmware.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>

#include "io_16654.h"
#include "io_usbvend.h"
#include "io_ti.h"

#define DRIVER_AUTHOR "Greg Kroah-Hartman <greg@kroah.com> and David Iacovelli"
#define DRIVER_DESC "Edgeport USB Serial Driver"

#define EPROM_PAGE_SIZE		64


/* different hardware types */
#define HARDWARE_TYPE_930	0
#define HARDWARE_TYPE_TIUMP	1

/* IOCTL_PRIVATE_TI_GET_MODE Definitions */
#define	TI_MODE_CONFIGURING	0   /* Device has not entered start device */
#define	TI_MODE_BOOT		1   /* Staying in boot mode		   */
#define TI_MODE_DOWNLOAD	2   /* Made it to download mode		   */
#define TI_MODE_TRANSITIONING	3   /*
				     * Currently in boot mode but
				     * transitioning to download mode
				     */

/* read urb state */
#define EDGE_READ_URB_RUNNING	0
#define EDGE_READ_URB_STOPPING	1
#define EDGE_READ_URB_STOPPED	2


/* Product information read from the Edgeport */
struct product_info {
	int	TiMode;			/* Current TI Mode  */
	u8	hardware_type;		/* Type of hardware */
} __packed;

/*
 * Edgeport firmware header
 *
 * "build_number" has been set to 0 in all three of the images I have
 * seen, and Digi Tech Support suggests that it is safe to ignore it.
 *
 * "length" is the number of bytes of actual data following the header.
 *
 * "checksum" is the low order byte resulting from adding the values of
 * all the data bytes.
 */
struct edgeport_fw_hdr {
	u8 major_version;
	u8 minor_version;
	__le16 build_number;
	__le16 length;
	u8 checksum;
} __packed;

struct edgeport_port {
	u16 uart_base;
	u16 dma_address;
	u8 shadow_msr;
	u8 shadow_mcr;
	u8 shadow_lsr;
	u8 lsr_mask;
	u32 ump_read_timeout;		/*
					 * Number of milliseconds the UMP will
					 * wait without data before completing
					 * a read short
					 */
	int baud_rate;
	int close_pending;
	int lsr_event;

	struct edgeport_serial	*edge_serial;
	struct usb_serial_port	*port;
	u8 bUartMode;		/* Port type, 0: RS232, etc. */
	spinlock_t ep_lock;
	int ep_read_urb_state;
	int ep_write_urb_in_use;
};

struct edgeport_serial {
	struct product_info product_info;
	u8 TI_I2C_Type;			/* Type of I2C in UMP */
	u8 TiReadI2C;			/*
					 * Set to TRUE if we have read the
					 * I2c in Boot Mode
					 */
	struct mutex es_lock;
	int num_ports_open;
	struct usb_serial *serial;
	struct delayed_work heartbeat_work;
	int fw_version;
	bool use_heartbeat;
};


/* Devices that this driver supports */
static const struct usb_device_id edgeport_1port_id_table[] = {
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_1) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_TI3410_EDGEPORT_1) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_TI3410_EDGEPORT_1I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_PROXIMITY) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_MOTION) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_MOISTURE) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_TEMPERATURE) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_HUMIDITY) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_POWER) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_LIGHT) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_RADIATION) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_DISTANCE) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_ACCELERATION) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_PROX_DIST) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_PLUS_PWR_HP4CD) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_PLUS_PWR_PCI) },
	{ }
};

static const struct usb_device_id edgeport_2port_id_table[] = {
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_2) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_2C) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_2I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_421) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_21) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_42) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_4) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_4I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_22I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_221C) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_22C) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_21C) },
	/* The 4, 8 and 16 port devices show up as multiple 2 port devices */
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_4S) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_8) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_8S) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_416) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_416B) },
	{ }
};

/* Devices that this driver supports */
static const struct usb_device_id id_table_combined[] = {
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_1) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_TI3410_EDGEPORT_1) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_TI3410_EDGEPORT_1I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_PROXIMITY) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_MOTION) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_MOISTURE) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_TEMPERATURE) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_HUMIDITY) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_POWER) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_LIGHT) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_RADIATION) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_DISTANCE) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_ACCELERATION) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_WP_PROX_DIST) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_PLUS_PWR_HP4CD) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_PLUS_PWR_PCI) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_2) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_2C) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_2I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_421) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_21) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_42) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_4) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_4I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_22I) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_221C) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_22C) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_21C) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_4S) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_8) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_8S) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_416) },
	{ USB_DEVICE(USB_VENDOR_ID_ION, ION_DEVICE_ID_TI_EDGEPORT_416B) },
	{ }
};

MODULE_DEVICE_TABLE(usb, id_table_combined);

static bool ignore_cpu_rev;
static int default_uart_mode;		/* RS232 */

static void edge_tty_recv(struct usb_serial_port *port, unsigned char *data,
		int length);

static void stop_read(struct edgeport_port *edge_port);
static int restart_read(struct edgeport_port *edge_port);

static void edge_set_termios(struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios);
static void edge_send(struct usb_serial_port *port, struct tty_struct *tty);

static int do_download_mode(struct edgeport_serial *serial,
		const struct firmware *fw);
static int do_boot_mode(struct edgeport_serial *serial,
		const struct firmware *fw);

/* sysfs attributes */
static int edge_create_sysfs_attrs(struct usb_serial_port *port);
static int edge_remove_sysfs_attrs(struct usb_serial_port *port);

/*
 * Some release of Edgeport firmware "down3.bin" after version 4.80
 * introduced code to automatically disconnect idle devices on some
 * Edgeport models after periods of inactivity, typically ~60 seconds.
 * This occurs without regard to whether ports on the device are open
 * or not.  Digi International Tech Support suggested:
 *
 * 1.  Adding driver "heartbeat" code to reset the firmware timer by
 *     requesting a descriptor record every 15 seconds, which should be
 *     effective with newer firmware versions that require it, and benign
 *     with older versions that do not. In practice 40 seconds seems often
 *     enough.
 * 2.  The heartbeat code is currently required only on Edgeport/416 models.
 */
#define FW_HEARTBEAT_VERSION_CUTOFF ((4 << 8) + 80)
#define FW_HEARTBEAT_SECS 40

/* Timeouts in msecs: firmware downloads take longer */
#define TI_VSEND_TIMEOUT_DEFAULT 1000
#define TI_VSEND_TIMEOUT_FW_DOWNLOAD 10000

static int ti_vread_sync(struct usb_device *dev, u8 request, u16 value,
		u16 index, void *data, int size)
{
	int status;

	status = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0), request,
			(USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_IN),
			value, index, data, size, 1000);
	if (status < 0)
		return status;
	if (status != size) {
		dev_dbg(&dev->dev, "%s - wanted to read %d, but only read %d\n",
			__func__, size, status);
		return -ECOMM;
	}
	return 0;
}

static int ti_vsend_sync(struct usb_device *dev, u8 request, u16 value,
		u16 index, void *data, int size, int timeout)
{
	int status;

	status = usb_control_msg(dev, usb_sndctrlpipe(dev, 0), request,
			(USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT),
			value, index, data, size, timeout);
	if (status < 0)
		return status;

	return 0;
}

static int read_port_cmd(struct usb_serial_port *port, u8 command, u16 value,
		void *data, int size)
{
	return ti_vread_sync(port->serial->dev, command, value,
			UMPM_UART1_PORT + port->port_number,
			data, size);
}

static int send_port_cmd(struct usb_serial_port *port, u8 command, u16 value,
		void *data, int size)
{
	return ti_vsend_sync(port->serial->dev, command, value,
			UMPM_UART1_PORT + port->port_number,
			data, size, TI_VSEND_TIMEOUT_DEFAULT);
}

/* clear tx/rx buffers and fifo in TI UMP */
static int purge_port(struct usb_serial_port *port, u16 mask)
{
	int port_number = port->port_number;

	dev_dbg(&port->dev, "%s - port %d, mask %x\n", __func__, port_number, mask);

	return send_port_cmd(port, UMPC_PURGE_PORT, mask, NULL, 0);
}

/**
 * read_download_mem - Read edgeport memory from TI chip
 * @dev: usb device pointer
 * @start_address: Device CPU address at which to read
 * @length: Length of above data
 * @address_type: Can read both XDATA and I2C
 * @buffer: pointer to input data buffer
 */
static int read_download_mem(struct usb_device *dev, int start_address,
				int length, u8 address_type, u8 *buffer)
{
	int status = 0;
	u8 read_length;
	u16 be_start_address;

	dev_dbg(&dev->dev, "%s - @ %x for %d\n", __func__, start_address, length);

	/*
	 * Read in blocks of 64 bytes
	 * (TI firmware can't handle more than 64 byte reads)
	 */
	while (length) {
		if (length > 64)
			read_length = 64;
		else
			read_length = (u8)length;

		if (read_length > 1) {
			dev_dbg(&dev->dev, "%s - @ %x for %d\n", __func__, start_address, read_length);
		}
		/*
		 * NOTE: Must use swab as wIndex is sent in little-endian
		 *       byte order regardless of host byte order.
		 */
		be_start_address = swab16((u16)start_address);
		status = ti_vread_sync(dev, UMPC_MEMORY_READ,
					(u16)address_type,
					be_start_address,
					buffer, read_length);

		if (status) {
			dev_dbg(&dev->dev, "%s - ERROR %x\n", __func__, status);
			return status;
		}

		if (read_length > 1)
			usb_serial_debug_data(&dev->dev, __func__, read_length, buffer);

		/* Update pointers/length */
		start_address += read_length;
		buffer += read_length;
		length -= read_length;
	}

	return status;
}

static int read_ram(struct usb_device *dev, int start_address,
						int length, u8 *buffer)
{
	return read_download_mem(dev, start_address, length,
					DTK_ADDR_SPACE_XDATA, buffer);
}

/* Read edgeport memory to a given block */
static int read_boot_mem(struct edgeport_serial *serial,
				int start_address, int length, u8 *buffer)
{
	int status = 0;
	int i;

	for (i = 0; i < length; i++) {
		status = ti_vread_sync(serial->serial->dev,
				UMPC_MEMORY_READ, serial->TI_I2C_Type,
				(u16)(start_address+i), &buffer[i], 0x01);
		if (status) {
			dev_dbg(&serial->serial->dev->dev, "%s - ERROR %x\n", __func__, status);
			return status;
		}
	}

	dev_dbg(&serial->serial->dev->dev, "%s - start_address = %x, length = %d\n",
		__func__, start_address, length);
	usb_serial_debug_data(&serial->serial->dev->dev, __func__, length, buffer);

	serial->TiReadI2C = 1;

	return status;
}

/* Write given block to TI EPROM memory */
static int write_boot_mem(struct edgeport_serial *serial,
				int start_address, int length, u8 *buffer)
{
	int status = 0;
	int i;
	u8 *temp;

	/* Must do a read before write */
	if (!serial->TiReadI2C) {
		temp = kmalloc(1, GFP_KERNEL);
		if (!temp)
			return -ENOMEM;

		status = read_boot_mem(serial, 0, 1, temp);
		kfree(temp);
		if (status)
			return status;
	}

	for (i = 0; i < length; ++i) {
		status = ti_vsend_sync(serial->serial->dev, UMPC_MEMORY_WRITE,
				buffer[i], (u16)(i + start_address), NULL,
				0, TI_VSEND_TIMEOUT_DEFAULT);
		if (status)
			return status;
	}

	dev_dbg(&serial->serial->dev->dev, "%s - start_sddr = %x, length = %d\n", __func__, start_address, length);
	usb_serial_debug_data(&serial->serial->dev->dev, __func__, length, buffer);

	return status;
}

/* Write edgeport I2C memory to TI chip	*/
static int write_i2c_mem(struct edgeport_serial *serial,
		int start_address, int length, u8 address_type, u8 *buffer)
{
	struct device *dev = &serial->serial->dev->dev;
	int status = 0;
	int write_length;
	u16 be_start_address;

	/* We can only send a maximum of 1 aligned byte page at a time */

	/* calculate the number of bytes left in the first page */
	write_length = EPROM_PAGE_SIZE -
				(start_address & (EPROM_PAGE_SIZE - 1));

	if (write_length > length)
		write_length = length;

	dev_dbg(dev, "%s - BytesInFirstPage Addr = %x, length = %d\n",
		__func__, start_address, write_length);
	usb_serial_debug_data(dev, __func__, write_length, buffer);

	/*
	 * Write first page.
	 *
	 * NOTE: Must use swab as wIndex is sent in little-endian byte order
	 *       regardless of host byte order.
	 */
	be_start_address = swab16((u16)start_address);
	status = ti_vsend_sync(serial->serial->dev, UMPC_MEMORY_WRITE,
				(u16)address_type, be_start_address,
				buffer,	write_length, TI_VSEND_TIMEOUT_DEFAULT);
	if (status) {
		dev_dbg(dev, "%s - ERROR %d\n", __func__, status);
		return status;
	}

	length		-= write_length;
	start_address	+= write_length;
	buffer		+= write_length;

	/*
	 * We should be aligned now -- can write max page size bytes at a
	 * time.
	 */
	while (length) {
		if (length > EPROM_PAGE_SIZE)
			write_length = EPROM_PAGE_SIZE;
		else
			write_length = length;

		dev_dbg(dev, "%s - Page Write Addr = %x, length = %d\n",
			__func__, start_address, write_length);
		usb_serial_debug_data(dev, __func__, write_length, buffer);

		/*
		 * Write next page.
		 *
		 * NOTE: Must use swab as wIndex is sent in little-endian byte
		 *       order regardless of host byte order.
		 */
		be_start_address = swab16((u16)start_address);
		status = ti_vsend_sync(serial->serial->dev, UMPC_MEMORY_WRITE,
				(u16)address_type, be_start_address, buffer,
				write_length, TI_VSEND_TIMEOUT_DEFAULT);
		if (status) {
			dev_err(dev, "%s - ERROR %d\n", __func__, status);
			return status;
		}

		length		-= write_length;
		start_address	+= write_length;
		buffer		+= write_length;
	}
	return status;
}

/*
 * Examine the UMP DMA registers and LSR
 *
 * Check the MSBit of the X and Y DMA byte count registers.
 * A zero in this bit indicates that the TX DMA buffers are empty
 * then check the TX Empty bit in the UART.
 */
static int tx_active(struct edgeport_port *port)
{
	int status;
	struct out_endpoint_desc_block *oedb;
	u8 *lsr;
	int bytes_left = 0;

	oedb = kmalloc(sizeof(*oedb), GFP_KERNEL);
	if (!oedb)
		return -ENOMEM;

	/*
	 * Sigh, that's right, just one byte, as not all platforms can
	 * do DMA from stack
	 */
	lsr = kmalloc(1, GFP_KERNEL);
	if (!lsr) {
		kfree(oedb);
		return -ENOMEM;
	}
	/* Read the DMA Count Registers */
	status = read_ram(port->port->serial->dev, port->dma_address,
						sizeof(*oedb), (void *)oedb);
	if (status)
		goto exit_is_tx_active;

	dev_dbg(&port->port->dev, "%s - XByteCount    0x%X\n", __func__, oedb->XByteCount);

	/* and the LSR */
	status = read_ram(port->port->serial->dev,
			port->uart_base + UMPMEM_OFFS_UART_LSR, 1, lsr);

	if (status)
		goto exit_is_tx_active;
	dev_dbg(&port->port->dev, "%s - LSR = 0x%X\n", __func__, *lsr);

	/* If either buffer has data or we are transmitting then return TRUE */
	if ((oedb->XByteCount & 0x80) != 0)
		bytes_left += 64;

	if ((*lsr & UMP_UART_LSR_TX_MASK) == 0)
		bytes_left += 1;

	/* We return Not Active if we get any kind of error */
exit_is_tx_active:
	dev_dbg(&port->port->dev, "%s - return %d\n", __func__, bytes_left);

	kfree(lsr);
	kfree(oedb);
	return bytes_left;
}

static int choose_config(struct usb_device *dev)
{
	/*
	 * There may be multiple configurations on this device, in which case
	 * we would need to read and parse all of them to find out which one
	 * we want. However, we just support one config at this point,
	 * configuration # 1, which is Config Descriptor 0.
	 */

	dev_dbg(&dev->dev, "%s - Number of Interfaces = %d\n",
		__func__, dev->config->desc.bNumInterfaces);
	dev_dbg(&dev->dev, "%s - MAX Power            = %d\n",
		__func__, dev->config->desc.bMaxPower * 2);

	if (dev->config->desc.bNumInterfaces != 1) {
		dev_err(&dev->dev, "%s - bNumInterfaces is not 1, ERROR!\n", __func__);
		return -ENODEV;
	}

	return 0;
}

static int read_rom(struct edgeport_serial *serial,
				int start_address, int length, u8 *buffer)
{
	int status;

	if (serial->product_info.TiMode == TI_MODE_DOWNLOAD) {
		status = read_download_mem(serial->serial->dev,
					       start_address,
					       length,
					       serial->TI_I2C_Type,
					       buffer);
	} else {
		status = read_boot_mem(serial, start_address, length,
								buffer);
	}
	return status;
}

static int write_rom(struct edgeport_serial *serial, int start_address,
						int length, u8 *buffer)
{
	if (serial->product_info.TiMode == TI_MODE_BOOT)
		return write_boot_mem(serial, start_address, length,
								buffer);

	if (serial->product_info.TiMode == TI_MODE_DOWNLOAD)
		return write_i2c_mem(serial, start_address, length,
						serial->TI_I2C_Type, buffer);
	return -EINVAL;
}

/* Read a descriptor header from I2C based on type */
static int get_descriptor_addr(struct edgeport_serial *serial,
				int desc_type, struct ti_i2c_desc *rom_desc)
{
	int start_address;
	int status;

	/* Search for requested descriptor in I2C */
	start_address = 2;
	do {
		status = read_rom(serial,
				   start_address,
				   sizeof(struct ti_i2c_desc),
				   (u8 *)rom_desc);
		if (status)
			return 0;

		if (rom_desc->Type == desc_type)
			return start_address;

		start_address = start_address + sizeof(struct ti_i2c_desc) +
						le16_to_cpu(rom_desc->Size);

	} while ((start_address < TI_MAX_I2C_SIZE) && rom_desc->Type);

	return 0;
}

/* Validate descriptor checksum */
static int valid_csum(struct ti_i2c_desc *rom_desc, u8 *buffer)
{
	u16 i;
	u8 cs = 0;

	for (i = 0; i < le16_to_cpu(rom_desc->Size); i++)
		cs = (u8)(cs + buffer[i]);

	if (cs != rom_desc->CheckSum) {
		pr_debug("%s - Mismatch %x - %x", __func__, rom_desc->CheckSum, cs);
		return -EINVAL;
	}
	return 0;
}

/* Make sure that the I2C image is good */
static int check_i2c_image(struct edgeport_serial *serial)
{
	struct device *dev = &serial->serial->dev->dev;
	int status = 0;
	struct ti_i2c_desc *rom_desc;
	int start_address = 2;
	u8 *buffer;
	u16 ttype;

	rom_desc = kmalloc(sizeof(*rom_desc), GFP_KERNEL);
	if (!rom_desc)
		return -ENOMEM;

	buffer = kmalloc(TI_MAX_I2C_SIZE, GFP_KERNEL);
	if (!buffer) {
		kfree(rom_desc);
		return -ENOMEM;
	}

	/* Read the first byte (Signature0) must be 0x52 or 0x10 */
	status = read_rom(serial, 0, 1, buffer);
	if (status)
		goto out;

	if (*buffer != UMP5152 && *buffer != UMP3410) {
		dev_err(dev, "%s - invalid buffer signature\n", __func__);
		status = -ENODEV;
		goto out;
	}

	do {
		/* Validate the I2C */
		status = read_rom(serial,
				start_address,
				sizeof(struct ti_i2c_desc),
				(u8 *)rom_desc);
		if (status)
			break;

		if ((start_address + sizeof(struct ti_i2c_desc) +
			le16_to_cpu(rom_desc->Size)) > TI_MAX_I2C_SIZE) {
			status = -ENODEV;
			dev_dbg(dev, "%s - structure too big, erroring out.\n", __func__);
			break;
		}

		dev_dbg(dev, "%s Type = 0x%x\n", __func__, rom_desc->Type);

		/* Skip type 2 record */
		ttype = rom_desc->Type & 0x0f;
		if (ttype != I2C_DESC_TYPE_FIRMWARE_BASIC
			&& ttype != I2C_DESC_TYPE_FIRMWARE_AUTO) {
			/* Read the descriptor data */
			status = read_rom(serial, start_address +
						sizeof(struct ti_i2c_desc),
						le16_to_cpu(rom_desc->Size),
						buffer);
			if (status)
				break;

			status = valid_csum(rom_desc, buffer);
			if (status)
				break;
		}
		start_address = start_address + sizeof(struct ti_i2c_desc) +
						le16_to_cpu(rom_desc->Size);

	} while ((rom_desc->Type != I2C_DESC_TYPE_ION) &&
				(start_address < TI_MAX_I2C_SIZE));

	if ((rom_desc->Type != I2C_DESC_TYPE_ION) ||
				(start_address > TI_MAX_I2C_SIZE))
		status = -ENODEV;

out:
	kfree(buffer);
	kfree(rom_desc);
	return status;
}

static int get_manuf_info(struct edgeport_serial *serial, u8 *buffer)
{
	int status;
	int start_address;
	struct ti_i2c_desc *rom_desc;
	struct edge_ti_manuf_descriptor *desc;
	struct device *dev = &serial->serial->dev->dev;

	rom_desc = kmalloc(sizeof(*rom_desc), GFP_KERNEL);
	if (!rom_desc)
		return -ENOMEM;

	start_address = get_descriptor_addr(serial, I2C_DESC_TYPE_ION,
								rom_desc);

	if (!start_address) {
		dev_dbg(dev, "%s - Edge Descriptor not found in I2C\n", __func__);
		status = -ENODEV;
		goto exit;
	}

	/* Read the descriptor data */
	status = read_rom(serial, start_address+sizeof(struct ti_i2c_desc),
					le16_to_cpu(rom_desc->Size), buffer);
	if (status)
		goto exit;

	status = valid_csum(rom_desc, buffer);

	desc = (struct edge_ti_manuf_descriptor *)buffer;
	dev_dbg(dev, "%s - IonConfig      0x%x\n", __func__, desc->IonConfig);
	dev_dbg(dev, "%s - Version          %d\n", __func__, desc->Version);
	dev_dbg(dev, "%s - Cpu/Board      0x%x\n", __func__, desc->CpuRev_BoardRev);
	dev_dbg(dev, "%s - NumPorts         %d\n", __func__, desc->NumPorts);
	dev_dbg(dev, "%s - NumVirtualPorts  %d\n", __func__, desc->NumVirtualPorts);
	dev_dbg(dev, "%s - TotalPorts       %d\n", __func__, desc->TotalPorts);

exit:
	kfree(rom_desc);
	return status;
}

/* Build firmware header used for firmware update */
static int build_i2c_fw_hdr(u8 *header, const struct firmware *fw)
{
	u8 *buffer;
	int buffer_size;
	int i;
	u8 cs = 0;
	struct ti_i2c_desc *i2c_header;
	struct ti_i2c_image_header *img_header;
	struct ti_i2c_firmware_rec *firmware_rec;
	struct edgeport_fw_hdr *fw_hdr = (struct edgeport_fw_hdr *)fw->data;

	/*
	 * In order to update the I2C firmware we must change the type 2 record
	 * to type 0xF2.  This will force the UMP to come up in Boot Mode.
	 * Then while in boot mode, the driver will download the latest
	 * firmware (padded to 15.5k) into the UMP ram.  And finally when the
	 * device comes back up in download mode the driver will cause the new
	 * firmware to be copied from the UMP Ram to I2C and the firmware will
	 * update the record type from 0xf2 to 0x02.
	 */

	/*
	 * Allocate a 15.5k buffer + 2 bytes for version number (Firmware
	 * Record)
	 */
	buffer_size = (((1024 * 16) - 512 ) +
			sizeof(struct ti_i2c_firmware_rec));

	buffer = kmalloc(buffer_size, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	/* Set entire image of 0xffs */
	memset(buffer, 0xff, buffer_size);

	/* Copy version number into firmware record */
	firmware_rec = (struct ti_i2c_firmware_rec *)buffer;

	firmware_rec->Ver_Major	= fw_hdr->major_version;
	firmware_rec->Ver_Minor	= fw_hdr->minor_version;

	/* Pointer to fw_down memory image */
	img_header = (struct ti_i2c_image_header *)&fw->data[4];

	memcpy(buffer + sizeof(struct ti_i2c_firmware_rec),
		&fw->data[4 + sizeof(struct ti_i2c_image_header)],
		le16_to_cpu(img_header->Length));

	for (i=0; i < buffer_size; i++) {
		cs = (u8)(cs + buffer[i]);
	}

	kfree(buffer);

	/* Build new header */
	i2c_header =  (struct ti_i2c_desc *)header;
	firmware_rec =  (struct ti_i2c_firmware_rec*)i2c_header->Data;

	i2c_header->Type	= I2C_DESC_TYPE_FIRMWARE_BLANK;
	i2c_header->Size	= cpu_to_le16(buffer_size);
	i2c_header->CheckSum	= cs;
	firmware_rec->Ver_Major	= fw_hdr->major_version;
	firmware_rec->Ver_Minor	= fw_hdr->minor_version;

	return 0;
}

/* Try to figure out what type of I2c we have */
static int i2c_type_bootmode(struct edgeport_serial *serial)
{
	struct device *dev = &serial->serial->dev->dev;
	int status;
	u8 *data;

	data = kmalloc(1, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	/* Try to read type 2 */
	status = ti_vread_sync(serial->serial->dev, UMPC_MEMORY_READ,
				DTK_ADDR_SPACE_I2C_TYPE_II, 0, data, 0x01);
	if (status)
		dev_dbg(dev, "%s - read 2 status error = %d\n", __func__, status);
	else
		dev_dbg(dev, "%s - read 2 data = 0x%x\n", __func__, *data);
	if ((!status) && (*data == UMP5152 || *data == UMP3410)) {
		dev_dbg(dev, "%s - ROM_TYPE_II\n", __func__);
		serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_II;
		goto out;
	}

	/* Try to read type 3 */
	status = ti_vread_sync(serial->serial->dev, UMPC_MEMORY_READ,
				DTK_ADDR_SPACE_I2C_TYPE_III, 0,	data, 0x01);
	if (status)
		dev_dbg(dev, "%s - read 3 status error = %d\n", __func__, status);
	else
		dev_dbg(dev, "%s - read 2 data = 0x%x\n", __func__, *data);
	if ((!status) && (*data == UMP5152 || *data == UMP3410)) {
		dev_dbg(dev, "%s - ROM_TYPE_III\n", __func__);
		serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_III;
		goto out;
	}

	dev_dbg(dev, "%s - Unknown\n", __func__);
	serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_II;
	status = -ENODEV;
out:
	kfree(data);
	return status;
}

static int bulk_xfer(struct usb_serial *serial, void *buffer,
						int length, int *num_sent)
{
	int status;

	status = usb_bulk_msg(serial->dev,
			usb_sndbulkpipe(serial->dev,
				serial->port[0]->bulk_out_endpointAddress),
			buffer, length, num_sent, 1000);
	return status;
}

/* Download given firmware image to the device (IN BOOT MODE) */
static int download_code(struct edgeport_serial *serial, u8 *image,
							int image_length)
{
	int status = 0;
	int pos;
	int transfer;
	int done;

	/* Transfer firmware image */
	for (pos = 0; pos < image_length; ) {
		/* Read the next buffer from file */
		transfer = image_length - pos;
		if (transfer > EDGE_FW_BULK_MAX_PACKET_SIZE)
			transfer = EDGE_FW_BULK_MAX_PACKET_SIZE;

		/* Transfer data */
		status = bulk_xfer(serial->serial, &image[pos],
							transfer, &done);
		if (status)
			break;
		/* Advance buffer pointer */
		pos += done;
	}

	return status;
}

/* FIXME!!! */
static int config_boot_dev(struct usb_device *dev)
{
	return 0;
}

static int ti_cpu_rev(struct edge_ti_manuf_descriptor *desc)
{
	return TI_GET_CPU_REVISION(desc->CpuRev_BoardRev);
}

static int check_fw_sanity(struct edgeport_serial *serial,
		const struct firmware *fw)
{
	u16 length_total;
	u8 checksum = 0;
	int pos;
	struct device *dev = &serial->serial->interface->dev;
	struct edgeport_fw_hdr *fw_hdr = (struct edgeport_fw_hdr *)fw->data;

	if (fw->size < sizeof(struct edgeport_fw_hdr)) {
		dev_err(dev, "incomplete fw header\n");
		return -EINVAL;
	}

	length_total = le16_to_cpu(fw_hdr->length) +
			sizeof(struct edgeport_fw_hdr);

	if (fw->size != length_total) {
		dev_err(dev, "bad fw size (expected: %u, got: %zu)\n",
				length_total, fw->size);
		return -EINVAL;
	}

	for (pos = sizeof(struct edgeport_fw_hdr); pos < fw->size; ++pos)
		checksum += fw->data[pos];

	if (checksum != fw_hdr->checksum) {
		dev_err(dev, "bad fw checksum (expected: 0x%x, got: 0x%x)\n",
				fw_hdr->checksum, checksum);
		return -EINVAL;
	}

	return 0;
}

/*
 * DownloadTIFirmware - Download run-time operating firmware to the TI5052
 *
 * This routine downloads the main operating code into the TI5052, using the
 * boot code already burned into E2PROM or ROM.
 */
static int download_fw(struct edgeport_serial *serial)
{
	struct device *dev = &serial->serial->interface->dev;
	int status = 0;
	struct usb_interface_descriptor *interface;
	const struct firmware *fw;
	const char *fw_name = "edgeport/down3.bin";
	struct edgeport_fw_hdr *fw_hdr;

	status = request_firmware(&fw, fw_name, dev);
	if (status) {
		dev_err(dev, "Failed to load image \"%s\" err %d\n",
				fw_name, status);
		return status;
	}

	if (check_fw_sanity(serial, fw)) {
		status = -EINVAL;
		goto out;
	}

	fw_hdr = (struct edgeport_fw_hdr *)fw->data;

	/* If on-board version is newer, "fw_version" will be updated later. */
	serial->fw_version = (fw_hdr->major_version << 8) +
			fw_hdr->minor_version;

	/*
	 * This routine is entered by both the BOOT mode and the Download mode
	 * We can determine which code is running by the reading the config
	 * descriptor and if we have only one bulk pipe it is in boot mode
	 */
	serial->product_info.hardware_type = HARDWARE_TYPE_TIUMP;

	/* Default to type 2 i2c */
	serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_II;

	status = choose_config(serial->serial->dev);
	if (status)
		goto out;

	interface = &serial->serial->interface->cur_altsetting->desc;
	if (!interface) {
		dev_err(dev, "%s - no interface set, error!\n", __func__);
		status = -ENODEV;
		goto out;
	}

	/*
	 * Setup initial mode -- the default mode 0 is TI_MODE_CONFIGURING
	 * if we have more than one endpoint we are definitely in download
	 * mode
	 */
	if (interface->bNumEndpoints > 1) {
		serial->product_info.TiMode = TI_MODE_DOWNLOAD;
		status = do_download_mode(serial, fw);
	} else {
		/* Otherwise we will remain in configuring mode */
		serial->product_info.TiMode = TI_MODE_CONFIGURING;
		status = do_boot_mode(serial, fw);
	}

out:
	release_firmware(fw);
	return status;
}

static int do_download_mode(struct edgeport_serial *serial,
		const struct firmware *fw)
{
	struct device *dev = &serial->serial->interface->dev;
	int status = 0;
	int start_address;
	struct edge_ti_manuf_descriptor *ti_manuf_desc;
	int download_cur_ver;
	int download_new_ver;
	struct edgeport_fw_hdr *fw_hdr = (struct edgeport_fw_hdr *)fw->data;
	struct ti_i2c_desc *rom_desc;

	dev_dbg(dev, "%s - RUNNING IN DOWNLOAD MODE\n", __func__);

	status = check_i2c_image(serial);
	if (status) {
		dev_dbg(dev, "%s - DOWNLOAD MODE -- BAD I2C\n", __func__);
		return status;
	}

	/*
	 * Validate Hardware version number
	 * Read Manufacturing Descriptor from TI Based Edgeport
	 */
	ti_manuf_desc = kmalloc(sizeof(*ti_manuf_desc), GFP_KERNEL);
	if (!ti_manuf_desc)
		return -ENOMEM;

	status = get_manuf_info(serial, (u8 *)ti_manuf_desc);
	if (status) {
		kfree(ti_manuf_desc);
		return status;
	}

	/* Check version number of ION descriptor */
	if (!ignore_cpu_rev && ti_cpu_rev(ti_manuf_desc) < 2) {
		dev_dbg(dev, "%s - Wrong CPU Rev %d (Must be 2)\n",
			__func__, ti_cpu_rev(ti_manuf_desc));
		kfree(ti_manuf_desc);
		return -EINVAL;
	}

	rom_desc = kmalloc(sizeof(*rom_desc), GFP_KERNEL);
	if (!rom_desc) {
		kfree(ti_manuf_desc);
		return -ENOMEM;
	}

	/* Search for type 2 record (firmware record) */
	start_address = get_descriptor_addr(serial,
			I2C_DESC_TYPE_FIRMWARE_BASIC, rom_desc);
	if (start_address != 0) {
		struct ti_i2c_firmware_rec *firmware_version;
		u8 *record;

		dev_dbg(dev, "%s - Found Type FIRMWARE (Type 2) record\n",
				__func__);

		firmware_version = kmalloc(sizeof(*firmware_version),
							GFP_KERNEL);
		if (!firmware_version) {
			kfree(rom_desc);
			kfree(ti_manuf_desc);
			return -ENOMEM;
		}

		/*
		 * Validate version number
		 * Read the descriptor data
		 */
		status = read_rom(serial, start_address +
				sizeof(struct ti_i2c_desc),
				sizeof(struct ti_i2c_firmware_rec),
				(u8 *)firmware_version);
		if (status) {
			kfree(firmware_version);
			kfree(rom_desc);
			kfree(ti_manuf_desc);
			return status;
		}

		/*
		 * Check version number of download with current
		 * version in I2c
		 */
		download_cur_ver = (firmware_version->Ver_Major << 8) +
				   (firmware_version->Ver_Minor);
		download_new_ver = (fw_hdr->major_version << 8) +
				   (fw_hdr->minor_version);

		dev_dbg(dev, "%s - >> FW Versions Device %d.%d  Driver %d.%d\n",
			__func__, firmware_version->Ver_Major,
			firmware_version->Ver_Minor,
			fw_hdr->major_version, fw_hdr->minor_version);

		/*
		 * Check if we have an old version in the I2C and
		 * update if necessary
		 */
		if (download_cur_ver < download_new_ver) {
			dev_dbg(dev, "%s - Update I2C dld from %d.%d to %d.%d\n",
				__func__,
				firmware_version->Ver_Major,
				firmware_version->Ver_Minor,
				fw_hdr->major_version,
				fw_hdr->minor_version);

			record = kmalloc(1, GFP_KERNEL);
			if (!record) {
				kfree(firmware_version);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return -ENOMEM;
			}
			/*
			 * In order to update the I2C firmware we must
			 * change the type 2 record to type 0xF2. This
			 * will force the UMP to come up in Boot Mode.
			 * Then while in boot mode, the driver will
			 * download the latest firmware (padded to
			 * 15.5k) into the UMP ram. Finally when the
			 * device comes back up in download mode the
			 * driver will cause the new firmware to be
			 * copied from the UMP Ram to I2C and the
			 * firmware will update the record type from
			 * 0xf2 to 0x02.
			 */
			*record = I2C_DESC_TYPE_FIRMWARE_BLANK;

			/*
			 * Change the I2C Firmware record type to
			 * 0xf2 to trigger an update
			 */
			status = write_rom(serial, start_address,
					sizeof(*record), record);
			if (status) {
				kfree(record);
				kfree(firmware_version);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return status;
			}

			/*
			 * verify the write -- must do this in order
			 * for write to complete before we do the
			 * hardware reset
			 */
			status = read_rom(serial,
						start_address,
						sizeof(*record),
						record);
			if (status) {
				kfree(record);
				kfree(firmware_version);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return status;
			}

			if (*record != I2C_DESC_TYPE_FIRMWARE_BLANK) {
				dev_err(dev, "%s - error resetting device\n",
						__func__);
				kfree(record);
				kfree(firmware_version);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return -ENODEV;
			}

			dev_dbg(dev, "%s - HARDWARE RESET\n", __func__);

			/* Reset UMP -- Back to BOOT MODE */
			status = ti_vsend_sync(serial->serial->dev,
					UMPC_HARDWARE_RESET,
					0, 0, NULL, 0,
					TI_VSEND_TIMEOUT_DEFAULT);

			dev_dbg(dev, "%s - HARDWARE RESET return %d\n",
					__func__, status);

			/* return an error on purpose. */
			kfree(record);
			kfree(firmware_version);
			kfree(rom_desc);
			kfree(ti_manuf_desc);
			return -ENODEV;
		}
		/* Same or newer fw version is already loaded */
		serial->fw_version = download_cur_ver;
		kfree(firmware_version);
	}
	/* Search for type 0xF2 record (firmware blank record) */
	else {
		start_address = get_descriptor_addr(serial,
				I2C_DESC_TYPE_FIRMWARE_BLANK, rom_desc);
		if (start_address != 0) {
#define HEADER_SIZE	(sizeof(struct ti_i2c_desc) + \
				sizeof(struct ti_i2c_firmware_rec))
			u8 *header;
			u8 *vheader;

			header = kmalloc(HEADER_SIZE, GFP_KERNEL);
			if (!header) {
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return -ENOMEM;
			}

			vheader = kmalloc(HEADER_SIZE, GFP_KERNEL);
			if (!vheader) {
				kfree(header);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return -ENOMEM;
			}

			dev_dbg(dev, "%s - Found Type BLANK FIRMWARE (Type F2) record\n",
					__func__);

			/*
			 * In order to update the I2C firmware we must change
			 * the type 2 record to type 0xF2. This will force the
			 * UMP to come up in Boot Mode.  Then while in boot
			 * mode, the driver will download the latest firmware
			 * (padded to 15.5k) into the UMP ram. Finally when the
			 * device comes back up in download mode the driver
			 * will cause the new firmware to be copied from the
			 * UMP Ram to I2C and the firmware will update the
			 * record type from 0xf2 to 0x02.
			 */
			status = build_i2c_fw_hdr(header, fw);
			if (status) {
				kfree(vheader);
				kfree(header);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return -EINVAL;
			}

			/*
			 * Update I2C with type 0xf2 record with correct
			 * size and checksum
			 */
			status = write_rom(serial,
						start_address,
						HEADER_SIZE,
						header);
			if (status) {
				kfree(vheader);
				kfree(header);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return -EINVAL;
			}

			/*
			 * verify the write -- must do this in order for
			 * write to complete before we do the hardware reset
			 */
			status = read_rom(serial, start_address,
							HEADER_SIZE, vheader);

			if (status) {
				dev_dbg(dev, "%s - can't read header back\n",
						__func__);
				kfree(vheader);
				kfree(header);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return status;
			}
			if (memcmp(vheader, header, HEADER_SIZE)) {
				dev_dbg(dev, "%s - write download record failed\n",
						__func__);
				kfree(vheader);
				kfree(header);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return -EINVAL;
			}

			kfree(vheader);
			kfree(header);

			dev_dbg(dev, "%s - Start firmware update\n", __func__);

			/* Tell firmware to copy download image into I2C */
			status = ti_vsend_sync(serial->serial->dev,
					UMPC_COPY_DNLD_TO_I2C,
					0, 0, NULL, 0,
					TI_VSEND_TIMEOUT_FW_DOWNLOAD);

			dev_dbg(dev, "%s - Update complete 0x%x\n", __func__,
					status);
			if (status) {
				dev_err(dev,
					"%s - UMPC_COPY_DNLD_TO_I2C failed\n",
					__func__);
				kfree(rom_desc);
				kfree(ti_manuf_desc);
				return status;
			}
		}
	}

	/* The device is running the download code */
	kfree(rom_desc);
	kfree(ti_manuf_desc);
	return 0;
}

static int do_boot_mode(struct edgeport_serial *serial,
		const struct firmware *fw)
{
	struct device *dev = &serial->serial->interface->dev;
	int status = 0;
	struct edge_ti_manuf_descriptor *ti_manuf_desc;
	struct edgeport_fw_hdr *fw_hdr = (struct edgeport_fw_hdr *)fw->data;

	dev_dbg(dev, "%s - RUNNING IN BOOT MODE\n", __func__);

	/* Configure the TI device so we can use the BULK pipes for download */
	status = config_boot_dev(serial->serial->dev);
	if (status)
		return status;

	if (le16_to_cpu(serial->serial->dev->descriptor.idVendor)
							!= USB_VENDOR_ID_ION) {
		dev_dbg(dev, "%s - VID = 0x%x\n", __func__,
			le16_to_cpu(serial->serial->dev->descriptor.idVendor));
		serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_II;
		goto stayinbootmode;
	}

	/*
	 * We have an ION device (I2c Must be programmed)
	 * Determine I2C image type
	 */
	if (i2c_type_bootmode(serial))
		goto stayinbootmode;

	/* Check for ION Vendor ID and that the I2C is valid */
	if (!check_i2c_image(serial)) {
		struct ti_i2c_image_header *header;
		int i;
		u8 cs = 0;
		u8 *buffer;
		int buffer_size;

		/*
		 * Validate Hardware version number
		 * Read Manufacturing Descriptor from TI Based Edgeport
		 */
		ti_manuf_desc = kmalloc(sizeof(*ti_manuf_desc), GFP_KERNEL);
		if (!ti_manuf_desc)
			return -ENOMEM;

		status = get_manuf_info(serial, (u8 *)ti_manuf_desc);
		if (status) {
			kfree(ti_manuf_desc);
			goto stayinbootmode;
		}

		/* Check for version 2 */
		if (!ignore_cpu_rev && ti_cpu_rev(ti_manuf_desc) < 2) {
			dev_dbg(dev, "%s - Wrong CPU Rev %d (Must be 2)\n",
				__func__, ti_cpu_rev(ti_manuf_desc));
			kfree(ti_manuf_desc);
			goto stayinbootmode;
		}

		kfree(ti_manuf_desc);

		/*
		 * In order to update the I2C firmware we must change the type
		 * 2 record to type 0xF2. This will force the UMP to come up
		 * in Boot Mode.  Then while in boot mode, the driver will
		 * download the latest firmware (padded to 15.5k) into the
		 * UMP ram. Finally when the device comes back up in download
		 * mode the driver will cause the new firmware to be copied
		 * from the UMP Ram to I2C and the firmware will update the
		 * record type from 0xf2 to 0x02.
		 *
		 * Do we really have to copy the whole firmware image,
		 * or could we do this in place!
		 */

		/* Allocate a 15.5k buffer + 3 byte header */
		buffer_size = (((1024 * 16) - 512) +
					sizeof(struct ti_i2c_image_header));
		buffer = kmalloc(buffer_size, GFP_KERNEL);
		if (!buffer)
			return -ENOMEM;

		/* Initialize the buffer to 0xff (pad the buffer) */
		memset(buffer, 0xff, buffer_size);
		memcpy(buffer, &fw->data[4], fw->size - 4);

		for (i = sizeof(struct ti_i2c_image_header);
				i < buffer_size; i++) {
			cs = (u8)(cs + buffer[i]);
		}

		header = (struct ti_i2c_image_header *)buffer;

		/* update length and checksum after padding */
		header->Length = cpu_to_le16((u16)(buffer_size -
					sizeof(struct ti_i2c_image_header)));
		header->CheckSum = cs;

		/* Download the operational code  */
		dev_dbg(dev, "%s - Downloading operational code image version %d.%d (TI UMP)\n",
				__func__,
				fw_hdr->major_version, fw_hdr->minor_version);
		status = download_code(serial, buffer, buffer_size);

		kfree(buffer);

		if (status) {
			dev_dbg(dev, "%s - Error downloading operational code image\n", __func__);
			return status;
		}

		/* Device will reboot */
		serial->product_info.TiMode = TI_MODE_TRANSITIONING;

		dev_dbg(dev, "%s - Download successful -- Device rebooting...\n", __func__);

		return 1;
	}

stayinbootmode:
	/* Eprom is invalid or blank stay in boot mode */
	dev_dbg(dev, "%s - STAYING IN BOOT MODE\n", __func__);
	serial->product_info.TiMode = TI_MODE_BOOT;

	return 1;
}

static int ti_do_config(struct edgeport_port *port, int feature, int on)
{
	on = !!on;	/* 1 or 0 not bitmask */

	return send_port_cmd(port->port, feature, on, NULL, 0);
}

static int restore_mcr(struct edgeport_port *port, u8 mcr)
{
	int status = 0;

	dev_dbg(&port->port->dev, "%s - %x\n", __func__, mcr);

	status = ti_do_config(port, UMPC_SET_CLR_DTR, mcr & MCR_DTR);
	if (status)
		return status;
	status = ti_do_config(port, UMPC_SET_CLR_RTS, mcr & MCR_RTS);
	if (status)
		return status;
	return ti_do_config(port, UMPC_SET_CLR_LOOPBACK, mcr & MCR_LOOPBACK);
}

/* Convert TI LSR to standard UART flags */
static u8 map_line_status(u8 ti_lsr)
{
	u8 lsr = 0;

#define MAP_FLAG(flagUmp, flagUart)    \
	if (ti_lsr & flagUmp) \
		lsr |= flagUart;

	MAP_FLAG(UMP_UART_LSR_OV_MASK, LSR_OVER_ERR)	/* overrun */
	MAP_FLAG(UMP_UART_LSR_PE_MASK, LSR_PAR_ERR)	/* parity error */
	MAP_FLAG(UMP_UART_LSR_FE_MASK, LSR_FRM_ERR)	/* framing error */
	MAP_FLAG(UMP_UART_LSR_BR_MASK, LSR_BREAK)	/* break detected */
	MAP_FLAG(UMP_UART_LSR_RX_MASK, LSR_RX_AVAIL)	/* rx data available */
	MAP_FLAG(UMP_UART_LSR_TX_MASK, LSR_TX_EMPTY)	/* tx hold reg empty */

#undef MAP_FLAG

	return lsr;
}

static void handle_new_msr(struct edgeport_port *edge_port, u8 msr)
{
	struct async_icount *icount;
	struct tty_struct *tty;

	dev_dbg(&edge_port->port->dev, "%s - %02x\n", __func__, msr);

	if (msr & (EDGEPORT_MSR_DELTA_CTS | EDGEPORT_MSR_DELTA_DSR |
			EDGEPORT_MSR_DELTA_RI | EDGEPORT_MSR_DELTA_CD)) {
		icount = &edge_port->port->icount;

		/* update input line counters */
		if (msr & EDGEPORT_MSR_DELTA_CTS)
			icount->cts++;
		if (msr & EDGEPORT_MSR_DELTA_DSR)
			icount->dsr++;
		if (msr & EDGEPORT_MSR_DELTA_CD)
			icount->dcd++;
		if (msr & EDGEPORT_MSR_DELTA_RI)
			icount->rng++;
		wake_up_interruptible(&edge_port->port->port.delta_msr_wait);
	}

	/* Save the new modem status */
	edge_port->shadow_msr = msr & 0xf0;

	tty = tty_port_tty_get(&edge_port->port->port);
	/* handle CTS flow control */
	if (tty && C_CRTSCTS(tty)) {
		if (msr & EDGEPORT_MSR_CTS)
			tty_wakeup(tty);
	}
	tty_kref_put(tty);
}

static void handle_new_lsr(struct edgeport_port *edge_port, int lsr_data,
							u8 lsr, u8 data)
{
	struct async_icount *icount;
	u8 new_lsr = (u8)(lsr & (u8)(LSR_OVER_ERR | LSR_PAR_ERR |
						LSR_FRM_ERR | LSR_BREAK));

	dev_dbg(&edge_port->port->dev, "%s - %02x\n", __func__, new_lsr);

	edge_port->shadow_lsr = lsr;

	if (new_lsr & LSR_BREAK)
		/*
		 * Parity and Framing errors only count if they
		 * occur exclusive of a break being received.
		 */
		new_lsr &= (u8)(LSR_OVER_ERR | LSR_BREAK);

	/* Place LSR data byte into Rx buffer */
	if (lsr_data)
		edge_tty_recv(edge_port->port, &data, 1);

	/* update input line counters */
	icount = &edge_port->port->icount;
	if (new_lsr & LSR_BREAK)
		icount->brk++;
	if (new_lsr & LSR_OVER_ERR)
		icount->overrun++;
	if (new_lsr & LSR_PAR_ERR)
		icount->parity++;
	if (new_lsr & LSR_FRM_ERR)
		icount->frame++;
}

static void edge_interrupt_callback(struct urb *urb)
{
	struct edgeport_serial *edge_serial = urb->context;
	struct usb_serial_port *port;
	struct edgeport_port *edge_port;
	struct device *dev;
	unsigned char *data = urb->transfer_buffer;
	int length = urb->actual_length;
	int port_number;
	int function;
	int retval;
	u8 lsr;
	u8 msr;
	int status = urb->status;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(&urb->dev->dev, "%s - urb shutting down with status: %d\n",
		    __func__, status);
		return;
	default:
		dev_err(&urb->dev->dev, "%s - nonzero urb status received: "
			"%d\n", __func__, status);
		goto exit;
	}

	if (!length) {
		dev_dbg(&urb->dev->dev, "%s - no data in urb\n", __func__);
		goto exit;
	}

	dev = &edge_serial->serial->dev->dev;
	usb_serial_debug_data(dev, __func__, length, data);

	if (length != 2) {
		dev_dbg(dev, "%s - expecting packet of size 2, got %d\n", __func__, length);
		goto exit;
	}

	port_number = TIUMP_GET_PORT_FROM_CODE(data[0]);
	function    = TIUMP_GET_FUNC_FROM_CODE(data[0]);
	dev_dbg(dev, "%s - port_number %d, function %d, info 0x%x\n", __func__,
		port_number, function, data[1]);

	if (port_number >= edge_serial->serial->num_ports) {
		dev_err(dev, "bad port number %d\n", port_number);
		goto exit;
	}

	port = edge_serial->serial->port[port_number];
	edge_port = usb_get_serial_port_data(port);
	if (!edge_port) {
		dev_dbg(dev, "%s - edge_port not found\n", __func__);
		return;
	}
	switch (function) {
	case TIUMP_INTERRUPT_CODE_LSR:
		lsr = map_line_status(data[1]);
		if (lsr & UMP_UART_LSR_DATA_MASK) {
			/*
			 * Save the LSR event for bulk read completion routine
			 */
			dev_dbg(dev, "%s - LSR Event Port %u LSR Status = %02x\n",
				__func__, port_number, lsr);
			edge_port->lsr_event = 1;
			edge_port->lsr_mask = lsr;
		} else {
			dev_dbg(dev, "%s - ===== Port %d LSR Status = %02x ======\n",
				__func__, port_number, lsr);
			handle_new_lsr(edge_port, 0, lsr, 0);
		}
		break;

	case TIUMP_INTERRUPT_CODE_MSR:	/* MSR */
		/* Copy MSR from UMP */
		msr = data[1];
		dev_dbg(dev, "%s - ===== Port %u MSR Status = %02x ======\n",
			__func__, port_number, msr);
		handle_new_msr(edge_port, msr);
		break;

	default:
		dev_err(&urb->dev->dev,
			"%s - Unknown Interrupt code from UMP %x\n",
			__func__, data[1]);
		break;

	}

exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval)
		dev_err(&urb->dev->dev,
			"%s - usb_submit_urb failed with result %d\n",
			 __func__, retval);
}

static void edge_bulk_in_callback(struct urb *urb)
{
	struct edgeport_port *edge_port = urb->context;
	struct device *dev = &edge_port->port->dev;
	unsigned char *data = urb->transfer_buffer;
	unsigned long flags;
	int retval = 0;
	int port_number;
	int status = urb->status;

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
		dev_err(&urb->dev->dev, "%s - nonzero read bulk status received: %d\n", __func__, status);
	}

	if (status == -EPIPE)
		goto exit;

	if (status) {
		dev_err(&urb->dev->dev, "%s - stopping read!\n", __func__);
		return;
	}

	port_number = edge_port->port->port_number;

	if (urb->actual_length > 0 && edge_port->lsr_event) {
		edge_port->lsr_event = 0;
		dev_dbg(dev, "%s ===== Port %u LSR Status = %02x, Data = %02x ======\n",
			__func__, port_number, edge_port->lsr_mask, *data);
		handle_new_lsr(edge_port, 1, edge_port->lsr_mask, *data);
		/* Adjust buffer length/pointer */
		--urb->actual_length;
		++data;
	}

	if (urb->actual_length) {
		usb_serial_debug_data(dev, __func__, urb->actual_length, data);
		if (edge_port->close_pending)
			dev_dbg(dev, "%s - close pending, dropping data on the floor\n",
								__func__);
		else
			edge_tty_recv(edge_port->port, data,
					urb->actual_length);
		edge_port->port->icount.rx += urb->actual_length;
	}

exit:
	/* continue read unless stopped */
	spin_lock_irqsave(&edge_port->ep_lock, flags);
	if (edge_port->ep_read_urb_state == EDGE_READ_URB_RUNNING)
		retval = usb_submit_urb(urb, GFP_ATOMIC);
	else if (edge_port->ep_read_urb_state == EDGE_READ_URB_STOPPING)
		edge_port->ep_read_urb_state = EDGE_READ_URB_STOPPED;

	spin_unlock_irqrestore(&edge_port->ep_lock, flags);
	if (retval)
		dev_err(dev, "%s - usb_submit_urb failed with result %d\n", __func__, retval);
}

static void edge_tty_recv(struct usb_serial_port *port, unsigned char *data,
		int length)
{
	int queued;

	queued = tty_insert_flip_string(&port->port, data, length);
	if (queued < length)
		dev_err(&port->dev, "%s - dropping data, %d bytes lost\n",
			__func__, length - queued);
	tty_flip_buffer_push(&port->port);
}

static void edge_bulk_out_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int status = urb->status;
	struct tty_struct *tty;

	edge_port->ep_write_urb_in_use = 0;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(&urb->dev->dev, "%s - urb shutting down with status: %d\n",
		    __func__, status);
		return;
	default:
		dev_err_console(port, "%s - nonzero write bulk status "
			"received: %d\n", __func__, status);
	}

	/* send any buffered data */
	tty = tty_port_tty_get(&port->port);
	edge_send(port, tty);
	tty_kref_put(tty);
}

static int edge_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	struct edgeport_serial *edge_serial;
	struct usb_device *dev;
	struct urb *urb;
	int status;
	u16 open_settings;
	u8 transaction_timeout;

	if (edge_port == NULL)
		return -ENODEV;

	dev = port->serial->dev;

	/* turn off loopback */
	status = ti_do_config(edge_port, UMPC_SET_CLR_LOOPBACK, 0);
	if (status) {
		dev_err(&port->dev,
				"%s - cannot send clear loopback command, %d\n",
			__func__, status);
		return status;
	}

	/* set up the port settings */
	if (tty)
		edge_set_termios(tty, port, &tty->termios);

	/* open up the port */

	/* milliseconds to timeout for DMA transfer */
	transaction_timeout = 2;

	edge_port->ump_read_timeout =
				max(20, ((transaction_timeout * 3) / 2));

	/* milliseconds to timeout for DMA transfer */
	open_settings = (u8)(UMP_DMA_MODE_CONTINOUS |
			     UMP_PIPE_TRANS_TIMEOUT_ENA |
			     (transaction_timeout << 2));

	dev_dbg(&port->dev, "%s - Sending UMPC_OPEN_PORT\n", __func__);

	/* Tell TI to open and start the port */
	status = send_port_cmd(port, UMPC_OPEN_PORT, open_settings, NULL, 0);
	if (status) {
		dev_err(&port->dev, "%s - cannot send open command, %d\n",
							__func__, status);
		return status;
	}

	/* Start the DMA? */
	status = send_port_cmd(port, UMPC_START_PORT, 0, NULL, 0);
	if (status) {
		dev_err(&port->dev, "%s - cannot send start DMA command, %d\n",
							__func__, status);
		return status;
	}

	/* Clear TX and RX buffers in UMP */
	status = purge_port(port, UMP_PORT_DIR_OUT | UMP_PORT_DIR_IN);
	if (status) {
		dev_err(&port->dev,
			"%s - cannot send clear buffers command, %d\n",
			__func__, status);
		return status;
	}

	/* Read Initial MSR */
	status = read_port_cmd(port, UMPC_READ_MSR, 0, &edge_port->shadow_msr, 1);
	if (status) {
		dev_err(&port->dev, "%s - cannot send read MSR command, %d\n",
							__func__, status);
		return status;
	}

	dev_dbg(&port->dev, "ShadowMSR 0x%X\n", edge_port->shadow_msr);

	/* Set Initial MCR */
	edge_port->shadow_mcr = MCR_RTS | MCR_DTR;
	dev_dbg(&port->dev, "ShadowMCR 0x%X\n", edge_port->shadow_mcr);

	edge_serial = edge_port->edge_serial;
	if (mutex_lock_interruptible(&edge_serial->es_lock))
		return -ERESTARTSYS;
	if (edge_serial->num_ports_open == 0) {
		/* we are the first port to open, post the interrupt urb */
		urb = edge_serial->serial->port[0]->interrupt_in_urb;
		urb->context = edge_serial;
		status = usb_submit_urb(urb, GFP_KERNEL);
		if (status) {
			dev_err(&port->dev,
				"%s - usb_submit_urb failed with value %d\n",
					__func__, status);
			goto release_es_lock;
		}
	}

	/*
	 * reset the data toggle on the bulk endpoints to work around bug in
	 * host controllers where things get out of sync some times
	 */
	usb_clear_halt(dev, port->write_urb->pipe);
	usb_clear_halt(dev, port->read_urb->pipe);

	/* start up our bulk read urb */
	urb = port->read_urb;
	edge_port->ep_read_urb_state = EDGE_READ_URB_RUNNING;
	urb->context = edge_port;
	status = usb_submit_urb(urb, GFP_KERNEL);
	if (status) {
		dev_err(&port->dev,
			"%s - read bulk usb_submit_urb failed with value %d\n",
				__func__, status);
		goto unlink_int_urb;
	}

	++edge_serial->num_ports_open;

	goto release_es_lock;

unlink_int_urb:
	if (edge_port->edge_serial->num_ports_open == 0)
		usb_kill_urb(port->serial->port[0]->interrupt_in_urb);
release_es_lock:
	mutex_unlock(&edge_serial->es_lock);
	return status;
}

static void edge_close(struct usb_serial_port *port)
{
	struct edgeport_serial *edge_serial;
	struct edgeport_port *edge_port;
	unsigned long flags;

	edge_serial = usb_get_serial_data(port->serial);
	edge_port = usb_get_serial_port_data(port);
	if (edge_serial == NULL || edge_port == NULL)
		return;

	/*
	 * The bulkreadcompletion routine will check
	 * this flag and dump add read data
	 */
	edge_port->close_pending = 1;

	usb_kill_urb(port->read_urb);
	usb_kill_urb(port->write_urb);
	edge_port->ep_write_urb_in_use = 0;
	spin_lock_irqsave(&edge_port->ep_lock, flags);
	kfifo_reset_out(&port->write_fifo);
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	dev_dbg(&port->dev, "%s - send umpc_close_port\n", __func__);
	send_port_cmd(port, UMPC_CLOSE_PORT, 0, NULL, 0);

	mutex_lock(&edge_serial->es_lock);
	--edge_port->edge_serial->num_ports_open;
	if (edge_port->edge_serial->num_ports_open <= 0) {
		/* last port is now closed, let's shut down our interrupt urb */
		usb_kill_urb(port->serial->port[0]->interrupt_in_urb);
		edge_port->edge_serial->num_ports_open = 0;
	}
	mutex_unlock(&edge_serial->es_lock);
	edge_port->close_pending = 0;
}

static int edge_write(struct tty_struct *tty, struct usb_serial_port *port,
				const unsigned char *data, int count)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);

	if (count == 0) {
		dev_dbg(&port->dev, "%s - write request of 0 bytes\n", __func__);
		return 0;
	}

	if (edge_port == NULL)
		return -ENODEV;
	if (edge_port->close_pending == 1)
		return -ENODEV;

	count = kfifo_in_locked(&port->write_fifo, data, count,
							&edge_port->ep_lock);
	edge_send(port, tty);

	return count;
}

static void edge_send(struct usb_serial_port *port, struct tty_struct *tty)
{
	int count, result;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	if (edge_port->ep_write_urb_in_use) {
		spin_unlock_irqrestore(&edge_port->ep_lock, flags);
		return;
	}

	count = kfifo_out(&port->write_fifo,
				port->write_urb->transfer_buffer,
				port->bulk_out_size);

	if (count == 0) {
		spin_unlock_irqrestore(&edge_port->ep_lock, flags);
		return;
	}

	edge_port->ep_write_urb_in_use = 1;

	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	usb_serial_debug_data(&port->dev, __func__, count, port->write_urb->transfer_buffer);

	/* set up our urb */
	port->write_urb->transfer_buffer_length = count;

	/* send the data out the bulk port */
	result = usb_submit_urb(port->write_urb, GFP_ATOMIC);
	if (result) {
		dev_err_console(port,
			"%s - failed submitting write urb, error %d\n",
				__func__, result);
		edge_port->ep_write_urb_in_use = 0;
		/* TODO: reschedule edge_send */
	} else
		edge_port->port->icount.tx += count;

	/*
	 * wakeup any process waiting for writes to complete
	 * there is now more room in the buffer for new writes
	 */
	if (tty)
		tty_wakeup(tty);
}

static int edge_write_room(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int room = 0;
	unsigned long flags;

	if (edge_port == NULL)
		return 0;
	if (edge_port->close_pending == 1)
		return 0;

	spin_lock_irqsave(&edge_port->ep_lock, flags);
	room = kfifo_avail(&port->write_fifo);
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	dev_dbg(&port->dev, "%s - returns %d\n", __func__, room);
	return room;
}

static int edge_chars_in_buffer(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int chars = 0;
	unsigned long flags;
	if (edge_port == NULL)
		return 0;

	spin_lock_irqsave(&edge_port->ep_lock, flags);
	chars = kfifo_len(&port->write_fifo);
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	dev_dbg(&port->dev, "%s - returns %d\n", __func__, chars);
	return chars;
}

static bool edge_tx_empty(struct usb_serial_port *port)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int ret;

	ret = tx_active(edge_port);
	if (ret > 0)
		return false;

	return true;
}

static void edge_throttle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int status;

	if (edge_port == NULL)
		return;

	/* if we are implementing XON/XOFF, send the stop character */
	if (I_IXOFF(tty)) {
		unsigned char stop_char = STOP_CHAR(tty);
		status = edge_write(tty, port, &stop_char, 1);
		if (status <= 0) {
			dev_err(&port->dev, "%s - failed to write stop character, %d\n", __func__, status);
		}
	}

	/*
	 * if we are implementing RTS/CTS, stop reads
	 * and the Edgeport will clear the RTS line
	 */
	if (C_CRTSCTS(tty))
		stop_read(edge_port);

}

static void edge_unthrottle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int status;

	if (edge_port == NULL)
		return;

	/* if we are implementing XON/XOFF, send the start character */
	if (I_IXOFF(tty)) {
		unsigned char start_char = START_CHAR(tty);
		status = edge_write(tty, port, &start_char, 1);
		if (status <= 0) {
			dev_err(&port->dev, "%s - failed to write start character, %d\n", __func__, status);
		}
	}
	/*
	 * if we are implementing RTS/CTS, restart reads
	 * are the Edgeport will assert the RTS line
	 */
	if (C_CRTSCTS(tty)) {
		status = restart_read(edge_port);
		if (status)
			dev_err(&port->dev,
				"%s - read bulk usb_submit_urb failed: %d\n",
							__func__, status);
	}

}

static void stop_read(struct edgeport_port *edge_port)
{
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	if (edge_port->ep_read_urb_state == EDGE_READ_URB_RUNNING)
		edge_port->ep_read_urb_state = EDGE_READ_URB_STOPPING;
	edge_port->shadow_mcr &= ~MCR_RTS;

	spin_unlock_irqrestore(&edge_port->ep_lock, flags);
}

static int restart_read(struct edgeport_port *edge_port)
{
	struct urb *urb;
	int status = 0;
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	if (edge_port->ep_read_urb_state == EDGE_READ_URB_STOPPED) {
		urb = edge_port->port->read_urb;
		status = usb_submit_urb(urb, GFP_ATOMIC);
	}
	edge_port->ep_read_urb_state = EDGE_READ_URB_RUNNING;
	edge_port->shadow_mcr |= MCR_RTS;

	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	return status;
}

static void change_port_settings(struct tty_struct *tty,
		struct edgeport_port *edge_port, struct ktermios *old_termios)
{
	struct device *dev = &edge_port->port->dev;
	struct ump_uart_config *config;
	int baud;
	unsigned cflag;
	int status;

	config = kmalloc (sizeof (*config), GFP_KERNEL);
	if (!config) {
		tty->termios = *old_termios;
		return;
	}

	cflag = tty->termios.c_cflag;

	config->wFlags = 0;

	/* These flags must be set */
	config->wFlags |= UMP_MASK_UART_FLAGS_RECEIVE_MS_INT;
	config->wFlags |= UMP_MASK_UART_FLAGS_AUTO_START_ON_ERR;
	config->bUartMode = (u8)(edge_port->bUartMode);

	switch (cflag & CSIZE) {
	case CS5:
		    config->bDataBits = UMP_UART_CHAR5BITS;
		    dev_dbg(dev, "%s - data bits = 5\n", __func__);
		    break;
	case CS6:
		    config->bDataBits = UMP_UART_CHAR6BITS;
		    dev_dbg(dev, "%s - data bits = 6\n", __func__);
		    break;
	case CS7:
		    config->bDataBits = UMP_UART_CHAR7BITS;
		    dev_dbg(dev, "%s - data bits = 7\n", __func__);
		    break;
	default:
	case CS8:
		    config->bDataBits = UMP_UART_CHAR8BITS;
		    dev_dbg(dev, "%s - data bits = 8\n", __func__);
			    break;
	}

	if (cflag & PARENB) {
		if (cflag & PARODD) {
			config->wFlags |= UMP_MASK_UART_FLAGS_PARITY;
			config->bParity = UMP_UART_ODDPARITY;
			dev_dbg(dev, "%s - parity = odd\n", __func__);
		} else {
			config->wFlags |= UMP_MASK_UART_FLAGS_PARITY;
			config->bParity = UMP_UART_EVENPARITY;
			dev_dbg(dev, "%s - parity = even\n", __func__);
		}
	} else {
		config->bParity = UMP_UART_NOPARITY;
		dev_dbg(dev, "%s - parity = none\n", __func__);
	}

	if (cflag & CSTOPB) {
		config->bStopBits = UMP_UART_STOPBIT2;
		dev_dbg(dev, "%s - stop bits = 2\n", __func__);
	} else {
		config->bStopBits = UMP_UART_STOPBIT1;
		dev_dbg(dev, "%s - stop bits = 1\n", __func__);
	}

	/* figure out the flow control settings */
	if (cflag & CRTSCTS) {
		config->wFlags |= UMP_MASK_UART_FLAGS_OUT_X_CTS_FLOW;
		config->wFlags |= UMP_MASK_UART_FLAGS_RTS_FLOW;
		dev_dbg(dev, "%s - RTS/CTS is enabled\n", __func__);
	} else {
		dev_dbg(dev, "%s - RTS/CTS is disabled\n", __func__);
		restart_read(edge_port);
	}

	/*
	 * if we are implementing XON/XOFF, set the start and stop
	 * character in the device
	 */
	config->cXon  = START_CHAR(tty);
	config->cXoff = STOP_CHAR(tty);

	/* if we are implementing INBOUND XON/XOFF */
	if (I_IXOFF(tty)) {
		config->wFlags |= UMP_MASK_UART_FLAGS_IN_X;
		dev_dbg(dev, "%s - INBOUND XON/XOFF is enabled, XON = %2x, XOFF = %2x\n",
			__func__, config->cXon, config->cXoff);
	} else
		dev_dbg(dev, "%s - INBOUND XON/XOFF is disabled\n", __func__);

	/* if we are implementing OUTBOUND XON/XOFF */
	if (I_IXON(tty)) {
		config->wFlags |= UMP_MASK_UART_FLAGS_OUT_X;
		dev_dbg(dev, "%s - OUTBOUND XON/XOFF is enabled, XON = %2x, XOFF = %2x\n",
			__func__, config->cXon, config->cXoff);
	} else
		dev_dbg(dev, "%s - OUTBOUND XON/XOFF is disabled\n", __func__);

	tty->termios.c_cflag &= ~CMSPAR;

	/* Round the baud rate */
	baud = tty_get_baud_rate(tty);
	if (!baud) {
		/* pick a default, any default... */
		baud = 9600;
	} else {
		/* Avoid a zero divisor. */
		baud = min(baud, 461550);
		tty_encode_baud_rate(tty, baud, baud);
	}

	edge_port->baud_rate = baud;
	config->wBaudRate = (u16)((461550L + baud/2) / baud);

	/* FIXME: Recompute actual baud from divisor here */

	dev_dbg(dev, "%s - baud rate = %d, wBaudRate = %d\n", __func__, baud, config->wBaudRate);

	dev_dbg(dev, "wBaudRate:   %d\n", (int)(461550L / config->wBaudRate));
	dev_dbg(dev, "wFlags:    0x%x\n", config->wFlags);
	dev_dbg(dev, "bDataBits:   %d\n", config->bDataBits);
	dev_dbg(dev, "bParity:     %d\n", config->bParity);
	dev_dbg(dev, "bStopBits:   %d\n", config->bStopBits);
	dev_dbg(dev, "cXon:        %d\n", config->cXon);
	dev_dbg(dev, "cXoff:       %d\n", config->cXoff);
	dev_dbg(dev, "bUartMode:   %d\n", config->bUartMode);

	/* move the word values into big endian mode */
	cpu_to_be16s(&config->wFlags);
	cpu_to_be16s(&config->wBaudRate);

	status = send_port_cmd(edge_port->port, UMPC_SET_CONFIG, 0, config,
			sizeof(*config));
	if (status)
		dev_dbg(dev, "%s - error %d when trying to write config to device\n",
			__func__, status);
	kfree(config);
}

static void edge_set_termios(struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);

	if (edge_port == NULL)
		return;
	/* change the port settings to the new ones specified */
	change_port_settings(tty, edge_port, old_termios);
}

static int edge_tiocmset(struct tty_struct *tty,
					unsigned int set, unsigned int clear)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	unsigned int mcr;
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);
	mcr = edge_port->shadow_mcr;
	if (set & TIOCM_RTS)
		mcr |= MCR_RTS;
	if (set & TIOCM_DTR)
		mcr |= MCR_DTR;
	if (set & TIOCM_LOOP)
		mcr |= MCR_LOOPBACK;

	if (clear & TIOCM_RTS)
		mcr &= ~MCR_RTS;
	if (clear & TIOCM_DTR)
		mcr &= ~MCR_DTR;
	if (clear & TIOCM_LOOP)
		mcr &= ~MCR_LOOPBACK;

	edge_port->shadow_mcr = mcr;
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	restore_mcr(edge_port, mcr);
	return 0;
}

static int edge_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	unsigned int result = 0;
	unsigned int msr;
	unsigned int mcr;
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	msr = edge_port->shadow_msr;
	mcr = edge_port->shadow_mcr;
	result = ((mcr & MCR_DTR)	? TIOCM_DTR: 0)	  /* 0x002 */
		  | ((mcr & MCR_RTS)	? TIOCM_RTS: 0)   /* 0x004 */
		  | ((msr & EDGEPORT_MSR_CTS)	? TIOCM_CTS: 0)   /* 0x020 */
		  | ((msr & EDGEPORT_MSR_CD)	? TIOCM_CAR: 0)   /* 0x040 */
		  | ((msr & EDGEPORT_MSR_RI)	? TIOCM_RI:  0)   /* 0x080 */
		  | ((msr & EDGEPORT_MSR_DSR)	? TIOCM_DSR: 0);  /* 0x100 */


	dev_dbg(&port->dev, "%s -- %x\n", __func__, result);
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	return result;
}

static void edge_break(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int status;
	int bv = 0;	/* Off */

	if (break_state == -1)
		bv = 1;	/* On */
	status = ti_do_config(edge_port, UMPC_SET_CLR_BREAK, bv);
	if (status)
		dev_dbg(&port->dev, "%s - error %d sending break set/clear command.\n",
			__func__, status);
}

static void edge_heartbeat_schedule(struct edgeport_serial *edge_serial)
{
	if (!edge_serial->use_heartbeat)
		return;

	schedule_delayed_work(&edge_serial->heartbeat_work,
			FW_HEARTBEAT_SECS * HZ);
}

static void edge_heartbeat_work(struct work_struct *work)
{
	struct edgeport_serial *serial;
	struct ti_i2c_desc *rom_desc;

	serial = container_of(work, struct edgeport_serial,
			heartbeat_work.work);

	rom_desc = kmalloc(sizeof(*rom_desc), GFP_KERNEL);

	/* Descriptor address request is enough to reset the firmware timer */
	if (!rom_desc || !get_descriptor_addr(serial, I2C_DESC_TYPE_ION,
			rom_desc)) {
		dev_err(&serial->serial->interface->dev,
				"%s - Incomplete heartbeat\n", __func__);
	}
	kfree(rom_desc);

	edge_heartbeat_schedule(serial);
}

static int edge_calc_num_ports(struct usb_serial *serial,
				struct usb_serial_endpoints *epds)
{
	struct device *dev = &serial->interface->dev;
	unsigned char num_ports = serial->type->num_ports;

	/* Make sure we have the required endpoints when in download mode. */
	if (serial->interface->cur_altsetting->desc.bNumEndpoints > 1) {
		if (epds->num_bulk_in < num_ports ||
				epds->num_bulk_out < num_ports ||
				epds->num_interrupt_in < 1) {
			dev_err(dev, "required endpoints missing\n");
			return -ENODEV;
		}
	}

	return num_ports;
}

static int edge_startup(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial;
	int status;
	u16 product_id;

	/* create our private serial structure */
	edge_serial = kzalloc(sizeof(struct edgeport_serial), GFP_KERNEL);
	if (!edge_serial)
		return -ENOMEM;

	mutex_init(&edge_serial->es_lock);
	edge_serial->serial = serial;
	INIT_DELAYED_WORK(&edge_serial->heartbeat_work, edge_heartbeat_work);
	usb_set_serial_data(serial, edge_serial);

	status = download_fw(edge_serial);
	if (status < 0) {
		kfree(edge_serial);
		return status;
	}

	if (status > 0)
		return 1;	/* bind but do not register any ports */

	product_id = le16_to_cpu(
			edge_serial->serial->dev->descriptor.idProduct);

	/* Currently only the EP/416 models require heartbeat support */
	if (edge_serial->fw_version > FW_HEARTBEAT_VERSION_CUTOFF) {
		if (product_id == ION_DEVICE_ID_TI_EDGEPORT_416 ||
			product_id == ION_DEVICE_ID_TI_EDGEPORT_416B) {
			edge_serial->use_heartbeat = true;
		}
	}

	edge_heartbeat_schedule(edge_serial);

	return 0;
}

static void edge_disconnect(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	cancel_delayed_work_sync(&edge_serial->heartbeat_work);
}

static void edge_release(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	cancel_delayed_work_sync(&edge_serial->heartbeat_work);
	kfree(edge_serial);
}

static int edge_port_probe(struct usb_serial_port *port)
{
	struct edgeport_port *edge_port;
	int ret;

	edge_port = kzalloc(sizeof(*edge_port), GFP_KERNEL);
	if (!edge_port)
		return -ENOMEM;

	spin_lock_init(&edge_port->ep_lock);
	edge_port->port = port;
	edge_port->edge_serial = usb_get_serial_data(port->serial);
	edge_port->bUartMode = default_uart_mode;

	switch (port->port_number) {
	case 0:
		edge_port->uart_base = UMPMEM_BASE_UART1;
		edge_port->dma_address = UMPD_OEDB1_ADDRESS;
		break;
	case 1:
		edge_port->uart_base = UMPMEM_BASE_UART2;
		edge_port->dma_address = UMPD_OEDB2_ADDRESS;
		break;
	default:
		dev_err(&port->dev, "unknown port number\n");
		ret = -ENODEV;
		goto err;
	}

	dev_dbg(&port->dev,
		"%s - port_number = %d, uart_base = %04x, dma_address = %04x\n",
		__func__, port->port_number, edge_port->uart_base,
		edge_port->dma_address);

	usb_set_serial_port_data(port, edge_port);

	ret = edge_create_sysfs_attrs(port);
	if (ret)
		goto err;

	/*
	 * The LSR does not tell when the transmitter shift register has
	 * emptied so add a one-character drain delay.
	 */
	port->port.drain_delay = 1;

	return 0;
err:
	kfree(edge_port);

	return ret;
}

static void edge_port_remove(struct usb_serial_port *port)
{
	struct edgeport_port *edge_port;

	edge_port = usb_get_serial_port_data(port);
	edge_remove_sysfs_attrs(port);
	kfree(edge_port);
}

/* Sysfs Attributes */

static ssize_t uart_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct usb_serial_port *port = to_usb_serial_port(dev);
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);

	return sprintf(buf, "%d\n", edge_port->bUartMode);
}

static ssize_t uart_mode_store(struct device *dev,
	struct device_attribute *attr, const char *valbuf, size_t count)
{
	struct usb_serial_port *port = to_usb_serial_port(dev);
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	unsigned int v = simple_strtoul(valbuf, NULL, 0);

	dev_dbg(dev, "%s: setting uart_mode = %d\n", __func__, v);

	if (v < 256)
		edge_port->bUartMode = v;
	else
		dev_err(dev, "%s - uart_mode %d is invalid\n", __func__, v);

	return count;
}
static DEVICE_ATTR_RW(uart_mode);

static int edge_create_sysfs_attrs(struct usb_serial_port *port)
{
	return device_create_file(&port->dev, &dev_attr_uart_mode);
}

static int edge_remove_sysfs_attrs(struct usb_serial_port *port)
{
	device_remove_file(&port->dev, &dev_attr_uart_mode);
	return 0;
}

#ifdef CONFIG_PM
static int edge_suspend(struct usb_serial *serial, pm_message_t message)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	cancel_delayed_work_sync(&edge_serial->heartbeat_work);

	return 0;
}

static int edge_resume(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	edge_heartbeat_schedule(edge_serial);

	return 0;
}
#endif

static struct usb_serial_driver edgeport_1port_device = {
	.driver = {
		.owner		= THIS_MODULE,
		.name		= "edgeport_ti_1",
	},
	.description		= "Edgeport TI 1 port adapter",
	.id_table		= edgeport_1port_id_table,
	.num_ports		= 1,
	.num_bulk_out		= 1,
	.open			= edge_open,
	.close			= edge_close,
	.throttle		= edge_throttle,
	.unthrottle		= edge_unthrottle,
	.attach			= edge_startup,
	.calc_num_ports		= edge_calc_num_ports,
	.disconnect		= edge_disconnect,
	.release		= edge_release,
	.port_probe		= edge_port_probe,
	.port_remove		= edge_port_remove,
	.set_termios		= edge_set_termios,
	.tiocmget		= edge_tiocmget,
	.tiocmset		= edge_tiocmset,
	.tiocmiwait		= usb_serial_generic_tiocmiwait,
	.get_icount		= usb_serial_generic_get_icount,
	.write			= edge_write,
	.write_room		= edge_write_room,
	.chars_in_buffer	= edge_chars_in_buffer,
	.tx_empty		= edge_tx_empty,
	.break_ctl		= edge_break,
	.read_int_callback	= edge_interrupt_callback,
	.read_bulk_callback	= edge_bulk_in_callback,
	.write_bulk_callback	= edge_bulk_out_callback,
#ifdef CONFIG_PM
	.suspend		= edge_suspend,
	.resume			= edge_resume,
#endif
};

static struct usb_serial_driver edgeport_2port_device = {
	.driver = {
		.owner		= THIS_MODULE,
		.name		= "edgeport_ti_2",
	},
	.description		= "Edgeport TI 2 port adapter",
	.id_table		= edgeport_2port_id_table,
	.num_ports		= 2,
	.num_bulk_out		= 1,
	.open			= edge_open,
	.close			= edge_close,
	.throttle		= edge_throttle,
	.unthrottle		= edge_unthrottle,
	.attach			= edge_startup,
	.calc_num_ports		= edge_calc_num_ports,
	.disconnect		= edge_disconnect,
	.release		= edge_release,
	.port_probe		= edge_port_probe,
	.port_remove		= edge_port_remove,
	.set_termios		= edge_set_termios,
	.tiocmget		= edge_tiocmget,
	.tiocmset		= edge_tiocmset,
	.tiocmiwait		= usb_serial_generic_tiocmiwait,
	.get_icount		= usb_serial_generic_get_icount,
	.write			= edge_write,
	.write_room		= edge_write_room,
	.chars_in_buffer	= edge_chars_in_buffer,
	.tx_empty		= edge_tx_empty,
	.break_ctl		= edge_break,
	.read_int_callback	= edge_interrupt_callback,
	.read_bulk_callback	= edge_bulk_in_callback,
	.write_bulk_callback	= edge_bulk_out_callback,
#ifdef CONFIG_PM
	.suspend		= edge_suspend,
	.resume			= edge_resume,
#endif
};

static struct usb_serial_driver * const serial_drivers[] = {
	&edgeport_1port_device, &edgeport_2port_device, NULL
};

module_usb_serial_driver(serial_drivers, id_table_combined);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_FIRMWARE("edgeport/down3.bin");

module_param(ignore_cpu_rev, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(ignore_cpu_rev,
			"Ignore the cpu revision when connecting to a device");

module_param(default_uart_mode, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(default_uart_mode, "Default uart_mode, 0=RS232, ...");
