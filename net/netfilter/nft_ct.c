// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2008-2009 Patrick McHardy <kaber@trash.net>
 * Copyright (c) 2016 Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * Development of this code funded by Astaro AG (http://www.astaro.com/)
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_acct.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_labels.h>
#include <net/netfilter/nf_conntrack_timeout.h>
#include <net/netfilter/nf_conntrack_l4proto.h>
#include <net/netfilter/nf_conntrack_expect.h>

struct nft_ct {
	enum nft_ct_keys	key:8;
	enum ip_conntrack_dir	dir:8;
	union {
		u8		dreg;
		u8		sreg;
	};
};

struct nft_ct_helper_obj  {
	struct nf_conntrack_helper *helper4;
	struct nf_conntrack_helper *helper6;
	u8 l4proto;
};

#ifdef CONFIG_NF_CONNTRACK_ZONES
static DEFINE_PER_CPU(struct nf_conn *, nft_ct_pcpu_template);
static unsigned int nft_ct_pcpu_template_refcnt __read_mostly;
#endif

static u64 nft_ct_get_eval_counter(const struct nf_conn_counter *c,
				   enum nft_ct_keys k,
				   enum ip_conntrack_dir d)
{
	if (d < IP_CT_DIR_MAX)
		return k == NFT_CT_BYTES ? atomic64_read(&c[d].bytes) :
					   atomic64_read(&c[d].packets);

	return nft_ct_get_eval_counter(c, k, IP_CT_DIR_ORIGINAL) +
	       nft_ct_get_eval_counter(c, k, IP_CT_DIR_REPLY);
}

static void nft_ct_get_eval(const struct nft_expr *expr,
			    struct nft_regs *regs,
			    const struct nft_pktinfo *pkt)
{
	const struct nft_ct *priv = nft_expr_priv(expr);
	u32 *dest = &regs->data[priv->dreg];
	enum ip_conntrack_info ctinfo;
	const struct nf_conn *ct;
	const struct nf_conn_help *help;
	const struct nf_conntrack_tuple *tuple;
	const struct nf_conntrack_helper *helper;
	unsigned int state;

	ct = nf_ct_get(pkt->skb, &ctinfo);

	switch (priv->key) {
	case NFT_CT_STATE:
		if (ct)
			state = NF_CT_STATE_BIT(ctinfo);
		else if (ctinfo == IP_CT_UNTRACKED)
			state = NF_CT_STATE_UNTRACKED_BIT;
		else
			state = NF_CT_STATE_INVALID_BIT;
		*dest = state;
		return;
	default:
		break;
	}

	if (ct == NULL)
		goto err;

	switch (priv->key) {
	case NFT_CT_DIRECTION:
		nft_reg_store8(dest, CTINFO2DIR(ctinfo));
		return;
	case NFT_CT_STATUS:
		*dest = ct->status;
		return;
#ifdef CONFIG_NF_CONNTRACK_MARK
	case NFT_CT_MARK:
		*dest = ct->mark;
		return;
#endif
#ifdef CONFIG_NF_CONNTRACK_SECMARK
	case NFT_CT_SECMARK:
		*dest = ct->secmark;
		return;
#endif
	case NFT_CT_EXPIRATION:
		*dest = jiffies_to_msecs(nf_ct_expires(ct));
		return;
	case NFT_CT_HELPER:
		if (ct->master == NULL)
			goto err;
		help = nfct_help(ct->master);
		if (help == NULL)
			goto err;
		helper = rcu_dereference(help->helper);
		if (helper == NULL)
			goto err;
		strncpy((char *)dest, helper->name, NF_CT_HELPER_NAME_LEN);
		return;
#ifdef CONFIG_NF_CONNTRACK_LABELS
	case NFT_CT_LABELS: {
		struct nf_conn_labels *labels = nf_ct_labels_find(ct);

		if (labels)
			memcpy(dest, labels->bits, NF_CT_LABELS_MAX_SIZE);
		else
			memset(dest, 0, NF_CT_LABELS_MAX_SIZE);
		return;
	}
#endif
	case NFT_CT_BYTES:
	case NFT_CT_PKTS: {
		const struct nf_conn_acct *acct = nf_conn_acct_find(ct);
		u64 count = 0;

		if (acct)
			count = nft_ct_get_eval_counter(acct->counter,
							priv->key, priv->dir);
		memcpy(dest, &count, sizeof(count));
		return;
	}
	case NFT_CT_AVGPKT: {
		const struct nf_conn_acct *acct = nf_conn_acct_find(ct);
		u64 avgcnt = 0, bcnt = 0, pcnt = 0;

		if (acct) {
			pcnt = nft_ct_get_eval_counter(acct->counter,
						       NFT_CT_PKTS, priv->dir);
			bcnt = nft_ct_get_eval_counter(acct->counter,
						       NFT_CT_BYTES, priv->dir);
			if (pcnt != 0)
				avgcnt = div64_u64(bcnt, pcnt);
		}

		memcpy(dest, &avgcnt, sizeof(avgcnt));
		return;
	}
	case NFT_CT_L3PROTOCOL:
		nft_reg_store8(dest, nf_ct_l3num(ct));
		return;
	case NFT_CT_PROTOCOL:
		nft_reg_store8(dest, nf_ct_protonum(ct));
		return;
#ifdef CONFIG_NF_CONNTRACK_ZONES
	case NFT_CT_ZONE: {
		const struct nf_conntrack_zone *zone = nf_ct_zone(ct);
		u16 zoneid;

		if (priv->dir < IP_CT_DIR_MAX)
			zoneid = nf_ct_zone_id(zone, priv->dir);
		else
			zoneid = zone->id;

		nft_reg_store16(dest, zoneid);
		return;
	}
#endif
	case NFT_CT_ID:
		*dest = nf_ct_get_id(ct);
		return;
	default:
		break;
	}

	tuple = &ct->tuplehash[priv->dir].tuple;
	switch (priv->key) {
	case NFT_CT_SRC:
		memcpy(dest, tuple->src.u3.all,
		       nf_ct_l3num(ct) == NFPROTO_IPV4 ? 4 : 16);
		return;
	case NFT_CT_DST:
		memcpy(dest, tuple->dst.u3.all,
		       nf_ct_l3num(ct) == NFPROTO_IPV4 ? 4 : 16);
		return;
	case NFT_CT_PROTO_SRC:
		nft_reg_store16(dest, (__force u16)tuple->src.u.all);
		return;
	case NFT_CT_PROTO_DST:
		nft_reg_store16(dest, (__force u16)tuple->dst.u.all);
		return;
	case NFT_CT_SRC_IP:
		if (nf_ct_l3num(ct) != NFPROTO_IPV4)
			goto err;
		*dest = tuple->src.u3.ip;
		return;
	case NFT_CT_DST_IP:
		if (nf_ct_l3num(ct) != NFPROTO_IPV4)
			goto err;
		*dest = tuple->dst.u3.ip;
		return;
	case NFT_CT_SRC_IP6:
		if (nf_ct_l3num(ct) != NFPROTO_IPV6)
			goto err;
		memcpy(dest, tuple->src.u3.ip6, sizeof(struct in6_addr));
		return;
	case NFT_CT_DST_IP6:
		if (nf_ct_l3num(ct) != NFPROTO_IPV6)
			goto err;
		memcpy(dest, tuple->dst.u3.ip6, sizeof(struct in6_addr));
		return;
	default:
		break;
	}
	return;
err:
	regs->verdict.code = NFT_BREAK;
}

#ifdef CONFIG_NF_CONNTRACK_ZONES
static void nft_ct_set_zone_eval(const struct nft_expr *expr,
				 struct nft_regs *regs,
				 const struct nft_pktinfo *pkt)
{
	struct nf_conntrack_zone zone = { .dir = NF_CT_DEFAULT_ZONE_DIR };
	const struct nft_ct *priv = nft_expr_priv(expr);
	struct sk_buff *skb = pkt->skb;
	enum ip_conntrack_info ctinfo;
	u16 value = nft_reg_load16(&regs->data[priv->sreg]);
	struct nf_conn *ct;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct) /* already tracked */
		return;

	zone.id = value;

	switch (priv->dir) {
	case IP_CT_DIR_ORIGINAL:
		zone.dir = NF_CT_ZONE_DIR_ORIG;
		break;
	case IP_CT_DIR_REPLY:
		zone.dir = NF_CT_ZONE_DIR_REPL;
		break;
	default:
		break;
	}

	ct = this_cpu_read(nft_ct_pcpu_template);

	if (likely(atomic_read(&ct->ct_general.use) == 1)) {
		nf_ct_zone_add(ct, &zone);
	} else {
		/* previous skb got queued to userspace */
		ct = nf_ct_tmpl_alloc(nft_net(pkt), &zone, GFP_ATOMIC);
		if (!ct) {
			regs->verdict.code = NF_DROP;
			return;
		}
	}

	atomic_inc(&ct->ct_general.use);
	nf_ct_set(skb, ct, IP_CT_NEW);
}
#endif

