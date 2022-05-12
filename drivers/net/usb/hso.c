// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 *
 * Driver for Option High Speed Mobile Devices.
 *
 *  Copyright (C) 2008 Option International
 *                     Filip Aben <f.aben@option.com>
 *                     Denis Joseph Barrow <d.barow@option.com>
 *                     Jan Dumon <j.dumon@option.com>
 *  Copyright (C) 2007 Andrew Bird (Sphere Systems Ltd)
 *  			<ajb@spheresystems.co.uk>
 *  Copyright (C) 2008 Greg Kroah-Hartman <gregkh@suse.de>
 *  Copyright (C) 2008 Novell, Inc.
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Description of the device:
 *
 * Interface 0:	Contains the IP network interface on the bulk end points.
 *		The multiplexed serial ports are using the interrupt and
 *		control endpoints.
 *		Interrupt contains a bitmap telling which multiplexed
 *		serialport needs servicing.
 *
 * Interface 1:	Diagnostics port, uses bulk only, do not submit urbs until the
 *		port is opened, as this have a huge impact on the network port
 *		throughput.
 *
 * Interface 2:	Standard modem interface - circuit switched interface, this
 *		can be used to make a standard ppp connection however it
 *              should not be used in conjunction with the IP network interface
 *              enabled for USB performance reasons i.e. if using this set
 *              ideally disable_net=1.
 *
 *****************************************************************************/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/ethtool.h>
#include <linux/usb.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/kmod.h>
#include <linux/rfkill.h>
#include <linux/ip.h>
#include <linux/uaccess.h>
#include <linux/usb/cdc.h>
#include <net/arp.h>
#include <asm/byteorder.h>
#include <linux/serial_core.h>
#include <linux/serial.h>


#define MOD_AUTHOR			"Option Wireless"
#define MOD_DESCRIPTION			"USB High Speed Option driver"

#define HSO_MAX_NET_DEVICES		10
#define HSO__MAX_MTU			2048
#define DEFAULT_MTU			1500
#define DEFAULT_MRU			1500

#define CTRL_URB_RX_SIZE		1024
#define CTRL_URB_TX_SIZE		64

#define BULK_URB_RX_SIZE		4096
#define BULK_URB_TX_SIZE		8192

#define MUX_BULK_RX_BUF_SIZE		HSO__MAX_MTU
#define MUX_BULK_TX_BUF_SIZE		HSO__MAX_MTU
#define MUX_BULK_RX_BUF_COUNT		4
#define USB_TYPE_OPTION_VENDOR		0x20

/* These definitions are used with the struct hso_net flags element */
/* - use *_bit operations on it. (bit indices not values.) */
#define HSO_NET_RUNNING			0

#define	HSO_NET_TX_TIMEOUT		(HZ*10)

#define HSO_SERIAL_MAGIC		0x48534f31

/* Number of ttys to handle */
#define HSO_SERIAL_TTY_MINORS		256

#define MAX_RX_URBS			2

/*****************************************************************************/
/* Debugging functions                                                       */
/*****************************************************************************/
#define hso_dbg(lvl, fmt, ...)						\
do {									\
	if ((lvl) & debug)						\
		pr_info("[%d:%s] " fmt,					\
			__LINE__, __func__, ##__VA_ARGS__);		\
} while (0)

/*****************************************************************************/
/* Enumerators                                                               */
/*****************************************************************************/
enum pkt_parse_state {
	WAIT_IP,
	WAIT_DATA,
	WAIT_SYNC
};

/*****************************************************************************/
/* Structs                                                                   */
/*****************************************************************************/

struct hso_shared_int {
	struct usb_endpoint_descriptor *intr_endp;
	void *shared_intr_buf;
	struct urb *shared_intr_urb;
	struct usb_device *usb;
	int use_count;
	int ref_count;
	struct mutex shared_int_lock;
};

struct hso_net {
	struct hso_device *parent;
	struct net_device *net;
	struct rfkill *rfkill;
	char name[24];

	struct usb_endpoint_descriptor *in_endp;
	struct usb_endpoint_descriptor *out_endp;

	struct urb *mux_bulk_rx_urb_pool[MUX_BULK_RX_BUF_COUNT];
	struct urb *mux_bulk_tx_urb;
	void *mux_bulk_rx_buf_pool[MUX_BULK_RX_BUF_COUNT];
	void *mux_bulk_tx_buf;

	struct sk_buff *skb_rx_buf;
	struct sk_buff *skb_tx_buf;

	enum pkt_parse_state rx_parse_state;
	spinlock_t net_lock;

	unsigned short rx_buf_size;
	unsigned short rx_buf_missing;
	struct iphdr rx_ip_hdr;

	unsigned long flags;
};

enum rx_ctrl_state{
	RX_IDLE,
	RX_SENT,
	RX_PENDING
};

#define BM_REQUEST_TYPE (0xa1)
#define B_NOTIFICATION  (0x20)
#define W_VALUE         (0x0)
#define W_LENGTH        (0x2)

#define B_OVERRUN       (0x1<<6)
#define B_PARITY        (0x1<<5)
#define B_FRAMING       (0x1<<4)
#define B_RING_SIGNAL   (0x1<<3)
#define B_BREAK         (0x1<<2)
#define B_TX_CARRIER    (0x1<<1)
#define B_RX_CARRIER    (0x1<<0)

struct hso_serial_state_notification {
	u8 bmRequestType;
	u8 bNotification;
	u16 wValue;
	u16 wIndex;
	u16 wLength;
	u16 UART_state_bitmap;
} __packed;

struct hso_tiocmget {
	struct mutex mutex;
	wait_queue_head_t waitq;
	int    intr_completed;
	struct usb_endpoint_descriptor *endp;
	struct urb *urb;
	struct hso_serial_state_notification *serial_state_notification;
	u16    prev_UART_state_bitmap;
	struct uart_icount icount;
};


struct hso_serial {
	struct hso_device *parent;
	int magic;
	u8 minor;

	struct hso_shared_int *shared_int;

	/* rx/tx urb could be either a bulk urb or a control urb depending
	   on which serial port it is used on. */
	struct urb *rx_urb[MAX_RX_URBS];
	u8 num_rx_urbs;
	u8 *rx_data[MAX_RX_URBS];
	u16 rx_data_length;	/* should contain allocated length */

	struct urb *tx_urb;
	u8 *tx_data;
	u8 *tx_buffer;
	u16 tx_data_length;	/* should contain allocated length */
	u16 tx_data_count;
	u16 tx_buffer_count;
	struct usb_ctrlrequest ctrl_req_tx;
	struct usb_ctrlrequest ctrl_req_rx;

	struct usb_endpoint_descriptor *in_endp;
	struct usb_endpoint_descriptor *out_endp;

	enum rx_ctrl_state rx_state;
	u8 rts_state;
	u8 dtr_state;
	unsigned tx_urb_used:1;

	struct tty_port port;
	/* from usb_serial_port */
	spinlock_t serial_lock;

	int (*write_data) (struct hso_serial *serial);
	struct hso_tiocmget  *tiocmget;
	/* Hacks required to get flow control
	 * working on the serial receive buffers
	 * so as not to drop characters on the floor.
	 */
	int  curr_rx_urb_idx;
	u8   rx_urb_filled[MAX_RX_URBS];
	struct tasklet_struct unthrottle_tasklet;
};

struct hso_device {
	union {
		struct hso_serial *dev_serial;
		struct hso_net *dev_net;
	} port_data;

	u32 port_spec;

	u8 is_active;
	u8 usb_gone;
	struct work_struct async_get_intf;
	struct work_struct async_put_intf;

	struct usb_device *usb;
	struct usb_interface *interface;

	struct device *dev;
	struct kref ref;
	struct mutex mutex;
};

/* Type of interface */
#define HSO_INTF_MASK		0xFF00
#define	HSO_INTF_MUX		0x0100
#define	HSO_INTF_BULK   	0x0200

/* Type of port */
#define HSO_PORT_MASK		0xFF
#define HSO_PORT_NO_PORT	0x0
#define	HSO_PORT_CONTROL	0x1
#define	HSO_PORT_APP		0x2
#define	HSO_PORT_GPS		0x3
#define	HSO_PORT_PCSC		0x4
#define	HSO_PORT_APP2		0x5
#define HSO_PORT_GPS_CONTROL	0x6
#define HSO_PORT_MSD		0x7
#define HSO_PORT_VOICE		0x8
#define HSO_PORT_DIAG2		0x9
#define	HSO_PORT_DIAG		0x10
#define	HSO_PORT_MODEM		0x11
#define	HSO_PORT_NETWORK	0x12

/* Additional device info */
#define HSO_INFO_MASK		0xFF000000
#define HSO_INFO_CRC_BUG	0x01000000

/*****************************************************************************/
/* Prototypes                                                                */
/*****************************************************************************/
/* Serial driver functions */
static int hso_serial_tiocmset(struct tty_struct *tty,
			       unsigned int set, unsigned int clear);
static void ctrl_callback(struct urb *urb);
static int put_rxbuf_data(struct urb *urb, struct hso_serial *serial);
static void hso_kick_transmit(struct hso_serial *serial);
/* Helper functions */
static int hso_mux_submit_intr_urb(struct hso_shared_int *mux_int,
				   struct usb_device *usb, gfp_t gfp);
static void handle_usb_error(int status, const char *function,
			     struct hso_device *hso_dev);
static struct usb_endpoint_descriptor *hso_get_ep(struct usb_interface *intf,
						  int type, int dir);
static int hso_get_mux_ports(struct usb_interface *intf, unsigned char *ports);
static void hso_free_interface(struct usb_interface *intf);
static int hso_start_serial_device(struct hso_device *hso_dev, gfp_t flags);
static int hso_stop_serial_device(struct hso_device *hso_dev);
static int hso_start_net_device(struct hso_device *hso_dev);
static void hso_free_shared_int(struct hso_shared_int *shared_int);
static int hso_stop_net_device(struct hso_device *hso_dev);
static void hso_serial_ref_free(struct kref *ref);
static void hso_std_serial_read_bulk_callback(struct urb *urb);
static int hso_mux_serial_read(struct hso_serial *serial);
static void async_get_intf(struct work_struct *data);
static void async_put_intf(struct work_struct *data);
static int hso_put_activity(struct hso_device *hso_dev);
static int hso_get_activity(struct hso_device *hso_dev);
static void tiocmget_intr_callback(struct urb *urb);
/*****************************************************************************/
/* Helping functions                                                         */
/*****************************************************************************/

/* #define DEBUG */

static inline struct hso_net *dev2net(struct hso_device *hso_dev)
{
	return hso_dev->port_data.dev_net;
}

static inline struct hso_serial *dev2ser(struct hso_device *hso_dev)
{
	return hso_dev->port_data.dev_serial;
}

/* Debugging functions */
#ifdef DEBUG
static void dbg_dump(int line_count, const char *func_name, unsigned char *buf,
		     unsigned int len)
{
	static char name[255];

	sprintf(name, "hso[%d:%s]", line_count, func_name);
	print_hex_dump_bytes(name, DUMP_PREFIX_NONE, buf, len);
}

#define DUMP(buf_, len_)	\
	dbg_dump(__LINE__, __func__, (unsigned char *)buf_, len_)

#define DUMP1(buf_, len_)			\
	do {					\
		if (0x01 & debug)		\
			DUMP(buf_, len_);	\
	} while (0)
#else
#define DUMP(buf_, len_)
#define DUMP1(buf_, len_)
#endif

/* module parameters */
static int debug;
static int tty_major;
static int disable_net;

/* driver info */
static const char driver_name[] = "hso";
static const char tty_filename[] = "ttyHS";
static const char *version = __FILE__ ": " MOD_AUTHOR;
/* the usb driver itself (registered in hso_init) */
static struct usb_driver hso_driver;
/* serial structures */
static struct tty_driver *tty_drv;
static struct hso_device *serial_table[HSO_SERIAL_TTY_MINORS];
static struct hso_device *network_table[HSO_MAX_NET_DEVICES];
static DEFINE_SPINLOCK(serial_table_lock);

static const s32 default_port_spec[] = {
	HSO_INTF_MUX | HSO_PORT_NETWORK,
	HSO_INTF_BULK | HSO_PORT_DIAG,
	HSO_INTF_BULK | HSO_PORT_MODEM,
	0
};

static const s32 icon321_port_spec[] = {
	HSO_INTF_MUX | HSO_PORT_NETWORK,
	HSO_INTF_BULK | HSO_PORT_DIAG2,
	HSO_INTF_BULK | HSO_PORT_MODEM,
	HSO_INTF_BULK | HSO_PORT_DIAG,
	0
};

#define default_port_device(vendor, product)	\
	USB_DEVICE(vendor, product),	\
		.driver_info = (kernel_ulong_t)default_port_spec

#define icon321_port_device(vendor, product)	\
	USB_DEVICE(vendor, product),	\
		.driver_info = (kernel_ulong_t)icon321_port_spec

/* list of devices we support */
static const struct usb_device_id hso_ids[] = {
	{default_port_device(0x0af0, 0x6711)},
	{default_port_device(0x0af0, 0x6731)},
	{default_port_device(0x0af0, 0x6751)},
	{default_port_device(0x0af0, 0x6771)},
	{default_port_device(0x0af0, 0x6791)},
	{default_port_device(0x0af0, 0x6811)},
	{default_port_device(0x0af0, 0x6911)},
	{default_port_device(0x0af0, 0x6951)},
	{default_port_device(0x0af0, 0x6971)},
	{default_port_device(0x0af0, 0x7011)},
	{default_port_device(0x0af0, 0x7031)},
	{default_port_device(0x0af0, 0x7051)},
	{default_port_device(0x0af0, 0x7071)},
	{default_port_device(0x0af0, 0x7111)},
	{default_port_device(0x0af0, 0x7211)},
	{default_port_device(0x0af0, 0x7251)},
	{default_port_device(0x0af0, 0x7271)},
	{default_port_device(0x0af0, 0x7311)},
	{default_port_device(0x0af0, 0xc031)},	/* Icon-Edge */
	{icon321_port_device(0x0af0, 0xd013)},	/* Module HSxPA */
	{icon321_port_device(0x0af0, 0xd031)},	/* Icon-321 */
	{icon321_port_device(0x0af0, 0xd033)},	/* Icon-322 */
	{USB_DEVICE(0x0af0, 0x7301)},		/* GE40x */
	{USB_DEVICE(0x0af0, 0x7361)},		/* GE40x */
	{USB_DEVICE(0x0af0, 0x7381)},		/* GE40x */
	{USB_DEVICE(0x0af0, 0x7401)},		/* GI 0401 */
	{USB_DEVICE(0x0af0, 0x7501)},		/* GTM 382 */
	{USB_DEVICE(0x0af0, 0x7601)},		/* GE40x */
	{USB_DEVICE(0x0af0, 0x7701)},
	{USB_DEVICE(0x0af0, 0x7706)},
	{USB_DEVICE(0x0af0, 0x7801)},
	{USB_DEVICE(0x0af0, 0x7901)},
	{USB_DEVICE(0x0af0, 0x7A01)},
	{USB_DEVICE(0x0af0, 0x7A05)},
	{USB_DEVICE(0x0af0, 0x8200)},
	{USB_DEVICE(0x0af0, 0x8201)},
	{USB_DEVICE(0x0af0, 0x8300)},
	{USB_DEVICE(0x0af0, 0x8302)},
	{USB_DEVICE(0x0af0, 0x8304)},
	{USB_DEVICE(0x0af0, 0x8400)},
	{USB_DEVICE(0x0af0, 0x8600)},
	{USB_DEVICE(0x0af0, 0x8800)},
	{USB_DEVICE(0x0af0, 0x8900)},
	{USB_DEVICE(0x0af0, 0x9000)},
	{USB_DEVICE(0x0af0, 0x9200)},		/* Option GTM671WFS */
	{USB_DEVICE(0x0af0, 0xd035)},
	{USB_DEVICE(0x0af0, 0xd055)},
	{USB_DEVICE(0x0af0, 0xd155)},
	{USB_DEVICE(0x0af0, 0xd255)},
	{USB_DEVICE(0x0af0, 0xd057)},
	{USB_DEVICE(0x0af0, 0xd157)},
	{USB_DEVICE(0x0af0, 0xd257)},
	{USB_DEVICE(0x0af0, 0xd357)},
	{USB_DEVICE(0x0af0, 0xd058)},
	{USB_DEVICE(0x0af0, 0xc100)},
	{}
};
MODULE_DEVICE_TABLE(usb, hso_ids);

/* Sysfs attribute */
static ssize_t hso_sysfs_show_porttype(struct device *dev,
				       struct device_attribute *attr,
				       char *buf)
{
	struct hso_device *hso_dev = dev_get_drvdata(dev);
	char *port_name;

	if (!hso_dev)
		return 0;

	switch (hso_dev->port_spec & HSO_PORT_MASK) {
	case HSO_PORT_CONTROL:
		port_name = "Control";
		break;
	case HSO_PORT_APP:
		port_name = "Application";
		break;
	case HSO_PORT_APP2:
		port_name = "Application2";
		break;
	case HSO_PORT_GPS:
		port_name = "GPS";
		break;
	case HSO_PORT_GPS_CONTROL:
		port_name = "GPS Control";
		break;
	case HSO_PORT_PCSC:
		port_name = "PCSC";
		break;
	case HSO_PORT_DIAG:
		port_name = "Diagnostic";
		break;
	case HSO_PORT_DIAG2:
		port_name = "Diagnostic2";
		break;
	case HSO_PORT_MODEM:
		port_name = "Modem";
		break;
	case HSO_PORT_NETWORK:
		port_name = "Network";
		break;
	default:
		port_name = "Unknown";
		break;
	}

	return sprintf(buf, "%s\n", port_name);
}
static DEVICE_ATTR(hsotype, 0444, hso_sysfs_show_porttype, NULL);

static struct attribute *hso_serial_dev_attrs[] = {
	&dev_attr_hsotype.attr,
	NULL
};

ATTRIBUTE_GROUPS(hso_serial_dev);

static int hso_urb_to_index(struct hso_serial *serial, struct urb *urb)
{
	int idx;

	for (idx = 0; idx < serial->num_rx_urbs; idx++)
		if (serial->rx_urb[idx] == urb)
			return idx;
	dev_err(serial->parent->dev, "hso_urb_to_index failed\n");
	return -1;
}

/* converts mux value to a port spec value */
static u32 hso_mux_to_port(int mux)
{
	u32 result;

	switch (mux) {
	case 0x1:
		result = HSO_PORT_CONTROL;
		break;
	case 0x2:
		result = HSO_PORT_APP;
		break;
	case 0x4:
		result = HSO_PORT_PCSC;
		break;
	case 0x8:
		result = HSO_PORT_GPS;
		break;
	case 0x10:
		result = HSO_PORT_APP2;
		break;
	default:
		result = HSO_PORT_NO_PORT;
	}
	return result;
}

/* converts port spec value to a mux value */
static u32 hso_port_to_mux(int port)
{
	u32 result;

	switch (port & HSO_PORT_MASK) {
	case HSO_PORT_CONTROL:
		result = 0x0;
		break;
	case HSO_PORT_APP:
		result = 0x1;
		break;
	case HSO_PORT_PCSC:
		result = 0x2;
		break;
	case HSO_PORT_GPS:
		result = 0x3;
		break;
	case HSO_PORT_APP2:
		result = 0x4;
		break;
	default:
		result = 0x0;
	}
	return result;
}

static struct hso_serial *get_serial_by_shared_int_and_type(
					struct hso_shared_int *shared_int,
					int mux)
{
	int i, port;

	port = hso_mux_to_port(mux);

	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] &&
		    (dev2ser(serial_table[i])->shared_int == shared_int) &&
		    ((serial_table[i]->port_spec & HSO_PORT_MASK) == port)) {
			return dev2ser(serial_table[i]);
		}
	}

	return NULL;
}

