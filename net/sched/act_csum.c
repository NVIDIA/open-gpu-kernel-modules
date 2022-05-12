// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Checksum updating actions
 *
 * Copyright (c) 2010 Gregoire Baron <baronchon@n7mm.org>
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>

#include <linux/netlink.h>
#include <net/netlink.h>
#include <linux/rtnetlink.h>

#include <linux/skbuff.h>

#include <net/ip.h>
#include <net/ipv6.h>
#include <net/icmp.h>
#include <linux/icmpv6.h>
#include <linux/igmp.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/ip6_checksum.h>
#include <net/sctp/checksum.h>

#include <net/act_api.h>
#include <net/pkt_cls.h>

#include <linux/tc_act/tc_csum.h>
#include <net/tc_act/tc_csum.h>

static const struct nla_policy csum_policy[TCA_CSUM_MAX + 1] = {
	[TCA_CSUM_PARMS] = { .len = sizeof(struct tc_csum), },
};

static unsigned int csum_net_id;
static struct tc_action_ops act_csum_ops;

static int tcf_csum_init(struct net *net, struct nlattr *nla,
			 struct nlattr *est, struct tc_action **a, int ovr,
			 int bind, bool rtnl_held, struct tcf_proto *tp,
			 u32 flags, struct netlink_ext_ack *extack)
{
	struct tc_action_net *tn = net_generic(net, csum_net_id);
	struct tcf_csum_params *params_new;
	struct nlattr *tb[TCA_CSUM_MAX + 1];
	struct tcf_chain *goto_ch = NULL;
	struct tc_csum *parm;
	struct tcf_csum *p;
	int ret = 0, err;
	u32 index;

	if (nla == NULL)
		return -EINVAL;

	err = nla_parse_nested_deprecated(tb, TCA_CSUM_MAX, nla, csum_policy,
					  NULL);
	if (err < 0)
		return err;

	if (tb[TCA_CSUM_PARMS] == NULL)
		return -EINVAL;
	parm = nla_data(tb[TCA_CSUM_PARMS]);
	index = parm->index;
	err = tcf_idr_check_alloc(tn, &index, a, bind);
	if (!err) {
		ret = tcf_idr_create_from_flags(tn, index, est, a,
						&act_csum_ops, bind, flags);
		if (ret) {
			tcf_idr_cleanup(tn, index);
			return ret;
		}
		ret = ACT_P_CREATED;
	} else if (err > 0) {
		if (bind)/* dont override defaults */
			return 0;
		if (!ovr) {
			tcf_idr_release(*a, bind);
			return -EEXIST;
		}
	} else {
		return err;
	}

	err = tcf_action_check_ctrlact(parm->action, tp, &goto_ch, extack);
	if (err < 0)
		goto release_idr;

	p = to_tcf_csum(*a);

	params_new = kzalloc(sizeof(*params_new), GFP_KERNEL);
	if (unlikely(!params_new)) {
		err = -ENOMEM;
		goto put_chain;
	}
	params_new->update_flags = parm->update_flags;

	spin_lock_bh(&p->tcf_lock);
	goto_ch = tcf_action_set_ctrlact(*a, parm->action, goto_ch);
	params_new = rcu_replace_pointer(p->params, params_new,
					 lockdep_is_held(&p->tcf_lock));
	spin_unlock_bh(&p->tcf_lock);

	if (goto_ch)
		tcf_chain_put_by_act(goto_ch);
	if (params_new)
		kfree_rcu(params_new, rcu);

	return ret;
put_chain:
	if (goto_ch)
		tcf_chain_put_by_act(goto_ch);
release_idr:
	tcf_idr_release(*a, bind);
	return err;
}

/**
 * tcf_csum_skb_nextlayer - Get next layer pointer
 * @skb: sk_buff to use
 * @ihl: previous summed headers length
 * @ipl: complete packet length
 * @jhl: next header length
 *
 * Check the expected next layer availability in the specified sk_buff.
 * Return the next layer pointer if pass, NULL otherwise.
 */
