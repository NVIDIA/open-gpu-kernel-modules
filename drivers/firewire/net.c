// SPDX-License-Identifier: GPL-2.0-only
/*
 * IPv4 over IEEE 1394, per RFC 2734
 * IPv6 over IEEE 1394, per RFC 3146
 *
 * Copyright (C) 2009 Jay Fenlason <fenlason@redhat.com>
 *
 * based on eth1394 by Ben Collins et al
 */

#include <linux/bug.h>
#include <linux/compiler.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/ethtool.h>
#include <linux/firewire.h>
#include <linux/firewire-constants.h>
#include <linux/highmem.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/jiffies.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include <asm/unaligned.h>
#include <net/arp.h>
#include <net/firewire.h>

/* rx limits */
#define FWNET_MAX_FRAGMENTS		30 /* arbitrary, > TX queue depth */
#define FWNET_ISO_PAGE_COUNT		(PAGE_SIZE < 16*1024 ? 4 : 2)

/* tx limits */
#define FWNET_MAX_QUEUED_DATAGRAMS	20 /* < 64 = number of tlabels */
#define FWNET_MIN_QUEUED_DATAGRAMS	10 /* should keep AT DMA busy enough */
#define FWNET_TX_QUEUE_LEN		FWNET_MAX_QUEUED_DATAGRAMS /* ? */

#define IEEE1394_BROADCAST_CHANNEL	31
#define IEEE1394_ALL_NODES		(0xffc0 | 0x003f)
#define IEEE1394_MAX_PAYLOAD_S100	512
#define FWNET_NO_FIFO_ADDR		(~0ULL)

#define IANA_SPECIFIER_ID		0x00005eU
#define RFC2734_SW_VERSION		0x000001U
#define RFC3146_SW_VERSION		0x000002U

#define IEEE1394_GASP_HDR_SIZE	8

#define RFC2374_UNFRAG_HDR_SIZE	4
#define RFC2374_FRAG_HDR_SIZE	8
#define RFC2374_FRAG_OVERHEAD	4

#define RFC2374_HDR_UNFRAG	0	/* unfragmented		*/
#define RFC2374_HDR_FIRSTFRAG	1	/* first fragment	*/
#define RFC2374_HDR_LASTFRAG	2	/* last fragment	*/
#define RFC2374_HDR_INTFRAG	3	/* interior fragment	*/

static bool fwnet_hwaddr_is_multicast(u8 *ha)
{
	return !!(*ha & 1);
}

/* IPv4 and IPv6 encapsulation header */
struct rfc2734_header {
	u32 w0;
	u32 w1;
};

#define fwnet_get_hdr_lf(h)		(((h)->w0 & 0xc0000000) >> 30)
#define fwnet_get_hdr_ether_type(h)	(((h)->w0 & 0x0000ffff))
#define fwnet_get_hdr_dg_size(h)	((((h)->w0 & 0x0fff0000) >> 16) + 1)
#define fwnet_get_hdr_fg_off(h)		(((h)->w0 & 0x00000fff))
#define fwnet_get_hdr_dgl(h)		(((h)->w1 & 0xffff0000) >> 16)

#define fwnet_set_hdr_lf(lf)		((lf) << 30)
#define fwnet_set_hdr_ether_type(et)	(et)
#define fwnet_set_hdr_dg_size(dgs)	(((dgs) - 1) << 16)
#define fwnet_set_hdr_fg_off(fgo)	(fgo)

#define fwnet_set_hdr_dgl(dgl)		((dgl) << 16)

static inline void fwnet_make_uf_hdr(struct rfc2734_header *hdr,
		unsigned ether_type)
{
	hdr->w0 = fwnet_set_hdr_lf(RFC2374_HDR_UNFRAG)
		  | fwnet_set_hdr_ether_type(ether_type);
}

static inline void fwnet_make_ff_hdr(struct rfc2734_header *hdr,
		unsigned ether_type, unsigned dg_size, unsigned dgl)
{
	hdr->w0 = fwnet_set_hdr_lf(RFC2374_HDR_FIRSTFRAG)
		  | fwnet_set_hdr_dg_size(dg_size)
		  | fwnet_set_hdr_ether_type(ether_type);
	hdr->w1 = fwnet_set_hdr_dgl(dgl);
}

static inline void fwnet_make_sf_hdr(struct rfc2734_header *hdr,
		unsigned lf, unsigned dg_size, unsigned fg_off, unsigned dgl)
{
	hdr->w0 = fwnet_set_hdr_lf(lf)
		  | fwnet_set_hdr_dg_size(dg_size)
		  | fwnet_set_hdr_fg_off(fg_off);
	hdr->w1 = fwnet_set_hdr_dgl(dgl);
}

/* This list keeps track of what parts of the datagram have been filled in */
struct fwnet_fragment_info {
	struct list_head fi_link;
	u16 offset;
	u16 len;
};

struct fwnet_partial_datagram {
	struct list_head pd_link;
	struct list_head fi_list;
	struct sk_buff *skb;
	/* FIXME Why not use skb->data? */
	char *pbuf;
	u16 datagram_label;
	u16 ether_type;
	u16 datagram_size;
};

static DEFINE_MUTEX(fwnet_device_mutex);
static LIST_HEAD(fwnet_device_list);

struct fwnet_device {
	struct list_head dev_link;
	spinlock_t lock;
	enum {
		FWNET_BROADCAST_ERROR,
		FWNET_BROADCAST_RUNNING,
		FWNET_BROADCAST_STOPPED,
	} broadcast_state;
	struct fw_iso_context *broadcast_rcv_context;
	struct fw_iso_buffer broadcast_rcv_buffer;
	void **broadcast_rcv_buffer_ptrs;
	unsigned broadcast_rcv_next_ptr;
	unsigned num_broadcast_rcv_ptrs;
	unsigned rcv_buffer_size;
	/*
	 * This value is the maximum unfragmented datagram size that can be
	 * sent by the hardware.  It already has the GASP overhead and the
	 * unfragmented datagram header overhead calculated into it.
	 */
	unsigned broadcast_xmt_max_payload;
	u16 broadcast_xmt_datagramlabel;

	/*
	 * The CSR address that remote nodes must send datagrams to for us to
	 * receive them.
	 */
	struct fw_address_handler handler;
	u64 local_fifo;

	/* Number of tx datagrams that have been queued but not yet acked */
	int queued_datagrams;

	int peer_count;
	struct list_head peer_list;
	struct fw_card *card;
	struct net_device *netdev;
};

struct fwnet_peer {
	struct list_head peer_link;
	struct fwnet_device *dev;
	u64 guid;

	/* guarded by dev->lock */
	struct list_head pd_list; /* received partial datagrams */
	unsigned pdg_size;        /* pd_list size */

	u16 datagram_label;       /* outgoing datagram label */
	u16 max_payload;          /* includes RFC2374_FRAG_HDR_SIZE overhead */
	int node_id;
	int generation;
	unsigned speed;
};

/* This is our task struct. It's used for the packet complete callback.  */
struct fwnet_packet_task {
	struct fw_transaction transaction;
	struct rfc2734_header hdr;
	struct sk_buff *skb;
	struct fwnet_device *dev;