static struct hso_serial *get_serial_by_index(unsigned index)
{
	struct hso_serial *serial = NULL;
	unsigned long flags;

	spin_lock_irqsave(&serial_table_lock, flags);
	if (serial_table[index])
		serial = dev2ser(serial_table[index]);
	spin_unlock_irqrestore(&serial_table_lock, flags);

	return serial;
}

static int obtain_minor(struct hso_serial *serial)
{
	int index;
	unsigned long flags;

	spin_lock_irqsave(&serial_table_lock, flags);
	for (index = 0; index < HSO_SERIAL_TTY_MINORS; index++) {
		if (serial_table[index] == NULL) {
			serial_table[index] = serial->parent;
			serial->minor = index;
			spin_unlock_irqrestore(&serial_table_lock, flags);
			return 0;
		}
	}
	spin_unlock_irqrestore(&serial_table_lock, flags);

	pr_err("%s: no free serial devices in table\n", __func__);
	return -1;
}

static void release_minor(struct hso_serial *serial)
{
	unsigned long flags;

	spin_lock_irqsave(&serial_table_lock, flags);
	serial_table[serial->minor] = NULL;
	spin_unlock_irqrestore(&serial_table_lock, flags);
}

static void handle_usb_error(int status, const char *function,
			     struct hso_device *hso_dev)
{
	char *explanation;

	switch (status) {
	case -ENODEV:
		explanation = "no device";
		break;
	case -ENOENT:
		explanation = "endpoint not enabled";
		break;
	case -EPIPE:
		explanation = "endpoint stalled";
		break;
	case -ENOSPC:
		explanation = "not enough bandwidth";
		break;
	case -ESHUTDOWN:
		explanation = "device disabled";
		break;
	case -EHOSTUNREACH:
		explanation = "device suspended";
		break;
	case -EINVAL:
	case -EAGAIN:
	case -EFBIG:
	case -EMSGSIZE:
		explanation = "internal error";
		break;
	case -EILSEQ:
	case -EPROTO:
	case -ETIME:
	case -ETIMEDOUT:
		explanation = "protocol error";
		if (hso_dev)
			usb_queue_reset_device(hso_dev->interface);
		break;
	default:
		explanation = "unknown status";
		break;
	}

	/* log a meaningful explanation of an USB status */
	hso_dbg(0x1, "%s: received USB status - %s (%d)\n",
		function, explanation, status);
}

/* Network interface functions */

/* called when net interface is brought up by ifconfig */
static int hso_net_open(struct net_device *net)
{
	struct hso_net *odev = netdev_priv(net);
	unsigned long flags = 0;

	if (!odev) {
		dev_err(&net->dev, "No net device !\n");
		return -ENODEV;
	}

	odev->skb_tx_buf = NULL;

	/* setup environment */
	spin_lock_irqsave(&odev->net_lock, flags);
	odev->rx_parse_state = WAIT_IP;
	odev->rx_buf_size = 0;
	odev->rx_buf_missing = sizeof(struct iphdr);
	spin_unlock_irqrestore(&odev->net_lock, flags);

	/* We are up and running. */
	set_bit(HSO_NET_RUNNING, &odev->flags);
	hso_start_net_device(odev->parent);

	/* Tell the kernel we are ready to start receiving from it */
	netif_start_queue(net);

	return 0;
}

/* called when interface is brought down by ifconfig */
static int hso_net_close(struct net_device *net)
{
	struct hso_net *odev = netdev_priv(net);

	/* we don't need the queue anymore */
	netif_stop_queue(net);
	/* no longer running */
	clear_bit(HSO_NET_RUNNING, &odev->flags);

	hso_stop_net_device(odev->parent);

	/* done */
	return 0;
}

/* USB tells is xmit done, we should start the netqueue again */
static void write_bulk_callback(struct urb *urb)
{
	struct hso_net *odev = urb->context;
	int status = urb->status;

	/* Sanity check */
	if (!odev || !test_bit(HSO_NET_RUNNING, &odev->flags)) {
		dev_err(&urb->dev->dev, "%s: device not running\n", __func__);
		return;
	}

	/* Do we still have a valid kernel network device? */
	if (!netif_device_present(odev->net)) {
		dev_err(&urb->dev->dev, "%s: net device not present\n",
			__func__);
		return;
	}

	/* log status, but don't act on it, we don't need to resubmit anything
	 * anyhow */
	if (status)
		handle_usb_error(status, __func__, odev->parent);

	hso_put_activity(odev->parent);

	/* Tell the network interface we are ready for another frame */
	netif_wake_queue(odev->net);
}

/* called by kernel when we need to transmit a packet */
static netdev_tx_t hso_net_start_xmit(struct sk_buff *skb,
					    struct net_device *net)
{
	struct hso_net *odev = netdev_priv(net);
	int result;

	/* Tell the kernel, "No more frames 'til we are done with this one." */
	netif_stop_queue(net);
	if (hso_get_activity(odev->parent) == -EAGAIN) {
		odev->skb_tx_buf = skb;
		return NETDEV_TX_OK;
	}

	/* log if asked */
	DUMP1(skb->data, skb->len);
	/* Copy it from kernel memory to OUR memory */
	memcpy(odev->mux_bulk_tx_buf, skb->data, skb->len);
	hso_dbg(0x1, "len: %d/%d\n", skb->len, MUX_BULK_TX_BUF_SIZE);

	/* Fill in the URB for shipping it out. */
	usb_fill_bulk_urb(odev->mux_bulk_tx_urb,
			  odev->parent->usb,
			  usb_sndbulkpipe(odev->parent->usb,
					  odev->out_endp->
					  bEndpointAddress & 0x7F),
			  odev->mux_bulk_tx_buf, skb->len, write_bulk_callback,
			  odev);

	/* Deal with the Zero Length packet problem, I hope */
	odev->mux_bulk_tx_urb->transfer_flags |= URB_ZERO_PACKET;

