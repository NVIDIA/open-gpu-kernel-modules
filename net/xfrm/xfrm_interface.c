// SPDX-License-Identifier: GPL-2.0
/*
 *	XFRM virtual interface
 *
 *	Copyright (C) 2018 secunet Security Networks AG
 *
 *	Author:
 *	Steffen Klassert <steffen.klassert@secunet.com>
 */

#include <linux/module.h>
#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include <linux/icmp.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/net.h>
#include <linux/in6.h>
#include <linux/netdevice.h>
#include <linux/if_link.h>
#include <linux/if_arp.h>
#include <linux/icmpv6.h>
#include <linux/init.h>
#include <linux/route.h>
#include <linux/rtnetlink.h>
#include <linux/netfilter_ipv6.h>
#include <linux/slab.h>
#include <linux/hash.h>

#include <linux/uaccess.h>
#include <linux/atomic.h>

#include <net/icmp.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/ip6_route.h>
#include <net/ip_tunnels.h>
#include <net/addrconf.h>
#include <net/xfrm.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>
#include <linux/etherdevice.h>

static int xfrmi_dev_init(struct net_device *dev);
static void xfrmi_dev_setup(struct net_device *dev);
static struct rtnl_link_ops xfrmi_link_ops __read_mostly;
static unsigned int xfrmi_net_id __read_mostly;
static const struct net_device_ops xfrmi_netdev_ops;

#define XFRMI_HASH_BITS	8
#define XFRMI_HASH_SIZE	BIT(XFRMI_HASH_BITS)

struct xfrmi_net {
	/* lists for storing interfaces in use */
	struct xfrm_if __rcu *xfrmi[XFRMI_HASH_SIZE];
};

#define for_each_xfrmi_rcu(start, xi) \
	for (xi = rcu_dereference(start); xi; xi = rcu_dereference(xi->next))

static u32 xfrmi_hash(u32 if_id)
{
	return hash_32(if_id, XFRMI_HASH_BITS);
}

static struct xfrm_if *xfrmi_lookup(struct net *net, struct xfrm_state *x)
{
	struct xfrmi_net *xfrmn = net_generic(net, xfrmi_net_id);
	struct xfrm_if *xi;

	for_each_xfrmi_rcu(xfrmn->xfrmi[xfrmi_hash(x->if_id)], xi) {
		if (x->if_id == xi->p.if_id &&
		    (xi->dev->flags & IFF_UP))
			return xi;
	}

	return NULL;
}

static struct xfrm_if *xfrmi_decode_session(struct sk_buff *skb,
					    unsigned short family)
{
	struct net_device *dev;
	int ifindex = 0;

	if (!secpath_exists(skb) || !skb->dev)
		return NULL;

	switch (family) {
	case AF_INET6:
		ifindex = inet6_sdif(skb);
		break;
	case AF_INET:
		ifindex = inet_sdif(skb);
		break;
	}

	if (ifindex) {
		struct net *net = xs_net(xfrm_input_state(skb));

		dev = dev_get_by_index_rcu(net, ifindex);
	} else {
		dev = skb->dev;
	}

	if (!dev || !(dev->flags & IFF_UP))
		return NULL;
	if (dev->netdev_ops != &xfrmi_netdev_ops)
		return NULL;

	return netdev_priv(dev);
}

static void xfrmi_link(struct xfrmi_net *xfrmn, struct xfrm_if *xi)
{
	struct xfrm_if __rcu **xip = &xfrmn->xfrmi[xfrmi_hash(xi->p.if_id)];

	rcu_assign_pointer(xi->next , rtnl_dereference(*xip));
	rcu_assign_pointer(*xip, xi);
}

static void xfrmi_unlink(struct xfrmi_net *xfrmn, struct xfrm_if *xi)
{
	struct xfrm_if __rcu **xip;
	struct xfrm_if *iter;

	for (xip = &xfrmn->xfrmi[xfrmi_hash(xi->p.if_id)];
	     (iter = rtnl_dereference(*xip)) != NULL;
	     xip = &iter->next) {
		if (xi == iter) {
			rcu_assign_pointer(*xip, xi->next);
			break;
		}
	}
}

