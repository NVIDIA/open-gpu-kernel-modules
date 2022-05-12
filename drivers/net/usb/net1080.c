// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Net1080 based USB host-to-host cables
 * Copyright (C) 2000-2005 by David Brownell
 */

// #define	DEBUG			// error path messages, extra info
// #define	VERBOSE			// more; success messages

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/workqueue.h>
#include <linux/mii.h>
#include <linux/usb.h>
#include <linux/usb/usbnet.h>
#include <linux/slab.h>

#include <asm/unaligned.h>


/*
 * Netchip 1080 driver ... http://www.netchip.com
 * (Sept 2004:  End-of-life announcement has been sent.)
 * Used in (some) LapLink cables
 */

#define frame_errors	data[1]

/*
 * NetChip framing of ethernet packets, supporting additional error
 * checks for links that may drop bulk packets from inside messages.
 * Odd USB length == always short read for last usb packet.
 *	- nc_header
 *	- Ethernet header (14 bytes)
 *	- payload
 *	- (optional padding byte, if needed so length becomes odd)
 *	- nc_trailer
 *
 * This framing is to be avoided for non-NetChip devices.
 */

struct nc_header {		// packed:
	__le16	hdr_len;		// sizeof nc_header (LE, all)
	__le16	packet_len;		// payload size (including ethhdr)
	__le16	packet_id;		// detects dropped packets
#define MIN_HEADER	6

	// all else is optional, and must start with:
	// __le16	vendorId;	// from usb-if
	// __le16	productId;
} __packed;

#define	PAD_BYTE	((unsigned char)0xAC)

struct nc_trailer {
	__le16	packet_id;
} __packed;

// packets may use FLAG_FRAMING_NC and optional pad
#define FRAMED_SIZE(mtu) (sizeof (struct nc_header) \
				+ sizeof (struct ethhdr) \
				+ (mtu) \
				+ 1 \
				+ sizeof (struct nc_trailer))

#define MIN_FRAMED	FRAMED_SIZE(0)

/* packets _could_ be up to 64KB... */
#define NC_MAX_PACKET	32767


/*
 * Zero means no timeout; else, how long a 64 byte bulk packet may be queued
 * before the hardware drops it.  If that's done, the driver will need to
 * frame network packets to guard against the dropped USB packets.  The win32
 * driver sets this for both sides of the link.
 */
#define	NC_READ_TTL_MS	((u8)255)	// ms

/*
 * We ignore most registers and EEPROM contents.
 */
#define	REG_USBCTL	((u8)0x04)
#define REG_TTL		((u8)0x10)
#define REG_STATUS	((u8)0x11)

/*
 * Vendor specific requests to read/write data
 */
#define	REQUEST_REGISTER	((u8)0x10)
#define	REQUEST_EEPROM		((u8)0x11)

static int
nc_vendor_read(struct usbnet *dev, u8 req, u8 regnum, u16 *retval_ptr)
{
	int status = usbnet_read_cmd(dev, req,
				     USB_DIR_IN | USB_TYPE_VENDOR |
				     USB_RECIP_DEVICE,
				     0, regnum, retval_ptr,
				     sizeof *retval_ptr);
	if (status > 0)
		status = 0;
	if (!status)
		le16_to_cpus(retval_ptr);
	return status;
}

static inline int
nc_register_read(struct usbnet *dev, u8 regnum, u16 *retval_ptr)
{
	return nc_vendor_read(dev, REQUEST_REGISTER, regnum, retval_ptr);
}

static void
nc_vendor_write(struct usbnet *dev, u8 req, u8 regnum, u16 value)
{
	usbnet_write_cmd(dev, req,
			 USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			 value, regnum, NULL, 0);
}

static inline void
nc_register_write(struct usbnet *dev, u8 regnum, u16 value)
{
	nc_vendor_write(dev, REQUEST_REGISTER, regnum, value);
}


#if 0
static void nc_dump_registers(struct usbnet *dev)
{
	u8	reg;
	u16	*vp = kmalloc(sizeof (u16));

	if (!vp)
		return;

	netdev_dbg(dev->net, "registers:\n");
	for (reg = 0; reg < 0x20; reg++) {
		int retval;

		// reading some registers is trouble
		if (reg >= 0x08 && reg <= 0xf)
			continue;
		if (reg >= 0x12 && reg <= 0x1e)
			continue;

		retval = nc_register_read(dev, reg, vp);
		if (retval < 0)
			netdev_dbg(dev->net, "reg [0x%x] ==> error %d\n",
				   reg, retval);
		else
			netdev_dbg(dev->net, "reg [0x%x] = 0x%x\n", reg, *vp);
	}
	kfree(vp);
}
#endif


