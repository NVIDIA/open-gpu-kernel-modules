// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012-2016 Pablo Neira Ayuso <pablo@netfilter.org>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables.h>

#define nft_objref_priv(expr)	*((struct nft_object **)nft_expr_priv(expr))

static void nft_objref_eval(const struct nft_expr *expr,
			    struct nft_regs *regs,
			    const struct nft_pktinfo *pkt)
{
	struct nft_object *obj = nft_objref_priv(expr);

	obj->ops->eval(obj, regs, pkt);
}

static int nft_objref_init(const struct nft_ctx *ctx,
			   const struct nft_expr *expr,
			   const struct nlattr * const tb[])
{
	struct nft_object *obj = nft_objref_priv(expr);
	u8 genmask = nft_genmask_next(ctx->net);
	u32 objtype;

	if (!tb[NFTA_OBJREF_IMM_NAME] ||
	    !tb[NFTA_OBJREF_IMM_TYPE])
		return -EINVAL;

	objtype = ntohl(nla_get_be32(tb[NFTA_OBJREF_IMM_TYPE]));
	obj = nft_obj_lookup(ctx->net, ctx->table,
			     tb[NFTA_OBJREF_IMM_NAME], objtype,
			     genmask);
	if (IS_ERR(obj))
		return -ENOENT;

	nft_objref_priv(expr) = obj;
	obj->use++;

	return 0;
}

static int nft_objref_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	const struct nft_object *obj = nft_objref_priv(expr);

	if (nla_put_string(skb, NFTA_OBJREF_IMM_NAME, obj->key.name) ||
	    nla_put_be32(skb, NFTA_OBJREF_IMM_TYPE,
			 htonl(obj->ops->type->type)))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}

static void nft_objref_deactivate(const struct nft_ctx *ctx,
				  const struct nft_expr *expr,
				  enum nft_trans_phase phase)
{
	struct nft_object *obj = nft_objref_priv(expr);

	if (phase == NFT_TRANS_COMMIT)
		return;

	obj->use--;
}

static void nft_objref_activate(const struct nft_ctx *ctx,
				const struct nft_expr *expr)
{
	struct nft_object *obj = nft_objref_priv(expr);

	obj->use++;
}

static struct nft_expr_type nft_objref_type;
static const struct nft_expr_ops nft_objref_ops = {
	.type		= &nft_objref_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_object *)),
	.eval		= nft_objref_eval,
	.init		= nft_objref_init,
	.activate	= nft_objref_activate,
	.deactivate	= nft_objref_deactivate,
	.dump		= nft_objref_dump,
};

struct nft_objref_map {
	struct nft_set		*set;
	u8			sreg;
	struct nft_set_binding	binding;
};

static void nft_objref_map_eval(const struct nft_expr *expr,
				struct nft_regs *regs,
				const struct nft_pktinfo *pkt)
{
	struct nft_objref_map *priv = nft_expr_priv(expr);
	const struct nft_set *set = priv->set;
	struct net *net = nft_net(pkt);
	const struct nft_set_ext *ext;
	struct nft_object *obj;
	bool found;

	found = set->ops->lookup(net, set, &regs->data[priv->sreg], &ext);
	if (!found) {
		ext = nft_set_catchall_lookup(net, set);
		if (!ext) {
			regs->verdict.code = NFT_BREAK;
			return;
		}
	}
	obj = *nft_set_ext_obj(ext);
	obj->ops->eval(obj, regs, pkt);
}

static int nft_objref_map_init(const struct nft_ctx *ctx,
			       const struct nft_expr *expr,
			       const struct nlattr * const tb[])
{
	struct nft_objref_map *priv = nft_expr_priv(expr);
	u8 genmask = nft_genmask_next(ctx->net);
	struct nft_set *set;
	int err;

	set = nft_set_lookup_global(ctx->net, ctx->table,
				    tb[NFTA_OBJREF_SET_NAME],
				    tb[NFTA_OBJREF_SET_ID], genmask);
	if (IS_ERR(set))
		return PTR_ERR(set);

	if (!(set->flags & NFT_SET_OBJECT))
		return -EINVAL;

	err = nft_parse_register_load(tb[NFTA_OBJREF_SET_SREG], &priv->sreg,
				      set->klen);
	if (err < 0)
		return err;

	priv->binding.flags = set->flags & NFT_SET_OBJECT;

	err = nf_tables_bind_set(ctx, set, &priv->binding);
	if (err < 0)
		return err;

	priv->set = set;
	return 0;
}