	int outstanding_pkts;
	u64 fifo_addr;
	u16 dest_node;
	u16 max_payload;
	u8 generation;
	u8 speed;
	u8 enqueued;
};

/*
 * Get fifo address embedded in hwaddr
 */
static __u64 fwnet_hwaddr_fifo(union fwnet_hwaddr *ha)
{
	return (u64)get_unaligned_be16(&ha->uc.fifo_hi) << 32
	       | get_unaligned_be32(&ha->uc.fifo_lo);
}

/*
 * saddr == NULL means use device source address.
 * daddr == NULL means leave destination address (eg unresolved arp).
 */
static int fwnet_header_create(struct sk_buff *skb, struct net_device *net,
			unsigned short type, const void *daddr,
			const void *saddr, unsigned len)
{
	struct fwnet_header *h;

	h = skb_push(skb, sizeof(*h));
	put_unaligned_be16(type, &h->h_proto);

	if (net->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		memset(h->h_dest, 0, net->addr_len);

		return net->hard_header_len;
	}

	if (daddr) {
		memcpy(h->h_dest, daddr, net->addr_len);

		return net->hard_header_len;
	}

	return -net->hard_header_len;
}

static int fwnet_header_cache(const struct neighbour *neigh,
			      struct hh_cache *hh, __be16 type)
{
	struct net_device *net;
	struct fwnet_header *h;

	if (type == cpu_to_be16(ETH_P_802_3))
		return -1;
	net = neigh->dev;
	h = (struct fwnet_header *)((u8 *)hh->hh_data + HH_DATA_OFF(sizeof(*h)));
	h->h_proto = type;
	memcpy(h->h_dest, neigh->ha, net->addr_len);

	/* Pairs with the READ_ONCE() in neigh_resolve_output(),
	 * neigh_hh_output() and neigh_update_hhs().
	 */
	smp_store_release(&hh->hh_len, FWNET_HLEN);

	return 0;
}

/* Called by Address Resolution module to notify changes in address. */
static void fwnet_header_cache_update(struct hh_cache *hh,
		const struct net_device *net, const unsigned char *haddr)
{
	memcpy((u8 *)hh->hh_data + HH_DATA_OFF(FWNET_HLEN), haddr, net->addr_len);
}

static int fwnet_header_parse(const struct sk_buff *skb, unsigned char *haddr)
{
	memcpy(haddr, skb->dev->dev_addr, FWNET_ALEN);

	return FWNET_ALEN;
}

static const struct header_ops fwnet_header_ops = {
	.create         = fwnet_header_create,
	.cache		= fwnet_header_cache,
	.cache_update	= fwnet_header_cache_update,
	.parse          = fwnet_header_parse,
};

/* FIXME: is this correct for all cases? */
static bool fwnet_frag_overlap(struct fwnet_partial_datagram *pd,
			       unsigned offset, unsigned len)
{
	struct fwnet_fragment_info *fi;
	unsigned end = offset + len;

	list_for_each_entry(fi, &pd->fi_list, fi_link)
		if (offset < fi->offset + fi->len && end > fi->offset)
			return true;

	return false;
}

/* Assumes that new fragment does not overlap any existing fragments */
static struct fwnet_fragment_info *fwnet_frag_new(
	struct fwnet_partial_datagram *pd, unsigned offset, unsigned len)
{
	struct fwnet_fragment_info *fi, *fi2, *new;
	struct list_head *list;

	list = &pd->fi_list;
	list_for_each_entry(fi, &pd->fi_list, fi_link) {
		if (fi->offset + fi->len == offset) {
			/* The new fragment can be tacked on to the end */
			/* Did the new fragment plug a hole? */
			fi2 = list_entry(fi->fi_link.next,
					 struct fwnet_fragment_info, fi_link);
			if (fi->offset + fi->len == fi2->offset) {
				/* glue fragments together */
				fi->len += len + fi2->len;
				list_del(&fi2->fi_link);
				kfree(fi2);
			} else {
				fi->len += len;
			}

			return fi;
		}
		if (offset + len == fi->offset) {
			/* The new fragment can be tacked on to the beginning */
			/* Did the new fragment plug a hole? */
			fi2 = list_entry(fi->fi_link.prev,
					 struct fwnet_fragment_info, fi_link);
			if (fi2->offset + fi2->len == fi->offset) {
				/* glue fragments together */
				fi2->len += fi->len + len;
				list_del(&fi->fi_link);
				kfree(fi);

				return fi2;
			}
			fi->offset = offset;
			fi->len += len;

			return fi;
		}
		if (offset > fi->offset + fi->len) {
			list = &fi->fi_link;
			break;
		}
		if (offset + len < fi->offset) {
			list = fi->fi_link.prev;
			break;
		}
	}

	new = kmalloc(sizeof(*new), GFP_ATOMIC);
	if (!new)
		return NULL;

	new->offset = offset;
	new->len = len;
	list_add(&new->fi_link, list);

	return new;
}

static struct fwnet_partial_datagram *fwnet_pd_new(struct net_device *net,
		struct fwnet_peer *peer, u16 datagram_label, unsigned dg_size,
		void *frag_buf, unsigned frag_off, unsigned frag_len)
{
	struct fwnet_partial_datagram *new;
	struct fwnet_fragment_info *fi;

	new = kmalloc(sizeof(*new), GFP_ATOMIC);
	if (!new)
		goto fail;

	INIT_LIST_HEAD(&new->fi_list);
	fi = fwnet_frag_new(new, frag_off, frag_len);
	if (fi == NULL)
		goto fail_w_new;

	new->datagram_label = datagram_label;
	new->datagram_size = dg_size;
	new->skb = dev_alloc_skb(dg_size + LL_RESERVED_SPACE(net));
	if (new->skb == NULL)
		goto fail_w_fi;

	skb_reserve(new->skb, LL_RESERVED_SPACE(net));
	new->pbuf = skb_put(new->skb, dg_size);
	memcpy(new->pbuf + frag_off, frag_buf, frag_len);
	list_add_tail(&new->pd_link, &peer->pd_list);

	return new;

fail_w_fi:
	kfree(fi);
fail_w_new:
	kfree(new);
fail:
	return NULL;
}

static struct fwnet_partial_datagram *fwnet_pd_find(struct fwnet_peer *peer,
						    u16 datagram_label)
{
	struct fwnet_partial_datagram *pd;

	list_for_each_entry(pd, &peer->pd_list, pd_link)
		if (pd->datagram_label == datagram_label)
			return pd;

	return NULL;
}


static void fwnet_pd_delete(struct fwnet_partial_datagram *old)
{
	struct fwnet_fragment_info *fi, *n;

	list_for_each_entry_safe(fi, n, &old->fi_list, fi_link)
		kfree(fi);

	list_del(&old->pd_link);
	dev_kfree_skb_any(old->skb);
	kfree(old);
}

static bool fwnet_pd_update(struct fwnet_peer *peer,
		struct fwnet_partial_datagram *pd, void *frag_buf,
		unsigned frag_off, unsigned frag_len)
{
	if (fwnet_frag_new(pd, frag_off, frag_len) == NULL)
		return false;

	memcpy(pd->pbuf + frag_off, frag_buf, frag_len);

	/*
	 * Move list entry to beginning of list so that oldest partial
	 * datagrams percolate to the end of the list
	 */
	list_move_tail(&pd->pd_link, &peer->pd_list);

	return true;
}