static void nft_ct_set_eval(const struct nft_expr *expr,
			    struct nft_regs *regs,
			    const struct nft_pktinfo *pkt)
{
	const struct nft_ct *priv = nft_expr_priv(expr);
	struct sk_buff *skb = pkt->skb;
#if defined(CONFIG_NF_CONNTRACK_MARK) || defined(CONFIG_NF_CONNTRACK_SECMARK)
	u32 value = regs->data[priv->sreg];
#endif
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct == NULL || nf_ct_is_template(ct))
		return;

	switch (priv->key) {
#ifdef CONFIG_NF_CONNTRACK_MARK
	case NFT_CT_MARK:
		if (ct->mark != value) {
			ct->mark = value;
			nf_conntrack_event_cache(IPCT_MARK, ct);
		}
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_SECMARK
	case NFT_CT_SECMARK:
		if (ct->secmark != value) {
			ct->secmark = value;
			nf_conntrack_event_cache(IPCT_SECMARK, ct);
		}
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_LABELS
	case NFT_CT_LABELS:
		nf_connlabels_replace(ct,
				      &regs->data[priv->sreg],
				      &regs->data[priv->sreg],
				      NF_CT_LABELS_MAX_SIZE / sizeof(u32));
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_EVENTS
	case NFT_CT_EVENTMASK: {
		struct nf_conntrack_ecache *e = nf_ct_ecache_find(ct);
		u32 ctmask = regs->data[priv->sreg];

		if (e) {
			if (e->ctmask != ctmask)
				e->ctmask = ctmask;
			break;
		}

		if (ctmask && !nf_ct_is_confirmed(ct))
			nf_ct_ecache_ext_add(ct, ctmask, 0, GFP_ATOMIC);
		break;
	}
#endif
	default:
		break;
	}
}

static const struct nla_policy nft_ct_policy[NFTA_CT_MAX + 1] = {
	[NFTA_CT_DREG]		= { .type = NLA_U32 },
	[NFTA_CT_KEY]		= { .type = NLA_U32 },
	[NFTA_CT_DIRECTION]	= { .type = NLA_U8 },
	[NFTA_CT_SREG]		= { .type = NLA_U32 },
};

#ifdef CONFIG_NF_CONNTRACK_ZONES
static void nft_ct_tmpl_put_pcpu(void)
{
	struct nf_conn *ct;
	int cpu;

	for_each_possible_cpu(cpu) {
		ct = per_cpu(nft_ct_pcpu_template, cpu);
		if (!ct)
			break;
		nf_ct_put(ct);
		per_cpu(nft_ct_pcpu_template, cpu) = NULL;
	}
}