static void xfrmi_dev_free(struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);

	gro_cells_destroy(&xi->gro_cells);
	free_percpu(dev->tstats);
}

static int xfrmi_create(struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);
	struct net *net = dev_net(dev);
	struct xfrmi_net *xfrmn = net_generic(net, xfrmi_net_id);
	int err;

	dev->rtnl_link_ops = &xfrmi_link_ops;
	err = register_netdevice(dev);
	if (err < 0)
		goto out;

	xfrmi_link(xfrmn, xi);

	return 0;

out:
	return err;
}

static struct xfrm_if *xfrmi_locate(struct net *net, struct xfrm_if_parms *p)
{
	struct xfrm_if __rcu **xip;
	struct xfrm_if *xi;
	struct xfrmi_net *xfrmn = net_generic(net, xfrmi_net_id);

	for (xip = &xfrmn->xfrmi[xfrmi_hash(p->if_id)];
	     (xi = rtnl_dereference(*xip)) != NULL;
	     xip = &xi->next)
		if (xi->p.if_id == p->if_id)
			return xi;

	return NULL;
}

static void xfrmi_dev_uninit(struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);
	struct xfrmi_net *xfrmn = net_generic(xi->net, xfrmi_net_id);

	xfrmi_unlink(xfrmn, xi);
}

static void xfrmi_scrub_packet(struct sk_buff *skb, bool xnet)
{
	skb->tstamp = 0;
	skb->pkt_type = PACKET_HOST;
	skb->skb_iif = 0;
	skb->ignore_df = 0;
	skb_dst_drop(skb);
	nf_reset_ct(skb);
	nf_reset_trace(skb);

	if (!xnet)
		return;

	ipvs_reset(skb);
	secpath_reset(skb);
	skb_orphan(skb);
	skb->mark = 0;
}

static int xfrmi_rcv_cb(struct sk_buff *skb, int err)
{
	const struct xfrm_mode *inner_mode;
	struct net_device *dev;
	struct xfrm_state *x;
	struct xfrm_if *xi;
	bool xnet;

	if (err && !secpath_exists(skb))
		return 0;

	x = xfrm_input_state(skb);

	xi = xfrmi_lookup(xs_net(x), x);
	if (!xi)
		return 1;

	dev = xi->dev;
	skb->dev = dev;

	if (err) {
		dev->stats.rx_errors++;
		dev->stats.rx_dropped++;

		return 0;
	}

	xnet = !net_eq(xi->net, dev_net(skb->dev));

	if (xnet) {
		inner_mode = &x->inner_mode;

		if (x->sel.family == AF_UNSPEC) {
			inner_mode = xfrm_ip2inner_mode(x, XFRM_MODE_SKB_CB(skb)->protocol);
			if (inner_mode == NULL) {
				XFRM_INC_STATS(dev_net(skb->dev),
					       LINUX_MIB_XFRMINSTATEMODEERROR);
				return -EINVAL;
			}
		}

		if (!xfrm_policy_check(NULL, XFRM_POLICY_IN, skb,
				       inner_mode->family))
			return -EPERM;
	}

	xfrmi_scrub_packet(skb, xnet);
	dev_sw_netstats_rx_add(dev, skb->len);

	return 0;
}