static bool fwnet_pd_is_complete(struct fwnet_partial_datagram *pd)
{
	struct fwnet_fragment_info *fi;

	fi = list_entry(pd->fi_list.next, struct fwnet_fragment_info, fi_link);

	return fi->len == pd->datagram_size;
}

/* caller must hold dev->lock */
static struct fwnet_peer *fwnet_peer_find_by_guid(struct fwnet_device *dev,
						  u64 guid)
{
	struct fwnet_peer *peer;

	list_for_each_entry(peer, &dev->peer_list, peer_link)
		if (peer->guid == guid)
			return peer;

	return NULL;
}

/* caller must hold dev->lock */
static struct fwnet_peer *fwnet_peer_find_by_node_id(struct fwnet_device *dev,
						int node_id, int generation)
{
	struct fwnet_peer *peer;

	list_for_each_entry(peer, &dev->peer_list, peer_link)
		if (peer->node_id    == node_id &&
		    peer->generation == generation)
			return peer;

	return NULL;
}

/* See IEEE 1394-2008 table 6-4, table 8-8, table 16-18. */
static unsigned fwnet_max_payload(unsigned max_rec, unsigned speed)
{
	max_rec = min(max_rec, speed + 8);
	max_rec = clamp(max_rec, 8U, 11U); /* 512...4096 */

	return (1 << (max_rec + 1)) - RFC2374_FRAG_HDR_SIZE;
}


static int fwnet_finish_incoming_packet(struct net_device *net,
					struct sk_buff *skb, u16 source_node_id,
					bool is_broadcast, u16 ether_type)
{
	struct fwnet_device *dev;
	int status;
	__be64 guid;

	switch (ether_type) {
	case ETH_P_ARP:
	case ETH_P_IP:
#if IS_ENABLED(CONFIG_IPV6)
	case ETH_P_IPV6:
#endif
		break;
	default:
		goto err;
	}

	dev = netdev_priv(net);
	/* Write metadata, and then pass to the receive level */
	skb->dev = net;
	skb->ip_summed = CHECKSUM_NONE;

	/*
	 * Parse the encapsulation header. This actually does the job of
	 * converting to an ethernet-like pseudo frame header.
	 */
	guid = cpu_to_be64(dev->card->guid);
	if (dev_hard_header(skb, net, ether_type,
			   is_broadcast ? net->broadcast : net->dev_addr,
			   NULL, skb->len) >= 0) {
		struct fwnet_header *eth;
		u16 *rawp;
		__be16 protocol;

		skb_reset_mac_header(skb);
		skb_pull(skb, sizeof(*eth));
		eth = (struct fwnet_header *)skb_mac_header(skb);
		if (fwnet_hwaddr_is_multicast(eth->h_dest)) {
			if (memcmp(eth->h_dest, net->broadcast,
				   net->addr_len) == 0)
				skb->pkt_type = PACKET_BROADCAST;
#if 0
			else
				skb->pkt_type = PACKET_MULTICAST;
#endif
		} else {
			if (memcmp(eth->h_dest, net->dev_addr, net->addr_len))
				skb->pkt_type = PACKET_OTHERHOST;
		}
		if (ntohs(eth->h_proto) >= ETH_P_802_3_MIN) {
			protocol = eth->h_proto;
		} else {
			rawp = (u16 *)skb->data;
			if (*rawp == 0xffff)
				protocol = htons(ETH_P_802_3);
			else
				protocol = htons(ETH_P_802_2);
		}
		skb->protocol = protocol;
	}
	status = netif_rx(skb);
	if (status == NET_RX_DROP) {
		net->stats.rx_errors++;
		net->stats.rx_dropped++;
	} else {
		net->stats.rx_packets++;
		net->stats.rx_bytes += skb->len;
	}

	return 0;

 err:
	net->stats.rx_errors++;
	net->stats.rx_dropped++;

	dev_kfree_skb_any(skb);

	return -ENOENT;
}

static int fwnet_incoming_packet(struct fwnet_device *dev, __be32 *buf, int len,
				 int source_node_id, int generation,
				 bool is_broadcast)
{
	struct sk_buff *skb;
	struct net_device *net = dev->netdev;
	struct rfc2734_header hdr;
	unsigned lf;
	unsigned long flags;
	struct fwnet_peer *peer;
	struct fwnet_partial_datagram *pd;
	int fg_off;
	int dg_size;
	u16 datagram_label;
	int retval;
	u16 ether_type;

	if (len <= RFC2374_UNFRAG_HDR_SIZE)
		return 0;

	hdr.w0 = be32_to_cpu(buf[0]);
	lf = fwnet_get_hdr_lf(&hdr);
	if (lf == RFC2374_HDR_UNFRAG) {
		/*
		 * An unfragmented datagram has been received by the ieee1394
		 * bus. Build an skbuff around it so we can pass it to the
		 * high level network layer.
		 */
		ether_type = fwnet_get_hdr_ether_type(&hdr);
		buf++;
		len -= RFC2374_UNFRAG_HDR_SIZE;

		skb = dev_alloc_skb(len + LL_RESERVED_SPACE(net));
		if (unlikely(!skb)) {
			net->stats.rx_dropped++;

			return -ENOMEM;
		}
		skb_reserve(skb, LL_RESERVED_SPACE(net));
		skb_put_data(skb, buf, len);

		return fwnet_finish_incoming_packet(net, skb, source_node_id,
						    is_broadcast, ether_type);
	}

	/* A datagram fragment has been received, now the fun begins. */

	if (len <= RFC2374_FRAG_HDR_SIZE)
		return 0;

	hdr.w1 = ntohl(buf[1]);
	buf += 2;
	len -= RFC2374_FRAG_HDR_SIZE;
	if (lf == RFC2374_HDR_FIRSTFRAG) {
		ether_type = fwnet_get_hdr_ether_type(&hdr);
		fg_off = 0;
	} else {
		ether_type = 0;
		fg_off = fwnet_get_hdr_fg_off(&hdr);
	}
	datagram_label = fwnet_get_hdr_dgl(&hdr);
	dg_size = fwnet_get_hdr_dg_size(&hdr);

	if (fg_off + len > dg_size)
		return 0;

	spin_lock_irqsave(&dev->lock, flags);

	peer = fwnet_peer_find_by_node_id(dev, source_node_id, generation);
	if (!peer) {
		retval = -ENOENT;
		goto fail;
	}

	pd = fwnet_pd_find(peer, datagram_label);
	if (pd == NULL) {
		while (peer->pdg_size >= FWNET_MAX_FRAGMENTS) {
			/* remove the oldest */
			fwnet_pd_delete(list_first_entry(&peer->pd_list,
				struct fwnet_partial_datagram, pd_link));
			peer->pdg_size--;
		}
		pd = fwnet_pd_new(net, peer, datagram_label,
				  dg_size, buf, fg_off, len);
		if (pd == NULL) {
			retval = -ENOMEM;
			goto fail;
		}
		peer->pdg_size++;
	} else {
		if (fwnet_frag_overlap(pd, fg_off, len) ||
		    pd->datagram_size != dg_size) {
			/*
			 * Differing datagram sizes or overlapping fragments,
			 * discard old datagram and start a new one.
			 */
			fwnet_pd_delete(pd);
			pd = fwnet_pd_new(net, peer, datagram_label,
					  dg_size, buf, fg_off, len);
			if (pd == NULL) {
				peer->pdg_size--;
				retval = -ENOMEM;
				goto fail;
			}
		} else {
			if (!fwnet_pd_update(peer, pd, buf, fg_off, len)) {
				/*
				 * Couldn't save off fragment anyway
				 * so might as well obliterate the
				 * datagram now.
				 */
				fwnet_pd_delete(pd);
				peer->pdg_size--;
				retval = -ENOMEM;
				goto fail;
			}
		}
	} /* new datagram or add to existing one */

	if (lf == RFC2374_HDR_FIRSTFRAG)
		pd->ether_type = ether_type;

	if (fwnet_pd_is_complete(pd)) {
		ether_type = pd->ether_type;
		peer->pdg_size--;
		skb = skb_get(pd->skb);
		fwnet_pd_delete(pd);

		spin_unlock_irqrestore(&dev->lock, flags);

		return fwnet_finish_incoming_packet(net, skb, source_node_id,
						    false, ether_type);
	}
	/*
	 * Datagram is not complete, we're done for the
	 * moment.
	 */
	retval = 0;
 fail:
	spin_unlock_irqrestore(&dev->lock, flags);

	return retval;
}