static bool nft_ct_tmpl_alloc_pcpu(void)
{
	struct nf_conntrack_zone zone = { .id = 0 };
	struct nf_conn *tmp;
	int cpu;

	if (nft_ct_pcpu_template_refcnt)
		return true;

	for_each_possible_cpu(cpu) {
		tmp = nf_ct_tmpl_alloc(&init_net, &zone, GFP_KERNEL);
		if (!tmp) {
			nft_ct_tmpl_put_pcpu();
			return false;
		}

		atomic_set(&tmp->ct_general.use, 1);
		per_cpu(nft_ct_pcpu_template, cpu) = tmp;
	}

	return true;
}
#endif

static int nft_ct_get_init(const struct nft_ctx *ctx,
			   const struct nft_expr *expr,
			   const struct nlattr * const tb[])
{
	struct nft_ct *priv = nft_expr_priv(expr);
	unsigned int len;
	int err;

	priv->key = ntohl(nla_get_be32(tb[NFTA_CT_KEY]));
	priv->dir = IP_CT_DIR_MAX;
	switch (priv->key) {
	case NFT_CT_DIRECTION:
		if (tb[NFTA_CT_DIRECTION] != NULL)
			return -EINVAL;
		len = sizeof(u8);
		break;
	case NFT_CT_STATE:
	case NFT_CT_STATUS:
#ifdef CONFIG_NF_CONNTRACK_MARK
	case NFT_CT_MARK:
#endif
#ifdef CONFIG_NF_CONNTRACK_SECMARK
	case NFT_CT_SECMARK:
#endif
	case NFT_CT_EXPIRATION:
		if (tb[NFTA_CT_DIRECTION] != NULL)
			return -EINVAL;
		len = sizeof(u32);
		break;
#ifdef CONFIG_NF_CONNTRACK_LABELS
	case NFT_CT_LABELS:
		if (tb[NFTA_CT_DIRECTION] != NULL)
			return -EINVAL;
		len = NF_CT_LABELS_MAX_SIZE;
		break;
#endif
	case NFT_CT_HELPER:
		if (tb[NFTA_CT_DIRECTION] != NULL)
			return -EINVAL;
		len = NF_CT_HELPER_NAME_LEN;
		break;

	case NFT_CT_L3PROTOCOL:
	case NFT_CT_PROTOCOL:
		/* For compatibility, do not report error if NFTA_CT_DIRECTION
		 * attribute is specified.
		 */
		len = sizeof(u8);
		break;
	case NFT_CT_SRC:
	case NFT_CT_DST:
		if (tb[NFTA_CT_DIRECTION] == NULL)
			return -EINVAL;

		switch (ctx->family) {
		case NFPROTO_IPV4:
			len = sizeof_field(struct nf_conntrack_tuple,
					   src.u3.ip);
			break;
		case NFPROTO_IPV6:
		case NFPROTO_INET:
			len = sizeof_field(struct nf_conntrack_tuple,
					   src.u3.ip6);
			break;
		default:
			return -EAFNOSUPPORT;
		}
		break;
	case NFT_CT_SRC_IP:
	case NFT_CT_DST_IP:
		if (tb[NFTA_CT_DIRECTION] == NULL)
			return -EINVAL;

		len = sizeof_field(struct nf_conntrack_tuple, src.u3.ip);
		break;
	case NFT_CT_SRC_IP6:
	case NFT_CT_DST_IP6:
		if (tb[NFTA_CT_DIRECTION] == NULL)
			return -EINVAL;

		len = sizeof_field(struct nf_conntrack_tuple, src.u3.ip6);
		break;
	case NFT_CT_PROTO_SRC:
	case NFT_CT_PROTO_DST:
		if (tb[NFTA_CT_DIRECTION] == NULL)
			return -EINVAL;
		len = sizeof_field(struct nf_conntrack_tuple, src.u.all);
		break;
	case NFT_CT_BYTES:
	case NFT_CT_PKTS:
	case NFT_CT_AVGPKT:
		len = sizeof(u64);
		break;
#ifdef CONFIG_NF_CONNTRACK_ZONES
	case NFT_CT_ZONE:
		len = sizeof(u16);
		break;
#endif
	case NFT_CT_ID:
		len = sizeof(u32);
		break;
	default:
		return -EOPNOTSUPP;
	}

	if (tb[NFTA_CT_DIRECTION] != NULL) {
		priv->dir = nla_get_u8(tb[NFTA_CT_DIRECTION]);
		switch (priv->dir) {
		case IP_CT_DIR_ORIGINAL:
		case IP_CT_DIR_REPLY:
			break;
		default:
			return -EINVAL;
		}
	}

	err = nft_parse_register_store(ctx, tb[NFTA_CT_DREG], &priv->dreg, NULL,
				       NFT_DATA_VALUE, len);
	if (err < 0)
		return err;

	err = nf_ct_netns_get(ctx->net, ctx->family);
	if (err < 0)
		return err;

	if (priv->key == NFT_CT_BYTES ||
	    priv->key == NFT_CT_PKTS  ||
	    priv->key == NFT_CT_AVGPKT)
		nf_ct_set_acct(ctx->net, true);

	return 0;
}

static void __nft_ct_set_destroy(const struct nft_ctx *ctx, struct nft_ct *priv)
{
	switch (priv->key) {
#ifdef CONFIG_NF_CONNTRACK_LABELS
	case NFT_CT_LABELS:
		nf_connlabels_put(ctx->net);
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_ZONES
	case NFT_CT_ZONE:
		if (--nft_ct_pcpu_template_refcnt == 0)
			nft_ct_tmpl_put_pcpu();
		break;
#endif
	default:
		break;
	}
}

