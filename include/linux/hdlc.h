/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Generic HDLC support routines for Linux
 *
 * Copyright (C) 1999-2005 Krzysztof Halasa <khc@pm.waw.pl>
 */
#ifndef __HDLC_H
#define __HDLC_H


#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/hdlc/ioctl.h>
#include <uapi/linux/hdlc.h>

/* This structure is a private property of HDLC protocols.
   Hardware drivers have no interest here */

struct hdlc_proto {
	int (*open)(struct net_device *dev);
	void (*close)(struct net_device *dev);
	void (*start)(struct net_device *dev); /* if open & DCD */
	void (*stop)(struct net_device *dev); /* if open & !DCD */
	void (*detach)(struct net_device *dev);
	int (*ioctl)(struct net_device *dev, struct ifreq *ifr);
	__be16 (*type_trans)(struct sk_buff *skb, struct net_device *dev);
	int (*netif_rx)(struct sk_buff *skb);
	netdev_tx_t (*xmit)(struct sk_buff *skb, struct net_device *dev);
	struct module *module;
	struct hdlc_proto *next; /* next protocol in the list */
};


/* Pointed to by netdev_priv(dev) */
typedef struct hdlc_device {
	/* used by HDLC layer to take control over HDLC device from hw driver*/
	int (*attach)(struct net_device *dev,
		      unsigned short encoding, unsigned short parity);

	/* hardware driver must handle this instead of dev->hard_start_xmit */
	netdev_tx_t (*xmit)(struct sk_buff *skb, struct net_device *dev);

	/* Things below are for HDLC layer internal use only */
	const struct hdlc_proto *proto;
	int carrier;
	int open;
	spinlock_t state_lock;
	void *state;
	void *priv;
} hdlc_device;



/* Exported from hdlc module */

/* Called by hardware driver when a user requests HDLC service */
int hdlc_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);

/* Must be used by hardware driver on module startup/exit */
#define register_hdlc_device(dev)	register_netdev(dev)
void unregister_hdlc_device(struct net_device *dev);


void register_hdlc_protocol(struct hdlc_proto *proto);
void unregister_hdlc_protocol(struct hdlc_proto *proto);

struct net_device *alloc_hdlcdev(void *priv);

static inline struct hdlc_device* dev_to_hdlc(struct net_device *dev)
{
	return netdev_priv(dev);
}

static __inline__ void debug_frame(const struct sk_buff *skb)
{
	int i;

	for (i=0; i < skb->len; i++) {
		if (i == 100) {
			printk("...\n");
			return;
		}
		printk(" %02X", skb->data[i]);
	}
	printk("\n");
}


/* Must be called by hardware driver when HDLC device is being opened */
int hdlc_open(struct net_device *dev);
/* Must be called by hardware driver when HDLC device is being closed */
void hdlc_close(struct net_device *dev);
/* Must be pointed to by hw driver's dev->netdev_ops->ndo_start_xmit */
netdev_tx_t hdlc_start_xmit(struct sk_buff *skb, struct net_device *dev);

int attach_hdlc_protocol(struct net_device *dev, struct hdlc_proto *proto,
			 size_t size);
/* May be used by hardware driver to gain control over HDLC device */
int detach_hdlc_protocol(struct net_device *dev);

static __inline__ __be16 hdlc_type_trans(struct sk_buff *skb,
					 struct net_device *dev)
{
	hdlc_device *hdlc = dev_to_hdlc(dev);

	skb->dev = dev;
	skb_reset_mac_header(skb);

	if (hdlc->proto->type_trans)
		return hdlc->proto->type_trans(skb, dev);
	else
		return htons(ETH_P_HDLC);
}

#endif /* __HDLC_H */