static void fwnet_receive_packet(struct fw_card *card, struct fw_request *r,
		int tcode, int destination, int source, int generation,
		unsigned long long offset, void *payload, size_t length,
		void *callback_data)
{
	struct fwnet_device *dev = callback_data;
	int rcode;

	if (destination == IEEE1394_ALL_NODES) {
		kfree(r);

		return;
	}

	if (offset != dev->handler.offset)
		rcode = RCODE_ADDRESS_ERROR;
	else if (tcode != TCODE_WRITE_BLOCK_REQUEST)
		rcode = RCODE_TYPE_ERROR;
	else if (fwnet_incoming_packet(dev, payload, length,
				       source, generation, false) != 0) {
		dev_err(&dev->netdev->dev, "incoming packet failure\n");
		rcode = RCODE_CONFLICT_ERROR;
	} else
		rcode = RCODE_COMPLETE;

	fw_send_response(card, r, rcode);
}

static int gasp_source_id(__be32 *p)
{
	return be32_to_cpu(p[0]) >> 16;
}

static u32 gasp_specifier_id(__be32 *p)
{
	return (be32_to_cpu(p[0]) & 0xffff) << 8 |
	       (be32_to_cpu(p[1]) & 0xff000000) >> 24;
}

static u32 gasp_version(__be32 *p)
{
	return be32_to_cpu(p[1]) & 0xffffff;
}

static void fwnet_receive_broadcast(struct fw_iso_context *context,
		u32 cycle, size_t header_length, void *header, void *data)
{
	struct fwnet_device *dev;
	struct fw_iso_packet packet;
	__be16 *hdr_ptr;
	__be32 *buf_ptr;
	int retval;
	u32 length;
	unsigned long offset;
	unsigned long flags;

	dev = data;
	hdr_ptr = header;
	length = be16_to_cpup(hdr_ptr);

	spin_lock_irqsave(&dev->lock, flags);

	offset = dev->rcv_buffer_size * dev->broadcast_rcv_next_ptr;
	buf_ptr = dev->broadcast_rcv_buffer_ptrs[dev->broadcast_rcv_next_ptr++];
	if (dev->broadcast_rcv_next_ptr == dev->num_broadcast_rcv_ptrs)
		dev->broadcast_rcv_next_ptr = 0;

	spin_unlock_irqrestore(&dev->lock, flags);

	if (length > IEEE1394_GASP_HDR_SIZE &&
	    gasp_specifier_id(buf_ptr) == IANA_SPECIFIER_ID &&
	    (gasp_version(buf_ptr) == RFC2734_SW_VERSION
#if IS_ENABLED(CONFIG_IPV6)
	     || gasp_version(buf_ptr) == RFC3146_SW_VERSION
#endif
	    ))
		fwnet_incoming_packet(dev, buf_ptr + 2,
				      length - IEEE1394_GASP_HDR_SIZE,
				      gasp_source_id(buf_ptr),
				      context->card->generation, true);

	packet.payload_length = dev->rcv_buffer_size;
	packet.interrupt = 1;
	packet.skip = 0;
	packet.tag = 3;
	packet.sy = 0;
	packet.header_length = IEEE1394_GASP_HDR_SIZE;

	spin_lock_irqsave(&dev->lock, flags);

	retval = fw_iso_context_queue(dev->broadcast_rcv_context, &packet,
				      &dev->broadcast_rcv_buffer, offset);

	spin_unlock_irqrestore(&dev->lock, flags);

	if (retval >= 0)
		fw_iso_context_queue_flush(dev->broadcast_rcv_context);
	else
		dev_err(&dev->netdev->dev, "requeue failed\n");
}

static struct kmem_cache *fwnet_packet_task_cache;

static void fwnet_free_ptask(struct fwnet_packet_task *ptask)
{
	dev_kfree_skb_any(ptask->skb);
	kmem_cache_free(fwnet_packet_task_cache, ptask);
}

/* Caller must hold dev->lock. */
static void dec_queued_datagrams(struct fwnet_device *dev)
{
	if (--dev->queued_datagrams == FWNET_MIN_QUEUED_DATAGRAMS)
		netif_wake_queue(dev->netdev);
}

static int fwnet_send_packet(struct fwnet_packet_task *ptask);

