// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) B.A.T.M.A.N. contributors:
 *
 * Marek Lindner, Simon Wunderlich
 */

#include "bat_iv_ogm.h"
#include "main.h"

#include <linux/atomic.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/bug.h>
#include <linux/byteorder/generic.h>
#include <linux/cache.h>
#include <linux/errno.h>
#include <linux/etherdevice.h>
#include <linux/gfp.h>
#include <linux/if_ether.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/lockdep.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/pkt_sched.h>
#include <linux/prandom.h>
#include <linux/printk.h>
#include <linux/random.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <net/genetlink.h>
#include <net/netlink.h>
#include <uapi/linux/batadv_packet.h>
#include <uapi/linux/batman_adv.h>

#include "bat_algo.h"
#include "bitarray.h"
#include "gateway_client.h"
#include "hard-interface.h"
#include "hash.h"
#include "log.h"
#include "netlink.h"
#include "network-coding.h"
#include "originator.h"
#include "routing.h"
#include "send.h"
#include "translation-table.h"
#include "tvlv.h"

static void batadv_iv_send_outstanding_bat_ogm_packet(struct work_struct *work);

/**
 * enum batadv_dup_status - duplicate status
 */
enum batadv_dup_status {
	/** @BATADV_NO_DUP: the packet is no duplicate */
	BATADV_NO_DUP = 0,

	/**
	 * @BATADV_ORIG_DUP: OGM is a duplicate in the originator (but not for
	 *  the neighbor)
	 */
	BATADV_ORIG_DUP,

	/** @BATADV_NEIGH_DUP: OGM is a duplicate for the neighbor */
	BATADV_NEIGH_DUP,

	/**
	 * @BATADV_PROTECTED: originator is currently protected (after reboot)
	 */
	BATADV_PROTECTED,
};

/**
 * batadv_ring_buffer_set() - update the ring buffer with the given value
 * @lq_recv: pointer to the ring buffer
 * @lq_index: index to store the value at
 * @value: value to store in the ring buffer
 */
static void batadv_ring_buffer_set(u8 lq_recv[], u8 *lq_index, u8 value)
{
	lq_recv[*lq_index] = value;
	*lq_index = (*lq_index + 1) % BATADV_TQ_GLOBAL_WINDOW_SIZE;
}

/**
 * batadv_ring_buffer_avg() - compute the average of all non-zero values stored
 * in the given ring buffer
 * @lq_recv: pointer to the ring buffer
 *
 * Return: computed average value.
 */
static u8 batadv_ring_buffer_avg(const u8 lq_recv[])
{
	const u8 *ptr;
	u16 count = 0;
	u16 i = 0;
	u16 sum = 0;

	ptr = lq_recv;

	while (i < BATADV_TQ_GLOBAL_WINDOW_SIZE) {
		if (*ptr != 0) {
			count++;
			sum += *ptr;
		}

		i++;
		ptr++;
	}

	if (count == 0)
		return 0;

	return (u8)(sum / count);
}

/**
 * batadv_iv_ogm_orig_get() - retrieve or create (if does not exist) an
 *  originator
 * @bat_priv: the bat priv with all the soft interface information
 * @addr: mac address of the originator
 *
 * Return: the originator object corresponding to the passed mac address or NULL
 * on failure.
 * If the object does not exist, it is created and initialised.
 */
static struct batadv_orig_node *
batadv_iv_ogm_orig_get(struct batadv_priv *bat_priv, const u8 *addr)
{
	struct batadv_orig_node *orig_node;
	int hash_added;

	orig_node = batadv_orig_hash_find(bat_priv, addr);
	if (orig_node)
		return orig_node;

	orig_node = batadv_orig_node_new(bat_priv, addr);
	if (!orig_node)
		return NULL;

	spin_lock_init(&orig_node->bat_iv.ogm_cnt_lock);

	kref_get(&orig_node->refcount);
	hash_added = batadv_hash_add(bat_priv->orig_hash, batadv_compare_orig,
				     batadv_choose_orig, orig_node,
				     &orig_node->hash_entry);
	if (hash_added != 0)
		goto free_orig_node_hash;

	return orig_node;

free_orig_node_hash:
	/* reference for batadv_hash_add */
	batadv_orig_node_put(orig_node);
	/* reference from batadv_orig_node_new */
	batadv_orig_node_put(orig_node);

	return NULL;
}

static struct batadv_neigh_node *
batadv_iv_ogm_neigh_new(struct batadv_hard_iface *hard_iface,
			const u8 *neigh_addr,
			struct batadv_orig_node *orig_node,
			struct batadv_orig_node *orig_neigh)
{
	struct batadv_neigh_node *neigh_node;

	neigh_node = batadv_neigh_node_get_or_create(orig_node,
						     hard_iface, neigh_addr);
	if (!neigh_node)
		goto out;

	neigh_node->orig_node = orig_neigh;

out:
	return neigh_node;
}

static int batadv_iv_ogm_iface_enable(struct batadv_hard_iface *hard_iface)
{
	struct batadv_ogm_packet *batadv_ogm_packet;
	unsigned char *ogm_buff;
	u32 random_seqno;

	mutex_lock(&hard_iface->bat_iv.ogm_buff_mutex);

	/* randomize initial seqno to avoid collision */
	get_random_bytes(&random_seqno, sizeof(random_seqno));
	atomic_set(&hard_iface->bat_iv.ogm_seqno, random_seqno);

	hard_iface->bat_iv.ogm_buff_len = BATADV_OGM_HLEN;
	ogm_buff = kmalloc(hard_iface->bat_iv.ogm_buff_len, GFP_ATOMIC);
	if (!ogm_buff) {
		mutex_unlock(&hard_iface->bat_iv.ogm_buff_mutex);
		return -ENOMEM;
	}

	hard_iface->bat_iv.ogm_buff = ogm_buff;

	batadv_ogm_packet = (struct batadv_ogm_packet *)ogm_buff;
	batadv_ogm_packet->packet_type = BATADV_IV_OGM;
	batadv_ogm_packet->version = BATADV_COMPAT_VERSION;
	batadv_ogm_packet->ttl = 2;
	batadv_ogm_packet->flags = BATADV_NO_FLAGS;
	batadv_ogm_packet->reserved = 0;
	batadv_ogm_packet->tq = BATADV_TQ_MAX_VALUE;

	mutex_unlock(&hard_iface->bat_iv.ogm_buff_mutex);

	return 0;
}

static void batadv_iv_ogm_iface_disable(struct batadv_hard_iface *hard_iface)
{
	mutex_lock(&hard_iface->bat_iv.ogm_buff_mutex);

	kfree(hard_iface->bat_iv.ogm_buff);
	hard_iface->bat_iv.ogm_buff = NULL;

	mutex_unlock(&hard_iface->bat_iv.ogm_buff_mutex);
}

static void batadv_iv_ogm_iface_update_mac(struct batadv_hard_iface *hard_iface)
{
	struct batadv_ogm_packet *batadv_ogm_packet;
	void *ogm_buff;

	mutex_lock(&hard_iface->bat_iv.ogm_buff_mutex);

	ogm_buff = hard_iface->bat_iv.ogm_buff;
	if (!ogm_buff)
		goto unlock;

	batadv_ogm_packet = ogm_buff;
	ether_addr_copy(batadv_ogm_packet->orig,
			hard_iface->net_dev->dev_addr);
	ether_addr_copy(batadv_ogm_packet->prev_sender,
			hard_iface->net_dev->dev_addr);

unlock:
	mutex_unlock(&hard_iface->bat_iv.ogm_buff_mutex);
}

static void
batadv_iv_ogm_primary_iface_set(struct batadv_hard_iface *hard_iface)
{
	struct batadv_ogm_packet *batadv_ogm_packet;
	void *ogm_buff;

	mutex_lock(&hard_iface->bat_iv.ogm_buff_mutex);

	ogm_buff = hard_iface->bat_iv.ogm_buff;
	if (!ogm_buff)
		goto unlock;

	batadv_ogm_packet = ogm_buff;
	batadv_ogm_packet->ttl = BATADV_TTL;

unlock:
	mutex_unlock(&hard_iface->bat_iv.ogm_buff_mutex);
}

/* when do we schedule our own ogm to be sent */
static unsigned long
batadv_iv_ogm_emit_send_time(const struct batadv_priv *bat_priv)
{
	unsigned int msecs;

	msecs = atomic_read(&bat_priv->orig_interval) - BATADV_JITTER;
	msecs += prandom_u32_max(2 * BATADV_JITTER);

	return jiffies + msecs_to_jiffies(msecs);
}

/* when do we schedule a ogm packet to be sent */
static unsigned long batadv_iv_ogm_fwd_send_time(void)
{
	return jiffies + msecs_to_jiffies(prandom_u32_max(BATADV_JITTER / 2));
}

/* apply hop penalty for a normal link */
static u8 batadv_hop_penalty(u8 tq, const struct batadv_priv *bat_priv)
{
	int hop_penalty = atomic_read(&bat_priv->hop_penalty);
	int new_tq;

	new_tq = tq * (BATADV_TQ_MAX_VALUE - hop_penalty);
	new_tq /= BATADV_TQ_MAX_VALUE;

	return new_tq;
}

/**
 * batadv_iv_ogm_aggr_packet() - checks if there is another OGM attached
 * @buff_pos: current position in the skb
 * @packet_len: total length of the skb
 * @ogm_packet: potential OGM in buffer
 *
 * Return: true if there is enough space for another OGM, false otherwise.
 */
static bool
batadv_iv_ogm_aggr_packet(int buff_pos, int packet_len,
			  const struct batadv_ogm_packet *ogm_packet)
{
	int next_buff_pos = 0;

	/* check if there is enough space for the header */
	next_buff_pos += buff_pos + sizeof(*ogm_packet);
	if (next_buff_pos > packet_len)
		return false;

	/* check if there is enough space for the optional TVLV */
	next_buff_pos += ntohs(ogm_packet->tvlv_len);

	return (next_buff_pos <= packet_len) &&
	       (next_buff_pos <= BATADV_MAX_AGGREGATION_BYTES);
}

/* send a batman ogm to a given interface */
static void batadv_iv_ogm_send_to_if(struct batadv_forw_packet *forw_packet,
				     struct batadv_hard_iface *hard_iface)
{
	struct batadv_priv *bat_priv = netdev_priv(hard_iface->soft_iface);
	const char *fwd_str;
	u8 packet_num;
	s16 buff_pos;
	struct batadv_ogm_packet *batadv_ogm_packet;
	struct sk_buff *skb;
	u8 *packet_pos;

	if (hard_iface->if_status != BATADV_IF_ACTIVE)
		return;

	packet_num = 0;
	buff_pos = 0;
	packet_pos = forw_packet->skb->data;
	batadv_ogm_packet = (struct batadv_ogm_packet *)packet_pos;

	/* adjust all flags and log packets */
	while (batadv_iv_ogm_aggr_packet(buff_pos, forw_packet->packet_len,
					 batadv_ogm_packet)) {
		/* we might have aggregated direct link packets with an
		 * ordinary base packet
		 */
		if (forw_packet->direct_link_flags & BIT(packet_num) &&
		    forw_packet->if_incoming == hard_iface)
			batadv_ogm_packet->flags |= BATADV_DIRECTLINK;
		else
			batadv_ogm_packet->flags &= ~BATADV_DIRECTLINK;

		if (packet_num > 0 || !forw_packet->own)
			fwd_str = "Forwarding";
		else
			fwd_str = "Sending own";

		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "%s %spacket (originator %pM, seqno %u, TQ %d, TTL %d, IDF %s) on interface %s [%pM]\n",
			   fwd_str, (packet_num > 0 ? "aggregated " : ""),
			   batadv_ogm_packet->orig,
			   ntohl(batadv_ogm_packet->seqno),
			   batadv_ogm_packet->tq, batadv_ogm_packet->ttl,
			   ((batadv_ogm_packet->flags & BATADV_DIRECTLINK) ?
			    "on" : "off"),
			   hard_iface->net_dev->name,
			   hard_iface->net_dev->dev_addr);