static int
xfrmi_xmit2(struct sk_buff *skb, struct net_device *dev, struct flowi *fl)
{
	struct xfrm_if *xi = netdev_priv(dev);
	struct net_device_stats *stats = &xi->dev->stats;
	struct dst_entry *dst = skb_dst(skb);
	unsigned int length = skb->len;
	struct net_device *tdev;
	struct xfrm_state *x;
	int err = -1;
	int mtu;

	dst_hold(dst);
	dst = xfrm_lookup_with_ifid(xi->net, dst, fl, NULL, 0, xi->p.if_id);
	if (IS_ERR(dst)) {
		err = PTR_ERR(dst);
		dst = NULL;
		goto tx_err_link_failure;
	}

	x = dst->xfrm;
	if (!x)
		goto tx_err_link_failure;

	if (x->if_id != xi->p.if_id)
		goto tx_err_link_failure;

	tdev = dst->dev;

	if (tdev == dev) {
		stats->collisions++;
		net_warn_ratelimited("%s: Local routing loop detected!\n",
				     dev->name);
		goto tx_err_dst_release;
	}

	mtu = dst_mtu(dst);
	if ((!skb_is_gso(skb) && skb->len > mtu) ||
	    (skb_is_gso(skb) && !skb_gso_validate_network_len(skb, mtu))) {
		skb_dst_update_pmtu_no_confirm(skb, mtu);

		if (skb->protocol == htons(ETH_P_IPV6)) {
			if (mtu < IPV6_MIN_MTU)
				mtu = IPV6_MIN_MTU;

			icmpv6_ndo_send(skb, ICMPV6_PKT_TOOBIG, 0, mtu);
		} else {
			if (!(ip_hdr(skb)->frag_off & htons(IP_DF)))
				goto xmit;
			icmp_ndo_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
				      htonl(mtu));
		}

		dst_release(dst);
		return -EMSGSIZE;
	}

xmit:
	xfrmi_scrub_packet(skb, !net_eq(xi->net, dev_net(dev)));
	skb_dst_set(skb, dst);
	skb->dev = tdev;

	err = dst_output(xi->net, skb->sk, skb);
	if (net_xmit_eval(err) == 0) {
		dev_sw_netstats_tx_add(dev, 1, length);
	} else {
		stats->tx_errors++;
		stats->tx_aborted_errors++;
	}

	return 0;
tx_err_link_failure:
	stats->tx_carrier_errors++;
	dst_link_failure(skb);
tx_err_dst_release:
	dst_release(dst);
	return err;
}

static netdev_tx_t xfrmi_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);
	struct net_device_stats *stats = &xi->dev->stats;
	struct dst_entry *dst = skb_dst(skb);
	struct flowi fl;
	int ret;

	memset(&fl, 0, sizeof(fl));

	switch (skb->protocol) {
	case htons(ETH_P_IPV6):
		xfrm_decode_session(skb, &fl, AF_INET6);
		memset(IP6CB(skb), 0, sizeof(*IP6CB(skb)));
		if (!dst) {
			fl.u.ip6.flowi6_oif = dev->ifindex;
			fl.u.ip6.flowi6_flags |= FLOWI_FLAG_ANYSRC;
			dst = ip6_route_output(dev_net(dev), NULL, &fl.u.ip6);
			if (dst->error) {
				dst_release(dst);
				stats->tx_carrier_errors++;
				goto tx_err;
			}
			skb_dst_set(skb, dst);
		}
		break;
	case htons(ETH_P_IP):
		xfrm_decode_session(skb, &fl, AF_INET);
		memset(IPCB(skb), 0, sizeof(*IPCB(skb)));
		if (!dst) {
			struct rtable *rt;

			fl.u.ip4.flowi4_oif = dev->ifindex;
			fl.u.ip4.flowi4_flags |= FLOWI_FLAG_ANYSRC;
			rt = __ip_route_output_key(dev_net(dev), &fl.u.ip4);
			if (IS_ERR(rt)) {
				stats->tx_carrier_errors++;
				goto tx_err;
			}
			skb_dst_set(skb, &rt->dst);
		}
		break;
	default:
		goto tx_err;
	}

	fl.flowi_oif = xi->p.link;

	ret = xfrmi_xmit2(skb, dev, &fl);
	if (ret < 0)
		goto tx_err;

	return NETDEV_TX_OK;

tx_err:
	stats->tx_errors++;
	stats->tx_dropped++;
	kfree_skb(skb);
	return NETDEV_TX_OK;
}