static void fwnet_transmit_packet_done(struct fwnet_packet_task *ptask)
{
	struct fwnet_device *dev = ptask->dev;
	struct sk_buff *skb = ptask->skb;
	unsigned long flags;
	bool free;

	spin_lock_irqsave(&dev->lock, flags);

	ptask->outstanding_pkts--;

	/* Check whether we or the networking TX soft-IRQ is last user. */
	free = (ptask->outstanding_pkts == 0 && ptask->enqueued);
	if (free)
		dec_queued_datagrams(dev);

	if (ptask->outstanding_pkts == 0) {
		dev->netdev->stats.tx_packets++;
		dev->netdev->stats.tx_bytes += skb->len;
	}

	spin_unlock_irqrestore(&dev->lock, flags);

	if (ptask->outstanding_pkts > 0) {
		u16 dg_size;
		u16 fg_off;
		u16 datagram_label;
		u16 lf;

		/* Update the ptask to point to the next fragment and send it */
		lf = fwnet_get_hdr_lf(&ptask->hdr);
		switch (lf) {
		case RFC2374_HDR_LASTFRAG:
		case RFC2374_HDR_UNFRAG:
		default:
			dev_err(&dev->netdev->dev,
				"outstanding packet %x lf %x, header %x,%x\n",
				ptask->outstanding_pkts, lf, ptask->hdr.w0,
				ptask->hdr.w1);
			BUG();

		case RFC2374_HDR_FIRSTFRAG:
			/* Set frag type here for future interior fragments */
			dg_size = fwnet_get_hdr_dg_size(&ptask->hdr);
			fg_off = ptask->max_payload - RFC2374_FRAG_HDR_SIZE;
			datagram_label = fwnet_get_hdr_dgl(&ptask->hdr);
			break;

		case RFC2374_HDR_INTFRAG:
			dg_size = fwnet_get_hdr_dg_size(&ptask->hdr);
			fg_off = fwnet_get_hdr_fg_off(&ptask->hdr)
				  + ptask->max_payload - RFC2374_FRAG_HDR_SIZE;
			datagram_label = fwnet_get_hdr_dgl(&ptask->hdr);
			break;
		}

		if (ptask->dest_node == IEEE1394_ALL_NODES) {
			skb_pull(skb,
				 ptask->max_payload + IEEE1394_GASP_HDR_SIZE);
		} else {
			skb_pull(skb, ptask->max_payload);
		}
		if (ptask->outstanding_pkts > 1) {
			fwnet_make_sf_hdr(&ptask->hdr, RFC2374_HDR_INTFRAG,
					  dg_size, fg_off, datagram_label);
		} else {
			fwnet_make_sf_hdr(&ptask->hdr, RFC2374_HDR_LASTFRAG,
					  dg_size, fg_off, datagram_label);
			ptask->max_payload = skb->len + RFC2374_FRAG_HDR_SIZE;
		}
		fwnet_send_packet(ptask);
	}

	if (free)
		fwnet_free_ptask(ptask);
}

static void fwnet_transmit_packet_failed(struct fwnet_packet_task *ptask)
{
	struct fwnet_device *dev = ptask->dev;
	unsigned long flags;
	bool free;

	spin_lock_irqsave(&dev->lock, flags);

	/* One fragment failed; don't try to send remaining fragments. */
	ptask->outstanding_pkts = 0;

	/* Check whether we or the networking TX soft-IRQ is last user. */
	free = ptask->enqueued;
	if (free)
		dec_queued_datagrams(dev);

	dev->netdev->stats.tx_dropped++;
	dev->netdev->stats.tx_errors++;

	spin_unlock_irqrestore(&dev->lock, flags);

	if (free)
		fwnet_free_ptask(ptask);
}

static void fwnet_write_complete(struct fw_card *card, int rcode,
				 void *payload, size_t length, void *data)
{
	struct fwnet_packet_task *ptask = data;
	static unsigned long j;
	static int last_rcode, errors_skipped;

	if (rcode == RCODE_COMPLETE) {
		fwnet_transmit_packet_done(ptask);
	} else {
		if (printk_timed_ratelimit(&j,  1000) || rcode != last_rcode) {
			dev_err(&ptask->dev->netdev->dev,
				"fwnet_write_complete failed: %x (skipped %d)\n",
				rcode, errors_skipped);

			errors_skipped = 0;
			last_rcode = rcode;
		} else {
			errors_skipped++;
		}
		fwnet_transmit_packet_failed(ptask);
	}
}

static int fwnet_send_packet(struct fwnet_packet_task *ptask)
{
	struct fwnet_device *dev;
	unsigned tx_len;
	struct rfc2734_header *bufhdr;
	unsigned long flags;
	bool free;

	dev = ptask->dev;
	tx_len = ptask->max_payload;
	switch (fwnet_get_hdr_lf(&ptask->hdr)) {
	case RFC2374_HDR_UNFRAG:
		bufhdr = skb_push(ptask->skb, RFC2374_UNFRAG_HDR_SIZE);
		put_unaligned_be32(ptask->hdr.w0, &bufhdr->w0);
		break;

	case RFC2374_HDR_FIRSTFRAG:
	case RFC2374_HDR_INTFRAG:
	case RFC2374_HDR_LASTFRAG:
		bufhdr = skb_push(ptask->skb, RFC2374_FRAG_HDR_SIZE);
		put_unaligned_be32(ptask->hdr.w0, &bufhdr->w0);
		put_unaligned_be32(ptask->hdr.w1, &bufhdr->w1);
		break;

	default:
		BUG();
	}
	if (ptask->dest_node == IEEE1394_ALL_NODES) {
		u8 *p;
		int generation;
		int node_id;
		unsigned int sw_version;

		/* ptask->generation may not have been set yet */
		generation = dev->card->generation;
		smp_rmb();
		node_id = dev->card->node_id;

		switch (ptask->skb->protocol) {
		default:
			sw_version = RFC2734_SW_VERSION;
			break;
#if IS_ENABLED(CONFIG_IPV6)
		case htons(ETH_P_IPV6):
			sw_version = RFC3146_SW_VERSION;
#endif
		}

		p = skb_push(ptask->skb, IEEE1394_GASP_HDR_SIZE);
		put_unaligned_be32(node_id << 16 | IANA_SPECIFIER_ID >> 8, p);
		put_unaligned_be32((IANA_SPECIFIER_ID & 0xff) << 24
						| sw_version, &p[4]);

		/* We should not transmit if broadcast_channel.valid == 0. */
		fw_send_request(dev->card, &ptask->transaction,
				TCODE_STREAM_DATA,
				fw_stream_packet_destination_id(3,
						IEEE1394_BROADCAST_CHANNEL, 0),
				generation, SCODE_100, 0ULL, ptask->skb->data,
				tx_len + 8, fwnet_write_complete, ptask);

		spin_lock_irqsave(&dev->lock, flags);

		/* If the AT tasklet already ran, we may be last user. */
		free = (ptask->outstanding_pkts == 0 && !ptask->enqueued);
		if (!free)
			ptask->enqueued = true;
		else
			dec_queued_datagrams(dev);

		spin_unlock_irqrestore(&dev->lock, flags);

		goto out;
	}

	fw_send_request(dev->card, &ptask->transaction,
			TCODE_WRITE_BLOCK_REQUEST, ptask->dest_node,
			ptask->generation, ptask->speed, ptask->fifo_addr,
			ptask->skb->data, tx_len, fwnet_write_complete, ptask);

	spin_lock_irqsave(&dev->lock, flags);

	/* If the AT tasklet already ran, we may be last user. */
	free = (ptask->outstanding_pkts == 0 && !ptask->enqueued);
	if (!free)
		ptask->enqueued = true;
	else
		dec_queued_datagrams(dev);

	spin_unlock_irqrestore(&dev->lock, flags);

	netif_trans_update(dev->netdev);
 out:
	if (free)
		fwnet_free_ptask(ptask);

	return 0;
}

static void fwnet_fifo_stop(struct fwnet_device *dev)
{
	if (dev->local_fifo == FWNET_NO_FIFO_ADDR)
		return;

	fw_core_remove_address_handler(&dev->handler);
	dev->local_fifo = FWNET_NO_FIFO_ADDR;
}

static int fwnet_fifo_start(struct fwnet_device *dev)
{
	int retval;

	if (dev->local_fifo != FWNET_NO_FIFO_ADDR)
		return 0;

	dev->handler.length = 4096;
	dev->handler.address_callback = fwnet_receive_packet;
	dev->handler.callback_data = dev;

	retval = fw_core_add_address_handler(&dev->handler,
					     &fw_high_memory_region);
	if (retval < 0)
		return retval;

	dev->local_fifo = dev->handler.offset;

	return 0;
}