		buff_pos += BATADV_OGM_HLEN;
		buff_pos += ntohs(batadv_ogm_packet->tvlv_len);
		packet_num++;
		packet_pos = forw_packet->skb->data + buff_pos;
		batadv_ogm_packet = (struct batadv_ogm_packet *)packet_pos;
	}

	/* create clone because function is called more than once */
	skb = skb_clone(forw_packet->skb, GFP_ATOMIC);
	if (skb) {
		batadv_inc_counter(bat_priv, BATADV_CNT_MGMT_TX);
		batadv_add_counter(bat_priv, BATADV_CNT_MGMT_TX_BYTES,
				   skb->len + ETH_HLEN);
		batadv_send_broadcast_skb(skb, hard_iface);
	}
}

/* send a batman ogm packet */
static void batadv_iv_ogm_emit(struct batadv_forw_packet *forw_packet)
{
	struct net_device *soft_iface;

	if (!forw_packet->if_incoming) {
		pr_err("Error - can't forward packet: incoming iface not specified\n");
		return;
	}

	soft_iface = forw_packet->if_incoming->soft_iface;

	if (WARN_ON(!forw_packet->if_outgoing))
		return;

	if (forw_packet->if_outgoing->soft_iface != soft_iface) {
		pr_warn("%s: soft interface switch for queued OGM\n", __func__);
		return;
	}

	if (forw_packet->if_incoming->if_status != BATADV_IF_ACTIVE)
		return;

	/* only for one specific outgoing interface */
	batadv_iv_ogm_send_to_if(forw_packet, forw_packet->if_outgoing);
}

/**
 * batadv_iv_ogm_can_aggregate() - find out if an OGM can be aggregated on an
 *  existing forward packet
 * @new_bat_ogm_packet: OGM packet to be aggregated
 * @bat_priv: the bat priv with all the soft interface information
 * @packet_len: (total) length of the OGM
 * @send_time: timestamp (jiffies) when the packet is to be sent
 * @directlink: true if this is a direct link packet
 * @if_incoming: interface where the packet was received
 * @if_outgoing: interface for which the retransmission should be considered
 * @forw_packet: the forwarded packet which should be checked
 *
 * Return: true if new_packet can be aggregated with forw_packet
 */
static bool
batadv_iv_ogm_can_aggregate(const struct batadv_ogm_packet *new_bat_ogm_packet,
			    struct batadv_priv *bat_priv,
			    int packet_len, unsigned long send_time,
			    bool directlink,
			    const struct batadv_hard_iface *if_incoming,
			    const struct batadv_hard_iface *if_outgoing,
			    const struct batadv_forw_packet *forw_packet)
{
	struct batadv_ogm_packet *batadv_ogm_packet;
	int aggregated_bytes = forw_packet->packet_len + packet_len;
	struct batadv_hard_iface *primary_if = NULL;
	bool res = false;
	unsigned long aggregation_end_time;

	batadv_ogm_packet = (struct batadv_ogm_packet *)forw_packet->skb->data;
	aggregation_end_time = send_time;
	aggregation_end_time += msecs_to_jiffies(BATADV_MAX_AGGREGATION_MS);

	/* we can aggregate the current packet to this aggregated packet
	 * if:
	 *
	 * - the send time is within our MAX_AGGREGATION_MS time
	 * - the resulting packet won't be bigger than
	 *   MAX_AGGREGATION_BYTES
	 * otherwise aggregation is not possible
	 */
	if (!time_before(send_time, forw_packet->send_time) ||
	    !time_after_eq(aggregation_end_time, forw_packet->send_time))
		return false;

	if (aggregated_bytes > BATADV_MAX_AGGREGATION_BYTES)
		return false;

	/* packet is not leaving on the same interface. */
	if (forw_packet->if_outgoing != if_outgoing)
		return false;

	/* check aggregation compatibility
	 * -> direct link packets are broadcasted on
	 *    their interface only
	 * -> aggregate packet if the current packet is
	 *    a "global" packet as well as the base
	 *    packet
	 */
	primary_if = batadv_primary_if_get_selected(bat_priv);
	if (!primary_if)
		return false;

	/* packets without direct link flag and high TTL
	 * are flooded through the net
	 */
	if (!directlink &&
	    !(batadv_ogm_packet->flags & BATADV_DIRECTLINK) &&
	    batadv_ogm_packet->ttl != 1 &&

	    /* own packets originating non-primary
	     * interfaces leave only that interface
	     */
	    (!forw_packet->own ||
	     forw_packet->if_incoming == primary_if)) {
		res = true;
		goto out;
	}

	/* if the incoming packet is sent via this one
	 * interface only - we still can aggregate
	 */
	if (directlink &&
	    new_bat_ogm_packet->ttl == 1 &&
	    forw_packet->if_incoming == if_incoming &&

	    /* packets from direct neighbors or
	     * own secondary interface packets
	     * (= secondary interface packets in general)
	     */
	    (batadv_ogm_packet->flags & BATADV_DIRECTLINK ||
	     (forw_packet->own &&
	      forw_packet->if_incoming != primary_if))) {
		res = true;
		goto out;
	}

out:
	if (primary_if)
		batadv_hardif_put(primary_if);
	return res;
}

/**
 * batadv_iv_ogm_aggregate_new() - create a new aggregated packet and add this
 *  packet to it.
 * @packet_buff: pointer to the OGM
 * @packet_len: (total) length of the OGM
 * @send_time: timestamp (jiffies) when the packet is to be sent
 * @direct_link: whether this OGM has direct link status
 * @if_incoming: interface where the packet was received
 * @if_outgoing: interface for which the retransmission should be considered
 * @own_packet: true if it is a self-generated ogm
 */
static void batadv_iv_ogm_aggregate_new(const unsigned char *packet_buff,
					int packet_len, unsigned long send_time,
					bool direct_link,
					struct batadv_hard_iface *if_incoming,
					struct batadv_hard_iface *if_outgoing,
					int own_packet)
{
	struct batadv_priv *bat_priv = netdev_priv(if_incoming->soft_iface);
	struct batadv_forw_packet *forw_packet_aggr;
	struct sk_buff *skb;
	unsigned char *skb_buff;
	unsigned int skb_size;
	atomic_t *queue_left = own_packet ? NULL : &bat_priv->batman_queue_left;

	if (atomic_read(&bat_priv->aggregated_ogms) &&
	    packet_len < BATADV_MAX_AGGREGATION_BYTES)
		skb_size = BATADV_MAX_AGGREGATION_BYTES;
	else
		skb_size = packet_len;

	skb_size += ETH_HLEN;

	skb = netdev_alloc_skb_ip_align(NULL, skb_size);
	if (!skb)
		return;

	forw_packet_aggr = batadv_forw_packet_alloc(if_incoming, if_outgoing,
						    queue_left, bat_priv, skb);
	if (!forw_packet_aggr) {
		kfree_skb(skb);
		return;
	}

	forw_packet_aggr->skb->priority = TC_PRIO_CONTROL;
	skb_reserve(forw_packet_aggr->skb, ETH_HLEN);

	skb_buff = skb_put(forw_packet_aggr->skb, packet_len);
	forw_packet_aggr->packet_len = packet_len;
	memcpy(skb_buff, packet_buff, packet_len);

	forw_packet_aggr->own = own_packet;
	forw_packet_aggr->direct_link_flags = BATADV_NO_FLAGS;
	forw_packet_aggr->send_time = send_time;

	/* save packet direct link flag status */
	if (direct_link)
		forw_packet_aggr->direct_link_flags |= 1;

	INIT_DELAYED_WORK(&forw_packet_aggr->delayed_work,
			  batadv_iv_send_outstanding_bat_ogm_packet);

	batadv_forw_packet_ogmv1_queue(bat_priv, forw_packet_aggr, send_time);
}

/* aggregate a new packet into the existing ogm packet */
static void batadv_iv_ogm_aggregate(struct batadv_forw_packet *forw_packet_aggr,
				    const unsigned char *packet_buff,
				    int packet_len, bool direct_link)
{
	unsigned long new_direct_link_flag;

	skb_put_data(forw_packet_aggr->skb, packet_buff, packet_len);
	forw_packet_aggr->packet_len += packet_len;
	forw_packet_aggr->num_packets++;

	/* save packet direct link flag status */
	if (direct_link) {
		new_direct_link_flag = BIT(forw_packet_aggr->num_packets);
		forw_packet_aggr->direct_link_flags |= new_direct_link_flag;
	}
}

/**
 * batadv_iv_ogm_queue_add() - queue up an OGM for transmission
 * @bat_priv: the bat priv with all the soft interface information
 * @packet_buff: pointer to the OGM
 * @packet_len: (total) length of the OGM
 * @if_incoming: interface where the packet was received
 * @if_outgoing: interface for which the retransmission should be considered
 * @own_packet: true if it is a self-generated ogm
 * @send_time: timestamp (jiffies) when the packet is to be sent
 */
static void batadv_iv_ogm_queue_add(struct batadv_priv *bat_priv,
				    unsigned char *packet_buff,
				    int packet_len,
				    struct batadv_hard_iface *if_incoming,
				    struct batadv_hard_iface *if_outgoing,
				    int own_packet, unsigned long send_time)
{
	/* _aggr -> pointer to the packet we want to aggregate with
	 * _pos -> pointer to the position in the queue
	 */
	struct batadv_forw_packet *forw_packet_aggr = NULL;
	struct batadv_forw_packet *forw_packet_pos = NULL;
	struct batadv_ogm_packet *batadv_ogm_packet;
	bool direct_link;
	unsigned long max_aggregation_jiffies;

	batadv_ogm_packet = (struct batadv_ogm_packet *)packet_buff;
	direct_link = !!(batadv_ogm_packet->flags & BATADV_DIRECTLINK);
	max_aggregation_jiffies = msecs_to_jiffies(BATADV_MAX_AGGREGATION_MS);

	/* find position for the packet in the forward queue */
	spin_lock_bh(&bat_priv->forw_bat_list_lock);
	/* own packets are not to be aggregated */
	if (atomic_read(&bat_priv->aggregated_ogms) && !own_packet) {
		hlist_for_each_entry(forw_packet_pos,
				     &bat_priv->forw_bat_list, list) {
			if (batadv_iv_ogm_can_aggregate(batadv_ogm_packet,
							bat_priv, packet_len,
							send_time, direct_link,
							if_incoming,
							if_outgoing,
							forw_packet_pos)) {
				forw_packet_aggr = forw_packet_pos;
				break;
			}
		}
	}

	/* nothing to aggregate with - either aggregation disabled or no
	 * suitable aggregation packet found
	 */
	if (!forw_packet_aggr) {
		/* the following section can run without the lock */
		spin_unlock_bh(&bat_priv->forw_bat_list_lock);

		/* if we could not aggregate this packet with one of the others
		 * we hold it back for a while, so that it might be aggregated
		 * later on
		 */
		if (!own_packet && atomic_read(&bat_priv->aggregated_ogms))
			send_time += max_aggregation_jiffies;

		batadv_iv_ogm_aggregate_new(packet_buff, packet_len,
					    send_time, direct_link,
					    if_incoming, if_outgoing,
					    own_packet);
	} else {
		batadv_iv_ogm_aggregate(forw_packet_aggr, packet_buff,
					packet_len, direct_link);
		spin_unlock_bh(&bat_priv->forw_bat_list_lock);
	}
}