static int nft_ct_set_init(const struct nft_ctx *ctx,
			   const struct nft_expr *expr,
			   const struct nlattr * const tb[])
{
	struct nft_ct *priv = nft_expr_priv(expr);
	unsigned int len;
	int err;

	priv->dir = IP_CT_DIR_MAX;
	priv->key = ntohl(nla_get_be32(tb[NFTA_CT_KEY]));
	switch (priv->key) {
#ifdef CONFIG_NF_CONNTRACK_MARK
	case NFT_CT_MARK:
		if (tb[NFTA_CT_DIRECTION])
			return -EINVAL;
		len = sizeof_field(struct nf_conn, mark);
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_LABELS
	case NFT_CT_LABELS:
		if (tb[NFTA_CT_DIRECTION])
			return -EINVAL;
		len = NF_CT_LABELS_MAX_SIZE;
		err = nf_connlabels_get(ctx->net, (len * BITS_PER_BYTE) - 1);
		if (err)
			return err;
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_ZONES
	case NFT_CT_ZONE:
		if (!nft_ct_tmpl_alloc_pcpu())
			return -ENOMEM;
		nft_ct_pcpu_template_refcnt++;
		len = sizeof(u16);
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_EVENTS
	case NFT_CT_EVENTMASK:
		if (tb[NFTA_CT_DIRECTION])
			return -EINVAL;
		len = sizeof(u32);
		break;
#endif
#ifdef CONFIG_NF_CONNTRACK_SECMARK
	case NFT_CT_SECMARK:
		if (tb[NFTA_CT_DIRECTION])
			return -EINVAL;
		len = sizeof(u32);
		break;
#endif
	default:
		return -EOPNOTSUPP;
	}

	if (tb[NFTA_CT_DIRECTION]) {
		priv->dir = nla_get_u8(tb[NFTA_CT_DIRECTION]);
		switch (priv->dir) {
		case IP_CT_DIR_ORIGINAL:
		case IP_CT_DIR_REPLY:
			break;
		default:
			err = -EINVAL;
			goto err1;
		}
	}

	err = nft_parse_register_load(tb[NFTA_CT_SREG], &priv->sreg, len);
	if (err < 0)
		goto err1;

	err = nf_ct_netns_get(ctx->net, ctx->family);
	if (err < 0)
		goto err1;

	return 0;

err1:
	__nft_ct_set_destroy(ctx, priv);
	return err;
}

static void nft_ct_get_destroy(const struct nft_ctx *ctx,
			       const struct nft_expr *expr)
{
	nf_ct_netns_put(ctx->net, ctx->family);
}

static void nft_ct_set_destroy(const struct nft_ctx *ctx,
			       const struct nft_expr *expr)
{
	struct nft_ct *priv = nft_expr_priv(expr);

	__nft_ct_set_destroy(ctx, priv);
	nf_ct_netns_put(ctx->net, ctx->family);
}

static int nft_ct_get_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	const struct nft_ct *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_CT_DREG, priv->dreg))
		goto nla_put_failure;
	if (nla_put_be32(skb, NFTA_CT_KEY, htonl(priv->key)))
		goto nla_put_failure;

	switch (priv->key) {
	case NFT_CT_SRC:
	case NFT_CT_DST:
	case NFT_CT_SRC_IP:
	case NFT_CT_DST_IP:
	case NFT_CT_SRC_IP6:
	case NFT_CT_DST_IP6:
	case NFT_CT_PROTO_SRC:
	case NFT_CT_PROTO_DST:
		if (nla_put_u8(skb, NFTA_CT_DIRECTION, priv->dir))
			goto nla_put_failure;
		break;
	case NFT_CT_BYTES:
	case NFT_CT_PKTS:
	case NFT_CT_AVGPKT:
	case NFT_CT_ZONE:
		if (priv->dir < IP_CT_DIR_MAX &&
		    nla_put_u8(skb, NFTA_CT_DIRECTION, priv->dir))
			goto nla_put_failure;
		break;
	default:
		break;
	}

	return 0;

nla_put_failure:
	return -1;
}

static int nft_ct_set_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	const struct nft_ct *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_CT_SREG, priv->sreg))
		goto nla_put_failure;
	if (nla_put_be32(skb, NFTA_CT_KEY, htonl(priv->key)))
		goto nla_put_failure;

	switch (priv->key) {
	case NFT_CT_ZONE:
		if (priv->dir < IP_CT_DIR_MAX &&
		    nla_put_u8(skb, NFTA_CT_DIRECTION, priv->dir))
			goto nla_put_failure;
		break;
	default:
		break;
	}

	return 0;

nla_put_failure:
	return -1;
}

static struct nft_expr_type nft_ct_type;
static const struct nft_expr_ops nft_ct_get_ops = {
	.type		= &nft_ct_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_ct)),
	.eval		= nft_ct_get_eval,
	.init		= nft_ct_get_init,
	.destroy	= nft_ct_get_destroy,
	.dump		= nft_ct_get_dump,
};

static const struct nft_expr_ops nft_ct_set_ops = {
	.type		= &nft_ct_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_ct)),
	.eval		= nft_ct_set_eval,
	.init		= nft_ct_set_init,
	.destroy	= nft_ct_set_destroy,
	.dump		= nft_ct_set_dump,
};

#ifdef CONFIG_NF_CONNTRACK_ZONES
static const struct nft_expr_ops nft_ct_set_zone_ops = {
	.type		= &nft_ct_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_ct)),
	.eval		= nft_ct_set_zone_eval,
	.init		= nft_ct_set_init,
	.destroy	= nft_ct_set_destroy,
	.dump		= nft_ct_set_dump,
};
#endif