static void *tcf_csum_skb_nextlayer(struct sk_buff *skb,
				    unsigned int ihl, unsigned int ipl,
				    unsigned int jhl)
{
	int ntkoff = skb_network_offset(skb);
	int hl = ihl + jhl;

	if (!pskb_may_pull(skb, ipl + ntkoff) || (ipl < hl) ||
	    skb_try_make_writable(skb, hl + ntkoff))
		return NULL;
	else
		return (void *)(skb_network_header(skb) + ihl);
}

static int tcf_csum_ipv4_icmp(struct sk_buff *skb, unsigned int ihl,
			      unsigned int ipl)
{
	struct icmphdr *icmph;

	icmph = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*icmph));
	if (icmph == NULL)
		return 0;

	icmph->checksum = 0;
	skb->csum = csum_partial(icmph, ipl - ihl, 0);
	icmph->checksum = csum_fold(skb->csum);

	skb->ip_summed = CHECKSUM_NONE;

	return 1;
}

static int tcf_csum_ipv4_igmp(struct sk_buff *skb,
			      unsigned int ihl, unsigned int ipl)
{
	struct igmphdr *igmph;

	igmph = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*igmph));
	if (igmph == NULL)
		return 0;

	igmph->csum = 0;
	skb->csum = csum_partial(igmph, ipl - ihl, 0);
	igmph->csum = csum_fold(skb->csum);

	skb->ip_summed = CHECKSUM_NONE;

	return 1;
}

static int tcf_csum_ipv6_icmp(struct sk_buff *skb, unsigned int ihl,
			      unsigned int ipl)
{
	struct icmp6hdr *icmp6h;
	const struct ipv6hdr *ip6h;

	icmp6h = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*icmp6h));
	if (icmp6h == NULL)
		return 0;

	ip6h = ipv6_hdr(skb);
	icmp6h->icmp6_cksum = 0;
	skb->csum = csum_partial(icmp6h, ipl - ihl, 0);
	icmp6h->icmp6_cksum = csum_ipv6_magic(&ip6h->saddr, &ip6h->daddr,
					      ipl - ihl, IPPROTO_ICMPV6,
					      skb->csum);

	skb->ip_summed = CHECKSUM_NONE;

	return 1;
}

static int tcf_csum_ipv4_tcp(struct sk_buff *skb, unsigned int ihl,
			     unsigned int ipl)
{
	struct tcphdr *tcph;
	const struct iphdr *iph;

	if (skb_is_gso(skb) && skb_shinfo(skb)->gso_type & SKB_GSO_TCPV4)
		return 1;

	tcph = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*tcph));
	if (tcph == NULL)
		return 0;

	iph = ip_hdr(skb);
	tcph->check = 0;
	skb->csum = csum_partial(tcph, ipl - ihl, 0);
	tcph->check = tcp_v4_check(ipl - ihl,
				   iph->saddr, iph->daddr, skb->csum);

	skb->ip_summed = CHECKSUM_NONE;

	return 1;
}

static int tcf_csum_ipv6_tcp(struct sk_buff *skb, unsigned int ihl,
			     unsigned int ipl)
{
	struct tcphdr *tcph;
	const struct ipv6hdr *ip6h;

	if (skb_is_gso(skb) && skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6)
		return 1;

	tcph = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*tcph));
	if (tcph == NULL)
		return 0;

	ip6h = ipv6_hdr(skb);
	tcph->check = 0;
	skb->csum = csum_partial(tcph, ipl - ihl, 0);
	tcph->check = csum_ipv6_magic(&ip6h->saddr, &ip6h->daddr,
				      ipl - ihl, IPPROTO_TCP,
				      skb->csum);

	skb->ip_summed = CHECKSUM_NONE;

	return 1;
}

