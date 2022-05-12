// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2009 Peter Holik
 *
 * Intellon usb PLC (Powerline Communications) usb net driver
 *
 * http://www.tandel.be/downloads/INT51X1_Datasheet.pdf
 *
 * Based on the work of Jan 'RedBully' Seiffert
 */

/*
 */

#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/slab.h>
#include <linux/mii.h>
#include <linux/usb.h>
#include <linux/usb/usbnet.h>

#define INT51X1_VENDOR_ID	0x09e1
#define INT51X1_PRODUCT_ID	0x5121

#define INT51X1_HEADER_SIZE	2	/* 2 byte header */

#define PACKET_TYPE_PROMISCUOUS		(1 << 0)
#define PACKET_TYPE_ALL_MULTICAST	(1 << 1) /* no filter */
#define PACKET_TYPE_DIRECTED		(1 << 2)
#define PACKET_TYPE_BROADCAST		(1 << 3)
#define PACKET_TYPE_MULTICAST		(1 << 4) /* filtered */

#define SET_ETHERNET_PACKET_FILTER	0x43

static int int51x1_rx_fixup(struct usbnet *dev, struct sk_buff *skb)
{
	int len;

	if (!(pskb_may_pull(skb, INT51X1_HEADER_SIZE))) {
		netdev_err(dev->net, "unexpected tiny rx frame\n");
		return 0;
	}

	len = le16_to_cpu(*(__le16 *)&skb->data[skb->len - 2]);

	skb_trim(skb, len);

	return 1;
}

static struct sk_buff *int51x1_tx_fixup(struct usbnet *dev,
		struct sk_buff *skb, gfp_t flags)
{
	int pack_len = skb->len;
	int pack_with_header_len = pack_len + INT51X1_HEADER_SIZE;
	int headroom = skb_headroom(skb);
	int tailroom = skb_tailroom(skb);
	int need_tail = 0;
	__le16 *len;

	/* if packet and our header is smaler than 64 pad to 64 (+ ZLP) */
	if ((pack_with_header_len) < dev->maxpacket)
		need_tail = dev->maxpacket - pack_with_header_len + 1;
	/*
	 * usbnet would send a ZLP if packetlength mod urbsize == 0 for us,
	 * but we need to know ourself, because this would add to the length
	 * we send down to the device...
	 */
	else if (!(pack_with_header_len % dev->maxpacket))
		need_tail = 1;

	if (!skb_cloned(skb) &&
			(headroom + tailroom >= need_tail + INT51X1_HEADER_SIZE)) {
		if (headroom < INT51X1_HEADER_SIZE || tailroom < need_tail) {
			skb->data = memmove(skb->head + INT51X1_HEADER_SIZE,
					skb->data, skb->len);
			skb_set_tail_pointer(skb, skb->len);
		}
	} else {
		struct sk_buff *skb2;

		skb2 = skb_copy_expand(skb,
				INT51X1_HEADER_SIZE,
				need_tail,
				flags);
		dev_kfree_skb_any(skb);
		if (!skb2)
			return NULL;
		skb = skb2;
	}

	pack_len += need_tail;
	pack_len &= 0x07ff;

	len = __skb_push(skb, INT51X1_HEADER_SIZE);
	*len = cpu_to_le16(pack_len);

	if(need_tail)
		__skb_put_zero(skb, need_tail);

	return skb;
}

static void int51x1_set_multicast(struct net_device *netdev)
{
	struct usbnet *dev = netdev_priv(netdev);
	u16 filter = PACKET_TYPE_DIRECTED | PACKET_TYPE_BROADCAST;

	if (netdev->flags & IFF_PROMISC) {
		/* do not expect to see traffic of other PLCs */
		filter |= PACKET_TYPE_PROMISCUOUS;
		netdev_info(dev->net, "promiscuous mode enabled\n");
	} else if (!netdev_mc_empty(netdev) ||
		  (netdev->flags & IFF_ALLMULTI)) {
		filter |= PACKET_TYPE_ALL_MULTICAST;
		netdev_dbg(dev->net, "receive all multicast enabled\n");
	} else {
		/* ~PROMISCUOUS, ~MULTICAST */
		netdev_dbg(dev->net, "receive own packets only\n");
	}

	usbnet_write_cmd_async(dev, SET_ETHERNET_PACKET_FILTER,
			       USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			       filter, 0, NULL, 0);
}

static const struct net_device_ops int51x1_netdev_ops = {
	.ndo_open		= usbnet_open,
	.ndo_stop		= usbnet_stop,
	.ndo_start_xmit		= usbnet_start_xmit,
	.ndo_tx_timeout		= usbnet_tx_timeout,
	.ndo_change_mtu		= usbnet_change_mtu,
	.ndo_get_stats64	= dev_get_tstats64,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_rx_mode	= int51x1_set_multicast,
};

static int int51x1_bind(struct usbnet *dev, struct usb_interface *intf)
{
	int status = usbnet_get_ethernet_addr(dev, 3);

	if (status)
		return status;

	dev->net->hard_header_len += INT51X1_HEADER_SIZE;
	dev->hard_mtu = dev->net->mtu + dev->net->hard_header_len;
	dev->net->netdev_ops = &int51x1_netdev_ops;

	return usbnet_get_endpoints(dev, intf);
}

static const struct driver_info int51x1_info = {
	.description = "Intellon usb powerline adapter",
	.bind        = int51x1_bind,
	.rx_fixup    = int51x1_rx_fixup,
	.tx_fixup    = int51x1_tx_fixup,
	.in          = 1,
	.out         = 2,
	.flags       = FLAG_ETHER,
};

static const struct usb_device_id products[] = {
	{
	USB_DEVICE(INT51X1_VENDOR_ID, INT51X1_PRODUCT_ID),
		.driver_info = (unsigned long) &int51x1_info,
	},
	{},
};
MODULE_DEVICE_TABLE(usb, products);

static struct usb_driver int51x1_driver = {
	.name       = "int51x1",
	.id_table   = products,
	.probe      = usbnet_probe,
	.disconnect = usbnet_disconnect,
	.suspend    = usbnet_suspend,
	.resume     = usbnet_resume,
	.disable_hub_initiated_lpm = 1,
};

module_usb_driver(int51x1_driver);

MODULE_AUTHOR("Peter Holik");
MODULE_DESCRIPTION("Intellon usb powerline adapter");
MODULE_LICENSE("GPL");