static const struct nft_expr_ops *
nft_ct_select_ops(const struct nft_ctx *ctx,
		    const struct nlattr * const tb[])
{
	if (tb[NFTA_CT_KEY] == NULL)
		return ERR_PTR(-EINVAL);

	if (tb[NFTA_CT_DREG] && tb[NFTA_CT_SREG])
		return ERR_PTR(-EINVAL);

	if (tb[NFTA_CT_DREG])
		return &nft_ct_get_ops;

	if (tb[NFTA_CT_SREG]) {
#ifdef CONFIG_NF_CONNTRACK_ZONES
		if (nla_get_be32(tb[NFTA_CT_KEY]) == htonl(NFT_CT_ZONE))
			return &nft_ct_set_zone_ops;
#endif
		return &nft_ct_set_ops;
	}

	return ERR_PTR(-EINVAL);
}

static struct nft_expr_type nft_ct_type __read_mostly = {
	.name		= "ct",
	.select_ops	= nft_ct_select_ops,
	.policy		= nft_ct_policy,
	.maxattr	= NFTA_CT_MAX,
	.owner		= THIS_MODULE,
};

static void nft_notrack_eval(const struct nft_expr *expr,
			     struct nft_regs *regs,
			     const struct nft_pktinfo *pkt)
{
	struct sk_buff *skb = pkt->skb;
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct;

	ct = nf_ct_get(pkt->skb, &ctinfo);
	/* Previously seen (loopback or untracked)?  Ignore. */
	if (ct || ctinfo == IP_CT_UNTRACKED)
		return;

	nf_ct_set(skb, ct, IP_CT_UNTRACKED);
}

static struct nft_expr_type nft_notrack_type;
static const struct nft_expr_ops nft_notrack_ops = {
	.type		= &nft_notrack_type,
	.size		= NFT_EXPR_SIZE(0),
	.eval		= nft_notrack_eval,
};

static struct nft_expr_type nft_notrack_type __read_mostly = {
	.name		= "notrack",
	.ops		= &nft_notrack_ops,
	.owner		= THIS_MODULE,
};

#ifdef CONFIG_NF_CONNTRACK_TIMEOUT
static int
nft_ct_timeout_parse_policy(void *timeouts,
			    const struct nf_conntrack_l4proto *l4proto,
			    struct net *net, const struct nlattr *attr)
{
	struct nlattr **tb;
	int ret = 0;

	tb = kcalloc(l4proto->ctnl_timeout.nlattr_max + 1, sizeof(*tb),
		     GFP_KERNEL);

	if (!tb)
		return -ENOMEM;

	ret = nla_parse_nested_deprecated(tb,
					  l4proto->ctnl_timeout.nlattr_max,
					  attr,
					  l4proto->ctnl_timeout.nla_policy,
					  NULL);
	if (ret < 0)
		goto err;

	ret = l4proto->ctnl_timeout.nlattr_to_obj(tb, net, timeouts);

err:
	kfree(tb);
	return ret;
}

struct nft_ct_timeout_obj {
	struct nf_ct_timeout    *timeout;
	u8			l4proto;
};

static void nft_ct_timeout_obj_eval(struct nft_object *obj,
				    struct nft_regs *regs,
				    const struct nft_pktinfo *pkt)
{
	const struct nft_ct_timeout_obj *priv = nft_obj_data(obj);
	struct nf_conn *ct = (struct nf_conn *)skb_nfct(pkt->skb);
	struct nf_conn_timeout *timeout;
	const unsigned int *values;

	if (priv->l4proto != pkt->tprot)
		return;

	if (!ct || nf_ct_is_template(ct) || nf_ct_is_confirmed(ct))
		return;

	timeout = nf_ct_timeout_find(ct);
	if (!timeout) {
		timeout = nf_ct_timeout_ext_add(ct, priv->timeout, GFP_ATOMIC);
		if (!timeout) {
			regs->verdict.code = NF_DROP;
			return;
		}
	}

	rcu_assign_pointer(timeout->timeout, priv->timeout);

	/* adjust the timeout as per 'new' state. ct is unconfirmed,
	 * so the current timestamp must not be added.
	 */
	values = nf_ct_timeout_data(timeout);
	if (values)
		nf_ct_refresh(ct, pkt->skb, values[0]);
}

static int nft_ct_timeout_obj_init(const struct nft_ctx *ctx,
				   const struct nlattr * const tb[],
				   struct nft_object *obj)
{
	struct nft_ct_timeout_obj *priv = nft_obj_data(obj);
	const struct nf_conntrack_l4proto *l4proto;
	struct nf_ct_timeout *timeout;
	int l3num = ctx->family;
	__u8 l4num;
	int ret;

	if (!tb[NFTA_CT_TIMEOUT_L4PROTO] ||
	    !tb[NFTA_CT_TIMEOUT_DATA])
		return -EINVAL;

	if (tb[NFTA_CT_TIMEOUT_L3PROTO])
		l3num = ntohs(nla_get_be16(tb[NFTA_CT_TIMEOUT_L3PROTO]));

	l4num = nla_get_u8(tb[NFTA_CT_TIMEOUT_L4PROTO]);
	priv->l4proto = l4num;

	l4proto = nf_ct_l4proto_find(l4num);

	if (l4proto->l4proto != l4num) {
		ret = -EOPNOTSUPP;
		goto err_proto_put;
	}

	timeout = kzalloc(sizeof(struct nf_ct_timeout) +
			  l4proto->ctnl_timeout.obj_size, GFP_KERNEL);
	if (timeout == NULL) {
		ret = -ENOMEM;
		goto err_proto_put;
	}

	ret = nft_ct_timeout_parse_policy(&timeout->data, l4proto, ctx->net,
					  tb[NFTA_CT_TIMEOUT_DATA]);
	if (ret < 0)
		goto err_free_timeout;