	/* Send the URB on its merry way. */
	result = usb_submit_urb(odev->mux_bulk_tx_urb, GFP_ATOMIC);
	if (result) {
		dev_warn(&odev->parent->interface->dev,
			"failed mux_bulk_tx_urb %d\n", result);
		net->stats.tx_errors++;
		netif_start_queue(net);
	} else {
		net->stats.tx_packets++;
		net->stats.tx_bytes += skb->len;
	}
	dev_kfree_skb(skb);
	/* we're done */
	return NETDEV_TX_OK;
}

static const struct ethtool_ops ops = {
	.get_link = ethtool_op_get_link
};

/* called when a packet did not ack after watchdogtimeout */
static void hso_net_tx_timeout(struct net_device *net, unsigned int txqueue)
{
	struct hso_net *odev = netdev_priv(net);

	if (!odev)
		return;

	/* Tell syslog we are hosed. */
	dev_warn(&net->dev, "Tx timed out.\n");

	/* Tear the waiting frame off the list */
	if (odev->mux_bulk_tx_urb)
		usb_unlink_urb(odev->mux_bulk_tx_urb);

	/* Update statistics */
	net->stats.tx_errors++;
}

/* make a real packet from the received USB buffer */
static void packetizeRx(struct hso_net *odev, unsigned char *ip_pkt,
			unsigned int count, unsigned char is_eop)
{
	unsigned short temp_bytes;
	unsigned short buffer_offset = 0;
	unsigned short frame_len;

	/* log if needed */
	hso_dbg(0x1, "Rx %d bytes\n", count);
	DUMP(ip_pkt, min(128, (int)count));

	while (count) {
		switch (odev->rx_parse_state) {
		case WAIT_IP:
			/* waiting for IP header. */
			/* wanted bytes - size of ip header */
			temp_bytes =
			    (count <
			     odev->rx_buf_missing) ? count : odev->
			    rx_buf_missing;

			memcpy(((unsigned char *)(&odev->rx_ip_hdr)) +
			       odev->rx_buf_size, ip_pkt + buffer_offset,
			       temp_bytes);

			odev->rx_buf_size += temp_bytes;
			buffer_offset += temp_bytes;
			odev->rx_buf_missing -= temp_bytes;
			count -= temp_bytes;

			if (!odev->rx_buf_missing) {
				/* header is complete allocate an sk_buffer and
				 * continue to WAIT_DATA */
				frame_len = ntohs(odev->rx_ip_hdr.tot_len);

				if ((frame_len > DEFAULT_MRU) ||
				    (frame_len < sizeof(struct iphdr))) {
					dev_err(&odev->net->dev,
						"Invalid frame (%d) length\n",
						frame_len);
					odev->rx_parse_state = WAIT_SYNC;
					continue;
				}
				/* Allocate an sk_buff */
				odev->skb_rx_buf = netdev_alloc_skb(odev->net,
								    frame_len);
				if (!odev->skb_rx_buf) {
					/* We got no receive buffer. */
					hso_dbg(0x1, "could not allocate memory\n");
					odev->rx_parse_state = WAIT_SYNC;
					continue;
				}

				/* Copy what we got so far. make room for iphdr
				 * after tail. */
				skb_put_data(odev->skb_rx_buf,
					     (char *)&(odev->rx_ip_hdr),
					     sizeof(struct iphdr));

				/* ETH_HLEN */
				odev->rx_buf_size = sizeof(struct iphdr);

				/* Filip actually use .tot_len */
				odev->rx_buf_missing =
				    frame_len - sizeof(struct iphdr);
				odev->rx_parse_state = WAIT_DATA;
			}
			break;

		case WAIT_DATA:
			temp_bytes = (count < odev->rx_buf_missing)
					? count : odev->rx_buf_missing;

			/* Copy the rest of the bytes that are left in the
			 * buffer into the waiting sk_buf. */
			/* Make room for temp_bytes after tail. */
			skb_put_data(odev->skb_rx_buf,
				     ip_pkt + buffer_offset,
				     temp_bytes);

			odev->rx_buf_missing -= temp_bytes;
			count -= temp_bytes;
			buffer_offset += temp_bytes;
			odev->rx_buf_size += temp_bytes;
			if (!odev->rx_buf_missing) {
				/* Packet is complete. Inject into stack. */
				/* We have IP packet here */
				odev->skb_rx_buf->protocol = cpu_to_be16(ETH_P_IP);
				skb_reset_mac_header(odev->skb_rx_buf);

				/* Ship it off to the kernel */
				netif_rx(odev->skb_rx_buf);
				/* No longer our buffer. */
				odev->skb_rx_buf = NULL;

				/* update out statistics */
				odev->net->stats.rx_packets++;

				odev->net->stats.rx_bytes += odev->rx_buf_size;

				odev->rx_buf_size = 0;
				odev->rx_buf_missing = sizeof(struct iphdr);
				odev->rx_parse_state = WAIT_IP;
			}
			break;

		case WAIT_SYNC:
			hso_dbg(0x1, " W_S\n");
			count = 0;
			break;
		default:
			hso_dbg(0x1, "\n");
			count--;
			break;
		}
	}

	/* Recovery mechanism for WAIT_SYNC state. */
	if (is_eop) {
		if (odev->rx_parse_state == WAIT_SYNC) {
			odev->rx_parse_state = WAIT_IP;
			odev->rx_buf_size = 0;
			odev->rx_buf_missing = sizeof(struct iphdr);
		}
	}
}

static void fix_crc_bug(struct urb *urb, __le16 max_packet_size)
{
	static const u8 crc_check[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
	u32 rest = urb->actual_length % le16_to_cpu(max_packet_size);

	if (((rest == 5) || (rest == 6)) &&
	    !memcmp(((u8 *)urb->transfer_buffer) + urb->actual_length - 4,
		    crc_check, 4)) {
		urb->actual_length -= 4;
	}
}

/* Moving data from usb to kernel (in interrupt state) */
static void read_bulk_callback(struct urb *urb)
{
	struct hso_net *odev = urb->context;
	struct net_device *net;
	int result;
	unsigned long flags;
	int status = urb->status;

	/* is al ok?  (Filip: Who's Al ?) */
	if (status) {
		handle_usb_error(status, __func__, odev->parent);
		return;
	}

	/* Sanity check */
	if (!odev || !test_bit(HSO_NET_RUNNING, &odev->flags)) {
		hso_dbg(0x1, "BULK IN callback but driver is not active!\n");
		return;
	}
	usb_mark_last_busy(urb->dev);

	net = odev->net;

	if (!netif_device_present(net)) {
		/* Somebody killed our network interface... */
		return;
	}

	if (odev->parent->port_spec & HSO_INFO_CRC_BUG)
		fix_crc_bug(urb, odev->in_endp->wMaxPacketSize);

	/* do we even have a packet? */
	if (urb->actual_length) {
		/* Handle the IP stream, add header and push it onto network
		 * stack if the packet is complete. */
		spin_lock_irqsave(&odev->net_lock, flags);
		packetizeRx(odev, urb->transfer_buffer, urb->actual_length,
			    (urb->transfer_buffer_length >
			     urb->actual_length) ? 1 : 0);
		spin_unlock_irqrestore(&odev->net_lock, flags);
	}

	/* We are done with this URB, resubmit it. Prep the USB to wait for
	 * another frame. Reuse same as received. */
	usb_fill_bulk_urb(urb,
			  odev->parent->usb,
			  usb_rcvbulkpipe(odev->parent->usb,
					  odev->in_endp->
					  bEndpointAddress & 0x7F),
			  urb->transfer_buffer, MUX_BULK_RX_BUF_SIZE,
			  read_bulk_callback, odev);

	/* Give this to the USB subsystem so it can tell us when more data
	 * arrives. */
	result = usb_submit_urb(urb, GFP_ATOMIC);
	if (result)
		dev_warn(&odev->parent->interface->dev,
			 "%s failed submit mux_bulk_rx_urb %d\n", __func__,
			 result);
}

/* Serial driver functions */

static void hso_init_termios(struct ktermios *termios)
{
	/*
	 * The default requirements for this device are:
	 */
	termios->c_iflag &=
		~(IGNBRK	/* disable ignore break */
		| BRKINT	/* disable break causes interrupt */
		| PARMRK	/* disable mark parity errors */
		| ISTRIP	/* disable clear high bit of input characters */
		| INLCR		/* disable translate NL to CR */
		| IGNCR		/* disable ignore CR */
		| ICRNL		/* disable translate CR to NL */
		| IXON);	/* disable enable XON/XOFF flow control */

	/* disable postprocess output characters */
	termios->c_oflag &= ~OPOST;

	termios->c_lflag &=
		~(ECHO		/* disable echo input characters */
		| ECHONL	/* disable echo new line */
		| ICANON	/* disable erase, kill, werase, and rprnt
				   special characters */
		| ISIG		/* disable interrupt, quit, and suspend special
				   characters */
		| IEXTEN);	/* disable non-POSIX special characters */

	termios->c_cflag &=
		~(CSIZE		/* no size */
		| PARENB	/* disable parity bit */
		| CBAUD		/* clear current baud rate */
		| CBAUDEX);	/* clear current buad rate */

	termios->c_cflag |= CS8;	/* character size 8 bits */

	/* baud rate 115200 */
	tty_termios_encode_baud_rate(termios, 115200, 115200);
}

static void _hso_serial_set_termios(struct tty_struct *tty,
				    struct ktermios *old)
{
	struct hso_serial *serial = tty->driver_data;

	if (!serial) {
		pr_err("%s: no tty structures", __func__);
		return;
	}

	hso_dbg(0x8, "port %d\n", serial->minor);

	/*
	 *	Fix up unsupported bits
	 */
	tty->termios.c_iflag &= ~IXON; /* disable enable XON/XOFF flow control */

	tty->termios.c_cflag &=
		~(CSIZE		/* no size */
		| PARENB	/* disable parity bit */
		| CBAUD		/* clear current baud rate */
		| CBAUDEX);	/* clear current buad rate */

	tty->termios.c_cflag |= CS8;	/* character size 8 bits */

	/* baud rate 115200 */
	tty_encode_baud_rate(tty, 115200, 115200);
}

static void hso_resubmit_rx_bulk_urb(struct hso_serial *serial, struct urb *urb)
{
	int result;
	/* We are done with this URB, resubmit it. Prep the USB to wait for
	 * another frame */
	usb_fill_bulk_urb(urb, serial->parent->usb,
			  usb_rcvbulkpipe(serial->parent->usb,
					  serial->in_endp->
					  bEndpointAddress & 0x7F),
			  urb->transfer_buffer, serial->rx_data_length,
			  hso_std_serial_read_bulk_callback, serial);
	/* Give this to the USB subsystem so it can tell us when more data
	 * arrives. */
	result = usb_submit_urb(urb, GFP_ATOMIC);
	if (result) {
		dev_err(&urb->dev->dev, "%s failed submit serial rx_urb %d\n",
			__func__, result);
	}
}




static void put_rxbuf_data_and_resubmit_bulk_urb(struct hso_serial *serial)
{
	int count;
	struct urb *curr_urb;

	while (serial->rx_urb_filled[serial->curr_rx_urb_idx]) {
		curr_urb = serial->rx_urb[serial->curr_rx_urb_idx];
		count = put_rxbuf_data(curr_urb, serial);
		if (count == -1)
			return;
		if (count == 0) {
			serial->curr_rx_urb_idx++;
			if (serial->curr_rx_urb_idx >= serial->num_rx_urbs)
				serial->curr_rx_urb_idx = 0;
			hso_resubmit_rx_bulk_urb(serial, curr_urb);
		}
	}
}

static void put_rxbuf_data_and_resubmit_ctrl_urb(struct hso_serial *serial)
{
	int count = 0;
	struct urb *urb;

	urb = serial->rx_urb[0];
	if (serial->port.count > 0) {
		count = put_rxbuf_data(urb, serial);
		if (count == -1)
			return;
	}
	/* Re issue a read as long as we receive data. */

	if (count == 0 && ((urb->actual_length != 0) ||
			   (serial->rx_state == RX_PENDING))) {
		serial->rx_state = RX_SENT;
		hso_mux_serial_read(serial);
	} else
		serial->rx_state = RX_IDLE;
}


/* read callback for Diag and CS port */
static void hso_std_serial_read_bulk_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	int status = urb->status;
	unsigned long flags;

	hso_dbg(0x8, "--- Got serial_read_bulk callback %02x ---\n", status);

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "serial == NULL\n");
		return;
	}
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}

	hso_dbg(0x1, "Actual length = %d\n", urb->actual_length);
	DUMP1(urb->transfer_buffer, urb->actual_length);

	/* Anyone listening? */
	if (serial->port.count == 0)
		return;

	if (serial->parent->port_spec & HSO_INFO_CRC_BUG)
		fix_crc_bug(urb, serial->in_endp->wMaxPacketSize);
	/* Valid data, handle RX data */
	spin_lock_irqsave(&serial->serial_lock, flags);
	serial->rx_urb_filled[hso_urb_to_index(serial, urb)] = 1;
	put_rxbuf_data_and_resubmit_bulk_urb(serial);
	spin_unlock_irqrestore(&serial->serial_lock, flags);
}