static int tcf_csum_ipv4_udp(struct sk_buff *skb, unsigned int ihl,
			     unsigned int ipl, int udplite)
{
	struct udphdr *udph;
	const struct iphdr *iph;
	u16 ul;

	if (skb_is_gso(skb) && skb_shinfo(skb)->gso_type & SKB_GSO_UDP)
		return 1;

	/*
	 * Support both UDP and UDPLITE checksum algorithms, Don't use
	 * udph->len to get the real length without any protocol check,
	 * UDPLITE uses udph->len for another thing,
	 * Use iph->tot_len, or just ipl.
	 */

	udph = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*udph));
	if (udph == NULL)
		return 0;

	iph = ip_hdr(skb);
	ul = ntohs(udph->len);

	if (udplite || udph->check) {

		udph->check = 0;

		if (udplite) {
			if (ul == 0)
				skb->csum = csum_partial(udph, ipl - ihl, 0);
			else if ((ul >= sizeof(*udph)) && (ul <= ipl - ihl))
				skb->csum = csum_partial(udph, ul, 0);
			else
				goto ignore_obscure_skb;
		} else {
			if (ul != ipl - ihl)
				goto ignore_obscure_skb;

			skb->csum = csum_partial(udph, ul, 0);
		}

		udph->check = csum_tcpudp_magic(iph->saddr, iph->daddr,
						ul, iph->protocol,
						skb->csum);

		if (!udph->check)
			udph->check = CSUM_MANGLED_0;
	}

	skb->ip_summed = CHECKSUM_NONE;

ignore_obscure_skb:
	return 1;
}

static int tcf_csum_ipv6_udp(struct sk_buff *skb, unsigned int ihl,
			     unsigned int ipl, int udplite)
{
	struct udphdr *udph;
	const struct ipv6hdr *ip6h;
	u16 ul;

	if (skb_is_gso(skb) && skb_shinfo(skb)->gso_type & SKB_GSO_UDP)
		return 1;

	/*
	 * Support both UDP and UDPLITE checksum algorithms, Don't use
	 * udph->len to get the real length without any protocol check,
	 * UDPLITE uses udph->len for another thing,
	 * Use ip6h->payload_len + sizeof(*ip6h) ... , or just ipl.
	 */

	udph = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*udph));
	if (udph == NULL)
		return 0;

	ip6h = ipv6_hdr(skb);
	ul = ntohs(udph->len);

	udph->check = 0;

	if (udplite) {
		if (ul == 0)
			skb->csum = csum_partial(udph, ipl - ihl, 0);

		else if ((ul >= sizeof(*udph)) && (ul <= ipl - ihl))
			skb->csum = csum_partial(udph, ul, 0);

		else
			goto ignore_obscure_skb;
	} else {
		if (ul != ipl - ihl)
			goto ignore_obscure_skb;

		skb->csum = csum_partial(udph, ul, 0);
	}

	udph->check = csum_ipv6_magic(&ip6h->saddr, &ip6h->daddr, ul,
				      udplite ? IPPROTO_UDPLITE : IPPROTO_UDP,
				      skb->csum);

	if (!udph->check)
		udph->check = CSUM_MANGLED_0;

	skb->ip_summed = CHECKSUM_NONE;

ignore_obscure_skb:
	return 1;
}

static int tcf_csum_sctp(struct sk_buff *skb, unsigned int ihl,
			 unsigned int ipl)
{
	struct sctphdr *sctph;

	if (skb_is_gso(skb) && skb_is_gso_sctp(skb))
		return 1;

	sctph = tcf_csum_skb_nextlayer(skb, ihl, ipl, sizeof(*sctph));
	if (!sctph)
		return 0;

	sctph->checksum = sctp_compute_cksum(skb,
					     skb_network_offset(skb) + ihl);
	skb->ip_summed = CHECKSUM_NONE;
	skb->csum_not_inet = 0;

	return 1;
}