/*-------------------------------------------------------------------------*/

/*
 * Control register
 */

#define	USBCTL_WRITABLE_MASK	0x1f0f
// bits 15-13 reserved, r/o
#define	USBCTL_ENABLE_LANG	(1 << 12)
#define	USBCTL_ENABLE_MFGR	(1 << 11)
#define	USBCTL_ENABLE_PROD	(1 << 10)
#define	USBCTL_ENABLE_SERIAL	(1 << 9)
#define	USBCTL_ENABLE_DEFAULTS	(1 << 8)
// bits 7-4 reserved, r/o
#define	USBCTL_FLUSH_OTHER	(1 << 3)
#define	USBCTL_FLUSH_THIS	(1 << 2)
#define	USBCTL_DISCONN_OTHER	(1 << 1)
#define	USBCTL_DISCONN_THIS	(1 << 0)

static inline void nc_dump_usbctl(struct usbnet *dev, u16 usbctl)
{
	netif_dbg(dev, link, dev->net,
		  "net1080 %s-%s usbctl 0x%x:%s%s%s%s%s; this%s%s; other%s%s; r/o 0x%x\n",
		  dev->udev->bus->bus_name, dev->udev->devpath,
		  usbctl,
		  (usbctl & USBCTL_ENABLE_LANG) ? " lang" : "",
		  (usbctl & USBCTL_ENABLE_MFGR) ? " mfgr" : "",
		  (usbctl & USBCTL_ENABLE_PROD) ? " prod" : "",
		  (usbctl & USBCTL_ENABLE_SERIAL) ? " serial" : "",
		  (usbctl & USBCTL_ENABLE_DEFAULTS) ? " defaults" : "",

		  (usbctl & USBCTL_FLUSH_THIS) ? " FLUSH" : "",
		  (usbctl & USBCTL_DISCONN_THIS) ? " DIS" : "",

		  (usbctl & USBCTL_FLUSH_OTHER) ? " FLUSH" : "",
		  (usbctl & USBCTL_DISCONN_OTHER) ? " DIS" : "",

		  usbctl & ~USBCTL_WRITABLE_MASK);
}

/*-------------------------------------------------------------------------*/

/*
 * Status register
 */

#define	STATUS_PORT_A		(1 << 15)

#define	STATUS_CONN_OTHER	(1 << 14)
#define	STATUS_SUSPEND_OTHER	(1 << 13)
#define	STATUS_MAILBOX_OTHER	(1 << 12)
#define	STATUS_PACKETS_OTHER(n)	(((n) >> 8) & 0x03)

#define	STATUS_CONN_THIS	(1 << 6)
#define	STATUS_SUSPEND_THIS	(1 << 5)
#define	STATUS_MAILBOX_THIS	(1 << 4)
#define	STATUS_PACKETS_THIS(n)	(((n) >> 0) & 0x03)

#define	STATUS_UNSPEC_MASK	0x0c8c
#define	STATUS_NOISE_MASK 	((u16)~(0x0303|STATUS_UNSPEC_MASK))


static inline void nc_dump_status(struct usbnet *dev, u16 status)
{
	netif_dbg(dev, link, dev->net,
		  "net1080 %s-%s status 0x%x: this (%c) PKT=%d%s%s%s; other PKT=%d%s%s%s; unspec 0x%x\n",
		  dev->udev->bus->bus_name, dev->udev->devpath,
		  status,

		  // XXX the packet counts don't seem right
		  // (1 at reset, not 0); maybe UNSPEC too

		  (status & STATUS_PORT_A) ? 'A' : 'B',
		  STATUS_PACKETS_THIS(status),
		  (status & STATUS_CONN_THIS) ? " CON" : "",
		  (status & STATUS_SUSPEND_THIS) ? " SUS" : "",
		  (status & STATUS_MAILBOX_THIS) ? " MBOX" : "",

		  STATUS_PACKETS_OTHER(status),
		  (status & STATUS_CONN_OTHER) ? " CON" : "",
		  (status & STATUS_SUSPEND_OTHER) ? " SUS" : "",
		  (status & STATUS_MAILBOX_OTHER) ? " MBOX" : "",

		  status & STATUS_UNSPEC_MASK);
}

/*-------------------------------------------------------------------------*/

/*
 * TTL register
 */

#define	TTL_OTHER(ttl)	(0x00ff & (ttl >> 8))
#define MK_TTL(this,other)	((u16)(((other)<<8)|(0x00ff&(this))))

/*-------------------------------------------------------------------------*/