/*
 * This needs to be a tasklet otherwise we will
 * end up recursively calling this function.
 */
static void hso_unthrottle_tasklet(struct tasklet_struct *t)
{
	struct hso_serial *serial = from_tasklet(serial, t,
						 unthrottle_tasklet);
	unsigned long flags;

	spin_lock_irqsave(&serial->serial_lock, flags);
	if ((serial->parent->port_spec & HSO_INTF_MUX))
		put_rxbuf_data_and_resubmit_ctrl_urb(serial);
	else
		put_rxbuf_data_and_resubmit_bulk_urb(serial);
	spin_unlock_irqrestore(&serial->serial_lock, flags);
}

static	void hso_unthrottle(struct tty_struct *tty)
{
	struct hso_serial *serial = tty->driver_data;

	tasklet_hi_schedule(&serial->unthrottle_tasklet);
}

/* open the requested serial port */
static int hso_serial_open(struct tty_struct *tty, struct file *filp)
{
	struct hso_serial *serial = get_serial_by_index(tty->index);
	int result;

	/* sanity check */
	if (serial == NULL || serial->magic != HSO_SERIAL_MAGIC) {
		WARN_ON(1);
		tty->driver_data = NULL;
		hso_dbg(0x1, "Failed to open port\n");
		return -ENODEV;
	}

	mutex_lock(&serial->parent->mutex);
	result = usb_autopm_get_interface(serial->parent->interface);
	if (result < 0)
		goto err_out;

	hso_dbg(0x1, "Opening %d\n", serial->minor);

	/* setup */
	tty->driver_data = serial;
	tty_port_tty_set(&serial->port, tty);

	/* check for port already opened, if not set the termios */
	serial->port.count++;
	if (serial->port.count == 1) {
		serial->rx_state = RX_IDLE;
		/* Force default termio settings */
		_hso_serial_set_termios(tty, NULL);
		tasklet_setup(&serial->unthrottle_tasklet,
			      hso_unthrottle_tasklet);
		result = hso_start_serial_device(serial->parent, GFP_KERNEL);
		if (result) {
			hso_stop_serial_device(serial->parent);
			serial->port.count--;
		} else {
			kref_get(&serial->parent->ref);
		}
	} else {
		hso_dbg(0x1, "Port was already open\n");
	}

	usb_autopm_put_interface(serial->parent->interface);

	/* done */
	if (result)
		hso_serial_tiocmset(tty, TIOCM_RTS | TIOCM_DTR, 0);
err_out:
	mutex_unlock(&serial->parent->mutex);
	return result;
}

/* close the requested serial port */
static void hso_serial_close(struct tty_struct *tty, struct file *filp)
{
	struct hso_serial *serial = tty->driver_data;
	u8 usb_gone;

	hso_dbg(0x1, "Closing serial port\n");

	/* Open failed, no close cleanup required */
	if (serial == NULL)
		return;

	mutex_lock(&serial->parent->mutex);
	usb_gone = serial->parent->usb_gone;

	if (!usb_gone)
		usb_autopm_get_interface(serial->parent->interface);

	/* reset the rts and dtr */
	/* do the actual close */
	serial->port.count--;

	if (serial->port.count <= 0) {
		serial->port.count = 0;
		tty_port_tty_set(&serial->port, NULL);
		if (!usb_gone)
			hso_stop_serial_device(serial->parent);
		tasklet_kill(&serial->unthrottle_tasklet);
	}

	if (!usb_gone)
		usb_autopm_put_interface(serial->parent->interface);

	mutex_unlock(&serial->parent->mutex);
}

/* close the requested serial port */
static int hso_serial_write(struct tty_struct *tty, const unsigned char *buf,
			    int count)
{
	struct hso_serial *serial = tty->driver_data;
	int space, tx_bytes;
	unsigned long flags;

	/* sanity check */
	if (serial == NULL) {
		pr_err("%s: serial is NULL\n", __func__);
		return -ENODEV;
	}

	spin_lock_irqsave(&serial->serial_lock, flags);

	space = serial->tx_data_length - serial->tx_buffer_count;
	tx_bytes = (count < space) ? count : space;

	if (!tx_bytes)
		goto out;

	memcpy(serial->tx_buffer + serial->tx_buffer_count, buf, tx_bytes);
	serial->tx_buffer_count += tx_bytes;

out:
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	hso_kick_transmit(serial);
	/* done */
	return tx_bytes;
}

/* how much room is there for writing */
static int hso_serial_write_room(struct tty_struct *tty)
{
	struct hso_serial *serial = tty->driver_data;
	int room;
	unsigned long flags;

	spin_lock_irqsave(&serial->serial_lock, flags);
	room = serial->tx_data_length - serial->tx_buffer_count;
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	/* return free room */
	return room;
}

static void hso_serial_cleanup(struct tty_struct *tty)
{
	struct hso_serial *serial = tty->driver_data;

	if (!serial)
		return;

	kref_put(&serial->parent->ref, hso_serial_ref_free);
}

/* setup the term */
static void hso_serial_set_termios(struct tty_struct *tty, struct ktermios *old)
{
	struct hso_serial *serial = tty->driver_data;
	unsigned long flags;

	if (old)
		hso_dbg(0x16, "Termios called with: cflags new[%u] - old[%u]\n",
			(unsigned int)tty->termios.c_cflag,
			(unsigned int)old->c_cflag);

	/* the actual setup */
	spin_lock_irqsave(&serial->serial_lock, flags);
	if (serial->port.count)
		_hso_serial_set_termios(tty, old);
	else
		tty->termios = *old;
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	/* done */
}

/* how many characters in the buffer */
static int hso_serial_chars_in_buffer(struct tty_struct *tty)
{
	struct hso_serial *serial = tty->driver_data;
	int chars;
	unsigned long flags;

	/* sanity check */
	if (serial == NULL)
		return 0;

	spin_lock_irqsave(&serial->serial_lock, flags);
	chars = serial->tx_buffer_count;
	spin_unlock_irqrestore(&serial->serial_lock, flags);

	return chars;
}
static int tiocmget_submit_urb(struct hso_serial *serial,
			       struct hso_tiocmget *tiocmget,
			       struct usb_device *usb)
{
	int result;

	if (serial->parent->usb_gone)
		return -ENODEV;
	usb_fill_int_urb(tiocmget->urb, usb,
			 usb_rcvintpipe(usb,
					tiocmget->endp->
					bEndpointAddress & 0x7F),
			 tiocmget->serial_state_notification,
			 sizeof(struct hso_serial_state_notification),
			 tiocmget_intr_callback, serial,
			 tiocmget->endp->bInterval);
	result = usb_submit_urb(tiocmget->urb, GFP_ATOMIC);
	if (result) {
		dev_warn(&usb->dev, "%s usb_submit_urb failed %d\n", __func__,
			 result);
	}
	return result;

}

static void tiocmget_intr_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	struct hso_tiocmget *tiocmget;
	int status = urb->status;
	u16 UART_state_bitmap, prev_UART_state_bitmap;
	struct uart_icount *icount;
	struct hso_serial_state_notification *serial_state_notification;
	struct usb_device *usb;
	struct usb_interface *interface;
	int if_num;

	/* Sanity checks */
	if (!serial)
		return;
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}

	/* tiocmget is only supported on HSO_PORT_MODEM */
	tiocmget = serial->tiocmget;
	if (!tiocmget)
		return;
	BUG_ON((serial->parent->port_spec & HSO_PORT_MASK) != HSO_PORT_MODEM);

	usb = serial->parent->usb;
	interface = serial->parent->interface;

	if_num = interface->cur_altsetting->desc.bInterfaceNumber;

	/* wIndex should be the USB interface number of the port to which the
	 * notification applies, which should always be the Modem port.
	 */
	serial_state_notification = tiocmget->serial_state_notification;
	if (serial_state_notification->bmRequestType != BM_REQUEST_TYPE ||
	    serial_state_notification->bNotification != B_NOTIFICATION ||
	    le16_to_cpu(serial_state_notification->wValue) != W_VALUE ||
	    le16_to_cpu(serial_state_notification->wIndex) != if_num ||
	    le16_to_cpu(serial_state_notification->wLength) != W_LENGTH) {
		dev_warn(&usb->dev,
			 "hso received invalid serial state notification\n");
		DUMP(serial_state_notification,
		     sizeof(struct hso_serial_state_notification));
	} else {
		unsigned long flags;

		UART_state_bitmap = le16_to_cpu(serial_state_notification->
						UART_state_bitmap);
		prev_UART_state_bitmap = tiocmget->prev_UART_state_bitmap;
		icount = &tiocmget->icount;
		spin_lock_irqsave(&serial->serial_lock, flags);
		if ((UART_state_bitmap & B_OVERRUN) !=
		   (prev_UART_state_bitmap & B_OVERRUN))
			icount->parity++;
		if ((UART_state_bitmap & B_PARITY) !=
		   (prev_UART_state_bitmap & B_PARITY))
			icount->parity++;
		if ((UART_state_bitmap & B_FRAMING) !=
		   (prev_UART_state_bitmap & B_FRAMING))
			icount->frame++;
		if ((UART_state_bitmap & B_RING_SIGNAL) &&
		   !(prev_UART_state_bitmap & B_RING_SIGNAL))
			icount->rng++;
		if ((UART_state_bitmap & B_BREAK) !=
		   (prev_UART_state_bitmap & B_BREAK))
			icount->brk++;
		if ((UART_state_bitmap & B_TX_CARRIER) !=
		   (prev_UART_state_bitmap & B_TX_CARRIER))
			icount->dsr++;
		if ((UART_state_bitmap & B_RX_CARRIER) !=
		   (prev_UART_state_bitmap & B_RX_CARRIER))
			icount->dcd++;
		tiocmget->prev_UART_state_bitmap = UART_state_bitmap;
		spin_unlock_irqrestore(&serial->serial_lock, flags);
		tiocmget->intr_completed = 1;
		wake_up_interruptible(&tiocmget->waitq);
	}
	memset(serial_state_notification, 0,
	       sizeof(struct hso_serial_state_notification));
	tiocmget_submit_urb(serial,
			    tiocmget,
			    serial->parent->usb);
}

/*
 * next few functions largely stolen from drivers/serial/serial_core.c
 */
/* Wait for any of the 4 modem inputs (DCD,RI,DSR,CTS) to change
 * - mask passed in arg for lines of interest
 *   (use |'ed TIOCM_RNG/DSR/CD/CTS for masking)
 * Caller should use TIOCGICOUNT to see which one it was
 */
static int
hso_wait_modem_status(struct hso_serial *serial, unsigned long arg)
{
	DECLARE_WAITQUEUE(wait, current);
	struct uart_icount cprev, cnow;
	struct hso_tiocmget  *tiocmget;
	int ret;

	tiocmget = serial->tiocmget;
	if (!tiocmget)
		return -ENOENT;
	/*
	 * note the counters on entry
	 */
	spin_lock_irq(&serial->serial_lock);
	memcpy(&cprev, &tiocmget->icount, sizeof(struct uart_icount));
	spin_unlock_irq(&serial->serial_lock);
	add_wait_queue(&tiocmget->waitq, &wait);
	for (;;) {
		spin_lock_irq(&serial->serial_lock);
		memcpy(&cnow, &tiocmget->icount, sizeof(struct uart_icount));
		spin_unlock_irq(&serial->serial_lock);
		set_current_state(TASK_INTERRUPTIBLE);
		if (((arg & TIOCM_RNG) && (cnow.rng != cprev.rng)) ||
		    ((arg & TIOCM_DSR) && (cnow.dsr != cprev.dsr)) ||
		    ((arg & TIOCM_CD)  && (cnow.dcd != cprev.dcd))) {
			ret = 0;
			break;
		}
		schedule();
		/* see if a signal did it */
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			break;
		}
		cprev = cnow;
	}
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&tiocmget->waitq, &wait);

	return ret;
}

/*
 * Get counter of input serial line interrupts (DCD,RI,DSR,CTS)
 * Return: write counters to the user passed counter struct
 * NB: both 1->0 and 0->1 transitions are counted except for
 *     RI where only 0->1 is counted.
 */
static int hso_get_count(struct tty_struct *tty,
		  struct serial_icounter_struct *icount)
{
	struct uart_icount cnow;
	struct hso_serial *serial = tty->driver_data;
	struct hso_tiocmget  *tiocmget = serial->tiocmget;

	memset(icount, 0, sizeof(struct serial_icounter_struct));