static int tcf_csum_ipv4(struct sk_buff *skb, u32 update_flags)
{
	const struct iphdr *iph;
	int ntkoff;

	ntkoff = skb_network_offset(skb);

	if (!pskb_may_pull(skb, sizeof(*iph) + ntkoff))
		goto fail;

	iph = ip_hdr(skb);

	switch (iph->frag_off & htons(IP_OFFSET) ? 0 : iph->protocol) {
	case IPPROTO_ICMP:
		if (update_flags & TCA_CSUM_UPDATE_FLAG_ICMP)
			if (!tcf_csum_ipv4_icmp(skb, iph->ihl * 4,
						ntohs(iph->tot_len)))
				goto fail;
		break;
	case IPPROTO_IGMP:
		if (update_flags & TCA_CSUM_UPDATE_FLAG_IGMP)
			if (!tcf_csum_ipv4_igmp(skb, iph->ihl * 4,
						ntohs(iph->tot_len)))
				goto fail;
		break;
	case IPPROTO_TCP:
		if (update_flags & TCA_CSUM_UPDATE_FLAG_TCP)
			if (!tcf_csum_ipv4_tcp(skb, iph->ihl * 4,
					       ntohs(iph->tot_len)))
				goto fail;
		break;
	case IPPROTO_UDP:
		if (update_flags & TCA_CSUM_UPDATE_FLAG_UDP)
			if (!tcf_csum_ipv4_udp(skb, iph->ihl * 4,
					       ntohs(iph->tot_len), 0))
				goto fail;
		break;
	case IPPROTO_UDPLITE:
		if (update_flags & TCA_CSUM_UPDATE_FLAG_UDPLITE)
			if (!tcf_csum_ipv4_udp(skb, iph->ihl * 4,
					       ntohs(iph->tot_len), 1))
				goto fail;
		break;
	case IPPROTO_SCTP:
		if ((update_flags & TCA_CSUM_UPDATE_FLAG_SCTP) &&
		    !tcf_csum_sctp(skb, iph->ihl * 4, ntohs(iph->tot_len)))
			goto fail;
		break;
	}

	if (update_flags & TCA_CSUM_UPDATE_FLAG_IPV4HDR) {
		if (skb_try_make_writable(skb, sizeof(*iph) + ntkoff))
			goto fail;

		ip_send_check(ip_hdr(skb));
	}

	return 1;

fail:
	return 0;
}

static int tcf_csum_ipv6_hopopts(struct ipv6_opt_hdr *ip6xh, unsigned int ixhl,
				 unsigned int *pl)
{
	int off, len, optlen;
	unsigned char *xh = (void *)ip6xh;

	off = sizeof(*ip6xh);
	len = ixhl - off;

	while (len > 1) {
		switch (xh[off]) {
		case IPV6_TLV_PAD1:
			optlen = 1;
			break;
		case IPV6_TLV_JUMBO:
			optlen = xh[off + 1] + 2;
			if (optlen != 6 || len < 6 || (off & 3) != 2)
				/* wrong jumbo option length/alignment */
				return 0;
			*pl = ntohl(*(__be32 *)(xh + off + 2));
			goto done;
		default:
			optlen = xh[off + 1] + 2;
			if (optlen > len)
				/* ignore obscure options */
				goto done;
			break;
		}
		off += optlen;
		len -= optlen;
	}

done:
	return 1;
}

