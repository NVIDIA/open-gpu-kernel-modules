// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2006 Patrick McHardy <kaber@trash.net>
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/skbuff.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_NFLOG.h>
#include <net/netfilter/nf_log.h>

MODULE_AUTHOR("Patrick McHardy <kaber@trash.net>");
MODULE_DESCRIPTION("Xtables: packet logging to netlink using NFLOG");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_NFLOG");
MODULE_ALIAS("ip6t_NFLOG");

static unsigned int
nflog_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_nflog_info *info = par->targinfo;
	struct net *net = xt_net(par);
	struct nf_loginfo li;

	li.type		     = NF_LOG_TYPE_ULOG;
	li.u.ulog.copy_len   = info->len;
	li.u.ulog.group	     = info->group;
	li.u.ulog.qthreshold = info->threshold;
	li.u.ulog.flags	     = 0;

	if (info->flags & XT_NFLOG_F_COPY_LEN)
		li.u.ulog.flags |= NF_LOG_F_COPY_LEN;

	nf_log_packet(net, xt_family(par), xt_hooknum(par), skb, xt_in(par),
		      xt_out(par), &li, "%s", info->prefix);

	return XT_CONTINUE;
}

static int nflog_tg_check(const struct xt_tgchk_param *par)
{
	const struct xt_nflog_info *info = par->targinfo;

	if (info->flags & ~XT_NFLOG_MASK)
		return -EINVAL;
	if (info->prefix[sizeof(info->prefix) - 1] != '\0')
		return -EINVAL;

	return nf_logger_find_get(par->family, NF_LOG_TYPE_ULOG);
}

static void nflog_tg_destroy(const struct xt_tgdtor_param *par)
{
	nf_logger_put(par->family, NF_LOG_TYPE_ULOG);
}

static struct xt_target nflog_tg_reg __read_mostly = {
	.name       = "NFLOG",
	.revision   = 0,
	.family     = NFPROTO_UNSPEC,
	.checkentry = nflog_tg_check,
	.destroy    = nflog_tg_destroy,
	.target     = nflog_tg,
	.targetsize = sizeof(struct xt_nflog_info),
	.me         = THIS_MODULE,
};

static int __init nflog_tg_init(void)
{
	return xt_register_target(&nflog_tg_reg);
}

static void __exit nflog_tg_exit(void)
{
	xt_unregister_target(&nflog_tg_reg);
}

module_init(nflog_tg_init);
module_exit(nflog_tg_exit);
MODULE_SOFTDEP("pre: nfnetlink_log");
