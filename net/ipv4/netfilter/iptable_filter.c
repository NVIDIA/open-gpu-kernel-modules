// SPDX-License-Identifier: GPL-2.0-only
/*
 * This is the 1999 rewrite of IP Firewalling, aiming for kernel 2.3.x.
 *
 * Copyright (C) 1999 Paul `Rusty' Russell & Michael J. Neuling
 * Copyright (C) 2000-2004 Netfilter Core Team <coreteam@netfilter.org>
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/slab.h>
#include <net/ip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables filter table");

#define FILTER_VALID_HOOKS ((1 << NF_INET_LOCAL_IN) | \
			    (1 << NF_INET_FORWARD) | \
			    (1 << NF_INET_LOCAL_OUT))
static int __net_init iptable_filter_table_init(struct net *net);

static const struct xt_table packet_filter = {
	.name		= "filter",
	.valid_hooks	= FILTER_VALID_HOOKS,
	.me		= THIS_MODULE,
	.af		= NFPROTO_IPV4,
	.priority	= NF_IP_PRI_FILTER,
	.table_init	= iptable_filter_table_init,
};

static unsigned int
iptable_filter_hook(void *priv, struct sk_buff *skb,
		    const struct nf_hook_state *state)
{
	return ipt_do_table(skb, state, priv);
}

static struct nf_hook_ops *filter_ops __read_mostly;

/* Default to forward because I got too much mail already. */
static bool forward __read_mostly = true;
module_param(forward, bool, 0000);

static int __net_init iptable_filter_table_init(struct net *net)
{
	struct ipt_replace *repl;
	int err;

	repl = ipt_alloc_initial_table(&packet_filter);
	if (repl == NULL)
		return -ENOMEM;
	/* Entry 1 is the FORWARD hook */
	((struct ipt_standard *)repl->entries)[1].target.verdict =
		forward ? -NF_ACCEPT - 1 : -NF_DROP - 1;

	err = ipt_register_table(net, &packet_filter, repl, filter_ops);
	kfree(repl);
	return err;
}

static int __net_init iptable_filter_net_init(struct net *net)
{
	if (net == &init_net || !forward)
		return iptable_filter_table_init(net);

	return 0;
}

static void __net_exit iptable_filter_net_pre_exit(struct net *net)
{
	ipt_unregister_table_pre_exit(net, "filter");
}

static void __net_exit iptable_filter_net_exit(struct net *net)
{
	ipt_unregister_table_exit(net, "filter");
}

static struct pernet_operations iptable_filter_net_ops = {
	.init = iptable_filter_net_init,
	.pre_exit = iptable_filter_net_pre_exit,
	.exit = iptable_filter_net_exit,
};

static int __init iptable_filter_init(void)
{
	int ret;

	filter_ops = xt_hook_ops_alloc(&packet_filter, iptable_filter_hook);
	if (IS_ERR(filter_ops))
		return PTR_ERR(filter_ops);

	ret = register_pernet_subsys(&iptable_filter_net_ops);
	if (ret < 0)
		kfree(filter_ops);

	return ret;
}

static void __exit iptable_filter_fini(void)
{
	unregister_pernet_subsys(&iptable_filter_net_ops);
	kfree(filter_ops);
}

module_init(iptable_filter_init);
module_exit(iptable_filter_fini);