static int tcf_csum_ipv6(struct sk_buff *skb, u32 update_flags)
{
	struct ipv6hdr *ip6h;
	struct ipv6_opt_hdr *ip6xh;
	unsigned int hl, ixhl;
	unsigned int pl;
	int ntkoff;
	u8 nexthdr;

	ntkoff = skb_network_offset(skb);

	hl = sizeof(*ip6h);

	if (!pskb_may_pull(skb, hl + ntkoff))
		goto fail;

	ip6h = ipv6_hdr(skb);

	pl = ntohs(ip6h->payload_len);
	nexthdr = ip6h->nexthdr;

	do {
		switch (nexthdr) {
		case NEXTHDR_FRAGMENT:
			goto ignore_skb;
		case NEXTHDR_ROUTING:
		case NEXTHDR_HOP:
		case NEXTHDR_DEST:
			if (!pskb_may_pull(skb, hl + sizeof(*ip6xh) + ntkoff))
				goto fail;
			ip6xh = (void *)(skb_network_header(skb) + hl);
			ixhl = ipv6_optlen(ip6xh);
			if (!pskb_may_pull(skb, hl + ixhl + ntkoff))
				goto fail;
			ip6xh = (void *)(skb_network_header(skb) + hl);
			if ((nexthdr == NEXTHDR_HOP) &&
			    !(tcf_csum_ipv6_hopopts(ip6xh, ixhl, &pl)))
				goto fail;
			nexthdr = ip6xh->nexthdr;
			hl += ixhl;
			break;
		case IPPROTO_ICMPV6:
			if (update_flags & TCA_CSUM_UPDATE_FLAG_ICMP)
				if (!tcf_csum_ipv6_icmp(skb,
							hl, pl + sizeof(*ip6h)))
					goto fail;
			goto done;
		case IPPROTO_TCP:
			if (update_flags & TCA_CSUM_UPDATE_FLAG_TCP)
				if (!tcf_csum_ipv6_tcp(skb,
						       hl, pl + sizeof(*ip6h)))
					goto fail;
			goto done;
		case IPPROTO_UDP:
			if (update_flags & TCA_CSUM_UPDATE_FLAG_UDP)
				if (!tcf_csum_ipv6_udp(skb, hl,
						       pl + sizeof(*ip6h), 0))
					goto fail;
			goto done;
		case IPPROTO_UDPLITE:
			if (update_flags & TCA_CSUM_UPDATE_FLAG_UDPLITE)
				if (!tcf_csum_ipv6_udp(skb, hl,
						       pl + sizeof(*ip6h), 1))
					goto fail;
			goto done;
		case IPPROTO_SCTP:
			if ((update_flags & TCA_CSUM_UPDATE_FLAG_SCTP) &&
			    !tcf_csum_sctp(skb, hl, pl + sizeof(*ip6h)))
				goto fail;
			goto done;
		default:
			goto ignore_skb;
		}
	} while (pskb_may_pull(skb, hl + 1 + ntkoff));

done:
ignore_skb:
	return 1;

fail:
	return 0;
}

static int tcf_csum_act(struct sk_buff *skb, const struct tc_action *a,
			struct tcf_result *res)
{
	struct tcf_csum *p = to_tcf_csum(a);
	bool orig_vlan_tag_present = false;
	unsigned int vlan_hdr_count = 0;
	struct tcf_csum_params *params;
	u32 update_flags;
	__be16 protocol;
	int action;

	params = rcu_dereference_bh(p->params);

	tcf_lastuse_update(&p->tcf_tm);
	tcf_action_update_bstats(&p->common, skb);

	action = READ_ONCE(p->tcf_action);
	if (unlikely(action == TC_ACT_SHOT))
		goto drop;

	update_flags = params->update_flags;
	protocol = skb_protocol(skb, false);
again:
	switch (protocol) {
	case cpu_to_be16(ETH_P_IP):
		if (!tcf_csum_ipv4(skb, update_flags))
			goto drop;
		break;
	case cpu_to_be16(ETH_P_IPV6):
		if (!tcf_csum_ipv6(skb, update_flags))
			goto drop;
		break;
	case cpu_to_be16(ETH_P_8021AD):
		fallthrough;
	case cpu_to_be16(ETH_P_8021Q):
		if (skb_vlan_tag_present(skb) && !orig_vlan_tag_present) {
			protocol = skb->protocol;
			orig_vlan_tag_present = true;
		} else {
			struct vlan_hdr *vlan = (struct vlan_hdr *)skb->data;

			protocol = vlan->h_vlan_encapsulated_proto;
			skb_pull(skb, VLAN_HLEN);
			skb_reset_network_header(skb);
			vlan_hdr_count++;
		}
		goto again;
	}

out:
	/* Restore the skb for the pulled VLAN tags */
	while (vlan_hdr_count--) {
		skb_push(skb, VLAN_HLEN);
		skb_reset_network_header(skb);
	}