static int xfrmi4_err(struct sk_buff *skb, u32 info)
{
	const struct iphdr *iph = (const struct iphdr *)skb->data;
	struct net *net = dev_net(skb->dev);
	int protocol = iph->protocol;
	struct ip_comp_hdr *ipch;
	struct ip_esp_hdr *esph;
	struct ip_auth_hdr *ah ;
	struct xfrm_state *x;
	struct xfrm_if *xi;
	__be32 spi;

	switch (protocol) {
	case IPPROTO_ESP:
		esph = (struct ip_esp_hdr *)(skb->data+(iph->ihl<<2));
		spi = esph->spi;
		break;
	case IPPROTO_AH:
		ah = (struct ip_auth_hdr *)(skb->data+(iph->ihl<<2));
		spi = ah->spi;
		break;
	case IPPROTO_COMP:
		ipch = (struct ip_comp_hdr *)(skb->data+(iph->ihl<<2));
		spi = htonl(ntohs(ipch->cpi));
		break;
	default:
		return 0;
	}

	switch (icmp_hdr(skb)->type) {
	case ICMP_DEST_UNREACH:
		if (icmp_hdr(skb)->code != ICMP_FRAG_NEEDED)
			return 0;
	case ICMP_REDIRECT:
		break;
	default:
		return 0;
	}

	x = xfrm_state_lookup(net, skb->mark, (const xfrm_address_t *)&iph->daddr,
			      spi, protocol, AF_INET);
	if (!x)
		return 0;

	xi = xfrmi_lookup(net, x);
	if (!xi) {
		xfrm_state_put(x);
		return -1;
	}

	if (icmp_hdr(skb)->type == ICMP_DEST_UNREACH)
		ipv4_update_pmtu(skb, net, info, 0, protocol);
	else
		ipv4_redirect(skb, net, 0, protocol);
	xfrm_state_put(x);

	return 0;
}

static int xfrmi6_err(struct sk_buff *skb, struct inet6_skb_parm *opt,
		    u8 type, u8 code, int offset, __be32 info)
{
	const struct ipv6hdr *iph = (const struct ipv6hdr *)skb->data;
	struct net *net = dev_net(skb->dev);
	int protocol = iph->nexthdr;
	struct ip_comp_hdr *ipch;
	struct ip_esp_hdr *esph;
	struct ip_auth_hdr *ah;
	struct xfrm_state *x;
	struct xfrm_if *xi;
	__be32 spi;

	switch (protocol) {
	case IPPROTO_ESP:
		esph = (struct ip_esp_hdr *)(skb->data + offset);
		spi = esph->spi;
		break;
	case IPPROTO_AH:
		ah = (struct ip_auth_hdr *)(skb->data + offset);
		spi = ah->spi;
		break;
	case IPPROTO_COMP:
		ipch = (struct ip_comp_hdr *)(skb->data + offset);
		spi = htonl(ntohs(ipch->cpi));
		break;
	default:
		return 0;
	}

	if (type != ICMPV6_PKT_TOOBIG &&
	    type != NDISC_REDIRECT)
		return 0;

	x = xfrm_state_lookup(net, skb->mark, (const xfrm_address_t *)&iph->daddr,
			      spi, protocol, AF_INET6);
	if (!x)
		return 0;

	xi = xfrmi_lookup(net, x);
	if (!xi) {
		xfrm_state_put(x);
		return -1;
	}

	if (type == NDISC_REDIRECT)
		ip6_redirect(skb, net, skb->dev->ifindex, 0,
			     sock_net_uid(net, NULL));
	else
		ip6_update_pmtu(skb, net, info, 0, 0, sock_net_uid(net, NULL));
	xfrm_state_put(x);

	return 0;
}

static int xfrmi_change(struct xfrm_if *xi, const struct xfrm_if_parms *p)
{
	if (xi->p.link != p->link)
		return -EINVAL;

	xi->p.if_id = p->if_id;

	return 0;
}

static int xfrmi_update(struct xfrm_if *xi, struct xfrm_if_parms *p)
{
	struct net *net = xi->net;
	struct xfrmi_net *xfrmn = net_generic(net, xfrmi_net_id);
	int err;

	xfrmi_unlink(xfrmn, xi);
	synchronize_net();
	err = xfrmi_change(xi, p);
	xfrmi_link(xfrmn, xi);
	netdev_state_change(xi->dev);
	return err;
}

static int xfrmi_get_iflink(const struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);

	return xi->p.link;
}

static const struct net_device_ops xfrmi_netdev_ops = {
	.ndo_init	= xfrmi_dev_init,
	.ndo_uninit	= xfrmi_dev_uninit,
	.ndo_start_xmit = xfrmi_xmit,
	.ndo_get_stats64 = dev_get_tstats64,
	.ndo_get_iflink = xfrmi_get_iflink,
};