static int net1080_reset(struct usbnet *dev)
{
	u16		usbctl, status, ttl;
	u16		vp;
	int		retval;

	// nc_dump_registers(dev);

	if ((retval = nc_register_read(dev, REG_STATUS, &vp)) < 0) {
		netdev_dbg(dev->net, "can't read %s-%s status: %d\n",
			   dev->udev->bus->bus_name, dev->udev->devpath, retval);
		goto done;
	}
	status = vp;
	nc_dump_status(dev, status);

	if ((retval = nc_register_read(dev, REG_USBCTL, &vp)) < 0) {
		netdev_dbg(dev->net, "can't read USBCTL, %d\n", retval);
		goto done;
	}
	usbctl = vp;
	nc_dump_usbctl(dev, usbctl);

	nc_register_write(dev, REG_USBCTL,
			USBCTL_FLUSH_THIS | USBCTL_FLUSH_OTHER);

	if ((retval = nc_register_read(dev, REG_TTL, &vp)) < 0) {
		netdev_dbg(dev->net, "can't read TTL, %d\n", retval);
		goto done;
	}
	ttl = vp;

	nc_register_write(dev, REG_TTL,
			MK_TTL(NC_READ_TTL_MS, TTL_OTHER(ttl)) );
	netdev_dbg(dev->net, "assigned TTL, %d ms\n", NC_READ_TTL_MS);

	netif_info(dev, link, dev->net, "port %c, peer %sconnected\n",
		   (status & STATUS_PORT_A) ? 'A' : 'B',
		   (status & STATUS_CONN_OTHER) ? "" : "dis");
	retval = 0;

done:
	return retval;
}

static int net1080_check_connect(struct usbnet *dev)
{
	int			retval;
	u16			status;
	u16			vp;

	retval = nc_register_read(dev, REG_STATUS, &vp);
	status = vp;
	if (retval != 0) {
		netdev_dbg(dev->net, "net1080_check_conn read - %d\n", retval);
		return retval;
	}
	if ((status & STATUS_CONN_OTHER) != STATUS_CONN_OTHER)
		return -ENOLINK;
	return 0;
}

static void nc_ensure_sync(struct usbnet *dev)
{
	if (++dev->frame_errors <= 5)
		return;

	if (usbnet_write_cmd_async(dev, REQUEST_REGISTER,
					USB_DIR_OUT | USB_TYPE_VENDOR |
					USB_RECIP_DEVICE,
					USBCTL_FLUSH_THIS |
					USBCTL_FLUSH_OTHER,
					REG_USBCTL, NULL, 0))
		return;

	netif_dbg(dev, rx_err, dev->net,
		  "flush net1080; too many framing errors\n");
	dev->frame_errors = 0;
}

static int net1080_rx_fixup(struct usbnet *dev, struct sk_buff *skb)
{
	struct nc_header	*header;
	struct nc_trailer	*trailer;
	u16			hdr_len, packet_len;

	/* This check is no longer done by usbnet */
	if (skb->len < dev->net->hard_header_len)
		return 0;

	if (!(skb->len & 0x01)) {
		netdev_dbg(dev->net, "rx framesize %d range %d..%d mtu %d\n",
			   skb->len, dev->net->hard_header_len, dev->hard_mtu,
			   dev->net->mtu);
		dev->net->stats.rx_frame_errors++;
		nc_ensure_sync(dev);
		return 0;
	}

	header = (struct nc_header *) skb->data;
	hdr_len = le16_to_cpup(&header->hdr_len);
	packet_len = le16_to_cpup(&header->packet_len);
	if (FRAMED_SIZE(packet_len) > NC_MAX_PACKET) {
		dev->net->stats.rx_frame_errors++;
		netdev_dbg(dev->net, "packet too big, %d\n", packet_len);
		nc_ensure_sync(dev);
		return 0;
	} else if (hdr_len < MIN_HEADER) {
		dev->net->stats.rx_frame_errors++;
		netdev_dbg(dev->net, "header too short, %d\n", hdr_len);
		nc_ensure_sync(dev);
		return 0;
	} else if (hdr_len > MIN_HEADER) {
		// out of band data for us?
		netdev_dbg(dev->net, "header OOB, %d bytes\n", hdr_len - MIN_HEADER);
		nc_ensure_sync(dev);
		// switch (vendor/product ids) { ... }
	}
	skb_pull(skb, hdr_len);

	trailer = (struct nc_trailer *)
		(skb->data + skb->len - sizeof *trailer);
	skb_trim(skb, skb->len - sizeof *trailer);

	if ((packet_len & 0x01) == 0) {
		if (skb->data [packet_len] != PAD_BYTE) {
			dev->net->stats.rx_frame_errors++;
			netdev_dbg(dev->net, "bad pad\n");
			return 0;
		}
		skb_trim(skb, skb->len - 1);
	}
	if (skb->len != packet_len) {
		dev->net->stats.rx_frame_errors++;
		netdev_dbg(dev->net, "bad packet len %d (expected %d)\n",
			   skb->len, packet_len);
		nc_ensure_sync(dev);
		return 0;
	}
	if (header->packet_id != get_unaligned(&trailer->packet_id)) {
		dev->net->stats.rx_fifo_errors++;
		netdev_dbg(dev->net, "(2+ dropped) rx packet_id mismatch 0x%x 0x%x\n",
			   le16_to_cpu(header->packet_id),
			   le16_to_cpu(trailer->packet_id));
		return 0;
	}
#if 0
	netdev_dbg(dev->net, "frame <rx h %d p %d id %d\n", header->hdr_len,
		   header->packet_len, header->packet_id);
#endif
	dev->frame_errors = 0;
	return 1;
}