	if (!tiocmget)
		 return -ENOENT;
	spin_lock_irq(&serial->serial_lock);
	memcpy(&cnow, &tiocmget->icount, sizeof(struct uart_icount));
	spin_unlock_irq(&serial->serial_lock);

	icount->cts         = cnow.cts;
	icount->dsr         = cnow.dsr;
	icount->rng         = cnow.rng;
	icount->dcd         = cnow.dcd;
	icount->rx          = cnow.rx;
	icount->tx          = cnow.tx;
	icount->frame       = cnow.frame;
	icount->overrun     = cnow.overrun;
	icount->parity      = cnow.parity;
	icount->brk         = cnow.brk;
	icount->buf_overrun = cnow.buf_overrun;

	return 0;
}


static int hso_serial_tiocmget(struct tty_struct *tty)
{
	int retval;
	struct hso_serial *serial = tty->driver_data;
	struct hso_tiocmget  *tiocmget;
	u16 UART_state_bitmap;

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "no tty structures\n");
		return -EINVAL;
	}
	spin_lock_irq(&serial->serial_lock);
	retval = ((serial->rts_state) ? TIOCM_RTS : 0) |
	    ((serial->dtr_state) ? TIOCM_DTR : 0);
	tiocmget = serial->tiocmget;
	if (tiocmget) {

		UART_state_bitmap = le16_to_cpu(
			tiocmget->prev_UART_state_bitmap);
		if (UART_state_bitmap & B_RING_SIGNAL)
			retval |=  TIOCM_RNG;
		if (UART_state_bitmap & B_RX_CARRIER)
			retval |=  TIOCM_CD;
		if (UART_state_bitmap & B_TX_CARRIER)
			retval |=  TIOCM_DSR;
	}
	spin_unlock_irq(&serial->serial_lock);
	return retval;
}

static int hso_serial_tiocmset(struct tty_struct *tty,
			       unsigned int set, unsigned int clear)
{
	int val = 0;
	unsigned long flags;
	int if_num;
	struct hso_serial *serial = tty->driver_data;
	struct usb_interface *interface;

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "no tty structures\n");
		return -EINVAL;
	}

	if ((serial->parent->port_spec & HSO_PORT_MASK) != HSO_PORT_MODEM)
		return -EINVAL;

	interface = serial->parent->interface;
	if_num = interface->cur_altsetting->desc.bInterfaceNumber;

	spin_lock_irqsave(&serial->serial_lock, flags);
	if (set & TIOCM_RTS)
		serial->rts_state = 1;
	if (set & TIOCM_DTR)
		serial->dtr_state = 1;

	if (clear & TIOCM_RTS)
		serial->rts_state = 0;
	if (clear & TIOCM_DTR)
		serial->dtr_state = 0;

	if (serial->dtr_state)
		val |= 0x01;
	if (serial->rts_state)
		val |= 0x02;

	spin_unlock_irqrestore(&serial->serial_lock, flags);

	return usb_control_msg(serial->parent->usb,
			       usb_sndctrlpipe(serial->parent->usb, 0), 0x22,
			       0x21, val, if_num, NULL, 0,
			       USB_CTRL_SET_TIMEOUT);
}

static int hso_serial_ioctl(struct tty_struct *tty,
			    unsigned int cmd, unsigned long arg)
{
	struct hso_serial *serial = tty->driver_data;
	int ret = 0;
	hso_dbg(0x8, "IOCTL cmd: %d, arg: %ld\n", cmd, arg);

	if (!serial)
		return -ENODEV;
	switch (cmd) {
	case TIOCMIWAIT:
		ret = hso_wait_modem_status(serial, arg);
		break;
	default:
		ret = -ENOIOCTLCMD;
		break;
	}
	return ret;
}


/* starts a transmit */
static void hso_kick_transmit(struct hso_serial *serial)
{
	unsigned long flags;
	int res;

	spin_lock_irqsave(&serial->serial_lock, flags);
	if (!serial->tx_buffer_count)
		goto out;

	if (serial->tx_urb_used)
		goto out;

	/* Wakeup USB interface if necessary */
	if (hso_get_activity(serial->parent) == -EAGAIN)
		goto out;

	/* Switch pointers around to avoid memcpy */
	swap(serial->tx_buffer, serial->tx_data);
	serial->tx_data_count = serial->tx_buffer_count;
	serial->tx_buffer_count = 0;

	/* If serial->tx_data is set, it means we switched buffers */
	if (serial->tx_data && serial->write_data) {
		res = serial->write_data(serial);
		if (res >= 0)
			serial->tx_urb_used = 1;
	}
out:
	spin_unlock_irqrestore(&serial->serial_lock, flags);
}

/* make a request (for reading and writing data to muxed serial port) */
static int mux_device_request(struct hso_serial *serial, u8 type, u16 port,
			      struct urb *ctrl_urb,
			      struct usb_ctrlrequest *ctrl_req,
			      u8 *ctrl_urb_data, u32 size)
{
	int result;
	int pipe;

	/* Sanity check */
	if (!serial || !ctrl_urb || !ctrl_req) {
		pr_err("%s: Wrong arguments\n", __func__);
		return -EINVAL;
	}

	/* initialize */
	ctrl_req->wValue = 0;
	ctrl_req->wIndex = cpu_to_le16(hso_port_to_mux(port));
	ctrl_req->wLength = cpu_to_le16(size);

	if (type == USB_CDC_GET_ENCAPSULATED_RESPONSE) {
		/* Reading command */
		ctrl_req->bRequestType = USB_DIR_IN |
					 USB_TYPE_OPTION_VENDOR |
					 USB_RECIP_INTERFACE;
		ctrl_req->bRequest = USB_CDC_GET_ENCAPSULATED_RESPONSE;
		pipe = usb_rcvctrlpipe(serial->parent->usb, 0);
	} else {
		/* Writing command */
		ctrl_req->bRequestType = USB_DIR_OUT |
					 USB_TYPE_OPTION_VENDOR |
					 USB_RECIP_INTERFACE;
		ctrl_req->bRequest = USB_CDC_SEND_ENCAPSULATED_COMMAND;
		pipe = usb_sndctrlpipe(serial->parent->usb, 0);
	}
	/* syslog */
	hso_dbg(0x2, "%s command (%02x) len: %d, port: %d\n",
		type == USB_CDC_GET_ENCAPSULATED_RESPONSE ? "Read" : "Write",
		ctrl_req->bRequestType, ctrl_req->wLength, port);

	/* Load ctrl urb */
	ctrl_urb->transfer_flags = 0;
	usb_fill_control_urb(ctrl_urb,
			     serial->parent->usb,
			     pipe,
			     (u8 *) ctrl_req,
			     ctrl_urb_data, size, ctrl_callback, serial);
	/* Send it on merry way */
	result = usb_submit_urb(ctrl_urb, GFP_ATOMIC);
	if (result) {
		dev_err(&ctrl_urb->dev->dev,
			"%s failed submit ctrl_urb %d type %d\n", __func__,
			result, type);
		return result;
	}

	/* done */
	return size;
}

/* called by intr_callback when read occurs */
static int hso_mux_serial_read(struct hso_serial *serial)
{
	if (!serial)
		return -EINVAL;

	/* clean data */
	memset(serial->rx_data[0], 0, CTRL_URB_RX_SIZE);
	/* make the request */

	if (serial->num_rx_urbs != 1) {
		dev_err(&serial->parent->interface->dev,
			"ERROR: mux'd reads with multiple buffers "
			"not possible\n");
		return 0;
	}
	return mux_device_request(serial,
				  USB_CDC_GET_ENCAPSULATED_RESPONSE,
				  serial->parent->port_spec & HSO_PORT_MASK,
				  serial->rx_urb[0],
				  &serial->ctrl_req_rx,
				  serial->rx_data[0], serial->rx_data_length);
}

/* used for muxed serial port callback (muxed serial read) */
static void intr_callback(struct urb *urb)
{
	struct hso_shared_int *shared_int = urb->context;
	struct hso_serial *serial;
	unsigned char *port_req;
	int status = urb->status;
	unsigned long flags;
	int i;

	usb_mark_last_busy(urb->dev);

	/* sanity check */
	if (!shared_int)
		return;

	/* status check */
	if (status) {
		handle_usb_error(status, __func__, NULL);
		return;
	}
	hso_dbg(0x8, "--- Got intr callback 0x%02X ---\n", status);

	/* what request? */
	port_req = urb->transfer_buffer;
	hso_dbg(0x8, "port_req = 0x%.2X\n", *port_req);
	/* loop over all muxed ports to find the one sending this */
	for (i = 0; i < 8; i++) {
		/* max 8 channels on MUX */
		if (*port_req & (1 << i)) {
			serial = get_serial_by_shared_int_and_type(shared_int,
								   (1 << i));
			if (serial != NULL) {
				hso_dbg(0x1, "Pending read interrupt on port %d\n",
					i);
				spin_lock_irqsave(&serial->serial_lock, flags);
				if (serial->rx_state == RX_IDLE &&
					serial->port.count > 0) {
					/* Setup and send a ctrl req read on
					 * port i */
					if (!serial->rx_urb_filled[0]) {
						serial->rx_state = RX_SENT;
						hso_mux_serial_read(serial);
					} else
						serial->rx_state = RX_PENDING;
				} else {
					hso_dbg(0x1, "Already a read pending on port %d or port not open\n",
						i);
				}
				spin_unlock_irqrestore(&serial->serial_lock,
						       flags);
			}
		}
	}
	/* Resubmit interrupt urb */
	hso_mux_submit_intr_urb(shared_int, urb->dev, GFP_ATOMIC);
}

/* called for writing to muxed serial port */
static int hso_mux_serial_write_data(struct hso_serial *serial)
{
	if (NULL == serial)
		return -EINVAL;

	return mux_device_request(serial,
				  USB_CDC_SEND_ENCAPSULATED_COMMAND,
				  serial->parent->port_spec & HSO_PORT_MASK,
				  serial->tx_urb,
				  &serial->ctrl_req_tx,
				  serial->tx_data, serial->tx_data_count);
}

/* write callback for Diag and CS port */
static void hso_std_serial_write_bulk_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	int status = urb->status;
	unsigned long flags;

	/* sanity check */
	if (!serial) {
		hso_dbg(0x1, "serial == NULL\n");
		return;
	}

	spin_lock_irqsave(&serial->serial_lock, flags);
	serial->tx_urb_used = 0;
	spin_unlock_irqrestore(&serial->serial_lock, flags);
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}
	hso_put_activity(serial->parent);
	tty_port_tty_wakeup(&serial->port);
	hso_kick_transmit(serial);

	hso_dbg(0x1, "\n");
}

/* called for writing diag or CS serial port */
static int hso_std_serial_write_data(struct hso_serial *serial)
{
	int count = serial->tx_data_count;
	int result;

	usb_fill_bulk_urb(serial->tx_urb,
			  serial->parent->usb,
			  usb_sndbulkpipe(serial->parent->usb,
					  serial->out_endp->
					  bEndpointAddress & 0x7F),
			  serial->tx_data, serial->tx_data_count,
			  hso_std_serial_write_bulk_callback, serial);

	result = usb_submit_urb(serial->tx_urb, GFP_ATOMIC);
	if (result) {
		dev_warn(&serial->parent->usb->dev,
			 "Failed to submit urb - res %d\n", result);
		return result;
	}

	return count;
}

/* callback after read or write on muxed serial port */
static void ctrl_callback(struct urb *urb)
{
	struct hso_serial *serial = urb->context;
	struct usb_ctrlrequest *req;
	int status = urb->status;
	unsigned long flags;

	/* sanity check */
	if (!serial)
		return;

	spin_lock_irqsave(&serial->serial_lock, flags);
	serial->tx_urb_used = 0;
	spin_unlock_irqrestore(&serial->serial_lock, flags);
	if (status) {
		handle_usb_error(status, __func__, serial->parent);
		return;
	}

	/* what request? */
	req = (struct usb_ctrlrequest *)(urb->setup_packet);
	hso_dbg(0x8, "--- Got muxed ctrl callback 0x%02X ---\n", status);
	hso_dbg(0x8, "Actual length of urb = %d\n", urb->actual_length);
	DUMP1(urb->transfer_buffer, urb->actual_length);

	if (req->bRequestType ==
	    (USB_DIR_IN | USB_TYPE_OPTION_VENDOR | USB_RECIP_INTERFACE)) {
		/* response to a read command */
		serial->rx_urb_filled[0] = 1;
		spin_lock_irqsave(&serial->serial_lock, flags);
		put_rxbuf_data_and_resubmit_ctrl_urb(serial);
		spin_unlock_irqrestore(&serial->serial_lock, flags);
	} else {
		hso_put_activity(serial->parent);
		tty_port_tty_wakeup(&serial->port);
		/* response to a write command */
		hso_kick_transmit(serial);
	}
}