static void xfrmi_dev_setup(struct net_device *dev)
{
	dev->netdev_ops 	= &xfrmi_netdev_ops;
	dev->header_ops		= &ip_tunnel_header_ops;
	dev->type		= ARPHRD_NONE;
	dev->mtu		= ETH_DATA_LEN;
	dev->min_mtu		= ETH_MIN_MTU;
	dev->max_mtu		= IP_MAX_MTU;
	dev->flags 		= IFF_NOARP;
	dev->needs_free_netdev	= true;
	dev->priv_destructor	= xfrmi_dev_free;
	netif_keep_dst(dev);

	eth_broadcast_addr(dev->broadcast);
}

#define XFRMI_FEATURES (NETIF_F_SG |		\
			NETIF_F_FRAGLIST |	\
			NETIF_F_GSO_SOFTWARE |	\
			NETIF_F_HW_CSUM)

static int xfrmi_dev_init(struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);
	struct net_device *phydev = __dev_get_by_index(xi->net, xi->p.link);
	int err;

	dev->tstats = netdev_alloc_pcpu_stats(struct pcpu_sw_netstats);
	if (!dev->tstats)
		return -ENOMEM;

	err = gro_cells_init(&xi->gro_cells, dev);
	if (err) {
		free_percpu(dev->tstats);
		return err;
	}

	dev->features |= NETIF_F_LLTX;
	dev->features |= XFRMI_FEATURES;
	dev->hw_features |= XFRMI_FEATURES;

	if (phydev) {
		dev->needed_headroom = phydev->needed_headroom;
		dev->needed_tailroom = phydev->needed_tailroom;

		if (is_zero_ether_addr(dev->dev_addr))
			eth_hw_addr_inherit(dev, phydev);
		if (is_zero_ether_addr(dev->broadcast))
			memcpy(dev->broadcast, phydev->broadcast,
			       dev->addr_len);
	} else {
		eth_hw_addr_random(dev);
		eth_broadcast_addr(dev->broadcast);
	}

	return 0;
}

static int xfrmi_validate(struct nlattr *tb[], struct nlattr *data[],
			 struct netlink_ext_ack *extack)
{
	return 0;
}

static void xfrmi_netlink_parms(struct nlattr *data[],
			       struct xfrm_if_parms *parms)
{
	memset(parms, 0, sizeof(*parms));

	if (!data)
		return;

	if (data[IFLA_XFRM_LINK])
		parms->link = nla_get_u32(data[IFLA_XFRM_LINK]);

	if (data[IFLA_XFRM_IF_ID])
		parms->if_id = nla_get_u32(data[IFLA_XFRM_IF_ID]);
}

static int xfrmi_newlink(struct net *src_net, struct net_device *dev,
			struct nlattr *tb[], struct nlattr *data[],
			struct netlink_ext_ack *extack)
{
	struct net *net = dev_net(dev);
	struct xfrm_if_parms p;
	struct xfrm_if *xi;
	int err;

	xfrmi_netlink_parms(data, &p);
	xi = xfrmi_locate(net, &p);
	if (xi)
		return -EEXIST;

	xi = netdev_priv(dev);
	xi->p = p;
	xi->net = net;
	xi->dev = dev;

	err = xfrmi_create(dev);
	return err;
}

static void xfrmi_dellink(struct net_device *dev, struct list_head *head)
{
	unregister_netdevice_queue(dev, head);
}

static int xfrmi_changelink(struct net_device *dev, struct nlattr *tb[],
			   struct nlattr *data[],
			   struct netlink_ext_ack *extack)
{
	struct xfrm_if *xi = netdev_priv(dev);
	struct net *net = xi->net;
	struct xfrm_if_parms p;

	xfrmi_netlink_parms(data, &p);
	xi = xfrmi_locate(net, &p);
	if (!xi) {
		xi = netdev_priv(dev);
	} else {
		if (xi->dev != dev)
			return -EEXIST;
	}

	return xfrmi_update(xi, &p);
}

