// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015 Pablo Neira Ayuso <pablo@netfilter.org>
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables.h>
#include <net/netfilter/ipv6/nf_dup_ipv6.h>

struct nft_dup_ipv6 {
	u8	sreg_addr;
	u8	sreg_dev;
};

static void nft_dup_ipv6_eval(const struct nft_expr *expr,
			      struct nft_regs *regs,
			      const struct nft_pktinfo *pkt)
{
	struct nft_dup_ipv6 *priv = nft_expr_priv(expr);
	struct in6_addr *gw = (struct in6_addr *)&regs->data[priv->sreg_addr];
	int oif = priv->sreg_dev ? regs->data[priv->sreg_dev] : -1;

	nf_dup_ipv6(nft_net(pkt), pkt->skb, nft_hook(pkt), gw, oif);
}

static int nft_dup_ipv6_init(const struct nft_ctx *ctx,
			     const struct nft_expr *expr,
			     const struct nlattr * const tb[])
{
	struct nft_dup_ipv6 *priv = nft_expr_priv(expr);
	int err;

	if (tb[NFTA_DUP_SREG_ADDR] == NULL)
		return -EINVAL;

	err = nft_parse_register_load(tb[NFTA_DUP_SREG_ADDR], &priv->sreg_addr,
				      sizeof(struct in6_addr));
	if (err < 0)
		return err;

	if (tb[NFTA_DUP_SREG_DEV])
		err = nft_parse_register_load(tb[NFTA_DUP_SREG_DEV],
					      &priv->sreg_dev, sizeof(int));

	return err;
}

static int nft_dup_ipv6_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	struct nft_dup_ipv6 *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_DUP_SREG_ADDR, priv->sreg_addr))
		goto nla_put_failure;
	if (priv->sreg_dev &&
	    nft_dump_register(skb, NFTA_DUP_SREG_DEV, priv->sreg_dev))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}

static struct nft_expr_type nft_dup_ipv6_type;
static const struct nft_expr_ops nft_dup_ipv6_ops = {
	.type		= &nft_dup_ipv6_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_dup_ipv6)),
	.eval		= nft_dup_ipv6_eval,
	.init		= nft_dup_ipv6_init,
	.dump		= nft_dup_ipv6_dump,
};

static const struct nla_policy nft_dup_ipv6_policy[NFTA_DUP_MAX + 1] = {
	[NFTA_DUP_SREG_ADDR]	= { .type = NLA_U32 },
	[NFTA_DUP_SREG_DEV]	= { .type = NLA_U32 },
};

static struct nft_expr_type nft_dup_ipv6_type __read_mostly = {
	.family		= NFPROTO_IPV6,
	.name		= "dup",
	.ops		= &nft_dup_ipv6_ops,
	.policy		= nft_dup_ipv6_policy,
	.maxattr	= NFTA_DUP_MAX,
	.owner		= THIS_MODULE,
};

static int __init nft_dup_ipv6_module_init(void)
{
	return nft_register_expr(&nft_dup_ipv6_type);
}

static void __exit nft_dup_ipv6_module_exit(void)
{
	nft_unregister_expr(&nft_dup_ipv6_type);
}

module_init(nft_dup_ipv6_module_init);
module_exit(nft_dup_ipv6_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pablo Neira Ayuso <pablo@netfilter.org>");
MODULE_ALIAS_NFT_AF_EXPR(AF_INET6, "dup");
MODULE_DESCRIPTION("IPv6 nftables packet duplication support");