	return action;

drop:
	tcf_action_inc_drop_qstats(&p->common);
	action = TC_ACT_SHOT;
	goto out;
}

static int tcf_csum_dump(struct sk_buff *skb, struct tc_action *a, int bind,
			 int ref)
{
	unsigned char *b = skb_tail_pointer(skb);
	struct tcf_csum *p = to_tcf_csum(a);
	struct tcf_csum_params *params;
	struct tc_csum opt = {
		.index   = p->tcf_index,
		.refcnt  = refcount_read(&p->tcf_refcnt) - ref,
		.bindcnt = atomic_read(&p->tcf_bindcnt) - bind,
	};
	struct tcf_t t;

	spin_lock_bh(&p->tcf_lock);
	params = rcu_dereference_protected(p->params,
					   lockdep_is_held(&p->tcf_lock));
	opt.action = p->tcf_action;
	opt.update_flags = params->update_flags;

	if (nla_put(skb, TCA_CSUM_PARMS, sizeof(opt), &opt))
		goto nla_put_failure;

	tcf_tm_dump(&t, &p->tcf_tm);
	if (nla_put_64bit(skb, TCA_CSUM_TM, sizeof(t), &t, TCA_CSUM_PAD))
		goto nla_put_failure;
	spin_unlock_bh(&p->tcf_lock);

	return skb->len;

nla_put_failure:
	spin_unlock_bh(&p->tcf_lock);
	nlmsg_trim(skb, b);
	return -1;
}

static void tcf_csum_cleanup(struct tc_action *a)
{
	struct tcf_csum *p = to_tcf_csum(a);
	struct tcf_csum_params *params;

	params = rcu_dereference_protected(p->params, 1);
	if (params)
		kfree_rcu(params, rcu);
}

static int tcf_csum_walker(struct net *net, struct sk_buff *skb,
			   struct netlink_callback *cb, int type,
			   const struct tc_action_ops *ops,
			   struct netlink_ext_ack *extack)
{
	struct tc_action_net *tn = net_generic(net, csum_net_id);

	return tcf_generic_walker(tn, skb, cb, type, ops, extack);
}

static int tcf_csum_search(struct net *net, struct tc_action **a, u32 index)
{
	struct tc_action_net *tn = net_generic(net, csum_net_id);

	return tcf_idr_search(tn, a, index);
}

static size_t tcf_csum_get_fill_size(const struct tc_action *act)
{
	return nla_total_size(sizeof(struct tc_csum));
}

static struct tc_action_ops act_csum_ops = {
	.kind		= "csum",
	.id		= TCA_ID_CSUM,
	.owner		= THIS_MODULE,
	.act		= tcf_csum_act,
	.dump		= tcf_csum_dump,
	.init		= tcf_csum_init,
	.cleanup	= tcf_csum_cleanup,
	.walk		= tcf_csum_walker,
	.lookup		= tcf_csum_search,
	.get_fill_size  = tcf_csum_get_fill_size,
	.size		= sizeof(struct tcf_csum),
};

static __net_init int csum_init_net(struct net *net)
{
	struct tc_action_net *tn = net_generic(net, csum_net_id);

	return tc_action_net_init(net, tn, &act_csum_ops);
}

static void __net_exit csum_exit_net(struct list_head *net_list)
{
	tc_action_net_exit(net_list, csum_net_id);
}

static struct pernet_operations csum_net_ops = {
	.init = csum_init_net,
	.exit_batch = csum_exit_net,
	.id   = &csum_net_id,
	.size = sizeof(struct tc_action_net),
};

MODULE_DESCRIPTION("Checksum updating actions");
MODULE_LICENSE("GPL");

static int __init csum_init_module(void)
{
	return tcf_register_action(&act_csum_ops, &csum_net_ops);
}

static void __exit csum_cleanup_module(void)
{
	tcf_unregister_action(&act_csum_ops, &csum_net_ops);
}

module_init(csum_init_module);
module_exit(csum_cleanup_module);