static void batadv_iv_ogm_forward(struct batadv_orig_node *orig_node,
				  const struct ethhdr *ethhdr,
				  struct batadv_ogm_packet *batadv_ogm_packet,
				  bool is_single_hop_neigh,
				  bool is_from_best_next_hop,
				  struct batadv_hard_iface *if_incoming,
				  struct batadv_hard_iface *if_outgoing)
{
	struct batadv_priv *bat_priv = netdev_priv(if_incoming->soft_iface);
	u16 tvlv_len;

	if (batadv_ogm_packet->ttl <= 1) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv, "ttl exceeded\n");
		return;
	}

	if (!is_from_best_next_hop) {
		/* Mark the forwarded packet when it is not coming from our
		 * best next hop. We still need to forward the packet for our
		 * neighbor link quality detection to work in case the packet
		 * originated from a single hop neighbor. Otherwise we can
		 * simply drop the ogm.
		 */
		if (is_single_hop_neigh)
			batadv_ogm_packet->flags |= BATADV_NOT_BEST_NEXT_HOP;
		else
			return;
	}

	tvlv_len = ntohs(batadv_ogm_packet->tvlv_len);

	batadv_ogm_packet->ttl--;
	ether_addr_copy(batadv_ogm_packet->prev_sender, ethhdr->h_source);

	/* apply hop penalty */
	batadv_ogm_packet->tq = batadv_hop_penalty(batadv_ogm_packet->tq,
						   bat_priv);

	batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
		   "Forwarding packet: tq: %i, ttl: %i\n",
		   batadv_ogm_packet->tq, batadv_ogm_packet->ttl);

	if (is_single_hop_neigh)
		batadv_ogm_packet->flags |= BATADV_DIRECTLINK;
	else
		batadv_ogm_packet->flags &= ~BATADV_DIRECTLINK;

	batadv_iv_ogm_queue_add(bat_priv, (unsigned char *)batadv_ogm_packet,
				BATADV_OGM_HLEN + tvlv_len,
				if_incoming, if_outgoing, 0,
				batadv_iv_ogm_fwd_send_time());
}

/**
 * batadv_iv_ogm_slide_own_bcast_window() - bitshift own OGM broadcast windows
 *  for the given interface
 * @hard_iface: the interface for which the windows have to be shifted
 */
static void
batadv_iv_ogm_slide_own_bcast_window(struct batadv_hard_iface *hard_iface)
{
	struct batadv_priv *bat_priv = netdev_priv(hard_iface->soft_iface);
	struct batadv_hashtable *hash = bat_priv->orig_hash;
	struct hlist_head *head;
	struct batadv_orig_node *orig_node;
	struct batadv_orig_ifinfo *orig_ifinfo;
	unsigned long *word;
	u32 i;
	u8 *w;

	for (i = 0; i < hash->size; i++) {
		head = &hash->table[i];

		rcu_read_lock();
		hlist_for_each_entry_rcu(orig_node, head, hash_entry) {
			hlist_for_each_entry_rcu(orig_ifinfo,
						 &orig_node->ifinfo_list,
						 list) {
				if (orig_ifinfo->if_outgoing != hard_iface)
					continue;

				spin_lock_bh(&orig_node->bat_iv.ogm_cnt_lock);
				word = orig_ifinfo->bat_iv.bcast_own;
				batadv_bit_get_packet(bat_priv, word, 1, 0);
				w = &orig_ifinfo->bat_iv.bcast_own_sum;
				*w = bitmap_weight(word,
						   BATADV_TQ_LOCAL_WINDOW_SIZE);
				spin_unlock_bh(&orig_node->bat_iv.ogm_cnt_lock);
			}
		}
		rcu_read_unlock();
	}
}

/**
 * batadv_iv_ogm_schedule_buff() - schedule submission of hardif ogm buffer
 * @hard_iface: interface whose ogm buffer should be transmitted
 */
static void batadv_iv_ogm_schedule_buff(struct batadv_hard_iface *hard_iface)
{
	struct batadv_priv *bat_priv = netdev_priv(hard_iface->soft_iface);
	unsigned char **ogm_buff = &hard_iface->bat_iv.ogm_buff;
	struct batadv_ogm_packet *batadv_ogm_packet;
	struct batadv_hard_iface *primary_if, *tmp_hard_iface;
	int *ogm_buff_len = &hard_iface->bat_iv.ogm_buff_len;
	u32 seqno;
	u16 tvlv_len = 0;
	unsigned long send_time;

	lockdep_assert_held(&hard_iface->bat_iv.ogm_buff_mutex);

	/* interface already disabled by batadv_iv_ogm_iface_disable */
	if (!*ogm_buff)
		return;

	/* the interface gets activated here to avoid race conditions between
	 * the moment of activating the interface in
	 * hardif_activate_interface() where the originator mac is set and
	 * outdated packets (especially uninitialized mac addresses) in the
	 * packet queue
	 */
	if (hard_iface->if_status == BATADV_IF_TO_BE_ACTIVATED)
		hard_iface->if_status = BATADV_IF_ACTIVE;

	primary_if = batadv_primary_if_get_selected(bat_priv);

	if (hard_iface == primary_if) {
		/* tt changes have to be committed before the tvlv data is
		 * appended as it may alter the tt tvlv container
		 */
		batadv_tt_local_commit_changes(bat_priv);
		tvlv_len = batadv_tvlv_container_ogm_append(bat_priv, ogm_buff,
							    ogm_buff_len,
							    BATADV_OGM_HLEN);
	}

	batadv_ogm_packet = (struct batadv_ogm_packet *)(*ogm_buff);
	batadv_ogm_packet->tvlv_len = htons(tvlv_len);

	/* change sequence number to network order */
	seqno = (u32)atomic_read(&hard_iface->bat_iv.ogm_seqno);
	batadv_ogm_packet->seqno = htonl(seqno);
	atomic_inc(&hard_iface->bat_iv.ogm_seqno);

	batadv_iv_ogm_slide_own_bcast_window(hard_iface);

	send_time = batadv_iv_ogm_emit_send_time(bat_priv);

	if (hard_iface != primary_if) {
		/* OGMs from secondary interfaces are only scheduled on their
		 * respective interfaces.
		 */
		batadv_iv_ogm_queue_add(bat_priv, *ogm_buff, *ogm_buff_len,
					hard_iface, hard_iface, 1, send_time);
		goto out;
	}

	/* OGMs from primary interfaces are scheduled on all
	 * interfaces.
	 */
	rcu_read_lock();
	list_for_each_entry_rcu(tmp_hard_iface, &batadv_hardif_list, list) {
		if (tmp_hard_iface->soft_iface != hard_iface->soft_iface)
			continue;

		if (!kref_get_unless_zero(&tmp_hard_iface->refcount))
			continue;

		batadv_iv_ogm_queue_add(bat_priv, *ogm_buff,
					*ogm_buff_len, hard_iface,
					tmp_hard_iface, 1, send_time);

		batadv_hardif_put(tmp_hard_iface);
	}
	rcu_read_unlock();

out:
	if (primary_if)
		batadv_hardif_put(primary_if);
}

static void batadv_iv_ogm_schedule(struct batadv_hard_iface *hard_iface)
{
	if (hard_iface->if_status == BATADV_IF_NOT_IN_USE ||
	    hard_iface->if_status == BATADV_IF_TO_BE_REMOVED)
		return;

	mutex_lock(&hard_iface->bat_iv.ogm_buff_mutex);
	batadv_iv_ogm_schedule_buff(hard_iface);
	mutex_unlock(&hard_iface->bat_iv.ogm_buff_mutex);
}

/**
 * batadv_iv_orig_ifinfo_sum() - Get bcast_own sum for originator over interface
 * @orig_node: originator which reproadcasted the OGMs directly
 * @if_outgoing: interface which transmitted the original OGM and received the
 *  direct rebroadcast
 *
 * Return: Number of replied (rebroadcasted) OGMs which were transmitted by
 *  an originator and directly (without intermediate hop) received by a specific
 *  interface
 */
static u8 batadv_iv_orig_ifinfo_sum(struct batadv_orig_node *orig_node,
				    struct batadv_hard_iface *if_outgoing)
{
	struct batadv_orig_ifinfo *orig_ifinfo;
	u8 sum;

	orig_ifinfo = batadv_orig_ifinfo_get(orig_node, if_outgoing);
	if (!orig_ifinfo)
		return 0;

	spin_lock_bh(&orig_node->bat_iv.ogm_cnt_lock);
	sum = orig_ifinfo->bat_iv.bcast_own_sum;
	spin_unlock_bh(&orig_node->bat_iv.ogm_cnt_lock);

	batadv_orig_ifinfo_put(orig_ifinfo);

	return sum;
}

/**
 * batadv_iv_ogm_orig_update() - use OGM to update corresponding data in an
 *  originator
 * @bat_priv: the bat priv with all the soft interface information
 * @orig_node: the orig node who originally emitted the ogm packet
 * @orig_ifinfo: ifinfo for the outgoing interface of the orig_node
 * @ethhdr: Ethernet header of the OGM
 * @batadv_ogm_packet: the ogm packet
 * @if_incoming: interface where the packet was received
 * @if_outgoing: interface for which the retransmission should be considered
 * @dup_status: the duplicate status of this ogm packet.
 */
static void
batadv_iv_ogm_orig_update(struct batadv_priv *bat_priv,
			  struct batadv_orig_node *orig_node,
			  struct batadv_orig_ifinfo *orig_ifinfo,
			  const struct ethhdr *ethhdr,
			  const struct batadv_ogm_packet *batadv_ogm_packet,
			  struct batadv_hard_iface *if_incoming,
			  struct batadv_hard_iface *if_outgoing,
			  enum batadv_dup_status dup_status)
{
	struct batadv_neigh_ifinfo *neigh_ifinfo = NULL;
	struct batadv_neigh_ifinfo *router_ifinfo = NULL;
	struct batadv_neigh_node *neigh_node = NULL;
	struct batadv_neigh_node *tmp_neigh_node = NULL;
	struct batadv_neigh_node *router = NULL;
	u8 sum_orig, sum_neigh;
	u8 *neigh_addr;
	u8 tq_avg;

	batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
		   "%s(): Searching and updating originator entry of received packet\n",
		   __func__);

	rcu_read_lock();
	hlist_for_each_entry_rcu(tmp_neigh_node,
				 &orig_node->neigh_list, list) {
		neigh_addr = tmp_neigh_node->addr;
		if (batadv_compare_eth(neigh_addr, ethhdr->h_source) &&
		    tmp_neigh_node->if_incoming == if_incoming &&
		    kref_get_unless_zero(&tmp_neigh_node->refcount)) {
			if (WARN(neigh_node, "too many matching neigh_nodes"))
				batadv_neigh_node_put(neigh_node);
			neigh_node = tmp_neigh_node;
			continue;
		}

		if (dup_status != BATADV_NO_DUP)
			continue;

		/* only update the entry for this outgoing interface */
		neigh_ifinfo = batadv_neigh_ifinfo_get(tmp_neigh_node,
						       if_outgoing);
		if (!neigh_ifinfo)
			continue;

		spin_lock_bh(&tmp_neigh_node->ifinfo_lock);
		batadv_ring_buffer_set(neigh_ifinfo->bat_iv.tq_recv,
				       &neigh_ifinfo->bat_iv.tq_index, 0);
		tq_avg = batadv_ring_buffer_avg(neigh_ifinfo->bat_iv.tq_recv);
		neigh_ifinfo->bat_iv.tq_avg = tq_avg;
		spin_unlock_bh(&tmp_neigh_node->ifinfo_lock);

		batadv_neigh_ifinfo_put(neigh_ifinfo);
		neigh_ifinfo = NULL;
	}

	if (!neigh_node) {
		struct batadv_orig_node *orig_tmp;

		orig_tmp = batadv_iv_ogm_orig_get(bat_priv, ethhdr->h_source);
		if (!orig_tmp)
			goto unlock;

		neigh_node = batadv_iv_ogm_neigh_new(if_incoming,
						     ethhdr->h_source,
						     orig_node, orig_tmp);

		batadv_orig_node_put(orig_tmp);
		if (!neigh_node)
			goto unlock;
	} else {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Updating existing last-hop neighbor of originator\n");
	}

	rcu_read_unlock();
	neigh_ifinfo = batadv_neigh_ifinfo_new(neigh_node, if_outgoing);
	if (!neigh_ifinfo)
		goto out;

	neigh_node->last_seen = jiffies;

	spin_lock_bh(&neigh_node->ifinfo_lock);
	batadv_ring_buffer_set(neigh_ifinfo->bat_iv.tq_recv,
			       &neigh_ifinfo->bat_iv.tq_index,
			       batadv_ogm_packet->tq);
	tq_avg = batadv_ring_buffer_avg(neigh_ifinfo->bat_iv.tq_recv);
	neigh_ifinfo->bat_iv.tq_avg = tq_avg;
	spin_unlock_bh(&neigh_node->ifinfo_lock);

	if (dup_status == BATADV_NO_DUP) {
		orig_ifinfo->last_ttl = batadv_ogm_packet->ttl;
		neigh_ifinfo->last_ttl = batadv_ogm_packet->ttl;
	}

	/* if this neighbor already is our next hop there is nothing
	 * to change
	 */
	router = batadv_orig_router_get(orig_node, if_outgoing);
	if (router == neigh_node)
		goto out;

	if (router) {
		router_ifinfo = batadv_neigh_ifinfo_get(router, if_outgoing);
		if (!router_ifinfo)
			goto out;

		/* if this neighbor does not offer a better TQ we won't
		 * consider it
		 */
		if (router_ifinfo->bat_iv.tq_avg > neigh_ifinfo->bat_iv.tq_avg)
			goto out;
	}

	/* if the TQ is the same and the link not more symmetric we
	 * won't consider it either
	 */
	if (router_ifinfo &&
	    neigh_ifinfo->bat_iv.tq_avg == router_ifinfo->bat_iv.tq_avg) {
		sum_orig = batadv_iv_orig_ifinfo_sum(router->orig_node,
						     router->if_incoming);
		sum_neigh = batadv_iv_orig_ifinfo_sum(neigh_node->orig_node,
						      neigh_node->if_incoming);
		if (sum_orig >= sum_neigh)
			goto out;
	}

	batadv_update_route(bat_priv, orig_node, if_outgoing, neigh_node);
	goto out;

unlock:
	rcu_read_unlock();
out:
	if (neigh_node)
		batadv_neigh_node_put(neigh_node);
	if (router)
		batadv_neigh_node_put(router);
	if (neigh_ifinfo)
		batadv_neigh_ifinfo_put(neigh_ifinfo);
	if (router_ifinfo)
		batadv_neigh_ifinfo_put(router_ifinfo);
}

/**
 * batadv_iv_ogm_calc_tq() - calculate tq for current received ogm packet
 * @orig_node: the orig node who originally emitted the ogm packet
 * @orig_neigh_node: the orig node struct of the neighbor who sent the packet
 * @batadv_ogm_packet: the ogm packet
 * @if_incoming: interface where the packet was received
 * @if_outgoing: interface for which the retransmission should be considered
 *
 * Return: true if the link can be considered bidirectional, false otherwise
 */
static bool batadv_iv_ogm_calc_tq(struct batadv_orig_node *orig_node,
				  struct batadv_orig_node *orig_neigh_node,
				  struct batadv_ogm_packet *batadv_ogm_packet,
				  struct batadv_hard_iface *if_incoming,
				  struct batadv_hard_iface *if_outgoing)
{
	struct batadv_priv *bat_priv = netdev_priv(if_incoming->soft_iface);
	struct batadv_neigh_node *neigh_node = NULL, *tmp_neigh_node;
	struct batadv_neigh_ifinfo *neigh_ifinfo;
	u8 total_count;
	u8 orig_eq_count, neigh_rq_count, neigh_rq_inv, tq_own;
	unsigned int tq_iface_hop_penalty = BATADV_TQ_MAX_VALUE;
	unsigned int neigh_rq_inv_cube, neigh_rq_max_cube;
	unsigned int tq_asym_penalty, inv_asym_penalty;
	unsigned int combined_tq;
	bool ret = false;

	/* find corresponding one hop neighbor */
	rcu_read_lock();
	hlist_for_each_entry_rcu(tmp_neigh_node,
				 &orig_neigh_node->neigh_list, list) {
		if (!batadv_compare_eth(tmp_neigh_node->addr,
					orig_neigh_node->orig))
			continue;

		if (tmp_neigh_node->if_incoming != if_incoming)
			continue;

		if (!kref_get_unless_zero(&tmp_neigh_node->refcount))
			continue;

		neigh_node = tmp_neigh_node;
		break;
	}
	rcu_read_unlock();

	if (!neigh_node)
		neigh_node = batadv_iv_ogm_neigh_new(if_incoming,
						     orig_neigh_node->orig,
						     orig_neigh_node,
						     orig_neigh_node);

	if (!neigh_node)
		goto out;

	/* if orig_node is direct neighbor update neigh_node last_seen */
	if (orig_node == orig_neigh_node)
		neigh_node->last_seen = jiffies;

	orig_node->last_seen = jiffies;

	/* find packet count of corresponding one hop neighbor */
	orig_eq_count = batadv_iv_orig_ifinfo_sum(orig_neigh_node, if_incoming);
	neigh_ifinfo = batadv_neigh_ifinfo_new(neigh_node, if_outgoing);
	if (neigh_ifinfo) {
		neigh_rq_count = neigh_ifinfo->bat_iv.real_packet_count;
		batadv_neigh_ifinfo_put(neigh_ifinfo);
	} else {
		neigh_rq_count = 0;
	}

	/* pay attention to not get a value bigger than 100 % */
	if (orig_eq_count > neigh_rq_count)
		total_count = neigh_rq_count;
	else
		total_count = orig_eq_count;

	/* if we have too few packets (too less data) we set tq_own to zero
	 * if we receive too few packets it is not considered bidirectional
	 */
	if (total_count < BATADV_TQ_LOCAL_BIDRECT_SEND_MINIMUM ||
	    neigh_rq_count < BATADV_TQ_LOCAL_BIDRECT_RECV_MINIMUM)
		tq_own = 0;
	else
		/* neigh_node->real_packet_count is never zero as we
		 * only purge old information when getting new
		 * information
		 */
		tq_own = (BATADV_TQ_MAX_VALUE * total_count) /	neigh_rq_count;

	/* 1 - ((1-x) ** 3), normalized to TQ_MAX_VALUE this does
	 * affect the nearly-symmetric links only a little, but
	 * punishes asymmetric links more.  This will give a value
	 * between 0 and TQ_MAX_VALUE
	 */
	neigh_rq_inv = BATADV_TQ_LOCAL_WINDOW_SIZE - neigh_rq_count;
	neigh_rq_inv_cube = neigh_rq_inv * neigh_rq_inv * neigh_rq_inv;
	neigh_rq_max_cube = BATADV_TQ_LOCAL_WINDOW_SIZE *
			    BATADV_TQ_LOCAL_WINDOW_SIZE *
			    BATADV_TQ_LOCAL_WINDOW_SIZE;
	inv_asym_penalty = BATADV_TQ_MAX_VALUE * neigh_rq_inv_cube;
	inv_asym_penalty /= neigh_rq_max_cube;
	tq_asym_penalty = BATADV_TQ_MAX_VALUE - inv_asym_penalty;
	tq_iface_hop_penalty -= atomic_read(&if_incoming->hop_penalty);

	/* penalize if the OGM is forwarded on the same interface. WiFi
	 * interfaces and other half duplex devices suffer from throughput
	 * drops as they can't send and receive at the same time.
	 */
	if (if_outgoing && if_incoming == if_outgoing &&
	    batadv_is_wifi_hardif(if_outgoing))
		tq_iface_hop_penalty = batadv_hop_penalty(tq_iface_hop_penalty,
							  bat_priv);

	combined_tq = batadv_ogm_packet->tq *
		      tq_own *
		      tq_asym_penalty *
		      tq_iface_hop_penalty;
	combined_tq /= BATADV_TQ_MAX_VALUE *
		       BATADV_TQ_MAX_VALUE *
		       BATADV_TQ_MAX_VALUE;
	batadv_ogm_packet->tq = combined_tq;

	batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
		   "bidirectional: orig = %pM neigh = %pM => own_bcast = %2i, real recv = %2i, local tq: %3i, asym_penalty: %3i, iface_hop_penalty: %3i, total tq: %3i, if_incoming = %s, if_outgoing = %s\n",
		   orig_node->orig, orig_neigh_node->orig, total_count,
		   neigh_rq_count, tq_own, tq_asym_penalty,
		   tq_iface_hop_penalty, batadv_ogm_packet->tq,
		   if_incoming->net_dev->name,
		   if_outgoing ? if_outgoing->net_dev->name : "DEFAULT");

	/* if link has the minimum required transmission quality
	 * consider it bidirectional
	 */
	if (batadv_ogm_packet->tq >= BATADV_TQ_TOTAL_BIDRECT_LIMIT)
		ret = true;

out:
	if (neigh_node)
		batadv_neigh_node_put(neigh_node);
	return ret;
}

/**
 * batadv_iv_ogm_update_seqnos() -  process a batman packet for all interfaces,
 *  adjust the sequence number and find out whether it is a duplicate
 * @ethhdr: ethernet header of the packet
 * @batadv_ogm_packet: OGM packet to be considered
 * @if_incoming: interface on which the OGM packet was received
 * @if_outgoing: interface for which the retransmission should be considered
 *
 * Return: duplicate status as enum batadv_dup_status
 */
static enum batadv_dup_status
batadv_iv_ogm_update_seqnos(const struct ethhdr *ethhdr,
			    const struct batadv_ogm_packet *batadv_ogm_packet,
			    const struct batadv_hard_iface *if_incoming,
			    struct batadv_hard_iface *if_outgoing)
{
	struct batadv_priv *bat_priv = netdev_priv(if_incoming->soft_iface);
	struct batadv_orig_node *orig_node;
	struct batadv_orig_ifinfo *orig_ifinfo = NULL;
	struct batadv_neigh_node *neigh_node;
	struct batadv_neigh_ifinfo *neigh_ifinfo;
	bool is_dup;
	s32 seq_diff;
	bool need_update = false;
	int set_mark;
	enum batadv_dup_status ret = BATADV_NO_DUP;
	u32 seqno = ntohl(batadv_ogm_packet->seqno);
	u8 *neigh_addr;
	u8 packet_count;
	unsigned long *bitmap;

	orig_node = batadv_iv_ogm_orig_get(bat_priv, batadv_ogm_packet->orig);
	if (!orig_node)
		return BATADV_NO_DUP;

	orig_ifinfo = batadv_orig_ifinfo_new(orig_node, if_outgoing);
	if (WARN_ON(!orig_ifinfo)) {
		batadv_orig_node_put(orig_node);
		return 0;
	}

	spin_lock_bh(&orig_node->bat_iv.ogm_cnt_lock);
	seq_diff = seqno - orig_ifinfo->last_real_seqno;

