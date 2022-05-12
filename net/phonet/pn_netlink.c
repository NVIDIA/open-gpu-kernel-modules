// SPDX-License-Identifier: GPL-2.0-only
/*
 * File: pn_netlink.c
 *
 * Phonet netlink interface
 *
 * Copyright (C) 2008 Nokia Corporation.
 *
 * Authors: Sakari Ailus <sakari.ailus@nokia.com>
 *          Remi Denis-Courmont
 */

#include <linux/kernel.h>
#include <linux/netlink.h>
#include <linux/phonet.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <net/phonet/pn_dev.h>

/* Device address handling */

static int fill_addr(struct sk_buff *skb, struct net_device *dev, u8 addr,
		     u32 portid, u32 seq, int event);

void phonet_address_notify(int event, struct net_device *dev, u8 addr)
{
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(NLMSG_ALIGN(sizeof(struct ifaddrmsg)) +
			nla_total_size(1), GFP_KERNEL);
	if (skb == NULL)
		goto errout;
	err = fill_addr(skb, dev, addr, 0, 0, event);
	if (err < 0) {
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(skb);
		goto errout;
	}
	rtnl_notify(skb, dev_net(dev), 0,
		    RTNLGRP_PHONET_IFADDR, NULL, GFP_KERNEL);
	return;
errout:
	rtnl_set_sk_err(dev_net(dev), RTNLGRP_PHONET_IFADDR, err);
}

static const struct nla_policy ifa_phonet_policy[IFA_MAX+1] = {
	[IFA_LOCAL] = { .type = NLA_U8 },
};

static int addr_doit(struct sk_buff *skb, struct nlmsghdr *nlh,
		     struct netlink_ext_ack *extack)
{
	struct net *net = sock_net(skb->sk);
	struct nlattr *tb[IFA_MAX+1];
	struct net_device *dev;
	struct ifaddrmsg *ifm;
	int err;
	u8 pnaddr;

	if (!netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if (!netlink_capable(skb, CAP_SYS_ADMIN))
		return -EPERM;

	ASSERT_RTNL();

	err = nlmsg_parse_deprecated(nlh, sizeof(*ifm), tb, IFA_MAX,
				     ifa_phonet_policy, extack);
	if (err < 0)
		return err;

	ifm = nlmsg_data(nlh);
	if (tb[IFA_LOCAL] == NULL)
		return -EINVAL;
	pnaddr = nla_get_u8(tb[IFA_LOCAL]);
	if (pnaddr & 3)
		/* Phonet addresses only have 6 high-order bits */
		return -EINVAL;

	dev = __dev_get_by_index(net, ifm->ifa_index);
	if (dev == NULL)
		return -ENODEV;

	if (nlh->nlmsg_type == RTM_NEWADDR)
		err = phonet_address_add(dev, pnaddr);
	else
		err = phonet_address_del(dev, pnaddr);
	if (!err)
		phonet_address_notify(nlh->nlmsg_type, dev, pnaddr);
	return err;
}

static int fill_addr(struct sk_buff *skb, struct net_device *dev, u8 addr,
			u32 portid, u32 seq, int event)
{
	struct ifaddrmsg *ifm;
	struct nlmsghdr *nlh;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(*ifm), 0);
	if (nlh == NULL)
		return -EMSGSIZE;

	ifm = nlmsg_data(nlh);
	ifm->ifa_family = AF_PHONET;
	ifm->ifa_prefixlen = 0;
	ifm->ifa_flags = IFA_F_PERMANENT;
	ifm->ifa_scope = RT_SCOPE_LINK;
	ifm->ifa_index = dev->ifindex;
	if (nla_put_u8(skb, IFA_LOCAL, addr))
		goto nla_put_failure;
	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}

static int getaddr_dumpit(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct phonet_device_list *pndevs;
	struct phonet_device *pnd;
	int dev_idx = 0, dev_start_idx = cb->args[0];
	int addr_idx = 0, addr_start_idx = cb->args[1];

	pndevs = phonet_device_list(sock_net(skb->sk));
	rcu_read_lock();
	list_for_each_entry_rcu(pnd, &pndevs->list, list) {
		u8 addr;

		if (dev_idx > dev_start_idx)
			addr_start_idx = 0;
		if (dev_idx++ < dev_start_idx)
			continue;

		addr_idx = 0;
		for_each_set_bit(addr, pnd->addrs, 64) {
			if (addr_idx++ < addr_start_idx)
				continue;

			if (fill_addr(skb, pnd->netdev, addr << 2,
					 NETLINK_CB(cb->skb).portid,
					cb->nlh->nlmsg_seq, RTM_NEWADDR) < 0)
				goto out;
		}
	}

out:
	rcu_read_unlock();
	cb->args[0] = dev_idx;
	cb->args[1] = addr_idx;

	return skb->len;
}

/* Routes handling */