	timeout->l3num = l3num;
	timeout->l4proto = l4proto;

	ret = nf_ct_netns_get(ctx->net, ctx->family);
	if (ret < 0)
		goto err_free_timeout;

	priv->timeout = timeout;
	return 0;

err_free_timeout:
	kfree(timeout);
err_proto_put:
	return ret;
}

static void nft_ct_timeout_obj_destroy(const struct nft_ctx *ctx,
				       struct nft_object *obj)
{
	struct nft_ct_timeout_obj *priv = nft_obj_data(obj);
	struct nf_ct_timeout *timeout = priv->timeout;

	nf_ct_untimeout(ctx->net, timeout);
	nf_ct_netns_put(ctx->net, ctx->family);
	kfree(priv->timeout);
}

static int nft_ct_timeout_obj_dump(struct sk_buff *skb,
				   struct nft_object *obj, bool reset)
{
	const struct nft_ct_timeout_obj *priv = nft_obj_data(obj);
	const struct nf_ct_timeout *timeout = priv->timeout;
	struct nlattr *nest_params;
	int ret;

	if (nla_put_u8(skb, NFTA_CT_TIMEOUT_L4PROTO, timeout->l4proto->l4proto) ||
	    nla_put_be16(skb, NFTA_CT_TIMEOUT_L3PROTO, htons(timeout->l3num)))
		return -1;

	nest_params = nla_nest_start(skb, NFTA_CT_TIMEOUT_DATA);
	if (!nest_params)
		return -1;

	ret = timeout->l4proto->ctnl_timeout.obj_to_nlattr(skb, &timeout->data);
	if (ret < 0)
		return -1;
	nla_nest_end(skb, nest_params);
	return 0;
}

static const struct nla_policy nft_ct_timeout_policy[NFTA_CT_TIMEOUT_MAX + 1] = {
	[NFTA_CT_TIMEOUT_L3PROTO] = {.type = NLA_U16 },
	[NFTA_CT_TIMEOUT_L4PROTO] = {.type = NLA_U8 },
	[NFTA_CT_TIMEOUT_DATA]	  = {.type = NLA_NESTED },
};

static struct nft_object_type nft_ct_timeout_obj_type;

static const struct nft_object_ops nft_ct_timeout_obj_ops = {
	.type		= &nft_ct_timeout_obj_type,
	.size		= sizeof(struct nft_ct_timeout_obj),
	.eval		= nft_ct_timeout_obj_eval,
	.init		= nft_ct_timeout_obj_init,
	.destroy	= nft_ct_timeout_obj_destroy,
	.dump		= nft_ct_timeout_obj_dump,
};

static struct nft_object_type nft_ct_timeout_obj_type __read_mostly = {
	.type		= NFT_OBJECT_CT_TIMEOUT,
	.ops		= &nft_ct_timeout_obj_ops,
	.maxattr	= NFTA_CT_TIMEOUT_MAX,
	.policy		= nft_ct_timeout_policy,
	.owner		= THIS_MODULE,
};
#endif /* CONFIG_NF_CONNTRACK_TIMEOUT */

static int nft_ct_helper_obj_init(const struct nft_ctx *ctx,
				  const struct nlattr * const tb[],
				  struct nft_object *obj)
{
	struct nft_ct_helper_obj *priv = nft_obj_data(obj);
	struct nf_conntrack_helper *help4, *help6;
	char name[NF_CT_HELPER_NAME_LEN];
	int family = ctx->family;
	int err;

	if (!tb[NFTA_CT_HELPER_NAME] || !tb[NFTA_CT_HELPER_L4PROTO])
		return -EINVAL;

	priv->l4proto = nla_get_u8(tb[NFTA_CT_HELPER_L4PROTO]);
	if (!priv->l4proto)
		return -ENOENT;

	nla_strscpy(name, tb[NFTA_CT_HELPER_NAME], sizeof(name));

	if (tb[NFTA_CT_HELPER_L3PROTO])
		family = ntohs(nla_get_be16(tb[NFTA_CT_HELPER_L3PROTO]));

	help4 = NULL;
	help6 = NULL;

	switch (family) {
	case NFPROTO_IPV4:
		if (ctx->family == NFPROTO_IPV6)
			return -EINVAL;

		help4 = nf_conntrack_helper_try_module_get(name, family,
							   priv->l4proto);
		break;
	case NFPROTO_IPV6:
		if (ctx->family == NFPROTO_IPV4)
			return -EINVAL;

		help6 = nf_conntrack_helper_try_module_get(name, family,
							   priv->l4proto);
		break;
	case NFPROTO_NETDEV:
	case NFPROTO_BRIDGE:
	case NFPROTO_INET:
		help4 = nf_conntrack_helper_try_module_get(name, NFPROTO_IPV4,
							   priv->l4proto);
		help6 = nf_conntrack_helper_try_module_get(name, NFPROTO_IPV6,
							   priv->l4proto);
		break;
	default:
		return -EAFNOSUPPORT;
	}

	/* && is intentional; only error if INET found neither ipv4 or ipv6 */
	if (!help4 && !help6)
		return -ENOENT;

	priv->helper4 = help4;
	priv->helper6 = help6;

	err = nf_ct_netns_get(ctx->net, ctx->family);
	if (err < 0)
		goto err_put_helper;

	return 0;

err_put_helper:
	if (priv->helper4)
		nf_conntrack_helper_put(priv->helper4);
	if (priv->helper6)
		nf_conntrack_helper_put(priv->helper6);
	return err;
}

static void nft_ct_helper_obj_destroy(const struct nft_ctx *ctx,
				      struct nft_object *obj)
{
	struct nft_ct_helper_obj *priv = nft_obj_data(obj);