	/* signalize caller that the packet is to be dropped. */
	if (!hlist_empty(&orig_node->neigh_list) &&
	    batadv_window_protected(bat_priv, seq_diff,
				    BATADV_TQ_LOCAL_WINDOW_SIZE,
				    &orig_ifinfo->batman_seqno_reset, NULL)) {
		ret = BATADV_PROTECTED;
		goto out;
	}

	rcu_read_lock();
	hlist_for_each_entry_rcu(neigh_node, &orig_node->neigh_list, list) {
		neigh_ifinfo = batadv_neigh_ifinfo_new(neigh_node,
						       if_outgoing);
		if (!neigh_ifinfo)
			continue;

		neigh_addr = neigh_node->addr;
		is_dup = batadv_test_bit(neigh_ifinfo->bat_iv.real_bits,
					 orig_ifinfo->last_real_seqno,
					 seqno);

		if (batadv_compare_eth(neigh_addr, ethhdr->h_source) &&
		    neigh_node->if_incoming == if_incoming) {
			set_mark = 1;
			if (is_dup)
				ret = BATADV_NEIGH_DUP;
		} else {
			set_mark = 0;
			if (is_dup && ret != BATADV_NEIGH_DUP)
				ret = BATADV_ORIG_DUP;
		}

		/* if the window moved, set the update flag. */
		bitmap = neigh_ifinfo->bat_iv.real_bits;
		need_update |= batadv_bit_get_packet(bat_priv, bitmap,
						     seq_diff, set_mark);

		packet_count = bitmap_weight(bitmap,
					     BATADV_TQ_LOCAL_WINDOW_SIZE);
		neigh_ifinfo->bat_iv.real_packet_count = packet_count;
		batadv_neigh_ifinfo_put(neigh_ifinfo);
	}
	rcu_read_unlock();

	if (need_update) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "%s updating last_seqno: old %u, new %u\n",
			   if_outgoing ? if_outgoing->net_dev->name : "DEFAULT",
			   orig_ifinfo->last_real_seqno, seqno);
		orig_ifinfo->last_real_seqno = seqno;
	}

out:
	spin_unlock_bh(&orig_node->bat_iv.ogm_cnt_lock);
	batadv_orig_node_put(orig_node);
	batadv_orig_ifinfo_put(orig_ifinfo);
	return ret;
}

/**
 * batadv_iv_ogm_process_per_outif() - process a batman iv OGM for an outgoing
 *  interface
 * @skb: the skb containing the OGM
 * @ogm_offset: offset from skb->data to start of ogm header
 * @orig_node: the (cached) orig node for the originator of this OGM
 * @if_incoming: the interface where this packet was received
 * @if_outgoing: the interface for which the packet should be considered
 */
static void
batadv_iv_ogm_process_per_outif(const struct sk_buff *skb, int ogm_offset,
				struct batadv_orig_node *orig_node,
				struct batadv_hard_iface *if_incoming,
				struct batadv_hard_iface *if_outgoing)
{
	struct batadv_priv *bat_priv = netdev_priv(if_incoming->soft_iface);
	struct batadv_hardif_neigh_node *hardif_neigh = NULL;
	struct batadv_neigh_node *router = NULL;
	struct batadv_neigh_node *router_router = NULL;
	struct batadv_orig_node *orig_neigh_node;
	struct batadv_orig_ifinfo *orig_ifinfo;
	struct batadv_neigh_node *orig_neigh_router = NULL;
	struct batadv_neigh_ifinfo *router_ifinfo = NULL;
	struct batadv_ogm_packet *ogm_packet;
	enum batadv_dup_status dup_status;
	bool is_from_best_next_hop = false;
	bool is_single_hop_neigh = false;
	bool sameseq, similar_ttl;
	struct sk_buff *skb_priv;
	struct ethhdr *ethhdr;
	u8 *prev_sender;
	bool is_bidirect;

	/* create a private copy of the skb, as some functions change tq value
	 * and/or flags.
	 */
	skb_priv = skb_copy(skb, GFP_ATOMIC);
	if (!skb_priv)
		return;

	ethhdr = eth_hdr(skb_priv);
	ogm_packet = (struct batadv_ogm_packet *)(skb_priv->data + ogm_offset);

	dup_status = batadv_iv_ogm_update_seqnos(ethhdr, ogm_packet,
						 if_incoming, if_outgoing);
	if (batadv_compare_eth(ethhdr->h_source, ogm_packet->orig))
		is_single_hop_neigh = true;

	if (dup_status == BATADV_PROTECTED) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: packet within seqno protection time (sender: %pM)\n",
			   ethhdr->h_source);
		goto out;
	}

	if (ogm_packet->tq == 0) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: originator packet with tq equal 0\n");
		goto out;
	}

	if (is_single_hop_neigh) {
		hardif_neigh = batadv_hardif_neigh_get(if_incoming,
						       ethhdr->h_source);
		if (hardif_neigh)
			hardif_neigh->last_seen = jiffies;
	}

	router = batadv_orig_router_get(orig_node, if_outgoing);
	if (router) {
		router_router = batadv_orig_router_get(router->orig_node,
						       if_outgoing);
		router_ifinfo = batadv_neigh_ifinfo_get(router, if_outgoing);
	}

	if ((router_ifinfo && router_ifinfo->bat_iv.tq_avg != 0) &&
	    (batadv_compare_eth(router->addr, ethhdr->h_source)))
		is_from_best_next_hop = true;

	prev_sender = ogm_packet->prev_sender;
	/* avoid temporary routing loops */
	if (router && router_router &&
	    (batadv_compare_eth(router->addr, prev_sender)) &&
	    !(batadv_compare_eth(ogm_packet->orig, prev_sender)) &&
	    (batadv_compare_eth(router->addr, router_router->addr))) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: ignoring all rebroadcast packets that may make me loop (sender: %pM)\n",
			   ethhdr->h_source);
		goto out;
	}

	if (if_outgoing == BATADV_IF_DEFAULT)
		batadv_tvlv_ogm_receive(bat_priv, ogm_packet, orig_node);

	/* if sender is a direct neighbor the sender mac equals
	 * originator mac
	 */
	if (is_single_hop_neigh)
		orig_neigh_node = orig_node;
	else
		orig_neigh_node = batadv_iv_ogm_orig_get(bat_priv,
							 ethhdr->h_source);

	if (!orig_neigh_node)
		goto out;

	/* Update nc_nodes of the originator */
	batadv_nc_update_nc_node(bat_priv, orig_node, orig_neigh_node,
				 ogm_packet, is_single_hop_neigh);

	orig_neigh_router = batadv_orig_router_get(orig_neigh_node,
						   if_outgoing);

	/* drop packet if sender is not a direct neighbor and if we
	 * don't route towards it
	 */
	if (!is_single_hop_neigh && !orig_neigh_router) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: OGM via unknown neighbor!\n");
		goto out_neigh;
	}

	is_bidirect = batadv_iv_ogm_calc_tq(orig_node, orig_neigh_node,
					    ogm_packet, if_incoming,
					    if_outgoing);

	/* update ranking if it is not a duplicate or has the same
	 * seqno and similar ttl as the non-duplicate
	 */
	orig_ifinfo = batadv_orig_ifinfo_new(orig_node, if_outgoing);
	if (!orig_ifinfo)
		goto out_neigh;

	sameseq = orig_ifinfo->last_real_seqno == ntohl(ogm_packet->seqno);
	similar_ttl = (orig_ifinfo->last_ttl - 3) <= ogm_packet->ttl;

	if (is_bidirect && (dup_status == BATADV_NO_DUP ||
			    (sameseq && similar_ttl))) {
		batadv_iv_ogm_orig_update(bat_priv, orig_node,
					  orig_ifinfo, ethhdr,
					  ogm_packet, if_incoming,
					  if_outgoing, dup_status);
	}
	batadv_orig_ifinfo_put(orig_ifinfo);

	/* only forward for specific interface, not for the default one. */
	if (if_outgoing == BATADV_IF_DEFAULT)
		goto out_neigh;

	/* is single hop (direct) neighbor */
	if (is_single_hop_neigh) {
		/* OGMs from secondary interfaces should only scheduled once
		 * per interface where it has been received, not multiple times
		 */
		if (ogm_packet->ttl <= 2 &&
		    if_incoming != if_outgoing) {
			batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
				   "Drop packet: OGM from secondary interface and wrong outgoing interface\n");
			goto out_neigh;
		}
		/* mark direct link on incoming interface */
		batadv_iv_ogm_forward(orig_node, ethhdr, ogm_packet,
				      is_single_hop_neigh,
				      is_from_best_next_hop, if_incoming,
				      if_outgoing);

		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Forwarding packet: rebroadcast neighbor packet with direct link flag\n");
		goto out_neigh;
	}

	/* multihop originator */
	if (!is_bidirect) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: not received via bidirectional link\n");
		goto out_neigh;
	}

	if (dup_status == BATADV_NEIGH_DUP) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: duplicate packet received\n");
		goto out_neigh;
	}

	batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
		   "Forwarding packet: rebroadcast originator packet\n");
	batadv_iv_ogm_forward(orig_node, ethhdr, ogm_packet,
			      is_single_hop_neigh, is_from_best_next_hop,
			      if_incoming, if_outgoing);

out_neigh:
	if (orig_neigh_node && !is_single_hop_neigh)
		batadv_orig_node_put(orig_neigh_node);
out:
	if (router_ifinfo)
		batadv_neigh_ifinfo_put(router_ifinfo);
	if (router)
		batadv_neigh_node_put(router);
	if (router_router)
		batadv_neigh_node_put(router_router);
	if (orig_neigh_router)
		batadv_neigh_node_put(orig_neigh_router);
	if (hardif_neigh)
		batadv_hardif_neigh_put(hardif_neigh);

	consume_skb(skb_priv);
}

/**
 * batadv_iv_ogm_process_reply() - Check OGM for direct reply and process it
 * @ogm_packet: rebroadcast OGM packet to process
 * @if_incoming: the interface where this packet was received
 * @orig_node: originator which reproadcasted the OGMs
 * @if_incoming_seqno: OGM sequence number when rebroadcast was received
 */
static void batadv_iv_ogm_process_reply(struct batadv_ogm_packet *ogm_packet,
					struct batadv_hard_iface *if_incoming,
					struct batadv_orig_node *orig_node,
					u32 if_incoming_seqno)
{
	struct batadv_orig_ifinfo *orig_ifinfo;
	s32 bit_pos;
	u8 *weight;

	/* neighbor has to indicate direct link and it has to
	 * come via the corresponding interface
	 */
	if (!(ogm_packet->flags & BATADV_DIRECTLINK))
		return;

	if (!batadv_compare_eth(if_incoming->net_dev->dev_addr,
				ogm_packet->orig))
		return;

	orig_ifinfo = batadv_orig_ifinfo_get(orig_node, if_incoming);
	if (!orig_ifinfo)
		return;

	/* save packet seqno for bidirectional check */
	spin_lock_bh(&orig_node->bat_iv.ogm_cnt_lock);
	bit_pos = if_incoming_seqno - 2;
	bit_pos -= ntohl(ogm_packet->seqno);
	batadv_set_bit(orig_ifinfo->bat_iv.bcast_own, bit_pos);
	weight = &orig_ifinfo->bat_iv.bcast_own_sum;
	*weight = bitmap_weight(orig_ifinfo->bat_iv.bcast_own,
				BATADV_TQ_LOCAL_WINDOW_SIZE);
	spin_unlock_bh(&orig_node->bat_iv.ogm_cnt_lock);

	batadv_orig_ifinfo_put(orig_ifinfo);
}

/**
 * batadv_iv_ogm_process() - process an incoming batman iv OGM
 * @skb: the skb containing the OGM
 * @ogm_offset: offset to the OGM which should be processed (for aggregates)
 * @if_incoming: the interface where this packet was received
 */