static void __fwnet_broadcast_stop(struct fwnet_device *dev)
{
	unsigned u;

	if (dev->broadcast_state != FWNET_BROADCAST_ERROR) {
		for (u = 0; u < FWNET_ISO_PAGE_COUNT; u++)
			kunmap(dev->broadcast_rcv_buffer.pages[u]);
		fw_iso_buffer_destroy(&dev->broadcast_rcv_buffer, dev->card);
	}
	if (dev->broadcast_rcv_context) {
		fw_iso_context_destroy(dev->broadcast_rcv_context);
		dev->broadcast_rcv_context = NULL;
	}
	kfree(dev->broadcast_rcv_buffer_ptrs);
	dev->broadcast_rcv_buffer_ptrs = NULL;
	dev->broadcast_state = FWNET_BROADCAST_ERROR;
}

static void fwnet_broadcast_stop(struct fwnet_device *dev)
{
	if (dev->broadcast_state == FWNET_BROADCAST_ERROR)
		return;
	fw_iso_context_stop(dev->broadcast_rcv_context);
	__fwnet_broadcast_stop(dev);
}

static int fwnet_broadcast_start(struct fwnet_device *dev)
{
	struct fw_iso_context *context;
	int retval;
	unsigned num_packets;
	unsigned max_receive;
	struct fw_iso_packet packet;
	unsigned long offset;
	void **ptrptr;
	unsigned u;

	if (dev->broadcast_state != FWNET_BROADCAST_ERROR)
		return 0;

	max_receive = 1U << (dev->card->max_receive + 1);
	num_packets = (FWNET_ISO_PAGE_COUNT * PAGE_SIZE) / max_receive;

	ptrptr = kmalloc_array(num_packets, sizeof(void *), GFP_KERNEL);
	if (!ptrptr) {
		retval = -ENOMEM;
		goto failed;
	}
	dev->broadcast_rcv_buffer_ptrs = ptrptr;

	context = fw_iso_context_create(dev->card, FW_ISO_CONTEXT_RECEIVE,
					IEEE1394_BROADCAST_CHANNEL,
					dev->card->link_speed, 8,
					fwnet_receive_broadcast, dev);
	if (IS_ERR(context)) {
		retval = PTR_ERR(context);
		goto failed;
	}

	retval = fw_iso_buffer_init(&dev->broadcast_rcv_buffer, dev->card,
				    FWNET_ISO_PAGE_COUNT, DMA_FROM_DEVICE);
	if (retval < 0)
		goto failed;

	dev->broadcast_state = FWNET_BROADCAST_STOPPED;

	for (u = 0; u < FWNET_ISO_PAGE_COUNT; u++) {
		void *ptr;
		unsigned v;

		ptr = kmap(dev->broadcast_rcv_buffer.pages[u]);
		for (v = 0; v < num_packets / FWNET_ISO_PAGE_COUNT; v++)
			*ptrptr++ = (void *) ((char *)ptr + v * max_receive);
	}
	dev->broadcast_rcv_context = context;

	packet.payload_length = max_receive;
	packet.interrupt = 1;
	packet.skip = 0;
	packet.tag = 3;
	packet.sy = 0;
	packet.header_length = IEEE1394_GASP_HDR_SIZE;
	offset = 0;

	for (u = 0; u < num_packets; u++) {
		retval = fw_iso_context_queue(context, &packet,
				&dev->broadcast_rcv_buffer, offset);
		if (retval < 0)
			goto failed;

		offset += max_receive;
	}
	dev->num_broadcast_rcv_ptrs = num_packets;
	dev->rcv_buffer_size = max_receive;
	dev->broadcast_rcv_next_ptr = 0U;
	retval = fw_iso_context_start(context, -1, 0,
			FW_ISO_CONTEXT_MATCH_ALL_TAGS); /* ??? sync */
	if (retval < 0)
		goto failed;

	/* FIXME: adjust it according to the min. speed of all known peers? */
	dev->broadcast_xmt_max_payload = IEEE1394_MAX_PAYLOAD_S100
			- IEEE1394_GASP_HDR_SIZE - RFC2374_UNFRAG_HDR_SIZE;
	dev->broadcast_state = FWNET_BROADCAST_RUNNING;

	return 0;

 failed:
	__fwnet_broadcast_stop(dev);
	return retval;
}

static void set_carrier_state(struct fwnet_device *dev)
{
	if (dev->peer_count > 1)
		netif_carrier_on(dev->netdev);
	else
		netif_carrier_off(dev->netdev);
}

/* ifup */
static int fwnet_open(struct net_device *net)
{
	struct fwnet_device *dev = netdev_priv(net);
	int ret;

	ret = fwnet_broadcast_start(dev);
	if (ret)
		return ret;

	netif_start_queue(net);

	spin_lock_irq(&dev->lock);
	set_carrier_state(dev);
	spin_unlock_irq(&dev->lock);

	return 0;
}

/* ifdown */
static int fwnet_stop(struct net_device *net)
{
	struct fwnet_device *dev = netdev_priv(net);

	netif_stop_queue(net);
	fwnet_broadcast_stop(dev);

	return 0;
}