static int nft_objref_map_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	const struct nft_objref_map *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_OBJREF_SET_SREG, priv->sreg) ||
	    nla_put_string(skb, NFTA_OBJREF_SET_NAME, priv->set->name))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}

static void nft_objref_map_deactivate(const struct nft_ctx *ctx,
				      const struct nft_expr *expr,
				      enum nft_trans_phase phase)
{
	struct nft_objref_map *priv = nft_expr_priv(expr);

	nf_tables_deactivate_set(ctx, priv->set, &priv->binding, phase);
}

static void nft_objref_map_activate(const struct nft_ctx *ctx,
				    const struct nft_expr *expr)
{
	struct nft_objref_map *priv = nft_expr_priv(expr);

	priv->set->use++;
}

static void nft_objref_map_destroy(const struct nft_ctx *ctx,
				   const struct nft_expr *expr)
{
	struct nft_objref_map *priv = nft_expr_priv(expr);

	nf_tables_destroy_set(ctx, priv->set);
}

static struct nft_expr_type nft_objref_type;
static const struct nft_expr_ops nft_objref_map_ops = {
	.type		= &nft_objref_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_objref_map)),
	.eval		= nft_objref_map_eval,
	.init		= nft_objref_map_init,
	.activate	= nft_objref_map_activate,
	.deactivate	= nft_objref_map_deactivate,
	.destroy	= nft_objref_map_destroy,
	.dump		= nft_objref_map_dump,
};

static const struct nft_expr_ops *
nft_objref_select_ops(const struct nft_ctx *ctx,
                      const struct nlattr * const tb[])
{
	if (tb[NFTA_OBJREF_SET_SREG] &&
	    (tb[NFTA_OBJREF_SET_NAME] ||
	     tb[NFTA_OBJREF_SET_ID]))
		return &nft_objref_map_ops;
	else if (tb[NFTA_OBJREF_IMM_NAME] &&
		 tb[NFTA_OBJREF_IMM_TYPE])
		return &nft_objref_ops;

	return ERR_PTR(-EOPNOTSUPP);
}

static const struct nla_policy nft_objref_policy[NFTA_OBJREF_MAX + 1] = {
	[NFTA_OBJREF_IMM_NAME]	= { .type = NLA_STRING,
				    .len = NFT_OBJ_MAXNAMELEN - 1 },
	[NFTA_OBJREF_IMM_TYPE]	= { .type = NLA_U32 },
	[NFTA_OBJREF_SET_SREG]	= { .type = NLA_U32 },
	[NFTA_OBJREF_SET_NAME]	= { .type = NLA_STRING,
				    .len = NFT_SET_MAXNAMELEN - 1 },
	[NFTA_OBJREF_SET_ID]	= { .type = NLA_U32 },
};

static struct nft_expr_type nft_objref_type __read_mostly = {
	.name		= "objref",
	.select_ops	= nft_objref_select_ops,
	.policy		= nft_objref_policy,
	.maxattr	= NFTA_OBJREF_MAX,
	.owner		= THIS_MODULE,
};

static int __init nft_objref_module_init(void)
{
	return nft_register_expr(&nft_objref_type);
}

static void __exit nft_objref_module_exit(void)
{
	nft_unregister_expr(&nft_objref_type);
}

module_init(nft_objref_module_init);
module_exit(nft_objref_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pablo Neira Ayuso <pablo@netfilter.org>");
MODULE_ALIAS_NFT_EXPR("objref");
MODULE_DESCRIPTION("nftables stateful object reference module");