/* handle RX data for serial port */
static int put_rxbuf_data(struct urb *urb, struct hso_serial *serial)
{
	struct tty_struct *tty;
	int count;

	/* Sanity check */
	if (urb == NULL || serial == NULL) {
		hso_dbg(0x1, "serial = NULL\n");
		return -2;
	}

	tty = tty_port_tty_get(&serial->port);

	if (tty && tty_throttled(tty)) {
		tty_kref_put(tty);
		return -1;
	}

	/* Push data to tty */
	hso_dbg(0x1, "data to push to tty\n");
	count = tty_buffer_request_room(&serial->port, urb->actual_length);
	if (count >= urb->actual_length) {
		tty_insert_flip_string(&serial->port, urb->transfer_buffer,
				       urb->actual_length);
		tty_flip_buffer_push(&serial->port);
	} else {
		dev_warn(&serial->parent->usb->dev,
			 "dropping data, %d bytes lost\n", urb->actual_length);
	}

	tty_kref_put(tty);

	serial->rx_urb_filled[hso_urb_to_index(serial, urb)] = 0;

	return 0;
}


/* Base driver functions */

static void hso_log_port(struct hso_device *hso_dev)
{
	char *port_type;
	char port_dev[20];

	switch (hso_dev->port_spec & HSO_PORT_MASK) {
	case HSO_PORT_CONTROL:
		port_type = "Control";
		break;
	case HSO_PORT_APP:
		port_type = "Application";
		break;
	case HSO_PORT_GPS:
		port_type = "GPS";
		break;
	case HSO_PORT_GPS_CONTROL:
		port_type = "GPS control";
		break;
	case HSO_PORT_APP2:
		port_type = "Application2";
		break;
	case HSO_PORT_PCSC:
		port_type = "PCSC";
		break;
	case HSO_PORT_DIAG:
		port_type = "Diagnostic";
		break;
	case HSO_PORT_DIAG2:
		port_type = "Diagnostic2";
		break;
	case HSO_PORT_MODEM:
		port_type = "Modem";
		break;
	case HSO_PORT_NETWORK:
		port_type = "Network";
		break;
	default:
		port_type = "Unknown";
		break;
	}
	if ((hso_dev->port_spec & HSO_PORT_MASK) == HSO_PORT_NETWORK) {
		sprintf(port_dev, "%s", dev2net(hso_dev)->net->name);
	} else
		sprintf(port_dev, "/dev/%s%d", tty_filename,
			dev2ser(hso_dev)->minor);

	dev_dbg(&hso_dev->interface->dev, "HSO: Found %s port %s\n",
		port_type, port_dev);
}

static int hso_start_net_device(struct hso_device *hso_dev)
{
	int i, result = 0;
	struct hso_net *hso_net = dev2net(hso_dev);

	if (!hso_net)
		return -ENODEV;

	/* send URBs for all read buffers */
	for (i = 0; i < MUX_BULK_RX_BUF_COUNT; i++) {

		/* Prep a receive URB */
		usb_fill_bulk_urb(hso_net->mux_bulk_rx_urb_pool[i],
				  hso_dev->usb,
				  usb_rcvbulkpipe(hso_dev->usb,
						  hso_net->in_endp->
						  bEndpointAddress & 0x7F),
				  hso_net->mux_bulk_rx_buf_pool[i],
				  MUX_BULK_RX_BUF_SIZE, read_bulk_callback,
				  hso_net);

		/* Put it out there so the device can send us stuff */
		result = usb_submit_urb(hso_net->mux_bulk_rx_urb_pool[i],
					GFP_NOIO);
		if (result)
			dev_warn(&hso_dev->usb->dev,
				"%s failed mux_bulk_rx_urb[%d] %d\n", __func__,
				i, result);
	}

	return result;
}

static int hso_stop_net_device(struct hso_device *hso_dev)
{
	int i;
	struct hso_net *hso_net = dev2net(hso_dev);

	if (!hso_net)
		return -ENODEV;

	for (i = 0; i < MUX_BULK_RX_BUF_COUNT; i++) {
		if (hso_net->mux_bulk_rx_urb_pool[i])
			usb_kill_urb(hso_net->mux_bulk_rx_urb_pool[i]);

	}
	if (hso_net->mux_bulk_tx_urb)
		usb_kill_urb(hso_net->mux_bulk_tx_urb);

	return 0;
}

static int hso_start_serial_device(struct hso_device *hso_dev, gfp_t flags)
{
	int i, result = 0;
	struct hso_serial *serial = dev2ser(hso_dev);

	if (!serial)
		return -ENODEV;

	/* If it is not the MUX port fill in and submit a bulk urb (already
	 * allocated in hso_serial_start) */
	if (!(serial->parent->port_spec & HSO_INTF_MUX)) {
		for (i = 0; i < serial->num_rx_urbs; i++) {
			usb_fill_bulk_urb(serial->rx_urb[i],
					  serial->parent->usb,
					  usb_rcvbulkpipe(serial->parent->usb,
							  serial->in_endp->
							  bEndpointAddress &
							  0x7F),
					  serial->rx_data[i],
					  serial->rx_data_length,
					  hso_std_serial_read_bulk_callback,
					  serial);
			result = usb_submit_urb(serial->rx_urb[i], flags);
			if (result) {
				dev_warn(&serial->parent->usb->dev,
					 "Failed to submit urb - res %d\n",
					 result);
				break;
			}
		}
	} else {
		mutex_lock(&serial->shared_int->shared_int_lock);
		if (!serial->shared_int->use_count) {
			result =
			    hso_mux_submit_intr_urb(serial->shared_int,
						    hso_dev->usb, flags);
		}
		serial->shared_int->use_count++;
		mutex_unlock(&serial->shared_int->shared_int_lock);
	}
	if (serial->tiocmget)
		tiocmget_submit_urb(serial,
				    serial->tiocmget,
				    serial->parent->usb);
	return result;
}

static int hso_stop_serial_device(struct hso_device *hso_dev)
{
	int i;
	struct hso_serial *serial = dev2ser(hso_dev);
	struct hso_tiocmget  *tiocmget;

	if (!serial)
		return -ENODEV;

	for (i = 0; i < serial->num_rx_urbs; i++) {
		if (serial->rx_urb[i]) {
			usb_kill_urb(serial->rx_urb[i]);
			serial->rx_urb_filled[i] = 0;
		}
	}
	serial->curr_rx_urb_idx = 0;

	if (serial->tx_urb)
		usb_kill_urb(serial->tx_urb);

	if (serial->shared_int) {
		mutex_lock(&serial->shared_int->shared_int_lock);
		if (serial->shared_int->use_count &&
		    (--serial->shared_int->use_count == 0)) {
			struct urb *urb;

			urb = serial->shared_int->shared_intr_urb;
			if (urb)
				usb_kill_urb(urb);
		}
		mutex_unlock(&serial->shared_int->shared_int_lock);
	}
	tiocmget = serial->tiocmget;
	if (tiocmget) {
		wake_up_interruptible(&tiocmget->waitq);
		usb_kill_urb(tiocmget->urb);
	}

	return 0;
}

static void hso_serial_tty_unregister(struct hso_serial *serial)
{
	tty_unregister_device(tty_drv, serial->minor);
	release_minor(serial);
}

static void hso_serial_common_free(struct hso_serial *serial)
{
	int i;

	for (i = 0; i < serial->num_rx_urbs; i++) {
		/* unlink and free RX URB */
		usb_free_urb(serial->rx_urb[i]);
		/* free the RX buffer */
		kfree(serial->rx_data[i]);
	}

	/* unlink and free TX URB */
	usb_free_urb(serial->tx_urb);
	kfree(serial->tx_buffer);
	kfree(serial->tx_data);
	tty_port_destroy(&serial->port);
}

static int hso_serial_common_create(struct hso_serial *serial, int num_urbs,
				    int rx_size, int tx_size)
{
	int i;

	tty_port_init(&serial->port);

	if (obtain_minor(serial))
		goto exit2;

	/* register our minor number */
	serial->parent->dev = tty_port_register_device_attr(&serial->port,
			tty_drv, serial->minor, &serial->parent->interface->dev,
			serial->parent, hso_serial_dev_groups);
	if (IS_ERR(serial->parent->dev)) {
		release_minor(serial);
		goto exit2;
	}

	serial->magic = HSO_SERIAL_MAGIC;
	spin_lock_init(&serial->serial_lock);
	serial->num_rx_urbs = num_urbs;

	/* RX, allocate urb and initialize */

	/* prepare our RX buffer */
	serial->rx_data_length = rx_size;
	for (i = 0; i < serial->num_rx_urbs; i++) {
		serial->rx_urb[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (!serial->rx_urb[i])
			goto exit;
		serial->rx_urb[i]->transfer_buffer = NULL;
		serial->rx_urb[i]->transfer_buffer_length = 0;
		serial->rx_data[i] = kzalloc(serial->rx_data_length,
					     GFP_KERNEL);
		if (!serial->rx_data[i])
			goto exit;
	}

	/* TX, allocate urb and initialize */
	serial->tx_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!serial->tx_urb)
		goto exit;
	serial->tx_urb->transfer_buffer = NULL;
	serial->tx_urb->transfer_buffer_length = 0;
	/* prepare our TX buffer */
	serial->tx_data_count = 0;
	serial->tx_buffer_count = 0;
	serial->tx_data_length = tx_size;
	serial->tx_data = kzalloc(serial->tx_data_length, GFP_KERNEL);
	if (!serial->tx_data)
		goto exit;

	serial->tx_buffer = kzalloc(serial->tx_data_length, GFP_KERNEL);
	if (!serial->tx_buffer)
		goto exit;

	return 0;
exit:
	hso_serial_tty_unregister(serial);
exit2:
	hso_serial_common_free(serial);
	return -1;
}

/* Creates a general hso device */
static struct hso_device *hso_create_device(struct usb_interface *intf,
					    int port_spec)
{
	struct hso_device *hso_dev;

	hso_dev = kzalloc(sizeof(*hso_dev), GFP_ATOMIC);
	if (!hso_dev)
		return NULL;

	hso_dev->port_spec = port_spec;
	hso_dev->usb = interface_to_usbdev(intf);
	hso_dev->interface = intf;
	kref_init(&hso_dev->ref);
	mutex_init(&hso_dev->mutex);

	INIT_WORK(&hso_dev->async_get_intf, async_get_intf);
	INIT_WORK(&hso_dev->async_put_intf, async_put_intf);

	return hso_dev;
}

/* Removes a network device in the network device table */
static int remove_net_device(struct hso_device *hso_dev)
{
	int i;

	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] == hso_dev) {
			network_table[i] = NULL;
			break;
		}
	}
	if (i == HSO_MAX_NET_DEVICES)
		return -1;
	return 0;
}

/* Frees our network device */
static void hso_free_net_device(struct hso_device *hso_dev, bool bailout)
{
	int i;
	struct hso_net *hso_net = dev2net(hso_dev);

	if (!hso_net)
		return;

	remove_net_device(hso_net->parent);

	if (hso_net->net)
		unregister_netdev(hso_net->net);

	/* start freeing */
	for (i = 0; i < MUX_BULK_RX_BUF_COUNT; i++) {
		usb_free_urb(hso_net->mux_bulk_rx_urb_pool[i]);
		kfree(hso_net->mux_bulk_rx_buf_pool[i]);
		hso_net->mux_bulk_rx_buf_pool[i] = NULL;
	}
	usb_free_urb(hso_net->mux_bulk_tx_urb);
	kfree(hso_net->mux_bulk_tx_buf);
	hso_net->mux_bulk_tx_buf = NULL;

	if (hso_net->net && !bailout)
		free_netdev(hso_net->net);

	kfree(hso_dev);
}

static const struct net_device_ops hso_netdev_ops = {
	.ndo_open	= hso_net_open,
	.ndo_stop	= hso_net_close,
	.ndo_start_xmit = hso_net_start_xmit,
	.ndo_tx_timeout = hso_net_tx_timeout,
};

/* initialize the network interface */
static void hso_net_init(struct net_device *net)
{
	struct hso_net *hso_net = netdev_priv(net);

	hso_dbg(0x1, "sizeof hso_net is %zu\n", sizeof(*hso_net));

	/* fill in the other fields */
	net->netdev_ops = &hso_netdev_ops;
	net->watchdog_timeo = HSO_NET_TX_TIMEOUT;
	net->flags = IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
	net->type = ARPHRD_NONE;
	net->mtu = DEFAULT_MTU - 14;
	net->tx_queue_len = 10;
	net->ethtool_ops = &ops;

	/* and initialize the semaphore */
	spin_lock_init(&hso_net->net_lock);
}

/* Adds a network device in the network device table */
static int add_net_device(struct hso_device *hso_dev)
{
	int i;

	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] == NULL) {
			network_table[i] = hso_dev;
			break;
		}
	}
	if (i == HSO_MAX_NET_DEVICES)
		return -1;
	return 0;
}