static netdev_tx_t fwnet_tx(struct sk_buff *skb, struct net_device *net)
{
	struct fwnet_header hdr_buf;
	struct fwnet_device *dev = netdev_priv(net);
	__be16 proto;
	u16 dest_node;
	unsigned max_payload;
	u16 dg_size;
	u16 *datagram_label_ptr;
	struct fwnet_packet_task *ptask;
	struct fwnet_peer *peer;
	unsigned long flags;

	spin_lock_irqsave(&dev->lock, flags);

	/* Can this happen? */
	if (netif_queue_stopped(dev->netdev)) {
		spin_unlock_irqrestore(&dev->lock, flags);

		return NETDEV_TX_BUSY;
	}

	ptask = kmem_cache_alloc(fwnet_packet_task_cache, GFP_ATOMIC);
	if (ptask == NULL)
		goto fail;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		goto fail;

	/*
	 * Make a copy of the driver-specific header.
	 * We might need to rebuild the header on tx failure.
	 */
	memcpy(&hdr_buf, skb->data, sizeof(hdr_buf));
	proto = hdr_buf.h_proto;

	switch (proto) {
	case htons(ETH_P_ARP):
	case htons(ETH_P_IP):
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
#endif
		break;
	default:
		goto fail;
	}

	skb_pull(skb, sizeof(hdr_buf));
	dg_size = skb->len;

	/*
	 * Set the transmission type for the packet.  ARP packets and IP
	 * broadcast packets are sent via GASP.
	 */
	if (fwnet_hwaddr_is_multicast(hdr_buf.h_dest)) {
		max_payload        = dev->broadcast_xmt_max_payload;
		datagram_label_ptr = &dev->broadcast_xmt_datagramlabel;

		ptask->fifo_addr   = FWNET_NO_FIFO_ADDR;
		ptask->generation  = 0;
		ptask->dest_node   = IEEE1394_ALL_NODES;
		ptask->speed       = SCODE_100;
	} else {
		union fwnet_hwaddr *ha = (union fwnet_hwaddr *)hdr_buf.h_dest;
		__be64 guid = get_unaligned(&ha->uc.uniq_id);
		u8 generation;

		peer = fwnet_peer_find_by_guid(dev, be64_to_cpu(guid));
		if (!peer)
			goto fail;

		generation         = peer->generation;
		dest_node          = peer->node_id;
		max_payload        = peer->max_payload;
		datagram_label_ptr = &peer->datagram_label;

		ptask->fifo_addr   = fwnet_hwaddr_fifo(ha);
		ptask->generation  = generation;
		ptask->dest_node   = dest_node;
		ptask->speed       = peer->speed;
	}

	ptask->hdr.w0 = 0;
	ptask->hdr.w1 = 0;
	ptask->skb = skb;
	ptask->dev = dev;

	/* Does it all fit in one packet? */
	if (dg_size <= max_payload) {
		fwnet_make_uf_hdr(&ptask->hdr, ntohs(proto));
		ptask->outstanding_pkts = 1;
		max_payload = dg_size + RFC2374_UNFRAG_HDR_SIZE;
	} else {
		u16 datagram_label;

		max_payload -= RFC2374_FRAG_OVERHEAD;
		datagram_label = (*datagram_label_ptr)++;
		fwnet_make_ff_hdr(&ptask->hdr, ntohs(proto), dg_size,
				  datagram_label);
		ptask->outstanding_pkts = DIV_ROUND_UP(dg_size, max_payload);
		max_payload += RFC2374_FRAG_HDR_SIZE;
	}

	if (++dev->queued_datagrams == FWNET_MAX_QUEUED_DATAGRAMS)
		netif_stop_queue(dev->netdev);

	spin_unlock_irqrestore(&dev->lock, flags);

	ptask->max_payload = max_payload;
	ptask->enqueued    = 0;

	fwnet_send_packet(ptask);

	return NETDEV_TX_OK;

 fail:
	spin_unlock_irqrestore(&dev->lock, flags);

	if (ptask)
		kmem_cache_free(fwnet_packet_task_cache, ptask);

	if (skb != NULL)
		dev_kfree_skb(skb);

	net->stats.tx_dropped++;
	net->stats.tx_errors++;

	/*
	 * FIXME: According to a patch from 2003-02-26, "returning non-zero
	 * causes serious problems" here, allegedly.  Before that patch,
	 * -ERRNO was returned which is not appropriate under Linux 2.6.
	 * Perhaps more needs to be done?  Stop the queue in serious
	 * conditions and restart it elsewhere?
	 */
	return NETDEV_TX_OK;
}

static const struct ethtool_ops fwnet_ethtool_ops = {
	.get_link	= ethtool_op_get_link,
};

static const struct net_device_ops fwnet_netdev_ops = {
	.ndo_open       = fwnet_open,
	.ndo_stop	= fwnet_stop,
	.ndo_start_xmit = fwnet_tx,
};

static void fwnet_init_dev(struct net_device *net)
{
	net->header_ops		= &fwnet_header_ops;
	net->netdev_ops		= &fwnet_netdev_ops;
	net->watchdog_timeo	= 2 * HZ;
	net->flags		= IFF_BROADCAST | IFF_MULTICAST;
	net->features		= NETIF_F_HIGHDMA;
	net->addr_len		= FWNET_ALEN;
	net->hard_header_len	= FWNET_HLEN;
	net->type		= ARPHRD_IEEE1394;
	net->tx_queue_len	= FWNET_TX_QUEUE_LEN;
	net->ethtool_ops	= &fwnet_ethtool_ops;
}

/* caller must hold fwnet_device_mutex */
static struct fwnet_device *fwnet_dev_find(struct fw_card *card)
{
	struct fwnet_device *dev;

	list_for_each_entry(dev, &fwnet_device_list, dev_link)
		if (dev->card == card)
			return dev;

	return NULL;
}

static int fwnet_add_peer(struct fwnet_device *dev,
			  struct fw_unit *unit, struct fw_device *device)
{
	struct fwnet_peer *peer;

	peer = kmalloc(sizeof(*peer), GFP_KERNEL);
	if (!peer)
		return -ENOMEM;

	dev_set_drvdata(&unit->device, peer);

	peer->dev = dev;
	peer->guid = (u64)device->config_rom[3] << 32 | device->config_rom[4];
	INIT_LIST_HEAD(&peer->pd_list);
	peer->pdg_size = 0;
	peer->datagram_label = 0;
	peer->speed = device->max_speed;
	peer->max_payload = fwnet_max_payload(device->max_rec, peer->speed);

	peer->generation = device->generation;
	smp_rmb();
	peer->node_id = device->node_id;

	spin_lock_irq(&dev->lock);
	list_add_tail(&peer->peer_link, &dev->peer_list);
	dev->peer_count++;
	set_carrier_state(dev);
	spin_unlock_irq(&dev->lock);

	return 0;
}

static int fwnet_probe(struct fw_unit *unit,
		       const struct ieee1394_device_id *id)
{
	struct fw_device *device = fw_parent_device(unit);
	struct fw_card *card = device->card;
	struct net_device *net;
	bool allocated_netdev = false;
	struct fwnet_device *dev;
	int ret;
	union fwnet_hwaddr *ha;

	mutex_lock(&fwnet_device_mutex);

	dev = fwnet_dev_find(card);
	if (dev) {
		net = dev->netdev;
		goto have_dev;
	}

	net = alloc_netdev(sizeof(*dev), "firewire%d", NET_NAME_UNKNOWN,
			   fwnet_init_dev);
	if (net == NULL) {
		mutex_unlock(&fwnet_device_mutex);
		return -ENOMEM;
	}

	allocated_netdev = true;
	SET_NETDEV_DEV(net, card->device);
	dev = netdev_priv(net);

	spin_lock_init(&dev->lock);
	dev->broadcast_state = FWNET_BROADCAST_ERROR;
	dev->broadcast_rcv_context = NULL;
	dev->broadcast_xmt_max_payload = 0;
	dev->broadcast_xmt_datagramlabel = 0;
	dev->local_fifo = FWNET_NO_FIFO_ADDR;
	dev->queued_datagrams = 0;
	INIT_LIST_HEAD(&dev->peer_list);
	dev->card = card;
	dev->netdev = net;

	ret = fwnet_fifo_start(dev);
	if (ret < 0)
		goto out;
	dev->local_fifo = dev->handler.offset;

	/*
	 * default MTU: RFC 2734 cl. 4, RFC 3146 cl. 4
	 * maximum MTU: RFC 2734 cl. 4.2, fragment encapsulation header's
	 *              maximum possible datagram_size + 1 = 0xfff + 1
	 */
	net->mtu = 1500U;
	net->min_mtu = ETH_MIN_MTU;
	net->max_mtu = 4096U;

	/* Set our hardware address while we're at it */
	ha = (union fwnet_hwaddr *)net->dev_addr;
	put_unaligned_be64(card->guid, &ha->uc.uniq_id);
	ha->uc.max_rec = dev->card->max_receive;
	ha->uc.sspd = dev->card->link_speed;
	put_unaligned_be16(dev->local_fifo >> 32, &ha->uc.fifo_hi);
	put_unaligned_be32(dev->local_fifo & 0xffffffff, &ha->uc.fifo_lo);

	memset(net->broadcast, -1, net->addr_len);

	ret = register_netdev(net);
	if (ret)
		goto out;

	list_add_tail(&dev->dev_link, &fwnet_device_list);
	dev_notice(&net->dev, "IP over IEEE 1394 on card %s\n",
		   dev_name(card->device));
 have_dev:
	ret = fwnet_add_peer(dev, unit, device);
	if (ret && allocated_netdev) {
		unregister_netdev(net);
		list_del(&dev->dev_link);
 out:
		fwnet_fifo_stop(dev);
		free_netdev(net);
	}

	mutex_unlock(&fwnet_device_mutex);

	return ret;
}

