// SPDX-License-Identifier: GPL-2.0-only
/* module that allows mangling of the arp payload */
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_arp/arpt_mangle.h>
#include <net/sock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bart De Schuymer <bdschuym@pandora.be>");
MODULE_DESCRIPTION("arptables arp payload mangle target");

static unsigned int
target(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct arpt_mangle *mangle = par->targinfo;
	const struct arphdr *arp;
	unsigned char *arpptr;
	int pln, hln;

	if (skb_ensure_writable(skb, skb->len))
		return NF_DROP;

	arp = arp_hdr(skb);
	arpptr = skb_network_header(skb) + sizeof(*arp);
	pln = arp->ar_pln;
	hln = arp->ar_hln;
	/* We assume that pln and hln were checked in the match */
	if (mangle->flags & ARPT_MANGLE_SDEV) {
		if (ARPT_DEV_ADDR_LEN_MAX < hln ||
		   (arpptr + hln > skb_tail_pointer(skb)))
			return NF_DROP;
		memcpy(arpptr, mangle->src_devaddr, hln);
	}
	arpptr += hln;
	if (mangle->flags & ARPT_MANGLE_SIP) {
		if (ARPT_MANGLE_ADDR_LEN_MAX < pln ||
		   (arpptr + pln > skb_tail_pointer(skb)))
			return NF_DROP;
		memcpy(arpptr, &mangle->u_s.src_ip, pln);
	}
	arpptr += pln;
	if (mangle->flags & ARPT_MANGLE_TDEV) {
		if (ARPT_DEV_ADDR_LEN_MAX < hln ||
		   (arpptr + hln > skb_tail_pointer(skb)))
			return NF_DROP;
		memcpy(arpptr, mangle->tgt_devaddr, hln);
	}
	arpptr += hln;
	if (mangle->flags & ARPT_MANGLE_TIP) {
		if (ARPT_MANGLE_ADDR_LEN_MAX < pln ||
		   (arpptr + pln > skb_tail_pointer(skb)))
			return NF_DROP;
		memcpy(arpptr, &mangle->u_t.tgt_ip, pln);
	}
	return mangle->target;
}

static int checkentry(const struct xt_tgchk_param *par)
{
	const struct arpt_mangle *mangle = par->targinfo;

	if (mangle->flags & ~ARPT_MANGLE_MASK ||
	    !(mangle->flags & ARPT_MANGLE_MASK))
		return -EINVAL;

	if (mangle->target != NF_DROP && mangle->target != NF_ACCEPT &&
	   mangle->target != XT_CONTINUE)
		return -EINVAL;
	return 0;
}

static struct xt_target arpt_mangle_reg __read_mostly = {
	.name		= "mangle",
	.family		= NFPROTO_ARP,
	.target		= target,
	.targetsize	= sizeof(struct arpt_mangle),
	.checkentry	= checkentry,
	.me		= THIS_MODULE,
};

static int __init arpt_mangle_init(void)
{
	return xt_register_target(&arpt_mangle_reg);
}

static void __exit arpt_mangle_fini(void)
{
	xt_unregister_target(&arpt_mangle_reg);
}

module_init(arpt_mangle_init);
module_exit(arpt_mangle_fini);