static void batadv_iv_ogm_process(const struct sk_buff *skb, int ogm_offset,
				  struct batadv_hard_iface *if_incoming)
{
	struct batadv_priv *bat_priv = netdev_priv(if_incoming->soft_iface);
	struct batadv_orig_node *orig_neigh_node, *orig_node;
	struct batadv_hard_iface *hard_iface;
	struct batadv_ogm_packet *ogm_packet;
	u32 if_incoming_seqno;
	bool has_directlink_flag;
	struct ethhdr *ethhdr;
	bool is_my_oldorig = false;
	bool is_my_addr = false;
	bool is_my_orig = false;

	ogm_packet = (struct batadv_ogm_packet *)(skb->data + ogm_offset);
	ethhdr = eth_hdr(skb);

	/* Silently drop when the batman packet is actually not a
	 * correct packet.
	 *
	 * This might happen if a packet is padded (e.g. Ethernet has a
	 * minimum frame length of 64 byte) and the aggregation interprets
	 * it as an additional length.
	 *
	 * TODO: A more sane solution would be to have a bit in the
	 * batadv_ogm_packet to detect whether the packet is the last
	 * packet in an aggregation.  Here we expect that the padding
	 * is always zero (or not 0x01)
	 */
	if (ogm_packet->packet_type != BATADV_IV_OGM)
		return;

	/* could be changed by schedule_own_packet() */
	if_incoming_seqno = atomic_read(&if_incoming->bat_iv.ogm_seqno);

	if (ogm_packet->flags & BATADV_DIRECTLINK)
		has_directlink_flag = true;
	else
		has_directlink_flag = false;

	batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
		   "Received BATMAN packet via NB: %pM, IF: %s [%pM] (from OG: %pM, via prev OG: %pM, seqno %u, tq %d, TTL %d, V %d, IDF %d)\n",
		   ethhdr->h_source, if_incoming->net_dev->name,
		   if_incoming->net_dev->dev_addr, ogm_packet->orig,
		   ogm_packet->prev_sender, ntohl(ogm_packet->seqno),
		   ogm_packet->tq, ogm_packet->ttl,
		   ogm_packet->version, has_directlink_flag);

	rcu_read_lock();
	list_for_each_entry_rcu(hard_iface, &batadv_hardif_list, list) {
		if (hard_iface->if_status != BATADV_IF_ACTIVE)
			continue;

		if (hard_iface->soft_iface != if_incoming->soft_iface)
			continue;

		if (batadv_compare_eth(ethhdr->h_source,
				       hard_iface->net_dev->dev_addr))
			is_my_addr = true;

		if (batadv_compare_eth(ogm_packet->orig,
				       hard_iface->net_dev->dev_addr))
			is_my_orig = true;

		if (batadv_compare_eth(ogm_packet->prev_sender,
				       hard_iface->net_dev->dev_addr))
			is_my_oldorig = true;
	}
	rcu_read_unlock();

	if (is_my_addr) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: received my own broadcast (sender: %pM)\n",
			   ethhdr->h_source);
		return;
	}

	if (is_my_orig) {
		orig_neigh_node = batadv_iv_ogm_orig_get(bat_priv,
							 ethhdr->h_source);
		if (!orig_neigh_node)
			return;

		batadv_iv_ogm_process_reply(ogm_packet, if_incoming,
					    orig_neigh_node, if_incoming_seqno);

		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: originator packet from myself (via neighbor)\n");
		batadv_orig_node_put(orig_neigh_node);
		return;
	}

	if (is_my_oldorig) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: ignoring all rebroadcast echos (sender: %pM)\n",
			   ethhdr->h_source);
		return;
	}

	if (ogm_packet->flags & BATADV_NOT_BEST_NEXT_HOP) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "Drop packet: ignoring all packets not forwarded from the best next hop (sender: %pM)\n",
			   ethhdr->h_source);
		return;
	}

	orig_node = batadv_iv_ogm_orig_get(bat_priv, ogm_packet->orig);
	if (!orig_node)
		return;

	batadv_iv_ogm_process_per_outif(skb, ogm_offset, orig_node,
					if_incoming, BATADV_IF_DEFAULT);

	rcu_read_lock();
	list_for_each_entry_rcu(hard_iface, &batadv_hardif_list, list) {
		if (hard_iface->if_status != BATADV_IF_ACTIVE)
			continue;

		if (hard_iface->soft_iface != bat_priv->soft_iface)
			continue;

		if (!kref_get_unless_zero(&hard_iface->refcount))
			continue;

		batadv_iv_ogm_process_per_outif(skb, ogm_offset, orig_node,
						if_incoming, hard_iface);

		batadv_hardif_put(hard_iface);
	}
	rcu_read_unlock();

	batadv_orig_node_put(orig_node);
}

static void batadv_iv_send_outstanding_bat_ogm_packet(struct work_struct *work)
{
	struct delayed_work *delayed_work;
	struct batadv_forw_packet *forw_packet;
	struct batadv_priv *bat_priv;
	bool dropped = false;

	delayed_work = to_delayed_work(work);
	forw_packet = container_of(delayed_work, struct batadv_forw_packet,
				   delayed_work);
	bat_priv = netdev_priv(forw_packet->if_incoming->soft_iface);

	if (atomic_read(&bat_priv->mesh_state) == BATADV_MESH_DEACTIVATING) {
		dropped = true;
		goto out;
	}

	batadv_iv_ogm_emit(forw_packet);

	/* we have to have at least one packet in the queue to determine the
	 * queues wake up time unless we are shutting down.
	 *
	 * only re-schedule if this is the "original" copy, e.g. the OGM of the
	 * primary interface should only be rescheduled once per period, but
	 * this function will be called for the forw_packet instances of the
	 * other secondary interfaces as well.
	 */
	if (forw_packet->own &&
	    forw_packet->if_incoming == forw_packet->if_outgoing)
		batadv_iv_ogm_schedule(forw_packet->if_incoming);

out:
	/* do we get something for free()? */
	if (batadv_forw_packet_steal(forw_packet,
				     &bat_priv->forw_bat_list_lock))
		batadv_forw_packet_free(forw_packet, dropped);
}

static int batadv_iv_ogm_receive(struct sk_buff *skb,
				 struct batadv_hard_iface *if_incoming)
{
	struct batadv_priv *bat_priv = netdev_priv(if_incoming->soft_iface);
	struct batadv_ogm_packet *ogm_packet;
	u8 *packet_pos;
	int ogm_offset;
	bool res;
	int ret = NET_RX_DROP;

	res = batadv_check_management_packet(skb, if_incoming, BATADV_OGM_HLEN);
	if (!res)
		goto free_skb;

	/* did we receive a B.A.T.M.A.N. IV OGM packet on an interface
	 * that does not have B.A.T.M.A.N. IV enabled ?
	 */
	if (bat_priv->algo_ops->iface.enable != batadv_iv_ogm_iface_enable)
		goto free_skb;

	batadv_inc_counter(bat_priv, BATADV_CNT_MGMT_RX);
	batadv_add_counter(bat_priv, BATADV_CNT_MGMT_RX_BYTES,
			   skb->len + ETH_HLEN);

	ogm_offset = 0;
	ogm_packet = (struct batadv_ogm_packet *)skb->data;

	/* unpack the aggregated packets and process them one by one */
	while (batadv_iv_ogm_aggr_packet(ogm_offset, skb_headlen(skb),
					 ogm_packet)) {
		batadv_iv_ogm_process(skb, ogm_offset, if_incoming);

		ogm_offset += BATADV_OGM_HLEN;
		ogm_offset += ntohs(ogm_packet->tvlv_len);

		packet_pos = skb->data + ogm_offset;
		ogm_packet = (struct batadv_ogm_packet *)packet_pos;
	}

	ret = NET_RX_SUCCESS;

free_skb:
	if (ret == NET_RX_SUCCESS)
		consume_skb(skb);
	else
		kfree_skb(skb);

	return ret;
}

/**
 * batadv_iv_ogm_neigh_get_tq_avg() - Get the TQ average for a neighbour on a
 *  given outgoing interface.
 * @neigh_node: Neighbour of interest
 * @if_outgoing: Outgoing interface of interest
 * @tq_avg: Pointer of where to store the TQ average
 *
 * Return: False if no average TQ available, otherwise true.
 */
static bool
batadv_iv_ogm_neigh_get_tq_avg(struct batadv_neigh_node *neigh_node,
			       struct batadv_hard_iface *if_outgoing,
			       u8 *tq_avg)
{
	struct batadv_neigh_ifinfo *n_ifinfo;

	n_ifinfo = batadv_neigh_ifinfo_get(neigh_node, if_outgoing);
	if (!n_ifinfo)
		return false;

	*tq_avg = n_ifinfo->bat_iv.tq_avg;
	batadv_neigh_ifinfo_put(n_ifinfo);

	return true;
}

/**
 * batadv_iv_ogm_orig_dump_subentry() - Dump an originator subentry into a
 *  message
 * @msg: Netlink message to dump into
 * @portid: Port making netlink request
 * @seq: Sequence number of netlink message
 * @bat_priv: The bat priv with all the soft interface information
 * @if_outgoing: Limit dump to entries with this outgoing interface
 * @orig_node: Originator to dump
 * @neigh_node: Single hops neighbour
 * @best: Is the best originator
 *
 * Return: Error code, or 0 on success
 */
static int
batadv_iv_ogm_orig_dump_subentry(struct sk_buff *msg, u32 portid, u32 seq,
				 struct batadv_priv *bat_priv,
				 struct batadv_hard_iface *if_outgoing,
				 struct batadv_orig_node *orig_node,
				 struct batadv_neigh_node *neigh_node,
				 bool best)
{
	void *hdr;
	u8 tq_avg;
	unsigned int last_seen_msecs;

	last_seen_msecs = jiffies_to_msecs(jiffies - orig_node->last_seen);

	if (!batadv_iv_ogm_neigh_get_tq_avg(neigh_node, if_outgoing, &tq_avg))
		return 0;

	if (if_outgoing != BATADV_IF_DEFAULT &&
	    if_outgoing != neigh_node->if_incoming)
		return 0;

	hdr = genlmsg_put(msg, portid, seq, &batadv_netlink_family,
			  NLM_F_MULTI, BATADV_CMD_GET_ORIGINATORS);
	if (!hdr)
		return -ENOBUFS;

	if (nla_put(msg, BATADV_ATTR_ORIG_ADDRESS, ETH_ALEN,
		    orig_node->orig) ||
	    nla_put(msg, BATADV_ATTR_NEIGH_ADDRESS, ETH_ALEN,
		    neigh_node->addr) ||
	    nla_put_u32(msg, BATADV_ATTR_HARD_IFINDEX,
			neigh_node->if_incoming->net_dev->ifindex) ||
	    nla_put_u8(msg, BATADV_ATTR_TQ, tq_avg) ||
	    nla_put_u32(msg, BATADV_ATTR_LAST_SEEN_MSECS,
			last_seen_msecs))
		goto nla_put_failure;

	if (best && nla_put_flag(msg, BATADV_ATTR_FLAG_BEST))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);
	return 0;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}

/**
 * batadv_iv_ogm_orig_dump_entry() - Dump an originator entry into a message
 * @msg: Netlink message to dump into
 * @portid: Port making netlink request
 * @seq: Sequence number of netlink message
 * @bat_priv: The bat priv with all the soft interface information
 * @if_outgoing: Limit dump to entries with this outgoing interface
 * @orig_node: Originator to dump
 * @sub_s: Number of sub entries to skip
 *
 * This function assumes the caller holds rcu_read_lock().
 *
 * Return: Error code, or 0 on success
 */