static int hso_rfkill_set_block(void *data, bool blocked)
{
	struct hso_device *hso_dev = data;
	int enabled = !blocked;
	int rv;

	mutex_lock(&hso_dev->mutex);
	if (hso_dev->usb_gone)
		rv = 0;
	else
		rv = usb_control_msg(hso_dev->usb, usb_sndctrlpipe(hso_dev->usb, 0),
				       enabled ? 0x82 : 0x81, 0x40, 0, 0, NULL, 0,
				       USB_CTRL_SET_TIMEOUT);
	mutex_unlock(&hso_dev->mutex);
	return rv;
}

static const struct rfkill_ops hso_rfkill_ops = {
	.set_block = hso_rfkill_set_block,
};

/* Creates and sets up everything for rfkill */
static void hso_create_rfkill(struct hso_device *hso_dev,
			     struct usb_interface *interface)
{
	struct hso_net *hso_net = dev2net(hso_dev);
	struct device *dev = &hso_net->net->dev;
	static u32 rfkill_counter;

	snprintf(hso_net->name, sizeof(hso_net->name), "hso-%d",
		 rfkill_counter++);

	hso_net->rfkill = rfkill_alloc(hso_net->name,
				       &interface_to_usbdev(interface)->dev,
				       RFKILL_TYPE_WWAN,
				       &hso_rfkill_ops, hso_dev);
	if (!hso_net->rfkill)
		return;

	if (rfkill_register(hso_net->rfkill) < 0) {
		rfkill_destroy(hso_net->rfkill);
		hso_net->rfkill = NULL;
		dev_err(dev, "%s - Failed to register rfkill\n", __func__);
		return;
	}
}

static struct device_type hso_type = {
	.name	= "wwan",
};

/* Creates our network device */
static struct hso_device *hso_create_net_device(struct usb_interface *interface,
						int port_spec)
{
	int result, i;
	struct net_device *net;
	struct hso_net *hso_net;
	struct hso_device *hso_dev;

	hso_dev = hso_create_device(interface, port_spec);
	if (!hso_dev)
		return NULL;

	/* allocate our network device, then we can put in our private data */
	/* call hso_net_init to do the basic initialization */
	net = alloc_netdev(sizeof(struct hso_net), "hso%d", NET_NAME_UNKNOWN,
			   hso_net_init);
	if (!net) {
		dev_err(&interface->dev, "Unable to create ethernet device\n");
		goto exit;
	}

	hso_net = netdev_priv(net);

	hso_dev->port_data.dev_net = hso_net;
	hso_net->net = net;
	hso_net->parent = hso_dev;

	hso_net->in_endp = hso_get_ep(interface, USB_ENDPOINT_XFER_BULK,
				      USB_DIR_IN);
	if (!hso_net->in_endp) {
		dev_err(&interface->dev, "Can't find BULK IN endpoint\n");
		goto exit;
	}
	hso_net->out_endp = hso_get_ep(interface, USB_ENDPOINT_XFER_BULK,
				       USB_DIR_OUT);
	if (!hso_net->out_endp) {
		dev_err(&interface->dev, "Can't find BULK OUT endpoint\n");
		goto exit;
	}
	SET_NETDEV_DEV(net, &interface->dev);
	SET_NETDEV_DEVTYPE(net, &hso_type);

	/* start allocating */
	for (i = 0; i < MUX_BULK_RX_BUF_COUNT; i++) {
		hso_net->mux_bulk_rx_urb_pool[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (!hso_net->mux_bulk_rx_urb_pool[i])
			goto exit;
		hso_net->mux_bulk_rx_buf_pool[i] = kzalloc(MUX_BULK_RX_BUF_SIZE,
							   GFP_KERNEL);
		if (!hso_net->mux_bulk_rx_buf_pool[i])
			goto exit;
	}
	hso_net->mux_bulk_tx_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!hso_net->mux_bulk_tx_urb)
		goto exit;
	hso_net->mux_bulk_tx_buf = kzalloc(MUX_BULK_TX_BUF_SIZE, GFP_KERNEL);
	if (!hso_net->mux_bulk_tx_buf)
		goto exit;

	add_net_device(hso_dev);

	/* registering our net device */
	result = register_netdev(net);
	if (result) {
		dev_err(&interface->dev, "Failed to register device\n");
		goto exit;
	}

	hso_log_port(hso_dev);

	hso_create_rfkill(hso_dev, interface);

	return hso_dev;
exit:
	hso_free_net_device(hso_dev, true);
	return NULL;
}

static void hso_free_tiomget(struct hso_serial *serial)
{
	struct hso_tiocmget *tiocmget;
	if (!serial)
		return;
	tiocmget = serial->tiocmget;
	if (tiocmget) {
		usb_free_urb(tiocmget->urb);
		tiocmget->urb = NULL;
		serial->tiocmget = NULL;
		kfree(tiocmget->serial_state_notification);
		tiocmget->serial_state_notification = NULL;
		kfree(tiocmget);
	}
}

/* Frees an AT channel ( goes for both mux and non-mux ) */
static void hso_free_serial_device(struct hso_device *hso_dev)
{
	struct hso_serial *serial = dev2ser(hso_dev);

	if (!serial)
		return;

	hso_serial_common_free(serial);

	if (serial->shared_int) {
		mutex_lock(&serial->shared_int->shared_int_lock);
		if (--serial->shared_int->ref_count == 0)
			hso_free_shared_int(serial->shared_int);
		else
			mutex_unlock(&serial->shared_int->shared_int_lock);
	}
	hso_free_tiomget(serial);
	kfree(serial);
	kfree(hso_dev);
}

/* Creates a bulk AT channel */
static struct hso_device *hso_create_bulk_serial_device(
			struct usb_interface *interface, int port)
{
	struct hso_device *hso_dev;
	struct hso_serial *serial;
	int num_urbs;
	struct hso_tiocmget *tiocmget;

	hso_dev = hso_create_device(interface, port);
	if (!hso_dev)
		return NULL;

	serial = kzalloc(sizeof(*serial), GFP_KERNEL);
	if (!serial)
		goto exit;

	serial->parent = hso_dev;
	hso_dev->port_data.dev_serial = serial;

	if ((port & HSO_PORT_MASK) == HSO_PORT_MODEM) {
		num_urbs = 2;
		serial->tiocmget = kzalloc(sizeof(struct hso_tiocmget),
					   GFP_KERNEL);
		if (!serial->tiocmget)
			goto exit;
		serial->tiocmget->serial_state_notification
			= kzalloc(sizeof(struct hso_serial_state_notification),
					   GFP_KERNEL);
		if (!serial->tiocmget->serial_state_notification)
			goto exit;
		tiocmget = serial->tiocmget;
		tiocmget->endp = hso_get_ep(interface,
					    USB_ENDPOINT_XFER_INT,
					    USB_DIR_IN);
		if (!tiocmget->endp) {
			dev_err(&interface->dev, "Failed to find INT IN ep\n");
			goto exit;
		}

		tiocmget->urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!tiocmget->urb)
			goto exit;

		mutex_init(&tiocmget->mutex);
		init_waitqueue_head(&tiocmget->waitq);
	} else {
		num_urbs = 1;
	}

	if (hso_serial_common_create(serial, num_urbs, BULK_URB_RX_SIZE,
				     BULK_URB_TX_SIZE))
		goto exit;

	serial->in_endp = hso_get_ep(interface, USB_ENDPOINT_XFER_BULK,
				     USB_DIR_IN);
	if (!serial->in_endp) {
		dev_err(&interface->dev, "Failed to find BULK IN ep\n");
		goto exit2;
	}

	if (!
	    (serial->out_endp =
	     hso_get_ep(interface, USB_ENDPOINT_XFER_BULK, USB_DIR_OUT))) {
		dev_err(&interface->dev, "Failed to find BULK OUT ep\n");
		goto exit2;
	}

	serial->write_data = hso_std_serial_write_data;

	/* setup the proc dirs and files if needed */
	hso_log_port(hso_dev);

	/* done, return it */
	return hso_dev;

exit2:
	hso_serial_tty_unregister(serial);
	hso_serial_common_free(serial);
exit:
	hso_free_tiomget(serial);
	kfree(serial);
	kfree(hso_dev);
	return NULL;
}

/* Creates a multiplexed AT channel */
static
struct hso_device *hso_create_mux_serial_device(struct usb_interface *interface,
						int port,
						struct hso_shared_int *mux)
{
	struct hso_device *hso_dev;
	struct hso_serial *serial;
	int port_spec;

	port_spec = HSO_INTF_MUX;
	port_spec &= ~HSO_PORT_MASK;

	port_spec |= hso_mux_to_port(port);
	if ((port_spec & HSO_PORT_MASK) == HSO_PORT_NO_PORT)
		return NULL;

	hso_dev = hso_create_device(interface, port_spec);
	if (!hso_dev)
		return NULL;

	serial = kzalloc(sizeof(*serial), GFP_KERNEL);
	if (!serial)
		goto exit;

	hso_dev->port_data.dev_serial = serial;
	serial->parent = hso_dev;

	if (hso_serial_common_create
	    (serial, 1, CTRL_URB_RX_SIZE, CTRL_URB_TX_SIZE))
		goto exit;

	serial->tx_data_length--;
	serial->write_data = hso_mux_serial_write_data;

	serial->shared_int = mux;
	mutex_lock(&serial->shared_int->shared_int_lock);
	serial->shared_int->ref_count++;
	mutex_unlock(&serial->shared_int->shared_int_lock);

	/* setup the proc dirs and files if needed */
	hso_log_port(hso_dev);

	/* done, return it */
	return hso_dev;

exit:
	if (serial) {
		tty_unregister_device(tty_drv, serial->minor);
		kfree(serial);
	}
	kfree(hso_dev);
	return NULL;

}

static void hso_free_shared_int(struct hso_shared_int *mux)
{
	usb_free_urb(mux->shared_intr_urb);
	kfree(mux->shared_intr_buf);
	mutex_unlock(&mux->shared_int_lock);
	kfree(mux);
}

static
struct hso_shared_int *hso_create_shared_int(struct usb_interface *interface)
{
	struct hso_shared_int *mux = kzalloc(sizeof(*mux), GFP_KERNEL);

	if (!mux)
		return NULL;

	mux->intr_endp = hso_get_ep(interface, USB_ENDPOINT_XFER_INT,
				    USB_DIR_IN);
	if (!mux->intr_endp) {
		dev_err(&interface->dev, "Can't find INT IN endpoint\n");
		goto exit;
	}

	mux->shared_intr_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!mux->shared_intr_urb)
		goto exit;
	mux->shared_intr_buf =
		kzalloc(le16_to_cpu(mux->intr_endp->wMaxPacketSize),
			GFP_KERNEL);
	if (!mux->shared_intr_buf)
		goto exit;

	mutex_init(&mux->shared_int_lock);

	return mux;

exit:
	kfree(mux->shared_intr_buf);
	usb_free_urb(mux->shared_intr_urb);
	kfree(mux);
	return NULL;
}

/* Gets the port spec for a certain interface */
static int hso_get_config_data(struct usb_interface *interface)
{
	struct usb_device *usbdev = interface_to_usbdev(interface);
	u8 *config_data = kmalloc(17, GFP_KERNEL);
	u32 if_num = interface->cur_altsetting->desc.bInterfaceNumber;
	s32 result;

	if (!config_data)
		return -ENOMEM;
	if (usb_control_msg(usbdev, usb_rcvctrlpipe(usbdev, 0),
			    0x86, 0xC0, 0, 0, config_data, 17,
			    USB_CTRL_SET_TIMEOUT) != 0x11) {
		kfree(config_data);
		return -EIO;
	}

	/* check if we have a valid interface */
	if (if_num > 16) {
		kfree(config_data);
		return -EINVAL;
	}

	switch (config_data[if_num]) {
	case 0x0:
		result = 0;
		break;
	case 0x1:
		result = HSO_PORT_DIAG;
		break;
	case 0x2:
		result = HSO_PORT_GPS;
		break;
	case 0x3:
		result = HSO_PORT_GPS_CONTROL;
		break;
	case 0x4:
		result = HSO_PORT_APP;
		break;
	case 0x5:
		result = HSO_PORT_APP2;
		break;
	case 0x6:
		result = HSO_PORT_CONTROL;
		break;
	case 0x7:
		result = HSO_PORT_NETWORK;
		break;
	case 0x8:
		result = HSO_PORT_MODEM;
		break;
	case 0x9:
		result = HSO_PORT_MSD;
		break;
	case 0xa:
		result = HSO_PORT_PCSC;
		break;
	case 0xb:
		result = HSO_PORT_VOICE;
		break;
	default:
		result = 0;
	}

	if (result)
		result |= HSO_INTF_BULK;

	if (config_data[16] & 0x1)
		result |= HSO_INFO_CRC_BUG;

	kfree(config_data);
	return result;
}