static struct sk_buff *
net1080_tx_fixup(struct usbnet *dev, struct sk_buff *skb, gfp_t flags)
{
	struct sk_buff		*skb2;
	struct nc_header	*header = NULL;
	struct nc_trailer	*trailer = NULL;
	int			padlen = sizeof (struct nc_trailer);
	int			len = skb->len;

	if (!((len + padlen + sizeof (struct nc_header)) & 0x01))
		padlen++;
	if (!skb_cloned(skb)) {
		int	headroom = skb_headroom(skb);
		int	tailroom = skb_tailroom(skb);

		if (padlen <= tailroom &&
		    sizeof(struct nc_header) <= headroom)
			/* There's enough head and tail room */
			goto encapsulate;

		if ((sizeof (struct nc_header) + padlen) <
				(headroom + tailroom)) {
			/* There's enough total room, so just readjust */
			skb->data = memmove(skb->head
						+ sizeof (struct nc_header),
					    skb->data, skb->len);
			skb_set_tail_pointer(skb, len);
			goto encapsulate;
		}
	}

	/* Create a new skb to use with the correct size */
	skb2 = skb_copy_expand(skb,
				sizeof (struct nc_header),
				padlen,
				flags);
	dev_kfree_skb_any(skb);
	if (!skb2)
		return skb2;
	skb = skb2;

encapsulate:
	/* header first */
	header = skb_push(skb, sizeof *header);
	header->hdr_len = cpu_to_le16(sizeof (*header));
	header->packet_len = cpu_to_le16(len);
	header->packet_id = cpu_to_le16((u16)dev->xid++);

	/* maybe pad; then trailer */
	if (!((skb->len + sizeof *trailer) & 0x01))
		skb_put_u8(skb, PAD_BYTE);
	trailer = skb_put(skb, sizeof *trailer);
	put_unaligned(header->packet_id, &trailer->packet_id);
#if 0
	netdev_dbg(dev->net, "frame >tx h %d p %d id %d\n",
		   header->hdr_len, header->packet_len,
		   header->packet_id);
#endif
	return skb;
}

static int net1080_bind(struct usbnet *dev, struct usb_interface *intf)
{
	unsigned	extra = sizeof (struct nc_header)
				+ 1
				+ sizeof (struct nc_trailer);

	dev->net->hard_header_len += extra;
	dev->rx_urb_size = dev->net->hard_header_len + dev->net->mtu;
	dev->hard_mtu = NC_MAX_PACKET;
	return usbnet_get_endpoints (dev, intf);
}

static const struct driver_info	net1080_info = {
	.description =	"NetChip TurboCONNECT",
	.flags =	FLAG_POINTTOPOINT | FLAG_FRAMING_NC,
	.bind =		net1080_bind,
	.reset =	net1080_reset,
	.check_connect = net1080_check_connect,
	.rx_fixup =	net1080_rx_fixup,
	.tx_fixup =	net1080_tx_fixup,
};

static const struct usb_device_id	products [] = {
{
	USB_DEVICE(0x0525, 0x1080),	// NetChip ref design
	.driver_info =	(unsigned long) &net1080_info,
}, {
	USB_DEVICE(0x06D0, 0x0622),	// Laplink Gold
	.driver_info =	(unsigned long) &net1080_info,
},
	{ },		// END
};
MODULE_DEVICE_TABLE(usb, products);

static struct usb_driver net1080_driver = {
	.name =		"net1080",
	.id_table =	products,
	.probe =	usbnet_probe,
	.disconnect =	usbnet_disconnect,
	.suspend =	usbnet_suspend,
	.resume =	usbnet_resume,
	.disable_hub_initiated_lpm = 1,
};

module_usb_driver(net1080_driver);

MODULE_AUTHOR("David Brownell");
MODULE_DESCRIPTION("NetChip 1080 based USB Host-to-Host Links");
MODULE_LICENSE("GPL");