static int fill_route(struct sk_buff *skb, struct net_device *dev, u8 dst,
			u32 portid, u32 seq, int event)
{
	struct rtmsg *rtm;
	struct nlmsghdr *nlh;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(*rtm), 0);
	if (nlh == NULL)
		return -EMSGSIZE;

	rtm = nlmsg_data(nlh);
	rtm->rtm_family = AF_PHONET;
	rtm->rtm_dst_len = 6;
	rtm->rtm_src_len = 0;
	rtm->rtm_tos = 0;
	rtm->rtm_table = RT_TABLE_MAIN;
	rtm->rtm_protocol = RTPROT_STATIC;
	rtm->rtm_scope = RT_SCOPE_UNIVERSE;
	rtm->rtm_type = RTN_UNICAST;
	rtm->rtm_flags = 0;
	if (nla_put_u8(skb, RTA_DST, dst) ||
	    nla_put_u32(skb, RTA_OIF, dev->ifindex))
		goto nla_put_failure;
	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}

void rtm_phonet_notify(int event, struct net_device *dev, u8 dst)
{
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(NLMSG_ALIGN(sizeof(struct ifaddrmsg)) +
			nla_total_size(1) + nla_total_size(4), GFP_KERNEL);
	if (skb == NULL)
		goto errout;
	err = fill_route(skb, dev, dst, 0, 0, event);
	if (err < 0) {
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(skb);
		goto errout;
	}
	rtnl_notify(skb, dev_net(dev), 0,
			  RTNLGRP_PHONET_ROUTE, NULL, GFP_KERNEL);
	return;
errout:
	rtnl_set_sk_err(dev_net(dev), RTNLGRP_PHONET_ROUTE, err);
}

static const struct nla_policy rtm_phonet_policy[RTA_MAX+1] = {
	[RTA_DST] = { .type = NLA_U8 },
	[RTA_OIF] = { .type = NLA_U32 },
};

static int route_doit(struct sk_buff *skb, struct nlmsghdr *nlh,
		      struct netlink_ext_ack *extack)
{
	struct net *net = sock_net(skb->sk);
	struct nlattr *tb[RTA_MAX+1];
	struct net_device *dev;
	struct rtmsg *rtm;
	int err;
	u8 dst;

	if (!netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if (!netlink_capable(skb, CAP_SYS_ADMIN))
		return -EPERM;

	ASSERT_RTNL();

	err = nlmsg_parse_deprecated(nlh, sizeof(*rtm), tb, RTA_MAX,
				     rtm_phonet_policy, extack);
	if (err < 0)
		return err;

	rtm = nlmsg_data(nlh);
	if (rtm->rtm_table != RT_TABLE_MAIN || rtm->rtm_type != RTN_UNICAST)
		return -EINVAL;
	if (tb[RTA_DST] == NULL || tb[RTA_OIF] == NULL)
		return -EINVAL;
	dst = nla_get_u8(tb[RTA_DST]);
	if (dst & 3) /* Phonet addresses only have 6 high-order bits */
		return -EINVAL;

	dev = __dev_get_by_index(net, nla_get_u32(tb[RTA_OIF]));
	if (dev == NULL)
		return -ENODEV;

	if (nlh->nlmsg_type == RTM_NEWROUTE)
		err = phonet_route_add(dev, dst);
	else
		err = phonet_route_del(dev, dst);
	if (!err)
		rtm_phonet_notify(nlh->nlmsg_type, dev, dst);
	return err;
}

static int route_dumpit(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	u8 addr;

	rcu_read_lock();
	for (addr = cb->args[0]; addr < 64; addr++) {
		struct net_device *dev = phonet_route_get_rcu(net, addr << 2);

		if (!dev)
			continue;

		if (fill_route(skb, dev, addr << 2, NETLINK_CB(cb->skb).portid,
			       cb->nlh->nlmsg_seq, RTM_NEWROUTE) < 0)
			goto out;
	}

out:
	rcu_read_unlock();
	cb->args[0] = addr;

	return skb->len;
}

int __init phonet_netlink_register(void)
{
	int err = rtnl_register_module(THIS_MODULE, PF_PHONET, RTM_NEWADDR,
				       addr_doit, NULL, 0);
	if (err)
		return err;

	/* Further rtnl_register_module() cannot fail */
	rtnl_register_module(THIS_MODULE, PF_PHONET, RTM_DELADDR,
			     addr_doit, NULL, 0);
	rtnl_register_module(THIS_MODULE, PF_PHONET, RTM_GETADDR,
			     NULL, getaddr_dumpit, 0);
	rtnl_register_module(THIS_MODULE, PF_PHONET, RTM_NEWROUTE,
			     route_doit, NULL, 0);
	rtnl_register_module(THIS_MODULE, PF_PHONET, RTM_DELROUTE,
			     route_doit, NULL, 0);
	rtnl_register_module(THIS_MODULE, PF_PHONET, RTM_GETROUTE,
			     NULL, route_dumpit, 0);
	return 0;
}