static int
batadv_iv_ogm_orig_dump_entry(struct sk_buff *msg, u32 portid, u32 seq,
			      struct batadv_priv *bat_priv,
			      struct batadv_hard_iface *if_outgoing,
			      struct batadv_orig_node *orig_node, int *sub_s)
{
	struct batadv_neigh_node *neigh_node_best;
	struct batadv_neigh_node *neigh_node;
	int sub = 0;
	bool best;
	u8 tq_avg_best;

	neigh_node_best = batadv_orig_router_get(orig_node, if_outgoing);
	if (!neigh_node_best)
		goto out;

	if (!batadv_iv_ogm_neigh_get_tq_avg(neigh_node_best, if_outgoing,
					    &tq_avg_best))
		goto out;

	if (tq_avg_best == 0)
		goto out;

	hlist_for_each_entry_rcu(neigh_node, &orig_node->neigh_list, list) {
		if (sub++ < *sub_s)
			continue;

		best = (neigh_node == neigh_node_best);

		if (batadv_iv_ogm_orig_dump_subentry(msg, portid, seq,
						     bat_priv, if_outgoing,
						     orig_node, neigh_node,
						     best)) {
			batadv_neigh_node_put(neigh_node_best);

			*sub_s = sub - 1;
			return -EMSGSIZE;
		}
	}

 out:
	if (neigh_node_best)
		batadv_neigh_node_put(neigh_node_best);

	*sub_s = 0;
	return 0;
}

/**
 * batadv_iv_ogm_orig_dump_bucket() - Dump an originator bucket into a
 *  message
 * @msg: Netlink message to dump into
 * @portid: Port making netlink request
 * @seq: Sequence number of netlink message
 * @bat_priv: The bat priv with all the soft interface information
 * @if_outgoing: Limit dump to entries with this outgoing interface
 * @head: Bucket to be dumped
 * @idx_s: Number of entries to be skipped
 * @sub: Number of sub entries to be skipped
 *
 * Return: Error code, or 0 on success
 */
static int
batadv_iv_ogm_orig_dump_bucket(struct sk_buff *msg, u32 portid, u32 seq,
			       struct batadv_priv *bat_priv,
			       struct batadv_hard_iface *if_outgoing,
			       struct hlist_head *head, int *idx_s, int *sub)
{
	struct batadv_orig_node *orig_node;
	int idx = 0;

	rcu_read_lock();
	hlist_for_each_entry_rcu(orig_node, head, hash_entry) {
		if (idx++ < *idx_s)
			continue;

		if (batadv_iv_ogm_orig_dump_entry(msg, portid, seq, bat_priv,
						  if_outgoing, orig_node,
						  sub)) {
			rcu_read_unlock();
			*idx_s = idx - 1;
			return -EMSGSIZE;
		}
	}
	rcu_read_unlock();

	*idx_s = 0;
	*sub = 0;
	return 0;
}

/**
 * batadv_iv_ogm_orig_dump() - Dump the originators into a message
 * @msg: Netlink message to dump into
 * @cb: Control block containing additional options
 * @bat_priv: The bat priv with all the soft interface information
 * @if_outgoing: Limit dump to entries with this outgoing interface
 */
static void
batadv_iv_ogm_orig_dump(struct sk_buff *msg, struct netlink_callback *cb,
			struct batadv_priv *bat_priv,
			struct batadv_hard_iface *if_outgoing)
{
	struct batadv_hashtable *hash = bat_priv->orig_hash;
	struct hlist_head *head;
	int bucket = cb->args[0];
	int idx = cb->args[1];
	int sub = cb->args[2];
	int portid = NETLINK_CB(cb->skb).portid;

	while (bucket < hash->size) {
		head = &hash->table[bucket];

		if (batadv_iv_ogm_orig_dump_bucket(msg, portid,
						   cb->nlh->nlmsg_seq,
						   bat_priv, if_outgoing, head,
						   &idx, &sub))
			break;

		bucket++;
	}

	cb->args[0] = bucket;
	cb->args[1] = idx;
	cb->args[2] = sub;
}

/**
 * batadv_iv_ogm_neigh_diff() - calculate tq difference of two neighbors
 * @neigh1: the first neighbor object of the comparison
 * @if_outgoing1: outgoing interface for the first neighbor
 * @neigh2: the second neighbor object of the comparison
 * @if_outgoing2: outgoing interface for the second neighbor
 * @diff: pointer to integer receiving the calculated difference
 *
 * The content of *@diff is only valid when this function returns true.
 * It is less, equal to or greater than 0 if the metric via neigh1 is lower,
 * the same as or higher than the metric via neigh2
 *
 * Return: true when the difference could be calculated, false otherwise
 */
static bool batadv_iv_ogm_neigh_diff(struct batadv_neigh_node *neigh1,
				     struct batadv_hard_iface *if_outgoing1,
				     struct batadv_neigh_node *neigh2,
				     struct batadv_hard_iface *if_outgoing2,
				     int *diff)
{
	struct batadv_neigh_ifinfo *neigh1_ifinfo, *neigh2_ifinfo;
	u8 tq1, tq2;
	bool ret = true;

	neigh1_ifinfo = batadv_neigh_ifinfo_get(neigh1, if_outgoing1);
	neigh2_ifinfo = batadv_neigh_ifinfo_get(neigh2, if_outgoing2);

	if (!neigh1_ifinfo || !neigh2_ifinfo) {
		ret = false;
		goto out;
	}

	tq1 = neigh1_ifinfo->bat_iv.tq_avg;
	tq2 = neigh2_ifinfo->bat_iv.tq_avg;
	*diff = (int)tq1 - (int)tq2;

out:
	if (neigh1_ifinfo)
		batadv_neigh_ifinfo_put(neigh1_ifinfo);
	if (neigh2_ifinfo)
		batadv_neigh_ifinfo_put(neigh2_ifinfo);

	return ret;
}

/**
 * batadv_iv_ogm_neigh_dump_neigh() - Dump a neighbour into a netlink message
 * @msg: Netlink message to dump into
 * @portid: Port making netlink request
 * @seq: Sequence number of netlink message
 * @hardif_neigh: Neighbour to be dumped
 *
 * Return: Error code, or 0 on success
 */
static int
batadv_iv_ogm_neigh_dump_neigh(struct sk_buff *msg, u32 portid, u32 seq,
			       struct batadv_hardif_neigh_node *hardif_neigh)
{
	void *hdr;
	unsigned int last_seen_msecs;

	last_seen_msecs = jiffies_to_msecs(jiffies - hardif_neigh->last_seen);

	hdr = genlmsg_put(msg, portid, seq, &batadv_netlink_family,
			  NLM_F_MULTI, BATADV_CMD_GET_NEIGHBORS);
	if (!hdr)
		return -ENOBUFS;

	if (nla_put(msg, BATADV_ATTR_NEIGH_ADDRESS, ETH_ALEN,
		    hardif_neigh->addr) ||
	    nla_put_u32(msg, BATADV_ATTR_HARD_IFINDEX,
			hardif_neigh->if_incoming->net_dev->ifindex) ||
	    nla_put_u32(msg, BATADV_ATTR_LAST_SEEN_MSECS,
			last_seen_msecs))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);
	return 0;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}

/**
 * batadv_iv_ogm_neigh_dump_hardif() - Dump the neighbours of a hard interface
 *  into a message
 * @msg: Netlink message to dump into
 * @portid: Port making netlink request
 * @seq: Sequence number of netlink message
 * @bat_priv: The bat priv with all the soft interface information
 * @hard_iface: Hard interface to dump the neighbours for
 * @idx_s: Number of entries to skip
 *
 * This function assumes the caller holds rcu_read_lock().
 *
 * Return: Error code, or 0 on success
 */
static int
batadv_iv_ogm_neigh_dump_hardif(struct sk_buff *msg, u32 portid, u32 seq,
				struct batadv_priv *bat_priv,
				struct batadv_hard_iface *hard_iface,
				int *idx_s)
{
	struct batadv_hardif_neigh_node *hardif_neigh;
	int idx = 0;

	hlist_for_each_entry_rcu(hardif_neigh,
				 &hard_iface->neigh_list, list) {
		if (idx++ < *idx_s)
			continue;

		if (batadv_iv_ogm_neigh_dump_neigh(msg, portid, seq,
						   hardif_neigh)) {
			*idx_s = idx - 1;
			return -EMSGSIZE;
		}
	}

	*idx_s = 0;
	return 0;
}

/**
 * batadv_iv_ogm_neigh_dump() - Dump the neighbours into a message
 * @msg: Netlink message to dump into
 * @cb: Control block containing additional options
 * @bat_priv: The bat priv with all the soft interface information
 * @single_hardif: Limit dump to this hard interface
 */
static void
batadv_iv_ogm_neigh_dump(struct sk_buff *msg, struct netlink_callback *cb,
			 struct batadv_priv *bat_priv,
			 struct batadv_hard_iface *single_hardif)
{
	struct batadv_hard_iface *hard_iface;
	int i_hardif = 0;
	int i_hardif_s = cb->args[0];
	int idx = cb->args[1];
	int portid = NETLINK_CB(cb->skb).portid;

	rcu_read_lock();
	if (single_hardif) {
		if (i_hardif_s == 0) {
			if (batadv_iv_ogm_neigh_dump_hardif(msg, portid,
							    cb->nlh->nlmsg_seq,
							    bat_priv,
							    single_hardif,
							    &idx) == 0)
				i_hardif++;
		}
	} else {
		list_for_each_entry_rcu(hard_iface, &batadv_hardif_list,
					list) {
			if (hard_iface->soft_iface != bat_priv->soft_iface)
				continue;

			if (i_hardif++ < i_hardif_s)
				continue;

			if (batadv_iv_ogm_neigh_dump_hardif(msg, portid,
							    cb->nlh->nlmsg_seq,
							    bat_priv,
							    hard_iface, &idx)) {
				i_hardif--;
				break;
			}
		}
	}
	rcu_read_unlock();

	cb->args[0] = i_hardif;
	cb->args[1] = idx;
}

/**
 * batadv_iv_ogm_neigh_cmp() - compare the metrics of two neighbors
 * @neigh1: the first neighbor object of the comparison
 * @if_outgoing1: outgoing interface for the first neighbor
 * @neigh2: the second neighbor object of the comparison
 * @if_outgoing2: outgoing interface for the second neighbor
 *
 * Return: a value less, equal to or greater than 0 if the metric via neigh1 is
 * lower, the same as or higher than the metric via neigh2
 */
static int batadv_iv_ogm_neigh_cmp(struct batadv_neigh_node *neigh1,
				   struct batadv_hard_iface *if_outgoing1,
				   struct batadv_neigh_node *neigh2,
				   struct batadv_hard_iface *if_outgoing2)
{
	bool ret;
	int diff;

	ret = batadv_iv_ogm_neigh_diff(neigh1, if_outgoing1, neigh2,
				       if_outgoing2, &diff);
	if (!ret)
		return 0;

	return diff;
}

/**
 * batadv_iv_ogm_neigh_is_sob() - check if neigh1 is similarly good or better
 *  than neigh2 from the metric prospective
 * @neigh1: the first neighbor object of the comparison
 * @if_outgoing1: outgoing interface for the first neighbor
 * @neigh2: the second neighbor object of the comparison
 * @if_outgoing2: outgoing interface for the second neighbor
 *
 * Return: true if the metric via neigh1 is equally good or better than
 * the metric via neigh2, false otherwise.
 */
static bool
batadv_iv_ogm_neigh_is_sob(struct batadv_neigh_node *neigh1,
			   struct batadv_hard_iface *if_outgoing1,
			   struct batadv_neigh_node *neigh2,
			   struct batadv_hard_iface *if_outgoing2)
{
	bool ret;
	int diff;

