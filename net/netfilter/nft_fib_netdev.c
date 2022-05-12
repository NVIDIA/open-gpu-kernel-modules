// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017 Pablo M. Bermudo Garay <pablombg@gmail.com>
 *
 * This code is based on net/netfilter/nft_fib_inet.c, written by
 * Florian Westphal <fw@strlen.de>.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables_core.h>
#include <net/netfilter/nf_tables.h>
#include <net/ipv6.h>

#include <net/netfilter/nft_fib.h>

static void nft_fib_netdev_eval(const struct nft_expr *expr,
				struct nft_regs *regs,
				const struct nft_pktinfo *pkt)
{
	const struct nft_fib *priv = nft_expr_priv(expr);

	switch (ntohs(pkt->skb->protocol)) {
	case ETH_P_IP:
		switch (priv->result) {
		case NFT_FIB_RESULT_OIF:
		case NFT_FIB_RESULT_OIFNAME:
			return nft_fib4_eval(expr, regs, pkt);
		case NFT_FIB_RESULT_ADDRTYPE:
			return nft_fib4_eval_type(expr, regs, pkt);
		}
		break;
	case ETH_P_IPV6:
		if (!ipv6_mod_enabled())
			break;
		switch (priv->result) {
		case NFT_FIB_RESULT_OIF:
		case NFT_FIB_RESULT_OIFNAME:
			return nft_fib6_eval(expr, regs, pkt);
		case NFT_FIB_RESULT_ADDRTYPE:
			return nft_fib6_eval_type(expr, regs, pkt);
		}
		break;
	}

	regs->verdict.code = NFT_BREAK;
}

static struct nft_expr_type nft_fib_netdev_type;
static const struct nft_expr_ops nft_fib_netdev_ops = {
	.type		= &nft_fib_netdev_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_fib)),
	.eval		= nft_fib_netdev_eval,
	.init		= nft_fib_init,
	.dump		= nft_fib_dump,
	.validate	= nft_fib_validate,
};

static struct nft_expr_type nft_fib_netdev_type __read_mostly = {
	.family		= NFPROTO_NETDEV,
	.name		= "fib",
	.ops		= &nft_fib_netdev_ops,
	.policy		= nft_fib_policy,
	.maxattr	= NFTA_FIB_MAX,
	.owner		= THIS_MODULE,
};

static int __init nft_fib_netdev_module_init(void)
{
	return nft_register_expr(&nft_fib_netdev_type);
}

static void __exit nft_fib_netdev_module_exit(void)
{
	nft_unregister_expr(&nft_fib_netdev_type);
}

module_init(nft_fib_netdev_module_init);
module_exit(nft_fib_netdev_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pablo M. Bermudo Garay <pablombg@gmail.com>");
MODULE_ALIAS_NFT_AF_EXPR(5, "fib");
MODULE_DESCRIPTION("nftables netdev fib lookups support");