static size_t xfrmi_get_size(const struct net_device *dev)
{
	return
		/* IFLA_XFRM_LINK */
		nla_total_size(4) +
		/* IFLA_XFRM_IF_ID */
		nla_total_size(4) +
		0;
}

static int xfrmi_fill_info(struct sk_buff *skb, const struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);
	struct xfrm_if_parms *parm = &xi->p;

	if (nla_put_u32(skb, IFLA_XFRM_LINK, parm->link) ||
	    nla_put_u32(skb, IFLA_XFRM_IF_ID, parm->if_id))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -EMSGSIZE;
}

static struct net *xfrmi_get_link_net(const struct net_device *dev)
{
	struct xfrm_if *xi = netdev_priv(dev);

	return xi->net;
}

static const struct nla_policy xfrmi_policy[IFLA_XFRM_MAX + 1] = {
	[IFLA_XFRM_LINK]	= { .type = NLA_U32 },
	[IFLA_XFRM_IF_ID]	= { .type = NLA_U32 },
};

static struct rtnl_link_ops xfrmi_link_ops __read_mostly = {
	.kind		= "xfrm",
	.maxtype	= IFLA_XFRM_MAX,
	.policy		= xfrmi_policy,
	.priv_size	= sizeof(struct xfrm_if),
	.setup		= xfrmi_dev_setup,
	.validate	= xfrmi_validate,
	.newlink	= xfrmi_newlink,
	.dellink	= xfrmi_dellink,
	.changelink	= xfrmi_changelink,
	.get_size	= xfrmi_get_size,
	.fill_info	= xfrmi_fill_info,
	.get_link_net	= xfrmi_get_link_net,
};

static void __net_exit xfrmi_exit_batch_net(struct list_head *net_exit_list)
{
	struct net *net;
	LIST_HEAD(list);

	rtnl_lock();
	list_for_each_entry(net, net_exit_list, exit_list) {
		struct xfrmi_net *xfrmn = net_generic(net, xfrmi_net_id);
		struct xfrm_if __rcu **xip;
		struct xfrm_if *xi;
		int i;

		for (i = 0; i < XFRMI_HASH_SIZE; i++) {
			for (xip = &xfrmn->xfrmi[i];
			     (xi = rtnl_dereference(*xip)) != NULL;
			     xip = &xi->next)
				unregister_netdevice_queue(xi->dev, &list);
		}
	}
	unregister_netdevice_many(&list);
	rtnl_unlock();
}

static struct pernet_operations xfrmi_net_ops = {
	.exit_batch = xfrmi_exit_batch_net,
	.id   = &xfrmi_net_id,
	.size = sizeof(struct xfrmi_net),
};

static struct xfrm6_protocol xfrmi_esp6_protocol __read_mostly = {
	.handler	=	xfrm6_rcv,
	.input_handler	=	xfrm_input,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi6_err,
	.priority	=	10,
};

static struct xfrm6_protocol xfrmi_ah6_protocol __read_mostly = {
	.handler	=	xfrm6_rcv,
	.input_handler	=	xfrm_input,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi6_err,
	.priority	=	10,
};

static struct xfrm6_protocol xfrmi_ipcomp6_protocol __read_mostly = {
	.handler	=	xfrm6_rcv,
	.input_handler	=	xfrm_input,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi6_err,
	.priority	=	10,
};

#if IS_REACHABLE(CONFIG_INET6_XFRM_TUNNEL)
static int xfrmi6_rcv_tunnel(struct sk_buff *skb)
{
	const xfrm_address_t *saddr;
	__be32 spi;

	saddr = (const xfrm_address_t *)&ipv6_hdr(skb)->saddr;
	spi = xfrm6_tunnel_spi_lookup(dev_net(skb->dev), saddr);

	return xfrm6_rcv_spi(skb, IPPROTO_IPV6, spi, NULL);
}

static struct xfrm6_tunnel xfrmi_ipv6_handler __read_mostly = {
	.handler	=	xfrmi6_rcv_tunnel,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi6_err,
	.priority	=	2,
};