	if (priv->helper4)
		nf_conntrack_helper_put(priv->helper4);
	if (priv->helper6)
		nf_conntrack_helper_put(priv->helper6);

	nf_ct_netns_put(ctx->net, ctx->family);
}

static void nft_ct_helper_obj_eval(struct nft_object *obj,
				   struct nft_regs *regs,
				   const struct nft_pktinfo *pkt)
{
	const struct nft_ct_helper_obj *priv = nft_obj_data(obj);
	struct nf_conn *ct = (struct nf_conn *)skb_nfct(pkt->skb);
	struct nf_conntrack_helper *to_assign = NULL;
	struct nf_conn_help *help;

	if (!ct ||
	    nf_ct_is_confirmed(ct) ||
	    nf_ct_is_template(ct) ||
	    priv->l4proto != nf_ct_protonum(ct))
		return;

	switch (nf_ct_l3num(ct)) {
	case NFPROTO_IPV4:
		to_assign = priv->helper4;
		break;
	case NFPROTO_IPV6:
		to_assign = priv->helper6;
		break;
	default:
		WARN_ON_ONCE(1);
		return;
	}

	if (!to_assign)
		return;

	if (test_bit(IPS_HELPER_BIT, &ct->status))
		return;

	help = nf_ct_helper_ext_add(ct, GFP_ATOMIC);
	if (help) {
		rcu_assign_pointer(help->helper, to_assign);
		set_bit(IPS_HELPER_BIT, &ct->status);
	}
}

static int nft_ct_helper_obj_dump(struct sk_buff *skb,
				  struct nft_object *obj, bool reset)
{
	const struct nft_ct_helper_obj *priv = nft_obj_data(obj);
	const struct nf_conntrack_helper *helper;
	u16 family;

	if (priv->helper4 && priv->helper6) {
		family = NFPROTO_INET;
		helper = priv->helper4;
	} else if (priv->helper6) {
		family = NFPROTO_IPV6;
		helper = priv->helper6;
	} else {
		family = NFPROTO_IPV4;
		helper = priv->helper4;
	}

	if (nla_put_string(skb, NFTA_CT_HELPER_NAME, helper->name))
		return -1;

	if (nla_put_u8(skb, NFTA_CT_HELPER_L4PROTO, priv->l4proto))
		return -1;

	if (nla_put_be16(skb, NFTA_CT_HELPER_L3PROTO, htons(family)))
		return -1;

	return 0;
}

static const struct nla_policy nft_ct_helper_policy[NFTA_CT_HELPER_MAX + 1] = {
	[NFTA_CT_HELPER_NAME] = { .type = NLA_STRING,
				  .len = NF_CT_HELPER_NAME_LEN - 1 },
	[NFTA_CT_HELPER_L3PROTO] = { .type = NLA_U16 },
	[NFTA_CT_HELPER_L4PROTO] = { .type = NLA_U8 },
};

static struct nft_object_type nft_ct_helper_obj_type;
static const struct nft_object_ops nft_ct_helper_obj_ops = {
	.type		= &nft_ct_helper_obj_type,
	.size		= sizeof(struct nft_ct_helper_obj),
	.eval		= nft_ct_helper_obj_eval,
	.init		= nft_ct_helper_obj_init,
	.destroy	= nft_ct_helper_obj_destroy,
	.dump		= nft_ct_helper_obj_dump,
};

static struct nft_object_type nft_ct_helper_obj_type __read_mostly = {
	.type		= NFT_OBJECT_CT_HELPER,
	.ops		= &nft_ct_helper_obj_ops,
	.maxattr	= NFTA_CT_HELPER_MAX,
	.policy		= nft_ct_helper_policy,
	.owner		= THIS_MODULE,
};

struct nft_ct_expect_obj {
	u16		l3num;
	__be16		dport;
	u8		l4proto;
	u8		size;
	u32		timeout;
};

static int nft_ct_expect_obj_init(const struct nft_ctx *ctx,
				  const struct nlattr * const tb[],
				  struct nft_object *obj)
{
	struct nft_ct_expect_obj *priv = nft_obj_data(obj);

	if (!tb[NFTA_CT_EXPECT_L4PROTO] ||
	    !tb[NFTA_CT_EXPECT_DPORT] ||
	    !tb[NFTA_CT_EXPECT_TIMEOUT] ||
	    !tb[NFTA_CT_EXPECT_SIZE])
		return -EINVAL;

	priv->l3num = ctx->family;
	if (tb[NFTA_CT_EXPECT_L3PROTO])
		priv->l3num = ntohs(nla_get_be16(tb[NFTA_CT_EXPECT_L3PROTO]));

	priv->l4proto = nla_get_u8(tb[NFTA_CT_EXPECT_L4PROTO]);
	priv->dport = nla_get_be16(tb[NFTA_CT_EXPECT_DPORT]);
	priv->timeout = nla_get_u32(tb[NFTA_CT_EXPECT_TIMEOUT]);
	priv->size = nla_get_u8(tb[NFTA_CT_EXPECT_SIZE]);

	return nf_ct_netns_get(ctx->net, ctx->family);
}

static void nft_ct_expect_obj_destroy(const struct nft_ctx *ctx,
				       struct nft_object *obj)
{
	nf_ct_netns_put(ctx->net, ctx->family);
}

static int nft_ct_expect_obj_dump(struct sk_buff *skb,
				  struct nft_object *obj, bool reset)
{
	const struct nft_ct_expect_obj *priv = nft_obj_data(obj);

	if (nla_put_be16(skb, NFTA_CT_EXPECT_L3PROTO, htons(priv->l3num)) ||
	    nla_put_u8(skb, NFTA_CT_EXPECT_L4PROTO, priv->l4proto) ||
	    nla_put_be16(skb, NFTA_CT_EXPECT_DPORT, priv->dport) ||
	    nla_put_u32(skb, NFTA_CT_EXPECT_TIMEOUT, priv->timeout) ||
	    nla_put_u8(skb, NFTA_CT_EXPECT_SIZE, priv->size))
		return -1;

