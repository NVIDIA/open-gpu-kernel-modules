// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * NET3:	Fibre Channel device handling subroutines
 *
 *		Vineet Abraham <vma@iol.unh.edu>
 *		v 1.0 03/22/99
 */

#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/fcdevice.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/net.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/export.h>
#include <net/arp.h>

/*
 *	Put the headers on a Fibre Channel packet.
 */

static int fc_header(struct sk_buff *skb, struct net_device *dev,
		     unsigned short type,
		     const void *daddr, const void *saddr, unsigned int len)
{
	struct fch_hdr *fch;
	int hdr_len;

	/*
	 * Add the 802.2 SNAP header if IP as the IPv4 code calls
	 * dev->hard_header directly.
	 */
	if (type == ETH_P_IP || type == ETH_P_ARP)
	{
		struct fcllc *fcllc;

		hdr_len = sizeof(struct fch_hdr) + sizeof(struct fcllc);
		fch = skb_push(skb, hdr_len);
		fcllc = (struct fcllc *)(fch+1);
		fcllc->dsap = fcllc->ssap = EXTENDED_SAP;
		fcllc->llc = UI_CMD;
		fcllc->protid[0] = fcllc->protid[1] = fcllc->protid[2] = 0x00;
		fcllc->ethertype = htons(type);
	}
	else
	{
		hdr_len = sizeof(struct fch_hdr);
		fch = skb_push(skb, hdr_len);
	}

	if(saddr)
		memcpy(fch->saddr,saddr,dev->addr_len);
	else
		memcpy(fch->saddr,dev->dev_addr,dev->addr_len);

	if(daddr)
	{
		memcpy(fch->daddr,daddr,dev->addr_len);
		return hdr_len;
	}
	return -hdr_len;
}

static const struct header_ops fc_header_ops = {
	.create	 = fc_header,
};

static void fc_setup(struct net_device *dev)
{
	dev->header_ops		= &fc_header_ops;
	dev->type		= ARPHRD_IEEE802;
	dev->hard_header_len	= FC_HLEN;
	dev->mtu		= 2024;
	dev->addr_len		= FC_ALEN;
	dev->tx_queue_len	= 100; /* Long queues on fc */
	dev->flags		= IFF_BROADCAST;

	memset(dev->broadcast, 0xFF, FC_ALEN);
}

/**
 * alloc_fcdev - Register fibre channel device
 * @sizeof_priv: Size of additional driver-private structure to be allocated
 *	for this fibre channel device
 *
 * Fill in the fields of the device structure with fibre channel-generic values.
 *
 * Constructs a new net device, complete with a private data area of
 * size @sizeof_priv.  A 32-byte (not bit) alignment is enforced for
 * this private data area.
 */
struct net_device *alloc_fcdev(int sizeof_priv)
{
	return alloc_netdev(sizeof_priv, "fc%d", NET_NAME_UNKNOWN, fc_setup);
}
EXPORT_SYMBOL(alloc_fcdev);