/*
 * FIXME abort partially sent fragmented datagrams,
 * discard partially received fragmented datagrams
 */
static void fwnet_update(struct fw_unit *unit)
{
	struct fw_device *device = fw_parent_device(unit);
	struct fwnet_peer *peer = dev_get_drvdata(&unit->device);
	int generation;

	generation = device->generation;

	spin_lock_irq(&peer->dev->lock);
	peer->node_id    = device->node_id;
	peer->generation = generation;
	spin_unlock_irq(&peer->dev->lock);
}

static void fwnet_remove_peer(struct fwnet_peer *peer, struct fwnet_device *dev)
{
	struct fwnet_partial_datagram *pd, *pd_next;

	spin_lock_irq(&dev->lock);
	list_del(&peer->peer_link);
	dev->peer_count--;
	set_carrier_state(dev);
	spin_unlock_irq(&dev->lock);

	list_for_each_entry_safe(pd, pd_next, &peer->pd_list, pd_link)
		fwnet_pd_delete(pd);

	kfree(peer);
}

static void fwnet_remove(struct fw_unit *unit)
{
	struct fwnet_peer *peer = dev_get_drvdata(&unit->device);
	struct fwnet_device *dev = peer->dev;
	struct net_device *net;
	int i;

	mutex_lock(&fwnet_device_mutex);

	net = dev->netdev;

	fwnet_remove_peer(peer, dev);

	if (list_empty(&dev->peer_list)) {
		unregister_netdev(net);

		fwnet_fifo_stop(dev);

		for (i = 0; dev->queued_datagrams && i < 5; i++)
			ssleep(1);
		WARN_ON(dev->queued_datagrams);
		list_del(&dev->dev_link);

		free_netdev(net);
	}

	mutex_unlock(&fwnet_device_mutex);
}

static const struct ieee1394_device_id fwnet_id_table[] = {
	{
		.match_flags  = IEEE1394_MATCH_SPECIFIER_ID |
				IEEE1394_MATCH_VERSION,
		.specifier_id = IANA_SPECIFIER_ID,
		.version      = RFC2734_SW_VERSION,
	},
#if IS_ENABLED(CONFIG_IPV6)
	{
		.match_flags  = IEEE1394_MATCH_SPECIFIER_ID |
				IEEE1394_MATCH_VERSION,
		.specifier_id = IANA_SPECIFIER_ID,
		.version      = RFC3146_SW_VERSION,
	},
#endif
	{ }
};

static struct fw_driver fwnet_driver = {
	.driver = {
		.owner  = THIS_MODULE,
		.name   = KBUILD_MODNAME,
		.bus    = &fw_bus_type,
	},
	.probe    = fwnet_probe,
	.update   = fwnet_update,
	.remove   = fwnet_remove,
	.id_table = fwnet_id_table,
};

static const u32 rfc2374_unit_directory_data[] = {
	0x00040000,	/* directory_length		*/
	0x1200005e,	/* unit_specifier_id: IANA	*/
	0x81000003,	/* textual descriptor offset	*/
	0x13000001,	/* unit_sw_version: RFC 2734	*/
	0x81000005,	/* textual descriptor offset	*/
	0x00030000,	/* descriptor_length		*/
	0x00000000,	/* text				*/
	0x00000000,	/* minimal ASCII, en		*/
	0x49414e41,	/* I A N A			*/
	0x00030000,	/* descriptor_length		*/
	0x00000000,	/* text				*/
	0x00000000,	/* minimal ASCII, en		*/
	0x49507634,	/* I P v 4			*/
};

static struct fw_descriptor rfc2374_unit_directory = {
	.length = ARRAY_SIZE(rfc2374_unit_directory_data),
	.key    = (CSR_DIRECTORY | CSR_UNIT) << 24,
	.data   = rfc2374_unit_directory_data
};

#if IS_ENABLED(CONFIG_IPV6)
static const u32 rfc3146_unit_directory_data[] = {
	0x00040000,	/* directory_length		*/
	0x1200005e,	/* unit_specifier_id: IANA	*/
	0x81000003,	/* textual descriptor offset	*/
	0x13000002,	/* unit_sw_version: RFC 3146	*/
	0x81000005,	/* textual descriptor offset	*/
	0x00030000,	/* descriptor_length		*/
	0x00000000,	/* text				*/
	0x00000000,	/* minimal ASCII, en		*/
	0x49414e41,	/* I A N A			*/
	0x00030000,	/* descriptor_length		*/
	0x00000000,	/* text				*/
	0x00000000,	/* minimal ASCII, en		*/
	0x49507636,	/* I P v 6			*/
};

static struct fw_descriptor rfc3146_unit_directory = {
	.length = ARRAY_SIZE(rfc3146_unit_directory_data),
	.key    = (CSR_DIRECTORY | CSR_UNIT) << 24,
	.data   = rfc3146_unit_directory_data
};
#endif

static int __init fwnet_init(void)
{
	int err;

	err = fw_core_add_descriptor(&rfc2374_unit_directory);
	if (err)
		return err;

#if IS_ENABLED(CONFIG_IPV6)
	err = fw_core_add_descriptor(&rfc3146_unit_directory);
	if (err)
		goto out;
#endif

	fwnet_packet_task_cache = kmem_cache_create("packet_task",
			sizeof(struct fwnet_packet_task), 0, 0, NULL);
	if (!fwnet_packet_task_cache) {
		err = -ENOMEM;
		goto out2;
	}

	err = driver_register(&fwnet_driver.driver);
	if (!err)
		return 0;

	kmem_cache_destroy(fwnet_packet_task_cache);
out2:
#if IS_ENABLED(CONFIG_IPV6)
	fw_core_remove_descriptor(&rfc3146_unit_directory);
out:
#endif
	fw_core_remove_descriptor(&rfc2374_unit_directory);

	return err;
}
module_init(fwnet_init);

static void __exit fwnet_cleanup(void)
{
	driver_unregister(&fwnet_driver.driver);
	kmem_cache_destroy(fwnet_packet_task_cache);
#if IS_ENABLED(CONFIG_IPV6)
	fw_core_remove_descriptor(&rfc3146_unit_directory);
#endif
	fw_core_remove_descriptor(&rfc2374_unit_directory);
}
module_exit(fwnet_cleanup);

MODULE_AUTHOR("Jay Fenlason <fenlason@redhat.com>");
MODULE_DESCRIPTION("IP over IEEE1394 as per RFC 2734/3146");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(ieee1394, fwnet_id_table);