	ret = batadv_iv_ogm_neigh_diff(neigh1, if_outgoing1, neigh2,
				       if_outgoing2, &diff);
	if (!ret)
		return false;

	ret = diff > -BATADV_TQ_SIMILARITY_THRESHOLD;
	return ret;
}

static void batadv_iv_iface_enabled(struct batadv_hard_iface *hard_iface)
{
	/* begin scheduling originator messages on that interface */
	batadv_iv_ogm_schedule(hard_iface);
}

/**
 * batadv_iv_init_sel_class() - initialize GW selection class
 * @bat_priv: the bat priv with all the soft interface information
 */
static void batadv_iv_init_sel_class(struct batadv_priv *bat_priv)
{
	/* set default TQ difference threshold to 20 */
	atomic_set(&bat_priv->gw.sel_class, 20);
}

static struct batadv_gw_node *
batadv_iv_gw_get_best_gw_node(struct batadv_priv *bat_priv)
{
	struct batadv_neigh_node *router;
	struct batadv_neigh_ifinfo *router_ifinfo;
	struct batadv_gw_node *gw_node, *curr_gw = NULL;
	u64 max_gw_factor = 0;
	u64 tmp_gw_factor = 0;
	u8 max_tq = 0;
	u8 tq_avg;
	struct batadv_orig_node *orig_node;

	rcu_read_lock();
	hlist_for_each_entry_rcu(gw_node, &bat_priv->gw.gateway_list, list) {
		orig_node = gw_node->orig_node;
		router = batadv_orig_router_get(orig_node, BATADV_IF_DEFAULT);
		if (!router)
			continue;

		router_ifinfo = batadv_neigh_ifinfo_get(router,
							BATADV_IF_DEFAULT);
		if (!router_ifinfo)
			goto next;

		if (!kref_get_unless_zero(&gw_node->refcount))
			goto next;

		tq_avg = router_ifinfo->bat_iv.tq_avg;

		switch (atomic_read(&bat_priv->gw.sel_class)) {
		case 1: /* fast connection */
			tmp_gw_factor = tq_avg * tq_avg;
			tmp_gw_factor *= gw_node->bandwidth_down;
			tmp_gw_factor *= 100 * 100;
			tmp_gw_factor >>= 18;

			if (tmp_gw_factor > max_gw_factor ||
			    (tmp_gw_factor == max_gw_factor &&
			     tq_avg > max_tq)) {
				if (curr_gw)
					batadv_gw_node_put(curr_gw);
				curr_gw = gw_node;
				kref_get(&curr_gw->refcount);
			}
			break;

		default: /* 2:  stable connection (use best statistic)
			  * 3:  fast-switch (use best statistic but change as
			  *     soon as a better gateway appears)
			  * XX: late-switch (use best statistic but change as
			  *     soon as a better gateway appears which has
			  *     $routing_class more tq points)
			  */
			if (tq_avg > max_tq) {
				if (curr_gw)
					batadv_gw_node_put(curr_gw);
				curr_gw = gw_node;
				kref_get(&curr_gw->refcount);
			}
			break;
		}

		if (tq_avg > max_tq)
			max_tq = tq_avg;

		if (tmp_gw_factor > max_gw_factor)
			max_gw_factor = tmp_gw_factor;

		batadv_gw_node_put(gw_node);

next:
		batadv_neigh_node_put(router);
		if (router_ifinfo)
			batadv_neigh_ifinfo_put(router_ifinfo);
	}
	rcu_read_unlock();

	return curr_gw;
}

static bool batadv_iv_gw_is_eligible(struct batadv_priv *bat_priv,
				     struct batadv_orig_node *curr_gw_orig,
				     struct batadv_orig_node *orig_node)
{
	struct batadv_neigh_ifinfo *router_orig_ifinfo = NULL;
	struct batadv_neigh_ifinfo *router_gw_ifinfo = NULL;
	struct batadv_neigh_node *router_gw = NULL;
	struct batadv_neigh_node *router_orig = NULL;
	u8 gw_tq_avg, orig_tq_avg;
	bool ret = false;

	/* dynamic re-election is performed only on fast or late switch */
	if (atomic_read(&bat_priv->gw.sel_class) <= 2)
		return false;

	router_gw = batadv_orig_router_get(curr_gw_orig, BATADV_IF_DEFAULT);
	if (!router_gw) {
		ret = true;
		goto out;
	}

	router_gw_ifinfo = batadv_neigh_ifinfo_get(router_gw,
						   BATADV_IF_DEFAULT);
	if (!router_gw_ifinfo) {
		ret = true;
		goto out;
	}

	router_orig = batadv_orig_router_get(orig_node, BATADV_IF_DEFAULT);
	if (!router_orig)
		goto out;

	router_orig_ifinfo = batadv_neigh_ifinfo_get(router_orig,
						     BATADV_IF_DEFAULT);
	if (!router_orig_ifinfo)
		goto out;

	gw_tq_avg = router_gw_ifinfo->bat_iv.tq_avg;
	orig_tq_avg = router_orig_ifinfo->bat_iv.tq_avg;

	/* the TQ value has to be better */
	if (orig_tq_avg < gw_tq_avg)
		goto out;

	/* if the routing class is greater than 3 the value tells us how much
	 * greater the TQ value of the new gateway must be
	 */
	if ((atomic_read(&bat_priv->gw.sel_class) > 3) &&
	    (orig_tq_avg - gw_tq_avg < atomic_read(&bat_priv->gw.sel_class)))
		goto out;

	batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
		   "Restarting gateway selection: better gateway found (tq curr: %i, tq new: %i)\n",
		   gw_tq_avg, orig_tq_avg);

	ret = true;
out:
	if (router_gw_ifinfo)
		batadv_neigh_ifinfo_put(router_gw_ifinfo);
	if (router_orig_ifinfo)
		batadv_neigh_ifinfo_put(router_orig_ifinfo);
	if (router_gw)
		batadv_neigh_node_put(router_gw);
	if (router_orig)
		batadv_neigh_node_put(router_orig);

	return ret;
}

/**
 * batadv_iv_gw_dump_entry() - Dump a gateway into a message
 * @msg: Netlink message to dump into
 * @portid: Port making netlink request
 * @cb: Control block containing additional options
 * @bat_priv: The bat priv with all the soft interface information
 * @gw_node: Gateway to be dumped
 *
 * Return: Error code, or 0 on success
 */
static int batadv_iv_gw_dump_entry(struct sk_buff *msg, u32 portid,
				   struct netlink_callback *cb,
				   struct batadv_priv *bat_priv,
				   struct batadv_gw_node *gw_node)
{
	struct batadv_neigh_ifinfo *router_ifinfo = NULL;
	struct batadv_neigh_node *router;
	struct batadv_gw_node *curr_gw = NULL;
	int ret = 0;
	void *hdr;

	router = batadv_orig_router_get(gw_node->orig_node, BATADV_IF_DEFAULT);
	if (!router)
		goto out;

	router_ifinfo = batadv_neigh_ifinfo_get(router, BATADV_IF_DEFAULT);
	if (!router_ifinfo)
		goto out;

	curr_gw = batadv_gw_get_selected_gw_node(bat_priv);

	hdr = genlmsg_put(msg, portid, cb->nlh->nlmsg_seq,
			  &batadv_netlink_family, NLM_F_MULTI,
			  BATADV_CMD_GET_GATEWAYS);
	if (!hdr) {
		ret = -ENOBUFS;
		goto out;
	}

	genl_dump_check_consistent(cb, hdr);

	ret = -EMSGSIZE;

	if (curr_gw == gw_node)
		if (nla_put_flag(msg, BATADV_ATTR_FLAG_BEST)) {
			genlmsg_cancel(msg, hdr);
			goto out;
		}

	if (nla_put(msg, BATADV_ATTR_ORIG_ADDRESS, ETH_ALEN,
		    gw_node->orig_node->orig) ||
	    nla_put_u8(msg, BATADV_ATTR_TQ, router_ifinfo->bat_iv.tq_avg) ||
	    nla_put(msg, BATADV_ATTR_ROUTER, ETH_ALEN,
		    router->addr) ||
	    nla_put_string(msg, BATADV_ATTR_HARD_IFNAME,
			   router->if_incoming->net_dev->name) ||
	    nla_put_u32(msg, BATADV_ATTR_BANDWIDTH_DOWN,
			gw_node->bandwidth_down) ||
	    nla_put_u32(msg, BATADV_ATTR_BANDWIDTH_UP,
			gw_node->bandwidth_up)) {
		genlmsg_cancel(msg, hdr);
		goto out;
	}

	genlmsg_end(msg, hdr);
	ret = 0;

out:
	if (curr_gw)
		batadv_gw_node_put(curr_gw);
	if (router_ifinfo)
		batadv_neigh_ifinfo_put(router_ifinfo);
	if (router)
		batadv_neigh_node_put(router);
	return ret;
}

/**
 * batadv_iv_gw_dump() - Dump gateways into a message
 * @msg: Netlink message to dump into
 * @cb: Control block containing additional options
 * @bat_priv: The bat priv with all the soft interface information
 */
static void batadv_iv_gw_dump(struct sk_buff *msg, struct netlink_callback *cb,
			      struct batadv_priv *bat_priv)
{
	int portid = NETLINK_CB(cb->skb).portid;
	struct batadv_gw_node *gw_node;
	int idx_skip = cb->args[0];
	int idx = 0;

	spin_lock_bh(&bat_priv->gw.list_lock);
	cb->seq = bat_priv->gw.generation << 1 | 1;

	hlist_for_each_entry(gw_node, &bat_priv->gw.gateway_list, list) {
		if (idx++ < idx_skip)
			continue;

		if (batadv_iv_gw_dump_entry(msg, portid, cb, bat_priv,
					    gw_node)) {
			idx_skip = idx - 1;
			goto unlock;
		}
	}

	idx_skip = idx;
unlock:
	spin_unlock_bh(&bat_priv->gw.list_lock);

	cb->args[0] = idx_skip;
}

static struct batadv_algo_ops batadv_batman_iv __read_mostly = {
	.name = "BATMAN_IV",
	.iface = {
		.enable = batadv_iv_ogm_iface_enable,
		.enabled = batadv_iv_iface_enabled,
		.disable = batadv_iv_ogm_iface_disable,
		.update_mac = batadv_iv_ogm_iface_update_mac,
		.primary_set = batadv_iv_ogm_primary_iface_set,
	},
	.neigh = {
		.cmp = batadv_iv_ogm_neigh_cmp,
		.is_similar_or_better = batadv_iv_ogm_neigh_is_sob,
		.dump = batadv_iv_ogm_neigh_dump,
	},
	.orig = {
		.dump = batadv_iv_ogm_orig_dump,
	},
	.gw = {
		.init_sel_class = batadv_iv_init_sel_class,
		.get_best_gw_node = batadv_iv_gw_get_best_gw_node,
		.is_eligible = batadv_iv_gw_is_eligible,
		.dump = batadv_iv_gw_dump,
	},
};

/**
 * batadv_iv_init() - B.A.T.M.A.N. IV initialization function
 *
 * Return: 0 on success or negative error number in case of failure
 */
int __init batadv_iv_init(void)
{
	int ret;

	/* batman originator packet */
	ret = batadv_recv_handler_register(BATADV_IV_OGM,
					   batadv_iv_ogm_receive);
	if (ret < 0)
		goto out;

	ret = batadv_algo_register(&batadv_batman_iv);
	if (ret < 0)
		goto handler_unregister;

	goto out;

handler_unregister:
	batadv_recv_handler_unregister(BATADV_IV_OGM);
out:
	return ret;
}