static struct xfrm6_tunnel xfrmi_ip6ip_handler __read_mostly = {
	.handler	=	xfrmi6_rcv_tunnel,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi6_err,
	.priority	=	2,
};
#endif

static struct xfrm4_protocol xfrmi_esp4_protocol __read_mostly = {
	.handler	=	xfrm4_rcv,
	.input_handler	=	xfrm_input,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi4_err,
	.priority	=	10,
};

static struct xfrm4_protocol xfrmi_ah4_protocol __read_mostly = {
	.handler	=	xfrm4_rcv,
	.input_handler	=	xfrm_input,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi4_err,
	.priority	=	10,
};

static struct xfrm4_protocol xfrmi_ipcomp4_protocol __read_mostly = {
	.handler	=	xfrm4_rcv,
	.input_handler	=	xfrm_input,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi4_err,
	.priority	=	10,
};

#if IS_REACHABLE(CONFIG_INET_XFRM_TUNNEL)
static int xfrmi4_rcv_tunnel(struct sk_buff *skb)
{
	return xfrm4_rcv_spi(skb, IPPROTO_IPIP, ip_hdr(skb)->saddr);
}

static struct xfrm_tunnel xfrmi_ipip_handler __read_mostly = {
	.handler	=	xfrmi4_rcv_tunnel,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi4_err,
	.priority	=	3,
};

static struct xfrm_tunnel xfrmi_ipip6_handler __read_mostly = {
	.handler	=	xfrmi4_rcv_tunnel,
	.cb_handler	=	xfrmi_rcv_cb,
	.err_handler	=	xfrmi4_err,
	.priority	=	2,
};
#endif

static int __init xfrmi4_init(void)
{
	int err;

	err = xfrm4_protocol_register(&xfrmi_esp4_protocol, IPPROTO_ESP);
	if (err < 0)
		goto xfrm_proto_esp_failed;
	err = xfrm4_protocol_register(&xfrmi_ah4_protocol, IPPROTO_AH);
	if (err < 0)
		goto xfrm_proto_ah_failed;
	err = xfrm4_protocol_register(&xfrmi_ipcomp4_protocol, IPPROTO_COMP);
	if (err < 0)
		goto xfrm_proto_comp_failed;
#if IS_REACHABLE(CONFIG_INET_XFRM_TUNNEL)
	err = xfrm4_tunnel_register(&xfrmi_ipip_handler, AF_INET);
	if (err < 0)
		goto xfrm_tunnel_ipip_failed;
	err = xfrm4_tunnel_register(&xfrmi_ipip6_handler, AF_INET6);
	if (err < 0)
		goto xfrm_tunnel_ipip6_failed;
#endif

	return 0;

#if IS_REACHABLE(CONFIG_INET_XFRM_TUNNEL)
xfrm_tunnel_ipip6_failed:
	xfrm4_tunnel_deregister(&xfrmi_ipip_handler, AF_INET);
xfrm_tunnel_ipip_failed:
	xfrm4_protocol_deregister(&xfrmi_ipcomp4_protocol, IPPROTO_COMP);
#endif
xfrm_proto_comp_failed:
	xfrm4_protocol_deregister(&xfrmi_ah4_protocol, IPPROTO_AH);
xfrm_proto_ah_failed:
	xfrm4_protocol_deregister(&xfrmi_esp4_protocol, IPPROTO_ESP);
xfrm_proto_esp_failed:
	return err;
}

static void xfrmi4_fini(void)
{
#if IS_REACHABLE(CONFIG_INET_XFRM_TUNNEL)
	xfrm4_tunnel_deregister(&xfrmi_ipip6_handler, AF_INET6);
	xfrm4_tunnel_deregister(&xfrmi_ipip_handler, AF_INET);
#endif
	xfrm4_protocol_deregister(&xfrmi_ipcomp4_protocol, IPPROTO_COMP);
	xfrm4_protocol_deregister(&xfrmi_ah4_protocol, IPPROTO_AH);
	xfrm4_protocol_deregister(&xfrmi_esp4_protocol, IPPROTO_ESP);
}