	return 0;
}

static void nft_ct_expect_obj_eval(struct nft_object *obj,
				   struct nft_regs *regs,
				   const struct nft_pktinfo *pkt)
{
	const struct nft_ct_expect_obj *priv = nft_obj_data(obj);
	struct nf_conntrack_expect *exp;
	enum ip_conntrack_info ctinfo;
	struct nf_conn_help *help;
	enum ip_conntrack_dir dir;
	u16 l3num = priv->l3num;
	struct nf_conn *ct;

	ct = nf_ct_get(pkt->skb, &ctinfo);
	if (!ct || nf_ct_is_confirmed(ct) || nf_ct_is_template(ct)) {
		regs->verdict.code = NFT_BREAK;
		return;
	}
	dir = CTINFO2DIR(ctinfo);

	help = nfct_help(ct);
	if (!help)
		help = nf_ct_helper_ext_add(ct, GFP_ATOMIC);
	if (!help) {
		regs->verdict.code = NF_DROP;
		return;
	}

	if (help->expecting[NF_CT_EXPECT_CLASS_DEFAULT] >= priv->size) {
		regs->verdict.code = NFT_BREAK;
		return;
	}
	if (l3num == NFPROTO_INET)
		l3num = nf_ct_l3num(ct);

	exp = nf_ct_expect_alloc(ct);
	if (exp == NULL) {
		regs->verdict.code = NF_DROP;
		return;
	}
	nf_ct_expect_init(exp, NF_CT_EXPECT_CLASS_DEFAULT, l3num,
		          &ct->tuplehash[!dir].tuple.src.u3,
		          &ct->tuplehash[!dir].tuple.dst.u3,
		          priv->l4proto, NULL, &priv->dport);
	exp->timeout.expires = jiffies + priv->timeout * HZ;

	if (nf_ct_expect_related(exp, 0) != 0)
		regs->verdict.code = NF_DROP;
}

static const struct nla_policy nft_ct_expect_policy[NFTA_CT_EXPECT_MAX + 1] = {
	[NFTA_CT_EXPECT_L3PROTO]	= { .type = NLA_U16 },
	[NFTA_CT_EXPECT_L4PROTO]	= { .type = NLA_U8 },
	[NFTA_CT_EXPECT_DPORT]		= { .type = NLA_U16 },
	[NFTA_CT_EXPECT_TIMEOUT]	= { .type = NLA_U32 },
	[NFTA_CT_EXPECT_SIZE]		= { .type = NLA_U8 },
};

static struct nft_object_type nft_ct_expect_obj_type;

static const struct nft_object_ops nft_ct_expect_obj_ops = {
	.type		= &nft_ct_expect_obj_type,
	.size		= sizeof(struct nft_ct_expect_obj),
	.eval		= nft_ct_expect_obj_eval,
	.init		= nft_ct_expect_obj_init,
	.destroy	= nft_ct_expect_obj_destroy,
	.dump		= nft_ct_expect_obj_dump,
};

static struct nft_object_type nft_ct_expect_obj_type __read_mostly = {
	.type		= NFT_OBJECT_CT_EXPECT,
	.ops		= &nft_ct_expect_obj_ops,
	.maxattr	= NFTA_CT_EXPECT_MAX,
	.policy		= nft_ct_expect_policy,
	.owner		= THIS_MODULE,
};

static int __init nft_ct_module_init(void)
{
	int err;

	BUILD_BUG_ON(NF_CT_LABELS_MAX_SIZE > NFT_REG_SIZE);

	err = nft_register_expr(&nft_ct_type);
	if (err < 0)
		return err;

	err = nft_register_expr(&nft_notrack_type);
	if (err < 0)
		goto err1;

	err = nft_register_obj(&nft_ct_helper_obj_type);
	if (err < 0)
		goto err2;

	err = nft_register_obj(&nft_ct_expect_obj_type);
	if (err < 0)
		goto err3;
#ifdef CONFIG_NF_CONNTRACK_TIMEOUT
	err = nft_register_obj(&nft_ct_timeout_obj_type);
	if (err < 0)
		goto err4;
#endif
	return 0;

#ifdef CONFIG_NF_CONNTRACK_TIMEOUT
err4:
	nft_unregister_obj(&nft_ct_expect_obj_type);
#endif
err3:
	nft_unregister_obj(&nft_ct_helper_obj_type);
err2:
	nft_unregister_expr(&nft_notrack_type);
err1:
	nft_unregister_expr(&nft_ct_type);
	return err;
}

static void __exit nft_ct_module_exit(void)
{
#ifdef CONFIG_NF_CONNTRACK_TIMEOUT
	nft_unregister_obj(&nft_ct_timeout_obj_type);
#endif
	nft_unregister_obj(&nft_ct_expect_obj_type);
	nft_unregister_obj(&nft_ct_helper_obj_type);
	nft_unregister_expr(&nft_notrack_type);
	nft_unregister_expr(&nft_ct_type);
}

module_init(nft_ct_module_init);
module_exit(nft_ct_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick McHardy <kaber@trash.net>");
MODULE_ALIAS_NFT_EXPR("ct");
MODULE_ALIAS_NFT_EXPR("notrack");
MODULE_ALIAS_NFT_OBJ(NFT_OBJECT_CT_HELPER);
MODULE_ALIAS_NFT_OBJ(NFT_OBJECT_CT_TIMEOUT);
MODULE_ALIAS_NFT_OBJ(NFT_OBJECT_CT_EXPECT);
MODULE_DESCRIPTION("Netfilter nf_tables conntrack module");