/* called once for each interface upon device insertion */
static int hso_probe(struct usb_interface *interface,
		     const struct usb_device_id *id)
{
	int mux, i, if_num, port_spec;
	unsigned char port_mask;
	struct hso_device *hso_dev = NULL;
	struct hso_shared_int *shared_int;
	struct hso_device *tmp_dev = NULL;

	if (interface->cur_altsetting->desc.bInterfaceClass != 0xFF) {
		dev_err(&interface->dev, "Not our interface\n");
		return -ENODEV;
	}

	if_num = interface->cur_altsetting->desc.bInterfaceNumber;

	/* Get the interface/port specification from either driver_info or from
	 * the device itself */
	if (id->driver_info) {
		/* if_num is controlled by the device, driver_info is a 0 terminated
		 * array. Make sure, the access is in bounds! */
		for (i = 0; i <= if_num; ++i)
			if (((u32 *)(id->driver_info))[i] == 0)
				goto exit;
		port_spec = ((u32 *)(id->driver_info))[if_num];
	} else {
		port_spec = hso_get_config_data(interface);
		if (port_spec < 0)
			goto exit;
	}

	/* Check if we need to switch to alt interfaces prior to port
	 * configuration */
	if (interface->num_altsetting > 1)
		usb_set_interface(interface_to_usbdev(interface), if_num, 1);
	interface->needs_remote_wakeup = 1;

	/* Allocate new hso device(s) */
	switch (port_spec & HSO_INTF_MASK) {
	case HSO_INTF_MUX:
		if ((port_spec & HSO_PORT_MASK) == HSO_PORT_NETWORK) {
			/* Create the network device */
			if (!disable_net) {
				hso_dev = hso_create_net_device(interface,
								port_spec);
				if (!hso_dev)
					goto exit;
				tmp_dev = hso_dev;
			}
		}

		if (hso_get_mux_ports(interface, &port_mask))
			/* TODO: de-allocate everything */
			goto exit;

		shared_int = hso_create_shared_int(interface);
		if (!shared_int)
			goto exit;

		for (i = 1, mux = 0; i < 0x100; i = i << 1, mux++) {
			if (port_mask & i) {
				hso_dev = hso_create_mux_serial_device(
						interface, i, shared_int);
				if (!hso_dev)
					goto exit;
			}
		}

		if (tmp_dev)
			hso_dev = tmp_dev;
		break;

	case HSO_INTF_BULK:
		/* It's a regular bulk interface */
		if ((port_spec & HSO_PORT_MASK) == HSO_PORT_NETWORK) {
			if (!disable_net)
				hso_dev =
				    hso_create_net_device(interface, port_spec);
		} else {
			hso_dev =
			    hso_create_bulk_serial_device(interface, port_spec);
		}
		if (!hso_dev)
			goto exit;
		break;
	default:
		goto exit;
	}

	/* save our data pointer in this device */
	usb_set_intfdata(interface, hso_dev);

	/* done */
	return 0;
exit:
	hso_free_interface(interface);
	return -ENODEV;
}

/* device removed, cleaning up */
static void hso_disconnect(struct usb_interface *interface)
{
	hso_free_interface(interface);

	/* remove reference of our private data */
	usb_set_intfdata(interface, NULL);
}

static void async_get_intf(struct work_struct *data)
{
	struct hso_device *hso_dev =
	    container_of(data, struct hso_device, async_get_intf);
	usb_autopm_get_interface(hso_dev->interface);
}

static void async_put_intf(struct work_struct *data)
{
	struct hso_device *hso_dev =
	    container_of(data, struct hso_device, async_put_intf);
	usb_autopm_put_interface(hso_dev->interface);
}

static int hso_get_activity(struct hso_device *hso_dev)
{
	if (hso_dev->usb->state == USB_STATE_SUSPENDED) {
		if (!hso_dev->is_active) {
			hso_dev->is_active = 1;
			schedule_work(&hso_dev->async_get_intf);
		}
	}

	if (hso_dev->usb->state != USB_STATE_CONFIGURED)
		return -EAGAIN;

	usb_mark_last_busy(hso_dev->usb);

	return 0;
}

static int hso_put_activity(struct hso_device *hso_dev)
{
	if (hso_dev->usb->state != USB_STATE_SUSPENDED) {
		if (hso_dev->is_active) {
			hso_dev->is_active = 0;
			schedule_work(&hso_dev->async_put_intf);
			return -EAGAIN;
		}
	}
	hso_dev->is_active = 0;
	return 0;
}

/* called by kernel when we need to suspend device */
static int hso_suspend(struct usb_interface *iface, pm_message_t message)
{
	int i, result;

	/* Stop all serial ports */
	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] && (serial_table[i]->interface == iface)) {
			result = hso_stop_serial_device(serial_table[i]);
			if (result)
				goto out;
		}
	}

	/* Stop all network ports */
	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] &&
		    (network_table[i]->interface == iface)) {
			result = hso_stop_net_device(network_table[i]);
			if (result)
				goto out;
		}
	}

out:
	return 0;
}

/* called by kernel when we need to resume device */
static int hso_resume(struct usb_interface *iface)
{
	int i, result = 0;
	struct hso_net *hso_net;

	/* Start all serial ports */
	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] && (serial_table[i]->interface == iface)) {
			if (dev2ser(serial_table[i])->port.count) {
				result =
				    hso_start_serial_device(serial_table[i], GFP_NOIO);
				hso_kick_transmit(dev2ser(serial_table[i]));
				if (result)
					goto out;
			}
		}
	}

	/* Start all network ports */
	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] &&
		    (network_table[i]->interface == iface)) {
			hso_net = dev2net(network_table[i]);
			if (hso_net->flags & IFF_UP) {
				/* First transmit any lingering data,
				   then restart the device. */
				if (hso_net->skb_tx_buf) {
					dev_dbg(&iface->dev,
						"Transmitting"
						" lingering data\n");
					hso_net_start_xmit(hso_net->skb_tx_buf,
							   hso_net->net);
					hso_net->skb_tx_buf = NULL;
				}
				result = hso_start_net_device(network_table[i]);
				if (result)
					goto out;
			}
		}
	}

out:
	return result;
}

static void hso_serial_ref_free(struct kref *ref)
{
	struct hso_device *hso_dev = container_of(ref, struct hso_device, ref);

	hso_free_serial_device(hso_dev);
}

static void hso_free_interface(struct usb_interface *interface)
{
	struct hso_serial *serial;
	int i;

	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] &&
		    (serial_table[i]->interface == interface)) {
			serial = dev2ser(serial_table[i]);
			tty_port_tty_hangup(&serial->port, false);
			mutex_lock(&serial->parent->mutex);
			serial->parent->usb_gone = 1;
			mutex_unlock(&serial->parent->mutex);
			cancel_work_sync(&serial_table[i]->async_put_intf);
			cancel_work_sync(&serial_table[i]->async_get_intf);
			hso_serial_tty_unregister(serial);
			kref_put(&serial->parent->ref, hso_serial_ref_free);
		}
	}

	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] &&
		    (network_table[i]->interface == interface)) {
			struct rfkill *rfk = dev2net(network_table[i])->rfkill;
			/* hso_stop_net_device doesn't stop the net queue since
			 * traffic needs to start it again when suspended */
			netif_stop_queue(dev2net(network_table[i])->net);
			hso_stop_net_device(network_table[i]);
			cancel_work_sync(&network_table[i]->async_put_intf);
			cancel_work_sync(&network_table[i]->async_get_intf);
			if (rfk) {
				rfkill_unregister(rfk);
				rfkill_destroy(rfk);
			}
			hso_free_net_device(network_table[i], false);
		}
	}
}

/* Helper functions */

/* Get the endpoint ! */
static struct usb_endpoint_descriptor *hso_get_ep(struct usb_interface *intf,
						  int type, int dir)
{
	int i;
	struct usb_host_interface *iface = intf->cur_altsetting;
	struct usb_endpoint_descriptor *endp;

	for (i = 0; i < iface->desc.bNumEndpoints; i++) {
		endp = &iface->endpoint[i].desc;
		if (((endp->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == dir) &&
		    (usb_endpoint_type(endp) == type))
			return endp;
	}

	return NULL;
}

/* Get the byte that describes which ports are enabled */
static int hso_get_mux_ports(struct usb_interface *intf, unsigned char *ports)
{
	int i;
	struct usb_host_interface *iface = intf->cur_altsetting;

	if (iface->extralen == 3) {
		*ports = iface->extra[2];
		return 0;
	}

	for (i = 0; i < iface->desc.bNumEndpoints; i++) {
		if (iface->endpoint[i].extralen == 3) {
			*ports = iface->endpoint[i].extra[2];
			return 0;
		}
	}

	return -1;
}

/* interrupt urb needs to be submitted, used for serial read of muxed port */
static int hso_mux_submit_intr_urb(struct hso_shared_int *shared_int,
				   struct usb_device *usb, gfp_t gfp)
{
	int result;

	usb_fill_int_urb(shared_int->shared_intr_urb, usb,
			 usb_rcvintpipe(usb,
				shared_int->intr_endp->bEndpointAddress & 0x7F),
			 shared_int->shared_intr_buf,
			 1,
			 intr_callback, shared_int,
			 shared_int->intr_endp->bInterval);

	result = usb_submit_urb(shared_int->shared_intr_urb, gfp);
	if (result)
		dev_warn(&usb->dev, "%s failed mux_intr_urb %d\n", __func__,
			result);

	return result;
}

/* operations setup of the serial interface */
static const struct tty_operations hso_serial_ops = {
	.open = hso_serial_open,
	.close = hso_serial_close,
	.write = hso_serial_write,
	.write_room = hso_serial_write_room,
	.cleanup = hso_serial_cleanup,
	.ioctl = hso_serial_ioctl,
	.set_termios = hso_serial_set_termios,
	.chars_in_buffer = hso_serial_chars_in_buffer,
	.tiocmget = hso_serial_tiocmget,
	.tiocmset = hso_serial_tiocmset,
	.get_icount = hso_get_count,
	.unthrottle = hso_unthrottle
};

static struct usb_driver hso_driver = {
	.name = driver_name,
	.probe = hso_probe,
	.disconnect = hso_disconnect,
	.id_table = hso_ids,
	.suspend = hso_suspend,
	.resume = hso_resume,
	.reset_resume = hso_resume,
	.supports_autosuspend = 1,
	.disable_hub_initiated_lpm = 1,
};

static int __init hso_init(void)
{
	int i;
	int result;

	/* put it in the log */
	pr_info("%s\n", version);

	/* Initialise the serial table semaphore and table */
	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++)
		serial_table[i] = NULL;

	/* allocate our driver using the proper amount of supported minors */
	tty_drv = alloc_tty_driver(HSO_SERIAL_TTY_MINORS);
	if (!tty_drv)
		return -ENOMEM;

	/* fill in all needed values */
	tty_drv->driver_name = driver_name;
	tty_drv->name = tty_filename;

	/* if major number is provided as parameter, use that one */
	if (tty_major)
		tty_drv->major = tty_major;

	tty_drv->minor_start = 0;
	tty_drv->type = TTY_DRIVER_TYPE_SERIAL;
	tty_drv->subtype = SERIAL_TYPE_NORMAL;
	tty_drv->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
	tty_drv->init_termios = tty_std_termios;
	hso_init_termios(&tty_drv->init_termios);
	tty_set_operations(tty_drv, &hso_serial_ops);

	/* register the tty driver */
	result = tty_register_driver(tty_drv);
	if (result) {
		pr_err("%s - tty_register_driver failed(%d)\n",
		       __func__, result);
		goto err_free_tty;
	}

	/* register this module as an usb driver */
	result = usb_register(&hso_driver);
	if (result) {
		pr_err("Could not register hso driver - error: %d\n", result);
		goto err_unreg_tty;
	}

	/* done */
	return 0;
err_unreg_tty:
	tty_unregister_driver(tty_drv);
err_free_tty:
	put_tty_driver(tty_drv);
	return result;
}

static void __exit hso_exit(void)
{
	pr_info("unloaded\n");

	tty_unregister_driver(tty_drv);
	/* deregister the usb driver */
	usb_deregister(&hso_driver);
	put_tty_driver(tty_drv);
}

/* Module definitions */
module_init(hso_init);
module_exit(hso_exit);

MODULE_AUTHOR(MOD_AUTHOR);
MODULE_DESCRIPTION(MOD_DESCRIPTION);
MODULE_LICENSE("GPL");

/* change the debug level (eg: insmod hso.ko debug=0x04) */
MODULE_PARM_DESC(debug, "debug level mask [0x01 | 0x02 | 0x04 | 0x08 | 0x10]");
module_param(debug, int, 0644);

/* set the major tty number (eg: insmod hso.ko tty_major=245) */
MODULE_PARM_DESC(tty_major, "Set the major tty number");
module_param(tty_major, int, 0644);

/* disable network interface (eg: insmod hso.ko disable_net=1) */
MODULE_PARM_DESC(disable_net, "Disable the network interface");
module_param(disable_net, int, 0644);