static int __init xfrmi6_init(void)
{
	int err;

	err = xfrm6_protocol_register(&xfrmi_esp6_protocol, IPPROTO_ESP);
	if (err < 0)
		goto xfrm_proto_esp_failed;
	err = xfrm6_protocol_register(&xfrmi_ah6_protocol, IPPROTO_AH);
	if (err < 0)
		goto xfrm_proto_ah_failed;
	err = xfrm6_protocol_register(&xfrmi_ipcomp6_protocol, IPPROTO_COMP);
	if (err < 0)
		goto xfrm_proto_comp_failed;
#if IS_REACHABLE(CONFIG_INET6_XFRM_TUNNEL)
	err = xfrm6_tunnel_register(&xfrmi_ipv6_handler, AF_INET6);
	if (err < 0)
		goto xfrm_tunnel_ipv6_failed;
	err = xfrm6_tunnel_register(&xfrmi_ip6ip_handler, AF_INET);
	if (err < 0)
		goto xfrm_tunnel_ip6ip_failed;
#endif

	return 0;

#if IS_REACHABLE(CONFIG_INET6_XFRM_TUNNEL)
xfrm_tunnel_ip6ip_failed:
	xfrm6_tunnel_deregister(&xfrmi_ipv6_handler, AF_INET6);
xfrm_tunnel_ipv6_failed:
	xfrm6_protocol_deregister(&xfrmi_ipcomp6_protocol, IPPROTO_COMP);
#endif
xfrm_proto_comp_failed:
	xfrm6_protocol_deregister(&xfrmi_ah6_protocol, IPPROTO_AH);
xfrm_proto_ah_failed:
	xfrm6_protocol_deregister(&xfrmi_esp6_protocol, IPPROTO_ESP);
xfrm_proto_esp_failed:
	return err;
}

static void xfrmi6_fini(void)
{
#if IS_REACHABLE(CONFIG_INET6_XFRM_TUNNEL)
	xfrm6_tunnel_deregister(&xfrmi_ip6ip_handler, AF_INET);
	xfrm6_tunnel_deregister(&xfrmi_ipv6_handler, AF_INET6);
#endif
	xfrm6_protocol_deregister(&xfrmi_ipcomp6_protocol, IPPROTO_COMP);
	xfrm6_protocol_deregister(&xfrmi_ah6_protocol, IPPROTO_AH);
	xfrm6_protocol_deregister(&xfrmi_esp6_protocol, IPPROTO_ESP);
}

static const struct xfrm_if_cb xfrm_if_cb = {
	.decode_session =	xfrmi_decode_session,
};

static int __init xfrmi_init(void)
{
	const char *msg;
	int err;

	pr_info("IPsec XFRM device driver\n");

	msg = "tunnel device";
	err = register_pernet_device(&xfrmi_net_ops);
	if (err < 0)
		goto pernet_dev_failed;

	msg = "xfrm4 protocols";
	err = xfrmi4_init();
	if (err < 0)
		goto xfrmi4_failed;

	msg = "xfrm6 protocols";
	err = xfrmi6_init();
	if (err < 0)
		goto xfrmi6_failed;


	msg = "netlink interface";
	err = rtnl_link_register(&xfrmi_link_ops);
	if (err < 0)
		goto rtnl_link_failed;

	xfrm_if_register_cb(&xfrm_if_cb);

	return err;

rtnl_link_failed:
	xfrmi6_fini();
xfrmi6_failed:
	xfrmi4_fini();
xfrmi4_failed:
	unregister_pernet_device(&xfrmi_net_ops);
pernet_dev_failed:
	pr_err("xfrmi init: failed to register %s\n", msg);
	return err;
}

static void __exit xfrmi_fini(void)
{
	xfrm_if_unregister_cb();
	rtnl_link_unregister(&xfrmi_link_ops);
	xfrmi4_fini();
	xfrmi6_fini();
	unregister_pernet_device(&xfrmi_net_ops);
}

module_init(xfrmi_init);
module_exit(xfrmi_fini);
MODULE_LICENSE("GPL");
MODULE_ALIAS_RTNL_LINK("xfrm");
MODULE_ALIAS_NETDEV("xfrm0");
MODULE_AUTHOR("Steffen Klassert");
MODULE_DESCRIPTION("XFRM virtual interface");
